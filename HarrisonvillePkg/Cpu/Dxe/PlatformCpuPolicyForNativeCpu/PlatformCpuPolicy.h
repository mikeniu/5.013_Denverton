//
// This file contains an 'Intel Pre-EFI Module' and is licensed
// for Intel CPUs and Chipsets under the terms of your license 
// agreement with Intel or your vendor.  This file may be      
// modified by the user, subject to additional terms of the    
// license agreement                                           
//
/*++

Copyright (c)  1999 - 2015 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  PlatformCpuPolicy.h

Abstract:

  Header file for common server processor policy driver.

++*/

// GC_TODO: fix comment to end with --*/
//
// GC_TODO: fix comment to end with --*/
//
// GC_TODO: fix comment to end with --*/
//
#ifndef _PLATFORM_CPU_POLICY_H_
#define _PLATFORM_CPU_POLICY_H_

#include <PiDxe.h>
#include <Protocol/PlatformCpu.h>
#include <Protocol/PlatformType.h>
#include <Protocol/IntelCpuPcdsSetDone.h>
#include <SetupVariable.h>
#include <Library/BaseLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PcdLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/CpuConfigLib.h>
#include <PchAccess.h>
#include <Library/PchSbiAccessLib.h>
#include <Platform.h>
#include <Library/MmPciLib.h>
#include <Library/IoLib.h>

//
// Energy Performance Bias for External BIOS
//
#define EPB_DEFAULT                4

//
// Driver data signature
//
#define PLATFORM_CPU_POLICY_SIGNATURE SIGNATURE_32 ('P', 'C', 'P', 'D')

//
// Containment record macro
//
#define PLATFORM_CPU_POLICY_INSTANCE_FROM_EFI_PLATFORM_CPU_PROTOCOL_THIS(a) \
  CR ( \
  a, \
  PLATFORM_CPU_POLICY_INSTANCE, \
  PlatformCpu, \
  PLATFORM_CPU_POLICY_SIGNATURE \
  )

//
// Driver entry point
//
EFI_STATUS
PlatformCpuPolicyEntryPoint (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
/*++
  
Routine Description:

  This is the EFI driver entry point for the CpuPolicy Driver. This
  driver is responsible for getting microcode patches from FV.

Arguments:

  ImageHandle     - Handle for the image of this driver.
  SystemTable     - Pointer to the EFI System Table.

Returns:

  EFI_SUCCESS     - Protocol installed sucessfully.

--*/
;

#endif
