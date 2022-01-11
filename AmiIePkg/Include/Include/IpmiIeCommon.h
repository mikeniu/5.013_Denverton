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

/** @file IpmiIeCommon.h
    IPMI IE Controller Common Structures and Function Prototypes.

**/

#ifndef _EFI_IPMI_IE_COMMON_H_
#define _EFI_IPMI_IE_COMMON_H_

//----------------------------------------------------------------

#include <Token.h>

//----------------------------------------------------------------

//
//    Macros Used
//
#define MAX_TEMP_DATA                    0xFF

#pragma pack(1)

//
//    Ie Elog instance data
//
typedef struct {
    UINTN                 Signature;
    UINT8                 SlaveAddress;
    UINT8                 TempData[MAX_TEMP_DATA];
    EFI_IE_STATUS         IeStatus;
    UINT64                ErrorStatus;
    UINT8                 SoftErrorCount;
    EFI_IPMI_TRANSPORT    IpmiTransport;
} EFI_IPMI_IE_INSTANCE_DATA;

EFI_STATUS
EfiIpmiIeStatus (
  IN  EFI_IPMI_TRANSPORT    *This,
  OUT EFI_IE_STATUS         *IeStatus,
  OUT EFI_SM_COM_ADDRESS    *ComAddress );

EFI_STATUS
IpmiIeStatus (
  IN  EFI_IPMI_TRANSPORT    *This,
  OUT EFI_IE_STATUS        *IeStatus,
  OUT EFI_SM_COM_ADDRESS    *ComAddress,
  IN  VOID                  *Common );

//   IE DCMI interface starts

EFI_STATUS
IpmiIeDcmiSendCommand  (
  IN  EFI_IPMI_TRANSPORT     *This,
  IN  UINT8                  NetFunction,
  IN  UINT8                  Lun,
  IN  UINT8                  Command,
  IN  UINT8                  *CommandData,
  IN  UINT8                  CommandDataSize,
  OUT UINT8                  *ResponseData,
  OUT UINT8                  *ResponseDataSize,
  IN  VOID                   *Common );

EFI_STATUS
EfiIeDcmiSendCommand (
  IN  EFI_IPMI_TRANSPORT     *This,
  IN  UINT8                  NetFunction,
  IN  UINT8                  Lun,
  IN  UINT8                  Command,
  IN  UINT8                  *CommandData,
  IN  UINT8                  CommandDataSize,
  OUT UINT8                  *ResponseData,
  OUT UINT8                  *ResponseDataSize );

EFI_STATUS
SendDataToIeDcmi (
  IN EFI_IPMI_IE_INSTANCE_DATA    *IpmiInstance,
  IN VOID                          *Context,
  IN UINT8                         *Data,
  IN UINT8                         DataSize );

EFI_STATUS
ReceiveDataFromIeDcmi (
  IN  EFI_IPMI_IE_INSTANCE_DATA    *IpmiInstance,
  IN  VOID                         *Context,
  OUT UINT8                        *Data,
  OUT UINT8                        *DataSize );

#pragma pack()

#endif

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
