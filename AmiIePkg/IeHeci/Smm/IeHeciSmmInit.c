//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/**


@file:

  IeHeciSmmInit.c

@brief:

 IE-HECI Smm Driver, implements IE-HECI Protocol.

**/

#include <Uefi.h>
    
#include <Protocol/IeHeciDxe.h>

#include <Library/BaseLib.h>
#include <Library/PciLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>

#include <Library/MemoryAllocationLib.h>
#include <library/PcdLib.h>
#include <Library/IeHeciCoreLib.h>
#include <Library/TimerLib.h>
#include <Library/SmmServicesTableLib.h>

#include <Register/PchRegsPcr.h>
#include <Register/PchRegsPsf.h>
#include <PchReservedResources.h>

#include <IeHeciRegs.h>
#include <IeHeciMsgs.h>




/*****************************************************************************
 * Local definitions
 *****************************************************************************/
typedef struct
{
  IE_HECI_PROTOCOL Protocol;
  IE_HECI_DEVICE       Heci;
  HECI_MSG_HEADER      MsgBuf[HECI_MSG_MAXLEN / sizeof(HECI_MSG_HEADER)];
} IE_HECI1_PROTOCOL_INSTANCE;

IE_HECI1_PROTOCOL_INSTANCE  *gSmmIeProtocolInstance;

/*****************************************************************************
 * Local function prototypes.
 *****************************************************************************/
EFI_STATUS
HeciSendwAck(
  IN     IE_HECI_PROTOCOL         *This,
  IN OUT UINT32                   *Message,
  IN OUT UINT32                   *Length,
  IN     UINT8                    HostAddress,
  IN     UINT8                    IeAddress
  );

EFI_STATUS
HeciReadMsg(
  IN     IE_HECI_PROTOCOL         *This,
  IN     UINT32                   Blocking,
  IN     UINT32                   *MessageBody,
  IN OUT UINT32                   *Length
  );

EFI_STATUS
HeciSendMsg(
  IN     IE_HECI_PROTOCOL         *This,
  IN     UINT32                   *Message,
  IN     UINT32                   Length,
  IN     UINT8                    HostAddress,
  IN     UINT8                    IeAddress
  );

EFI_STATUS
HeciInitialize(
  IN     IE_HECI_PROTOCOL         *This
  );


/*****************************************************************************
 * Function definitions.
 *****************************************************************************/

/**
  The entry point of the HECI SMM Driver. Installs HECI Protocol interface.

  @param[in]  ImageHandle Handle of the file being invoked.
  @param[in]  SystemTable Pointer to system table.
  
  @retval EFI_SUCCESS   Indicates that HECI protocol install completed successfully.
 **/
EFI_STATUS
IeHeciSmmEntryPoint (
IN EFI_HANDLE             ImageHandle,
IN EFI_SYSTEM_TABLE       *SystemTable
)
{
  EFI_STATUS               Status;
  EFI_HANDLE               IeHandle = NULL;

  gSmmIeProtocolInstance = AllocateZeroPool(sizeof (IE_HECI1_PROTOCOL_INSTANCE));
  if (!gSmmIeProtocolInstance) {
     return EFI_OUT_OF_RESOURCES;
  }

  gSmmIeProtocolInstance->Protocol.InitializeHeci = HeciInitialize;
  gSmmIeProtocolInstance->Protocol.ReadMsg = HeciReadMsg;
  gSmmIeProtocolInstance->Protocol.SendMsg = HeciSendMsg;
  gSmmIeProtocolInstance->Protocol.SendwAck = HeciSendwAck;
  gSmmIeProtocolInstance->Heci.Bus = IE_HECI1_BUS;
  gSmmIeProtocolInstance->Heci.Dev = IE_HECI1_DEV;
  gSmmIeProtocolInstance->Heci.Fun = IE_HECI1_FUN;
  gSmmIeProtocolInstance->Heci.Hidm = HECI_HIDM_MSI;
  gSmmIeProtocolInstance->Heci.Mbar = HECI1_IE_MBAR_DEFAULT;

  Status = gSmst->SmmInstallProtocolInterface (
                        &IeHandle,
                        &gSmmIeHeciProtocolGuid,
                        EFI_NATIVE_INTERFACE,
                        &gSmmIeProtocolInstance->Protocol);
  if (EFI_ERROR(Status)) {
     FreePool(gSmmIeProtocolInstance);
     return Status;
  }

  return Status;
}


/**
  Initialize HECI-1 for use in SMM phase.

  Determines if the HECI-1 device is present and, if present, initializes it for
  use by the BIOS.

  param[in] This            The address of IE HECI Protocol

  retval EFI_SUCCESS        The function completed successfully.
  retval EFI_DEVICE_ERROR   No HECI device
 **/
EFI_STATUS
HeciInitialize (
  IN     IE_HECI_PROTOCOL                 *This
  )
{
   UINT32 Timeout = HECI_INIT_TIMEOUT / HECI_TIMEOUT_UNIT;

   if (This == NULL){
    return EFI_INVALID_PARAMETER;
  }
  return HeciInit(&((IE_HECI1_PROTOCOL_INSTANCE*)This)->Heci, &Timeout);

} // HeciInitialize()


/**
  Read a message from HECI queue.

  @param[in] This              The address of HECI Protocol.
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
  IN     IE_HECI_PROTOCOL       *This,
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
  Timeout = (Blocking) ? (HECI_READ_TIMEOUT / HECI_TIMEOUT_UNIT) : 0;
  MsgLen = 0;
  pFrgHdr = &((IE_HECI1_PROTOCOL_INSTANCE*)This)->MsgBuf[0];
  pFrgBdy = (UINT8*)&((IE_HECI1_PROTOCOL_INSTANCE*)This)->MsgBuf[1];
  do{
    FrgLen = sizeof(((IE_HECI1_PROTOCOL_INSTANCE*)This)->MsgBuf);
    Status = HeciMsgRecv(&((IE_HECI1_PROTOCOL_INSTANCE*)This)->Heci, &Timeout, pFrgHdr, &FrgLen);
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

  @param[in] This                 The address of HECI Protocol.
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
  IN      IE_HECI_PROTOCOL     *This,
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
  Timeout = HECI_SEND_TIMEOUT / HECI_TIMEOUT_UNIT;
  //
  // Fragment the message into queue-sized packets and loop until completely sent
  //
  pFrgHdr = &((IE_HECI1_PROTOCOL_INSTANCE*)This)->MsgBuf[0];
  pFrgBdy = (UINT8*)&((IE_HECI1_PROTOCOL_INSTANCE*)This)->MsgBuf[1];
  pFrgHdr->DWord = 0;
  pFrgHdr->Bits.IeAddress = IeAddress;
  pFrgHdr->Bits.HostAddress = HostAddress;
  ASSERT(((IE_HECI1_PROTOCOL_INSTANCE*)This)->Heci.HMtu > 0); // make sure HECI is initialized when sending
  do{
    if (Len <= ((IE_HECI1_PROTOCOL_INSTANCE*)This)->Heci.HMtu){
      pFrgHdr->Bits.Length = Len;
      pFrgHdr->Bits.MsgComplete = 1; // Set msg complete in last fragment

    } else {
      pFrgHdr->Bits.Length = ((IE_HECI1_PROTOCOL_INSTANCE*)This)->Heci.HMtu;
    }
    for (i = 0; i < pFrgHdr->Bits.Length; i++){
      pFrgBdy[i] = pMsg[i];
    }
    pMsg += pFrgHdr->Bits.Length;
    Len -= pFrgHdr->Bits.Length;
    Status = HeciMsgSend(&((IE_HECI1_PROTOCOL_INSTANCE*)This)->Heci, &Timeout, pFrgHdr);
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
  IN      IE_HECI_PROTOCOL         *This,
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
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
