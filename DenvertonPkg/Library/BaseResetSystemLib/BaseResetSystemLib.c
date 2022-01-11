/** @file
  System reset library services.

@copyright
  Copyright (c) 2007 - 2016 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by the
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor. This file may be modified by the user, subject to
  the additional terms of the license agreement.

@par Specification Reference:
**/
#include <Base.h>
#include <Library/ResetSystemLib.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/PciLib.h>
#include <PchAccess.h>

/**
  Calling this function causes a system-wide reset. This sets
  all circuitry within the system to its initial state. This type of reset
  is asynchronous to system operation and operates without regard to
  cycle boundaries.

  System reset should not return, if it returns, it means the system does
  not support cold reset.
**/
VOID
EFIAPI
ResetCold (
  VOID
  )
{
  IoWrite8 (R_PCH_RST_CNT, V_PCH_RST_CNT_HARDSTARTSTATE);
  IoWrite8 (R_PCH_RST_CNT, V_PCH_RST_CNT_HARDRESET);
}

/**
  Calling this function causes a system-wide initialization. The processors
  are set to their initial state, and pending cycles are not corrupted.

  System reset should not return, if it returns, it means the system does
  not support warm reset.
**/
VOID
EFIAPI
ResetWarm (
  VOID
  )
{
  IoWrite8 (R_PCH_RST_CNT, V_PCH_RST_CNT_SOFTSTARTSTATE);
  IoWrite8 (R_PCH_RST_CNT, V_PCH_RST_CNT_SOFTRESET);
}

/**
  Calling this function causes the system to enter a power state equivalent
  to the ACPI G2/S5 or G3 states.

  System shutdown should not return, if it returns, it means the system does
  not support shut down reset.
**/
VOID
EFIAPI
ResetShutdown (
  VOID
  )
{
  UINT16  PchPmioBase;
  UINT16  Data16;
  UINT32  Data32;

  //
  // Firstly, ACPI decode must be enabled
  //
  PciOr8 (
    PCI_LIB_ADDRESS (DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_PMC, PCI_FUNCTION_NUMBER_PCH_PMC, R_PCH_PMC_ACPI_CNT),
    (UINT8) (B_PCH_PMC_ACPI_CNT_ACPI_EN)
    );

  PchPmioBase = (UINT16) (PciRead16 (PCI_LIB_ADDRESS (DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_PMC, PCI_FUNCTION_NUMBER_PCH_PMC, R_PCH_PMC_ACPI_BASE)) & ~BIT0);

  //
  // Then, GPE0_EN should be disabled to avoid any GPI waking up the system from S5
  //
  Data16 = 0;
  IoWrite16 (
    (UINTN)(PchPmioBase + R_PCH_ACPI_GPE0_EN_127_96),
    (UINT16)Data16
    );

  //
  // Secondly, PwrSts register must be cleared
  //
  // Write a "1" to bit[8] of power button status register at
  // (PM_BASE + PM1_STS_OFFSET) to clear this bit
  //
  Data16 = B_PCH_ACPI_PM1_STS_PWRBTN;
  IoWrite16 (
    (UINTN)(PchPmioBase + R_PCH_ACPI_PM1_STS),
    (UINT16)Data16
    );

  //
  // Disable SMI on S5 sleep state. This allows ayatem to go to S5.
  //
  
  // Denverton Aptiov Override Start - EIP#364364
  // To avoid disabling of SMI on S5 sleep state and thus avoiding system entering into S5 state
  // This mechanism allows all the handlers registered for SMI to be serviced and then system is 
  // put into S5 sleep state.
  
  // IoWrite32 (PchPmioBase + R_PCH_SMI_EN,(UINT32)(IoRead32 (PchPmioBase + R_PCH_SMI_EN) & (~B_PCH_SMI_EN_ON_SLP_EN)));
  // Denverton Aptiov Override End - EIP#364364
  //
  // Finally, transform system into S5 sleep state
  //
  Data32 = IoRead32 ((UINTN) (PchPmioBase + R_PCH_ACPI_PM1_CNT));

  Data32 = (UINT32) ((Data32 & ~(B_PCH_ACPI_PM1_CNT_SLP_TYP + B_PCH_ACPI_PM1_CNT_SLP_EN)) | V_PCH_ACPI_PM1_CNT_S5);

  IoWrite32 (
    (UINTN)(PchPmioBase + R_PCH_ACPI_PM1_CNT),
    (UINT32)Data32
    );

  Data32 = Data32 | B_PCH_ACPI_PM1_CNT_SLP_EN;

  IoWrite32 (
    (UINTN)(PchPmioBase + R_PCH_ACPI_PM1_CNT),
    (UINT32)Data32
    );

  return;
}

/**
  Calling this function causes the system to enter a power state for capsule
  update.

  Reset update should not return, if it returns, it means the system does
  not support capsule update.

**/
VOID
EFIAPI
EnterS3WithImmediateWake (
  VOID
  )
{
  ASSERT (FALSE);
}

