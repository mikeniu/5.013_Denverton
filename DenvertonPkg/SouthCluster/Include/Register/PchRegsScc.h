/** @file
  Register names for SCC module.

  Conventions:

  - Prefixes:
    Definitions beginning with "R_" are registers
    Definitions beginning with "B_" are bits within registers
    Definitions beginning with "V_" are meaningful values of bits within the registers
    Definitions beginning with "S_" are register sizes
    Definitions beginning with "N_" are the bit position
  - In general, PCH registers are denoted by "_PCH_" in register names
  - Registers / bits that are different between PCH generations are denoted by
    "_PCH_<generation_name>_" in register/bit names. e.g., "_PCH_CHV_"
  - Registers / bits that are different between SKUs are denoted by "_<SKU_name>"
    at the end of the register/bit names
  - Registers / bits of new devices introduced in a PCH generation will be just named
    as "_PCH_" without <generation_name> inserted.

@copyright
  Copyright (c) 1999 - 2015 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
#ifndef _PCH_REGS_SCC_H_
#define _PCH_REGS_SCC_H_


//
// SCC Modules Registers
//

//
// SCC SDIO Modules
// PCI Config Space Registers
//
#define PCI_BUS_NUMBER_PCH_SCC_EMMC              0
#define PCI_DEVICE_NUMBER_PCH_SCC_SDIO_0         28
#define PCI_DEVICE_NUMBER_PCH_SCC_SDIO_1         17
#define PCI_DEVICE_NUMBER_PCH_SCC_SDIO_2         18
#define PCI_FUNCTION_NUMBER_PCH_SCC_SDIO         0

#define PCI_DEVICE_NUMBER_PCH_SCC_EMMC           28
#define PCI_FUNCTION_NUMBER_PCH_SCC_EMMC         0

#define R_PCH_SCC_SDIO_DEVVENDID                 0x00  // Device ID & Vendor ID
#define B_PCH_SCC_SDIO_DEVVENDID_DID             0xFFFF0000 // Device ID
#define B_PCH_SCC_SDIO_DEVVENDID_VID             0x0000FFFF // Vendor ID

#define R_PCH_SCC_SDIO_STSCMD                    0x04  // Status & Command
#define B_PCH_SCC_SDIO_STSCMD_RMA                BIT29 // RMA
#define B_PCH_SCC_SDIO_STSCMD_RCA                BIT28 // RCA
#define B_PCH_SCC_SDIO_STSCMD_CAPLIST            BIT20 // Capability List
#define B_PCH_SCC_SDIO_STSCMD_INTRSTS            BIT19 // Interrupt Status
#define B_PCH_SCC_SDIO_STSCMD_INTRDIS            BIT10 // Interrupt Disable
#define B_PCH_SCC_SDIO_STSCMD_SERREN             BIT8  // SERR# Enable
#define B_PCH_SCC_SDIO_STSCMD_BME                BIT2  // Bus Master Enable
#define B_PCH_SCC_SDIO_STSCMD_MSE                BIT1  // Memory Space Enable

#define R_PCH_SCC_SDIO_REVCC                     0x08  // Revision ID & Class Code
#define B_PCH_SCC_SDIO_REVCC_CC                  0xFFFFFF00 // Class Code
#define B_PCH_SCC_SDIO_REVCC_RID                 0x000000FF // Revision ID

#define R_PCH_SCC_SDIO_CLHB                      0x0C
#define B_PCH_SCC_SDIO_CLHB_MULFNDEV             BIT23 // Multi Function Device
#define B_PCH_SCC_SDIO_CLHB_HT                   0x007F0000 // Header Type
#define B_PCH_SCC_SDIO_CLHB_LT                   0x0000FF00 // Latency Timer
#define B_PCH_SCC_SDIO_CLHB_CLS                  0x000000FF // Cache Line Size

#define R_PCH_SCC_SDIO_BAR                       0x10  // BAR
#define B_PCH_SCC_SDIO_BAR_BA                    0xFFFFF000 // Base Address
#define V_PCH_SCC_SDIO_BAR_SIZE                  0x1000
#define N_PCH_SCC_SDIO_BAR_ALIGNMENT             12
#define B_PCH_SCC_SDIO_BAR_SI                    0x00000FF0 // Size Indicator
#define B_PCH_SCC_SDIO_BAR_PF                    BIT3  // Prefetchable
#define B_PCH_SCC_SDIO_BAR_TYPE                  (BIT2 | BIT1) // Type
#define B_PCH_SCC_SDIO_BAR_MS                    BIT0  // Message Space

#define R_PCH_SCC_SDIO_BAR_HIGH                  0x14  // BAR High
#define B_PCH_SCC_SDIO_BAR_HIGH_BA               0xFFFFFFFF // Base Address

#define R_PCH_SCC_SDIO_BAR1                      0x18  // BAR 1
#define B_PCH_SCC_SDIO_BAR1_BA                   0xFFFFF000 // Base Address
#define V_PCH_SCC_SDIO_BAR1_SIZE                 0x1000
#define B_PCH_SCC_SDIO_BAR1_SI                   0x00000FF0 // Size Indicator
#define B_PCH_SCC_SDIO_BAR1_PF                   BIT3  // Prefetchable
#define B_PCH_SCC_SDIO_BAR1_TYPE                 (BIT2 | BIT1) // Type
#define B_PCH_SCC_SDIO_BAR1_MS                   BIT0  // Message Space

#define R_PCH_SCC_SDIO_BAR1_HIGH                 0x1C  // BAR 1 High
#define B_PCH_SCC_SDIO_BAR1_HIGH_BA              0xFFFFFFFF // Base Address

#define R_PCH_SCC_SDIO_SSID                      0x2C  // Sub System ID
#define B_PCH_SCC_SDIO_SSID_SID                  0xFFFF0000 // Sub System ID
#define B_PCH_SCC_SDIO_SSID_SVID                 0x0000FFFF // Sub System Vendor ID

#define R_PCH_SCC_SDIO_ERBAR                     0x30  // Expansion ROM BAR
#define B_PCH_SCC_SDIO_ERBAR_BA                  0xFFFFFFFF // Expansion ROM Base Address

#define R_PCH_SCC_SDIO_CAPPTR                    0x34  // Capability Pointer
#define B_PCH_SCC_SDIO_CAPPTR_CPPWR              0xFF  // Capability Pointer Power

#define R_PCH_SCC_SDIO_INTR                      0x3C  // Interrupt
#define B_PCH_SCC_SDIO_INTR_ML                   0xFF000000 // Max Latency
#define B_PCH_SCC_SDIO_INTR_MG                   0x00FF0000
#define B_PCH_SCC_SDIO_INTR_IP                   0x00000F00 // Interrupt Pin
#define B_PCH_SCC_SDIO_INTR_IL                   0x000000FF // Interrupt Line

#define R_PCH_SCC_SDIO_PCAPID                    0x80  // Power Capability ID
#define B_PCH_SCC_SDIO_PCAPID_PS                 0xF8000000 // PME Support
#define B_PCH_SCC_SDIO_PCAPID_VS                 0x00070000 // Version
#define B_PCH_SCC_SDIO_PCAPID_NC                 0x0000FF00 // Next Capability
#define B_PCH_SCC_SDIO_PCAPID_PC                 0x000000FF // Power Capability

#define R_PCH_SCC_SDIO_PCS                       0x84  // PME Control Status
#define B_PCH_SCC_SDIO_PCS_PMESTS                BIT15 // PME Status
#define B_PCH_SCC_SDIO_PCS_PMEEN                 BIT8  // PME Enable
#define B_PCH_SCC_SDIO_PCS_NSS                   BIT3  // No Soft Reset
#define B_PCH_SCC_SDIO_PCS_PS                    (BIT1 | BIT0) // Power State

#define R_PCH_SCC_SDIO_MANID                     0xF8  // Manufacturer ID
#define B_PCH_SCC_SDIO_MANID_MANID               0xFFFFFFFF // Manufacturer ID

//
// SCC SDIO Module
// MMIO Space Register
//
#define R_PCH_SCC_SDIO_MEM_TIMEOUT_CTL           0x2E  // Timeout Control
#define B_PCH_SCC_SDIO_MEM_TIMEOUT_CTL_DTCV      0x0F  // Data Timeout Counter Value

#define R_PCH_SCC_SDIO_MEM_CESHC2                0x3C  // Auto CMD12 Error Status Register & Host Control 2
#define B_PCH_SCC_SDIO_MEM_CESHC2_ASYNC_INT      BIT30 // Asynchronous Interrupt Enable


#define R_SCC_MEM_SW_LTR_VALUE                              0x804  // Software LTR Register
#define R_SCC_MEM_AUTO_LTR_VALUE                            0x808  // Auto LTR Value
#define R_SCC_MEM_CAP_BYPASS_CNTL                           0x810  // Capabilities Bypass Control
#define V_SCC_MEM_CAP_BYPASS_CNTL_EN                        0x5A
#define R_SCC_MEM_CAP_BYPASS_REG1                           0x814  // Capabilities Bypass Register 1
#define V_SCC_MEM_CAP_BYPASS_REG1_DEFAULTS                  0x3C80EB1E
#define B_SCC_MEM_CAP_BYPASS_REG1_TIMER_COUNT               (BIT20 | BIT19 | BIT18 | BIT17)
#define N_SCC_MEM_CAP_BYPASS_REG1_TIMER_COUNT               17
#define V_SCC_MEM_CAP_BYPASS_REG1_TIMER_COUNT               0x8
#define R_SCC_MEM_CAP_BYPASS_REG2                           0x818  // Capabilities Bypass Register 2
#define V_SCC_MEM_CAP_BYPASS_REG2_DEFAULTS                  0x040040C8
#define R_SCC_MEM_IDLE_CTRL                                 0x81C  // DevIdle Control per SCC slice
#define B_SCC_MEM_CAP_BYPASS_REG1_HS400                     BIT29
#define R_SCC_MEM_TX_CMD_DLL_CNTL                           0x820  // Tx CMD Path Ctrl
#define R_SCC_MEM_TX_DATA_DLL_CNTL1                         0x824  // Tx Data Path Ctrl 1
#define R_SCC_MEM_TX_DATA_DLL_CNTL2                         0x828  // Tx Data Path Ctrl 2
#define R_SCC_MEM_RX_CMD_DATA_DLL_CNTL1                     0x82C  // Rx CMD&Data Path Ctrl 1
#define R_SCC_MEM_RX_STROBE_DLL_CNTL                        0x830  // Rx Strobe Ctrl Path
#define R_SCC_MEM_RX_CMD_DATA_DLL_CNTL2                     0x834  // Rx CMD&Data Path Ctrl 2
#define N_SCC_MEM_RX_CMD_DATA_DLL_CNTL2_CLKSRC_RX           16
#define V_SCC_MEM_RX_CMD_DATA_DLL_CNTL2_CLKSRC_RX_CLK_AUTO  0x2
#define R_SCC_MEM_MASTER_DLL_SW_CNTL                        0x838  // Master DLL Software Ctrl

///
/// SCC Private Space
///
#define PCH_SCC_EP_PORT_ID                       0xD6  // SCC EP Private Space PortID
#define PCH_SCC_EP_PRIVATE_READ_OPCODE           0x06  // CUnit to SCC EP Private Space Read Opcode
#define PCH_SCC_EP_PRIVATE_WRITE_OPCODE          0x07  // CUnit to SCC EP Private Space Write Opcode

#define R_PCH_SCC_EP_IOSFCTL                     0x00  // IOSF Control
#define B_PCH_SCC_EP_IOSFCTL_NSNPDIS             BIT7  // Non-Snoop Disable

#define N_PCH_SCC_EP_PCICFGCTR1_IPIN1            8
#define N_PCH_SCC_EP_PCICFGCTR1_PCI_IRQ          20
#define N_PCH_SCC_EP_PCICFGCTR1_ACPI_IRQ         12
#define B_PCH_SCC_EP_PCICFGCTR1_PCI_IRQ          BIT1
#define B_PCH_SCC_EP_PCICFGCTR1_ACPI_IRQ         BIT0

#define R_PCH_SCC_EP_PCICFGCTR1                  0x200 // PCI Configuration Control 1 - eMMC
#define B_PCH_SCC_EP_PCICFGCTR1_PCI_IRQ1         BIT24 // PCI_IRQ1
#define M_PCH_SCC_EP_PCICFGCTR1_PCI_IRQ1         0xF00FFFFF// PCI_IRQ1 [27:20]
#define B_PCH_SCC_EP_PCICFGCTR1_IPIN1            (BIT11 | BIT10 | BIT9 | BIT8) // Interrupt Pin
#define B_PCH_SCC_EP_PCICFGCTR1_B1D1             BIT7  // BAR 1 Disable
#define B_PCH_SCC_EP_PCICFGCTR1_PS               0x7C  // PME Support
#define B_PCH_SCC_EP_PCICFGCTR1_ACPI_INT_EN1     BIT1  // ACPI Interrupt Enable
#define B_PCH_SCC_EP_PCICFGCTR1_PCI_CFG_DIS1     BIT0  // PCI Configuration Space Disable

#define R_PCH_SCC_EP_PCICFGCTR2                  0x204 // PCI Configuration Control 2 - SD Card
#define B_PCH_SCC_EP_PCICFGCTR2_IPIN1            (BIT11 | BIT10 | BIT9 | BIT8) // Interrupt Pin
#define B_PCH_SCC_EP_PCICFGCTR2_B1D1             BIT7  // BAR 1 Disable
#define B_PCH_SCC_EP_PCICFGCTR2_PS               0x7C  // PME Support
#define B_PCH_SCC_EP_PCICFGCTR2_ACPI_INT_EN1     BIT1  // ACPI Interrupt Enable
#define B_PCH_SCC_EP_PCICFGCTR2_PCI_CFG_DIS1     BIT0  // PCI Configuration Space Disable

#define R_PCH_SCC_EP_PCICFGCTR3                  0x208 // PCI Configuration Control 3 - SDIO
#define B_PCH_SCC_EP_PCICFGCTR3_IPIN1            (BIT11 | BIT10 | BIT9 | BIT8) // Interrupt Pin
#define B_PCH_SCC_EP_PCICFGCTR3_B1D1             BIT7  // BAR 1 Disable
#define B_PCH_SCC_EP_PCICFGCTR3_PS               0x7C  // PME Support
#define B_PCH_SCC_EP_PCICFGCTR3_ACPI_INT_EN1     BIT1  // ACPI Interrupt Enable
#define B_PCH_SCC_EP_PCICFGCTR3_PCI_CFG_DIS1     BIT0  // PCI Configuration Space Disable

#define B_PCH_PMC_FUNC_DIS_SDIO3                  BIT10 // SCC SDIO #3 (Device 18, SD Card) Disable
#define B_PCH_PMC_FUNC_DIS_SDIO2                  BIT9  // SCC SDIO #2 (Device 17, SDIO) Disable
#define B_PCH_PMC_FUNC_DIS_SDIO1                  BIT8  // SCC SDIO #1 (Device 16, eMMC) Disable
#endif
