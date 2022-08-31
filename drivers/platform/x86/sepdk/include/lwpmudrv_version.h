/*
 * Copyright (C) 2010 Intel Corporation
 *
 * This software and the related documents are Intel copyrighted materials, and your use of them
 * is governed by the express license under which they were provided to you ("License"). Unless
 * the License provides otherwise, you may not use, modify, copy, publish, distribute, disclose
 * or transmit this software or the related documents without Intel's prior written permission.
 *
 * This software and the related documents are provided as is, with no express or implied
 * warranties, other than those that are expressly stated in the License.
*/

/*
 *  File  : lwpmudrv_version.h
 */

#ifndef _LWPMUDRV_VERSION_H_
#define _LWPMUDRV_VERSION_H_

#define _STRINGIFY(x)   #x
#define STRINGIFY(x)    _STRINGIFY(x)
#define _STRINGIFY_W(x) L#x
#define STRINGIFY_W(x)  _STRINGIFY_W(x)

#define SEP_MAJOR_VERSION       5
#define SEP_MINOR_VERSION       37
// API version is independent of major/minor and tracks driver version
#define SEP_API_VERSION         12

#define SEP_PREV_MAJOR_VERSION  5
#define EMON_PREV_MAJOR_VERSION 11

#if defined(BUILD_SCOPE_RELEASE)
#define SEP_RELEASE_STRING ""
#else
#define SEP_RELEASE_STRING "Beta"
#endif

#define EMON_MAJOR_VERSION          11
#define EMON_MINOR_VERSION          SEP_MINOR_VERSION
#define EMON_PRODUCT_RELEASE_STRING SEP_RELEASE_STRING

#if defined(SEP_ENABLE_PRIVATE_CPUS)
#define PRODUCT_TYPE "private"
#define SEP_NAME     "sepint"
#define SEP_NAME_W   L"sepint"
#else
#if defined(SEP_ENABLE_NDA_CPUS)
#define PRODUCT_TYPE "NDA"
#else
#define PRODUCT_TYPE "public"
#endif
#define SEP_NAME   "sep"
#define SEP_NAME_W L"sep"
#endif

#if !defined(PRODUCT_BUILDER)
#define PRODUCT_BUILDER unknown
#endif

#define TB_FILE_EXT   ".tb7"
#define TB_FILE_EXT_W L".tb7"

#define IPT_FILE_EXT   ".ipt"
#define IPT_FILE_EXT_W L".ipt"

#define IPT_INFO_FILE_EXT   ".pif"
#define IPT_INFO_FILE_EXT_W L".pif"

#define SEP_PRODUCT_NAME  "Sampling Enabling Product"
#define EMON_PRODUCT_NAME "EMON"

#define PRODUCT_VERSION_DATE __DATE__ " at " __TIME__

#define SEP_PRODUCT_COPYRIGHT \
	"Copyright(C) 2007-2020 Intel Corporation. All rights reserved."
#define EMON_PRODUCT_COPYRIGHT \
	"Copyright(C) 1993-2020 Intel Corporation. All rights reserved."

#define PRODUCT_DISCLAIMER \
	"Warning: This computer program is protected under U.S. and international\ncopyright laws, and may only be used or copied in accordance with the terms\nof the license agreement.  Except as permitted by such license, no part\nof this computer program may be reproduced, stored in a retrieval system,\nor transmitted in any form or by any means without the express written consent\nof Intel Corporation."
#define PRODUCT_VERSION \
	STRINGIFY(SEP_MAJOR_VERSION) "." STRINGIFY(SEP_MINOR_VERSION)

#define SEP_MSG_PREFIX                                       \
	SEP_NAME STRINGIFY(SEP_MAJOR_VERSION) "_" STRINGIFY( \
		SEP_MINOR_VERSION) ":"
#define SEP_VERSION_STR \
	STRINGIFY(SEP_MAJOR_VERSION) "." STRINGIFY(SEP_MINOR_VERSION)

#if defined(DRV_OS_WINDOWS)

#define SEP_DRIVER_NAME   SEP_NAME "drv" STRINGIFY(SEP_MAJOR_VERSION)
#define SEP_DRIVER_NAME_W SEP_NAME_W L"drv" STRINGIFY_W(SEP_MAJOR_VERSION)
#define SEP_DEVICE_NAME   SEP_DRIVER_NAME

#define SEP_PREV_DRIVER_NAME SEP_NAME "drv" STRINGIFY(SEP_PREV_MAJOR_VERSION)
#define SEP_PREV_DRIVER_NAME_W \
	SEP_NAME_W L"drv" STRINGIFY_W(SEP_PREV_MAJOR_VERSION)
#define SEP_PREV_DEVICE_NAME SEP_PREV_DRIVER_NAME

#endif

#if defined(DRV_OS_LINUX) || defined(DRV_OS_SOLARIS) || \
	defined(DRV_OS_ANDROID) || defined(DRV_OS_FREEBSD)

#define SEP_DRIVER_NAME   SEP_NAME STRINGIFY(SEP_MAJOR_VERSION)
#define SEP_SAMPLES_NAME  SEP_DRIVER_NAME "_s"
#define SEP_UNCORE_NAME   SEP_DRIVER_NAME "_u"
#define SEP_SIDEBAND_NAME SEP_DRIVER_NAME "_b"
#define SEP_EMON_NAME     SEP_DRIVER_NAME "_e"
#define SEP_IPT_NAME      SEP_DRIVER_NAME "_pt"
#define SEP_IPT_INFO_NAME SEP_DRIVER_NAME "_ptinfo"
#define SEP_DEVICE_NAME   "/dev/" SEP_DRIVER_NAME

#define SEP_PREV_DRIVER_NAME   SEP_NAME STRINGIFY(SEP_PREV_MAJOR_VERSION)
#define SEP_PREV_SAMPLES_NAME  SEP_PREV_DRIVER_NAME "_s"
#define SEP_PREV_UNCORE_NAME   SEP_PREV_DRIVER_NAME "_u"
#define SEP_PREV_SIDEBAND_NAME SEP_PREV_DRIVER_NAME "_b"
#define SEP_PREV_DEVICE_NAME   "/dev/" SEP_PREV_DRIVER_NAME

#endif

#if defined(DRV_OS_MAC)

#define SEP_DRIVER_NAME  SEP_NAME STRINGIFY(SEP_MAJOR_VERSION)
#define SEP_SAMPLES_NAME SEP_DRIVER_NAME "_s"
#define SEP_DEVICE_NAME  SEP_DRIVER_NAME

#define SEP_PREV_DRIVER_NAME  SEP_NAME STRINGIFY(SEP_PREV_MAJOR_VERSION)
#define SEP_PREV_SAMPLES_NAME SEP_PREV_DRIVER_NAME "_s"
#define SEP_PREV_DEVICE_NAME  SEP_PREV_DRIVER_NAME

#endif

#endif
