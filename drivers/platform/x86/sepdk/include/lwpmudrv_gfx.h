/*
 * Copyright (C) 2011 Intel Corporation
 *
 * This software and the related documents are Intel copyrighted materials, and your use of them
 * is governed by the express license under which they were provided to you ("License"). Unless
 * the License provides otherwise, you may not use, modify, copy, publish, distribute, disclose
 * or transmit this software or the related documents without Intel's prior written permission.
 *
 * This software and the related documents are provided as is, with no express or implied
 * warranties, other than those that are expressly stated in the License.
*/

#ifndef _LWPMUDRV_GFX_H_
#define _LWPMUDRV_GFX_H_

#if defined(__cplusplus)
extern "C" {
#endif

#define GFX_BASE_ADDRESS    0xFF200000
#define GFX_BASE_NEW_OFFSET 0x00080000
#define GFX_PERF_REG        0x040 // location of GFX counter relative to base
#define GFX_NUM_COUNTERS    9 // max number of GFX counters per counter group
#define GFX_CTR_OVF_VAL     0xFFFFFFFF // overflow value for GFX counters

#define GFX_REG_CTR_CTRL    0x01FF
#define GFX_CTRL_DISABLE    0x1E00

//#define GFX_COMPUTE_DELTAS 1  // use event count deltas instead of raw counts

#if defined(__cplusplus)
}
#endif

#endif
