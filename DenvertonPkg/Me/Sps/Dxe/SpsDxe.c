/**

Copyright (c) 2006 - 2016, Intel Corporation.

This source code and any documentation accompanying it ("Material") is furnished
under license and may only be used or copied in accordance with the terms of that
license.  No license, express or implied, by estoppel or otherwise, to any
intellectual property rights is granted to you by disclosure or delivery of these
Materials.  The Materials are subject to change without notice and should not be
construed as a commitment by Intel Corporation to market, license, sell or support
any product or technology.  Unless otherwise provided for in the license under which
this Material is provided, the Material is provided AS IS, with no warranties of
any kind, express or implied, including without limitation the implied warranties
of fitness, merchantability, or non-infringement.  Except as expressly permitted by
the license for the Material, neither Intel Corporation nor its suppliers assumes
any responsibility for any errors or inaccuracies that may appear herein.  Except
as expressly permitted by the license for the Material, no part of the Material
may be reproduced, stored in a retrieval system, transmitted in any form, or
distributed by any means without the express written consent of Intel Corporation.


  @file SpsDxe.c

  This driver implements the DXE phase of SPS support as defined in
  SPS ME-BIOS Interface Specification.

**/
#include <Base.h>
#include <Uefi.h>
#include <PiDxe.h>

#include <Library/PcdLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DxeServicesTableLib.h>
#include <Library/HobLib.h>
#include <Library/IoLib.h>
#include <Library/MemoryAllocationLib.h>

#include <Library/DebugLib.h>
#include <Library/PrintLib.h>

#include <Guid/GlobalVariable.h>

#include <Guid/HobList.h>
#include <Guid/EventLegacyBios.h>
#include <Guid/SpsInfoHobGuid.h>
#include <Guid/EventGroup.h>

#include <Protocol/MpService.h>
#include <Protocol/ExitPmAuth.h>

#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/CpuLib.h>
#include <Library/CpuConfigLib.h>
#include <Library/MePlatformPolicyUpdateDxeLib.h>
#include <Library/MeTypeLib.h>

#include <Protocol/CpuIo.h>
#include <Protocol/HeciProtocol.h>
#include <Protocol/PciIo.h>
#include <Protocol/SpsSiEn.h>
#include <Protocol/SpsNm.h>

#include <Sps.h>
#include <HeciRegs.h>
#include <MeAccess.h>
#include "SpsDxe.h"
#include "MeSetup.h"

/******************************************************************************
 * Function prototypes
 */


/******************************************************************************
 * Variables
 */
EFI_EVENT mSpsEndOfDxeEvent = NULL;
EFI_EVENT mSpsReadyToBootEvent = NULL;
SPS_DXE_CONTEXT *mpSpsContext = NULL;


/******************************************************************************
 * Functions
 */
/**
    TBD.
    @param pSpsMode -
    @param pSystemTable - pointer to system table
    @param pHob -
    @retval EFI status is returned.
**/
EFI_STATUS
GetSpsConfiguration (
  IN ME_FW_DISCOVERY_MODE   *pSpsMode,
  IN EFI_SYSTEM_TABLE       *SystemTable,
  EFI_HOB_TYPE_SPS_INFO     *pHob
  )
{

  VOID                  *HobList;
  EFI_GUID              TempGUID;
  EFI_STATUS            Status;
  EFI_HOB_GUID_TYPE     *GuidHob;
  EFI_HOB_TYPE_SPS_INFO *pInfoData;
  UINT8                 Index;

  DEBUG((EFI_D_INFO, "[SPS] Getting Info from PEI\n"));

  if (!MeTypeIsSps())
  {
    return EFI_UNSUPPORTED;
  }

  Status = EFI_ERROR (1);
  for (Index = 0; Index < SystemTable->NumberOfTableEntries; Index++)
  {
    CopyMem (&TempGUID, &(SystemTable->ConfigurationTable[Index].VendorGuid), sizeof (EFI_GUID));
    if (CompareGuid (&gEfiHobListGuid, &TempGUID))
    {
      HobList = SystemTable->ConfigurationTable[Index].VendorTable;
      Status  = EFI_SUCCESS;
      break;
    }
  }

  if (EFI_ERROR(Status))
  {
    DEBUG((EFI_D_ERROR, "[SPS] ERROR: Cannot locate any HOB\n"));
    return EFI_UNSUPPORTED;
  }

  DEBUG((EFI_D_INFO, "[SPS] Looking for SPS HOB info from PEI\n"));

  GuidHob = GetFirstGuidHob (&gEfiSpsInfoHobGuid);
  if (GuidHob == NULL)
  {
    DEBUG((EFI_D_ERROR, "[SPS] ERROR: Cannot locate SPS HOB\n"));
    return EFI_UNSUPPORTED;
  }
  pInfoData = GET_GUID_HOB_DATA (GuidHob);
  if (pInfoData == NULL)
  {
    DEBUG((EFI_D_ERROR, "[SPS] ERROR: Cannot get pointer to SPS HOB data\n"));
    return EFI_UNSUPPORTED;
  }

  DEBUG((EFI_D_INFO, "[SPS] HOB: flow %d, feature set 0x%04X, pwr opt boot %d, cores2disable %d\n",
        pInfoData->WorkFlow, pInfoData->FeatureSet, pInfoData->PwrOptBoot, pInfoData->Cores2Disable));

  CopyMem(pHob, pInfoData, sizeof(*pHob));

  if (pInfoData->WorkFlow != ME_FUNCTIONAL)
  {
    pSpsMode->WorkFlow = pInfoData->WorkFlow;
    DEBUG((EFI_D_ERROR, "[SPS] ERROR: ME is non functional (flow %d)\n", pInfoData->WorkFlow));
    Status = EFI_UNSUPPORTED;
    return Status;
  }
  pSpsMode->WorkFlow        = pInfoData->WorkFlow;
  pSpsMode->FeatureSet.Data = pInfoData->FeatureSet.Data;

  if (!pSpsMode->FeatureSet.Bits.NodeManager)
  {
    pSpsMode->Mode = SIEN_MODE;
  } else {
    pSpsMode->Mode = NM_MODE;
  }

  return EFI_SUCCESS;
} // GetSpsConfiguration()


/**
    SPS DXE driver entry point.
    @param ImageHandle - handle to SPS DXE image
    @param pSystemTable - pointer to system table
    @retval Void.
**/
EFI_STATUS
SpsDxeEntryPoint (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *pSystemTable
  )
{
  EFI_STATUS            Status;
  EFI_HOB_TYPE_SPS_INFO HobInfo;
  DXE_ME_POLICY_PROTOCOL *MePolicy = NULL;

  DEBUG((EFI_D_ERROR | EFI_D_INFO, "[SPS] DXE PHASE \n"));

  Status = gBS->AllocatePool(EfiBootServicesData, sizeof(*mpSpsContext), (VOID**)&mpSpsContext);
  if (EFI_ERROR(Status))
  {
    DEBUG((EFI_D_ERROR, "[SPS] ERROR: Memory allocation failed (%r). Driver will be unloaded.\n", Status));
    return Status;
  }
  SetMem(mpSpsContext, sizeof(*mpSpsContext), 0);

  Status = gBS->LocateProtocol (&gDxePlatformMePolicyProtocolGuid, NULL, &mpSpsContext->MePolicy);

  if (EFI_ERROR(Status))
  {
    DEBUG((EFI_D_ERROR, "[SPS] ERROR: Cannot get MePolicy configuration (%r)\n"
                        "      Using hardcoded defaults\n", Status));
    Status = gBS->AllocatePool(EfiBootServicesData, sizeof(DXE_ME_POLICY_PROTOCOL), (VOID**)&MePolicy);
    if (EFI_ERROR(Status)) {
      FreePool(mpSpsContext);
      DEBUG((EFI_D_ERROR, "[SPS] ERROR: Cannot create default MePolicy configuration (%r). Driver will be unloaded.\n", Status));
      ASSERT_EFI_ERROR(Status);
      return Status;
    }
    mpSpsContext->MePolicy = MePolicy;
    SetMem(mpSpsContext->MePolicy, sizeof(DXE_ME_POLICY_PROTOCOL), 0);
    mpSpsContext->MePolicy->MeNmConfig.MeHmrfpoLockEnabled = TRUE;
    mpSpsContext->MePolicy->MeNmConfig.MeGrLockEnabled = TRUE;
    mpSpsContext->MePolicy->MeConfig.EndOfPostEnabled = TRUE;

    mpSpsContext->MePolicy->MeConfig.HeciCommunication1 = SETUP_HECI_DEVICE_AUTO;
    mpSpsContext->MePolicy->MeConfig.HeciCommunication2 = SETUP_HECI_DEVICE_AUTO;
    mpSpsContext->MePolicy->MeConfig.HeciCommunication3 = SETUP_HECI_DEVICE_AUTO;
    mpSpsContext->MePolicy->MeConfig.MeIderEnabled = SETUP_HECI_DEVICE_DISABLED;
    mpSpsContext->MePolicy->MeConfig.MeKtEnabled = SETUP_HECI_DEVICE_DISABLED;

    // Policy functions
    mpSpsContext->MePolicy->saveMePolicyToSetupConfiguration = saveMePolicyToSetupConfiguration;
  }

  Status = GetSpsConfiguration(&mpSpsContext->SpsMode, pSystemTable, &HobInfo);
  if (EFI_ERROR(Status))
  {
    DEBUG((EFI_D_ERROR, "[SPS] ERROR: Cannot get SPS configuration. Driver will be unloaded.\n"));
    if (MePolicy) {
      FreePool(MePolicy);
    }
    FreePool(mpSpsContext);
    return Status;
  }


  if (GetBootModeHob() != BOOT_ON_S4_RESUME &&
      (mpSpsContext->MePolicy->MeNmConfig.NmPwrOptBoot != HobInfo.PwrOptBoot ||
       mpSpsContext->MePolicy->MeNmConfig.NmCores2Disable != HobInfo.Cores2Disable))
  {
    if (!mpSpsContext->MePolicy->MeNmConfig.NmPwrOptBootOverride)
    {
      DEBUG((EFI_D_INFO, "[SPS] Updating NM boot mode %d->%d\n",
               mpSpsContext->MePolicy->MeNmConfig.NmPwrOptBoot, HobInfo.PwrOptBoot));
      mpSpsContext->MePolicy->MeNmConfig.NmPwrOptBoot = HobInfo.PwrOptBoot;
      Status = !EFI_SUCCESS;
    }
    if (!mpSpsContext->MePolicy->MeNmConfig.NmCores2DisableOverride)
    {
      DEBUG((EFI_D_INFO, "[SPS] Updating boot mode %d->%d, cores to disable %d -> %d\n",
               mpSpsContext->MePolicy->MeNmConfig.NmPwrOptBoot, HobInfo.PwrOptBoot,
               mpSpsContext->MePolicy->MeNmConfig.NmCores2Disable, HobInfo.Cores2Disable));
      mpSpsContext->MePolicy->MeNmConfig.NmCores2Disable = HobInfo.Cores2Disable;
      Status = !EFI_SUCCESS;
    }
    if (Status != EFI_SUCCESS)
    {
      if (mpSpsContext->MePolicy->saveMePolicyToSetupConfiguration) {
        Status = mpSpsContext->MePolicy->saveMePolicyToSetupConfiguration(mpSpsContext->MePolicy);
      } else {
        Status = EFI_INVALID_PARAMETER;
      }

      ASSERT_EFI_ERROR(Status);
    }
  }

  if (!mpSpsContext->SpsMode.FeatureSet.Bits.NodeManager)
  {
    EFI_SPS_SIEN_PROTOCOL *SiEnProtocol;
    Status = gBS->AllocatePool (
                      EfiReservedMemoryType,
                      sizeof (EFI_SPS_SIEN_PROTOCOL),
                      &SiEnProtocol
                      );
    ASSERT_EFI_ERROR (Status);

    Status = gBS->InstallProtocolInterface (
                      &ImageHandle,
                      &gEfiSpsSiEnProtocolGuid,
                      EFI_NATIVE_INTERFACE,
                      SiEnProtocol
                      );
    ASSERT_EFI_ERROR (Status);
    DEBUG((EFI_D_INFO, "[SPS] SiliconEnabling Mode\n"));
  } // if (!NodeManager)

  if (mpSpsContext->SpsMode.FeatureSet.Bits.NodeManager)
  {
    EFI_SPS_NM_PROTOCOL *pNmProtocol;
    GatherCPUInfoData(mpSpsContext);

    Status = gBS->AllocatePool (
                      EfiReservedMemoryType,
                      sizeof (EFI_SPS_NM_PROTOCOL),
                      &pNmProtocol
                      );

    Status = gBS->InstallProtocolInterface (
                      &ImageHandle,
                      &gEfiSpsNmProtocolGuid,
                      EFI_NATIVE_INTERFACE,
                      pNmProtocol
                      );
    DEBUG((EFI_D_INFO, "[SPS] NM Mode \n"));
  }

  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  SpsEndOfDxeCallback,
                  mpSpsContext,
                  &gEfiEndOfDxeEventGroupGuid,
                  &mSpsEndOfDxeEvent
                  );

  ASSERT_EFI_ERROR(Status);

  Status = EfiCreateEventReadyToBootEx(TPL_CALLBACK, SpsReadyToBootCallback,
                                       mpSpsContext, &mSpsReadyToBootEvent);
  ASSERT_EFI_ERROR(Status);

  return EFI_SUCCESS;
} // SpsDxeEntryPoint()
