/**
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
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

@file:

  IeHeciInit.c

@brief:

 IE-HECI PEIM, implements IE-HECI PPI.

**/
#include <PiPei.h>
#include <Uefi.h>

#include <Ppi/IeHeci.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <Ppi/PciCfg.h>
#include <Ppi/EndOfPeiPhase.h>
#include <Ppi/PciCfg.h>

#include <Library/PeiServicesLib.h>
#include <Library/BaseLib.h>
#include <Library/PciLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>

#include <Library/MemoryAllocationLib.h>
#include <library/PcdLib.h>
#include <Library/IeHeciCoreLib.h>
#include <Library/TimerLib.h>

#include <Library/PchPcrLib.h>
#include <Register/PchRegsPcr.h>
#include <Register/PchRegsPsf.h>
#include <PchReservedResources.h>

#include <IeHeciRegs.h>
#include <IeHeciMsgs.h>
#include <Ppi/IePlatformPolicyPei.h>
#include <Library/IePlatformPolicyUpdatePeiLib.h>

/*****************************************************************************
 * Local definitions
 *****************************************************************************/
typedef struct _IE_HECI1_PPI_EXT_
{
  PEI_IE_HECI_PPI    Ppi;
  IE_HECI_DEVICE     Heci;
  HECI_MSG_HEADER MsgBuf[HECI_MSG_MAXLEN / sizeof(HECI_MSG_HEADER)];
} IE_HECI1_PPI_EXT;


/*****************************************************************************
 * Local function prototypes.
 *****************************************************************************/
EFI_STATUS
HeciSendwAck(
  IN     PEI_IE_HECI_PPI              *This,
  IN OUT UINT32                       *Message,
  IN OUT UINT32                       *Length,
  IN     UINT8                        HostAddress,
  IN     UINT8                        IeAddress
  );

EFI_STATUS
HeciReadMsg(
  IN     PEI_IE_HECI_PPI              *This,
  IN     UINT32                       Blocking,
  IN     UINT32                       *MessageBody,
  IN OUT UINT32                       *Length
  );

EFI_STATUS
HeciSendMsg(
  IN     PEI_IE_HECI_PPI              *This,
  IN     UINT32                       *Message,
  IN     UINT32                       Length,
  IN     UINT8                        HostAddress,
  IN     UINT8                        IeAddress
  );

EFI_STATUS
HeciInitialize(
  IN     PEI_IE_HECI_PPI              *This
  );

EFI_STATUS
HeciMemoryInitDoneNotify(
  IN      PEI_IE_HECI_PPI             *This,
  IN      UINT32                      MrcStatus,
  IN OUT  UINT8                       *Action
  );


BOOLEAN
ConfigureHeciDevices(VOID);

VOID
ConfigureHeciDevice(
  IN      UINT16                      HeciDevReg,
  IN      UINT8                       Enabled
  );

/*****************************************************************************
 * Function definitions.
 *****************************************************************************/

/**
  Internal function performing Heci platform PPIs init needed in PEI phase

  @param[in] PeiServices          General purpose services available to every PEIM.

  @retval EFI_SUCCESS             The function completed successfully.
  @retval EFI_DEVICE_ERROR        ME FPT is bad
 **/
EFI_STATUS
PeiInstallHeciPpi (
  IN CONST EFI_PEI_SERVICES **PeiServices)
{
  EFI_STATUS  Status;

   struct _HECI_PPI_{
     EFI_PEI_PPI_DESCRIPTOR    Desc;
     IE_HECI1_PPI_EXT          Heci1PpiExt;
   } *pHeciPpi;

   //
   // Initialize Heci platform PPIs
   // Do not use EdkIIGlueLib here because PeiService pointer used in GlueLib
   // is not updated after memory installed.
   //
   pHeciPpi = AllocatePool(sizeof(*pHeciPpi));
   ASSERT(pHeciPpi);
   pHeciPpi->Desc.Flags =
       EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST;
   pHeciPpi->Desc.Guid = &gPeiIeHeciPpiGuid;
   pHeciPpi->Desc.Ppi = &pHeciPpi->Heci1PpiExt;
   pHeciPpi->Heci1PpiExt.Ppi.SendwAck = HeciSendwAck;
   pHeciPpi->Heci1PpiExt.Ppi.ReadMsg = HeciReadMsg;
   pHeciPpi->Heci1PpiExt.Ppi.SendMsg = HeciSendMsg;
   pHeciPpi->Heci1PpiExt.Ppi.InitializeHeci = HeciInitialize;
   pHeciPpi->Heci1PpiExt.Ppi.MemoryInitDoneNotify = HeciMemoryInitDoneNotify;
   pHeciPpi->Heci1PpiExt.Heci.Bus = IE_HECI1_BUS;
   pHeciPpi->Heci1PpiExt.Heci.Dev = IE_HECI1_DEV;
   pHeciPpi->Heci1PpiExt.Heci.Fun = IE_HECI1_FUN;
   pHeciPpi->Heci1PpiExt.Heci.Hidm = HECI_HIDM_MSI;
   pHeciPpi->Heci1PpiExt.Heci.Mbar = HECI1_IE_MBAR_DEFAULT;

   Status = (**PeiServices).InstallPpi(PeiServices, &pHeciPpi->Desc);

   if(EFI_ERROR(Status)){
     DEBUG((EFI_D_ERROR, "[IE] ERROR: Can't install HeciPpi\n"));
   }else{
     DEBUG((EFI_D_INFO, "[IE] HeciPpi installed.\n"));
   }

   ASSERT_EFI_ERROR(Status);

   return Status;

}

/**
  The entry point of the HECI PEIM. Installs HECI PPI interface.

  @param[in]  FileHandle  Handle of the file being invoked.
  @param[in]  PeiServices Describes the list of possible PEI Services.

  @retval EFI_SUCCESS   Indicates that HECI initialization completed successfully.
 **/
EFI_STATUS
IeHeciPeimEntryPoint (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS  Status;

  //
  // Install HECI PPI
  //
  Status = PeiInstallHeciPpi(PeiServices);
  ASSERT_EFI_ERROR (Status);

  return Status;
}


/**
  Initialize HECI-1 for use in PEI phase.

  Determines if the HECI-1 device is present and, if present, initializes it for
  use by the BIOS.

  param[in] This            The address of HECI PPI

  retval EFI_SUCCESS        The function completed successfully.
  retval EFI_DEVICE_ERROR   No HECI device
 **/
EFI_STATUS
HeciInitialize (
  IN     PEI_IE_HECI_PPI                 *This
  )
{
  UINT32 Timeout = PEI_HECI_INIT_TIMEOUT / HECI_TIMEOUT_UNIT;

  if (This == NULL){
    return EFI_INVALID_PARAMETER;
  }
  return HeciInit(&((IE_HECI1_PPI_EXT*)This)->Heci, &Timeout);
} // HeciInitialize()


/**
  Read a message from HECI queue.

  @param[in] This              The address of HECI PPI.
  @param[in] Blocking          Used to determine if the read is BLOCKING or NON_BLOCKING
  @param[in] MessageBody       Pointer to a buffer used to receive a message.
  @param[in, out] Length       Pointer to the length of the buffer on input and the length
                               of the message on return. (in bytes)

  @retval EFI_SUCCESS          One message packet read
  @retval EFI_TIMEOUT          HECI is not ready for communication
  @retval EFI_DEVICE_ERROR     Zero-length message packet read
  @retval EFI_BUFFER_TOO_SMALL The caller's buffer was not large enough
 **/
EFI_STATUS
HeciReadMsg (
  IN     PEI_IE_HECI_PPI        *This,
  IN     UINT32                 Blocking,
  IN     UINT32                 *MessageBody,
  IN OUT UINT32                 *Length
  )
{
  EFI_STATUS       Status;
  UINT32           i, Timeout, MsgLen;
  UINT32           FrgLen;
  UINT8            *pFrgBdy;
  HECI_MSG_HEADER  *pFrgHdr;

  if (This == NULL || MessageBody == NULL || Length == NULL){
    return EFI_INVALID_PARAMETER;
  }
  Timeout = (Blocking) ? (PEI_HECI_READ_TIMEOUT / HECI_TIMEOUT_UNIT) : 0;
  MsgLen = 0;
  pFrgHdr = &((IE_HECI1_PPI_EXT*)This)->MsgBuf[0];
  pFrgBdy = (UINT8*)&((IE_HECI1_PPI_EXT*)This)->MsgBuf[1];
  do{
    FrgLen = sizeof(((IE_HECI1_PPI_EXT*)This)->MsgBuf);
    Status = HeciMsgRecv(&((IE_HECI1_PPI_EXT*)This)->Heci, &Timeout, pFrgHdr, &FrgLen);
    if (EFI_ERROR(Status)){
      break;
    } else {
      FrgLen -= sizeof(HECI_MSG_HEADER);
      if (MsgLen + FrgLen > *Length){
        FrgLen = *Length - MsgLen;
      }
      for (i = 0; i < FrgLen; i++){
        ((UINT8*)MessageBody)[MsgLen++] = pFrgBdy[i];
      }
    }
  }
  while (!pFrgHdr->Bits.MsgComplete);

  *Length = MsgLen;
  return Status;
}


/**
  Function sends one message (of any length) through the HECI circular buffer.

  @param[in] This                 The address of HECI PPI.
  @param[in] Message              Pointer to the message data to be sent.
  @param[in] Length               Length of the message in bytes.
  @param[in] HostAddress          The address of the host processor.
  @param[in] IeAddress            Address of the ME subsystem the message is being sent to.

  @retval EFI_SUCCESS             One message packet sent.
  @retval EFI_DEVICE_ERROR        Failed to initialize HECI
  @retval EFI_TIMEOUT             HECI is not ready for communication
  @exception EFI_UNSUPPORTED      Current ME mode doesn't support send message through HEC
 **/
EFI_STATUS
HeciSendMsg (
  IN      PEI_IE_HECI_PPI          *This,
  IN      UINT32                   *Message,
  IN      UINT32                   Length,
  IN      UINT8                    HostAddress,
  IN      UINT8                    IeAddress)
{
  EFI_STATUS      Status;
  UINT32          i, Timeout;
  UINT32          Len = Length;
  UINT8           *pMsg = (UINT8*)Message;
  UINT8           *pFrgBdy;
  HECI_MSG_HEADER *pFrgHdr;

  if (This == NULL || Message == NULL){
    return EFI_INVALID_PARAMETER;
  }
  Timeout = PEI_HECI_SEND_TIMEOUT / HECI_TIMEOUT_UNIT;
  //
  // Fragment the message into queue-sized packets and loop until completely sent
  //
  pFrgHdr = &((IE_HECI1_PPI_EXT*)This)->MsgBuf[0];
  pFrgBdy = (UINT8*)&((IE_HECI1_PPI_EXT*)This)->MsgBuf[1];
  pFrgHdr->DWord = 0;
  pFrgHdr->Bits.IeAddress = IeAddress;
  pFrgHdr->Bits.HostAddress = HostAddress;
  ASSERT(((IE_HECI1_PPI_EXT*)This)->Heci.HMtu > 0); // make sure HECI is initialized when sending
  do{
    if (Len <= ((IE_HECI1_PPI_EXT*)This)->Heci.HMtu){
      pFrgHdr->Bits.Length = Len;
      pFrgHdr->Bits.MsgComplete = 1; // Set msg complete in last fragment

    } else {
      pFrgHdr->Bits.Length = ((IE_HECI1_PPI_EXT*)This)->Heci.HMtu;
    }
    for (i = 0; i < pFrgHdr->Bits.Length; i++){
      pFrgBdy[i] = pMsg[i];
    }
    pMsg += pFrgHdr->Bits.Length;
    Len -= pFrgHdr->Bits.Length;
    Status = HeciMsgSend(&((IE_HECI1_PPI_EXT*)This)->Heci, &Timeout, pFrgHdr);
    if (EFI_ERROR(Status)){
      goto GetOut;
    }
  }
  while (Len > 0);

GetOut:
  return Status;
}


/**
  Send HECI message and wait for response.

  @param[in,out]  This                 The address of HECI PPI.
  @param[in]      Message              Pointer to the message data to be sent.
  @param[in]      Length               Length of the message in bytes.
  @param[in]      HostAddress          The address of the host processor.
  @param[in]      IeAddress            Address of the ME subsystem the message is being sent to.


 **/
EFI_STATUS
HeciSendwAck(
  IN      PEI_IE_HECI_PPI              *This,
  IN OUT  UINT32                       *Message,
  IN OUT  UINT32                       *Length,
  IN      UINT8                        HostAddress,
  IN      UINT8                        IeAddress)
{
  EFI_STATUS  Status;

  Status = HeciSendMsg(This, Message, *Length, HostAddress, IeAddress);
  if (!EFI_ERROR(Status)){
    Status = HeciReadMsg(This, TRUE, Message, Length);
  }
  return Status;
} // HeciSendwAck()

/**
  Send HECI message to signal that memory is ready to use after MRC phase.

  @param[in]      This                 The address of HECI PPI.
  @param[in]      MrcStatus            Status returned by MRC.
  @param[in,out]  Action               Requested action at the bios side.

  @retval EFI_SUCCESS                  If message was sent.
 **/
EFI_STATUS HeciMemoryInitDoneNotify(
  IN      PEI_IE_HECI_PPI    *This,
  IN      UINT32              MrcStatus,
  IN OUT  UINT8              *Action)
{
  EFI_STATUS             Status;
  MEMORY_INIT_DONE_MSG   Msg;
  UINT32                 MsgLength;
  IEFS1                  IeFs1;

  // update HECI devices based on setup configuration first
  if (ConfigureHeciDevices() == FALSE) {
    // IE is disabled in SoftStraps. No activity for IE
    DEBUG(( EFI_D_INFO, "[IE] IE communication disabled. Can't send MemoryInitDoneNotify\n" ));
    return EFI_UNSUPPORTED;
  }

  // Read IE FW status
  IeFs1.UInt32 = HeciPciReadIefs1();

  // Check the IE Firmware status
  if( IeFs1.Bits.ErrorCode != 0 || IeFs1.Bits.CurrentState <= 1 ){
    DEBUG(( EFI_D_ERROR, "[IE] ERROR: IE not working. IE_FS1=%X\n",IeFs1.UInt32));
    return EFI_DEVICE_ERROR;
  }else{
    DEBUG(( EFI_D_INFO, "[IE] IE_FS1=%X.\n",IeFs1.UInt32));
  }

  // Initialize IE-HECI
  Status = This->InitializeHeci(This);

  if(EFI_ERROR(Status)){
    DEBUG((EFI_D_ERROR, "[IE] ERROR: HECI Init failed. Status = %X\n", Status));
    return Status;
  }else{
    DEBUG((EFI_D_INFO, "[IE] HECI Init done.\n"));
  }


  DEBUG((EFI_D_INFO, "[IE] Memory Init Done procedure started.\n"));

  Msg.Request.Header.Data               = 0;
  Msg.Request.Header.Fields.Command     = MEMORY_INIT_DONE_CMD;
  Msg.Request.Header.Fields.IsResponse  = 0;
  Msg.Request.Header.Fields.GroupId     = BUP_COMMON_GROUP_ID;
  Msg.Request.Header.Fields.Reserved    = 0;
  Msg.Request.Header.Fields.Result      = 0;
  Msg.Request.Data.Fields.Status        = MrcStatus;

  MsgLength  = sizeof ( MEMORY_INIT_DONE_MSG);

  Status = This->SendwAck (
                      This,
                      (UINT32*) (&Msg),
                      &MsgLength,
                      BIOS_FIXED_HOST_ADDR,
                      HECI_CORE_MESSAGE_ADDR
                      );

  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "[IE] Error: Memory Init Done procedure fails. Status=%x\n",Status));
    *Action = 0;
  } else{
    DEBUG((EFI_D_INFO, "[IE] Memory Init Done procedure ends. Action = %X\n", Msg.Response.Action));
    *Action = Msg.Response.Action;
  }

  return Status;
}


VOID
ConfigureHeciDevice(
  IN UINT16   HeciDevReg,
  IN UINT8    Enabled
  )
{

  if (!Enabled) {
    PchPcrAndThenOr32 (
      PID_PSF3, HeciDevReg + R_PCH_PCR_PSFX_T0_SHDW_PCIEN,
      (UINT32)~0,
      B_PCH_PCR_PSFX_T0_SHDW_PCIEN_FUNDIS
      );
  } else {
    PchPcrAndThenOr32 (
      PID_PSF3, HeciDevReg + R_PCH_PCR_PSFX_T0_SHDW_PCIEN,
      (UINT32)~B_PCH_PCR_PSFX_T0_SHDW_PCIEN_FUNDIS,
      0
      );
  }
}

BOOLEAN
ConfigureHeciDevices(VOID)
{

  UINT16                        SubsystemId;
  UINT8                         IeHeci1Enabled;
  UINT8                         IeHeci2Enabled;
  UINT8                         IeHeci3Enabled;
  UINT8                         IeIderEnabled;
  UINT8                         IeKtEnabled;
  EFI_STATUS                    Status;
  PEI_IE_PLATFORM_POLICY_PPI    *PeiIePlatformPolicyPpi;
  BOOLEAN                       retVal = TRUE;

  // Update subsystemId for all IE HECI devs
  Status = PeiServicesLocatePpi (
                            &gPeiIePlatformPolicyPpiGuid,
                            0,
                            NULL,
                            &PeiIePlatformPolicyPpi
                            );

  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "[IE] ERROR: Can't get IE Policy PPI (Status=%X). Use module default.\n", Status));
    ASSERT_EFI_ERROR (Status);

    SubsystemId = 0x8086;
    if (IeTypeIsDfx() && IeIsEnabled()) {
      IeHeci1Enabled = 1;
      IeHeci2Enabled = 1;
      IeHeci3Enabled = 1;
      IeIderEnabled = 1;
      IeKtEnabled = 1;
      DEBUG(( EFI_D_INFO, "[IE] DfxIE Detected - IE Heci devices enabled.\n"));
    } else {
      IeHeci1Enabled = 0;
      IeHeci2Enabled = 0;
      IeHeci3Enabled = 0;
      IeIderEnabled = 0;
      IeKtEnabled = 0;
    }
  } else {
    SubsystemId = PeiIePlatformPolicyPpi->SubsystemId;
    IeHeci1Enabled = PeiIePlatformPolicyPpi->IeHeci1Enabled;
    IeHeci2Enabled = PeiIePlatformPolicyPpi->IeHeci2Enabled;
    IeHeci3Enabled = PeiIePlatformPolicyPpi->IeHeci3Enabled;
    IeIderEnabled = PeiIePlatformPolicyPpi->IeIderEnabled;
    IeKtEnabled = PeiIePlatformPolicyPpi->IeKtEnabled;
  }

  if (IeHeci1Enabled == 0) {
    retVal = FALSE; // no Heci communication
  }

  DEBUG((EFI_D_INFO, "[IE] Update subsystemId to value %X.\n", SubsystemId));
  PciWrite16(PCI_LIB_ADDRESS(IE_HECI1_BUS, IE_HECI1_DEV, IE_HECI1_FUN, 0) +  HECI_R_SUBSYSID, SubsystemId);
  PciWrite16(PCI_LIB_ADDRESS(IE_HECI1_BUS, IE_HECI1_DEV, IE_HECI2_FUN, 0) +  HECI_R_SUBSYSID, SubsystemId);
  PciWrite16(PCI_LIB_ADDRESS(IE_HECI1_BUS, IE_HECI1_DEV, IE_HECI3_FUN, 0) +  HECI_R_SUBSYSID, SubsystemId);
  PciWrite16(PCI_LIB_ADDRESS(IE_HECI1_BUS, IE_HECI1_DEV, IE_IDER_FUN,  0) +  HECI_R_SUBSYSID, SubsystemId);
  PciWrite16(PCI_LIB_ADDRESS(IE_HECI1_BUS, IE_HECI1_DEV, IE_KT_FUN,    0) +  HECI_R_SUBSYSID, SubsystemId);



  ConfigureHeciDevice(R_PCH_PCR_PSF3_T0_SHDW_HECI1_REG_BASE, IeHeci1Enabled );
  DEBUG((EFI_D_INFO, "[IE] HECI1: %s.\n",(IeHeci1Enabled?L"enabled":L"disabled") ));

  ConfigureHeciDevice(R_PCH_PCR_PSF3_T0_SHDW_HECI2_REG_BASE, IeHeci2Enabled);
  DEBUG((EFI_D_INFO, "[IE] HECI2: %s.\n",(IeHeci2Enabled?L"enabled":L"disabled")));

  ConfigureHeciDevice(R_PCH_PCR_PSF3_T0_SHDW_HECI3_REG_BASE, IeHeci3Enabled );
  DEBUG((EFI_D_INFO, "[IE] HECI3: %s.\n",(IeHeci3Enabled?L"enabled":L"disabled")));

  ConfigureHeciDevice(R_PCH_PCR_PSF3_T0_SHDW_IDER_REG_BASE, IeIderEnabled);
  DEBUG((EFI_D_INFO, "[IE] Ider: %s.\n",(IeIderEnabled?L"enabled":L"disabled")));

  ConfigureHeciDevice(R_PCH_PCR_PSF3_T0_SHDW_KT_REG_BASE, IeKtEnabled);
  DEBUG((EFI_D_INFO, "[IE] KT: %s.\n",(IeKtEnabled?L"enabled":L"disabled")));

  return retVal;
}
