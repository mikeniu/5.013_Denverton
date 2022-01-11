/** @file
  This file is a library for FIA MUX Policy Initialization.

@copyright
 Copyright (c) 2015 - 2016 Intel Corporation. All rights reserved
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
#include <Library/IoLib.h>
#include <Library/Hoblib.h>
#include <Library/DebugLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>

#include <Guid/FiaMuxHobGuid.h>
#include <Library/MeFiaMuxLib.h>
#include <Library/PeiFiaMuxPolicyInitLib.h>
#include <Library/MmPciLib.h>
#include <SimicsDebug.h>

/**
  Get FIA_MUX_CONFIG_HOB HOB

  @return FiaMuxConfigHob 		FIA MUX Config HOB structure
**/
FIA_MUX_CONFIG_HOB *
FiaMuxPolicyGetHob (
  VOID
  )
{
  EFI_HOB_GUID_TYPE     *GuidHob;
  FIA_MUX_CONFIG_HOB    *FiaMuxConfigHob;

  GuidHob = GetFirstGuidHob (&gFiaMuxPolicyHobGuid);
  ASSERT (GuidHob != NULL);

  FiaMuxConfigHob = (FIA_MUX_CONFIG_HOB *)GET_GUID_HOB_DATA(GuidHob);

  return FiaMuxConfigHob;
}

/**
  Build FIA_MUX_CONFIG_HOB HOB

  @param FiaMuxConfigHob 		FIA MUX Config HOB structure
**/
VOID
FiaMuxPolicySetHob(
  IN FIA_MUX_CONFIG_HOB *FiaMuxConfigHob
  )
{
  EFI_PEI_HOB_POINTERS             Hob;
  UINTN                            BufferSize;

  BufferSize = sizeof (FIA_MUX_CONFIG_HOB);
  Hob.Raw = BuildGuidHob (
	  &gFiaMuxPolicyHobGuid,
      BufferSize
      );
  ASSERT (Hob.Raw);

  CopyMem((VOID *)Hob.Raw, FiaMuxConfigHob, sizeof(FIA_MUX_CONFIG_HOB));
}

/**
  Build FIA_MUX_CONFIG_HOB from FIA_MUX_GLOBAL_POLICY_PPI

  @param[in] FiaMuxGlobalPolicyPpi - The pointer to FIA MUX Policy PPI instance
**/
VOID
FiaMuxCreateHob(
  IN  FIA_MUX_GLOBAL_POLICY_PPI *FiaMuxGlobalPolicyPpi
  )
{
  FIA_MUX_CONFIG_HOB     FiaMuxConfigHob;

  //
  // Allocate structure in stack and zero it.
  //
  ZeroMem((VOID *)&FiaMuxConfigHob, sizeof (FIA_MUX_CONFIG_HOB));

  //
  // Init FiaMuxConfigHob structure
  //
  CopyMem((VOID *)&(FiaMuxConfigHob.FiaMuxConfig), (VOID *)FiaMuxGlobalPolicyPpi, sizeof(FIA_MUX_CONFIG));

  //
  // Create FIA MUX configure HOB
  //
  FiaMuxPolicySetHob(&FiaMuxConfigHob);
}

EFI_STATUS
FiaMuxCreatePolicyDefaults(
  OUT  FIA_MUX_GLOBAL_POLICY_PPI **FiaMuxGlobalPolicyPpi
  )
/**
  FiaMuxCreatePolicyDefaults creates the default setting of FIA MUX Policy.
  It allocates and zero out buffer, and fills in with safe and reasonable default settings.

  @param[out] FiaMuxGlobalPolicyPpi - The pointer to get FIA MUX Policy PPI instance

  @retval EFI_SUCCESS               - The policy default is initialized.
  @retval EFI_OUT_OF_RESOURCES      - Insufficient resources to create buffer
**/
{
  FIA_MUX_GLOBAL_POLICY_PPI *FiaMuxGlobalPolicy;
  EFI_STATUS Status;

  FiaMuxGlobalPolicy = (FIA_MUX_GLOBAL_POLICY_PPI *)AllocateZeroPool(sizeof (FIA_MUX_GLOBAL_POLICY_PPI));

  if (FiaMuxGlobalPolicy == NULL) {
    ASSERT (FALSE);
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Default FIA MUX configuration
  //
  Status = MeFiaMuxConfigGet(&FiaMuxGlobalPolicy->FiaMuxConfig.FiaMuxConfig,
                             &FiaMuxGlobalPolicy->FiaMuxConfig.SkuNumLanesAllowed);

  DEBUG((DEBUG_INFO, "FiaMuxCreatePolicyDefaults() MeFiaMuxConfigGet status = %r, MuxConfiguration 0x%lx, NumLanesAllowed 0x%x\n",
          Status, (UINT64)FiaMuxGlobalPolicy->FiaMuxConfig.FiaMuxConfig.MuxConfiguration.MeFiaMuxLaneConfig,
          FiaMuxGlobalPolicy->FiaMuxConfig.SkuNumLanesAllowed));
  DEBUG((DEBUG_INFO, "    SataLaneConfiguration 0x%lx, PcieRootPortsConfiguration 0x%lx\n",
          (UINT64)FiaMuxGlobalPolicy->FiaMuxConfig.FiaMuxConfig.SataLaneConfiguration.MeFiaSataLaneConfig,
          FiaMuxGlobalPolicy->FiaMuxConfig.FiaMuxConfig.PcieRootPortsConfiguration.MeFiaPcieRootPortsConfig));

  if (PEI_IN_SIMICS) {
    DEBUG ((DEBUG_INFO, "In SIMICs MeFiaMuxConfigGet() always return EFI_SUCCESS\n"));
    FiaMuxGlobalPolicy->FiaMuxConfig.SkuNumLanesAllowed = ME_FIA_MUX_LANE_NUM_MAX;
    Status = EFI_SUCCESS;
  }

  //
  // Set the FiaMuxConfigGetStatus
  // Set FiaMuxConfigRequest same as FiaMuxConfig as default
  //
  FiaMuxGlobalPolicy->FiaMuxConfigStatus.FiaMuxConfigGetStatus = Status;
  CopyMem(&FiaMuxGlobalPolicy->FiaMuxConfig.FiaMuxConfigRequest, &FiaMuxGlobalPolicy->FiaMuxConfig.FiaMuxConfig, sizeof(ME_FIA_CONFIG));

  if (Status != EFI_SUCCESS) {
    //
    // Request ME FIA MUX configuration fail,
    // Setting FIA MUX configuration to default failure configure.
    //
    FiaMuxGlobalPolicy->FiaMuxConfig.SkuNumLanesAllowed = ME_FIA_MUX_LANE_NUM_MAX;
    FiaMuxGlobalPolicy->FiaMuxConfig.FiaMuxConfig.MuxConfiguration.MeFiaMuxLaneConfig = ME_FIA_MUX_LANE_XHCI_ONLY;
    FiaMuxGlobalPolicy->FiaMuxConfig.FiaMuxConfig.SataLaneConfiguration.MeFiaSataLaneConfig = (UINT64)0;
    FiaMuxGlobalPolicy->FiaMuxConfig.FiaMuxConfig.PcieRootPortsConfiguration.MeFiaPcieRootPortsConfig = (UINT64)0;
    FiaMuxGlobalPolicy->FiaMuxConfig.FiaMuxConfigRequest.MuxConfiguration.MeFiaMuxLaneConfig = ME_FIA_MUX_LANE_XHCI_ONLY;
    FiaMuxGlobalPolicy->FiaMuxConfig.FiaMuxConfigRequest.SataLaneConfiguration.MeFiaSataLaneConfig = (UINT64)0;
    FiaMuxGlobalPolicy->FiaMuxConfig.FiaMuxConfigRequest.PcieRootPortsConfiguration.MeFiaPcieRootPortsConfig = (UINT64)0;
  }

  *FiaMuxGlobalPolicyPpi = FiaMuxGlobalPolicy;

  return EFI_SUCCESS;
}

EFI_STATUS
FiaMuxInstallPolicyPpi(
  IN  FIA_MUX_GLOBAL_POLICY_PPI *FiaMuxGlobalPolicyPpi
  )
/**
  FiaMuxInstallPolicyPpi installs FiaMuxGlobalPolicyPpi.

  @param[in] FiaMuxGlobalPolicyPpi - The pointer to FIA MUX Policy PPI instance

  @retval EFI_SUCCESS              - The policy is installed.
  @retval EFI_OUT_OF_RESOURCES     - Insufficient resources to create buffer
**/
{
  EFI_STATUS             Status;
  EFI_PEI_PPI_DESCRIPTOR *FiaMuxGlobalPolicyPpiDec;

  FiaMuxGlobalPolicyPpiDec = (EFI_PEI_PPI_DESCRIPTOR *)AllocateZeroPool(sizeof (EFI_PEI_PPI_DESCRIPTOR));
  if (FiaMuxGlobalPolicyPpiDec == NULL) {
	ASSERT (FALSE);
	return EFI_OUT_OF_RESOURCES;
  }

  FiaMuxGlobalPolicyPpiDec->Flags = EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST;
  FiaMuxGlobalPolicyPpiDec->Guid = &gFiaMuxPolicyPpiGuid;
  FiaMuxGlobalPolicyPpiDec->Ppi = FiaMuxGlobalPolicyPpi;

  //
  // Install FIA MUX Policy PPI
  //
  Status = PeiServicesInstallPpi(FiaMuxGlobalPolicyPpiDec);
  ASSERT_EFI_ERROR (Status);

  //
  // Create FIA MUX configure HOB
  //
  FiaMuxCreateHob(FiaMuxGlobalPolicyPpi);

  return Status;
}
