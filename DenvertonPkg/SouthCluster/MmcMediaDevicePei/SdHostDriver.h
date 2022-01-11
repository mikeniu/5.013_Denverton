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

#include <Library/DebugLib.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiLib.h>
#include <Library/DevicePathLib.h>
#include <Library/IoLib.h>
#include <Library/PciLib.h>
#include <Library/PcdLib.h>
#include <Library/TimerLib.h>

#include <IndustryStandard/Pci.h>
#include <IndustryStandard/SDCard.h>
#include <SaCommonDefinitions.h>
#include <Ppi/Sdhc.h>


#define PEI_SDHOST_DATA_SIGNATURE  SIGNATURE_32 ('p', 's', 'd', 'h')

#define PEI_SDHOST_DATA_FROM_THIS(a)  \
    CR(a, PEI_SDHOST_DATA, SdControllerPpi, PEI_SDHOST_DATA_SIGNATURE)

#define BLOCK_SIZE   0x200
#define TIME_OUT_1S  1000


#define EMMC_DEV_NUMBER                                28
#define INTEL_VENDOR_ID                                0x8086
#define POULSBO_FUNC_SDIO1                             0x811C
#define POULSBO_FUNC_SDIO2                             0x811D
#define POULSBO_FUNC_SDIO3                             0x811E
#define R_SCH_LPC_REV_ID                               0x08
#define   V_SCH_LPC_REV_ID_A0                          0x00
#define   V_SCH_LPC_REV_ID_A1                          0x00
#define   V_SCH_LPC_REV_ID_A2                          0x01
#define   V_SCH_LPC_REV_ID_B0                          0x02
#define   V_SCH_LPC_REV_ID_B1                          0x03
#define   V_SCH_LPC_REV_ID_C0                          0x04
#define   V_SCH_LPC_REV_ID_D0                          0x05
#define   V_SCH_LPC_REV_ID_D1                          0x06
#define   V_SCH_LPC_REV_ID_D2                          0x07


#define BUFFER_CTL_REGISTER 0x84


#pragma pack(1)

extern UINT32  mSdBaseAddress;

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
  PEI_SD_CONTROLLER_PPI      SdControllerPpi;
  EFI_PEI_PPI_DESCRIPTOR     PpiList;
  EFI_PEI_PCI_CFG2_PPI       *PciCfgPpi;
  UINT8                      SdControllerId;
  UINTN                      BaseAddress;
  UINT16                     PciVid;
  UINT16                     PciDid;
  BOOLEAN                    IsAutoStopCmd;
  BOOLEAN                    IsPenwell;
  BOOLEAN                    IsEmmc;
  BOOLEAN                    EnableVerboseDebug;
  UINT32                     BaseClockInMHz;
  UINT32                     CurrentClockInKHz;
  UINT32                     BlockLength;
  EFI_UNICODE_STRING_TABLE   *ControllerNameTable;
  UINT32                     ControllerVersion;
} PEI_SDHOST_DATA;



//
// Prototypes
// Driver model protocol interface
//
EFI_STATUS
EFIAPI
SdHostDriverEntryPoint (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  );




EFI_STATUS
EFIAPI
HostSendCommand (
  IN   PEI_SD_CONTROLLER_PPI      *This,
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
  IN PEI_SD_CONTROLLER_PPI       *This,
  IN UINT32                      MaxFrequencyInKHz
  );

EFI_STATUS
EFIAPI
SetBusWidth (
  IN PEI_SD_CONTROLLER_PPI       *This,
  IN UINT32                      BusWidth
  );

EFI_STATUS
EFIAPI
SetHostVoltage (
  IN PEI_SD_CONTROLLER_PPI       *This,
  IN UINT32                      Voltage
  );

EFI_STATUS
EFIAPI
SetHostSpeedMode(
  IN  PEI_SD_CONTROLLER_PPI    *This,
  IN  UINT32                   DdrMode
  );

EFI_STATUS
EFIAPI
SetHostDdrMode(
  IN  PEI_SD_CONTROLLER_PPI    *This,
  IN  UINT32                   DdrMode
  );

EFI_STATUS
EFIAPI
ResetSdHost (
  IN PEI_SD_CONTROLLER_PPI       *This,
  IN RESET_TYPE                  ResetType
  );

EFI_STATUS
EFIAPI
EnableAutoStopCmd (
  IN PEI_SD_CONTROLLER_PPI       *This,
  IN BOOLEAN                     Enable
  );

EFI_STATUS
EFIAPI
MMC_Detected (
  IN PEI_SD_CONTROLLER_PPI       *This
  );

EFI_STATUS
EFIAPI
DetectCardAndInitHost (
  IN PEI_SD_CONTROLLER_PPI       *This
  );

EFI_STATUS
EFIAPI
SetBlockLength (
  IN PEI_SD_CONTROLLER_PPI       *This,
  IN UINT32                      BlockLength
  );

EFI_STATUS
EFIAPI
SetupDevice(
  IN  PEI_SD_CONTROLLER_PPI    *This
  );

UINT8
SdHostRead8 (
  IN PEI_SDHOST_DATA             *SdHost,
  IN UINTN                       Offset
  );

UINT16
SdHostRead16 (
  IN PEI_SDHOST_DATA             *SdHost,
  IN UINTN                       Offset
  );

UINT32
SdHostRead32 (
  IN PEI_SDHOST_DATA             *SdHost,
  IN UINTN                       Offset
  );

UINT8
SdHostWrite8 (
  IN PEI_SDHOST_DATA             *SdHost,
  IN UINTN                       Offset,
  IN UINT8                       Data
  );

UINT16
SdHostWrite16 (
  IN PEI_SDHOST_DATA             *SdHost,
  IN UINTN                       Offset,
  IN UINT16                      Data
  );

UINT32
SdHostWrite32 (
  IN PEI_SDHOST_DATA             *SdHost,
  IN UINTN                       Offset,
  IN UINT32                      Data
  );


#endif
