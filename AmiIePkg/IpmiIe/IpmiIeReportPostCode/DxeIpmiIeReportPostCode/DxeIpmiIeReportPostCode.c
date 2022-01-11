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

/** @file DxeIpmiIeReportPostCode.c
    IPMI DxeIpmiIeReportPostCode Driver.
**/

#include <DxeIpmiIeReportPostCode.h>

//Global Variables
EFI_IPMI_TRANSPORT  *gIpmiTransport = NULL;
UINT8               gCheckpointBuffer[MAXIMUM_POST_CODE];
UINT8               gBufferCount = 0;
UINT8               gCurrentBufferIndex = 0;
BOOLEAN             gStartDxePostCodeStatus = TRUE;

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
    Status = gIpmiTransport->SendIpmiCommand (
                                 gIpmiTransport,
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
    Sends Post codes to IE through HECI
        
    @param PeiServices pointer to the PEI Boot Services table,
    @param Checkpoint  checkpoint to send to IE.

    @retval VOID

**/

VOID DxeIpmiIeReportPostCode(
    IN VOID *Dummy,
    IN UINT8 Checkpoint
)
{
    EFI_STATUS          Status = EFI_SUCCESS;
    POST_CODE_INFO_HOB  *PostCodeHob = NULL;
    
    if (gStartDxePostCodeStatus == TRUE) {
        //Obtain POST_CODE_INFO_HOB
        PostCodeHob = (POST_CODE_INFO_HOB*)GetFirstGuidHob(&gAmiPostCodeInfoHobGuid);

        //Save the PEI POST CODES in DXE POST CODE Buffer
        for (; PostCodeHob, PostCodeHob->CurrentBufferIndex < PostCodeHob->BufferCount;
             PostCodeHob->CurrentBufferIndex++) {
             gCheckpointBuffer[gBufferCount++] = PostCodeHob->CheckpointBuffer[PostCodeHob->CurrentBufferIndex];
        }
        gStartDxePostCodeStatus = FALSE;
    }
    
	//Save DXE check points to Buffer    
    gCheckpointBuffer[gBufferCount++] = Checkpoint;
    
    if (gIpmiTransport == NULL) {
        //Locating gEfiDxeIpmiIeTransportProtocol
        Status = gBS->LocateProtocol (
                  &gEfiDxeIpmiIeTransportProtocolGuid,
                  NULL,
                  (VOID **)&gIpmiTransport);
                  
        if (EFI_ERROR(Status))
        	return;
    }
  
    //If we are here, start sending the post codes to IE.
	for (; gCurrentBufferIndex < gBufferCount; gCurrentBufferIndex++) {
        Status = SendPostCode (gCheckpointBuffer[gCurrentBufferIndex]);

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
