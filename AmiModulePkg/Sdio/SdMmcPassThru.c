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

/** @file SdMmcPassThru.c
    Contains SdMmc PassThru protocol APIs definition

**/

//---------------------------------------------------------------------------
#include  "SdMmcPassThruSupport.h"
//---------------------------------------------------------------------------
#if defined ( MDE_PKG_VERSION ) && (MDE_PKG_VERSION > 12) 
// SD card command list
UINT8 SdCommandClass0[13] = { // Class0 commands
0, 2, 3, 4, 7, 8, 9, 10, 11, 12, 13, 15, 0xFF};

UINT8 SdCommandClass1[1] = { // Class1 commands
0xFF }; 

UINT8 SdCommandClass2[7] = { // Class2 commands
16, 17, 18, 19, 20, 23, 0xFF};

UINT8 SdCommandClass3[1] = { // Class3 Commands
 0xFF }; 

UINT8 SdCommandClass4[7] = { // Class4 commands
16, 20, 23, 24, 25, 27, 0xFF};

UINT8 SdCommandClass5[4] = { // Class5 commands
32, 33, 38, 0xFF};

UINT8 SdCommandClass6[4] = { // Class6 commands
28, 29, 30, 0xFF};

UINT8 SdCommandClass7[4] = { // Class7 commands
16, 40, 42, 0xFF};

UINT8 SdCommandClass8[3] = { // Class8 commands
55, 56, 0xFF};

UINT8 SdCommandClass9[4] = { // Class9 commands
52, 53, 54, 0xFF};

UINT8 SdCommandClass10[2] = { // Class10 commands
6, 0xFF};

UINT8 SdCommandClass11[5] = { // Class11 commands   
48, 49, 58, 59, 0xFF};

// MMC command list
UINT8 MmcCommandClass0[15] = { // Class0 commands
0, 1, 2, 3, 4, 7, 8, 9, 10, 11, 12, 13, 15, 19, 0xFF};

UINT8 MmcCommandClass1[1] = { // Class1 commands
0xFF }; 

UINT8 MmcCommandClass2[5] = { // Class2 commands
16, 17, 18, 21, 0xFF};

UINT8 MmcCommandClass3[1] = { // Class3 Commands
 0xFF }; 

UINT8 MmcCommandClass4[7] = { // Class4 commands
23, 24, 25, 26, 27, 49, 0xFF};

UINT8 MmcCommandClass5[4] = { // Class5 commands
35, 36, 38, 0xFF};

UINT8 MmcCommandClass6[5] = { // Class6 commands
28, 29, 30, 31, 0xFF};

UINT8 MmcCommandClass7[2] = { // Class7 commands
42, 0xFF};

UINT8 MmcCommandClass8[3] = { // Class8 commands
55, 56, 0xFF};

UINT8 MmcCommandClass9[3] = { // Class9 commands
39, 40, 0xFF};

UINT8 MmcCommandClass10[3] = { // Class10 commands
53, 54, 0xFF};

UINT8 MmcCommandClass11[6] = { // Class11 commands   
44, 45, 46, 47, 48, 0xFF};

UINT8 SdIoCommandList[4] = {
7, 52, 53, 0xFF
};
/**
    Sends an SDMmc command to an SD device that is attached 
    to the SD controller

    @param  IN  EFI_SD_MMC_PASS_THRU_PROTOCOL *This,
    @param  IN  UINT8                       Slot,
    @param  IN  OUT                         EFI_SD_MMC_PASS_THRU_COMMAND_PACKET *Packet,
    @param  IN  EFI_EVENT                   Event OPTIONAL

    @retval EFI_STATUS

    @note   A pointer to the SDMmc command to send to the SD device specified by the Slot number.
**/
EFI_STATUS
PassThru (
    IN EFI_SD_MMC_PASS_THRU_PROTOCOL                *This,
    IN UINT8                                        Slot,
    IN OUT EFI_SD_MMC_PASS_THRU_COMMAND_PACKET      *Packet,
    IN EFI_EVENT                                    Event
)
{
    EFI_STATUS                    Status;
    SDIO_DEVICE_INTERFACE         *SdioDevInterface = NULL;
    SD_MMC_PASS_THRU_PROTOCOL     *SdMmcPassThruInstance = \
                                  (SD_MMC_PASS_THRU_PROTOCOL *)This;
    UINT32                        SdioBaseAddr = \
                                  (UINT32) SdMmcPassThruInstance->SdioBusInterface->SdioBaseAddress;
    BOOLEAN                       Read;
    UINT64                        MaxBytes;
    UINT32                        NumBlks;
    UINT16                        CommandClass;
    VOID                          *TransferBuffer;
    UINT16                        ClassIndex, Index;
    BOOLEAN                       CommandSupported = FALSE;
    UINT8                         IdentificationCommandList [6] = { 1, 2, 3, 8, 9 , 11,};
    UINT8                         *SdCommandClassList[] = { SdCommandClass0, SdCommandClass1, SdCommandClass2,\
                                  SdCommandClass3, SdCommandClass4, SdCommandClass5, SdCommandClass6, \
                                  SdCommandClass7, SdCommandClass8, SdCommandClass9, SdCommandClass10, SdCommandClass11 };
    UINT8                         *MmcCommandClassList[] = { MmcCommandClass0, MmcCommandClass1, MmcCommandClass2,\
                                      MmcCommandClass3, MmcCommandClass4, MmcCommandClass5, MmcCommandClass6, \
                                      MmcCommandClass7, MmcCommandClass8, MmcCommandClass9, MmcCommandClass10, MmcCommandClass11 };
    UINT8                         *SupportedClass;
    
    if (Packet == NULL || Slot == 0xFF) {
        return EFI_INVALID_PARAMETER;
    }

    // Finding the SdioDeviceInterface pointer
    SdioDevInterface = GetSdioDevInterface(
                           SdMmcPassThruInstance->SdioBusInterface, 
                           Slot
                           );
    
    // If SdioDevInterface is NULL,then no device is connected to the Slot
    if (SdioDevInterface == NULL) {
        return EFI_NO_MEDIA;
    }
    
	if(SdioDevInterface->MassStorageDevice) {
		// Commands listed in IdentificationCommandList is supported only during Card initialization, Hence 
		// return EFI_UNSUPPORTED for these commands
	    
		for (Index = 0; Index <= sizeof (IdentificationCommandList); Index++ ) {
			if (IdentificationCommandList [Index] == Packet->SdMmcCmdBlk->CommandIndex) {
				return EFI_UNSUPPORTED;
			}	
		}
	
		//From the CSD register ( Bit 84: 95), get the Supported CommandClass for the card 
		CommandClass = (SdioDevInterface->SdioDevInfo->d4CSD[2] & 0xFFF00000) >> 20; 
    
		// Check whether the input packet command is supported or not.
		for (ClassIndex = 0; ClassIndex <= 11; ClassIndex++) {
			if(CommandClass & (1 << ClassIndex)) {
				if ((SdioDevInterface->SdioDevInfo->bMode == MMC_HIGH_CAP ) || 
					(SdioDevInterface->SdioDevInfo->bMode == MMC_STAND_CAP )) {
					// Get the supported command list for Sd card
					SupportedClass = MmcCommandClassList [ClassIndex];
				} else {
					// Get the supported command list for MMC card
					SupportedClass = SdCommandClassList [ClassIndex];
				}
				for (Index = 0; SupportedClass [Index] != 0xFF; Index++ ) {
					if (SupportedClass [Index] == Packet->SdMmcCmdBlk->CommandIndex) {
						CommandSupported = TRUE;
						break;
					}
				}
			}
			if (CommandSupported) {
				break;
			}
		}
		//check for Sdio command lists
    } else if (SdioDevInterface->SdioDevInfo->IODevice) {
		for (Index =0; SdIoCommandList[Index] !=0xFF;Index++ ) {
			if (SdIoCommandList [Index] == Packet->SdMmcCmdBlk->CommandIndex) {
			    CommandSupported = TRUE;
			    break;
			}		
		}
      
    }
	
    // The command described by the SD Command Packet is not supported.
	if (!CommandSupported) {
		return EFI_UNSUPPORTED;
	}
	
	switch (Packet->SdMmcCmdBlk->ResponseType) {
	    case SdMmcResponseTypeR1:
	        Packet->SdMmcCmdBlk->ResponseType =  RESP1_TYPE;
	        break;
	    case SdMmcResponseTypeR1b:
	        Packet->SdMmcCmdBlk->ResponseType = RESP1B_TYPE;
	        break;
	    case SdMmcResponseTypeR2:
	        Packet->SdMmcCmdBlk->ResponseType =  RESP2_TYPE;
	        break;
	    case SdMmcResponseTypeR3:
	        Packet->SdMmcCmdBlk->ResponseType =  RESP3_TYPE;
	        break;
	    case SdMmcResponseTypeR4: 
	        Packet->SdMmcCmdBlk->ResponseType = RESP4_TYPE;
	        break;
	    case SdMmcResponseTypeR5: 
	        Packet->SdMmcCmdBlk->ResponseType =  RESP5_TYPE;
	        break;
	    case SdMmcResponseTypeR5b:
	        Packet->SdMmcCmdBlk->ResponseType =  RESP5B_TYPE;
	        break;
	    case SdMmcResponseTypeR6: 
	        Packet->SdMmcCmdBlk->ResponseType =  RESP6_TYPE;
	        break;
	    case SdMmcResponseTypeR7: 
	        Packet->SdMmcCmdBlk->ResponseType =  RESP7_TYPE;
	        break;
	}
    if (Packet->SdMmcCmdBlk->CommandType != SdMmcCommandTypeAdtc) { // Process Non data commands
        
        Status = SdMmcCommand(SdioDevInterface->SdioDevInfo,
                              Slot, 
                              (Packet->SdMmcCmdBlk->CommandIndex << 8) | (Packet->SdMmcCmdBlk->ResponseType), 
                              Packet->SdMmcCmdBlk->CommandArgument
                              );
        
        
        // Update the Status once transaction is completed.
        Packet->TransactionStatus = Status;
        
        if (EFI_ERROR(Status)) {
            return EFI_DEVICE_ERROR;
        }
    } else { // Process Data Commands
    	
    	//Packet defines a data command but both InDataBuffer
    	//and OutDataBuffer are NULL.
        if ( (Packet->InDataBuffer == NULL) && (Packet->OutDataBuffer == NULL) ) {
            return EFI_INVALID_PARAMETER;
        }
        
        // Check for Read/Write Commands
        Read = (Packet->InDataBuffer) ? 1  :  0;
        
        TransferBuffer = (UINTN)(Read) ? Packet->InDataBuffer: Packet->OutDataBuffer;
        
        if(SdioDevInterface->MassStorageDevice) {
            
    		// Calculate the maximum byte transfer supported by the host
    		MaxBytes = Mul64(((UINT64)SdioDevInterface->SdioDevInfo->dMaxLBA),
                          SdioDevInterface->SdioDevInfo->wBlockSize);
        
    		// The InTransferLength or OutTransferLength or exceeds the limit supported by SD card
    		if(((UINTN)(Read) ? Packet->InTransferLength:Packet->OutTransferLength) > MaxBytes) {
    			return EFI_BAD_BUFFER_SIZE;
    		}
        
    		//Calculate the block count for transfer
    		NumBlks = ((UINTN)(Read) ? Packet->InTransferLength:Packet->OutTransferLength) \
                                            / SdioDevInterface->SdioDevInfo->wBlockSize;
        
    		//Send Read/Write command
    		Status = SdMmcReadWriteData(SdioDevInterface->SdioDevInfo,
                                    	Slot,
                                    	(UINT8)Packet->SdMmcCmdBlk->CommandIndex,
                                    	Packet->SdMmcCmdBlk->CommandArgument,
                                    	TransferBuffer,
                                    	NumBlks,
                                    	SdioDevInterface->SdioDevInfo->wBlockSize,
                                    	Packet->SdMmcCmdBlk->ResponseType,
                                    	FALSE,
                                    	Read
                                    	);
        } else if (SdioDevInterface->SdioDevInfo->IODevice) {
            
            Status = SdIoReadWriteCmd(SdioDevInterface->SdioDevInfo,
                                      Slot,
                                      Packet->SdMmcCmdBlk->CommandIndex,
                                      Packet->SdMmcCmdBlk->CommandArgument,
                                      TransferBuffer,
                                      Packet->SdMmcCmdBlk->ResponseType,
                                      Read );      
    	}
    	
#if SDMMC_VERBOSE_PRINT
        DEBUG((EFI_D_VERBOSE,"SdMmc: PassThru Status = %r\n",Status));
#endif
    	// Update the Status once transaction is completed.
    	Packet->TransactionStatus = Status;
    	if(EFI_ERROR(Status)) {
    		return EFI_DEVICE_ERROR;
    	} 	
    }
    return EFI_SUCCESS;
}

/**
    Retrieves the next slot number for SdMmc devices on the
    SD controller.

    @param  IN EFI_SD_MMC_PASS_THRU_PROTOCOL   *This,
    @param  IN OUT UINT8   *Slot

    @retval EFI_SUCCESS or EFI_NOT_FOUND

**/

EFI_STATUS 
GetNextSlot (
    IN  EFI_SD_MMC_PASS_THRU_PROTOCOL *This,
    IN  OUT UINT8                     *Slot
)
{
    SDIO_DEVICE_INTERFACE           *SdioDevInterface = NULL;
    DLINK                           *dlink = NULL;
    SD_MMC_PASS_THRU_PROTOCOL       *SdMmcPassThruInstance = \
                                      (SD_MMC_PASS_THRU_PROTOCOL *)This;
    UINT8                           PreviousSlot;

    PreviousSlot = SdMmcPassThruInstance->PreviousSlot;
    
    // If Slot is NULL (or) If Slot is not 0xFF and Slot was not returned 
    // on a previous call to GetNextSlot()
    if(Slot == NULL || ((0xFF != *Slot ) && (PreviousSlot != *Slot))) {
        return EFI_INVALID_PARAMETER;
    }

    // If on input Slot is 0xFFFF, then the slot number of the first slot on the
    // SD controller is returned.
    if(0xFF == *Slot) {
        dlink = SdMmcPassThruInstance->SdioBusInterface->SdioDeviceList.pHead;
        if (!dlink) {
            return EFI_NOT_FOUND;
        }
        SdioDevInterface = OUTTER(dlink, SdioDeviceLink, SDIO_DEVICE_INTERFACE);
        *Slot = SdioDevInterface->SlotNumber;
        SdMmcPassThruInstance->PreviousSlot = *Slot;
        return EFI_SUCCESS;
    }

    // If Slot is a slot number that was returned on a previous call to GetNextSlot(), 
    // then the slot number of the next slot on the SD controller is returned.
    if(PreviousSlot == *Slot){
        // Find the SdioDeviceInterface pointer
        dlink = SdMmcPassThruInstance->SdioBusInterface->SdioDeviceList.pHead;
        if (!dlink) {
            return EFI_NOT_FOUND;
        }
        do {
            SdioDevInterface = OUTTER(dlink, SdioDeviceLink, SDIO_DEVICE_INTERFACE);
            // If SdioDevInterface->SlotNumber exceeds the Previous Slot value, 
            // then initialize the Slot value accordingly
            if (SdioDevInterface->SlotNumber > *Slot) { 
                *Slot = SdioDevInterface->SlotNumber;
                SdMmcPassThruInstance->PreviousSlot = *Slot;
                return EFI_SUCCESS;
            }
            dlink = dlink-> pNext;
            SdioDevInterface = NULL;
        } while (dlink);
    }
    return EFI_NOT_FOUND;
}

/**
    Routine Searches device path by specific Type and SubType

    @param DevicePath A pointer to the device path protocol
    @param Type       Device path Node Type
    @param SubType    Device path node subtype

    @retval EFI_DEVICE_PATH_PROTOCOL*

**/

EFI_DEVICE_PATH_PROTOCOL*
SearchDevicePathNode (
  IN OUT EFI_DEVICE_PATH_PROTOCOL  *DevicePath,
  IN     UINT8                      Type,
  IN     UINT8                      SubType
)
{
    if (DevicePath == NULL) {
        return NULL;
    }

    while (!IsDevicePathEnd (DevicePath)) {
        if ((DevicePathType (DevicePath) == Type) 
             && (DevicePathSubType (DevicePath) == SubType)) {
            return DevicePath;
        }
        DevicePath = NextDevicePathNode (DevicePath);
    }

    return NULL;
}

/**
    This function allocates and builds a device path node for 
    an SD card on the SD controller.

    @param  IN  EFI_SD_MMC_PASS_THRU_PROTOCOL     *This,
    @param  IN  UINT8                             Slot,
    @param  IN  OUT EFI_DEVICE_PATH_PROTOCOL    **DevicePath

    @retval EFI_SUCCESS or EFI_NOT_FOUND

**/
EFI_STATUS
BuildDevicePath (
    IN EFI_SD_MMC_PASS_THRU_PROTOCOL     *This,
    IN UINT8                             Slot,
    IN OUT EFI_DEVICE_PATH_PROTOCOL      **DevicePath
)
{
    EFI_STATUS                    Status;
    SD_DEVICE_PATH                *SdDevicePathNode = NULL;
    SDIO_DEVICE_INTERFACE         *SdioDevInterface = NULL;
    SD_MMC_PASS_THRU_PROTOCOL     *SdMmcPassThruInstance = \
                                          (SD_MMC_PASS_THRU_PROTOCOL *)This;
  
    // If DevicePath is null, return EFI_INVALID_PARAMETER
    if(NULL == DevicePath) {
        return  EFI_INVALID_PARAMETER;
    }
	
    // Allocate memory for the DevicepathNode
    Status = pBS->AllocatePool(EfiBootServicesData,
                               sizeof(SD_DEVICE_PATH),
                               (VOID**)&SdDevicePathNode
                               );
    if(EFI_ERROR(Status)) {           
        return EFI_OUT_OF_RESOURCES;
    }
    
    // Finding the SdioDeviceInterface pointer
    SdioDevInterface = GetSdioDevInterface(
                         SdMmcPassThruInstance->SdioBusInterface, 
                         Slot
                         );
    // The SD card specified by Slot does not exist on the SD controller
    if (SdioDevInterface == NULL) {
        return EFI_NOT_FOUND;
    }
	
    // Get the SD Device Path Node
    SdDevicePathNode = (SD_DEVICE_PATH*)SearchDevicePathNode (
                                            SdioDevInterface->DevicePathProtocol, 
                                            MESSAGING_DEVICE_PATH, 
                                            MSG_SD_DP
                                            );
    pBS->CopyMem( *DevicePath, SdDevicePathNode, sizeof(SD_DEVICE_PATH));
    
    return EFI_SUCCESS;
}

/**
    Retrieves an SD card slot number based on the input device path.
	
    @param IN EFI_SD_MMC_PASS_THRU_PROTOCOL     *This,
    @param IN EFI_DEVICE_PATH_PROTOCOL          *DevicePath,
	@param IN UINT8                             *Slot,

    @retval EFI_STATUS

**/
EFI_STATUS 
GetSlotNumber (
    IN EFI_SD_MMC_PASS_THRU_PROTOCOL    *This,
    IN EFI_DEVICE_PATH_PROTOCOL         *DevicePath,
    OUT UINT8                           *Slot
) 
{
    UINT16        DevicePathLength = 0;

    if(!DevicePath || !This || !Slot) {
        return EFI_INVALID_PARAMETER;
    }
    
    //Calculate DevicepathLength
    DevicePathLength = (DevicePath)->Length[0] | (DevicePath)->Length[1] << 8;

    //If device path doesn't match return EFI_UNSUPPORTED
    if ((DevicePath->Type != MESSAGING_DEVICE_PATH) || 
        (DevicePath->SubType != MSG_SD_DP) ||
        (DevicePathLength != sizeof(SD_DEVICE_PATH))) {
        return EFI_UNSUPPORTED;
    }
	
    *Slot = ((SD_DEVICE_PATH* )DevicePath)->SlotNumber;
    
    return EFI_SUCCESS;
	
}

/**
    Resets an SD card that is connected to the SD controller.
    
    @param  IN EFI_SD_MMC_PASS_THRU_PROTOCOL   *This,
    @param  IN UINT8                           Slot

    @retval EFI_SUCCESS or EFI_NOT_FOUND

**/

EFI_STATUS
ResetDevice (
    IN  EFI_SD_MMC_PASS_THRU_PROTOCOL  *This,
    IN  UINT8                          Slot
)
{
    UINT16                        CommandIndex;
    UINT32                        CommandArgument;
    SDIO_DEVICE_INTERFACE         *SdioDevInterface = NULL;
    SD_MMC_PASS_THRU_PROTOCOL     *SdMmcPassThruInstance = \
                                      (SD_MMC_PASS_THRU_PROTOCOL *)This;
    EFI_STATUS                    Status;
    
    // Slot number is invalid.
    if(Slot == 0xFF) {
        return EFI_INVALID_PARAMETER;
    }
    
    // Finding the SdioDeviceInterface pointer
    SdioDevInterface = GetSdioDevInterface(
                                 SdMmcPassThruInstance->SdioBusInterface, 
                                 Slot
                                 );
    
    // SD Device not present in the Slot.
    if (SdioDevInterface == NULL) {
        return EFI_NO_MEDIA;
    }
	if(SdioDevInterface->SdioDevInfo->IODevice ) {
		CommandIndex = (IO_RW_DIRECT << 8) | (RESP5_TYPE);
		
		//Enable RES (bit-3) of IO_ABORT register
		CommandArgument = (UINT32)((1 << 31) + ((IO_ABORT_REGISTER) << 9) + RESET );    
	    Status = SdMmcCommand(SdioDevInterface->SdioDevInfo,
	                         Slot,CommandIndex,
	                         CommandArgument
	                         );	    
	    
	} else {
		// Issue CMD0 with argument of 0x00000000 to reset the device.
		CommandIndex = GO_IDLE_STATE_CMD0;
		CommandArgument=0;
		Status = SdMmcCommand(SdioDevInterface->SdioDevInfo,
                         Slot,CommandIndex,
                         CommandArgument
                         );
	}
	
#if SDMMC_VERBOSE_PRINT
        DEBUG((EFI_D_VERBOSE,"SdMmc: Reset Status= %r\n",Status));
#endif  
        
    if(EFI_ERROR(Status)) {
        return EFI_DEVICE_ERROR;
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
