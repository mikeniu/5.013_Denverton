//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

/** @file SbCspLib.h
  This file contains South Bridge(LPC) chipset porting library functions
  and data structure definitions for both PEI & DXE stages.
  
*/

#ifndef __SBLIB_H__
#define __SBLIB_H__

#include <Efi.h>
#include <Pei.h>
#include <Protocol/PciRootBridgeIo.h>
#include <Protocol/PciIo.h>
#include <Protocol/AmiSio.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <AmiCspLib.h>

#if     CSM_SUPPORT
#include <Protocol/LegacyInterrupt.h>
#endif

#if ACPI_SUPPORT
#include <Protocol/S3SaveState.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if     CSM_SUPPORT
EFI_STATUS SbGenInitializeRouterRegisters(
    IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL *PciRBIo
);

UINT8 SBGen_GetPIRQIndex(
    IN UINT8 PIRQRegister
);

EFI_STATUS SbGenReadPirq(
    IN EFI_LEGACY_INTERRUPT_PROTOCOL    *This,
    IN UINT8                            PirqNumber,
    OUT UINT8                           *PirqData 
);

EFI_STATUS SbGenWritePirq(
    IN EFI_LEGACY_INTERRUPT_PROTOCOL    *This,
    IN UINT8                            PirqNumber,
    IN UINT8                            PirqData
);
#endif

VOID SbLibShutdown(
    VOID
);

#if SB_RESET_PPI_SUPPORT
VOID SBLib_ResetSystem(
    IN  EFI_RESET_TYPE  ResetType
);
#endif

#if SB_STALL_PPI_SUPPORT
EFI_STATUS CountTime(
    IN  UINTN   DelayTime,
    IN  UINT16  BaseAddr            // only needs to be 16 bit for I/O address
);
#endif

EFI_STATUS SbLibSetLpcDeviceDecoding(
    IN EFI_PCI_IO_PROTOCOL      *LpcPciIo,
    IN UINT16                   Base,
    IN UINT8                    DevUid,
    IN SIO_DEV_TYPE             Type
);

EFI_STATUS SbLibSetLpcGenericDecoding(
    IN EFI_PCI_IO_PROTOCOL      *LpcPciIo,
    IN UINT16                   Base,
    IN UINT16                   Length,
    IN BOOLEAN                  Enable
);

BOOLEAN SbIsDefaultConfigMode (
    IN EFI_PEI_SERVICES                 **PeiServices,
    IN EFI_PEI_READ_ONLY_VARIABLE2_PPI  *ReadVariablePpi
);

UINT8 ReadCmos(
    IN UINT8 Index
);

VOID WriteCmos(
    IN UINT8 Index, 
    IN UINT8 Value
);

BOOLEAN SbLib_GetSmiState(
    VOID
);

VOID SbLib_SmiDisable(
    VOID
);

VOID SbLib_SmiEnable(
    VOID
);

VOID CspLibCheckPowerLoss(
    VOID
);

#if SMM_SUPPORT
EFI_STATUS SbSmmSaveRestoreStates (
    IN BOOLEAN                      Save
);
#endif

#if BIOS_LOCK_ENABLE
VOID BiosLockEnableSmiFlashHook (
    IN UINT8                        SwSmiNum,
    IN OUT UINT64                   Buffer
);
#endif

EFI_STATUS SbFlashProtectedRange (
    VOID
);

//---------------------------------------------------------------------------
//  CMOS Manager Support
//
//  South bridge should implement functions to support access to additional 
//  CMOS banks that exist beyond the first 128 bytes.
//---------------------------------------------------------------------------

#if CMOS_MANAGER_SUPPORT
#include <CmosAccess.h>

EFI_STATUS ReadWriteCmosBank2 (
    IN EFI_PEI_SERVICES             **PeiServices,  // NULL in DXE phase
    IN CMOS_ACCESS_TYPE             AccessType,
    IN UINT16                       CmosRegister,
    IN OUT UINT8                    *CmosParameterValue
);

BOOLEAN SbGetRtcPowerStatus (
    IN EFI_PEI_SERVICES             **PeiServices  // NULL in DXE phase
);

#endif  // #if CMOS_MANAGER_SUPPORT


//---------------------------------------------------------------------------
// OEM Generic Functions Support
//---------------------------------------------------------------------------
typedef enum {
	SB_DEV_FUNCTION_0,
	SB_DEV_ADSP,
	SB_DEV_SATA1,
	SB_DEV_SMBUS,
	SB_DEV_AZALIA,
	SB_DEV_EHCI2,
	SB_DEV_LPC_BRIDGE,
	SB_DEV_EHCI1,
	SB_DEV_PCI_EX_PORT1,
	SB_DEV_PCI_EX_PORT2,
	SB_DEV_PCI_EX_PORT3,
	SB_DEV_PCI_EX_PORT4,
	SB_DEV_PCI_EX_PORT5,
	SB_DEV_PCI_EX_PORT6,
	SB_DEV_PCI_EX_PORT7,
	SB_DEV_PCI_EX_PORT8,
	SB_DEV_THERMAL,
	SB_DEV_SATA2,
	SB_DEV_XHCI,
	SB_DEV_LAN,
	SB_DEV_MAX
} SB_DEV_TYPE;

typedef enum {
	SbComA,
	SbComB,
	SbLpt,
	SbFdd,
	SbGameL,
	SbGameH,
	SbKbc,
	SbMc,
	SbCnf1,	// 0x2E & 0x2F
	SbCnf2	// 0x4E & 0x4F
} SB_LPC_SIO_TYPE;

typedef enum {
	TYPE_HOT_PLUG,
	TYPE_SWGPE,
	TYPE_TCOSCI,
	TYPE_RI,
	TYPE_PCI_EXP,
	TYPE_BATLOW,
	TYPE_PME,
	TYPE_ME_SCI,
	TYPE_PME_B0,
	TYPE_GP27,
	TYPE_WADT
} SB_GPE0_TYPE;

typedef enum {
	GPE0_EN_OP_SAVE,
	GPE0_EN_OP_RESTORE,
	GPE0_EN_OP_CLEAR_ALL
} GPE0_EN_OP;

typedef enum {
	CPU_THERMAL_TRIP_STATUS,
	AFTERG3_EN,
	PWR_FLR
} SB_MISC_TYPE;

typedef enum {
	PCONF0,
	PCONF1,
	PADVAL
} GPIO_REG_TYPE;

typedef enum {
	GPIO_NC,
	GPIO_SC,
	GPIO_SUS,
	GPIO_Normal
} AMI_GPIO_GROUP_TYPE;

typedef struct _AMI_OEM_GPIO {
	AMI_GPIO_GROUP_TYPE   Group;
	UINT8                 PinNum;
} AMI_OEM_GPIO;

typedef enum _SB_WAKEUP_TYPE {
	SbWakeupTypeOther = 1,
	SbWakeupTypeUnknown = 2,
	SbWakeupTypeApmTimer = 3,
	SbWakeupTypeModemRing = 4,
	SbWakeupTypeLanRemote = 5,
	SbWakeupTypePowerSwitch = 6,
	SbWakeupTypePciPme = 7,
	SbWakeupTypeAcPowerRestored = 8,
	SbWakeupTypeSmBiosMax,
	SbWakeupTypeMax
} SB_WAKEUP_TYPE;

typedef enum {
	SbResetFull = 3,  // This is the same with power cycle reset of Intel Pch chipsets
	SbResetGlobal,
	SbResetGlobalWithEc,
	SbResetTypeMax,
} SB_EXT_RESET_TYPE;

VOID SbEnableDisableFunctions (
    IN SB_DEV_TYPE  SbDevType,
    IN BOOLEAN      Enable
);

VOID SbSaveRestoreAllHwSmi (
    IN BOOLEAN      Save
);

VOID SbDisableAllHwSmi (
    VOID
);

BOOLEAN SbCheckNmiStatus (
    VOID
);

BOOLEAN SbEnableDisableNmi (
    IN BOOLEAN      Enable
);

VOID SbSaveRestoreNmi (
    IN BOOLEAN      Save
);

VOID SbSwSmiTrigger (
    IN UINT8        SwSmi
);

VOID SbSwSmiIo (
    IN     UINT8    SwSmi,
    IN OUT UINT8    *Data
);

UINT16 SbGetSwSmi (
    IN OUT UINT32   *DataValue
);

UINT16 SbGetTco2StsAddress (
    IN UINT16       AcpiBaseAddr
);

UINT16 SbGetTco2CntAddress (
    IN UINT16       AcpiBaseAddr
);

VOID SetAfterG3Bit (
    IN BOOLEAN      Set
);

BOOLEAN SbCheckAfterG3 (
    VOID
);

VOID SbDisableLpcDevices (
    IN SB_LPC_SIO_TYPE  SioType
);

VOID SbEnableEhciSmi (
    VOID
);

VOID SbDisableEhciSmi (
    VOID
);

BOOLEAN IsPowerButtonPressed (
    VOID
);

VOID SbEnablePciPme (
    IN UINTN        PciAddress
);

VOID SbDisablePciPme (
    IN UINTN        PciAddress
);

VOID SbEnableWolPmConfg (
    VOID
);

VOID SbDisableWolPmConfg (
    VOID
);

UINT16 SbGetIoTrapInfo (
    IN OUT UINT32   *TrappedIoData 
);

UINT16 SbGetAcpiBaseAddress (
    VOID
);

UINT16 SbGetPm1CntOffset (
    VOID
);

UINT32 SbGetRcrbAddress (
    VOID
);

BOOLEAN SbIsRtcPwrValid (
    VOID
);

BOOLEAN SbGetGpe0En (
    IN SB_GPE0_TYPE Gpe0Type
);

VOID SbSetGpe0En (
    IN SB_GPE0_TYPE Gpe0Type
);

VOID SbResetGpe0En (
    IN SB_GPE0_TYPE Gpe0Type
);

VOID SbGpe0Operation (
    IN GPE0_EN_OP   Operation
);

BOOLEAN SbGetGpe0Sts (
    IN SB_GPE0_TYPE Gpe0Type
);

VOID SbClearGpe0Sts (
    IN SB_GPE0_TYPE Gpe0Type
);

VOID SbSetGpe0GpinEn (
    IN AMI_OEM_GPIO Gpio
);

BOOLEAN SbGetGpe0GpinEn (
    IN AMI_OEM_GPIO Gpio
);

VOID SbResetGpe0GpinEn (
    IN AMI_OEM_GPIO Gpio
);

BOOLEAN SbGetGpe0GpinSts (
    IN AMI_OEM_GPIO Gpio
);

VOID SbClearGpe0GpinSts (
    IN AMI_OEM_GPIO Gpio
);

BOOLEAN SbGetGpioUseSel (
    IN AMI_OEM_GPIO Gpio
);

VOID SbProgGpioUseSel (
    IN AMI_OEM_GPIO Gpio,
    IN BOOLEAN      GpioMode,
    IN UINT8        MulFunc
);

BOOLEAN SbGetGpioIoSel (
    IN AMI_OEM_GPIO Gpio
);

VOID SbSetGpioIoSel (
    IN AMI_OEM_GPIO Gpio,
    IN BOOLEAN      InputMode
);

BOOLEAN SbGetGpioLvlSel (
    IN AMI_OEM_GPIO Gpio
);

VOID SbSetGpioLvlSel (
    IN AMI_OEM_GPIO Gpio,
    IN BOOLEAN      High
);

UINT32 SbReadAltGpiSmiSts (
    VOID
);

VOID SbClearAltGpiSmiSts (
    IN AMI_OEM_GPIO Gpio
);

VOID SbProgramAltGpiSmi (
    IN AMI_OEM_GPIO Gpio,
    IN BOOLEAN      Set
);

VOID SbProgramGpioRout (
    IN AMI_OEM_GPIO Gpio,
    IN UINT8        Mode
);

BOOLEAN IsTopSwapOn(
    VOID
);

VOID  SetTopSwap(
    IN BOOLEAN                      On
);

VOID SbProgramGpioRegister (
    IN AMI_OEM_GPIO   Gpio,
    IN GPIO_REG_TYPE  RegType,
    IN UINT32         AndData,
    IN UINT32         OrData
);

UINT32 SbReadGpioRegister (
    IN AMI_OEM_GPIO   Gpio,
    IN GPIO_REG_TYPE  RegType
);

EFI_STATUS SbGetMiscBitStatus(
    IN SB_MISC_TYPE         SbMiscType,
    IN OUT UINT8            *BitStatus
);

EFI_STATUS SbProgramMiscBit(
    IN SB_MISC_TYPE         SbMiscType,
    IN BOOLEAN              Set
);

BOOLEAN SbIsXhciRouting ( 
    VOID 
);

EFI_STATUS SbGetChipLanMacAddress ( 
    IN OUT UINT8            *MacAddress
);

EFI_STATUS ReadSpiDescriptor (
    IN UINT8                SectionSelect,
    IN UINT8                SectionIndex,
    OUT UINT32              *SectionData
);

#if SMBIOS_SUPPORT && SB_WAKEUP_TYPE_FN
UINT8 getWakeupTypeForSmbios(
    VOID
);
#endif

//---------------------------------------------------------------------------
// Chipset PCI Macros, Porting Required.
//---------------------------------------------------------------------------

#define READ_PCI8_SB(Rx)          READ_PCI8(LPC_BUS, LPC_DEVICE, LPC_FUNC, Rx)
#define WRITE_PCI8_SB(Rx, Val)    WRITE_PCI8(LPC_BUS, LPC_DEVICE, LPC_FUNC, Rx, Val)
#define SET_PCI8_SB(Rx, Set)      SET_PCI8(LPC_BUS, LPC_DEVICE, LPC_FUNC, Rx, Set)
#define RESET_PCI8_SB(Rx, Rst)    RESET_PCI8(LPC_BUS, LPC_DEVICE, LPC_FUNC, Rx, Rst)
#define RW_PCI8_SB(Rx, St, Rt)    RW_PCI8(LPC_BUS, LPC_DEVICE, LPC_FUNC, Rx, St, Rt)
#define READ_PCI16_SB(Rx)         READ_PCI16(LPC_BUS, LPC_DEVICE, LPC_FUNC, Rx)
#define WRITE_PCI16_SB(Rx, Val)   WRITE_PCI16(LPC_BUS, LPC_DEVICE, LPC_FUNC, Rx, Val)
#define SET_PCI16_SB(Rx, Set)     SET_PCI16(LPC_BUS, LPC_DEVICE, LPC_FUNC, Rx, Set)
#define RESET_PCI16_SB(Rx, Rst)   RESET_PCI16(LPC_BUS, LPC_DEVICE, LPC_FUNC, Rx, Rst)
#define RW_PCI16_SB(Rx, St, Rt)   RW_PCI16(LPC_BUS, LPC_DEVICE, LPC_FUNC, Rx, St, Rt)
#define READ_PCI32_SB(Rx)         READ_PCI32(LPC_BUS, LPC_DEVICE, LPC_FUNC, Rx)
#define WRITE_PCI32_SB(Rx, Val)   WRITE_PCI32(LPC_BUS, LPC_DEVICE, LPC_FUNC, Rx, Val)
#define SET_PCI32_SB(Rx, Set)     SET_PCI32(LPC_BUS, LPC_DEVICE, LPC_FUNC, Rx, Set)
#define RESET_PCI32_SB(Rx, Rst)   RESET_PCI32(LPC_BUS, LPC_DEVICE, LPC_FUNC, Rx, Rst)
#define RW_PCI32_SB(Rx, St, Rt)   RW_PCI32(LPC_BUS, LPC_DEVICE, LPC_FUNC, Rx, St, Rt)

#ifdef __cplusplus
}
#endif
#endif


//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
