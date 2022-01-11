/** @file
  This Dxe driver will initialize RapidStart Persistent Data Variable and also verify RapidStart Store valid or not

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
#include "Base.h"
//include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiLib.h>
#include <Library/HobLib.h>
#include <Guid/NonVolatileMemoryHob.h>
#include <Include/SetupVariable.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Include/PlatformDefinitions.h>
#include <Library/PchCycleDecodingLib.h>

#define ADR_EN (0xf0)


/**
Routine Description:

  This function fills the Policy and Setup Internal ADR settings with the hardwares defaults.

Arguments:

  Policy   Policy's list of Internal ADR source settings
  Setup    Setup menu's list of Internal ADR source settings
  SaPlatformPolicyPpi    The SA Platform Policy PPI instance

Returns:

  EFI_SUCCESS   Operation success.
**/
VOID
FillInternAdrSources(){

  EFI_STATUS            Status;
  UINTN const           PmcXramAddr = PCH_PWRM_BASE_ADDRESS;
  UINT32                AdrSourceEnables;
  UINT8                 Idx;
  EFI_GUID              SystemConfigurationGuid = SYSTEM_CONFIGURATION_GUID;
  SYSTEM_CONFIGURATION  SetupDataBuffer;
  SYSTEM_CONFIGURATION* const SetupData = &SetupDataBuffer;
  UINTN                 SetupVariableSize = sizeof(SetupDataBuffer);
  UINT32                Attributes;
  UINT32                const Entries = sizeof(SetupDataBuffer.PmcInternAdrSources)/sizeof(SetupDataBuffer.PmcInternAdrSources[0]);

  ASSERT(NULL != SetupData);
  Attributes = EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS;
  //
  // Get Management configuration from Setup Data
  //
  // Denverton Aptiov override Start - EIP#217439
  Status = gRT->GetVariable (
                  L"IntelSetup",
                  &SystemConfigurationGuid,
                  &Attributes,
                  &SetupVariableSize,
                  &SetupDataBuffer
                  );
  // Denverton Aptiov override End - EIP#217439

  if (!EFI_ERROR (Status)) {
    //
    // The ADR internal sources only need to be updated the very first time the BIOS runs
    //
    if (0 == SetupData->PmcInternAdrSourcesSet) {
      SetupData->PmcInternAdrSourcesSet = 1;
      Status = PchPwrmBaseSet((UINT32)PmcXramAddr);
      ASSERT_EFI_ERROR(Status);

      //
      // Move each source bit from PMC's ADR_EN into the Setup menu's Setup data
      // and the NonVolatile Memory Policy's source array
      //
      AdrSourceEnables = *((UINT32 volatile *)(PmcXramAddr+ADR_EN));
      DEBUG ((DEBUG_INFO, "%a() - ADR_EN:%x {",__FUNCTION__, AdrSourceEnables));
      for ( Idx=0; Idx < Entries; Idx++ ) {
         SetupData->PmcInternAdrSources[Idx] = (0 < (AdrSourceEnables & (1 << Idx)));
         if ( (AdrSourceEnables & (1<< Idx)) ) {
           DEBUG ((DEBUG_INFO, "%2x, ", Idx));
         }
      }
      DEBUG ((DEBUG_INFO, "}\n",__FUNCTION__, Idx));

      Status = gRT->SetVariable (
                      L"IntelSetup",
                      &SystemConfigurationGuid,
                      Attributes,
                      SetupVariableSize,
                      &SetupDataBuffer
                      );

    }
  }
}

EFI_STATUS
EFIAPI
NonVolatileMemDxeEntryPoint (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  NON_VOLATILE_MEM_HOB *NonVolatileMemHob;
  EFI_HOB_GUID_TYPE    *GuidHob;

  DEBUG ((DEBUG_ERROR, "%a()\n",__FUNCTION__));
  //
  // Search for the Platform Info PEIM GUID HOB.
  //
  GuidHob       = GetFirstGuidHob (&gNonVolatileMemHobGuid);
  ASSERT (GuidHob);
  NonVolatileMemHob  = GET_GUID_HOB_DATA (GuidHob);

  DEBUG ((DEBUG_ERROR, "NonVolatileMemHob {NonVolMemMode:%x, ", NonVolatileMemHob->NonVolMemMode));
  DEBUG ((DEBUG_ERROR,
    "C2fLowStart:%x, C2fLowLength:%x, C2fHighStart:%x, C2fHighLength:%x}\n",
    NonVolatileMemHob->C2fLowStart, NonVolatileMemHob->C2fLowLength,
    NonVolatileMemHob->C2fHighStart, NonVolatileMemHob->C2fHighLength
    ));
  DEBUG ((DEBUG_ERROR,
    " NVDIMM_SMBUS_ADDR {[0]:%x, [1]:%x, [2]:%x, [3]:%x}\n",
    NonVolatileMemHob->NVDIMM_SMBUS_ADDR[0], NonVolatileMemHob->NVDIMM_SMBUS_ADDR[1],
    NonVolatileMemHob->NVDIMM_SMBUS_ADDR[2], NonVolatileMemHob->NVDIMM_SMBUS_ADDR[3]
    ));

  FillInternAdrSources();
  //dnvtodo - Update NVDIMM charge state if in NVDIMM mode

  //dnvtodo - return if not in SMI+C2F not enabled
  //dnvtodo - Register a SMI handler to do C2F for SMI+C2F
  //dnvtodo - Setup GPIO to generate SMI for SMI+C2F

  return EFI_SUCCESS;
}
