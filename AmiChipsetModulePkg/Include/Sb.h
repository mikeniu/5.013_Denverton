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

#ifndef  _SB_H   //To Avoid this header get compiled twice
#define  _SB_H

/** @file Sb.h
    This header file contains South bridge(LPC) related structure and
    constant definitions.

*/

#include <Token.h>

#if SB_DEVICE_REGS_RC_DEFINITION_SUPPORT
#include SB_DEVICE_REGS_RC_DEFINITION_HEADER
#endif

//----------------------------------------------------------------------------
#define SB_PCIE_CFG_ADDRESS(bus, dev, func, reg) \
    ((VOID*)(UINTN)(PcdGet64 (PcdPciExpressBaseAddress) + ((UINT8)(bus & 0xff) << 20) + ((UINT8)(dev & 0x1f) << 15) + ((UINT8)(func & 0x07) << 12) + (reg & 0xfff)))

// Denverton AptioV override Start - EIP#235471
#define SB_PCI_CFG_ADDRESS(bus, dev, func, reg) \
    ((UINT32)( 0x80000000 + ((UINT8)(bus & 0xff) << 16) + ((UINT8)(dev & 0x1f) << 11) + ((UINT8)(func & 0x07) << 8) + (reg & 0xff)))
// Denverton AptioV override End - EIP#235471

// Denverton AptioV Override Start - EIP#305851
#define R_PCH_PCIE_PLKCTL                         0xEA // Lock Key Register for all RW-L Registers
#define B_PCH_PCIE_PLKCTL_CL                      BIT0 // Lock Key bit for all RW-L 
// Denverton AptioV Override End - EIP#305851

// Denverton AptioV override Start - EIP#270210 - Macros for SSVID and SSID programming in Denverton
#define PCI_vRP_SVID                    0x008C        // Subsystem Vendor ID for the vRP(Virtual Root Port) registers 

//----------------------------------------------------------------------------
// INTEL PCH PCI Bus/Device/Function/Register Number Macros as per Denverton EDS
//----------------------------------------------------------------------------

#define GLREG_BUS                       0
#define GLREG_DEV                       0x4
#define GLREG_FUN                       0
#define RCEC_BUS                        0
#define RCEC_DEV                        0x5
#define RCEC_FUN                        0
#define QAT_BUS                         0
#define QAT_DEV                         0x6
#define QAT_FUN                         0
// Denverton AptioV Override  Start - EIP#294261 
#define RP0_BUS                         0
#define RP0_DEV                         0x9
#define RP0_FUN                         0
#define RP1_BUS                         0
#define RP1_DEV                         0xA
#define RP1_FUN                         0
#define RP2_BUS                         0
#define RP2_DEV                         0xB
#define RP2_FUN                         0
#define RP3_BUS                         0
#define RP3_DEV                         0xC
#define RP3_FUN                         0
#define RP4_BUS                         0
#define RP4_DEV                         0xE
#define RP4_FUN                         0
#define RP5_BUS                         0
#define RP5_DEV                         0xF
#define RP5_FUN                         0
#define RP6_BUS                         0
#define RP6_DEV                         0x10
#define RP6_FUN                         0
#define RP7_BUS                         0
#define RP7_DEV                         0x11
#define RP7_FUN                         0
// Denverton AptioV Override  End - EIP#294261 
#define SMB_BUS                         0
#define SMB_DEV                         0x12
#define SMB_FUN                         0
#define SATA0_BUS                       0
#define SATA0_DEV                       0x13
#define SATA0_FUN                       0
#define SATA1_BUS                       0
#define SATA1_DEV                       0x14
#define SATA1_FUN                       0
#define USB_BUS                         0
#define USB_DEV                         0x15
#define USB_FUN                         0
#define LAN0_BUS                        0
#define LAN0_DEV                        0x16
#define LAN0_FUN                        0
#define LAN1_BUS                        0
#define LAN1_DEV                        0x17
#define LAN1_FUN                        0
#define SYSMGT0_BUS                     0
#define SYSMGT0_DEV                     0x18
#define SYSMGT0_FUN                     0
#define SYSMGT1_BUS                     0
#define SYSMGT1_DEV                     0x18
#define SYSMGT1_FUN                     1
#define SYSMGT3_BUS                     0
#define SYSMGT3_DEV                     0x18
#define SYSMGT3_FUN                     3
#define SYSMGT4_BUS                     0
#define SYSMGT4_DEV                     0x18
#define SYSMGT4_FUN                     4
#define HSUART0_BUS                     0
#define HSUART0_DEV                     0x1A
#define HSUART0_FUN                     0
#define HSUART1_BUS                     0
#define HSUART1_DEV                     0x1A
#define HSUART1_FUN                     1
#define HSUART2_BUS                     0
#define HSUART2_DEV                     0x1A
#define HSUART2_FUN                     2
#define EMMC_BUS                        0
#define EMMC_DEV                        0x1C
#define EMMC_FUN                        0
#define LPC_BUS                         0
#define LPC_DEV                         0x1F
#define LPC_FUN                         0
#define P2SB_BUS                        0
#define P2SB_DEV                        0x1F
#define P2SB_FUN                        1
#define PMC_BUS                         0
#define PMC_DEV                         0x1F
#define PMC_FUN                         2
#define SMBUS_LEGACY_BUS                0
#define SMBUS_LEGACY_DEV                0x1F
#define SMBUS_LEGACY_FUN                4
#define SPI_BUS                         0
#define SPI_DEV                         0x1F
#define SPI_FUN                         5
#define TRACEHUB_BUS                    0
#define TRACEHUB_DEV                    0x1F
#define TRACEHUB_FUN                    7


#define GLREG_BUS_DEV_FUN                         EFI_PEI_PCI_CFG_ADDRESS(GLREG_BUS, GLREG_DEV, GLREG_FUN, 0)
#define RCEC_BUS_DEV_FUN                          EFI_PEI_PCI_CFG_ADDRESS(RCEC_BUS, RCEC_DEV, RCEC_FUN, 0)
#define QAT_BUS_DEV_FUN                           EFI_PEI_PCI_CFG_ADDRESS(QAT_BUS, QAT_DEV, QAT_FUN, 0)
// Denverton AptioV Override  Start - EIP#294261 
#define RP0_BUS_DEV_FUN                           EFI_PEI_PCI_CFG_ADDRESS(RP0_BUS, RP0_DEV, RP0_FUN, 0)
#define RP1_BUS_DEV_FUN                           EFI_PEI_PCI_CFG_ADDRESS(RP1_BUS, RP1_DEV, RP1_FUN, 0)
#define RP2_BUS_DEV_FUN                           EFI_PEI_PCI_CFG_ADDRESS(RP2_BUS, RP2_DEV, RP2_FUN, 0)
#define RP3_BUS_DEV_FUN                           EFI_PEI_PCI_CFG_ADDRESS(RP3_BUS, RP3_DEV, RP3_FUN, 0)
#define RP4_BUS_DEV_FUN                           EFI_PEI_PCI_CFG_ADDRESS(RP4_BUS, RP4_DEV, RP4_FUN, 0)
#define RP5_BUS_DEV_FUN                           EFI_PEI_PCI_CFG_ADDRESS(RP5_BUS, RP5_DEV, RP5_FUN, 0)
#define RP6_BUS_DEV_FUN                           EFI_PEI_PCI_CFG_ADDRESS(RP6_BUS, RP6_DEV, RP6_FUN, 0)
#define RP7_BUS_DEV_FUN                           EFI_PEI_PCI_CFG_ADDRESS(RP7_BUS, RP7_DEV, RP7_FUN, 0)
// Denverton AptioV Override  End - EIP#294261 
#define SMB_BUS_DEV_FUN                           EFI_PEI_PCI_CFG_ADDRESS(SMB_BUS, SMB_DEV, SMB_FUN, 0)
#define SATA0_BUS_DEV_FUN                         EFI_PEI_PCI_CFG_ADDRESS(SATA0_BUS, SATA0_DEV, SATA0_FUN, 0)
#define SATA1_BUS_DEV_FUN                         EFI_PEI_PCI_CFG_ADDRESS(SATA1_BUS, SATA1_DEV, SATA1_FUN, 0)
#define USB_BUS_DEV_FUN                           EFI_PEI_PCI_CFG_ADDRESS(USB_BUS, USB_DEV, USB_FUN, 0)
#define LAN0_BUS_DEV_FUN                          EFI_PEI_PCI_CFG_ADDRESS(LAN0_BUS, LAN0_DEV, LAN0_FUN, 0)
#define LAN1_BUS_DEV_FUN                          EFI_PEI_PCI_CFG_ADDRESS(LAN1_BUS, LAN1_DEV, LAN1_FUN, 0)
#define SYSMGT0_BUS_DEV_FUN                       EFI_PEI_PCI_CFG_ADDRESS(SYSMGT0_BUS, SYSMGT0_DEV, SYSMGT0_FUN, 0)
#define SYSMGT1_BUS_DEV_FUN                       EFI_PEI_PCI_CFG_ADDRESS(SYSMGT1_BUS, SYSMGT1_DEV, SYSMGT1_FUN, 0)
#define SYSMGT3_BUS_DEV_FUN                       EFI_PEI_PCI_CFG_ADDRESS(SYSMGT3_BUS, SYSMGT3_DEV, SYSMGT3_FUN, 0)
#define SYSMGT4_BUS_DEV_FUN                       EFI_PEI_PCI_CFG_ADDRESS(SYSMGT4_BUS, SYSMGT4_DEV, SYSMGT4_FUN, 0)
#define HSUART0_BUS_DEV_FUN                       EFI_PEI_PCI_CFG_ADDRESS(HSUART0_BUS, HSUART0_DEV, HSUART0_FUN, 0)
#define HSUART1_BUS_DEV_FUN                       EFI_PEI_PCI_CFG_ADDRESS(HSUART1_BUS, HSUART1_DEV, HSUART1_FUN, 0)
#define HSUART2_BUS_DEV_FUN                       EFI_PEI_PCI_CFG_ADDRESS(HSUART2_BUS, HSUART2_DEV, HSUART2_FUN, 0)
#define EMMC_BUS_DEV_FUN                          EFI_PEI_PCI_CFG_ADDRESS(EMMC_BUS, EMMC_DEV, EMMC_FUN, 0)
#define LPC_BUS_DEV_FUN                           EFI_PEI_PCI_CFG_ADDRESS(LPC_BUS, LPC_DEV, LPC_FUN, 0)
#define P2SB_BUS_DEV_FUN                          EFI_PEI_PCI_CFG_ADDRESS(P2SB_BUS, P2SB_DEV, P2SB_FUN, 0)
#define PMC_BUS_DEV_FUN                           EFI_PEI_PCI_CFG_ADDRESS(PMC_BUS, PMC_DEV, PMC_FUN, 0)
#define SMB_BUS_LEGACY_DEV_FUN                    EFI_PEI_PCI_CFG_ADDRESS(SMBUS_LEGACY_BUS, SMBUS_LEGACY_DEV, SMBUS_LEGACY_FUN, 0)
#define SPI_BUS_DEV_FUN                           EFI_PEI_PCI_CFG_ADDRESS(SPI_BUS, SPI_DEV, SPI_FUN, 0)
#define TRACEHUB_BUS_DEV_FUN                      EFI_PEI_PCI_CFG_ADDRESS(TRACEHUB_BUS, TRACEHUB_DEV, TRACEHUB_FUN, 0)
// Denverton AptioV override End - EIP#270210 - Macros for SSVID and SSID programming in Denverton

#define NUM_BITS_IN_ACPI_TIMER      24      // Programmed to 24 not 32
// This is the maximum possible bits in the timer.  Currently this is 32 according to the spec
#define MAX_ACPI_TIMER_BITS         32


// Denverton AptioV override Start - EIP#273894 - To solve 0xC3 unexpected reboot

#define DEFAULT_PCI_BUS_NUMBER_PCH  0
#define PCI_DEVICE_NUMBER_PCH_SMBUS           31
#define PCI_FUNCTION_NUMBER_PCH_SMBUS         4

#define R_PCH_SMBUS_TCOCTL  0x54
#define R_PCH_SMBUS_TCOBASE 0x50
#define B_PCH_SMBUS_TCOCTL_TCO_BASE_EN        BIT8
#define R_PCH_SMBUS_TCOBASE                   0x50
#define B_PCH_SMBUS_TCOBASE_BAR               0x0000FFE0

#define PCH_TCO_BASE_ADDRESS        0x400
#define R_PCH_TCO1_CNT    0x08
#define R_PCH_TCO2_STS    0x06
#define B_PCH_TCO_CNT_TMR_HLT                     BIT11
#define B_PCH_TCO2_STS_SECOND_TO                  BIT1

// Denverton AptioV override End - EIP#273894 - To solve 0xC3 unexpected reboot

#endif  // #ifndef  _SB_H_

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

