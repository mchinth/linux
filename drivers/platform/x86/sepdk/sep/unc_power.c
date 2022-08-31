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









#include "lwpmudrv_defines.h"
#include "lwpmudrv_types.h"
#include "rise_errors.h"
#include "lwpmudrv_ecb.h"
#include "lwpmudrv_struct.h"

#include "inc/ecb_iterators.h"
#include "inc/control.h"
#include "inc/unc_common.h"
#include "inc/utility.h"

extern U64                    *read_counter_info;
extern U64                    *prev_counter_data;
extern EMON_BUFFER_DRIVER_HELPER emon_buffer_driver_helper;
static U64                   **prev_val_per_thread;
static U64                   **acc_per_thread;
extern DRV_CONFIG              drv_cfg;
extern U64                     max_rmid;
static U32                     local_id = 0;

/*!
 * @fn          static VOID unc_rdt_program_autormid(VOID*)
 *
 * @brief       Program RMID into PRQ_ASSOC register based on core ID in auto-rmid mode
 *
 * @param       Flag to indicate if RMID must be generated based on core ID or
 *              re-used from the previous value in PQR_ASSOC
 *
 * @return      None
 *
 * <I>Special Notes:</I>
 */
static VOID
unc_rdt_program_autormid(PVOID param)
{
	U32       this_cpu;
	U32       cur_grp;
	U64       msr_value;
	CPU_STATE pcpu;
	U64       read_val;
	U32       is_save_pqr_assoc = *((U32 *)param);

	SEP_DRV_LOG_TRACE_IN("Param: %p.", param);

	this_cpu = CONTROL_THIS_CPU();
	pcpu     = &pcb[this_cpu];
	cur_grp  = LWPMU_DEVICE_cur_group(&devices[local_id])[0];

	FOR_EACH_REG_UNC_OPERATION (pecb, local_id, idx, PMU_OPERATION_READ) {
		if (ECB_entries_reg_rw_type(pecb, idx) ==
		    PMU_REG_RW_READ_MASK_WRITE) {
			if (is_save_pqr_assoc) {
				msr_value = SYS_Read_MSR(
					(U32)ECB_entries_aux_reg_id_to_read(
						pecb, idx));
				CPU_STATE_prev_rdt_pqr_assoc(pcpu) = msr_value;

				read_val =
					((this_cpu <= max_rmid) ? this_cpu : 0);
				msr_value &=
					~ECB_entries_aux_read_mask(pecb, idx);
				msr_value |=
					read_val &
					ECB_entries_aux_read_mask(pecb, idx);
				SYS_Write_MSR(
					(U32)ECB_entries_aux_reg_id_to_read(
						pecb, idx),
					msr_value);
				SEP_DRV_LOG_TRACE(
					"cpu 0x%x wrote 0x%x with %llu",
					this_cpu,
					(U32)ECB_entries_aux_reg_id_to_read(
						pecb, idx),
					msr_value);
			} else {
				SYS_Write_MSR(
					(U32)ECB_entries_aux_reg_id_to_read(
						pecb, idx),
					CPU_STATE_prev_rdt_pqr_assoc(pcpu));
				SEP_DRV_LOG_TRACE(
					"cpu 0x%x wrote 0x%x with %llu",
					this_cpu,
					(U32)ECB_entries_aux_reg_id_to_read(
						pecb, idx),
					CPU_STATE_prev_rdt_pqr_assoc(pcpu));
			}
		}
	}
	END_FOR_EACH_REG_UNC_OPERATION;

	SEP_DRV_LOG_TRACE_OUT("");
	return;
}

/*!
 * @fn unc_power_Allocate(param)
 *
 * @param    param    device index
 *
 * @return   None     No return needed
 *
 * @brief    Allocate arrays required for reading counts
 */
static VOID
unc_power_Allocate(PVOID param)
{
	U32 id;
	U32 cur_grp;
	ECB pecb;
	U32 i;
	U32 j;

	SEP_DRV_LOG_TRACE_IN("Param: %p.", param);

	id      = *((U32 *)param);
	cur_grp = LWPMU_DEVICE_cur_group(&devices[id])[0];
	pecb    = LWPMU_DEVICE_PMU_register_data(&devices[id])[cur_grp];

	if (!pecb) {
		SEP_DRV_LOG_TRACE_OUT("Early exit (!pecb).");
		return;
	}

	acc_per_thread = CONTROL_Allocate_Memory(
		GLOBAL_STATE_num_cpus(driver_state) * sizeof(U64 *));
	if (acc_per_thread == NULL) {
		SEP_DRV_LOG_ERROR_TRACE_OUT(
			"Unable to allocate memory for acc_per_thread!");
		return;
	}

	prev_val_per_thread = CONTROL_Allocate_Memory(
		GLOBAL_STATE_num_cpus(driver_state) * sizeof(U64 *));
	if (prev_val_per_thread == NULL) {
		SEP_DRV_LOG_ERROR_TRACE_OUT(
			"Unable to allocate memory for prev_val_per_thread!");
		return;
	}

	for (i = 0; i < (U32)GLOBAL_STATE_num_cpus(driver_state); i++) {
		acc_per_thread[i] = CONTROL_Allocate_Memory(
			ECB_num_events(pecb) * sizeof(U64));
		if (acc_per_thread[i] == NULL) {
			SEP_DRV_LOG_ERROR_TRACE_OUT(
				"Unable to allocate memory for acc_per_thread[%u]!",
				i);
			return;
		}

		prev_val_per_thread[i] = CONTROL_Allocate_Memory(
			ECB_num_events(pecb) * sizeof(U64));
		if (prev_val_per_thread[i] == NULL) {
			SEP_DRV_LOG_ERROR_TRACE_OUT(
				"Unable to allocate memory for prev_val_per_thread[%u]!",
				i);
			return;
		}

		// initialize all values to 0
		for (j = 0; j < ECB_num_events(pecb); j++) {
			acc_per_thread[i][j]      = 0LL;
			prev_val_per_thread[i][j] = 0LL;
		}
	}

	if (DRV_CONFIG_rdt_auto_rmid(drv_cfg)) {
		U32 is_save_pqr_assoc = TRUE;
		local_id              = id;
		CONTROL_Invoke_Parallel(unc_rdt_program_autormid,
					(VOID *)&is_save_pqr_assoc);
	}

	SEP_DRV_LOG_TRACE_OUT("");
	return;
}

/*!
 * @fn unc_power_Free(param)
 *
 * @param    param    device index
 *
 * @return   None     No return needed
 *
 * @brief    Free arrays required for reading counts
 */
static VOID
unc_power_Free(PVOID param)
{
	U32 i;

	SEP_DRV_LOG_TRACE_IN("Param: %p.", param);

	if (acc_per_thread) {
		for (i = 0; i < (U32)GLOBAL_STATE_num_cpus(driver_state); i++) {
			acc_per_thread[i] =
				CONTROL_Free_Memory(acc_per_thread[i]);
		}
		acc_per_thread = CONTROL_Free_Memory(acc_per_thread);
	}

	if (prev_val_per_thread) {
		for (i = 0; i < (U32)GLOBAL_STATE_num_cpus(driver_state); i++) {
			prev_val_per_thread[i] =
				CONTROL_Free_Memory(prev_val_per_thread[i]);
		}
		prev_val_per_thread = CONTROL_Free_Memory(prev_val_per_thread);
	}

	if (DRV_CONFIG_rdt_auto_rmid(drv_cfg)) {
		U32 is_save_pqr_assoc = FALSE;
		CONTROL_Invoke_Parallel(unc_rdt_program_autormid,
					(VOID *)&is_save_pqr_assoc);
	}

	SEP_DRV_LOG_TRACE_OUT("");
	return;
}

/*!
 * @fn          static VOID rdt_Write_PMU(ECB pecb, U32 idx)
 *
 * @brief       Program event selection register with eventID
 *              and RMID
 *              RMID is fetched from PQR_ASSOC register.
 *
 * @param       pecb    Pointer to ECB structure
 *              idx     Event register array index in pecb
 *
 * @return      None
 *
 * <I>Special Notes:</I>
 */
static VOID
rdt_Write_PMU(ECB pecb, U32 idx)
{
	U64 msr_value        = 0;
	U64 read_val         = 0;
	U64 masked_val       = 0;
	U32 read_shift_index = 0;

	SEP_DRV_LOG_TRACE_IN("Param: %p, %u.", pecb, idx);

	msr_value = ECB_entries_reg_value(pecb, idx);
	read_val  = 1;
	while (!(ECB_entries_aux_read_mask(pecb, idx) & read_val)) {
		read_val = read_val << 1;
		read_shift_index++;
	}
	read_val = SYS_Read_MSR((U32)ECB_entries_aux_reg_id_to_read(pecb, idx));
	masked_val = (read_val & ECB_entries_aux_read_mask(pecb, idx)) >>
		     read_shift_index;
	masked_val <<= ECB_entries_aux_shift_index(pecb, idx);
	msr_value |= (U64)masked_val;
	SYS_Write_MSR(ECB_entries_reg_id(pecb, idx), msr_value);
	SEP_DRV_LOG_TRACE("Wrote 0x%x with 0x%x.",
			  ECB_entries_reg_id(pecb, idx), msr_value);

	SEP_DRV_LOG_TRACE_OUT("");
	return;
}

/*!
 * @fn          static U64 common_Read_Data_Counter(ECB pecb, U32 idx)
 *
 * @brief       Read data counter.
 *
 * @param       pecb    Pointer to ECB structure
 *              idx     Event register array index in pecb
 *
 * @return      Counter value
 *
 * <I>Special Notes:</I>
 */
static U64
common_Read_Data_Counter(ECB pecb, U32 idx)
{
	U64 offset           = 0;
	U32 read_shift_index = 0;
	U64 mask_value       = 0;
	U64 value            = 0;

	SEP_DRV_LOG_TRACE_IN("Param: %p, %u.", pecb, idx);

	if (ECB_entries_reg_rw_type(pecb, idx) == PMU_REG_RW_READ_MERGE_READ) {
		mask_value = 1;
		while (!(ECB_entries_aux_read_mask(pecb, idx) & mask_value)) {
			mask_value = mask_value << 1;
			read_shift_index++;
		}
		offset = (SYS_Read_MSR((U32)ECB_entries_aux_reg_id_to_read(
				  pecb, idx)) &
			  ECB_entries_aux_read_mask(pecb, idx)) >>
			 read_shift_index;
	}
	// The SYS_Read_MSR call below is common for RDT and Power event reads.
	// Offset is 0 except for RDT CAT Mask events.
	value = SYS_Read_MSR(ECB_entries_reg_id(pecb, idx) + (U32)offset);

	// Verify error bit (CTR[63]) and unavailable bit (CTR[62])
	// and discard data if they are set in CTR.
	if (ECB_entries_reg_rw_type(pecb, idx) ==
	    PMU_REG_RW_READ_VALIDATE_MASK) {
		if ((value >> ECB_entries_bit_position(pecb, idx)) &
		    ECB_entries_aux_read_mask(pecb, idx)) {
			value = 0;
		}
	}
	if (ECB_entries_max_bits(pecb, idx)) {
		value &= ECB_entries_max_bits(pecb, idx);
	}
	SEP_DRV_LOG_TRACE_OUT("val read: %llu: ", value);

	return value;
}

/*!
 * @fn unc_power_Trigger_Read(param, id, read_from_intr)
 *
 * @param    param          pointer to sample buffer
 * @param    id             device index
 * @param    read_from_intr Read data from interrupt or timer
 *
 * @return   None     No return needed
 *
 * @brief    Read the Uncore count data and store into the buffer param
 */
static VOID
unc_power_Trigger_Read(PVOID param, U32 id, U32 read_from_intr)
{
	U64 *data = (U64 *)param;
	U32  cur_grp;
	U32  this_cpu;
	U32  package_num;
	U32  index = 0;
	U64  diff  = 0;
	U64  value;

	SEP_DRV_LOG_TRACE_IN("Param: %p, id: %u, intr mode: %u.", param, id,
			     read_from_intr);

	this_cpu    = CONTROL_THIS_CPU();
	package_num = core_to_package_map[this_cpu];
	cur_grp     = LWPMU_DEVICE_cur_group(&devices[id])[package_num];

	FOR_EACH_REG_UNC_OPERATION (pecb, id, idx, PMU_OPERATION_READ) {
		if (ECB_entries_reg_rw_type(pecb, idx) ==
		    PMU_REG_RW_READ_MASK_WRITE) {
			rdt_Write_PMU(pecb, idx);
			continue;
		}
		// If the function is invoked from pmi, the event we are
		// reading counts must be an unc intr event.
		// If the function is invoked from timer, the event must not be
		// an interrupt read event.
		if ((read_from_intr &&
		     !ECB_entries_unc_evt_intr_read_get(pecb, idx)) ||
		    (!read_from_intr &&
		     ECB_entries_unc_evt_intr_read_get(pecb, idx))) {
			index++;
			continue;
		}
		// Write GroupID based on interrupt read event or timer event into
		// the respective groupd id offsets
		if (read_from_intr) {
			data = (U64 *)((S8 *)param +
				       ECB_group_id_offset_in_trigger_evt_desc(
					       pecb));
		} else {
			data = (U64 *)((S8 *)param + ECB_group_offset(pecb));
		}
		*data = cur_grp + 1;

		data = (U64 *)((S8 *)param +
			       ECB_entries_counter_event_offset(pecb, idx));

		value = common_Read_Data_Counter(pecb, idx);

		//check for overflow if not a static counter
		if (ECB_entries_counter_type(pecb, idx) == STATIC_COUNTER) {
			*data = value;
		} else {
			if (value < prev_val_per_thread[this_cpu][index]) {
				diff = ECB_entries_max_bits(pecb, idx) -
				       prev_val_per_thread[this_cpu][index];
				diff += value;
			} else {
				diff = value -
				       prev_val_per_thread[this_cpu][index];
			}
			acc_per_thread[this_cpu][index] += diff;
			prev_val_per_thread[this_cpu][index] = value;
			*data = acc_per_thread[this_cpu][index];
		}
		index++;
	}
	END_FOR_EACH_REG_UNC_OPERATION;

	SEP_DRV_LOG_TRACE_OUT("");
	return;
}

/* ------------------------------------------------------------------------- */
/*!
 * @fn unc_power_Enable_PMU(param)
 *
 * @param    None
 *
 * @return   None
 *
 * @brief    Capture the previous values to calculate delta later.
 */
static VOID
unc_power_Enable_PMU(PVOID param)
{
	U32       j;
	U64         *buffer = prev_counter_data;
	U32       dev_idx;
	U32       this_cpu;
	CPU_STATE pcpu;
	U32       package_event_count = 0;
	U32       thread_event_count  = 0;
	U32       module_event_count  = 0;
	U64       tmp_value           = 0;
	U32       package_id          = 0;
	U32       module_id           = 0;
	U32       core_id             = 0;
	U32       thread_id           = 0;

	SEP_DRV_LOG_TRACE_IN("Param: %p.", param);

	dev_idx    = *((U32 *)param);
	this_cpu   = CONTROL_THIS_CPU();
	pcpu       = &pcb[this_cpu];
	package_id = core_to_package_map[this_cpu];
	module_id  = core_to_module_map[this_cpu];
	core_id    = core_to_phys_core_map[this_cpu];
	thread_id  = core_to_thread_map[this_cpu];

	// NOTE THAT the enable function currently captures previous values
	// for EMON collection to avoid unnecessary memory copy.
	if (!DRV_CONFIG_emon_mode(drv_cfg)) {
		SEP_DRV_LOG_TRACE_OUT("Early exit (!emon_mode).");
		return;
	}

	FOR_EACH_REG_UNC_OPERATION (pecb, dev_idx, idx, PMU_OPERATION_READ) {
		if (ECB_entries_reg_rw_type(pecb, idx) ==
		    PMU_REG_RW_READ_MASK_WRITE) {
			rdt_Write_PMU(pecb, idx);
			continue;
		}
		if (ECB_entries_event_scope(pecb, idx) == PACKAGE_EVENT) {
			j = EMON_BUFFER_UNCORE_PACKAGE_POWER_EVENT_OFFSET(
				package_id,
				EMON_BUFFER_DRIVER_HELPER_num_entries_per_package(
					emon_buffer_driver_helper),
				EMON_BUFFER_DRIVER_HELPER_power_device_offset_in_package(
					emon_buffer_driver_helper),
				package_event_count);
			package_event_count++;
		} else if (ECB_entries_event_scope(pecb, idx) == MODULE_EVENT) {
			j = EMON_BUFFER_UNCORE_MODULE_POWER_EVENT_OFFSET(
				package_id,
				EMON_BUFFER_DRIVER_HELPER_num_entries_per_package(
					emon_buffer_driver_helper),
				EMON_BUFFER_DRIVER_HELPER_power_device_offset_in_package(
					emon_buffer_driver_helper),
				EMON_BUFFER_DRIVER_HELPER_power_num_package_events(
					emon_buffer_driver_helper),
				CPU_STATE_cpu_module_master(pcpu),
				EMON_BUFFER_DRIVER_HELPER_power_num_module_events(
					emon_buffer_driver_helper),
				module_event_count);
			module_event_count++;
		} else {
			j = EMON_BUFFER_UNCORE_THREAD_POWER_EVENT_OFFSET(
				package_id,
				EMON_BUFFER_DRIVER_HELPER_num_entries_per_package(
					emon_buffer_driver_helper),
				EMON_BUFFER_DRIVER_HELPER_power_device_offset_in_package(
					emon_buffer_driver_helper),
				EMON_BUFFER_DRIVER_HELPER_power_num_package_events(
					emon_buffer_driver_helper),
				GLOBAL_STATE_num_modules(driver_state),
				EMON_BUFFER_DRIVER_HELPER_power_num_module_events(
					emon_buffer_driver_helper),
				module_enum_supported, module_id,
				max_cores_per_module*max_threads_per_core, core_id,
				max_threads_per_core, thread_id,
				EMON_BUFFER_DRIVER_HELPER_power_num_thread_events(
					emon_buffer_driver_helper),
				thread_event_count);
			thread_event_count++;
		}

		tmp_value = SYS_Read_MSR(ECB_entries_reg_id(pecb, idx));
		if (ECB_entries_reg_rw_type(pecb, idx) ==
		    PMU_REG_RW_READ_VALIDATE_MASK) {
			if ((tmp_value >> ECB_entries_bit_position(pecb, idx)) &
			    ECB_entries_aux_read_mask(pecb, idx)) {
				tmp_value = 0;
			}
		}
		if (ECB_entries_max_bits(pecb, idx)) {
			tmp_value &= ECB_entries_max_bits(pecb, idx);
		}
		buffer[j] = tmp_value;
		SEP_DRV_LOG_TRACE("j=%u, value=%llu, cpu=%u, th_evt_ct=%u", j,
				  buffer[j], this_cpu, thread_event_count);
	}
	END_FOR_EACH_REG_UNC_OPERATION;

	SEP_DRV_LOG_TRACE_OUT("");
	return;
}

/* ------------------------------------------------------------------------- */
/*!
 * @fn unc_power_Read_PMU_Data(param)
 *
 * @param    param    The read thread node to process
 *
 * @return   None     No return needed
 *
 * @brief    Read the Uncore count data and store into the buffer param;
 *           Uncore PMU does not support sampling, i.e. ignore the id parameter.
 */
static VOID
unc_power_Read_PMU_Data(PVOID param, U32 dev_idx)
{
	U32       j;
	U64         *buffer      = (U64 *)param;
	U64         *prev_buffer = prev_counter_data;
	U32       this_cpu;
	CPU_STATE pcpu;
	U32       package_event_count = 0;
	U32       thread_event_count  = 0;
	U32       module_event_count  = 0;
	U64       tmp_value;
	U32       package_id = 0;
	U32       module_id  = 0;
	U32       core_id    = 0;
	U32       thread_id  = 0;

	SEP_DRV_LOG_TRACE_IN("Param: %p.", param);

	this_cpu   = CONTROL_THIS_CPU();
	pcpu       = &pcb[this_cpu];
	package_id = core_to_package_map[this_cpu];
	module_id  = core_to_module_map[this_cpu];
	core_id    = core_to_phys_core_map[this_cpu];
	thread_id  = core_to_thread_map[this_cpu];

	// NOTE THAT the read_pmu function on for EMON collection.
	if (!DRV_CONFIG_emon_mode(drv_cfg)) {
		SEP_DRV_LOG_TRACE_OUT("Early exit (!emon_mode).");
		return;
	}

	FOR_EACH_REG_UNC_OPERATION (pecb, dev_idx, idx, PMU_OPERATION_READ) {
		if (ECB_entries_reg_rw_type(pecb, idx) ==
		    PMU_REG_RW_READ_MASK_WRITE) {
			rdt_Write_PMU(pecb, idx);
			continue;
		}

		if (ECB_entries_event_scope(pecb, idx) == PACKAGE_EVENT) {
			j = EMON_BUFFER_UNCORE_PACKAGE_POWER_EVENT_OFFSET(
				package_id,
				EMON_BUFFER_DRIVER_HELPER_num_entries_per_package(
					emon_buffer_driver_helper),
				EMON_BUFFER_DRIVER_HELPER_power_device_offset_in_package(
					emon_buffer_driver_helper),
				package_event_count);
			package_event_count++;
		} else if (ECB_entries_event_scope(pecb, idx) == MODULE_EVENT) {
			j = EMON_BUFFER_UNCORE_MODULE_POWER_EVENT_OFFSET(
				package_id,
				EMON_BUFFER_DRIVER_HELPER_num_entries_per_package(
					emon_buffer_driver_helper),
				EMON_BUFFER_DRIVER_HELPER_power_device_offset_in_package(
					emon_buffer_driver_helper),
				EMON_BUFFER_DRIVER_HELPER_power_num_package_events(
					emon_buffer_driver_helper),
				CPU_STATE_cpu_module_master(pcpu),
				EMON_BUFFER_DRIVER_HELPER_power_num_module_events(
					emon_buffer_driver_helper),
				module_event_count);
			module_event_count++;
		} else {
			j = EMON_BUFFER_UNCORE_THREAD_POWER_EVENT_OFFSET(
				package_id,
				EMON_BUFFER_DRIVER_HELPER_num_entries_per_package(
					emon_buffer_driver_helper),
				EMON_BUFFER_DRIVER_HELPER_power_device_offset_in_package(
					emon_buffer_driver_helper),
				EMON_BUFFER_DRIVER_HELPER_power_num_package_events(
					emon_buffer_driver_helper),
				GLOBAL_STATE_num_modules(driver_state),
				EMON_BUFFER_DRIVER_HELPER_power_num_module_events(
					emon_buffer_driver_helper),
				module_enum_supported, module_id,
				max_cores_per_module*max_threads_per_core, core_id,
				max_threads_per_core, thread_id,
				EMON_BUFFER_DRIVER_HELPER_power_num_thread_events(
					emon_buffer_driver_helper),
				thread_event_count);
			thread_event_count++;
		}

		tmp_value = common_Read_Data_Counter(pecb, idx);

		if (ECB_entries_counter_type(pecb, idx) == STATIC_COUNTER) {
			buffer[j] = tmp_value;
		} else {
			if (tmp_value >= prev_buffer[j]) {
				buffer[j] = tmp_value - prev_buffer[j];
			} else {
				buffer[j] = tmp_value +
					    (ECB_entries_max_bits(pecb, idx) -
					     prev_buffer[j]);
			}
		}
		SEP_DRV_LOG_TRACE("j=%u, value=%llu, cpu=%u", j, buffer[j],
				  this_cpu);
	}
	END_FOR_EACH_REG_UNC_OPERATION;

	SEP_DRV_LOG_TRACE_OUT("");
	return;
}

/*
 * Initialize the dispatch table
 */
DISPATCH_NODE unc_power_dispatch = {
	.init = unc_power_Allocate,      // initialize
	.fini = unc_power_Free,          // destroy
	.write = UNC_COMMON_Dummy_Func,   // write
	.freeze = NULL,                    // freeze
	.restart = unc_power_Enable_PMU,    // restart
	.read_data = unc_power_Read_PMU_Data, // read
	.check_overflow = NULL,                    // check for overflow
	.swap_group = NULL,                    // swap group
	.read_lbrs = NULL,                    // read lbrs
	.cleanup = NULL,                    // cleanup
	.hw_errata = NULL,                    // hw errata
	.read_power = NULL,                    // read power
	.check_overflow_errata = NULL,                    // check overflow errata
	.read_counts = NULL,                    // read counts
	.check_overflow_gp_errata = NULL,                    // check overflow gp errata
	.read_ro = NULL,                    // read_ro
	.platform_info = NULL,                    // platform info
	.trigger_read = unc_power_Trigger_Read,  // trigger read
	.scan_for_uncore = NULL,                    // scan for uncore
	.read_metrics = NULL                     // read metrics
};

DISPATCH_NODE unc_rdt_dispatch = {
	.init = unc_power_Allocate,      // initialize
	.fini = unc_power_Free,          // destroy
	.write = NULL,                    // write
	.freeze = NULL,                    // freeze
	.restart = unc_power_Enable_PMU,    // restart
	.read_data = unc_power_Read_PMU_Data, // read
	.check_overflow = NULL,                    // check for overflow
	.swap_group = NULL,                    // swap group
	.read_lbrs = NULL,                    // read lbrs
	.cleanup = NULL,                    // cleanup
	.hw_errata = NULL,                    // hw errata
	.read_power = NULL,                    // read power
	.check_overflow_errata = NULL,                    // check overflow errata
	.read_counts = NULL,                    // read counts
	.check_overflow_gp_errata = NULL,                    // check overflow gp errata
	.read_ro = NULL,                    // read_ro
	.platform_info = NULL,                    // platform info
	.trigger_read = unc_power_Trigger_Read,  // trigger read
	.scan_for_uncore = NULL,                    // scan for uncore
	.read_metrics = NULL                     // read metrics
};

