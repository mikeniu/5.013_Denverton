/** @file
  This file is the code for Intel NonVolatile Memory Policy initialization.

@copyright
  Copyright (c) 1999 - 2016 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may be modified by the user, subject to
  the additional terms of the license agreement
**/
#include "NonVolatileMemPolicyInit.h"
#include "Base.h"
#include "Uefi.h"
#include <NonVolatileMemPolicy.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/ConfigBlockLib.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <Include/SetupVariable.h>
#include <Ppi/SaPolicy.h>
#include <Library/AdrCfgLib.h>
#include <Library/PchCycleDecodingLib.h>


EFI_GUID  mSystemConfigurationGuid = SYSTEM_CONFIGURATION_GUID;
//
// converts a size from number of megabytes to number of bytes
//
#define BITS_IN_MB              20
#define CONVERT_MB_TO_BYTES(MB) (MB << BITS_IN_MB)
#ifndef VOLATILE
#define VOLATILE    volatile
#endif
typedef UINT32 VOLATILE * UINT32_REG;

//
// Nonvolatile memory modes
//
#define NONVOL_DISABLED 0
#define NONVOL_ADR      1
#define NONVOL_C2F      2
#define NONVOL_C2F_SMI  3
#define NONVOL_NVDIMM   4

/**
Routine Description:

  This function performs the NonVolatileMem part of SA PEI Platform Policy initialization.

Arguments:

  PeiServices   General purpose services available to every PEIM.
  SaPolicyPpi    The SA Platform Policy PPI instance

Returns:

  EFI_SUCCESS   Operation success.
**/
EFI_STATUS
UpdateNonVolatileSettings (
  IN CONST SYSTEM_CONFIGURATION        *SystemConfiguration,
  IN CONST NON_VOLATILE_MEM_POLICY_PPI *NonVolatileMemPolicyPpi,
  IN OUT   SI_SA_POLICY_PPI            *SiSaPolicyPpi
  )
{
  EFI_STATUS              Status;
  NV_MEMORY_CONFIGURATION *NonVolMemConfig;

  DEBUG ((DEBUG_INFO, "%a()\n", __FUNCTION__));
  Status = GetConfigBlock ((CONFIG_BLOCK_STRUCT *)SiSaPolicyPpi, (UINT16)EnumNvMemConfigId, (VOID *)&NonVolMemConfig);
  ASSERT_EFI_ERROR (Status);

  //
  // Pass NonVolatile Memory Settings into the MRC
  //
  NonVolMemConfig->Mode = NonVolatileMemPolicyPpi->NonVolMemMode;
  NonVolMemConfig->Size = NonVolatileMemPolicyPpi->NonVolMemSize;
  NonVolMemConfig->Interleaving = NonVolatileMemPolicyPpi->NonVolMemInterleaving;
  //NonVolMemConfig->AdrRecoveryDetected = NonVolatileMemPolicyPpi->AdrRecoveryDetected; moved to NonvolatilePei driver
  NonVolMemConfig->eraseArmNVDIMMS = NonVolatileMemPolicyPpi->eraseArmNVDIMMS;
  NonVolMemConfig->restoreNVDIMMS = NonVolatileMemPolicyPpi->restoreNVDIMMS;
  DEBUG ((DEBUG_INFO, "%a() - NonVolMemConfig {Mode:%x Size:%x Interl:%x AdrRecov:%x Erase:%x, Restore:%x}\n",__FUNCTION__, NonVolMemConfig->Mode, NonVolMemConfig->Size, NonVolMemConfig->Interleaving, NonVolMemConfig->AdrRecoveryDetected, NonVolMemConfig->eraseArmNVDIMMS, NonVolMemConfig->restoreNVDIMMS));

  return Status;
}


/**
Routine Description:

  This function transfers the Setup menu settings to the NonVolatile Memory Policy.

Arguments:

  PeiServices   General purpose services available to every PEIM.
  SaPolicyPpi    The SA Platform Policy PPI instance

Returns:

  EFI_SUCCESS   Operation success.
**/
VOID
MoveSetupValuesToPolicy (
  IN CONST SYSTEM_CONFIGURATION        *SystemConfiguration,
  IN OUT   NON_VOLATILE_MEM_POLICY_PPI *NonVolMemPolicyPpi
  )
{
  DEBUG ((DEBUG_INFO, "%a()\n", __FUNCTION__));
  ASSERT(NULL != NonVolMemPolicyPpi);
  if (NonVolMemPolicyPpi == NULL) {
    DEBUG ((EFI_D_ERROR, "MoveSetupValuesToPolicy() - NonVolMemPolicyPpi pointer is null!\n"));
    return;
  }
  ASSERT(NULL != SystemConfiguration);
  if (SystemConfiguration == NULL) {
    DEBUG ((EFI_D_ERROR, "MoveSetupValuesToPolicy() - SystemConfiguration pointer is null!\n"));
    return;
  }

  NonVolMemPolicyPpi->NonVolMemMode = SystemConfiguration->NonVolMemMode;
  NonVolMemPolicyPpi->NonVolMemSize = SystemConfiguration->NonVolMemSize;
  //
  // set to zero if NonVolatile Memory mode is disabled
  //
  if ( (NONVOL_DISABLED < NonVolMemPolicyPpi->NonVolMemMode) && (NonVolMemPolicyPpi->NonVolMemMode < NONVOL_NVDIMM) ) {
    NonVolMemPolicyPpi->NonVolMemSize = (SystemConfiguration->NonVolMemSize);
  } else {
    NonVolMemPolicyPpi->NonVolMemSize = 0;
  }

  NonVolMemPolicyPpi->NonVolMemAdrStatusSrc = SystemConfiguration->NonVolMemAdrStatusSrc;
  NonVolMemPolicyPpi->NonVolMemInterleaving = SystemConfiguration->NonVolMemInterleaving;
  NonVolMemPolicyPpi->restoreNVDIMMS = SystemConfiguration->restoreNVDIMMS;
  NonVolMemPolicyPpi->eraseArmNVDIMMS = SystemConfiguration->eraseArmNVDIMMS;
  NonVolMemPolicyPpi->NonVolMemCacheFlushing = SystemConfiguration->NonVolMemCacheFlushing;
  NonVolMemPolicyPpi->NonVolMemC2fLbaLocMethod = SystemConfiguration->NonVolMemC2fLbaLocMethod;
  NonVolMemPolicyPpi->NonVolMemC2fLba = SystemConfiguration->NonVolMemC2fLba;

  NonVolMemPolicyPpi->NonVolMemTestMode = SystemConfiguration->NonVolMemTestMode;

}

/**
Routine Description:

  This function is the entry point of the policy initialization.

Arguments:

  PeiServices   General purpose services available to every PEIM.
  SaPolicyPpi    The SA Platform Policy PPI instance

Returns:

  EFI_SUCCESS   Operation success.
**/
EFI_STATUS
NonVolatileMemPolicyInitEntryPoint (
  IN EFI_PEI_FILE_HANDLE       FileHandle,
  IN CONST EFI_PEI_SERVICES    **PeiServices
  )
/**
  This PEIM performs Non-Volatile Memory Policy initialization.

  @param[in] FfsHeader    - Pointer to Firmware File System file header.
  @param[in] PeiServices  - General purpose services available to every PEIM.

  @retval EFI_SUCCESS - The PPI is installed and initialized.
  @retval EFI ERRORS  - The PPI is not successfully installed.
**/
{
  EFI_STATUS                  Status;
  EFI_PEI_PPI_DESCRIPTOR      *NonVolMemPolicyPpiDesc;
  NON_VOLATILE_MEM_POLICY_PPI *NonVolMemPolicyPpi;
  SI_SA_POLICY_PPI            *SiSaPolicyPpi;

  //
  // Allocate descriptor and PPI structures
  //
  NonVolMemPolicyPpi = (NON_VOLATILE_MEM_POLICY_PPI *) AllocateZeroPool (sizeof (NON_VOLATILE_MEM_POLICY_PPI));
  ASSERT (NonVolMemPolicyPpi != NULL);

  if (NonVolMemPolicyPpi == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  NonVolMemPolicyPpiDesc = (EFI_PEI_PPI_DESCRIPTOR *) AllocateZeroPool (sizeof (EFI_PEI_PPI_DESCRIPTOR));
  ASSERT (NonVolMemPolicyPpiDesc != NULL);

  if (NonVolMemPolicyPpiDesc == NULL) {
    FreePool (NonVolMemPolicyPpi); // Denverton AptioV override - EIP#279291
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Initialize the PPI
  //
  NonVolMemPolicyPpiDesc->Flags  = EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST;
  NonVolMemPolicyPpiDesc->Guid   = &gNonVolatileMemPpiGuid;
  NonVolMemPolicyPpiDesc->Ppi    = NonVolMemPolicyPpi;
  NonVolMemPolicyPpi->Revision = NON_VOLATILE_MEM_POLICY_PPI_REVISION;

  //
  // transfer setup to the policy
  //
  {
    EFI_PEI_READ_ONLY_VARIABLE2_PPI *VariableServices;
    SYSTEM_CONFIGURATION            SystemConfiguration;
    UINTN                           VarSize;

    //
    // Locate system configuration variable
    //
    Status = (*PeiServices)->LocatePpi (
            PeiServices,
            &gEfiPeiReadOnlyVariable2PpiGuid, // GUID
            0,                            // INSTANCE
            NULL,                         // EFI_PEI_PPI_DESCRIPTOR
            &VariableServices             // PPI
    );
    ASSERT_EFI_ERROR ( Status );

    VarSize = sizeof (SYSTEM_CONFIGURATION);
    Status = VariableServices->GetVariable (
            VariableServices,
            PLATFORM_SETUP_VARIABLE_NAME,
            &mSystemConfigurationGuid,
            NULL,
            &VarSize,
            &SystemConfiguration
            );
    ASSERT_EFI_ERROR ( Status );

    MoveSetupValuesToPolicy(&SystemConfiguration, NonVolMemPolicyPpi);

    ///
    /// Get platform policy settings through the SaPolicy PPI
    ///
    Status = (*PeiServices)->LocatePpi (
                                      PeiServices,
                                      &gSiSaPolicyPpiGuid,
                                      0,
                                      NULL,
                                      &SiSaPolicyPpi
                                      );
    ASSERT_EFI_ERROR ( Status );
    UpdateNonVolatileSettings(&SystemConfiguration, NonVolMemPolicyPpi, SiSaPolicyPpi);

  }

  ///
  /// Install SA Platform Policy PPI
  ///
  Status = (**PeiServices).InstallPpi (PeiServices, NonVolMemPolicyPpiDesc);
  ASSERT_EFI_ERROR (Status);

  return Status;
}


