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

/** @file BootSectorWriteProtect.c
    MBR and GPT write protection driver

**/

//---------------------------------------------------------------------------

#include <Token.h>
#include "AmiDxeLib.h"
#include "Protocol/LegacyBiosExt.h"
#include <Protocol/GraphicsOutput.h>
#include <Protocol/SimpleTextOut.h>
#include <Protocol/SimpleTextIn.h>
#include <Protocol/ConsoleControl.h>
#include <Protocol/AmiBlockIoWriteProtection.h>
#include "Setup.h"
#include "BootSectorWriteProtect.h"

//---------------------------------------------------------------------------

AMI_BLOCKIO_WRITE_PROTECTION_PROTOCOL AmiBlockIoWriteProtectionProtocol;

VOID *gGptWriteNotifyReg = NULL;

CHAR16* MsgLine1 = L"BootSector Write !!    ";
CHAR16* MsgLine2 = L" Continue (Y/N)? ";

DLIST   GptBlockIOList;
BOOLEAN GptProtectionEnabled=FALSE;

/**
    Allocates the stack for the BootSectorWrite Protect module in EBDA

    @param ImageHandle
    @param SystemTable

    @retval EFI_STATUS

**/

EFI_STATUS
EFIAPI
BootSectorWriteProtectEntryPoint (
    IN  EFI_HANDLE        ImageHandle,
    IN  EFI_SYSTEM_TABLE  *SystemTable
)
{
    EFI_STATUS      Status;
    EFI_EVENT       LegacyEvent;
    EFI_EVENT       ReadyToBootEvent;
    EFI_GUID        guidSetup = SETUP_GUID;
    SETUP_DATA      SetupData;
    UINTN           Size = sizeof(SETUP_DATA);
    EFI_HANDLE      Handle = NULL;

    InitAmiLib(ImageHandle, SystemTable);

    Status = pRS->GetVariable(L"Setup",&guidSetup, NULL, &Size, &SetupData);
    if (EFI_ERROR(Status)) {
        return Status;
    }

    if(SetupData.BootSecWp == 0 ) {
        // GPT write protect message disabled.
        return EFI_NOT_FOUND;
    }

    // Legacy Write Protection Support
    CreateLegacyBootEvent(TPL_CALLBACK,
                           &LegacyBootSectorWriteProtectEnable,
                           NULL, 
                           &LegacyEvent);

    // GPT Protection Support
    Status = CreateReadyToBootEvent(TPL_CALLBACK, 
                                    &GptProtectionEnable,
                                    NULL,
                                    &ReadyToBootEvent);
    if(!EFI_ERROR(Status)) {
#if GPT_PARTITION_WRITE_PROTECTION
        {
            EFI_EVENT   BlockIoEvent;
        
            // Call back to find out GPT partition BlockIO List
            Status = RegisterProtocolCallback( &gEfiBlockIoProtocolGuid,
                                               FindGptInBlockIO,
                                               NULL,
                                               &BlockIoEvent,
                                               &gGptWriteNotifyReg);
            if(!EFI_ERROR(Status)) {
                pBS->CloseEvent(ReadyToBootEvent);
            }
            // Initialize the List
            DListInit(&GptBlockIOList);
        }
#endif
    }

    AmiBlockIoWriteProtectionProtocol.BlockIoWriteProtectionCheck = BlockIoWriteProtectionCheck;

    Status = pBS->InstallMultipleProtocolInterfaces(&Handle,
                                                    &gAmiBlockIoWriteProtectionProtocolGuid,
                                                    &AmiBlockIoWriteProtectionProtocol,
                                                    NULL);

    return Status;
}

/**
    Enable the GPT BootSectorWrite Protection

    @param Event
    @param Context

    @retval VOID

**/
VOID
EFIAPI
GptProtectionEnable (
    IN  EFI_EVENT    Event,
    IN  VOID         *Context
)
{
    GptProtectionEnabled=TRUE;
    pBS->CloseEvent(Event);
    return;
}

/**
    Enable the BootSectorWrite Protection for the Legacy

    @param Event
    @param Context

    @retval VOID

**/
VOID
EFIAPI
LegacyBootSectorWriteProtectEnable (
    IN  EFI_EVENT    Event,
    IN  VOID         *Context
)
{
    EFI_STATUS                      Status;
    EFI_LEGACY_BIOS_EXT_PROTOCOL    *BiosExtensions = NULL;
    EFI_LEGACY_BIOS_PROTOCOL        *Bios = NULL;
    UINT32                          EbdaAddress;
    UINT32                          EbdaStartOffset;
    UINT32                          LockUnlockAddr, LockUnlockSize;
    UINTN                           BootSectorWriteProtectLocation = 0;
    UINT16                          *VpPtr;
    UINT8                           *Misc_Info= (UINT8*)0xFF670;

    // Find the BootSectorWrite Protect module in Csm16 area
    Status = FindBootSectorWriteProtectLocation(&BootSectorWriteProtectLocation);

    if(EFI_ERROR(Status)) {
        return ;
    }

    Status = pBS->LocateProtocol(&gEfiLegacyBiosProtocolGuid,
                                 NULL,
                                 (VOID**)&Bios);

    if (EFI_ERROR(Status)) {
        return ;
    }

    Status = pBS->LocateProtocol(&gEfiLegacyBiosExtProtocolGuid,
                                 NULL,
                                 (VOID**)&BiosExtensions);

    if (EFI_ERROR(Status)) { 
        return ;
    }

    //
    // Allocate EBDA area for BootSectorWrite Protect module Stack
    //
    Status = BiosExtensions->AllocateEbda(BiosExtensions,
                                          1,
                                          &EbdaAddress,
                                          &EbdaStartOffset);
    if (!EFI_ERROR(Status)) { 
        Status = BiosExtensions->UnlockShadow((UINT8*)BootSectorWriteProtectLocation,
                                              0xF,
                                              &LockUnlockAddr,
                                              &LockUnlockSize);
        if (!EFI_ERROR(Status)) { 
            // Update the MISC info byte for checking the MBR write
            *(Misc_Info) |= CSM16_I13_HDD_MBR_WP_BIT;

            // Skip the Signature
            BootSectorWriteProtectLocation += 4;

            // Save the EBDA start offset
            VpPtr = (UINT16*)BootSectorWriteProtectLocation;
            *VpPtr = EbdaStartOffset;

            // Update that EBDA successfully allocated
            *((UINT8*)(VpPtr+1)) = (UINT8)0xAA;
            
            BiosExtensions->LockShadow(LockUnlockAddr, LockUnlockSize);
            
            pBS->CloseEvent(Event);
        }
    }
    return;
}

/**
    Find the BootSectorWrite Protection module in E0000 and F0000 Region

    @param BootSectorWriteProtectLocation

    @retval Status

**/
EFI_STATUS
FindBootSectorWriteProtectLocation (
    IN  OUT UINTN   *BootSectorWriteProtectLocation
)
{

    UINT8 *PtrCsmLocation = (UINT8*)0xe0000;

    for( ; PtrCsmLocation < (UINT8*)0x100000 ; PtrCsmLocation++ ) {
        if(MemCmp(PtrCsmLocation, "$PV$", 4) == 0) {
            *BootSectorWriteProtectLocation = (UINTN)PtrCsmLocation;
            return EFI_SUCCESS;
        }
    }

    return EFI_NOT_FOUND;
}

/**
    Return the user input.

    @param This
    @param BlockIo
    @param StartLBA
    @param BufferSize

    @retval EFI_SUCCESS Proceed with writing boot sector.
    @retval EFI_ACCESS_DENIED Abort writing to boot sector.

**/

EFI_STATUS
EFIAPI
BlockIoWriteProtectionCheck (
    IN  AMI_BLOCKIO_WRITE_PROTECTION_PROTOCOL    *This,
    IN  EFI_BLOCK_IO_PROTOCOL                    *BlockIo,
    IN  EFI_LBA                                  StartLBA,
    IN  UINTN                                    BufferSize
)
{
    EFI_STATUS                          Status;
    EFI_STATUS                          rStatus = EFI_SUCCESS;
    EFI_GRAPHICS_OUTPUT_PROTOCOL        *Gop = NULL; 
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL       *PixelBuffer = NULL;
    EFI_CONSOLE_CONTROL_PROTOCOL        *ConsoleControl = NULL;
    EFI_CONSOLE_CONTROL_SCREEN_MODE     ScreenMode;
    EFI_INPUT_KEY                       Key = {0,0};
    UINT32                              GraphicsMode;
    UINTN                               LastAttrib;
    UINTN                               X, Y;
    UINTN                               Column, Row;
    UINTN                               MaxX, MaxY;

#if GPT_PARTITION_WRITE_PROTECTION
    GPT_PARTITION_INFO                  *GptBlockRecord=NULL;
    EFI_LBA                             EndLBA;
    // Get the GPT record for the BlockIO
    GptBlockRecord=CheckBlockIoInGptRecord(BlockIo);
    if(GptBlockRecord != NULL && GptProtectionEnabled ) {
        // If the Write LBA is not in the partition allow the write to happen
        AdjustBufferSize(BlockIo,&BufferSize);
        EndLBA = (StartLBA + (BufferSize/BlockIo->Media->BlockSize)) -1;
        if (StartLBA >= GptBlockRecord->FirstUsableLBA && EndLBA <= GptBlockRecord->LastUsableLBA ) {
            return EFI_SUCCESS;
        }
    } else if( StartLBA != 0) {
        return EFI_SUCCESS;
    }
#else
    if( StartLBA != 0) {
        return EFI_SUCCESS;
    }
#endif
   
    // Locate GOP Protocol
    Status = pBS->LocateProtocol(&gEfiGraphicsOutputProtocolGuid,
                                 NULL,
                                 (VOID**)&Gop);
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status)) {
        ERROR_CODE(DXE_MBR_GPT_WRITE_PROTECTED, EFI_ERROR_MAJOR);
        return rStatus;
    }

    // Allocate buffer to save screen
    Status = pBS->AllocatePool(EfiBootServicesData,
                               Gop->Mode->Info->VerticalResolution *
                               Gop->Mode->Info->HorizontalResolution *
                               sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL),
                               (VOID**)&PixelBuffer );
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status)) {
        ERROR_CODE(DXE_MBR_GPT_WRITE_PROTECTED, EFI_ERROR_MAJOR);
        return rStatus;
    }

    // Save current graphics mode
    GraphicsMode = Gop->Mode->Mode;
    
    // Save current cursor position
    Column = pST->ConOut->Mode->CursorColumn;
    Row = pST->ConOut->Mode->CursorRow;

    if (PixelBuffer) {
        // Save the screen to buffer
        Status=Gop->Blt(Gop,
                        PixelBuffer,
                        EfiBltVideoToBltBuffer,
                        0,
                        0,
                        0,
                        0,
                        Gop->Mode->Info->HorizontalResolution,
                        Gop->Mode->Info->VerticalResolution,
                        0 );

        if (EFI_ERROR(Status)) {
            pBS->FreePool( PixelBuffer );
            PixelBuffer = NULL;
            ERROR_CODE(DXE_MBR_GPT_WRITE_PROTECTED, EFI_ERROR_MAJOR);
            return rStatus;
        }
    } // if (PixelBuffer)

    // Locate ConsoleControl
    Status = pBS->LocateProtocol( &gEfiConsoleControlProtocolGuid,
                                  NULL,
                                  (VOID**)&ConsoleControl );
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status)) {
        ERROR_CODE(DXE_MBR_GPT_WRITE_PROTECTED, EFI_ERROR_MAJOR);
        return rStatus;
    }

    // Set text mode
    ConsoleControl->GetMode( ConsoleControl,
                             &ScreenMode,
                             NULL,
                             NULL );
    if (ScreenMode == EfiConsoleControlScreenGraphics) {
        ConsoleControl->SetMode( ConsoleControl, EfiConsoleControlScreenText );
    }

    // Save text color attributes
    LastAttrib = pST->ConOut->Mode->Attribute;

    // Set desired color attributes and clear the screen
    pST->ConOut->SetAttribute( pST->ConOut, EFI_TEXT_ATTR(EFI_LIGHTGRAY, EFI_BLACK) );
    pST->ConOut->EnableCursor( pST->ConOut, FALSE );
    pST->ConOut->ClearScreen( pST->ConOut); 

    // Get maximum of x-y co-ordinate
    pST->ConOut->QueryMode(pST->ConOut,
                           pST->ConOut->Mode->Mode,
                           &MaxX,
                           &MaxY);

    // Set co-ordinates to keep the message in center
    X = MaxX/2 - EfiStrLen(MsgLine1)/2; 
    Y = MaxY/2 - 1; 

    //Set cursor position and print the message
    pST->ConOut->SetCursorPosition( pST->ConOut, X, Y );
    pST->ConOut->OutputString ( pST->ConOut, MsgLine1 );
    pST->ConOut->SetCursorPosition( pST->ConOut, X, Y+1 );
    pST->ConOut->OutputString ( pST->ConOut, MsgLine2 );

    // Restore text color attributes
    pST->ConOut->SetAttribute( pST->ConOut, LastAttrib );

    // Wait for user input until key is Y/y/N/n
    do {
        Status = pBS->CheckEvent ( pST->ConIn->WaitForKey );
        if(!EFI_ERROR(Status)) {
            Status = pST->ConIn->ReadKeyStroke ( pST->ConIn, &Key );
            if (!EFI_ERROR (Status)) {
                if (Key.UnicodeChar == L'Y' || Key.UnicodeChar == L'y') {
                    rStatus = EFI_SUCCESS; 
                    break;
                } else if (Key.UnicodeChar == L'N' || Key.UnicodeChar == L'n') {
                    rStatus = EFI_ACCESS_DENIED; 
                    break;
                }
            }
        }
    }while(1);

    if (PixelBuffer){
        // Restore graphics mode
        Status = Gop->SetMode(Gop, GraphicsMode);
        if (!EFI_ERROR(Status)) {
            // Restore graphics screen
            Status = Gop->Blt(
                        Gop, PixelBuffer, EfiBltBufferToVideo, 0, 0, 0, 0,
                        Gop->Mode->Info->HorizontalResolution,
                        Gop->Mode->Info->VerticalResolution, 0
                     );
            if (EFI_ERROR(Status)) {
                pBS->FreePool( PixelBuffer );
                PixelBuffer = NULL;
                ERROR_CODE(DXE_MBR_GPT_WRITE_PROTECTED, EFI_ERROR_MAJOR);
                return rStatus;
            }
            
            // Restore cursor position
            pST->ConOut->SetCursorPosition( pST->ConOut, Column, Row );
        }
        pBS->FreePool(PixelBuffer); 
    }

    // On success log MBR write operation
    if(rStatus == EFI_SUCCESS) {
        ERROR_CODE(DXE_MBR_GPT_WRITE_PROTECTED, EFI_ERROR_MAJOR);
    }
    return rStatus;
}

/**
    Return the number of Unicode characters in String.

    @param String String to process

    @retval Number of Unicode characters in String.

**/

UINTN
EfiStrLen (
    IN CHAR16   *String
)
{
    UINTN Length;
    for (Length=0; *String; String++, Length++);
    return Length;
}

/**
    Calculates correct Buffer size for BlockIO transfer

    @param BlockIo  BlockIo Protocol Interface
    @param BufferSize Pointer To the Size Needed

    @retval Modifies Buffer Size to reflect correct number of bytes
    @retval Nothing

**/

VOID
AdjustBufferSize (
    IN  EFI_BLOCK_IO_PROTOCOL    *BlockIo,
    IN  OUT UINTN                *BufferSize
)
{
    UINTN   sz;
    
    // Calculate the size of transaction
    sz=BlockIo->Media->BlockSize * (*BufferSize/BlockIo->Media->BlockSize);
    
    if (*BufferSize % BlockIo->Media->BlockSize) 
        sz+=BlockIo->Media->BlockSize;
    
    *BufferSize=sz;
}

/**
    Checks if the GPT table is valid

    @param  BlockIo Pointer to the Parent BlockIo instance
    @param  GptHeader Pointer to a memory area that contains the GPT header to validate
    @param  HdrLoc Header location

    @retval VOID
    @retval EFI_SUCCESS GPT is valid
    @retval EFI_INVALID_PARAMETER checks to see if the CRC, signature, and Header
        Location are valid
    @retval EFI_OUT_OF_RESOURCES Not enough free memory to allocate to read the
        GPT entry table

    @Note: Referrals: CalculateCrc32 MemCmp AdjustBufferSize AllocatePool ReadBlocks
        FreePool
**/
EFI_STATUS 
ValidateGptHeader (
    IN  EFI_BLOCK_IO_PROTOCOL    *BlockIo,
    IN  GPT_HEADER               *GptHeader,
    IN  EFI_LBA                  HdrLoc
)
{
    EFI_STATUS          Status;
    UINT32              Crc;
    UINT32              Temp32;
    UINTN               Size;
    PARTITION_ENTRY     *GptEntryTable;

    // Get CRC of the Partition Header
    Temp32 = GptHeader->Header.CRC32;
    GptHeader->Header.CRC32 = 0;

    pBS->CalculateCrc32((UINT8*)GptHeader, sizeof(GPT_HEADER), &Crc);
    GptHeader->Header.CRC32 = Crc;

    // Check calculated CRC
    // Check signature
    // Check that MyLBA parameter points to the location of the current Header
    if (MemCmp(EFI_GPT_HEADER_ID, &GptHeader->Header.Signature, sizeof(UINT64))
            || (Temp32 != Crc)
            || (GptHeader->MyLba != HdrLoc) ){
        return EFI_INVALID_PARAMETER;
    }

    // Calculate the size of transaction
    Size = GptHeader->NumberOfPartitionEntries *
           GptHeader->SizeOfPartitionEntry;
    
    AdjustBufferSize(BlockIo,&Size);

    // allocate memory for the Entry array
    Status = pBS->AllocatePool( EfiBootServicesData,
                                Size,
                                (VOID**)&GptEntryTable);

    if (EFI_ERROR(Status)) {
        return EFI_OUT_OF_RESOURCES;
    }
    // get the data from the entry array
    BlockIo->ReadBlocks(BlockIo,
                        BlockIo->Media->MediaId,
                        GptHeader->PartitionEntryLba,
                        Size,
                        GptEntryTable);

    // calculate the CRC value for the entry array
    pBS->CalculateCrc32((UINT8*)GptEntryTable,
                        GptHeader->NumberOfPartitionEntries*
                        GptHeader->SizeOfPartitionEntry,
                        &Crc);

    // the the allocated memory
    if (GptEntryTable != NULL ) {
        pBS->FreePool(GptEntryTable);
    }
    
    // check if the CRC that was calculated matches the value that was stored
    if (Crc != GptHeader->Crc32){
        return EFI_INVALID_PARAMETER;
    }

    // All checks passed, the GPT is valid
    return EFI_SUCCESS;
}

/**
    Check if either GPT is valid. if both are bad,
    then it is not a valid GPT system return error

    @param  BlockIo pointer to the BlockIo partition so we can read and 
    write to the Block Device
    @param  Gpt Pointer to a memory area that contains the GPT header
    GPT_HEADER

    @retval EFI_SUCCESS If a valid GPT is found
    @retval EFI_UNSUPPORTED if no valid GPT is found
    @retval EFI_OUT_OF_RESOURCES not enough allocatable memory is available

    @Note: Referrals: AllocatePool ReadBlocks FreePool AdjustBufferSize
    ValidateGPTHeader CopyGPT CopyMem
**/
EFI_STATUS 
VerifyPartionTables (
    IN  EFI_BLOCK_IO_PROTOCOL   *BlockIo,
    OUT GPT_HEADER              *Gpt
)
{
    EFI_STATUS          Status;
    GPT_HEADER          *Primary = NULL;
    GPT_HEADER          *Secondary = NULL;
    MASTER_BOOT_RECORD  *Pmbr = NULL;
    UINTN               Size;
    UINT8               i, FoundPP = 0;

    // Allocate a buffer for the Protective MBR
    Status = pBS->AllocatePool(EfiBootServicesData,
                               BlockIo->Media->BlockSize,
                               (VOID**)&Pmbr);

    if (EFI_ERROR(Status)){
        return EFI_OUT_OF_RESOURCES;
    }

    // Read the Protective MBR from LBA #0
    Status = BlockIo->ReadBlocks (BlockIo,
                                  BlockIo->Media->MediaId,
                                  0,
                                  BlockIo->Media->BlockSize,
                                  Pmbr);

    if (EFI_ERROR (Status)) {
        pBS->FreePool(Pmbr);
        return Status;
    }
    for (i=0; i < 4; i++){
        // See if Protective MBR is valid and if not free allocated memory and return EFI_UNSUPPORTED
        if (Pmbr->PartRec[i].OSType == 0xee){
            FoundPP++;
        }
    }
    if (FoundPP == 0) {
        pBS->FreePool(Pmbr);
        return EFI_UNSUPPORTED;
    }

    // Allocate memory to hold the data for the Primary and Secondary GPT headers
    // Allocate Primary
    Size = sizeof(GPT_HEADER);
    AdjustBufferSize(BlockIo, &Size);

    Status = pBS->AllocatePool(EfiBootServicesData,
                               Size,
                               (VOID**)&Primary);
    
    // if error is returned, exit with No resources available
    if (EFI_ERROR(Status)) {
        pBS->FreePool(Pmbr);
        return EFI_OUT_OF_RESOURCES;
    }
    
    // Allocate for secondary header
    Status = pBS->AllocatePool(EfiBootServicesData,
                               Size,
                               (VOID**)&Secondary);
    // if error, free memory allocated for primary and then exit with no resources available
    if (EFI_ERROR(Status)) {
        pBS->FreePool(Pmbr);
        pBS->FreePool(Primary);
        return EFI_OUT_OF_RESOURCES;
    }
    
    // Read primary and backup table
    BlockIo->ReadBlocks(BlockIo,
                        BlockIo->Media->MediaId,
                        1,
                        Size,
                        Primary);

    BlockIo->ReadBlocks(BlockIo,
                        BlockIo->Media->MediaId,
                        BlockIo->Media->LastBlock,
                        Size,
                        Secondary);

    // read primary header from the block device and verify if it is valid
    if (EFI_ERROR(ValidateGptHeader(BlockIo, Primary, 1))) {
        // if primary is not valid, check secondary to see if it is valid
        if (EFI_ERROR(ValidateGptHeader(BlockIo, Secondary, BlockIo->Media->LastBlock))) {
            pBS->FreePool(Pmbr);
            pBS->FreePool(Primary);
            pBS->FreePool(Secondary);
            return EFI_UNSUPPORTED;
        }
        
        // if secondary table is good, copy to primary
        else
            pBS->CopyMem(Gpt, Secondary, sizeof(GPT_HEADER));
    } else {
        // if secondary is bad, copy primary to secondary
        pBS->CopyMem(Gpt, Primary, sizeof(GPT_HEADER));
    }

    // before we exit free all memory allocation and exit with success
    pBS->FreePool(Pmbr);
    pBS->FreePool(Primary);
    pBS->FreePool(Secondary);

    return EFI_SUCCESS;
}

/**
    Checks Block Io in the Gpt Header

    @param BlockIo pointer to the BlockIo partition

    @retval VOID
    @retval GPT_PARTITION_INFO*

**/
GPT_PARTITION_INFO*
CheckBlockIoInGptRecord (
    IN  EFI_BLOCK_IO_PROTOCOL   *BlockIo
)
{
    GPT_PARTITION_INFO *TempRecord = OUTTER(GptBlockIOList.pHead, Link, GPT_PARTITION_INFO);

    while ( TempRecord != NULL) {
        if(TempRecord-> BlockIoAdd == (UINTN)BlockIo) {
            return TempRecord;
        }  
        // go to the next element in the list
        TempRecord = OUTTER(TempRecord->Link.pNext, Link, GPT_PARTITION_INFO);
    }

    return NULL;
}


/**
    This function detects GPT partition and  Block range is saved
    only for the Physical BlockIO and Write enabled device.

    @param Event 
    @param Context 

    @retval VOID

**/
VOID
EFIAPI
FindGptInBlockIO (
    IN  EFI_EVENT   Event,
    IN  VOID        *Context
)
{
    EFI_STATUS              Status;
    UINTN                   i;
    GPT_HEADER              *Gpt = NULL;
    UINTN                   NumberofHandles = 0;
    EFI_HANDLE              *BlockIoHandles = 0;
    EFI_BLOCK_IO_PROTOCOL   *BlockIo;
    GPT_PARTITION_INFO      *GptBlockRecord = NULL;

    //
    // Call back on BlockIo Protocol
    //
    Status = pBS->LocateHandleBuffer( ByRegisterNotify,
                                      NULL,
                                      gGptWriteNotifyReg,
                                      &NumberofHandles,
                                      &BlockIoHandles );

    if ( EFI_ERROR( Status ) || BlockIoHandles == 0 || NumberofHandles == 0 )
        return ;

    // Allocate memory for a valid GPT partition header
    Status = pBS->AllocatePool(EfiBootServicesData,
                               sizeof(GPT_HEADER),
                               (VOID**)&Gpt);

    if (EFI_ERROR(Status)) {
        return ;
    }

    for ( i = 0; i != NumberofHandles; i++ ) {
        Status = pBS->HandleProtocol( BlockIoHandles[i],
                                      &gEfiBlockIoProtocolGuid,
                                      (VOID**)&BlockIo );
        if ( EFI_ERROR( Status )) {
            continue;
        }

        // We want to remove any Block I/O instances that refer to Logical partitions.
        // A Block I/O instance is added to the raw device and any partition discovered on
        // the media. This prevents duplications in our table.
        if (BlockIo->Media->LogicalPartition) {
            continue;
        }

        if (BlockIo->Media->ReadOnly) {
            continue;
        }

        // verify that the GPT headers on this BlockIo device are valid
        if (EFI_ERROR(VerifyPartionTables(BlockIo,Gpt )) ) {
            continue ;
        }

        // Check the BlockIo entry in the existing List
        GptBlockRecord=CheckBlockIoInGptRecord(BlockIo);

        if(GptBlockRecord == NULL) {
            // Create database record and add to database
            Status = pBS->AllocatePool (EfiBootServicesData,
                                        sizeof (GPT_PARTITION_INFO),
                                        (VOID**)&GptBlockRecord);

            if(EFI_ERROR(Status)) {
                continue;
            }

            DListAdd (&GptBlockIOList, &(GptBlockRecord->Link));
        }

        // Update the GPT information Record
        GptBlockRecord->BlockIoAdd = (UINTN )BlockIo;
        GptBlockRecord->FirstUsableLBA = Gpt->FirstUsableLba;
        GptBlockRecord->LastUsableLBA = Gpt->LastUsableLba;
    }

    pBS->FreePool(Gpt);

    return;  
}

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
