 /**

Copyright (c) 1996-2016, Intel Corporation.

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

  @file SpsPei.c

  This driver manages the initial phase of SPS ME firmware support specified in
  SPS ME-BIOS Interface Specification.

**/
#include <Base.h>
#include <PiPei.h>
#include <Uefi.h>
#include <Library/PeiServicesLib.h>
#include <Library/PeiServicesTablePointerLib.h>

#include <Ppi/HeciPpi.h>
#include <Ppi/PciCfg.h>
#include <Ppi/EndOfPeiPhase.h>

#include <Guid/SpsInfoHobGuid.h>

#include <Guid/GlobalVariable.h>

#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/IoLib.h>
#include <Library/HobLib.h>
#include <Library/PcdLib.h>
#include <Library/PciLib.h>
#include <Library/TimerLib.h>

#include <Register/PchRegsPcr.h>
#include <Register/PchRegsPsf.h>
#include <PchReservedResources.h>
#include <Library/SocketLga1156Lib.h>
#include <Library/SocketLga775Lib.h>
#include <Library/PchPcrLib.h>

#include <HeciRegs.h>
#include <MeAccess.h>
#include <PchAccess.h>

#include <Sps.h>
#include <Library/MeTypeLib.h>
#include <Ppi/MeUma.h>

#include "SpsPei.h"

/*****************************************************************************
 * Local definitions
 */
#define SPS_INIT_TIMEOUT  2000000 // [us] Timeout when waiting for InitComplete
#define SPS_RESET_TIMEOUT 5000000 // [us] Timeout when waiting for InitComplete
#define STALL_1US        1
#define STALL_1MS        1000
#define STALL_1S         1000000


/*****************************************************************************
 * Local functions prototypes
 */
EFI_STATUS SpsNonS3Path(IN CONST EFI_PEI_SERVICES**, IN EFI_PEI_NOTIFY_DESCRIPTOR*, IN VOID*);
EFI_STATUS SpsS3Path(IN CONST EFI_PEI_SERVICES**, IN EFI_PEI_NOTIFY_DESCRIPTOR*, IN VOID*);


/*****************************************************************************
 * Variables
 */
static EFI_PEI_NOTIFY_DESCRIPTOR  mSpsPeiNonS3NotifyList[] = {
  {
    (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gMemoryInitDoneSentGuid,
    SpsNonS3Path
  }
};
static EFI_PEI_NOTIFY_DESCRIPTOR  mSpsPeiS3NotifyList[] = {
  {
    (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gMemoryInitDoneSentGuid,
    SpsS3Path
  }
};

/*****************************************************************************
 @brief
   Function SPS meaningfull state (Normal or Recovery)

    @param WaitTimeout -  Pointer to wait timeout

    @retval EFI_SUCCESS   - ME is in Normal in Recovery state
    @retval EFI_TIMEOUT   - ME is not in Normal or Recovery state
                            within defined timeout
**/

EFI_STATUS
WaitForMeMeaningfulState(
  UINT32  *SpsMeaningfulStateTimeout
  )
{
  SPS_MEFS1 MeFs1;
  UINT32    SpsTimeout = SPS_INIT_TIMEOUT / STALL_1MS;

  DEBUG((DEBUG_INFO, "[SPS] Wait for ME Normal or Recovery state\n"));
  if (SpsMeaningfulStateTimeout == NULL)
  {
    SpsMeaningfulStateTimeout = &SpsTimeout;
  }

  // Wait for ME meaningful state
  MeFs1.UInt32 = HeciPciRead32(SPS_REG_MEFS1);
  while ((MeFs1.Bits.CurrentState != MEFS1_CURSTATE_RECOVERY) &&
         (MeFs1.Bits.CurrentState != MEFS1_CURSTATE_NORMAL))
  {
    if (!*SpsMeaningfulStateTimeout)
    {
      DEBUG((DEBUG_ERROR, "[SPS] ERROR: Timeout when waiting for ME meaningful state\n"));
      return EFI_TIMEOUT;
    }
    MicroSecondDelay(STALL_1MS);
    MeFs1.UInt32 = HeciPciRead32(SPS_REG_MEFS1);
    *SpsMeaningfulStateTimeout--;
  }

  DEBUG((DEBUG_INFO, "[SPS] Wait for ME state reaches ME State = %d\n",
         MeFs1.Bits.CurrentState));

  return EFI_SUCCESS;
} // WaitForMeMeaningfulState()

/*****************************************************************************
 @brief
   Function get ME Policy

    @param PeiMePlatformPolicyPpi - retrieved ME Policy pointer

    @retval Status      - operation status

**/
EFI_STATUS
SetupInit (
  OUT PEI_ME_PLATFORM_POLICY_PPI  **PeiMePlatformPolicyPpi
  )
{
  EFI_STATUS  Status;

  Status = PeiServicesLocatePpi(
                  &gPeiMePlatformPolicyPpiGuid,
                  0,
                  NULL,
                  PeiMePlatformPolicyPpi
                  );

  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_ERROR, "[SPS] ERROR: Unable to locate PPI gPeiMePlatformPolicyPpiGuid (%r)\n", Status));
    *PeiMePlatformPolicyPpi = NULL;
  }

  return Status;
} // SetupInit

/*****************************************************************************
 @brief
  Waits For SPS ME FW initialization

  @param[in] none

  @retval EFI_SUCCESS      Success
  @retval EFI_UNSUPPORTED  Failure, go to non-functional flow

**/
EFI_STATUS
WaitMeFwInitialization(
  VOID
  )
{
  EFI_STATUS                    Status;
  UINT32                        SpsInitTimeout;
  SPS_MEFS1                     MeFs1;
  SPS_MEFS2                     MeFs2;
  PEI_ME_PLATFORM_POLICY_PPI    *PeiMePlatformPolicyPpi;
  EFI_BOOT_MODE                 BootMode;
  CONST EFI_PEI_SERVICES        **PeiServices = GetPeiServicesTablePointer ();

  Status = SetupInit(&PeiMePlatformPolicyPpi);
  if (EFI_ERROR(Status))
  {
    SpsInitTimeout = SPS_INIT_TIMEOUT / STALL_1MS;
  }
  else
  {
    SpsInitTimeout = PeiMePlatformPolicyPpi->MeTimeout * (STALL_1S / STALL_1MS);
  }

  //
  // Wait for ME init complete. Timeout is 2 seconds.
  //
  DEBUG((DEBUG_INFO, "[SPS] Waiting for ME firmware init complete\n"));
  MeFs1.UInt32 = HeciPciRead32(SPS_REG_MEFS1);
  while (!MeFs1.Bits.InitComplete)
  {
    if (!SpsInitTimeout)
    {
      DEBUG((DEBUG_ERROR, "[SPS] ERROR: Timeout when waiting for ME init complete\n"));
      return EFI_UNSUPPORTED;
    }
    MicroSecondDelay(STALL_1MS);
    MeFs1.UInt32 = HeciPciRead32(SPS_REG_MEFS1);
    SpsInitTimeout--;
  }

  //
  // Get Boot mode
  //
  BootMode = BOOT_WITH_DEFAULT_SETTINGS;
  Status = (*PeiServices)->GetBootMode(PeiServices, &BootMode);
  if (EFI_ERROR(Status))
  {
    DEBUG((DEBUG_ERROR, "[SPS] ERROR: Cannot read boot mode (%r)\n", Status));
    BootMode = BOOT_WITH_DEFAULT_SETTINGS;
  }


  switch (MeFs1.Bits.CurrentState)
  {
    case MEFS1_CURSTATE_RECOVERY:
      MeFs2.UInt32 = HeciPciRead32(SPS_REG_MEFS2);
      DEBUG((DEBUG_WARN, "[SPS] WARNING: ME is in recovery mode (cause: %d)\n",
                         MeFs2.Bits.RecoveryCause));
      // Fall through to normal case
    case MEFS1_CURSTATE_NORMAL:
      break;

    default:
      return EFI_UNSUPPORTED;
  }

  return EFI_SUCCESS;

} // WaitMeFwInitialization()

/*****************************************************************************
 @brief
  Set boot mode requested by NM in ME.

  @param[in] PowerOptBoot - Power vs performance optimized boot is requested

  @retval EFI_SUCCESS   Always success
**/
EFI_STATUS
NmSetBootMode(
  BOOLEAN PerfOptBoot)
{
  UINT64  PlatformInfo;
  UINT64  PerfCtrl;

  //
  // In Platform Info MSR we got minimum and maximum ratio for processor.
  //
  PlatformInfo = AsmReadMsr64(MSR_PLATFORM_INFO);
  //
  // Read current Performance Control MSR, clear ratio bits
  //
  PerfCtrl = AsmReadMsr64(EFI_MSR_IA32_PERF_CTL) & ~0x7F00;

  if (PerfOptBoot)
  { //
    // Set max ratio in Performance Control MSR
    //
    PerfCtrl |= PlatformInfo & 0x7F00;
    DEBUG((DEBUG_INFO, "[SPS] NM Boot Mode: Performance Optimized\n"));
  }
  else
  { //
    // Set min ratio in Performance Control MSR
    //
    // Denverton AptioV override start - EIP#264461
    // PerfCtrl |= (PlatformInfo >> 32) & 0x7F00;
    PerfCtrl |= RShiftU64(PlatformInfo, 32) & 0x7F00;
    // Denverton AptioV override end - EIP#264461
    DEBUG((DEBUG_INFO, "[SPS] NM Boot Mode: Power Optimized\n"));
  }
  AsmWriteMsr64(EFI_MSR_IA32_PERF_CTL, PerfCtrl);

  return EFI_SUCCESS;
} // NmSetBootMode()


/*****************************************************************************
 @brief
   Create HOB with SPS info for DXE

 @param[in] WorkFlow      The state of ME firmware observed at PEI
 @param[in] FeatureSet    ME features enabled reported in Get ME-BIOS Interface response
 @param[in] PwrOptBoot    Whether NM in ME requests power optimized boot
 @param[in] Cores2Disable Whetherm NM in ME requests disabling cores

 @retval EFI_SUCCESS      The function completed successfully.
 @retval EFI_OUT_OF_RESOURCES HOB creation failed
**/
EFI_STATUS
SpsHobCreate(
  UINT8                  WorkFlow,
  UINT64                 FeatureSet,
  BOOLEAN                PwrOptBoot,
  UINT8                  Cores2Disable)
{
  EFI_HOB_TYPE_SPS_INFO  SpsHob;
  EFI_HOB_GUID_TYPE      *pGuidHob;
  CONST EFI_PEI_SERVICES **PeiServices = GetPeiServicesTablePointer ();

  //
  // Zero the HOB buffer
  //
  (*PeiServices)->SetMem(&SpsHob, sizeof(SpsHob), 0);

  //
  // Initialize the HOB data
  //
  SpsHob.FeatureSet.Data = FeatureSet;
  SpsHob.WorkFlow = WorkFlow;
  SpsHob.PwrOptBoot = PwrOptBoot;
  SpsHob.Cores2Disable = Cores2Disable;

  BuildGuidDataHob(&gEfiSpsInfoHobGuid, &SpsHob, sizeof(SpsHob));

  DEBUG((DEBUG_INFO, "[SPS] HOB: features 0x%02x, flow %d, boot mode %d, cores to disable %d\n",
         SpsHob.FeatureSet, SpsHob.WorkFlow, SpsHob.PwrOptBoot, SpsHob.Cores2Disable));

  pGuidHob = GetFirstGuidHob(&gEfiSpsInfoHobGuid);
  if (pGuidHob != NULL)
  {
     return EFI_SUCCESS;
  }
  DEBUG((DEBUG_ERROR, "[SPS] ERROR: SPS FW HOB: HAS NOT BEEN REGISTERED"));
  return EFI_OUT_OF_RESOURCES;
} // SpsHobCreate()

/*****************************************************************************
 @brief
  This function sends ICC Set Clock Settings command to SPS firmware.

  @param[in] PpiHeci HECI driver PPI
  @param[in] Mode ICC Clock SSC Setting

  @retval EFI_SUCCESS             ICC Set Clock Settings command sent successfully
  @retval EFI_INVALID_PARAMETER   Pointer of PpiHeci is invalid
**/
EFI_STATUS
SpsPeiSetCurrenClockingMode (
  IN  HECI_PPI  *PpiHeci,
  IN UINT8      Mode
  )
{
  EFI_STATUS                        Status;
  UINT32                            CommandSize;
  UINT32                            ResponseSize;
  ICC_SETGET_CLOCK_SETTINGS_BUFFER  Msg;
  CONST EFI_PEI_SERVICES **PeiServices = GetPeiServicesTablePointer ();

  DEBUG ((DEBUG_INFO, "[SPS] (ICC) SpsSetCurrenClockingMode\n"));

  if (NULL == PpiHeci) {
    DEBUG ((DEBUG_ERROR, "[SPS] ERROR: (ICC) SpsSetCurrenClockingMode: PpiHeci is NULL\n"));
    return EFI_INVALID_PARAMETER;
  }

  if (NULL == PeiServices) {
    DEBUG ((DEBUG_ERROR, "[SPS] ERROR: (ICC) SpsSetCurrenClockingMode: Can't locate PeiServices\n"));
    return EFI_INVALID_PARAMETER;
  }

  (*PeiServices)->SetMem(&Msg, sizeof(Msg), 0);

  CommandSize                         = sizeof(ICC_SETGET_CLOCK_SETTINGS_REQ);
  ResponseSize                        = sizeof(ICC_SETGET_CLOCK_SETTINGS_RSP);

  Msg.Message.IccHeader.ApiVersion    = ICC_API_4_0;
  Msg.Message.IccHeader.IccCommand    = ICC_CMD_SET_CLOCK_SETTINGS;
  Msg.Message.IccHeader.IccStatus     = ICC_STS_SUCCESS;
  Msg.Message.IccHeader.BufferLength  = CommandSize - sizeof (ICC_MSG_HEADER);
  Msg.Message.IccHeader.Reserved      = 0;
  Msg.Message.RequestedClock          = REQUESTED_CLOCK_PCIe_Gen2_PLL;
  Msg.Message.SettingType             = 0;
  Msg.Message.SSCsetting              = Mode;

  Status = PpiHeci->SendwAck(HECI1_DEVICE,
                             (UINT32*)&Msg.Message,
                             CommandSize,
                             &ResponseSize,
                             SPS_CLIENTID_BIOS,
                             SPS_CLIENTID_ME_ICC
                             );

  if (!EFI_ERROR(Status) && ((Msg.Response.IccHeader.IccCommand != ICC_CMD_SET_CLOCK_SETTINGS) || (Msg.Response.IccHeader.IccStatus != ICC_STATUS_SUCCESS))) {
    DEBUG ((DEBUG_ERROR, "[SPS] ERROR: (ICC) SpsSetCurrenClockingMode: Wrong response! IccHeader.IccResponse = 0x%x\n", Msg.Response.IccHeader.IccStatus));
    Status = EFI_DEVICE_ERROR;
  }

  DEBUG ((DEBUG_INFO, "[SPS] (ICC) SpsSetCurrenClockingMode exit status = %r \n", Status));
  return Status;
} // SpsPeiSetCurrenClockingMode()

/*****************************************************************************
 @brief
  This function is called on S3 resume path. It sends END_OF_POST message to
  SPS firmware.

  NOTE: This function runs after S3 script. It MUST PRESERVE content of
        all the registers it uses.

  @param[in] PeiServices      unused
  @param[in] NotifyDescriptor unused
  @param[in] Ppi              unused

  @retval EFI_SUCCESS             MeMode copied
  @retval EFI_INVALID_PARAMETER   Pointer of MeMode is invalid
**/
EFI_STATUS
SpsS3Path (
  IN CONST EFI_PEI_SERVICES    **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDescriptor,
  IN VOID                      *Ppi)
{
  EFI_STATUS                    Status;
  HECI_PPI                     *pHeciPpi;
  UINT32                        Len;
  PEI_ME_PLATFORM_POLICY_PPI   *PeiMePlatformPolicyPpi;

  UINT32 FuncDisHeci1;
  UINT32 MbarL = 0;
  UINT32 MbarH = 0;
  UINT8  Cmd = 0;
  UINT8  Irq = 0;

  union
  {
    MKHI_MSG_END_OF_POST_REQ EopReq;
    MKHI_MSG_END_OF_POST_RSP EopRsp;
  } HeciMsg;

  DEBUG((DEBUG_INFO, "[SPS] SpsS3Path called.\n"));

  Status = WaitMeFwInitialization();
  if (EFI_ERROR(Status))
  {
    DEBUG((DEBUG_ERROR, "[SPS] ERROR: ME not initialized correctly.\n"));
    SpsHobCreate(ME_NONFUNCTIONAL, 0, FALSE, 0);
    return Status;
  }

  Status = PeiServicesLocatePpi(&gHeciPpiGuid, 0, NULL, &pHeciPpi);
  if (EFI_ERROR(Status))
  {
    DEBUG((DEBUG_ERROR, "[SPS] ERROR: Cannot locate HECI PPI\n"));
    return Status;
  }
  //
  // Save the registers that may change in this function
  //
  PchPcrRead32 (PID_PSF2, R_PCH_PCR_PSF2_T0_SHDW_HECI1_REG_BASE + R_PCH_PCR_PSFX_T0_SHDW_PCIEN, &FuncDisHeci1);
  PchPcrAndThenOr32 (PID_PSF2,
                     R_PCH_PCR_PSF2_T0_SHDW_HECI1_REG_BASE + R_PCH_PCR_PSFX_T0_SHDW_PCIEN,
                    (UINT32)~B_PCH_PCR_PSFX_T0_SHDW_PCIEN_FUNDIS,
                     0);
  MbarL = HeciPciRead32(R_HECIMBAR);
  MbarH = HeciPciRead32(R_HECIMBAR + 4);
  Cmd = HeciPciRead8(R_COMMAND);
  Irq = HeciPciRead8(R_IRQ);

  //
  // If HECI is already configured to 64-bit address force its reinitialization to 32-bit.
  // PEI works in 32-bit.
  //
  if (MbarH != 0 && (MbarL & 0x4))
  {
    HeciPciWrite32(R_HECIMBAR, 0);
  }
  pHeciPpi->InitializeHeci(HECI1_DEVICE);

  Status = SetupInit(&PeiMePlatformPolicyPpi);
  if (EFI_ERROR(Status))
  {
    DEBUG((DEBUG_ERROR, "[SPS] ERROR: Cannot read system configuration, using defaults\n"));
  }

  //
  // Send END_OF_POST notification if not desibled in ME debug options
  //
  if ((PeiMePlatformPolicyPpi == NULL) || (PeiMePlatformPolicyPpi->MeEndOfPostEnabled))
  {
    DEBUG((DEBUG_INFO, "[SPS] Sending END_OF_POST to ME\n"));
    HeciMsg.EopReq.Mkhi.Data = 0x00000CFF;
    Len = sizeof(HeciMsg);
    Status = pHeciPpi->SendwAck(HECI1_DEVICE, (UINT32*)&HeciMsg.EopReq, sizeof(HeciMsg.EopReq),
                                &Len, SPS_CLIENTID_BIOS, SPS_CLIENTID_ME_MKHI);
    if (EFI_ERROR(Status))
    {
      DEBUG((DEBUG_ERROR, "[SPS] ERROR: Cannot send END_OF_POST (%r)\n", Status));
    }
    else if (HeciMsg.EopRsp.Mkhi.Data != 0x00008CFF)
    {
      DEBUG((DEBUG_ERROR, "[SPS] ERROR: Invalid END_OF_POST response (MKHI: 0x%X)\n",
                          HeciMsg.EopRsp.Mkhi.Data));
    }
  } // if (MeEndOfPostEnabled)
  //
  // Restore the registers that may have changed
  //
  HeciPciWrite32(R_HECIMBAR + 4, MbarH);
  HeciPciWrite32(R_HECIMBAR, MbarL);
  HeciPciWrite8(R_COMMAND, Irq);
  HeciPciWrite8(R_COMMAND, Cmd);
  PchPcrWrite32(PID_PSF2,
                R_PCH_PCR_PSF2_T0_SHDW_HECI1_REG_BASE + R_PCH_PCR_PSFX_T0_SHDW_PCIEN,
                FuncDisHeci1);

  return EFI_SUCCESS;
} // SpsS3Path()


/* This Function will be called after MRC and HECI Protocol launched */
EFI_STATUS
SpsNonS3Path (
  IN CONST EFI_PEI_SERVICES    **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDescriptor,
  IN VOID                      *Ppi)
{
  EFI_STATUS                    Status;
  SPS_FEATURE_SET               SpsFeatureSet;
  SPS_MEFS1                     MeFs1;
  SPS_NMFS                      NmFs;
  HECI_PPI                      *pHeciPpi;
  PEI_HECI2_PPI                 *pHeci2Ppi;
  UINTN                         Attempts;
  UINT32                        RspLen;
  PEI_ME_PLATFORM_POLICY_PPI    *PeiMePlatformPolicyPpi;

  union
  {
    SPS_MSG_GET_MEBIOS_INTERFACE_REQ MeBiosVerReq;
    SPS_MSG_GET_MEBIOS_INTERFACE_RSP MeBiosVerRsp;
  } HeciMsg;

  DEBUG((DEBUG_INFO, "[SPS] SpsNonS3Path called\n"));

  Status = WaitMeFwInitialization();
  if (EFI_ERROR(Status))
  {
    DEBUG((DEBUG_ERROR, "[SPS] ERROR: ME not initialized correctly\n"));
    SpsHobCreate(ME_NONFUNCTIONAL, 0, FALSE, 0);

    return Status;
  }

  Status = SetupInit(&PeiMePlatformPolicyPpi);
  if (EFI_ERROR(Status))
  {
    // Cannot read system configuration - continue with zeros as defaults
    DEBUG((DEBUG_ERROR, "[SPS] ERROR: Cannot read system configuration, using defaults\n"));
  }

  Status = PeiServicesLocatePpi(&gHeciPpiGuid, 0, NULL, &pHeciPpi);
  if (EFI_ERROR(Status))
  {
    DEBUG((DEBUG_ERROR, "[SPS] ERROR: Cannot locate HECI PPI\n"));
    return Status;
  }

  Status = pHeciPpi->InitializeHeci(HECI1_DEVICE);
  if (EFI_ERROR(Status))
  {
    DEBUG((DEBUG_ERROR, "[SPS] ERROR: Cannot initialize HECI-1 (%r)\n",
      Status));

    return Status;
  }

  // Get ME - BIOS interface Version
  DEBUG((DEBUG_INFO, "[SPS] Sending ME-BIOS Interface Version request\n"));
  Attempts = 0;
  do
  {
    // Initialize Heci Message buffer
    (*PeiServices)->SetMem(&HeciMsg, sizeof(HeciMsg), 0);

    // Set Command Data
    HeciMsg.MeBiosVerReq.Command.Data = SPS_CMD_GET_MEBIOS_INTERFACE_REQ;

    // Send message
    Status = pHeciPpi->SendMsg(
      HECI1_DEVICE,
      (UINT32*)&HeciMsg.MeBiosVerReq,
      sizeof(HeciMsg.MeBiosVerReq),
      SPS_CLIENTID_BIOS,
      SPS_CLIENTID_ME_SPS
      );

    if (!EFI_ERROR(Status))
    {
      // Set response length
      RspLen = sizeof(SPS_MSG_GET_MEBIOS_INTERFACE_RSP);

      // Read response
      Status = pHeciPpi->ReadMsg(
        HECI1_DEVICE,
        TRUE,
        (UINT32*)&HeciMsg.MeBiosVerRsp,
        &RspLen
        );

      if (EFI_ERROR(Status))
      {
        // Cannot read message response, will send again if number of retries not exceeded
        DEBUG((DEBUG_ERROR, "[SPS] ERROR: Cannot get SPS_GET_MEBIOS_INTERFACE response (status: %r)\n",
          Status));
      }
      else if(!SpsMeBiosRspCommandDataVerify(HeciMsg.MeBiosVerRsp.Command.Data))
      {
        DEBUG((DEBUG_ERROR, "[SPS] ERROR: Invalid GET_MEBIOS_INTERFACE response command data (cmd: 0x%X)\n",
          HeciMsg.MeBiosVerRsp.Command));

        Status = EFI_ABORTED;
      }
      else if((RspLen != sizeof(SPS_MSG_GET_MEBIOS_INTERFACE_RSP)) &&
              (RspLen != sizeof(SPS_MSG_GET_MEBIOS_INTERFACE_RSP) - sizeof(UINT32))) // TODO: Remove this after FW transition to new format
      {
        DEBUG((DEBUG_ERROR, "[ME] ERROR: Invalid SPS_GET_MEBIOS_INTERFACE response length (length %d)\n",
          RspLen));

        Status = EFI_ABORTED;
      }
    }
    else
    {
      DEBUG((DEBUG_ERROR, "[SPS] ERROR: Cannot send SPS_GET_MEBIOS_INTERFACE (status %r)\n",
        Status));
    }

    Attempts++;
  }
  while (EFI_ERROR(Status) && !SpsMaxNumberOfMsgResendReached(Attempts));

  if(EFI_ERROR(Status))
  {
    // Cannot get the ME - BIOS interface version
    SpsHobCreate(ME_NONFUNCTIONAL, 0, FALSE, 0);
    return Status;
  }
  else
  {
    // Initialize SPS ME Firmware Status #1 buffer
    (*PeiServices)->SetMem(&MeFs1, sizeof(MeFs1), 0);

    // Read SPS ME Firmware Status #1 Register
    MeFs1.UInt32 = HeciPciRead32(SPS_REG_MEFS1);

    // Verify received ME - BIOS interface version
    if ((MeFs1.Bits.CurrentState == MEFS1_CURSTATE_RECOVERY) ?
        !SpsMeBiosRcvVersionVerify(HeciMsg.MeBiosVerRsp.VerMajor, HeciMsg.MeBiosVerRsp.VerMinor) :
        !SpsMeBiosOprVersionVerify(HeciMsg.MeBiosVerRsp.VerMajor, HeciMsg.MeBiosVerRsp.VerMinor))
    {
      DEBUG((DEBUG_ERROR, "[SPS] ERROR: Incompatible SPS ME-BIOS interface definition %d.%d\n",
        HeciMsg.MeBiosVerRsp.VerMajor, HeciMsg.MeBiosVerRsp.VerMinor));

      SpsHobCreate(ME_NONFUNCTIONAL, 0, FALSE, 0);
      return EFI_ABORTED;
    }
  }

  // Received ME - BIOS interface version valid
  DEBUG((DEBUG_INFO, "[SPS] SPS ME-BIOS interface version is %d.%d\n\tFeature set is 0x%X\n",
    HeciMsg.MeBiosVerRsp.VerMajor, HeciMsg.MeBiosVerRsp.VerMinor, HeciMsg.MeBiosVerRsp.FeatureSet));

  // Initialize SPS Feature Set storage
  (*PeiServices)->SetMem(&SpsFeatureSet, sizeof(SpsFeatureSet), 0);

  // Store received SPS Features Set
  SpsFeatureSet.Data = HeciMsg.MeBiosVerRsp.FeatureSet;

  if (SpsFeatureSet.Bits.NodeManager)
  {
    // Initialize SPS NM Firmware Status Register buffer
    (*PeiServices)->SetMem(&NmFs, sizeof(NmFs), 0);

    // Read SPS NM Firmware Status Register
    NmFs.UInt32 = Heci2PciRead32(SPS_REG_NMFS);

    if (NmFs.Bits.NmEnabled)
    {
      if ( PeiMePlatformPolicyPpi != NULL && PeiMePlatformPolicyPpi->NmPwrOptBootOverride )
      {
        NmFs.Bits.PerfOptBoot = !PeiMePlatformPolicyPpi->NmPwrOptBoot;
      }

      if ( PeiMePlatformPolicyPpi != NULL && PeiMePlatformPolicyPpi->NmCores2DisableOverride)
      {
        NmFs.Bits.Cores2Disable = PeiMePlatformPolicyPpi->NmCores2Disable;
      }

      NmSetBootMode((BOOLEAN)NmFs.Bits.PerfOptBoot);
    }
    else
    {
      DEBUG((DEBUG_ERROR, "[SPS] ERROR: NMFS not configured while NM enabled\n\t(feature set: 0x%X, NMFS: 0x%X)\n",
          HeciMsg.MeBiosVerRsp.FeatureSet, NmFs.UInt32));
    }

    Status = PeiServicesLocatePpi(&gPeiHeci2PpiGuid, 0, NULL, &pHeci2Ppi);
    if (!EFI_ERROR(Status))
    {
      Status = pHeci2Ppi->Heci2Initialize();
      if (EFI_ERROR(Status))
      {
        DEBUG((DEBUG_ERROR, "[SPS] ERROR: Cannot initialize HECI-2 (%r)\n", Status));
      }
    }
  }
  else
  {
    // no cores to disable, performance optimized boot
    NmFs.UInt32 |= SPS_NMFS_PERF_OPT_BOOT_SET;
  }

  SpsHobCreate(ME_FUNCTIONAL, HeciMsg.MeBiosVerRsp.FeatureSet, !NmFs.Bits.PerfOptBoot, (UINT8)NmFs.Bits.Cores2Disable);
  
    if(PeiMePlatformPolicyPpi != NULL)
    {
	  // Denverton AptioV override start - EIP#332161
      // Send ICC Clock SSC Settings only when supported by ME
      if (RShiftU64(SpsFeatureSet.Data, 2) & 0x01)
          //if (SpsFeatureSet.Bits.ICC)
	  // Denverton AptioV override end - EIP#332161
      {
      
      // Send ICC Clock SSC Setting to ME
      DEBUG((DEBUG_INFO, "[SPS] (ICC) Send ICC Set Clock Settings command (SSC Setting %d)\n", PeiMePlatformPolicyPpi->SpsIccClkSscSetting));
      Status = SpsPeiSetCurrenClockingMode(pHeciPpi, PeiMePlatformPolicyPpi->SpsIccClkSscSetting);
      if (EFI_ERROR(Status))
      {
        DEBUG((DEBUG_ERROR, "[SPS] ERROR: (ICC) Unable to Set ICC Clock SSC Setting (%r)\n", Status));
      }
    }
    else
    {
      DEBUG((DEBUG_INFO, "[SPS] (ICC) ME doesn't support ICC Clock Settings \n"));
    }
  }
  else
  {
    DEBUG((DEBUG_ERROR, "[SPS] ERROR: (ICC) Can't retrieve current ME Policy ICC Clock SSC Setting\n"));
  }

#ifdef FSP_FLAG
  Status = SpsFspInitComplete(pHeciPpi, PeiMePlatformPolicyPpi, ME_FUNCTIONAL, HeciMsg.MeBiosVerRsp.FeatureSet,
          	  !NmFs.Bits.PerfOptBoot, (UINT8)NmFs.Bits.Cores2Disable);
  if (EFI_ERROR(Status))
  {
    DEBUG((EFI_D_ERROR, "[SPS] ERROR: Unable to complete ME FSP initialization (%r)\n", Status));
  }
#endif /* FSP_FLAG */

  return EFI_SUCCESS;
} // SpsNonS3Path()

/*****************************************************************************
 @brief
   Function reads ME Reset Counter from ME FS1

 @retval UINT       Read ME Reset Counter
**/
UINTN
GetMeResetCounter(
  VOID
  )
{
  SPS_MEFS1 MeFs1;

  MeFs1.UInt32 = HeciPciRead32(SPS_REG_MEFS1);

  return MeFs1.Bits.MeResetCounter;
} // GetMeResetCounter()

/*****************************************************************************
 @brief
   Function waits for ME Reset Counter change.

  @param[in] StartResetCounter Reset counter we wait to be changed
  @param[in] pSpsInitTimeout  reset counter we wait to be changed

 @retval EFI_SUCCESS       The function completed successfully.
 @retval EFI_TIMEOUT       Timeout on ME transition
**/
EFI_STATUS
WaitMeResetCounterChange(UINTN StartResetCounter,
                        UINT32 *pSpsInitTimeout)
{
  UINT32 InitTimeout = SPS_INIT_TIMEOUT / STALL_1MS;
  if (pSpsInitTimeout == NULL)
  {
    pSpsInitTimeout = &InitTimeout;
  }

  while (StartResetCounter == GetMeResetCounter())
  {
    if (!*pSpsInitTimeout)
    {
      DEBUG((DEBUG_ERROR, "[SPS] ERROR: Timeout when waiting for ME Reset Counter Change\n"));
      return EFI_TIMEOUT;
    }
    MicroSecondDelay(STALL_1MS);
    *pSpsInitTimeout--;
  }
  return EFI_SUCCESS;
} // WaitMeResetCounterChange()

/*****************************************************************************
 @brief
   Execute SPS pre DID reset

  @param[in] VOID

 @retval EFI_SUCCESS       The function completed successfully.
 @retval EFI_DEVICE_ERROR  No Heci device
 @retval EFI_TIMEOUT       Timeout on ME transition
**/
EFI_STATUS
SpsExecutePreDidReset(VOID)
{
  EFI_STATUS                    Status = EFI_SUCCESS;
  UINT32                        SpsInitTimeout = SPS_RESET_TIMEOUT / STALL_1MS;
  HECI_PPI                     *pHeciPpi;
  UINT32                        RspLen;
  UINTN                         resetCounter;
  union
  {
    MKHI_MSG_HMRFPO_MERESET_REQ Req;
    MKHI_MSG_HMRFPO_MERESET_RSP Rsp;
  } HeciMsg;

  DEBUG((DEBUG_WARN, "[SPS] WARNING: Execute ME pre-DID reset\n"));

  Status = PeiServicesLocatePpi(&gHeciPpiGuid, 0, NULL, (VOID**)&pHeciPpi);
  if (EFI_ERROR(Status))
  {
    DEBUG((DEBUG_ERROR, "[SPS] ERROR: Cannot locate HECI PPI (%r)\n", Status));
    ASSERT_EFI_ERROR (Status);
    return EFI_DEVICE_ERROR;
  }

  resetCounter = GetMeResetCounter();

  HeciMsg.Req.Mkhi.Data               = 0;
  HeciMsg.Req.Mkhi.Fields.Command     = HMRFPO_CMD_MERESET;
  HeciMsg.Req.Mkhi.Fields.IsResponse  = 0;
  HeciMsg.Req.Mkhi.Fields.GroupId     = MKHI_GRP_HMRFPO;
  HeciMsg.Req.Mkhi.Fields.Reserved    = 0;
  HeciMsg.Req.Mkhi.Fields.Result      = 0;
  HeciMsg.Req.Nonce                   = 0;

  RspLen = sizeof(HeciMsg);
  Status = pHeciPpi->SendwAck(HECI1_DEVICE, (UINT32*)&HeciMsg.Req,
                              sizeof(HeciMsg.Req), &RspLen,
                              BIOS_FIXED_HOST_ADDR, HECI_CORE_MESSAGE_ADDR);
  if (!EFI_ERROR (Status))
  {
    // check if reset has been accepted
    if ((HeciMsg.Req.Mkhi.Fields.Command == HMRFPO_CMD_MERESET) &&
        (HeciMsg.Req.Mkhi.Fields.GroupId == MKHI_GRP_HMRFPO) &&
        (HeciMsg.Rsp.Status == 0))
    {
      // Wait for reset counter change
      Status = WaitMeResetCounterChange(resetCounter, &SpsInitTimeout);
      if (!EFI_ERROR (Status))
      {
        if (!EFI_ERROR(WaitForMeMeaningfulState(&SpsInitTimeout)))
        {
          DEBUG((DEBUG_INFO, "[SPS] Pre-DID reset has been executed successfully\n"));
        }
        else
        {
          Status = EFI_TIMEOUT;
        }
      }
      else
      {
        DEBUG((DEBUG_ERROR, "[SPS] ERROR: Pre-DID reset, ME Reset Counter not changed (%r)\n", Status));
      }
    }
    else
    {
      DEBUG((DEBUG_ERROR, "[SPS] Pre-DID reset is not accepted by ME. Continue without reset (%r)\n", Status));
    }
  }
  else
  {
    DEBUG((DEBUG_ERROR, "[SPS] Pre-DID reset, no ACK. Continue without reset (%r)\n", Status));
    Status = EFI_SUCCESS;
  }

  return Status;
} // SpsExecutePreDidReset()

/*****************************************************************************
 @brief
   SPS PEI entry point.

 @param[in] FileHandle  PEIM file handle
 @param[in] PeiServices General purpose services available to every PEIM

 @retval EFI_SUCCESS    The function completed successfully.
**/
EFI_STATUS
SpsPeiEntryPoint(
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS                    Status;
  PEI_ME_PLATFORM_POLICY_PPI    *PeiMePlatformPolicyPpi = NULL;
  SPS_MEFS1                     MeFs1;
  SPS_MEFS2                     MeFs2;
  EFI_BOOT_MODE                 BootMode;
  EFI_PEI_NOTIFY_DESCRIPTOR     *pNotifyList;


  DEBUG((DEBUG_INFO, "[SPS] SpsPeiEntryPoint called.\n"));

  //
  // Check if there is other than SPS ME on-board.
  //
  if (!MeTypeIsSps()) {
    DEBUG((DEBUG_INFO, "[SPS] Other ME FW detected.\n"));
    goto MeNonFunctional;
  }

  //
  // Get Boot mode
  //
  BootMode = BOOT_WITH_DEFAULT_SETTINGS;
  Status = (*PeiServices)->GetBootMode(PeiServices, &BootMode);
  if (EFI_ERROR(Status))
  {
    DEBUG((DEBUG_ERROR, "[SPS] ERROR: Cannot read boot mode (%r)\n", Status));
    BootMode = BOOT_WITH_DEFAULT_SETTINGS;
  }

  Status = SetupInit(&PeiMePlatformPolicyPpi);
  //
  // Verify if it is SPS firmware running in Recovery.
  // If yes, try reset SPS
  //
  DEBUG((DEBUG_INFO, "[SPS] Pre-DID reset "));
  if ((PeiMePlatformPolicyPpi == NULL) ||
      (PeiMePlatformPolicyPpi->EnableMePreDidReset == 0))
  {
    // [SPS] Pre-DID reset
    DEBUG((DEBUG_INFO, " is disabled\n"));
  }
  else {
    // [SPS] Pre-DID reset
    DEBUG((DEBUG_INFO, " execution\n"));
    if (EFI_ERROR(WaitForMeMeaningfulState(NULL)))
    {
      goto MeNonFunctional;
    }

    MeFs1.UInt32 = HeciPciRead32(SPS_REG_MEFS1);

    if (((MeFs1.Bits.CurrentState == MEFS1_CURSTATE_RECOVERY) || (PeiMePlatformPolicyPpi->EnableMePreDidReset == 2)) &&
        (BootMode != BOOT_ON_S3_RESUME))
    {
      MeFs2.UInt32 = HeciPciRead32(SPS_REG_MEFS2);
      if (MeFs2.Bits.RecoveryCause == SPS_RCAUSE_MEERROR)
      {
        DEBUG((DEBUG_WARN, "[SPS] WARNING: ME is in recovery mode (cause: %d)\n",
                           MeFs2.Bits.RecoveryCause));
        Status = SpsExecutePreDidReset();
        MeFs1.UInt32 = HeciPciRead32(SPS_REG_MEFS1);
        switch (Status)
        {
          case EFI_TIMEOUT:
            DEBUG((DEBUG_ERROR, "[SPS] ERROR: Pre-DID reset timeout failure causes ME non-functional flow\n"));
            goto MeNonFunctional;
          break;
          case EFI_SUCCESS:
            if (MeFs1.Bits.CurrentState == MEFS1_CURSTATE_NORMAL)
            {
              DEBUG((DEBUG_INFO, "[SPS] Pre-DID reset finished successfully, ME in Normal state\n"));
            }
          break;
          default:
            DEBUG((DEBUG_ERROR, "[SPS] ERROR: Pre-DID reset failure. Continue according to ME state\n"));
          break;
        }
      }
    }
  }

  //
  // Install Protocols To be Called once HECI Initialization for PEI is performed
  //
  if (BootMode == BOOT_ON_S3_RESUME)
  {
    DEBUG((DEBUG_INFO, "[SPS] S3 resume path\n"));
    pNotifyList = &mSpsPeiS3NotifyList[0];
  }
  else
  {
    DEBUG((DEBUG_INFO, "[SPS] Non S3 boot path\n"));
    pNotifyList = &mSpsPeiNonS3NotifyList[0];
  }
  Status = (*PeiServices)->NotifyPpi(PeiServices, pNotifyList);
  if (EFI_ERROR(Status))
  {
    DEBUG((DEBUG_INFO, "[SPS] ERROR: Cannot register PPI notify handler (%r)\n", Status));
  }

  return EFI_SUCCESS;

MeNonFunctional:
  DEBUG((DEBUG_ERROR, "[SPS] ERROR: Non SPS firmware running in ME\n"
                      "             (MEFS1: 0x%08X, MEFS2: 0x%08X)\n",
                      HeciPciRead32(SPS_REG_MEFS1), HeciPciRead32(SPS_REG_MEFS2)));
  SpsHobCreate(ME_NONFUNCTIONAL, 0, FALSE, 0);

  return EFI_UNSUPPORTED;
} // SpsPeiEntryPoint()
