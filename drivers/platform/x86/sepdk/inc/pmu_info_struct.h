/****
    Copyright (C) 2019 Intel Corporation.  All Rights Reserved.

    This file is part of SEP Development Kit.

    SEP Development Kit is free software; you can redistribute it
    and/or modify it under the terms of the GNU General Public License
    version 2 as published by the Free Software Foundation.

    SEP Development Kit is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
****/









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
			U64 bus : 8;
			U64 dev : 5;
			U64 func : 3;
			U64 offset : 16;
			U64 rsvd : 32;
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
			U32 bus : 8;
			U32 dev : 5;
			U32 func : 3;
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

