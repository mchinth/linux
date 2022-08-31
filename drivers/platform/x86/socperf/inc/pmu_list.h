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

#ifndef _PMU_LIST_H_INC_
#define _PMU_LIST_H_INC_

/************************************************************/
/*
 * PMU list API for checking valid PMU list
 *
 ************************************************************/

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
 * @fn         OS_STATUS PMU_LIST_Initialize (void)
 * @brief      Detect the CPU id and locate the applicable PMU list
 *
 * @param      None 
 *
 * @return     OS_SUCCESS
 */
extern OS_STATUS PMU_LIST_Initialize(void);

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
 * @fn         OS_STATUS PMU_LIST_Clean_Up (void)
 * @brief      Clean up all the search trees
 *
 * @param      None 
 *
 * @return     OS_SUCCESS
 */
extern OS_STATUS PMU_LIST_Clean_Up(void);

#endif
