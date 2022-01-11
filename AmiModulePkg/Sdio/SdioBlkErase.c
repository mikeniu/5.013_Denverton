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

/** @file SdioBlkErase.c
    BlockErase function implemented

**/

//----------------------------------------------------------------------
#include "SdioDriver.h"
//----------------------------------------------------------------------
#if defined ( MDE_PKG_VERSION ) && (MDE_PKG_VERSION > 12) 

/**
    Erase a specified number of device blocks.


    @param  This 
    @param  MediaId 
    @param  LBA,
    @param  Token 
    @param  Size

    @retval EFI_STATUS
**/
EFI_STATUS
SdioBlockErase (
    IN  EFI_ERASE_BLOCK_PROTOCOL   *This,
    IN  UINT32                     MediaId,
    IN  EFI_LBA                    LBA,
    IN OUT EFI_ERASE_BLOCK_TOKEN  *Token,
    IN UINTN                      Size
) {
    EFI_STATUS  Status;
    UINT32      CardStatus;
    UINT8       EraseStartCommand, EraseEndCommand;
    UINT64      EndLBA;
    
    SDIO_DEVICE_INTERFACE   *SdioDevInterface = ((SDIO_BLOCK_ERASE *)This)->SdioDevInterface;
       
    //Parameter Set Commands (Erase group selection) are not allowed, while the
    //device is in Programming State.
    Status= SdMmcCardGetStatus(SdioDevInterface->SdioDevInfo,
                               SdioDevInterface->SlotNumber 
                               );
    if(EFI_ERROR(Status) && 
      (SdioDevInterface->SdioDevInfo->CurrentState == CARDSTATUS_PRG )) {
        return EFI_DEVICE_ERROR;
    }
    
    // Calculate the EndLBA, based on bytes to be erased and StartLBA
    EndLBA = LBA + ((Size / SdioDevInterface->SdioDevInfo->wBlockSize) - 1);
    
    // Validate whether the input size is EraseLengthGranularity aligned.
    if (((Size / SdioDevInterface->SdioDevInfo->wBlockSize) % This->EraseLengthGranularity) ||
         (EndLBA > SdioDevInterface->SdioDevInfo->dMaxLBA)){
        return EFI_INVALID_PARAMETER;
    }
    
    // Find out whether the device is MMC or SD card.
    // For MMC card, EraseCommand sequence is ERASE_GROUP_START, ERASE_GROUP_END, ERASE 
    // For SD card, EraseCommand sequence is ERASE_WR_BLK_START, ERASE_WR_BLK_END, ERASE 

    if ((SdioDevInterface->SdioDevInfo->bMode == MMC_HIGH_CAP ) || 
        (SdioDevInterface->SdioDevInfo->bMode == MMC_STAND_CAP )) {
		        
        EraseStartCommand = ERASE_GROUP_START;
        EraseEndCommand   = ERASE_GROUP_END;
        
        // Data address for media =<2 GB is a 32 bit byte address 
        // Data address for media > 2 GB is a 32 bit sector (512B) address.        
        if(SdioDevInterface->SdioDevInfo->bMode == MMC_STAND_CAP ) {
            LBA = MultU64x32(LBA , SdioDevInterface->SdioDevInfo->wBlockSize);
            EndLBA = MultU64x32(EndLBA , SdioDevInterface->SdioDevInfo->wBlockSize) ;
        } 
    } else {
        EraseStartCommand = ERASE_WR_BLK_START;
        EraseEndCommand   = ERASE_WR_BLK_END;
        
        //SDSC Cards use byte unit address,
        //SDHC and SDXC Cards use block unit address (512 bytes)
        if(SdioDevInterface->SdioDevInfo->bMode == SD_STAND_CAP ) {
            LBA = MultU64x32(LBA , SdioDevInterface->SdioDevInfo->wBlockSize);
            EndLBA = MultU64x32(EndLBA , SdioDevInterface->SdioDevInfo->wBlockSize) ;
        } 
    }        
    // Specify the Start address 
    Status = SdMmcCommand(SdioDevInterface->SdioDevInfo,
                         SdioDevInterface->SlotNumber,
                         (EraseStartCommand << 8) | RESP1_TYPE,
                         (UINT32)LBA
                         );
    if(EFI_ERROR(Status)) {
        return EFI_DEVICE_ERROR;
    }        
    // Specify the End address
    Status = SdMmcCommand(SdioDevInterface->SdioDevInfo,
                          SdioDevInterface->SlotNumber,
                          (EraseEndCommand << 8) | RESP1_TYPE,
                          (UINT32)EndLBA
                           );
    if(EFI_ERROR(Status)) {
        return EFI_DEVICE_ERROR;
    }
    // Erase the specified blocks
    Status = SdMmcCommand(SdioDevInterface->SdioDevInfo,
                          SdioDevInterface->SlotNumber,
                          (ERASE << 8) | RESP1_TYPE,
                          0
                          );
    if(EFI_ERROR(Status)) {
        CardStatus = SDIO_REG32(SdioDevInterface->SdioDevInfo->SdioBaseAddress,RESP0);
        if (CardStatus & WP_ERASE_SKIP) {
            // Erase range includes write protected sectors
            return EFI_WRITE_PROTECTED;  
        } else {
            return EFI_DEVICE_ERROR;
        }
    }
    return EFI_SUCCESS;
}

/**
    Initializes Sdio Erase BLock interface

    @param  SdioDevInterface

    @retval EFI_STATUS

    @note  
  Here is the control flow of this function:
  1. Initialize EFI_ERASE_BLOCK_PROTOCOL Protocol.


**/
EFI_STATUS
InitSdioBlockErase (
    IN  SDIO_DEVICE_INTERFACE            *SdioDevInterface
)
{
    EFI_STATUS                 Status;
    EFI_ERASE_BLOCK_PROTOCOL   *BlkErase;
    SDIO_BLOCK_ERASE           *SdioBlkErase;
    UINT32                     Temp32 = 0;
   
    Status = pBS->AllocatePool (EfiBootServicesData,
               sizeof(SDIO_BLOCK_ERASE),
               (VOID**)&SdioBlkErase);

    if (EFI_ERROR(Status)) {
        return Status;
    }
   
    SdioDevInterface->SdioBlkErase = SdioBlkErase;
    SdioBlkErase->SdioDevInterface = SdioDevInterface;

    BlkErase = &(SdioBlkErase->BlkErase);
    BlkErase->EraseBlocks = SdioBlockErase;
   
    BlkErase->Revision = EFI_ERASE_BLOCK_PROTOCOL_REVISION; 
   
    if ((SdioDevInterface->SdioDevInfo->bMode == MMC_HIGH_CAP ) || 
       (SdioDevInterface->SdioDevInfo->bMode == MMC_STAND_CAP )) {
	    //
	    //For MMC, the Erase Group size is defined in the CSD when ERASE_GROUP_DEF is disabled,
	    //and in the EXT_CSD when ERASE_GROUP_DEF is enabled.
	    //
	    if(SdioDevInterface->SdioDevInfo->Ext_Csd.ERASE_GROUP_DEF) {
	        //EraseUnitSize (in bytes) = 512Kbyte * HC_ERASE_GRP_SIZE
            Temp32 = 512* 1024*SdioDevInterface->SdioDevInfo->Ext_Csd.HC_ERASE_GRP_SIZE;
	        BlkErase->EraseLengthGranularity = Temp32/SdioDevInterface->SdioDevInfo->wBlockSize;
	    } else {
	        //Erasable unit (In block) = (ERASE_GRP_SIZE + 1) * (ERASE_GRP_MULT + 1)
	        Temp32 = (((SdioDevInterface->SdioDevInfo->d4CSD[1] & ERASE_GRP_SIZE) >> 10) + 1) \
		              *(((SdioDevInterface->SdioDevInfo->d4CSD[1] & ERASE_GRP_MULT) >> 5) + 1);	    
	        BlkErase->EraseLengthGranularity = Temp32;
	    }
    } else {
    	//
	    // For SDSC card,Erase unit is one or multiple units of SECTOR_SIZE, when ERASE_BLK_EN is disabled
	    // Else it will be one or multiple units of 512 bytes, when ERASE_BLK_EN is enabled.
	    //
	    if ((SdioDevInterface->SdioDevInfo->bMode == SD_STAND_CAP) &&
	        !(SdioDevInterface->SdioDevInfo->d4CSD[1] & ERASE_BLK_EN)) {
            
	        Temp32 = SdioDevInterface->SdioDevInfo->d4CSD[1] & SECTOR_SIZE;
	        BlkErase->EraseLengthGranularity = (Temp32 >> 7) + 1;	    	   
        } else {
            //
            //For, SDHC and SDXC Cards, ERASE_BLK_EN is fixed to 1
            //hence the host can erase one or multiple units of 512 bytes
            //
            BlkErase->EraseLengthGranularity = 1;   
        }
   }
   return EFI_SUCCESS;
}

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
