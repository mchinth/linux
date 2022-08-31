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

#include "lwpmudrv_defines.h"
#include "lwpmudrv_types.h"
#include "rise_errors.h"
#include "lwpmudrv_ecb.h"
#include "lwpmudrv_struct.h"
#include "socperfdrv.h"
#include "inc/control.h"
#include "inc/utility.h"

#include "inc/pmu_info_struct.h"
#include "inc/pmu_info_mmio.h"
#include "inc/pmu_info.h"
#include "inc/pmu_list.h"

static S32 pmu_info_index = -1;

static PMU_SEARCH_NODE *mmio_root;

static void
pmu_list_Lookup_PMU_Info(const PMU_INFO_NODE *pmu_list,
			 U32                  family,
			 U32                  model,
			 U32                  stepping)
{
	S32 i = 0;

	while (!(pmu_list[i].family == 0 && pmu_list[i].model == 0 &&
		 pmu_list[i].stepping_to == 0)) {
		if (pmu_list[i].family == family &&
		    pmu_list[i].model == model &&
		    pmu_list[i].stepping_from <= stepping &&
		    pmu_list[i].stepping_to >= stepping) {
			pmu_info_index = i;
			return;
		}
		i++;
	}
}

/****************************************************************************************
 * Common helper fuctions for search algorithm
 ****************************************************************************************/

static S32
pmu_list_Max_Height(PMU_SEARCH_NODE *node_left, PMU_SEARCH_NODE *node_right)
{
	if (node_left && node_right) {
		return (node_left->height > node_right->height) ?
			       node_left->height :
			       node_right->height;
	} else if (node_left) {
		return node_left->height;
	} else if (node_right) {
		return node_right->height;
	}
	return 0;
}

static PMU_SEARCH_NODE *
pmu_list_Right_Rotate(PMU_SEARCH_NODE *node)
{
	PMU_SEARCH_NODE *nn, *r_child_nn;

	nn         = node->left;
	r_child_nn = nn->right;

	// Rotate
	nn->right  = node;       // node becomes right child
	node->left = r_child_nn; // original right child becomes left child

	// update height
	node->height = 1 + pmu_list_Max_Height(node->left, node->right);
	nn->height   = 1 + pmu_list_Max_Height(nn->left, nn->right);

	return nn;
}

static PMU_SEARCH_NODE *
pmu_list_Left_Rotate(PMU_SEARCH_NODE *node)
{
	PMU_SEARCH_NODE *nn, *l_child_nn;

	nn         = node->right;
	l_child_nn = nn->left;

	// Rotate
	nn->left    = node;       // node becomes left child
	node->right = l_child_nn; // original left child becomes right child

	// update height
	node->height = 1 + pmu_list_Max_Height(node->left, node->right);
	nn->height   = 1 + pmu_list_Max_Height(nn->left, nn->right);

	return nn;
}

static PMU_SEARCH_NODE *
pmu_list_Create_Node(U64 key, S32 range, void *addr)
{
	PMU_SEARCH_NODE *temp = (PMU_SEARCH_NODE *)SOCPERF_Allocate_Memory(
		sizeof(PMU_SEARCH_NODE));
	temp->key    = key;
	temp->range  = (U16)range;
	temp->left   = NULL;
	temp->right  = NULL;
	temp->height = 1;
	temp->addr   = addr;
	return temp;
}

static void pmu_list_Delete_Tree(PMU_SEARCH_NODE *node)
{
	if (node == NULL) {
		return;
	}

	pmu_list_Delete_Tree(node->left);
	pmu_list_Delete_Tree(node->right);

	node->left  = NULL;
	node->right = NULL;

	SOCPERF_Free_Memory(node);
}

/****************************************************************************************
 * Range is not used: for PCI and MMIO
 ****************************************************************************************/

static PMU_SEARCH_NODE *
pmu_list_Balance_Tree(PMU_SEARCH_NODE *node, U64 key)
{
	S32 height_delta = 0;

	if (node->left && node->right) {
		height_delta = node->left->height - node->right->height;
	} else if (node->left) {
		height_delta = node->left->height;
	} else if (node->right) {
		height_delta = 0 - (node->right->height);
	}

	if (height_delta == 0) {
		// tree is balanced
		return node;
	}
	// if Delta > 1, balance left tree
	else if ((height_delta > 1) && (node->key > key)) {
		node = pmu_list_Right_Rotate(node);
	} else if ((height_delta > 1) && (node->key < key)) {
		node->left = pmu_list_Left_Rotate(node->left);
		node       = pmu_list_Right_Rotate(node);
	}
	// if Delta < -1, balance right tree
	else if ((height_delta < -1) && (node->key < key)) {
		node = pmu_list_Left_Rotate(node);
	} else if ((height_delta < -1) && (node->key > key)) {
		node->right = pmu_list_Right_Rotate(node->right);
		node        = pmu_list_Left_Rotate(node);
	}
	return node;
}

static PMU_SEARCH_NODE *
pmu_list_Insert_Node(PMU_SEARCH_NODE *node, U64 key, void *addr)
{
	if (node == NULL) {
		// make it root, range = 0
		node = pmu_list_Create_Node(key, 0, addr);
	} else if (node->key < key) {
		// insert to right sub tree
		node->right = pmu_list_Insert_Node(node->right, key, addr);
	} else if (node->key > key) {
		// insert to left sub tree
		node->left = pmu_list_Insert_Node(node->left, key, addr);
	} else {
		// do nothing
		return node;
	}

	// update height
	node->height = 1 + pmu_list_Max_Height(node->left, node->right);

	// Balance the tree
	return pmu_list_Balance_Tree(node, key);
}

static PMU_SEARCH_NODE *
pmu_list_binary_search(PMU_SEARCH_NODE *node, U64 key)
{
	if (node == NULL) {
		return NULL;
	}
	if (node->key == key) {
		return node;
	} else if (node->key < key) {
		return pmu_list_binary_search(node->right, key);
	} else {
		return pmu_list_binary_search(node->left, key);
	}
}

extern DRV_BOOL
PMU_LIST_Check_MMIO(PMU_MMIO_BAR_INFO_NODE primary,
		    PMU_MMIO_BAR_INFO_NODE secondary,
		    U32                    offset)
{
	PMU_SEARCH_NODE         *temp;
	U64                      key;
	PMU_MMIO_UNIT_INFO_NODE *unit_info = NULL;
	DRV_BOOL                 ret       = FALSE;

	SOCPERF_PRINT_DEBUG("");

	if (pmu_info_index == -1 || mmio_root == NULL) {
		SOCPERF_PRINT_DEBUG("Success");
		return FALSE;
	}

	if (primary.bar_prog_type == MMIO_SINGLE_BAR_TYPE ||
	    primary.bar_prog_type == MMIO_DUAL_BAR_TYPE) {
		key = (U64)primary.u.reg << 32 | offset;
	} else if (primary.bar_prog_type == MMIO_DIRECT_BAR_TYPE) {
		key = (U64)primary.mask << 32 | offset;
	} else {
		SOCPERF_PRINT_ERROR("Invalid BAR prog type %d",
				    primary.bar_prog_type);
		SOCPERF_PRINT_DEBUG("Success");
		return FALSE;
	}

	temp = pmu_list_binary_search(mmio_root, key);
	if (temp != NULL) {
		if (primary.bar_prog_type == MMIO_DIRECT_BAR_TYPE) {
			ret = TRUE;
		} else if (primary.bar_prog_type == MMIO_SINGLE_BAR_TYPE) {
			unit_info = (PMU_MMIO_UNIT_INFO_NODE *)temp->addr;
			if (unit_info &&
			    (unit_info->primary.mask == primary.mask) &&
			    (unit_info->primary.shift == primary.shift)) {
				ret = TRUE;
			}
		} else if (primary.bar_prog_type == MMIO_DUAL_BAR_TYPE) {
			unit_info = (PMU_MMIO_UNIT_INFO_NODE *)temp->addr;
			if (unit_info &&
			    (unit_info->primary.mask == primary.mask) &&
			    (unit_info->primary.shift == primary.shift) &&
			    (unit_info->secondary.u.s.offset ==
			     secondary.u.s.offset) &&
			    (unit_info->secondary.mask == secondary.mask) &&
			    (unit_info->secondary.shift == secondary.shift)) {
				ret = TRUE;
			}
		}
	}

	SOCPERF_PRINT_DEBUG("Success");
	return ret;
}

extern OS_STATUS PMU_LIST_Initialize(void)
{
	U64 rax, rbx, rcx, rdx;
	U32 family, model, stepping;

	SOCPERF_PRINT_DEBUG("");

	SOCPERF_UTILITY_Read_Cpuid(0x1, &rax, &rbx, &rcx, &rdx);

	family = (U32)(rax >> 8 & 0x0f);
	model  = (U32)(rax >> 12 & 0xf0); /* extended model bits */
	model |= (U32)(rax >> 4 & 0x0f);
	stepping = (U32)(rax & 0x0f);

	pmu_info_index = -1;
	pmu_list_Lookup_PMU_Info(pmu_info_list, family, model, stepping);

	SOCPERF_PRINT("PMU check enabled! F%x.M%x.S%x index=%d\n", family,
		      model, stepping, pmu_info_index);

	SOCPERF_PRINT_DEBUG("Success");
	return OS_SUCCESS;
}

extern OS_STATUS PMU_LIST_Build_MMIO_List(void)
{
	U32                      unit_idx = 0;
	U32                      reg_idx  = 0;
	U64                      key;
	PMU_MMIO_UNIT_INFO_NODE *unit_info_list;

	SOCPERF_PRINT_DEBUG("");

	if (pmu_info_index == -1) {
		SOCPERF_PRINT("No MMIO list information detected!\n");
		SOCPERF_PRINT_DEBUG("Success");
		return OS_SUCCESS;
	}

	unit_info_list = pmu_info_list[pmu_info_index].mmio_info_list;

	if (!unit_info_list) {
		SOCPERF_PRINT("No MMIO list information detected!\n");
		return OS_SUCCESS;
	}

	SOCPERF_DRV_MEMSET(&key, 0, sizeof(U64));

	while (unit_info_list[unit_idx]
		       .reg_offset_list) { //Iterate through unit list
		reg_idx = 0;
		while (unit_info_list[unit_idx].reg_offset_list[reg_idx] !=
		       0x0) { //Iterate through offset list
			switch (unit_info_list[unit_idx].primary.bar_prog_type) {
			case MMIO_SINGLE_BAR_TYPE:
			case MMIO_DUAL_BAR_TYPE:
				key = (U64)unit_info_list[unit_idx].primary.u.reg
					      << 32 |
				      (U64)unit_info_list[unit_idx]
					      .reg_offset_list[reg_idx];
				mmio_root = pmu_list_Insert_Node(
					mmio_root, key,
					(void *)(&unit_info_list[unit_idx]));
				break;
			case MMIO_DIRECT_BAR_TYPE:
				key = (U64)unit_info_list[unit_idx].primary.mask
					      << 32 |
				      (U64)unit_info_list[unit_idx]
					      .reg_offset_list[reg_idx];
				mmio_root = pmu_list_Insert_Node(
					mmio_root, key,
					(void *)(&unit_info_list[unit_idx]));
				break;
			}
			reg_idx++;
		}
		unit_idx++;
	}

	SOCPERF_PRINT_DEBUG("Success");
	return OS_SUCCESS;
}

extern OS_STATUS PMU_LIST_Clean_Up(void)
{
	SOCPERF_PRINT_DEBUG("");

	pmu_info_index = -1;

	if (mmio_root) {
		pmu_list_Delete_Tree(mmio_root);
		mmio_root = NULL;
	}

	SOCPERF_PRINT_DEBUG("Success");
	return OS_SUCCESS;
}
