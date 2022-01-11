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

/** @file BootSectorWriteProtect.h
    Header File for the Boot Sector Write Protect Driver

**/

#ifndef _BOOT_SECTOR_WRITE_PROTECT_H__
#define _BOOT_SECTOR_WRITE_PROTECT_H__

//---------------------------------------------------------------------------

#include <Protocol/BlockIo.h>
#include <Protocol/AmiBlockIoWriteProtection.h>

//---------------------------------------------------------------------------

#define CSM16_I13_HDD_MBR_WP_BIT        0x20
#define EFI_GPT_HEADER_ID  "EFI PART"


#pragma pack(1)

typedef struct {
    EFI_TABLE_HEADER    Header;
    EFI_LBA             MyLba;
    EFI_LBA             AlternateLba;
    EFI_LBA             FirstUsableLba;
    EFI_LBA             LastUsableLba;
    EFI_GUID            DiskGuid;
    EFI_LBA             PartitionEntryLba;
    UINT32              NumberOfPartitionEntries;
    UINT32              SizeOfPartitionEntry;
    UINT32              Crc32;
} GPT_HEADER;

typedef struct {
    EFI_GUID    PartitionTypeGuid;
    EFI_GUID    UniquePartitionGuid;
    EFI_LBA     StartingLba;
    EFI_LBA     EndingLba;
    UINT64      Attributes;
    CHAR16      PartitionName[36];
} PARTITION_ENTRY;

typedef struct _MBR_PARTITION {
    UINT8       BootIndicator;
    UINT8       StartHead;
    UINT8       StartSector;
    UINT8       StartTrack;
    UINT8       OSType;
    UINT8       EndHead;
    UINT8       EndSector;
    UINT8       EndTrack;
    UINT32      StartingLba;
    UINT32      SizeInLba;
} MBR_PARTITION;

typedef struct _MASTER_BOOT_RECORD {
    UINT8                   BootCode[440];
    UINT32                  UniqueMbrSig;
    UINT16                  Unknown;
    MBR_PARTITION           PartRec[4];
    UINT16                  Sig;
} MASTER_BOOT_RECORD;

#pragma pack()


typedef struct {
    DLINK       Link;
    UINTN       BlockIoAdd;
    UINT64      FirstUsableLBA;
    UINT64      LastUsableLBA;
} GPT_PARTITION_INFO;


EFI_STATUS
EFIAPI
BootSectorWriteProtectEntryPoint (
    IN  EFI_HANDLE        ImageHandle,
    IN  EFI_SYSTEM_TABLE  *SystemTable
);

UINTN
EfiStrLen (
    IN  CHAR16  *String
);

EFI_STATUS
EFIAPI
BlockIoWriteProtectionCheck (
    IN  AMI_BLOCKIO_WRITE_PROTECTION_PROTOCOL   *This,
    IN  EFI_BLOCK_IO_PROTOCOL                   *BlockIo,
    IN  EFI_LBA                                 LBA,
    IN  UINTN                                   BufferSize
);

EFI_STATUS
FindBootSectorWriteProtectLocation (
    IN  OUT UINTN*  BootSectorWriteProtectLocation
);

VOID 
EFIAPI
LegacyBootSectorWriteProtectEnable (
    IN  EFI_EVENT    Event,
    IN  VOID         *Context
);

VOID 
EFIAPI
FindGptInBlockIO (
    IN  EFI_EVENT    Event,
    IN  VOID         *Context
);

VOID 
AdjustBufferSize (
    IN  EFI_BLOCK_IO_PROTOCOL   *BlockIo,
    IN  OUT UINTN               *BufferSize
);

EFI_STATUS
ValidateGptHeader (
    IN  EFI_BLOCK_IO_PROTOCOL   *BlockIo,
    IN  GPT_HEADER              *GptHeader,
    IN  EFI_LBA                 HdrLoc
);

EFI_STATUS 
VerifyPartionTables (
    IN  EFI_BLOCK_IO_PROTOCOL   *BlockIo,
    OUT GPT_HEADER              *Gpt
);

GPT_PARTITION_INFO*
CheckBlockIoInGptRecord (
    IN  EFI_BLOCK_IO_PROTOCOL   *BlockIo
);

VOID
EFIAPI
GptProtectionEnable (
    IN  EFI_EVENT    Event,
    IN  VOID         *Context
);

#endif

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
