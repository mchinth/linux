#
# Version: 1.6
#
# Copyright (C) 2008 Intel Corporation.  All Rights Reserved.
#
#     This file is part of SEP Development Kit
#
#     SEP Development Kit is free software; you can redistribute it
#     and/or modify it under the terms of the GNU General Public License
#     version 2 as published by the Free Software Foundation.
#
#     SEP Development Kit is distributed in the hope that it will be useful,
#     but WITHOUT ANY WARRANTY; without even the implied warranty of
#     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#     GNU General Public License for more details.
#
#     As a special exception, you may use this file as part of a free software
#     library without restriction.  Specifically, if other files instantiate
#     templates or use macros or inline functions from this file, or you compile
#     this file and link it with other files to produce an executable, this
#     file does not by itself cause the resulting executable to be covered by
#     the GNU General Public License.  This exception does not however
#     invalidate any other reasons why the executable file might be covered by
#     the GNU General Public License.
#

# -------------------- user configurable options ------------------------

# base name of SEP driver
DRIVER_NAME = "sep5"

ccflags-y := -I$(srctree)/drivers/platform/x86/sepdk/include -I$(srctree)/drivers/platform/x86/sepdk/inc -I$(srctree)/drivers/platform/x86/sepdk

ccflags-y  += -DSEP_CONFIG_MODULE_LAYOUT

ifeq ($(shell test -e ../../../../../include/linux/kaiser.h && echo -n YES),YES)
    ccflags-y += -DKAISER_HEADER_PRESENT
endif

obj-m := $(DRIVER_NAME).o

ifdef CONFIG_X86_64
	arch-objs :=    core2.o                \
			perfver4.o             \
			sys64.o                \
			silvermont.o           \
			pci.o                  \
			apic.o                 \
			pebs.o                 \
			ipt.o                  \
			unc_gt.o               \
			unc_mmio.o             \
			unc_msr.o              \
			unc_common.o           \
			unc_pci.o              \
			sepdrv_p_state.o
else
	arch-objs :=    core2.o                \
			perfver4.o             \
			sys32.o                \
			silvermont.o           \
			pci.o                  \
			apic.o                 \
			pebs.o                 \
			ipt.o                  \
			unc_gt.o               \
			unc_mmio.o             \
			unc_msr.o              \
			unc_common.o           \
			unc_pci.o              \
			sepdrv_p_state.o
endif

ifdef CONFIG_SEP_PER_USER_MODE
    ccflags-y += -DSECURE_SEP
endif

ccflags-y += -DDRV_CPU_HOTPLUG -DDRV_USE_TASKLET_WORKAROUND -DDRV_SAFE_MSR -DDRV_USE_RDPMC

ifdef CONFIG_SEP_BUILD_PMT
    ccflags-y += -DDRV_PMT_ENABLE
    pmt-objs := unc_pmt.o
endif


ifdef CONFIG_SEP_MINLOG_MODE
    ccflags-y += -DDRV_MINIMAL_LOGGING
else
    ifdef CONFIG_SEP_MAXLOG_MODE
        ccflags-y += -DDRV_MAXIMAL_LOGGING
    endif
endif


$(DRIVER_NAME)-y :=               \
		lwpmudrv.o        \
		control.o         \
		cpumon.o          \
		eventmux.o        \
		linuxos.o         \
		output.o          \
		pmi.o             \
		sys_info.o        \
		utility.o         \
		valleyview_sochap.o    \
		unc_power.o       \
		pmu_list.o        \
		unc_sa.o          \
		$(pmt-objs)       \
		$(arch-objs)


