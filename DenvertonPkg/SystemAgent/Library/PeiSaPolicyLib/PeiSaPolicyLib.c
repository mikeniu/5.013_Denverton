/** @file
  This file is PeiSaPolicyLib library.

@copyright
  Copyright (c) 2014 - 2016 Intel Corporation. All rights reserved
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
#include "PeiSaPolicyLib.h"
#include <Include/Library/ConfigBlockLib.h>
#ifndef LPT_FLAG
#include <Library/GpioLib.h>
#endif

GLOBAL_REMOVE_IF_UNREFERENCED IP_BLOCK_STRUCT  SaIpBlocks [] = {
//      Block ID                         Revision,                                               Block Size
       {EnumPlatformConfigId,           SA_PLATFORM_CONFIG_REVISION,                        sizeof(SA_PLATFORM_CONFIG)},
       {EnumVtdConfigId,                VTD_CONFIG_REVISION,                                sizeof(VTD_CONFIG)},
       {EnumMemConfigId,                MEMORY_CONFIG_REVISION,                             sizeof(MEMORY_CONFIGURATION)},
       {EnumNvMemConfigId,              NV_MEMORY_CONFIG_REVISION,                          sizeof(NV_MEMORY_CONFIGURATION)}
	   };

//
// Function call to Load defaults for Individial IP Blocks
//
EFI_STATUS
EFIAPI
LoadSaPlatformDefault (
  IN   VOID          *ConfigBlockPointer
 )
{
  SA_PLATFORM_CONFIG                     *PlatformData;

  PlatformData = ConfigBlockPointer;

  //
  // Policy initialization commented out here is because it's the same with default 0 and no need to re-do again.
  // Update the REVISION number
  //
  PlatformData->Revision = SA_POLICY_PPI_REVISION_1;
  DEBUG ((DEBUG_INFO, "Inside  case EnumPlatformConfigId\n"));
  //
  // Initialize the Svid Configuration
  //
  PlatformData->DefaultSvidSid.SubSystemVendorId = V_SA_MC_VID;
  PlatformData->DefaultSvidSid.SubSystemId = 0x2010;
  //
  // Initialize the Platform Configuration
  //
  PlatformData->SpdAddressTable[0]  = 0xA0;
  PlatformData->SpdAddressTable[1]  = 0xA2;
  PlatformData->SpdAddressTable[2]  = 0xA4;
  PlatformData->SpdAddressTable[3]  = 0xA6;
  PlatformData->MchBar              = 0xFED10000;
  PlatformData->SmbusBar            = 0xEFA0;
  PlatformData->TsegSize            = 0x800000;
  PlatformData->MmioSize            = 0x400;
//  PlatformData->BoardId             = 0;
//  PlatformData->MmioSizeAdjustment  = 0;
//  PlatformData->BdatEnable   = FALSE;
//  PlatformData->ChapDeviceEnable = FALSE;
//  PlatformData->Device4Enable    = FALSE;
  PlatformData->EnableSsaClockGating = FALSE;
  PlatformData->MsiRedirAlgorithm    = 0x0;
  PlatformData->SchedulerLatency = 8;
  PlatformData->BestEffortMaxLat = 0;
  SetMem(PlatformData->CosCatAgent, sizeof(PlatformData->CosCatAgent), 0);

  DEBUG ((DEBUG_INFO, "Exiting  case EnumPlatformConfigId\n"));
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
LoadVtdDefault(
IN   VOID   *ConfigBlockPointer
)
{
  VTD_CONFIG   *Vtd;
  Vtd = ConfigBlockPointer;

  DEBUG ((DEBUG_INFO, "Inside  case EnumVtdConfigId\n"));
  DEBUG ((DEBUG_INFO, "Vtd->BlockId = 0x%x\n", Vtd->BlockId));
  DEBUG ((DEBUG_INFO, "Vtd->BlockSize = 0x%x\n", Vtd->BlockSize));

  //
  // Initialize the Vtd Configuration
  //
  Vtd->VtdEnable       = 0;
  Vtd->InterruptRemappingSupport = 0;
  DEBUG ((DEBUG_INFO, "Vtd 0x%x and remapping 0x%x \n", Vtd->BlockSize, Vtd->InterruptRemappingSupport));
  Vtd->BaseAddress[0]  = 0xFED90000;
  Vtd->RmrrUsbBaseAddress  = 0x3E2E0000;
  Vtd->RmrrUsbLimitAddress = 0x3E2FFFFF;
  Vtd->X2ApicOptOut    = 1;
  DEBUG ((DEBUG_INFO, "Exiting  case EnumVtdConfigId\n"));
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
LoadMemConfigDefault(
  IN VOID *ConfigBlockPointer
 )
{
  DEBUG ((DEBUG_INFO, "Exiting  case EnumMemConfigId\n"));
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
LoadNvMemConfigDefault(
  IN VOID *ConfigBlockPointer
 )
{
  DEBUG ((DEBUG_INFO, "Exiting  case EnumNvMemConfigId\n"));
  return EFI_SUCCESS;
}


/**
  LoadConfigBlockDefault - TBD

  @param[in] TBD

  @retval EFI_SUCCESS                   The policy is installed.
  @retval EFI_OUT_OF_RESOURCES          Insufficient resources to create buffer
**/
EFI_STATUS
EFIAPI
LoadConfigBlockDefault (
  IN   VOID          *ConfigBlockPointer,
  IN   UINT16         BlockId
  )
{
  switch(BlockId){
    case EnumPlatformConfigId:
      LoadSaPlatformDefault(ConfigBlockPointer);
    break;
    case EnumVtdConfigId:
      LoadVtdDefault(ConfigBlockPointer);
    break;
    case EnumMemConfigId:
      LoadMemConfigDefault(ConfigBlockPointer);
    break;
    case EnumNvMemConfigId:
      LoadNvMemConfigDefault(ConfigBlockPointer);
    break;
    default:
      return EFI_NOT_FOUND;
    break;
  }
  return EFI_SUCCESS;
}
/**
  CreateConfigBlocks generates the config blocksg of SA Policy.
  It allocates and zero out buffer, and fills in the Intel default settings.

  @param[out] SiSaPolicyPpi      The pointer to get SA Policy PPI instance

  @retval EFI_SUCCESS                   The policy default is initialized.
  @retval EFI_OUT_OF_RESOURCES          Insufficient resources to create buffer
**/

EFI_STATUS
EFIAPI
CreateConfigBlocks(
  IN OUT  SI_SA_POLICY_PPI          **SiSaPolicyPpi
  )
{
  UINT32                            TotalPolicySize;
  UINT8                             TotalBlockCount;
  UINT8                             BlockCount;
  VOID                              *ConfigBlockPointer;
  EFI_STATUS                        Status;
  SI_SA_POLICY_PPI                  *SaInitPolicy;

  TotalBlockCount = sizeof(SaIpBlocks) / sizeof (IP_BLOCK_STRUCT);
  DEBUG ((DEBUG_INFO, "TotalBlockCount = 0x%x\n", TotalBlockCount));

  TotalPolicySize = (UINT32) sizeof (SI_SA_POLICY_PPI);

  for (BlockCount = 0 ; BlockCount < TotalBlockCount; BlockCount++) {
    TotalPolicySize += (UINT32)SaIpBlocks[BlockCount].BlockSize;
    DEBUG ((DEBUG_INFO, "TotalPolicySize after adding  Block[0x%x]= 0x%x\n", BlockCount, TotalPolicySize));
  }
  DEBUG ((DEBUG_INFO, "TotalPolicySize Final = 0x%x\n", TotalPolicySize));

  //
  // Allocate descriptor and PPI structures
  //
  SaInitPolicy = (SI_SA_POLICY_PPI *) AllocateZeroPool (TotalPolicySize);
  DEBUG ((DEBUG_INFO, "SaInitPolicy= 0x%x \n", SaInitPolicy));
  ASSERT (SaInitPolicy != NULL);
  if (SaInitPolicy == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Initialize ConfigBlockPointer to NULL
  //
  ConfigBlockPointer = NULL;
  //
  // Loop to identify each config block from SaIpBlocks[] Table and add each of them
  //
  for (BlockCount = 0 ; BlockCount < TotalBlockCount; BlockCount++) {
	Status = AddNewConfigBlock ((CONFIG_BLOCK_STRUCT *)SaInitPolicy, (VOID *)&ConfigBlockPointer, (UINT16)SaIpBlocks[BlockCount].BlockId, (UINT16) SaIpBlocks[BlockCount].BlockRevision, (UINT32)SaIpBlocks[BlockCount].BlockSize);
	ASSERT_EFI_ERROR (Status);
    LoadConfigBlockDefault( (VOID *)ConfigBlockPointer, (UINT16)SaIpBlocks[BlockCount].BlockId);
	DEBUG ((DEBUG_INFO, " Addnew config block for SaIpBlocks[BlockCount].BlockId = 0x%x \n", SaIpBlocks[BlockCount].BlockId ));
  }

  // Assignment for returning SaInitPolicy config block base address
  *SiSaPolicyPpi = SaInitPolicy;
  return EFI_SUCCESS;
}

/**
  SiSaInstallPolicyPpi installs SiSaPolicyPpi.
  While installed, RC assumes the Policy is ready and finalized. So please update and override
  any setting before calling this function.

  @param[in] SiSaPolicyPpi

  @retval EFI_SUCCESS                   The policy is installed.
  @retval EFI_OUT_OF_RESOURCES          Insufficient resources to create buffer
**/
EFI_STATUS
EFIAPI
SiSaInstallPolicyPpi (
  IN  SI_SA_POLICY_PPI           *SiSaPolicyPpi
  )
{
  EFI_STATUS                  Status;
  EFI_PEI_PPI_DESCRIPTOR      *SiSaInstallPolicyPpiDesc;
  SA_DATA_HOB                 *SaDataHob;
  SA_PLATFORM_CONFIG          *SaPlatformConfig;
  VTD_CONFIG                  *VtdConfig;

  Status = GetConfigBlock ((CONFIG_BLOCK_STRUCT *)SiSaPolicyPpi, (UINT16)EnumPlatformConfigId, (VOID *)&SaPlatformConfig);
  ASSERT_EFI_ERROR (Status);

  Status = GetConfigBlock ((CONFIG_BLOCK_STRUCT *)SiSaPolicyPpi, (UINT16)EnumVtdConfigId, (VOID *)&VtdConfig);
  ASSERT_EFI_ERROR (Status);

  SiSaInstallPolicyPpiDesc = (EFI_PEI_PPI_DESCRIPTOR *) AllocateZeroPool (sizeof (EFI_PEI_PPI_DESCRIPTOR));
  ASSERT (SiSaInstallPolicyPpiDesc != NULL);
  if (SiSaInstallPolicyPpiDesc == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Initialize the PPI
  //
  SiSaInstallPolicyPpiDesc->Flags  = EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST;
  SiSaInstallPolicyPpiDesc->Guid   = &gSiSaPolicyPpiGuid;
  SiSaInstallPolicyPpiDesc->Ppi    = SiSaPolicyPpi;

  //
  // Install PEI SA Policy PPI
  //
  Status = PeiServicesInstallPpi (SiSaInstallPolicyPpiDesc);
  ASSERT_EFI_ERROR (Status);

  //
  // Create SA Hob
  //
  ///
  /// Create HOB for SA Data
  ///
  Status = PeiServicesCreateHob (
                             EFI_HOB_TYPE_GUID_EXTENSION,
                             sizeof (SA_DATA_HOB),
                             (VOID **) &SaDataHob
                             );
  ASSERT_EFI_ERROR (Status);

  ///
  /// Initialize default HOB data
  ///
  SaDataHob->EfiHobGuidType.Name                       = gSaDataHobGuid;
  SaDataHob->VtdData.VtdEnable                         = (BOOLEAN) (VtdConfig->VtdEnable);
  SaDataHob->VtdData.BaseAddress[0]                    = VtdConfig->BaseAddress[0];
  SaDataHob->VtdData.RmrrUsbBaseAddress                = VtdConfig->RmrrUsbBaseAddress;
  SaDataHob->VtdData.RmrrUsbLimitAddress               = VtdConfig->RmrrUsbLimitAddress;
  SaDataHob->SaPlatform.EnableSsaClockGating           = SaPlatformConfig->EnableSsaClockGating;
  SaDataHob->SaPlatform.MsiRedirAlgorithm              = SaPlatformConfig->MsiRedirAlgorithm;
  SaDataHob->SaPlatform.SchedulerLatency			         = SaPlatformConfig->SchedulerLatency;
  CopyMem(SaDataHob->SaPlatform.CosCatAgent, SaPlatformConfig->CosCatAgent, sizeof(SaPlatformConfig->CosCatAgent));
  SaDataHob->SaPlatform.BestEffortMaxLat			         = SaPlatformConfig->BestEffortMaxLat;
  SaDataHob->VtdData.InterruptRemappingSupport         = (BOOLEAN) (VtdConfig->InterruptRemappingSupport);

  DEBUG ((DEBUG_INFO, "SA Data HOB installed\n"));

  return Status;
}
