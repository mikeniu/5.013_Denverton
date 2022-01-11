//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

/** @file SmBusPorting.c
    SMBUS driver porting functions

*/

#include "Library/SmBusCommon.h"
#include <Sb.h>

//Porting Required - Put unique GUID for given SMBus controller
#define SM_BUS_CONTROLLER_IDENTIFIER_GUID \
    { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 }

EFI_GUID SmBusIdentifierGuid = SM_BUS_CONTROLLER_IDENTIFIER_GUID;

UINT8 SmBusPlatformReservedAddress[] = {
//Porting required - put platform hardcoded SMBus addresses here
    0x00
};

UINT8 SmBusPlatformReservedAddressSize = \
                        sizeof(SmBusPlatformReservedAddress) / sizeof(UINT8); 

/**
    This function sends commands via SMBUS interface

    @param Context SMBUS device private data
    @param SlaveAddress slave address value
    @param Command command
    @param Operation operation
    @param PecCheck parity check flag
    @param Length pointer to size of data buffer
    @param Buffer pointer to data buffer

    @retval EFI_STATUS

    @note  Porting required

**/
EFI_STATUS Execute (
    IN     SMBUS_PRIVATE            *Context,
    IN     EFI_SMBUS_DEVICE_ADDRESS SlaveAddress,
    IN     EFI_SMBUS_DEVICE_COMMAND Command,
    IN     EFI_SMBUS_OPERATION      Operation,
    IN     BOOLEAN                  PecCheck,
    IN OUT UINTN                    *Length,
    IN OUT VOID                     *Buffer
)
{
//Porting required - implement internal SMBus protocols here
    UINT16               SmBusIoAddr16;
    
    if (Context == NULL) {
      SmBusIoAddr16 = MmioRead16 ((UINTN) SMBUS_PCIE_CFG_ADDRESS(R_BASE_ADDRESS)) & 0xFFF0;
    } else {
      SmBusIoAddr16 = Context->SmBusBase;
    }

    return EFI_UNSUPPORTED;
}

/**
    This function checks if SMBUS host received any notifications

    @param Context SMBUS device private data
    @param SlaveAddress pointer to return address of notification device
    @param Data pointer to notification data

    @retval EFI_STATUS

    @note  Porting required

**/
EFI_STATUS CheckNotify (
    IN  SMBUS_PRIVATE            *Context,
    OUT EFI_SMBUS_DEVICE_ADDRESS *SlaveAddress,
    OUT UINTN                    *Data
)
{
//Porting required
    return EFI_UNSUPPORTED;
}

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
