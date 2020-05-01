/* SPDX-License-Identifier: GPL-2.0 AND BSD-3-Clause
 *
 * This file is provided under a dual BSD/GPLv2 license.  When using or
 * redistributing this file, you may do so under either license.
 *
 * GPL LICENSE SUMMARY
 *
 * Copyright(c) 2020 Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * Contact Information:
 * SoC Watch Developer Team <socwatchdevelopers@intel.com>
 * Intel Corporation,
 * 1300 S Mopac Expwy,
 * Austin, TX 78746
 *
 * BSD LICENSE
 *
 * Copyright(c) 2020 Intel Corporation.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in
 *     the documentation and/or other materials provided with the
 *     distribution.
 *   * Neither the name of Intel Corporation nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <linux/pci.h> /* struct pci_dev */

#include "sw_kernel_defines.h" /* pw_pr_force() */
#include "sw_structs.h" /* sw_pci_dev_msg, sw_pci_dev_info */
#include "sw_pci.h"

static struct sw_pci_dev_msg s_pci_dev_list;

void sw_print_pci_devices_i(void) {
	int i = 0;

	for(i = 0; i < s_pci_dev_list.num_entries; ++i) {
		pw_pr_debug("bus: %x, dev: %x, func: %x, vendor: %x, device: %x, class: %x, header: %x\n",
				s_pci_dev_list.info[i].bus, s_pci_dev_list.info[i].device, s_pci_dev_list.info[i].function,
				s_pci_dev_list.info[i].vendorID, s_pci_dev_list.info[i].deviceID, s_pci_dev_list.info[i].classID,
				s_pci_dev_list.info[i].headerType);
	}
}

void sw_pci_enumerate_devices(void)
{
	struct pci_dev *dev = NULL;
	s_pci_dev_list.num_entries = 0;

	for_each_pci_dev(dev) {
		if (s_pci_dev_list.num_entries < MAX_PCI_DEVICES) {
			struct sw_pci_dev_info *pci_dev_info =
				&(s_pci_dev_list.info[s_pci_dev_list.num_entries++]);

			pci_dev_info->bus = dev->bus->number;
			pci_dev_info->device = PCI_SLOT(dev->devfn);
			pci_dev_info->function = PCI_FUNC(dev->devfn);
			pci_dev_info->vendorID = dev->vendor;
			pci_dev_info->deviceID = dev->device;
			pci_dev_info->classID = dev->class;
			pci_dev_info->headerType = dev->hdr_type;
		}
	}

	sw_print_pci_devices_i();
}

struct sw_pci_dev_msg const *sw_pci_dev_list(void)
{
	return &s_pci_dev_list;
}
