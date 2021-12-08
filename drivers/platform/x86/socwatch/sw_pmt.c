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
 * pmt_telem_read() - Read qwords from telemetry sram
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
pmt_telem_read(struct telem_endpoint *ep, u32 offset, u64 *data,
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
static struct telem_endpoint* s_telem_endpoints[MAX_TELEM_ENDPOINTS]; /* TODO: make this a linked list instead */
size_t s_endpoint_index = 0;

static struct _sw_aggregator_msg s_telem_aggregators;

void sw_read_pmt_info(char *dst, int cpu,
		const struct sw_driver_io_descriptor *descriptor,
		u16 counter_size_in_bytes)
{
	struct sw_pmt_payload *payload = (struct sw_pmt_payload *)dst;
	int retval = 0;
	const struct sw_driver_aggr_telem_io_descriptor *td =
		&(descriptor->aggr_telem_descriptor);
	u32 sampleId = (u32)td->sample_id;
	u32 guid = (u32)td->guid;
	u16 epId = (u16)td->endpoint_id;
	u16 pciId = 0;

	struct telem_endpoint *ep = NULL;
	u32 index = 0;
	for (index = 0; index < s_telem_aggregators.num_telem_endpoints; index ++) {
		if (epId == s_telem_aggregators.info[index].epId &&
			guid == s_telem_aggregators.info[index].globallyUniqueId) {
			ep = s_telem_endpoints[index];
			pciId = s_telem_aggregators.info[index].pciId;
			break; // found the target endpoint, no need to continue looking
		}
	}
	if (!ep) {
		return;
	}
	pw_pr_debug("PMT: Reading counter from device:0x%x:0x%x:0x%x at sample_id:0x%x.\n",
		guid,
		pciId,
		epId,
		sampleId);

	payload->GUID = guid;
	payload->pciId = (sw_pmt_pci_location)pciId;
	payload->epId = epId;

	retval = pmt_telem_read(ep, sampleId, &(payload->data), 1);
	pw_pr_debug("PMT: Value at offset 0x%x: 0x%llx\n", sampleId, payload->data);

	if (retval) {
		pw_pr_error("PMT: Error reading PMT value from sample_id %d, val = %d\n", sampleId, retval);
	}
}

bool sw_pmt_available(void)
{
	/* 1: check if the PMT driver is loaded */
	if (!pmt_telem_read) {
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
	unsigned long handle = 0;
	sw_pmt_pci_location pciId;
	if (!sw_pmt_available()) {
		return false;
	}
	s_telem_aggregators.num_telem_endpoints = 0;
	s_endpoint_index = 0;
	/*
	 * Retrieve list of telemetry endpoints.
	 */
	while ((handle = pmt_telem_get_next_endpoint(handle)) && s_endpoint_index < (MAX_TELEM_ENDPOINTS-1)) {
		struct telem_endpoint_info ep_info;
		if (pmt_telem_get_endpoint_info(handle, &ep_info)) {
			pw_pr_error("PMT: Could not retrieve telemetry header for PMT endpoint %lu\n", handle);
			continue;
		}
		s_telem_endpoints[s_endpoint_index] = pmt_telem_register_endpoint(handle);
		s_telem_aggregators.info[s_telem_aggregators.num_telem_endpoints].globallyUniqueId = ep_info.header.guid;
		s_telem_aggregators.info[s_telem_aggregators.num_telem_endpoints].epId = handle;

		pciId.bdf.busNumber = ep_info.pdev->bus->number;
		pciId.bdf.deviceNumber = PCI_SLOT(ep_info.pdev->devfn);
		pciId.bdf.functionNumber = PCI_FUNC(ep_info.pdev->devfn);

		s_telem_aggregators.info[s_telem_aggregators.num_telem_endpoints].pciId = pciId.busSlot;
		pw_pr_debug("PMT: Found PMT endpoint guid:0x%x epId:0x%lx pciId:%d|%d:%d:%d\n", ep_info.header.guid, handle,
								s_telem_aggregators.info[s_telem_aggregators.num_telem_endpoints].pciId,
				pciId.bdf.busNumber, pciId.bdf.deviceNumber, pciId.bdf.functionNumber);

		s_telem_aggregators.num_telem_endpoints++;
		++s_endpoint_index;
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
