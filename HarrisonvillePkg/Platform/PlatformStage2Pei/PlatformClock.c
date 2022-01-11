/*++
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
--*/
/*++

Copyright (c) 2000 - 2015 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

Module Name:

  PlatformClock.c

Abstract:

  Clock chip initialization code

--*/


#include "PlatformStage2.h"
#include "PlatformBoardId.h"
#include "Guid\PlatformInfo.h"

UINT8                         MobileConfigurationTable[] = MOBILE_CLOCK_CONTROLLER_SETTINGS;
UINT8                         DesktopConfigurationTable[] = DESKTOP_CLOCK_CONTROLLER_SETTINGS;
EFI_STATUS
GetPlatformInfo(
  IN CONST EFI_PEI_SERVICES             **PeiServices,
  OUT PLATFORM_INFO_HOB                 **Platform
);

EFI_STATUS
ConfigurePlatformClocks (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *SmbusPpi
  )
/*++

Routine Description:

  Configure the clock generator to enable free-running operation.  This keeps
  the clocks from being stopped when the system enters C3 or C4.

Arguments:

  None

Returns:

  EFI_SUCCESS    The function completed successfully.

--*/
{
  EFI_STATUS                    Status;
  UINTN                         Length=0;
  CLOCK_GENERATOR_TYPE          ClockType = ClockGeneratorCk505;
  UINT8                         ClockAddress = CLOCK_CONTROLLER_SMBUS_ADDRESS;
  BOOLEAN                       EnableSpreadSpectrum;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI *VariableServices;
  UINTN                         VarSize;
  SYSTEM_CONFIGURATION          SystemConfiguration;
  PLATFORM_INFO_HOB             *PlatformInfoHob;
  UINTN                         PchSpiBase;

  PchSpiBase = MmioRead32 (MmPciBase (
                            DEFAULT_PCI_BUS_NUMBER_PCH,
                          PCI_DEVICE_NUMBER_PCH_SPI,
                          PCI_FUNCTION_NUMBER_PCH_SPI)
                          + R_PCH_SPI_BAR0) &~(0xFFF);

  //
  // Check Integrated Clock Mode Select (PCH Strap 17 [0]) to know if skip CK505 programming.
  // 0 = Full Integrated Clock Mode
  // 1 = Buffered Through Clock Mode
  //
  if ((MmioRead16 (PchSpiBase + R_PCH_SPI_HSFSC) & B_PCH_SPI_HSFSC_FDV) == B_PCH_SPI_HSFSC_FDV) {
    MmioAndThenOr32 (PchSpiBase + R_PCH_SPI_FDOC,
                     (UINT32) (~(B_PCH_SPI_FDOC_FDSS_MASK | B_PCH_SPI_FDOC_FDSI_MASK)),
                     (UINT32) (V_PCH_SPI_FDOC_FDSS_PCHS | R_PCH_SPI_STRP17)
                     );
    if ((MmioRead32 (PchSpiBase + R_PCH_SPI_FDOC) & (UINT32) B_PCH_SPI_STRP17_CLK_MODE) == 0) {
      DEBUG(( DEBUG_ERROR, "Integrated Clock Mode Select is Full Integrated Clock Mode.\n"));
      return EFI_SUCCESS;
    }
  }

  //
  // Locate variable service and get system configuration info
  //
  Status = (*PeiServices)->LocatePpi (
             PeiServices,
             &gEfiPeiReadOnlyVariable2PpiGuid, // GUID
             0,                            // INSTANCE
             NULL,                         // EFI_PEI_PPI_DESCRIPTOR
             &VariableServices             // PPI
             );
  ASSERT_EFI_ERROR (Status);
  VarSize = sizeof (SYSTEM_CONFIGURATION);
  Status = VariableServices->GetVariable (
                               VariableServices,
                               PLATFORM_SETUP_VARIABLE_NAME,
                               &mSystemConfigurationGuid,
                               NULL,
                               &VarSize,
                               &SystemConfiguration
                               );
  //
  // Bit 0-1, Byte 4 is the Spread Spectrum enabling bits: 1 on; 0 off
  //
  if (Status != EFI_SUCCESS) {
    EnableSpreadSpectrum = FALSE;
  } else {
    EnableSpreadSpectrum = SystemConfiguration.spreadSpectrumEnable; //DNV_TODO: SystemConfiguration.EnableClockSpreadSpec;
  }

  Status = GetPlatformInfo (PeiServices, &PlatformInfoHob);
  ASSERT_EFI_ERROR ( Status);
  if (EFI_ERROR (Status) || (PlatformInfoHob == NULL)) {
    return Status;
  }

  if ((PlatformInfoHob->PlatformFlavor == FlavorMobile) ||
      (PlatformInfoHob->PlatformFlavor == FlavorEmbedded)) {
    Length = sizeof (MobileConfigurationTable);
    Status = ConfigureClockGenerator (PeiServices, ClockType, ClockAddress, EnableSpreadSpectrum, Length, MobileConfigurationTable);
    ASSERT_EFI_ERROR ( Status);
  } else if (PlatformInfoHob->PlatformFlavor  == FlavorDesktop) {
    Length = sizeof (DesktopConfigurationTable);
    Status = ConfigureClockGenerator (PeiServices, ClockType, ClockAddress, EnableSpreadSpectrum, Length, DesktopConfigurationTable);
    ASSERT_EFI_ERROR ( Status);

  }

  return EFI_SUCCESS;
}


