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

/** @file PostCodeCommon.h
    Common macros, structure and GUID used by IeReportPostCode modules 
    drivers
**/

#ifndef __POSTCODE_COMMON_H__
#define __POSTCODE_COMMON_H__

#include <Library/HobLib.h>
#include <Ppi/IpmiIeTransportPpi.h>

//Maximum Post Code can be given in POST
#define MAXIMUM_POST_CODE 255

#pragma pack(1)
//Structure Definitions
typedef struct {
    EFI_HOB_GUID_TYPE       EfiHobGuidType;
    //EFI_PHYSICAL_ADDRESS is  used instead of PEI_IPMI_TRANSPORT_PPI *
    //since size of Pointer used in in PEI and DXE phase differs, 
    //getting the HOB in DXE phase corrupts other variables
    EFI_PHYSICAL_ADDRESS    IpmiTransportPpi;  
    UINT8                   BufferCount;
    UINT8                   CurrentBufferIndex;
    UINT8                   CheckpointBuffer[MAXIMUM_POST_CODE];
} POST_CODE_INFO_HOB;
#pragma pack()

extern EFI_GUID gAmiPostCodeInfoHobGuid;

#endif

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
