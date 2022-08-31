/*
 * Copyright (C) 2007 Intel Corporation
 *
 * This software and the related documents are Intel copyrighted materials, and your use of them
 * is governed by the express license under which they were provided to you ("License"). Unless
 * the License provides otherwise, you may not use, modify, copy, publish, distribute, disclose
 * or transmit this software or the related documents without Intel's prior written permission.
 *
 * This software and the related documents are provided as is, with no express or implied
 * warranties, other than those that are expressly stated in the License.
*/

#ifndef _LWPMUDRV_TYPES_H_
#define _LWPMUDRV_TYPES_H_

#if defined(__cplusplus)
extern "C" {
#endif

#if defined(BUILD_DRV_ESX)
//SR: added size_t def
typedef unsigned long size_t;
typedef unsigned long ssize_t;
#endif

typedef unsigned char  U8;
typedef char           S8;
typedef short          S16;
typedef unsigned short U16;
typedef unsigned int   U32;
typedef int            S32;
#if defined(DRV_OS_WINDOWS)
typedef unsigned __int64 U64;
typedef __int64          S64;
#elif defined(DRV_OS_LINUX) || defined(DRV_OS_SOLARIS) ||                      \
	defined(DRV_OS_MAC) || defined(DRV_OS_ANDROID) ||                      \
	defined(DRV_OS_FREEBSD)
typedef unsigned long long U64;
typedef long long          S64;
typedef unsigned long      ULONG;
typedef void               VOID;
typedef void *             LPVOID;

#if defined(BUILD_DRV_ESX)
//SR: added UWORD64 def
typedef union _UWORD64 {
	struct {
		U32 low;
		S32 hi;
	} c;
	S64 qword;
} UWORD64, *PWORD64;
#endif
#else
#error "Undefined OS"
#endif

#if defined(DRV_IA32)
typedef S32 SIOP;
typedef U32 UIOP;
#elif defined(DRV_EM64T)
typedef S64  SIOP;
typedef U64  UIOP;
#else
#error "Unexpected Architecture seen"
#endif

typedef U32   DRV_BOOL;
typedef void *PVOID;

#if !defined(__DEFINE_STCHAR__)
#define __DEFINE_STCHAR__
#if defined(UNICODE)
typedef wchar_t STCHAR;
#define VTSA_T(x) L##x
#else
typedef char STCHAR;
#define VTSA_T(x) x
#endif
#endif

#if defined(DRV_OS_WINDOWS)
#include <wchar.h>
typedef wchar_t DRV_STCHAR;
typedef wchar_t VTSA_CHAR;
#else
typedef char DRV_STCHAR;
#endif

//
// Handy Defines
//
typedef U32 DRV_STATUS;

#define MAX_STRING_LENGTH      1024
#define MAXNAMELEN             256
#define MAX_PATH_BUFFER_LENGTH 1100
#define MAX_PATH_USER_LENGTH   1024

#if defined(DRV_OS_WINDOWS)
#define WIN_MAX_RELATIVE_PATH_LENGTH  200
#define WIN_VOLUME_LABEL_INDICATOR    L":\\"
#define WIN_PATH_LENGTH_EXTEND_PREFIX L"\\\\?\\"
#endif

#if defined(DRV_OS_WINDOWS)
#define UNLINK _unlink
#define RENAME rename
#define WCSDUP _wcsdup
#endif
#if defined(DRV_OS_LINUX) || defined(DRV_OS_SOLARIS) || defined(DRV_OS_MAC) || \
	defined(DRV_OS_ANDROID) || defined(DRV_OS_FREEBSD)
#define UNLINK unlink
#define RENAME rename
#endif

#if defined(DRV_OS_SOLARIS) && !defined(_KERNEL)
//wcsdup is missing on Solaris
#include <stdlib.h>
#include <wchar.h>

static inline wchar_t *
solaris_wcsdup(const wchar_t *wc)
{
	wchar_t *tmp = (wchar_t *)malloc((wcslen(wc) + 1) * sizeof(wchar_t));
	wcscpy(tmp, wc);
	return tmp;
}
#define WCSDUP solaris_wcsdup
#endif

#if defined(DRV_OS_LINUX) || defined(DRV_OS_FREEBSD) || defined(DRV_OS_MAC)
#define WCSDUP wcsdup
#endif

#if !defined(_WCHAR_T_DEFINED)
#if defined(DRV_OS_LINUX) || defined(DRV_OS_ANDROID) || defined(DRV_OS_SOLARIS)
#if !defined(_GNU_SOURCE)
#define _GNU_SOURCE
#endif
#endif
#endif

#if (defined(DRV_OS_LINUX) || defined(DRV_OS_ANDROID)) && !defined(__KERNEL__)
#include <wchar.h>
typedef wchar_t VTSA_CHAR;
#endif

#if (defined(DRV_OS_MAC) || defined(DRV_OS_FREEBSD) ||                         \
     defined(DRV_OS_SOLARIS)) &&                                               \
	!defined(_KERNEL)
#include <wchar.h>
typedef wchar_t VTSA_CHAR;
#endif

#define TRUE  1
#define FALSE 0

#define ALIGN_4(x)  (((x) + 3) & ~3)
#define ALIGN_8(x)  (((x) + 7) & ~7)
#define ALIGN_16(x) (((x) + 15) & ~15)
#define ALIGN_32(x) (((x) + 31) & ~31)

#if defined(__cplusplus)
}
#endif

#endif
