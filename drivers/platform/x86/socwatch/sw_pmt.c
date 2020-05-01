/* SPDX-License-Identifier: GPL-2.0 AND BSD-3-Clause
 *
 * This file is provided under a dual BSD/GPLv2 license.  When using or
 * redistributing this file, you may do so under either license.
 *
 * GPL LICENSE SUMMARY
 *
 * Copyright(c) 2019 Intel Corporation.
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
 * Copyright(c) 2019 Intel Corporation.
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

#include <linux/compiler.h>     /* Definition of __weak */
#include <linux/kref.h> /* struct kref */
#include <linux/notifier.h> /* struct notifier_block */
#include <linux/pci.h> /* struct pci_dev */
#include <linux/ioport.h> /* struct resource */
#include <linux/kref.h> /* struct kref */

#include "sw_structs.h"      /* sw_driver_io_descriptor */
#include "sw_kernel_defines.h"  /* pw_pr_debug */
#include "sw_pmt.h"

/* *********************************
 * Begin PMT driver import
 * *********************************
 */

/*
 * Struct definitions taken from PMT driver.
 */

struct telem_header {
        u8      access_type;
        u8      telem_type;
        u16     size;
        u32     guid;
        u32     base_offset;
};

struct telem_endpoint {
        struct pci_dev        *parent;
        struct telem_header   header;
        void __iomem          *base;
        struct resource       res;
        bool                  present;
        struct kref           kref;
};

struct telem_endpoint_info {
        struct pci_dev          *pdev;
        struct telem_header     header;
};

/*
 * Weak linkage of functions from the PMT driver
 */

/**
 * pmt_telem_get_next_endpoint() - Get next device id for a telemetry endpoint
 * @start:  starting devid to look from
 *
 * This functions can be used in a while loop predicate to retrieve the devid
 * of all available telemetry endpoints. Functions pmt_telem_get_next_endpoint()
 * and pmt_telem_register_endpoint() can be used inside of the loop to examine
 * endpoint info and register to receive a pointer to the endpoint. The pointer
 * is then usable in the telemetry read calls to access the telemetry data.
 *
 * Return:
 * * devid       - devid of the next present endpoint from start
 * * 0           - when no more endpoints are present after start
 */
extern int __weak
pmt_telem_get_next_endpoint(int start);

/**
 * pmt_telem_register_endpoint() - Register a telemetry endpoint
 * @devid: device id/handle of the telemetry endpoint
 *
 * Increments the kref usage counter for the endpoint.
 *
 * Return:
 * * endpoint    - On success returns pointer to the telemetry endpoint
 * * -ENXIO      - telemetry endpoint not found
 */
extern struct telem_endpoint * __weak
pmt_telem_register_endpoint(int devid);

/**
 * pmt_telem_unregister_endpoint() - Unregister a telemetry endpoint
 * @ep:   ep structure to populate.
 *
 * Decrements the kref usage counter for the endpoint.
 */
extern void __weak
pmt_telem_unregister_endpoint(struct telem_endpoint *ep);

/**
 * pmt_telem_get_endpoint_info() - Get info for an endpoint from its devid
 * @devid:  device id/handle of the telemetry endpoint
 * @info:   Endpoint info structure to be populated
 *
 * Return:
 * * 0           - Success
 * * -ENXIO      - telemetry endpoint not found for the devid
 * * -EINVAL     - @info is NULL
 */
extern int __weak
pmt_telem_get_endpoint_info(int devid,
				struct telem_endpoint_info *info);

/**
 * pmt_telem_read32() - Read dwords from telemetry sram
 * @ep:     Telemetry endpoint to be read
 * @offset: Register offset in bytes
 * @data:   Allocated dword buffer
 * @count:  Number of dwords requested
 *
 * Callers must ensure reads are aligned. When the call returns -ENODEV,
 * the device has been removed and callers should unregister the telemetry
 * endpoint.
 *
 * Return:
 * * 0           - Success
 * * -ENODEV	 - The device is not present.
 * * -EINVAL	 - The offset is out out bounds
 * * -EPIPE	 - The device was removed during the read. Data written
 *		   but should be considered invalid.
 */
extern int __weak
pmt_telem_read32(struct telem_endpoint *ep, u32 offset, u32 *data,
		     u32 count);

/**
 * pmt_telem_read64() - Read qwords from counter sram
 * @ep:     Telemetry endpoint to be read
 * @offset: Register offset in bytes
 * @data:   Allocated qword buffer
 * @count:  Number of qwords requested
 *
 * Callers must ensure reads are aligned. When the call returns -ENODEV,
 * the device has been removed and callers should unregister the telemetry
 * endpoint.
 *
 * Return:
 * * 0           - Success
 * * -ENODEV	 - The device is not present.
 * * -EINVAL	 - The offset is out out bounds
 * * -EPIPE	 - The device was removed during the read. Data written
 *		   but should be considered not valid.
 */
extern int __weak
pmt_telem_read64(struct telem_endpoint *ep, u32 offset, u64 *data,
		     u32 count);

/* Notifiers */

#define PMT_TELEM_NOTIFY_ADD	0
#define PMT_TELEM_NOTIFY_REMOVE	1

/**
 * pmt_telem_register_notifier() - Receive notification endpoint events
 * @nb:   Notifier block
 *
 * Events:
 *   PMT_TELEM_NOTIFY_ADD   - An endpoint has been added. Notifier data
 *                            is the devid
 *   PMT_TELEM_NOTIF_REMOVE - An endpoint has been removed. Notifier data
 *                            is the devid
 */
extern int __weak
pmt_telem_register_notifier(struct notifier_block *nb);

/**
 * pmt_telem_unregister_notifier() - Unregister notification of endpoint events
 * @nb:   Notifier block
 *
 */
extern int __weak
pmt_telem_unregister_notifier(struct notifier_block *nb);

/* *********************************
 * End PMT driver import
 * *********************************
 */

#define MAX_TELEM_ENDPOINTS MAX_AGGR_TELEM_ENDPOINTS /* For now */
static struct telem_endpoint *s_telem_endpoints[MAX_TELEM_ENDPOINTS]; /* TODO: make this a linked list instead */
size_t s_endpoint_index = 0;

static struct _sw_aggregator_msg s_telem_aggregators;

void sw_read_pmt_info(char *dst, int cpu,
		const struct sw_driver_io_descriptor *descriptor,
		u16 counter_size_in_bytes)
{
	u64 *data64 = (u64 *)dst;
	u32 *data32 = (u32 *)dst;
	int retval = 0;
	const struct sw_driver_aggr_telem_io_descriptor *td =
		&(descriptor->aggr_telem_descriptor);
	u32 offset = (u32)td->offset;
	u32 guid = (u32)td->guid;
	u32 pciDevId = (u32)td->pciDevId;

	struct telem_endpoint *ep = NULL;
	u32 i = 0;
	for (i = 0; i < s_telem_aggregators.num_telem_endpoints; i ++) {
		if (guid == s_telem_aggregators.info[i].globallyUniqueId &&
			pciDevId == s_telem_aggregators.info[i].pciDevId) {
				ep = s_telem_endpoints[i];
				break; // found the target endpoint, no need to continue looking
		}
	}
	if (!ep) {
		return;
	}
	pw_pr_debug("PMT: Reading counter from device:0x%x:0x%x at offset:0x%x size:%u\n",
		guid,
		pciDevId,
		offset,
		counter_size_in_bytes);

	switch (counter_size_in_bytes) {
	case 4:
		retval = pmt_telem_read32(ep, offset, data32, 1);
		pw_pr_debug("PMT: Value at offset 0x%x: 0x%x\n", offset, *data32);
		break;
	case 8:
		retval = pmt_telem_read64(ep, offset, data64, 1);
		pw_pr_debug("PMT: Value at offset 0x%x: 0x%llx\n", offset, *data64);
		break;
	default:
		pw_pr_error("PMT: Invalid PMT counter size %u\n", counter_size_in_bytes);
		return;
	}
	if (retval) {
		pw_pr_error("PMT: Error reading %u byte PMT value from offset 0x%x, val = %d\n", descriptor->counter_size_in_bytes, offset, retval);
	}
}

bool sw_pmt_available(void)
{
	/* 1: check if the PMT driver is loaded */
	if (!pmt_telem_get_endpoint_info) {
		pw_pr_debug("PMT driver not found!\n");
		return false;
	}
	pw_pr_debug("PMT driver found!\n");
	/* 2: TODO: other checks here */
	/*
	 * Note: registering telemetry endpoints done in 'register' since
	 * those endpoints also need to be unregistered (Done in 'fini')
	 */
	return true;
}

bool sw_pmt_register(void)
{
	unsigned long index = 0;
	if (!sw_pmt_available()) {
		return false;
	}
	s_telem_aggregators.num_telem_endpoints = 0;
	s_endpoint_index = 0;
	/*
	 * Retrieve list of telemetry endpoints.
	 */
	s_endpoint_index = 0;
	while ((index = pmt_telem_get_next_endpoint(index)) && s_endpoint_index < (MAX_TELEM_ENDPOINTS-1)) {
		struct telem_endpoint_info ep_info;
		if (pmt_telem_get_endpoint_info(index, &ep_info)) {
			pw_pr_error("PMT: Could not retrieve telemetry header for PMT endpoint %lu\n", index);
			continue;
		}
		s_telem_endpoints[s_endpoint_index] = pmt_telem_register_endpoint(index);
		s_telem_aggregators.info[s_telem_aggregators.num_telem_endpoints].globallyUniqueId = ep_info.header.guid;
		s_telem_aggregators.info[s_telem_aggregators.num_telem_endpoints].pciDevId = PCI_DEVID(ep_info.pdev->bus->number, ep_info.pdev->devfn);
		s_telem_aggregators.num_telem_endpoints++;
		++s_endpoint_index;
		pw_pr_debug("PMT: Found PMT endpoint guid:0x%x pciId:0x%0x\n", ep_info.header.guid, PCI_DEVID(ep_info.pdev->bus->number, ep_info.pdev->devfn));
	}
	return s_endpoint_index > 0;
}

bool sw_pmt_unregister(void)
{
	size_t i=0;
	if (!sw_pmt_available()) {
		return false;
	}
	for (i=0; i<s_endpoint_index; ++i) {
		pmt_telem_unregister_endpoint(s_telem_endpoints[i]);
	}
	s_endpoint_index = 0;
	s_telem_aggregators.num_telem_endpoints = 0;
	return true;
}

struct _sw_aggregator_msg *sw_pmt_aggregators(void)
{
	return &s_telem_aggregators;
}
