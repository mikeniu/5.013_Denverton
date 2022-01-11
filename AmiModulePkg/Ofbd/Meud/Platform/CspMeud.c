//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2018, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
/** @file CspMeud.c

    Meud related functions.
*/
#include <Uefi.h>
#include "Token.h"

#include <Protocol/SmmSwDispatch.h>

#include <Protocol/HeciSmm.h>
//#include <Protocol/SpsSmmHmrfpoProtocol.h>	

#include <Library/PciLib.h>

#include <Library/PchPcrLib.h>

#include <Sps.h>

#include <AmiLib.h>
#include <AmiDxeLib.h>
#include <Flash.h>
#include <Meud.h>

#include "CspMeud.h"

extern volatile UINT8   *gSPIBASE;

HECI_PROTOCOL           *SmmHeci = NULL;
//SPS_SMM_HMRFPO_PROTOCOL *mSpsSmmHmrfpo = NULL;	

UINT8   MacAddr[6];
UINT8   HMRFPO_STATUS;

EFI_PHYSICAL_ADDRESS   Phy_Address;
EFI_PHYSICAL_ADDRESS   OPRx_Buffer;
OFBD_TC_55_ME_PROCESS_STRUCT  *StructPtr;

UINT8   gMeStateBeforeFlashUpdate = 0xFF;

UINT64  Nounce;
UINT32  Factory_Base;
UINT32  Factory_Limit;

UINTN
EFIAPI
MicroSecondDelay (
  IN      UINTN                     MicroSeconds
  );

FLASH_REGIONS_DESCRIPTOR
FlashRegionsDescriptor[] =
{
    { FDT_BLK, "FDR" },
    { BIOS_BLK, "BIOS" },
    { PDR_BLK, "PDR" },
    { REGA_BLK, "IE" },
    { REGB_BLK, "GBEA" },
    { REGC_BLK, "GBEB" },
    { ME_BLK, "ME" },
    { MAX_BLK, "" }
};

/**
    Check Heci PCI device

    @param VOID

    @retval BOOLEAN
**/
BOOLEAN
IsHeciHidden(
    IN  VOID
)
{
    UINT32  D22F0FunctionDisable;

    PchPcrRead32(
        PID_PSF2,
        R_PCH_PCR_PSF2_T0_SHDW_HECI1_REG_BASE + R_PCH_PCR_PSFX_T0_SHDW_PCIEN,
        &D22F0FunctionDisable );
    return (D22F0FunctionDisable & B_PCH_PCR_PSFX_T0_SHDW_PCIEN_FUNDIS);
}
/**
    UnHide Heci PCI device

    @param VOID

    @retval BOOLEAN
**/
BOOLEAN
UnHideHeci(
    IN  VOID 
)
{
    if( IsHeciHidden() )
    {
        PchPcrAndThenOr32(
            PID_PSF2,
            R_PCH_PCR_PSF2_T0_SHDW_HECI1_REG_BASE + R_PCH_PCR_PSFX_T0_SHDW_PCIEN,
            (UINT8)~B_PCH_PCR_PSFX_T0_SHDW_PCIEN_FUNDIS,
            0 );
        return TRUE;
    }
    else
        return FALSE;
}
/**
    Hide Heci PCI device

    @param HeciHide

    @retval BOOLEAN
**/
HideHeci(
    IN  BOOLEAN HeciHide
)
{
    if( HeciHide )
    {
        PchPcrAndThenOr32(
            PID_PSF2,
            R_PCH_PCR_PSF2_T0_SHDW_HECI1_REG_BASE + R_PCH_PCR_PSFX_T0_SHDW_PCIEN,
            (UINT8)~0,
            B_PCH_PCR_PSFX_T0_SHDW_PCIEN_FUNDIS );
    }
}
/**
    Get Host Firmware Status pass to MEUD

    @param VOID

    @retval UINT32
**/
UINT32
GetHfs(
    IN  VOID
)
{
    UINT32  Hfs = 0;
    BOOLEAN HeciIsHide;

    HeciIsHide = UnHideHeci();

    Hfs = HeciPciRead32( 0x40 );
    if( Hfs == 0xFFFFFFFF )
        Hfs = 0;

    HideHeci( HeciIsHide );

    return Hfs;
}
/**
    Send HMRFPO MERESET by SpsSmmHmrfpoProtocol

    @param VOID

    @retval EFI_STATUS
**/
EFI_STATUS
HmrfpoMeReset(
    IN  VOID
)
{
    EFI_STATUS  Status;
    BOOLEAN     HeciIsHide;
    SPS_MEFS1   MeFs1;
    UINT32      Timeout;
    MKHI_MSG_HMRFPO_MERESET_REQ HmrfpoMeResetReq;
/*    
    if( mSpsSmmHmrfpo == NULL )
        return EFI_DEVICE_ERROR;
*/
    HeciIsHide = UnHideHeci();

//    Status = mSpsSmmHmrfpo->MeReset();
    
    HeciIsHide = UnHideHeci (); 


    HmrfpoMeResetReq.Mkhi.Data = 0x00000005;
    HmrfpoMeResetReq.Nonce = Nounce;

    Status = SmmHeci->SendMsg(HECI1_DEVICE, (UINT32*)&HmrfpoMeResetReq, sizeof(HmrfpoMeResetReq),
                                          SPS_CLIENTID_BIOS, SPS_CLIENTID_ME_MKHI);

    //
    // If Intel ME was not running in operational mode before 
    // flash update, return success.
    //        
    MeFs1.UInt32 = GetHfs();
    if ( gMeStateBeforeFlashUpdate == MeFs1.Bits.CurrentState ) {
        Status = EFI_SUCCESS;
        goto MERESET_END;
    }

    //
    // From ME-BIOS specification:
    // HMRFPO_MERESET message triggers Intel ME-only reset. The reset occurs immediately after the
    // HMRFPO_MERESET response is written to HECI, so if there was no error and the 
    // request has been accepted it is possible that BIOS will not have enough time to read 
    // the response. It is crucial to not repeat the request in such a case. MEFS1 register 
    // should be observed to confirm Intel ME is re-initializing.
    // So check for MEFS1.CurrentState to confirm that ME is re-initializing.
    // ME state transition: RECOVERY -> RESET -> INIT -> OPERATIONAL
    Timeout = SPS_RESET_TIMEOUT / STALL_1MS;
    MeFs1.UInt32 = GetHfs();
    
    while ( MeFs1.Bits.CurrentState != MEFS1_CURSTATE_INIT ) {
        if (!Timeout) {
            Status = EFI_DEVICE_ERROR;
            goto MERESET_END;
        } else if ( gMeStateBeforeFlashUpdate == MeFs1.Bits.CurrentState ) {
            Status = EFI_SUCCESS;
            goto MERESET_END;
        }

        MicroSecondDelay(STALL_1MS);
        MeFs1.UInt32 = GetHfs();
        Timeout--;
    }
    
    //
    // Wait for ME init complete. Timeout is 2 seconds.
    //
    Timeout = SPS_INIT_TIMEOUT / STALL_1MS;
    MeFs1.UInt32 = GetHfs();
    while ( !MeFs1.Bits.InitComplete ) {
        if (!Timeout) {
            Status = EFI_DEVICE_ERROR;
            goto MERESET_END;
        } else if ( gMeStateBeforeFlashUpdate == MeFs1.Bits.CurrentState ) {
            Status = EFI_SUCCESS;
            goto MERESET_END;
        }

        MicroSecondDelay(STALL_1MS);
        MeFs1.UInt32 = GetHfs();
        Timeout--;
    }
    //
    // Now, ME should be in normal operational State.
    //
    
    Timeout = SPS_NORMAL_TIMEOUT / STALL_1MS;
    MeFs1.UInt32 = GetHfs();
    while ( MeFs1.Bits.CurrentState != MEFS1_CURSTATE_NORMAL ) {
        if (!Timeout) {
            Status = EFI_DEVICE_ERROR;
            goto MERESET_END;
        } 
        
        MicroSecondDelay(STALL_1MS);
        MeFs1.UInt32 = GetHfs();
        Timeout--;
    }

MERESET_END:
    HideHeci( HeciIsHide );

    return Status;
}
/**
    Send HMRFPO ENABLE by SpsSmmHmrfpoProtocol

    @param VOID

    @retval EFI_STATUS
**/
EFI_STATUS
HmrfpoEnableMsg(
    IN  VOID
)
{
    EFI_STATUS  Status;
    UINT32      len;
    BOOLEAN     HeciIsHide;
    UINT8       Data8 = 0;
/*
    if( mSpsSmmHmrfpo == NULL )
        return EFI_DEVICE_ERROR;
*/
    HeciIsHide = UnHideHeci();
  
    
    //
    // We are required to handle 3 retries for HMRFPO_Enable.
    //
    for (Data8 = 3; Data8--; )
    {
      HeciMsg.HmrfpoEnableReq.Mkhi.Data = 0x00000105;
      HeciMsg.HmrfpoEnableReq.Nonce = Nounce;
      len = sizeof(HeciMsg.HmrfpoEnableRsp);
      Status = SmmHeci->SendwACK(HECI1_DEVICE, (UINT32*)&HeciMsg.HmrfpoEnableReq,
                               sizeof(HeciMsg.HmrfpoEnableReq), &len,
                               SPS_CLIENTID_BIOS, SPS_CLIENTID_ME_MKHI);
      if (!EFI_ERROR(Status))
      {
        if (HeciMsg.HmrfpoEnableRsp.Mkhi.Data != 0x00008105)
        {
//          TRACE((-1, "[SPS] ERROR: Invalid HMRFPO_ENABLE response (MKHI: 0x%08X), "
//                              "%d tries left\n",
//                              HeciMsg.HmrfpoEnableRsp.Mkhi.Data, Data8));
          continue;
        }
//        if (HeciMsg.HmrfpoEnableRsp.Status != 0)
//        {
//          TRACE((-1, "[SPS] ERROR: HMRFPO_ENABLE refused (cause: %d)\n",
//                              HeciMsg.HmrfpoEnableRsp.Status));
//        }
        break;
      }
//      TRACE((-1, "[SPS] HMRFPO_ENABLE send failed (%r), %d tries left\n",
//                          Status, Data8));
    }

//    Status = mSpsSmmHmrfpo->Enable();

    HideHeci( HeciIsHide );

    return Status;
}
/**
    Send HMRFPO LOCK by SpsSmmHmrfpoProtocol

    @param VOID

    @retval EFI_STATUS
**/
EFI_STATUS
HmrfpoLockMsg(
    IN  VOID
)
{
    EFI_STATUS  Status;
    BOOLEAN     HeciIsHide;
    UINT32      len;
    static BOOLEAN  NonceObtained = FALSE;     
    
/*
    if( mSpsSmmHmrfpo == NULL )
        return EFI_DEVICE_ERROR;
*/
    HeciIsHide = UnHideHeci();

//    Status = mSpsSmmHmrfpo->Lock();
    // Clear Status

    HeciMsg.HmrfpoLockReq.Mkhi.Data = 0x00000205;
    len = sizeof(HeciMsg.HmrfpoLockRsp);
    Status = SmmHeci->SendwACK(HECI1_DEVICE, (UINT32*)&HeciMsg.HmrfpoLockReq,
                        sizeof(HeciMsg.HmrfpoLockReq), &len,
                        SPS_CLIENTID_BIOS, SPS_CLIENTID_ME_MKHI);


    HMRFPO_STATUS = HeciMsg.HmrfpoLockRsp.Status;

    if ((Status == EFI_SUCCESS) &&
            (HeciMsg.HmrfpoLockRsp.Mkhi.Data == \
            0x00008205) && \
            (HeciMsg.HmrfpoLockRsp.Status == 0))
    {
        if(NonceObtained == FALSE)
        {
            Nounce = HeciMsg.HmrfpoLockRsp.Nonce;
            NonceObtained = TRUE;
        }
            
    }

    HideHeci( HeciIsHide );

    return Status;
}
/**
    Mapping AMI defined region type to EDS defined

    @param AmiRegDef

    @retval UINT8
**/
UINT8
RegionMapping(
    IN  UINT8   AmiRegDef
)
{
    UINT8 EdsRegion;

    switch( AmiRegDef )
    {
        //Mapping old definition to region number
        case PDR_BLK:
            EdsRegion = 4;
        break;

        case GBE_BLK:
            EdsRegion = 3;
        break;

        case ME_BLK:
            EdsRegion = 2;
        break;

        case ME_OPR_BLK:
            //This type is only defined for SPS
            EdsRegion = 0xFF;
        break;

        case BIOS_BLK:
            EdsRegion = 1;
        break;

        case DER_BLK:
            EdsRegion = 5;
        break;
            
        case BIOS_2ND_BLK:
            EdsRegion = 6;
        break;

        default:
            EdsRegion = AmiRegDef;
        break;
    }

    return EdsRegion;
}
/**
    Find index of FlashRegionsDescriptor

    @param AmiRegDef AMI defined region type

    @retval UINT8 Return MAX_BLK if not found match index
**/
UINT8
FindDescriptorIndex(
    IN  UINT8   AmiRegDef
)
{
    UINT8   Index;
    UINT8   Regions;

    Regions = sizeof(FlashRegionsDescriptor) / sizeof(FLASH_REGIONS_DESCRIPTOR);
    for( Index = 0; Index < Regions; Index++ )
    {
        if( FlashRegionsDescriptor[Index].FlashRegion == AmiRegDef )
            return Index;
    }

    return MAX_BLK;
}
/**
    Read the FLMSTR1 in descriptor for checking write access

    @param Region Region type

    @retval BOOLEAN TRUE region can be written, FALSE region can't be written
**/
BOOLEAN
CheckRegionWriteAccess(
    IN  UINT8   Region
)
{
    static UINT32   Flmstr1 = 0;
    UINT32          Value32;
    UINT8           *Address;
    UINT8           EdsRegion;
    UINT8           Shift;

    //If FDOPSS bit in SPI address is not set, the access is accepted
    if( ((*(UINT32*)(gSPIBASE + 0x04)) & BIT13) == 0 )
        return TRUE;

    if( Flmstr1 == 0 )
    {
        //Read FLMAP1 for Fmba
        Address = (UINT8*)FLASH_BASE_ADDRESS(0x18);
        FlashRead( Address, (UINT8*)&Value32, sizeof(UINT32) );

        //Read FLMSTR1 for write access
        Address = (UINT8*)FLASH_BASE_ADDRESS(((Value32 & (0x000000FF)) << 0x04));
        FlashRead( Address, (UINT8*)&Value32, sizeof(UINT32) );

        Flmstr1 = Value32;
    }
    else
    {
        //Since the descriptor region may be changed in the update process,
        //it should read onboard FLMSTR1 only once.
        Value32 = Flmstr1;
    }

    //Me region register should be locked, but we will call HMRFPO to unlock
    //just retur true here.
    if( Region == ME_BLK )
        return TRUE;

    EdsRegion = RegionMapping( Region );

    if( (EdsRegion == 0xFF) || (EdsRegion == MAX_BLK) )
        return TRUE;
    
    if( EdsRegion >= 0x0C )
        Shift = EdsRegion - 0x08; //Region 12 to 15 is bit[4:7]
    else
        Shift = EdsRegion + 0x14; //Region 0 to 11 is bit[20:31]

    return (((Value32 >> Shift) & 0x01) == 0x01) ? TRUE : FALSE;
}
/**
    Compare the FREG register from SPI base and FLREG register
    in update image to determin wheter region is changed.
    If this function checks descriptor region, it will check all
    FREG and FLREG registers in order to avoiding region layout
    change.

    @param Region Region type

    @retval BOOLEAN TRUE region range is different, FALSE region range is same
**/
BOOLEAN
RegionRangeChanged(
    IN  UINT8   Region
)
{
    UINT8   EdsRegion;
    UINT8   Limit;
    UINT32  Offset;
    UINT32  Freg;
    UINT32  Flreg;

    EdsRegion = RegionMapping( Region );

    //Undefined region or OPR just assume not changed
    if( (EdsRegion == 0xFF) || (EdsRegion == MAX_BLK) )
        return FALSE;

    if( Region == FDT_BLK )
        Limit = 0x10;  //Check all registers if it's descriptor
    else
        Limit = EdsRegion + 0x01;  //Only check single region register

    for( EdsRegion; EdsRegion < Limit; EdsRegion++ )
    {
        //Get on-board FREG from SPI base
        if( EdsRegion >= 0x0C )
            //Region 12 to 15 is start from offset 0xE0
            Offset = 0xE0 + ((EdsRegion - 0x0C) * sizeof(UINT32));
        else
            //Region 0 to 11 is start from offset 0x54
            Offset = 0x54 + EdsRegion * sizeof(UINT32);

        Freg = *(UINT32*)(gSPIBASE + Offset);

        //Get FLREG from image's descriptor region
        Flreg = *(UINT32*)(Phy_Address + 0x40 + (EdsRegion * sizeof(UINT32)));

        if( Freg != Flreg )
            return TRUE;
    }

    return FALSE;
}
/**
    Get regions' offset and length by FDOC register

    @param Region
    @param Offset
    @param Length

    @retval EFI_STATUS
**/
EFI_STATUS
GetRegionOffset(
    UINT8   Region,
    UINT32  *Offset,
    UINT32  *Length
)
{
    volatile UINT32 *FDOC;
    volatile UINT32 *FDOD;
    UINT8           EdsRegion;
    UINT32          Buffer32;
    UINT32          RegionStart;
    UINT32          RegionEnd;

    InitializeSpiBase();

    FDOC = (UINT32*)(gSPIBASE + 0xB4); 
    FDOD = (UINT32*)(gSPIBASE + 0xB8);
    *FDOC = 0;

    if( *FDOD != FLASH_VALID_SIGNATURE )
        return EFI_UNSUPPORTED;

    EdsRegion = RegionMapping( Region );

    if( EdsRegion != 0xFF )
        *FDOC = (0x2000 + (EdsRegion * 0x04));
    else
        return EFI_UNSUPPORTED;

    Buffer32 = *FDOD;

    //If the region is unsued
    if( Buffer32 == 0x00007FFF )
        return EFI_UNSUPPORTED;

    RegionEnd = Buffer32 >> 16;
    RegionStart = Buffer32 & 0xFFFF;

    *Offset = RegionStart << 12;
    *Length = (RegionEnd - RegionStart + 1) << 12;

    return EFI_SUCCESS;
}
/**
    Get flash parts' capacity by FDOC register

    @param VOID

    @retval UINT32
**/
UINT32
GetFlashCapacity(
    IN  VOID
)
{
    volatile UINT32 *FDOC;
    volatile UINT32 *FDOD;
    UINT32          FlashDensity = 0;
    UINT16          Components;
    UINT8           i,j;

    InitializeSpiBase();

    FDOC = (UINT32*)(gSPIBASE + 0xB4);
    FDOD = (UINT32*)(gSPIBASE + 0xB8);

    *FDOC = 0;
    // Invalid !! Not Support ME
    if( *FDOD != FLASH_VALID_SIGNATURE )
        return 0;

    //Get flash part numbers
    *FDOC = 0x04;
    Components = (*FDOD >> 8) & 0x03;

    *FDOC = 0x1000;
    j = *FDOD;

    for( i = 0 ; i < ( Components + 1 ); i++ )
    {
        switch( j & 0x07 )
        {
            case 0:
                FlashDensity += 0x80000;
            break;
            case 1:
                FlashDensity += 0x100000;
            break;
            case 2:
                FlashDensity += 0x200000;
            break;
            case 3:
                FlashDensity += 0x400000;
            break;
            case 4:
                FlashDensity += 0x800000;
            break;
            case 5:
                FlashDensity += 0x1000000;
            break;
            case 6:
                FlashDensity += 0x2000000;
            break;
            case 7:
                FlashDensity += 0x4000000;
            break;
            default:
            break;
        }
#if BITS_OF_SPI_DENSITY
        j = j >> 4;
#else
        j = j >> 3;
#endif
    }

    return FlashDensity;
}
/**
    Report ME Base address and Length to AFU

    @param Func_Num
    @param BASE_Address
    @param Length

    @retval EFI_STATUS
**/
EFI_STATUS
CspReportMeInfo
(
    IN      UINT8   Func_Num,
    IN  OUT UINT32  *BASE_Address,
    IN  OUT UINT32  *Length
)
{
    // Fill these 2 item for Afu can update BIOS with whole image input
    *BASE_Address = 0;
    *Length = GetFlashCapacity() - FLASH_SIZE;

    switch( Func_Num )
    {
        case 0:
            // Command 0 is for Ignition FW, only ME6 support this.
            return EFI_UNSUPPORTED;
        break;
        case 3:
            // Always rteturn SUCCESS here !!
        break;
        default:
            return EFI_UNSUPPORTED;
        break;
    }

    return EFI_SUCCESS;
}
/**
    Send Enable and Global reset MSG to ME FW.

    @param DispatchHandle
    @param DispatchContext

    @retval VOID
**/
VOID
EFIAPI
MEUDSMIHandler(
    IN  EFI_HANDLE                  DispatchHandle,
    IN  EFI_SMM_SW_DISPATCH_CONTEXT *DispatchContext
)
{
    // This is not needed anymore, SPS can be disabled in runtime.
/*
    HmrfpoEnableMsg();
    SEND_MEI_GLOBAL_RESET_MSG();
*/
}
/**
    Locate Smm Heci protocol

    @param Protocol
    @param Interface
    @param Handle

    @retval EFI_STATUS
**/
EFI_STATUS
EFIAPI
LocateSmmHeciProtocol(
    CONST EFI_GUID  *Protocol,
    VOID            *Interface,
    EFI_HANDLE      Handle
)
{
    EFI_STATUS  Status;

    Status = pSmst->SmmLocateProtocol(
                        &gSmmHeciProtocolGuid,
                        NULL,
                        &SmmHeci );
    if( EFI_ERROR(Status) )
    {
        DEBUG((EFI_D_ERROR, "Smm Heci Protocol locate Status: %r\n", Status));
    }

    return Status;
}
/**
    Locate Smm Hmrfpo protocol

    @param Protocol
    @param Interface
    @param Handle

    @retval EFI_STATUS
**/
/*
EFI_STATUS
EFIAPI
LocateSmmHmrfpoProtocol(
    CONST EFI_GUID  *Protocol,
    VOID            *Interface,
    EFI_HANDLE      Handle
)
{
    EFI_STATUS  Status;

    Status = pSmst->SmmLocateProtocol(
                        &gSpsSmmHmrfpoProtocolGuid,
                        NULL,
                        &mSpsSmmHmrfpo );
    if( EFI_ERROR(Status) )
    {
        DEBUG((EFI_D_ERROR, "Smm Hmrfpo locate Status: %r\n", Status));
    }

    return Status;
}
*/

/**
    Send HMRFPO Lock command and HMRFPO Enable command

    @param Protocol
    @param Interface
    @param Handle

    @retval EFI_STATUS
**/
EFI_STATUS
EFIAPI
InitSmmHMRFPOLock(
    CONST EFI_GUID  *Protocol,
    VOID            *Interface,
    EFI_HANDLE      Handle
)
{
    EFI_STATUS  Status;
      
    Status = HmrfpoLockMsg();
        
    return Status;
}
/**
    Elink in OFBD entry

    @param  VOID
    @retval VOID
**/
VOID
CspMeudInSmm(
    IN  VOID
)
{
    EFI_STATUS  Status;
    UINT8       Timer = 0;
    VOID        *ProtocolNotifyRegistration;
    VOID        *RegistrationHMRFPOLock;

    //Locate the buffer for saving non-BIOS region image from AFU
    // Denverton AptioV override Start - EIP#333995
    // To solve:  CPU Exception when using AFU to flash ME image with >16MB flash part
       Status = pBS->AllocatePages(AllocateAnyPages, EfiRuntimeServicesData,
                                   EFI_SIZE_TO_PAGES(GetFlashCapacity() - FLASH_SIZE), &Phy_Address);
    // Denverton AptioV override End - EIP#333995
    if( EFI_ERROR(Status) )
        return;

    //Locate the buffer for saving OPR partiton in ME region
    Status = pBS->AllocatePages(
                    AllocateAnyPages,
                    EfiRuntimeServicesData,
                    EFI_SIZE_TO_PAGES(RT_OPR_BUFFER),
                    &OPRx_Buffer );
    if( EFI_ERROR(Status) )
        return;

    Status = pSmst->SmmLocateProtocol(
                        &gSmmHeciProtocolGuid,
                        NULL,
                        &SmmHeci );
    if( EFI_ERROR(Status) )
    {
        DEBUG((EFI_D_ERROR, "Smm Heci Protocol locate Status: %r\n", Status));
        Status = pSmst->SmmRegisterProtocolNotify(
                            &gSmmHeciProtocolGuid,
                            LocateSmmHeciProtocol,
                            &ProtocolNotifyRegistration );
    }
/*
    Status = pSmst->SmmLocateProtocol(
                        &gSpsSmmHmrfpoProtocolGuid,
                        NULL,
                        &mSpsSmmHmrfpo );
    if( EFI_ERROR(Status) )
    {
        ProtocolNotifyRegistration = NULL;
        Status = pSmst->SmmRegisterProtocolNotify(
                            &gSpsSmmHmrfpoProtocolGuid,
                            LocateSmmHmrfpoProtocol,
                            &ProtocolNotifyRegistration );
    }
*/

    Status = pSmst->SmmRegisterProtocolNotify(
                        &gEfiSmmEndOfDxeProtocolGuid,
                        InitSmmHMRFPOLock,
                        &RegistrationHMRFPOLock );
    
    return;
}
/**
    Handle ME Process

    @param Result
    @param Message

    @retval VOID
**/
VOID
MEProcessHandleResult(
    IN  UINT16  Result,
    IN  CHAR8   *Message
)
{
    StructPtr->UpdateResult = Result;
    MemCpy( (UINT8*)(StructPtr->ddMessageBuffer), Message, Strlen(Message) );

    *(CHAR8*)(StructPtr->ddMessageBuffer + Strlen(Message)) = 0;
}
/**
    Send Hmrfpo enable message

    @param InSmm

    @retval EFI_STATUS
**/
EFI_STATUS
SendEnableMessage(
    IN  BOOLEAN InSmm
)
{
    UINT8       Counter;
    EFI_STATUS  Status;

    if( gMeStateBeforeFlashUpdate == 0xFF )
    {
        gMeStateBeforeFlashUpdate = (0x0F & GetHfs());
    }

    if( InSmm )
    {
        Counter = 0;
        do
        {
            Status = HmrfpoEnableMsg();
            if( !EFI_ERROR(Status) )
                break;
            Counter++;
            SmmHeci->ReInitHeci( HECI1_DEVICE );
        }while( Counter < 3 );
        GetHfs();
    }
    else
    {
        UINTN       VariableSize = 1;
        EFI_GUID    gMEUDErrorguid = MEUD_ERROR_GUID;

        IoWrite8( 0xB2, Disable_ME_SW_SMI );

        // Get Status
        Status = pRS->GetVariable(
                        L"ENABLESTATUS",
                        &gMEUDErrorguid,
                        NULL,
                        &VariableSize,
                        &HMRFPO_STATUS );

        // Error, if variable not exist
        if( Status == EFI_NOT_FOUND )
            return Status;

        return EFI_SUCCESS;
    }

    return Status;
}
/**
    Init the Length and Offset need to be updated. If needed, send ENABLE MESSAGE

    @param ProgramOffset
    @param ProgramLength
    @param Step
    @param InSmm

    @retval EFI_STATUS
**/
EFI_STATUS
HandleBuffer(
    IN  OUT UINT32  *ProgramOffset,
    IN  OUT UINT32  *ProgramLength,
    IN  OUT UINT8   *Step,
    IN      BOOLEAN InSmm
)
{
    EFI_STATUS  Status;
    UINT32      Offset;
    UINT32      Length;
    UINT32      Hfs = GetHfs();

    switch( StructPtr->bBlockType )
    {
        case ME_BLK:
// Denverton SPS does not set BIT09 to denote ME FW initial done.
            if(0)// !(Hfs & BIT09) )
            {
                MEProcessHandleResult(
                    BIT03,
                    "ME FW not Initial done" );

                return EFI_NOT_READY;
            }
            Status = GetRegionOffset( StructPtr->bBlockType, &Offset, &Length );
            //If Fpt bad or recovery load fault
            if((Hfs & BIT05) || (Hfs & BIT10))
                *Step = UpdateMfsb;
            else
                *Step = UpdateFtprToOpr;
        break;

        case GBE_BLK:
            Status = GetRegionOffset( StructPtr->bBlockType, &Offset, &Length );
            if( Status == EFI_NOT_FOUND )
                return EFI_UNSUPPORTED;
            // Store Mac address
            if( Length )
            {
                UINT8   *Address = (UINT8*)FLASH_BASE_ADDRESS(Offset);
                UINT8   ByteOffset = 0x27;  //Shared Init Control Word (Word 0x13)
                UINT8   SharedInitHigh;

                FlashRead( (Address + ByteOffset), &SharedInitHigh, 1 );

                //Bit 15:14, if the valid field doesn't equal 10b the NVM data is not valid
                if( (SharedInitHigh & 0xC0) != 0x80 )
                {
                    //Other MAC will be in offset 0x2000
                    Address = (UINT8*)FLASH_BASE_ADDRESS(Offset * 2);
                }

                FlashRead( Address, MacAddr, 0x06 );
            }
            *Step = UpdateNonMeRegion;
        break;

// MAC information of two Ports is available in GBEA and two ports in GBEB.        
        case REGB_BLK:
        case REGC_BLK:
            Status = GetRegionOffset( StructPtr->bBlockType, &Offset, &Length );
            if( Status == EFI_NOT_FOUND )
                return EFI_UNSUPPORTED;

            if( Length )
            {
                UINT8   *Address = (UINT8*)FLASH_BASE_ADDRESS(Offset);
                Process10GbeMac( Address, Save10GbeMac );
            }
            *Step = 0;
        break;

        case ME_OPR_BLK:
        {
            if( (Hfs & 0xF) != 2 )
            {
                Status = SendEnableMessage(InSmm);
                if( EFI_ERROR(Status) || (HMRFPO_STATUS != 0) )
                {
                    MEProcessHandleResult(
                        (BIT03 | BIT00),
                        "Error when sending Enable Message to ME !!" );
                    return Status;
                }
            }
            Offset = OPR1_START;
            if( OPR2_LENGTH )
            {
/*
                // If dual image system
                // 3.8.2.3
                if( !InSmm )
                {
                    // Update Factory_Limit.
                    UINTN   VariableSize = sizeof(UINT32);

                    Status = pRS->GetVariable(
                                    L"Factory_Limit",
                                    &gSmmHeciProtocolGuid,
                                    NULL,
                                    &VariableSize,
                                    &Factory_Limit );
                    if( EFI_ERROR(Status) )
                        Factory_Limit = 0;
                }
                if( Factory_Limit == (FPT_LENGTH + MFSB_LENGTH + FTPR_LENGTH +
                                     MFS_LENGTH + OPR1_LENGTH) )
                    Offset = OPR2_START;
*/
            }
            Length = OPR1_LENGTH;
            *Step = UpdateNonMeRegion;
        }
        break;
        
        default:
            Status = GetRegionOffset( StructPtr->bBlockType, &Offset, &Length );
            if( EFI_ERROR(Status) )
                return EFI_UNSUPPORTED;
            *Step = UpdateNonMeRegion;
        break;
    }

    *ProgramOffset = Offset;
    *ProgramLength = Length;

    return EFI_SUCCESS;
}
/**
    Handle Program Length and Offset , Steps and Program Buffer

    @param ProgramOffset
    @param ProgramLength
    @param Step
    @param InSmm
    @param ProgramBuffer

    @retval EFI_STATUS
**/
EFI_STATUS
HnadleMEUpdateSteps(
    IN  OUT UINT32  *ProgramOffset,
    IN  OUT UINT32  *ProgramLength,
    IN      UINT8   Step,
    IN      BOOLEAN InSmm,
    IN  OUT UINT8   **ProgramBuffer
)
{
    static BOOLEAN          SmmPages = FALSE;
    EFI_PHYSICAL_ADDRESS    SmmBuffer;
    static UINTN            BufferPages;
    EFI_STATUS              Status;
    UINT32                  Offset;
    UINT32                  Length;

    if( Step == UpdateFtprToOpr )
    {
        //Program FTPR to OPR1 and OPR2
        //Send Enable to ME
        //Retry 3 times for Enable message.
        Status = SendEnableMessage( InSmm );
        if( EFI_ERROR(Status) || (HMRFPO_STATUS != 0) )
        {
            MEProcessHandleResult(
                (BIT03 | BIT02 | BIT00),
                "Error when sending Enable Message to ME !!" );
            return Status;
        }

        // Prepare buffer, the length and offset is defferent
        if( OPR2_LENGTH )
            BufferPages = EFI_SIZE_TO_PAGES(OPR1_LENGTH + OPR2_LENGTH);
        else
            BufferPages = EFI_SIZE_TO_PAGES(OPR1_LENGTH);

        if( InSmm && !SmmPages )
        {
            Status = pSmst->SmmAllocatePages(
                                AllocateAnyPages,
                                EfiRuntimeServicesData,
                                BufferPages,
                                &SmmBuffer );
            if( !EFI_ERROR(Status) )
            {
                SmmPages = TRUE;
                OPRx_Buffer = SmmBuffer;
            }
        }

        MemSet( (UINT8*)(OPRx_Buffer), EFI_PAGES_TO_SIZE(BufferPages), 0xFF );

        MemCpy(
            (UINT8*)(OPRx_Buffer),
            (UINT8*)(Phy_Address + FTPR_START),
            FTPR_LENGTH );

        // If OPR2 exist
        if( OPR2_LENGTH )
        {
            MemCpy(
                (UINT8*)(OPRx_Buffer + OPR1_LENGTH),
                (UINT8*)(Phy_Address + FTPR_START),
                FTPR_LENGTH );
        }

        // Update Buffer ,Length and Offset
        *ProgramBuffer = (UINT8*)OPRx_Buffer;

        Offset = OPR1_START;

        if( OPR2_LENGTH )
            Length = OPR1_LENGTH + OPR2_LENGTH;
        else
            Length = OPR1_LENGTH;
    }

    if( Step == UpdateMfsb )
    {
        //Update Mfsb
        Length = MFSB_LENGTH;
        Offset = MFSB_START;
        *ProgramBuffer = (UINT8*)(Phy_Address + Offset);
    }

    if( Step == UpdateFtpr )
    {
        //Update Ftpr
        Length = FTPR_LENGTH;
        Offset = FTPR_START;
        *ProgramBuffer = (UINT8*)(Phy_Address + Offset);
    }

    //Update factory data partitions
    if( Step == UpdatePartitions )
    {
        // Free the memory we allocate last time.
        if( SmmPages )
        {
            pSmst->SmmFreePages( OPRx_Buffer, BufferPages );
            SmmPages = FALSE;
        }

        // Enable again !!
        Status = SendEnableMessage( InSmm );

        if( EFI_ERROR(Status) || (HMRFPO_STATUS != 0) )
        {
            MEProcessHandleResult(
                (BIT03 | BIT02 | BIT00),
                "Error when sending Enable Message to ME !!");
            if( !EFI_ERROR(Status) )
                Status = EFI_WRITE_PROTECTED;
            return Status;
        }

        Offset = MFS_START;

        // Just take care of length
        Length = MFS_LENGTH;

#if defined(FPTB_LENGTH)
        Length+= (UINT32)FPTB_LENGTH;
#endif 
#if defined(IVB1_LENGTH)
        Length+= (UINT32)IVB1_LENGTH;
#endif
#if defined(IVB2_LENGTH)
        Length+= (UINT32)IVB2_LENGTH;
#endif
#if defined(BIS_LENGTH)
        Length+= (UINT32)BIS_LENGTH;
#endif
#if defined(FLOG_LENGTH)
        Length+= (UINT32)FLOG_LENGTH;
#endif
#if defined(UTOK_LENGTH)
        Length+= (UINT32)UTOK_LENGTH;
#endif
#if defined(DLMP_LENGTH)
        Length+= (UINT32)DLMP_LENGTH;
#endif

        *ProgramBuffer = (UINT8*)(Phy_Address + Offset);
    }

    //Update OPR1 and OPR2
    if( Step == UpdateOpr )
    {
        // Take care of length
        if( OPR2_LENGTH )
            Length = OPR1_LENGTH + OPR2_LENGTH;
        else
            Length = OPR1_LENGTH;

        Offset = OPR1_START;
        *ProgramBuffer = (UINT8*)(Phy_Address + Offset);
    }

    *ProgramOffset = Offset;
    *ProgramLength = Length;

    return EFI_SUCCESS;

}
/**
    Verifies if the device has been erased properly or if the current
    byte is the same as the byte to be written at this location

    @param pDestination Starting address of where the data will be written
    @param pSource Starting address of the data that is supposed to be written 
                    or "NULL" for checking erase completed
    @param Length Length of the data set to check

    @retval BOOLEAN
**/
static
BOOLEAN
IsEqualOrClean(
    IN  UINT8   *pDestination,
    IN  UINT8   *pSource,
    IN  UINT32  Length
)
{
    UINT8   Buffer[256];
    UINT8   Data;
    UINT32  TempLength = Length;
    UINT32  RemainBytes = Length;
    UINT32  i, j;

    for( i = 0; i < Length; TempLength = RemainBytes, i = (Length - RemainBytes) )
    {
        // Initial local read buffer.
        MemSet( Buffer, 256, 0 );
        // Read flash through SPI Cycle if SpiCycle is requested.
        FlashRead( pDestination + i, Buffer, 256 );
        RemainBytes -= 256;
        // Verify whether flash contents is the same as written Data or is clean.
        for( j = 0; j < (TempLength - RemainBytes); j++ )
        {
            Data = (pSource != NULL) ? *(pSource + i + j) : 0xFF;
            if( Buffer[j] != Data )
                return FALSE;    
        }
    }
    return TRUE;
}
/**
    Update region

    @param Buffer
    @param InSmm

    @retval EFI_STATUS
**/
EFI_STATUS
UpdateRegions(
    IN  UINT8   *Buffer,
    IN  BOOLEAN InSmm
)
{
    static UINT32   Offset;
    static UINT32   Length;
    UINT8           *Address;
    EFI_STATUS      Status;
    BOOLEAN         NeedToVerify = FALSE;
    static UINT8    Step = UpdateNonMeRegion;
    static BOOLEAN  NewRegion;
    UINTN           Counter = 0;
    static UINT8    RetryTimes = RETRY_OPR_COUNT;
    static UINT8*   ProgramBuffer;
    static BOOLEAN  WriteAccess;
    static BOOLEAN  RegionChanged;
    UINT8           Index;
    UINT8           Messages[100];   // Denverton AptioV override - EIP#378592

    // Prepare Offset and Length to be updated
    // If BIT02 , update buffer
    if( (StructPtr->bHandleRequest & BIT02) )
    {
        if( !InSmm )
        {
            (UINT8*)Phy_Address = Buffer;
            Status = pBS->AllocatePages(
                            AllocateAnyPages,
                            EfiRuntimeServicesData,
                            EFI_SIZE_TO_PAGES(RT_OPR_BUFFER),
                            &OPRx_Buffer );
        }

        Status = HandleBuffer( &Offset, &Length, &Step, InSmm );

        if( EFI_ERROR(Status) )
        {
            MEProcessHandleResult( BIT03, "UN SUPPORT TYPE" );
            return Status;
        }
        // Frist In
        NewRegion = TRUE;
        WriteAccess = FALSE;
        RegionChanged = FALSE;
        ProgramBuffer = (UINT8*)(Phy_Address + Offset);
    }

    if( ((StructPtr->bBlockType) == GBE_BLK) && NewRegion )
    {
        UINT16  *GBEPtr;
        UINT16  i, CheckSum16;

        // Set MAC address to buffer
        MemCpy( (Buffer + Offset), MacAddr, 0x06 );

        //Calculate the checksum
        GBEPtr = (UINT16*)(Buffer + Offset);
        CheckSum16 = 0;
        // GBE checksum is from word offset 0x00 to 0x3F
        for( i = 0; i < 0x3F; i++ )
            CheckSum16 += GBEPtr[i];
        // Checksum should be 0xBABA, also include itself.
        CheckSum16 = 0xBABA - CheckSum16;
        // Checksum word is at offset 0x3F;
        MemCpy( &GBEPtr[0x3F], &CheckSum16, sizeof(UINT16) );
    }

    // MAC information of two Ports is available in GBEA and two ports in GBEB.
    if((((StructPtr->bBlockType) == REGB_BLK) || ((StructPtr->bBlockType) == REGC_BLK)) && NewRegion)
    {
        VOID    *Region10Gbe;
        Region10Gbe = (VOID*)(Buffer + Offset);
        Process10GbeMac( Region10Gbe, Restore10GbeMac );
    }

    // Handle Step
    if( (StructPtr->bBlockType == ME_BLK) && NewRegion )
    {
        Status = HnadleMEUpdateSteps( &Offset, &Length, Step, InSmm, &ProgramBuffer );
        if( EFI_ERROR(Status) )
            return Status;
    }

    if( NewRegion )
    {
        NewRegion = FALSE;
        WriteAccess = CheckRegionWriteAccess( StructPtr->bBlockType );
        RegionChanged = RegionRangeChanged( StructPtr->bBlockType );
    }

    Address = (UINT8*)FLASH_BASE_ADDRESS(Offset);

    if( WriteAccess == TRUE )
    {
        FlashBlockWriteEnable( Address );
        if( IsEqualOrClean( Address, NULL, FLASH_BLOCK_SIZE ) )
        {
            //The flash content is clean
            FlashProgram( Address, ProgramBuffer, FLASH_BLOCK_SIZE );
            if( IsEqualOrClean( Address, ProgramBuffer, FLASH_BLOCK_SIZE ) )
                Status = EFI_SUCCESS;
            else
                Status = EFI_DEVICE_ERROR;
        }
        else
        {
            //The flash content is not clean
            if( IsEqualOrClean( Address, ProgramBuffer, FLASH_BLOCK_SIZE ) )
            {
                //The flash content is same
                Status = EFI_SUCCESS;
            }
            else
            {
                //The flash content is different
                FlashEraseBlock( Address );
                FlashProgram( Address, ProgramBuffer, FLASH_BLOCK_SIZE );
                if( IsEqualOrClean( Address, ProgramBuffer, FLASH_BLOCK_SIZE ) )
                    Status = EFI_SUCCESS;
                else
                    Status = EFI_DEVICE_ERROR;
            }
        }
        FlashBlockWriteDisable( Address );
    }
    else
    {
        //The region is locked.
        Index = FindDescriptorIndex( StructPtr->bBlockType );

        if( RegionChanged == TRUE )
        {
            //The region range changed, there should be layout changed.
            //Force to stop whole update process.
            if( Index != MAX_BLK )
            {
                Sprintf(
                    Messages,
                    "%s is locked and layout changed, block updating",
                    FlashRegionsDescriptor[Index].Command );
            }
            else
                Sprintf( Messages, "There is locked region and layout changed, block updating.");

            //Show the message and block whole updating process
            MEProcessHandleResult( (BIT00 | BIT02), Messages );
        }
        else
        {
            if( Index != MAX_BLK )
                Sprintf( Messages, "%s is locked, skip updating.", FlashRegionsDescriptor[Index].Command );
            else
                Sprintf( Messages, "There is locked region, skip updating.");

            //Show the message and just return the success bit to AFU in
            //order to updating other regions. If needs to block whole updating
            //process, change to set (BIT00 | BIT02)
            MEProcessHandleResult( (BIT03 | BIT02), Messages );
        }
        return EFI_SUCCESS;
    }

    ProgramBuffer = ProgramBuffer + FLASH_BLOCK_SIZE;
    Length -= FLASH_BLOCK_SIZE;
    Offset += FLASH_BLOCK_SIZE;

    // End of Region Update
    if( !EFI_ERROR(Status) )
    {
        if( Length == 0 )
        {
            NewRegion = TRUE;
            // Only when Updating MER, the Step is lnot 0.
#if CHECK_FPTR_IN_OPR
            if( (Step == UpdateFtprToOpr) ||
                (Step == UpdateOpr) || (StructPtr->bBlockType == ME_OPR_BLK) )
#else
            if( (Step == UpdateOpr) || (StructPtr->bBlockType == ME_OPR_BLK) )
#endif
                NeedToVerify = TRUE;
        }
        else
        {
            // OEM can output message here in every block updated.
            // Remember to Set BIT02
            MEProcessHandleResult( (BIT01), " " );
            return EFI_SUCCESS;
        }

        if( NeedToVerify )
        {
            if( InSmm )
            {
                Counter = 0;
                do
                {
                    Status = HmrfpoLockMsg();
                    if( !EFI_ERROR(Status) )
                        break;
                    Counter++;
                    SmmHeci->ReInitHeci( HECI1_DEVICE );
                }while( Counter < 3 );
            }
            else
            {
                EFI_GUID   MEUDErrorguid = MEUD_ERROR_GUID;
                UINTN      VarianleSize;
                pRS->SetVariable(
                        L"LOCKMSG",
                        &MEUDErrorguid,
                        (EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS),
                        sizeof(UINTN),
                        &Counter );

                IoWrite8( SW_SMI_IO_ADDRESS, Disable_ME_SW_SMI );

                VarianleSize = sizeof(UINT8);

                Status = pRS->GetVariable(
                                L"LOCKSTATUS",
                                &MEUDErrorguid,
                                NULL,
                                &VarianleSize,
                                &HMRFPO_STATUS );

                // Delete Flag
                pRS->SetVariable(
                        L"LOCKSTATUS",
                        &MEUDErrorguid,
                        (EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS),
                        0,
                        &HMRFPO_STATUS );
            }

            // If success.
            if( !EFI_ERROR(Status) )
            {
                // Check Status
                if( HMRFPO_STATUS == 2 )
                {
                    MEProcessHandleResult( (BIT01 | BIT02), "Warming : HMRFPO_LOCK_NOT_VALID !!");
                    // Retry
                    if( RetryTimes > 0 )
                    {
                        Step = UpdateFtprToOpr;
                        RetryTimes -= 1;
                        return EFI_SUCCESS;
                    }
                }
                else if( Step == UpdateOpr )
                {
                    // Send MERESET Message.
                    Status = HmrfpoMeReset();
                }
            }
        }
        else
            Status = EFI_SUCCESS;
    }

    // Show Strings
    if( !EFI_ERROR(Status) )
    {
        switch( Step )
        {
            case UpdateFtprToOpr:
                MEProcessHandleResult(
                    (BIT01 | BIT02),
                    "Successful Update Recovery Loader to OPRx!!" );
            break;

            case UpdateMfsb:
                MEProcessHandleResult(
                    (BIT01 | BIT02),
                    "Successful Update MFSB!!" );
            break;

            case UpdateFtpr:
                MEProcessHandleResult(
                    (BIT01 | BIT02),
                    "Successful Update FTPR!!" );
            break;

            case UpdatePartitions:
                MEProcessHandleResult(
                    (BIT01 | BIT02),
                    "Successful Update factory data partitions!!" );
            break;

            case UpdateOpr:
                MEProcessHandleResult(
                    (BIT03 | BIT02),
                    "ME Entire Image update success !!" );
            break;

            default:
            {
                Index = FindDescriptorIndex( StructPtr->bBlockType );

                if( Index != MAX_BLK )
                    Sprintf( Messages, "Update success for %s", FlashRegionsDescriptor[Index].Command);
                else
                    Sprintf( Messages, "Update success");

                MEProcessHandleResult( (BIT03 | BIT02), Messages );
            }
        }
        if( Step != UpdateNonMeRegion )
            Step += 1;
    }
    else
    {
        // check Flash verify result first.
        if( Status == EFI_DEVICE_ERROR )
        {
            Sprintf( Messages, "Flash Update Error at address 0x%08X !! Please trying assert HDA_SDO pin.", (UINT32)Address );
            MEProcessHandleResult((BIT00 | BIT02), Messages);
            return EFI_SUCCESS;                  
        }                      
        switch(Step)
        {
            case UpdateFtprToOpr:
                MEProcessHandleResult(
                    (BIT00 | BIT02),
                    "Error when Update Recovery Loader to OPRx!!" );
            break;

            case UpdateMfsb:
                MEProcessHandleResult(
                    (BIT00 | BIT02),
                    "Error when Update MFSB!!" );
            break;

            case UpdateFtpr:
                MEProcessHandleResult(
                    (BIT00 | BIT02),
                    "Error when Update FTPR!!" );
            break;

            case UpdatePartitions:
                MEProcessHandleResult(
                    (BIT00 | BIT02),
                    "Error when Update factory data partitions!!" );
            break;

            case UpdateOpr:
                MEProcessHandleResult(
                    (BIT00 | BIT02),
                    "Error on ME Entire Image update!!" );
            break;

            default:
                StructPtr->UpdateResult = BIT03;
            break;
        }
        if( Step != UpdateNonMeRegion )
            Step += 1;
    }
    return EFI_SUCCESS;
}
/**
    Handle ME Process

    @param MEProcessStructPtr

    @retval VOID
**/
VOID
MEProcessHandler
(
    IN  OUT OFBD_TC_55_ME_PROCESS_STRUCT    **MEProcessStructPtr
)
{
    EFI_STATUS                  Status;
    static UINTN                NumberOfPages;
    static UINT32               SizeCopied;
    static BOOLEAN              UseSmmMem = FALSE;
    static EFI_PHYSICAL_ADDRESS SMM_Address;
    static BOOLEAN              HeciIsHide;

    StructPtr = *MEProcessStructPtr;

    switch( StructPtr->bHandleRequest )
    {
        // Allocate Buffer
        case 1:
            HeciIsHide = UnHideHeci();
            // If Non-BIOS(ME/GbE/DER/PDR) region can't be identified 
            // by FDOC, the TotalBlocks could be less than Non-BIOS Region,
            // then, the SMM memroy will get destroyed when AFU upload
            // Non-BIOS region data to SMM. So, below change to use Non-BIOS 
            // region size instead of TotalBlocks size for allocating memory
            // to avoid such situation.
            NumberOfPages = EFI_SIZE_TO_PAGES(GetFlashCapacity() - FLASH_SIZE);
            Status = pSmst->SmmAllocatePages(
                                AllocateAnyPages,
                                EfiRuntimeServicesData,
                                NumberOfPages,
                                &SMM_Address );
            if( !EFI_ERROR(Status) )
            {
                UseSmmMem = TRUE;
                Phy_Address = SMM_Address;
            }
            // No memory allocated
            if( !Phy_Address )
                MEProcessHandleResult(
                    (BIT00 | BIT02),
                    "Error : No Memory Allocated!!" );

            SizeCopied = 0;
        break;

        // Recieve Data from AFU
        case 2:
            MemCpy(
                (UINT8*)(Phy_Address + SizeCopied),
                (UINT8*)StructPtr->ddMeDataBuffer,
                StructPtr->ddMeDataSize );
            SizeCopied += StructPtr->ddMeDataSize;
        break;

        // Update
        case 4:
            UpdateRegions( (UINT8*)Phy_Address, TRUE );
        break;

        // Continue....
        case 8:
            UpdateRegions( (UINT8*)Phy_Address, TRUE );
        break;

        // Free Buffer
        case 0x10:
            if( UseSmmMem )
                pSmst->SmmFreePages( Phy_Address, NumberOfPages );

            HideHeci( HeciIsHide );
        break;
    }
}
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2018, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
