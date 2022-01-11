//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2012, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

//**********************************************************************
// $Header: $
//
// $Revision: $
//
// $Date: $
//**********************************************************************
// Revision History
// ----------------
// $Log: $
// 
// 
//**********************************************************************
/** @file Meud.c

**/
//**********************************************************************
#include "Efi.h"
#include "Token.h"
#include <Library/DebugLib.h>
#include <AmiLib.h>
#include <AmiDxeLib.h>
#include "Meud.h"
#include "../Ofbd.h"
#include <Protocol/SmiFlash.h>
#include <Flash.h>

extern FLASH_REGIONS_DESCRIPTOR FlashRegionsDescriptor[];

EFI_SMI_FLASH_PROTOCOL *mSmiFlash;
UINT32 FlashCapacity;

/**
    Locate SmiFlash protocol callback

    @param Event
    @param Context

    @retval VOID

**/
#if defined(PI_SPECIFICATION_VERSION) && (PI_SPECIFICATION_VERSION >= 0x0001000A)
EFI_STATUS
EFIAPI
MEUDCallback(
    CONST EFI_GUID  *Protocol,
    VOID            *Interface,
    EFI_HANDLE      Handle
)
#else
VOID
MEUDCallback(
    EFI_EVENT   Event,
    VOID        *Context
)
#endif
{
    EFI_GUID    EfiSmiFlashProtocolGuid = EFI_SMI_FLASH_GUID;
    EFI_STATUS  Status;

#if (PI_SPECIFICATION_VERSION >= 0x1000A)
    Status = pSmst->SmmLocateProtocol( &EfiSmiFlashProtocolGuid, NULL, &mSmiFlash );
    return EFI_SUCCESS;
#else
    Status = pBS->LocateProtocol( &EfiSmiFlashProtocolGuid, NULL, &mSmiFlash );
    if( EFI_ERROR(Status) ) mSmiFlash = NULL:
#endif
}
/**
    OFBD ME Firmware Update InSmm Function

    @param VOID

    @retval VOID

**/
VOID
MeudInSmm(
    VOID
)
{
    EFI_STATUS              Status;
    EFI_GUID                EfiSmmBase2ProtocolGuid = EFI_SMM_BASE2_PROTOCOL_GUID;
    EFI_GUID                EfiSmiFlashProtocolGuid = EFI_SMI_FLASH_GUID;
    EFI_SMM_BASE2_PROTOCOL  *SmmBase2;
    EFI_SMM_SYSTEM_TABLE2   *pSmst = NULL;

    FlashCapacity = GetFlashCapacity();

    Status = pBS->LocateProtocol( &EfiSmmBase2ProtocolGuid, NULL, &SmmBase2 );
    DEBUG((DEBUG_INFO,"The Status of locate protocol(EfiSmmBase2ProtocolGuid) is %r\n", Status));
    SmmBase2->GetSmstLocation( SmmBase2, &pSmst );

#if (PI_SPECIFICATION_VERSION >= 0x1000A)
    Status = pSmst->SmmLocateProtocol( &EfiSmiFlashProtocolGuid, NULL, &mSmiFlash );
#else
    Status = pBS->LocateProtocol( &EfiSmiFlashProtocolGuid, NULL, &mSmiFlash );
#endif

    if( EFI_ERROR(Status) )
    {
#if defined(PI_SPECIFICATION_VERSION) && (PI_SPECIFICATION_VERSION >= 0x0001000A)
        VOID    *ProtocolNotifyRegistration;
        Status = pSmst->SmmRegisterProtocolNotify(
                            &EfiSmiFlashProtocolGuid,
                            MEUDCallback,
                            &ProtocolNotifyRegistration );
#else
        EFI_EVENT   SmiFlashCallbackEvt;
        VOID        *MEUDReg;
        RegisterProtocolCallback(
            &EfiSmiFlashProtocolGuid,
            MEUDCallback,
            NULL,
            &SmiFlashCallbackEvt,
            &MEUDReg );
#endif
    }
    CspMeudInSmm();
}
/**
    OFBD ME Firmware Update Entry point

    @param Buffer
    @param pOFBDDataHandled

    @retval VOID

**/
VOID
MeudEntry(
    IN      VOID    *Buffer,
    IN  OUT UINT8   *pOFBDDataHandled
)
{
    OFBD_HDR                        *pOFBDHdr;
    OFBD_EXT_HDR                    *pOFBDExtHdr;
    OFBD_TC_55_MEUD_STRUCT          *MEUDStructPtr;
    EFI_STATUS                      Status = EFI_SUCCESS;
    UINT32                          Buffer1, Buffer2;
    OFBD_TC_55_ME_INFO_STRUCT       *MEInfoStructPtr;
    OFBD_TC_55_ME_PROCESS_STRUCT    *MEProcessStructPtr;

#if (OFBD_VERSION >= 0x0210)
    static UINT8    MacAddr[8];
    UINT32          Length, Offset;
    BOOLEAN         FlashStatus = TRUE;
    UINT8           *Address;
    UINT32          Size;
    UINT8           *Data;
#endif

    pOFBDHdr = (OFBD_HDR*)Buffer;
    pOFBDExtHdr = (OFBD_EXT_HDR*)((UINT8*)Buffer + \
                  (pOFBDHdr->OFBD_HDR_SIZE));
    MEUDStructPtr = (OFBD_TC_55_MEUD_STRUCT*)((UINT8*)pOFBDExtHdr + \
                    sizeof(OFBD_EXT_HDR));
    MEInfoStructPtr = (OFBD_TC_55_ME_INFO_STRUCT*)MEUDStructPtr;
    MEProcessStructPtr = (OFBD_TC_55_ME_PROCESS_STRUCT*)MEUDStructPtr;

    if( *pOFBDDataHandled == 0 )
    {
        if( pOFBDHdr->OFBD_FS & OFBD_FS_MEUD )
        {
            //Check Type Code ID
            if( pOFBDExtHdr->TypeCodeID == OFBD_EXT_TC_MEUD )
            {
                switch( MEUDStructPtr->bSubFunction )
                {
                    case 0 :
                        Status = CspReportMeInfo( 0 , \
                                    &(MEUDStructPtr->dMERuntimeBase), \
                                    &(MEUDStructPtr->dMERuntimeLength) );

                        MEUDStructPtr->dMEBiosRegionBase = \
                                                    FlashCapacity - FLASH_SIZE;
                        MEUDStructPtr->dMEBiosRegionLength = (UINT32)FLASH_SIZE;

                        if( FlashCapacity == 0 )
                        {
                            // Fail , Return
                            *pOFBDDataHandled = 0xFE;
                            return;
                        }
                        *pOFBDDataHandled = 0xFF;
                        MEUDStructPtr->bReturnStatus = OFBD_TC_MEUD_OK;
                        return;
                    case 3 :
                        Status = CspReportMeInfo( 3, \
                                    &(MEUDStructPtr->dMERuntimeBase), \
                                    &(MEUDStructPtr->dMERuntimeLength) );

                        MEUDStructPtr->dMEBiosRegionBase = \
                                                    FlashCapacity - FLASH_SIZE;
                        MEUDStructPtr->dMEBiosRegionLength = (UINT32)FLASH_SIZE;

                        // If FlashCapacity == 0, Not support ME Update
                        if( FlashCapacity == 0 )
                        {
                            // Fail , Return
                            *pOFBDDataHandled = 0xFE;
                            return;
                        }

                        *pOFBDDataHandled = 0xFF;
                        MEUDStructPtr->bReturnStatus = OFBD_TC_MEUD_OK;
                        return;
                    case 1 :
                        // Send this again to check is this suppoet.
                        Status = CspReportMeInfo( 0, \
                                    &Buffer1, \
                                    &Buffer2 );
                        if( !EFI_ERROR(Status) )
                            Status = HmrfpoEnableMsg();

                        if( EFI_ERROR(Status) )
                        {
                            // Fail , Return
                            *pOFBDDataHandled = 0xFE;
                            return;
                        }
                    case 4 :
                        // Send this again to check is this suppoet.
                        Status = CspReportMeInfo( 3, \
                                    &Buffer1, \
                                    &Buffer2 );
                        if( EFI_ERROR(Status) )
                        {
                            // Fail , Return
                            *pOFBDDataHandled = 0xFE;
                            return;
                        }
                        MEUDStructPtr->bReturnStatus = OFBD_TC_MEUD_OK;
                        *pOFBDDataHandled = 0xFF;
                        mSmiFlash->FlashCapacity = FlashCapacity;
#if (OFBD_VERSION >= 0x0210)
                        // Get GBE Region for Restore MAC address
                        Offset = 0;
                        Length = 0;
                        Status = GetRegionOffset( 3, &Offset, &Length );
                        if( !EFI_ERROR(Status) && (Length != 0) )
                        {
                            // Before Erase, Get mac address
                            Address = (UINT8*)FLASH_BASE_ADDRESS(Offset);
                            FlashRead( Address, MacAddr, 6 );
                        }
#endif
                        return;
                    case 2 :
                        Status = HmrfpoLockMsg();
                        if( EFI_ERROR(Status) )
                        {
                            // Fail , Return
                            *pOFBDDataHandled = 0xFE;
                            return;
                        }
                    case 5 :
                        mSmiFlash->FlashCapacity = FLASH_SIZE;
                        MEUDStructPtr->bReturnStatus = OFBD_TC_MEUD_OK;
                        *pOFBDDataHandled = 0xFF;
                        return;

                    default :
                        *pOFBDDataHandled = 0xFE;
                        break;
// =============== OFBD Version 2.1 and AFU version 2.37 or newer ===============
#if (OFBD_VERSION >= 0x0210)
                    // earse
                    case 6 :

                        Size = MEUDStructPtr->ddBlockSize;
                        Status = CspReportMeInfo( 3, \
                                    &Buffer1,
                                    &Buffer2 );

                        // Return if Update Block address is over our expect
                        if (MEUDStructPtr->ddBlockAddr > Buffer2 )
                        {
                            *pOFBDDataHandled = 0xFF;
                            MEUDStructPtr->bReturnStatus = OFBD_TC_MEUD_OK;
                            return;
                        }

                        Address = (UINT8*)FLASH_BASE_ADDRESS(MEUDStructPtr->ddBlockAddr);
                        // Skip the address not reported by CSP_ReportMEInf
                        if( Buffer1 > MEUDStructPtr->ddBlockAddr )
                            Address = (UINT8*)FLASH_BASE_ADDRESS(Buffer1);

                        //FlashDeviceWriteEnable();

                        for( ; FlashStatus && (Size > 0); Address += FlashBlockSize, Size -= FlashBlockSize )
                        {
                            FlashBlockWriteEnable( Address );
                            FlashStatus = FlashEraseBlock( Address );
                            FlashBlockWriteDisable( Address );
                        }
                        //FlashDeviceWriteDisable();
                        if( FlashStatus )
                        {
                            *pOFBDDataHandled = 0xFF;
                            MEUDStructPtr->bReturnStatus = OFBD_TC_MEUD_OK;
                        }
                        break;
                    // program
                    case 7 :
                        Size = MEUDStructPtr->ddBlockSize;
                        Data = (UINT8*)pOFBDHdr;
                        Status = CspReportMeInfo( 3, \
                                    &Buffer1,
                                    &Buffer2 );

                        Offset = 0;
                        Length = 0;
                        Status = GetRegionOffset( 3, &Offset, &Length );
                        if( EFI_ERROR(Status) )
                        {
                            // Fail, Return
                            *pOFBDDataHandled = 0xFE;
                            return;
                        }

                        if( (MEUDStructPtr->ddBlockAddr > Buffer2) ||
                            (Buffer1 > MEUDStructPtr->ddBlockAddr) )
                        {
                            *pOFBDDataHandled = 0xFF;
                            MEUDStructPtr->bReturnStatus = OFBD_TC_MEUD_OK;
                            return;
                        }

                        Data += MEUDStructPtr->ddFlashBufOffset;
                        Address = (UINT8*)FLASH_BASE_ADDRESS(MEUDStructPtr->ddBlockAddr);
                        // Restore MAC Address
                        if( (MEUDStructPtr->ddBlockAddr == Offset) && (Length != 0) )
                            MemCpy( Data, MacAddr, 6 );

                        //FlashDeviceWriteEnable();

                        FlashBlockWriteEnable( Address );
                        FlashStatus = FlashProgram( Address, Data, Size );
                        FlashBlockWriteDisable( Address );

                        //FlashDeviceWriteDisable();
                        if( FlashStatus )
                        {
                            *pOFBDDataHandled = 0xFF;
                            MEUDStructPtr->bReturnStatus = OFBD_TC_MEUD_OK;
                        }
                        break;

                    // Read
                    case 8 :
                        Address = (UINT8*)FLASH_BASE_ADDRESS(MEUDStructPtr->ddBlockAddr);
                        Data = (UINT8*)pOFBDHdr;
                        Data += MEUDStructPtr->ddFlashBufOffset;

                        FlashRead( Address, Data, MEUDStructPtr->ddBlockSize );
                        *pOFBDDataHandled = 0xFF;
                        MEUDStructPtr->bReturnStatus = OFBD_TC_MEUD_OK;
                        break;

                    case 9 :
                    // Get Info
                    {
                        UINT8    Index = 0;
                        UINT16   TotalBlocks = 0;

                        //Search for all regions
                        for( Index = 0; Index < MAX_BLK; Index++ )
                        {
                            if( FlashRegionsDescriptor[Index].FlashRegion == MAX_BLK )
                                break;

                            Status = GetRegionOffset(
                                        FlashRegionsDescriptor[Index].FlashRegion,
                                        &MEInfoStructPtr->BlockInfo[TotalBlocks].StartAddress,
                                        &MEInfoStructPtr->BlockInfo[TotalBlocks].BlockSize );
                            if( !EFI_ERROR(Status) )
                            {
                                UINT8   String[32];

                                MemCpy(
                                    MEInfoStructPtr->BlockInfo[TotalBlocks].Command,
                                    FlashRegionsDescriptor[Index].Command,
                                    4 );

                                MemCpy( &String[0], "Flash ", 6 );
                                MemCpy( &String[6], FlashRegionsDescriptor[Index].Command, 4 );
                                MemCpy( &String[10], " Region", 8 );

                                MemCpy(
                                    MEInfoStructPtr->BlockInfo[TotalBlocks].Description,
                                    String,
                                    18 );

                                MEInfoStructPtr->BlockInfo[TotalBlocks].Type = FlashRegionsDescriptor[Index].FlashRegion;

                                // Status = 1 means Protect
                                MEInfoStructPtr->BlockInfo[TotalBlocks].Status = 0;

                                TotalBlocks += 1;
                            }
                        }
                        //
                        // Fill SPS Partition Info
                        //
#if defined(OPR1_LENGTH)
                        // OPR1
                        MEInfoStructPtr->BlockInfo[TotalBlocks].StartAddress = OPR1_START;
                        MEInfoStructPtr->BlockInfo[TotalBlocks].BlockSize = OPR1_LENGTH;

                        // OPR2
                        if( OPR2_LENGTH != 0 )
                            MEInfoStructPtr->BlockInfo[TotalBlocks].BlockSize += OPR2_LENGTH;

                        MemCpy( MEInfoStructPtr->BlockInfo[TotalBlocks].Command,"OPR", 4 );
                        MemCpy( MEInfoStructPtr->BlockInfo[TotalBlocks].Description,
                                "Flash Operation Region of SPS", 64 );

                        MEInfoStructPtr->BlockInfo[TotalBlocks].Type = ME_OPR_BLK;
                        // Status = 1 means Protect
                        MEInfoStructPtr->BlockInfo[TotalBlocks].Status = 0;
                        TotalBlocks += 1;
#endif
                        MEInfoStructPtr->TotalBlocks = TotalBlocks;
                        *pOFBDDataHandled = 0xFF;
                        MEUDStructPtr->bReturnStatus = OFBD_TC_MEUD_OK;
                        break;
                    }

                    case 10 :
                        // ME Process Handle
                        // In CSP_MEUD.c
                        MEProcessHandler( &MEProcessStructPtr );
                        *pOFBDDataHandled = 0xFF;
                        MEUDStructPtr->bReturnStatus = OFBD_TC_MEUD_OK;
                        break;
#endif //#if (OFBD_VERSION >= 0x0210)
                }// End of Switch
            }
        }
    }
    return;
}
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2012, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************