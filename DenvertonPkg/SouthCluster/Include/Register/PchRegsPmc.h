/** @file
  Register names for PCH PMC device

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
 Copyright (c) 1999 - 2017 Intel Corporation. All rights reserved
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

**/
#ifndef _PCH_REGS_PMC_H_
#define _PCH_REGS_PMC_H_

//
// PMC Registers (D31:F2)
//
#define PCI_DEVICE_NUMBER_PCH_PMC                           31
#define PCI_FUNCTION_NUMBER_PCH_PMC                         2

#define V_PCH_PMC_VENDOR_ID                                 V_PCH_INTEL_VENDOR_ID
#define V_PCH_H_PMC_DEVICE_ID                               0x9D21
#define V_PCH_LP_PMC_DEVICE_ID                              0x9D21
#define R_PCH_PMC_PM_DATA_BAR                               0x10
#define B_PCH_PMC_PM_DATA_BAR                               0xFFFFC000
#define R_PCH_PMC_ACPI_BASE                                 0x40
#define B_PCH_PMC_ACPI_BASE_BAR                             0xFFFC
#define R_PCH_PMC_ACPI_CNT                                  0x44
#define B_PCH_PMC_ACPI_CNT_PWRM_EN                          BIT8                          ///< PWRM enable
#define B_PCH_PMC_ACPI_CNT_ACPI_EN                          BIT7                          ///< ACPI eanble
#define B_PCH_PMC_ACPI_CNT_SCIS                             (BIT2 | BIT1 | BIT0)          ///< SCI IRQ select
#define V_PCH_PMC_ACPI_CNT_SCIS_IRQ9                        0
#define V_PCH_PMC_ACPI_CNT_SCIS_IRQ10                       1
#define V_PCH_PMC_ACPI_CNT_SCIS_IRQ11                       2
#define V_PCH_PMC_ACPI_CNT_SCIS_IRQ20                       4
#define V_PCH_PMC_ACPI_CNT_SCIS_IRQ21                       5
#define V_PCH_PMC_ACPI_CNT_SCIS_IRQ22                       6
#define V_PCH_PMC_ACPI_CNT_SCIS_IRQ23                       7
#define R_PCH_PMC_PWRM_BASE                                 0x48
#define B_PCH_PMC_PWRM_BASE_BAR                   0xFFFFF000                    ///< PWRM Base Bar is 4K aligned
#define R_PCH_PMC_GEN_PMCON_A                               0xA0
#define B_PCH_PMC_GEN_PMCON_A_DC_PP_DIS                     BIT30
#define B_PCH_PMC_GEN_PMCON_A_DSX_PP_DIS                    BIT29
#define B_PCH_PMC_GEN_PMCON_A_AG3_PP_EN                     BIT28
#define B_PCH_PMC_GEN_PMCON_A_SX_PP_EN                      BIT27
#define B_PCH_PMC_GEN_PMCON_A_DISB                          BIT23
#define B_PCH_PMC_GEN_PMCON_A_MEM_SR                        BIT21
#define B_PCH_PMC_GEN_PMCON_A_MS4V                          BIT18
#define B_PCH_PMC_GEN_PMCON_A_GBL_RST_STS                   BIT16
#define B_PCH_PMC_GEN_PMCON_A_ALLOW_PLL_SD_INC0             BIT13
#define B_PCH_PMC_GEN_PMCON_A_ALLOW_SPXB_CG_INC0            BIT12
#define B_PCH_PMC_GEN_PMCON_A_BIOS_PCI_EXP_EN               BIT10
#define B_PCH_PMC_GEN_PMCON_A_PWRBTN_LVL                    BIT9
#define B_PCH_PMC_GEN_PMCON_A_ALLOW_L1LOW_C0                BIT7
#define B_PCH_PMC_GEN_PMCON_A_ALLOW_L1LOW_OPI_ON            BIT6
#define B_PCH_PMC_GEN_PMCON_A_ALLOW_L1LOW_BCLKREQ_ON        BIT5
#define B_PCH_PMC_GEN_PMCON_A_SMI_LOCK                      BIT4
#define B_PCH_PMC_GEN_PMCON_A_ESPI_SMI_LOCK                 BIT3             ///< ESPI SMI lock
#define B_PCH_PMC_GEN_PMCON_A_PER_SMI_SEL                   0x0003
#define V_PCH_PMC_GEN_PMCON_A_PER_SMI_64S                   0x0000
#define V_PCH_PMC_GEN_PMCON_A_PER_SMI_32S                   0x0001
#define V_PCH_PMC_GEN_PMCON_A_PER_SMI_16S                   0x0002
#define V_PCH_PMC_GEN_PMCON_A_PER_SMI_8S                    0x0003
#define R_PCH_PMC_GEN_PMCON_B                               0xA4
#define B_PCH_PMC_GEN_PMCON_B_SLPSX_STR_POL_LOCK            BIT18            ///< Lock down SLP_S3/SLP_S4 Minimum Assertion width
#define B_PCH_PMC_GEN_PMCON_B_ACPI_BASE_LOCK                BIT17            ///< Lock ACPI BASE at 0x40, only cleared by reset when set
#define B_PCH_PMC_GEN_PMCON_B_PM_DATA_BAR_DIS               BIT16
#define B_PCH_PMC_GEN_PMCON_B_PME_B0_S5_DIS                 BIT15
#define B_PCH_PMC_GEN_PMCON_B_SUS_PWR_FLR                   BIT14
#define B_PCH_PMC_GEN_PMCON_B_WOL_EN_OVRD                   BIT13
#define B_PCH_PMC_GEN_PMCON_B_DISABLE_SX_STRETCH            BIT12
#define B_PCH_PMC_GEN_PMCON_B_SLP_S3_MAW                    0xC00
#define V_PCH_PMC_GEN_PMCON_B_SLP_S3_MAW_60US               0x000
#define V_PCH_PMC_GEN_PMCON_B_SLP_S3_MAW_1MS                0x400
#define V_PCH_PMC_GEN_PMCON_B_SLP_S3_MAW_50MS               0x800
#define V_PCH_PMC_GEN_PMCON_B_SLP_S3_MAW_2S                 0xC00
#define B_PCH_PMC_GEN_PMCON_B_HOST_RST_STS                  BIT9
#define B_PCH_PMC_GEN_PMCON_B_SWSMI_RTSL                    0xC0
#define V_PCH_PMC_GEN_PMCON_B_SWSMI_RTSL_64MS               0xC0
#define V_PCH_PMC_GEN_PMCON_B_SWSMI_RTSL_32MS               0x80
#define V_PCH_PMC_GEN_PMCON_B_SWSMI_RTSL_16MS               0x40
#define V_PCH_PMC_GEN_PMCON_B_SWSMI_RTSL_1_5MS              0x00
#define B_PCH_PMC_GEN_PMCON_B_SLP_S4_MAW                    0x30
#define V_PCH_PMC_GEN_PMCON_B_SLP_S4_MAW_1S                 0x30
#define V_PCH_PMC_GEN_PMCON_B_SLP_S4_MAW_2S                 0x20
#define V_PCH_PMC_GEN_PMCON_B_SLP_S4_MAW_3S                 0x10
#define V_PCH_PMC_GEN_PMCON_B_SLP_S4_MAW_4S                 0x00
#define B_PCH_PMC_GEN_PMCON_B_SLP_S4_ASE                    BIT3
#define B_PCH_PMC_GEN_PMCON_B_RTC_PWR_STS                   BIT2
#define B_PCH_PMC_GEN_PMCON_B_PWR_FLR                       BIT1
#define B_PCH_PMC_GEN_PMCON_B_AFTERG3_EN                    BIT0
#define R_PCH_PMC_BM_CX_CNF                                 0xA8
#define B_PCH_PMC_BM_CX_CNF_STORAGE_BREAK_EN                BIT31
#define B_PCH_PMC_BM_CX_CNF_PCIE_BREAK_EN                   BIT30
#define B_PCH_PMC_BM_CX_CNF_EHCI_BREAK_EN                   BIT26
#define B_PCH_PMC_BM_CX_CNF_AZ_BREAK_EN                     BIT24
#define B_PCH_PMC_BM_CX_CNF_DPSN_BREAK_EN                   BIT19
#define B_PCH_PMC_BM_CX_CNF_XHCI_BREAK_EN                   BIT17
#define B_PCH_PMC_BM_CX_CNF_SATA3_BREAK_EN                  BIT16
#define B_PCH_PMC_BM_CX_CNF_SCRATCHPAD                      BIT15
#define B_PCH_PMC_BM_CX_CNF_PHOLD_BM_STS_BLOCK              BIT14
#define B_PCH_PMC_BM_CX_CNF_MASK_CF                         BIT11
#define B_PCH_PMC_BM_CX_CNF_BM_STS_ZERO_EN                  BIT10
#define B_PCH_PMC_BM_CX_CNF_PM_SYNC_MSG_MODE                BIT9
#define R_PCH_PMC_ETR3                                      0xAC
#define B_PCH_PMC_ETR3_CF9LOCK                              BIT31           ///< CF9h Lockdown
#define B_PCH_PMC_ETR3_USB_CACHE_DIS                        BIT21
#define B_PCH_PMC_ETR3_CF9GR                                BIT20           ///< CF9h Global Reset
#define B_PCH_PMC_ETR3_SKIP_HOST_RST_HS                     BIT19
#define B_PCH_PMC_ETR3_CWORWRE                              BIT18
#define R_PCH_PMC_PMC_THROT_1                               0xB0
#define B_PCH_PMC_PMC_PMC_THROT_LOCK                        BIT15
#define B_PCH_PMC_PMC_THROT_1_VRALERT_EN                    BIT0
#define R_PCH_PMC_MDAP                                      0xC0
#define B_PCH_PMC_MDAP_MDAP_POLICY_EN                       BIT31
#define B_PCH_PMC_MDAP_PDMA_EN                              BIT30
#define B_PCH_PMC_MDAP_AUTO_POLICY_CTL                      BIT29
#define B_PCH_PMC_MDAP_MDAP_VALUE                           0x1FFFF

//
// ACPI and legacy I/O register offsets from ACPIBASE
//
#define R_PCH_ACPI_PM1_STS                        0x00
#define S_PCH_ACPI_PM1_STS                        2
#define B_PCH_ACPI_PM1_STS_WAK                    0x8000
#define B_PCH_ACPI_PM1_STS_PRBTNOR                0x0800
#define B_PCH_ACPI_PM1_STS_RTC                    0x0400
#define B_PCH_ACPI_PM1_STS_PWRBTN                 0x0100
#define B_PCH_ACPI_PM1_STS_GBL                    0x0020
#define B_PCH_ACPI_PM1_STS_BM                     0x0010
#define B_PCH_ACPI_PM1_STS_TMROF                  0x0001
#define N_PCH_ACPI_PM1_STS_WAK                    15
#define N_PCH_ACPI_PM1_STS_PRBTNOR                11
#define N_PCH_ACPI_PM1_STS_RTC                    10
#define N_PCH_ACPI_PM1_STS_PWRBTN                 8
#define N_PCH_ACPI_PM1_STS_GBL                    5
#define N_PCH_ACPI_PM1_STS_BM                     4
#define N_PCH_ACPI_PM1_STS_TMROF                  0

#define R_PCH_ACPI_PM1_EN                         0x02
#define S_PCH_ACPI_PM1_EN                         2
#define B_PCH_ACPI_PM1_EN_RTC                     0x0400
#define B_PCH_ACPI_PM1_EN_PWRBTN                  0x0100
#define B_PCH_ACPI_PM1_EN_GBL                     0x0020
#define B_PCH_ACPI_PM1_EN_TMROF                   0x0001
#define N_PCH_ACPI_PM1_EN_RTC                     10
#define N_PCH_ACPI_PM1_EN_PWRBTN                  8
#define N_PCH_ACPI_PM1_EN_GBL                     5
#define N_PCH_ACPI_PM1_EN_TMROF                   0

#define R_PCH_ACPI_PM1_CNT                        0x04
#define S_PCH_ACPI_PM1_CNT                        4
#define B_PCH_ACPI_PM1_CNT_SLP_EN                 0x00002000
#define B_PCH_ACPI_PM1_CNT_SLP_TYP                0x00001C00
#define V_PCH_ACPI_PM1_CNT_S0                     0x00000000
#define V_PCH_ACPI_PM1_CNT_S1                     0x00000400
#define V_PCH_ACPI_PM1_CNT_S3                     0x00001400
#define V_PCH_ACPI_PM1_CNT_S4                     0x00001800
#define V_PCH_ACPI_PM1_CNT_S5                     0x00001C00
#define B_PCH_ACPI_PM1_CNT_GBL_RLS                0x00000004
#define B_PCH_ACPI_PM1_CNT_BM_RLD                 0x00000002
#define B_PCH_ACPI_PM1_CNT_SCI_EN                 0x00000001

#define R_PCH_ACPI_PM1_TMR                        0x08
#define V_PCH_ACPI_TMR_FREQUENCY                  3579545
#define B_PCH_ACPI_PM1_TMR_VAL                    0xFFFFFF
#define V_PCH_ACPI_PM1_TMR_MAX_VAL                0x1000000       ///< The timer is 24 bit overflow

#define R_PCH_SMI_EN                              0x30
#define S_PCH_SMI_EN                              4
#define B_PCH_SMI_EN_LEGACY_USB3                  BIT31
#define B_PCH_SMI_EN_GPIO_UNLOCK_SMI              BIT27
#define B_PCH_SMI_EN_INTEL_USB2                   BIT18
#define B_PCH_SMI_EN_LEGACY_USB2                  BIT17
#define B_PCH_SMI_EN_PERIODIC                     BIT14
#define B_PCH_SMI_EN_TCO                          BIT13
#define B_PCH_SMI_EN_MCSMI                        BIT11
#define B_PCH_SMI_EN_BIOS_RLS                     BIT7
#define B_PCH_SMI_EN_SWSMI_TMR                    BIT6
#define B_PCH_SMI_EN_APMC                         BIT5
#define B_PCH_SMI_EN_ON_SLP_EN                    BIT4
#define B_PCH_SMI_EN_LEGACY_USB                   BIT3
#define B_PCH_SMI_EN_BIOS                         BIT2
#define B_PCH_SMI_EN_EOS                          BIT1
#define B_PCH_SMI_EN_GBL_SMI                      BIT0
#define N_PCH_SMI_EN_LEGACY_USB3                  31
#define N_PCH_SMI_EN_ESPI                         28
#define N_PCH_SMI_EN_GPIO_UNLOCK                  27
#define N_PCH_SMI_EN_INTEL_USB2                   18
#define N_PCH_SMI_EN_LEGACY_USB2                  17
#define N_PCH_SMI_EN_PERIODIC                     14
#define N_PCH_SMI_EN_TCO                          13
#define N_PCH_SMI_EN_MCSMI                        11
#define N_PCH_SMI_EN_BIOS_RLS                     7
#define N_PCH_SMI_EN_SWSMI_TMR                    6
#define N_PCH_SMI_EN_APMC                         5
#define N_PCH_SMI_EN_ON_SLP_EN                    4
#define N_PCH_SMI_EN_LEGACY_USB                   3
#define N_PCH_SMI_EN_BIOS                         2
#define N_PCH_SMI_EN_EOS                          1
#define N_PCH_SMI_EN_GBL_SMI                      0

#define R_PCH_SMI_STS                             0x34
#define S_PCH_SMI_STS                             4
#define B_PCH_SMI_STS_LEGACY_USB3                 BIT31
#define B_PCH_SMI_STS_GPIO_UNLOCK                 BIT27
#define B_PCH_SMI_STS_SPI                         BIT26
#define B_PCH_SMI_STS_MONITOR                     BIT21
#define B_PCH_SMI_STS_PCI_EXP                     BIT20
#define B_PCH_SMI_STS_PATCH                       BIT19
#define B_PCH_SMI_STS_INTEL_USB2                  BIT18
#define B_PCH_SMI_STS_LEGACY_USB2                 BIT17
#define B_PCH_SMI_STS_SMBUS                       BIT16
#define B_PCH_SMI_STS_SERIRQ                      BIT15
#define B_PCH_SMI_STS_PERIODIC                    BIT14
#define B_PCH_SMI_STS_TCO                         BIT13
#define B_PCH_SMI_STS_DEVMON                      BIT12
#define B_PCH_SMI_STS_MCSMI                       BIT11
#define B_PCH_SMI_STS_GPIO_SMI                    BIT10
#define B_PCH_SMI_STS_GPE1                        BIT10
#define B_PCH_SMI_STS_GPE0                        BIT9
#define B_PCH_SMI_STS_PM1_STS_REG                 BIT8
#define B_PCH_SMI_STS_SWSMI_TMR                   BIT6
#define B_PCH_SMI_STS_APM                         BIT5
#define B_PCH_SMI_STS_ON_SLP_EN                   BIT4
#define B_PCH_SMI_STS_LEGACY_USB                  BIT3
#define B_PCH_SMI_STS_BIOS                        BIT2
#define N_PCH_SMI_STS_LEGACY_USB3                 31
#define N_PCH_SMI_STS_ESPI                        28
#define N_PCH_SMI_STS_GPIO_UNLOCK                 27
#define N_PCH_SMI_STS_SPI                         26
#define N_PCH_SMI_STS_MONITOR                     21
#define N_PCH_SMI_STS_PCI_EXP                     20
#define N_PCH_SMI_STS_PATCH                       19
#define N_PCH_SMI_STS_INTEL_USB2                  18
#define N_PCH_SMI_STS_LEGACY_USB2                 17
#define N_PCH_SMI_STS_SMBUS                       16
#define N_PCH_SMI_STS_SERIRQ                      15
#define N_PCH_SMI_STS_PERIODIC                    14
#define N_PCH_SMI_STS_TCO                         13
#define N_PCH_SMI_STS_DEVMON                      12
#define N_PCH_SMI_STS_MCSMI                       11
#define N_PCH_SMI_STS_GPIO_SMI                    10
#define N_PCH_SMI_STS_GPE0                        9
#define N_PCH_SMI_STS_PM1_STS_REG                 8
#define N_PCH_SMI_STS_SWSMI_TMR                   6
#define N_PCH_SMI_STS_APM                         5
#define N_PCH_SMI_STS_ON_SLP_EN                   4
#define N_PCH_SMI_STS_LEGACY_USB                  3
#define N_PCH_SMI_STS_BIOS                        2

#define R_PCH_ACPI_GPE_CNTL                       0x40
#define B_PCH_ACPI_GPE_CNTL_SWGPE_CTRL            BIT17

#define R_PCH_DEVACT_STS                          0x44
#define S_PCH_DEVACT_STS                          2
#define B_PCH_DEVACT_STS_MASK                     0x13E1
#define B_PCH_DEVACT_STS_KBC                      0x1000
#define B_PCH_DEVACT_STS_PIRQDH                   0x0200
#define B_PCH_DEVACT_STS_PIRQCG                   0x0100
#define B_PCH_DEVACT_STS_PIRQBF                   0x0080
#define B_PCH_DEVACT_STS_PIRQAE                   0x0040
#define B_PCH_DEVACT_STS_D0_TRP                   0x0001
#define N_PCH_DEVACT_STS_KBC                      12
#define N_PCH_DEVACT_STS_PIRQDH                   9
#define N_PCH_DEVACT_STS_PIRQCG                   8
#define N_PCH_DEVACT_STS_PIRQBF                   7
#define N_PCH_DEVACT_STS_PIRQAE                   6

#define R_PCH_ACPI_PM2_CNT                        0x50
#define B_PCH_ACPI_PM2_CNT_ARB_DIS                0x01

#define R_PCH_OC_WDT_CTL                          0x54
#define B_PCH_OC_WDT_CTL_RLD                      BIT31
#define B_PCH_OC_WDT_CTL_ICCSURV_STS              BIT25
#define B_PCH_OC_WDT_CTL_NO_ICCSURV_STS           BIT24
#define B_PCH_OC_WDT_CTL_FORCE_ALL                BIT15
#define B_PCH_OC_WDT_CTL_EN                       BIT14
#define B_PCH_OC_WDT_CTL_ICCSURV                  BIT13
#define B_PCH_OC_WDT_CTL_LCK                      BIT12
#define B_PCH_OC_WDT_CTL_TOV_MASK                 0x3FF
#define B_PCH_OC_WDT_CTL_FAILURE_STS              BIT23
#define B_PCH_OC_WDT_CTL_UNXP_RESET_STS           BIT22
#define B_PCH_OC_WDT_CTL_AFTER_POST               0x3F0000
#define V_PCH_OC_WDT_CTL_STATUS_FAILURE           1
#define V_PCH_OC_WDT_CTL_STATUS_OK                0

#define R_PCH_ACPI_GPE0_STS_127_96                0x8C
#define S_PCH_ACPI_GPE0_STS_127_96                4
#define B_PCH_ACPI_GPE0_STS_127_96_WADT           BIT18
#define B_PCH_ACPI_GPE0_STS_127_96_LAN_WAKE       BIT16
#define B_PCH_ACPI_GPE0_STS_127_96_PME_B0         BIT13
#define B_PCH_ACPI_GPE0_STS_127_96_ME_SCI         BIT12
#define B_PCH_ACPI_GPE0_STS_127_96_PME            BIT11
#define B_PCH_ACPI_GPE0_STS_127_96_BATLOW         BIT10
#define B_PCH_ACPI_GPE0_STS_127_96_PCI_EXP        BIT9
#define B_PCH_ACPI_GPE0_STS_127_96_RI             BIT8
#define B_PCH_ACPI_GPE0_STS_127_96_SMB_WAK        BIT7
#define B_PCH_ACPI_GPE0_STS_127_96_TC0SCI         BIT6
#define B_PCH_ACPI_GPE0_STS_127_96_SWGPE          BIT2
#define B_PCH_ACPI_GPE0_STS_127_96_HOT_PLUG       BIT1
#define N_PCH_ACPI_GPE0_STS_127_96_PME_B0         13
#define N_PCH_ACPI_GPE0_STS_127_96_PME            11
#define N_PCH_ACPI_GPE0_STS_127_96_BATLOW         10
#define N_PCH_ACPI_GPE0_STS_127_96_PCI_EXP        9
#define N_PCH_ACPI_GPE0_STS_127_96_RI             8
#define N_PCH_ACPI_GPE0_STS_127_96_SMB_WAK        7
#define N_PCH_ACPI_GPE0_STS_127_96_TC0SCI         6
#define N_PCH_ACPI_GPE0_STS_127_96_SWGPE          2
#define N_PCH_ACPI_GPE0_STS_127_96_HOT_PLUG       1

#define R_PCH_ACPI_GPE0_EN_31_0                   0x90
#define R_PCH_ACPI_GPE0_EN_63_31                  0x94
#define R_PCH_ACPI_GPE0_EN_94_64                  0x98
#define R_PCH_ACPI_GPE0_EN_127_96                 0x9C
#define S_PCH_ACPI_GPE0_EN_127_96                 4
#define B_PCH_ACPI_GPE0_EN_127_96_WADT            BIT18
#define B_PCH_ACPI_GPE0_EN_127_96_USB_CON_DSX     BIT17
#define B_PCH_ACPI_GPE0_EN_127_96_LAN_WAKE        BIT16
#define B_PCH_ACPI_GPE0_EN_127_96_PME_B0          BIT13
#define B_PCH_ACPI_GPE0_EN_127_96_ME_SCI          BIT12
#define B_PCH_ACPI_GPE0_EN_127_96_PME             BIT11
#define B_PCH_ACPI_GPE0_EN_127_96_BATLOW          BIT10
#define B_PCH_ACPI_GPE0_EN_127_96_PCI_EXP         BIT9
#define B_PCH_ACPI_GPE0_EN_127_96_RI              BIT8
#define B_PCH_ACPI_GPE0_EN_127_96_TC0SCI          BIT6
#define B_PCH_ACPI_GPE0_EN_127_96_SWGPE           BIT2
#define B_PCH_ACPI_GPE0_EN_127_96_HOT_PLUG        BIT1
#define N_PCH_ACPI_GPE0_EN_127_96_PME_B0          13
#define N_PCH_ACPI_GPE0_EN_127_96_USB3            12
#define N_PCH_ACPI_GPE0_EN_127_96_PME             11
#define N_PCH_ACPI_GPE0_EN_127_96_BATLOW          10
#define N_PCH_ACPI_GPE0_EN_127_96_PCI_EXP         9
#define N_PCH_ACPI_GPE0_EN_127_96_RI              8
#define N_PCH_ACPI_GPE0_EN_127_96_TC0SCI          6
#define N_PCH_ACPI_GPE0_EN_127_96_SWGPE           2
#define N_PCH_ACPI_GPE0_EN_127_96_HOT_PLUG        1


//
// TCO register I/O map
//
#define R_PCH_TCO_RLD                             0x0
#define R_PCH_TCO_DAT_IN                          0x2
#define R_PCH_TCO_DAT_OUT                         0x3
#define R_PCH_TCO1_STS                            0x04
#define S_PCH_TCO1_STS                            2
#define B_PCH_TCO1_STS_DMISERR                    BIT12
#define B_PCH_TCO1_STS_DMISMI                     BIT10
#define B_PCH_TCO1_STS_DMISCI                     BIT9
#define B_PCH_TCO1_STS_BIOSWR                     BIT8
#define B_PCH_TCO1_STS_NEWCENTURY                 BIT7
#define B_PCH_TCO1_STS_TIMEOUT                    BIT3
#define B_PCH_TCO1_STS_TCO_INT                    BIT2
#define B_PCH_TCO1_STS_SW_TCO_SMI                 BIT1
#define B_PCH_TCO1_STS_NMI2SMI                    BIT0
#define N_PCH_TCO1_STS_DMISMI                     10
#define N_PCH_TCO1_STS_BIOSWR                     8
#define N_PCH_TCO1_STS_NEWCENTURY                 7
#define N_PCH_TCO1_STS_TIMEOUT                    3
#define N_PCH_TCO1_STS_SW_TCO_SMI                 1
#define N_PCH_TCO1_STS_NMI2SMI                    0

#define R_PCH_TCO2_STS                            0x06
#define S_PCH_TCO2_STS                            2
#define B_PCH_TCO2_STS_SMLINK_SLV_SMI             BIT4
#define B_PCH_TCO2_STS_BAD_BIOS                   BIT3
#define B_PCH_TCO2_STS_BOOT                       BIT2
#define B_PCH_TCO2_STS_SECOND_TO                  BIT1
#define B_PCH_TCO2_STS_INTRD_DET                  BIT0
#define N_PCH_TCO2_STS_INTRD_DET                  0

#define R_PCH_TCO1_CNT                            0x08
#define S_PCH_TCO1_CNT                            2
#define B_PCH_TCO_CNT_LOCK                        BIT12
#define B_PCH_TCO_CNT_TMR_HLT                     BIT11
#define B_PCH_TCO_CNT_NMI2SMI_EN                  BIT9
#define B_PCH_TCO_CNT_NMI_NOW                     BIT8
#define N_PCH_TCO_CNT_NMI2SMI_EN                  9

#define R_PCH_TCO2_CNT                            0x0A
#define S_PCH_TCO2_CNT                            2
#define B_PCH_TCO2_CNT_OS_POLICY                  0x0030
#define B_PCH_TCO2_CNT_GPI11_ALERT_DISABLE        0x0008
#define B_PCH_TCO2_CNT_INTRD_SEL                  0x0006
#define N_PCH_TCO2_CNT_INTRD_SEL                  2

#define R_PCH_TCO_MESSAGE1                        0x0C
#define R_PCH_TCO_MESSAGE2                        0x0D
#define R_PCH_TCO_WDCNT                           0x0E
#define R_PCH_TCO_SW_IRQ_GEN                      0x10
#define B_PCH_TCO_IRQ12_CAUSE                     BIT1
#define B_PCH_TCO_IRQ1_CAUSE                      BIT0
#define R_PCH_TCO_TMR                             0x12

//
// PWRM Registers
//
#define R_PCH_WADT_AC                                       0x0                         ///< Wake Alarm Device Timer: AC
#define R_PCH_WADT_DC                                       0x4                         ///< Wake Alarm Device Timer: DC
#define R_PCH_WADT_EXP_AC                                   0x8                         ///< Wake Alarm Device Expired Timer: AC
#define R_PCH_WADT_EXP_DC                                   0xC                         ///< Wake Alarm Device Expired Timer: DC
#define R_PCH_PWRM_PRSTS                                    0x10                        ///< Power and Reset Status
#define B_PCH_PWRM_PRSTS_PM_WD_TMR                          BIT15                       ///< Power Management Watchdog Timer
#define B_PCH_PWRM_PRSTS_VE_WD_TMR_STS                      BIT7                        ///< VE Watchdog Timer Status
#define B_PCH_PWRM_PRSTS_ME_WD_TMR_STS                      BIT6                        ///< Management Engine Watchdog Timer Status
#define B_PCH_PWRM_PRSTS_WOL_OVR_WK_STS                     BIT5
#define B_PCH_PWRM_PRSTS_FIELD_1                            BIT4
#define B_PCH_PWRM_PRSTS_ME_HOST_PWRDN                      BIT3
#define B_PCH_PWRM_PRSTS_ME_HRST_WARM_STS                   BIT2
#define B_PCH_PWRM_PRSTS_ME_HRST_COLD_STS                   BIT1
#define B_PCH_PWRM_PRSTS_ME_WAKE_STS                        BIT0
#define R_PCH_PWRM_14                                       0x14
#define R_PCH_PWRM_CFG                                      0x18                        ///< Power Management Configuration
#define B_PCH_PWRM_CFG_ALLOW_24_OSC_SD                      BIT29                       ///< Allow 24MHz Crystal Oscillator Shutdown
#define B_PCH_PWRM_CFG_ALLOW_USB2_CORE_PG                   BIT25                       ///< Allow USB2 Core Power Gating
#define B_PCH_PWRM_CFG_RTC_DS_WAKE_DIS                      BIT21                       ///< RTC Wake from Deep S4/S5 Disable
#define B_PCH_PWRM_CFG_SX_ENT_TO_EN                         BIT3                        ///< S1/3/4/5 Entry Timeout Enable Bit Mask
#define B_PCH_PWRM_CFG_SSMAW_MASK                           (BIT19 | BIT18)             ///< SLP_SUS# Min Assertion Width
#define V_PCH_PWRM_CFG_SSMAW_4S                             (BIT19 | BIT18)             ///< 4 seconds
#define V_PCH_PWRM_CFG_SSMAW_1S                             BIT19                       ///< 1 second
#define V_PCH_PWRM_CFG_SSMAW_0_5S                           BIT18                       ///< 0.5 second (500ms)
#define V_PCH_PWRM_CFG_SSMAW_0S                             0                           ///< 0 second
#define B_PCH_PWRM_CFG_SAMAW_MASK                           (BIT17 | BIT16)             ///< SLP_A# Min Assertion Width
#define V_PCH_PWRM_CFG_SAMAW_2S                             (BIT17 | BIT16)             ///< 2 seconds
#define V_PCH_PWRM_CFG_SAMAW_98ms                           BIT17                       ///< 98ms
#define V_PCH_PWRM_CFG_SAMAW_4S                             BIT16                       ///< 4 seconds
#define V_PCH_PWRM_CFG_SAMAW_0S                             0                           ///< 0 second
#define B_PCH_PWRM_CFG_RPCD_MASK                            (BIT9 | BIT8)               ///< Reset Power Cycle Duration
#define V_PCH_PWRM_CFG_RPCD_1S                              (BIT9 | BIT8)               ///< 1-2 seconds
#define V_PCH_PWRM_CFG_RPCD_2S                              BIT9                        ///< 2-3 seconds
#define V_PCH_PWRM_CFG_RPCD_3S                              BIT8                        ///< 3-4 seconds
#define V_PCH_PWRM_CFG_RPCD_4S                              0                           ///< 4-5 seconds (Default)
#define R_PCH_PWRM_S0_S1_PWRGATE_POL                        0x24                        ///< S0/S1 Power Gating Policies
#define R_PCH_PWRM_S3_PWRGATE_POL                           0x28                        ///< S3 Power Gating Policies
#define B_PCH_PWRM_S3DC_GATE_SUS                            BIT1                        ///< Deep S3 Enable in DC Mode
#define B_PCH_PWRM_S3AC_GATE_SUS                            BIT0                        ///< Deep S3 Enable in AC Mode
#define R_PCH_PWRM_S4_PWRGATE_POL                           0x2C                        ///< Deep S4 Power Policies
#define B_PCH_PWRM_S4DC_GATE_SUS                            BIT1                        ///< Deep S4 Enable in DC Mode
#define B_PCH_PWRM_S4AC_GATE_SUS                            BIT0                        ///< Deep S4 Enable in AC Mode
#define R_PCH_PWRM_S5_PWRGATE_POL                           0x30                        ///< Deep S5 Power Policies
#define B_PCH_PWRM_S5DC_GATE_SUS                            BIT15                       ///< Deep S5 Enable in DC Mode
#define B_PCH_PWRM_S5AC_GATE_SUS                            BIT14                       ///< Deep S5 Enable in AC Mode
#define R_PCH_PWRM_DSX_CFG                                  0x34                        ///< Deep SX Configuration
#define B_PCH_PWRM_DSX_CFG_WAKE_PIN_DSX_EN                  BIT2                        ///< WAKE# Pin DeepSx Enable
#define B_PCH_PWRM_DSX_CFG_ACPRES_PD_DSX_DIS                BIT1                        ///< AC_PRESENT pin pulldown in DeepSx disable
#define B_PCH_PWRM_DSX_CFG_LAN_WAKE_EN                      BIT0                        ///< LAN_WAKE Pin DeepSx Enable
#define R_PCH_PWRM_CFG2                                     0x3C                        ///< Power Management Configuration Reg 2
#define B_PCH_PWRM_CFG2_DRAM_RESET_CTL                      (1 << 26)                   ///< DRAM RESET# control
#define B_PCH_PWRM_CFG2_USB_DSX_PER_PORT_EN	                BIT13                       ///< USB Wake On Connect in/after DeepSx Per-Port
#define V_PCH_PWRM_EN_SN_SLOW_RING                          0x22D8701
#define V_PCH_PWRM_EN_SN_SLOW_RING2                         0x1
#define V_PCH_PWRM_EN_SN_SA                                 0x22D8701
#define V_PCH_PWRM_EN_SN_SA2                                0x1
#define V_PCH_PWRM_EN_SN_SLOW_RING_CF                       0x00018000
#define V_PCH_PWRM_EN_NS_SA                                 0x42400
#define V_PCH_PWRM_EN_CW_SLOW_RING                          0x9F8D8301
#define V_PCH_PWRM_EN_CW_SLOW_RING2                         0x20187E1
#define V_PCH_PWRM_EN_CW_SA                                 0x1F8F8301
#define V_PCH_PWRM_EN_CW_SA2                                0x20187E1
#define V_PCH_PWRM_EN_CW_SLOW_RING_CF                       0x00018000
#define V_PCH_PWRM_EN_PA_SLOW_RING                          0x00180300
#define V_PCH_PWRM_EN_PA_SLOW_RING2                         0x2002001
#define V_PCH_PWRM_EN_PA_SA                                 0x180300
#define V_PCH_PWRM_EN_PA_SA2                                0x02002001
#define V_PCH_PWRM_EN_MISC_EVENT                            0x00060000
#define V_PCH_PWRM_CFG3                                     0x1040000
#define V_PCH_PWRM_PM_DOWN_PPB_CFG                          0x00000001
#define N_PCH_PWRM_CFG2_PBOP                                29

#define R_PCH_PWRM_EN_SN_SLOW_RING                          0x48                        ///< Enable Snoop Request to SLOW_RING
#define R_PCH_PWRM_EN_SN_SLOW_RING2                         0x4C                        ///< Enable Snoop Request to SLOW_RING 2nd Reg
#define R_PCH_PWRM_EN_SN_SA                                 0x50                        ///< Enable Snoop Request to SA
#define R_PCH_PWRM_EN_SN_SA2                                0x54                        ///< Enable Snoop Request to SA 2nd Reg
#define R_PCH_PWRM_EN_SN_SLOW_RING_CF                       0x58                        ///< Enable Snoop Request to SLOW_RING_CF
#define R_PCH_PWRM_EN_NS_SA                                 0x68                        ///< Enable Non-Snoop Request to SA
#define R_PCH_PWRM_EN_CW_SLOW_RING                          0x80                        ///< Enable Clock Wake to SLOW_RING
#define R_PCH_PWRM_EN_CW_SLOW_RING2                         0x84                        ///< Enable Clock Wake to SLOW_RING 2nd Reg
#define R_PCH_PWRM_EN_CW_SA                                 0x88                        ///< Enable Clock Wake to SA
#define R_PCH_PWRM_EN_CW_SA2                                0x8C                        ///< Enable Clock Wake to SA 2nd Reg
#define R_PCH_PWRM_EN_CW_SLOW_RING_CF                       0x98                        ///< Enable Clock Wake to SLOW_RING_CF
#define R_PCH_PWRM_EN_PA_SLOW_RING                          0xA8                        ///< Enable Pegged Active to SLOW_RING
#define R_PCH_PWRM_EN_PA_SLOW_RING2                         0xAC                        ///< Enable Pegged Active to SLOW_RING 2nd Reg
#define R_PCH_PWRM_EN_PA_SA                                 0xB0                        ///< Enable Pegged Active to SA
#define R_PCH_PWRM_EN_PA_SA2                                0xB4                        ///< Enable Pegged Active to SA 2nd Reg
#define R_PCH_PWRM_EN_MISC_EVENT                            0xC0                        ///< Enable Misc PM_SYNC Events
#define R_PCH_PWRM_PMSYNC_TPR_CONFIG                        0xC4
#define B_PCH_PWRM_PMSYNC_TPR_CONFIG_LOCK                   BIT31
#define B_PCH_PWRM_PMSYNC_PCH2CPU_TT_EN                     BIT26
#define B_PCH_PWRM_PMSYNC_PCH2CPU_TT_STATE                  (BIT25 | BIT24)
#define N_PCH_PWRM_PMSYNC_PCH2CPU_TT_STATE                  24
#define V_PCH_PWRM_PMSYNC_PCH2CPU_TT_STATE_1                1
#define R_PCH_PWRM_PMSYNC_MISC_CFG                          0xC8
#define B_PCH_PWRM_PMSYNC_PM_SYNC_LOCK                      BIT15                       ///< PM_SYNC Configuration Lock
#define B_PCH_PWRM_PMSYNC_GPIO_D_SEL                        BIT11
#define B_PCH_PWRM_PMSYNC_GPIO_C_SEL                        BIT10
#define R_PCH_PWRM_PMSYNC_TPR_CONFIG2                       0xCC
#define R_PCH_PWRM_PM_SYNC_STATE_HYS                        0xD0                        ///< PM_SYNC State Hysteresis
#define R_PCH_PWRM_PM_SYNC_MODE                             0xD4                        ///< PM_SYNC Pin Mode
#define R_PCH_PWRM_CFG3                                     0xE0                        ///< Power Management Configuration Reg 3
#define B_PCH_PWRM_CFG3_DSX_WLAN_PP_EN                      BIT16                       ///< Deep-Sx WLAN Phy Power Enable
#define B_PCH_PWRM_CFG3_HOST_WLAN_PP_EN                     BIT17                       ///< Host Wireless LAN Phy Power Enable
#define R_PCH_PWRM_PM_DOWN_PPB_CFG                          0xE4                        ///< PM_DOWN PCH_POWER_BUDGET CONFIGURATION
#define R_PCH_PWRM_CFG4                                     0xE8                        ///< Power Management Configuration Reg 4
#define B_PCH_PWRM_CFG4_U2_PHY_PG_EN                        BIT30                       ///< USB2 PHY SUS Well Power Gating Enable
#define B_PCH_PWRM_CFG4_CPU_IOVR_RAMP_DUR                   (0x000001FF)                ///< CPU I/O VR Ramp Duration, [8:0]
#define N_PCH_PWRM_CFG4_CPU_IOVR_RAMP_DUR                   0
#define V_PCH_PWRM_CFG4_CPU_IOVR_RAMP_DUR_70US              0x007
#define V_PCH_PWRM_CFG4_CPU_IOVR_RAMP_DUR_240US             0x018
#define R_PCH_PWRM_CPU_EPOC                                 0xEC
#define R_PCH_PWRM_GPIO_CFG                                 0x120
#define B_PCH_PWRM_GPIO_CFG_GPE0_DW2                        (BIT11 | BIT10 | BIT9 | BIT8)
#define N_PCH_PWRM_GPIO_CFG_GPE0_DW2                        8
#define B_PCH_PWRM_GPIO_CFG_GPE0_DW1                        (BIT7 | BIT6 | BIT5 | BIT4)
#define N_PCH_PWRM_GPIO_CFG_GPE0_DW1                        4
#define B_PCH_PWRM_GPIO_CFG_GPE0_DW0                        (BIT3 | BIT2 | BIT1 | BIT0)
#define N_PCH_PWRM_GPIO_CFG_GPE0_DW0                        0
#define R_PCH_PWRM_PM_SYNC_MODE_C0                          0xF4                        ///< PM_SYNC Pin Mode in C0
#define R_PCH_PWRM_124                                      0x124
#define R_PCH_PWRM_128                                      0x128
#define R_PCH_PWRM_MODPHY_PM_CFG1                           0x200
#define R_PCH_PWRM_MODPHY_PM_CFG1_MLSXSWPGP                 0xFFFF
#define R_PCH_PWRM_MODPHY_PM_CFG2                           0x204    ///< ModPHY Power Management Configuration Reg 2
#define B_PCH_PWRM_MODPHY_PM_CFG2_MLSPDDGE                  BIT30    ///< ModPHY Lane SUS Power Domain Dynamic Gating Enable
#define B_PCH_PWRM_MODPHY_PM_CFG2_EMFC                      BIT29    ///< Enable ModPHY FET Control
#define B_PCH_PWRM_MODPHY_PM_CFG2_EFRT                      (BIT28 | BIT27 | BIT26 | BIT25 | BIT24)    ///< External FET Ramp Time
#define B_PCH_PWRM_MODPHY_PM_CFG2_MLSXSWPGP                 (BIT3 | BIT2 | BIT1 | BIT0)
#define V_PCH_PWRM_MODPHY_PM_CFG2_MLSXSWPGP                 0x0
#define R_PCH_PWRM_MODPHY_PM_CFG1                           0x200    ///< MODPHY_PM_CFG1
#define B_PCH_PWRM_MODPHY_PM_CFG1_MLS0SWPGP                 (BIT31 | BIT30 | BIT29 | BIT28 | BIT27 | BIT26 | BIT25 | BIT24 | \
                                                             BIT23 | BIT22 | BIT21 | BIT20 | BIT19 | BIT18 | BIT17 | BIT16)   ///< ModPHY Lane S0 SUS Well Power Gating Policy (MLS0SWPGP) [31:16]
#define B_PCH_PWRM_MODPHY_PM_CFG1_MLSXSWPGP                 (BIT15 | BIT14 | BIT13 | BIT12 | BIT11 | BIT10 | BIT9 | BIT8 | \
                                                             BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0)   ///< ModPHY Lane Sx SUS Well Power Gating Policy (MLSXSWPGP) [15:0]
#define V_PCH_PWRM_MODPHY_PM_CFG1                           0x00000000    ///< MODPHY_PM_CFG1 Value ONLY for A0 stepping
#define N_PCH_PWRM_MODPHY_PM_CFG2_EFRT                      24
#define V_PCH_PWRM_MODPHY_PM_CFG2_EFRT_200US                0x0A
#define B_PCH_PWRM_MODPHY_PM_CFG2_ASLOR_UFS                 BIT16    ///< UFS ModPHY SPD SPD Override
#define R_PCH_PWRM_MODPHY_PM_CFG3                           0x208    ///< ModPHY Power Management Configuration Reg 3
#define B_PCH_PWRM_MODPHY_PM_CFG3_MSPDRTREQ_UFS             BIT16    ///< UFS ModPHY SPD RT Request
#define B_PCH_PWRM_MODPHY_PM_CFG3_MSPDRTREQ_XDCI            BIT15    ///< xDCI ModPHY SPD RT Request
#define B_PCH_PWRM_MODPHY_PM_CFG3_MSPDRTREQ_XHCI            BIT14    ///< xHCI ModPHY SPD RT Request
#define B_PCH_PWRM_MODPHY_PM_CFG3_MSPDRTREQ_GBE             BIT13    ///< GbE ModPHY SPD RT Request
#define B_PCH_PWRM_MODPHY_PM_CFG3_MSPDRTREQ_SATA            BIT12    ///< SATA ModPHY SPD RT Request
#define R_PCH_PWRM_30C                                      0x30C
#define R_PCH_PWRM_OBFF_CFG                                 0x314             ///< OBFF Configuration
#define R_PCH_PWRM_31C                                      0x31C
#define R_PCH_PWRM_CPPM_MISC_CFG                            0x320             ///< CPPM Miscellaneous Configuration
#define R_PCH_PWRM_CPPM_CG_POL1A                            0x324             ///< CPPM Clock Gating Policy Reg 1
#define R_PCH_PWRM_CPPM_CG_POL2A                            0x340             ///< CPPM Clock Gating Policy Reg 3
#define R_PCH_PWRM_34C                                      0x34C
#define R_PCH_PWRM_CPPM_CG_POL3A                            0x3A8             ///< CPPM Clock Gating Policy Reg 5
#define B_PCH_PWRM_CPPM_CG_POLXA_CPPM_GX_QUAL               BIT30             ///< CPPM Shutdown Qualifier Enable for Clock Source Group X
#define B_PCH_PWRM_CPPM_CG_POLXA_LTR_GX_THRESH              (0x000001FF)      ///< LTR Threshold for Clock Source Group X, [8:0]
#define R_PCH_PWRM_3D0                                      0x3D0
#define R_PCH_PWRM_CPPM_MPG_POL1A                           0x3E0             ///< CPPM ModPHY Gating Policy Reg 1A
#define B_PCH_PWRM_CPPM_MPG_POL1A_CPPM_MODPHY_QUAL          BIT30             ///< CPPM Shutdown Qualifier Enable for ModPHY
#define B_PCH_PWRM_CPPM_MPG_POL1A_LT_MODPHY_SEL             BIT29             ///< ASLT/PLT Selection for ModPHY
#define B_PCH_PWRM_CPPM_MPG_POL1A_LTR_MODPHY_THRESH         (0x000001FF)      ///< LTR Threshold for ModPHY, [8:0]
#define R_PCH_PWRM_CS_SD_CTL1                               0x3E8             ///< Clock Source Shutdown Control Reg 1
#define B_PCH_PWRM_CS_SD_CTL1_CS5_CTL_CFG                   (BIT22 | BIT21 | BIT20)    ///< Clock Source 5 Control Configuration
#define N_PCH_PWRM_CS_SD_CTL1_CS5_CTL_CFG                   20
#define B_PCH_PWRM_CS_SD_CTL1_CS1_CTL_CFG                   (BIT2 | BIT1 | BIT0)       ///< Clock Source 1 Control Configuration
#define N_PCH_PWRM_CS_SD_CTL1_CS1_CTL_CFG                   0
#define R_PCH_PWRM_CS_SD_CTL2                               0x3EC             ///< Clock Source Shutdown Control Reg 2
#define R_PCH_PWRM_HSWPGCR1                                 0x5D0
#define B_PCH_PWRM_SW_PG_CTRL_LOCK                          BIT31
#define B_PCH_PWRM_DFX_SW_PG_CTRL                           BIT0
#define R_PCH_PWRM_600                                      0x600
#define R_PCH_PWRM_604                                      0x604



//
//Bits to control Function Disable for several IP's
//
#define R_PCH_PWRM_ST_PG_FDIS_PMC_1                         0x620 ///< Static PG Related Function Disable Register 1
#define  B_PCH_PWRM_ST_PG_FDIS_PMC_1_ST_FDIS_LK             BIT31 ///< Static Function Disable Lock (ST_FDIS_LK)
#define  B_PCH_PWRM_ST_FDIS_LK             BIT31
#define  B_PCH_PWRM_EXI_FDIS_PMC           BIT12
#define  B_PCH_PWRM_NPK_FDIS_PMC           BIT11
#define  B_PCH_PWRM_IE_FDIS_PMC            BIT10
#define  B_PCH_PWRM_ME_FDIS_PMC            BIT9
#define  B_PCH_PWRM_XHCI_FDIS_PMC          BIT8
#define  B_PCH_PWRM_EMMC_FDIS_PMC          BIT7
#define  B_PCH_PWRM_NCPM_FDIS_PMC          BIT6
#define  B_PCH_PWRM_PCIE1_FDIS_PMC         BIT5
#define  B_PCH_PWRM_PCIE0_FDIS_PMC         BIT4
#define  B_PCH_PWRM_SATA1_FDIS_PMC         BIT3
#define  B_PCH_PWRM_SATA0_FDIS_PMC         BIT2
#define  B_PCH_PWRM_GBE1_FDIS_PMC          BIT1
#define  B_PCH_PWRM_GBE0_FDIS_PMC          BIT0
#define R_PCH_PWRM_ST_PG_FDIS_PMC_2                         0x624 ///< Static Function Disable Control Register 2
#define R_PCH_PWRM_NST_PG_FDIS_1                            0x628
#define R_PCH_PWRM_FUSE_DIS_RD_1                            0x640 ///< Fuse Disable Read 1 Register
#define R_PCH_PWRM_STPG_FUSE_SS_DIS_RD_2                    0x644 ///< Fuse Disable Read 2 Register
#define  B_PCH_PWRM_ME_FUSE_SS_DIS        BIT15
#define  B_PCH_PWRM_IE_FUSE_SS_DIS        BIT14
#define  B_PCH_PWRM_USB2_FUSE_SS_DIS      BIT13
#define  B_PCH_PWRM_LPC_FUSE_SS_DIS       BIT12
#define  B_PCH_PWRM_ESPISPI_FUSE_SS_DIS   BIT11
#define  B_PCH_PWRM_NPK_FUSE_SS_DIS       BIT10
#define  B_PCH_PWRM_EXI_FUSE_SS_DIS       BIT9
#define  B_PCH_PWRM_EMMC_FUSE_SS_DIS      BIT8
#define  B_PCH_PWRM_XHCI_FUSE_SS_DIS      BIT7
#define  B_PCH_PWRM_NCPM_FUSE_SS_DIS      BIT6
#define  B_PCH_PWRM_PCIE1_FUSE_SS_DIS     BIT5
#define  B_PCH_PWRM_PCIE0_FUSE_SS_DIS     BIT4
#define  B_PCH_PWRM_SATA1_FUSE_SS_DIS     BIT3
#define  B_PCH_PWRM_SATA0_FUSE_SS_DIS     BIT2
#define  B_PCH_PWRM_FUSE_DIS_RD_2_GBE1_FUSE_DIS             BIT1
#define  B_PCH_PWRM_FUSE_DIS_RD_2_GBE0_FUSE_DIS             BIT0

#endif
