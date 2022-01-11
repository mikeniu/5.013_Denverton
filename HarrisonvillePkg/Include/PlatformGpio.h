//
// This file contains an 'Intel Peripheral Driver' and is      
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may 
// be modified by the user, subject to additional terms of the 
// license agreement                                           
//
/*++

Copyright (c) 1999 - 2013 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  PlatformGpio.h

--*/

#ifndef _PLATFORM_DEFINITIONS_GPIO_H_
#define _PLATFORM_DEFINITIONS_GPIO_H_


//
// GPIO Register Settings for MOBILE - GRAYSREEF 
//
// Field Descriptions:
//    USE: Defines the pin's usage model:  GPIO (G) or Native (N) mode.
//    I/O: Defines whether GPIOs are inputs (I) or outputs (O).
//         (Note:  Only meaningful for pins used as GPIOs.)
//    LVL: This field gives you the initial value for "output" GPIO's.
//    INV: Defines whether Input GPIOs activation level is inverted.
//         (Note:  Only affects the level sent to the GPE logic and does not
//         affect the level read through the GPIO registers.)
//
// Signal                  UsedAs                       USE     I/O      LVL     INV
// ---------------------------------------------------------------------------------
// NC                      S_GPIO                       G       I        -       -
// TACH1/GPIO1             SMC_EXTSMI#                  G       I        -       I
// PIRQE#/GPIO2            RSVD_TESTMODE                G       I        -       -
// PIRQF#/GPIO3            SATA_ODD_DA#                 G       I        -       I

// PIRQG#/GPIO4            EXTTS_SNI_DRV0_PCH           G       I        -       -
// PIRQH#/GPIO5            EXTTS_SNI_DRV1_PCH           G       I        -       -
// TACH2/GPIO6             DGPU_HPD_INTR#               G       I        -       -
// TACH3/GPIO7             SMC_RUNTIME_SCI#             G       I        -       I

// GPIO8                   PCH_GPIO8 (DDR voltage Bit0) G       0        H       -
// GPIO9                   USB_OC#_10_11                N       -        -       -
// GPIO10                  USB_OC#_12_13                N       -        -       -
// GPIO11                  Free GPIO to Sinai           G       I        -       -
// GPIO12                  PM_LANPHY_ENABLE             N       -        -       -
// GPIO13                  HDA_DOCK_RST#                N       -        -       -
// GPIO14                  FREE GPIO                    G       I        -       -
// GPIO15                  HOST_ALERT#1                 G       I        -       -

// PCH_GPIO16              SATA_DET#4                   N       -        -       -
// PCH_GPIO17              DGPU_PWROK                   G       I        -       -
// GPIO18                  CLK_SLOT1_OE#                N       -        -       -
// SATA1GP/GPIO19          FREE GPIO                    G       I        -       -

// GPIO20                  CLK_SLOT2_OE#                N       -        -       -
// SATA0GP/GPIO21          SATA_DET#0                   N       -        -       -
// SCLOCK/GPIO22           BIOS_REC                     G       I        -       -
// LDRQ1#/GPIO23           PCH_DRQ1#                    N       -        -       -

// GPIO24                  PCH_GPIO24(DDR voltage Bit2) G       O        H       -
// SRC3CLKRQ# /GPIO[25]    CLK_SLOT3_OE#                N       -        -       -
// SRC4CLKRQ#/GPIO[26]     CLK_SLOT4_OE#                N       -        -       -
// GPIO27                  SMC_WAKE_SCI                 G       I        -       -

// GPIO28                  FREE GPIO                    G       O        L       -  may connect to EC
// GPIO29                  PM_SLP_LAN                   N       -        -       -
// GPIO30                  suswarn#/SUS_PWR_NACK        N       -        -       -
// GPIO31                  AC_PRESENT                   G       I        -       -

// GPIO32                  PM_CLKRUN#                   N       -        -       -
// GPIO33                  HDA_DOCK_EN#                 N       -        -       -
// GPIO34                  SATA_PWR_EN#0                G       O        L       -
// GPIO35                  SATA_PWR_EN#1                N       -        -       -

// GPIO36                  SATA_ODD_PRSNT#              N       -        -       -
// GPIO37                  Free GPIO (Also a strap)     G       O        L       -  (NOT USED)
// GPIO38                  MFG_MODE                     G       I        -       -
// GPIO39                  GFX_CRB_DET                  G       I        -       -

// GPIO40                  USB_OC#_2                    N       -        -       -
// GPIO41                  USB_OC#_5                    N       -        -       -
// GPIO42                  USB_OC#_6_7                  N       -        -       -
// GPIO43                  USB_OC#_8_9                  N       -        -       -

// GPIO44                  CLK_SLOT5_OE#                N       -        -       -
// GPIO45                  CLK_PCIE_LAN_REQ#            N       -        -       -
// GPIO46                  PCH_GPIO46DDR Volatage bit1) G       O        H       -
// GPIO47                  PEG_CLKREQ#                  N       -        -       -

// GPIO48                  BIOS_RESP                    G       I        -       -

// GPIO49                  CRIT_TEMP_REP#               G       I        -       -
// GPIO50                  DGPU_HOLD_RST#               G       O        H       -
// GPIO51                  DGPU_EDID_SELECT#            G       O        H       -

// GPIO52                  DGPU_SELECT#                 G       O        H       -
// GPIO53                  DGPU_PWM_SELECT#             G       O        H       -
// GPIO54                  DGPU_PWR_EN#                 G       O        H       -
// GPIO55                  Free GPIO ( used as strap)   G       O        L       -

// GPIO56                  MC_CLKREQ#                   N       -        -       -
// GPIO57                  NFC_IRQ                      G       I        -       -
// GPIO58                  SML1_CLK                     N       -        -       -
// GPIO59                  USB_OC#_0_1                  N       -        -       -

// GPIO60                  DRAMRST_CNTRL_PCH            G       O        H       -
// GPIO61                  PM_SUS_STAT#                 N       -        -       -
// GPIO62                  SUS_CLK                      N       -        -       -
// GPIO63                  PM_SLP_S5#                   N       -        -       -

// GPIO64                  FREE GPIO                    G       O        L       -
// GPIO65                  CLK_REF14_SIO/CLK_REF14_LPCN N       -        -       -
// GPIO66                  PECI_HDR                     G       O        L       -
// GPIO67                  DGPU_PRSNT#                  G       I        -       -

// GPIO68                  SATA_ODD_PWRGT               G       O        H       -
// GPIO69                  SV_DETECT                    G       I        -       -
// GPIO70                  USB3_DET_P2#                 N       -        -       -
// GPIO71                  USB3_DET_P3#                 N       -        -       -

// GPIO72                  PM_BATLOW#                   N       -        -       -
// GPIO73                  CLK_REQ#_DOCK                N       -        -       -
// GPIO74                  IMVP_THM_R#/AMB_THM2_R#      N       -        -       -
// GPIO75                  SML1_DATA                    N       -        -       -

#define   MB_RVP_GPIO_USE_SEL_VAL            BIT31 | BIT28 | BIT27 | BIT24 | BIT22 | BIT19 | BIT17 | BIT15 | BIT14 | \
                                                 BIT11 | BIT8 | BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0
#define   MB_RVP_GPIO_USE_SEL2_VAL           BIT28 | BIT25 | BIT23 | BIT22 | BIT21 | BIT20 | BIT19 | BIT18 | BIT17 | \
                                                 BIT16 | BIT14 | BIT7 | BIT6 | BIT5 | BIT2
#define   MB_RVP_GPIO_USE_SEL3_VAL           BIT5 | BIT4 | BIT3 | BIT2 | BIT0

#define   MB_RVP_GPIO_IO_SEL_VAL             BIT31 | BIT27 | BIT22 | BIT19 | BIT17 | BIT15 | BIT14 | BIT11 | \
                                                 BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0
#define   MB_RVP_GPIO_IO_SEL2_VAL            BIT25 | BIT17 | BIT16 | BIT7 | BIT6
#define   MB_RVP_GPIO_IO_SEL3_VAL            BIT5 | BIT3

#define   MB_RVP_GPIO_LVL_VAL                BIT24 | BIT8
#define   MB_RVP_GPIO_LVL2_VAL               BIT28 | BIT22 | BIT21 | BIT20 | BIT19 | BIT18 | BIT14
//MB_RVP_GPIO_LVL2_VAL
//BIT18 = {dGPU HLD RST GPIO}    0 - RELEASE THE DGPU RESET, 1 - HOLD THE DGPU RESET {GPIO 50:SgDgpuHoldRst}
//BIT22 = {dGPU PWR Enable GPIO} 0 - POWER OFF THE DGPU, 1 - POWER ON THE DGPU {GPIO 54:SgDgpuPwrEnable}
#define   MB_RVP_GPIO_LVL3_VAL               BIT4

#define   MB_RVP_GPIO_INV_VAL                BIT7 | BIT3 | BIT1
#define   MB_RVP_GPIO_BLINK_VAL              0x00000000
#define   MB_RVP_GPIO_RST_SEL1               BIT30 | BIT24 | BIT8 // Bit30 = 1 is required by Section 4.5 of PCH BIOS Spec V0.5
#define   MB_RVP_GPIO_RST_SEL2               BIT14


//
// GPIO Register Settings for MOBILE - Basking Ridge 
//
// Field Descriptions:
//    USE: Defines the pin's usage model:  GPIO (G) or Native (N) mode.
//    I/O: Defines whether GPIOs are inputs (I) or outputs (O).
//         (Note:  Only meaningful for pins used as GPIOs.)
//    LVL: This field gives you the initial value for "output" GPIO's.
//    INV: Defines whether Input GPIOs activation level is inverted.
//         (Note:  Only affects the level sent to the GPE logic and does not
//         affect the level read through the GPIO registers.)
//
// Signal                  UsedAs                       USE     I/O      LVL     INV
// ---------------------------------------------------------------------------------
// NC                      S_GPIO                       G       I        -       -
// TACH1/GPIO1             SMC_EXTSMI#                  G       I        -       I
// PIRQE#/GPIO2            RSVD_TESTMODE                G       I        -       -
// PIRQF#/GPIO3            PCH_PCI_IRQ_N                G       I        -       I

// PIRQG#/GPIO4            EXTTS_SNI_DRV0_PCH           G       I        -       -
// PIRQH#/GPIO5            EXTTS_SNI_DRV1_PCH           G       I        -       -
// TACH2/GPIO6             DGPU_HPD_INTR#               G       O        L       -
// TACH3/GPIO7             SMC_RUNTIME_SCI#             G       I        -       I

// GPIO8                   PCH_GPIO8 (DDR voltage Bit0) G       0        H       -
// GPIO9                   USB_OC#_10_11                N       -        -       -
// GPIO10                  USB_OC#_12_13                N       -        -       -
// GPIO11                  SATA_ODD_DA_N                G       I        -       -
// GPIO12                  PM_LANPHY_ENABLE             N       -        -       -
// GPIO13                  HDA_DOCK_RST#                N       -        -       -
// GPIO14                  USB_8_9_PWR                  G       O        H       -
// GPIO15                  HOST_ALERT#1                 G       O        L       -

// PCH_GPIO16              SATA_DET#4                   N       -        -       -
// PCH_GPIO17              DGPU_PWROK                   G       I        -       -
// GPIO18                  CLK_SLOT1_OE#                N       -        -       -
// SATA1GP/GPIO19          BBS_BIT0_R                   G       I        -       -

// GPIO20                  CLK_SLOT2_OE#                N       -        -       -
// SATA0GP/GPIO21          SATA_DET#0                   N       -        -       -
// SCLOCK/GPIO22           BIOS_REC                     G       I        -       -
// LDRQ1#/GPIO23           PCH_DRQ1#                    N       -        -       -

// GPIO24                  PCH_GPIO24(DDR voltage Bit2) G       O        H       -
// SRC3CLKRQ# /GPIO[25]    CLK_SLOT3_OE#                N       -        -       -
// SRC4CLKRQ#/GPIO[26]     CLK_SLOT4_OE#                N       -        -       -
// GPIO27                  SMC_WAKE_SCI                 G       I        -       -

// GPIO28                  PLL_ODVR_EN                  G       O        L       -  may connect to EC
// GPIO29                  PM_SLP_LAN                   N       -        -       -
// GPIO30                  suswarn#/SUS_PWR_NACK        N       -        -       -
// GPIO31                  AC_PRESENT                   G       I        -       -

// GPIO32                  PM_CLKRUN#                   N       -        -       -
// GPIO33                  HDA_DOCK_EN#                 N       -        -       -
// GPIO34                  SATA_PWR_EN#0                G       O        L       -
// GPIO35                  SATA_PWR_EN#1                N       -        -       -

// GPIO36                  SATA_ODD_PRSNT#              N       -        -       -
// GPIO37                  SATA_ODD_N                   G       I        -       -
// GPIO38                  MFG_MODE                     G       I        -       -
// GPIO39                  GFX_CRB_DET                  G       I        -       -

// GPIO40                  USB_OC#_2                    N       -        -       -
// GPIO41                  USB_OC_5_R_N                 G       O        H       -
// GPIO42                  USB_OC#_6_7                  N       -        -       -
// GPIO43                  USB_OC#_8_9                  N       -        -       -

// GPIO44                  CLK_SLOT5_OE#                N       -        -       -
// GPIO45                  CLK_PCIE_LAN_REQ#            N       -        -       -
// GPIO46                  PCH_GPIO46DDR Volatage bit1) G       O        H       -
// GPIO47                  PEG_CLKREQ#                  N       -        -       -

// GPIO48                  BIOS_RESP                    G       I        -       -

// GPIO49                  CRIT_TEMP_REP#               G       I        -       -
// GPIO50                  DGPU_HOLD_RST#               G       O        H       -
// GPIO51                  DGPU_EDID_SELECT#            G       O        H       -

// GPIO52                  DGPU_SELECT#                 G       O        H       -
// GPIO53                  DGPU_PWM_SELECT#             G       O        H       -
// GPIO54                  DGPU_PWR_EN#                 G       O        H       -
// GPIO55                  STP_A16OVR                   G       O        H       -

// GPIO56                  MC_CLKREQ#                   N       -        -       -
// GPIO57                  NFC_IRQ                      G       I        -       -
// GPIO58                  SML1_CLK                     N       -        -       -
// GPIO59                  USB_OC#_0_1                  N       -        -       -

// GPIO60                  DRAMRST_CNTRL_PCH            G       O        H       -
// GPIO61                  PM_SUS_STAT#                 N       -        -       -
// GPIO62                  SUS_CLK                      N       -        -       -
// GPIO63                  PM_SLP_S5#                   N       -        -       -

// GPIO64                  CK_PCH_PCI_SIO_DOCK_R        N       -        -       -
// GPIO65                  CLK_REF14_SIO/CLK_REF14_LPCN N       -        -       -
// GPIO66                  PECI_HDR                     G       O        L       -
// GPIO67                  DGPU_PRSNT#                  G       I        -       -

// GPIO68                  SATA_ODD_PWRGT               G       O        H       -
// GPIO69                  SV_DETECT                    G       I        -       -
// GPIO70                  USB3_DET_P2#                 N       -        -       -
// GPIO71                  USB3_DET_P3#                 N       -        -       -

// GPIO72                  PM_BATLOW#                   N       -        -       -
// GPIO73                  CLK_REQ#_DOCK                N       -        -       -

#define   MB_RVP_BR_GPIO_USE_SEL_VAL            BIT28 | BIT27 | BIT24 | BIT22 | BIT19 | BIT17 | BIT15 | BIT14 | \
                                                 BIT11 | BIT8 | BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0
#define   MB_RVP_BR_GPIO_USE_SEL2_VAL           BIT28 | BIT25 | BIT23 | BIT22 | BIT21 | BIT20 | BIT19 | BIT18 | BIT17 | \
                                                 BIT16 | BIT14 | BIT9 | BIT7 | BIT6 | BIT5 | BIT2
#define   MB_RVP_BR_GPIO_USE_SEL3_VAL           BIT5 | BIT4 | BIT3 | BIT2

#define   MB_RVP_BR_GPIO_IO_SEL_VAL             BIT27 | BIT22 | BIT19 | BIT17 | BIT11 | \
                                                 BIT7 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0
#define   MB_RVP_BR_GPIO_IO_SEL2_VAL            BIT25 | BIT17 | BIT16 | BIT7 | BIT6 | BIT5
#define   MB_RVP_BR_GPIO_IO_SEL3_VAL            BIT5 | BIT3

#define   MB_RVP_BR_GPIO_LVL_VAL                BIT24 | BIT14 | BIT8
#define   MB_RVP_BR_GPIO_LVL2_VAL               BIT28 | BIT23 | BIT22 | BIT21 | BIT20 | BIT19 | BIT18 | BIT14 | BIT9
#define   MB_RVP_BR_GPIO_LVL3_VAL               BIT4

#define   MB_RVP_BR_GPIO_INV_VAL                BIT19 | BIT11 | BIT7 | BIT3 | BIT1
#define   MB_RVP_BR_GPIO_BLINK_VAL              0x00000000
#define   MB_RVP_BR_GPIO_RST_SEL1               BIT30 | BIT24 | BIT8
#define   MB_RVP_BR_GPIO_RST_SEL2               BIT14


// GPIO Register Settings for DESKTOP - FLATHEAD CREEK 
//
// Field Descriptions:
//    USE: Defines the pin's usage model:  GPIO (G) or Native (N) mode.
//    I/O: Defines whether GPIOs are inputs (I) or outputs (O).
//         (Note:  Only meaningful for pins used as GPIOs.)
//    LVL: This field gives you the initial value for "output" GPIO's.
//    INV: Defines whether Input GPIOs activation level is inverted.
//         (Note:  Only affects the level sent to the GPE logic and does not
//         affect the level read through the GPIO registers.)
//
// Signal                  UsedAs                       USE     I/O      LVL     INV
// ---------------------------------------------------------------------------------
// BMBUSY#/GPIO0           FP_AUD_DETECT                G       I        -       -
// TACH1/GPIO1             PCH_GP1                      G       I        -       I
// PIRQE#/GPIO2            TEST_SETUP_MENU              G       I        -       I
// PIRQF#/GPIO3            P_INTF_N                     N       -        -       -

// PIRQG#/GPIO4            EXTTS_SNI_DRV0_PCH           G       I        -       -
// PIRQH#/GPIO5            EXTTS_SNI_DRV1_PCH           G       I        -       -
// TACH2/GPIO6             PCH_GP6 brd ID bit0          G       I        -       -
// TACH3/GPIO7             SMC_RUNTIME_SCI_N            G       I        -       I

// HOST_ALERT2/GPIO8       IGC_EN_N                     G       I        -       I
// USB_OC#5/GPIO9          USB_OC5_R_N                  N       -        -       -
// USB_OC#6/GPIO10         USB_OC6_R_N                  N       -        -       -
// SMB_ALERT#/GPIO11       PCH_PORT80_LED               G       O        L       -

// GPIO12                  LAN_DISABLE_N                N       -        -       -
// SMB_ALERT#/GPIO13       GPIO_PCIE_RESET              G       O        L       -
// USB_OC7#/GPIO14         IO_PME_N                     G       I        -       I
// HOST_ALERT#1/GPIO15     LED_DRIVE_GP15               G       O        L       -

// PCH_GPIO16              SATA4GP                      G       I        -       -
// PCH_GPIO17              PCH_GP17                     G       I        -       I
// GPIO18                  PCH_GP18                     G       I        -       -
// SATA1GP/GPIO19          PCH_SATA1GP                  G       I        -       -

// GPIO20                  PCH_SMI_N                    N       -        -       -
// SATA0GP/GPIO21          PCH_SATA0GP                  N       I/N      -       -
// SCLOCK/GPIO22           PCH_CONFIG_JUMPER            G       I        -       -
// LDRQ1#/GPIO23           LPC_L_DRQ1_N                 N       -        -       -

// MEM_LED/GPIO24          H_SKTOCC_N                   G       I        -       I
// STP_CPU#                GP25_PD                      G       I        L       -
// S4_STATE#/GPIO26        GP26_PD                      G       I        L       -
// GPIO27                  LANWAKE_N                    G       I        -       -

// GPIO28                  PCH_GP28                     G       I        -       -
// GPIO29                  SLP_LAN_N                    N       O/N      L       -
// GPIO30                  SUSWARNB                     N       -        -       -
// GPIO31                  PCH_GP31                     G       I        -       -

// CLKRUN#/GPIO32          PCH_GP32                     G       I        -       -
// HDA_DOCK_EN#/GPIO33     PCH_GP33                     G       I        -       -
// GPIO34                  PCH_GP34 brd ID bit2         G       I        -       -
// GPIO35                  2X4_POWER_DETECT             G       I        -       -

// GPIO36                  PCH_GP36                     G       I        -       -
// GPIO37                  PCH_GP37                     G       I        -       -
// GPIO38                  MFG_MODE                     G       I        -       -
// GPIO39                  GP39_Gfx_CRB_DET             G       I        -       -

// OC1#/GPIO40             USB_OC1_R_N                  N       -        -       -
// OC2#/GPIO41             USB_OC2_R_N                  N       -        -       -
// OC3#/GPIO42             USB_OC3_R_N                  N       -        -       -
// OC4#/GPIO43             USB_OC4_R_N                  N       -        -       -

// PCIECLKRQ5#/GPIO44      PCH_GP44                     G       I        -       -
// PCIECLKRQ6#/GPIO45      PCH_GP45                     G       O        H       -
// PCIECLKRQ7#/GPIO46      PCH_GP46                     G       O        H       -
// PEF_A_CLKRQ#/GPIO47     NC                           N       -        -       -

// SDATAOUT1/GPIO48        SV_ADVANCE_GP48              G       I        -       -
// GPIO49                  PCH_GP49                     G       I        -       -
// REQ1#/GPIO50            PCIEX1_SLOT5_PRSNT2_N        G       I        -       -
// GNT1#/GPIO51            PCH_GP51                     G       I        -       -

// REQ2#/GPIO52            PCIEX1_SLOT4_PRSNT2_N        G       I        -       -
// GNT2#/GPIO53            PCH_GP53                     G       I        -       -
// REQ3#/GPIO54            PCIEX1_SLOT6_PRSNT2_N        G       I        -       -
// GNT3#/GPIO55            PCH_GP55                     G       I        -       -

// PEG_B_CLKRQ#/GPIO56     NC                           N       -        -       -
// GPIO57                  NFC_GP57                     G       I        -       -
// GPIO58                  SML1CLK_PCH                  N       -        -       -
// OC0#/GPIO59             USB_OC0_R_N                  N       -        -       -

// GPIO60                  PCH_GP60                     G       O        L       -
// GPIO61                  LPCPD_n                      N       -        -       -
// GPIO62                  TPEV_SUSCLK                  N       -        -       -
// GPIO63                  TP_SLP_S5_N                  N       -        -       -

// GPIO64                  CK_TPEV_FLEX0_PCH            N       -        -       -
// GPIO65                  CK_TPEV_14M_PA               N       -        -       -
// GPIO66                  CK_TPEV_FLEX2_PCH            N       -        -       -
// GPIO67                  CK_TPEV_FLEX3                N       -        -       -

// GPIO68                  PCIEX16_PRSNT2_N             G       I        -       -
// GPIO69                  SV_DETECT                    G       I        -       -
// GPIO70                  BOARD_DIFF                   G       I        -       -
// GPIO71                  PCH_GP71_PU                  G       I        -       -
 
// GPIO72                  PWRGD_CTRL_DISABLE           G       O        H       -
// GPIO73                  GP73_PD (Not applicable)     N       -        -       -
// GPIO74                  PCHHOT/PCH_GP74_PU           G       I        -       -
// GPIO75                  SML1_DATA_PCH                N       -        -       -

#define   DT_RVP_GPIO_USE_SEL_VAL            BIT31 | BIT29 | BIT28 | BIT27 | BIT26 | BIT25 | BIT24 | BIT22 | BIT21 | \
                                                 BIT19 | BIT18 | BIT17 | BIT16 | BIT15 | BIT14 | BIT13 | BIT11 | BIT8 | BIT7 | \
                                                 BIT6 | BIT5 | BIT4 | BIT2 | BIT1 | BIT0
#define   DT_RVP_GPIO_USE_SEL2_VAL           BIT28 | BIT25 | BIT23 | BIT22 | BIT21 | BIT20 | BIT19 | BIT18 | BIT17 | BIT16 | BIT15 | \
                                                 BIT14 | BIT13 | BIT12 | BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0
#define   DT_RVP_GPIO_USE_SEL3_VAL           BIT10 | BIT8 | BIT7 | BIT6 | BIT5 | BIT4

#define   DT_RVP_GPIO_IO_SEL_VAL             BIT31 | BIT28 | BIT27 | BIT26 | BIT25 | BIT24 | BIT22 | BIT21 | \
                                                 BIT19 | BIT18 | BIT17 | BIT16 | BIT14 | BIT8 | BIT7 | \
                                                 BIT6 | BIT5 | BIT4 | BIT2 | BIT1 | BIT0
#define   DT_RVP_GPIO_IO_SEL2_VAL            BIT25 | BIT23 | BIT22 | BIT21 | BIT20 | BIT19 | BIT18 | BIT17 | BIT16 | \
                                                 BIT12 | BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0
#define   DT_RVP_GPIO_IO_SEL3_VAL            BIT10 | BIT7 | BIT6 | BIT5 | BIT4

#define   DT_RVP_GPIO_LVL_VAL                BIT29
#define   DT_RVP_GPIO_LVL2_VAL               BIT14 | BIT13
#define   DT_RVP_GPIO_LVL3_VAL               BIT8

#define   DT_RVP_GPIO_INV_VAL                BIT24 | BIT17 | BIT14 | BIT8 | BIT7 |BIT2 | BIT1
#define   DT_RVP_GPIO_BLINK_VAL              0x00000000
#define   DT_RVP_GPIO_RST_SEL1               0x00000000
#define   DT_RVP_GPIO_RST_SEL2               0x00000000


// GPIO Register Settings for DESKTOP - SHB DT Refresh
//
// Field Descriptions:
//    USE: Defines the pin's usage model:  GPIO (G) or Native (N) mode.
//    I/O: Defines whether GPIOs are inputs (I) or outputs (O).
//         (Note:  Only meaningful for pins used as GPIOs.)
//    LVL: This field gives you the initial value for "output" GPIO's.
//    INV: Defines whether Input GPIOs activation level is inverted.
//         (Note:  Only affects the level sent to the GPE logic and does not
//         affect the level read through the GPIO registers.)
//
// Signal                  UsedAs                       USE     I/O      LVL     INV
// ---------------------------------------------------------------------------------
// BMBUSY#/GPIO0           FP_AUD_DETECT                G       I        -       -
// TACH1/GPIO1             PCH_GP1                      G       I        -       I
// PIRQE#/GPIO2            TEST_SETUP_MENU              G       I        -       I
// PIRQF#/GPIO3            P_INTF_N                     N       -        -       -

// PIRQG#/GPIO4            EXTTS_SNI_DRV0_PCH           G       I        -       -
// PIRQH#/GPIO5            GP5_TBCIO_PLUG_EVENT         G       I        -       -
// TACH2/GPIO6             PCH_GP6 brd ID bit0          G       I        -       -
// TACH3/GPIO7             SMC_RUNTIME_SCI_N            G       I        -       I

// HOST_ALERT2/GPIO8       GP8_NGFF_WLAN_RST_N          G       O        L       -
// USB_OC#5/GPIO9          USB_OC5_R_N                  N       -        -       -
// USB_OC#6/GPIO10         USB_OC6_R_N                  N       -        -       -
// SMB_ALERT#/GPIO11       GP11_NGFF_WIGIG_RST_N        G       O        L       -

// GPIO12                  LAN_DISABLE_N                N       -        -       -
// SMB_ALERT#/GPIO13       GPIO_PCIE_RESET              G       O        L       -
// USB_OC7#/GPIO14         IO_PME_N                     G       I        -       I
// HOST_ALERT#1/GPIO15     GP74_BT_RF_KILL_N            G       O        H       -

// PCH_GPIO16              SATAE_PCIEP1_IFDET           G       I        -       -
// PCH_GPIO17              PCH_GP17                     G       I        -       I
// GPIO18                  PCH_GP18                     G       I        -       -
// SATA1GP/GPIO19          PCH_SATA1GP                  G       I        -       -

// GPIO20                  PCH_SMI_N                    N       -        -       -
// SATA0GP/GPIO21          PCH_SATA0GP                  G       I/N      -       -
// SCLOCK/GPIO22           PCH_CONFIG_JUMPER            G       I        -       -
// LDRQ1#/GPIO23           LPC_L_DRQ1_N                 N       -        -       -

// MEM_LED/GPIO24          H_SKTOCC_N                   G       I        -       I
// STP_CPU#                GP25_WIGIG_CLKREQ/IVCAM_DFU  G       I        L       -
// S4_STATE#/GPIO26        NGFF_WLAN_CLKREQ_R/NFC_FW_DL G       I        L       -                                
// GPIO27                  LANWAKE_N                    G       I        -       -

// GPIO28                  PCH_NFC_RESET                G       O        H       -
// GPIO29                  SLP_LAN_N                    G       O/N      H       -
// GPIO30                  SUSWARNB                     N       -        -       -
// GPIO31                  GP31_WIGIG_RST_N             G       O        L       -

// CLKRUN#/GPIO32          PCH_GP32                     G       I        -       -
// HDA_DOCK_EN#/GPIO33     GP33_TB_FRC_PWR              G       O        L       -
// GPIO34                  PCH_GP34 brd ID bit2         G       I        -       -
// GPIO35                  2X4_POWER_DETECT             G       I        -       -

// GPIO36                  PCH_GP36                     G       I        -       -
// GPIO37                  PCH_GP37                     G       I        -       -
// GPIO38                  MFG_MODE                     G       I        -       -
// GPIO39                  GP39_Gfx_CRB_DET             G       I        -       -

// OC1#/GPIO40             USB_OC1_R_N                  N       -        -       -
// OC2#/GPIO41             USB_OC2_R_N                  N       -        -       -
// OC3#/GPIO42             USB_OC3_R_N                  N       -        -       -
// OC4#/GPIO43             USB_OC4_R_N                  N       -        -       -

// PCIECLKRQ5#/GPIO44      GP44_WLAN_DISABLE_N          G       O        H       -
// PCIECLKRQ6#/GPIO45      PCH_GP45                     G       O        H       -
// PCIECLKRQ7#/GPIO46      PCH_GP46                     G       O        H       -
// PEF_A_CLKRQ#/GPIO47     NC                           G       O        L       -

// SDATAOUT1/GPIO48        SV_ADVANCE_GP48              G       I        -       -
// GPIO49                  SATAE_PCIEP2_IFDET?         G       I        -       -
// REQ1#/GPIO50            PCIEX1_SLOT5_PRSNT2_N        G       I        -       -
// GNT1#/GPIO51            PCH_GP51                     G       I        -       -

// REQ2#/GPIO52            PCIEX1_SLOT4_PRSNT2_N        G       I        -       -
// GNT2#/GPIO53            PCH_GP53                     G       I        -       -
// REQ3#/GPIO54            PCIEX1_SLOT6_PRSNT2_N        G       I        -       -
// GNT3#/GPIO55            PCH_GP55                     G       I        -       -

// PEG_B_CLKRQ#/GPIO56     NC                           N       -        -       -
// GPIO57                  NFC_GP57                     G       I        -       -
// GPIO58                  SML1CLK_PCH                  N       -        -       -
// OC0#/GPIO59             USB_OC0_R_N                  N       -        -       -

// GPIO60                  PCH_GP60                     G       O        L       -
// GPIO61                  LPCPD_n                      N       -        -       -
// GPIO62                  TPEV_SUSCLK                  N       -        -       -
// GPIO63                  TP_SLP_S5_N                  N       -        -       -

// GPIO64                  PCH_GP64_IVCAM_DFU           G       O        L       -
// GPIO65                  CK_TPEV_14M_PA               N       -        -       -
// GPIO66                  GP66_NFC_FW_DL               G       O        L       -
// GPIO67                  CK_TPEV_FLEX3                N       -        -       -

// GPIO68                  PCIEX16_PRSNT2_N             G       I        -       -
// GPIO69                  SV_DETECT                    G       I        -       -
// GPIO70                  BOARD_DIFF                   G       I        -       -
// GPIO71                  PCH_GP71_PU                  G       I        -       -
 
// GPIO72                  PWRGD_CTRL_DISABLE           G       O        H       -
// GPIO73                  GP73_PCIEP1_CLKREQ           G       I        -       -
// GPIO74                  ANC_BOOT_GUARD               G       O        L       _
// GPIO75                  SML1_DATA_PCH                N       -        -       -

#define   DT_SHB_REF_GPIO_USE_SEL_VAL            BIT31 | BIT29 | BIT28 | BIT27 | BIT26 | BIT25 | BIT24 | BIT22 | BIT21 | \
                                                 BIT19 | BIT18 | BIT17 | BIT16 | BIT15 | BIT14 | BIT13 | BIT11 | BIT8 | BIT7 | \
                                                 BIT6 | BIT5 | BIT4 | BIT2 | BIT1 | BIT0
#define   DT_SHB_REF_GPIO_USE_SEL2_VAL           BIT28 | BIT25 | BIT23 | BIT22 | BIT21 | BIT20 | BIT19 | BIT18 | BIT17 | BIT16 | BIT15 | \
                                                 BIT14 | BIT13 | BIT12 | BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0
#define   DT_SHB_REF_GPIO_USE_SEL3_VAL           BIT10 | BIT9 | BIT8 | BIT7 | BIT6 | BIT5 | BIT4 | BIT2 | BIT0

#define   DT_SHB_REF_GPIO_IO_SEL_VAL             BIT27 | BIT26 | BIT25 | BIT24 | BIT22 | BIT21 | \
                                                 BIT19 | BIT18 | BIT17 | BIT16 | BIT14 | BIT7 | \
                                                 BIT6 | BIT5 | BIT4 | BIT2 | BIT1 | BIT0
#define   DT_SHB_REF_GPIO_IO_SEL2_VAL            BIT25 | BIT23 | BIT22 | BIT21 | BIT20 | BIT19 | BIT18 | BIT17 | BIT16 | \
                                                 BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT0
#define   DT_SHB_REF_GPIO_IO_SEL3_VAL            BIT9 | BIT7 | BIT6 | BIT5 | BIT4
	
#define   DT_SHB_REF_GPIO_LVL_VAL                BIT29 | BIT28 | BIT15
#define   DT_SHB_REF_GPIO_LVL2_VAL               BIT14 | BIT13 | BIT12
#define   DT_SHB_REF_GPIO_LVL3_VAL               BIT8

#define   DT_SHB_REF_GPIO_INV_VAL                BIT24 | BIT17 | BIT14 | BIT7 |BIT2 | BIT1
#define   DT_SHB_REF_GPIO_BLINK_VAL              0x00000000
#define   DT_SHB_REF_GPIO_RST_SEL1               0x00000000
#define   DT_SHB_REF_GPIO_RST_SEL2               0x00000000


//
//
// GPIO Register Settings for DESKTOP - OC CRB with LPT
//
// Field Descriptions:
//    USE: Defines the pin's usage model:  GPIO (G) or Native (N) mode.
//    I/O: Defines whether GPIOs are inputs (I) or outputs (O).
//         (Note:  Only meaningful for pins used as GPIOs.)
//    LVL: This field gives you the initial value for "output" GPIO's.
//    INV: Defines whether Input GPIOs activation level is inverted.
//         (Note:  Only affects the level sent to the GPE logic and does not
//         affect the level read through the GPIO registers.)
//
// Signal                  UsedAs                       USE     I/O      LVL     INV
// ---------------------------------------------------------------------------------
// BMBUSY#/GPIO0           FP_AUD_DETECT                G       I        -       -
// TACH1/GPIO1             PCH_GP1                      G       I        -       I
// PIRQE#/GPIO2            TEST_SETUP_MENU              G       I        -       I
// PIRQF#/GPIO3            P_INTF_N                     N       -        -       -

// PIRQG#/GPIO4            EXTTS_SNI_DRV0_PCH           G       I        -       -
// PIRQH#/GPIO5            EXTTS_SNI_DRV1_PCH           G       I        -       -
// TACH2/GPIO6             PCH_GP6 brd ID bit0          G       I        -       -
// TACH3/GPIO7             SMC_RUNTIME_SCI_N            G       I        -       I

// HOST_ALERT2/GPIO8       IGC_EN_N                     G       I        -       I
// USB_OC#5/GPIO9          USB_OC5_R_N                  N       -        -       -
// USB_OC#6/GPIO10         USB_OC6_R_N                  N       -        -       -
// SMB_ALERT#/GPIO11       PCH_PORT80_LED               G       O        L       -

// GPIO12                  LAN_DISABLE_N                N       -        -       -
// SMB_ALERT#/GPIO13       GPIO_PCIE_RESET              G       O        L       -
// USB_OC7#/GPIO14         IO_PME_N                     G       I        -       I
// HOST_ALERT#1/GPIO15     LED_DRIVE_GP15               G       O        L       -

// PCH_GPIO16              SATA4GP                      G       I        -       -
// PCH_GPIO17              PCH_GP17                     G       I        -       I
// GPIO18                  PCH_GP18                     G       I        -       -
// SATA1GP/GPIO19          PCH_SATA1GP                  G       I        -       -

// GPIO20                  PCH_SMI_N                    N       -        -       -
// SATA0GP/GPIO21          PCH_SATA0GP                  N       I/N      -       -
// SCLOCK/GPIO22           PCH_CONFIG_JUMPER            G       I        -       -
// LDRQ1#/GPIO23           LPC_L_DRQ1_N                 N       -        -       -

// MEM_LED/GPIO24          H_SKTOCC_N                   G       I        -       I
// STP_CPU#                GP25_SAFE_BOOT_MODE          G       I        -       -
// S4_STATE#/GPIO26        GP26_PD                      G       I        L       -
// GPIO27                  LANWAKE_N                    G       I        -       -

// GPIO28                  PCH_GP28                     G       I        -       -
// GPIO29                  SLP_LAN_N                    N       O/N      L       -
// GPIO30                  SUSWARNB                     N       -        -       -
// GPIO31                  PCH_GP31                     G       I        -       -

// GPIO32                  PCH_GP32                     G       O        L       -
// HDA_DOCK_EN#/GPIO33     PCH_GP33                     G       I        -       -
// GPIO34                  PCH_GP34 brd ID bit2         G       I        -       -
// GPIO35                  2X4_POWER_DETECT             G       I        -       -

// GPIO36                  PCH_GP36                     G       I        -       -
// GPIO37                  PCH_GP37                     G       I        -       -
// GPIO38                  MFG_MODE                     G       I        -       -
// GPIO39                  GP39_Gfx_CRB_DET             G       I        -       -

// OC1#/GPIO40             USB_OC1_R_N                  N       -        -       -
// OC2#/GPIO41             USB_OC2_R_N                  N       -        -       -
// OC3#/GPIO42             USB_OC3_R_N                  N       -        -       -
// OC4#/GPIO43             USB_OC4_R_N                  N       -        -       -

// PCIECLKRQ5#/GPIO44      PCH_GP44                     G       I        -       -
// PCIECLKRQ6#/GPIO45      PCH_GP45                     G       O        L       -
// PCIECLKRQ7#/GPIO46      PCH_GP46                     G       O        L       -
// PEF_A_CLKRQ#/GPIO47     NC                           N       -        -       -

// SDATAOUT1/GPIO48        SV_ADVANCE_GP48              G       I        -       -
// GPIO49                  PCH_GP49                     G       I        -       -
// REQ1#/GPIO50            PCIEX1_SLOT5_PRSNT2_N        G       I        -       -
// GNT1#/GPIO51            PCH_GP51                     G       I        -       -

// REQ2#/GPIO52            PCIEX1_SLOT4_PRSNT2_N        G       I        -       -
// GNT2#/GPIO53            PCH_GP53                     G       I        -       -
// REQ3#/GPIO54            PCIEX1_SLOT6_PRSNT2_N        G       I        -       -
// GNT3#/GPIO55            PCH_GP55                     G       I        -       -

// PEG_B_CLKRQ#/GPIO56     NC                           N       -        -       -
// GPIO57                  NFC_GP57                     G       I        -       -
// GPIO58                  SML1CLK_PCH                  N       -        -       -
// OC0#/GPIO59             USB_OC0_R_N                  N       -        -       -

// GPIO60                  PCH_GP60                     G       O        H       -
// GPIO61                  LPCPD_n                      N       -        -       -
// GPIO62                  TPEV_SUSCLK                  N       -        -       -
// GPIO63                  TP_SLP_S5_N                  N       -        -       -

// GPIO64                  CK_TPEV_FLEX0_PCH            N       -        -       -
// GPIO65                  CK_TPEV_14M_PA               N       -        -       -
// GPIO66                  CK_TPEV_FLEX2_PCH            N       -        -       -
// GPIO67                  CK_TPEV_FLEX3                N       -        -       -

// GPIO68                  PCIEX16_PRSNT2_N             G       I        -       -
// GPIO69                  SV_DETECT                    G       I        -       -
// GPIO70                  BOARD_DIFF                   G       I        -       -
// GPIO71                  PCH_GP71_PU                  G       I        -       -
 
// GPIO72                  PWRGD_CTRL_DISABLE           G       O        H       -
// GPIO73                  GP73_PD (Not applicable)     N       -        -       -
// GPIO74                  PCHHOT/PCH_GP74_PU           G       I        -       -
// GPIO75                  SML1_DATA_PCH                N       -        -       -

#define   DT_RVP_OC_GPIO_USE_SEL_VAL            BIT31 | BIT29 | BIT28 | BIT27 | BIT26 | BIT25 | BIT24 | BIT22 | BIT21 | \
                                                 BIT19 | BIT18 | BIT17 | BIT16 | BIT15 | BIT14 | BIT13 | BIT11 | BIT8 | BIT7 | \
                                                 BIT6 | BIT5 | BIT4 | BIT2 | BIT1 | BIT0
#define   DT_RVP_OC_GPIO_USE_SEL2_VAL           BIT28 | BIT25 | BIT23 | BIT22 | BIT21 | BIT20 | BIT19 | BIT18 | BIT17 | BIT16 | BIT15 | \
                                                 BIT14 | BIT13 | BIT12 | BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0
#define   DT_RVP_OC_GPIO_USE_SEL3_VAL           BIT10 | BIT8 | BIT7 | BIT6 | BIT5 | BIT4

#define   DT_RVP_OC_GPIO_IO_SEL_VAL             BIT31 | BIT28 | BIT27 | BIT26 | BIT25 | BIT24 | BIT22 | BIT21 | \
                                                 BIT19 | BIT18 | BIT17 | BIT16 | BIT14 | BIT8 | BIT7 | \
                                                 BIT6 | BIT5 | BIT4 | BIT2 | BIT1 | BIT0
#define   DT_RVP_OC_GPIO_IO_SEL2_VAL            BIT25 | BIT23 | BIT22 | BIT21 | BIT20 | BIT19 | BIT18 | BIT17 | BIT16 | \
                                                 BIT12 | BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 
#define   DT_RVP_OC_GPIO_IO_SEL3_VAL            BIT10 | BIT7 | BIT6 | BIT5 | BIT4

#define   DT_RVP_OC_GPIO_LVL_VAL                BIT29
#define   DT_RVP_OC_GPIO_LVL2_VAL               BIT28
#define   DT_RVP_OC_GPIO_LVL3_VAL               BIT8

#define   DT_RVP_OC_GPIO_INV_VAL                BIT24 | BIT17 | BIT14 | BIT8 | BIT7 |BIT2 | BIT1
#define   DT_RVP_OC_GPIO_BLINK_VAL              0x00000000
#define   DT_RVP_OC_GPIO_RST_SEL1               0x00000000
#define   DT_RVP_OC_GPIO_RST_SEL2               0x00000000


//
// Tumalo Falls GPIO settings
// GPIO Register Settings for Tumalo Falls (Desktop / Mobile SV south bridge board)
//
// Field Descriptions:
//    USE: Defines the pin's usage model:  GPIO (G) or Native (N) mode.
//    I/O: Defines whether GPIOs are inputs (I) or outputs (O).
//         (Note:  Only meaningful for pins used as GPIOs.)
//    LVL: This field gives you the initial value for "output" GPIO's.
//         (Note: The output level is dependent upon whether the pin is inverted.)
//    INV: Defines whether Input GPIOs activation level is inverted.
//         (Note:  Only affects the level sent to the GPE logic and does not
//         affect the level read through the GPIO registers.)
//
// Signal                  UsedAs                       USE     I/O      LVL     INV
// ---------------------------------------------------------------------------------
// BMBUSY#/GPIO0           ?                            G       I        -       -
// TACH1/GPIO1             ?                            G       I        -       -
// PIRQE#/GPIO2            ?                            G       I        -       -
// PIRQF#/GPIO3            ?                            G       I        -       -

// PIRQG#/GPIO4            ?                            G       I        -       -
// PIRQH#/GPIO5            ?                            G       I        -       -
// TACH2/GPIO6             ?                            G       I        -       -
// TACH3/GPIO7             ?                            G       I        -       -

// OCS/GPIO8               ?                            G       O        H       -
// OC#5/GPIO9              OC#5                         N       -        -       -
// OC#6/GPIO10             OC#6                         N       -        -       -
// SMB_ALERT#/GPIO11       SMB_ALERT                    N       -        -       -

// LANPHYPC/GPIO12         LANPHYPC                     N       -        -       -
// DOCK_AZ_RST#/GPIO13     ?                            G       I        -       -
// OC#7/GPIO14             OC#7                         N       -        -       -
// GPIO15                  ?                            G       O        L       -

// SATA4GP/GPIO16          ?                            N       -        -       -
// TACH0/GPIO17            ?                            G       I        -       -
// SRC1CLKRQ#/GPIO18       SRC1CLKRQ##                  N       -        -       -
// SATA1GP/GPIO19          ?                            N       -        -       -

// SRC2CLKRQ#/GPIO20       SRC2CLKRQ#                   N       -        -       -
// SATA0GP/GPIO21          ?                            N       -        -       -
// SCLOCK/GPIO22           ?                            G       I        -       -
// LDRQ1#/GPIO23           LDRQ1#                       N       -        -       -

// GPIO24                  ?                            G       O        L       -
// SRC3CLKRQ#/GPIO25       SRC3CLKRQ#                   N       -        -       -
// SRC4CLKRQ#/GPIO26       SRC4CLKRQ#                   N       -        -       -
// GPIO27                  ?                            G       I        L       -

// GPIO28                  ?                            G       O        L       -
// SLP_WLAN#/GPIO29        SLP_WLAN#                    N       -        -       -
// GPIO30                  ?                            G       I        -       -
// ACPRESENT/GPIO31        ?                            G       I        -       -

// CLKRUN#/GPIO32          CLKRUN#                      N       -        -       -
// DOCK_EN#/GPIO33         ?                            G       O        L       -
// STP_PCI#/GPIO34         ?                            G       O        H       -
// SATACLKREQ#/GPIO35      ?                            G       O        L       -

// SATA2GP/GPIO36          ?                            N       -        -       -
// SATA3GP/GPIO37          ?                            N       -        -       -
// SLOAD/GPIO38            ?                            G       I        -       -
// SDATAOUT[0]/GPIO39      ?                            G       I        -       -

// OC1#/GPIO40             OC#1                         N       -        -       -
// OC2#/GPIO41             OC#2                         N       -        -       -
// OC3#/GPIO42             OC#3                         N       -        -       -
// OC4#/GPIO43             OC#4                         N       -        -       -

// SRC5CLKRQ#/GPIO44       SRC5CLKRQ#                   N       -        -       -
// SRC6CLKRQ#/GPIO45       SRC6CLKRQ#                   N       -        -       -
// SRC7CLKRQ#/GPIO46       SRC7CLKRQ#                   N       -        -       -
// PEG_A_CLKRQ#/GPIO47     PEG_A_CLKRQ#                 N       -        -       -

// SDATAOUT1/GPIO48        ?                            G       I        -       -
// SATA5GP/GPIO49          ?                            N       -        -       -
// GPIO50                  ?                            G       O        H       -
// GSXDOUT#/GPIO51         GSXDOUT#                     N       -        -       -

// GSXLOAD#/GPIO52         GSXLOAD#                     N       -        -       -
// GSXDIN#/GPIO53          GSXDIN#                      N       -        -       -
// GSXRESET#/GPIO54        GSXRESET#                    N       -        -       -
// GPIO55                  ?                            G       O        H       -

// PEG_B_CLKRQ#/GPIO56     PEG_B_CLKRQ#                 N       -        -       -
// GPIO57                  ?                            G       I        -       -
// SML1CLK/GPIO58          SML1_CLK                     N       -        -       -
// OC0#/GPIO59             OC0#                         N       -        -       -

// SML0ALERT#/GPIO60       SML0ALERT#                   N       -        -       -
// SUS_STAT#/GPIO61        SUS_STAT#                    N       -        -       -
// SUSCLK/GPIO62           SUS_CLK                      N       -        -       -
// SLP_S5#/GPIO63          SLP_S5#                      N       -        -       -

// CLKOUTFLEX0/GPIO64      CLKOUTFLEX0                  N       -        -       -
// CLKOUTFLEX1/GPIO65      CLKOUTFLEX1                  N       -        -       -
// CLKOUTFLEX2/GPIO66      CLKOUTFLEX2                  N       -        -       -
// CLKOUTFLEX3/GPIO67      CLKOUTFLEX3                  N       -        -       -

// TACH4/GPIO68            ?                            G       I        -       -
// TACH5/GPIO69            SV_BOARD_DETECT              G       I        -       -
// USB3_P5#/GPIO70         USB3_P5#                     N       -        -       -
// USB3_P6#/GPIO71         USB3_P6                      N       -        -       -

// BATLOW#/GPIO72          BATLOW#                      N       -        -       -
// SRC0CLKRQ#/GPIO73       SRC0CLKRQ#                   N       -        -       -
// SML1ALERT#/GPIO74       SML1ALERT#                   N       -        -       -
// SML1DATA#/GPIO75        SML1DATA#                    N       -        -       -

#define SV_GPIO_USE_SEL_VAL             BIT31 | BIT28 | BIT27 | BIT24 | BIT22 | BIT17 | BIT15 | \
                                            BIT13 | BIT8 | BIT7 | BIT6| BIT5| BIT4 | BIT3 | BIT2| BIT1 | BIT0

#define SV_GPIO_USE_SEL2_VAL            BIT25 | BIT23 | BIT18 | BIT16 | BIT7 | BIT6| BIT3 | BIT2| BIT1

#define SV_GPIO_USE_SEL3_VAL            BIT5 | BIT4

#define SV_GPIO_IO_SEL_VAL              BIT31 | BIT27 | BIT22 | BIT17 | \
                                            BIT13 | BIT7 | BIT6| BIT5| BIT4 | BIT3 | BIT2| BIT1 | BIT0

#define SV_GPIO_IO_SEL2_VAL             BIT25 | BIT16 | BIT7 | BIT6| BIT2

#define SV_GPIO_IO_SEL3_VAL             0x00000000

#define SV_GPIO_LVL_VAL                 BIT8
#define SV_GPIO_LVL2_VAL                BIT23 | BIT18 | BIT1
#define SV_GPIO_LVL3_VAL                0x00000000

#define SV_GPIO_INV_VAL                 0x00000000
#define SV_GPIO_BLINK_VAL               0x00000000
#define SV_GPIO_RST_SEL1                0x00000000
#define SV_GPIO_RST_SEL2                0x00000000


//
// GPIO Register Settings for ULT ERB- WhiteTip Mountain1 - LPTLP Interposer
//
// Field Descriptions:
//    USE: Defines the pin's usage model:  GPIO (G) or Native (N) mode.
//    I/O: Defines whether GPIOs are inputs (I) or outputs (O).
//         (Note:  Only meaningful for pins used as GPIOs.)
//    LVL: This field gives you the initial value for "output" GPIO's.
//    INV: Defines whether Input GPIOs activation level is inverted.
//         (Note:  Only affects the level sent to the GPE logic and does not
//         affect the level read through the GPIO registers.)
//
// Signal                  UsedAs                       USE     I/O      LVL     INV
// ---------------------------------------------------------------------------------
// BMBUSYB_GP0                  PCH_AUDIO_PWR_N         G       O        L       -
// GP1_UART1_TXD                UART1_TD                G       I        -       -
// GP2_UART1_RTSB               UART1_RTS_N             G       I        -       -
// GP3_UART1_CTSB               UART1_CTS_N             G       I        -       -
// GP4_I2C0_SDA                 I2C0_SDA                G       I        -       -
// GP5_I2C0_SCL                 I2C0_SCL                G       I        -       -
// GP6_I2C1_SDA                 I2C1_SDA                G       I        -       -
// GP7_I2C1_SCL                 I2C1_SCL                G       I        -       -

// GP8_OCS                      PCIE_SLOT3_WAKE_N       G       I        -       I
// OC5B_GP9                     ACCEL_INT2_CLV          G       I        -       -
// OC6B_GP10                    SMC_RUNTIME_SCI_N       G       I        -       I
// GP11_SMBALERTB               SMBALERT_N              N       -        -       -
// GP12_LANPHYPC                LANPHY_EN               N       -        -       -
// GP13                         dockazrst#              N       -        -       -
// GP14                         SENSOR_HUB_INT_N        G       I        -       -
// GP15                         LAN_PWR_EN              G       O        L       -

// GP16_MGPIO9                  LAN_DLY_RST_N           G       O        L       -
// GP17_MGPIO10                 PCH_AUDIO_PWR_N         G       I        -       -
// GP18_SRC0CLKRQB              CK_REQ_P1_SLOT2_N       N       -        -       -
// GP19_SATA1GP                 DIRECT_ESATA1_DETECT_N/ G       I        -       -
//                              /BBS0_INTRPSR
// GP20_SRC2CLKRQB              CK_REQ_P2_SLOT3_N       N       -        -       -
// GP21_SATA0GP                 MPCIE_RST_N             G       O        L       -
//                              /NGFF_WIFI_RST
// GP22_SRC4CLKRQB_TRST         CK_REQ_SRC4             G       I        -       -
// GP23_SRC5CLKRQB_TDI2         CK_REQ_P5_MC_N          G       I        -       -

// GP24_MGPIO0                  PG_LED_MEGPIO           G       I        -       -
// GP25_SRC3CLKRQB              NC                      N       -        -       -
// GP26_SRC4CLKRQB_TRST         CK_REQ_SRC4             N       -        -       -
// GP27_MGPIO6                  SMC_WAKE_SCI_N          G       I        -       -
// GP28_MGPIO7                  NFC_RESET               G       O        H       -
// GP29_SLP_WLANB_MGPIO3        SLP_WLAN_N              N       -        -       -
// GP30                         SUSWARN_N               N       -        -       -
// GP31_ACPRESENT_MGPIO2        ACPRESENT               N       -        -       -

// GP32_CLKRUNB                 CLKRUN_N                N       -        -       -
// GP33_DEVSLP0                 dock_en#                N       -        -       -
// GP34                         MPCIE_RST_N             G       I        -       -
// GP35                         GYRO_INT2_CLV           G       O        L       -
// GP36_SATA2GP                 SATA_ODD_PRSNT_N        G       I        -       -
// GP37_SATA3GP                 MSATA_MPCIE_DET_N       G       I        -       -
// GP38_DEVSLP1                 NC                      G       I        -       -
// GP39_DEVSLP2                 NC                      G       I        -       -

// GP40_OC0B                   USB_OC_67_N              N       -        -       -
// GP41_OC1B                   USB_OC_45_N              N       -        -       -
// GP42_OC2B                   USB32_P1_PWREN           G       O        H       -
// GP43                        PCIE_SLOT3_RST_N         G       O        L       -
// GP44                        CK_REQ_P5_MC_N           N       -        -       -
// GP45_TMS2                   PCIE_SLOT4_WAKE_N        G       I        -       -
// GP46_TDO2                   SNSR_HUB_PWREN_N         N       -        -       -
// GP47                        SPI_TPM_HDR_IRQ_N        G       I        -       -

// GP48                        GYRO_INT2_CLV            G       I        -       -
// GP49                        CRIT_TEMP_REP_N /        G       I        -       -
//                             SATA_ODD_DA_N
// GP50                        TCHP_INTR_N              G       I        -       -
// GP51_SML1CLK_MGPIO11        SML1_CK                  G       I        -       -
// GP52_GSXSLOAD               GYRO_INT1_CLV            G       I        -       -
// GP53_GSXDIN                 CODEC_IRQ                G       I        -       -
// GP54_GSXSRESETB             PCH_MC_WAKE_R_N          G       I        -       -
// GP55_GSXCLK                 SDIO_D0                  G       I        -       -

// GP56                        PCIE_SLOT4_RST_N         G       O        L       -
// GP57                        NFC_IRQ                  G       I        -       -
// GP58                        SPI_TPM_HDR_PWREN_N      N       -        -       -
// GP59_OC0B                   USB_OC_01_N              N       -        -       -
// GP60_SML0ALERTB_MGPIO4      PCIE_SLOT3_PWREN_N       G       I        -       -
// GP61_SUS_STATB              PM_SUS_STAT_N            N       -        -       -
// GP62_SUSCLK                 SUS_CK                   N       -        -       -
// GP63_SLP_S5B                PM_SLP_S5_N              N       -        -       -

// GP64_SDIO_CLK               SDIO_CLK                 N       -        -       -
// GP65_SDIO_CMD               SDIO_CMD                 N       -        -       -
// GP66_SDIO_D0                SDIO_D0                  N       -        -       -
// GP67_SDIO_D1                SDIO_D1                  N       -        -       -
// GP68_SDIO_D2                SDIO_D2                  N       -        -       -
// GP69_SDIO_D3                SDIO_D3                  G       I        -       -
// GP70_SDIO_POWER_EN          SDIO_POWER_EN            N       -        -       -
// GP71_MPHYPC                 mPHY_PC                  N       -        -       -

// GP72_BATLOWB                BATLOW_N                 N       -        -       -
// GP73                        CK_REQ_P0_SLOT1_N        N       -        -       -
// GP74_SML1ALERTB_PCHHOTB     PCH_HOT_N                G       I        -       -
// GP75_SML1DATA_MGPIO12       SML1_DATA                N       -        -       -

#define   MB_RVP_ULT_ERB_GPIO_USE_SEL_VAL            BIT28 | BIT27 | BIT24 | BIT23 |BIT22 | BIT21 | BIT19 | BIT17 | BIT16 | BIT15 | BIT14 | \
                                                     BIT10 |BIT9 |BIT8 | BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0
#define   MB_RVP_ULT_ERB_GPIO_USE_SEL2_VAL           BIT28 | BIT25 | BIT24 | BIT23 | BIT22 | BIT21 | BIT20 | BIT19 | BIT18 | \
                                                     BIT17 | BIT16 | BIT15 | BIT13 | BIT11 | BIT10 | BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 
#define   MB_RVP_ULT_ERB_GPIO_USE_SEL3_VAL           BIT10 | BIT5

#define   MB_RVP_ULT_ERB_GPIO_IO_SEL_VAL             BIT27 | BIT24 | BIT23 | BIT22 |  BIT19 | BIT17 | BIT14 | \
                                                     BIT10 |BIT9 |BIT8 | BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 
#define   MB_RVP_ULT_ERB_GPIO_IO_SEL2_VAL            BIT28 | BIT25 | BIT23 | BIT22 | BIT21 | BIT20 | BIT19 | BIT18 | \
                                                     BIT17 | BIT16 | BIT15 | BIT13 | BIT7 | BIT6 | BIT5 | BIT4 | BIT2
#define   MB_RVP_ULT_ERB_GPIO_IO_SEL3_VAL            BIT10 | BIT5

#define   MB_RVP_ULT_ERB_GPIO_LVL_VAL                BIT28
#define   MB_RVP_ULT_ERB_GPIO_LVL2_VAL               BIT28 | BIT10
#define   MB_RVP_ULT_ERB_GPIO_LVL3_VAL               0x00000000

#define   MB_RVP_ULT_ERB_GPIO_INV_VAL                BIT10 | BIT8
#define   MB_RVP_ULT_ERB_GPIO_BLINK_VAL              0x00000000
#define   MB_RVP_ULT_ERB_GPIO_RST_SEL1               BIT30 | BIT24 | BIT8 // Bit30 = 1 is required by Section 4.5 of PCH BIOS Spec V0.5
#define   MB_RVP_ULT_ERB_GPIO_RST_SEL2               BIT14


//
// GPIO Register Settings for ULT ERB- Big Creek - LPTLP Interposer
//
// Field Descriptions:
//    USE: Defines the pin's usage model:  GPIO (G) or Native (N) mode.
//    I/O: Defines whether GPIOs are inputs (I) or outputs (O).
//         (Note:  Only meaningful for pins used as GPIOs.)
//    LVL: This field gives you the initial value for "output" GPIO's.
//    INV: Defines whether Input GPIOs activation level is inverted.
//         (Note:  Only affects the level sent to the GPE logic and does not
//         affect the level read through the GPIO registers.)
//
// Signal                  UsedAs                       USE     I/O      LVL     INV
// ---------------------------------------------------------------------------------
// GP0_UART1_RXD                UART1_RXD               N       -        -       -
// GP1_UART1_TXD                UART1_TD                N       -        -       -
// GP2_UART1_RTSB               UART1_RTS_N             N       -        -       -
// GP3_UART1_CTSB               UART1_CTS_N             N       -        -       -
// GP4_I2C0_SDA                 I2C0_SDA                N       -        -       -
// GP5_I2C0_SCL                 I2C0_SCL                N       -        -       -
// GP6_I2C1_SDA                 I2C1_SDA                N       -        -       -
// GP7_I2C1_SCL                 I2C1_SCL                N       -        -       -

// GP8                          PCIE_SLOT3_WAKE_N       N       -        -       -
// GP9                          TOUCH_INTR_N            N       -        -       -
// GP10                         TCHP_INTR_N             N       -        -       -
// GP11_SMBALERTB               SMBALERT_N              N       -        -       -
// GP12_LANPHYPC                LANPHY_EN               N       -        -       -
// GP13                         USB32_P0_PWREN          N       -        -       -
// GP14                         MPCIE_WAKE_N            N       -        -       -
// GP15                         LAN_PWR_EN              G       O        H       -

// GP16_MGPIO9                  LAN_DLY_RST_N           N       -        -       -
// GP17_MGPIO10                 PCH_AUDIO_PWR_N         N       -        -       -
// GP18_SRC0CLKRQB              CK_REQ_P0_SLOT1_N       N       -        -       -
// GP19_SRC1CLKRQB              CK_REQ_P1_SLOT2_N       N       -        -       -
// GP20_SRC2CLKRQB              CK_REQ_P2_SLOT3_N       N       -        -       -
// GP21_SRC3CLKRQB              CK_REQ_P3_SLOT4_N       N       -        -       -
// GP22_SRC4CLKRQB_TRST         CK_REQ_SRC4             N       -        -       -
// GP23_SRC5CLKRQB_TDI2         CK_REQ_P5_MC_N          N       -        -       -

// GP24_MGPIO0                   USB2_4_5_PWREN         G       O        H       -
// GP25_USBWAKEOUTB              USB_WAKEOUT_N          N       -        -       -
// GP26_MGPIO5                   NFC_IRQ                N       -        -       -
// GP27_MGPIO6                   SMC_WAKE_SCI_N         G       O        H       -
// GP28_MGPIO7                   NFC_RESET              G       O        H       -
// GP29_SLP_WLANB_MGPIO3         SLP_WLAN_N             N       -        -       -
// GP30                          SUSWARN_N              N       -        -       -
// GP31_ACPRESENT_MGPIO2         ACPRESENT              N       -        -       -

// GP32_CLKRUNB                  CLKRUN_N               N       -        -       -
// GP33_DEVSLP0                  SATA0_PHYSLP_MPCIE     N       -        -       -
// GP34                          MPCIE_RST_N            N       -        -       -
// GP35                          DIRECT_ESATA1_DETECT_N G       O        H       -
// GP36_SATA2GP                  SATA_ODD_PRSNT_N       N       -        -       -
// GP37_SATA3GP                  MSATA_MPCIE_DET_N      N       -        -       -
// GP38_DEVSLP1                  SATA1_PHYSLP_DIRECT    N       -        -       -
// GP39_DEVSLP2                  SATA2_PHYSLP_CMNHDR    N       -        -       -

// GP40_OC0B                     USB_OC_01_N            N       -        -       -
// GP41_OC1B                     USB_OC_67_N            N       -        -       -
// GP42_OC2B                     USB_OC_45_N            N       -        -       -
// GP43_OC3B                     USB32_P1_PWREN         G       O        H       -
// GP44                          SATA_ODD_DA_N          G       O        H       -
// GP45_TMS2                     PCIE_SLOT4_WAKE_N      G       I        -       -
// GP46_TDO2                     PCIE_SLOT5_WAKE_N      G       I        -       -
// GP47                          SMC_EXTSMI_N           G       O        H       -

// GP48                          GYRO_INT2_CLV          N       -        -       -
// GP49                          ALS_INT_N_CLV          N       -        -       -
// GP50                          CODEC_IRQ              G       O        H       -
// GP51_GSXDOUT                  SENSOR_HUB_INT_N       G       O        L       -
// GP52_GSXSLOAD                 GYRO_INT1_CLV          G       O        L       -
// GP53_GSXDIN                   ACCEL_INT1_CLV         G       O        L       -
// GP54_GSXSRESETB               SENSOR_HUB_RST_N       G       O        H       -
// GP55_GSXCLK                   SMC_RUNTIME_SCI_N      G       O        H       -

// GP56                          PCIE_SLOT4_RST_N       G       O        H       -
// GP57                          PCIE_SLOT4_PWREN_N     G       O        H       -
// GP58                          SPI_TPM_HDR_PWREN_N    G       O        H       -
// GP59                          PCIE_SLOT3_RST_N       N       -        -       -
// GP60_SML0ALERTB_MGPIO4        PCIE_SLOT3_PWREN_N     N       -        -       -
// GP61_SUS_STATB                PM_SUS_STAT_N          N       -        -       -
// GP62_SUSCLK                   SUS_CK                 N       -        -       -
// GP63_SLP_S5B                  PM_SLP_S5_N            N       -        -       -

// GP64_SDIO_CLK                 SDIO_CLK               N       -        -       -
// GP65_SDIO_CMD                 SDIO_CMD               N       -        -       -
// GP66_SDIO_D0                  SDIO_D0                N       -        -       -
// GP67_SDIO_D1                  SDIO_D1                N       -        -       -
// GP68_SDIO_D2                  SDIO_D2                N       -        -       -
// GP69_SDIO_D3                  SDIO_D3                N       -        -       -
// GP70_SDIO_POWER_EN            SDIO_POWER_EN          N       -        -       -
// GP71_MPHYPC                   mPHY_PC                N       -        -       -

// GP72_BATLOWB                  BATLOW_N               N       -        -       -
// GP73                          PCH_HOT_R_N            N       -        -       -
// GP74_SML1DATA_MGPIO12         SML1_DATA              N       -        -       -
// GP75_SML1CLK_MGPIO11          SML1_CK                N       -        -       -

#define   SV_ULT_ERB_GPIO_USE_SEL_VAL            BIT28 | BIT27 | BIT24 | BIT15
#define   SV_ULT_ERB_GPIO_USE_SEL2_VAL           BIT25 | BIT24 | BIT23 | BIT22 | BIT21 | BIT20 | BIT19 | BIT15 | \
                                                 BIT14 | BIT13 | BIT12 | BIT11 | BIT3
#define   SV_ULT_ERB_GPIO_USE_SEL3_VAL           0x00000000

#define   SV_ULT_ERB_GPIO_IO_SEL_VAL             0x00000000
#define   SV_ULT_ERB_GPIO_IO_SEL2_VAL            BIT14 | BIT13
#define   SV_ULT_ERB_GPIO_IO_SEL3_VAL            0x00000000

#define   SV_ULT_ERB_GPIO_LVL_VAL                BIT28 | BIT27 | BIT24 | BIT15
#define   SV_ULT_ERB_GPIO_LVL2_VAL               BIT25 | BIT24 | BIT23 | BIT22 | BIT15 | BIT12 | BIT11 | BIT3
#define   SV_ULT_ERB_GPIO_LVL3_VAL               0x00000000

#define   SV_ULT_ERB_GPIO_INV_VAL                BIT10 | BIT8
#define   SV_ULT_ERB_GPIO_BLINK_VAL              0x00000000
#define   SV_ULT_ERB_GPIO_RST_SEL1               0x00000000
#define   SV_ULT_ERB_GPIO_RST_SEL2               0x00000000

#endif
