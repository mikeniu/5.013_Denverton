//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************

/** @file SdioBus.h
**/

#ifndef _AMI_SDIO_BUS_H
#define _AMI_SDIO_BUS_H

#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------

#include <AmiDxeLib.h>
#include <SdioController.h>
#if defined ( MDE_PKG_VERSION ) && (MDE_PKG_VERSION > 12) 
#include <Protocol/EraseBlock.h>
#endif
//----------------------------------------------------------------------

#define PCI_CL_SYSTEM_PERIPHERALS           0x08
#define PCI_CL_SYSTEM_PERIPHERALS_SCL_SD    0x05

#define     MAX_SLOTS_PER_CONTROLLER        0x06

typedef    struct _SDIO_DEVICE_INTERFACE SDIO_DEVICE_INTERFACE;

typedef VOID 
(EFIAPI *EFI_SDIO_INVOKE_API) (
VOID*   fURP    
);

typedef struct{
    EFI_HANDLE                  ControllerHandle;
    UINT64                      SdioBaseAddress[MAX_SLOTS_PER_CONTROLLER];
    DLIST                       SdioDeviceList;
    EFI_DEVICE_PATH_PROTOCOL    *DevicePathProtocol;
    EFI_PCI_IO_PROTOCOL         *PciIO;
    EFI_SDIO_INVOKE_API         SdioInvokeApi;
    UINT64                      AdmaDescriptorAddress;
    UINT8                       NumberOfSlotsImplemented;
} AMI_SDIO_BUS_PROTOCOL;

typedef struct _SDIO_DISK_INFO{ 
    EFI_DISK_INFO_PROTOCOL                DiskInfo;                // should be the first Entry
    SDIO_DEVICE_INTERFACE                *SdioDevInterface;
}SDIO_DISK_INFO;

typedef struct _SDIO_BLOCK_IO{ 
    EFI_BLOCK_IO_PROTOCOL                BlkIo;                    // should be the first Entry
    SDIO_DEVICE_INTERFACE                *SdioDevInterface;
}SDIO_BLOCK_IO;
#if defined ( MDE_PKG_VERSION ) && (MDE_PKG_VERSION > 12) 
typedef struct _SDIO_BLOCK_ERASE{ 
    EFI_ERASE_BLOCK_PROTOCOL             BlkErase;                   
    SDIO_DEVICE_INTERFACE                *SdioDevInterface;
} SDIO_BLOCK_ERASE;
#endif
typedef struct _SDIO_DEVICE_INTERFACE{
    EFI_HANDLE                  SdioDeviceHandle;
    BOOLEAN                     MassStorageDevice;
    UINT8                       SlotNumber;
    UINT8                       DeviceState;
    CHAR16                      UnicodePNMString[27];
    SDIO_DEVICE_INFO            *SdioDevInfo;
    AMI_SDIO_BUS_PROTOCOL       *SdioBusInterface;
    EFI_DEVICE_PATH_PROTOCOL    *DevicePathProtocol; 
    SDIO_BLOCK_IO               *SdioBlkIo;
#if defined ( MDE_PKG_VERSION ) && (MDE_PKG_VERSION > 12) 
    SDIO_BLOCK_ERASE            *SdioBlkErase;
#endif
    SDIO_DISK_INFO              *SdioDiskInfo;
    DLINK                       SdioDeviceLink; 

};

extern EFI_GUID gSdioBusInitProtocolGuid;

/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif

#endif 

//****************************************************************************
//****************************************************************************
//**                                                                        **
//**             (C)Copyright 1985-2017, American Megatrends, Inc.          **
//**                                                                        **
//**                          All Rights Reserved.                          **
//**                                                                        **
//**                 5555 Oakbrook Parkway, Norcross, GA 30093              **
//**                                                                        **
//**                          Phone (770)-246-8600                          **
//**                                                                        **
//****************************************************************************
//****************************************************************************
