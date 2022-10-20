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









#ifndef _PCI_H_
#define _PCI_H_

#if defined(DRV_PMT_ENABLE)
#include <linux/compiler.h> /* Definition of __weak */
#include <linux/kref.h>     /* struct kref */
#include <linux/notifier.h> /* struct notifier_block */
#include <linux/pci.h>      /* struct pci_dev */
#include <linux/ioport.h>   /* struct resource */
#endif

#include "lwpmudrv_defines.h"

/*
 * PCI Config Address macros
 */
#define PCI_ENABLE  0x80000000

#define PCI_ADDR_IO 0xCF8
#define PCI_DATA_IO 0xCFC

#define BIT0        0x1
#define BIT1        0x2

/*
 * Macro for forming a PCI configuration address
 */
#define FORM_PCI_ADDR(bus, dev, fun, off)                               \
	(((PCI_ENABLE)) | ((bus & 0xFF) << 16) | ((dev & 0x1F) << 11) | \
	 ((fun & 0x07) << 8) | ((off & 0xFF) << 0))

#define VENDOR_ID_MASK             0x0000FFFF
#define DEVICE_ID_MASK             0xFFFF0000
#define DEVICE_ID_BITSHIFT         16
#define LOWER_4_BYTES_MASK         0x00000000FFFFFFFF
#define MAX_BUSNO                  256
#define NEXT_ADDR_OFFSET           4
#define NEXT_ADDR_SHIFT            32
#define DRV_IS_PCI_VENDOR_ID_INTEL 0x8086
#define MAX_PCI_DEVS               32

#define CONTINUE_IF_NOT_GENUINE_INTEL_DEVICE(value, vendor_id, device_id)      \
	{                                                                      \
		vendor_id = value & VENDOR_ID_MASK;                            \
		device_id = (value & DEVICE_ID_MASK) >> DEVICE_ID_BITSHIFT;    \
									       \
		if (vendor_id != DRV_IS_PCI_VENDOR_ID_INTEL) {                 \
			continue;                                              \
		}                                                              \
	}

#define CHECK_IF_GENUINE_INTEL_DEVICE(value, vendor_id, device_id, valid)      \
	{                                                                      \
		vendor_id = value & VENDOR_ID_MASK;                            \
		device_id = (value & DEVICE_ID_MASK) >> DEVICE_ID_BITSHIFT;    \
									       \
		valid = 1;                                                     \
		if (vendor_id != DRV_IS_PCI_VENDOR_ID_INTEL) {                 \
			valid = 0;                                             \
		}                                                              \
	}

#define PCI_FIND_DVSEC_CAPABILITY_BAR(config, bar_list) \
	PCI_Find_DVSEC_Capability_BAR(config, bar_list)

typedef struct SEP_MMIO_NODE_S SEP_MMIO_NODE;

struct SEP_MMIO_NODE_S {
	U64 physical_address;
	U64 virtual_address;
	U64 map_token;
	U32 size;
};

#define SEP_MMIO_NODE_physical_address(x) ((x)->physical_address)
#define SEP_MMIO_NODE_virtual_address(x)  ((x)->virtual_address)
#define SEP_MMIO_NODE_map_token(x)        ((x)->map_token)
#define SEP_MMIO_NODE_size(x)             ((x)->size)

#if defined(DRV_PMT_ENABLE)
/*
 * Struct definitions taken from PMT driver.
 */

struct telem_header {
	u8  access_type;
	u8  telem_type;
	u16 size;
	u32 guid;
	u32 base_offset;
};

struct telem_endpoint {
	struct pci_dev     *parent;
	struct telem_header header;
	void __iomem       *base;
	struct resource     res;
	bool                present;
	struct kref         kref;
};

struct telem_endpoint_info {
	struct pci_dev     *pdev;
	struct telem_header header;
};

#define PMT_TELEM_NOTIFY_ADD    0
#define PMT_TELEM_NOTIFY_REMOVE 1

/*
 * Weak linkage of functions from the PMT driver
 */

/**
 * pmt_telem_get_next_endpoint() - Get next device id for a telemetry endpoint
 * @start:  starting devid to look from
 *
 * This functions can be used in a while loop predicate to retrieve the devid
 * of all available telemetry endpoints. Functions pmt_telem_get_next_endpoint()
 * and pmt_telem_register_endpoint() can be used inside of the loop to examine
 * endpoint info and register to receive a pointer to the endpoint. The pointer
 * is then usable in the telemetry read calls to access the telemetry data.
 *
 * Return:
 * * devid       - devid of the next present endpoint from start
 * * 0           - when no more endpoints are present after start
 */
extern int __weak
pmt_telem_get_next_endpoint(int start);

/**
 * pmt_telem_register_endpoint() - Register a telemetry endpoint
 * @devid: device id/handle of the telemetry endpoint
 *
 * Increments the kref usage counter for the endpoint.
 *
 * Return:
 * * endpoint    - On success returns pointer to the telemetry endpoint
 * * -ENXIO      - telemetry endpoint not found
 */
extern struct telem_endpoint *__weak
pmt_telem_register_endpoint(int devid);

/**
 * pmt_telem_unregister_endpoint() - Unregister a telemetry endpoint
 * @ep:   ep structure to populate.
 *
 * Decrements the kref usage counter for the endpoint.
 */
extern void __weak
pmt_telem_unregister_endpoint(struct telem_endpoint *ep);

/**
 * pmt_telem_get_endpoint_info() - Get info for an endpoint from its devid
 * @devid:  device id/handle of the telemetry endpoint
 * @info:   Endpoint info structure to be populated
 *
 * Return:
 * * 0           - Success
 * * -ENXIO      - telemetry endpoint not found for the devid
 * * -EINVAL     - @info is NULL
 */
extern int __weak
pmt_telem_get_endpoint_info(int devid, struct telem_endpoint_info *info);

/**
 * pmt_telem_read() - Read qwords from telemetry sram
 * @ep:     Telemetry endpoint to be read
 * @offset: Register offset in bytes
 * @data:   Allocated qword buffer
 * @count:  Number of qwords requested
 *
 * Callers must ensure reads are aligned. When the call returns -ENODEV,
 * the device has been removed and callers should unregister the telemetry
 * endpoint.
 *
 * Return:
 * * 0           - Success
 * * -ENODEV     - The device is not present.
 * * -EINVAL     - The offset is out out bounds
 * * -EPIPE      - The device was removed during the read. Data written
 *                 but should be considered not valid.
 */
extern int __weak
pmt_telem_read(struct telem_endpoint *ep, u32 offset, u64 *data, u32 count);

/**
 * pmt_telem_register_notifier() - Receive notification endpoint events
 * @nb:   Notifier block
 *
 * Events:
 *   PMT_TELEM_NOTIFY_ADD   - An endpoint has been added. Notifier data
 *                            is the devid
 *   PMT_TELEM_NOTIF_REMOVE - An endpoint has been removed. Notifier data
 *                            is the devid
 */
extern int __weak
pmt_telem_register_notifier(struct notifier_block *nb);

/**
 * pmt_telem_unregister_notifier() - Unregister notification of endpoint events
 * @nb:   Notifier block
 *
 */
extern int __weak
pmt_telem_unregister_notifier(struct notifier_block *nb);

#endif

extern OS_STATUS
PCI_Map_Memory(SEP_MMIO_NODE *node, U64 phy_address, U32 map_size);

extern void PCI_Unmap_Memory(SEP_MMIO_NODE *node);

extern int PCI_Read_From_Memory_Address(U32 addr, U32 *val);

extern int PCI_Write_To_Memory_Address(U32 addr, U32 val);

extern int PCI_Read_From_Memory_Address_U64(U64 addr, U64* val);

extern int PCI_Write_To_Memory_Address_U64(U64 addr, U64 val);

/*** UNIVERSAL PCI ACCESSORS ***/

extern VOID PCI_Initialize(VOID);

extern VOID PCI_Cleanup(VOID);

extern U32
PCI_Read_U32(U32 domain, U32 bus, U32 device, U32 function, U32 offset);

extern U32
PCI_Read_U32_Valid(U32 domain,
		   U32 bus,
		   U32 device,
		   U32 function,
		   U32 offset,
		   U32 invalid_value);

extern U64
PCI_Read_U64(U32 domain, U32 bus, U32 device, U32 function, U32 offset);

extern U64
PCI_Read_U64_Valid(U32 domain,
		   U32 bus,
		   U32 device,
		   U32 function,
		   U32 offset,
		   U64 invalid_value);

extern U32
PCI_Write_U32(U32 domain,
	      U32 bus,
	      U32 device,
	      U32 function,
	      U32 offset,
	      U32 value);

extern U32
PCI_Write_U64(U32 domain,
	      U32 bus,
	      U32 device,
	      U32 function,
	      U32 offset,
	      U64 value);

extern VOID
PCI_Find_DVSEC_Capability_BAR(UNCORE_DISCOVERY_DVSEC_CONFIG config,
			      DRV_PCI_DEVICE_ENTRY          bar_list);

/*** UNIVERSAL MMIO ACCESSORS ***/

extern U32 PCI_MMIO_Read_U32(U64 virtual_address_base, U32 offset);

extern U64 PCI_MMIO_Read_U64(U64 virtual_address_base, U32 offset);

extern void
PCI_MMIO_Write_U32(U64 virtual_address_base, U32 offset, U32 value);

extern void
PCI_MMIO_Write_U64(U64 virtual_address_base, U32 offset, U64 value);

#if defined(DRV_PMT_ENABLE)
extern U32 PCI_PMT_Read_U32(struct telem_endpoint *telem_ep, U32 offset);

extern U64 PCI_PMT_Read_U64(struct telem_endpoint *telem_ep, U32 offset);
#endif

#endif

