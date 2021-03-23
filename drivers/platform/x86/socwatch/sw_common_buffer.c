/* ********************************************************************************
 # INTEL CONFIDENTIAL
 # Copyright 2019 Intel Corporation.

 # This software and the related documents are Intel copyrighted materials, and
 # your use of them is governed by the express license under which they were
 # provided to you (License). Unless the License provides otherwise, you may not
 # use, modify, copy, publish, distribute, disclose or transmit this software or
 # the related documents without Intel's prior written permission.

 # This software and the related documents are provided as is, with no express or
 # implied warranties, other than those that are expressly stated in the License.
 # ********************************************************************************/

#if defined(SWW_MERGE)
#include <wdm.h>
#include <errno.h>
#include "sw_defines.h"
#include "sw_structs.h"
#include "sw_common_buffer.h"
#include "sw_win_defs.h"
#include "internal.h"
#include "util.h"
#else
#include "sw_internal.h"
#include "sw_output_buffer.h"
#include "sw_kernel_defines.h"
#include "sw_mem.h"
#include "sw_lock_defs.h"
#include "sw_overhead_measurements.h"
#endif

/* *************************************************
 * For circular buffer (continuous profiling)
 * *************************************************
 */
static char *output_buffer = NULL;

struct buffer {
    union {
        char *data;
        size_t free_pages;
    };
    size_t read_index, write_index;
    unsigned long size;
};
SW_DECLARE_RWLOCK(sw_continuous_lock);

static struct buffer buffer; /* TODO: rename */

/* -------------------------------------------------
 * Function definitions.
 * -------------------------------------------------
 */

/* *************************************************
 * For circular buffer (continuous profiling)
 * *************************************************
 */
#define MIN(x, y) ( (x) <= (y) ? (x) : (y) )

#define IS_BUFFER_EMPTY(buffer) ( (buffer).write_index == (buffer).read_index )
#define IS_BUFFER_FULL(buffer) ( (buffer).write_index == ((buffer).read_index + 1) & (buffer.size - 1) )

static __inline size_t get_space_available(struct buffer *buffer)
{
    size_t read = 0, write = 0;
    SMP_MB // Linux: smp_mb();
    read = buffer->read_index;
    write = buffer->write_index;
    if (write < read) {
        return read - write;
    }
    return (buffer->size - write) + read;
}

static __inline size_t get_data_available(struct buffer *buffer)
{
    size_t read = 0, write = 0;
    SMP_MB // Linux: smp_mb();
    read = buffer->read_index;
    write = buffer->write_index;
    if (read <= write) {
        return write - read;
    }

    return (buffer->size - read) + write;
}

static void copy_wraparound(const char *src, size_t src_size, size_t *index)
{
    size_t buff_size_left = buffer.size - *index;
    size_t to_write = MIN(buff_size_left, src_size);
    size_t _index = *index;
    if (src_size < buff_size_left) {
        memcpy_s(&buffer.data[_index], src_size, src, src_size);
        _index += src_size;
    } else {
        memcpy_s(&buffer.data[_index], to_write, src, to_write);
        _index = 0;
        src += to_write;
        to_write = src_size - to_write;
        memcpy_s(&buffer.data[_index], to_write, src, to_write);
        _index += to_write;
        pw_pr_debug("DEBUG: wrap memcpy_s\n");
    }
    *index = (*index + src_size) & (buffer.size - 1);
}

int enqueue_circular_data(struct sw_driver_msg *msg, enum sw_wakeup_action action)
{
    size_t size = SW_DRIVER_MSG_HEADER_SIZE() + msg->payload_len;
    bool wrapped = false;

    msg->tsc = 0;

    READ_LOCK(sw_continuous_lock);
    while (true) {
        size_t old_write_index = buffer.write_index, new_write_index = (old_write_index + size) & (buffer.size - 1);
        if (get_space_available(&buffer) < size) {
            break;
        }
        if (CAS32(&buffer.write_index, old_write_index, new_write_index)) {
            msg->tsc = SW_TIMESTAMP;
            wrapped = new_write_index <= old_write_index;
            /* First copy header */
            copy_wraparound((const char *)msg, SW_DRIVER_MSG_HEADER_SIZE(), &old_write_index);
            /* Then copy payload */
            copy_wraparound((const char *)msg->p_payload, msg->payload_len, &old_write_index);
            pw_pr_debug("[cpu=%d] successfully wrote with new_write = %lu for ts %llu\n", curr_cpu(), new_write_index, msg->tsc);
            break;
        }
    }
    READ_UNLOCK(sw_continuous_lock);
    if (!msg->tsc) {
        pw_pr_error("ERROR: couldn't enqueue data\n");
    }
    if (wrapped) {
        pw_pr_debug("DEBUG: wrapped!\n");
    }
    return msg->tsc ? 0 : -1;
}

/*
 * Returns # of bytes successfully consumed on success
 * 0 on EOF (no error condition)
 */
size_t consume_circular_data(void *dest, size_t bytes_to_read)
{
    size_t read_index = 0, write_index = 0, dst_index = 0;
    size_t to_read = 0;
    bool wrapped = false;
    size_t read_size = bytes_to_read;
    unsigned long bytes_not_copied = 0;
    struct sw_driver_continuous_collect data = {0};

    WRITE_LOCK(sw_continuous_lock);
    SMP_MB // Linux: smp_mb();
    read_index = buffer.read_index;
    write_index = buffer.write_index;
    read_size -= SW_DRIVER_CONTINUOUS_COLLECT_HEADER_SIZE(); /* EXE sends size as header + payload; we only want payload */
    data.collection_size = to_read = MIN(read_size, get_data_available(&buffer));
    pw_pr_debug("DEBUG: read = %zu, write = %zu, avail = %zu, to_read = %zu\n", read_index, write_index, get_data_available(&buffer), to_read);
    while (to_read) {
        size_t curr_read = to_read;
        if (read_index + to_read > buffer.size) {
            curr_read = buffer.size - read_index;
            wrapped = true;
            pw_pr_debug("DEBUG: read = %zu, to_read = %zu, curr_read = %zu, buffer.size = %lu, WRAPPED!\n", read_index, to_read, curr_read, buffer.size);
        }
        memcpy_s(&output_buffer[dst_index], curr_read, &buffer.data[read_index], curr_read);
        read_index = (read_index + curr_read) & (buffer.size - 1);
        to_read -= curr_read;
        dst_index += curr_read;
    }
    buffer.read_index = read_index;
    SMP_MB // Linux: smp_mb();
    pw_pr_debug("DEBUG: read at end of while = %zu\n", buffer.read_index);
    WRITE_UNLOCK(sw_continuous_lock);

#ifdef SWW_MERGE
    if (memcpy_s(dest, SW_DRIVER_CONTINUOUS_COLLECT_HEADER_SIZE(), (char *)&data, SW_DRIVER_CONTINUOUS_COLLECT_HEADER_SIZE()) != STATUS_SUCCESS) {
        pw_pr_error("consume_circular_data couldn't copy header\n");
        return 0;
    }
    if (memcpy_s((char *)dest + SW_DRIVER_CONTINUOUS_COLLECT_HEADER_SIZE(), data.collection_size, output_buffer, data.collection_size) != STATUS_SUCCESS) {
        pw_pr_error("consume_circular_data couldn't copy data\n");
        return 0;
    }
#else
    // TODO
    /*
     * Call 'copy_to_user' instead of 'sw_copy_to_user' since
     * sw_copy_to_user expects to see a 'struct uio' while this
     * is called from an IOCTL which does NOT have a 'struct uio'
     */
    bytes_not_copied = copy_to_user(dest, (char *)&data, SW_DRIVER_CONTINUOUS_COLLECT_HEADER_SIZE()); // dst, src
    if (bytes_not_copied) {
        return 0;
    }
    pw_pr_debug("DEBUG: collection size = %u\n", data.collection_size);
    if (data.collection_size) {
        bytes_not_copied = copy_to_user(dest+SW_DRIVER_CONTINUOUS_COLLECT_HEADER_SIZE(), output_buffer, data.collection_size); // dst, src
        if (bytes_not_copied) {
            return 0;
        }
    }
#endif
    return data.collection_size;
}

#ifdef SWW_MERGE
NTSTATUS initialize_circular_buffer(size_t size)
#else
long initialize_circular_buffer(size_t size)
#endif // SWW_MERGE
{
    size_t alloc_size = size, read_size = size;
    /*
     * We require a power of two size
     */
    pw_pr_debug("DEBUG: old alloc size = %zu\n", alloc_size);
    if ((alloc_size & (alloc_size - 1)) != 0) {
        alloc_size = 1ULL << sw_fls(alloc_size); // Linux: fls(alloc_size);
    }
    pw_pr_debug("DEBUG: new alloc size = %zu\n", alloc_size);
    /* Create double-sized buffer */
    alloc_size <<= 1;
    pw_pr_debug("DEBUG: double alloc size = %zu\n", alloc_size);
    memset(&buffer, 0, sizeof(buffer));
    buffer.free_pages = ALLOCATE_PAGES(alloc_size); // Linux: sw_allocate_pages(GFP_KERNEL | __GFP_ZERO, alloc_size);
    if (!buffer.free_pages) {
        pw_pr_error("Couldn't allocate space for buffer!\n");
#ifdef SWW_MERGE
        return STATUS_NO_MEMORY;
#else
        return -ENOMEM;
#endif // SWW_MERGE
    }
    buffer.read_index = buffer.write_index = 0;
    buffer.size = alloc_size;
    SW_INIT_RWLOCK(sw_continuous_lock);
    /*
     * Create temp output buffer
     */
    output_buffer = SW_MALLOC(read_size); // Linux: vmalloc(read_size);
    if (!output_buffer) {
        pw_pr_error("Couldn't create temporary buffer for data output!\n");
#ifdef SWW_MERGE
        return STATUS_NO_MEMORY;
#else
        return -ENOMEM;
#endif // SWW_MERGE
    }
    return 0;
}

void reset_output_buffers(void)
{
    buffer.read_index = buffer.write_index = 0;
}


void destroy_circular_buffer(void)
{
    if (buffer.free_pages) {
        RELEASE_PAGES(buffer.free_pages, buffer.size); // Linux: sw_release_pages(buffer.free_pages, buffer.size);
        buffer.free_pages = 0;
    }
    if (output_buffer) {
        SW_FREE(output_buffer); // Linux: vfree(output_buffer);
        output_buffer = NULL;
    }
    SW_DESTROY_RWLOCK(sw_continuous_lock);
    pw_pr_debug("DEBUG: read = %zu, write = %zu\n", buffer.read_index, buffer.write_index);
}
