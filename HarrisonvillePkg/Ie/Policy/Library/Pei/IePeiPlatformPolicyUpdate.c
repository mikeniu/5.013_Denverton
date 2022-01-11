/**
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c) 2015 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file IePeiPlatformPolicyUpdate.c

  Do Platform Stage IE initialization.

**/
#include <Library/HobLib.h>
#include <SetupVariable.h>
#include <Ppi/ReadOnlyVariable.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <Library/PeiServicesLib.h>
#include <Library/BaseMemoryLib.h>
#include <Ppi/Spi.h>
#include <Ppi/IePlatformPolicyPei.h>
#include "Library/DebugLib.h"
#include "IeHob.h"
#include "Library/IeHeciCoreLib.h"

#define R_PCH_SPI_STRP_IE_DISABLE       47

/**
  Read soft-strap to determine if IE device is enabled

  param                     None

  retval TRUE               IE device is enabled
  retval FALSE              IE device is disabled
 **/
BOOLEAN IeIsEnabled( VOID )
{
  EFI_STATUS        Status;
  UINT32            SoftStrapValue;
  PCH_SPI_PPI       *SpiPpi;

  Status = PeiServicesLocatePpi( &gPchSpiPpiGuid,
                                  0,
                                  NULL,
                                  (VOID **) &SpiPpi
                                );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[IE Policy] : Cannot locate SpiPPI (%r)\n", Status));
    return 0;
  }

  Status = SpiPpi->ReadPchSoftStrap (SpiPpi, R_PCH_SPI_STRP_IE_DISABLE * sizeof(UINT32), sizeof (SoftStrapValue),(UINT8*)&SoftStrapValue);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[IE Policy] : Cannot read soft-strap %d (%r)\n", R_PCH_SPI_STRP_IE_DISABLE, Status));
    return 0;
  }
  DEBUG ((DEBUG_INFO, "[IE Policy] : Read soft-strap %d (0x%08x). Soft-strap bit (13) is ", R_PCH_SPI_STRP_IE_DISABLE, SoftStrapValue));
  if (SoftStrapValue & BIT13) {
    DEBUG ((DEBUG_INFO, " set, IE Disabled\n"));
    return FALSE;
  } else {
    DEBUG ((DEBUG_INFO, "not set, IE Enabled\n"));
    return TRUE;
  }
}

/*

  Create IePolicy Hob

  @param PeiIePlatformPolicyPpi IE policy

  @retval NONE

 */
VOID
CreateIePolicyHob(PEI_IE_PLATFORM_POLICY_PPI  *PeiIePlatformPolicyPpi)
{
  IE_HOB *IeHob;

  if (PeiIePlatformPolicyPpi == NULL) {
    return;
  }

  IeHob = GetFirstGuidHob (&gIeHobGuid);
  if (IeHob == NULL) {
    EFI_STATUS  Status = EFI_SUCCESS;

    ///
    /// Create HOB for ME Data
    ///
    Status = PeiServicesCreateHob (
               EFI_HOB_TYPE_GUID_EXTENSION,
               sizeof (IE_HOB),
               (VOID **) &IeHob
               );
    ASSERT_EFI_ERROR (Status);
    if (EFI_ERROR (Status)) {
      return;
    }

    DEBUG ((DEBUG_INFO, "[IE Policy] IE HOB installed\n"));

    //
    // Initialize default HOB data
    //
    ZeroMem (&(IeHob->IeDisabledInSoftStraps), (sizeof (IE_HOB) - sizeof (EFI_HOB_GUID_TYPE)));
    IeHob->EfiHobGuidType.Name = gIeHobGuid;
  }
  IeHob->IeDisabledInSoftStraps = PeiIePlatformPolicyPpi->IeDisabledInSoftStraps;
  DEBUG ((DEBUG_INFO, "[IE Policy] IE FW HOB data updated (%d)\n", PeiIePlatformPolicyPpi->IeDisabledInSoftStraps));
}

/*

  Install the Ie Platform Policy Library

  @param PeiIePlatformPolicyPpi  Ie Platform Policy Ppi

  @retval EFI_SUCCESS           Initialization complete.
  @retval EFI_UNSUPPORTED       The chipset is unsupported by this driver.
  @retval EFI_OUT_OF_RESOURCES  Do not have enough resources to initialize the driver.
  @retval EFI_DEVICE_ERROR      Device error, driver exits abnormally.
  @retval EFI_NOT_FOUND         Not all required services are loaded. Try later.

*/

EFI_STATUS
UpdatePeiIePlatformPolicy (
  IN OUT  PEI_IE_PLATFORM_POLICY_PPI  *PeiIePlatformPolicyPpi
  )
{
  EFI_STATUS                      Status = EFI_SUCCESS;
#if defined(IE_SUPPORT) && IE_SUPPORT
  EFI_PEI_READ_ONLY_VARIABLE2_PPI *VariableServices;
  SYSTEM_CONFIGURATION            SystemConfiguration;
  UINTN                           VarSize;

  //
  // Locate system configuration variable
  //
  Status = PeiServicesLocatePpi (
                            &gEfiPeiReadOnlyVariable2PpiGuid, // GUID
                            0,                                // INSTANCE
                            NULL,                             // EFI_PEI_PPI_DESCRIPTOR
                            &VariableServices                 // PPI
                            );
  ASSERT_EFI_ERROR (Status);

  if (Status != EFI_SUCCESS) {
      DEBUG ((DEBUG_ERROR, "[IE Policy] Error: System configuration variable cannot be located.\n"));
      return EFI_NOT_FOUND;
  }

  VarSize = sizeof (SYSTEM_CONFIGURATION);
  Status = VariableServices->GetVariable (
                              VariableServices,
                              PLATFORM_SETUP_VARIABLE_NAME,
                              &gSetupVariableGuid,
                              NULL,
                              &VarSize,
                              &SystemConfiguration
                              );

  if (Status == EFI_SUCCESS) {

    PeiIePlatformPolicyPpi->IeHeci1Enabled = SystemConfiguration.IeHeci1Enabled;
    PeiIePlatformPolicyPpi->IeHeci2Enabled = SystemConfiguration.IeHeci2Enabled;
    PeiIePlatformPolicyPpi->IeHeci3Enabled = SystemConfiguration.IeHeci3Enabled;
    PeiIePlatformPolicyPpi->IeIderEnabled = SystemConfiguration.IeIderEnabled;
    PeiIePlatformPolicyPpi->IeKtEnabled = SystemConfiguration.IeKtEnabled;
    PeiIePlatformPolicyPpi->SubsystemId = SystemConfiguration.SubsystemId;
  } else {
    // Can't get Setup configuration, configure default values for Policy
    ZeroMem(PeiIePlatformPolicyPpi, sizeof(PEI_IE_PLATFORM_POLICY_PPI));
    PeiIePlatformPolicyPpi->SubsystemId = 0x8086;
  }
  PeiIePlatformPolicyPpi->IeDisabledInSoftStraps = !IeIsEnabled();
  if (PeiIePlatformPolicyPpi->IeDisabledInSoftStraps == 1) {
    PeiIePlatformPolicyPpi->IeHeci1Enabled = 0;
    PeiIePlatformPolicyPpi->IeHeci2Enabled = 0;
    PeiIePlatformPolicyPpi->IeHeci3Enabled = 0;
    PeiIePlatformPolicyPpi->IeIderEnabled = 0;
    PeiIePlatformPolicyPpi->IeKtEnabled = 0;
  } else {
    PeiIePlatformPolicyPpi->DfxIe = IeTypeIsDfx();
    if (PeiIePlatformPolicyPpi->DfxIe) {
      PeiIePlatformPolicyPpi->IeHeci1Enabled = 1;
      PeiIePlatformPolicyPpi->IeHeci2Enabled = 1;
      PeiIePlatformPolicyPpi->IeHeci3Enabled = 1;
      PeiIePlatformPolicyPpi->IeIderEnabled = 1;
      PeiIePlatformPolicyPpi->IeKtEnabled = 1;
      DEBUG(( EFI_D_INFO, "[IE Policy] DfxIE Detected - IE Heci devices enabled.\n"));
    }
  }
  CreateIePolicyHob(PeiIePlatformPolicyPpi);

#else
  PeiIePlatformPolicyPpi = NULL;
  Status                 = EFI_NOT_FOUND;
#endif

  return Status;
}
