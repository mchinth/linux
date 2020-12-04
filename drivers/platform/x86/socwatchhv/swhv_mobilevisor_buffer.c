/* SPDX-License-Identifier: GPL-2.0 AND BSD-3-Clause
 *
 * This file is provided under a dual BSD/GPLv2 license.  When using or
 * redistributing this file, you may do so under either license.
 *
 * GPL LICENSE SUMMARY
 *
 * Copyright(c) 2014 - 2019 Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * Contact Information:
 * SoC Watch Developer Team <socwatchdevelopers@intel.com>
 * Intel Corporation,
 * 1300 S Mopac Expwy,
 * Austin, TX 78746
 *
 * BSD LICENSE
 *
 * Copyright(c) 2014 - 2019 Intel Corporation.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in
 *     the documentation and/or other materials provided with the
 *     distribution.
 *   * Neither the name of Intel Corporation nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/timer.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/interrupt.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <asm/io.h>
#include "swhv_mobilevisor_buffer.h"
#include "swhv_driver.h"

#if defined(SOFIA_3G)
#include "asm/mv/mv_svc_hypercalls.h"
#else
#include "asm/mv/socwatch/mv_svc_socwatch.h"
#endif

#define BUFFER_SIZE 16

struct socwatch_buffer_info *pPhysBufferInfo;
struct socwatch_buffer_info *pBufferInfo;

#if defined(DRV_VMM_32)
	void *pbuf_addr;
	int pbuf_offset;
	int pbuf_size;
#endif

typedef struct PWCollector_msg PWCollector_msg_t;


/* TODO check if vmm delivers injected interrupts on stop to deliver partially filled buffers */
/* cpu is an OUT */
bool vmm_is_buffer_ready(int num_cpus, int *cpu)
{
	int i;
	/* check each buffer_status value for SOCWATCH_BUFFER_VALID, */
	/* if found return true and the cpu index representing which buffer is read to be copied */
	if (pBufferInfo != NULL) {
		for (i = 0; i < num_cpus; i++) {
			/* this code assumes the vmm decides when the buffer is ready to consume - when */
			/* it's full or when the collection has been stopped and the buffer is partially filled */
			if (pBufferInfo[i].buffer_status == SOCWATCH_BUFFER_VALID) {
				*cpu = i;
				return true;
			}
		}
	} else {
		pw_pr_debug("sofia-proto: vmm_is_buffer_ready: ERROR: pBufferInfo is NULL!\n");
		return false;
	}

	/* didn't find any ready buffers */
	return false;
};

int vmm_init_buffers(int num_cpus)
{
	int i, j;

	/* allocate the structure that holds information about the buffers passed to the vmm */
#if defined(DRV_VMM_32)
	pBufferInfo = (struct socwatch_buffer_info *)
		__get_free_pages(GFP_KERNEL | GFP_DMA32,
		get_order(num_cpus * sizeof(struct socwatch_buffer_info)));
	if (pBufferInfo == NULL) {
		pw_pr_debug("sofia-proto: failed to allocate struct socwatch_buffer_info structure\n");
		return -ENOMEM;
	}
	memset(pBufferInfo, 0, num_cpus * sizeof(struct socwatch_buffer_info));

	pbuf_size = (num_cpus * SOCWATCH_BUFFER_COUNT_PER_CORE * BUFFER_SIZE * 1024);
	pbuf_addr = (void *)__get_free_pages(GFP_KERNEL | GFP_DMA32, get_order(pbuf_size));
	if (pbuf_addr == NULL) {
		pw_pr_debug("sofia-proto: failed to allocate page for sharing with vmm\n");
		return -ENOMEM;
	}
	memset(pbuf_addr, 0, pbuf_size);
	for (i = 0; i < num_cpus; i++) {
		pBufferInfo[i].num_buffers_allocated = SOCWATCH_BUFFER_COUNT_PER_CORE;
		pBufferInfo[i].buffer_length = BUFFER_SIZE;

		for (j = 0; j < pBufferInfo[i].num_buffers_allocated; j++) {
			pBufferInfo[i].buffer_start[j] = (uint64_t)(unsigned long)(pbuf_addr + pbuf_offset);
			if (pBufferInfo[i].buffer_start[j] == 0) {
				pw_pr_debug("sofia-proto: failed to allocate buffer for sharing with vmm\n");
				return -ENOMEM;
			}
			pbuf_offset += pBufferInfo[i].buffer_length * 1024;
		}
	}
#else
	pBufferInfo = kmalloc_array(num_cpus, sizeof(struct socwatch_buffer_info), GFP_KERNEL);
	if (pBufferInfo == NULL) {
		pw_pr_debug("sofia-proto: failed to allocate struct socwatch_buffer_info structure\n");
		return -ENOMEM;
	}

	/* num_buffers_allocated should be twice the number of cpus in the system */
	/* pBufferInfo->num_buffers_allocated = num_cpus*2; */

	for (i = 0; i < num_cpus; i++) {
		pBufferInfo[i].num_buffers_allocated = SOCWATCH_BUFFER_COUNT_PER_CORE;
		pBufferInfo[i].buffer_length = BUFFER_SIZE;

		for (j = 0; j < pBufferInfo[i].num_buffers_allocated; j++) {
			pBufferInfo[i].buffer_start[j] = (uint64_t)(unsigned long)
				kmalloc(pBufferInfo[i].buffer_length * 1024, GFP_KERNEL);
			if (pBufferInfo[i].buffer_start[j] == 0) {
				pw_pr_debug("sofia-proto: failed to allocate buffer for sharing with vmm\n");
				return -ENOMEM;
			}
		}
	}
#endif
	/* printk(KERN_INFO "sofia-proto: done allocating data buffers.\n"); */

	/* convert virtual buffer addresses to physical buffer addresses - the vmm requires physical addresses */

	for (i = 0; i < num_cpus; i++) {
		for (j = 0; j < pBufferInfo[i].num_buffers_allocated; j++) {
			pBufferInfo[i].buffer_start[j] = (uint64_t)(unsigned long)
				virt_to_phys((void *)(unsigned long)pBufferInfo[i].buffer_start[j]);
			if (pBufferInfo[i].buffer_start[j] == 0) {
				pw_pr_debug("sofia-proto: failed to convert data buffer %d start address to physical address on core %d.\n", j, i);
				/* not sure what error to return but let's start with -EIO */
				return -EIO;
			}
		}
	}

	/* set buffer status to 'consumed' to indicate the vmm can use buffer immediately */
	for (i = 0; i < num_cpus; i++)
		pBufferInfo[i].buffer_status = SOCWATCH_BUFFER_CONSUMED;

	pPhysBufferInfo = (struct socwatch_buffer_info *)virt_to_phys(pBufferInfo);
	if (pPhysBufferInfo == NULL) {
		pw_pr_debug("sofia-proto: failed to convert socwatch_buffer_info struct to physical memory address\n");
		return -EIO;
	}
	return 0;
};

void vmm_destroy_buffers(int num_cpus)
{
#if defined(DRV_VMM_32)
	free_pages((unsigned long)pbuf_addr, get_order(pbuf_size));
	free_pages((unsigned long)pBufferInfo, get_order(num_cpus * sizeof(struct socwatch_buffer_info)));
	pbuf_offset = 0;
	pbuf_size = 0;
	pbuf_addr = NULL;
#else
	int i, j;

	if (pBufferInfo) {
	for (i = 0; i < num_cpus; i++) {
		for (j = 0; j < pBufferInfo[j].num_buffers_allocated; j++) {
			/* convert back to virtual memory of necessary */
			pBufferInfo[i].buffer_start[j] = (uint64_t)(unsigned long)
					phys_to_virt((unsigned long)pBufferInfo[i].buffer_start[j]);
			if (pBufferInfo[i].buffer_start[j] != 0)
				kfree((void *)(unsigned long)pBufferInfo[i].buffer_start[j]);
			}
		}
		if (pBufferInfo)
			kfree(pBufferInfo);
	}
#endif
};

void vmm_reset_buffers(int num_cpus)
{
	int i;

	if (pBufferInfo) {
		for (i = 0; i < num_cpus; i++) {
			/* set buffer status to 'consumed' to indicate the vmm can use buffer immediately */
			pBufferInfo[i].buffer_status = SOCWATCH_BUFFER_CONSUMED;
		}
	}
};
