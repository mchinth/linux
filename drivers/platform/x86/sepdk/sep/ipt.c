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









#include "inc/master.h"

#define ipt_node_on_cpu(cpu) CPU_STATE_ipt_node(&pcb[cpu])
#define ipt_node_mine        CPU_STATE_ipt_node(&pcb[this_cpu])

extern IPT_CONFIG   ipt_config;
static IPT_DISPATCH ipt_dispatch         = NULL;
static U64          ipt_enable_value     = 0ULL;
static U32          ipt_buffer_num       = 0;
static U64          cr3match_value       = 0ULL;
static U64          ipt_buffer_size_mask = 0ULL;

/* ------------------------------------------------------------------------- */
/*!
 * @fn          VOID ipt_Initialize (PVOID)
 *
 * @brief       Initialize IPT
 *
 * @param       param  - current cpu id
 *
 * @return      NONE
 *
 * <I>Special Notes:</I>
 */
static VOID
ipt_Initialize(PVOID param)
{
	S32 i;
	U64 value;

	SEP_DRV_LOG_TRACE_IN("");

	for (i = 0; i < GLOBAL_STATE_num_cpus(driver_state); i++) {
		CPU_STATE_ipt_collected_count(&pcb[i]) = 0;
		CPU_STATE_ipt_dropped_count(&pcb[i])   = 0;
		CPU_STATE_ipt_dropped_packets(&pcb[i]) = 0;
		CPU_STATE_ipt_data_offset(&pcb[i])     = 0;
	}

	SEP_DRV_LOG_TRACE("Reset IPT MSRs to clear exiting settings");
	value = SYS_Read_MSR(IA32_RTIT_CTL);
	SYS_Write_MSR(IA32_RTIT_CTL, value & ~IA32_RTIT_CTL_TRACEEN);
	SYS_Write_MSR(IA32_RTIT_CTL, 0ULL);
	SYS_Write_MSR(IA32_RTIT_STATUS, 0ULL);
	SYS_Write_MSR(IA32_RTIT_OUTPUT_BASE, 0ULL);
	SYS_Write_MSR(IA32_RTIT_OUTPUT_MASK_PTRS, 0ULL);

	SEP_DRV_LOG_TRACE("VMX on IPT supported: %llu",
			  (SYS_Read_MSR(IA32_VMX_MISC) >> 14) & 0x1ULL);

	SEP_DRV_LOG_TRACE_OUT("");
	return;
}

/* ------------------------------------------------------------------------- */
/*!
 * @fn          VOID ipt_Finalize (S32)
 *
 * @brief       Finalize IPT
 *
 * @param       param  - current cpu id
 *
 * @return      NONE
 *
 * <I>Special Notes:</I>
 */
static VOID
ipt_Finalize(PVOID param)
{
	U64 value;

	SEP_DRV_LOG_TRACE_IN("");

	SEP_DRV_LOG_TRACE("Reset IPT MSRs to clear exiting settings");
	value = SYS_Read_MSR(IA32_RTIT_CTL);
	SYS_Write_MSR(IA32_RTIT_CTL, value & ~IA32_RTIT_CTL_TRACEEN);
	SYS_Write_MSR(IA32_RTIT_CTL, 0ULL);
	SYS_Write_MSR(IA32_RTIT_STATUS, 0ULL);
	SYS_Write_MSR(IA32_RTIT_OUTPUT_BASE, 0ULL);
	SYS_Write_MSR(IA32_RTIT_OUTPUT_MASK_PTRS, 0ULL);

	SEP_DRV_LOG_TRACE_OUT("");
	return;
}

/* ------------------------------------------------------------------------- */
/*!
 * @fn          VOID ipt_Enable (S32)
 *
 * @brief       Enable IPT
 *
 * @param       this_cpu   current cpu id
 *
 * @return      NONE
 *
 * <I>Special Notes:</I>
 */
static VOID
ipt_Enable(S32 this_cpu)
{
	U64 value, collection = 0ULL;

	SEP_DRV_LOG_TRACE_IN("");

	if (ipt_node_mine.topa_virt_address) {
		value = SYS_Read_MSR(IA32_RTIT_CTL);
		SYS_Write_MSR(IA32_RTIT_CTL, value & ~IA32_RTIT_CTL_TRACEEN);

		// Disable LBRs and BTS because IPT can't be enabled with LBR on hardware
		SYS_Write_MSR(IA32_DEBUG_CTRL, 0ULL);

		// ToPA interrupt threshold
		((U64 *)ipt_node_mine
			 .topa_virt_address)[ipt_buffer_num / 4 * 3] |= 0x04;
		// ToPA stop buffering
		((U64 *)ipt_node_mine.topa_virt_address)[ipt_buffer_num - 1] |=
			0x10;
		// End of buffer
		((U64 *)ipt_node_mine.topa_virt_address)[ipt_buffer_num] =
			ipt_node_mine.topa_phys_address | 0x1;

		SYS_Write_MSR(IA32_RTIT_STATUS, 0ULL);
		SYS_Write_MSR(IA32_RTIT_OUTPUT_BASE,
			      ipt_node_mine.topa_phys_address);
		SYS_Write_MSR(IA32_RTIT_OUTPUT_MASK_PTRS, 0x7F);

		if (cr3match_value) {
			SYS_Write_MSR(IA32_RTIT_CR3_MATCH, cr3match_value);
		}
		collection = IA32_RTIT_CTL_TOPA | IA32_RTIT_CTL_TSCEN |
			     ipt_enable_value;
		SYS_Write_MSR(IA32_RTIT_CTL, collection);
		SEP_DRV_LOG_TRACE("IA32_RTIT_STATUS = 0x%llx",
				  SYS_Read_MSR(IA32_RTIT_STATUS));
		SYS_Write_MSR(IA32_RTIT_CTL,
			      collection | IA32_RTIT_CTL_TRACEEN);

		SEP_DRV_LOG_TRACE("collection = %llx", collection);
	} else {
		SEP_DRV_LOG_ERROR("invalid ToPA buffer");
	}

	SEP_DRV_LOG_TRACE_OUT("");
	return;
}

/* ------------------------------------------------------------------------- */
/*!
 * @fn          VOID ipt_Disable (S32)
 *
 * @brief       Disable IPT
 *
 * @param       this_cpu  current cpu id
 *
 * @return      NONE
 *
 * <I>Special Notes:</I>
 */
static VOID
ipt_Disable(S32 this_cpu)
{
	U64 value;

	SEP_DRV_LOG_TRACE_IN("");

	SEP_DRV_LOG_TRACE("disable IPT collection");
	value = SYS_Read_MSR(IA32_RTIT_CTL);
	SYS_Write_MSR(IA32_RTIT_CTL, value & ~IA32_RTIT_CTL_TRACEEN);
	SYS_Write_MSR(IA32_RTIT_CTL, 0ULL);

	SEP_DRV_LOG_TRACE_OUT("");
	return;
}

/* ------------------------------------------------------------------------- */
/*!
 * @fn          VOID ipt_Flush ()
 *
 * @brief       Flush ToPA buffer to internal buffer
 *
 * @param       this_cpu        - cpu id
 *
 * @return      NONE
 *
 */
static U64
ipt_Flush(S32 this_cpu)
{
	U32                    data_size;
	S32                    copied_size;
	DRV_IPT_DROP_INFO_NODE ipt_drop_info;

	SEP_DRV_LOG_TRACE_IN("");

	// Copy packets to driver buffer for flushing
	data_size =
		(U32)(((U64)SYS_Read_MSR(IA32_RTIT_OUTPUT_MASK_PTRS) >> 32) &
		      ipt_buffer_size_mask);
	data_size += (U32)(((U64)SYS_Read_MSR(IA32_RTIT_OUTPUT_MASK_PTRS) &
			    RTIT_OUTPUT_MASK_PTRS_TABLEOFFSET)
			   << 5);

	SEP_DRV_LOG_TRACE("IA32_RTIT_OUTPUT_MASK_PTRS = 0x%llx",
			  SYS_Read_MSR(IA32_RTIT_OUTPUT_MASK_PTRS));
	SEP_DRV_LOG_TRACE("IA32_RTIT_STATUS = 0x%llx",
			  SYS_Read_MSR(IA32_RTIT_STATUS));

	if (data_size == 0) {
		SEP_DRV_LOG_ERROR_TRACE_OUT("No IPT packet available to flush");
		return 0;
	}

	copied_size =
		OUTPUT_IPT_Fill(ipt_node_mine.outbuf_virt_address, data_size);
	if ((U32)copied_size < data_size) {
		SEP_DRV_LOG_ERROR("IPT packet loss copied_size = %d, data_size = %u",
				  copied_size, data_size);
		CPU_STATE_ipt_dropped_count(&pcb[this_cpu])++;
		CPU_STATE_ipt_dropped_packets(&pcb[this_cpu]) +=
			(data_size - copied_size);

		memset(&ipt_drop_info, 0, sizeof(DRV_IPT_DROP_INFO_NODE));
		DRV_IPT_DROP_INFO_length(ipt_drop_info) =
			sizeof(DRV_IPT_DROP_INFO_NODE);
		DRV_IPT_DROP_INFO_descriptor_id(ipt_drop_info) =
			IPT_BUFFER_DROP;
		DRV_IPT_DROP_INFO_drop_size(ipt_drop_info) = data_size;
		DRV_IPT_DROP_INFO_offset(ipt_drop_info) =
			CPU_STATE_ipt_data_offset(&pcb[this_cpu]);

		if (OUTPUT_IPT_Info_Fill((char *)&ipt_drop_info,
					 sizeof(DRV_IPT_DROP_INFO_NODE)) <
		    sizeof(DRV_IPT_DROP_INFO_NODE)) {
			SEP_DRV_LOG_WARNING("Failed to flush the IPT buffer information");
		}
	}

	if (copied_size) {
		CPU_STATE_ipt_collected_count(&pcb[this_cpu])++;
		SEP_DRV_LOG_TRACE_OUT("copied_size = %d", copied_size);
		CPU_STATE_ipt_data_offset(&pcb[this_cpu]) += copied_size;
	} else {
		SEP_DRV_LOG_ERROR_TRACE_OUT("Failed to copy packets from HW buffer");
	}

	return copied_size;
}

/*
 * Initialize the IPT micro dispatch tables
 */
IPT_DISPATCH_NODE gen2_ipt = {
	ipt_Initialize,
	ipt_Finalize,
	ipt_Enable,
	ipt_Disable,
	ipt_Flush
};

/* ------------------------------------------------------------------------- */
/*!
 * @fn          OS_STATUS ipt_Allocate_Buffers (VOID)
 *
 * @brief       Allocate memory and set up MSRs in preparation for IPT
 *
 * @param       NONE
 *
 * @return      OS_STATUS status
 *
 */
static OS_STATUS
ipt_Allocate_Buffers(VOID)
{
	S32 i;
	U32 j;

	SEP_DRV_LOG_TRACE_IN("");

	for (i = 0; i < GLOBAL_STATE_num_cpus(driver_state); i++) {
		ipt_node_on_cpu(i).topa_virt_address =
			CONTROL_Allocate_Memory_Aligned((size_t)IPT_BUF_SIZE,
							TRUE);
		if (!(ipt_node_on_cpu(i).topa_virt_address)) {
			SEP_DRV_LOG_ERROR_TRACE_OUT("IPT buffer allocation failed!");
			return OS_NO_MEM;
		}
		ipt_node_on_cpu(i).topa_phys_address =
			VIRT_TO_PHYS_ADDR(ipt_node_on_cpu(i).topa_virt_address);
		ipt_node_on_cpu(i).outbuf_virt_address =
			CONTROL_Allocate_Memory_Aligned(
				(size_t)IPT_BUF_SIZE * ipt_buffer_num, TRUE);
		if (!(ipt_node_on_cpu(i).outbuf_virt_address)) {
			SEP_DRV_LOG_ERROR_TRACE_OUT("IPT buffer allocation failed!");
			return OS_NO_MEM;
		}
		SEP_DRV_LOG_TRACE("cpu%d topa_va=%llx, topa_pa=%llx", i,
				  ipt_node_on_cpu(i).topa_virt_address,
				  ipt_node_on_cpu(i).topa_phys_address);

		for (j = 0; j < ipt_buffer_num; j++) {
			ipt_node_on_cpu(i)
				.outbuf_phys_address[j] = VIRT_TO_PHYS_ADDR(
				(PVOID)((U64)(ipt_node_on_cpu(i)
						      .outbuf_virt_address) +
					j * IPT_BUF_SIZE));
			((U64 *)ipt_node_on_cpu(i).topa_virt_address)[j] =
				ipt_node_on_cpu(i).outbuf_phys_address[j];
			SEP_DRV_LOG_TRACE("cpu%d buffer%d outbuf_pa=%llx", i, j,
					  ipt_node_on_cpu(i)
						  .outbuf_phys_address[j]);
		}
	}

	SEP_DRV_LOG_TRACE_OUT("");
	return OS_SUCCESS;
}

/* ------------------------------------------------------------------------- */
/*!
 * @fn          VOID ipt_Dellocate_Buffers (VOID *params)
 *
 * @brief       Clean up IPT buffers
 *
 * @param       NONE
 *
 * @return      NONE
 *
 */
static VOID
ipt_Deallocate_Buffers(VOID)
{
	S32 i;

	SEP_DRV_LOG_TRACE_IN("");

	for (i = 0; i < GLOBAL_STATE_num_cpus(driver_state); i++) {
		if (ipt_node_on_cpu(i).topa_virt_address) {
			ipt_node_on_cpu(i).topa_virt_address =
				CONTROL_Free_Memory_Aligned(
					ipt_node_on_cpu(i).topa_virt_address,
					TRUE);
			ipt_node_on_cpu(i).topa_phys_address = 0ULL;
		}
		if (ipt_node_on_cpu(i).outbuf_virt_address) {
			ipt_node_on_cpu(i).outbuf_virt_address =
				CONTROL_Free_Memory_Aligned(
					ipt_node_on_cpu(i).outbuf_virt_address,
					TRUE);
			memset(ipt_node_on_cpu(i).outbuf_phys_address, 0,
			       sizeof(U64) * IPT_BUF_NUM_MAX);
		}
	}

	SEP_DRV_LOG_TRACE_OUT("");
	return;
}

/* ------------------------------------------------------------------------- */
/*!
 * @fn          U64 IPT_TOPA_Interrupted (this_cpu, overflow_status)
 *
 * @brief       Figure out if the IPT ToPA interrupt occurs
 *
 * @param       this_cpu        -- the current cpu
 *              overflow_status -- current value of the global overflow status
 *
 * @return      data size from IPT ToPA buffer
 */
extern U64
IPT_TOPA_Interrupted(S32 this_cpu, U64 overflow_status)
{
	U64 data_size = 0;

	SEP_DRV_LOG_TRACE_IN("this_cpus: %d, overflow_status: %llu", this_cpu,
			     overflow_status);

	if (!(overflow_status & PERF_GLOBAL_STATUS_TOPA_PMI_MASK)) {
		SEP_DRV_LOG_TRACE_OUT("overflow_status: %llu", overflow_status);
		return 0;
	}

	if (!ipt_dispatch || !ipt_dispatch->flush || !ipt_dispatch->enable ||
	    !ipt_dispatch->disable) {
		SEP_DRV_LOG_TRACE_OUT("early exit");
		return 0;
	}

	SEP_DRV_LOG_TRACE("IA32_RTIT_STATUS = 0x%llx",
			  SYS_Read_MSR(IA32_RTIT_STATUS));

	ipt_dispatch->disable(this_cpu);

	data_size = ipt_dispatch->flush(this_cpu);

	SYS_Write_MSR(IA32_PERF_GLOBAL_OVF_CTRL,
		      PERF_GLOBAL_STATUS_TOPA_PMI_MASK);

	ipt_dispatch->enable(this_cpu);

	SEP_DRV_LOG_TRACE_OUT("data_size: %llu", data_size);
	return data_size;
}

/* ------------------------------------------------------------------------- */
/*!
 * @fn          U64 IPT_Start (PVOID param)
 *
 * @brief       Start IPT collection
 *
 * @param       PVOID param
 *
 * @return      NONE
 */
extern VOID
IPT_Start(PVOID param)
{
	S32 this_cpu;

	SEP_DRV_LOG_TRACE_IN("");

	if (!ipt_dispatch || !ipt_dispatch->enable) {
		SEP_DRV_LOG_TRACE_OUT("early exit");
		return;
	}

	this_cpu = CONTROL_THIS_CPU();

	ipt_dispatch->enable(this_cpu);

	SEP_DRV_LOG_TRACE_OUT("");
	return;
}

/* ------------------------------------------------------------------------- */
/*!
 * @fn          U64 IPT_TOPA_Flush (PVOID param)
 *
 * @brief       Flush the remaining IPT ToPA buffer
 *
 * @param       PVOID param
 *
 * @return      NONE
 */
extern VOID
IPT_TOPA_Flush(PVOID param)
{
	S32 this_cpu;
	U64 data_size = 0;

	SEP_DRV_LOG_TRACE_IN("");

	if (!ipt_dispatch || !ipt_dispatch->flush || !ipt_dispatch->disable) {
		SEP_DRV_LOG_TRACE_OUT("early exit");
		return;
	}

	this_cpu = CONTROL_THIS_CPU();

	SEP_DRV_LOG_TRACE("IA32_RTIT_STATUS = 0x%llx",
			  SYS_Read_MSR(IA32_RTIT_STATUS));

	ipt_dispatch->disable(this_cpu);
	data_size = ipt_dispatch->flush(this_cpu);

	SEP_DRV_LOG_TRACE_OUT("");
	return;
}

/* ------------------------------------------------------------------------- */
/*!
 * @fn          VOID IPT_Initialize (DRV_CONFIG pcfg)
 *
 * @brief       Initialize the IPT buffers
 *
 * @param       pcfg  -  Driver Configuration
 *
 * @return      NONE
 */
extern OS_STATUS
IPT_Initialize(DRV_CONFIG pcfg)
{
	OS_STATUS status = OS_SUCCESS;
	S32       this_cpu;

	SEP_DRV_LOG_TRACE_IN("");

	this_cpu = CONTROL_THIS_CPU();

	if (IPT_CONFIG_branch(ipt_config)) {
		ipt_enable_value |= IA32_RTIT_CTL_BRANCHEN;
		SEP_DRV_LOG_TRACE("branch enabled 0x%llx", ipt_enable_value);
	}
	if (IPT_CONFIG_power(ipt_config)) {
		ipt_enable_value |= IA32_RTIT_CTL_PWREVTEN;
		SEP_DRV_LOG_TRACE("power enabled 0x%llx", ipt_enable_value);
	}
	if (IPT_CONFIG_usr(ipt_config)) {
		ipt_enable_value |= IA32_RTIT_CTL_USER;
		SEP_DRV_LOG_TRACE("user enabled 0x%llx", ipt_enable_value);
	}
	if (IPT_CONFIG_os(ipt_config)) {
		ipt_enable_value |= IA32_RTIT_CTL_OS;
		SEP_DRV_LOG_TRACE("os enabled 0x%llx", ipt_enable_value);
	}
	if (IPT_CONFIG_cyc(ipt_config)) {
		ipt_enable_value |= IA32_RTIT_CTL_CYCEN;
		ipt_enable_value |= IPT_CONFIG_cycthresh(ipt_config) << 19;
		SEP_DRV_LOG_TRACE("cyc enabled 0x%llx", ipt_enable_value);
	}
	if (IPT_CONFIG_mtc(ipt_config)) {
		ipt_enable_value |= IA32_RTIT_CTL_MTCEN;
		ipt_enable_value |= IPT_CONFIG_mtcfreq(ipt_config) << 14;
		SEP_DRV_LOG_TRACE("mtc enabled 0x%llx", ipt_enable_value);
	}
	if (IPT_CONFIG_tsc(ipt_config)) {
		ipt_enable_value |= IA32_RTIT_CTL_TSCEN;
		SEP_DRV_LOG_TRACE("tsc enabled 0x%llx", ipt_enable_value);
	}
	if (IPT_CONFIG_psbfreq(ipt_config)) {
		ipt_enable_value |= IPT_CONFIG_psbfreq(ipt_config) << 24;
		SEP_DRV_LOG_TRACE("psbfreq enabled 0x%llx", ipt_enable_value);
	}
	if (IPT_CONFIG_cr3filter(ipt_config)) {
		ipt_enable_value |= IA32_RTIT_CTL_CR3FILTER;
		cr3match_value = IPT_CONFIG_cr3match(ipt_config);
		SEP_DRV_LOG_TRACE("cr3fileter enabled 0x%llx",
				  ipt_enable_value);
		SEP_DRV_LOG_TRACE("cr3match 0x%llx\n", cr3match_value);
	}
	if (IPT_CONFIG_ptw(ipt_config)) {
		ipt_enable_value |= IA32_RTIT_CTL_PTWEN;
		SEP_DRV_LOG_TRACE("ptw enabled 0x%llx", ipt_enable_value);
	}
	if (IPT_CONFIG_ringbuf(ipt_config)) {
		SEP_DRV_LOG_TRACE("ringbuf enabled");
	}
	switch (IPT_CONFIG_bufsize(ipt_config)) {
	case IPT_BUFFER_SMALL:
		ipt_buffer_num = IPT_BUF_NUM_SMALL;
		break;
	case IPT_BUFFER_LARGE:
		ipt_buffer_num = IPT_BUF_NUM_LARGE;
		break;
	default:
		ipt_buffer_num = IPT_BUF_NUM_DEFAULT;
		break;
	}
	SEP_DRV_LOG_TRACE("bufsize set to %u", ipt_buffer_num);
	ipt_buffer_size_mask = ipt_buffer_num * 4096 - 1;
	SEP_DRV_LOG_TRACE("buf size mask set to 0x%llx", ipt_buffer_size_mask);

	SEP_DRV_LOG_TRACE("ipt_enable_value 0x%llx, ipt_buffer_num = %u",
			  ipt_enable_value, ipt_buffer_num);

	if (DRV_CONFIG_ipt_mode(pcfg)) {
		SEP_DRV_LOG_TRACE("collection mode=%u",
				  DRV_CONFIG_ipt_mode(pcfg));
		switch (DRV_CONFIG_ipt_mode(pcfg)) {
		case 2:
			SEP_DRV_LOG_TRACE("Set up the 2nd gen IPT dispatch table");
			ipt_dispatch = &gen2_ipt;
			break;
		default:
			SEP_DRV_LOG_TRACE("Unknown IPT generation. Will not collect IPT information");
			break;
		}
		if (ipt_dispatch) {
			if (ipt_dispatch->init) {
				CONTROL_Invoke_Parallel(ipt_dispatch->init,
							(PVOID)(size_t)this_cpu);
			}
			status = ipt_Allocate_Buffers();
		}
	}

	SEP_DRV_LOG_TRACE_OUT("");
	return status;
}

/* ------------------------------------------------------------------------- */
/*!
 * @fn          VOID IPT_Destroy (DRV_CONFIG pcfg)
 *
 * @brief       Clean up the IPT related buffers
 *
 * @param       pcfg  -  Driver Configuration
 *
 * @return      NONE
 */
extern VOID
IPT_Destroy(DRV_CONFIG pcfg)
{
	S32 this_cpu;

	SEP_DRV_LOG_TRACE_IN("");

	this_cpu = CONTROL_THIS_CPU();

	if (DRV_CONFIG_ipt_mode(pcfg) && ipt_dispatch) {
		if (ipt_dispatch->fini) {
			CONTROL_Invoke_Parallel(ipt_dispatch->fini,
						(PVOID)(size_t)this_cpu);
		}

		ipt_Deallocate_Buffers();

		ipt_dispatch = NULL;
	}

	ipt_enable_value     = 0ULL;
	cr3match_value       = 0ULL;
	ipt_buffer_num       = 0;
	ipt_buffer_size_mask = 0ULL;

	SEP_DRV_LOG_TRACE_OUT("");
	return;
}

