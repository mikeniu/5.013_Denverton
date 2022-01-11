/** @file
  DataTypes.h
  This file include all the external data types.
 
  Copyright (c) 1999 - 2015 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

**/

#ifndef _DATATYPES_H_
#define _DATATYPES_H_

#pragma warning (disable : 4214 )

#ifndef EFI_MEMORY_INIT
#include "Bind.h"
typedef uint8_t     BOOLEAN;
#ifndef MRC_DATA_REQUIRED_FROM_OUTSIDE
#ifndef UINTN
typedef uintn_t     UINTN;
#endif
#endif

#if SIM
typedef my_int8_t   INT8;
#else
typedef int8_t      INT8;
#endif
typedef uint8_t     UINT8;
typedef int16_t     INT16;
typedef uint16_t    UINT16;
#ifndef INT32
typedef int         INT32;
#endif
#ifndef UINT32
typedef uint32_t    UINT32;
#endif
typedef long long INT64;
typedef unsigned long long UINT64;
typedef uint8_t     CHAR8;
typedef uint16_t    CHAR16;
#endif

//
// Modifiers to abstract standard types to aid in debug of problems
//
#ifndef CONST
#define CONST       const
#endif

#ifndef STATIC
#define STATIC      static
#endif

#ifndef VOID
#define VOID        void
#endif

#ifndef VOLATILE
#define VOLATILE    volatile
#endif
//
// Constants. They may exist in other build structures, so #ifndef them.
//
#ifndef IN
#define             IN
#endif

#ifndef OUT
#define             OUT
#endif

#ifndef TRUE
#define TRUE        ((BOOLEAN) 1 == 1)
#endif

#ifndef FALSE
#define FALSE       ((BOOLEAN) 0 == 1)
#endif

#ifndef NULL
#define NULL        ((VOID *) 0)
#endif

typedef UINT32           MMRC_STATUS;
#define MMRC_SUCCESS     0
#define MMRC_FAILURE     0xFFFFFFFF

#endif // DATATYPES_H

