/** @file
  @todo ADD DESCRIPTION

@copyright
  Copyright (c) 2010 - 2013 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
--*/

#include <Uefi.h>
#include <Library/PcdLib.h>

#include "PlatformDefinitions.h"
#include "CmosMap.h"
#include "PlatformCmosLib.h"

#define DEFAULT_VALUE         0
#define	DEFAULT_ATTRIBUTES    0
#define	EXCLUDE_FROM_CHECKSUM	 CMOS_ATTRIBUTE_EXCLUDE_FROM_CHECKSUM
#define EXCLUDE_FROM_INIT_DATA   CMOS_ATTRIBUTE_EXCLUDE_FROM_INIT_DATA

#define PCI_DEVICE_NUMBER_PCH_PMC                 0x1F
#define PCI_FUNCTION_NUMBER_PCH_PMC               0x02
#define R_PCH_PMC_GEN_PMCON_B                     0xA4
#define B_PCH_PMC_GEN_PMCON_B_RTC_PWR_STS         0x04

// Add the CMOS entry below
CMOS_ENTRY mCmosTable[] = {
{ RTC_ADDRESS_CENTURY, DEFAULT_VALUE, EXCLUDE_FROM_CHECKSUM },
{ CMOS_DEBUG_PRINT_LEVEL_REG, (UINT8)(PLATFORM_DEBUG_PRINT_ERROR_LEVEL_VALUE & 0xFF), EXCLUDE_FROM_CHECKSUM },
{ CMOS_DEBUG_PRINT_LEVEL_1_REG, (UINT8)((PLATFORM_DEBUG_PRINT_ERROR_LEVEL_VALUE >> 8) & 0xFF), EXCLUDE_FROM_CHECKSUM },
{ CMOS_DEBUG_PRINT_LEVEL_2_REG, (UINT8)((PLATFORM_DEBUG_PRINT_ERROR_LEVEL_VALUE >> 16) & 0xFF), EXCLUDE_FROM_CHECKSUM },
{ CMOS_DEBUG_PRINT_LEVEL_3_REG, (UINT8)((PLATFORM_DEBUG_PRINT_ERROR_LEVEL_VALUE >> 24) & 0xFF), EXCLUDE_FROM_CHECKSUM }
};

/**
  Funtion to return platform CMOS entry.

  @param [out]  CmosEntry  Platform CMOS entry.
  @param [out]  CmosEntryCount Number of platform CMOS entry.

  @return Status.
**/
EFI_STATUS
EFIAPI
GetPlatformCmosEntry (
  OUT CMOS_ENTRY  **CmosEntry,
  OUT UINTN       *CmosEntryCount
  )
{
  *CmosEntry = mCmosTable;
  *CmosEntryCount = sizeof(mCmosTable)/sizeof(mCmosTable[0]);
  return EFI_SUCCESS;
}

/**
  Funtion to check if Battery lost or CMOS cleared.

  @reval TRUE  Battery is always present.
  @reval FALSE CMOS is cleared.
**/
BOOLEAN
EFIAPI
CheckCmosBatteryStatus (
  VOID
  )
{
  UINT16    Data;
  UINTN     Address;

  //
  // Check if the CMOS battery is present
  // Checks RTC_PWR_STS bit in the ICH GEN_PMCON_3 register
  //
  Address = PCIEX_BASE_ADDRESS + (UINT32)(PCI_DEVICE_NUMBER_PCH_PMC << 15) + (UINT32)(PCI_FUNCTION_NUMBER_PCH_PMC << 12)+ R_PCH_PMC_GEN_PMCON_B; 
  Data = (*(volatile UINT16*)(UINTN)(Address));
  if ((Data & B_PCH_PMC_GEN_PMCON_B_RTC_PWR_STS) != 0) {
    return FALSE;
  }
  return TRUE;
}
