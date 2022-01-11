/** @file
  Register definition for PSF component

  Conventions:

  - Prefixes:
    Definitions beginning with "R_" are registers
    Definitions beginning with "B_" are bits within registers
    Definitions beginning with "V_" are meaningful values within the bits
    Definitions beginning with "S_" are register sizes
    Definitions beginning with "N_" are the bit position
  - In general, PCH registers are denoted by "_PCH_" in register names
  - Registers / bits that are different between PCH generations are denoted by
    "_PCH_[generation_name]_" in register/bit names.
  - Registers / bits that are specific to PCH-H denoted by "_H_" in register/bit names.
    Registers / bits that are specific to PCH-LP denoted by "_LP_" in register/bit names.
    e.g., "_PCH_H_", "_PCH_LP_"
    Registers / bits names without _H_ or _LP_ apply for both H and LP.
  - Registers / bits that are different between SKUs are denoted by "_[SKU_name]"
    at the end of the register/bit names
  - Registers / bits of new devices introduced in a PCH generation will be just named
    as "_PCH_" without [generation_name] inserted.

@copyright
 Copyright (c) 2014 - 2015 Intel Corporation. All rights reserved
 This software and associated documentation (if any) is furnished
 under a license and may only be used or copied in accordance
 with the terms of the license. Except as permitted by the
 license, no part of this software or documentation may be
 reproduced, stored in a retrieval system, or transmitted in any
 form or by any means without the express written consent of
 Intel Corporation.
 This file contains an 'Intel Peripheral Driver' and is uniquely
 identified as "Intel Reference Module" and is licensed for Intel
 CPUs and chipsets under the terms of your license agreement with
 Intel or your vendor. This file may be modified by the user, subject
 to additional terms of the license agreement.

@par Specification Reference:
**/
#ifndef _PCH_REGS_PSF_H_
#define _PCH_REGS_PSF_H_

//
// Private chipset regsiter (Memory space) offset definition
// The PCR register defines is used for PCR MMIO programming and PCH SBI programming as well.
//

//
// PSFx segment registers
//
#define R_PCH_PCR_PSF_GLOBAL_CONFIG                     0x4000                  ///< PSF Segment Global Configuration Register
#define B_PCH_PCR_PSF_GLOBAL_CONFIG_ENTCG               BIT4
#define B_PCH_PCR_PSF_GLOBAL_CONFIG_ENLCG               BIT3
#define R_PCH_PCR_PSF_ROOTSPACE_CONFIG_RS0              0x4014                  ///< PSF Segment Rootspace Configuration Register
#define B_PCH_PCR_PSF_ROOTSPACE_CONFIG_RS0_ENADDRP2P    BIT1
#define B_PCH_PCR_PSF_ROOTSPACE_CONFIG_RS0_VTDEN        BIT0

#define R_PCH_PCR_PSF_PORT_CONFIG_PG0_PORT0             0x4020                  ///< PSF Segment Port Configuration Register

#define S_PCH_PSF_DEV_GNTCNT_RELOAD_DGCR                4
#define S_PCH_PSF_TARGET_GNTCNT_RELOAD                  4
#define B_PCH_PSF_DEV_GNTCNT_RELOAD_DGCR_GNT_CNT_RELOAD 0x3F
#define B_PCH_PSF_TARGET_GNTCNT_RELOAD_GNT_CNT_RELOAD   0x3F

//
// PSFx PCRs definitions
//
#define R_PCH_PCR_PSFX_T0_SHDW_BAR0                     0                       ///< PCI BAR0
#define R_PCH_PCR_PSFX_T0_SHDW_BAR1                     0x04                    ///< PCI BAR1
#define R_PCH_PCR_PSFX_T0_SHDW_BAR2                     0x08                    ///< PCI BAR2
#define R_PCH_PCR_PSFX_T0_SHDW_BAR3                     0x0C                    ///< PCI BAR3
#define R_PCH_PCR_PSFX_T0_SHDW_BAR4                     0x10                    ///< PCI BAR4
#define R_PCH_PCR_PSFX_T0_SHDW_BAR5                     0x14                    ///< PCI BAR5
#define R_PCH_PCR_PSFX_T0_SHDW_PCIEN                    0x1C                    ///< PCI configuration space enable bits
#define B_PCH_PCR_PSFX_T0_SHDW_PCIEN_BAR0DIS            BIT16                   ///< Disable BAR0
#define B_PCH_PCR_PSFX_T0_SHDW_PCIEN_BAR1DIS            BIT17                   ///< Disable BAR1
#define B_PCH_PCR_PSFX_T0_SHDW_PCIEN_BAR2DIS            BIT18                   ///< Disable BAR2
#define B_PCH_PCR_PSFX_T0_SHDW_PCIEN_BAR3DIS            BIT19                   ///< Disable BAR3
#define B_PCH_PCR_PSFX_T0_SHDW_PCIEN_BAR4DIS            BIT20                   ///< Disable BAR4
#define B_PCH_PCR_PSFX_T0_SHDW_PCIEN_BAR5DIS            BIT21                   ///< Disable BAR5
#define B_PCH_PCR_PSFX_T0_SHDW_PCIEN_FUNDIS             BIT8                    ///< Function disable
#define B_PCH_PCR_PSFX_T0_SHDW_PCIEN_MEMEN              BIT1                    ///< Memory decoding enable
#define B_PCH_PCR_PSFX_T0_SHDW_PCIEN_IOEN               BIT0                    ///< IO decoding enable
#define R_PCH_PCR_PSFX_T0_SHDW_PMCSR                    0x20                    ///< PCI power management configuration
#define B_PCH_PCR_PSFX_T0_SHDW_PMCSR_PWRST              (BIT1 | BIT0)           ///< Power status
#define R_PCH_PCR_PSFX_T0_SHDW_CFG_DIS                  0x38                    ///< PCI configuration disable
#define B_PCH_PCR_PSFX_T0_SHDW_CFG_DIS_CFGDIS           BIT0                    ///< config disable

#define R_PCH_PCR_PSFX_T1_SHDW_PCIEN                    0x3C                    ///< PCI configuration space enable bits
#define B_PCH_PCR_PSFX_T1_SHDW_PCIEN_FUNDIS             BIT8                    ///< Function disable
#define B_PCH_PCR_PSFX_T1_SHDW_PCIEN_MEMEN              BIT1                    ///< Memory decoding enable
#define B_PCH_PCR_PSFX_T1_SHDW_PCIEN_IOEN               BIT0                    ///< IO decoding enable

#define B_PCH_PCR_PSFX_TX_AGENT_FUNCTION_CONFIG_DEVICE          0x01F0                 ///< device number
#define N_PCH_PCR_PSFX_TX_AGENT_FUNCTION_CONFIG_DEVICE          4
#define B_PCH_PCR_PSFX_TX_AGENT_FUNCTION_CONFIG_FUNCTION        (BIT3 | BIT2 | BIT1)   ///< function number
#define N_PCH_PCR_PSFX_TX_AGENT_FUNCTION_CONFIG_FUNCTION        1


//
// PSF1 PCRs
//
#define R_PCH_PCR_PSF1_T0_SHDW_TRACE_HUB_REG_BASE          0x0100                  ///< D31F7 PSF base address (TraceHub PCI)
#define R_PCH_PCR_PSF1_T0_SHDW_TRACE_HUB_ACPI_REG_BASE     0x0200                  ///< D31F6 PSF base address ((TraceHub ACPI)
#define R_PCH_PCR_PSF1_T0_SHDW_IEPMT_REG_BASE        0x0400                  ///< D27F5 PSF base address (IE: PMT)
#define R_PCH_PCR_PSF1_T0_SHDW_PMT_REG_BASE          0x0500                  ///< D25F2 PSF base address (CSME: PMT)
#define R_PCH_PCR_PSF1_T1_SHDW_PCIE09_REG_BASE       0x2000                  ///< D09F0 PSF base address (PCIE PORT 01)
#define R_PCH_PCR_PSF1_T1_SHDW_PCIE10_REG_BASE       0x2100                  ///< D10F0 PSF base address (PCIE PORT 02)
#define R_PCH_PCR_PSF1_T1_SHDW_PCIE11_REG_BASE       0x2200                  ///< D11F0 PSF base address (PCIE PORT 03)
#define R_PCH_PCR_PSF1_T1_SHDW_PCIE12_REG_BASE       0x2300                  ///< D12F0 PSF base address (PCIE PORT 04)
#define R_PCH_PCR_PSF1_T1_SHDW_PCIE14_REG_BASE       0x2500                  ///< D14F0 PSF base address (PCIE PORT 05)
#define R_PCH_PCR_PSF1_T1_SHDW_PCIE15_REG_BASE       0x2600                  ///< D15F0 PSF base address (PCIE PORT 06)
#define R_PCH_PCR_PSF1_T1_SHDW_PCIE16_REG_BASE       0x2700                  ///< D16F0 PSF base address (PCIE PORT 07)
#define R_PCH_PCR_PSF1_T1_SHDW_PCIE17_REG_BASE       0x2800                  ///< D17F0 PSF base address (PCIE PORT 08)


//PSF 1 Multicast Message Configuration

#define R_PCH_PCR_PSF1_RC_OWNER_RS0                                 0x4008      ///< Destination ID




//
// controls the PCI configuration header of a PCI function
//
#define R_PCH_PCR_PSF1_T1_AGENT_FUNCTION_CONFIG_PCIE80_RS0_D9_F0   0x40DC   ///< SPA
#define R_PCH_PCR_PSF1_T1_AGENT_FUNCTION_CONFIG_PCIE80_RS0_D10_F0   0x40D8   ///< SPA
#define R_PCH_PCR_PSF1_T1_AGENT_FUNCTION_CONFIG_PCIE80_RS0_D11_F0   0x40D4   ///< SPA
#define R_PCH_PCR_PSF1_T1_AGENT_FUNCTION_CONFIG_PCIE80_RS0_D12_F0   0x40D0   ///< SPA
#define R_PCH_PCR_PSF1_T1_AGENT_FUNCTION_CONFIG_PCIE81_RS0_D14_F0   0x40C8   ///< SPB
#define R_PCH_PCR_PSF1_T1_AGENT_FUNCTION_CONFIG_PCIE81_RS0_D15_F0   0x40C4   ///< SPB
#define R_PCH_PCR_PSF1_T1_AGENT_FUNCTION_CONFIG_PCIE81_RS0_D16_F0   0x40C0   ///< SPB
#define R_PCH_PCR_PSF1_T1_AGENT_FUNCTION_CONFIG_PCIE81_RS0_D17_F0   0x40BC   ///< SPB

//
// PSF1 grant count registers
//
#define R_PCH_PCR_PSF1_DEV_GNTCNT_RELOAD_DGCR0           0x40E4
#define R_PCH_PCR_PSF1_TARGET_GNTCNT_RELOAD_PG1_TGT0     0x4250

//
// PSF2 PCRs (PID:PSF2)
//
#define R_PCH_PCR_PSF2_T0_SHDW_XHCI_REG_BASE           0x0100                  ///< D21F0 PSF base address (XHCI)
#define R_PCH_PCR_PSF2_T0_SHDW_IEPMT_REG_BASE          0x0200                  ///< D27F5 PSF base address (IE: PMT)
#define R_PCH_PCR_PSF2_T0_SHDW_FTPM_REG_BASE           0x0800                  ///< D24F7 PSF base address (CSME: fTPM)
#define R_PCH_PCR_PSF2_T0_SHDW_SATA0_REG_BASE          0x0A00                  ///< D19F0 PSF base address (SATA0)
#define R_PCH_PCR_PSF2_T0_SHDW_SATA1_REG_BASE          0x0B00                  ///< D20F0 PSF base address (SATA1)
// It seems that there are some inconsistency between this header and documentation.
// Changes below are add according to values from documentation in version: IP release: psf/v14ww50a
#define R_PCH_PCR_PSF2_T0_SHDW_HECI1_REG_BASE           0x0700                  ///< D27F0 PSF base address (ME: HECI1)
#define R_PCH_PCR_PSF2_T0_SHDW_HECI2_REG_BASE           0x0600                  ///< D27F1 PSF base address (ME: HECI2)
#define R_PCH_PCR_PSF2_T0_SHDW_IDER_REG_BASE            0x0400                  ///< D27F2 PSF base address (ME: IDER)
#define R_PCH_PCR_PSF2_T0_SHDW_KT_REG_BASE              0x0300                  ///< D27F3 PSF base address (ME: KT)
#define R_PCH_PCR_PSF2_T0_SHDW_HECI3_REG_BASE           0x0500                  ///< D27F4 PSF base address (ME: HECI3)

// Other PSF2 PCRs definition
#define R_PCH_H_PCR_PSF2_T0_SHDW_SATA_VS_CAP_VR_RS0_D19_F0    0x0A24
#define R_PCH_H_PCR_PSF2_T0_SHDW_SATA_MMIOPI_VR_RS0_D19_F0    0x0A30
#define R_PCH_H_PCR_PSF2_T0_SHDW_SATA_VS_CAP_VR_RS0_D20_F0    0x0B24
#define R_PCH_H_PCR_PSF2_T0_SHDW_SATA_MMIOPI_VR_RS0_D20_F0    0x0B30

//
// PSF3 PCRs (PID:PSF3)
//
#define R_PCH_PCR_PSF3_T0_SHDW_SPI_RS0_D31F5_REG_BASE            0x0100                  ///< D31F5 PSF base address (SPI)
#define R_PCH_PCR_PSF3_T0_SHDW_SPI_RS0_D31F0_REG_BASE            0x0200                  ///< D31F5 PSF base address (SPI)
#define R_PCH_PCR_PSF3_T0_SHDW_SPI_RS1_D25F0_REG_BASE            0x0300                  ///< D31F5 PSF base address (SPI)
#define R_PCH_PCR_PSF3_T0_SHDW_SPI_RS1_D25F1_REG_BASE            0x0400                  ///< D31F5 PSF base address (SPI)
#define R_PCH_PCR_PSF3_T0_SHDW_SPI_RS2_D25F0_REG_BASE            0x0500                  ///< D31F5 PSF base address (SPI)
#define R_PCH_PCR_PSF3_T0_SHDW_SPI_RS2_D25F1_REG_BASE            0x0600                  ///< D31F5 PSF base address (SPI)
#define R_PCH_PCR_PSF3_T0_SHDW_PMC_RS0_D31F2_REG_BASE            0x0900                  ///< D31F2 PSF base address (PMC)
#define R_PCH_PCR_PSF3_T0_SHDW_PMC_RS1_D26F0_REG_BASE            0x0A00                  ///< D31F2 PSF base address (PMC)
#define R_PCH_PCR_PSF3_T0_SHDW_PMC_RS2_D26F0_REG_BASE            0x0B00                  ///< D31F2 PSF base address (PMC)
#define R_PCH_PCR_PSF3_T0_SHDW_LPC_REG_BASE            0x0700                  ///< D31F0 PSF base address (LPC)
#define R_PCH_PCR_PSF3_T0_SHDW_P2SB_REG_BASE           0x0800                  ///< D31F1 PSF base address (P2SB)
#define R_PCH_PCR_PSF3_T0_SHDW_LEGSMBUS_REG_BASE       0x0C00                  ///< D31F4 PSF base address (Legacy SMBUS)
#define R_PCH_PCR_PSF3_T0_SHDW_SMBUS_REG_BASE          0x0D00                  ///< D18F0 PSF base address (SMBUS)
#define R_PCH_PCR_PSF3_T0_SHDW_EMMC_REG_BASE           0x0E00                  ///< D28F0 PSF base address (EMMC)
#define R_PCH_PCR_PSF3_T0_SHDW_HSUART1_REG_BASE        0x0F00                  ///< D26F0 PSF base address (HSUAET #1)
#define R_PCH_PCR_PSF3_T0_SHDW_HSUART2_REG_BASE        0x1000                  ///< D26F1 PSF base address (HSUAET #2)
#define R_PCH_PCR_PSF3_T0_SHDW_HSUART3_REG_BASE        0x1100                  ///< D26F2 PSF base address (HSUAET #3)
#define R_PCH_PCR_PSF3_T0_SHDW_KT_REG_BASE             0x1200                  ///< D27F3 PSF base address (IE: KT redirection)
#define R_PCH_PCR_PSF3_T0_SHDW_IDER_REG_BASE           0x1300                  ///< D27F2 PSF base address (IE: PTIO: IDEr)
#define R_PCH_PCR_PSF3_T0_SHDW_HECI3_REG_BASE          0x1400                  ///< D27F4 PSF base address (IE: HECI #3)
#define R_PCH_PCR_PSF3_T0_SHDW_HECI2_REG_BASE          0x1500                  ///< D27F1 PSF base address (IE: HECI #2)
#define R_PCH_PCR_PSF3_T0_SHDW_HECI1_REG_BASE          0x1600                  ///< D27F0 PSF base address (IE: HECI #1)

#endif
