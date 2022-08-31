/****
    Copyright (C) 2012 Intel Corporation.  All Rights Reserved.

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









#ifndef _IPT_H_
#define _IPT_H_

#define IPT_BUF_NUM_DEFAULT 16
#define IPT_BUF_NUM_SMALL   8
#define IPT_BUF_NUM_LARGE   31
#define IPT_BUF_NUM_MAX     IPT_BUF_NUM_LARGE
#define IPT_BUF_SIZE        0x1000 // 4096 = 4K
#define IPT_BUF_SIZE_MASK   0xFFFF // up to 16 * 4096

typedef struct IPT_NODE_S IPT_NODE;

struct IPT_NODE_S {
	PVOID topa_virt_address;
	U64   topa_phys_address;
	PVOID outbuf_virt_address;
	U64   outbuf_phys_address[IPT_BUF_NUM_MAX];
};

#define IPT_NODE_topa_virt_address(x)   ((x).topa_virt_address)
#define IPT_NODE_topa_phys_address(x)   ((x).topa_phys_address)
#define IPT_NODE_outbuf_virt_address(x) ((x).outbuf_virt_address)
#define IPT_NODE_outbuf_phys_address(x) ((x).outbuf_phys_address)

extern OS_STATUS IPT_Initialize(DRV_CONFIG cfg);

extern VOID IPT_Destroy(DRV_CONFIG cfg);

extern VOID IPT_Start(PVOID param);

extern U64 IPT_TOPA_Interrupted(S32 this_cpu, U64 overflow_status);

extern VOID IPT_TOPA_Flush(PVOID param);

/*
 *  Dispatch table for virtualized functions.
 *  Used to enable common functionality for different
 *  processor microarchitectures
 */
typedef struct IPT_DISPATCH_NODE_S IPT_DISPATCH_NODE;
typedef IPT_DISPATCH_NODE         *IPT_DISPATCH;
struct IPT_DISPATCH_NODE_S {
	VOID (*init)(PVOID);
	VOID (*fini)(PVOID);
	VOID (*enable)(S32);
	VOID (*disable)(S32);
	U64 (*flush)(S32);
};

#endif

