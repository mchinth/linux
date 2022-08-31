/****
    Copyright (C) 2020 Intel Corporation.  All Rights Reserved.

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









#include "lwpmudrv_defines.h"
#include "lwpmudrv_types.h"
#include "rise_errors.h"
#include "lwpmudrv_ecb.h"
#include "lwpmudrv_struct.h"

#include "inc/ecb_iterators.h"
#include "inc/control.h"
#include "inc/unc_common.h"
#include "inc/utility.h"
#include "inc/pci.h"

extern U64                      *read_counter_info;
extern U64                      *prev_counter_data;
extern DRV_CONFIG                drv_cfg;
extern EMON_BUFFER_DRIVER_HELPER emon_buffer_driver_helper;

extern DRV_PMT_TELEM_DEV_NODE pmt_devices[MAX_PMT_DEVICES];

#define MASK_32BIT 0xffffffff
#define MASK_64BIT 0xffffffff00000000ULL

#define IS_MASTER(device_type, cpu)                   \
	(((device_type) == DRV_SINGLE_INSTANCE) ?     \
		 CPU_STATE_system_master(&pcb[cpu]) : \
		 CPU_STATE_socket_master(&pcb[(cpu)]))
#define GET_PACKAGE_NUM(device_type, cpu) \
	(((device_type) == DRV_SINGLE_INSTANCE) ? 0 : core_to_package_map[cpu])
#define IS_64BIT(mask) (((mask) >> 32) != 0)

#define EVENT_COUNTER_MAX_TRY 30

/*!
 * @fn          static VOID unc_pmt_Write_PMU(VOID*)
 *
 * @brief       Initial write of PMU registers
 *              Walk through the enties and write the value of the register accordingly.
 *              When current_group = 0, then this is the first time this routine is called,
 *
 * @param       None
 *
 * @return      None
 *
 * <I>Special Notes:</I>
 */
static VOID
unc_pmt_Write_PMU(VOID *param)
{
	U32            dev_idx      = 0;
	U32            offset_delta = 0;
	U32            event_id     = 0;
	U32            this_cpu     = 0;
	U32            package_num  = 0;
	U32            cur_grp      = 0;
	U32            entry        = 0;
	U32            dev_node     = 0;
	U64            tmp_value    = 0;
	ECB            pecb_entry;
	DEV_UNC_CONFIG pcfg_unc;
	U32            pmt_index = 0;

	SEP_DRV_LOG_TRACE_IN("Param: %p.", param);

	dev_idx  = *((U32 *)param);
	this_cpu = CONTROL_THIS_CPU();
	pcfg_unc = (DEV_UNC_CONFIG)LWPMU_DEVICE_pcfg(&devices[dev_idx]);
	if (!IS_MASTER(DEV_UNC_CONFIG_device_type(pcfg_unc), this_cpu)) {
		SEP_DRV_LOG_TRACE_OUT("Early exit (!is_master).");
		return;
	}
	package_num = GET_PACKAGE_NUM(DEV_UNC_CONFIG_device_type(pcfg_unc),
					this_cpu);
	cur_grp     = LWPMU_DEVICE_cur_group(&devices[(dev_idx)])[package_num];
	pecb_entry  =
		LWPMU_DEVICE_PMU_register_data(&devices[(dev_idx)])[(cur_grp)];
	if (!pecb_entry) {
		SEP_DRV_LOG_TRACE_OUT("Early exit (!pecb).");
		return;
	}

	dev_node = ECB_dev_node(pecb_entry);
	entry    = package_num;

	if (DRV_CONFIG_emon_mode(drv_cfg)) {
		SEP_DRV_LOG_TRACE_OUT("Early exit (!event_based_counts).");
		return;
	}

	FOR_EACH_REG_UNC_OPERATION (pecb, dev_idx, idx, PMU_OPERATION_READ) {
		offset_delta = ECB_entries_reg_offset(pecb, idx);
		SEP_DRV_LOG_TRACE("od=0x%x", offset_delta);

		// this is needed for overflow detection of the accumulators.
		pmt_index = ECB_entries_unit_id(pecb, idx);
		if (pmt_index >= MAX_PMT_DEVICES ||
		    !pmt_devices[pmt_index].telem_ep) {
			continue;
		}
		tmp_value = PCI_PMT_Read_U64(pmt_devices[pmt_index].telem_ep,
					     offset_delta);
		tmp_value = tmp_value >> ECB_entries_aux_shift_index(pecb, idx);
		tmp_value &= (U64)ECB_entries_max_bits(pecb, idx);
		LWPMU_DEVICE_prev_value(
			&devices[dev_idx])[package_num][event_id] = tmp_value;
		SEP_DRV_LOG_TRACE("cpu=%u, device=%u, package=%u, entry=%u, event_id=%u, value=0x%llu",
				  this_cpu, dev_idx, package_num, entry,
				  event_id, tmp_value);
		event_id++;

		if (LWPMU_DEVICE_counter_mask(&devices[dev_idx]) == 0) {
			LWPMU_DEVICE_counter_mask(&devices[dev_idx]) =
				(U64)ECB_entries_max_bits(pecb, idx);
		}
	}
	END_FOR_EACH_REG_UNC_OPERATION;

	SEP_DRV_LOG_TRACE_OUT("");
	return;
}

/*!
 * @fn         static VOID unc_pmt_Enable_PMU(PVOID)
 *
 * @brief      Capture the previous values to calculate delta later.
 *
 * @param      None
 *
 * @return     None
 *
 * <I>Special Notes:</I>
 */
static void
unc_pmt_Enable_PMU(PVOID param)
{
	U32            j            = 0;
	U32            this_cpu     = 0;
	U32            dev_idx      = 0;
	U32            package_num  = 0;
	U32            offset_delta = 0;
	U32            cur_grp      = 0;
	U32            num_events   = 0;
	U32            dev_node     = 0;
	U64         *buffer         = prev_counter_data;
	ECB            pecb_entry;
	DEV_UNC_CONFIG pcfg_unc;
	U32            pmt_index = 0;

	SEP_DRV_LOG_TRACE_IN("Param: %p.", param);

	dev_idx  = *((U32 *)param);
	this_cpu = CONTROL_THIS_CPU();
	pcfg_unc = (DEV_UNC_CONFIG)LWPMU_DEVICE_pcfg(&devices[dev_idx]);
	if (!IS_MASTER(DEV_UNC_CONFIG_device_type(pcfg_unc), this_cpu)) {
		SEP_DRV_LOG_TRACE_OUT("Early exit (!IS_MASTER).");
		return;
	}

	package_num = GET_PACKAGE_NUM(DEV_UNC_CONFIG_device_type(pcfg_unc),
					this_cpu);
	cur_grp     = LWPMU_DEVICE_cur_group(&devices[(dev_idx)])[package_num];
	pecb_entry  =
		LWPMU_DEVICE_PMU_register_data(&devices[(dev_idx)])[(cur_grp)];
	if (!pecb_entry) {
		SEP_DRV_LOG_TRACE_OUT("Early exit (!pecb) for group %u.",
				      cur_grp);
		return;
	}
	SEP_DRV_LOG_TRACE("enable PMU for group %u", cur_grp);
	dev_node = ECB_dev_node(pecb_entry);

	// NOTE THAT the enable function currently captures previous values
	// for EMON collection to avoid unnecessary memory copy.
	// capturing previous values enable freerunning counter delta computation
	if (DRV_CONFIG_emon_mode(drv_cfg)) {
		num_events = ECB_num_events(pecb_entry);
		FOR_EACH_REG_UNC_OPERATION (pecb, dev_idx, idx,
					    PMU_OPERATION_READ) {
			offset_delta = ECB_entries_reg_offset(pecb, idx);

			if ((ECB_entries_event_scope(pecb, idx) ==
			     PACKAGE_EVENT) ||
			    (ECB_entries_event_scope(pecb, idx) ==
			     SYSTEM_EVENT)) {
				if (ECB_entries_event_scope(pecb, idx) ==
				    SYSTEM_EVENT) {
					j = ECB_entries_uncore_buffer_offset_in_system(
						pecb, idx);
				} else {
					j = EMON_BUFFER_UNCORE_PACKAGE_EVENT_OFFSET(
						package_num,
						EMON_BUFFER_DRIVER_HELPER_num_entries_per_package(
							emon_buffer_driver_helper),
						ECB_entries_uncore_buffer_offset_in_package(
							pecb, idx));
				}

				pmt_index = ECB_entries_unit_id(pecb, idx);
				if (pmt_index >= MAX_PMT_DEVICES ||
				    !pmt_devices[pmt_index].telem_ep) {
					continue;
				}
				buffer[j] = PCI_PMT_Read_U64(
					pmt_devices[pmt_index].telem_ep,
					offset_delta);
				buffer[j] =
					buffer[j] >>
					ECB_entries_aux_shift_index(pecb, idx);
				buffer[j] &=
					(U64)ECB_entries_max_bits(pecb, idx);
			}
		}
		END_FOR_EACH_REG_UNC_OPERATION;
	}

	SEP_DRV_LOG_TRACE_OUT("");
	return;
}

/* ------------------------------------------------------------------------- */
/*!
 * @fn       void unc_pmt_Trigger_Read(id)
 *
 * @param    id       Device index
 *
 * @return   None     No return needed
 *
 * @brief    Read the Uncore data from counters and store into buffer
 */
static VOID
unc_pmt_Trigger_Read(PVOID param, U32 id, U32 read_from_intr)
{
	U32            this_cpu     = 0;
	U32            cur_grp      = 0;
	U32            index        = 0;
	U32            offset_delta = 0;
	U32            package_num  = 0;
	U32            entry        = 0;
	U32            dev_node     = 0;
	U64            diff         = 0;
	U64            value        = 0ULL;
	U64           *data;
	ECB            pecb_entry;
	DEV_UNC_CONFIG pcfg_unc;
	U32            pmt_index = 0;

	SEP_DRV_LOG_TRACE_IN("Param: %p, id: %u.", param, id);

	this_cpu = CONTROL_THIS_CPU();
	pcfg_unc = (DEV_UNC_CONFIG)LWPMU_DEVICE_pcfg(&devices[id]);
#if !defined(DRV_OS_MAC)
	if (!IS_MASTER(DEV_UNC_CONFIG_device_type(pcfg_unc), this_cpu)) {
		SEP_DRV_LOG_TRACE_OUT("Early exit (!IS_MASTER).");
		return;
	}
#endif

	package_num = GET_PACKAGE_NUM(DEV_UNC_CONFIG_device_type(pcfg_unc),
					this_cpu);
	cur_grp     = LWPMU_DEVICE_cur_group(&devices[id])[package_num];
	pecb_entry  = LWPMU_DEVICE_PMU_register_data(&devices[id])[(cur_grp)];
	if (!pecb_entry) {
		SEP_DRV_LOG_TRACE_OUT("Early exit (!pecb).");
		return;
	}

	dev_node = ECB_dev_node(pecb_entry);
	entry    = package_num;

	// Write GroupID
	data  = (U64 *)((S8 *)param + ECB_group_offset(pecb_entry));
	*data = cur_grp + 1;
	//Read in the counts into temporary buffer
	FOR_EACH_REG_UNC_OPERATION (pecb, id, idx, PMU_OPERATION_READ) {
		offset_delta = ECB_entries_reg_offset(pecb, idx);
		pmt_index    = ECB_entries_unit_id(pecb, idx);
		if (pmt_index >= MAX_PMT_DEVICES ||
		    !pmt_devices[pmt_index].telem_ep) {
			continue;
		}
		value = PCI_PMT_Read_U64(pmt_devices[pmt_index].telem_ep,
					 offset_delta);
		value = value >> ECB_entries_aux_shift_index(pecb, idx);
		value &= (U64)ECB_entries_max_bits(pecb, idx);

		data = (U64 *)((S8 *)param +
			       ECB_entries_counter_event_offset(pecb, idx));
		//check for overflow if not a static counter
		if (ECB_entries_counter_type(pecb, idx) == STATIC_COUNTER) {
			*data = value;
		} else {
			if (value < LWPMU_DEVICE_prev_value(
					    &devices[id])[package_num][index]) {
				diff = LWPMU_DEVICE_counter_mask(&devices[id]) -
				       LWPMU_DEVICE_prev_value(
					       &devices[id])[package_num][index];
				diff += value;
			} else {
				diff = value -
				       LWPMU_DEVICE_prev_value(
					       &devices[id])[package_num][index];
			}
			LWPMU_DEVICE_acc_value(
				&devices[id])[package_num][cur_grp][index] +=
				diff;
			LWPMU_DEVICE_prev_value(
				&devices[id])[package_num][index] = value;
			*data = LWPMU_DEVICE_acc_value(
				&devices[id])[package_num][cur_grp][index];
		}
		index++;
	}
	END_FOR_EACH_REG_UNC_OPERATION;

	SEP_DRV_LOG_TRACE_OUT("");
	return;
}

/* ------------------------------------------------------------------------- */
/*!
 * @fn unc_pmt_Read_PMU_Data(param)
 *
 * @param    param    dummy parameter which is not used
 *
 * @return   None     No return needed
 *
 * @brief    Read all the data MSR's into a buffer.  Called by the interrupt handler.
 *
 */
static VOID
unc_pmt_Read_PMU_Data(PVOID param, U32 dev_idx)
{
	U32            j            = 0;
	U32            offset_delta = 0;
	U32            cur_grp      = 0;
	U32            num_events   = 0;
	U32            package_num  = 0;
	U32            entry        = 0;
	U32            dev_node     = 0;
	U32            num_pkgs     = num_packages;
	U32            this_cpu     = 0;
	U64            tmp_value    = 0ULL;
	U64           *buffer       = (U64 *)param;
	U64           *prev_buffer  = prev_counter_data;
	ECB            pecb_entry;
	DEV_UNC_CONFIG pcfg_unc;
	U32            pmt_index = 0;

	SEP_DRV_LOG_TRACE_IN("Param: %p.", param);

	this_cpu = CONTROL_THIS_CPU();
	pcfg_unc = (DEV_UNC_CONFIG)LWPMU_DEVICE_pcfg(&devices[dev_idx]);
	if (!IS_MASTER(DEV_UNC_CONFIG_device_type(pcfg_unc), this_cpu)) {
		SEP_DRV_LOG_TRACE_OUT("Early exit (!IS_MASTER).");
		return;
	}

	package_num = GET_PACKAGE_NUM(DEV_UNC_CONFIG_device_type(pcfg_unc),
					this_cpu);
	cur_grp     = LWPMU_DEVICE_cur_group(&devices[(dev_idx)])[package_num];
	pecb_entry  =
		LWPMU_DEVICE_PMU_register_data(&devices[(dev_idx)])[(cur_grp)];
	if (!pecb_entry) {
		SEP_DRV_LOG_TRACE_OUT("Early exit (!pecb).");
		return;
	}

	dev_node = ECB_dev_node(pecb_entry);
	entry    = package_num;

	if (DEV_UNC_CONFIG_device_type(pcfg_unc) == DRV_SINGLE_INSTANCE) {
		num_pkgs = 1;
	}

	num_events = ECB_num_events(pecb_entry);
	FOR_EACH_REG_UNC_OPERATION (pecb, dev_idx, idx, PMU_OPERATION_READ) {
		offset_delta = ECB_entries_reg_offset(pecb, idx);
		if ((ECB_entries_event_scope(pecb, idx) == PACKAGE_EVENT) ||
		    (ECB_entries_event_scope(pecb, idx) == SYSTEM_EVENT)) {
			if (ECB_entries_event_scope(pecb, idx) ==
			    SYSTEM_EVENT) {
				j = ECB_entries_uncore_buffer_offset_in_system(
					pecb, idx);
			} else {
				j = EMON_BUFFER_UNCORE_PACKAGE_EVENT_OFFSET(
					package_num,
					EMON_BUFFER_DRIVER_HELPER_num_entries_per_package(
						emon_buffer_driver_helper),
					ECB_entries_uncore_buffer_offset_in_package(
						pecb, idx));
			}

			pmt_index = ECB_entries_unit_id(pecb, idx);
			if (pmt_index >= MAX_PMT_DEVICES ||
			    !pmt_devices[pmt_index].telem_ep) {
				continue;
			}
			tmp_value = PCI_PMT_Read_U64(
				pmt_devices[pmt_index].telem_ep, offset_delta);
			tmp_value = tmp_value >>
				    ECB_entries_aux_shift_index(pecb, idx);
			tmp_value &= (U64)ECB_entries_max_bits(pecb, idx);
			if (ECB_entries_counter_type(pecb, idx) ==
			    STATIC_COUNTER) {
				buffer[j] = tmp_value;
			} else {
				if (tmp_value >= prev_buffer[j]) {
					buffer[j] = tmp_value - prev_buffer[j];
				} else {
					buffer[j] = tmp_value +
						    (ECB_entries_max_bits(pecb,
									  idx) -
						     prev_buffer[j]);
				}
			}
			SEP_DRV_LOG_TRACE("j=%u, v=%llu", j, buffer[j]);
		}
	}
	END_FOR_EACH_REG_UNC_OPERATION;

	SEP_DRV_LOG_TRACE_OUT("");
	return;
}

/*
 * Initialize the dispatch table
 */
DISPATCH_NODE unc_pmt_dispatch = {
	NULL,                  // initialize
	NULL,                  // destroy
	unc_pmt_Write_PMU,     // write
	NULL,                  // freeze
	unc_pmt_Enable_PMU,    // restart
	unc_pmt_Read_PMU_Data, // read
	NULL,                  // check for overflow
	NULL,                  // swap group
	NULL,                  // read lbrs
	UNC_COMMON_Dummy_Func, // cleanup
	NULL,                  // hw errata
	NULL,                  // read power
	NULL,                  // check overflow errata
	NULL,                  // read counts
	NULL,                  // check overflow gp errata
	NULL,                  // read_ro
	NULL,                  // platform info
	unc_pmt_Trigger_Read,  // trigger read
	NULL,                  // scan for uncore
	NULL                   // read metrics
};

