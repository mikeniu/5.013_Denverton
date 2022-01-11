//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

/** @file SbCspIoDecodeLib.c
    This file contains code for South Bridge LPC IO Decode
    initialization in the Library stage

*/

//-------------------------------------------------------------------------
// Include(s)
//-------------------------------------------------------------------------

#include <Token.h>
#include <AmiChipsetIoLib.h>
#include <AmiChipsetModulePkg/Include/Sb.h>
#include <Library/PcdLib.h>
#include <Protocol/AmiSio.h>


/**
    This function set LPC Bridge Generic Decoding

    @param *LpcPciIo - Pointer to LPC PCI IO Protocol (This is not used but kept for backwards compatible)
    @param Base      - I/O base address
    @param Length    - I/O Length
    @param Enabled   - Enable/Disable the generic decode range register
    
    @retval EFI_SUCCESS - Set successfully.
    @retval EFI_UNSUPPORTED - This function is not implemented or the Length more than the maximum supported size of generic range decoding.
    @retval EFI_INVALID_PARAMETER - the Input parameter is invalid.
    @retval EFI_OUT_OF_RESOURCES - There is not available Generic Decoding Register.
    @retval EFI_NOT_FOUND - the generic decode range will be disabled is not found.

**/
EFI_STATUS AmiSioLibSetLpcGenericDecoding (
    IN VOID           *LpcPciIo,
    IN UINT16         Base,
    IN UINT16         Length,
    IN BOOLEAN        Enable
    )
{
/**** PORTING REQUIRED ****
    UINTN             D31F0Addr;
    UINT32            IoGenDecode32;
    UINT16            IoGenDecIndex;
    UINT16            Buffer16;
    UINT8             Bsf8 = 0;
    UINT8             Bsr8 = 0;

    if (Length > 0x100) return EFI_UNSUPPORTED;

    if (Length == 0) return EFI_INVALID_PARAMETER;

    if (Length < 4) Length = 4;

    D31F0Addr = (UINTN) CSP_PCIE_CFG_ADDRESS(LPC_BUS, LPC_DEVICE, LPC_FUNC, 0);

    //
    // Read I/O Generic Decodes Register.
    //
    for (IoGenDecIndex = 0; IoGenDecIndex < 4; IoGenDecIndex++) {
      IoGenDecode32 = MmioRead32 (D31F0Addr + SB_LPC_GEN1_DEC + IoGenDecIndex * 4);

      if (Enable) {
          if ((IoGenDecode32 & 1) == 0) break;
      } else {
          if (((IoGenDecode32 & 0xfffc) == Base) && (IoGenDecode32 & 1)) {
            //
            // Disable & clear the base/mask fields
            //
            IoGenDecode32 = 0;
            break;
          }
      }
    }

    if (Enable) {
        if (IoGenDecIndex == 4) return EFI_OUT_OF_RESOURCES;

        Buffer16 = Length;
        
        while ((Buffer16 % 2) == 0) {
            Buffer16 /= 2;
            Bsf8++;
        }

        while (Length) {
            Length >>= 1;
            Bsr8++;
        }

        if (Bsf8 == (Bsr8 - 1)) Bsr8--;

        Length = (1 << Bsr8) - 1 ;

        Base &= (~Length);

        IoGenDecode32 = Base | (UINT32)((Length >> 2) << 18) | 1;

    } else {
        if (IoGenDecIndex == 4) return EFI_NOT_FOUND;
    }

    MmioWrite32 (D31F0Addr + SB_LPC_GEN1_DEC + IoGenDecIndex * 4, IoGenDecode32);
****/
    return EFI_SUCCESS;
}

/**
    This function sets LPC Bridge Device Decoding

    @param *LpcPciIo - Pointer to LPC PCI IO Protocol (This is not used but kept for backwards compatible)
    @param Base      - I/O base address, if Base is 0 means disabled the decode of the device
    @param DevUid    - The device Unique ID
    @param Type      - Device Type, please refer to AMISIO.h
    
    @retval EFI_SUCCESS - Set successfully.
    @retval EFI_UNSUPPORTED - There is not proper Device Decoding register for the device UID.

**/
EFI_STATUS AmiSioLibSetLpcDeviceDecoding (
    IN VOID                 *LpcPciIo,
    IN UINT16               Base,
    IN UINT8                DevUid,
    IN SIO_DEV_TYPE         Type
    )
{
/**** PORTING REQUIRED ****
    UINT16                  ComRange[] = { 0x3f8, 0x2f8, 0x220, 0x228,\
                                           0x238, 0x2e8, 0x338, 0x3e8, 0};
    UINT16                  LptRange[] = { 0x378, 0x278, 0x3bc, 0};
    UINT16                  FpcRange[] = { 0x3f0, 0x370, 0};
    UINT16                  IoRangeMask16 = 0xffff;
    UINT16                  IoRangeSet16 = 0;
    UINT16                  IoEnMask16 = 0xffff;
    UINT16                  IoEnSet16 = 0;
    UINT8                   Index;
    UINTN                   D31F0Addr;
    
    switch (Type) {
      case (dsFDC) :
        //
        // FDC Address Range
        //
        if (Base == 0) IoEnMask16 &= ~BIT03;
        else {
            for (Index = 0; (FpcRange[Index] != 0) && (FpcRange[Index] != Base); Index++);
            if (FpcRange[Index]) {
                IoEnSet16 |= BIT03;
                IoRangeMask16 &= ~BIT12;
                IoRangeSet16 |= (Index << 12);
            }
            else return EFI_UNSUPPORTED;
        }
        break;

      case (dsLPT) :
        //
        // LPT Address Range
        //
        if (Base == 0) IoEnMask16 &= ~BIT02;
        else {
            for (Index = 0; (LptRange[Index] != 0) && (LptRange[Index] != Base); Index++);
            if (LptRange[Index]) {
                IoEnSet16 |= BIT02;
                IoRangeMask16 &= ~(BIT09 | BIT08);
                IoRangeSet16 |= (Index << 8);
            } else return EFI_UNSUPPORTED;
        }
        break;

      case (dsUART) :
        //
        // ComA Address Range
        //
        if (Base == 0) {
            if (DevUid) IoEnMask16 &= ~BIT01;
            else IoEnMask16 &= ~BIT00;
        } else {
            for (Index = 0; (ComRange[Index] != 0) && (ComRange[Index] != Base); Index++);
            if (ComRange[Index]) {
                if (DevUid) {
                    IoEnSet16 |= BIT01;
                    IoRangeMask16 &= ~(BIT06 | BIT05 | BIT04);
                    IoRangeSet16 |= (Index << 4);
                } else {
                    IoEnSet16 |= BIT00;
                    IoRangeMask16 &= ~(BIT02 | BIT01 | BIT00);
                    IoRangeSet16 |= Index;
                }
            } else return EFI_UNSUPPORTED;
        }
        break;

      //
      // KBC Address Enable
      //
      case (dsPS2K) :
      case (dsPS2M) :
      case (dsPS2CK) :
      case (dsPS2CM) :
        if (Base == 0) IoEnMask16 &= ~BIT10;
        else IoEnSet16 |= BIT10;
        break;

      case (dsGAME) :
        //
        // Game Port Address Enable
        //
        if (Base == 0) IoEnMask16 &= ~(BIT09 | BIT08);
        else {
            if (Base == 0x200) {
                IoEnSet16 |= BIT08;
            } else {
                if (Base == 0x208) IoEnSet16 |= BIT09;
                else return EFI_UNSUPPORTED;
            }
        }
        break;

      case (0xff) :
        //
        // LPC CFG Address Enable
        //
        if (Base == 0x2e) IoEnSet16 |= BIT12;
        else {
            if (Base == 0x4e) IoEnSet16 |= BIT13;
            else {
                if (Base == 0x62) IoEnSet16 |= BIT11;
                else {
                if (Base) AmiSioLibSetLpcGenericDecoding( LpcPciIo, \
                                                       Base , \
                                                       4, \
                                                       TRUE );
                    else return EFI_UNSUPPORTED;
                }
            }
        }
        break;

      default :
        return EFI_UNSUPPORTED;
    }

    //
    // Program LPC Decode and IO Enable registers
    //
    D31F0Addr = (UINTN) CSP_PCIE_CFG_ADDRESS(LPC_BUS, LPC_DEVICE, LPC_FUNC, 0);
    MmioAndThenOr16 (D31F0Addr + SB_LPC_IODEC, IoRangeMask16, IoRangeSet16);
    MmioAndThenOr16 (D31F0Addr + SB_LPC_EN, IoEnMask16, IoEnSet16);
****/
    return EFI_SUCCESS;
}

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
