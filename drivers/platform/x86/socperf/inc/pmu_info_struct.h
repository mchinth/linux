/* ***********************************************************************************************

  This file is provided under a dual BSD/GPLv2 license.  When using or
  redistributing this file, you may do so under either license.

  GPL LICENSE SUMMARY

  Copyright (C) 2019-2021 Intel Corporation. All rights reserved.

  This program is free software; you can redistribute it and/or modify
  it under the terms of version 2 of the GNU General Public License as
  published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
  The full GNU General Public License is included in this distribution
  in the file called LICENSE.GPL.

  BSD LICENSE

  Copyright (C) 2019-2021 Intel Corporation. All rights reserved.
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:

    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in
      the documentation and/or other materials provided with the
      distribution.
    * Neither the name of Intel Corporation nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  ***********************************************************************************************
*/

#ifndef _PMU_INFO_STRUCT_H_INC_
#define _PMU_INFO_STRUCT_H_INC_

// Data Structures for storing entire PMU list
typedef struct PMU_MSR_INFO_NODE_S PMU_MSR_INFO_NODE;
struct PMU_MSR_INFO_NODE_S {
	U64 msr_id;
	U64 mask;
	U16 range;
	U16 dynamic; // to be updated
};

typedef struct PMU_PCI_INFO_NODE_S PMU_PCI_INFO_NODE;
struct PMU_PCI_INFO_NODE_S {
	union {
		struct {
			U64 bus    : 8;
			U64 dev    : 5;
			U64 func   : 3;
			U64 offset : 16;
			U64 rsvd   : 32;
		} s;
		U64 reg;
	} u;
};

typedef struct PMU_PCI_UNIT_INFO_NODE_S PMU_PCI_UNIT_INFO_NODE;
struct PMU_PCI_UNIT_INFO_NODE_S {
	U32  dev;
	U32  func;
	U16 *reg_offset_list;
};

typedef struct PMU_MMIO_BAR_INFO_NODE_S PMU_MMIO_BAR_INFO_NODE;
struct PMU_MMIO_BAR_INFO_NODE_S {
	union {
		struct {
			U32 bus    : 8;
			U32 dev    : 5;
			U32 func   : 3;
			U32 offset : 16;
		} s;
		U32 reg;
	} u;
	U8  shift;
	U8  bar_prog_type;
	U16 reserved;
	U64 mask;
};

enum { MMIO_SINGLE_BAR_TYPE = 1, MMIO_DUAL_BAR_TYPE, MMIO_DIRECT_BAR_TYPE };

typedef struct PMU_MMIO_UNIT_INFO_NODE_S PMU_MMIO_UNIT_INFO_NODE;
struct PMU_MMIO_UNIT_INFO_NODE_S {
	PMU_MMIO_BAR_INFO_NODE primary;
	PMU_MMIO_BAR_INFO_NODE secondary;
	U32                   *reg_offset_list;
};

typedef struct PMU_INFO_NODE_S PMU_INFO_NODE;
struct PMU_INFO_NODE_S {
	U16                      family;
	U16                      model;
	U16                      stepping_from;
	U16                      stepping_to;
	PMU_MSR_INFO_NODE      **msr_info_list;
	PMU_PCI_UNIT_INFO_NODE  *pci_info_list;
	PMU_MMIO_UNIT_INFO_NODE *mmio_info_list;
};

// Data Structure for search operation
typedef struct PMU_SEARCH_NODE_S PMU_SEARCH_NODE;
struct PMU_SEARCH_NODE_S {
	U64              key;  // common for MSR/PCI/MMIO
	void            *addr; // copy address of static node
	PMU_SEARCH_NODE *left;
	PMU_SEARCH_NODE *right;
	U16              height; // For balancing the search tree
	U16              range;  // For MSR
};

#endif
