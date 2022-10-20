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
#include "inc/pci.h"
#include "inc/unc_common.h"
#include "inc/utility.h"
#include "inc/pmu_info_struct.h"
#include "inc/pmu_list.h"

extern UNCORE_TOPOLOGY_INFO_NODE        uncore_topology;
extern PLATFORM_TOPOLOGY_PROG_NODE      platform_topology_prog_node;
extern UNCORE_DISCOVERY_TABLE_LIST_NODE uncore_discovery_tables;
extern U64                             *read_counter_info;
#if defined(DRV_PMT_ENABLE)
extern PMT_TOPOLOGY_DISCOVERY_LIST_NODE pmt_topology;
extern DRV_PMT_TELEM_DEV_NODE           pmt_devices[MAX_PMT_DEVICES];
extern U32                              pmt_dev_index;
#endif

/* this is the table to keep pci_bus structure for PCI devices
 * for both pci config access and mmio access
 */
UNC_PCIDEV_NODE unc_pcidev_map[MAX_DEVICES];

#define GET_PACKAGE_NUM(device_type, cpu) \
	(((device_type) == DRV_SINGLE_INSTANCE) ? 0 : core_to_package_map[cpu])

/************************************************************/
/*
 * unc common Dispatch functions
 *
 ************************************************************/
extern void
UNC_COMMON_Dummy_Func(PVOID param)
{
	UNREFERENCED_PARAMETER(param);
	SEP_DRV_LOG_TRACE_IN("Dummy param: %p.", param);
	SEP_DRV_LOG_TRACE_OUT("Empty function.");
	return;
}

/************************************************************/
/*
 * UNC common PCI  based API
 *
 ************************************************************/

/*!
 * @fn          OS_STATUS UNC_COMMON_Add_Bus_Map
 *
 * @brief       This code discovers which package's data is read off of which bus.
 *
 * @param       None
 *
 * @return      OS_STATUS
 *
 * <I>Special Notes:</I>
 *     This probably will move to the UBOX once that is programmed.
 */
OS_STATUS
UNC_COMMON_Add_Bus_Map(U32 uncore_did, U32 dev_node, U32 bus_no, U32 domain_no)
{
	U32 i       = 0;
	U32 entries = 0;

	UNREFERENCED_PARAMETER(uncore_did);
	SEP_DRV_LOG_TRACE_IN("");

	if (!UNC_PCIDEV_busno_list(&(unc_pcidev_map[dev_node]))) {
		// allocate array for holding bus mapping
		// package based device: an entry per package, all units in the same package are in the same bus.
		// system based device:  an entry per unit if in different bus
		entries = GET_MAX_PCIDEV_ENTRIES(num_packages);
		UNC_PCIDEV_busno_list(&(unc_pcidev_map[dev_node])) =
			CONTROL_Allocate_Memory(entries * sizeof(S32));
		if (UNC_PCIDEV_busno_list(&(unc_pcidev_map[dev_node])) == NULL) {
			SEP_DRV_LOG_ERROR_TRACE_OUT("Memory allocation failure!");
			return OS_NO_MEM;
		}
		UNC_PCIDEV_domainno_list(&(unc_pcidev_map[dev_node])) =
			CONTROL_Allocate_Memory(entries * sizeof(S32));
		if (UNC_PCIDEV_domainno_list(&(unc_pcidev_map[dev_node])) == NULL) {
			SEP_DRV_LOG_ERROR_TRACE_OUT("Memory allocation failure!");
			return OS_NO_MEM;
		}
		UNC_PCIDEV_num_entries(&(unc_pcidev_map[dev_node])) = 0;
		UNC_PCIDEV_max_entries(&(unc_pcidev_map[dev_node])) = entries;
		for (i = 0; i < entries; i++) {
			UNC_PCIDEV_busno_entry(&(unc_pcidev_map[dev_node]), i) = INVALID_BUS_NUMBER;
		}
	} else {
		entries = UNC_PCIDEV_max_entries(&(unc_pcidev_map[dev_node]));
	}

	for (i = 0; i < UNC_PCIDEV_num_entries(&(unc_pcidev_map[dev_node])); i++) {
		if ((UNC_PCIDEV_busno_entry(&(unc_pcidev_map[dev_node]), i) == (S32)bus_no) &&
		    (UNC_PCIDEV_domainno_entry(&(unc_pcidev_map[dev_node]), i) == (S32)domain_no)) {
			SEP_DRV_LOG_TRACE_OUT("Already in the map,  another unit, no add.");
			return OS_SUCCESS;
		}
	}
	if (i < entries) {
		UNC_PCIDEV_busno_entry(&(unc_pcidev_map[dev_node]), i)    = (S32)bus_no;
		UNC_PCIDEV_domainno_entry(&(unc_pcidev_map[dev_node]), i) = (S32)domain_no;
		UNC_PCIDEV_num_entries(&(unc_pcidev_map[dev_node]))++;
		SEP_DRV_LOG_TRACE_OUT("Add pkgno=%d num_packages=%d domainno=%x busno=%x devnode=%d.",
				      i, num_packages, domain_no, bus_no, dev_node);
		return OS_SUCCESS;
	}
	SEP_DRV_LOG_ERROR_TRACE_OUT("Exceed max map entries, drop this bus map!");
	return OS_NO_MEM;
}

extern OS_STATUS
UNC_COMMON_Init(void)
{
	U32 i = 0;

	for (i = 0; i < MAX_DEVICES; i++) {
		SEP_DRV_MEMSET(&(unc_pcidev_map[i]), 0, sizeof(UNC_PCIDEV_NODE));
	}

	SEP_DRV_MEMSET((char *)&uncore_topology, 0, sizeof(UNCORE_TOPOLOGY_INFO_NODE));
	SEP_DRV_MEMSET((char *)&platform_topology_prog_node, 0,
		       sizeof(PLATFORM_TOPOLOGY_PROG_NODE));
#if defined(DRV_PMT_ENABLE)
	SEP_DRV_MEMSET((char *)&pmt_topology, 0, sizeof(PMT_TOPOLOGY_DISCOVERY_LIST_NODE));
#endif

	return OS_SUCCESS;
}

/*!
 * @fn         extern VOID UNC_COMMON_Clean_Up(void)
 *
 * @brief      clear out out programming
 *
 * @param      None
 *
 * @return     None
 */
extern void
UNC_COMMON_Clean_Up(void)
{
	U32 i = 0;
	for (i = 0; i < MAX_DEVICES; i++) {
		if (UNC_PCIDEV_busno_list(&(unc_pcidev_map[i]))) {
			UNC_PCIDEV_busno_list(&(unc_pcidev_map[i])) =
				CONTROL_Free_Memory(UNC_PCIDEV_busno_list(&(unc_pcidev_map[i])));
		}
		if (UNC_PCIDEV_domainno_list(&(unc_pcidev_map[i]))) {
			UNC_PCIDEV_domainno_list(&(unc_pcidev_map[i])) =
				CONTROL_Free_Memory(UNC_PCIDEV_domainno_list(&(unc_pcidev_map[i])));
		}
		if (UNC_PCIDEV_mmio_map(&(unc_pcidev_map[i]))) {
			UNC_PCIDEV_mmio_map(&(unc_pcidev_map[i])) =
				CONTROL_Free_Memory(UNC_PCIDEV_mmio_map(&(unc_pcidev_map[i])));
		}
		SEP_DRV_MEMSET(&(unc_pcidev_map[i]), 0, sizeof(UNC_PCIDEV_NODE));
	}
	return;
}

#if defined(DRV_PMT_ENABLE)
/*!
 * @fn         extern U32 UNC_COMMON_Scan_PMT_Device()
 *
 * @brief      Map the PMT device discovered and notified by the pmt driver
 *
 * @param      None
 *
 * @return     Return true if the map is found and falst otherwise
 */
extern U32
UNC_COMMON_Scan_PMT_Device(void)
{
	U32 i = 0;
	U32 j = 0;
	U32 k = 0;

	for (i = 0; i < PMT_TOPOLOGY_DISCOVERY_LIST_num_pmt_devices(&pmt_topology); i++) {
		for (j = 0; j < PMT_TOPOLOGY_DISCOVERY_LIST_num_tiles(&pmt_topology, i); j++) {
			for (k = 0; k < pmt_dev_index; k++) {
				if (PMT_TOPOLOGY_DISCOVERY_LIST_tile_domain(&pmt_topology, i, j) ==
					    pmt_devices[k].domain &&
				    //PMT_TOPOLOGY_DISCOVERY_LIST_tile_bus(&pmt_topology, i, j) == pmt_devices[k].bus &&
				    PMT_TOPOLOGY_DISCOVERY_LIST_tile_device(&pmt_topology, i, j) ==
					    pmt_devices[k].device &&
				    PMT_TOPOLOGY_DISCOVERY_LIST_tile_function(
					    &pmt_topology, i, j) == pmt_devices[k].function &&
				    PMT_TOPOLOGY_DISCOVERY_LIST_tile_oobmsm_deviceid(
					    &pmt_topology, i, j) == pmt_devices[k].device_id &&
				    PMT_TOPOLOGY_DISCOVERY_LIST_tile_pmt_endpoint_guid(
					    &pmt_topology, i, j) == pmt_devices[k].guid &&
				    !pmt_devices[k].found) {
					pmt_devices[k].dev_idx = i;
					SEP_DRV_LOG_DETECTION("PMT Device ndex=%i bus=%x dev=%x func=%x devid=%x venid=%x found\n",
							      k, pmt_devices[k].bus,
							      pmt_devices[k].device,
							      pmt_devices[k].function,
							      pmt_devices[k].device_id,
							      pmt_devices[k].vendor_id);
					pmt_devices[k].found = 1;
					PMT_TOPOLOGY_DISCOVERY_LIST_topology_detected(
						&pmt_topology) = 1;
					PMT_TOPOLOGY_DISCOVERY_LIST_num_entries_found(&pmt_topology,
										      i)++;
					PMT_TOPOLOGY_DISCOVERY_LIST_tile_found(&pmt_topology, i,
									       j)   = 1;
					PMT_TOPOLOGY_DISCOVERY_LIST_tile_unit_id(&pmt_topology, i,
										 j) = k;
					PMT_TOPOLOGY_DISCOVERY_LIST_tile_bus(&pmt_topology, i, j) =
						pmt_devices[k].bus;
					//PMT_TOPOLOGY_DISCOVERY_LIST_tile_id(&pmt_topology, i, j)    = i;
					SEP_DRV_LOG_DETECTION("Found CTA device 0x%x at DBDF(%x:%x:%x:%x) [%u unit(s) so far].",
							      pmt_devices[k].device_id,
							      pmt_devices[k].domain,
							      pmt_devices[k].bus,
							      pmt_devices[k].device,
							      pmt_devices[k].function,
							      PMT_TOPOLOGY_DISCOVERY_LIST_num_entries_found(
								      &pmt_topology, i));
					break;
				}
			}
		}
	}

	return TRUE;
}
#endif

/*!
 * @fn          static VOID UNC_COMMON_PCI_Scan_For_Uncore(VOID*)
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

extern VOID
UNC_COMMON_PCI_Scan_For_Uncore(PVOID param, U32 dev_node, DEVICE_CALLBACK callback)
{
	U32 device_id;
	U32 value;
	U32 vendor_id;
	U32 busno;
	U32 domainno;
	U32 j, k, l;
	U32 device_found = 0;
	UNREFERENCED_PARAMETER(param);
	UNREFERENCED_PARAMETER(callback);

	SEP_DRV_LOG_TRACE_IN("Dummy param: %p, dev_node: %u, callback: %p.", param, dev_node,
			     callback);

	for (domainno = 0; domainno < num_pci_domains; domainno++) {
		for (busno = 0; busno < MAX_BUSNO; busno++) {
			for (j = 0; j < MAX_PCI_DEVNO; j++) {
				if (!(UNCORE_TOPOLOGY_INFO_pcidev_valid(&uncore_topology, dev_node,
									j))) {
					continue;
				}
				for (k = 0; k < MAX_PCI_FUNCNO; k++) {
					if (!(UNCORE_TOPOLOGY_INFO_pcidev_is_devno_funcno_valid(
						    &uncore_topology, dev_node, j, k))) {
						continue;
					}
					device_found = 0;
					// Assume a single pci domain is connected with multiple sockets with
					// consecutive package IDs
					// if the number of pci domains is smaller than the number of packages
					value = PCI_Read_U32_Valid(domainno, busno, j, k, 0, 0);
					CONTINUE_IF_NOT_GENUINE_INTEL_DEVICE(value, vendor_id,
									     device_id);
					SEP_DRV_LOG_DETECTION("Uncore deviceID=0x%x DBDF(%x:%x:%x:%x).",
							      device_id, domainno, busno, j, k);

					for (l = 0; l < UNCORE_TOPOLOGY_INFO_num_deviceid_entries(
								&uncore_topology, dev_node);
					     l++) {
						if (UNCORE_TOPOLOGY_INFO_deviceid(&uncore_topology,
										  dev_node,
										  l) == device_id) {
							device_found = 1;
							break;
						}
					}
					if (device_found) {
						if (UNC_COMMON_Add_Bus_Map(device_id, dev_node,
									   busno, domainno) ==
						    OS_SUCCESS) {
							UNCORE_TOPOLOGY_INFO_pcidev_num_entries_found(
								&uncore_topology, dev_node, j, k)++;
							SEP_DRV_LOG_DETECTION("Found device 0x%x at DBDF(%x:%x:%x:%x) [%u unit(s) so far].",
									      device_id, domainno,
									      busno, j, k,
									      UNCORE_TOPOLOGY_INFO_pcidev_num_entries_found(
										      &uncore_topology,
										      dev_node, j,
										      k));
						}
					}
				}
			}
		}
	}

	SEP_DRV_LOG_TRACE_OUT("");
	return;
}

/*!
 * @fn          extern VOID UNC_COMMON_PCI_Scan_For_CapId(UNCORE_DISCOVERY_DVSEC_CONFIG, DRV_PCI_DEVICE_ENTRY)
 *
 * @brief       Walk through the PCICFG space, identify all the matching capability IDs from all sockets,
 *              and domains and populate the corresponding BARs into the output array bar_list
 *
 * @param       config   - pointer to UNCORE_DISCOVERY_DVSEC_CONFIG structure
 *              bar_list - list returned from the function with the BAR addresses
 *
 * @return      None
 *
 * <I>Special Notes:</I>
 */
extern VOID
UNC_COMMON_PCI_Scan_For_CapId(UNCORE_DISCOVERY_DVSEC_CONFIG config, DRV_PCI_DEVICE_ENTRY bar_list)
{
	U32 device_id;
	U32 value;
	U32 vendor_id;
	U32 busno;
	U32 domainno;
	U32 j, k;
	U32 bir         = 0;
	U32 bar_offset  = 0;
	U32 bar         = 0;
	U32 i           = 0;
	U32 offset      = EXT_CAPID_START_OFFSET;
	U32 next_offset = 0;
	U32 cap_id      = 0;

	SEP_DRV_LOG_TRACE_IN("Config: %p, bar_list: %p.", config, bar_list);

	for (domainno = 0; domainno < num_pci_domains; domainno++) {
		for (busno = 0; busno < MAX_BUSNO; busno++) {
			for (j = 0; j < MAX_PCI_DEVNO; j++) {
				for (k = 0; k < MAX_PCI_FUNCNO; k++) {
					value = PCI_Read_U32_Valid(domainno, busno, j, k, 0, 0);
					CONTINUE_IF_NOT_GENUINE_INTEL_DEVICE(value, vendor_id,
									     device_id);
					offset = EXT_CAPID_START_OFFSET;
					while ((value = PCI_Read_U32_Valid(domainno, busno, j, k,
									   offset, 0))) {
						next_offset =
							(value & EXT_CAPID_LIST_NEXT_ADDR_MASK) >>
							EXT_CAPID_LIST_NEXT_ADDR_BITSHIFT;
						cap_id = (value & EXT_CAPID_MASK);
						if (cap_id ==
						    UNCORE_DISCOVERY_DVSEC_pci_ext_cap_id(config)) {
							value = PCI_Read_U32(
								domainno, busno, j, k,
								offset +
									UNCORE_DISCOVERY_DVSEC_dvsec_offset(
										config));
							if ((value &
							     UNCORE_DISCOVERY_DVSEC_dvsec_id_mask(
								     config)) ==
							    UNCORE_DISCOVERY_DVSEC_dvsec_id_pmon(
								    config)) {
								bir = PCI_Read_U32(
									domainno, busno, j, k,
									offset +
										UNCORE_DISCOVERY_DVSEC_dvsec_offset(
											config) +
										4);
								bir = bir &
								      UNCORE_DISCOVERY_DVSEC_dvsec_bir_mask(
									      config);
								bar_offset = 0x10 + (bir * 4);
								bar = PCI_Read_U32(domainno, busno,
										   j, k,
										   bar_offset);
								bar = bar & ~(PAGE_SIZE - 1);
								DRV_PCI_DEVICE_ENTRY_bar_address(
									&bar_list[i]) = bar;
								j = MAX_PCI_DEVNO;
								k = MAX_PCI_FUNCNO;
								SEP_DRV_LOG_TRACE("%u. BAR:0x%x", i,
										  bar);
								i++;
								break;
							}
						}
						if (!next_offset ||
						    next_offset >= EXT_CAPID_END_OFFSET) {
							break;
						}
						offset = next_offset;
					}
				}
			}
		}
	}
	SEP_DRV_LOG_TRACE_OUT("");
	return;
}

/* ------------------------------------------------------------------------- */
/*!
 * @fn static void unc_common_Read_MSR(pvoid param)
 *
 * @param param - pointer to the buffer to store the MSR counts
 *
 * @return none
 *
 * @brief
 * @brief  Read the U64 value at address in buf_drv_to_usr and
 * @brief  write the result into buf_usr_to_drv.
 *
 * <I>Special Notes</I>
 */
static VOID
unc_common_Read_MSR(PVOID param)
{
	MSR_DATA topo_msr = (MSR_DATA)param;

	SEP_DRV_LOG_TRACE_IN("");

	if (!param) {
		SEP_DRV_LOG_ERROR_TRACE_OUT("Error: param is null");
		return;
	}
	MSR_DATA_value(topo_msr) = (U64)SYS_Read_MSR((U32)MSR_DATA_addr(topo_msr));

	SEP_DRV_LOG_TRACE_OUT("");

	return;
}

/* ------------------------------------------------------------------------- */
/*!
 * @fn static OS_STATUS unc_common_Read_MSR_Package(IOCTL_ARGS arg)
 *
 * @param arg      U32 msr_address  - MSR address to be read
 * @param arg      U32 package_num  - Package on which to read MSR
 *
 * @return OS_STATUS
 *
 * @brief  Read the MSR value on a specific package
 * @brief  Returns the MSR value read
 *
 * <I>Special Notes</I>
 */
static U64
unc_common_Read_MSR_Package(U32 msr_address, U32 package_num)
{
	S32           i;
	U64           msr_value = 0ULL;
	MSR_DATA_NODE topo_msr_node;

	SEP_DRV_LOG_TRACE_IN("");

	for (i = 0; i < GLOBAL_STATE_num_cpus(driver_state); i++) {
		if (package_num == core_to_package_map[i]) {
			MSR_DATA_addr(&topo_msr_node) = msr_address;
#if !defined(DRV_OS_MAC)
			CONTROL_Invoke_Cpu(i, unc_common_Read_MSR, (VOID *)&topo_msr_node);
			msr_value = MSR_DATA_value(&topo_msr_node);
#else
			msr_value = SYS_Read_MSR(msr_address);
#endif
			break;
		}
	}

	SEP_DRV_LOG_TRACE_OUT("Success");

	return msr_value;
}

/*!
 * @fn          extern VOID UNC_COMMON_Get_Platform_Topology()
 *
 * @brief       This function will walk through the platform registers to retrieve information and calculate the bus no.
 *              Reads appropriate pci_config regs and populates the PLATFORM_TOPOLOGY_PROG_NODE structure with the reg value.
 *
 * @param       U32             dev_node - Device no.
 *
 * @return      None
 *
 * <I>Special Notes:</I>
 *                   device_num corresponds to Memory controller
 *                   func_num  corresponds to Channel number
 *                   reg_offset corresponds to dimm slot
 */
extern VOID
UNC_COMMON_Get_Platform_Topology(U32 dev_node)
{
	U32           num_registers = 0;
	U32           device_index  = 0;
	U32           bus_num       = 0;
	U32           domain_num    = 0;
	U32           i             = 0;
	U32           func_num      = 0;
	U32           num_pkgs      = num_packages;
	U32           device_num    = 0;
	U32           reg_offset    = 0;
	U32           len           = 0;
	U64           reg_value     = 0;
	U32           device_value  = 0;
	U64           reg_mask      = 0;
	U32           reg_id        = 0;
	U32           vendor_id;
	U32           device_id;
	U32           valid;
	U32           reg_idx          = 0;
	U32           num_found_units  = 0;
	U64           physical_address = 0ULL;
	U64           virtual_address  = 0ULL;
	U64           mmio_base1       = 0ULL;
	U64           mmio_base2       = 0ULL;
	SEP_MMIO_NODE tmp_map          = { 0 };

	PLATFORM_TOPOLOGY_REG_EXT topology_regs_ext = NULL;
	U32 unc_topo_node = PLATFORM_TOPOLOGY_PROG_EXT_topology_device_device_index(
		&platform_topology_prog_node, dev_node);

	SEP_DRV_LOG_TRACE_IN("Dev_node: %u.", dev_node);
	// Backward Compatibility - old version has less max devices (similar checks below)
	if (dev_node < MAX_TOPOLOGY_DEV_OLD) {
		PLATFORM_TOPOLOGY_PROG_topology_device_prog_valid(&platform_topology_prog_node,
								  dev_node) = 1;
	}
	PLATFORM_TOPOLOGY_PROG_EXT_topology_device_prog_valid(&platform_topology_prog_node,
							      dev_node) = 1;

	if (num_packages > MAX_PACKAGES) {
		SEP_DRV_LOG_ERROR("Num_packages %d > MAX_PACKAGE, getting for only %d packages.",
				  num_packages, MAX_PACKAGES);
		num_pkgs = MAX_PACKAGES;
	}

	num_registers = PLATFORM_TOPOLOGY_PROG_EXT_topology_device_num_registers(
		&platform_topology_prog_node, dev_node);
	topology_regs_ext = PLATFORM_TOPOLOGY_PROG_EXT_topology_topology_regs(
		&platform_topology_prog_node, dev_node);
	device_index = PLATFORM_TOPOLOGY_PROG_EXT_topology_device_device_index(
		&platform_topology_prog_node, dev_node);

	for (i = 0; i < num_pkgs; i++) {
		for (len = 0; len < num_registers; len++) {
			if (PLATFORM_TOPOLOGY_PROG_EXT_topology_device_distinct_buses(
				    &platform_topology_prog_node, dev_node)) {
				// bus numbers are different for devices within the package (for e.g. iio)
				reg_idx = PLATFORM_TOPOLOGY_REG_EXT_unit_index(topology_regs_ext,
									       len) +
					  i * num_found_units; //5;
			} else {
				//same bus numbers for all devices in the same package
				reg_idx = i;
			}
			if (PLATFORM_TOPOLOGY_REG_EXT_reg_type(topology_regs_ext, len) ==
			    PMU_REG_PROG_MSR) {
				reg_id = PLATFORM_TOPOLOGY_REG_EXT_reg_id(topology_regs_ext, len);

				// Validation check from PMU list
				if (!PMU_LIST_Check_MSR(reg_id)) {
					SEP_DRV_LOG_ERROR("Received invalid MSR 0x%x. Skipped!",
							  reg_id);
					continue;
				} else {
					SEP_DRV_LOG_TRACE("Verified MSR 0x%llx\n", reg_id);
				}

				if (PLATFORM_TOPOLOGY_PROG_EXT_topology_device_scope(
					    &platform_topology_prog_node, dev_node) ==
				    PACKAGE_EVENT) {
					reg_value = unc_common_Read_MSR_Package(reg_id, i);
				} else {
					reg_value = SYS_Read_MSR(reg_id);
				}
				reg_mask =
					PLATFORM_TOPOLOGY_REG_EXT_reg_mask(topology_regs_ext, len);
				PLATFORM_TOPOLOGY_REG_EXT_reg_value(topology_regs_ext, len, i) =
					reg_value & reg_mask;
				SEP_DRV_LOG_DETECTION("Device ID=%d unc_topo_node=%d Read MSR 0x%x Val=%llx\n",
						      dev_node, unc_topo_node,
						      PLATFORM_TOPOLOGY_REG_EXT_reg_id(
							      topology_regs_ext, len),
						      PLATFORM_TOPOLOGY_REG_EXT_reg_value(
							      topology_regs_ext, len, i));
			} else if (PLATFORM_TOPOLOGY_REG_EXT_reg_type(topology_regs_ext, len) ==
				   PMU_REG_PROG_PCI) {
				if (!IS_BUS_MAP_VALID(unc_topo_node, reg_idx)) {
					SEP_DRV_LOG_TRACE("BUS MAP invalid for device=%d reg_idx=%d\n",
							  dev_node, reg_idx);
					continue;
				}
				if (!PLATFORM_TOPOLOGY_REG_EXT_device_valid(topology_regs_ext,
									    len)) {
					SEP_DRV_LOG_TRACE("Device=%d reg_idx=%d invalid\n",
							  dev_node, reg_idx);
					continue;
				}
				bus_num    = GET_BUS_MAP(unc_topo_node, reg_idx);
				domain_num = GET_DOMAIN_MAP(unc_topo_node, reg_idx);
				device_num =
					PLATFORM_TOPOLOGY_REG_EXT_device(topology_regs_ext, len);
				func_num =
					PLATFORM_TOPOLOGY_REG_EXT_function(topology_regs_ext, len);
				reg_offset =
					PLATFORM_TOPOLOGY_REG_EXT_reg_id(topology_regs_ext, len);

				// Validation check from PMU list
				if (!PMU_LIST_Check_PCI((U8)bus_num, (U8)device_num, (U8)func_num,
							reg_offset)) {
					PLATFORM_TOPOLOGY_REG_EXT_device_valid(topology_regs_ext,
									       len) = 0;
					SEP_DRV_LOG_ERROR("Received invalid PCI device DBDF(%x:%x:%d:%d) Off=%x Skipped!",
							  domain_num, bus_num, device_num, func_num,
							  reg_offset);
					continue;
				} else {
					SEP_DRV_LOG_DETECTION("Verified PCI dev_node=%d unc_topo_node=%d reg_idx=%d DBDF(%x:%x:%d:%d) Off=%x\n",
							      dev_node, unc_topo_node, reg_idx,
							      domain_num, bus_num, device_num,
							      func_num, reg_offset);
				}

				device_value = PCI_Read_U32_Valid(domain_num, bus_num, device_num,
								  func_num, 0, 0);
				CHECK_IF_GENUINE_INTEL_DEVICE(device_value, vendor_id, device_id,
							      valid);
				if (!valid) {
					PLATFORM_TOPOLOGY_REG_EXT_device_valid(topology_regs_ext,
									       len) = 0;
				}
				if (PLATFORM_TOPOLOGY_REG_EXT_device_id(topology_regs_ext, len) &&
				    PLATFORM_TOPOLOGY_REG_EXT_device_id(topology_regs_ext, len) !=
					    device_id) {
					continue;
				}
				PLATFORM_TOPOLOGY_REG_EXT_reg_value(topology_regs_ext, len, i) =
					PCI_Read_U32_Valid(domain_num, bus_num, device_num,
							   func_num, reg_offset, PCI_INVALID_VALUE);
				PLATFORM_TOPOLOGY_REG_EXT_bus_number(topology_regs_ext, len, i) =
					(U16)bus_num;
				PLATFORM_TOPOLOGY_REG_EXT_bus(topology_regs_ext, len) =
					(U16)bus_num;
				PLATFORM_TOPOLOGY_REG_EXT_domain_number(topology_regs_ext, len, i) =
					(U16)domain_num;
				SEP_DRV_LOG_DETECTION("PCI topology DBDF(%x:%x:%d:%d) val=%llx\n",
						      domain_num, bus_num, device_num, func_num,
						      PLATFORM_TOPOLOGY_REG_EXT_reg_value(
							      topology_regs_ext, len, i));
				if (i == 0 && PLATFORM_TOPOLOGY_REG_EXT_unit_index(
						      topology_regs_ext, len) >= num_found_units) {
					num_found_units = PLATFORM_TOPOLOGY_REG_EXT_unit_index(
								  topology_regs_ext, len) +
							  1;
				}
			} else if (PLATFORM_TOPOLOGY_REG_EXT_reg_type(topology_regs_ext, len) ==
				   PMU_REG_PROG_MMIO) {
				if (!IS_BUS_MAP_VALID(unc_topo_node, reg_idx)) {
					SEP_DRV_LOG_TRACE("BUS MAP invalid for device=%d reg_idx=%d\n",
							  dev_node, reg_idx);
					continue;
				}
				if (!PLATFORM_TOPOLOGY_REG_EXT_device_valid(topology_regs_ext,
									    len)) {
					SEP_DRV_LOG_TRACE("Device=%d reg_idx=%d invalid\n",
							  dev_node, reg_idx);
					continue;
				}
				bus_num    = GET_BUS_MAP(unc_topo_node, reg_idx);
				domain_num = GET_DOMAIN_MAP(unc_topo_node, reg_idx);
				device_num =
					PLATFORM_TOPOLOGY_REG_EXT_device(topology_regs_ext, len);
				func_num =
					PLATFORM_TOPOLOGY_REG_EXT_function(topology_regs_ext, len);
				reg_offset =
					PLATFORM_TOPOLOGY_REG_EXT_reg_id(topology_regs_ext, len);

				// Validation check from PMU list
				if (!PMU_LIST_Check_PCI((U8)bus_num, (U8)device_num, (U8)func_num,
							PLATFORM_TOPOLOGY_REG_EXT_main_bar_offset(
								topology_regs_ext, len)) ||
				    !PMU_LIST_Check_PCI(
					    (U8)bus_num, (U8)device_num, (U8)func_num,
					    PLATFORM_TOPOLOGY_REG_EXT_secondary_bar_offset(
						    topology_regs_ext, len))) {
					PLATFORM_TOPOLOGY_REG_EXT_device_valid(topology_regs_ext,
									       len) = 0;
					SEP_DRV_LOG_ERROR("Received invalid MMIO device DBDF(%x:%x:%d:%d) Off=%llx Skipped!",
							  domain_num, bus_num, device_num, func_num,
							  PLATFORM_TOPOLOGY_REG_EXT_main_bar_offset(
								  topology_regs_ext, len));
					continue;
				}
				mmio_base1 = PCI_Read_U32(domain_num, bus_num, device_num, func_num,
							  PLATFORM_TOPOLOGY_REG_EXT_main_bar_offset(
								  topology_regs_ext, len));
				mmio_base1 = mmio_base1 & PLATFORM_TOPOLOGY_REG_EXT_main_bar_mask(
								  topology_regs_ext, len);
				if (PLATFORM_TOPOLOGY_REG_EXT_secondary_bar_offset(
					    topology_regs_ext, len)) {
					mmio_base2 = PCI_Read_U32(
						domain_num, bus_num, device_num, func_num,
						PLATFORM_TOPOLOGY_REG_EXT_secondary_bar_offset(
							topology_regs_ext, len));
					mmio_base2 = mmio_base2 &
						     PLATFORM_TOPOLOGY_REG_EXT_secondary_bar_mask(
							     topology_regs_ext, len);
				}
				physical_address =
					((mmio_base1 << PLATFORM_TOPOLOGY_REG_EXT_main_bar_shift(
						  topology_regs_ext, len)) |
					 (mmio_base2
					  << PLATFORM_TOPOLOGY_REG_EXT_secondary_bar_shift(
						     topology_regs_ext, len))) +
					PLATFORM_TOPOLOGY_REG_EXT_base_mmio_offset(
						topology_regs_ext, len);
				PCI_Map_Memory(&tmp_map, physical_address,
					       PLATFORM_TOPOLOGY_REG_EXT_mmio_map_size(
						       topology_regs_ext, len));
				virtual_address = SEP_MMIO_NODE_virtual_address(&tmp_map);
				PLATFORM_TOPOLOGY_REG_EXT_reg_value(topology_regs_ext, len, i) =
					PCI_MMIO_Read_U32(virtual_address, reg_offset);
				PCI_Unmap_Memory(&tmp_map);
				SEP_DRV_LOG_DETECTION("MMIO base1=%llx base2=%llx phys=%llx, virt=%llx\n",
						      mmio_base1, mmio_base2, physical_address,
						      virtual_address);
				SEP_DRV_LOG_DETECTION("MMIO dev_node=%d unc_topo_node=%d val = %llx bus=%x\n",
						      dev_node, unc_topo_node,
						      PLATFORM_TOPOLOGY_REG_EXT_reg_value(
							      topology_regs_ext, len, i),
						      bus_num);
			}
		}
		if (PLATFORM_TOPOLOGY_PROG_EXT_topology_device_scope(&platform_topology_prog_node,
								     dev_node) == SYSTEM_EVENT) {
			break;
		}
	}
	SEP_DRV_LOG_TRACE_OUT("");

	return;
}

/* ------------------------------------------------------------------------- */
/*!
 * @fn static void unc_common_Disc_Add_Allowlist_Key_Valu(U64, U16)
 *
 * @param   base_addr - base address of the MMIO uncore unit
 *          offset    - offset from base address
 *
 * @return  None
 *
 * @brief   Adds entry into MMIO Allowlist
 *
 * <I>Special Notes</I>
 */

static void
unc_common_Disc_Add_Allowlist_Key_Value(U64 base_addr, U64 offset)
{
	U64 key = 0;
	SEP_DRV_LOG_TRACE_IN("");

	key = base_addr << 16 | (U16)offset;
	PMU_LIST_Add_To_MMIO_List(key, NULL);

	SEP_DRV_LOG_TRACE_OUT("Success");
}
/* ------------------------------------------------------------------------- */
/*!
 * @fn static void unc_common_Disc_Add_Allowlist_Entry(UNCORE_DISCOVERY_TABLE)
 *
 * @param   UNCORE_DISCOVERY_TABLE pkg_table - pointer to discovery table
 *
 * @return  None
 *
 * @brief   Iterates the discovery entries to populate Allowlist
 *
 * <I>Special Notes</I>
 */
static void
unc_common_Disc_Add_Allowlist_Entry(UNCORE_DISCOVERY_TABLE pkg_table)
{
	U32                   i              = 0;
	U32                   j              = 0;
	U32                   max_blocks     = 0;
	UNCORE_DISCOVERY_UNIT unit_entry     = NULL;
	U64                   unit_ctrl_addr = 0;

	SEP_DRV_LOG_TRACE_IN("");

	max_blocks = (U32)UNCORE_DISCOVERY_GLOBAL_max_blocks(
		&UNCORE_DISCOVERY_TABLE_discovery_global_node(pkg_table));

	for (i = 0; i < max_blocks; i++) {
		unit_entry = &UNCORE_DISCOVERY_TABLE_discovery_unit_node_entry(pkg_table, i);
		if (UNCORE_DISCOVERY_UNIT_unit_type(unit_entry) == DISCOVERY_UNIT_CXLCM ||
		    UNCORE_DISCOVERY_UNIT_unit_type(unit_entry) == DISCOVERY_UNIT_CXLDP) {
			unit_ctrl_addr = UNCORE_DISCOVERY_UNIT_unit_ctrl_addr(unit_entry);
			if (!unit_ctrl_addr) {
				continue;
			}
			unc_common_Disc_Add_Allowlist_Key_Value(unit_ctrl_addr, 0x0);
			unc_common_Disc_Add_Allowlist_Key_Value(
				unit_ctrl_addr,
				UNCORE_DISCOVERY_UNIT_unit_status_offset(unit_entry));
			for (j = 0; j < UNCORE_DISCOVERY_UNIT_num_gp_ctrs(unit_entry); j++) {
				unc_common_Disc_Add_Allowlist_Key_Value(
					unit_ctrl_addr,
					(UNCORE_DISCOVERY_UNIT_evtsel0_offset(unit_entry) + j * 8));
				unc_common_Disc_Add_Allowlist_Key_Value(
					unit_ctrl_addr,
					(UNCORE_DISCOVERY_UNIT_ctr0_offset(unit_entry) + j * 8));
			}
		}
	}
	SEP_DRV_LOG_TRACE_OUT("Success");
}

/*!
 *  @fn          static VOID lwpmudrv_Read_Uncore_Discovery_Tables(VOID)
 *  @brief       Fetch the BAR for Uncore discovery table
 *               Using the BAR read the discovery table and populate it
 *
 *  @param       None
 *
 *  @return      None
 *
 *  <I>Special Notes:</I>
 */
extern VOID
UNC_COMMON_Read_Uncore_Discovery_Tables(void)
{
	SEP_MMIO_NODE                      mmio_node = { 0 };
	UNCORE_DISCOVERY_TABLE             pkg_table = NULL;
	U32                                i         = 0;
	U32                                stride_size, max_blocks;
	static DRV_PCI_DEVICE_ENTRY_NODE   bar_list[MAX_PACKAGES];
	UNCORE_DISCOVERY_DVSEC_CONFIG_NODE config_node;
	UNCORE_DISCOVERY_GLOBAL_NODE       global_node;

	SEP_DRV_LOG_TRACE_IN("");

	SEP_DRV_MEMSET((char *)bar_list, 0, (MAX_PACKAGES * sizeof(DRV_PCI_DEVICE_ENTRY_NODE)));
	config_node = UNCORE_DISCOVERY_dvsec_config_node(&uncore_discovery_tables);
	PCI_FIND_DVSEC_CAPABILITY_BAR(&config_node, bar_list);

	for (i = 0; i < num_packages; i++) {
		if (DRV_PCI_DEVICE_ENTRY_bar_address(&bar_list[i])) {
			pkg_table = &UNCORE_DISCOVERY_table_list_entry(&uncore_discovery_tables, i);
			SEP_DRV_MEMCPY(&UNCORE_DISCOVERY_TABLE_discovery_entry_node(pkg_table),
				       &bar_list[i], sizeof(DRV_PCI_DEVICE_ENTRY_NODE));
			SEP_DRV_MEMSET((char *)&mmio_node, 0, sizeof(SEP_MMIO_NODE));
			PCI_Map_Memory(&mmio_node, DRV_PCI_DEVICE_ENTRY_bar_address(&bar_list[i]),
				       sizeof(UNCORE_DISCOVERY_GLOBAL_NODE));
			global_node = *(UNCORE_DISCOVERY_GLOBAL)((
				char *)(UIOP)SEP_MMIO_NODE_virtual_address(&mmio_node));
			stride_size = (U32)UNCORE_DISCOVERY_GLOBAL_block_stride(&global_node);
			max_blocks  = (U32)UNCORE_DISCOVERY_GLOBAL_max_blocks(&global_node);
			SEP_DRV_MEMCPY(&UNCORE_DISCOVERY_TABLE_discovery_global_node(pkg_table),
				       &global_node, sizeof(UNCORE_DISCOVERY_GLOBAL_NODE));
			PCI_Unmap_Memory(&mmio_node);
			SEP_DRV_LOG_TRACE("stride_size: %u, max_blocks: %u, gbl_ctrl_addr: 0x%llx",
					  stride_size, max_blocks,
					  UNCORE_DISCOVERY_GLOBAL_global_ctrl_addr(
						  &UNCORE_DISCOVERY_TABLE_discovery_global_node(
							  pkg_table)));
			if ((stride_size * 8) != sizeof(UNCORE_DISCOVERY_UNIT_NODE)) {
				SEP_DRV_LOG_WARNING("Discovery unit stride %u unmatched size of UNCORE_DISCOVERY_UNIT_NODE %u",
						    (stride_size * 8),
						    sizeof(UNCORE_DISCOVERY_UNIT_NODE));
				return;
			}
			if ((sizeof(UNCORE_DISCOVERY_UNIT_NODE) * MAX_DISCOVERY_UNCORE_UNITS) <
			    (max_blocks * stride_size * 8)) {
				SEP_DRV_LOG_WARNING("Discovery table size %u greater than allocated memory in kernel %u",
						    (max_blocks * stride_size * 8),
						    sizeof(UNCORE_DISCOVERY_UNIT_NODE) *
							    MAX_DISCOVERY_UNCORE_UNITS);
				return;
			}
			SEP_DRV_MEMSET((char *)&mmio_node, 0, sizeof(SEP_MMIO_NODE));
			PCI_Map_Memory(&mmio_node,
				       DRV_PCI_DEVICE_ENTRY_bar_address(&bar_list[i]) +
					       (8 * stride_size),
				       (max_blocks * stride_size * 8));
			SEP_DRV_MEMCPY(UNCORE_DISCOVERY_TABLE_discovery_unit_node(pkg_table),
				       (char *)(UIOP)SEP_MMIO_NODE_virtual_address(&mmio_node),
				       (max_blocks * stride_size * 8));
			PCI_Unmap_Memory(&mmio_node);
			unc_common_Disc_Add_Allowlist_Entry(pkg_table);

#if defined(MYDEBUG)
			{
				U32 j = 0;
				for (j = 0; j < max_blocks; j++) {
					SEP_DRV_LOG_TRACE("%u:type:%u,id:%u,ad:0x%llx", j,
							  UNCORE_DISCOVERY_UNIT_unit_type(
								  &UNCORE_DISCOVERY_TABLE_discovery_unit_node_entry(
									  pkg_table, j)),
							  UNCORE_DISCOVERY_UNIT_unit_id(
								  &UNCORE_DISCOVERY_TABLE_discovery_unit_node_entry(
									  pkg_table, j)),
							  UNCORE_DISCOVERY_UNIT_unit_ctrl_addr(
								  &UNCORE_DISCOVERY_TABLE_discovery_unit_node_entry(
									  pkg_table, j)));
				}
			}
#endif
		}
	}
	SEP_DRV_LOG_TRACE_OUT("");
	return;
}

/************************************************************/
/*
 * UNC common MSR  based API
 *
 ************************************************************/

/*!
 * @fn         VOID UNC_COMMON_MSR_Clean_Up(PVOID)
 *
 * @brief      clear out out programming
 *
 * @param      None
 *
 * @return     None
 */
VOID
UNC_COMMON_MSR_Clean_Up(VOID *param)
{
	U32 dev_idx;

	SEP_DRV_LOG_TRACE_IN("Param: %p.", param);
	dev_idx = *((U32 *)param);
	FOR_EACH_REG_ENTRY_UNC (pecb, dev_idx, i) {
		if (ECB_entries_clean_up_get(pecb, i)) {
			SYS_Write_MSR(ECB_entries_reg_id(pecb, i), 0LL);
		}
	}
	END_FOR_EACH_REG_ENTRY_UNC;

	SEP_DRV_LOG_TRACE_OUT("");
	return;
}

