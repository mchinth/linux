/* ***********************************************************************************************
 *
 *  This file is provided under a dual BSD/GPLv2 license.  When using or
 *  redistributing this file, you may do so under either license.
 *
 *  GPL LICENSE SUMMARY
 *
 *  Copyright (C) 2019-2021 Intel Corporation. All rights reserved.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of version 2 of the GNU General Public License as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  BSD LICENSE
 *
 *  Copyright (C) 2019-2021 Intel Corporation. All rights reserved.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in
 *      the documentation and/or other materials provided with the
 *      distribution.
 *    * Neither the name of Intel Corporation nor the names of its
 *      contributors may be used to endorse or promote products derived
 *      from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *  ***********************************************************************************************
*/

#ifndef _PMU_INFO_H_INC_
#define _PMU_INFO_H_INC_

static const PMU_INFO_NODE pmu_info_list[] = {
	// CTI_Avoton = 0x406D0
	{ 0x6, 0x4D, 0x0, 0xF, NULL, NULL, atm_mmio_info_list },

	// CTI_Anniedale = 0x506A0
	{ 0x6, 0x5A, 0x0, 0xF, NULL, NULL, atm_mmio_info_list },

	// CTI_Broxton = 0x506C0
	{ 0x6, 0x5C, 0x0, 0xF, NULL, NULL, bxt_mmio_info_list },

	// CTI_Cherryview = 0x406C0
	{ 0x6, 0x4C, 0x0, 0xF, NULL, NULL, atm_mmio_info_list },

	// CTI_CougarMountain = 0x60E0
	{ 0x6, 0x6E, 0x0, 0xF, NULL, NULL, atm_mmio_info_list },

	// CTI_Denverton = 0x506F0
	{ 0x6, 0x5F, 0x0, 0xF, NULL, NULL, dnv_mmio_info_list },

	// CTI_Geminilake = 0x706A0
	{ 0x6, 0x7A, 0x0, 0xF, NULL, NULL, bxt_mmio_info_list },

	// CTI_Tangier = 0x406A0
	{ 0x6, 0x4A, 0x0, 0xF, NULL, NULL, atm_mmio_info_list },

	// CTI_Silvermont = 0x30670
	{ 0x6, 0x37, 0x0, 0xF, NULL, NULL, atm_mmio_info_list },

	// CTI_Broadwell = 0x306D0
	{ 0x6, 0x3D, 0x0, 0xF, NULL, NULL, client_common_mmio_list },

	// CTI_Cannonlake = 0x60660
	{ 0x6, 0x66, 0x0, 0xF, NULL, NULL, client_common_mmio_list },

	// CTI_Crystalwell = 0x40660
	{ 0x6, 0x46, 0x0, 0xF, NULL, NULL, client_common_mmio_list },

	// CTI_Haswell = 0x306C0
	{ 0x6, 0x3C, 0x0, 0xF, NULL, NULL, client_common_mmio_list },

	// CTI_Haswell_ULT = 0x40650
	{ 0x6, 0x45, 0x0, 0xF, NULL, NULL, client_common_mmio_list },

	// CTI_Skylake = 0x506E0
	{ 0x6, 0x5E, 0x0, 0xF, NULL, NULL, client_common_mmio_list },

	// Last
	{ 0x0, 0x0, 0x0, 0x0, NULL, NULL, NULL }
};

#endif
