//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

//<AMI_FHDR_START>
//****************************************************************************
// Name:        SdMmcTemplate.c
//
// Description: This file is template for chipset specific programming
//
//****************************************************************************
//<AMI_FHDR_END>

#include "SdioDriver.h"
#include "SdioController.h"

/**
    This function issues tuning command to card.
    This function is chipset specific porting, Care must be taken
    by porting engineer as this function is called twice from MmcCard.c
	for both HS200 and HS400 tuning. 

    @param  SdioDevInterface 
    @param  Port

    @retval EFI_STATUS

**/
EFI_STATUS 
SdMmcTunning (
    IN  SDIO_DEVICE_INFO        *SdioDevInfo,
    IN  UINT8                   Slot
)
{
    return EFI_SUCCESS;
}


/**
    This function programs the hardware specific initialization for IO card.
    @param  SdioDevInterface 
    @param  Port

    @retval EFI_STATUS

**/
EFI_STATUS 
IoCardEnableHook (
    IN  SDIO_DEVICE_INFO *SdioDevInfo,
    IN  UINT8                   Slot
){
    return EFI_SUCCESS;
}
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
