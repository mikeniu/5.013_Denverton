/** @file
  Header file for CpuPlatform Lib.

@copyright
  Copyright (c) 2012 - 2016 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by the
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
  This file contains an 'Intel Peripheral Driver' and is uniquely
  identified as "Intel Reference Module" and is licensed for Intel
  CPUs and chipsets under the terms of your license agreement with
  Intel or your vendor. This file may be modified by the user, subject
  to additional terms of the license agreement.

@par Specification Reference:
**/

#ifndef _CPU_PLATFORM_LIB_H_
#define _CPU_PLATFORM_LIB_H_

// EFI_AP_PROCEDURE is defined in PiDxeCis.h and it is the one that is being used.
#include <Pi/PiDxeCis.h>
#include <Protocol/MpService.h>

/**
  This function is to program Trace Hub ACPI base address to processor's MSR TRACE_HUB_STH_ACPIBAR_BASE.

  @param[in]  TraceHubAcpiBase - Base address of Trace Hub ACPI Base address
**/
VOID
EFIAPI
CpuWriteTraceHubAcpiBase (
  IN UINT64  TraceHubAcpiBase
  );
#endif
