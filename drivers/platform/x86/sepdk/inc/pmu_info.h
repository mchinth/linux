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









#ifndef _PMU_INFO_H_INC_
#define _PMU_INFO_H_INC_

U32 drv_type     = DRV_TYPE_PUBLIC;
S8 *drv_type_str = "PUBLIC";

#define MAX_PERFMON_VERSION 5

static const PMU_INFO_NODE architectural_pmu_info_list[] = {
	// No Perfmon version
	{0x6,  0x0, 0x0, 0xF, NULL,                NULL, NULL},

	// Perfmon version 1
	{ 0x6, 0x0, 0x0, 0xF, NULL,                NULL, NULL},

	// Perfmon version 2
	{ 0x6, 0x0, 0x0, 0xF, NULL,                NULL, NULL},

	// Perfmon version 3
	{ 0x6, 0x0, 0x0, 0xF, NULL,                NULL, NULL},

	// Perfmon version 4
	{ 0x6, 0x0, 0x0, 0xF, perfmon_v4_msr_list, NULL, NULL},

	// Perfmon version 5
	{ 0x6, 0x0, 0x0, 0xF, perfmon_v5_msr_list, NULL, NULL},

	// Last
	{ 0x0, 0x0, 0x0, 0x0, NULL,                NULL, NULL}
};

static const PMU_INFO_NODE pmu_info_list[] = {
	// CTI number = 108
	{0x6,  0x4F, 0x0, 0xF, bdx_msr_list,     bdx_pci_list,    skx_mmio_list          },

	// CTI number = 103
	{ 0x6, 0x5E, 0x0, 0xF, skl_msr_list,     NULL,            client_common_mmio_list},

	// CTI number = 107
	{ 0x6, 0x4E, 0x0, 0xF, skl_msr_list,     NULL,            client_common_mmio_list},

	// CTI number = 117
	{ 0x6, 0x9E, 0x0, 0x9, skl_msr_list,     NULL,            client_common_mmio_list},

	// CTI number = 116
	{ 0x6, 0x8E, 0x0, 0xB, skl_msr_list,     NULL,            client_common_mmio_list},

	// CTI number = 111
	{ 0x6, 0x55, 0x0, 0x4, skx_msr_list,     skx_pci_list,    skx_mmio_list          },

	// CTI number = 122
	{ 0x6, 0x9E, 0xA, 0xF, skl_msr_list,     NULL,            client_common_mmio_list},

	// CTI number = 127
	{ 0x6, 0x55, 0x5, 0x9, clx_msr_list,     skx_pci_list,    server_common_mmio_list},

	// CTI number = 128
	{ 0x6, 0x86, 0x0, 0xF, snr_msr_list,     snr_pci_list,    snr_mmio_info_list     },

	// CTI number = 123
	{ 0x6, 0x7E, 0x0, 0xF, icl_msr_list,     NULL,            client_common_mmio_list},

	// CTI number = 118
	{ 0x6, 0x7A, 0x0, 0xF, gml_msr_list,     NULL,            NULL                   },

	// CTI number = 113
	{ 0x6, 0x6E, 0x0, 0xF, slm_msr_list,     NULL,            NULL                   },

	// CTI number = 112
	{ 0x6, 0x5F, 0x0, 0xF, dnv_msr_list,     NULL,            bxt_dvt_mmio_info_list },

	// CTI number = 106
	{ 0x6, 0x5C, 0x0, 0xF, bxt_msr_list,     NULL,            bxt_dvt_mmio_info_list },

	// CTI number = 104
	{ 0x6, 0x56, 0x0, 0xF, bdx_msr_list,     bdw_de_pci_list, client_common_mmio_list},

	// CTI number = 105
	{ 0x6, 0x47, 0x0, 0xF, hsw_msr_list,     NULL,            client_common_mmio_list},

	// CTI number = 102
	{ 0x6, 0x57, 0x0, 0xF, knl_msr_list,     knl_pci_list,    server_common_mmio_list},

	// CTI number = 101
	{ 0x6, 0x5A, 0x0, 0xF, and_msr_list,     NULL,            NULL                   },

	// CTI number = 100
	{ 0x6, 0x4C, 0x0, 0xF, slm_msr_list,     NULL,            NULL                   },

	// CTI number = 98
	{ 0x6, 0x4A, 0x0, 0xF, slm_msr_list,     NULL,            NULL                   },

	// CTI number = 99
	{ 0x6, 0x3F, 0x0, 0xF, hsx_msr_list,     hsx_pci_list,    server_common_mmio_list},

	// CTI number = 97
	{ 0x6, 0x3D, 0x0, 0xF, hsw_msr_list,     NULL,            client_common_mmio_list},

	// CTI number = 96
	{ 0x6, 0x4D, 0x0, 0xF, avt_msr_list,     NULL,            NULL                   },

	// CTI number = 93
	{ 0x6, 0x46, 0x0, 0xF, hsw_msr_list,     NULL,            client_common_mmio_list},

	// CTI number = 94
	{ 0x6, 0x45, 0x0, 0xF, hsw_ult_msr_list, NULL,            client_common_mmio_list},

	// CTI number = 90
	{ 0x6, 0x37, 0x0, 0xF, slm_msr_list,     NULL,            NULL                   },

	// CTI number = 88
	{ 0x6, 0x3C, 0x0, 0xF, hsw_msr_list,     NULL,            client_common_mmio_list},

	// CTI number = 133
	{ 0x6, 0xA6, 0x0, 0xF, skl_msr_list,     NULL,            client_common_mmio_list},

	// CTI number = 150
	{ 0x6, 0x8E, 0xC, 0xF, skl_msr_list,     NULL,            client_common_mmio_list},

	// CTI number = 129
	{ 0x6, 0x8C, 0x0, 0xF, tgl_msr_list,     NULL,            tgl_mmio_info_list     },

	// CTI number = 142
	{ 0x6, 0x8D, 0x0, 0xF, tgl_msr_list,     NULL,            tgl_h_mmio_info_list   },

	// CTI number = 134
	{ 0x6, 0x96, 0x0, 0xF, ehl_msr_list,     NULL,            client_common_mmio_list},

	// CTI number = 135
	{ 0x6, 0x9C, 0x0, 0xF, ehl_msr_list,     NULL,            client_common_mmio_list},

	// CTI number = 140
	{ 0x6, 0xA7, 0x0, 0xF, icl_msr_list,     NULL,            client_common_mmio_list},

	// CTI number = 141
	{ 0x6, 0xA8, 0x0, 0xF, icl_msr_list,     NULL,            client_common_mmio_list},

	// CTI number = 125
	{ 0x6, 0x6A, 0x0, 0xF, icx_r_msr_list,   icx_r_pci_list,  icx_mmio_info_list     },

	// CTI number = 139
	{ 0x6, 0x58, 0x0, 0xF, skx_msr_list,     skx_pci_list,    NULL                   },

	// CTI number = 137
	{ 0x6, 0x8A, 0x0, 0xF, lkf_msr_list,     NULL,            client_common_mmio_list},

	// CTI number = 132
	{ 0x6, 0x55, 0xA, 0xF, clx_msr_list,     cpx_pci_list,    server_common_mmio_list},

	// CTI number = 136
	{ 0x6, 0xA5, 0x0, 0xF, skl_msr_list,     NULL,            client_common_mmio_list},

	// CTI number = 144
	{ 0x6, 0x6C, 0x0, 0xF, icx_r_msr_list,   icx_r_pci_list,  icx_mmio_info_list     },

	// CTI number = 120
	{ 0x6, 0x85, 0x0, 0xF, knl_msr_list,     knl_pci_list,    server_common_mmio_list},

	// CTI number = 119
	{ 0x6, 0x66, 0x0, 0xF, plat1_msr_list,   NULL,            client_common_mmio_list},

	// CTI number = 138
	{ 0x6, 0x75, 0x6, 0xF, slm_msr_list,     NULL,            NULL                   },

	// CTI number = 130
	{ 0x6, 0x9D, 0x0, 0xF, icl_msr_list,     NULL,            client_common_mmio_list},

	// CTI number = 131
	{ 0x6, 0x8F, 0x0, 0xF, plat4_msr_list,   plat4_pci_list,  plat4_mmio_info_list   },

	// CTI number = 145
	{ 0x6, 0x97, 0x0, 0xF, plat5_msr_list,   NULL,            plat5_mmio_info_list   },

	// CTI number = 148
	{ 0x6, 0x9A, 0x0, 0xF, plat5_msr_list,   NULL,            plat5_mmio_info_list   },

	// CTI number = 151
	{ 0x6, 0xB7, 0x0, 0xF, plat5_msr_list,   NULL,            plat5_mmio_info_list   },

	// CTI number = 152
	{ 0x6, 0xBA, 0x0, 0xF, plat5_msr_list,   NULL,            plat5_mmio_info_list   },

	// CTI number = 153
	{ 0x6, 0xBF, 0x0, 0xF, plat5_msr_list,   NULL,            plat5_mmio_info_list   },

	// Last
	{ 0x0, 0x0,  0x0, 0x0, NULL,             NULL,            NULL                   }
};

#endif

