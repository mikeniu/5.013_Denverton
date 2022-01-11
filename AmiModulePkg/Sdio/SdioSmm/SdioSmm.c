//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************


/** @file SdioSmm.c
    Smm driver to handle the SDIO MMIO operations

**/

//----------------------------------------------------------------------

#include "SdioSmm.h"
#include "SdioController.h"
#include <AmiDxeLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/AmiBufferValidationLib.h>
//----------------------------------------------------------------------

EFI_SMM_BASE2_PROTOCOL          *gSmmBase2;
EFI_SMM_SYSTEM_TABLE2           *pSmst2;

typedef VOID        (*API_FUNC)(SDIO_STRUC*);
SDIO_GLOBAL_DATA    *gSdioData;
UINT8               SDIO_Access_Mode;
BOOLEAN             gFirstAPICall = FALSE;
EFI_GUID            gAmiSdioCommunicationGuid = SDIO_PARAMETER_BASE_ADDRESS_GUID;
/**
 Type:        Function Dispatch Table

    This is the table of functions used by SDIO Storage API

**/

API_FUNC SdioApiTable[] = {
    SdMmcAPIRead,
    SdMmcAPIWrite,
    SdMmcApiDeviceGeometry,
    SdMmcApiReset,
    SdMmcNotSupported,  // Dummy
    SdMmcNotSupported,  // GetDeviceInfo
    SdMmcNotSupported   // VerifyDevice
};

/**
    Initialize Sdio SMM data area

    @param   DispatchHandle,
    @param   *Context         OPTIONAL,
    @param   *CommBuffer      OPTIONAL,
    @param   *CommBufferSize  OPTIONAL

    @retval  EFI_STATUS

**/

EFI_STATUS
EFIAPI
SdioInitSmmData (
    IN  EFI_HANDLE      DispatchHandle,
    IN  CONST   VOID    *Context        OPTIONAL,
    IN  OUT     VOID    *CommBuffer     OPTIONAL,
    IN  OUT     UINTN   *CommBufferSize OPTIONAL
  )
{

    EFI_STATUS          Status = EFI_SUCCESS;
    SDIO_GLOBAL_DATA    *OrgSdioData;
    UINT8               i;

    
    // After the first API call is invoked, don't initialize SMM data area. This is an additional
    // Security check so that data won't get corrupted.
    if (gFirstAPICall) {
        return EFI_SUCCESS;
    }
    
    // If input is invalid, stop processing this SMI
    if (CommBuffer == NULL || CommBufferSize == NULL) {
        return EFI_SUCCESS;
    }

    OrgSdioData = (SDIO_GLOBAL_DATA*)(*(UINTN *)CommBuffer);
    if(gSdioData == NULL) {

        // Copy IDENTIFY_CONTROLLER_DATA
        Status = pSmst2->SmmAllocatePool ( 
                                    EfiRuntimeServicesData, 
                                    sizeof (SDIO_GLOBAL_DATA),
                                    (VOID**)&(gSdioData));
        ASSERT_EFI_ERROR(Status);
        
        ZeroMem ((void*)gSdioData, sizeof(SDIO_GLOBAL_DATA));
        gSdioData->AdmaDescriptorAddress = OrgSdioData->AdmaDescriptorAddress;
    }
    
    for(i=0;i<MAX_SDIO_DEVICES;i++) {
        if(OrgSdioData->SdioDev[i].DevEntryUsed != FALSE) {
            if(gSdioData->SdioDev[i].DevEntryUsed == FALSE) {
                CopyMem ((SDIO_DEVICE_INFO *)&(gSdioData->SdioDev[i]), (SDIO_DEVICE_INFO *)&(OrgSdioData->SdioDev[i]), sizeof (SDIO_DEVICE_INFO));
            }
        } else {
            break;
        }
    }
            
    return EFI_SUCCESS;
    
}


/** 
    This is the entry point of the SDIO Smm driver.

    @param ImageHandle
    @param SystemTable

    @retval EFI_STATUS

**/

EFI_STATUS
EFIAPI
SdioSmmDriverEntryPoint (
    IN  EFI_HANDLE       ImageHandle,
    IN  EFI_SYSTEM_TABLE *SystemTable
)
{
    EFI_STATUS                      Status;
    EFI_HANDLE                      SwHandle = NULL;
    EFI_SMM_SW_DISPATCH2_PROTOCOL  *pSwDispatch = NULL;
    EFI_SMM_SW_REGISTER_CONTEXT     SwContext;
    EFI_HANDLE                      DispatchHandle =NULL;

    InitAmiSmmLib( ImageHandle, SystemTable );
    
    Status = pBS->LocateProtocol(&gEfiSmmBase2ProtocolGuid, NULL, (VOID **) &gSmmBase2);
    if (EFI_ERROR(Status)) return EFI_SUCCESS;

    Status = gSmmBase2->GetSmstLocation (gSmmBase2, &pSmst2);
    if (EFI_ERROR(Status)) return EFI_SUCCESS;

    Status = pSmst2->SmmLocateProtocol(&gEfiSmmSwDispatch2ProtocolGuid,
                                       NULL,
                                       &pSwDispatch);
    if (EFI_ERROR(Status)) return EFI_SUCCESS;


    SwContext.SwSmiInputValue = SDIO_SWSMI;
    Status = pSwDispatch->Register (pSwDispatch,
                                    SdioSWSMIHandler,
                                    &SwContext,
                                    &SwHandle);
    if (EFI_ERROR (Status)) return Status;

    //
    // Register SDIO handler to transfer data from DXE driver to SMM
    //
    Status = pSmst2->SmiHandlerRegister (
                      SdioInitSmmData,
                      &gAmiSdioCommunicationGuid,
                      &DispatchHandle
                      );
    
    ASSERT_EFI_ERROR (Status);

    return EFI_SUCCESS;
}

/**
    Invoked on reads from SW SMI port with value SDIO_SWSMI. This
    function dispatches the SDIO Request Packets (URP) to the
    appropriate functions.

    @param  DispatchHandle  - EFI Handle
    @param  DispatchContext - Pointer to the EFI_SMM_SW_DISPATCH_CONTEXT
    @param  CommBuffer
    @param  CommBufferSize

    @retval  Zero on successful completion
    @retval  Non zero on error

**/

EFI_STATUS
EFIAPI
SdioSWSMIHandler (
    EFI_HANDLE      DispatchHandle,
    IN  CONST VOID  *DispatchContext OPTIONAL,
    IN  OUT VOID    *CommBuffer      OPTIONAL,
    IN  OUT UINTN   *CommBufferSize  OPTIONAL
)
{
    SDIO_STRUC  *SdioURP=NULL;
    UINT8        bFuncIndex;
    UINT8        bNumberOfFunctions;
    UINT16       EbdaSeg;
    EFI_STATUS   Status;
    gFirstAPICall = TRUE;

    //
    // Get the fpURP pointer from EBDA
    //
    EbdaSeg = *((UINT16*)0x40E);
    SdioURP = *(SDIO_STRUC**)(UINTN)(((UINT32)EbdaSeg << 4) + SDIO_DATA_EBDA_OFFSET);
    SdioURP = (SDIO_STRUC*)((UINTN)SdioURP & 0xFFFFFFFF);

    // Validate if URP address is an non-SMRAM region to avoid SMRAM data 
    // corruption through SMI handlers
    Status = AmiValidateMemoryBuffer((VOID*)SdioURP, sizeof(SDIO_STRUC));

    if (EFI_ERROR(Status)) {
        // return without modifying the SdioeUrp buffer as it's not belongs to the normal Memory Region.
        return EFI_SUCCESS;
    }
    
    bFuncIndex = SdioURP->bFuncNumber;
    bNumberOfFunctions = sizeof SdioApiTable / sizeof (API_FUNC *);

    //
    // Make sure function number is valid; if function number is not zero
    // check for valid extended SDIO API function
    //
    if (bFuncIndex >= bNumberOfFunctions ) {
        SdioURP->bRetValue = SDIO_PARAMETER_FAILED;
        return EFI_SUCCESS;
    }

    //
    // Call the appropriate function
    //

    SdioApiTable[bFuncIndex](SdioURP);

    return EFI_SUCCESS;
}
/**
    Get the Index no for the DeviceAddress

    @param  DeviceAddress

    @retval DeviceIndex

**/
UINT8
GetDevEntry (
    IN  UINT8   DeviceAddress
)
{
    UINT8   i;

    for(i=0;i<MAX_SDIO_DEVICES;i++) {
        if(gSdioData->SdioDev[i].DeviceAddress == DeviceAddress) {
            return i;
        }
    }

    return 0xFF;

}

/**
    Read the Data from the Device

    @param Input details from caller

    @retval Output information stored in SDIO_STRUC

**/
VOID
SdMmcAPIRead (
    SDIO_STRUC *SdioURP
)
{
    SDIO_DEVICE_INFO  *SdioDevInfo;
    EFI_STATUS        Status;
    UINT8             Port;
    UINT32            Lba;
    UINT32            NumBlks;
    UINT32            TempNumBlks = MAX_NUMBLOCK_16BIT;
    UINT32            BufferAddress;
    UINT8             DevEntry;

    DevEntry=GetDevEntry(SdioURP->ApiData.Read.DeviceAddress);
    if(DevEntry == 0xFF) {
        SdioURP->bRetValue = SDIO_PARAMETER_FAILED;
        return;
    }
    SdioDevInfo=&gSdioData->SdioDev[DevEntry];

    // Validate if Buffer address is an non-SMRAM region to avoid SMRAM data 
    // corruption through SMI handlers
    Status = AmiValidateMemoryBuffer((VOID*)SdioURP->ApiData.Read.BufferAddress, 
                                     SdioURP->ApiData.Read.NumBlks * SdioDevInfo->wBlockSize);

    if (EFI_ERROR(Status)) {
        SdioURP->bRetValue = SDIO_PARAMETER_FAILED;
        return;
    }

    
    Port=SdioURP->ApiData.Read.Port;
    Lba=SdioURP->ApiData.Read.LBA;
    NumBlks=SdioURP->ApiData.Read.NumBlks;
    BufferAddress=SdioURP->ApiData.Read.BufferAddress;

    //
    // If "Forced FDD" option is selected that means the device has
    // to be emulated as a floppy drive even though it has a HDD emulated
    // image.  This is accomplished by hiding the first cylinder totally.
    // The partition table is in the first cylinder.  LBA value for all
    // the requests to the device will be offset with the number of sectors
    // in the cylinder.
    //

    //
    // Check for forced floppy emulated device and change LBA accordingly
    //
    if (SdioDevInfo->bEmuType == SDIO_EMU_FORCED_FDD) {
        //
        // Skip first track in case of floppy emulation
        //
        Lba += SdioDevInfo->bHiddenSectors;
    }

    //
    // If number of block are greater than 65535(16-bit) then divide the read into multiple read.
    //
    while(NumBlks > TempNumBlks) {
        Status = SdMmcRead(SdioDevInfo,Port,(EFI_LBA)Lba, TempNumBlks,(VOID*)BufferAddress);
        if(EFI_ERROR(Status)) {
            SdioURP->bRetValue = SDIO_GENERAL_FAILURE;
            return;
        }
        NumBlks = NumBlks - TempNumBlks;
        Lba += TempNumBlks;
        BufferAddress +=((SdioDevInfo->wBlockSize) * TempNumBlks);
    }
    Status = SdMmcRead(SdioDevInfo,Port,(EFI_LBA)Lba, NumBlks,(VOID*)BufferAddress);

    if(EFI_ERROR(Status)) {
        SdioURP->bRetValue = SDIO_GENERAL_FAILURE;
    } else {
        //
        // Check for forced floppy emulated device
        //
        if ((SdioDevInfo->bEmuType == SDIO_EMU_FORCED_FDD) &&
            (SdioURP->ApiData.Read.LBA == 0)) {
            //
            // This is a floppy emulated ZIP drive, with read to
            // first sector. Update the boot record so that floppy
            // emulation is okay.
            //
            // Force #of hidden sectors to 0
            //
            *(UINT32*)((UINTN)(BufferAddress + 0xB + 0x11)) = 0;

            //
            // FreeDOS workaround
            //
            if ((*(UINT32*)((UINTN)BufferAddress + 3) == 0x65657246) && // 'eerF'
                (*(UINT32*)((UINTN)BufferAddress + 7) == 0x20534F44) && // ' SOD'
                (*(UINT32*)((UINTN)BufferAddress + 0x3A) == 0x20202032)) {
                    *(UINT16*)((UINTN)BufferAddress + 0x42) =
                        *(UINT16*)((UINTN)BufferAddress + 0x42)-(UINT16)SdioDevInfo->bHiddenSectors;
                    *(UINT16*)((UINTN)BufferAddress + 0x46) =
                        *(UINT16*)((UINTN)BufferAddress + 0x46)-(UINT16)SdioDevInfo->bHiddenSectors;
                    *(UINT16*)((UINTN)BufferAddress + 0x4A) =
                        *(UINT16*)((UINTN)BufferAddress + 0x4A)-(UINT16)SdioDevInfo->bHiddenSectors;
            }
            //
            // Force physical drive# to 0
            // For FAT32, physical drive number is present in offset 40h
            //
            if ((*(UINT32*)((UINTN)BufferAddress + 0x52)) == 0x33544146) { // '3TAF' for FAT3
                *(UINT8*)((UINTN)BufferAddress + 0x40) = 0;
            } else {
                *(UINT8*)((UINTN)BufferAddress + 0x24) = 0;
            }
        }

        SdioURP->bRetValue = SDIO_SUCCESS;
    }

    return;
}

/**
    Write the Data to the Device

    @param Input details from caller

    @retval Output information stored in SDIO_STRUC

**/
VOID
SdMmcAPIWrite (
    SDIO_STRUC *SdioURP
)
{
    SDIO_DEVICE_INFO  *SdioDevInfo;
    EFI_STATUS        Status;
    UINT8             Port;
    UINT32            Lba;
    UINT32            NumBlks;
    UINT32            TempNumBlks = MAX_NUMBLOCK_16BIT;
    UINT32            BufferAddress;
    UINT8             DevEntry;
    #if SDIO_WriteProtect
    UINT32            Temp32;
    #endif

    DevEntry=GetDevEntry(SdioURP->ApiData.Read.DeviceAddress);
    if(DevEntry == 0xFF) {
        SdioURP->bRetValue = SDIO_PARAMETER_FAILED;
        return;
    }

    SdioDevInfo=&gSdioData->SdioDev[DevEntry];


    // Validate if Buffer address is an non-SMRAM region to avoid SMRAM data 
    // corruption through SMI handlers
    Status = AmiValidateMemoryBuffer((VOID*)SdioURP->ApiData.Read.BufferAddress, 
                                     SdioURP->ApiData.Read.NumBlks * SdioDevInfo->wBlockSize);

    if (EFI_ERROR(Status)) {
        SdioURP->bRetValue = SDIO_PARAMETER_FAILED;
        return;
    }
    
    Port=SdioURP->ApiData.Read.Port;
    Lba=SdioURP->ApiData.Read.LBA;
    NumBlks=SdioURP->ApiData.Read.NumBlks;
    BufferAddress=SdioURP->ApiData.Read.BufferAddress;

    #if SDIO_WriteProtect        
    Temp32=SDIO_REG32(SdioDevInfo->SdioBaseAddress,PSTATE);
    if(!(Temp32 & PSTATE_WP)) {
        SdioURP->bRetValue = SDIO_WRITE_PROTECT_ERR;
        return;
    }
    #endif

    //
    // If "Forced FDD" option is selected that means the device has
    // to be emulated as a floppy drive even though it has a HDD emulated
    // image.  This is accomplished by hiding the first cylinder totally.
    // The partition table is in the first cylinder.  LBA value for all
    // the requests to the device will be offset with the number of sectors
    // in the cylinder.
    //

    //
    // Check for forced floppy emulated device and change LBA accordingly
    //
    if (SdioDevInfo->bEmuType == SDIO_EMU_FORCED_FDD) {
        //
        // Skip first track in case of floppy emulation
        //
        Lba += SdioDevInfo->bHiddenSectors;
    }

    //
    // If number of block are greater than 65535(16-bit) then divide the write into multiple write.
    //
    while(NumBlks > TempNumBlks) {
        Status = SdMmcWrite(SdioDevInfo,Port,(EFI_LBA)Lba, (UINT32)TempNumBlks,(VOID*)BufferAddress);
        if(EFI_ERROR(Status)) {
            SdioURP->bRetValue = SDIO_GENERAL_FAILURE;
            return;
        }
        NumBlks = NumBlks - TempNumBlks;
        Lba += TempNumBlks;
        BufferAddress +=((SdioDevInfo->wBlockSize) * TempNumBlks);
    }
    Status = SdMmcWrite(SdioDevInfo,Port,(EFI_LBA)Lba, (UINT32)NumBlks,(VOID*)BufferAddress);
    
    if(EFI_ERROR(Status)) {
        SdioURP->bRetValue = SDIO_GENERAL_FAILURE;
    } else {
        SdioURP->bRetValue = SDIO_SUCCESS;
    }

    return;
}

/**
    Get the Device Geometry

        
    @param ImageHandle 
    @param SystemTable 

    @retval EFI_STATUS

**/
VOID SdMmcApiDeviceGeometry (
    SDIO_STRUC *SdioURP
)
{

    SDIO_DEVICE_INFO    *SdioDevInfo;
    UINT8               DevEntry;

    DevEntry=GetDevEntry(SdioURP->ApiData.Read.DeviceAddress);
    if(DevEntry == 0xFF) {
        SdioURP->bRetValue = SDIO_PARAMETER_FAILED;
        return;
    }
    SdioDevInfo=&gSdioData->SdioDev[DevEntry];

    SdioURP->ApiData.DeviceGeo.NumHeads=SdioDevInfo->NumHeads;
    SdioURP->ApiData.DeviceGeo.NumCylinders=SdioDevInfo->NumCylinders;
    SdioURP->ApiData.DeviceGeo.NumSectors=SdioDevInfo->NumSectors;
    SdioURP->ApiData.DeviceGeo.LBANumHeads=SdioDevInfo->LBANumHeads;
    SdioURP->ApiData.DeviceGeo.LBANumCyls=SdioDevInfo->LBANumCyls;
    SdioURP->ApiData.DeviceGeo.LBANumSectors=SdioDevInfo->LBANumSectors;
    SdioURP->ApiData.DeviceGeo.BlockSize=SdioDevInfo->wBlockSize;
    SdioURP->ApiData.DeviceGeo.MaxLBA=SdioDevInfo->dMaxLBA;

    SdioURP->bRetValue = SDIO_SUCCESS;

    return;

}

/**
    Reset the Device

        
    @param ImageHandle 
    @param SystemTable 

    @retval EFI_STATUS

**/
VOID
SdMmcApiReset (
    SDIO_STRUC *SdioURP
)
{

    SDIO_DEVICE_INFO    *SdioDevInfo;
    EFI_STATUS          Status;
    UINT8               DevEntry;

    DevEntry=GetDevEntry(SdioURP->ApiData.Read.DeviceAddress);
    if(DevEntry == 0xFF) {
        SdioURP->bRetValue = SDIO_PARAMETER_FAILED;
        return;
    }

    SdioDevInfo=&gSdioData->SdioDev[DevEntry];

    Status=SdMmcResetCard(SdioDevInfo,SdioDevInfo->SlotNumber);

    if(EFI_ERROR(Status)) {
        SdioURP->bRetValue = SDIO_GENERAL_FAILURE;
    } else {
        SdioURP->bRetValue = SDIO_SUCCESS;
    }

    return;

}


/**
    SdMMc SMI functions not supported and return EFI_SUCCESS

    @retval VOID
**/

VOID SdMmcNotSupported (
    SDIO_STRUC *SdioURP
)
{
    SdioURP->bRetValue = SDIO_SUCCESS;
    return;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
