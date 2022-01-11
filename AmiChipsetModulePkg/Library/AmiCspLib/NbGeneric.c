//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

/** @file NbGeneric.c
    This file contains generic NB code that is common between
    various components such as NB PEI, DXE etc

*/


// Module specific Includes
#include "Efi.h"
#include "Token.h"
#include <AmiLib.h>
#include <AmiCspLib.h>
#include <AmiDxeLib.h>
#include <Protocol/PciRootBridgeIo.h>
#include <SaRegs.h>  // Denverton AptioV Override  - EIP#295819

// Produced Protocols

// GUID Definitions

// Portable Constants

// Function Prototypes

// Function Definition

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
#if     CSM_SUPPORT

EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL *gPciRootBridgeIo;
//----------------------------------------------------------------------------
#define REGION_DECODE_ROM   0       //Read/Write ROM
#define REGION_LOCK     1       //Read Only
#define REGION_UNLOCK   3       //Read/Write Only
#define LEGACY_REGION_LOCK          0               // Read only Read to RAM, Write to ROM
#define LEGACY_REGION_BOOT_LOCK     1
#define LEGACY_REGION_UNLOCK        2               // Read/Write to RAM
#define LEGACY_REGION_DECODE_ROM    3               // Read/Write to ROM

typedef struct
{
    UINT8   Register;
    UINT8   Mask;
    UINT32  StartAddress;
    UINT32  Length;
} PAM_STRUCT;

/** Porting required for the following structure **/
PAM_STRUCT gPamStruct[] =
{
    {0x91,0xfc,0xc0000,0x4000}, 
    {0x91,0xcf,0xc4000,0x4000}, 
    {0x92,0xfc,0xc8000,0x4000}, 
    {0x92,0xcf,0xcc000,0x4000}, 
    {0x93,0xfc,0xd0000,0x4000}, 
    {0x93,0xcf,0xd4000,0x4000}, 
    {0x94,0xfc,0xd8000,0x4000}, 
    {0x94,0xcf,0xdc000,0x4000}, 
    {0x95,0xfc,0xe0000,0x4000},
    {0x95,0xcf,0xe4000,0x4000},
    {0x96,0xcf,0xec000,0x4000},
    {0x96,0xfc,0xe8000,0x4000},
    {0x90,0xcf,0xf0000,0x10000}
};

#define NUM_PAM_ENTRIES (sizeof(gPamStruct) / sizeof(PAM_STRUCT))

typedef struct {
    UINT64                  Address;
    EFI_BOOT_SCRIPT_WIDTH   Width;
} BOOT_SCRIPT_PCI_REGISTER_SAVE;

BOOT_SCRIPT_PCI_REGISTER_SAVE gPciRegistersSave[] = {
    0x00000090,  EfiBootScriptWidthUint32,
    0x00000094,  EfiBootScriptWidthUint16,
    0x00000096,  EfiBootScriptWidthUint8
};

//----------------------------------------------------------------------------
// Start OF CSM Related Porting Hooks
//----------------------------------------------------------------------------

//
// The following data structure specifies the PCI device/function number of the root
// bridge(s). Number of entries in this table defined by ROOT_BRIDGE_COUNT
// This table is a missing link between RootBridgeIo and PciIo, which allows to update
// BusNumXlat table with actual bus numbers.
// Each entry in the table is a pair of RootBridge UID (UINT32), provided in RootBridge
// device path, and PCI Dev/Func number (UINT8) that can be used to access Root Bridge on
// PCI bus.
ROOT_BRIDGE_MAPPING_ENTRY   RbMap[ROOT_BRIDGE_COUNT] = {
//          RB ID           Device function number
            { 0,            0x00}       // PORTING PORTING - Include device function number of RB
    };
UINTN                       RbCount = ROOT_BRIDGE_COUNT;

/**
    Helper function to get the Start and End Index for
    PAM register table.

    @param StartAddress       Start address of the PAM area
    @param Length             Length of the PAM area
    @param StartIndex         Start Index of the PAM (OUT)
    @param EndIndex           End Index of the PAM (OUT)

    @retval EFI_STATUS

**/
EFI_STATUS NbGetPamStartEndIndex(
    IN  UINT32   StartAddress,
    IN  UINT32   Length,
    OUT UINT32  *StartIndex,
    OUT UINT32  *EndIndex
)
{
    UINT32 StartIdx;
    UINT32 EndIdx;
    UINT32 TotalLength = 0;

    if (StartAddress < gPamStruct[0].StartAddress) return EFI_INVALID_PARAMETER;

    for(StartIdx = 0; StartIdx < NUM_PAM_ENTRIES; ++StartIdx) {
        if (StartAddress <= gPamStruct[StartIdx].StartAddress) break;
    }
    if (StartAddress < gPamStruct[StartIdx].StartAddress) StartIdx--;

    if (StartIdx == NUM_PAM_ENTRIES) return EFI_INVALID_PARAMETER;

    // Adjust the length of the requested region if starting address is
    // out of bounds.
    Length += StartAddress-gPamStruct[StartIdx].StartAddress;

    for(EndIdx = StartIdx; EndIdx < NUM_PAM_ENTRIES; ++EndIdx)
    {
        TotalLength += gPamStruct[EndIdx].Length;
        if (TotalLength >= Length) break;
    }
    if (EndIdx == NUM_PAM_ENTRIES) return EFI_INVALID_PARAMETER;

    *StartIndex = StartIdx;
    *EndIndex = EndIdx;
    return EFI_SUCCESS;
}


/**
    Program 0xc0000 - 0xfffff regions to Lock/Unlock.

    @param gBS            Pointer to the boot services table
    @param gRS            Pointer to the runtime services table
    @param StartAddress   Start address of the PAM area
    @param Length         Length of the PAM area
    @param Setting        Settings to be set for the above area
    @param Granularity    Granularity of the above area (return value)

    @retval EFI_STATUS

    @note  Here is the control flow of this function:
              1. Search the structure for the first entry matching 
                 the StartAddress.
              2. If not found, return EFI_INVALID_PARAMETER.
              3. Find the last entry in structure for the region to program,
                 by adding the lengths of the entries.
              4. If not found, return EFI_INVALID_PARAMETER.
              5. Read/Write each register for the entry to set region.
              6. Return the Granularity for the region.
**/
EFI_STATUS
NbProgramPamRegisters(
    IN EFI_BOOT_SERVICES    *gBS,
    IN EFI_RUNTIME_SERVICES *gRS,
    IN UINT32               StartAddress,
    IN UINT32               Length,
    IN UINT8                Setting,
    IN OUT UINT32           *Granularity
)
{
    EFI_STATUS  Status;

    //NB shadow programming.
    //Note: For this routine to work, the gPamStruct regions must be continuous.
//  UINT64      PciAddress;
    UINT32      StartIndex;
    UINT32      EndIndex;
    UINT32      i;
//  UINT8       Data;

    Status = NbGetPamStartEndIndex(
        StartAddress,
        Length,
        &StartIndex,
        &EndIndex
    );
    if (EFI_ERROR(Status)) return Status;
    if (Setting == LEGACY_REGION_LOCK) Setting = REGION_LOCK;
    else  if (Setting == LEGACY_REGION_UNLOCK) Setting = REGION_UNLOCK;
    else Setting = REGION_DECODE_ROM;

    for(i = StartIndex; i <= EndIndex; ++i) {
        /** Chipset porting 
        //Bus 0, Device 0x10, Function 0
        PciAddress = (0 << 24) + (0x10 << 16) + (0 << 8) + gPamStruct[i].Register;
        gPciRootBridgeIo->Pci.Read(
            gPciRootBridgeIo,
            EfiPciWidthUint8,
            PciAddress,
            1,
            &Data
            );

        Data &= gPamStruct[i].Mask;
        Data |= (gPamStruct[i].Mask == 0xfc) ? Setting : Setting << 4;

        gPciRootBridgeIo->Pci.Write(
            gPciRootBridgeIo,
            EfiPciWidthUint8,
            PciAddress,
            1,
            &Data
            );
        **/     // Chipset porting end here

    }

    if (Granularity)
        *Granularity = ( (StartAddress+Length) < 0xf0000 ) ? 0x4000 : 0x10000;


    return EFI_SUCCESS;
}

/**
    Program 0xc0000 - 0xfffff regions to Lock/Unlock during PEI stage

    @param PeiServices   Pointer to the PEI services table
    @param StartAddress  Start address of the PAM area
    @param Length        Length of the PAM area
    @param Setting       Settings to be set for the above area
    @param Granularity   Granularity of the above area (return value)

    @retval EFI_STATUS

**/
EFI_STATUS NbPeiProgramPamRegisters(
    IN EFI_PEI_SERVICES **PeiServices,
    IN UINT32           StartAddress,
    IN UINT32           Length,
    IN UINT8            Setting,
    IN OUT UINT32       *Granularity OPTIONAL
)
{
    EFI_PEI_PCI_CFG2_PPI *PciCfg = (*PeiServices)->PciCfg;
    EFI_STATUS  Status;
//  UINT64      PciAddress;
    UINT32      StartIndex;
    UINT32      EndIndex;
    UINT32      i;
//  UINT8       Data;

    Status = NbGetPamStartEndIndex(
        StartAddress,
        Length,
        &StartIndex,
        &EndIndex
    );
    if (EFI_ERROR(Status)) return Status;

    if (Setting == LEGACY_REGION_LOCK) Setting = REGION_LOCK;
    else  if (Setting == LEGACY_REGION_UNLOCK) Setting = REGION_UNLOCK;
    else Setting = REGION_DECODE_ROM;

    for(i = StartIndex; i <= EndIndex; ++i) {
/** CHIPSET PORTING 
        //Bus 0, Device 0x10, Function 0
        PciAddress = (0 << 24) + (0x10 << 16) + (0 << 8) + gPamStruct[i].Register;
        PciCfg->Read(
            PeiServices,
            PciCfg,
            EfiPeiPciCfgWidthUint8,
            PciAddress,
            &Data
        );

        Data &= gPamStruct[i].Mask;
        Data |= (gPamStruct[i].Mask == 0xfc) ? Setting : Setting << 4;

        PciCfg->Write(
            PeiServices,
            PciCfg,
            EfiPeiPciCfgWidthUint8,
            PciAddress,
            &Data
        );
 ** CHIPSET PORTING **/
    }

    if (Granularity)
        *Granularity = ( (StartAddress+Length) < 0xf0000 ) ? 0x4000 : 0x10000;


    return EFI_SUCCESS;
}

/**
    Writes the final settings of PAM registers to the BOOT Script

    @param BootScriptSave - Pointer to S3 boot script save protocol

    @retval EFI_STATUS

    @note  Here is the control flow of this function:
              1. From the PCI register save table, read the pCI register to save.
              2. Write to the boot script the value.
              3. Repeat 1 & 2 for all table entries.
**/
EFI_STATUS NbPamWriteBootScript(
    IN EFI_S3_SAVE_STATE_PROTOCOL     *BootScriptSave
)
{
    EFI_STATUS  Status = EFI_SUCCESS;
    UINTN       i;
    UINT32      Value;

    for (i = 0; i < sizeof(gPciRegistersSave)/ sizeof(BOOT_SCRIPT_PCI_REGISTER_SAVE); ++i) {
        gPciRootBridgeIo->Pci.Read(
            gPciRootBridgeIo,
            gPciRegistersSave[i].Width,
            gPciRegistersSave[i].Address,
            1,
            &Value
        );
        BOOT_SCRIPT_S3_PCI_CONFIG_WRITE_MACRO(
            BootScriptSave,
            gPciRegistersSave[i].Width,
            gPciRegistersSave[i].Address,
            1,
            &Value
        );
    }

    return Status;
}


//----------------------------------------------------------------------------
#endif          // END OF CSM Related Porting Hooks
//----------------------------------------------------------------------------

/**
    This function issues a CPU only reset.

    @param VOID

    @retval VOID
**/
VOID NbResetCpuOnly (
    VOID
)
{
    // Porting Required.
}

/**
    This function provides runtime interface to enable/disable
    writing in E000-F000 segment

    @param Enable if TRUE - enable writing, if FALSE - disable

    @retval VOID
**/
VOID NbRuntimeShadowRamWrite(
    IN BOOLEAN Enable
)
{
    // Denverton AptioV Override  Start - EIP#295819
    UINT64  MchBar;
    UINTN   HostCongSpaceAddress;
    UINT64  UnlockRegion = 0xE0000;
    UINT32  EndAddress;
    UINT32  StartAddress;
    UINT64  Data;
    UINT64  ImrNum;
    
    //
    //Get Host Configuration Space address (Bus 0,Dev 0, Fun 0)
    //PCIEX_BASE_ADDRESS + (UINTN) (Bus << 20) + (UINTN) (Device << 15) + (UINTN) (Function << 12));
    //
    HostCongSpaceAddress = (UINTN) PCIEX_BASE_ADDRESS;
    //
    //Get MCHBAR(Memory Controller Hub Base Address Register) from Host Configuration
    //Space (Offset 48h)
    //
    MchBar = MmioRead32 (HostCongSpaceAddress + R_SA_MCHBAR_LO) & ~BIT0;
    
    //
    //Check, which IMR register set holds the 0xE0000 and 0xF0000 memory region
    //
    for(ImrNum=0; ImrNum<20; ImrNum++){     //Left shift of ImrNum for 5 times, for move to the next offset of IMR register set.
        StartAddress = MmioRead32 ((UINTN)(MchBar + R_SA_MCHBAR_BIMR0BASE + LShiftU64(ImrNum, 5)));   //Read base address
        EndAddress = MmioRead32 ((UINTN)(MchBar + R_SA_MCHBAR_BIMR0MASK + LShiftU64(ImrNum, 5))); //Read Mask address
        if((StartAddress == 0x00)&&(EndAddress == 0x00))    //Skip if IMR not configured and move to next IMR.
        {
          continue;
        }
        
        //
        //Write Enable/Disable shadow region
        //
        StartAddress = (StartAddress << 10) & 0x7FFFFFFF;
        EndAddress   = 0xFFFFFFFF-((EndAddress << 10)+1);
        EndAddress   += StartAddress;
        
        Data = Enable ? (BUNIT_BIMR_WAC_WRITE_OPEN_FOR_ALL_CORE | V_SA_MCHBAR_BIMRCP_ENABLE_IA_UNSTRUSTED) : BUNIT_BIMR_WAC_WRITE_DISABLED;
        if((StartAddress <= UnlockRegion) && (EndAddress >= UnlockRegion)){
            MmioWrite64 (((UINTN)(MchBar + R_SA_MCHBAR_BIMR0WAC + LShiftU64(ImrNum, 5))), Data);  //Enable\Disable write access 
            Data = MmioRead64 ((UINTN)(MchBar + R_SA_MCHBAR_BIMR0WAC + LShiftU64(ImrNum, 5)));
            ASSERT((Data & BUNIT_BIMR_WAC_WRITE_OPEN_FOR_ALL_CORE) == (Enable ? BUNIT_BIMR_WAC_WRITE_OPEN_FOR_ALL_CORE : BUNIT_BIMR_WAC_WRITE_DISABLED)); //Verify once
         
            if(EndAddress < 0x100000){  //If Shadow region was locked by more than one IMR register set move to next IMR register set
                UnlockRegion = EndAddress + 1;
                continue;
            }
            break;
        }   
    }
    // Denverton AptioV Override  End - EIP#295819
}

/**
    This function provides runtime interface to enable/disable
    writing in C000-F000 segment

    @param The shadowing attributes of the BIOS range
    @param The BIOS range which is going to be shadowed

    @retval VOID
**/
EFI_STATUS
NbRuntimeShadowRamWriteExt (
    IN SHADOW_ARRTIBUTE   Attribute,
    IN SHADOW_BIOS_RANGE  Range
)
{
    /**** PORTING REQUIRED ****
    UINT8   PamData=0;
    UINT8   PamIndex=0;

    switch (Attribute) {
    case Disabled:
      PamData = 0x0;
      break;
    case ReadOnly:
      PamData = 0x1;
      break;
    case WriteOnly:
      PamData = 0x2;
      break;
    case ReadWrite:
      PamData = 0x3;
      break;
    default:
      return EFI_INVALID_PARAMETER;
    }

    switch (Range) {
    case C4000_16K:
      PamData <<= 4;
    case C0000_16K:
      PamIndex = PAM1;
      break;
    case CC000_16K:
      PamData <<= 4;
    case C8000_16K:
      PamIndex = PAM2;
      break;
    case D4000_16K:
      PamData <<= 4;
    case D0000_16K:
      PamIndex = PAM3;
      break;
    case DC000_16K:
      PamData <<= 4;
    case D8000_16K:
      PamIndex = PAM4;
      break;
    case E4000_16K:
      PamData <<= 4;
    case E0000_16K:
      PamIndex = PAM5;
      break;
    case EC000_16K:
      PamData <<= 4;
    case E8000_16K:
      PamIndex = PAM6;
      break;
    case F0000_64K:
      PamData <<= 4;
      PamIndex = PAM0;
      break;
    default:
      return EFI_INVALID_PARAMETER;
    }

    WRITE_PCI8_NB(PamIndex, PamData);
    **** PORTING REQUIRED ****/
    return EFI_SUCCESS;
}

/**
    Returns the base address of TSEG.

    @param VOID

    @retval The Base Address of TSEG.
**/
UINT32 NBGetTsegBase (VOID)
{
/**** PORTING REQUIRED ****
    Please check the datasheet of the chipset for the register to get
    base Address of TSEG
**** PORTING REQUIRED ****/
    return (MmioRead32 ((UINTN) NB_PCIE_CFG_ADDRESS(0, 0, 0, 0xB8)) & 0xFFF00000);
}

/**
    Check if Vtd is supported.

    @param VOID

    @retval TRUE  Vtd is supported
    @retval FALSE Vtd is not supported
**/
BOOLEAN NbCheckVtdSupport (
    VOID
)
{
/**** PORTING REQUIRED ****
    Please check the datasheet of the chipset for the register to know
    if Vtd is supported or not
**** PORTING REQUIRED ****/
    return FALSE;
}

/**
    Check if the system is Dual Channel or not

    @param MemoryChannelType  0:Symmetric, 1:Interleave, 2:Single

    @retval TRUE  It is Dual Channel
    @retval FALSE It is not Dual Channel
**/
BOOLEAN NbIsDualChannel (
    IN UINT8              MemoryChannelType 
)
{
/**** PORTING REQUIRED ****
    Please use registers or other way to check if 
    the system is Dual Channel or not
**** PORTING REQUIRED ****/
    return FALSE;
}

/**
    Lock and unlock Pavpc

    @param Mode               TRUE: Lock Pavpc; FALSE: Unlock Pavpc

    @retval EFI_UNSUPPORTED   This function is not supported
    @retval EFI_SUCCESS       Success to lock/unlock Pavpc

**/
EFI_STATUS NbLockPavpc (
    IN BOOLEAN            Mode
)
{
/**** PORTING REQUIRED ****
    Please check the datasheet of the chipset for the register to know
    how to lock and unlock Pavpc.
    If it is not supported by the chip, please return EFI_UNSUPPORTED.
**** PORTING REQUIRED ****/
    return EFI_UNSUPPORTED;
}

/**
    Get the information of the DIMM location indicated by MemoryAddr

    @param MemoryAddr         The system address to convert
    @param NbAddressDecode    Pointer to the buffer used to store NB_ADDRESS_DECODE

    @retval EFI_UNSUPPORTED   This function is not supported
    @retval EFI_SUCCESS       Success to get the information of DIMM location

**/
EFI_STATUS NbGetDimmLocInfo (
    IN CONST UINTN        MemoryAddr,
    OUT NB_ADDRESS_DECODE *NbAddressDecode
)
{
/**** PORTING REQUIRED ****
    Please check if MRC supports memory address decode.
    If it is not supported by MRC, please return EFI_UNSUPPORTED.
**** PORTING REQUIRED ****/
    return EFI_UNSUPPORTED;
}

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
