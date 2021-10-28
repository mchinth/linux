/* SPDX-License-Identifier: GPL-2.0 AND BSD-3-Clause
 *
 * This file is provided under a dual BSD/GPLv2 license.  When using or
 * redistributing this file, you may do so under either license.
 *
 * GPL LICENSE SUMMARY
 *
 * Copyright(c) 2021 Intel Corporation.
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
 * Copyright(c) 2021 Intel Corporation.
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

#ifndef __SW_PMT_STRUCTS_H__
#define __SW_PMT_STRUCTS_H__ 1

#include "sw_types.h"

/*
 * AGGREGATOR TELEMETRY
 */
#define MAX_AGGR_TELEM_ENDPOINTS 256

#pragma pack(push, 1)
/**
 * struct - sw_driver_aggr_telem_io_descriptor - Aggregate Telemetry Metric descriptor
 * This descriptor is used to interact with TA and PMT driver to get aggregate telemetry data
 * @num_entries: number of entries we want to read from aggregate telemetry SRAM.
 * Note: These entries should be contigous then only TA and PMT driver can read them together
 * @offset First offset which we want to read from aggregate telemetry data
 * All the offsets are specified in the XML file
 */
struct sw_driver_aggr_telem_io_descriptor {
	pw_u64_t  data_remapped_address;
	pw_u32_t  sample_id;
	pw_u32_t  guid;
	pw_u16_t  endpoint_id;
};
#pragma pack(pop)

#pragma pack(push, 1)
/*
 * Union PMT endpoint PCI location
 * it needs to be exactly 16 bits.
 * can be set using the 16 bit busSlot
 * or using the individual Bus, Device, Function
 * fields in the bdf structure.
 */
typedef union _sw_pmt_pci_location {
	pw_u16_t busSlot;
	struct _bdf {
		pw_u8_t busNumber;          //  0-255
		pw_u8_t deviceNumber : 5;   //  0-31
		pw_u8_t functionNumber : 3; //  0-7
	}bdf;
} sw_pmt_pci_location;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct sw_pmt_payload {
	pw_u32_t GUID;
	sw_pmt_pci_location pciId; // Must be 16 bits
	pw_u16_t epId;
	pw_u64_t data;
} sw_pmt_payload_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct _sw_aggregator_info {
	pw_u64_t startAddress;
	pw_u32_t globallyUniqueId;
	pw_u32_t size;
	pw_u16_t epId;
	pw_u16_t pciId;
	pw_u16_t collectionType; // SW_IO_AGGR_TA or SW_IO_AGGR_PMT
} sw_aggregator_info;

typedef struct _sw_aggregator_msg {
	pw_u32_t num_telem_endpoints;
	sw_aggregator_info info[MAX_AGGR_TELEM_ENDPOINTS]; /* Array of sw_aggregator_info structs. */
} sw_aggregator_msg;

#define AGGREGATOR_BUFFER_SIZE(num_telem_endpoints) (sizeof(sw_aggregator_info) * num_telem_endpoints + sizeof(pw_u32_t))
#pragma pack(pop)

#endif /* __SW_PMT_STRUCTS_H__ */
