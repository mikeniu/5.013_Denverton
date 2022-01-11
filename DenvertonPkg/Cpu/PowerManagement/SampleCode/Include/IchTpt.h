/*++
  This file contains an 'Intel Peripheral Driver' and uniquely        
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your   
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the
  license agreement
--*/
/*++

Copyright (c) 1999 - 2010 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  IchTpt.h

Abstract:

  Registers for ICH.

--*/

#ifndef _TPT_REGS_H_
#define _TPT_REGS_H_

//
// Definitions beginning with "R_" are registers
// Definitions beginning with "B_" are bits within registers
// Definitions beginning with "V_" are meaningful values of bits within the registers
// Definitions beginning with "S_" are register sizes
// Definitions beginning with "N_" are the bit position
//

#define V_ICH_INTEL_VENDOR_ID                   0x8086
#define V_ICH_DEFAULT_SID                       0x7270

#define PCI_BUS_NUMBER_ICH                      0x00

//
// DMI to PCI Bridge Registers (D30:F0)
//
#define PCI_DEVICE_NUMBER_ICH_HL2P             30
#define PCI_FUNCTION_NUMBER_ICH_HL2P           0

#define R_ICH_HL2P_VENDOR_ID                  0x00
#define   V_ICH_HL2P_VENDOR_ID                  V_ICH_INTEL_VENDOR_ID
#define R_ICH_HL2P_DEVICE_ID                  0x02
#define   V_ICH_HL2P_DEVICE_ID_0                0x2448
#define R_ICH_HL2P_COMMAND                    0x04
#define   B_ICH_HL2P_COMMAND_FBE                0x0200
#define   B_ICH_HL2P_COMMAND_SERR_EN            0x0100
#define   B_ICH_HL2P_COMMAND_WCC                0x0080
#define   B_ICH_HL2P_COMMAND_PER                0x0040
#define   B_ICH_HL2P_COMMAND_VPS                0x0020
#define   B_ICH_HL2P_COMMAND_MWE                0x0010
#define   B_ICH_HL2P_COMMAND_SCE                0x0008
#define   B_ICH_HL2P_COMMAND_BME                0x0004
#define   B_ICH_HL2P_COMMAND_MSE                0x0002
#define   B_ICH_HL2P_COMMAND_IOSE               0x0001
#define R_ICH_HL2P_PD_STS                     0x06
#define   B_ICH_HL2P_PD_STS_DPE                 0x8000
#define   B_ICH_HL2P_PD_STS_SSE                 0x4000
#define   B_ICH_HL2P_PD_STS_RMA                 0x2000
#define   B_ICH_HL2P_PD_STS_RTA                 0x1000
#define   B_ICH_HL2P_PD_STS_STA                 0x0800
#define   B_ICH_HL2P_PD_STS_DEVT_STS            0x0600
#define   B_ICH_HL2P_PD_STS_MDPED               0x0100
#define   B_ICH_HL2P_PD_STS_IOSE                0x0001
#define R_ICH_HL2P_REV_ID                     0x08
#define R_ICH_HL2P_SCC                        0x0A
#define R_ICH_HL2P_BCC                        0x0B
#define R_ICH_HL2P_PMLT                       0x0D
#define   B_ICH_HL2P_PMLT_MLC                   0xF8
#define R_ICH_HL2P_HEADTYP                    0x0E
#define   B_ICH_HL2P_HEADTYP_MFD                0x80
#define   B_ICH_HL2P_HEADTYP_HT                 0x7F
#define R_ICH_HL2P_PBUS_NUM                   0x18
#define R_ICH_HL2P_SBUS_NUM                   0x19
#define R_ICH_HL2P_SUB_BUS_NUM                0x1A
#define R_ICH_HL2P_SMLT                       0x1B
#define   B_ICH_HL2P_SMLT_MLC                   0xF8
#define R_ICH_HL2P_IO_BASE                      0x1C
#define   B_ICH_HL2P_IO_BASE_BAR                0xF0
#define   B_ICH_HL2P_IO_BASE_ADDR_CAP           0x0F
#define R_ICH_HL2P_IOLIM                      0x1D
#define   B_ICH_HL2P_IOLIM                      0xF0
#define   B_ICH_HL2P_IOLIM_ADDR_CAP             0x0F
#define R_ICH_HL2P_SECSTS                     0x1E
#define   B_ICH_HL2P_SECSTS_DPE                 0x8000
#define   B_ICH_HL2P_SECSTS_SSE                 0x4000
#define   B_ICH_HL2P_SECSTS_RMA                 0x2000
#define   B_ICH_HL2P_SECSTS_RTA                 0x1000
#define   B_ICH_HL2P_SECSTS_STA                 0x0800
#define   B_ICH_HL2P_SECSTS_DEVT_STS            0x0600
#define   B_ICH_HL2P_SECSTS_MDPED               0x0100
#define   B_ICH_HL2P_SECSTS_FB2B                0x0080
#define   B_ICH_HL2P_SECSTS_66MHZ_CAP           0x0020
#define R_ICH_HL2P_MEMBASE                    0x20
#define   B_ICH_HL2P_MEMBASE_BAR                0xFFF0
#define R_ICH_HL2P_MEMLIM                     0x22
#define   B_ICH_HL2P_MEMLIM                     0xFFF0
#define R_ICH_HL2P_PREF_MEM_BASE              0x24
#define   B_ICH_HL2P_PREF_MEM_BASE_BAR          0xFFF0
#define R_ICH_HL2P_PREF_MEM_MLT               0x26
#define   B_ICH_HL2P_PREF_MEM_MLT               0xFFF0
#define R_ICH_HL2P_PMBU32                     0x28
#define R_ICH_HL2P_PMLU32                     0x2C
#define R_ICH_HL2P_CAPP                       0x34
#define R_ICH_HL2P_INT_LINE                   0x3C
#define R_ICH_HL2P_BRIDGE_CNT                 0x3E
#define   B_ICH_HL2P_BRIDGE_CNT_FB2B            0x0080
#define   B_ICH_HL2P_BRIDGE_CNT_SBR             0x0040
#define   B_ICH_HL2P_BRIDGE_CNT_MAM             0x0020
#define   B_ICH_HL2P_BRIDGE_CNT_VGA16           0x0010
#define   B_ICH_HL2P_BRIDGE_CNT_VGA_EN          0x0008
#define   B_ICH_HL2P_BRIDGE_CNT_ISA_EN          0x0004
#define   B_ICH_HL2P_BRIDGE_CNT_SERR_EN         0x0002
#define   B_ICH_HL2P_BRIDGE_CNT_PER_EN          0x0001
#define R_ICH_HL2P_SPDH                       0x40
#define   B_ICH_HL2P_SPDH_HD7                   0x0080
#define   B_ICH_HL2P_SPDH_HD6                   0x0040
#define   B_ICH_HL2P_SPDH_HD5                   0x0020
#define   B_ICH_HL2P_SPDH_HD4                   0x0010
#define   B_ICH_HL2P_SPDH_HD3                   0x0008
#define   B_ICH_HL2P_SPDH_HD2                   0x0004
#define   B_ICH_HL2P_SPDH_HD1                   0x0002
#define   B_ICH_HL2P_SPDH_HD0                   0x0001
#define R_ICH_HL2P_DTC                        0x44
#define   B_ICH_HL2P_DTC_DDT                    0x80000000
#define   B_ICH_HL2P_DTC_BDT                    0x40000000
#define   B_ICH_HL2P_DTC_MDT_MASK               0x000000C0
#define   B_ICH_HL2P_DTC_AFADE                  0x00000010
#define   B_ICH_HL2P_DTC_NP                     0x00000008
#define   B_ICH_HL2P_DTC_MRMPD                  0x00000004
#define   B_ICH_HL2P_DTC_MRLPD                  0x00000002
#define   B_ICH_HL2P_DTC_MRPD                   0x00000001
#define R_ICH_HL2P_BPS                        0x48
#define R_ICH_HL2P_BPC                        0x4C
#define R_ICH_HL2P_SVCAP                      0x50
#define R_ICH_HL2P_SVID                       0x54

//
// PCI to LPC Bridge Registers (D31:F0)
//

#define PCI_DEVICE_NUMBER_ICH_LPC             31
#define PCI_FUNCTION_NUMBER_ICH_LPC           0

#define R_ICH_LPC_VENDOR_ID                   0x00
#define R_ICH_LPC_DEVICE_ID                   0x02
#define R_ICH_LPC_COMMAND                     0x04
#define   B_ICH_LPC_COMMAND_FBE                 0x0200
#define   B_ICH_LPC_COMMAND_SERR_EN             0x0100
#define   B_ICH_LPC_COMMAND_WCC                 0x0080
#define   B_ICH_LPC_COMMAND_PER                 0x0040
#define   B_ICH_LPC_COMMAND_VPS                 0x0020
#define   B_ICH_LPC_COMMAND_PMWE                0x0010
#define   B_ICH_LPC_COMMAND_SCE                 0x0008
#define   B_ICH_LPC_COMMAND_BME                 0x0004
#define   B_ICH_LPC_COMMAND_MSE                 0x0002
#define   B_ICH_LPC_COMMAND_IOSE                0x0001
#define R_ICH_LPC_DEV_STS                     0x06
#define   B_ICH_LPC_DEV_STS_DPE                 0x8000
#define   B_ICH_LPC_DEV_STS_SSE                 0x4000
#define   B_ICH_LPC_DEV_STS_RMA                 0x2000
#define   B_ICH_LPC_DEV_STS_RTA                 0x1000
#define   B_ICH_LPC_DEV_STS_STA                 0x0800
#define   B_ICH_LPC_DEV_STS_DEVT_STS            0x0600
#define   B_ICH_LPC_DEV_STS_MDPED               0x0100
#define   B_ICH_LPC_DEV_STS_FB2B                0x0080
#define   B_ICH_LPC_DEV_STS_UDF                 0x0040
#define   B_ICH_LPC_DEV_STS_66MHZ_CAP           0x0020
#define R_ICH_LPC_PLT                         0x0D
#define R_ICH_LPC_SS                          0x2C
#define R_ICH_LPC_ACPI_BASE                   0x40
#define   B_ICH_LPC_ACPI_BASE_BAR               0x0000FF80
#define R_ICH_LPC_ACPI_CNT                    0x44
#define   B_ICH_LPC_ACPI_CNT_ACPI_EN            0x80
#define   B_ICH_LPC_ACPI_CNT_SCI_IRG_SEL        0x07
#define R_ICH_LPC_GPIO_BASE                   0x48

#define R_ICH_LPC_GPIO_CNT                    0x4C
#define   B_ICH_LPC_GPIO_CNT_GPIO_EN            0x10
#define R_ICH_LPC_PIRQA_ROUT                  0x60
#define R_ICH_LPC_PIRQB_ROUT                  0x61
#define R_ICH_LPC_PIRQC_ROUT                  0x62
#define R_ICH_LPC_PIRQD_ROUT                  0x63

#define R_ICH_LPC_C5_EXIT_TIMING_CNT          0xA8
#define   B_ICH_LPC_SLOW_C5_EXIT_EN             BIT6
#define   B_ICH_LPC_C5_EXIT_PMSYNC_TO_STPCLK    0x38
#define    V_PMSYNC_STPCLK_16_17PCICLKS           0x00
#define    V_PMSYNC_STPCLK_80_86US                0x08
#define    V_PMSYNC_STPCLK_99_105US               0x10
#define    V_PMSYNC_STPCLK_118_124US              0x18
#define    V_PMSYNC_STPCLK_18_24US                0x20
#define    V_PMSYNC_STPCLK_38_44US                0x28
#define    V_PMSYNC_STPCLK_56_62US                0x30
#define    V_PMSYNC_STPCLK_199_205US              0x38
#define   B_ICH_LPC_C5_EXIT_DPRSTP_TO_STPCPU    0x07
#define    V_DPRSTP_STPCPU_22_28US                0x00
#define    V_DPRSTP_STPCPU_34_40US                0x01
#define    V_DPRSTP_STPCPU_56_62US                0x02
#define    V_DPRSTP_STPCPU_95_102US               0x03
#define    V_DPRSTP_STPCPU_138_144US              0x05
#define    V_DPRSTP_STPCPU_72_78US                0x06
#define    V_DPRSTP_STPCPU_199_205US              0x07

#define R_ICH_LPC_C4_TIMING_CNT               0xAA

//
// Bit values are the same for R_ICH_LPC_PIRQA_ROUT to R_ICH_LPC_PIRQH_ROUT
//
#define   B_ICH_LPC_PIRQX_ROUT_IRQEN            0x80
#define   B_ICH_LPC_PIRQX_ROUT                  0x0F
#define     V_ICH_LPC_PIRQX_ROUT_IRQ_3            0x03
#define     V_ICH_LPC_PIRQX_ROUT_IRQ_4            0x04
#define     V_ICH_LPC_PIRQX_ROUT_IRQ_5            0x05
#define     V_ICH_LPC_PIRQX_ROUT_IRQ_6            0x06
#define     V_ICH_LPC_PIRQX_ROUT_IRQ_7            0x07
#define     V_ICH_LPC_PIRQX_ROUT_IRQ_9            0x09
#define     V_ICH_LPC_PIRQX_ROUT_IRQ_10           0x0A
#define     V_ICH_LPC_PIRQX_ROUT_IRQ_11           0x0B
#define     V_ICH_LPC_PIRQX_ROUT_IRQ_12           0x0C
#define     V_ICH_LPC_PIRQX_ROUT_IRQ_14           0x0E
#define     V_ICH_LPC_PIRQX_ROUT_IRQ_15           0x0F
#define R_ICH_LPC_SERIRQ_CNT                  0x64
#define   B_ICH_LPC_SERIRQ_CNT_SIRQEN           0x80
#define   B_ICH_LPC_SERIRQ_CNT_SIRQMD           0x40
#define   B_ICH_LPC_SERIRQ_CNT_SIRQSZ           0x3C
#define   N_ICH_LPC_SERIRQ_CNT_SIRQSZ           2
#define   B_ICH_LPC_SERIRQ_CNT_SFPW             0x03
#define   N_ICH_LPC_SERIRQ_CNT_SFPW             0
#define R_ICH_LPC_PIRQE_ROUT                  0x68
#define R_ICH_LPC_PIRQF_ROUT                  0x69
#define R_ICH_LPC_PIRQG_ROUT                  0x6A
#define R_ICH_LPC_PIRQH_ROUT                  0x6B

#define R_ICH_LPC_IO_DEC                      0x80
#define   B_ICH_LPC_FDD_DEC                     0x1000
#define   B_ICH_LPC_LPT_DEC                     0x0300
#define   B_ICH_LPC_COMB_DEC                    0x0070
#define   B_ICH_LPC_COMA_DEC                    0x0007
#define R_ICH_LPC_ENABLES                     0x82
#define   B_ICH_LPC_ENABLES_ME2                 0x2000
#define   B_ICH_LPC_ENABLES_SE                  0x1000
#define   B_ICH_LPC_ENABLES_ME1                 0x0800
#define   B_ICH_LPC_ENABLES_KE                  0x0400
#define   B_ICH_LPC_ENABLES_HGE                 0x0200
#define   B_ICH_LPC_ENABLES_LGE                 0x0100
#define   B_ICH_LPC_ENABLES_FDE                 0x0008
#define   B_ICH_LPC_ENABLES_PPE                 0x0004
#define   B_ICH_LPC_ENABLES_CBE                 0x0002
#define   B_ICH_LPC_ENABLES_CAE                 0x0001
#define R_ICH_LPC_GEN1_DEC                    0x84
#define R_ICH_LPC_GEN2_DEC                    0x88
#define R_ICH_LPC_GEN3_DEC                    0x8C
#define R_ICH_LPC_GEN4_DEC                    0x90
#define R_ICH_LPC_FWH_BIOS_SEL                0xD0
#define   B_ICH_LPC_FWH_BIOS_SEL_F8             0xF0000000
#define   B_ICH_LPC_FWH_BIOS_SEL_F0             0x0F000000
#define   B_ICH_LPC_FWH_BIOS_SEL_E8             0x00F00000
#define   B_ICH_LPC_FWH_BIOS_SEL_E0             0x000F0000
#define   B_ICH_LPC_FWH_BIOS_SEL_D8             0x0000F000
#define   B_ICH_LPC_FWH_BIOS_SEL_D0             0x00000F00
#define   B_ICH_LPC_FWH_BIOS_SEL_C8             0x000000F0
#define   B_ICH_LPC_FWH_BIOS_SEL_C0             0x0000000F
#define R_ICH_LPC_FWH_BIOS_SEL2               0xD4
#define   B_ICH_LPC_FWH_BIOS_SEL2_70            0xF000
#define   B_ICH_LPC_FWH_BIOS_SEL2_60            0x0F00
#define   B_ICH_LPC_FWH_BIOS_SEL2_50            0x00F0
#define   B_ICH_LPC_FWH_BIOS_SEL2_40            0x000F
#define R_ICH_LPC_FWH_BIOS_DEC                0xD8
#define   B_ICH_LPC_FWH_BIOS_DEC_F8             0x8000
#define   B_ICH_LPC_FWH_BIOS_DEC_F0             0x4000
#define   B_ICH_LPC_FWH_BIOS_DEC_E8             0x2000
#define   B_ICH_LPC_FWH_BIOS_DEC_E0             0x1000
#define   B_ICH_LPC_FWH_BIOS_DEC_D8             0x0800
#define   B_ICH_LPC_FWH_BIOS_DEC_D0             0x0400
#define   B_ICH_LPC_FWH_BIOS_DEC_C8             0x0200
#define   B_ICH_LPC_FWH_BIOS_DEC_C0             0x0100
#define   B_ICH_LPC_FWH_BIOS_LEG_F              0x0080
#define   B_ICH_LPC_FWH_BIOS_LEG_E              0x0040
#define   B_ICH_LPC_FWH_BIOS_DEC_70             0x0008
#define   B_ICH_LPC_FWH_BIOS_DEC_60             0x0004
#define   B_ICH_LPC_FWH_BIOS_DEC_50             0x0002
#define   B_ICH_LPC_FWH_BIOS_DEC_40             0x0001
#define R_ICH_LPC_BIOS_CNTL                   0xDC
#define S_ICH_LPC_BIOS_CNTL                   1
#define   V_ICH_LPC_BIOS_CNTL_SRC               0x0C
#define   B_ICH_LPC_BIOS_CNTL_BLE               0x02
#define   B_ICH_LPC_BIOS_CNTL_BIOSWE            0x01
#define   N_ICH_LPC_BIOS_CNTL_BLE             1
#define   N_ICH_LPC_BIOS_CNTL_BIOSWE          0

#define R_ICH_LPC_RCBA                        0xF0
#define R_ICH_LPC_GEN_PMCON_1                 0xA0
#define   B_ICH_LPC_GEN_PMCON_PER_SMI_SEL       0x0003
#define    V_PER_SMI_64S                        0x0000
#define    V_PER_SMI_32S                        0x0001
#define    V_PER_SMI_16S                        0x0002
#define    V_PER_SMI_8S                         0x0003
#define   B_ICH_LPC_GEN_PMCON_CLKRUN_EN         0x0004
#define   B_ICH_LPC_GEN_PMCON_SS_EN             0x0008
#define   B_ICH_LPC_GEN_PMCON_SMI_LOCK          0x0010
#define   B_ICH_LPC_GEN_PMCON_CPUSLP_EN         0x0020
#define   B_ICH_LPC_GEN_PMCON_A64_EN            0x0040
#define   B_ICH_LPC_GEN_PMCON_C4ONC3_EN         0x0080
#define   B_ICH_LPC_GEN_PMCON_PWRBTN_LVL        0x0200
#define   B_ICH_LPC_GEN_PMCON_BIOS_PCI_EXP_EN   0x0400
#define R_ICH_LPC_GEN_PMCON_2                 0xA2
#define   B_ICH_LPC_GEN_PMCON_PWROK_FLR         0x01
#define   B_ICH_LPC_GEN_PMCON_CPUPWR_FLR        0x02
#define   B_ICH_LPC_GEN_PMCON_MIN_SLP_S4        0x04
#define   B_ICH_LPC_GEN_PMCON_CTS               0x08
#define   B_ICH_LPC_GEN_PMCON_SRS               0x10
#define   B_ICH_LPC_GEN_PMCON_CPLT              0x60
#define   B_ICH_LPC_GEN_PMCON_DRAM_INIT         0x80
#define R_ICH_LPC_GEN_PMCON_3                 0xA4
#define   B_ICH_LPC_GEN_PMCON_AFTERG3_EN        0x01
#define   B_ICH_LPC_GEN_PMCON_PWR_FLR           0x02
#define   B_ICH_LPC_GEN_PMCON_RTC_PWR_STS       0x04
#define   B_ICH_LPC_GEN_PMCON_SLP_S4_ASE        0x08
#define   B_ICH_LPC_GEN_PMCON_SLP_S4_MAW        0x30
#define   B_ICH_LPC_GEN_PMCON_SWSMI_RATE_SEL    0xC0
#define    V_SWSMI_RATE_64MS                      0xC0
#define    V_SWSMI_RATE_32MS                      0x80
#define    V_SWSMI_RATE_16MS                      0x40
#define    V_SWSMI_RATE_1_5MS                     0x00
#define R_ICH_LPC_C_STATE_CNF                 0xA9
#define   B_ICH_LPC_C_STATE_MEROM_MODE_EN       BIT7
#define   B_ICH_LPC_C_STATE_POPDOWN_EN          BIT4
#define   B_ICH_LPC_C_STATE_POPUP_EN            BIT3
#define   B_ICH_LPC_C_STATE_BM_STS_ZERO_EN      BIT2
#define R_ICH_LPC_USB_TDD                     0xAD
#define R_ICH_LPC_GPI_ROUT                    0xB8

//
// ACPI and legacy I/O register offsets from ABASE
//

#define R_ACPI_PM1_STS        0x00 
#define S_ACPI_PM1_STS           2
#define   B_WAK_STS             0x8000
#define   B_PRBTNOR_STS         0x0800
#define   B_RTC_STS             0x0400
#define   B_PWRBTN_STS          0x0100
#define   B_GBL_STS             0x0020
#define   B_BM_STS              0x0010
#define   B_TMROF_STS           0x0001
#define   N_WAK_STS          15
#define   N_PRBTNOR_STS      11
#define   N_RTC_STS          10
#define   N_PWRBTN_STS       8
#define   N_GBL_STS          5
#define   N_BM_STS           4
#define   N_TMROF_STS        0

#define R_ACPI_PM1_EN         0x02 
#define S_ACPI_PM1_EN            2
#define   B_RTC_EN              0x0400
#define   B_PWRBTN_EN           0x0100
#define   B_GBL_EN              0x0020
#define   B_TMROF_EN            0X0001
#define   N_RTC_EN           10
#define   N_PWRBTN_EN        8
#define   N_GBL_EN           5
#define   N_TMROF_EN         0

#define R_ACPI_PM1_CNT        0x04
#define S_ACPI_PM1_CNT           4
#define  B_SLP_EN               0x00002000
#define  B_SLP_TYP              0x00001C00
#define    V_S0                 0x00000000
#define    V_S1_DESK            0x00000400
#define    V_S1_MOBL            0x00000400
#define    V_S3                 0x00001400
#define    V_S4                 0x00001800
#define    V_S5                 0x00001C00
#define  B_GBL_RLS              0x00000004
#define  B_BM_RLD               0x00000002
#define  B_SCI_EN               0x00000001

#define R_ACPI_PM1_TMR        0x08
#define R_ACPI_PROC_CNT       0x10
#define  B_THTL_STS             0x00020000
#define  B_FORCE_THTL           0x00000100
#define  B_THRM_DTY             0x000000E0
#define  B_THTL_EN              0x00000010
#define  B_THTL_DTY             0x0000000E
#define R_ACPI_LV2            0x14
#define R_ACPI_LV3            0x15
#define R_ACPI_LV4            0x16
#define R_ACPI_PM2_CNT        0x20
#define  B_ARB_DIS              0x01

#define R_ACPI_GPE0_STS       0x28
#define S_ACPI_GPE0_STS          4
#define  B_GPInn_STS            0xFFFF0000
#define    V_GPI16_STS          0x80000000
#define  B_SATA_SCI_STS         0x00008000
#define  B_USB4_STS             0x00004000
#define  B_PME_B0_STS           0x00002000
#define  B_USB3_STS             0x00001000
#define  B_PME_STS              0x00000800
#define  B_BATLOW_STS           0x00000400
#define  B_PCI_EXP_STS          0x00000200
#define  B_RI_STS               0x00000100
#define  B_SMB_WAK_STS          0x00000080
#define  B_TC0SCI_STS           0x00000040
#define  B_AC97_STS             0x00000020
#define  B_USB2_STS             0x00000010
#define  B_USB1_STS             0x00000008
#define  B_SWGPE_STS            0x00000004
#define  B_HOT_PLUG_STS         0x00000002
#define  B_THRM_STS             0x00000001
#define  N_SATA_SCI_STS      15
#define  N_USB4_STS          14
#define  N_PME_B0_STS        13
#define  N_USB3_STS          12
#define  N_PME_STS           11
#define  N_BATLOW_STS        10
#define  N_PCI_EXP_STS       9
#define  N_RI_STS            8
#define  N_SMB_WAK_STS       7
#define  N_TC0SCI_STS        6
#define  N_AC97_STS          5
#define  N_USB2_STS          4
#define  N_USB1_STS          3
#define  N_SWGPE_STS         2
#define  N_HOT_PLUG_STS      1
#define  N_THRM_STS          0

#define R_ACPI_GPE0_EN        0x2C
#define S_ACPI_GPE0_EN           4
#define  B_GPInn_EN             0xFFFF0000
#define    V_GPI16_EN           0x80000000
#define  B_SATA_SCI_EN          0x00008000
#define  B_USB4_EN              0x00004000
#define  B_PME_B0_EN            0x00002000
#define  B_USB3_EN              0x00001000
#define  B_PME_EN               0x00000800
#define  B_BATLOW_EN            0x00000400
#define  B_PCI_EXP_EN           0x00000200
#define  B_RI_EN                0x00000100
#define  B_TC0SCI_EN            0x00000040
#define  B_AC97_EN              0x00000020
#define  B_USB2_EN              0x00000010
#define  B_USB1_EN              0x00000008
#define  B_SWGPE_EN             0x00000004
#define  B_HOT_PLUG_EN          0x00000002
#define  B_THRM_EN              0x00000001
#define  N_SATA_SCI_EN       15
#define  N_USB4_EN           14
#define  N_PME_B0_EN         13
#define  N_USB3_EN           12
#define  N_PME_EN            11
#define  N_BATLOW_EN         10
#define  N_PCI_EXP_EN        9
#define  N_RI_EN             8
#define  N_TC0SCI_EN         6
#define  N_AC97_EN           5
#define  N_USB2_EN           4
#define  N_USB1_EN           3
#define  N_SWGPE_EN          2
#define  N_HOT_PLUG_EN       1
#define  N_THRM_EN           0

#define R_ACPI_SMI_EN         0x30
#define S_ACPI_SMI_EN            4
#define  B_INTEL_USB2_EN        0x00040000
#define  B_LEGACY_USB2_EN       0x00020000
#define  B_PERIODIC_EN          0x00004000
#define  B_TCO_EN               0x00002000
#define  B_MCSMI_EN             0x00000800
#define  B_BIOS_RLS             0x00000080
#define  B_SWSMI_TMR_EN         0x00000040
#define  B_APMC_EN              0x00000020
#define  B_SMI_ON_SLP_EN        0x00000010
#define  B_LEGACY_USB_EN        0x00000008
#define  B_BIOS_EN              0x00000004
#define  B_EOS                  0x00000002
#define  B_GBL_SMI_EN           0x00000001
#define  N_INTEL_USB2_EN     18
#define  N_LEGACY_USB2_EN    17
#define  N_PERIODIC_EN       14
#define  N_TCO_EN            13
#define  N_MCSMI_EN          11
#define  N_BIOS_RLS          7
#define  N_SWSMI_TMR_EN      6
#define  N_APMC_EN           5
#define  N_SMI_ON_SLP_EN     4
#define  N_LEGACY_USB_EN     3
#define  N_BIOS_EN           2
#define  N_EOS               1
#define  N_GBL_SMI_EN        0

#define V_ACPI_TIMER_MAX_VALUE      0x1000000 // The timer is 24 bit overflow

#define R_ACPI_SMI_STS        0x34
#define S_ACPI_SMI_STS           4
#define  B_SPI_SMI_STS          0x04000000
#define  B_INTERNAL_TT_STS      0x00400000
#define  B_MONITOR_STS          0x00200000
#define  B_PCI_EXP_SMI_STS      0x00100000
#define  B_PATCH_SMI_STS        0x00080000
#define  B_INTEL_USB2_STS       0x00040000
#define  B_LEGACY_USB2_STS      0x00020000
#define  B_SMBUS_SMI_STS        0x00010000
#define  B_SERIRQ_SMI_STS       0x00008000
#define  B_PERIODIC_STS         0x00004000
#define  B_TCO_STS              0x00002000
#define  B_DEVMON_STS           0x00001000
#define  B_MCSMI_STS            0x00000800
#define  B_GPE1_STS             0x00000400
#define  B_GPE0_STS             0x00000200
#define  B_PM1_STS_REG          0x00000100
#define  B_SWSMI_TMR_STS        0x00000040
#define  B_APM_STS              0x00000020
#define  B_SMI_ON_SLP_EN_STS    0x00000010
#define  B_LEGACY_USB_STS       0x00000008
#define  B_BIOS_STS             0x00000004
#define  N_SPI_SMI_STS       26
#define  N_INTERNAL_TT_STS   22
#define  N_MONITOR_STS       21
#define  N_PCI_EXP_SMI_STS   20
#define  N_PATCH_SMI_STS     19
#define  N_INTEL_USB2_STS    18
#define  N_LEGACY_USB2_STS   17
#define  N_SMBUS_SMI_STS     16
#define  N_SERIRQ_SMI_STS    15
#define  N_PERIODIC_STS      14
#define  N_TCO_STS           13
#define  N_DEVMON_STS        12
#define  N_MCSMI_STS         11
#define  N_GPE1_STS          10
#define  N_GPE0_STS          9
#define  N_PM1_STS_REG       8
#define  N_SWSMI_TMR_STS     6
#define  N_APM_STS           5
#define  N_SMI_ON_SLP_EN_STS 4
#define  N_LEGACY_USB_STS    3
#define  N_BIOS_STS          2

#define R_ACPI_ALT_GP_SMI_EN  0x38
#define S_ACPI_ALT_GP_SMI_EN  2
#define R_ACPI_ALT_GP_SMI_STS 0x3A
#define S_ACPI_ALT_GP_SMI_STS 2

#define R_ACPI_DEVACT_STS       0x44
#define S_ACPI_DEVACT_STS          2
#define B_ACPI_DEVACT_STS_MASK  0x13C1
#define   B_KBC_ACT_STS           0x1000
#define   B_PIRQDH_ACT_STS        0x0200
#define   B_PIRQCG_ACT_STS        0x0100
#define   B_PIRQBF_ACT_STS        0x0080
#define   B_PIRQAE_ACT_STS        0x0040
#define   B_IDE_ACT_STS           0x0001
#define   N_KBC_ACT_STS           12
#define   N_PIRQDH_ACT_STS        9
#define   N_PIRQCG_ACT_STS        8
#define   N_PIRQBF_ACT_STS        7
#define   N_PIRQAE_ACT_STS        6
#define   N_IDE_ACT_STS           0
#define R_ACPI_GYS_CNT          0x50
#define   B_G_STATE               0x01

//
// TCO register I/O map
//

#define  TCO_BASE             0x60
#define  R_TCO_RLD            0x0000
#define  R_TCO_DAT_IN         0x02
#define  R_TCO_DAT_OUT        0x03

#define  R_TCO1_STS           0x04
#define  S_TCO1_STS           2
#define   B_DMISERR_STS         0x1000
#define   B_DMISMI_STS          0x0400
#define   B_DMISCI_STS          0x0200
#define   B_BIOSWR_STS          0x0100
#define   B_NEWCENTURY_STS      0x0080
#define   B_TIMEOUT             0x0008
#define   B_TCO_INT_STS         0x0004
#define   B_SW_TCO_SMI          0x0002
#define   B_NMI2SMI_STS         0x0001
#define  N_DMISMI_STS        10
#define  N_BIOSWR_STS        8
#define  N_NEWCENTURY_STS    7
#define  N_TIMEOUT           3
#define  N_SW_TCO_SMI        1
#define  N_NMI2SMI_STS       0

#define  R_TCO2_STS           0x06
#define  S_TCO2_STS           2
#define   B_SMLINK_SLV_SMI_STS  0x0010
#define   B_BOOT_STS            0x0004
#define   B_SECOND_TO_STS       0x0002
#define   B_INTRD_DET           0x0001
#define  N_INTRD_DET         0

#define  R_TCO1_CNT           0x08
#define  S_TCO1_CNT           2
#define   B_TCO_LOCK            BIT12
#define   B_TCO_TMR_HLT         BIT11
#define   B_SEND_NOW            BIT10
#define   B_NMI2SMI_EN          BIT9
#define   B_NMI_NOW             BIT8
#define  N_NMI2SMI_EN        9

#define  R_TCO2_CNT           0x0A
#define  S_TCO2_CNT           2
#define   B_OS_POLICY           0x0030
#define   B_GPI11_ALERT_DISABLE 0x0008
#define   B_INTRD_SEL           0x0006
#define  N_INTRD_SEL          2

#define  R_TCO_MESSAGE1       0x0C
#define  R_TCO_MESSAGE2       0x0D
#define  R_TCO_WDCNT          0x0E
#define  R_SW_IRQ_GEN         0x10
#define  R_TCO_TMR            0x12

//
// GPIO Init register offsets from GPIOBASE
//
#define GPIO_USE_SEL                      0x00
#define GPIO_IO_SEL                       0x04
#define GPIO_LVL                          0x0C
#define GPIO_BLINK                        0x18
#define GPIO_INV                          0x2C
#define GPIO_USE_SEL2                     0x30
#define GPIO_IO_SEL2                      0x34
#define GPIO_LVL2                         0x38

//
// Processor interface registers
//
#define R_NMI_SC                               0x61
#define   B_NMI_SC_SERR_NMI_STS                BIT7
#define   B_NMI_SC_IOCHK_NMI_STS               BIT6
#define   B_NMI_SC_TMR2_OUT_STS                BIT5
#define   B_NMI_SC_REF_TOGGLE                  BIT4
#define   B_NMI_SC_IOCHK_NMI_EN                BIT3
#define   B_NMI_SC_PCI_SERR_EN                 BIT2
#define   B_NMI_SC_SPKR_DAT_EN                 BIT1
#define   B_NMI_SC_TIM_CNT2_EN                 BIT0
#define R_NMI_EN                               0x70
#define   B_NMI_EN_NMI_EN                      BIT7


//
// PATA Controller Registers (D31:F1)
//
#define PCI_DEVICE_NUMBER_ICH_IDE              31
#define PCI_FUNCTION_NUMBER_ICH_IDE            1

#define R_ICH_IDE_VENDOR_ID                    0x00
#define   V_ICH_IDE_VENDOR_ID                    V_ICH_INTEL_VENDOR_ID
#define R_ICH_IDE_DEVICE_ID                    0x02
#define   V_ICH_IDE_DEVICE_ID                    0x27DF
#define R_ICH_IDE_COMMAND                      0x04
#define   B_ICH_IDE_COMMAND_INT_DIS              0x0400
#define   B_ICH_IDE_COMMAND_FBE                  0x0200
#define   B_ICH_IDE_COMMAND_SERR_EN              0x0100
#define   B_ICH_IDE_COMMAND_WCC                  0x0080
#define   B_ICH_IDE_COMMAND_PER                  0x0040
#define   B_ICH_IDE_COMMAND_VPS                  0x0020
#define   B_ICH_IDE_COMMAND_PMWE                 0x0010
#define   B_ICH_IDE_COMMAND_SCE                  0x0008
#define   B_ICH_IDE_COMMAND_BME                  0x0004
#define   B_ICH_IDE_COMMAND_MSE                  0x0002
#define   B_ICH_IDE_COMMAND_IOSE                 0x0001
#define R_ICH_IDE_BASE_ADDRESS                 0x20
#define   B_ICH_IDE_BASE_ADDRESS_BAR             0xFFE0
#define R_ICH_IDE_TIMP                         0x40
#define R_ICH_IDE_TIMS                         0x42
#define   B_ICH_IDE_DEC_EN                       0x8000
#define R_ICH_SLV_IDETIM                       0x44
#define R_ICH_SDMA_CNT                         0x48
#define R_ICH_SDMA_TIM                         0x4C
#define R_ICH_IDE_CONFIG                       0x54


#define V_ICH_IDE_SVID                         V_ICH_INTEL_VENDOR_ID
#define V_ICH_IDE_SID                          V_ICH_DEFAULT_SID

//
//  SATA Controller Registers (D31:F2)
//

#define PCI_DEVICE_NUMBER_ICH_SIDE              31
#define PCI_FUNCTION_NUMBER_ICH_SIDE            2
#define R_ICH_SIDE_VENDOR_ID                    0x00
#define   V_ICH_SIDE_VENDOR_ID                    V_ICH_INTEL_VENDOR_ID
#define R_ICH_SIDE_DEVICE_ID                    0x02
#define   V_ICH_SIDE_DEVICE_ID_0                  0x27C0
#define   V_ICH_SIDE_DEVICE_ID_1                  0x27C1
#define   V_ICH_SIDE_DEVICE_ID_2                  0x27C2

#define R_ICH_SIDE_COMMAND                      0x04
#define   B_ICH_SIDE_COMMAND_INT_DIS              0x0400
#define   B_ICH_SIDE_COMMAND_FBE                  0x0200
#define   B_ICH_SIDE_COMMAND_SERR_EN              0x0100
#define   B_ICH_SIDE_COMMAND_WCC                  0x0080
#define   B_ICH_SIDE_COMMAND_PER                  0x0040
#define   B_ICH_SIDE_COMMAND_VPS                  0x0020
#define   B_ICH_SIDE_COMMAND_PMWE                 0x0010
#define   B_ICH_SIDE_COMMAND_SCE                  0x0008
#define   B_ICH_SIDE_COMMAND_BME                  0x0004
#define   B_ICH_SIDE_COMMAND_MSE                  0x0002
#define   B_ICH_SIDE_COMMAND_IOSE                 0x0001
#define R_ICH_SIDE_INTERFACE_REGISTER           0x09
#define R_ICH_SIDE_SUB_CLASS_CODE               0x0A
#define R_ICH_SIDE_BASE_ADDRESS                 0x20
#define   B_ICH_SIDE_BASE_ADDRESS_BAR             0xFFE0
#define R_ICH_SIDE_AHCI_BAR                     0x24
#define R_ICH_SIDE_TIMP                         0x40
#define   B_ICH_SIDE_TIM_TIME0                    BIT0
#define   B_ICH_SIDE_TIM_TIME1                    BIT4
#define   B_ICH_SIDE_TIM_DTE0                     BIT3
#define   B_ICH_SIDE_TIM_DTE1                     BIT7
#define R_ICH_SIDE_TIMS                         0x42
#define R_ICH_SIDE_SIDETIM                      0x44
#define R_ICH_SIDE_SDMA_CNT                     0x48
#define   B_ICH_SIDE_SDMA_CNT_SSDE1               BIT3
#define   B_ICH_SIDE_SDMA_CNT_SSDE0               BIT2
#define   B_ICH_SIDE_SDMA_CNT_PSDE1               BIT1
#define   B_ICH_SIDE_SDMA_CNT_PSDE0               BIT0
#define R_ICH_SIDE_SDMA_TIM                     0x4A
#define   V_ICH_SIDE_SDMA_TIM_CT4_RP6             0x00
#define   V_ICH_SIDE_SDMA_TIM_CT3_RP5             0x01
#define   V_ICH_SIDE_SDMA_TIM_CT2_RP4             0x02
#define   V_ICH_SIDE_SDMA_TIM_CT3_RP8             0x01
#define   V_ICH_SIDE_SDMA_TIM_CT2_RP8             0x02
#define   V_ICH_SIDE_SDMA_TIM_CT3_RP16            0x01
#define R_ICH_SIDE_IDE_CONFIG                   0x54
#define   B_ICH_SIDE_IDE_CONFIG_FAST_SCB1         BIT15
#define   B_ICH_SIDE_IDE_CONFIG_FAST_SCB0         BIT14
#define   B_ICH_SIDE_IDE_CONFIG_FAST_PCB1         BIT13
#define   B_ICH_SIDE_IDE_CONFIG_FAST_PCB0         BIT12
#define   B_ICH_SIDE_IDE_CONFIG_SCB1              BIT3
#define   B_ICH_SIDE_IDE_CONFIG_SCB0              BIT2
#define   B_ICH_SIDE_IDE_CONFIG_PCB1              BIT1
#define   B_ICH_SIDE_IDE_CONFIG_PCB0              BIT0
#define R_ICH_SIDE_PID                          0x70
#define R_ICH_SIDE_PC                           0x72
#define R_ICH_SIDE_PMCS                         0x74
#define R_ICH_SIDE_MID                          0x80
#define R_ICH_SIDE_MC                           0x82
#define R_ICH_SIDE_MA                           0x84 
#define R_ICH_SIDE_MD                           0x88
#define R_ICH_SIDE_MAP                          0x90
#define   B_ICH_SIDE_MAP_MSM_MASK                 (BIT7 | BIT6)
#define   N_ICH_SIDE_MAP_USCC                     7
#define   B_ICH_SIDE_MAP_USCC                     0x80
#define    B_ICH_SIDE_MAP_MV_MASK                      (BIT1 | BIT0)
#define    V_ICH_SIDE_MAP_MV_NON_COMBINED               0x00
#define    V_ICH_SIDE_MAP_MV_COMBINED                   0x02
#define R_ICH_SIDE_PCS                          0x92
#define R_ICH_SIDE_SIR                          0x94
#define R_ICH_SIDE_BFCS                         0xE0
#define R_ICH_SIDE_BFTD1                        0xE4
#define R_ICH_SIDE_BFTD2                        0xE8
#define V_ICH_IDE_SVID                            V_ICH_INTEL_VENDOR_ID
#define V_ICH_IDE_SID                             V_ICH_DEFAULT_SID

//
// SMBus Controller Registers (D31:F3)
//
#define PCI_DEVICE_NUMBER_ICH_SMBUS            31
#define PCI_FUNCTION_NUMBER_ICH_SMBUS          3
#define   V_ICH_SMBUS_SVID                       V_ICH_INTEL_VENDOR_ID
#define   V_ICH_SMBUS_SID                        V_ICH_DEFAULT_SID

#define R_ICH_SMBUS_VID                         0x00
#define   V_ICH_SMBUS_VID                         0x8086
#define R_ICH_SMBUS_DID                         0x02  
#define   V_ICH_SMBUS_DID                         0x27DA
#define R_ICH_SMBUS_PCICMD                      0x04  
#define   B_ICH_SMBUS_PCICMD_IOSE                 BIT0
#define R_ICH_SMBUS_SCC                         0x0A  
#define   V_ICH_SMBUS_SCC                         0x05  
#define R_ICH_SMBUS_BCC                         0x0B  
#define   V_ICH_SMBUS_BCC                         0x0C
#define R_ICH_SMBUS_BASE                        0x20
#define   V_ICH_SMBUS_BASE_SIZE                   (1 << 5)
#define R_ICH_SMBUS_HOSTC                       0x40
#define   B_ICH_SMBUS_HOSTC_I2C_EN                BIT2
#define   B_ICH_SMBUS_HOSTC_SMI_EN                BIT1
#define   B_ICH_SMBUS_HOSTC_HST_EN                BIT0

//
// SMBus I/O Registers
//
#define SMBUS_R_HSTS                  0x00    // Host Status Register R/W
#define   SMBUS_B_HBSY                  0x01  
#define   SMBUS_B_INTR                  0x02  
#define   SMBUS_B_DERR                  0x04  
#define   SMBUS_B_BERR                  0x08  
#define   SMBUS_B_FAIL                  0x10  
#define   SMBUS_B_SMBALERT_STS          0x20  
#define   SMBUS_B_IUS                   0x40  
#define   SMBUS_B_BYTE_DONE_STS         0x80  
#define   SMBUS_B_HSTS_ALL              0xFF  
#define SMBUS_R_HCTL                  0x02    // Host Control Register R/W
#define   SMBUS_B_INTREN                0x01  
#define   SMBUS_B_KILL                  0x02  
#define   SMBUS_B_SMB_CMD               0x1C  
#define     SMBUS_V_SMB_CMD_QUICK         0x00
#define     SMBUS_V_SMB_CMD_BYTE          0x04
#define     SMBUS_V_SMB_CMD_BYTE_DATA     0x08
#define     SMBUS_V_SMB_CMD_WORD_DATA     0x0C
#define     SMBUS_V_SMB_CMD_PROCESS_CALL  0x10
#define     SMBUS_V_SMB_CMD_BLOCK         0x14
#define     SMBUS_V_SMB_CMD_IIC_READ      0x18
#define   SMBUS_B_LAST_BYTE             0x20  
#define   SMBUS_B_START                 0x40  
#define   SMBUS_B_PEC_EN                0x80  
#define SMBUS_R_HCMD                  0x03    // Host Command Register R/W
#define SMBUS_R_TSA                   0x04    // Transmit Slave Address Register R/W
#define   SMBUS_B_RW_SEL                0x01  
#define   SMBUS_B_ADDRESS               0xFE  
#define SMBUS_R_HD0                   0x05    // Data 0 Register R/W
#define SMBUS_R_HD1                   0x06    // Data 1 Register R/W
#define SMBUS_R_HBD                   0x07    // Host Block Data Register R/W
#define SMBUS_R_PEC                   0x08    // Packet Error Check Data Register R/W
#define SMBUS_R_RSA                   0x09    // Receive Slave Address Register R/W
#define   SMBUS_B_SLAVE_ADDR            0x7F  
#define SMBUS_R_SD                    0x0A    // Receive Slave Data Register R/W
#define SMBUS_R_AUXS                  0x0C    // Auxiliary Status Register R/WC
#define   SMBUS_B_CRCE                  0x01  
#define SMBUS_R_AUXC                  0x0D    // Auxiliary Control Register R/W
#define   SMBUS_B_AAC                  0x01  
#define   SMBUS_B_E32B                 0x02  
#define SMBUS_R_SMLC                  0x0E    // SMLINK Pin Control Register R/W
#define   SMBUS_B_SMLINK0_CUR_STS       0x01  
#define   SMBUS_B_SMLINK1_CUR_STS       0x02  
#define   SMBUS_B_SMLINK_CLK_CTL        0x04  
#define SMBUS_R_SMBC                  0x0F    // SMBus Pin Control Register R/W
#define   SMBUS_B_SMBCLK_CUR_STS        0x01  
#define   SMBUS_B_SMBDATA_CUR_STS       0x02  
#define   SMBUS_B_SMBCLK_CTL            0x04  
#define SMBUS_R_SSTS                  0x10    // Slave Status Register R/WC
#define   SMBUS_B_HOST_NOTIFY_STS       0x01  
#define SMBUS_R_SCMD                  0x11    // Slave Command Register R/W
#define   SMBUS_B_HOST_NOTIFY_INTREN    0x01  
#define   SMBUS_B_HOST_NOTIFY_WKEN      0x02  
#define   SMBUS_B_SMBALERT_DIS          0x04  
#define SMBUS_R_NDA                   0x14    // Notify Device Address Register RO
#define   SMBUS_B_DEVICE_ADDRESS        0xFE  
#define SMBUS_R_NDLB                  0x16    // Notify Data Low Byte Register RO
#define SMBUS_R_NDHB                  0x17    // Notify Data High Byte Register RO

//
// AC'97 Audio Definitions
//      

#define PCI_DEVICE_NUMBER_ICH_AC97             30
#define PCI_FUNCTION_NUMBER_ICH_AC97_AUDIO     2

#define R_ICH_AUDIO_VENDOR_ID                    0x00
#define   V_ICH_AUDIO_VENDOR_ID                  V_ICH_INTEL_VENDOR_ID
#define R_ICH_AUDIO_DEVICE_ID                  0x02
#define   V_ICH_AUDIO_DEVICE_ID_0                0x27DE
#define R_ICH_AUDIO_COMMAND                    0x04
#define   B_ICH_AUDIO_COMMAND_MSE                0x0002
#define   B_ICH_AUDIO_COMMAND_IOSE               0x0001
#define R_ICH_AUDIO_NAMBAR                      0x10
#define   V_ICH_AUDIO_NAMBAR_SIZE                 (1 << 8)
#define R_ICH_AUDIO_NABMBAR                     0x14
#define   V_ICH_AUDIO_NABMBAR_SIZE                (1 << 6)
#define R_ICH_AUDIO_MMBAR                      0x18
#define   V_ICH_AUDIO_MMBAR_SIZE                  (1 << 9)  
#define R_ICH_AUDIO_MBBAR                      0x1C
#define   V_ICH_AUDIO_MBBAR_SIZE                  (1 << 8)  
#define R_ICH_AUDIO_SVID                        0x2C
#define R_ICH_AUDIO_SID                         0x2E
#define R_ICH_AUDIO_CFG                         0x41
#define   B_ICH_AUDIO_CFG_IOSE                    BIT0

//
// AC'97 Audio Mixer Registers
//
#define R_AC97_AM_MASTER_VOLUME                 0x02
#define R_AC97_AM_VENDOR_ID1                    0x7C
#define R_AC97_AM_VENDOR_ID2                    0x7E

//
// AC'97 Audio Bus Master Control Registers
//
#define R_AC97_ABM_GLOBAL_CONTROL               0x2C
#define   B_AC97_ABM_COLD_RESET                   BIT1
#define R_AC97_ABM_GLOBAL_STATUS                0x30
#define   B_AC97_ABM_AC_SDIN0                     BIT8
#define   B_AC97_ABM_AC_SDIN1                     BIT9
#define   B_AC97_ABM_AC_SDIN2                     BIT28
#define R_AC97_ABM_CODE_ACCESS_SEM              0x34
#define   B_AC97_ABM_CAS                          BIT0


//
// AC'97 Modem Definitions
//      

#define PCI_FUNCTION_NUMBER_ICH_AC97_MODEM     3

#define R_ICH_MODEM_DEVICE_ID                  0x02
#define   V_ICH_MODEM_DEVICE_ID_0                0x27DD
#define R_ICH_MODEM_COMMAND                    0x04
#define   B_ICH_MODEM_COMMAND_MSE                 BIT1
#define   B_ICH_MODEM_COMMAND_IOSE                BIT0
#define R_ICH_MODEM_MMBAR                      0x10
#define   V_ICH_MODEM_MMBAR_SIZE                  (1 << 8)
#define R_ICH_MODEM_MBBAR                      0x14
#define   V_ICH_MODEM_MBBAR_SIZE                  (1 << 7)
#define R_ICH_MODEM_SVID                        0x2C
#define R_ICH_MODEM_SID                         0x2E

//
// AC'97 Modem Mixer Registers
//
#define R_AC97_MM_EXTENDED_ID                   0x3C
#define   V_AC97_MM_CODEC_MASK                    0xC000
#define   R_AC97_MM_PRIMARY_CODEC                 (0 << 14)
#define   R_AC97_MM_SECONDARY_CODEC               (1 << 14)
#define   B_AC97_MM_CODEC_LIN1_INDICATION         BIT0
#define R_AC97_MM_VENDOR_ID1                    0x7C
#define R_AC97_MM_VENDOR_ID2                    0x7E

#define AC97_MM_SECONDARY_CODEC_BASE            0x80

//
// AC'97 Modem Bus Master Control Registers
//
#define R_AC97_MBM_GLOBAL_STATUS                0x40


//
// USB Definitions
//

#define PCI_DEVICE_NUMBER_ICH_USB              29

#define R_ICH_EHCI_ACCESS_CNTL                 0x80
#define   V_ICH_EHCI_ACCESS_CNTL_ENABLE          0x01
#define   V_ICH_EHCI_ACCESS_CNTL_DISABLE         0x00

#define V_ICH_USB_SVID                         V_ICH_INTEL_VENDOR_ID
#define V_ICH_USB_SID                          V_ICH_DEFAULT_SID

//
// LAN Controller Registers (D8:F0)
//

//
// ICH7M Pci Express Root Ports (D28:F0~5)
//
#define PCI_DEVICE_NUMBER_ICH_ROOTPORT         28
#define PCI_FUNCTION_NUMBER_ICH_ROOTPORT_1     0 
#define PCI_FUNCTION_NUMBER_ICH_ROOTPORT_2     1 
#define PCI_FUNCTION_NUMBER_ICH_ROOTPORT_3     2 
#define PCI_FUNCTION_NUMBER_ICH_ROOTPORT_4     3 
#define PCI_FUNCTION_NUMBER_ICH_ROOTPORT_5     4 
#define PCI_FUNCTION_NUMBER_ICH_ROOTPORT_6     5 
#define R_ICH_PCIE_RPDCGEN                    0xE1 // PCIE Root Port Dynamic Clock Gate Enable
#define   B_ICH_PCIE_RPDCGEN_SRDLCGEN           BIT3 // Shared Resouce Dynamic Link Clock Gate Enable
#define   B_ICH_PCIE_RPDCGEN_SRDBCGEN           BIT2 // Shared Resouce Dynamic Backbone Clock Gate Enable
#define   B_ICH_PCIE_RPDCGEN_RPDLCGEN           BIT1 // Root Port Dynamic Link Clock Gate Enable
#define   B_ICH_PCIE_RPDCGEN_RPDBCGEN           BIT0 // Root Port Dynamic Backbone Clock Gate Enable


//
// Misc PCI register offsets and sizes
//
#define R_EFI_PCI_SVID                          0x2C
#define   V_EFI_PCI_SVID_SIZE                     0x2
#define R_EFI_PCI_SID                           0x2E
#define   V_EFI_PCI_SID_SIZE                      0x2
#define R_ICH_USB_DRIVE_STRENGTH               0xD8

#define IO_APIC_INDEX_REGISTER                  0xFEC00000
#define IO_APIC_DATA_REGISTER                   0xFEC00010
#define IO_APIC_BOOT_CONFIG_INDEX               0x03
#define IO_APIC_USE_SYSTEM_BUS                  0x01
#define R_APM_CNT                              0xB2
#define R_APM_STS                              0xB3

//
// Chipset configuration registers (Memory space)
// RCBA
//
#define ICH_RCRB_V0CAP           0x0010   // Virtual channel 0 resource capability
#define ICH_RCRB_V0CTL           0x0014   // Virtual channel 0 resource control
#define   B_RCRB_V0CTL_EN           BIT31
#define   V_RCRB_V0CTL_ID_MASK      (3 << 24)
#define   V_RCRB_V0CTL_TVM_MASK     0xFF
#define ICH_RCRB_V0STS           0x001A   // Virtual channel 0 status
#define ICH_RCRB_V1CAP           0x001C   // Virtual channel 1 resource capability
#define ICH_RCRB_V1CTL           0x0020   // Virtual channel 1 resource control
#define   B_RCRB_V1CTL_EN           BIT31
#define   V_RCRB_V1CTL_ID_MASK      (3 << 24)
#define   V_RCRB_V1CTL_TVM_MASK     0xFF
#define ICH_RCRB_V1STS           0x0026   // Virtual channel 1 status
#define   B_RCRB_V1STS_NP           BIT1
#define   B_RCRB_V1STS_ATS          BIT0
#define ICH_RCRB_V1PAT           0x0030   // Virtual channel 1 port arbitration table

#define ICH_RCRB_VPSTS           0x01FA   // Private virtual channel status
#define ICH_RCRB_UEM             0x0088   // Uncorrectable error mask
#define   B_RCRB_UEM_URE            BIT20 // Unsupported Request error mask
#define   B_RCRB_UEM_MT             BIT18 // Malformed TLP mask
#define   B_RCRB_UEM_RO             BIT17 // Receiver Overflow mask
#define   B_RCRB_UEM_UC             BIT16 // Unexpected Completion mask
#define   B_RCRB_UEM_CM             BIT15 // Completor abort Mask
#define   B_RCRB_UEM_CT             BIT14 // Completion Timeout Mask
#define   B_RCRB_UEM_PT             BIT12 // Poisoned TLP Mask
#define   B_RCRB_UEM_DLPE           BIT4  // Data Link Protocol Error mask
#define ICH_RCRB_ESD             0x0104   // Element Self Description
#define ICH_RCRB_ULD             0x0110   // Upstream Link Description
#define ICH_RCRB_ULBA            0x0114   // Upstream Link Base Address
#define ICH_RCRB_LCAP            0x01A4   // Link capabilities
#define   B_RCRB_LCAP_APMS       (BIT11 | BIT10)   // L0 is supported on DMI
#define ICH_RCRB_LCTL            0x01A8   // Link control
#define ICH_RCRB_L3A1            0x0200   // Level 3 Backbone Arbiter Configuration
#define ICH_RCRB_UNRL            0x0214   // Upstream Non-posted Request Limits
#define   B_RCRB_UNRL_V1D           BIT15 // VC1 Upstream Request Limit Disable
#define   B_RCRB_UNRL_V0D           BIT7  // VC0 Upstream Request Limit Disable
#define ICH_RCRB_UMR             0x0218   // Upstream Minimum Reserved
#define ICH_RCRB_GBC             0x0220   // Generic Backbone Configuration
#define   B_RCRB_GBC_CTD            BIT7  // Completer Timeout Disable
#define   B_RCRB_GBC_CAM            BIT6  // Credit Allocated Update Mode
#define ICH_RCRB_RPC             0x0224   // Root port configuration
#define ICH_RCRB_BAC             0x0228   // Bandwidth allocation configuration
#define ICH_RCRB_DMI_CNTL        0x0234   // DMI control register
#define   B_RCRB_DMI_CNTL_DMILCGEN  BIT1  // DMI Link Dynamic Clock Gate Enable
#define   B_RCRB_DMI_CNTL_DMIBCGEN  BIT0  // DMI Backbone Dynamic Clock Gate Enable
#define ICH_RCRB_TRSR            0x1E00   // Trap Status Register
#define   B_TRSR_CTSS_MASK       0x000F   // Cycle Trap SMI# Status mask
#define ICH_RCRB_TRCR            0x1E10   // Trapped Cycle Register
#define ICH_RCRB_TRWDR           0x1E18   // Trap Write Data Register
#define ICH_RCRB_IO_TRAP_0       0x1E80   // Trap Configuration Register
#define ICH_RCRB_IO_TRAP_1       0x1E88   // Trap Configuration Register
#define ICH_RCRB_IO_TRAP_2       0x1E90   // Trap Configuration Register
#define ICH_RCRB_IO_TRAP_3       0x1E98   // Trap Configuration Register
#define   B_TRAP_AND_SMI_ENABLE  BIT0     // Enable trapping bit
#define ICH_RCRB_DMI_LINK_CNTL   0x2010   // DMI port DFT link control register
#define ICH_RCRB_DMI_DBG         0x2024   // DMI debug and configuration
#define ICH_RCRB_DMI_DBG_FTS     0x2027   // DMI debug and configuration, Fast Training Sequence byte
#define ICH_RCRB_TCO_CNTL        0x3000   // TCO control register
#define ICH_RCRB_D31IP           0x3100   // Device 31 interrupt pin
#define ICH_RCRB_D30IP           0x3104   // Device 30 interrupt pin
#define ICH_RCRB_D29IP           0x3108   // Device 29 interrupt pin
#define ICH_RCRB_D28IP           0x310C   // Device 28 interrupt pin
#define ICH_RCRB_D27IP           0x3110   // Device 27 interrupt pin
#define ICH_RCRB_D31IR           0x3140   // Device 31 interrupt route
#define   B_DXXIR_IDR_MASK       (BIT14|BIT13|BIT12)
#define   V_DXXIR_IDR_PIRQA      (0)
#define   V_DXXIR_IDR_PIRQB                  (BIT12)
#define   V_DXXIR_IDR_PIRQC            (BIT13)
#define   V_DXXIR_IDR_PIRQD            (BIT13|BIT12)
#define   V_DXXIR_IDR_PIRQE      (BIT14)
#define   V_DXXIR_IDR_PIRQF      (BIT14|      BIT12)
#define   V_DXXIR_IDR_PIRQG      (BIT14|BIT13)
#define   V_DXXIR_IDR_PIRQH      (BIT10|BIT9 |BIT8)
#define   V_DXXIR_ICR_PIRQA      (0)
#define   V_DXXIR_ICR_PIRQB                  (BIT8)
#define   V_DXXIR_ICR_PIRQC            (BIT9)
#define   V_DXXIR_ICR_PIRQD            (BIT9 |BIT8)
#define   V_DXXIR_ICR_PIRQE      (BIT10)
#define   V_DXXIR_ICR_PIRQF      (BIT10|      BIT8)
#define   V_DXXIR_ICR_PIRQG      (BIT10|BIT9)
#define   V_DXXIR_ICR_PIRQH      (BIT10|BIT9 |BIT8)
#define   V_DXXIR_IBR_PIRQA      (0)
#define   V_DXXIR_IBR_PIRQB                  (BIT4)
#define   V_DXXIR_IBR_PIRQC            (BIT5)
#define   V_DXXIR_IBR_PIRQD            (BIT5 |BIT4)
#define   V_DXXIR_IBR_PIRQE      (BIT6 )
#define   V_DXXIR_IBR_PIRQF      (BIT6 |      BIT4)
#define   V_DXXIR_IBR_PIRQG      (BIT6 |BIT5)
#define   V_DXXIR_IBR_PIRQH      (BIT6 |BIT5 |BIT4)
#define   V_DXXIR_IAR_PIRQA      (0)
#define   V_DXXIR_IAR_PIRQB                  (BIT0)
#define   V_DXXIR_IAR_PIRQC            (BIT1)
#define   V_DXXIR_IAR_PIRQD            (BIT1 |BIT0)
#define   V_DXXIR_IAR_PIRQE      (BIT2 )
#define   V_DXXIR_IAR_PIRQF      (BIT2 |      BIT0)
#define   V_DXXIR_IAR_PIRQG      (BIT2 |BIT1)
#define   V_DXXIR_IAR_PIRQH      (BIT2 |BIT1 |BIT0)
#define ICH_RCRB_D30IR           0x3142         // Device 30 interrupt route
#define ICH_RCRB_D29IR           0x3144         // Device 29 interrupt route
#define ICH_RCRB_D28IR           0x3146         // Device 28 interrupt route
#define ICH_RCRB_D27IR           0x3148         // Device 27 interrupt route
#define ICH_RCRB_OIC             0x31FF         // Other Interrupt Control
#define   B_RCRB_OIC_CEN         BIT1           // Coprocessor Error Enable
#define   B_RCRB_OIC_AEN         BIT0           // APIC Enable
#define ICH_RCRB_RTC_CONF        0x3400         // RTC Configuration register
#define   ICH_RCRB_RTC_CONF_UCMOS_EN  BIT2      // Upper CMOS bank enable
#define ICH_RCRB_HPTC            0x3404         // High Performance Timer Configuration
#define ICH_RCRB_HPTC_AS         BIT1+BIT0      // Address selection
#define ICH_RCRB_HPTC_AE         BIT7           // Address enable
#define ICH_RCRB_GCS             0x3410         // General Control and Status
#define   B_RCRB_GCS_BBS         (BIT11 + BIT10)// Boot BIOS straps
#define   V_RCRB_GCS_BBS_SPI     BIT10          // Boot BIOS strapped to SPI
#define   V_RCRB_GCS_BBS_PCI     BIT11          // Boot BIOS strapped to PCI
#define   V_RCRB_GCS_BBS_LPC     (BIT11 + BIT10)// Boot BIOS strapped to LPC
#define   B_RCRB_GCS_SERM        BIT9           // Server Error Reporting Mode
#define   B_RCRB_GCS_MICLD       BIT7           // Mobile IDE Configuration Lock Down
#define   B_RCRB_GCS_FME         BIT6           // FERR# MUX Enable
#define   B_RCRB_GCS_NR          BIT5           // No Reboot strap
#define   B_RCRB_GCS_AME         BIT4           // Alternate Access Mode Enable
#define   B_RCRB_GCS_RPR         BIT2           // Reserved Page Route
#define   B_RCRB_GCS_BILD        BIT0           // BIOS Interface Lock-Down
#define   N_RCRB_GCS_BBS         BIT10          // Boot BIOS straps offset
#define ICH_RCRB_BUC             0x3414         // Backed Up Control
#define   B_RCRB_BUC_CBE         BIT2           // CPU BIST Enable
#define   B_RCRB_BUC_PRS         BIT1           // PATA Reset State
#define   B_RCRB_BUC_TS          BIT0           // Top Swap
#define R_ICH_RCRB_FUNC_DIS        0x3418         // Function Disable Register
#define   B_RCRB_FUNC_DIS_FUNCTION_0    BIT0    // Function 0 disable (cannot be disabled per ICH BIOS specification, present for SV only)
#define   B_RCRB_FUNC_DIS_PATA          BIT1    // Parallel ATA disable
#define   B_RCRB_FUNC_DIS_SATA          BIT2    // Serial ATA disable
#define   B_RCRB_FUNC_DIS_SMBUS         BIT3    // SMBUS disable

#define   B_RCRB_FUNC_DIS_MODEM         BIT6    // Modem disable
#define   B_RCRB_FUNC_DIS_LAN           BIT7    // Internal LAN disable
#define   B_RCRB_FUNC_DIS_UHCI0         BIT8    // UHCI controller 0 disable
#define   B_RCRB_FUNC_DIS_UHCI1         BIT9    // UHCI controller 1 disable
#define   B_RCRB_FUNC_DIS_UHCI2         BIT10   // UHCI controller 2 disable
#define   B_RCRB_FUNC_DIS_UHCI3         BIT11   // UHCI controller 3 disable
#define   B_RCRB_FUNC_DIS_EHCI          BIT15   // EHCI controller disable
#define   B_RCRB_FUNC_DIS_PCI_EX_PORT1  BIT16   // PCI Express port 1 disable
#define   B_RCRB_FUNC_DIS_PCI_EX_PORT2  BIT17   // PCI Express port 2 disable
#define   B_RCRB_FUNC_DIS_PCI_EX_PORT3  BIT18   // PCI Express port 3 disable
#define   B_RCRB_FUNC_DIS_PCI_EX_PORT4  BIT19   // PCI Express port 4 disable
#define   B_RCRB_FUNC_DIS_PCI_EX_PORT5  BIT20   // PCI Express port 5 disable
#define   B_RCRB_FUNC_DIS_PCI_EX_PORT6  BIT21   // PCI Express port 6 disable
#define ICH_RCRB_PRC             0x341C         // Power Reduction Control
#define   B_RCRB_PRC_EN_DCLK_GATE_LPC    BIT31   // Legacy(LPC) Dynamic Clock Gate Enable
#define   B_RCRB_PRC_EN_DCLK_GATE_PATA   BIT30   // PATA Dynamic Clock Gate Enable
#define   B_RCRB_PRC_EN_DCLK_GATE_UHCI   (BIT29|BIT28)   // USB UHCI Dynamic Clock Gate Enable
#define   B_RCRB_PRC_EN_DCLK_GATE_SATA3  BIT27   // SATA Port3 Dynamic Clock Gate Enable
#define   B_RCRB_PRC_EN_DCLK_GATE_SATA2  BIT26   // SATA Port2 Dynamic Clock Gate Enable
#define   B_RCRB_PRC_EN_DCLK_GATE_SATA1  BIT25   // SATA Port1 Dynamic Clock Gate Enable
#define   B_RCRB_PRC_EN_DCLK_GATE_SATA0  BIT24   // SATA Port0 Dynamic Clock Gate Enable
#define   B_RCRB_PRC_EN_SCLK_GATE_AC97   BIT23   // AC97 Static Clock Gate Enable
#define   B_RCRB_PRC_EN_DCLK_GATE_HDA    BIT22   // HDA Dynamic Clock Gate Enable
#define   B_RCRB_PRC_EN_SCLK_GATE_HDA    BIT21   // HDA Static Clock Gate Enable
#define   B_RCRB_PRC_EN_SCLK_GATE_EHCI   BIT20   // USB EHCI Static Clock Gate Enable
#define   B_RCRB_PRC_EN_DCLK_GATE_EHCI   BIT19   // USB EHCI Dynamic Clock Gate Enable
#define   B_RCRB_PRC_EN_DCLK_GATE_PCI    BIT16   // PCI Dynamic Clock Gate Enable
#define   B_RCRB_PRC_EN_DCLK_GATE_RX     BIT3    // DMI and PCIE Rx Dynamic Clock Gate Enable
#define   B_RCRB_PRC_EN_DCLK_GATE_PCIE_TX BIT2   // PCIE Tx Dynamic Clock Gate Enable
#define   B_RCRB_PRC_EN_DCLK_GATE_DMI_TX  BIT1   // DMI Tx Dynamic Clock Gate Enable
#define   B_RCRB_PRC_EN_SCLK_GATE_PCIE_RP BIT0   // PCIE Root Port Static Clock Gate Enable
#define ICH_RCRB_AC              0x3430         // Audio Control
#define ICH_RCRB_TSPC0           0x3E0E         // Thermal Sensor Control Policy  0
#define ICH_RCRB_TSPC1           0x3E4E         // Thermal Sensor Control Policy  1
#define ICH_RCRB_RECO            0x3FFC         // Resume Well ECO

//
// SPI Host Interface Registers
//

#define ICH_RCRB_SPI_BASE       0x3020                        // Base address of the SPI host interface registers
#define ICH_SPI_FADDR               (ICH_RCRB_SPI_BASE + 0x08)  // SPI Flash Address
#define B_ICH_SPI_FADDR_MASK          0x01FFFFFF                    // SPI Flash Address Mask (0~24bit)

#define ICH_SPI_SPIS            ICH_RCRB_SPI_BASE + 0x00      // SPI Status
#define   B_SPI_SPIS_SCL          BIT15                       // SPI Configuration Lockdown
#define   B_SPI_SPIS_BAS          BIT3                        // Blocked Access Status
#define   B_SPI_SPIS_CDS          BIT2                        // Cycle Done Status
#define   B_SPI_SPIS_SAG          BIT1                        // SPI Access Grant
#define   B_SPI_SPIS_SCIP         BIT0                        // SPI Cycle in Progress

#define ICH_SPI_SPIC            ICH_RCRB_SPI_BASE + 0x02      // SPI Control
#define   B_SPI_SPIC_SMI          BIT15                       // SPI SMI# Enable
#define   B_SPI_SPIC_DC           BIT14                       // SPI Data Cycle
#define   B_SPI_SPIC_DBC          0x7F00                      // SPI Data Byte Count (0..8,16,24,32,40,48,56,64)
#define   B_SPI_SPIC_COP          0x0070                      // Cycle Opcode Pointer
#define   B_SPI_SPIC_SPOP         BIT3                        // Sequence Prefix Opcode Pointer
#define   B_SPI_SPIC_ACS          BIT2                        // Atomic Cycle Sequence
#define   B_SPI_SPIC_SCGO         BIT1                        // SPI Cycle Go
#define   B_SPI_SPIC_AR           BIT0                        // SPI Access Request

#define ICH_SPI_SPIA            ICH_RCRB_SPI_BASE + 0x04      // SPI Address
#define ICH_SPI_SPID0           ICH_RCRB_SPI_BASE + 0x08      // SPI Data 0
#define ICH_SPI_SPID1           ICH_RCRB_SPI_BASE + 0x10      // SPI Data 1
#define ICH_SPI_SPID2           ICH_RCRB_SPI_BASE + 0x18      // SPI Data 2
#define ICH_SPI_SPID3           ICH_RCRB_SPI_BASE + 0x20      // SPI Data 3
#define ICH_SPI_SPID4           ICH_RCRB_SPI_BASE + 0x28      // SPI Data 4
#define ICH_SPI_SPID5           ICH_RCRB_SPI_BASE + 0x30      // SPI Data 5
#define ICH_SPI_SPID6           ICH_RCRB_SPI_BASE + 0x38      // SPI Data 6
#define ICH_SPI_SPID7           ICH_RCRB_SPI_BASE + 0x40      // SPI Data 7
#define   V_SPI_SPID_MAX          0x40                        // Maximum number of SPI data allowed
#define ICH_SPI_BBA             ICH_RCRB_SPI_BASE + 0x50      // BIOS Base Address
#define   B_SPI_BBA_UP_ADDR       0xFF000000                  // Upper bits programed in to ICH_SPI_BBA
#define   B_SPI_BBA_ADDR          0x00FFFF00                  // Bottom of System Flash
#define ICH_SPI_PREOP           ICH_RCRB_SPI_BASE + 0x54      // Prefix Opcode Configuration
#define   B_SPI_PREOP_0           ICH_RCRB_SPI_BASE + 0x54    // Prefix Opcode 0
#define   B_SPI_PREOP_1           ICH_RCRB_SPI_BASE + 0x55    // Prefix Opcode 1
#define ICH_SPI_OPTYPE          ICH_RCRB_SPI_BASE + 0x56      // Opcode Type Configuration
#define   V_SPI_OPTYPE_RDNOADDR   0x00                        // Read cycle type without address
#define   V_SPI_OPTYPE_WRNOADDR   0x01                        // Write cycle type without address
#define   V_SPI_OPTYPE_RDADDR     0x02                        // Address required; Read cycle type
#define   V_SPI_OPTYPE_WRADDR     0x03                        // Address required; Write cycle type
#define ICH_SPI_OPMENU          ICH_RCRB_SPI_BASE + 0x58      // Opcode Menu Configuration
#define ICH_SPI_PBR0            ICH_RCRB_SPI_BASE + 0x60      // Protected BIOS Range #0
#define   B_SPI_PBR0_WP           BIT31                       // Write Protection Enable
#define   B_SPI_PBR0_PRL          0x00FFF000                  // Protected Range Limit
#define   B_SPI_PBR0_PRB          0x00000FFF                  // Protected Range Base
#define ICH_SPI_PBR1            ICH_RCRB_SPI_BASE + 0x64      // Protected BIOS Range #1
#define ICH_SPI_PBR2            ICH_RCRB_SPI_BASE + 0x68      // Protected BIOS Range #2

#endif                 
