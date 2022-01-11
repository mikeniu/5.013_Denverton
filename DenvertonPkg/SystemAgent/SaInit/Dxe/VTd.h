/** @file
  This code provides a initialization of intel VT-d (Virtualization Technology for Directed I/O).

@copyright
  Copyright (c) 1999 - 2015 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
#ifndef _VT_D_H_
#define _VT_D_H_

///
/// Include files
///
#include <DmaRemappingTable.h>
#include <SaAccess.h>
#include <PchAccess.h>
#ifdef ECP_FLAG
#include <Protocol/AcpiTable/AcpiTable.h>
#else
#include <Uefi.h>
#include <Protocol/AcpiSystemDescriptionTable.h>
#include <Protocol/AcpiTable.h>
#endif
#include <Library/MmPciLib.h>
#include <CpuRegs.h>
//#include <PchInit/Dxe/PchInitCommon.h>
#include <Guid/EventGroup.h>
#include <Library/S3BootScriptLib.h>
#include <Protocol/FirmwareVolume2.h>


///
/// SA ACPI table data storage file
///
#include <Guid/SaAcpiTableStorage.h>
#include <SaDataHob.h>

#define VTD_ECAP_REG  0x10
#define IR            BIT3

#define VTD_GCMD_REG  0x18
#define QIE           BIT26
#define IRE           BIT25
#define VTD_MASK_RMR  0xFFF

EFI_STATUS
VtdInit (
  IN  SA_DATA_HOB    *SaDataHob
  )
/**
  Locate the VT-d ACPI tables data file and read ACPI SSDT tables.
  Publish the appropriate SSDT based on current configuration and capabilities.

  @param[in] DxePlatformSaPolicy            SA DxePlatformPolicy protocol

  @retval EFI_SUCCESS - Vtd initialization complete
  @retval Other       - No Vtd function initiated
**/
;

VOID
UpdateDmarEndOfDxe (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  )
/**
  EndOfDxe routine for update DMAR
**/
;
#endif
