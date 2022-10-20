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









#ifndef _PMU_LIST_H_INC_
#define _PMU_LIST_H_INC_

extern U32 drv_type;

/************************************************************/
/*
 * PMU list API for checking valid PMU list
 *
 ************************************************************/

/*!
 * @fn         DRV_BOOL PMU_LIST_Check_MSR (U32)
 *
 * @brief      Search the MSR address in the list
 *
 * @param      MSR address to be checked
 *
 * @return     TRUE if the MSR address is found in the list,
 *             FALSE otherwise
 */
extern DRV_BOOL PMU_LIST_Check_MSR(U32 msr_id);

/*!
 * @fn         DRV_BOOL PMU_LIST_Check_PCI (U8, U8, U8, U32)
 *
 * @brief      Search the PCI programming info in the list
 *
 * @param      bus_id - PCI bus id
 *             device_id - PCI device id
 *             func_id - PCI function id
 *             offset - PCI offset
 *
 * @return     TRUE if the PCI information is found in the list,
 *             FALSE otherwise
 */
extern DRV_BOOL
PMU_LIST_Check_PCI(U8 bus_id, U8 device_id, U8 func_id, U32 offset);

/*!
 * @fn         DRV_BOOL PMU_LIST_Check_MMIO (PMU_MMIO_BAR_INFO_NODE,
 *                                           PMU_MMIO_BAR_INFO_NODE,
 *                                           U32)
 *
 * @brief      Search the MMIO programming info in the list
 *
 * @param      primary - pimary MMIO BAR programming info
 *             secondary - secondary MMIO BAR programming info
 *             offset - MMIO offset
 *
 * @return     TRUE if the MMIO information is found in the list,
 *             FALSE otherwise
 */
extern DRV_BOOL
PMU_LIST_Check_MMIO(PMU_MMIO_BAR_INFO_NODE primary,
		    PMU_MMIO_BAR_INFO_NODE secondary,
		    U32                    offset);

/*!
 * @fn         OS_STATUS PMU_LIST_Initialize (S32 *)
 * @brief      Detect the CPU id and locate the applicable PMU list
 *
 * @param      index value of the allowlist. -1 if not found.
 *
 * @return     OS_SUCCESS
 */
extern OS_STATUS PMU_LIST_Initialize(S32 *idx);

/*!
 * @fn         OS_STATUS PMU_LIST_Build_MSR_List (void)
 * @brief      Build the MSR search tree
 *
 * @param      None
 *
 * @return     OS_SUCCESS
 */
extern OS_STATUS PMU_LIST_Build_MSR_List(void);

/*!
 * @fn         OS_STATUS PMU_LIST_Build_PCI_List (void)
 * @brief      Build the PCI search tree
 *
 * @param      None
 *
 * @return     OS_SUCCESS
 */
extern OS_STATUS PMU_LIST_Build_PCI_List(void);

/*!
 * @fn         OS_STATUS PMU_LIST_Build_MMIO_List (void)
 * @brief      Build the MMIO search tree
 *
 * @param      None
 *
 * @return     OS_SUCCESS
 */
extern OS_STATUS PMU_LIST_Build_MMIO_List(void);

/*!
 * @fn         OS_STATUS PMU_LIST_Add_To_MMIO_List (U64, void*, U8)
 *
 * @brief      Adds entry to the mmio list
 *
 * @param      U64   key  - key for the node in the MMIO BST
 *             void *addr - address to add into the node for additional checks
 *
 * @return     OS_SUCCESS
 */
extern OS_STATUS PMU_LIST_Add_To_MMIO_List(U64 key, void *addr);

/*!
 * @fn         OS_STATUS PMU_LIST_Clean_Up (void)
 * @brief      Clean up all the search trees
 *
 * @param      None
 *
 * @return     OS_SUCCESS
 */
extern OS_STATUS PMU_LIST_Clean_Up(void);

#endif

