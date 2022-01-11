//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

/** @file SmBusCommon.h
    This file contains shared PEI and DXE SMBus functions and 
    data structures definitions

*/
#ifndef __SMBUS_COMMON__H__
#define __SMBUS_COMMON__H__
#ifdef __cplusplus
extern "C" {
#endif

#include <Efi.h>
#include <IndustryStandard/SmBus.h>
#include <Library/IoLib.h>

#define SMBUS_ADDRESS_ARP           0x61 // SMBus Device Default Address

#define SMBUS_DATA_PREPARE_TO_ARP   0x01
#define SMBUS_DATA_RESET_DEVICE     0x02
#define SMBUS_DATA_GET_UDID_GENERAL 0x03
#define SMBUS_DATA_ASSIGN_ADDRESS   0x04

#define SMBUS_SPEC_RESERVED_ADDRESS 21
#define GET_UDID_BUFFER_SIZE        17

#define SMBUS_LOWEST_AVAILABLE_ADDRESS  0x08
#define SMBUS_HIGHEST_AVAILABLE_ADDRESS 0x77

typedef VOID (* SMBUS_WAIT ) (
    IN UINTN             Microseconds
);

extern EFI_GUID SmBusIdentifierGuid;
extern UINT8 SmBusPlatformReservedAddress[];
extern UINT8 SmBusPlatformReservedAddressSize;

/**
    AMI SMBUS driver private data structure

 Fields: Name                 Type                    Description
 SmBusBase                    UINT16                  SMBUS device base I/O address
 SmBusWait                    SMBUS_WAIT              Pointer to Wait function
 MaxDevices                   UINT8                   Maximum number of supported devices
 BoardReservedAddressCount    UINT8                   Number of board reserved addresses
 BoardReservedAddressList     UINT8*                  Pointer to board reserved addresses list
 ArpDeviceCount               UINT8                   Number of current devices
 ArpDeviceList                EFI_SMBUS_DEVICE_MAP*   Pointer to list of current devices
 
*/

typedef struct _SMBUS_PRIVATE
{
    UINT16               SmBusBase;
    SMBUS_WAIT           SmBusWait;
    UINT8                MaxDevices;
    UINT8                BoardReservedAddressCount;
    UINT8                *BoardReservedAddressList;
    UINT8                ArpDeviceCount;
    EFI_SMBUS_DEVICE_MAP *ArpDeviceList;

} SMBUS_PRIVATE;


//*******************************************************
//      Shared functions prototypes
//*******************************************************

VOID SmBusRead (
    IN   UINT16 SmBusBase,
    IN   UINT16 Offset,
    IN   UINTN  ByteCount,
    OUT  UINT8  *Buffer
);

VOID SmBusWrite (
    IN   UINT16 SmBusBase,
    IN   UINT16 Offset,
    IN   UINTN  ByteCount,
    IN   UINT8  *Buffer
);

EFI_STATUS Execute (
    IN     SMBUS_PRIVATE            *Context,
    IN     EFI_SMBUS_DEVICE_ADDRESS SlaveAddress,
    IN     EFI_SMBUS_DEVICE_COMMAND Command,
    IN     EFI_SMBUS_OPERATION      Operation,
    IN     BOOLEAN                  PecCheck,
    IN OUT UINTN                    *Length,
    IN OUT VOID                     *Buffer
);

EFI_STATUS CheckNotify (
    IN  SMBUS_PRIVATE            *Context,
    OUT EFI_SMBUS_DEVICE_ADDRESS *SlaveAddress,
    OUT UINTN                    *Data
);

EFI_STATUS ArpDevice (
    IN     SMBUS_PRIVATE            *Context,
    IN     BOOLEAN                  ArpAll,
    IN     EFI_SMBUS_UDID           *SmbusUdid, OPTIONAL
    IN OUT EFI_SMBUS_DEVICE_ADDRESS *SlaveAddress OPTIONAL
);

EFI_STATUS GetArpMap (
    IN     SMBUS_PRIVATE        *Context,
    IN OUT UINTN                *Length,
    IN OUT EFI_SMBUS_DEVICE_MAP **SmbusDeviceMap
);

EFI_STATUS PrepareToArp (
    IN SMBUS_PRIVATE *Context
);

EFI_STATUS ArpDeviceDirected (
    IN     SMBUS_PRIVATE            *Context,
    IN     EFI_SMBUS_UDID           *SmbusUdid,
    IN OUT EFI_SMBUS_DEVICE_ADDRESS *SlaveAddress
);

EFI_STATUS ArpDeviceFull (
    IN SMBUS_PRIVATE *Context
);

EFI_STATUS AssignAddress (
    IN SMBUS_PRIVATE        *Context,
    IN EFI_SMBUS_DEVICE_MAP *DeviceMap
);

EFI_STATUS GetUdidGeneral (
    IN  SMBUS_PRIVATE        *Context,
    OUT EFI_SMBUS_DEVICE_MAP *DeviceMap
);

BOOLEAN IsAddressAvailable (
    IN SMBUS_PRIVATE *Context,
    IN UINT8         Address
);

UINT8 GetAvailableAddress (
    IN SMBUS_PRIVATE *Context
);

VOID ConvertMapToBuffer (
    IN EFI_SMBUS_DEVICE_MAP *DeviceMap,
    OUT UINT8               *Buffer
);

VOID ConvertBufferToMap (
    OUT EFI_SMBUS_DEVICE_MAP *DeviceMap,
    IN UINT8                 *Buffer
);

/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
