//
// This file contains an 'Intel Peripheral Driver' and is      
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may 
// be modified by the user, subject to additional terms of the 
// license agreement                                           
//
/*++

Copyright (c)  1999 - 2015 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

--*/


/*++
Module Name:

  SdHostDriver.h

Abstract:

  Header file for driver.

--*/

#ifndef _SD_HOST_DRIVER_H
#define _SD_HOST_DRIVER_H

#include <Uefi.h>

#include <Library/UefiDriverEntryPoint.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiLib.h>
#include <Library/DevicePathLib.h>
#include <Library/IoLib.h>

#include <IndustryStandard/Pci.h>
#include <IndustryStandard/SdCard.h>

//
// Driver Consumed Protocol Prototypes
//
#include <Protocol/DevicePath.h>
#include <Protocol/PciIo.h>

//
// Driver Produced Protocol Prototypes
//
#include <Protocol/DriverBinding.h>
#include <Protocol/ComponentName.h>
#include <Protocol/ComponentName2.h>
#include <Protocol/SdHostIo.h>

extern EFI_COMPONENT_NAME_PROTOCOL  gSdHostComponentName;
extern EFI_DRIVER_BINDING_PROTOCOL  gSdHostDriverBinding;

#define SDHOST_DATA_SIGNATURE  SIGNATURE_32 ('s', 'd', 'h', 's')

#define SDHOST_DATA_FROM_THIS(a) \
    CR(a, SDHOST_DATA, SdHostIo, SDHOST_DATA_SIGNATURE)

#define BLOCK_SIZE   0x200
#define TIME_OUT_1S  1000


#define INTEL_VENDOR_ID             0x8086
#define POULSBO_FUNC_SDIO1          0x811C
#define POULSBO_FUNC_SDIO2          0x811D
#define POULSBO_FUNC_SDIO3          0x811E

#define EMMC_DEVICE_VENDOR_ID       0x19DB8086
#define EMMC_DEVICE_VENDOR_ID_LGY   0x0F508086
 
#define BUFFER_CTL_REGISTER 0x84


#pragma pack(1)
//
// PCI Class Code structure
//
typedef struct {
  UINT8 PI;
  UINT8 SubClassCode;
  UINT8 BaseCode;
} PCI_CLASSC;

#pragma pack()


typedef struct {
  UINTN                      Signature;
  EFI_SD_HOST_IO_PROTOCOL    SdHostIo;
  EFI_PCI_IO_PROTOCOL        *PciIo;
  UINT16                     PciVid;
  UINT16                     PciDid;
  BOOLEAN                    IsAutoStopCmd;
  BOOLEAN                    IsEmmc;
  BOOLEAN                    EnableVerboseDebug;
  UINT32                     BaseClockInMHz;
  UINT32                     CurrentClockInKHz; 
  UINT32                     BlockLength;
  EFI_UNICODE_STRING_TABLE   *ControllerNameTable;
  UINT32                     ControllerVersion;
} SDHOST_DATA;

//
// Prototypes
// Driver model protocol interface
//
EFI_STATUS
EFIAPI
SdHostDriverEntryPoint (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  );


EFI_STATUS
EFIAPI
SdHostDriverBindingSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL *This,
  IN EFI_HANDLE                  Controller,
  IN EFI_DEVICE_PATH_PROTOCOL    *RemainingDevicePath
  );


EFI_STATUS
EFIAPI
SdHostDriverBindingStart (
  IN EFI_DRIVER_BINDING_PROTOCOL     *This,
  IN EFI_HANDLE                      Controller,
  IN EFI_DEVICE_PATH_PROTOCOL        *RemainingDevicePath
  );

EFI_STATUS
EFIAPI
SdHostDriverBindingStop (
  IN  EFI_DRIVER_BINDING_PROTOCOL     *This,
  IN  EFI_HANDLE                      Controller,
  IN  UINTN                           NumberOfChildren,
  IN  EFI_HANDLE                      *ChildHandleBuffer
  );

//
// EFI Component Name Functions
//
EFI_STATUS
EFIAPI
SdHostComponentNameGetDriverName (
  IN  EFI_COMPONENT_NAME_PROTOCOL     *This,
  IN  CHAR8                           *Language,
  OUT CHAR16                          **DriverName
  );

EFI_STATUS
EFIAPI
SdHostComponentNameGetControllerName (
  IN  EFI_COMPONENT_NAME_PROTOCOL     *This,
  IN  EFI_HANDLE                      ControllerHandle,
  IN  EFI_HANDLE                      ChildHandle, OPTIONAL
  IN  CHAR8                           *Language,
  OUT CHAR16                          **ControllerName
  );


EFI_STATUS
EFIAPI
SendCommand (
  IN   EFI_SD_HOST_IO_PROTOCOL    *This,
  IN   UINT16                     CommandIndex,
  IN   UINT32                     Argument,
  IN   TRANSFER_TYPE              DataType,
  IN   UINT8                      *Buffer, OPTIONAL
  IN   UINT32                     BufferSize,    
  IN   RESPONSE_TYPE              ResponseType,
  IN   UINT32                     TimeOut,
  OUT  UINT32                     *ResponseData OPTIONAL
  );

EFI_STATUS
EFIAPI
SetClockFrequency (
  IN  EFI_SD_HOST_IO_PROTOCOL    *This,
  IN  UINT32                     MaxFrequencyInKHz          
  );

EFI_STATUS
EFIAPI
SetBusWidth (
  IN  EFI_SD_HOST_IO_PROTOCOL    *This,
  IN  UINT32                     BusWidth          
  );

EFI_STATUS
EFIAPI
SetHostVoltage (
  IN  EFI_SD_HOST_IO_PROTOCOL    *This,
  IN  UINT32                     Voltage                       
  );

EFI_STATUS
EFIAPI
SetHostDdrMode (
  IN  EFI_SD_HOST_IO_PROTOCOL    *This,
  IN UINT32                      DdrMode
);

/**
  Set Host SDR Mode

  @param[in] This                 Pointer to EFI_SD_HOST_IO_PROTOCOL
  @param[in] DdrMode              True for SDR Mode set, false for normal mode

  @retval EFI_SUCCESS             The function completed successfully 
  @retval EFI_INVALID_PARAMETER   A parameter was incorrect.
**/
EFI_STATUS
EFIAPI
SetHostSdrMode (
  IN  EFI_SD_HOST_IO_PROTOCOL    *This,
  IN UINT32                      SdrMode
);

/**
  Set Host High Speed

  @param[in] This                 Pointer to EFI_SD_HOST_IO_PROTOCOL
  @param[in] HighSpeed            True for High Speed Mode set, false for normal mode

  @retval EFI_SUCCESS             The function completed successfully 
  @retval EFI_INVALID_PARAMETER   A parameter was incorrect.
**/
EFI_STATUS
EFIAPI
SetHostSpeedMode(
  IN  EFI_SD_HOST_IO_PROTOCOL    *This,
  IN  UINT32                     HighSpeed                       
  );

EFI_STATUS
EFIAPI
ResetSdHost (
  IN  EFI_SD_HOST_IO_PROTOCOL    *This,
  IN  RESET_TYPE                 ResetType   
  );

EFI_STATUS
EFIAPI
EnableAutoStopCmd (
  IN  EFI_SD_HOST_IO_PROTOCOL    *This,
  IN  BOOLEAN                    Enable   
  );

EFI_STATUS
EFIAPI
DetectCardAndInitHost (
  IN  EFI_SD_HOST_IO_PROTOCOL    *This
  );

EFI_STATUS
EFIAPI
SetBlockLength (
  IN  EFI_SD_HOST_IO_PROTOCOL    *This,
  IN  UINT32                     BlockLength   
  );
EFI_STATUS 
EFIAPI    
SetupDevice(
             IN  EFI_SD_HOST_IO_PROTOCOL    *This
            );

UINT8
SdHostRead8 (
  IN     SDHOST_DATA                  *SdHost,
  IN     UINTN                        Offset
  );

UINT16
SdHostRead16 (
  IN     SDHOST_DATA                  *SdHost,
  IN     UINTN                        Offset
  );

UINT32
SdHostRead32 (
  IN     SDHOST_DATA                  *SdHost,
  IN     UINTN                        Offset
  );

UINT8
SdHostWrite8 (
  IN     SDHOST_DATA                  *SdHost,
  IN     UINTN                        Offset,
  IN     UINT8                        Data
  );

UINT16
SdHostWrite16 (
  IN     SDHOST_DATA                  *SdHost,
  IN     UINTN                        Offset,
  IN     UINT16                       Data
  );

UINT32
SdHostWrite32 (
  IN     SDHOST_DATA                  *SdHost,
  IN     UINTN                        Offset,
  IN     UINT32                       Data
  );

#endif
