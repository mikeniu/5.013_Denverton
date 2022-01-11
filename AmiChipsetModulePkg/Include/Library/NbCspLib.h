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

/** @file NbCspLib.h
  This file contains North/Host Bridge chipset porting library functions
  and data structure definitions for both PEI & DXE stages.

*/

#ifndef __NBLIB_H__
#define __NBLIB_H__

#include <Efi.h>
#include <Pei.h>
#include <Protocol/PciHostBridgeResourceAllocation.h>
#include <Protocol/PciRootBridgeIo.h>
#include <AmiDxeLib.h>
#include <AmiCspLib.h>

#if ACPI_SUPPORT
#include <Protocol/S3SaveState.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define LEGACY_REGION_LOCK          0               // Read only Read to RAM, Write to ROM
#define LEGACY_REGION_BOOT_LOCK     1
#define LEGACY_REGION_UNLOCK        2               // Read/Write to RAM
#define LEGACY_REGION_DECODE_ROM    3               // Read/Write to ROM

//
// Shadow Attribute
//
typedef enum {
	Disabled,   // Read/Write are both disabled
	ReadOnly,   // Read Only
	WriteOnly,  // Write Only
	ReadWrite   // Read/Write are both enabled
} SHADOW_ARRTIBUTE;

//
// BIOS Range to shadow
//
typedef enum {
    C0000_16K,  // 0C0000-0C3FFF
    C4000_16K,  // 0C4000-0C7FFF
    C8000_16K,  // 0C8000-0CBFFF
    CC000_16K,  // 0CC000-0CFFFF
    D0000_16K,  // 0D0000-0D3FFF
    D4000_16K,  // 0D4000-0D7FFF
    D8000_16K,  // 0D8000-0DBFFF
    DC000_16K,  // 0DC000-0DFFFF
    E0000_16K,  // 0E0000-0E3FFF
    E4000_16K,  // 0E4000-0E7FFF
    E8000_16K,  // 0E8000-0EBFFF
    EC000_16K,  // 0EC000-0EFFFF
    F0000_64K   // 0F0000-0FFFFF
} SHADOW_BIOS_RANGE;

typedef struct {
    UINT8     ChannelNumber;  //< Zero based channel number.
    UINT8     DimmNumber;     //< Zero based DIMM number.
    UINT8     Rank;           //< Zero based rank number.
    UINT8     Bank;           //< Zero based bank number.
    UINT16    Cas;            //< Zero based CAS number.
    UINT16    Ras;            //< Zero based RAS number.
} NB_ADDRESS_DECODE;

EFI_STATUS
NbProgramPamRegisters (
    IN EFI_BOOT_SERVICES    *gBS,
    IN EFI_RUNTIME_SERVICES *gRS,
    IN UINT32               StartAddress, 
    IN UINT32               Length,
    IN UINT8                Setting, 
    IN UINT32               *Granularity
);

EFI_STATUS NbPeiProgramPamRegisters(
    IN EFI_PEI_SERVICES **PeiServices,
    IN UINT32           StartAddress,
    IN UINT32           Length,
    IN UINT8            Setting,
    IN UINT32           *Granularity OPTIONAL
);

EFI_STATUS NbPamWriteBootScript(
    IN EFI_S3_SAVE_STATE_PROTOCOL *BootScriptSave
);

VOID NbResetCpuOnly (
    VOID
);

VOID NbRuntimeShadowRamWrite(
    IN BOOLEAN Enable
);

EFI_STATUS
NbRuntimeShadowRamWriteExt (
    IN SHADOW_ARRTIBUTE   Attribute,
    IN SHADOW_BIOS_RANGE  Range
);

UINT32 NBGetTsegBase (
    VOID
);

BOOLEAN NbCheckVtdSupport (
    VOID
);

BOOLEAN NbIsDualChannel (
    IN UINT8              MemoryChannelType
);

EFI_STATUS NbLockPavpc (
    IN BOOLEAN            MODE
);

EFI_STATUS NbGetDimmLocInfo (
    IN CONST UINTN        MemoryAddr,
    OUT NB_ADDRESS_DECODE *NbAddressDecode
);

//----------------------------------------------------------------------------
// Chipset PCI Macros, Porting Required.
//----------------------------------------------------------------------------

#define READ_PCI8_NB(Rx)          READ_PCI8(NB_BUS, NB_DEV, NB_FUN, Rx)
#define WRITE_PCI8_NB(Rx, Val)    WRITE_PCI8(NB_BUS, NB_DEV, NB_FUN, Rx, Val)
#define SET_PCI8_NB(Rx, Set)      SET_PCI8(NB_BUS, NB_DEV, NB_FUN, Rx, Set)
#define RESET_PCI8_NB(Rx, Rst)    RESET_PCI8(NB_BUS, NB_DEV, NB_FUN, Rx, Rst)
#define RW_PCI8_NB(Rx, St, Rt)    RW_PCI8(NB_BUS, NB_DEV, NB_FUN, Rx, St, Rt)
#define READ_PCI16_NB(Rx)         READ_PCI16(NB_BUS, NB_DEV, NB_FUN, Rx)
#define WRITE_PCI16_NB(Rx, Val)   WRITE_PCI16(NB_BUS, NB_DEV, NB_FUN, Rx, Val)
#define SET_PCI16_NB(Rx, Set)     SET_PCI16(NB_BUS, NB_DEV, NB_FUN, Rx, Set)
#define RESET_PCI16_NB(Rx, Rst)   RESET_PCI16(NB_BUS, NB_DEV, NB_FUN, Rx, Rst)
#define RW_PCI16_NB(Rx, St, Rt)   RW_PCI16(NB_BUS, NB_DEV, NB_FUN, Rx, St, Rt)
#define READ_PCI32_NB(Rx)         READ_PCI32(NB_BUS, NB_DEV, NB_FUN, Rx)
#define WRITE_PCI32_NB(Rx, Val)   WRITE_PCI32(NB_BUS, NB_DEV, NB_FUN, Rx, Val)
#define SET_PCI32_NB(Rx, Set)     SET_PCI32(NB_BUS, NB_DEV, NB_FUN, Rx, Set)
#define RESET_PCI32_NB(Rx, Rst)   RESET_PCI32(NB_BUS, NB_DEV, NB_FUN, Rx, Rst)
#define RW_PCI32_NB(Rx, St, Rt)   RW_PCI32(NB_BUS, NB_DEV, NB_FUN, Rx, St, Rt)

#ifdef __cplusplus
}
#endif
#endif


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
