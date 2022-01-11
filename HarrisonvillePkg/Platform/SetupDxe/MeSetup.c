/**
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**
@file

Copyright (c) 2011 - 2016, Intel Corporation. All rights reserved.
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

  Name:
    MeSetup.c

  Description:
    This file contains functions that update ME related strings in BIOS setup.

**/
#if defined(ME_SUPPORT_FLAG) && ME_SUPPORT_FLAG

#include "SetupPlatform.h"
#include <Library/PciLib.h>
#include <Library/HiiLib.h>
#include <Protocol/HeciProtocol.h>
#include <Protocol/MePlatformPolicyProtocol.h>
#include <Sps.h>
#include <Guid/SpsInfoHobGuid.h>
#include <Library/MeTypeLib.h>
#include <MeAccess.h>
#include <MeState.h>
#include <MeSetup.h>
#include <Library/MePlatformPolicyUpdateDxeLib.h>
#include "Guid/HiiSetupEnter.h" // Denverton AptioV override - EIP#264461

#define ME_SETUP_DEFAULT_STR_SIZE (UINTN)0x200

#define CheckMeStateForHeciCommunication(CurrentState) \
   ((MEFS1_CURSTATE_NORMAL == CurrentState) || \
    (MEFS1_CURSTATE_RECOVERY == CurrentState) || \
    (MEFS1_CURSTATE_TRANSITION == CurrentState))

/**
  This function updates ME FW Type in BIOS Setup

  printed.

  @param[in] None

  @return Void
**/

VOID UpdateMeFwTypeStrings(VOID)
{
#if defined(SPS_SUPPORT) && SPS_SUPPORT
  CHAR16 *MeTypeString = NULL;

  DEBUG((EFI_D_INFO, "[ME] UpdateMeFwType() called\n"));

  MeTypeString = HiiGetString(mAdvancedHiiHandle, STR_ME_FW_TYPE_SPS, NULL);
  if (MeTypeString) {
    HiiSetString(mAdvancedHiiHandle, STRING_TOKEN(STR_ME_FW_TYPE_VALUE), MeTypeString, NULL);
    FreePool (MeTypeString);
  }
#else
  return;
#endif
}

/**
  This function updates ME FW Status #1 and ME FW Status #2 in BIOS Setup

  @param[in] CurrentState ME FW Current State

  @return Void
**/
VOID UpdateMeFwStatusStrings(
  UINT32 MeFs1Data,
  UINT32 MeFs2Data
  )
{
  CHAR16    *pStrBuf =NULL;

  DEBUG((EFI_D_INFO, "[ME] UpdateMeFwStatus() called.\n"));
  DEBUG((EFI_D_INFO, "[ME] MEFS1: 0x%08X, MEFS2: 0x%08X\n", MeFs1Data, MeFs2Data));

  pStrBuf = AllocateZeroPool(ME_SETUP_DEFAULT_STR_SIZE);
  ASSERT(NULL != pStrBuf);

  if(NULL != pStrBuf)
  {
    UnicodeSPrint(pStrBuf, ME_SETUP_DEFAULT_STR_SIZE, L"0x%08X", MeFs1Data);
    HiiSetString(mAdvancedHiiHandle, STRING_TOKEN(STR_ME_FWSTS1_VALUE), pStrBuf, NULL);

    UnicodeSPrint(pStrBuf, ME_SETUP_DEFAULT_STR_SIZE, L"0x%08X", MeFs2Data);
    HiiSetString(mAdvancedHiiHandle, STRING_TOKEN(STR_ME_FWSTS2_VALUE), pStrBuf, NULL);

    FreePool(pStrBuf);
    pStrBuf = NULL;
  }
}

/**
  This function updates ME Current State in BIOS Setup

  @param[in] CurrentState ME FW Current State

  @return Void
**/
VOID UpdateMeFwCurrentStateStrings(UINT8 CurrentState)
{
  CHAR16    *pStrBuf =NULL;

  DEBUG((EFI_D_INFO, "[ME] UpdateMeCurrentState() called.\n"));

  pStrBuf = AllocateZeroPool(ME_SETUP_DEFAULT_STR_SIZE);
  ASSERT(NULL != pStrBuf);

  if(NULL != pStrBuf)
  {
    switch(CurrentState)
    {
      case MEFS1_CURSTATE_RESET:
        UnicodeSPrint(pStrBuf, ME_SETUP_DEFAULT_STR_SIZE, L"Reset");
        break;

      case MEFS1_CURSTATE_INIT:
        UnicodeSPrint(pStrBuf, ME_SETUP_DEFAULT_STR_SIZE, L"Initializing");
        break;

      case MEFS1_CURSTATE_RECOVERY:
        UnicodeSPrint(pStrBuf, ME_SETUP_DEFAULT_STR_SIZE, L"Recovery");
        break;

      case MEFS1_CURSTATE_DISABLED:
        UnicodeSPrint(pStrBuf, ME_SETUP_DEFAULT_STR_SIZE, L"Disabled");
        break;

      case MEFS1_CURSTATE_NORMAL:
        UnicodeSPrint(pStrBuf, ME_SETUP_DEFAULT_STR_SIZE, L"Operational");
        break;

      case MEFS1_CURSTATE_TRANSITION:
        UnicodeSPrint(pStrBuf, ME_SETUP_DEFAULT_STR_SIZE, L"Transitioning");
        break;

      case 0xF:
        UnicodeSPrint(pStrBuf, ME_SETUP_DEFAULT_STR_SIZE, L"DFX Firmware");
        HiiSetString(mAdvancedHiiHandle, STRING_TOKEN(STR_ME_FWVER_OPERATIONAL_VALUE),pStrBuf,NULL);
        HiiSetString(mAdvancedHiiHandle, STRING_TOKEN(STR_ME_FW_TYPE_VALUE), pStrBuf, NULL);
        FreePool(pStrBuf);
        pStrBuf = NULL;
        return;

      default:
        UnicodeSPrint(pStrBuf, ME_SETUP_DEFAULT_STR_SIZE, L"Unknown(%d)", CurrentState);
        break;
    }

    HiiSetString(mAdvancedHiiHandle, STRING_TOKEN(STR_ME_FWCURSTE_VALUE), pStrBuf, NULL);

    FreePool(pStrBuf);
    pStrBuf = NULL;
  }
}

/**
  This function updates ME Error Code in BIOS Setup

  @param[in] ErrorCode ME FW Error Code

  @return Void
**/
VOID UpdateMeFwErrorCodeStrings(UINT8 ErrorCode)
{
  CHAR16    *pStrBuf =NULL;

  DEBUG((EFI_D_INFO, "[ME] UpdateMeErrorCode() called.\n"));

  pStrBuf = AllocateZeroPool(ME_SETUP_DEFAULT_STR_SIZE);
  ASSERT(NULL != pStrBuf);

  if(NULL != pStrBuf)
  {
    switch(ErrorCode)
    {
      case MEFS1_ERRCODE_NOERROR:
        UnicodeSPrint(pStrBuf, ME_SETUP_DEFAULT_STR_SIZE, L"No Error");
        break;

      case MEFS1_ERRCODE_UNKNOWN:
        UnicodeSPrint(pStrBuf, ME_SETUP_DEFAULT_STR_SIZE, L"Uncategorized Failure");
        break;

      case MEFS1_ERRCODE_DISABLED:
        UnicodeSPrint(pStrBuf, ME_SETUP_DEFAULT_STR_SIZE, L"Disabled");
        break;

      case MEFS1_ERRCODE_IMAGE_FAIL:
        UnicodeSPrint(pStrBuf, ME_SETUP_DEFAULT_STR_SIZE, L"Image Failure");
        break;

      default:
        UnicodeSPrint(pStrBuf, ME_SETUP_DEFAULT_STR_SIZE, L"Unknown(%d)", ErrorCode);
        break;
    }

    HiiSetString(mAdvancedHiiHandle, STRING_TOKEN(STR_ME_FWERRCODE_VALUE), pStrBuf, NULL);

    FreePool(pStrBuf);
    pStrBuf = NULL;
  }
}

/**
  This function updates ME FW Version in BIOS Setup

  @param[in] pHeciProtocol HECI protocol pointer

  @return Void
**/
VOID UpdateMeFwVersionStrings(
  HECI_PROTOCOL *pHeciProtocol
  )
{
  EFI_STATUS  Status;
  CHAR16      *pStrBuf =NULL;
  UINT32      RequestLength;
  UINT32      ResponseLength;
  union {
    MKHI_MSG_GET_FW_VERSION_REQ      FwVerReq;
    MKHI_MSG_GET_FW_VERSION_RSP      FwVerRsp;
  } HeciMsg;

  DEBUG((EFI_D_INFO, "[ME] UpdateMeFwVersionStrings() called.\n"));

  if(NULL == pHeciProtocol)
  {
    DEBUG((EFI_D_ERROR, "[ME] UpdateMeFwVersionStrings() input parameter invalid (pHeciProtocl).\n"));
    return;
  }

  //
  // Send MKHI_GET_FW_VERSION request to ME
  //
  DEBUG((EFI_D_INFO, "[ME] Sending MKHI_GET_FW_VERSION request to ME\n"));

  gBS->SetMem(&HeciMsg.FwVerReq, sizeof(HeciMsg.FwVerReq), 0);
  HeciMsg.FwVerReq.Mkhi.Fields.GroupId = MKHI_GRP_GEN;
  HeciMsg.FwVerReq.Mkhi.Fields.Command = MKHI_CMD_GET_FW_VERSION;

  RequestLength = sizeof(HeciMsg.FwVerReq);
  ResponseLength = sizeof(HeciMsg.FwVerRsp);

  Status = pHeciProtocol->SendwACK(
                                   HECI1_DEVICE,
                                   (UINT32*)&HeciMsg.FwVerReq,
                                   RequestLength,
                                   &ResponseLength,
                                   SPS_CLIENTID_BIOS,
                                   SPS_CLIENTID_ME_MKHI
                                   );

  if(EFI_ERROR(Status))
  {
    DEBUG((EFI_D_ERROR, "[ME] ERROR: Cannot send MKHI_GET_FW_VERSION (status: %r)\n", Status));
  }
  else if(!((HeciMsg.FwVerRsp.Mkhi.Fields.IsResponse) &&
           (MKHI_GRP_GEN == HeciMsg.FwVerReq.Mkhi.Fields.GroupId) &&
           (MKHI_CMD_GET_FW_VERSION == HeciMsg.FwVerReq.Mkhi.Fields.Command)))
  {
    DEBUG((EFI_D_ERROR, "[ME] ERROR: Invalid MKHI_GET_FW_VERSION response (MKHI: 0x%X)\n", HeciMsg.FwVerRsp.Mkhi.Data));
  }
  else
  {
    pStrBuf = AllocateZeroPool(ME_SETUP_DEFAULT_STR_SIZE);
    ASSERT(NULL != pStrBuf);

    if(NULL != pStrBuf)
    {
      DEBUG((EFI_D_INFO, "[ME] Firmware version is: \n"
                         "             Operational: %02X:%d.%d.%d.%d\n"
                         "                Recovery: %02X:%d.%d.%d.%d\n",
                         HeciMsg.FwVerRsp.Act.Major >> 8,     /* get older byte   = productionId */
                         HeciMsg.FwVerRsp.Act.Major & 0x00FF, /* get younger byte = ME major ver. */
                         HeciMsg.FwVerRsp.Act.Minor,
                         HeciMsg.FwVerRsp.Act.Patch,
                         HeciMsg.FwVerRsp.Act.Build,
                         HeciMsg.FwVerRsp.Rcv.Major >> 8,     /* get older byte   = productionId */
                         HeciMsg.FwVerRsp.Rcv.Major & 0x00FF, /* get younger byte = ME major ver. */
                         HeciMsg.FwVerRsp.Rcv.Minor,
                         HeciMsg.FwVerRsp.Rcv.Patch,
                         HeciMsg.FwVerRsp.Rcv.Build));
      UnicodeSPrint(pStrBuf, ME_SETUP_DEFAULT_STR_SIZE, L"%02X:%d.%d.%d.%d",
                    HeciMsg.FwVerRsp.Act.Major >> 8,     /* get older byte   = productionId */
                    HeciMsg.FwVerRsp.Act.Major & 0x00FF, /* get younger byte = ME major ver. */
                    HeciMsg.FwVerRsp.Act.Minor,
                    HeciMsg.FwVerRsp.Act.Patch, HeciMsg.FwVerRsp.Act.Build);

      HiiSetString(mAdvancedHiiHandle, STRING_TOKEN(STR_ME_FWVER_OPERATIONAL_VALUE),pStrBuf,NULL);

      //
      // Update ME firmware info in platform information menu
      //
        // Denverton TODO start
        // STR_ME_REVISION_VALUE is not present in any form. If added in future need to check again
        // HiiSetString(mMainHiiHandle, STRING_TOKEN(STR_ME_REVISION_VALUE), pStrBuf, NULL);
        // Denverton TODO end

      UnicodeSPrint(pStrBuf, ME_SETUP_DEFAULT_STR_SIZE, L"%02X:%d.%d.%d.%d",
                    HeciMsg.FwVerRsp.Rcv.Major >> 8,     /* get older byte   = productionId */
                    HeciMsg.FwVerRsp.Rcv.Major & 0x00FF, /* get younger byte = ME major ver. */
                    HeciMsg.FwVerRsp.Rcv.Minor,
                    HeciMsg.FwVerRsp.Rcv.Patch,
                    HeciMsg.FwVerRsp.Rcv.Build);

      HiiSetString(mAdvancedHiiHandle, STRING_TOKEN(STR_ME_FWVER_RECOVERY_VALUE), pStrBuf, NULL);

      //
      // ME in dual-image configuration provides the version of the backup image yet
      //
      if (ResponseLength > sizeof(HeciMsg.FwVerRsp) - sizeof(HeciMsg.FwVerRsp.Bkp))
      {
        DEBUG((EFI_D_INFO, "                  Backup: %02X:%d.%d.%d.%d\n",
                           HeciMsg.FwVerRsp.Bkp.Major >> 8,     /* get older byte   = productionId */
                           HeciMsg.FwVerRsp.Bkp.Major & 0x00FF, /* get younger byte = ME major ver. */
                           HeciMsg.FwVerRsp.Bkp.Minor,
                           HeciMsg.FwVerRsp.Bkp.Patch,
                           HeciMsg.FwVerRsp.Bkp.Build));
        UnicodeSPrint(pStrBuf, ME_SETUP_DEFAULT_STR_SIZE, L"%02X:%d.%d.%d.%d",
                      HeciMsg.FwVerRsp.Bkp.Major >> 8,     /* get older byte   = productionId */
                      HeciMsg.FwVerRsp.Bkp.Major & 0x00FF, /* get younger byte = ME major ver. */
                      HeciMsg.FwVerRsp.Bkp.Minor,
                      HeciMsg.FwVerRsp.Bkp.Patch,
                      HeciMsg.FwVerRsp.Bkp.Build);

        HiiSetString(mAdvancedHiiHandle, STRING_TOKEN(STR_ME_FWVER_BACKUP_VALUE), pStrBuf, NULL);
      }

      FreePool(pStrBuf);
      pStrBuf = NULL;
    }
  }
}

#if defined(SPS_SUPPORT) && SPS_SUPPORT
/**
  This function updates ME FW SKU Info in BIOS Setup

  @param[in] pHeciProtocol HECI protocol pointer

  @return Void
**/
VOID UpdateMeSkuInfoStrings(
  HECI_PROTOCOL *pHeciProtocol
  )
{
  EFI_STATUS          Status;
  CHAR16              *pStrBuf =NULL;
  UINT32              RequestLength;
  UINT32              ResponseLength;
  SPS_FEATURE_SET     FeatureSet;
  ME_RC_CONFIGURATION MeRcConfiguration;
  union {
    SPS_MSG_GET_MEBIOS_INTERFACE_REQ MeBiosIfReq;
    SPS_MSG_GET_MEBIOS_INTERFACE_RSP MeBiosIfRsp;
  } HeciMsg;

  DEBUG((EFI_D_INFO, "[ME] UpdateMeSkuInfoStrings() called.\n"));

  if(NULL == pHeciProtocol)
  {
    DEBUG((EFI_D_ERROR, "[ME] UpdateMeSkuInfoStrings() input parameter invalid (pHeciProtocl).\n"));
    return;
  }

  //
  // Send SPS_CMD_GET_MEBIOS_INTERFACE request to ME
  //
  DEBUG((EFI_D_INFO, "[ME] Sending SPS_GET_MEBIOS_INTERFACE request to ME\n"));

  gBS->SetMem(&HeciMsg.MeBiosIfReq, sizeof(HeciMsg.MeBiosIfReq), 0);
  HeciMsg.MeBiosIfReq.Command.Data = SPS_CMD_GET_MEBIOS_INTERFACE_REQ;

  RequestLength = sizeof(HeciMsg.MeBiosIfReq);
  ResponseLength = sizeof(HeciMsg.MeBiosIfRsp);

  Status = pHeciProtocol->SendwACK(
                                  HECI1_DEVICE,
                                  (UINT32*)&HeciMsg.MeBiosIfReq,
                                  RequestLength,
                                  &ResponseLength,
                                  SPS_CLIENTID_BIOS,
                                  SPS_CLIENTID_ME_SPS
                                  );

  if (EFI_ERROR(Status))
  {
    DEBUG((EFI_D_ERROR, "[ME] ERROR: Cannot send SPS_GET_MEBIOS_INTERFACE (status: %r)\n",
                        Status));
  }
  else if((ResponseLength != sizeof(SPS_MSG_GET_MEBIOS_INTERFACE_RSP)) &&
          (ResponseLength != sizeof(SPS_MSG_GET_MEBIOS_INTERFACE_RSP) - sizeof(UINT32))) // TODO: Remove this after FW transition to new format
  {
    DEBUG((EFI_D_ERROR, "[ME] ERROR: Invalid SPS_GET_MEBIOS_INTERFACE response length (length %d)\n", ResponseLength));
  }
  else
  {
    pStrBuf = AllocateZeroPool(ME_SETUP_DEFAULT_STR_SIZE);
    ASSERT(NULL != pStrBuf);

    if(NULL != pStrBuf)
    {
      //
      // Fill the ME firmware SKU info in setup
      //
      FeatureSet.Data = HeciMsg.MeBiosIfRsp.FeatureSet;

      UnicodeSPrint(
        pStrBuf,
        ME_SETUP_DEFAULT_STR_SIZE,
        L"SiEn\n%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s",
        FeatureSet.Bits.NodeManager                 ? L"NM\n" : L"",
        FeatureSet.Bits.PeciProxy                   ? L"PECIProxy\n" : L"",
        FeatureSet.Bits.ICC                         ? L"ICC\n" : L"",
        FeatureSet.Bits.MeStorageServices           ? L"MeStorageServices\n" : L"",
        FeatureSet.Bits.BootGuard                   ? L"BootGuard\n" : L"",
        FeatureSet.Bits.PTT                         ? L"PTT\n" : L"",
        FeatureSet.Bits.OemDefinedCpuDebugPolicy    ? L"OemDefinedCpuDebugPolicy\n" : L"",
        FeatureSet.Bits.ResetSuppression            ? L"ResetSuppression\n" : L"",
        FeatureSet.Bits.PmBusProxy                  ? L"PmBusProxy\n" : L"",
        FeatureSet.Bits.CpuHotPlug                  ? L"CpuHotPlug\n" : L"",
        FeatureSet.Bits.MicProxy                    ? L"MicProxy\n" : L"",
        FeatureSet.Bits.MctpProxy                   ? L"MctpProxy\n" : L"",
        FeatureSet.Bits.ThermalReportVolumeAirflow  ? L"ThermalReport\n" : L"",
        FeatureSet.Bits.SocThermalReporting         ? L"SocThermalReporting\n" : L"",
        FeatureSet.Bits.DualBIOSSupport             ? L"DualBiosImage\n" : L"",
        FeatureSet.Bits.MPHYSurvivabilityProgramming? L"HSIO\n" : L"",
        FeatureSet.Bits.PECIOverDMI                 ? L"PECIOverDMI\n" : L"",
        FeatureSet.Bits.PCHDebug                    ? L"PCHDebug\n" : L"",
        FeatureSet.Bits.PowerThermalUtilitySupport  ? L"PowerThermalUtility\n" : L"",
        FeatureSet.Bits.FiaMuxConfiguration         ? L"FiaMuxConfiguration\n" : L"",
        FeatureSet.Bits.PchThermalSensorInit        ? L"PCHThermalSensorInit\n" : L"",
        FeatureSet.Bits.DeepSx                      ? L"DeepSx\n" : L"",
        FeatureSet.Bits.DualMeFwImage               ? L"DualMeImage\n" : L"",
        FeatureSet.Bits.DirectFwUpdate              ? L"DirectMeUpdate\n" : L"",
        FeatureSet.Bits.MctpInfrastructure          ? L"MctpInfrastructure\n" : L"",
        FeatureSet.Bits.CUPS                        ? L"CUPS\n" : L"",
        FeatureSet.Bits.FlashDescRegionVerification ? L"FlashDescRegionVerification\n" : L"",
        FeatureSet.Bits.TurboStateLimiting          ? L"TurboStateLimiting\n" : L"",
        FeatureSet.Bits.TelemetryHub                ? L"TelemetryHub\n" : L"",
        FeatureSet.Bits.MeShutdown                  ? L"MeShutdown\n" : L"",
        FeatureSet.Bits.ASA                         ? L"ASA" : L"",
        FeatureSet.Bits.WarmResetNotificationSubFlow? L"WarmResetNotificationSubFlow" : L""
        );

      HiiSetString(mAdvancedHiiHandle, STRING_TOKEN(STR_ME_FWSKU_VALUE), pStrBuf, NULL);

      Status = GetMeSetupConfig(&MeRcConfiguration);
      if (!EFI_ERROR(Status))
      {
        // SPS FW is running, set NM state value (NM Debug
        // Configuration Menu Items visibility depends off the NM State)
        MeRcConfiguration.NmEnabled = (UINT8)FeatureSet.Bits.NodeManager;

        // If ICC Clock Settings are not supported by the SPS FW setup opion will be suppressed
        MeRcConfiguration.SpsIccClkSscSettingSupported = (UINT8)FeatureSet.Bits.ICC;

        Status = SetMeSetupConfig(&MeRcConfiguration);
        if (EFI_ERROR(Status))
        {
          DEBUG((EFI_D_ERROR, "[ME] Can't update SPS NM status (%r).\n", Status));
        }
      }
      else
      {
        DEBUG((EFI_D_ERROR, "[ME] Can't get SPS NM status variable (%r).\n", Status));
      }

      FreePool(pStrBuf);
      pStrBuf = NULL;
    }
  }
}
#endif // SPS_SUPPORT

/**
  This function updates ME related strings in BIOS setup database.

  Some of the strings contain dummy values to be update with run-time date.
  These strings are updated by this function before any setup screen is
  printed.

  @param[in] None

  @return Void
**/
VOID InitMeSetupStrings(VOID)
{
  EFI_STATUS    Status;
  HECI_PROTOCOL *pHeciProtocol;
  SPS_MEFS1     MeFs1;
  SPS_MEFS2     MeFs2;
  UINT32        MeMode = ME_MODE_TEMP_DISABLED;

  DEBUG((EFI_D_INFO, "[ME] InitMeSetupStrings called.\n"));

  MeFs1.UInt32 = HeciPciRead32(SPS_REG_MEFS1);
  MeFs2.UInt32 = HeciPciRead32(SPS_REG_MEFS2);

  // Update ME FW type
  UpdateMeFwTypeStrings();

  // Update ME FW Status
  UpdateMeFwStatusStrings(MeFs1.UInt32, MeFs2.UInt32);

  // Update ME FW Current State
  UpdateMeFwCurrentStateStrings((UINT8)MeFs1.Bits.CurrentState);

  // Update ME FW Error Code
  UpdateMeFwErrorCodeStrings((UINT8)MeFs1.Bits.ErrorCode);

  // Check if ME will respond for HECI commands (i.e ME in operational / recovery)
  if(CheckMeStateForHeciCommunication((UINT8)MeFs1.Bits.CurrentState))
  {
    Status = gBS->LocateProtocol(&gHeciProtocolGuid, NULL, &pHeciProtocol);
    if(EFI_ERROR(Status))
    {
      DEBUG((EFI_D_ERROR, "[ME] ERROR: Cannot locate HECI protocol (status: %r)\n", Status));
    }
    else
    {
      Status = pHeciProtocol->GetMeMode (&MeMode);
      if(EFI_ERROR(Status))
      {
        DEBUG((EFI_D_ERROR, "[ME] ERROR: Cannot retrieve ME mode (status: %r)\n", Status));
      }
      else if (MeMode == ME_MODE_TEMP_DISABLED)
      {
        DEBUG((EFI_D_INFO, "[ME] ME is in TEMP DISABLED MODE (don't send messages).\n"));
      }
      else
      {
        // Update ME FW version (Send MKHI_GET_FW_VERSION request to ME)
        UpdateMeFwVersionStrings(pHeciProtocol);

#if defined(SPS_SUPPORT) && SPS_SUPPORT
        // Update the ME firmware SKU info in setup (Send SPS_GET_MEBIOS_INTERFACE request to ME)
        UpdateMeSkuInfoStrings(pHeciProtocol);
#endif // SPS_SUPPORT
      }
    }
  }
}

/**

    GC_TODO: add routine description

    @param MeRcConfiguration - GC_TODO: add arg description

    @retval EFI_SUCCESS - GC_TODO: add retval description

**/
EFI_STATUS EFIAPI MeAdvancedRouteConfig (
  IN OUT ME_RC_CONFIGURATION *MeRcConfiguration
)
{
  EFI_STATUS             Status;
  DXE_ME_POLICY_PROTOCOL *MePlatformPolicy;
  BOOLEAN                ResetRequests = FALSE;

  DEBUG((EFI_D_INFO, "[ME] MeAdvancedRouteConfig() called\n"));

  // Me Policy Needs to be updated
  Status = gBS->LocateProtocol (&gDxePlatformMePolicyProtocolGuid, NULL, &MePlatformPolicy);
  if (!EFI_ERROR(Status)) {
    Status = MePlatformPolicy->UpdateMePolicyFromSetup(MePlatformPolicy, (VOID *)MeRcConfiguration, &ResetRequests);
  }

  return EFI_SUCCESS;
}

#endif // ME_SUPPORT_FLAG
