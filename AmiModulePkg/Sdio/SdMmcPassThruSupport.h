//***********************************************************************
//***********************************************************************
//**                                                                   **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.          **
//**                                                                   **
//**                       All Rights Reserved.                        **
//**                                                                   **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093         **
//**                                                                   **
//**                       Phone: (770)-246-8600                       **
//**                                                                   **
//***********************************************************************
//***********************************************************************


/** @file SdMMcPassThruSupport.h
    Header file for SdMmcPassthru Driver

**/
#ifndef _SD_MMC_PASS_THRU_SUPPORT_H_
#define _SD_MMC_PASS_THRU_SUPPORT_H_

#ifdef __cplusplus
extern "C" {
#endif
#include <Token.h>
#if defined ( MDE_PKG_VERSION ) && (MDE_PKG_VERSION > 12) 
//---------------------------------------------------------------------------
#include <AmiDxeLib.h>
#include <Efi.h>
#include <SdioDriver.h>
#include <Protocol/SdMmcPassThru.h>
#include <Protocol/DevicePath.h>
#include <Protocol/SdioBus.h>
#include <SdioController.h>
//---------------------------------------------------------------------------


EFI_STATUS 
PassThru (
    IN EFI_SD_MMC_PASS_THRU_PROTOCOL             *This,
    IN UINT8                                     Slot,
    IN OUT EFI_SD_MMC_PASS_THRU_COMMAND_PACKET   *Packet,
    IN EFI_EVENT                                 Event 
);

EFI_STATUS 
GetNextSlot (
    IN EFI_SD_MMC_PASS_THRU_PROTOCOL       *This,
    IN OUT UINT8                           *Slot
);

EFI_STATUS
BuildDevicePath (
    IN EFI_SD_MMC_PASS_THRU_PROTOCOL     *This,
    IN UINT8                             Slot,
    IN OUT EFI_DEVICE_PATH_PROTOCOL      **DevicePath
);

EFI_STATUS 
GetSlotNumber (
    IN EFI_SD_MMC_PASS_THRU_PROTOCOL      *This,
    IN EFI_DEVICE_PATH_PROTOCOL           *DevicePath,
    OUT UINT8                             *Slot
);

EFI_STATUS
ResetDevice (
    IN EFI_SD_MMC_PASS_THRU_PROTOCOL      *This,
    IN UINT8                              Slot
);

typedef struct _SD_MMC_PASS_THRU_PROTOCOL {
    EFI_SD_MMC_PASS_THRU_PROTOCOL     EfiSdMmcPassThruProtocol;
    AMI_SDIO_BUS_PROTOCOL             *SdioBusInterface;
    UINT8                             PreviousSlot;
}SD_MMC_PASS_THRU_PROTOCOL;
#endif
#ifdef __cplusplus
}
#endif

#endif

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
//**********************************************************************

