/**

Copyright (c) 1996 - 2016, Intel Corporation.

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


  @file CallBacks.c

   This file contains callbacks of events that SPS is interested in.

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
#include <Library/PciLib.h>
#include <Library/S3BootScriptLib.h>

#include <Library/DebugLib.h>
#include <Library/PrintLib.h>

#include <Guid/GlobalVariable.h>

#include <Guid/HobList.h>
#include <Guid/EventLegacyBios.h>
#include <Guid/SpsInfoHobGuid.h>

#include <Protocol/MpService.h>
#include <Protocol/ExitPmAuth.h>
#include "Protocol/PchReset.h"

#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/CpuLib.h>
#include <Library/MePolicyLib.h>
#include <Library/MeTypeLib.h>

#include <Protocol/CpuIo.h>
#include <Protocol/HeciProtocol.h>
#include <Protocol/PciIo.h>

#include <Sps.h>
#include <PchAccess.h>

#include <SpsMisc.h>
#include <HeciRegs.h>
#include <MeAccess.h>
#include <MeSetup.h>

#include "SpsDxe.h"
#include <Token.h>  // Denverton AptioV Override - EIP#331778

/******************************************************************************
 * Variables
 */
UINT64 mNonce = 0;


/******************************************************************************
 * Functions
 */
/**
    SPS callback for ExitPmAuth event.
    Performs actions required by SPS ME-BIOS Spec before OS is loaded.

    @param Event    - the event, not used
    @param pContext - pointer to SPS firmware description

    @retval Void.
**/
VOID SpsEndOfDxeCallback(
  IN EFI_EVENT      Event,
  IN VOID          *pContext)
{
  EFI_STATUS        Status;
  UINT32            Data32[2];
  UINT8             Data8;
  UINT32            RspLen;
  HECI_PROTOCOL    *pHeci;
  SPS_DXE_CONTEXT  *pSpsContext = (SPS_DXE_CONTEXT*)pContext;

  union
  {
    MKHI_MSG_HMRFPO_LOCK_REQ   HmrfpoLockReq;
    MKHI_MSG_HMRFPO_LOCK_RSP   HmrfpoLockRsp;
    MKHI_MSG_HMRFPO_ENABLE_REQ HmrfpoEnableReq;
    MKHI_MSG_HMRFPO_ENABLE_RSP HmrfpoEnableRsp;
    MCTP_SET_BUS_OWNER_REQ     MctpSetBusOwnerReq;
    MCTP_SET_BUS_OWNER_RSP     MctpSetBusOwnerRsp;
  } HeciMsg;

  if (!MeTypeIsSps())
  {
    return;
  }

  gBS->CloseEvent (Event);

  Status = gBS->LocateProtocol(&gHeciProtocolGuid, NULL, &pHeci);
  if (EFI_ERROR (Status))
  {
    DEBUG((DEBUG_ERROR, "[SPS] ERROR: Cannot locate HECI protocol (%r)\n", Status));
  }
  else
  {
    //
    // Send MCPT Bus Owner Proxy configuration if set
    //
    if (pSpsContext->MePolicy->MeNmConfig.SpsMctpBusOwner != 0 &&
        pSpsContext->SpsMode.FeatureSet.Bits.MctpProxy)
    {
      DEBUG((DEBUG_INFO, "[SPS] Sending MCTP_SET_BUS_OWNER with address 0x%X\n",
             pSpsContext->MePolicy->MeNmConfig.SpsMctpBusOwner));
      HeciMsg.MctpSetBusOwnerReq.Command = MCPT_CMD_SET_BUS_OWNER_REQ;
      HeciMsg.MctpSetBusOwnerReq.Reserved0[0] =
      HeciMsg.MctpSetBusOwnerReq.Reserved0[1] =
      HeciMsg.MctpSetBusOwnerReq.Reserved0[2] = 0;
      HeciMsg.MctpSetBusOwnerReq.PCIeAddress = pSpsContext->MePolicy->MeNmConfig.SpsMctpBusOwner;
      HeciMsg.MctpSetBusOwnerReq.Location = 0;
      HeciMsg.MctpSetBusOwnerReq.Reserved1 = 0;
      RspLen = sizeof(HeciMsg.MctpSetBusOwnerRsp);
      Status = pHeci->SendwACK(HECI1_DEVICE, (UINT32*)&HeciMsg.MctpSetBusOwnerReq,
                               sizeof(HeciMsg.MctpSetBusOwnerReq), &RspLen,
                               SPS_CLIENTID_BIOS, SPS_CLIENTID_ME_MCTP);
      if (EFI_ERROR(Status))
      {
        DEBUG((DEBUG_ERROR, "[SPS] ERROR: Cannot send MCTP_SET_BUS_OWNER (%r)\n", Status));
      }
      else if (HeciMsg.MctpSetBusOwnerRsp.Result != MCPT_CMD_SET_BUS_OWNER_RSP_SUCCESS)
      {
        DEBUG((DEBUG_ERROR, "[SPS] ERROR: MCTP_SET_BUS_OWNER failure (cause: %d)\n",
               HeciMsg.MctpSetBusOwnerRsp.Result));
      }
    }
    // HMRFPOLOCK command is sent from CspMeud.c at SMM End of Dxe Callback for AFU to use Nonce word.
    // Denverton AptioV Override Start - EIP#331778
    // To use SpsFpt tool to flash ME region, HMRFPO Lock and Enable commands will be sent from Callbacks.c and MEUD_SUPPORT should be 0.
    #if ME_UPDATE_DFU_METHOD == 0
    #if ME_TESTMENU_FLAG
    //
    // Send HMRFPO_LOCK request if not disabled by ME Test Menu option.
    //
    if (pSpsContext->MePolicy->MeNmConfig.MeHmrfpoLockEnabled)
#endif
    {
      DEBUG((DEBUG_INFO, "[SPS] Sending HMRFPO_LOCK to ME\n"));
      HeciMsg.HmrfpoLockReq.Mkhi.Data = 0x00000205;
      RspLen = sizeof(HeciMsg.HmrfpoLockRsp);
      Status = pHeci->SendwACK(HECI1_DEVICE, (UINT32*)&HeciMsg.HmrfpoLockReq,
                               sizeof(HeciMsg.HmrfpoLockReq), &RspLen,
                               SPS_CLIENTID_BIOS, SPS_CLIENTID_ME_MKHI);
      if (EFI_ERROR(Status))
      {
        DEBUG((DEBUG_ERROR, "[SPS] ERROR: Cannot send HMRFPO_LOCK (%r)\n", Status));
      }
      else if (HeciMsg.HmrfpoLockRsp.Mkhi.Data != 0x00008205)
      {
        DEBUG((DEBUG_ERROR, "[SPS] ERROR: Invalid HMRFPO_LOCK response (MKHI: 0x%X)\n",
                            HeciMsg.HmrfpoLockRsp.Mkhi.Data));
      }
      else if (HeciMsg.HmrfpoLockRsp.Status != 0)
      {
        DEBUG((DEBUG_ERROR, "[SPS] ERROR: HMRFPO_LOCK failed (cause: %d)\n",
                            HeciMsg.HmrfpoLockRsp.Status));
      }
      else
      {
        //
        // Store the nonce word for future use.
        //
        mNonce = HeciMsg.HmrfpoLockRsp.Nonce;

#if ME_TESTMENU_FLAG
        //
        // Send HMRFPO_ENABLE request if enabled in ME debug options
        //
        if (pSpsContext->MePolicy->MeNmConfig.MeHmrfpoEnableEnabled)
        {
          DEBUG((DEBUG_INFO, "[SPS] Sending HMRFPO_ENABLE to ME\n"));
          //
          // We are required to handle 3 retries for HMRFPO_LOCK.
          //
          for (Data8 = 3; Data8--; )
          {
            HeciMsg.HmrfpoEnableReq.Mkhi.Data = 0x00000105;
            HeciMsg.HmrfpoEnableReq.Nonce = mNonce;
            RspLen = sizeof(HeciMsg.HmrfpoEnableRsp);
            Status = pHeci->SendwACK(HECI1_DEVICE, (UINT32*)&HeciMsg.HmrfpoEnableReq,
                                     sizeof(HeciMsg.HmrfpoEnableReq), &RspLen,
                                     SPS_CLIENTID_BIOS, SPS_CLIENTID_ME_MKHI);
            if (!EFI_ERROR(Status))
            {
              if (HeciMsg.HmrfpoEnableRsp.Mkhi.Data != 0x00008105)
              {
                DEBUG((DEBUG_ERROR, "[SPS] ERROR: Invalid HMRFPO_ENABLE response (MKHI: 0x%08X), "
                                    "%d tries left\n",
                                    HeciMsg.HmrfpoEnableRsp.Mkhi.Data, Data8));
                continue;
              }
              if (HeciMsg.HmrfpoEnableRsp.Status != 0)
              {
                DEBUG((DEBUG_ERROR, "[SPS] ERROR: HMRFPO_ENABLE refused (cause: %d)\n",
                                    HeciMsg.HmrfpoEnableRsp.Status));
              }
              break;
            }
            DEBUG((DEBUG_ERROR, "[SPS] HMRFPO_ENABLE send failed (%r), %d tries left\n",
                                Status, Data8));
          }
        }
#endif // ME_TESTMENU_FLAG
      }
    } // if (SystemCfgData.HmrfpoLockEnabled)
#endif  // ME_UPDATE_DFU_METHOD
// Denverton AptioV Override End - EIP#331778  
  } // if (HECI protocol located)
  //
  // Update S3 resume script
  // S3BootScript library is using specific form of PCI address, let's define macro for it.
  //
#define S3BOOTSCRIPT_PCIADDR(Bus, Dev, Fun, Reg) \
          (UINT64)(((Bus) & 0xFF) << 24 | ((Dev) & 0x1F) << 16 | ((Fun) & 0x07) << 8 | ((Reg) & 0xFF ))
  //
  // If HECI-1 (D22 F0) is SMI or SCI mode there is no regular OS driver and
  // we must save it's configuration.
  //
  Data8 = HeciPciRead8(R_HIDM);
  if (Data8 != HECI_INTERRUPUT_GENERATE_LEGACY_MSI)
  {
    DEBUG((DEBUG_INFO, "[SPS] Saving HECI-1 configuration to S3 boot script\n"));
    S3BootScriptSavePciCfgWrite(S3BootScriptWidthUint8,
                                S3BOOTSCRIPT_PCIADDR(HECI_BUS, HECI_DEV, HECI_FUN, R_HIDM),
                                1, &Data8);
    Data32[0] = HeciPciRead32(R_HECIMBAR);
    Data32[1] = HeciPciRead32(R_HECIMBAR + 4);
    S3BootScriptSavePciCfgWrite(S3BootScriptWidthUint32,
                                S3BOOTSCRIPT_PCIADDR(HECI_BUS, HECI_DEV, HECI_FUN, R_HECIMBAR),
                                2, &Data32[0]);
    Data8 = HeciPciRead8(R_IRQ);
    S3BootScriptSavePciCfgWrite(S3BootScriptWidthUint8,
                                S3BOOTSCRIPT_PCIADDR(HECI_BUS, HECI_DEV, HECI_FUN, R_IRQ),
                                1, &Data8);
    Data8 = HeciPciRead8(R_COMMAND);
    S3BootScriptSavePciCfgWrite(S3BootScriptWidthUint8,
                                S3BOOTSCRIPT_PCIADDR(HECI_BUS, HECI_DEV, HECI_FUN, R_COMMAND),
                                1, &Data8);
  }
  //
  // If HECI-2 (D22 F1) is SMI or SCI mode there is no regular OS driver and
  // we must save it's configuration.
  //
  Data8 = Heci2PciRead8(R_HIDM);
  if (Data8 != HECI_INTERRUPUT_GENERATE_LEGACY_MSI)
  {
    DEBUG((DEBUG_INFO, "[SPS] Saving HECI-2 configuration to S3 boot script\n"));
    S3BootScriptSavePciCfgWrite(S3BootScriptWidthUint8,
                                S3BOOTSCRIPT_PCIADDR(HECI_BUS, HECI_DEV, HECI2_FUN, R_HIDM),
                                1, &Data8);
    Data32[0] = Heci2PciRead32(R_HECIMBAR);
    Data32[1] = Heci2PciRead32(R_HECIMBAR + 4);
    S3BootScriptSavePciCfgWrite(S3BootScriptWidthUint32,
                                S3BOOTSCRIPT_PCIADDR(HECI_BUS, HECI_DEV, HECI2_FUN, R_HECIMBAR),
                                2, &Data32[0]);
    Data8 = Heci2PciRead8(R_IRQ);
    S3BootScriptSavePciCfgWrite(S3BootScriptWidthUint8,
                                S3BOOTSCRIPT_PCIADDR(HECI_BUS, HECI_DEV, HECI2_FUN, R_IRQ),
                                1, &Data8);
    Data8 = Heci2PciRead8(R_COMMAND);
    if (!(Data8 & B_MSE) && (Data32[0] != 0 || Data32[1] != 0))
    {
      Data8 = B_BME | B_MSE;
      Heci2PciWrite8(R_COMMAND, Data8);
    }
    S3BootScriptSavePciCfgWrite(S3BootScriptWidthUint8,
                                S3BOOTSCRIPT_PCIADDR(HECI_BUS, HECI_DEV, HECI2_FUN, R_COMMAND),
                                1, &Data8);
  }
  return;
} // SpsExitPmAuthCallback()


/**
    SPS callback for ready-to-boot event.
    Performs actions required by SPS ME-BIOS Spec before OS is loaded.

    @param Event    - the event, not used
    @param pContext - pointer to SPS firmware description

    @retval Void.
**/
VOID SpsReadyToBootCallback(
  IN EFI_EVENT      Event,
  IN VOID           *pContext)
{
  EFI_STATUS        Status;
  UINT8             Index;
  UINT8             HeciHideInMe[3];
  UINT32            RspLen;
  HECI_PROTOCOL     *pHeci;
  SPS_DXE_CONTEXT   *pSpsContext = (SPS_DXE_CONTEXT*)pContext;
  UINT8             DisableEop = 0;
  union
  {
    SPS_MSG_ME_SHUTDOWN_REQ        ShutdownReq;
    SPS_MSG_ME_SHUTDOWN_RSP        ShutdownRsp;
    MKHI_MSG_END_OF_POST_REQ       EopReq;
    MKHI_MSG_END_OF_POST_RSP       EopRsp;
    MKHI_MSG_HECI_STATE_CHANGE_REQ HeciStateReq;
    MKHI_MSG_HECI_STATE_CHANGE_RSP HeciStateRsp;
  } HeciMsg;

  PCH_RESET_PROTOCOL  *PchResetProtocol;
  UINTN               DataSize;
  VOID                *ResetData;
  RspLen = 0;
  SetMem(&HeciMsg.ShutdownReq, sizeof(HeciMsg.ShutdownReq), 0);

  gBS->CloseEvent(Event);

  if (pSpsContext->MePolicy->AnythingChanged) {
    // Update configuration of pSpsContext & S3 Script
    SpsEndOfDxeCallback(Event, pContext);
  }

  Status = gBS->LocateProtocol(&gHeciProtocolGuid, NULL, &pHeci);
  if (EFI_ERROR (Status))
  {
    DEBUG((DEBUG_ERROR, "[SPS] ERROR: Cannot locate HECI protocol (%r)\n", Status));
  }
  else
  {
    //
    // Send SPS_ME_SHUTDOWN notification if not disabled in ME options
    //
    if (pSpsContext->MePolicy->MeConfig.SpsMeShutdownEnabled)
    {
      DEBUG((DEBUG_INFO, "[SPS] Sending SPS_ME_SHUTDOWN to ME\n"));

      if (MeTypeIsSpsInRecovery()) {
        DEBUG((DEBUG_INFO, "[SPS] SPS_ME_SHUTDOWN not supported in recovery fw!"));
        Status = EFI_UNSUPPORTED;
      } else {
        HeciMsg.ShutdownReq.Command.Data = SPS_CMD_ME_SHUTDOWN_REQ;
        RspLen = sizeof(HeciMsg.ShutdownRsp);
        Status = pHeci->SendwACK(HECI1_DEVICE, (UINT32*)&HeciMsg.ShutdownReq,
            sizeof(HeciMsg.ShutdownReq), &RspLen,
            SPS_CLIENTID_BIOS, SPS_CLIENTID_ME_SPS);
      }

      if (EFI_ERROR(Status))
      {
        DEBUG((DEBUG_ERROR, "[SPS] ERROR: Cannot send SPS_ME_SHUTDOWN (%r)\n", Status));
      }
      else
      {
        if (HeciMsg.ShutdownRsp.Command.Data != SPS_CMD_ME_SHUTDOWN_RSP  ||
            RspLen < sizeof(HeciMsg.ShutdownRsp))
        {
          DEBUG((DEBUG_ERROR, "[SPS] ERROR: Invalid SPS_ME_SHUTDOWN response "
                "(cmd: 0x%X, len %d)\n", HeciMsg.ShutdownRsp.Command, RspLen));
        }
        else {
          if (HeciMsg.ShutdownRsp.Result == 0x0)
          {
            DisableEop = 1;
          }
          else if (HeciMsg.ShutdownRsp.Result == 0x1)
          {
            DEBUG((DEBUG_WARN, "[SPS] WARNING: SPS_ME_SHUTDOWN rejected - ME FW is in pre-End of Manufacturing state\n"));
          }
          else if (HeciMsg.ShutdownRsp.Result == 0x2)
          {
            DEBUG((DEBUG_WARN, "[SPS] WARNING: SPS_ME_SHUTDOWN rejected - ME FW has features that run after EOP\n"));
          }
        }
      }
    }

    //
    // Send END_OF_POST notification if not disabled in ME debug options
    //
    if (pSpsContext->MePolicy->MeConfig.EndOfPostEnabled && !DisableEop)
    {
      DEBUG((DEBUG_INFO, "[SPS] Sending END_OF_POST to ME\n"));

      HeciMsg.EopReq.Mkhi.Data = 0x00000CFF;
      RspLen = sizeof(HeciMsg.EopRsp);
      Status = pHeci->SendwACK(HECI1_DEVICE, (UINT32*)&HeciMsg.EopReq,
                               sizeof(HeciMsg.EopReq), &RspLen,
                               SPS_CLIENTID_BIOS, SPS_CLIENTID_ME_MKHI);
      if (EFI_ERROR(Status))
      {
        DEBUG((DEBUG_ERROR, "[SPS] ERROR: Cannot send END_OF_POST (%r)\n", Status));
        MeReportError(MSG_EOP_ERROR);
      }
      else if (HeciMsg.EopRsp.Mkhi.Data != 0x00008CFF)
      {
        DEBUG((DEBUG_ERROR, "[SPS] ERROR: Invalid END_OF_POST response (MKHI: 0x%X)\n",
                            HeciMsg.EopRsp.Mkhi.Data));
      }
      else if (RspLen == sizeof(HeciMsg.EopRsp) && // if response contains Action and
               HeciMsg.EopRsp.Action == 1)         // global reset was requested
      {
        DEBUG((DEBUG_WARN, "[SPS] WARNING: END_OF_POST response requests Global Reset!!!\n"));

        Status = gBS->LocateProtocol (&gPchResetProtocolGuid, NULL, &PchResetProtocol);

        if (!EFI_ERROR (Status)) {
          ResetData = PchResetProtocol->GetResetData (&gPchGlobalResetGuid, &DataSize);
          PchResetProtocol->Reset (PchResetProtocol, GlobalReset, DataSize, ResetData);
        }
      }
    } // if (SystemCfgData.MeEndOfPostEnabled)

    //
    // Send HECI_STATE_CHANGE if enabled in ME policy
    //
    HeciHideInMe[2] = pSpsContext->MePolicy->MeConfig.Heci3HideInMe;
    HeciHideInMe[1] = pSpsContext->MePolicy->MeConfig.Heci2HideInMe;
    HeciHideInMe[0] = pSpsContext->MePolicy->MeConfig.Heci1HideInMe;
    for (Index = 3; Index--;)
    {
      if (HeciHideInMe[Index] != SETUP_HECI_IN_ME_OFF)
      {
        HeciMsg.HeciStateReq.Mkhi.Data = 0x00000DFF;
        HeciMsg.HeciStateReq.Nonce = mNonce;
        HeciMsg.HeciStateReq.HeciId = Index + 1;
        HeciMsg.HeciStateReq.State = HeciHideInMe[Index] & 1;
        RspLen = sizeof(HeciMsg);

        DEBUG((DEBUG_INFO, "[SPS] Sending HECI_CHANGE_STATE to ME to %a HECI-%d\n",
               (HeciMsg.HeciStateReq.State) ? "hide" : "disable", HeciMsg.HeciStateReq.HeciId));

        Status = pHeci->SendMsg(HECI1_DEVICE, (UINT32*)&HeciMsg.HeciStateReq, sizeof(HeciMsg.HeciStateReq),
                                SPS_CLIENTID_BIOS, SPS_CLIENTID_ME_MKHI);
        if (!EFI_ERROR(Status))
        {
          //
          // We don't expect response if it is HECI-1 hidden/disabled.
          // ME executes the operation before we can read response, so it fails.
          //
          if (Index == 0)
          {
            HeciMsg.HeciStateRsp.Mkhi.Data = 0x00008DFF;
            HeciMsg.HeciStateRsp.Response = 0;
          }
          else
          {
            Status = pHeci->ReadMsg(HECI1_DEVICE, TRUE, (UINT32*)&HeciMsg.HeciStateRsp, &RspLen);
          }
        }
        if (EFI_ERROR(Status))
        {
          DEBUG((DEBUG_ERROR, "[SPS] ERROR: Cannot send HECI_CHANGE_STATE (%r)\n", Status));
        }
        else if (HeciMsg.HeciStateRsp.Mkhi.Data != 0x00008DFF)
        {
          DEBUG((DEBUG_ERROR, "[SPS] ERROR: Invalid HECI_CHANGE_STATE response (MKHI: 0x%X)\n",
                              HeciMsg.HeciStateRsp.Mkhi.Data));
        }
        else if (HeciMsg.HeciStateRsp.Response != 0)
        {
          DEBUG((DEBUG_ERROR, "[SPS] ERROR: HECI_CHANGE_STATE for HECI-%d failed (%d)\n",
                              Index + 1, HeciMsg.HeciStateRsp.Response));
        }
      }
    } // for (Index...)
  } // if (HECI protocol located)

#if ME_TESTMENU_FLAG
  //
  // Upon user menu request set the CF9 reset promotion to global reset promotion
  //
  if (pSpsContext->MePolicy->MeConfig.MeGrPromotionEnabled)
  {
    DEBUG((DEBUG_INFO, "[SPS] Promoting CF9 reset to global reset\n"));
    MmioOr32(MmPciBase (DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_PMC,
                               PCI_FUNCTION_NUMBER_PCH_PMC) + R_PCH_PMC_ETR3, (UINT32)B_PCH_PMC_ETR3_CF9GR);
  }
#endif

#if ME_TESTMENU_FLAG
  //
  // Lock global reset in PMIR register if not disabled in ME Test Menu options
  //
  if (pSpsContext->MePolicy->MeNmConfig.MeGrLockEnabled)
#endif
  {
    DEBUG((DEBUG_INFO, "[SPS] Disabling Global Reset capability\n"));
    MmioOr32(MmPciBase(DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_PMC,
                               PCI_FUNCTION_NUMBER_PCH_PMC) + R_PCH_PMC_ETR3, (UINT32)B_PCH_PMC_ETR3_CF9LOCK);
  }
  return;
} // SpsReadyToBootCallback()
