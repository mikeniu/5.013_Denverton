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
#include <Library/PeiServicesLib.h>
#include <Ppi/IeHeci.h>
#include <IeDcmi.h>



EFI_STATUS
SendDataToIeDcmi (
  IN EFI_IPMI_IE_INSTANCE_DATA    *IpmiInstance,
  IN VOID                          *Context,
  IN UINT8                         *Data,
  IN UINT8                         DataSize ) {

    IE_HECI_PPI_EXT       *IePpi;
    EFI_STATUS            Status;

    //
    // Locate the IE Ppi for communication.
    // Return if Ppi is not available.
    //
    Status = PeiServicesLocatePpi (
               &gPeiIeHeciPpiGuid,
               0,
               NULL,
               (VOID **) &IePpi
               );
    if(EFI_ERROR(Status)) {
        return Status;
    }

    Status = IePpi->Ppi.SendMsg (
                        &IePpi->Ppi,
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

    IE_HECI_PPI_EXT      *IePpi;
    EFI_STATUS           Status;
    //
    // Locate the IE Ppi for communication.
    // Return if Ppi is not available.
    //
    Status = PeiServicesLocatePpi (
               &gPeiIeHeciPpiGuid,
               0,
               NULL,
               (VOID **) &IePpi
               );
    if(EFI_ERROR(Status)) {
        return Status;
    }
    Status = IePpi->Ppi.ReadMsg (
                         &IePpi->Ppi,
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
