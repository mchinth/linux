/****
    Copyright (C) 2011 Intel Corporation.  All Rights Reserved.

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









#ifndef _MSRDEFS_H_
#define _MSRDEFS_H_

/*
 * Arch Perf monitoring version 3
 */
#define IA32_PMC0                 0x0C1
#define IA32_PMC1                 0x0C2
#define IA32_PMC2                 0x0C3
#define IA32_PMC3                 0x0C4
#define IA32_PMC4                 0x0C5
#define IA32_PMC5                 0x0C6
#define IA32_PMC6                 0x0C7
#define IA32_PMC7                 0x0C8
#define IA32_FULL_PMC0            0x4C1
#define IA32_FULL_PMC1            0x4C2
#define IA32_PERFEVTSEL0          0x186
#define IA32_PERFEVTSEL1          0x187
#define IA32_PERFEVTSEL2          0x188
#define IA32_PERFEVTSEL3          0x189
#define IA32_FIXED_CTR0           0x309
#define IA32_FIXED_CTR1           0x30A
#define IA32_FIXED_CTR2           0x30B
#define IA32_FIXED_CTR3           0x30C
#define IA32_PERF_CAPABILITIES    0x345
#define IA32_FIXED_CTRL           0x38D
#define IA32_PERF_GLOBAL_STATUS   0x38E
#define IA32_PERF_GLOBAL_CTRL     0x38F
#define IA32_PERF_GLOBAL_OVF_CTRL 0x390
#define IA32_PEBS_ENABLE          0x3F1
#define IA32_MISC_ENABLE          0x1A0
#define IA32_DS_AREA              0x600
#define IA32_DEBUG_CTRL           0x1D9
#undef IA32_LBR_FILTER_SELECT
#define IA32_LBR_FILTER_SELECT 0x1c8
#define IA32_PEBS_FRONTEND     0x3F7
#define IA32_PERF_METRICS      0x329
#define IA32_LBR_CTRL          0x14CE

#define COMPOUND_CTR_CTL       0x306
#define COMPOUND_PERF_CTR      0x307
#define COMPOUND_CTR_OVF_BIT   0x800
#define COMPOUND_CTR_OVF_SHIFT 12

#define FIXED_CORE_CYCLE_GLOBAL_CTRL_MASK 0x200000000
#define FIXED_CORE_CYCLE_FIXED_CTRL_MASK  0xF0

/*
 * Intel Processor Trace (IPT)
 */
#define IA32_RTIT_CTL              0x570
#define IA32_RTIT_STATUS           0x571
#define IA32_RTIT_CR3_MATCH        0x572
#define IA32_RTIT_OUTPUT_BASE      0x560
#define IA32_RTIT_OUTPUT_MASK_PTRS 0x561
#define IA32_RTIT_ADDR0_A          0x580
#define IA32_RTIT_ADDR0_B          0x581
#define IA32_RTIT_ADDR1_A          0x582
#define IA32_RTIT_ADDR1_B          0x583
#define IA32_RTIT_ADDR2_A          0x584
#define IA32_RTIT_ADDR2_B          0x585
#define IA32_RTIT_ADDR3_A          0x586
#define IA32_RTIT_ADDR3_B          0x587
#define IA32_VMX_MISC              0x485

#define IA32_RTIT_CTL_TRACEEN   0x1
#define IA32_RTIT_CTL_CYCEN     0x2
#define IA32_RTIT_CTL_OS        0x4
#define IA32_RTIT_CTL_USER      0x8
#define IA32_RTIT_CTL_PWREVTEN  0x10
#define IA32_RTIT_CTL_FUPONPTW  0x20
#define IA32_RTIT_CTL_FABRICEN  0x40
#define IA32_RTIT_CTL_CR3FILTER 0x80
#define IA32_RTIT_CTL_TOPA      0x100
#define IA32_RTIT_CTL_MTCEN     0x200
#define IA32_RTIT_CTL_TSCEN     0x400
#define IA32_RTIT_CTL_DISRETC   0x800
#define IA32_RTIT_CTL_PTWEN     0x1000
#define IA32_RTIT_CTL_BRANCHEN  0x2000
#define IA32_VMX_MISC_VMXONPT   0x4000

#define RTIT_OUTPUT_MASK_PTRS_TABLEOFFSET 0xFFFFFF80ULL

#define PERF_GLOBAL_STATUS_TOPA_PMI_MASK 0x80000000000000ULL

// REG INDEX inside GLOBAL CTRL SECTION
enum {
	GLOBAL_CTRL_REG_INDEX = 0,
	GLOBAL_OVF_CTRL_REG_INDEX,
	PEBS_ENABLE_REG_INDEX,
	DEBUG_CTRL_REG_INDEX,
	FIXED_CTRL_REG_INDEX,
};

// REG INDEX inside GLOBAL STATUS SECTION
enum {
	GLOBAL_STATUS_REG_INDEX = 0,
};

#endif

