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
/** @file SmmIpmiIeReportPostCode.c
    IPMI SmmIpmiIeReportPostCode Driver.
**/

#include <SmmIpmiIeReportPostCode.h>

// Global Variables
EFI_IPMI_TRANSPORT  *gSmmIpmiTransport = NULL;
UINT8               gSmmCheckpointBuffer[MAXIMUM_POST_CODE];
UINTN               gSmmBufferCount = 0;
UINT8               gSmmCurrentBufferIndex = 0;

/**
    Sends obtained Checkpoint to IE
     
    @param Checkpoint      Checkpoint to send

    @retval EFI_STATUS     Status of SendIpmiCommand

**/

EFI_STATUS
SendPostCode (
    IN UINT8   Checkpoint
)
{
    EFI_STATUS         Status;
    EFI_POSTCODE_DATA  PostCodeData;
    UINT8              ResponseData;
    UINT8              ResponseDataSize = sizeof(ResponseData);
   
    PostCodeData.PostCode = Checkpoint;
    
    //Send EFI_POSTCODE_DATA to IE
    Status = gSmmIpmiTransport->SendIpmiCommand (
                                    gSmmIpmiTransport,
                                    EFI_SM_NETFN_ADD_POSTCODE_OEM,
                                    IE_LUN,
                                    EFI_ADD_POSTCODE_OEM,
                                    (UINT8 *) &PostCodeData,
                                    sizeof (EFI_POSTCODE_DATA),
                                    (UINT8 *)&ResponseData,
                                    &ResponseDataSize );
    SERVER_IPMI_IE_DEBUG((EFI_D_INFO, "Status of Sending POST code - %x to IE: %r\n",Checkpoint,Status));
    return Status;
}

/**
    Sends Post codes to IE
        
    @param PeiServices pointer to the PEI Boot Services table,
    @param Checkpoint checkpoint to display

    @retval VOID

**/
VOID SmmIpmiIeReportPostCode(
    IN VOID *Dummy,
    IN UINT8 Checkpoint
)
{
    EFI_STATUS    Status = EFI_SUCCESS;
    
    //Save SMM post codes to Buffer	    
    gSmmCheckpointBuffer[gSmmBufferCount++] = Checkpoint;
    
    if (gSmmIpmiTransport == NULL) {
      
        // Locating gSmmIpmiIeTransport protocol
        Status = gSmst->SmmLocateProtocol (
                  &gEfiSmmIpmiIeTransportProtocolGuid,
                  NULL,
                  (VOID **)&gSmmIpmiTransport);
        
        if (EFI_ERROR(Status))
            return;
    }
    
    //Send SMM post codes in buffer to IE	    
    for (; gSmmCurrentBufferIndex < gSmmBufferCount; gSmmCurrentBufferIndex++) {
        Status = SendPostCode(gSmmCheckpointBuffer[gSmmCurrentBufferIndex]);
        
        if (EFI_ERROR(Status))
            return;    
    }  
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
