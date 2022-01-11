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

/** @file IeDcmi.c
    Communicate with IE to transfer DCMI messages.

**/
#include "IpmiIeCommunicate.h"
#include <Protocol/IeHeciDxe.h>
#include "IeDcmi.h"

extern IE_HECI_PROTOCOL             *gIeProtocol;

EFI_STATUS
SendDataToIeDcmi (
  IN EFI_IPMI_IE_INSTANCE_DATA    *IpmiInstance,
  IN VOID                          *Context,
  IN UINT8                         *Data,
  IN UINT8                         DataSize ) {

    EFI_STATUS Status;

    Status = gIeProtocol->SendMsg(
                         gIeProtocol,
                         (UINT32*)Data,
                         (UINT32)DataSize,
                         BIOS_IE_HECI_SLAVE_ADDRESS,
                         DCMI_IE_HECI_SLAVE_ADDRESS);
    return Status;
}

EFI_STATUS
ReceiveDataFromIeDcmi (
  IN  EFI_IPMI_IE_INSTANCE_DATA    *IpmiInstance,
  IN  VOID                          *Context,
  OUT UINT8                        *Data,
  OUT UINT8                        *DataSize ) {

    EFI_STATUS Status;
    Status = gIeProtocol->ReadMsg(
                            gIeProtocol,
                            TRUE,
                            (UINT32*)Data,
                            (UINT32*)DataSize);
    return Status;
}


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
