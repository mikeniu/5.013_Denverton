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

/** @file PeiIpmiIeReportPostCode.c
    IPMI PeiIpmiIeReportPostCode Driver.
**/

#include <PeiIpmiIeReportPostCode.h>

//Notify descriptor for gEfiPeiMemoryDiscoveredPpiGuid

EFI_PEI_NOTIFY_DESCRIPTOR gUpdateIpmiTransportPpi = {
    (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gEfiPeiMemoryDiscoveredPpiGuid,
    UpdateIpmiTransportPpi
};

/**
    Callback function for gEfiPeiMemoryDiscoveredPpi. 
    To update IpmiTransportPpi pointer present in PostCodeHob.
        
    @param PeiServices      Pointer to the PEI Boot Services table,
    @param NotifyDescriptor Pointer to the instance of
                            the EFI_PEI_NOTIFY_DESCRIPTOR
    @param Ppi              Pointer to the instance of the interface
    
    @retval EFI_STATUS

**/

EFI_STATUS
EFIAPI
UpdateIpmiTransportPpi (
  IN  EFI_PEI_SERVICES            **PeiServices,
  IN  EFI_PEI_NOTIFY_DESCRIPTOR   *NotifyDescriptor,
  IN  VOID                        *Ppi )
{
    POST_CODE_INFO_HOB  *PostCodeHob = NULL;
    EFI_STATUS Status;
    
    //Obtaining POST_CODE_INFO_HOB
    PostCodeHob = (POST_CODE_INFO_HOB *)GetFirstGuidHob(&gAmiPostCodeInfoHobGuid);
    
    if(!PostCodeHob)
    	return EFI_NOT_FOUND;

    //Updating IpmiIeTransportPpi instance in POST_CODE_INFO_HOB  \
    //by locating gEfiPeiIpmiIeTransportPpiGuid
    Status = ((EFI_PEI_SERVICES*) *PeiServices)->LocatePpi (
                (CONST EFI_PEI_SERVICES**) PeiServices,
                &gEfiPeiIpmiIeTransportPpiGuid,
                0,
                NULL,
                (VOID **)&((PEI_IPMI_TRANSPORT_PPI*)PostCodeHob->IpmiTransportPpi));
    SERVER_IPMI_IE_DEBUG((EFI_D_INFO, "Locate IpmiTransportPpi %r\n", Status));
    
    return Status;
}

/**
    Sends obtained Checkpoint to IE
     
    @param PeiServices     Pointer to the PEI Boot Services table,   
    @param Checkpoint      Checkpoint to send
    @param StartofPostCode Boolean to check the start of post code 

    @retval EFI_STATUS     Status of SendIpmiCommand

**/

EFI_STATUS
SendPostCode (
    IN PEI_IPMI_TRANSPORT_PPI  *IpmiTransportPpi,
    IN UINT8                   Checkpoint
)
{
    EFI_STATUS         Status;
    EFI_POSTCODE_DATA  PostCodeData;
    UINT8              ResponseData;
    UINT8              ResponseDataSize = sizeof(ResponseData);
  
    PostCodeData.PostCode = Checkpoint;
    
    //Send EFI_POSTCODE_DATA to IE
    Status = IpmiTransportPpi->SendIpmiCommand (
                                   IpmiTransportPpi,
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
    @param Checkpoint  checkpoint to send to IE

    @retval VOID

**/

VOID PeiIpmiIeReportPostCode(
    IN VOID **PeiServices,
    IN UINT8 Checkpoint
)
{
    EFI_STATUS              Status = EFI_SUCCESS;
    POST_CODE_INFO_HOB      *PostCodeHob = NULL;

    //Check for AmiProgressCodeInfoHob Presence
    PostCodeHob = (POST_CODE_INFO_HOB *)GetFirstGuidHob(&gAmiPostCodeInfoHobGuid);
    
    if (PostCodeHob == NULL) {
        //If HOB not present create a new Guided type HOB
        Status = ((EFI_PEI_SERVICES*) *PeiServices)->CreateHob(
                    (CONST EFI_PEI_SERVICES**) PeiServices,
                    EFI_HOB_TYPE_GUID_EXTENSION, 
                    sizeof(POST_CODE_INFO_HOB),
                    &PostCodeHob
                    );
        if (EFI_ERROR(Status)) {
            SERVER_IPMI_IE_DEBUG((EFI_D_INFO, "Creation of HOB fails with status - %r\n",Status));
            return;
        }
        
        PostCodeHob->EfiHobGuidType.Name = gAmiPostCodeInfoHobGuid;
        PostCodeHob->BufferCount = 0;
        PostCodeHob->CurrentBufferIndex = 0;
        PostCodeHob->IpmiTransportPpi = 0;
        
    	//Register callback function for gEfiPeiMemoryDiscoveredPpiGuid
    	//to update IpmiTransportPpi instance after memory discovered
        Status = ((EFI_PEI_SERVICES*) *PeiServices)->NotifyPpi(
                    (CONST EFI_PEI_SERVICES**) PeiServices,
                    &gUpdateIpmiTransportPpi);
    }
    
    //Store checkpoint in buffer
    PostCodeHob->CheckpointBuffer[PostCodeHob->BufferCount++] = Checkpoint;
    
    //Checking for the instance of Ipmi Transport Protocol
    if (((PEI_IPMI_TRANSPORT_PPI*)PostCodeHob->IpmiTransportPpi) == NULL) {
    
        //If not, Locating PeiIpmiIeTransportPpi
        Status = ((EFI_PEI_SERVICES*) *PeiServices)->LocatePpi (
                    (CONST EFI_PEI_SERVICES**) PeiServices,
                    &gEfiPeiIpmiIeTransportPpiGuid,
                    0,
                    NULL,
                    (VOID **)&((PEI_IPMI_TRANSPORT_PPI*)PostCodeHob->IpmiTransportPpi));
 
        if (EFI_ERROR(Status))
            return;
    }
    
    for(; PostCodeHob->CurrentBufferIndex < PostCodeHob->BufferCount; 
    		PostCodeHob->CurrentBufferIndex++) {
        //Send checkpoint buffer one by one to IE
    	Status = SendPostCode (
                     (PEI_IPMI_TRANSPORT_PPI*)PostCodeHob->IpmiTransportPpi,
                     PostCodeHob->CheckpointBuffer[PostCodeHob->CurrentBufferIndex]);
    	
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
