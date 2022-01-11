/*++
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
--*/
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

  MMCSDBlockIo.c

Abstract:

   Block I/O PPI for MMC/SD device

--*/

#include <MediaDeviceDriverPei.h>


EFI_PEI_RECOVERY_BLOCK_IO_PPI    mRecoveryBlkIoPpi = {
  BotGetNumberOfBlockDevices,
  BotGetMediaInfo,
  MMCSDBlockReadBlocks,
  MMCSDBlockWriteBlocks
};


EFI_PEI_PPI_DESCRIPTOR           mPpiList = {
  EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
  &gEfiPeiVirtualBlockIoPpiGuid,
  NULL
};


/**
  Gets the count of block I/O devices that one specific block driver detects.

  This function is used for getting the count of block I/O devices that one 
  specific block driver detects.  To the PEI ATAPI driver, it returns the number
  of all the detected ATAPI devices it detects during the enumeration process. 
  To the PEI legacy floppy driver, it returns the number of all the legacy 
  devices it finds during its enumeration process. If no device is detected, 
  then the function will return zero.  
  
  @param[in]  PeiServices          General-purpose services that are available 
                                   to every PEIM.
  @param[in]  This                 Indicates the EFI_PEI_RECOVERY_BLOCK_IO_PPI 
                                   instance.
  @param[out] NumberBlockDevices   The number of block I/O devices discovered.

  @retval     EFI_SUCCESS          Operation performed successfully.

**/
EFI_STATUS
EFIAPI
BotGetNumberOfBlockDevices (
  IN  EFI_PEI_SERVICES                         **PeiServices,
  IN  EFI_PEI_RECOVERY_BLOCK_IO_PPI            *This,
  OUT UINTN                                    *NumberBlockDevices
  )
{
  //
  // For eMMC devices, this value should be always 1
  //
  *NumberBlockDevices = 1;
  return EFI_SUCCESS;
}

/**
  Gets a block device's media information.

  This function will provide the caller with the specified block device's media 
  information. If the media changes, calling this function will update the media 
  information accordingly.

  @param[in]  PeiServices   General-purpose services that are available to every
                            PEIM
  @param[in]  This          Indicates the EFI_PEI_RECOVERY_BLOCK_IO_PPI instance.
  @param[in]  DeviceIndex   Specifies the block device to which the function wants 
                            to talk. Because the driver that implements Block I/O 
                            PPIs will manage multiple block devices, the PPIs that 
                            want to talk to a single device must specify the 
                            device index that was assigned during the enumeration
                            process. This index is a number from one to 
                            NumberBlockDevices.
  @param[out] MediaInfo     The media information of the specified block media.  
                            The caller is responsible for the ownership of this 
                            data structure.
  
  @retval EFI_SUCCESS        Media information about the specified block device 
                             was obtained successfully.
  @retval EFI_DEVICE_ERROR   Cannot get the media information due to a hardware 
                             error.

**/
EFI_STATUS
EFIAPI
BotGetMediaInfo (
  IN  EFI_PEI_SERVICES                          **PeiServices,
  IN  EFI_PEI_RECOVERY_BLOCK_IO_PPI             *This,
  IN  UINTN                                     DeviceIndex,
  OUT EFI_PEI_BLOCK_IO_MEDIA                    *MediaInfo
  )
{


  PEI_MMC_PARTITION_DATA      *Partition;
  DEBUG((EFI_D_INFO, "eMMC --> BotGetMediaInfo , this =0x%x\n", This));

  Partition = PEI_CARD_PARTITION_DATA_FROM_THIS (This);
  DEBUG((EFI_D_INFO, "eMMC --> BotGetMediaInfo , Partition =0x%x\n", Partition));


  MediaInfo->DeviceType = 4; //EmmcCard;
  MediaInfo->MediaPresent = Partition->BlockIoMedia.MediaPresent;
  MediaInfo->BlockSize = Partition->BlockIoMedia.BlockSize;
  MediaInfo->LastBlock = (UINTN)Partition->BlockIoMedia.LastBlock;
  DEBUG((EFI_D_INFO, "eMMC MediaPResent--> 0x%x\n", MediaInfo->MediaPresent));
  DEBUG((EFI_D_INFO, "eMMC BlockSize--> 0x%x\n", MediaInfo->BlockSize));
  DEBUG((EFI_D_INFO, "eMMC LastBlock--> 0x%x\n", MediaInfo->LastBlock));
  return EFI_SUCCESS;
}


/*++

  Routine Description:
    Implements EFI_BLOCK_IO_PROTOCOL.ReadBlocks() function.

  Arguments:
    This     The EFI_BLOCK_IO_PROTOCOL instance.
    MediaId  The media id that the read request is for.
    LBA      The starting logical block address to read from on the device.
    BufferSize
              The size of the Buffer in bytes. This must be a multiple of
              the intrinsic block size of the device.
    Buffer    A pointer to the destination buffer for the data. The caller
              is responsible for either having implicit or explicit ownership
              of the buffer.

  Returns:
    EFI_INVALID_PARAMETER - Parameter is error
    EFI_SUCCESS           - Success
    EFI_DEVICE_ERROR      - Hardware Error
    EFI_NO_MEDIA          - No media
    EFI_MEDIA_CHANGED     - Media Change
    EFI_BAD_BUFFER_SIZE   - Buffer size is bad
 --*/
EFI_STATUS
EFIAPI
MMCSDBlockReadBlocks (
  IN  EFI_PEI_SERVICES                    **PeiServices,
  IN   EFI_PEI_RECOVERY_BLOCK_IO_PPI      *This,
  IN   UINTN                              MediaId,
  IN   EFI_PEI_LBA                        LBA,
  IN   UINTN                              BufferSize,
  OUT  VOID                               *Buffer
  )
{
  EFI_STATUS                  Status;
  UINT32                      Address;
  PEI_CARD_DATA               *PeiCardData;
  PEI_SD_CONTROLLER_PPI       * SdControllerPpi;
  UINT32                      RemainingLength;
  UINT32                      TransferLength;
  UINT8                       *BufferPointer;
  BOOLEAN                     SectorAddressing;
  UINT64                      CardSize;
  PEI_MMC_PARTITION_DATA      *Partition;

  Status   = EFI_SUCCESS;
  Partition = PEI_CARD_PARTITION_DATA_FROM_THIS (This);
  PeiCardData  = Partition->PeiCardData;
  SdControllerPpi = PeiCardData->SdControllerPpi;

  //DEBUG((EFI_D_INFO, "eMMC --> MMCSDBlockReadBlocks , Partition =0x%x, card->partitons=0x%x cardData=0x%x\n", Partition, PeiCardData, PeiCardData->Partitions));

  DEBUG((EFI_D_INFO,
    "MMCSDBlockReadBlocks: Read(PART=%d, LBA=0x%08lx, Buffer=0x%08x, Size=0x%08x)\n",
    PEI_CARD_DATA_PARTITION_NUM (Partition), LBA, Buffer, BufferSize
    ));


  //DEBUG((EFI_D_INFO, "MMCSDBlockReadBlocks , BlockNumber=0x%x, BlockLen->partitons=0x%x, BlockSize=0x%x \n", PeiCardData->BlockNumber, PeiCardData->BlockLen, Partition->BlockIoMedia.BlockSize));

  Status = MmcSelectPartition (Partition);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  CardSize = MultU64x32 (PeiCardData->BlockNumber, PeiCardData->BlockLen);

  if ((PeiCardData->CardType == SDMemoryCard2High) || (CardSize >= SIZE_2GB)) {
    SectorAddressing = TRUE;
  } else {
    SectorAddressing = FALSE;
  }

  if (SectorAddressing) {
    //
    // Sector Address
    //
    Address = (UINT32)DivU64x32 (MultU64x32 (LBA, Partition->BlockIoMedia.BlockSize), 512);
  } else {
    //
    //Byte Address
    //
    Address  = (UINT32)MultU64x32 (LBA, Partition->BlockIoMedia.BlockSize);
  }


  if (!Buffer) {
    Status = EFI_INVALID_PARAMETER;
    DEBUG ((EFI_D_ERROR, "MMCSDBlockReadBlocks: Invalid parameter \n"));
    goto Done;
  }

  if ((BufferSize % Partition->BlockIoMedia.BlockSize) != 0) {
    Status = EFI_BAD_BUFFER_SIZE;
    DEBUG ((EFI_D_ERROR, "MMCSDBlockReadBlocks: Bad buffer size \n"));
    goto Done;
  }

  if (BufferSize == 0) {
    Status = EFI_SUCCESS;
    goto Done;
  }

  BufferPointer   = Buffer;
  RemainingLength = (UINT32)BufferSize;

  while (RemainingLength > 0) {
    DEBUG((EFI_D_INFO,
      "MMCSDBlockReadBlocks: BufferPointer=0x%08x, RemainingLength=0x%08x)\n",
      BufferPointer, RemainingLength
      ));
    if ((BufferSize >= Partition->BlockIoMedia.BlockSize) &&
        FeaturePcdGet (PcdMmcSdMultiBlockSupport)) {

      if (RemainingLength > SdControllerPpi->HostCapability.BoundarySize) {
        TransferLength = SdControllerPpi->HostCapability.BoundarySize;
      } else {
        TransferLength = RemainingLength;
      }

      if (PeiCardData->CardType == MMCCard) {
        if (!(PeiCardData->ExtCSDRegister.CARD_TYPE & (BIT2 | BIT3))) {
        Status = SendCommand (
                   SdControllerPpi,
                   SET_BLOCKLEN,
                   Partition->BlockIoMedia.BlockSize,
                   NoData,
                   NULL,
                   0,
                   ResponseR1,
                   TIMEOUT_COMMAND,
                   (UINT32*)&(PeiCardData->CardStatus)
                   );

        if (EFI_ERROR (Status)) {
          break;
            }
        }
        Status = SendCommand (
                   SdControllerPpi,
                   SET_BLOCK_COUNT,
                   TransferLength / Partition->BlockIoMedia.BlockSize,
                   NoData,
                   NULL,
                   0,
                   ResponseR1,
                   TIMEOUT_COMMAND,
                   (UINT32*)&(PeiCardData->CardStatus)
                   );
        if (EFI_ERROR (Status)) {
          break;
        }
      }
  if ((TransferLength> 0x200) && (SdControllerPpi->HostCapability.ADMA2Support))  {
//       SdHostIo->HostCapability.DmaMode=ADMA2; 
    }

      Status = SendCommand (
                 SdControllerPpi,
                 READ_MULTIPLE_BLOCK,
                 Address,
                 InData,
                 PeiCardData->AlignedBuffer,
                 TransferLength,
                 ResponseR1,
                 TIMEOUT_DATA,
                 (UINT32*)&(PeiCardData->CardStatus)
                 );

      if (EFI_ERROR (Status)) {
        DEBUG ((EFI_D_ERROR, "MMCSDBlockReadBlocks: READ_MULTIPLE_BLOCK -> Fail\n"));
        break;
      }
    } else {
      if (RemainingLength > Partition->BlockIoMedia.BlockSize) {
        TransferLength = Partition->BlockIoMedia.BlockSize;
      } else {
        TransferLength = RemainingLength;
      }

      Status = SendCommand (
                 SdControllerPpi,                 
                 READ_SINGLE_BLOCK,
                 Address,
                 InData,
                 PeiCardData->AlignedBuffer,
                 (UINT32)TransferLength,
                 ResponseR1,
                 TIMEOUT_DATA,
                 (UINT32*)&(PeiCardData->CardStatus)
                 );
      if (EFI_ERROR (Status)) {
        DEBUG ((EFI_D_ERROR, "MMCSDBlockReadBlocks: READ_SINGLE_BLOCK -> Fail\n"));
        break;
      }
    }
    DEBUG((EFI_D_INFO,
      "MMCSDBlockReadBlocks: COPY BufferPointer=0x%08x, AlignedBuffer=0x%08x TransferLength=0x%08x)\n",
      BufferPointer, PeiCardData->AlignedBuffer, TransferLength
      ));
    CopyMem (BufferPointer, PeiCardData->AlignedBuffer, TransferLength);

    if (SectorAddressing) {
      //
      // Sector Address
      //
      Address += TransferLength / 512;
    } else {
      //
      //Byte Address
      //
      Address += TransferLength;
    }
    BufferPointer   += TransferLength;
    RemainingLength -= TransferLength;
  }

  if (EFI_ERROR (Status)) {
    if ((PeiCardData->CardType == SDMemoryCard) ||
        (PeiCardData->CardType == SDMemoryCard2)||
        (PeiCardData->CardType == SDMemoryCard2High)) {
         SendCommand (
                 SdControllerPpi,           
                 STOP_TRANSMISSION,
                 0,
                 NoData,
                 NULL,
                 0,
                 ResponseR1b,
                 TIMEOUT_COMMAND,
                 (UINT32*)&(PeiCardData->CardStatus)
                 );
    } else {
       SendCommand (
                 SdControllerPpi,         
                 STOP_TRANSMISSION,
                 0,
                 NoData,
                 NULL,
                 0,
                 ResponseR1,
                 TIMEOUT_COMMAND,
                 (UINT32*)&(PeiCardData->CardStatus)
                 );
    }

  }

Done:
  return Status;
}

/*++

  Routine Description:
    Implements EFI_BLOCK_IO_PROTOCOL.WriteBlocks() function.

  Arguments:
    This     The EFI_BLOCK_IO_PROTOCOL instance.
    MediaId  The media id that the write request is for.
    LBA      The starting logical block address to write to on the device.
    BufferSize
              The size of the Buffer in bytes. This must be a multiple of
              the intrinsic block size of the device.
    Buffer    A pointer to the source buffer for the data. The caller
              is responsible for either having implicit or explicit ownership
              of the buffer.

  Returns:
    EFI_INVALID_PARAMETER - Parameter is error
    EFI_SUCCESS           - Success
    EFI_DEVICE_ERROR      - Hardware Error
    EFI_NO_MEDIA          - No media
    EFI_MEDIA_CHANGED     - Media Change
    EFI_BAD_BUFFER_SIZE   - Buffer size is bad
 --*/
EFI_STATUS
EFIAPI
MMCSDBlockWriteBlocks (
  IN  EFI_PEI_SERVICES                                **PeiServices,
  IN   EFI_PEI_RECOVERY_BLOCK_IO_PPI                  *This,
  IN   UINTN                                          MediaId,
  IN   EFI_PEI_LBA                                    LBA,
  IN   UINTN                                          BufferSize,
  IN   VOID                                           *Buffer
  )
{
  EFI_STATUS                  Status;
  UINT32                      Address;
  PEI_CARD_DATA               *PeiCardData;
  PEI_SD_CONTROLLER_PPI       *SdControllerPpi;
  UINT32                      RemainingLength;
  UINT32                      TransferLength;
  UINT8                       *BufferPointer;
  BOOLEAN                     SectorAddressing;
  UINT64                      CardSize;
  PEI_MMC_PARTITION_DATA      *Partition;

  Status   = EFI_SUCCESS;
  Partition = PEI_CARD_PARTITION_DATA_FROM_THIS (This);
  PeiCardData  = Partition->PeiCardData;
  SdControllerPpi = PeiCardData->SdControllerPpi;

  Status = MmcSelectPartition (Partition);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  CardSize = MultU64x32 (PeiCardData->BlockNumber, PeiCardData->BlockLen);

  if ((PeiCardData->CardType == SDMemoryCard2High) || (CardSize >= SIZE_2GB)) {
    SectorAddressing = TRUE;
  } else {
    SectorAddressing = FALSE;
  }

  if (SectorAddressing) {
    //
    // Sector Address
    //
    Address = (UINT32)DivU64x32 (MultU64x32 (LBA, Partition->BlockIoMedia.BlockSize), 512);
  } else {
    //
    //Byte Address
    //
    Address  = (UINT32)MultU64x32 (LBA, Partition->BlockIoMedia.BlockSize);
  }

  if (!Buffer) {
    Status = EFI_INVALID_PARAMETER;
    DEBUG ((EFI_D_ERROR, "MMCSDBlockWriteBlocks: Invalid parameter \n"));
    goto Done;
  }
  
  if (BufferSize == 0) {
    Status = EFI_SUCCESS;
    goto Done;
  }

  BufferPointer   = Buffer;
  RemainingLength = (UINT32)BufferSize;

  while (RemainingLength > 0) {

    if ((BufferSize >= Partition->BlockIoMedia.BlockSize) &&
        FeaturePcdGet (PcdMmcSdMultiBlockSupport)) {

      if (RemainingLength > SdControllerPpi->HostCapability.BoundarySize) {
        TransferLength = SdControllerPpi->HostCapability.BoundarySize;
      } else {
        TransferLength = RemainingLength;
      }

      if (PeiCardData->CardType == MMCCard) {
        if (!(PeiCardData->ExtCSDRegister.CARD_TYPE & (BIT2 | BIT3))) {
          Status = SendCommand (
                     SdControllerPpi,
                     SET_BLOCKLEN,
                     Partition->BlockIoMedia.BlockSize,
                     NoData,
                     NULL,
                     0,
                     ResponseR1,
                     TIMEOUT_COMMAND,
                     (UINT32*)&(PeiCardData->CardStatus)
                     );

          if (EFI_ERROR (Status)) {
            break;
          }
        }
        Status = SendCommand (
                   SdControllerPpi,
                   SET_BLOCK_COUNT,
                   TransferLength / Partition->BlockIoMedia.BlockSize,
                   NoData,
                   NULL,
                   0,
                   ResponseR1,
                   TIMEOUT_COMMAND,
                   (UINT32*)&(PeiCardData->CardStatus)
                   );
        if (EFI_ERROR (Status)) {
          break;
        }
      }

      CopyMem (PeiCardData->AlignedBuffer, BufferPointer, TransferLength);

      Status = SendCommand (
                 SdControllerPpi,
                 WRITE_MULTIPLE_BLOCK,
                 Address,
                 OutData,
                 PeiCardData->AlignedBuffer,
                 (UINT32)TransferLength,
                 ResponseR1,
                 TIMEOUT_DATA,
                 (UINT32*)&(PeiCardData->CardStatus)
                 );

      if (EFI_ERROR (Status)) {
        DEBUG ((EFI_D_ERROR, "MMCSDBlockWriteBlocks: WRITE_MULTIPLE_BLOCK -> Fail\n"));
        break;
      }
    } else {
      if (RemainingLength > Partition->BlockIoMedia.BlockSize) {
        TransferLength = Partition->BlockIoMedia.BlockSize;
      } else {
        TransferLength = RemainingLength;
      }

      CopyMem (PeiCardData->AlignedBuffer, BufferPointer, TransferLength);

      Status = SendCommand (
                 SdControllerPpi,
                 WRITE_BLOCK,
                 Address,
                 OutData,
                 PeiCardData->AlignedBuffer,
                 (UINT32)TransferLength,
                 ResponseR1,
                 TIMEOUT_DATA,
                 (UINT32*)&(PeiCardData->CardStatus)
                 );
    }

    if (SectorAddressing) {
      //
      // Sector Address
      //
      Address += TransferLength / 512;
    } else {
      //
      //Byte Address
      //
      Address += TransferLength;
    }
    BufferPointer   += TransferLength;
    RemainingLength -= TransferLength;
  }

  if (EFI_ERROR (Status)) {
    SendCommand (
      SdControllerPpi,
      STOP_TRANSMISSION,
      0,
      NoData,
      NULL,
      0,
      ResponseR1b,
      TIMEOUT_COMMAND,
      (UINT32*)&(PeiCardData->CardStatus)
      );
  }

Done:
  return Status;
}

/*++

  Routine Description:
    MMC/SD card BlockIo init function

  Arguments:
    CardData  -   Pointer to CARD_DATA

  Returns:
    EFI_SUCCESS - Success
--*/
EFI_STATUS
MMCSDBlockIoInit (
  IN  PEI_CARD_DATA    *PeiCardData
  )
{
  UINTN                   Loop;
  PEI_MMC_PARTITION_DATA  *Partition;
  EXT_CSD                 *ExtCsd;
  UINT64                  GP_CHUNK_SIZE;
  UINT32                  GP_SIZE_MULT;
  UINT64                  GppSize;
  UINTN                   GppIndex=0;

  Partition = PeiCardData->Partitions;

  ExtCsd = &PeiCardData->ExtCSDRegister;

  //
  // Determine GP partitioning chunk size
  //
  GP_CHUNK_SIZE = 0;
  if (((ExtCsd->PARTITIONING_SUPPORT & BIT0) == BIT0) &&
      ((ExtCsd->PARTITION_SETTING_COMPLETED & BIT0) == BIT0)) {
    GP_CHUNK_SIZE = MultU64x32 (ExtCsd->HC_WP_GRP_SIZE * ExtCsd->HC_ERASE_GRP_SIZE, SIZE_512KB);
  }

  for (Loop = 0; Loop < MAX_NUMBER_OF_PARTITIONS; Partition++, Loop++) {
    //
    //BlockIO protocol
    //
    Partition->BlockIo.GetNumberOfBlockDevices = BotGetNumberOfBlockDevices;
    Partition->BlockIo.GetBlockDeviceMediaInfo = BotGetMediaInfo;

    Partition->BlockIo.ReadBlocks  = MMCSDBlockReadBlocks ;


    Partition->BlockIoMedia.MediaId          = 0;
    Partition->BlockIoMedia.RemovableMedia   = FALSE;
    Partition->BlockIoMedia.MediaPresent     = TRUE;
    Partition->BlockIoMedia.LogicalPartition = FALSE;

    //
    //  Set the CardData
    //
    Partition->PeiCardData = PeiCardData;
    //
    // Force the User partition to be enabled
    //
    if (Loop == 0) {
      Partition->Present = TRUE;
    }

    if (PeiCardData->CSDRegister.PERM_WRITE_PROTECT || PeiCardData->CSDRegister.TMP_WRITE_PROTECT) {
      Partition->BlockIoMedia.ReadOnly         = TRUE;
    } else {
      Partition->BlockIoMedia.ReadOnly         = FALSE;
    }

    Partition->BlockIoMedia.WriteCaching     = FALSE;
    Partition->BlockIoMedia.BlockSize        = PeiCardData->BlockLen;
    Partition->BlockIoMedia.IoAlign          = 1;
    Partition->BlockIoMedia.LastBlock        = (EFI_LBA)(PeiCardData->BlockNumber - 1);


    //
    // Handle GPP partitions
    //
    GppSize = 0;
    if ((GP_CHUNK_SIZE != 0) && (Loop >= 4)) {
      Partition->BlockIoMedia.LastBlock = (EFI_LBA) 0;
      GppIndex = Loop - 4;
      GP_SIZE_MULT = MmcGetExtCsd24 (
                       PeiCardData,
                       OFFSET_OF (EXT_CSD, GP_SIZE_MULT_1) + (3 * GppIndex)
                       );
      GppSize = MultU64x32 (GP_SIZE_MULT, (UINT32)GP_CHUNK_SIZE);
    }

    if (GppSize != 0) {
      Partition->BlockIoMedia.LastBlock =
      DivU64x32 (GppSize, Partition->BlockIoMedia.BlockSize) - 1;
      DEBUG ((EFI_D_INFO,
        "GPP%d last-block: 0x%lx\n",
        GppIndex + 1,
        Partition->BlockIoMedia.LastBlock
        ));
      Partition->Present = TRUE;
    }

    //
    // Handle Boot partitions
    //
    if ((Loop == 1) || (Loop == 2)) {
        Partition->BlockIoMedia.LastBlock = 128 * 1024 * ((UINTN) MmcGetExtCsd8(PeiCardData, OFFSET_OF (EXT_CSD, BOOT_SIZE_MULTI))) / 512;
        Partition->Present = TRUE;
    }

    DEBUG ((EFI_D_INFO, "MMCSDBlockIoInit: Initialized\n"));
    
    CopyMem (
      &(PeiCardData->Partitions[Loop].BlockIo),
      &mRecoveryBlkIoPpi,
      sizeof (EFI_PEI_RECOVERY_BLOCK_IO_PPI)
      );
    CopyMem (
      &(PeiCardData->Partitions[Loop].BlkIoPpiList),
      &mPpiList,
      sizeof (EFI_PEI_PPI_DESCRIPTOR)
      );
    PeiCardData->Partitions[Loop].BlkIoPpiList.Ppi  = &PeiCardData->Partitions[Loop].BlockIo;
  }

  return EFI_SUCCESS;
}



