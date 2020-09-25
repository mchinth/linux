/* ********************************************************************************
 # INTEL CONFIDENTIAL
 # Copyright 2013 - 2020 Intel Corporation.

 # This software and the related documents are Intel copyrighted materials, and
 # your use of them is governed by the express license under which they were
 # provided to you (License). Unless the License provides otherwise, you may not
 # use, modify, copy, publish, distribute, disclose or transmit this software or
 # the related documents without Intel's prior written permission.

 # This software and the related documents are provided as is, with no express or
 # implied warranties, other than those that are expressly stated in the License.
 # ********************************************************************************/

#ifndef __SW_PCI_H__
#define __SW_PCI_H__

void sw_pci_enumerate_devices(void);
struct sw_pci_dev_msg const *sw_pci_dev_list(void);

#endif // __SW_PCI_H__
