//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//
/*++

Copyright (c)  1999 - 2012 Intel Corporation. All rights reserved
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

  MediaDeviceDriver.h

Abstract:

  Media Device Driver header

--*/
#ifndef _MEDIA_DEVICE_DRIVER_H
#define _MEDIA_DEVICE_DRIVER_H

#include <Uefi.h>

#include <Library/UefiDriverEntryPoint.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiLib.h>
#include <Library/DevicePathLib.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/TimerLib.h>

#include <Ppi/Sdhc.h>
#include <Ppi/PeiBlockIo.h>

#include <IndustryStandard/MMC.h>
#include <IndustryStandard/SDCard.h>

//
// Driver Consumed Protocol Prototypes
//
#include <Protocol/DevicePath.h>

//
// Driver Produced Protocol Prototypes
//
//#include <Ppi/BlockIo.h>
#include <Ppi/BlockIoPei.h>  //Override

extern UINT32  mSdBaseAddress;
#define CID_MANUFACTUER_ID_DEFAULT 0xFF
#define CID_MANUFACTUER_ID_SAMSUNG 0x15
#define CID_MANUFACTUER_ID_SANDISK 0x45
#define CID_MANUFACTUER_ID_TOSHIBA 0x11

extern UINT32  gMediaDeviceDebugLevel;


#define PEI_CARD_DATA_SIGNATURE  SIGNATURE_32 ('p', 'c', 'r', 'd')
#define PEI_CARD_PARTITION_SIGNATURE  SIGNATURE_32 ('p', 'c', 'a', 'r')
#define MAX_NUMBER_OF_PARTITIONS EMMC_MAX_NUMBER_OF_PARTITIONS

#define PEI_CARD_PARTITION_DATA_FROM_THIS(a) \
    CR(a, PEI_MMC_PARTITION_DATA, BlockIo, PEI_CARD_PARTITION_SIGNATURE)

#define PEI_CARD_DATA_FROM_THIS(a) \
    ((PEI_CARD_PARTITION_DATA_FROM_THIS(a))->PeiCardData)

#define PEI_CARD_DATA_PARTITION_NUM(p) \
    ((((UINTN) p) - ((UINTN) &(p->PeiCardData->Partitions))) / sizeof (*p))

    //
    // Used to check-call local functions inside this eMMC module
    //
    // Func - The function name which needs to be checked
    // Str   - Optional, it is the description of the output infomation
    // FailOrNot - When functional call is failed, it should stop or continue
    //
#define CHK_FUNC_CALL(Func, Str, FailOrNot) \
        do { \
            EFI_STATUS ret = Func; \
            if (EFI_SUCCESS!= ret && TRUE == FailOrNot) { \
                DEBUG((EFI_D_ERROR, "ERROR: %a,%d:", __FUNCTION__,__LINE__)); \
                if ( NULL != (void *)Str  ) {\
                    DEBUG((EFI_D_ERROR, "%a:", Str)); \
                } \
                DEBUG((EFI_D_ERROR, " Status = 0x%x\n", ret)); \
                return ret; \
            } \
        } while (0)


//
// Command timeout will be max 100 ms
//
#define  TIMEOUT_COMMAND     500
#define  TIMEOUT_DATA        5000

typedef enum {
  UnknownCard = 0,
  MMCCard,                // MMC card
  CEATACard,              // CE-ATA device
  SDMemoryCard,           // SD 1.1 card
  SDMemoryCard2,          // SD 2.0 or above standard card
  SDMemoryCard2High       // SD 2.0 or above high capacity card
} CARD_TYPE;


//typedef struct _CARD_DATA CARD_DATA;
typedef struct _PEI_CARD_DATA PEI_CARD_DATA;

typedef struct {
  //
  //BlockIO
  //
  UINT32                    Signature;
  EFI_HANDLE                Handle;
  BOOLEAN                   Present;
  EFI_DEVICE_PATH_PROTOCOL  *DevPath;
  EFI_PEI_RECOVERY_BLOCK_IO_PPI   BlockIo;
  EFI_PEI_PPI_DESCRIPTOR          BlkIoPpiList;
  PEI_BLOCK_IO_MEDIA        BlockIoMedia;
  PEI_CARD_DATA             *PeiCardData;
} PEI_MMC_PARTITION_DATA;

struct _PEI_CARD_DATA {
  //
  //BlockIO
  //
  UINT32                    Signature;
  EFI_HANDLE                Handle;
  PEI_MMC_PARTITION_DATA    Partitions[MAX_NUMBER_OF_PARTITIONS];
  EFI_PEI_PPI_DESCRIPTOR    PpiList;
  PEI_SD_CONTROLLER_PPI     *SdControllerPpi;
  EFI_PEI_PCI_CFG2_PPI      *PciCfgPpi;
  CARD_TYPE                 CardType;
  UINT8                     CurrentBusWidth;
  BOOLEAN                   DualVoltage;
  BOOLEAN                   NeedFlush;
  UINT8                     Reserved[3];
  UINT16                    Address;
  UINT32                    BlockLen;
  UINT32                    MaxFrequency;
  UINT64                    BlockNumber;
  //
  //Common used
  //
  CARD_STATUS               CardStatus;
  OCR                       OCRRegister;
  CID                       CIDRegister;
  CSD                       CSDRegister;
  EXT_CSD                   ExtCSDRegister;
  UINT8                     *RawBufferPointer;
  UINT8                     *AlignedBuffer;

  //
  //SD specific
  //
  SCR                       SCRRegister;
  SD_STATUS_REG             SDSattus;
  SWITCH_STATUS             SwitchStatus;
};


//
//
//

EFI_STATUS
EFIAPI
MediaDeviceDriverEntryPoint (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  );


EFI_STATUS
EFIAPI
BotGetNumberOfBlockDevices (
  IN  EFI_PEI_SERVICES                         **PeiServices,
  IN  EFI_PEI_RECOVERY_BLOCK_IO_PPI            *This,
  OUT UINTN                                    *NumberBlockDevices
  );

EFI_STATUS
EFIAPI
BotGetMediaInfo (
  IN  EFI_PEI_SERVICES                          **PeiServices,
  IN  EFI_PEI_RECOVERY_BLOCK_IO_PPI             *This,
  IN  UINTN                                     DeviceIndex,
  OUT EFI_PEI_BLOCK_IO_MEDIA                    *MediaInfo
  );

EFI_STATUS
EFIAPI
MMCSDBlockReadBlocks (
  IN  EFI_PEI_SERVICES                          **PeiServices,
  IN   EFI_PEI_RECOVERY_BLOCK_IO_PPI        *This,
  IN   UINT32                  MediaId,
  IN   EFI_LBA                 LBA,
  IN   UINTN                   BufferSize,
  OUT  VOID                    *Buffer
  );

EFI_STATUS
EFIAPI
MMCSDBlockWriteBlocks (
  IN  EFI_PEI_SERVICES                                **PeiServices,
  IN   EFI_PEI_RECOVERY_BLOCK_IO_PPI                  *This,
  IN   UINTN                                          MediaId,
  IN   EFI_PEI_LBA                                    LBA,
  IN   UINTN                                          BufferSize,
  IN   VOID                                           *Buffer
  );

EFI_STATUS
MMCSDCardInit (
  IN  PEI_CARD_DATA              *PeiCardData
  );


EFI_STATUS
MMCSDBlockIoInit (
  IN  PEI_CARD_DATA    *PeiCardData
  );

EFI_STATUS
SendCommand (
  IN  PEI_SD_CONTROLLER_PPI       *This,
  IN   UINT16                     CommandIndex,
  IN   UINT32                     Argument,
  IN   TRANSFER_TYPE              DataType,
  IN   UINT8                      *Buffer, OPTIONAL
  IN   UINT32                     BufferSize,
  IN   RESPONSE_TYPE              ResponseType,
  IN   UINT32                     TimeOut,
  OUT  UINT32                     *ResponseData
  );


EFI_STATUS
FastIO (
  IN  PEI_CARD_DATA    *PeiCardData,
  IN      UINT8       RegisterAddress,
  IN  OUT UINT8       *RegisterData,
  IN      BOOLEAN     Write
  );



EFI_STATUS
IndentifyDevice (
  IN  PEI_CARD_DATA              *PeiCardData
  );

EFI_STATUS
FlushCache (
  IN  PEI_CARD_DATA    *PeiCardData
  );

EFI_STATUS
StandByImmediate (
  IN  PEI_CARD_DATA           *PeiCardData
  );

EFI_STATUS
ReadDMAExt (
  IN  PEI_CARD_DATA    *PeiCardData,
  IN  EFI_LBA     LBA,
  IN  UINT8       *Buffer,
  IN  UINT16      SectorCount
  );

EFI_STATUS
WriteDMAExt (
  IN  PEI_CARD_DATA    *PeiCardData,
  IN  EFI_LBA     LBA,
  IN  UINT8       *Buffer,
  IN  UINT16      SectorCount
  );


EFI_STATUS
SoftwareReset (
  IN  PEI_CARD_DATA              *PeiCardData
  );

EFI_STATUS
SendAppCommand (
  IN  PEI_CARD_DATA              *PeiCardData,
  IN   UINT16                     CommandIndex,
  IN   UINT32                     Argument,
  IN   TRANSFER_TYPE              DataType,
  IN   UINT8                      *Buffer, OPTIONAL
  IN   UINT32                     BufferSize,
  IN   RESPONSE_TYPE              ResponseType,
  IN   UINT32                     TimeOut,
  OUT  UINT32                     *ResponseData
  );

UINT32
MmcGetExtCsd8 (
  IN  PEI_CARD_DATA    *PeiCardData,
  IN  UINTN            Offset
  );

UINT32
MmcGetExtCsd24 (
  IN  PEI_CARD_DATA    *PeiCardData,
  IN  UINTN            Offset
  );

UINT32
MmcGetExtCsd32 (
  IN  PEI_CARD_DATA    *PeiCardData,
  IN  UINTN            Offset
  );

UINTN
MmcGetCurrentPartitionNum (
  IN  PEI_CARD_DATA    *PeiCardData
  );

EFI_STATUS
MmcSelectPartitionNum (
  IN  PEI_CARD_DATA    *PeiCardData,
  IN  UINT8            Partition
  );

EFI_STATUS
MmcSelectPartition (
  IN  PEI_MMC_PARTITION_DATA     *Partition
  );

EFI_STATUS
MmcReadExtCsd (
  IN  PEI_CARD_DATA    *PeiCardData
  );

EFI_STATUS
MmcMoveToTranState (
  IN  PEI_CARD_DATA    *PeiCardData
  );

EFI_STATUS
MmcSelect (
  IN  PEI_CARD_DATA    *PeiCardData,
  IN  BOOLEAN          Select
  );

EFI_STATUS
MmcSendSwitch (
  IN  PEI_CARD_DATA        *PeiCardData,
  IN  SWITCH_ARGUMENT      *SwitchArgument
  );

EFI_STATUS
MmcGoIdleState (
  IN  PEI_CARD_DATA    *PeiCardData
  );

EFI_STATUS
MmcSetPartition (
  IN  PEI_CARD_DATA          *CardData,
  IN  UINT8                  Value
  );

#endif


