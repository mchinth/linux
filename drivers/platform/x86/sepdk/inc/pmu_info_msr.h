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









#ifndef _PMU_INFO_MSR_H_INC_
#define _PMU_INFO_MSR_H_INC_

static PMU_MSR_INFO_NODE perfmon_v4_architectural[] = {
	{ 0x1D9 }, { 0x3F1 },         { 0x345, 0, 0, 1 }, { 0x38D, 0, 0, 1 },
	{ 0x38E }, { 0x38F },         { 0x390 },          { 0x600 },
	{ 0x1A0 }, { 0x186, 0, 0x7 }, { 0x8b },           { 0x392 },
	{ 0x0 }
};

static PMU_MSR_INFO_NODE perfmon_v5_architectural[] = {
	{ 0x1D9 }, { 0x3F1 },         { 0x345, 0, 0, 1 }, { 0x38D, 0, 0, 1 },
	{ 0x38E }, { 0x38F },         { 0x390 },          { 0x600 },
	{ 0x1A0 }, { 0x186, 0, 0x7 }, { 0x8b },           { 0x392 },
	{ 0x3F2 }, { 0x0 }
};

static PMU_MSR_INFO_NODE core_1[] = {
	{ 0x1D9 }, { 0x3F1 }, { 0x345, 0, 0, 1 }, { 0x38D, 0, 0, 1 },
	{ 0x38E }, { 0x38F }, { 0x390 },          { 0x1C8 },
	{ 0x3F7 }, { 0x600 }, { 0x1A0 },          { 0x186, 0, 0x3 },
	{ 0x1C9 }, { 0xCE },  { 0x1A4 },          { 0x1A6 },
	{ 0x1A7 }, { 0x34 },  { 0x0 }
};

static PMU_MSR_INFO_NODE core_2[] = {
	{ 0xC5, 0, 0, 1 }, { 0xC6, 0, 0, 1 }, { 0xC7, 0, 0, 1 },
	{ 0xC8, 0, 0, 1 }, { 0x3F6 },         { 0x18A, 0, 0x3, 1 },
	{ 0x1AD },         { 0x1AE },         { 0x0 }
};

static PMU_MSR_INFO_NODE core_3[] = {
	{ 0x680, 0, 0xF }, { 0x6C0, 0, 0xF }, { 0x1B }, { 0x0 }
};

static PMU_MSR_INFO_NODE core_4[] = {
	{ 0x3F2 }, { 0x30C }, { 0x329, 0, 0, 1 }, { 0x0 }
};

static PMU_MSR_INFO_NODE core_5[] = {
	{ 0x40, 0, 0x7 }, { 0x60, 0, 0x7 }, { 0xCD }, { 0x0 }
};

static PMU_MSR_INFO_NODE core_6[] = { { 0xDC0, 0, 0x1F }, { 0x0 } };

static PMU_MSR_INFO_NODE core_7[] = {
	{ 0x690, 0, 0xF }, { 0x6D0, 0, 0xF }, { 0x0 }
};

static PMU_MSR_INFO_NODE core_8[] = { { 0x1AF }, { 0x0 } };

static PMU_MSR_INFO_NODE core_9[] = {
	{ 0x1500, 0, 0x1F }, { 0x1600, 0, 0x1F }, { 0x1200, 0, 0x1F },
	{ 0x14CE },          { 0x14CF },          { 0x1DD },
	{ 0x1DE },           { 0x1E0 },           { 0x0 }
};

static PMU_MSR_INFO_NODE core_10[] = {
	{ 0xC5 }, { 0xC6 }, { 0x18A }, { 0x18B }, { 0x0 }
};

static PMU_MSR_INFO_NODE core_11[]  = { { 0x392 }, { 0x0 } };

static PMU_MSR_INFO_NODE uncore_2[] = { { 0x700 }, { 0x702 }, { 0x0 } };

static PMU_MSR_INFO_NODE uncore_3[] = { { 0x1F80 }, { 0x1F82 }, { 0x0 } };

static PMU_MSR_INFO_NODE uncore_4[] = { { 0xE01, 0, 0x1 }, { 0x0 } };

static PMU_MSR_INFO_NODE uncore_5[] = { { 0x391, 0, 0x1 }, { 0x0 } };

static PMU_MSR_INFO_NODE uncore_6[] = { { 0x2FF0 }, { 0x2FFE }, { 0x0 } };

static PMU_MSR_INFO_NODE uncore_7[] = { { 0x2FF0 }, { 0x2FF2 }, { 0x0 } };

static PMU_MSR_INFO_NODE pmu1_1[] = { { 0x396 }, { 0x0 } };

static PMU_MSR_INFO_NODE pmu1_2[] = {
	{ 0xE00, 0, 0x6 }, { 0xE08, 0, 0x4 }, { 0xE10, 0, 0xB },
	{ 0xE20, 0, 0xB }, { 0xE30, 0, 0xB }, { 0xE40, 0, 0xB },
	{ 0xE50, 0, 0xB }, { 0xE60, 0, 0xB }, { 0xE70, 0, 0xB },
	{ 0xE80, 0, 0xB }, { 0xE90, 0, 0xB }, { 0xEA0, 0, 0xB },
	{ 0xEB0, 0, 0xB }, { 0xEC0, 0, 0xB }, { 0xED0, 0, 0xB },
	{ 0xEE0, 0, 0xB }, { 0xEF0, 0, 0xB }, { 0xF00, 0, 0xB },
	{ 0xF10, 0, 0xB }, { 0xF20, 0, 0xB }, { 0xF30, 0, 0xB },
	{ 0xF40, 0, 0xB }, { 0xF50, 0, 0xB }, { 0xF60, 0, 0xB },
	{ 0xF70, 0, 0xB }, { 0x0 }
};

static PMU_MSR_INFO_NODE pmu1_6[] = {
	{ 0x700, 0, 0x4 }, { 0x708, 0, 0x4 }, { 0x710, 0, 0x4 },
	{ 0x718, 0, 0x4 }, { 0x720, 0, 0x4 }, { 0x728, 0, 0x4 },
	{ 0x730, 0, 0x4 }, { 0x738, 0, 0x4 }, { 0x0 }
};

static PMU_MSR_INFO_NODE pmu1_7[] = {
	{ 0x700, 0, 0x69 }, { 0xF70, 0, 0x29 }, { 0x0 }
};

static PMU_MSR_INFO_NODE pmu1_8[] = { { 0x700, 0, 0x49 }, { 0x0 } };

static PMU_MSR_INFO_NODE pmu1_9[] = { { 0x2000, 0, 0x7C }, { 0x0 } };

static PMU_MSR_INFO_NODE pmu2_1[] = { { 0xE00, 0, 0x1BB }, { 0x0 } };

static PMU_MSR_INFO_NODE pmu2_2[] = { { 0xE00, 0, 0x1C8 }, { 0x0 } };

static PMU_MSR_INFO_NODE pmu2_3[] = {
	{ 0xE00, 0, 0x1E7 }, { 0xB60, 0, 0x6D }, { 0x0 }
};

static PMU_MSR_INFO_NODE pmu2_4[] = { { 0x1C00, 0, 0x5B }, { 0x0 } };

static PMU_MSR_INFO_NODE pmu2_5[] = { { 0x2000, 0, 0x7FF }, { 0x0 } };

static PMU_MSR_INFO_NODE pmu3_1[] = { { 0x2800, 0, 0x7BF }, { 0x0 } };

static PMU_MSR_INFO_NODE pmu4_1[] = {
	{ 0xA40, 0, 0x5 }, { 0xA47, 0, 0x4 }, { 0xB00, 0, 0xF },
	{ 0xA60, 0, 0x5 }, { 0xA67, 0, 0x4 }, { 0xB10, 0, 0xF },
	{ 0xA80, 0, 0x5 }, { 0xA87, 0, 0x4 }, { 0xB20, 0, 0xF },
	{ 0xAA0, 0, 0x5 }, { 0xAA7, 0, 0x4 }, { 0xB30, 0, 0xF },
	{ 0xAC0, 0, 0x5 }, { 0xAC7, 0, 0x4 }, { 0xB40, 0, 0xF },
	{ 0x0 }
};

static PMU_MSR_INFO_NODE pmu4_2[] = {
	{ 0xA50, 0, 0x5 }, { 0xA57, 0, 0x4 }, { 0xAA0, 0, 0xF },
	{ 0xA70, 0, 0x5 }, { 0xA77, 0, 0x4 }, { 0xAB0, 0, 0xF },
	{ 0xA90, 0, 0x5 }, { 0xA97, 0, 0x4 }, { 0xAC0, 0, 0xF },
	{ 0xAE0, 0, 0x5 }, { 0xAE7, 0, 0x4 }, { 0xB30, 0, 0xF },
	{ 0xB00, 0, 0x5 }, { 0xB07, 0, 0x4 }, { 0xB40, 0, 0xF },
	{ 0xB20, 0, 0x5 }, { 0xB27, 0, 0x4 }, { 0xB50, 0, 0xF },
	{ 0x0 }
};

static PMU_MSR_INFO_NODE pmu4_3[] = {
	{ 0x1E00, 0, 0x4 }, { 0x1E07, 0, 0x4 }, { 0x1F00, 0, 0xF },
	{ 0x1E10, 0, 0x4 }, { 0x1E17, 0, 0x4 }, { 0x1F10, 0, 0xF },
	{ 0x1E20, 0, 0x4 }, { 0x1E27, 0, 0x4 }, { 0x1F20, 0, 0xF },
	{ 0x1E30, 0, 0x4 }, { 0x1E37, 0, 0x4 }, { 0x1F30, 0, 0xF },
	{ 0x1E40, 0, 0x4 }, { 0x1E47, 0, 0x4 }, { 0x1F40, 0, 0xF },
	{ 0x1EAC },         { 0x1EBC },         { 0x1ECC },
	{ 0x1EDC },         { 0x1EDC },         { 0x1EEC },
	{ 0x0 }
};

static PMU_MSR_INFO_NODE pmu4_4[] = {
	{ 0x3000, 0, 0x1FF }, { 0x3800, 0, 0x1FF }, { 0x0 }
};

static PMU_MSR_INFO_NODE pmu5_msr_reg_list1[] = { { 0x300, 0, 0 }, { 0x0 } };

static PMU_MSR_INFO_NODE pmu5_msr_reg_list2[] = {
	{ 0xA09, 0, 0 }, { 0xA0A, 0, 0 }, { 0x0 }
};

static PMU_MSR_INFO_NODE pmu6_1[] = {
	{ 0xA58 }, { 0xA78 }, { 0xA98 }, { 0xAB8 }, { 0xAD8 }, { 0xA5B },
	{ 0xA5C }, { 0xA7B }, { 0xA7C }, { 0xA9B }, { 0xA9C }, { 0xABB },
	{ 0xABC }, { 0xADB }, { 0xADC }, { 0xA5F }, { 0xA7F }, { 0xA9F },
	{ 0xABF }, { 0xADF }, { 0xA59 }, { 0xA5A }, { 0xA79 }, { 0xA7A },
	{ 0xA99 }, { 0xA9A }, { 0xAB9 }, { 0xABA }, { 0xAD9 }, { 0xADA },
	{ 0x0 }
};

static PMU_MSR_INFO_NODE pmu6_2[] = {
	{ 0xA4A }, { 0xA6A }, { 0xA8A }, { 0xADA }, { 0xAFA }, { 0xB1A },
	{ 0xA4F }, { 0xA6F }, { 0xA8F }, { 0xADF }, { 0xAFF }, { 0xB1F },
	{ 0xA4D }, { 0xA6D }, { 0xA8D }, { 0xADD }, { 0xAFD }, { 0xB1D },
	{ 0xA4E }, { 0xA6E }, { 0xA8E }, { 0xADE }, { 0xAFE }, { 0xB1E },
	{ 0xA4B }, { 0xA6B }, { 0xA8B }, { 0xADB }, { 0xAFB }, { 0xB1B },
	{ 0xA4C }, { 0xA6C }, { 0xA8C }, { 0xADC }, { 0xAFC }, { 0xB1C },
	{ 0x0 }
};

static PMU_MSR_INFO_NODE pmu6_3[] = {
	{ 0x1EA0 }, { 0x1EB0 }, { 0x1EC0 }, { 0x1ED0 }, { 0x1EE0 }, { 0x1EA8 },
	{ 0x1EA9 }, { 0x1EB8 }, { 0x1EB9 }, { 0x1EC8 }, { 0x1EC9 }, { 0x1ED8 },
	{ 0x1ED9 }, { 0x1EE8 }, { 0x1EE9 }, { 0x1EA7 }, { 0x1EB7 }, { 0x1EC7 },
	{ 0x1ED7 }, { 0x1EE7 }, { 0x1EA1 }, { 0x1EA2 }, { 0x1EB1 }, { 0x1EB2 },
	{ 0x1EC1 }, { 0x1EC2 }, { 0x1ED1 }, { 0x1ED2 }, { 0x1EE1 }, { 0x1EE2 },
	{ 0x0 }
};

static PMU_MSR_INFO_NODE pmu6_4[] = { { 0x3400, 0, 0x1FF }, { 0x0 } };

static PMU_MSR_INFO_NODE pmu7_1[] = {
	{ 0x394 }, { 0x395 }, { 0x396 }, { 0x3B0, 0, 0x3 }, { 0x0 }
};

static PMU_MSR_INFO_NODE pmu7_2[] = { { 0x3B8, 0, 0x3 }, { 0x0 } };

static PMU_MSR_INFO_NODE pmu7_3[] = {
	{ 0x2FD0, 0, 0x6 }, { 0x2FD8, 0, 0x6 }, { 0x2FDE }, { 0x2FDF }, { 0x0 }
};

static PMU_MSR_INFO_NODE pmu8_1[] = {
	{ 0x715 }, { 0x717, 0, 4 }, { 0x711, 0, 4 }, { 0x710 }, { 0x100 },
	{ 0x0 }
};

static PMU_MSR_INFO_NODE pmu8_2[] = { { 0x3FD }, { 0x3FC }, { 0x0 } };

static PMU_MSR_INFO_NODE pmu8_3[] = { { 0x3F9 }, { 0x3F8 }, { 0x0 } };

static PMU_MSR_INFO_NODE pmu8_5[] = {
	{ 0x1EF5 }, { 0x1EF8, 0, 4 }, { 0x1EF1, 0, 4 }, { 0x1EF0 }, { 0x0 }
};

static PMU_MSR_INFO_NODE pmu8_6[] = { { 0x2FC0, 0, 0xF }, { 0x0 } };

static PMU_MSR_INFO_NODE pmu9_1[] = {
	{ 0x198 }, { 0x19C }, { 0x1A2 }, { 0x1B0 }, { 0x3F8 }, { 0x3F9 },
	{ 0x3FA }, { 0x3FC }, { 0x3FD }, { 0x60D }, { 0x611 }, { 0x0 }
};

static PMU_MSR_INFO_NODE pmu9_2[] = { { 0x639 }, { 0x0 } };

static PMU_MSR_INFO_NODE pmu9_3[] = {
	{ 0x3FE }, { 0x606 }, { 0x614 }, { 0x619 }, { 0x621 }, { 0x61D },
	{ 0x0 }
};

static PMU_MSR_INFO_NODE pmu9_4[] = {
	{ 0x199 }, { 0x771 }, { 0x774 }, { 0x0 }
};

static PMU_MSR_INFO_NODE pmu9_5[] = { { 0x660, 0, 0x5 }, { 0x0 } };

static PMU_MSR_INFO_NODE pmu9_6[] = {
	{ 0x611 }, { 0x606 }, { 0x639 }, { 0x641 }, { 0x0 }
};

static PMU_MSR_INFO_NODE pmu9_7[] = {
	{ 0x60D }, { 0x3F8 }, { 0x3F9 }, { 0x0 }
};

static PMU_MSR_INFO_NODE pmu9_8[] = {
	{ 0x3FA }, { 0x3FC }, { 0x3FD }, { 0x0 }
};

static PMU_MSR_INFO_NODE pmu9_9[] = {
	{ 0x1A2 }, { 0x198 }, { 0x19C }, { 0x611 }, { 0x0 }
};

static PMU_MSR_INFO_NODE pmu9_11[] = {
	{ 0x19C }, { 0x1B1 }, { 0x619 }, { 0x3FE }, { 0x0 }
};

static PMU_MSR_INFO_NODE pmu9_12[] = {
	{ 0x19C }, { 0x1B1 }, { 0x630 }, { 0x631 }, { 0x632 }, { 0x619 },
	{ 0x3FE }, { 0x0 }
};

static PMU_MSR_INFO_NODE pmu9_13[] = {
	{ 0x630 }, { 0x631 }, { 0x632 }, { 0x619 }, { 0x3FE }, { 0x653 },
	{ 0x61D }, { 0x655 }, { 0x656 }, { 0x19C }, { 0x0 }
};

static PMU_MSR_INFO_NODE pmu9_14[] = { { 0x660 }, { 0x0 } };

static PMU_MSR_INFO_NODE pmu9_15[] = {
	{ 0x660 }, { 0x1B0 }, { 0x639 }, { 0x0 }
};

static PMU_MSR_INFO_NODE pmu9_16[] = { { 0x661, 0, 0x4 }, { 0x0 } };

static PMU_MSR_INFO_NODE pmu9_18[] = {
	{ 0x606 }, { 0x1B0 }, { 0x614 }, { 0x619 }, { 0x0 }
};

static PMU_MSR_INFO_NODE pmu9_19[] = { { 0x17D2 }, { 0x0 } };

static PMU_MSR_INFO_NODE pmu9_20[] = {
	{ 0x64F }, { 0x1B1 }, { 0x613 }, { 0x0 }
};

static PMU_MSR_INFO_NODE pmu10_1[] = {
	{ 0xA40, 0, 0x9 }, { 0xA60, 0, 0x9 }, { 0xA80, 0, 0x9 },
	{ 0xAD0, 0, 0x9 }, { 0xAF0, 0, 0x9 }, { 0xB10, 0, 0x9 }, { 0x0 }
};

static PMU_MSR_INFO_NODE pmu10_2[] = {
	{ 0x1E50, 0, 0x4 }, { 0x1E60, 0, 0x4 }, { 0x1E70, 0, 0x4 },
	{ 0x1E80, 0, 0x4 }, { 0x1E90, 0, 0x4 }, { 0x1E57, 0, 0x4 },
	{ 0x1E67, 0, 0x4 }, { 0x1E77, 0, 0x4 }, { 0x1E87, 0, 0x4 },
	{ 0x1E97, 0, 0x4 }, { 0x0 }
};

static PMU_MSR_INFO_NODE pmu10_3[] = { { 0x3200, 0, 0x1FF }, { 0x0 } };

static PMU_MSR_INFO_NODE pmu11_1[] = {
	{ 0x720 },         { 0x72A },         { 0x734 },
	{ 0x73E },         { 0x721, 0, 0x3 }, { 0x72B, 0, 0x3 },
	{ 0x735, 0, 0x3 }, { 0x73F, 0, 0x3 }, { 0x726, 0, 0x3 },
	{ 0x730, 0, 0x3 }, { 0x73A, 0, 0x3 }, { 0x744, 0, 0x3 },
	{ 0x0 }
};

static PMU_MSR_INFO_NODE pmu12_1[] = {
	{ 0xC14 }, { 0x620 }, { 0x703, 0, 0x7 }, { 0x0 }
};

static PMU_MSR_INFO_NODE pmu12_2[] = {
	{ 0xC14 }, { 0x620 }, { 0x1F91, 0, 0x3 }, { 0x1F98, 0, 0x1 }, { 0x0 }
};

static PMU_MSR_INFO_NODE pmu12_4[] = {
	{ 0x620 },        { 0x2FD0 }, { 0x2FDE, 0, 0x2 }, { 0x2FD2, 0, 2 },
	{ 0x2FD8, 0, 2 }, { 0x0 }
};

static PMU_MSR_INFO_NODE pmu13[] = {
	{ 0xC8D }, { 0xC8E }, { 0xC8F }, { 0xD10 }, { 0xD11 }, { 0xD12 },
	{ 0xD13 }, { 0xC90 }, { 0xC91 }, { 0xC92 }, { 0xC93 }, { 0x0 }
};

static PMU_MSR_INFO_NODE *perfmon_v4_msr_list[] = {
	perfmon_v4_architectural,  0
};

static PMU_MSR_INFO_NODE *perfmon_v5_msr_list[] = {
	perfmon_v5_architectural, 0
};

static PMU_MSR_INFO_NODE *bdx_msr_list[] = {
	core_1, core_2, core_3, core_8, pmu1_1, pmu1_2,  pmu1_8,  pmu8_1,
	pmu8_2, pmu9_1, pmu9_2, pmu9_3, pmu9_4, pmu11_1, pmu12_1, uncore_2,
	pmu13,  0
};

static PMU_MSR_INFO_NODE *skx_msr_list[] = {
	core_1, core_2, core_3,  core_6,   core_7,             core_11,
	pmu2_1, pmu4_1, pmu6_1,  pmu8_1,   pmu8_2,             pmu9_1,
	pmu9_2, pmu9_3, pmu12_1, uncore_2, pmu5_msr_reg_list1, 0
};

static PMU_MSR_INFO_NODE *skl_msr_list[] = {
	core_1, core_2, core_3, core_6, core_7,  core_11,  pmu1_1, pmu1_7,
	pmu7_1, pmu9_6, pmu9_7, pmu9_8, pmu9_13, uncore_4, 0
};

static PMU_MSR_INFO_NODE *clx_msr_list[] = {
	core_1,             core_2, core_3,  core_6,  core_7,   core_11,
	pmu2_1,             pmu4_1, pmu6_1,  pmu8_1,  pmu8_2,   pmu9_1,
	pmu9_2,             pmu9_3, pmu9_11, pmu12_1, uncore_2, pmu13,
	pmu5_msr_reg_list1, 0
};

static PMU_MSR_INFO_NODE *plat1_msr_list[] = {
	core_1, core_2, core_3,  core_4, core_6, core_7,   core_11,
	pmu1_1, pmu1_6, pmu9_14, pmu7_1, pmu7_2, uncore_4, 0
};

static PMU_MSR_INFO_NODE *icx_r_msr_list[] = {
	core_1,   core_2,   core_3, core_4, core_6, core_7,  core_11,
	pmu2_3,   pmu4_2,   pmu6_2, pmu8_1, pmu8_3, pmu10_1, pmu12_1,
	uncore_2, uncore_5, pmu9_1, pmu9_2, pmu9_3, pmu13,   0
};

static PMU_MSR_INFO_NODE *hsw_msr_list[] = {
	core_1, core_2, core_3,  core_8,   pmu1_1, pmu1_8, pmu7_1, pmu9_6,
	pmu9_7, pmu9_8, pmu9_11, uncore_5, 0
};

static PMU_MSR_INFO_NODE *hsw_ult_msr_list[] = {
	core_1, core_2, core_3,  core_8,   pmu1_1,   pmu1_8, pmu7_1, pmu9_6,
	pmu9_7, pmu9_8, pmu9_12, uncore_5, 0
};

static PMU_MSR_INFO_NODE *slm_msr_list[] = {
	core_1, core_5, pmu9_8, pmu9_9, pmu9_15, 0
};

static PMU_MSR_INFO_NODE *avt_msr_list[] = {
	core_1, core_5, pmu9_1, pmu9_2, pmu9_5, 0
};

static PMU_MSR_INFO_NODE *and_msr_list[] = {
	core_1, core_5, pmu9_7, pmu9_8, pmu9_9, pmu9_15, pmu9_16, 0
};

static PMU_MSR_INFO_NODE *bxt_msr_list[] = {
	core_1, core_3, core_6, core_7, core_11, pmu9_8, pmu9_9, pmu9_15,
	pmu13,  0 };

static PMU_MSR_INFO_NODE *dnv_msr_list[] = {
	core_1, core_3, core_6, core_11, pmu9_1, pmu9_2, pmu9_5, 0
};

static PMU_MSR_INFO_NODE *gml_msr_list[] = {
	core_1, core_3, core_6, core_11, pmu9_2, pmu9_9, 0
};

static PMU_MSR_INFO_NODE *hsx_msr_list[] = {
	core_1, core_2, core_3, core_8, pmu1_1,  pmu1_2,  pmu1_8,   pmu8_1,
	pmu8_2, pmu9_1, pmu9_2, pmu9_3, pmu11_1, pmu12_1, uncore_2, 0
};

static PMU_MSR_INFO_NODE *icl_msr_list[] = {
	core_1, core_2, core_3,   core_4,   core_6, core_7, core_11, pmu1_1,
	pmu1_6, pmu7_1, uncore_4, uncore_5, pmu9_6, pmu9_7, pmu9_8,  pmu9_13,
	0
};

static PMU_MSR_INFO_NODE *snr_msr_list[] = {
	core_1, core_2, core_3, core_4, core_6,  core_7,  core_11,  pmu2_4,
	pmu4_3, pmu6_3, pmu8_2, pmu8_5, pmu10_2, pmu12_2, uncore_3, pmu9_1,
	pmu9_2, pmu9_3, 0
};

static PMU_MSR_INFO_NODE *knl_msr_list[] = {
	core_1,  core_2,  core_3,   pmu2_2, pmu9_2, pmu9_7, pmu9_8, pmu9_9,
	pmu9_18, pmu12_1, uncore_2, 0
};

static PMU_MSR_INFO_NODE *ehl_msr_list[] = {
	core_1, core_2, core_3,   core_4, core_6, core_7, core_11,  pmu1_1,
	pmu1_6, pmu7_1, uncore_4, pmu9_6, pmu9_7, pmu9_8, pmu9_13,  0
};

static PMU_MSR_INFO_NODE *tgl_msr_list[] = {
	core_1,  core_2, core_3, core_4,   core_6,   core_7, core_11, pmu1_1,
	pmu1_6,  pmu7_1, pmu7_2, uncore_5, uncore_4, pmu9_6, pmu9_7,  pmu9_8,
	pmu9_13, 0
};

static PMU_MSR_INFO_NODE *lkf_msr_list[] = {
	core_1, core_2, core_3,   core_4, core_6, core_7, core_11,  pmu1_1,
	pmu1_6, pmu7_1, uncore_4, 0
};

static PMU_MSR_INFO_NODE *plat4_msr_list[] = {
	core_1, core_2, core_4, core_9,  core_11, pmu2_5,             pmu3_1,
	pmu4_4, pmu6_4, pmu8_6, pmu10_3, pmu12_4, uncore_5,           uncore_6,
	pmu9_1, pmu9_2, pmu9_3, pmu9_20, pmu13,   pmu5_msr_reg_list2, pmu9_19, 0
};

static PMU_MSR_INFO_NODE *plat5_msr_list[] = {
	core_1,  core_2,  core_3,  core_4, core_6,   core_7, core_9, core_10,
	core_11, pmu1_1,  pmu1_9,  pmu7_3, uncore_7, pmu9_6, pmu9_7, pmu9_8,
	pmu9_13, pmu9_19, 0
};

#endif

