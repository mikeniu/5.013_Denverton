/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//
Copyright (c) 2009-2014 Intel Corporation.  All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license.  Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

Module Name:
    WheaPlatformHooks.h
   
Abstract:
    Whea Platform hook functions and platform data, which needs to be 
    ported.

-----------------------------------------------------------------------------*/


#ifndef _WHEA_PLATFORM_HOOKS_H_
#define _WHEA_PLATFORM_HOOKS_H_

#include <PiDxe.h>

#include "WheaPlatformDefs.h"

#include <Protocol\SmmBase2.h>
#include <Protocol\WheaPlatformSupport.h>

#include <Library\BaseLib.h>

// Data & Tables defined in WheaPlatform hooks

extern SYSTEM_GENERIC_ERROR_SOURCE      SysGenErrSources[];
extern UINTN                            NumSysGenericErrorSources;
extern SYSTEM_NATIVE_ERROR_SOURCE       SysNativeErrSources[];
extern UINTN                            NumNativeErrorSources;


#define MCI_REG_BASE    0x400
#define R_PCH_LPC_ACPI_BASE                       0x40
#define PCI_FUNCTION_NUMBER_PCH_LPC               0
#define PCI_DEVICE_NUMBER_PCH_LPC                 31
#define DEFAULT_PCI_BUS_NUMBER_PCH                0
#define R_PCH_LPC_GPIO_BASE                       0x48
#define R_PCH_GPIO_GPI_INV                        0x2C
#define R_PCH_ACPI_GPE0a_STS                      0x20
#define R_PCH_ALT_GP_SMI_EN                       0x38
#define R_PCH_ACPI_GPE0a_EN                       0x28
#define R_PCH_LPC_GPI_ROUT                        0xB8

 


//
// This is copied from Include\Acpi.h
//
#define CREATOR_ID_INTEL 0x4C544E49  //"LTNI""INTL"(Intel)
#define CREATOR_REV_INTEL 0x20090903

//
// Update Platform & OEM ID's in Whea ACPI tables.
//
VOID
UpdateAcpiTableIds(
  EFI_ACPI_DESCRIPTION_HEADER     *TableHeader
  );

//
// Memory device location informaton
//
EFI_STATUS
WheaGetMemoryFruInfo (
  IN MEMORY_DEV_INFO    *MemInfo,
  OUT EFI_GUID          *FruId,
  OUT CHAR8             *FruString,
  OUT UINT16            *Card 
  );

EFI_STATUS
ProgramWheaSignalsHook (
  BOOLEAN                       EnableWhea,
  EFI_SMM_BASE2_PROTOCOL       *SmmBase,
  EFI_SMM_SYSTEM_TABLE2        *Smst
  );

EFI_STATUS
TriggerWheaSci (
  EFI_SMM_BASE2_PROTOCOL         *SmmBase,
  EFI_SMM_SYSTEM_TABLE2         *Smst
  );

EFI_STATUS
TriggerWheaNmi (
  EFI_SMM_BASE2_PROTOCOL         *SmmBase,
  EFI_SMM_SYSTEM_TABLE2          *Smst
  );

EFI_STATUS
InitWheaPlatformHookLib (
  void
  );

#endif  //_WHEA_PLATFORM_HOOKS_H_

