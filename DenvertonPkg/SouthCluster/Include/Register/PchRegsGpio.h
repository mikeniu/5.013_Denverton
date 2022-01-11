/** @file
  Register names for PCH GPIO

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
 Copyright (c) 2013 - 2016 Intel Corporation. All rights reserved
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
#ifndef _PCH_REGS_GPIO_H_
#define _PCH_REGS_GPIO_H_

#define V_PCH_GPIO_NC_PAD_MAX               41
#define V_PCH_GPIO_SC_DFX_PAD_MAX           18
#define V_PCH_GPIO_SC0_PAD_MAX              53
#define V_PCH_GPIO_SC1_PAD_MAX              42

#define V_PCH_GPIO_GPP_A_PAD_MAX            24
#define V_PCH_GPIO_GPP_B_PAD_MAX            24
#define V_PCH_GPIO_GPP_C_PAD_MAX            24
#define V_PCH_GPIO_GPP_D_PAD_MAX            24
#define V_PCH_LP_GPIO_GPP_E_PAD_MAX         24
#define V_PCH_H_GPIO_GPP_E_PAD_MAX          13
#define V_PCH_GPIO_GPP_F_PAD_MAX            24
#define V_PCH_LP_GPIO_GPP_G_PAD_MAX         8
#define V_PCH_H_GPIO_GPP_G_PAD_MAX          24
#define V_PCH_H_GPIO_GPP_H_PAD_MAX          24
#define V_PCH_H_GPIO_GPP_I_PAD_MAX          11
#define V_PCH_GPIO_GPD_PAD_MAX              12

#define V_PCH_GPIO_GROUP_MAX                10
#define V_PCH_H_GPIO_GROUP_MAX              4
#define V_PCH_LP_GPIO_GROUP_MAX             8
#define V_PCH_GPIO_NUM_SUPPORTED_GPIS       204
#define S_PCH_GPIO_GP_SMI_EN                8
#define S_PCH_GPIO_GP_SMI_STS               8

///
/// Groups mapped to 2-tier General Purpose Event will all be under
/// one master GPE_111 (0x6F)
///
#define PCH_GPIO_2_TIER_MASTER_GPE_NUMBER  0x6F

#define V_PCH_PCR_GPIO_GPP_PADCFGLOCK_ALL   ~(0)

//
// GPIO Common Private Configuration Registers
//
#define R_PCH_PCR_GPIO_REV_ID               0x00
#define R_PCH_PCR_GPIO_CAP_LIST             0x04
#define R_PCH_PCR_GPIO_FAMBAR               0x08
#define R_PCH_PCR_GPIO_PADBAR               0x0C
#define B_PCH_PCR_GPIO_PADBAR               0x0000FFFF
#define R_PCH_PCR_GPIO_MISCCFG              0x10
#define B_PCH_PCR_GPIO_MISCCFG_GPE0_DW2     (BIT19 | BIT18 | BIT17 | BIT16)
#define N_PCH_PCR_GPIO_MISCCFG_GPE0_DW2     16
#define B_PCH_PCR_GPIO_MISCCFG_GPE0_DW1     (BIT15 | BIT14 | BIT13 | BIT12)
#define N_PCH_PCR_GPIO_MISCCFG_GPE0_DW1     12
#define B_PCH_PCR_GPIO_MISCCFG_GPE0_DW0     (BIT11 | BIT10 | BIT9 | BIT8)
#define N_PCH_PCR_GPIO_MISCCFG_GPE0_DW0     8
#define B_PCH_PCR_GPIO_MISCCFG_NCSI_STAGE_EN BIT4
#define N_PCH_PCR_GPIO_MISCCFG_NCSI_STAGE_EN 4
#define B_PCH_PCR_GPIO_MISCCFG_IRQ_ROUTE    BIT3
#define N_PCH_PCR_GPIO_MISCCFG_IRQ_ROUTE    3
#define B_PCH_PCR_GPIO_MISCCFG_GPDPCGEN     BIT1
#define B_PCH_PCR_GPIO_MISCCFG_GPDLCGEN     BIT0
// SKL PCH-H:
#define R_PCH_H_PCR_GPIO_MISCSECCFG         0x14

//
// GPIO Community 0 Private Configuration Registers
//

//
// Power Group NORTH_ALL
//
#define R_PCH_PCR_GPIO_NC_PAD_OWN           0x20
#define R_PCH_PCR_GPIO_NC_GPI_VWM_EN        0x70
#define R_PCH_PCR_GPIO_NC_PADCFGLOCK        0x90
#define R_PCH_PCR_GPIO_NC_PADCFGLOCKTX      0x94
#define R_PCH_PCR_GPIO_NC1_PADCFGLOCK_1     0x98
#define R_PCH_PCR_GPIO_NC1_PADCFGLOCKTX_1   0x9C
#define R_PCH_PCR_GPIO_NC_HOSTSW_OWN        0xC0
#define R_PCH_PCR_GPIO_NC_GPI_IS            0x0100
#define R_PCH_PCR_GPIO_NC_GPI_IE            0x0120
#define R_PCH_PCR_GPIO_NC_GPI_GPE_STS       0x0140
#define R_PCH_PCR_GPIO_NC_GPI_GPE_EN        0x0160
#define R_PCH_PCR_GPIO_NC_SMI_STS           0x0180
#define R_PCH_PCR_GPIO_NC_SMI_EN            0x01A0
#define R_PCH_PCR_GPIO_NC_NMI_STS           0x01C0
#define R_PCH_PCR_GPIO_NC_NMI_EN            0x01E0
#define R_PCH_PCR_GPIO_NC_PADCFG_OFFSET     0x400
//
// GPIO Community 1 Private Configuration Registers
//

//
// Power Group SOUTH_DFX
//
#define R_PCH_PCR_GPIO_SC_DFX_PAD_OWN       0x20
#define R_PCH_PCR_GPIO_SC_DFX_GPI_VWM_EN    0x70
#define R_PCH_PCR_GPIO_SC_DFX_PADCFGLOCK    0x90
#define R_PCH_PCR_GPIO_SC_DFX_PADCFGLOCKTX  0x94
#define R_PCH_PCR_GPIO_SC_DFX_HOSTSW_OWN    0xC0
#define R_PCH_PCR_GPIO_SC_DFX_GPI_IS        0x0100
#define R_PCH_PCR_GPIO_SC_DFX_GPI_IE        0x0120
#define R_PCH_PCR_GPIO_SC_DFX_GPI_GPE_STS   0x0140
#define R_PCH_PCR_GPIO_SC_DFX_GPI_GPE_EN    0x0160
#define R_PCH_PCR_GPIO_SC_DFX_PADCFG_OFFSET 0x400
//
// Power Group SOUTH_GROUP0
//
#define R_PCH_PCR_GPIO_SC0_PAD_OWN          0x2C
#define R_PCH_PCR_GPIO_SC0_GPI_VWM_EN       0x74
#define R_PCH_PCR_GPIO_SC0_PADCFGLOCK       0x98
#define R_PCH_PCR_GPIO_SC0_PADCFGLOCKTX     0x9C
#define R_PCH_PCR_GPIO_SC0_HOSTSW_OWN       0xC4
#define R_PCH_PCR_GPIO_SC0_GPI_IS           0x0104
#define R_PCH_PCR_GPIO_SC0_GPI_IE           0x0124
#define R_PCH_PCR_GPIO_SC0_GPI_GPE_STS      0x0144
#define R_PCH_PCR_GPIO_SC0_GPI_GPE_EN       0x0164
#define R_PCH_PCR_GPIO_SC0_SMI_STS          0x0184
#define R_PCH_PCR_GPIO_SC0_SMI_EN           0x01A4
#define R_PCH_PCR_GPIO_SC0_NMI_STS          0x01C4
#define R_PCH_PCR_GPIO_SC0_NMI_EN           0x01E4
#define R_PCH_PCR_GPIO_SC0_PADCFG_OFFSET    0x490
//
// Power Group SOUTH_GROUP1
//
#define R_PCH_PCR_GPIO_SC1_PAD_OWN          0x48
#define R_PCH_PCR_GPIO_SC1_GPI_VWM_EN       0x7C
#define R_PCH_PCR_GPIO_SC1_PADCFGLOCK       0xA8
#define R_PCH_PCR_GPIO_SC1_PADCFGLOCKTX     0xAC
#define R_PCH_PCR_GPIO_SC1_HOSTSW_OWN       0xCC
#define R_PCH_PCR_GPIO_SC1_GPI_IS           0x010C
#define R_PCH_PCR_GPIO_SC1_GPI_IE           0x012C
#define R_PCH_PCR_GPIO_SC1_GPI_GPE_STS      0x014C
#define R_PCH_PCR_GPIO_SC1_GPI_GPE_EN       0x016C
#define R_PCH_PCR_GPIO_SC1_SMI_STS          0x018C
#define R_PCH_PCR_GPIO_SC1_SMI_EN           0x01AC
#define R_PCH_PCR_GPIO_SC1_NMI_STS          0x01CC
#define R_PCH_PCR_GPIO_SC1_NMI_EN           0x01EC
#define R_PCH_PCR_GPIO_SC1_PADCFG_OFFSET    0x638

#define R_PCH_PCR_GPIO_GPP_PADCFGLOCK_NORTH_ALL_0     0x90
#define R_PCH_PCR_GPIO_GPP_PADCFGLOCKTX_NORTH_ALL_0   0x94
#define R_PCH_PCR_GPIO_GPP_PADCFGLOCK_NORTH_ALL_1     0x98
#define R_PCH_PCR_GPIO_GPP_PADCFGLOCKTX_NORTH_ALL_1   0x9C

#define R_PCH_PCR_GPIO_GPP_PADCFGLOCK_SOUTH_DFX_0        0x90
#define R_PCH_PCR_GPIO_GPP_PADCFGLOCKTX_SOUTH_DFX_0      0x94
#define R_PCH_PCR_GPIO_GPP_PADCFGLOCK_SOUTH_GROUP0_0     0x98
#define R_PCH_PCR_GPIO_GPP_PADCFGLOCKTX_SOUTH_GROUP0_0   0x9C
#define R_PCH_PCR_GPIO_GPP_PADCFGLOCK_SOUTH_GROUP0_1     0xA0
#define R_PCH_PCR_GPIO_GPP_PADCFGLOCKTX_SOUTH_GROUP0_1   0xA4
#define R_PCH_PCR_GPIO_GPP_PADCFGLOCK_SOUTH_GROUP1_0     0xA8
#define R_PCH_PCR_GPIO_GPP_PADCFGLOCKTX_SOUTH_GROUP1_0   0xAC
#define R_PCH_PCR_GPIO_GPP_PADCFGLOCK_SOUTH_GROUP1_1     0xB0
#define R_PCH_PCR_GPIO_GPP_PADCFGLOCKTX_SOUTH_GROUP1_1   0xB4

//
// GPIO Community 0 Private Configuration Registers
//
// SKL PCH-LP
#define R_PCH_LP_PCR_GPIO_GPP_A_PAD_OWN        0x20
#define R_PCH_LP_PCR_GPIO_GPP_B_PAD_OWN        0x30
#define R_PCH_LP_PCR_GPIO_GPP_A_GPI_VWM_EN     0x80
#define R_PCH_LP_PCR_GPIO_GPP_B_GPI_VWM_EN     0x84
#define R_PCH_LP_PCR_GPIO_GPP_A_PADCFGLOCK     0xA0
#define R_PCH_LP_PCR_GPIO_GPP_A_PADCFGLOCKTX   0xA4
#define R_PCH_LP_PCR_GPIO_GPP_B_PADCFGLOCK     0xA8
#define R_PCH_LP_PCR_GPIO_GPP_B_PADCFGLOCKTX   0xAC
// SKL PCH-H
#define R_PCH_H_PCR_GPIO_GPP_A_PAD_OWN        0x20
#define R_PCH_H_PCR_GPIO_GPP_B_PAD_OWN        0x2C
#define R_PCH_H_PCR_GPIO_GPP_A_GPI_VWM_EN     0x70
#define R_PCH_H_PCR_GPIO_GPP_B_GPI_VWM_EN     0x74
#define R_PCH_H_PCR_GPIO_GPP_A_PADCFGLOCK     0x90
#define R_PCH_H_PCR_GPIO_GPP_A_PADCFGLOCKTX   0x94
#define R_PCH_H_PCR_GPIO_GPP_B_PADCFGLOCK     0x98
#define R_PCH_H_PCR_GPIO_GPP_B_PADCFGLOCKTX   0x9C
// Common
#define R_PCH_PCR_GPIO_GPP_A_HOSTSW_OWN     0xD0
#define R_PCH_PCR_GPIO_GPP_B_HOSTSW_OWN     0xD4
#define R_PCH_PCR_GPIO_GPP_A_GPI_IS         0x0100
#define R_PCH_PCR_GPIO_GPP_B_GPI_IS         0x0104
#define R_PCH_PCR_GPIO_GPP_A_GPI_IE         0x0120
#define R_PCH_PCR_GPIO_GPP_B_GPI_IE         0x0124
#define R_PCH_PCR_GPIO_GPP_A_GPI_GPE_STS    0x0140
#define R_PCH_PCR_GPIO_GPP_B_GPI_GPE_STS    0x0144
#define R_PCH_PCR_GPIO_GPP_A_GPI_GPE_EN     0x0160
#define R_PCH_PCR_GPIO_GPP_B_GPI_GPE_EN     0x0164
#define R_PCH_PCR_GPIO_GPP_A_SMI_STS        0x0180
#define R_PCH_PCR_GPIO_GPP_B_SMI_STS        0x0184
#define R_PCH_PCR_GPIO_GPP_A_SMI_EN         0x01A0
#define R_PCH_PCR_GPIO_GPP_B_SMI_EN         0x01A4
#define R_PCH_PCR_GPIO_GPP_A_NMI_STS        0x01C0
#define R_PCH_PCR_GPIO_GPP_B_NMI_STS        0x01C4
#define R_PCH_PCR_GPIO_GPP_A_NMI_EN         0x01E0
#define R_PCH_PCR_GPIO_GPP_B_NMI_EN         0x01E4
#define R_PCH_PCR_GPIO_GPP_A_PADCFG_OFFSET  0x400
#define R_PCH_PCR_GPIO_GPP_B_PADCFG_OFFSET  0x4C0

//
// GPIO Community 1 Private Configuration Registers
//
//SKL PCH-LP:
#define R_PCH_LP_PCR_GPIO_GPP_C_PAD_OWN        0x20
#define R_PCH_LP_PCR_GPIO_GPP_D_PAD_OWN        0x30
#define R_PCH_LP_PCR_GPIO_GPP_E_PAD_OWN        0x40
#define R_PCH_LP_PCR_GPIO_GPP_C_GPI_VWM_EN     0x80
#define R_PCH_LP_PCR_GPIO_GPP_D_GPI_VWM_EN     0x84
#define R_PCH_LP_PCR_GPIO_GPP_E_GPI_VWM_EN     0x88
#define R_PCH_LP_PCR_GPIO_GPP_C_PADCFGLOCK     0xA0
#define R_PCH_LP_PCR_GPIO_GPP_C_PADCFGLOCKTX   0xA4
#define R_PCH_LP_PCR_GPIO_GPP_D_PADCFGLOCK     0xA8
#define R_PCH_LP_PCR_GPIO_GPP_D_PADCFGLOCKTX   0xAC
#define R_PCH_LP_PCR_GPIO_GPP_E_PADCFGLOCK     0xB0
#define R_PCH_LP_PCR_GPIO_GPP_E_PADCFGLOCKTX   0xB4
//SKL PCH-H:
#define R_PCH_H_PCR_GPIO_GPP_C_PAD_OWN        0x20
#define R_PCH_H_PCR_GPIO_GPP_D_PAD_OWN        0x2C
#define R_PCH_H_PCR_GPIO_GPP_E_PAD_OWN        0x38
#define R_PCH_H_PCR_GPIO_GPP_F_PAD_OWN        0x40
#define R_PCH_H_PCR_GPIO_GPP_G_PAD_OWN        0x4C
#define R_PCH_H_PCR_GPIO_GPP_H_PAD_OWN        0x58
#define R_PCH_H_PCR_GPIO_GPP_C_GPI_VWM_EN     0x70
#define R_PCH_H_PCR_GPIO_GPP_D_GPI_VWM_EN     0x74
#define R_PCH_H_PCR_GPIO_GPP_E_GPI_VWM_EN     0x78
#define R_PCH_H_PCR_GPIO_GPP_F_GPI_VWM_EN     0x7C
#define R_PCH_H_PCR_GPIO_GPP_G_GPI_VWM_EN     0x80
#define R_PCH_H_PCR_GPIO_GPP_H_GPI_VWM_EN     0x84
#define R_PCH_H_PCR_GPIO_GPP_C_PADCFGLOCK     0x90
#define R_PCH_H_PCR_GPIO_GPP_C_PADCFGLOCKTX   0x94
#define R_PCH_H_PCR_GPIO_GPP_D_PADCFGLOCK     0x98
#define R_PCH_H_PCR_GPIO_GPP_D_PADCFGLOCKTX   0x9C
#define R_PCH_H_PCR_GPIO_GPP_E_PADCFGLOCK     0xA0
#define R_PCH_H_PCR_GPIO_GPP_E_PADCFGLOCKTX   0xA4
#define R_PCH_H_PCR_GPIO_GPP_F_PADCFGLOCK     0xA8
#define R_PCH_H_PCR_GPIO_GPP_F_PADCFGLOCKTX   0xAC
#define R_PCH_H_PCR_GPIO_GPP_G_PADCFGLOCK     0xB0
#define R_PCH_H_PCR_GPIO_GPP_G_PADCFGLOCKTX   0xB4
#define R_PCH_H_PCR_GPIO_GPP_H_PADCFGLOCK     0xB8
#define R_PCH_H_PCR_GPIO_GPP_H_PADCFGLOCKTX   0xBC
// Common:
#define R_PCH_PCR_GPIO_GPP_C_HOSTSW_OWN       0xD0
#define R_PCH_PCR_GPIO_GPP_D_HOSTSW_OWN       0xD4
#define R_PCH_PCR_GPIO_GPP_E_HOSTSW_OWN       0xD8
// SKL PCH-H:
#define R_PCH_H_PCR_GPIO_GPP_F_HOSTSW_OWN     0xDC
#define R_PCH_H_PCR_GPIO_GPP_G_HOSTSW_OWN     0xE0
#define R_PCH_H_PCR_GPIO_GPP_H_HOSTSW_OWN     0xE4
// Common:
#define R_PCH_PCR_GPIO_GPP_C_GPI_IS           0x0100
#define R_PCH_PCR_GPIO_GPP_D_GPI_IS           0x0104
#define R_PCH_PCR_GPIO_GPP_E_GPI_IS           0x0108
// SKL PCH-H:
#define R_PCH_H_PCR_GPIO_GPP_F_GPI_IS         0x010C
#define R_PCH_H_PCR_GPIO_GPP_G_GPI_IS         0x0110
#define R_PCH_H_PCR_GPIO_GPP_H_GPI_IS         0x0114
// Common:
#define R_PCH_PCR_GPIO_GPP_C_GPI_IE           0x0120
#define R_PCH_PCR_GPIO_GPP_D_GPI_IE           0x0124
#define R_PCH_PCR_GPIO_GPP_E_GPI_IE           0x0128
// SKL PCH-H:
#define R_PCH_H_PCR_GPIO_GPP_F_GPI_IE         0x012C
#define R_PCH_H_PCR_GPIO_GPP_G_GPI_IE         0x0130
#define R_PCH_H_PCR_GPIO_GPP_H_GPI_IE         0x0134
// Common:
#define R_PCH_PCR_GPIO_GPP_C_GPI_GPE_STS      0x0140
#define R_PCH_PCR_GPIO_GPP_D_GPI_GPE_STS      0x0144
#define R_PCH_PCR_GPIO_GPP_E_GPI_GPE_STS      0x0148
// SKL PCH-H:
#define R_PCH_H_PCR_GPIO_GPP_F_GPI_GPE_STS    0x014C
#define R_PCH_H_PCR_GPIO_GPP_G_GPI_GPE_STS    0x0150
#define R_PCH_H_PCR_GPIO_GPP_H_GPI_GPE_STS    0x0154
// Common:
#define R_PCH_PCR_GPIO_GPP_C_GPI_GPE_EN       0x0160
#define R_PCH_PCR_GPIO_GPP_D_GPI_GPE_EN       0x0164
#define R_PCH_PCR_GPIO_GPP_E_GPI_GPE_EN       0x0168
// SKL PCH-H:
#define R_PCH_H_PCR_GPIO_GPP_F_GPI_GPE_EN     0x016C
#define R_PCH_H_PCR_GPIO_GPP_G_GPI_GPE_EN     0x0170
#define R_PCH_H_PCR_GPIO_GPP_H_GPI_GPE_EN     0x0174
// Common:
#define R_PCH_PCR_GPIO_GPP_C_SMI_STS          0x0180
#define R_PCH_PCR_GPIO_GPP_D_SMI_STS          0x0184
#define R_PCH_PCR_GPIO_GPP_E_SMI_STS          0x0188
// SKL PCH-H:
#define R_PCH_H_PCR_GPIO_GPP_F_SMI_STS        0x018C
#define R_PCH_H_PCR_GPIO_GPP_G_SMI_STS        0x0190
#define R_PCH_H_PCR_GPIO_GPP_H_SMI_STS        0x0194
// Common:
#define R_PCH_PCR_GPIO_GPP_C_SMI_EN           0x01A0
#define R_PCH_PCR_GPIO_GPP_D_SMI_EN           0x01A4
#define R_PCH_PCR_GPIO_GPP_E_SMI_EN           0x01A8
// SKL PCH-H:
#define R_PCH_H_PCR_GPIO_GPP_F_SMI_EN         0x01AC
#define R_PCH_H_PCR_GPIO_GPP_G_SMI_EN         0x01B0
#define R_PCH_H_PCR_GPIO_GPP_H_SMI_EN         0x01B4
// Common:
#define R_PCH_PCR_GPIO_GPP_C_NMI_STS          0x01C0
#define R_PCH_PCR_GPIO_GPP_D_NMI_STS          0x01C4
#define R_PCH_PCR_GPIO_GPP_E_NMI_STS          0x01C8
// SKL PCH-H:
#define R_PCH_H_PCR_GPIO_GPP_F_NMI_STS        0x01CC
#define R_PCH_H_PCR_GPIO_GPP_G_NMI_STS        0x01D0
#define R_PCH_H_PCR_GPIO_GPP_H_NMI_STS        0x01D4
// Common:
#define R_PCH_PCR_GPIO_GPP_C_NMI_EN           0x01E0
#define R_PCH_PCR_GPIO_GPP_D_NMI_EN           0x01E4
#define R_PCH_PCR_GPIO_GPP_E_NMI_EN           0x01E8
// SKL PCH-H:
#define R_PCH_H_PCR_GPIO_GPP_F_NMI_EN         0x01EC
#define R_PCH_H_PCR_GPIO_GPP_G_NMI_EN         0x01F0
#define R_PCH_H_PCR_GPIO_GPP_H_NMI_EN         0x01F4
// Common:
#define R_PCH_PCR_GPIO_CAP_LIST_1_PWM         0x0200
#define R_PCH_PCR_GPIO_PWMC                   0x0204
#define R_PCH_PCR_GPIO_CAP_LIST_2_SER_BLINK   0x0208
#define R_PCH_PCR_GPIO_GP_SER_BLINK           0x020C
#define B_PCH_PCR_GPIO_GP_SER_BLINK           0x1F
#define R_PCH_PCR_GPIO_GP_SER_CMDSTS          0x0210
#define B_PCH_PCR_GPIO_GP_SER_CMDSTS_DLS      (BIT23 | BIT22)
#define N_PCH_PCR_GPIO_GP_SER_CMDSTS_DLS      22
#define B_PCH_PCR_GPIO_GP_SER_CMDSTS_DRS      0x003F0000
#define N_PCH_PCR_GPIO_GP_SER_CMDSTS_DRS      16
#define B_PCH_PCR_GPIO_GP_SER_CMDSTS_BUSY     BIT8
#define B_PCH_PCR_GPIO_GP_SER_CMDSTS_GO       BIT0
#define R_PCH_PCR_GPIO_GP_SER_DATA            0x0210
// SKL PCH-H:
#define R_PCH_H_PCR_GPIO_GSX                  0x218
#define R_PCH_H_PCR_GPIO_GSX_CAP              0x21C
#define R_PCH_H_PCR_GPIO_GSX_C0CAP_DW0        0x220
#define R_PCH_H_PCR_GPIO_GSX_C0CAP_DW1        0x224
#define R_PCH_H_PCR_GPIO_GSX_C0GPILVL_DW0     0x228
#define R_PCH_H_PCR_GPIO_GSX_C0GPILVL_DW1     0x22C
#define R_PCH_H_PCR_GPIO_GSX_C0GPOLVL_DW0     0x230
#define R_PCH_H_PCR_GPIO_GSX_C0GPOLVL_DW1     0x234
#define R_PCH_H_PCR_GPIO_GSX_C0CMD            0x238
#define R_PCH_H_PCR_GPIO_GSX_C0CTM            0x23C
// Common:
#define R_PCH_PCR_GPIO_GPP_C_PADCFG_OFFSET    0x400
#define R_PCH_PCR_GPIO_GPP_D_PADCFG_OFFSET    0x4C0
#define R_PCH_PCR_GPIO_GPP_E_PADCFG_OFFSET    0x580
// SKL PCH-H:
#define R_PCH_H_PCR_GPIO_GPP_F_PADCFG_OFFSET  0x5E8
#define R_PCH_H_PCR_GPIO_GPP_G_PADCFG_OFFSET  0x6A8
#define R_PCH_H_PCR_GPIO_GPP_H_PADCFG_OFFSET  0x768

//
// GPIO Community 2 Private Configuration Registers
//
// SKL PCH-LP
#define R_PCH_LP_PCR_GPIO_GPD_PAD_OWN         0x20
#define R_PCH_LP_PCR_GPIO_GPD_GPI_VWM_EN      0x80
#define R_PCH_LP_PCR_GPIO_GPD_PADCFGLOCK      0xA0
#define R_PCH_LP_PCR_GPIO_GPD_PADCFGLOCKTX    0xA4
// SKL PCH-H:
#define R_PCH_H_PCR_GPIO_GPD_PAD_OWN          0x20
#define R_PCH_H_PCR_GPIO_GPD_GPI_VWM_EN       0x70
#define R_PCH_H_PCR_GPIO_GPD_PADCFGLOCK       0x90
#define R_PCH_H_PCR_GPIO_GPD_PADCFGLOCKTX     0x94
// Common:
#define R_PCH_PCR_GPIO_GPD_HOSTSW_OWN       0xD0
#define R_PCH_PCR_GPIO_GPD_GPI_IS           0x0100
#define R_PCH_PCR_GPIO_GPD_GPI_IE           0x0120
#define R_PCH_PCR_GPIO_GPD_GPI_GPE_STS      0x0140
#define R_PCH_PCR_GPIO_GPD_GPI_GPE_EN       0x0160
#define R_PCH_PCR_GPIO_GPD_PADCFG_OFFSET    0x400
//
// GPIO Community 3 Private Configuration Registers
//
// SKL PCH-LP:
#define R_PCH_LP_PCR_GPIO_GPP_F_PAD_OWN        0x20
#define R_PCH_LP_PCR_GPIO_GPP_G_PAD_OWN        0x30
#define R_PCH_LP_PCR_GPIO_GPP_F_GPI_VWM_EN     0x80
#define R_PCH_LP_PCR_GPIO_GPP_G_GPI_VWM_EN     0x84
#define R_PCH_LP_PCR_GPIO_GPP_F_PADCFGLOCK     0xA0
#define R_PCH_LP_PCR_GPIO_GPP_F_PADCFGLOCKTX   0xA4
#define R_PCH_LP_PCR_GPIO_GPP_G_PADCFGLOCK     0xA8
#define R_PCH_LP_PCR_GPIO_GPP_G_PADCFGLOCKTX   0xAC
#define R_PCH_LP_PCR_GPIO_GPP_F_HOSTSW_OWN     0xD0
#define R_PCH_LP_PCR_GPIO_GPP_G_HOSTSW_OWN     0xD4
#define R_PCH_LP_PCR_GPIO_GPP_F_GPI_IS         0x0100
#define R_PCH_LP_PCR_GPIO_GPP_G_GPI_IS         0x0104
#define R_PCH_LP_PCR_GPIO_GPP_F_GPI_IE         0x0120
#define R_PCH_LP_PCR_GPIO_GPP_G_GPI_IE         0x0124
#define R_PCH_LP_PCR_GPIO_GPP_F_GPI_GPE_STS    0x0140
#define R_PCH_LP_PCR_GPIO_GPP_G_GPI_GPE_STS    0x0144
#define R_PCH_LP_PCR_GPIO_GPP_F_GPI_GPE_EN     0x0160
#define R_PCH_LP_PCR_GPIO_GPP_G_GPI_GPE_EN     0x0164
#define R_PCH_LP_PCR_GPIO_GPP_F_PADCFG_OFFSET  0x400
#define R_PCH_LP_PCR_GPIO_GPP_G_PADCFG_OFFSET  0x4C0
// SKL PCH-H:
#define R_PCH_H_PCR_GPIO_GPP_I_PAD_OWN        0x20
#define R_PCH_H_PCR_GPIO_GPP_I_GPI_VWM_EN     0x70
#define R_PCH_H_PCR_GPIO_GPP_I_PADCFGLOCK     0x90
#define R_PCH_H_PCR_GPIO_GPP_I_PADCFGLOCKTX   0x94
#define R_PCH_H_PCR_GPIO_GPP_I_HOSTSW_OWN     0xD0
#define R_PCH_H_PCR_GPIO_GPP_I_GPI_IS         0x0100
#define R_PCH_H_PCR_GPIO_GPP_I_GPI_IE         0x0120
#define R_PCH_H_PCR_GPIO_GPP_I_GPI_GPE_STS    0x0140
#define R_PCH_H_PCR_GPIO_GPP_I_GPI_GPE_EN     0x0160
#define R_PCH_H_PCR_GPIO_GPP_I_SMI_STS        0x0180
#define R_PCH_H_PCR_GPIO_GPP_I_SMI_EN         0x01A0
#define R_PCH_H_PCR_GPIO_GPP_I_NMI_STS        0x01C0
#define R_PCH_H_PCR_GPIO_GPP_I_NMI_EN         0x01E0
#define R_PCH_H_PCR_GPIO_GPP_I_PADCFG_OFFSET  0x400

//
// Define Pad Number
//
#define V_GPIO_PAD0                                  0
#define V_GPIO_PAD1                                  1
#define V_GPIO_PAD2                                  2
#define V_GPIO_PAD3                                  3
#define V_GPIO_PAD4                                  4
#define V_GPIO_PAD5                                  5
#define V_GPIO_PAD6                                  6
#define V_GPIO_PAD7                                  7
#define V_GPIO_PAD8                                  8
#define V_GPIO_PAD9                                  9
#define V_GPIO_PAD10                                10
#define V_GPIO_PAD11                                11
#define V_GPIO_PAD12                                12
#define V_GPIO_PAD13                                13
#define V_GPIO_PAD14                                14
#define V_GPIO_PAD15                                15
#define V_GPIO_PAD16                                16
#define V_GPIO_PAD17                                17
#define V_GPIO_PAD18                                18
#define V_GPIO_PAD19                                19
#define V_GPIO_PAD20                                20
#define V_GPIO_PAD21                                21
#define V_GPIO_PAD22                                22
#define V_GPIO_PAD23                                23

//
// Host Software Pad Ownership modes
//
#define V_PCH_PCR_GPIO_HOSTSW_OWN_ACPI      0x00
#define V_PCH_PCR_GPIO_HOSTSW_OWN_GPIO      0x01

//
// Pad Ownership modes
//
#define V_PCH_PCR_GPIO_PAD_OWN_HOST         0x00
#define V_PCH_PCR_GPIO_PAD_OWN_CSME         0x01
#define V_PCH_PCR_GPIO_PAD_OWN_ISH          0x02

//
// Pad Configuration Register DW0
//

//Pad Reset Config
#define B_PCH_GPIO_RST_CONF             (BIT31 | BIT30)
#define N_PCH_GPIO_RST_CONF             30
#define V_PCH_GPIO_RST_CONF_POW_GOOD    0x00
#define V_PCH_GPIO_RST_CONF_DEEP_RST    0x01
#define V_PCH_GPIO_RST_CONF_GPIO_RST    0x02
#define V_PCH_GPIO_RST_CONF_RESUME_RST  0x03  // Only for GPD Group

//RX Pad State Select
#define B_PCH_GPIO_RX_PAD_STATE         BIT29
#define N_PCH_GPIO_RX_PAD_STATE         29
#define V_PCH_GPIO_RX_PAD_STATE_RAW     0x00
#define V_PCH_GPIO_RX_PAD_STATE_INT     0x01

//RX Raw Overrride to 1
#define B_PCH_GPIO_RX_RAW1              BIT28
#define N_PCH_GPIO_RX_RAW1              28
#define V_PCH_GPIO_RX_RAW1_DIS          0x00
#define V_PCH_GPIO_RX_RAW1_EN           0x01

//RX Level/Edge Configuration
#define B_PCH_GPIO_RX_LVL_EDG           (BIT26 | BIT25)
#define N_PCH_GPIO_RX_LVL_EDG           25
#define V_PCH_GPIO_RX_LVL_EDG_LVL       0x00
#define V_PCH_GPIO_RX_LVL_EDG_EDG       0x01
#define V_PCH_GPIO_RX_LVL_EDG_0         0x02
#define V_PCH_GPIO_RX_LVL_EDG_RIS_FAL   0x03

//RX Level/Edge Configuration
#define B_PCH_GPIO_PRE_GFRX_SEL         BIT24
#define N_PCH_GPIO_PRE_GFRX_SEL         24
#define V_PCH_GPIO_PRE_GFRX_SEL_DIS     0x00
#define V_PCH_GPIO_PRE_GFRX_SEL_EN      0x01

//RX Invert
#define B_PCH_GPIO_RXINV                BIT23
#define N_PCH_GPIO_RXINV                23
#define V_PCH_GPIO_RXINV_NO             0x00
#define V_PCH_GPIO_RXINV_YES            0x01

//RXTXENCFG
#define B_PCH_GPIO_RXTXENCFG            (BIT22 |  BIT21)
#define N_PCH_GPIO_RXTXENCFG            21
#define V_PCH_GPIO_RXTXENCFG_DEF_FUN    0x00
#define V_PCH_GPIO_RXTXENCFG_TX_EN_L    0x01
#define V_PCH_GPIO_RXTXENCFG_TX_EN_H    0x02
#define V_PCH_GPIO_RXTXENCFG_TXRXEN     0x03

//GPIO Input Route IOxAPIC
#define B_PCH_GPIO_RX_APIC_ROUTE        BIT20
#define N_PCH_GPIO_RX_APIC_ROUTE        20
#define V_PCH_GPIO_RX_APIC_ROUTE_DIS    0x00
#define V_PCH_GPIO_RX_APIC_ROUTE_EN     0x01

//GPIO Input Route SCI
#define B_PCH_GPIO_RX_SCI_ROUTE         BIT19
#define N_PCH_GPIO_RX_SCI_ROUTE         19
#define V_PCH_GPIO_RX_SCI_ROUTE_DIS     0x00
#define V_PCH_GPIO_RX_SCI_ROUTE_EN      0x01

//GPIO Input Route SMI
#define B_PCH_GPIO_RX_SMI_ROUTE         BIT18
#define N_PCH_GPIO_RX_SMI_ROUTE         18
#define V_PCH_GPIO_RX_SMI_ROUTE_DIS     0x00
#define V_PCH_GPIO_RX_SMI_ROUTE_EN      0x01

//GPIO Input Route NMI
#define B_PCH_GPIO_RX_NMI_ROUTE         BIT17
#define N_PCH_GPIO_RX_NMI_ROUTE         17
#define V_PCH_GPIO_RX_NMI_ROUTE_DIS     0x00
#define V_PCH_GPIO_RX_NMI_ROUTE_EN      0x01

//GPIO Pad Mode
#define B_PCH_GPIO_PAD_MODE             (BIT12 | BIT11 | BIT10)
#define N_PCH_GPIO_PAD_MODE             10
#define V_PCH_GPIO_PAD_MODE_GPIO        0
#define V_PCH_GPIO_PAD_MODE_NAT_1       1
#define V_PCH_GPIO_PAD_MODE_NAT_2       2
#define V_PCH_GPIO_PAD_MODE_NAT_3       3
#define V_PCH_GPIO_PAD_MODE_NAT_4       4
#define V_PCH_GPIO_PAD_MODE_NAT_5       5
#define V_PCH_GPIO_PAD_MODE_NAT_6       6
#define V_PCH_GPIO_PAD_MODE_NAT_7       7

//GPIO RX Disable
#define B_PCH_GPIO_RXDIS                BIT9
#define N_PCH_GPIO_RXDIS                9
#define V_PCH_GPIO_RXDIS_EN             0x00
#define V_PCH_GPIO_RXDIS_DIS            0x01

//GPIO TX Disable
#define B_PCH_GPIO_TXDIS                BIT8
#define N_PCH_GPIO_TXDIS                8
#define V_PCH_GPIO_TXDIS_EN             0x00
#define V_PCH_GPIO_TXDIS_DIS            0x01

//GPIO RX State
#define B_PCH_GPIO_RX_STATE             BIT1
#define N_PCH_GPIO_RX_STATE             1
#define V_PCH_GPIO_RX_STATE_LOW         0x00
#define V_PCH_GPIO_RX_STATE_HIGH        0x01

//GPIO TX State
#define B_PCH_GPIO_TX_STATE             BIT0
#define N_PCH_GPIO_TX_STATE             0
#define V_PCH_GPIO_TX_STATE_LOW         0x00
#define V_PCH_GPIO_TX_STATE_HIGH        0x01

//
// Pad Configuration Register DW1
//

//Padtol
#define B_PCH_GPIO_PADTOL               BIT25
#define N_PCH_GPIO_PADTOL               25
#define V_PCH_GPIO_PADTOL_NONE          0x00
#define V_PCH_GPIO_PADTOL_CLEAR         0x00
#define V_PCH_GPIO_PADTOL_SET           0x01

//Termination
#define B_PCH_GPIO_TERM                (BIT13 | BIT12 | BIT11 | BIT10)
#define N_PCH_GPIO_TERM                 10
#define V_PCH_GPIO_TERM_WPD_NONE        0x00
#define V_PCH_GPIO_TERM_WPD_5K          0x02
#define V_PCH_GPIO_TERM_WPD_20K         0x04
#define V_PCH_GPIO_TERM_WPU_NONE        0x08
#define V_PCH_GPIO_TERM_WPU_1K          0x09
#define V_PCH_GPIO_TERM_WPU_2K          0x0B
#define V_PCH_GPIO_TERM_WPU_5K          0x0A
#define V_PCH_GPIO_TERM_WPU_20K         0x0C
#define V_PCH_GPIO_TERM_WPU_1K_2K       0x0D
#define V_PCH_GPIO_TERM_NATIVE          0x0F

//Interrupt number
#define B_PCH_GPIO_INTSEL               0x7F
#define N_PCH_GPIO_INTSEL               0

//
// Ownership
//
#define V_PCH_GPIO_OWN_GPIO             0x01
#define V_PCH_GPIO_OWN_ACPI             0x00

//
// GPE 
//
#define V_PCH_GPIO_GPE_EN               0x01
#define V_PCH_GPIO_GPE_DIS              0x00
//
// SMI
//
#define V_PCH_GPIO_SMI_EN               0x01
#define V_PCH_GPIO_SMI_DIS              0x00
//
// NMI
//
#define V_PCH_GPIO_NMI_EN               0x01
#define V_PCH_GPIO_NMI_DIS              0x00
//
// Reserved: RSVD1  
//
#define V_PCH_GPIO_RSVD1                0x00

#define PID_NorthCommunity 0xC2
#define PID_SouthCommunity 0xC5

//GPIO_SDP
#define GPIO_GBE0_SDP0				14
#define R_PAD_CFG_DW0_GBE0_SDP0			0x400
#define PID_GBE0_SDP0				PID_NorthCommunity

#define GPIO_GBE1_SDP0				15
#define R_PAD_CFG_DW0_GBE1_SDP0			0x408
#define PID_GBE1_SDP0				PID_NorthCommunity

#define GPIO_GBE0_SDP1				16
#define R_PAD_CFG_DW0_GBE0_SDP1			0x410
#define PID_GBE0_SDP1				PID_NorthCommunity

#define GPIO_GBE1_SDP1				17
#define R_PAD_CFG_DW0_GBE1_SDP1			0x418
#define PID_GBE1_SDP1				PID_NorthCommunity

#define GPIO_GBE0_SDP2				18
#define R_PAD_CFG_DW0_GBE0_SDP2			0x420
#define PID_GBE0_SDP2				PID_NorthCommunity

#define GPIO_GBE1_SDP2				19
#define R_PAD_CFG_DW0_GBE1_SDP2			0x428
#define PID_GBE1_SDP2				PID_NorthCommunity

#define GPIO_GBE0_SDP3				20
#define R_PAD_CFG_DW0_GBE0_SDP3			0x430
#define PID_GBE0_SDP3				PID_NorthCommunity

#define GPIO_GBE1_SDP3				21
#define R_PAD_CFG_DW0_GBE1_SDP3			0x438
#define PID_GBE1_SDP3				PID_NorthCommunity

#define GPIO_GBE2_LED0				22
#define R_PAD_CFG_DW0_GBE2_LED0			0x440
#define PID_GBE2_LED0				PID_NorthCommunity

#define GPIO_GBE2_LED1				23
#define R_PAD_CFG_DW0_GBE2_LED1			0x448
#define PID_GBE2_LED1				PID_NorthCommunity

#define GPIO_GBE0_I2C_CLK			24
#define R_PAD_CFG_DW0_GBE0_I2C_CLK		0x450
#define PID_GBE0_I2C_CLK			PID_NorthCommunity

#define GPIO_GBE0_I2C_DATA			25
#define R_PAD_CFG_DW0_GBE0_I2C_DATA		0x458
#define PID_GBE0_I2C_DATA			PID_NorthCommunity

#define GPIO_GBE1_I2C_CLK			26
#define R_PAD_CFG_DW0_GBE1_I2C_CLK		0x460
#define PID_GBE1_I2C_CLK			PID_NorthCommunity

#define GPIO_GBE1_I2C_DATA			27
#define R_PAD_CFG_DW0_GBE1_I2C_DATA		0x468
#define PID_GBE1_I2C_DATA			PID_NorthCommunity

//GPIO_LAN
#define GPIO_SMB5_GBE_DATA			28
#define R_PAD_CFG_DW0_SMB5_GBE_DATA		0x470
#define PID_SMB5_GBE_DATA			PID_NorthCommunity

#define GPIO_SMB5_GBE_CLK			29
#define R_PAD_CFG_DW0_SMB5_GBE_CLK		0x478
#define PID_SMB5_GBE_CLK			PID_NorthCommunity

#define GPIO_SMB5_GBE_ALRT_N    	30
#define R_PAD_CFG_DW0_SMB5_GBE_ALRT_N    	0x480
#define PID_SMB5_GBE_ALRT_N    	    PID_NorthCommunity

#define GPIO_NCSI_CRS_DV        	31
#define R_PAD_CFG_DW0_NCSI_CRS_DV        	0x488
#define PID_NCSI_CRS_DV        	    PID_NorthCommunity

#define GPIO_NCSI_ARB_IN      		32
#define R_PAD_CFG_DW0_NCSI_ARB_IN      	0x490
#define PID_NCSI_ARB_IN      		PID_NorthCommunity

#define GPIO_NCSI_TX_EN				33
#define R_PAD_CFG_DW0_NCSI_TX_EN			0x498
#define PID_NCSI_TX_EN				PID_NorthCommunity

#define GPIO_NCSI_TXD0				34
#define R_PAD_CFG_DW0_NCSI_TXD0			0x4a0
#define PID_NCSI_TXD0				PID_NorthCommunity

#define GPIO_NCSI_TXD1				35
#define R_PAD_CFG_DW0_NCSI_TXD1			0x4a8
#define PID_NCSI_TXD1				PID_NorthCommunity

#define GPIO_NCSI_ARB_OUT			36
#define R_PAD_CFG_DW0_NCSI_ARB_OUT		0x4b0
#define PID_NCSI_ARB_OUT			PID_NorthCommunity

#define GPIO_GBE0_LED0				37
#define R_PAD_CFG_DW0_GBE0_LED0			0x4b8
#define PID_GBE0_LED0				PID_NorthCommunity

#define GPIO_GBE0_LED1				38
#define R_PAD_CFG_DW0_GBE0_LED1			0x4c0
#define PID_GBE0_LED1				PID_NorthCommunity

#define GPIO_GBE1_LED0				39
#define R_PAD_CFG_DW0_GBE1_LED0			0x4c8
#define PID_GBE1_LED0				PID_NorthCommunity

#define GPIO_GBE1_LED1				40
#define R_PAD_CFG_DW0_GBE1_LED1			0x4d0
#define PID_GBE1_LED1				PID_NorthCommunity

#define GPIO_GPIO_0					0
#define R_PAD_CFG_DW0_GPIO_0				0x4d8
#define PID_GPIO_0					PID_NorthCommunity

//GPIO_PCIE
#define GPIO_PCIE_CLKREQ0_N			41
#define R_PAD_CFG_DW0_PCIE_CLKREQ0_N		0x4e0
#define PID_PCIE_CLKREQ0_N			PID_NorthCommunity

#define GPIO_PCIE_CLKREQ1_N			42
#define R_PAD_CFG_DW0_PCIE_CLKREQ1_N			0x4e8
#define PID_PCIE_CLKREQ1_N			PID_NorthCommunity

#define GPIO_PCIE_CLKREQ2_N			43
#define R_PAD_CFG_DW0_PCIE_CLKREQ2_N		0x4f0
#define PID_PCIE_CLKREQ2_N			PID_NorthCommunity

#define GPIO_PCIE_CLKREQ3_N			44
#define R_PAD_CFG_DW0_PCIE_CLKREQ3_N		0x4f8
#define PID_PCIE_CLKREQ3_N			PID_NorthCommunity

#define GPIO_PCIE_CLKREQ4_N			45
#define R_PAD_CFG_DW0_PCIE_CLKREQ4_N			0x500
#define PID_PCIE_CLKREQ4_N			PID_NorthCommunity

#define GPIO_GPIO_1					   1
#define R_PAD_CFG_DW0_GPIO_1					0x508
#define PID_GPIO_1					   PID_NorthCommunity

#define GPIO_GPIO_2					   2
#define R_PAD_CFG_DW0_GPIO_2					0x510
#define PID_GPIO_2					   PID_NorthCommunity

//GPIO_SVID
#define GPIO_SVID_ALERT_N			   47
#define R_PAD_CFG_DW0_SVID_ALERT_N			   0x518
#define PID_SVID_ALERT_N			   PID_NorthCommunity

#define GPIO_SVID_DATA				    48
#define R_PAD_CFG_DW0_SVID_DATA				0x520
#define PID_SVID_DATA				    PID_NorthCommunity

#define GPIO_SVID_CLK				   49
#define R_PAD_CFG_DW0_SVID_CLK				0x528
#define PID_SVID_CLK				   PID_NorthCommunity

//GPIO_THERMAL
#define GPIO_THERMTRIP_N			50
#define R_PAD_CFG_DW0_THERMTRIP_N			0x530
#define PID_THERMTRIP_N				PID_NorthCommunity

#define GPIO_PROCHOT_N				51
#define R_PAD_CFG_DW0_PROCHOT_N			0x538
#define PID_PROCHOT_N				PID_NorthCommunity

#define GPIO_MEMHOT_N				52
#define R_PAD_CFG_DW0_MEMHOT_N			0x540
#define PID_MEMHOT_N				PID_NorthCommunity


//GPIO_DFX
#define GPIO_DFX_PORT_CLK0			53
#define R_PAD_CFG_DW0_DFX_PORT_CLK0		0x400
#define PID_DFX_PORT_CLK0			PID_SouthCommunity

#define GPIO_DFX_PORT_CLK1			54
#define R_PAD_CFG_DW0_DFX_PORT_CLK1		0x408
#define PID_DFX_PORT_CLK1			PID_SouthCommunity

#define GPIO_DFX_PORT0				55
#define R_PAD_CFG_DW0_DFX_PORT0			0x410
#define PID_DFX_PORT0				PID_SouthCommunity

#define GPIO_DFX_PORT1				56
#define R_PAD_CFG_DW0_DFX_PORT1			0x418
#define PID_DFX_PORT1				PID_SouthCommunity

#define GPIO_DFX_PORT2				57
#define R_PAD_CFG_DW0_DFX_PORT2			0x420
#define PID_DFX_PORT2				PID_SouthCommunity

#define GPIO_DFX_PORT3				58
#define R_PAD_CFG_DW0_DFX_PORT3			0x428
#define PID_DFX_PORT3				PID_SouthCommunity

#define GPIO_DFX_PORT4				59
#define R_PAD_CFG_DW0_DFX_PORT4			0x430
#define PID_DFX_PORT4				PID_SouthCommunity

#define GPIO_DFX_PORT5				60
#define R_PAD_CFG_DW0_DFX_PORT5			0x438
#define PID_DFX_PORT5				PID_SouthCommunity

#define GPIO_DFX_PORT6				61
#define R_PAD_CFG_DW0_DFX_PORT6			0x440
#define PID_DFX_PORT6				PID_SouthCommunity

#define GPIO_DFX_PORT7				62
#define R_PAD_CFG_DW0_DFX_PORT7			0x448
#define PID_DFX_PORT7				PID_SouthCommunity

#define GPIO_DFX_PORT8				63
#define R_PAD_CFG_DW0_DFX_PORT8			0x450
#define PID_DFX_PORT8				PID_SouthCommunity

#define GPIO_DFX_PORT9				134
#define R_PAD_CFG_DW0_DFX_PORT9			0x458
#define PID_DFX_PORT9				PID_SouthCommunity

#define GPIO_DFX_PORT10				135
#define R_PAD_CFG_DW0_DFX_PORT10			0x460
#define PID_DFX_PORT10				PID_SouthCommunity

#define GPIO_DFX_PORT11				136
#define R_PAD_CFG_DW0_DFX_PORT11			0x468
#define PID_DFX_PORT11				PID_SouthCommunity

#define GPIO_DFX_PORT12				137
#define R_PAD_CFG_DW0_DFX_PORT12			0x470
#define PID_DFX_PORT12				PID_SouthCommunity

#define GPIO_DFX_PORT13				138
#define R_PAD_CFG_DW0_DFX_PORT13			0x478
#define PID_DFX_PORT13				PID_SouthCommunity

#define GPIO_DFX_PORT14				139
#define R_PAD_CFG_DW0_DFX_PORT14			0x480
#define PID_DFX_PORT14				PID_SouthCommunity

#define GPIO_DFX_PORT15				140
#define R_PAD_CFG_DW0_DFX_PORT15			0x488
#define PID_DFX_PORT15				PID_SouthCommunity

//GPIO_MISC0
#define GPIO_SMB3_CLTT_DATA			12
#define R_PAD_CFG_DW0_SMB3_CLTT_DATA		0x490
#define PID_SMB3_CLTT_DATA			PID_SouthCommunity

#define GPIO_SMB3_CLTT_CLK			13
#define R_PAD_CFG_DW0_SMB3_CLTT_CLK		0x498
#define PID_SMB3_CLTT_CLK			PID_SouthCommunity

#define GPIO_PCIE_CLKREQ5_N			98
#define R_PAD_CFG_DW0_PCIE_CLKREQ5_N		0x4A0
#define PID_PCIE_CLKREQ5_N			PID_SouthCommunity

#define GPIO_PCIE_CLKREQ6_N			99
#define R_PAD_CFG_DW0_PCIE_CLKREQ6_N		0x4a8
#define PID_PCIE_CLKREQ6_N			PID_SouthCommunity

#define GPIO_PCIE_CLKREQ7_N			100
#define R_PAD_CFG_DW0_PCIE_CLKREQ7_N		0x4b0
#define PID_PCIE_CLKREQ7_N			PID_SouthCommunity

#define GPIO_UART0_RXD				101
#define R_PAD_CFG_DW0_UART0_RXD			0x4b8
#define PID_UART0_RXD				PID_SouthCommunity

#define GPIO_UART0_TXD				102
#define R_PAD_CFG_DW0_UART0_TXD			0x4c0
#define PID_UART0_TXD				PID_SouthCommunity

//GPIO_MISC1
#define GPIO_CPU_RESET_N			103
#define R_PAD_CFG_DW0_CPU_RESET_N			0x4c8
#define PID_CPU_RESET_N				PID_SouthCommunity

#define GPIO_NMI					104
#define R_PAD_CFG_DW0_NMI					0x4d0
#define PID_NMI						PID_SouthCommunity

#define GPIO_ERROR2_N				105
#define R_PAD_CFG_DW0_ERROR2_N			0x4d8
#define PID_ERROR2_N				PID_SouthCommunity

#define GPIO_ERROR1_N				106
#define R_PAD_CFG_DW0_ERROR1_N			0x4e0
#define PID_ERROR1_N				PID_SouthCommunity

#define GPIO_ERROR0_N				107
#define R_PAD_CFG_DW0_ERROR0_N			0x4e8
#define PID_ERROR0_N				PID_SouthCommunity

#define GPIO_IERR_N					108
#define R_PAD_CFG_DW0_IERR_N				0x4f0
#define PID_IERR_N					PID_SouthCommunity

#define GPIO_MCERR_N				109
#define R_PAD_CFG_DW0_MCERR_N				0x4f8
#define PID_MCERR_N					PID_SouthCommunity

#define GPIO_SMB0_LEG_CLK			110
#define R_PAD_CFG_DW0_SMB0_LEG_CLK		0x500
#define PID_SMB0_LEG_CLK			PID_SouthCommunity

#define GPIO_SMB0_LEG_DATA			111
#define R_PAD_CFG_DW0_SMB0_LEG_DATA		0x508
#define PID_SMB0_LEG_DATA			PID_SouthCommunity

#define GPIO_SMB0_LEG_ALRT_N		112
#define R_PAD_CFG_DW0_SMB0_LEG_ALRT_N		0x510
#define PID_SMB0_LEG_ALRT_N			PID_SouthCommunity

//GPIO_MISC2
#define GPIO_SMB1_HOST_DATA			113
#define R_PAD_CFG_DW0_SMB1_HOST_DATA		0x518
#define PID_SMB1_HOST_DATA			PID_SouthCommunity

#define GPIO_SMB1_HOST_CLK			114
#define R_PAD_CFG_DW0_SMB1_HOST_CLK		0x520
#define PID_SMB1_HOST_CLK			PID_SouthCommunity

#define GPIO_SMB2_PECI_DATA			115
#define R_PAD_CFG_DW0_SMB2_PECI_DATA		0x528
#define PID_SMB2_PECI_DATA			PID_SouthCommunity

#define GPIO_SMB2_PECI_CLK			116
#define R_PAD_CFG_DW0_SMB2_PECI_CLK		0x530
#define PID_SMB2_PECI_CLK			PID_SouthCommunity

#define GPIO_SMB4_CSME0_DATA		117
#define R_PAD_CFG_DW0_SMB4_CSME0_DATA		0x538
#define PID_SMB4_CSME0_DATA			PID_SouthCommunity

#define GPIO_SMB4_CSME0_CLK			118
#define R_PAD_CFG_DW0_SMB4_CSME0_CLK		0x540
#define PID_SMB4_CSME0_CLK			PID_SouthCommunity

#define GPIO_SMB4_CSME0_ALRT_N		119
#define R_PAD_CFG_DW0_SMB4_CSME0_ALRT_N	0x548
#define PID_SMB4_CSME0_ALRT_N		PID_SouthCommunity

#define GPIO_USB_OC0_N				120
#define R_PAD_CFG_DW0_USB_OC0_N			0x550
#define PID_USB_OC0_N				PID_SouthCommunity

#define GPIO_FLEX_CLK_SE0			121
#define R_PAD_CFG_DW0_FLEX_CLK_SE0		0x558
#define PID_FLEX_CLK_SE0			PID_SouthCommunity

#define GPIO_FLEX_CLK_SE1			122
#define R_PAD_CFG_DW0_FLEX_CLK_SE1		0x560
#define PID_FLEX_CLK_SE1			PID_SouthCommunity

#define GPIO_GPIO_4					4
#define R_PAD_CFG_DW0_GPIO_4				0x568
#define PID_GPIO_4					PID_SouthCommunity

#define GPIO_GPIO_5					5
#define R_PAD_CFG_DW0_GPIO_5				0x570
#define PID_GPIO_5					PID_SouthCommunity

#define GPIO_GPIO_6					6
#define R_PAD_CFG_DW0_GPIO_6				0x578
#define PID_GPIO_6					PID_SouthCommunity

#define GPIO_GPIO_7					7
#define R_PAD_CFG_DW0_GPIO_7				0x580
#define PID_GPIO_7					PID_SouthCommunity

//GPIO_SATA
#define GPIO_SATA0_LED_N			90
#define R_PAD_CFG_DW0_SATA0_LED_N			0x588
#define PID_SATA0_LED_N				PID_SouthCommunity

#define GPIO_SATA1_LED_N			91
#define R_PAD_CFG_DW0_SATA1_LED_N			0x590
#define PID_SATA1_LED_N				PID_SouthCommunity

#define GPIO_SATA_PDETECT0			92
#define R_PAD_CFG_DW0_SATA_PDETECT0		0x598
#define PID_SATA_PDETECT0			PID_SouthCommunity

#define GPIO_SATA_PDETECT1			93
#define R_PAD_CFG_DW0_SATA_PDETECT1		0x5A0
#define PID_SATA_PDETECT1			PID_SouthCommunity

#define GPIO_SATA0_SDOUT			94
#define R_PAD_CFG_DW0_SATA0_SDOUT			0x5A8
#define PID_SATA0_SDOUT				PID_SouthCommunity

#define GPIO_SATA1_SDOUT			95
#define R_PAD_CFG_DW0_SATA1_SDOUT			0x5B0
#define PID_SATA1_SDOUT				PID_SouthCommunity

#define GPIO_UART1_RXD				96
#define R_PAD_CFG_DW0_UART1_RXD			0x5B8
#define PID_UART1_RXD				PID_SouthCommunity

#define GPIO_UART1_TXD				97
#define R_PAD_CFG_DW0_UART1_TXD			0x5C0
#define PID_UART1_TXD				PID_SouthCommunity

#define GPIO_GPIO_8					8
#define R_PAD_CFG_DW0_GPIO_8				0x5C8
#define PID_GPIO_8					PID_SouthCommunity

#define GPIO_GPIO_9					9
#define R_PAD_CFG_DW0_GPIO_9				0x5D0
#define PID_GPIO_9					PID_SouthCommunity

//#define GPIO_GPIO_JTAG_ITP
#define GPIO_TCK					141
#define R_PAD_CFG_DW0_TCK					0x5D8
#define PID_TCK						PID_SouthCommunity

#define GPIO_TRST_N					142
#define R_PAD_CFG_DW0_TRST_N				0x5E0
#define PID_TRST_N					PID_SouthCommunity

#define GPIO_TMS					143
#define R_PAD_CFG_DW0_TMS					0x5E8
#define PID_TMS						PID_SouthCommunity

#define GPIO_TDI					144
#define R_PAD_CFG_DW0_TDI					0x5F0
#define PID_TDI						PID_SouthCommunity

#define GPIO_TDO					145
#define R_PAD_CFG_DW0_TDO					0x5F8
#define PID_TDO						PID_SouthCommunity

#define GPIO_CX_PRDY_N				146
#define R_PAD_CFG_DW0_CX_PRDY_N			0x600
#define PID_CX_PRDY_N				PID_SouthCommunity

#define GPIO_CX_PREQ_N				147
#define R_PAD_CFG_DW0_CX_PREQ_N			0x608
#define PID_CX_PREQ_N				PID_SouthCommunity

#define GPIO_CTBTRIGINOUT			148
#define R_PAD_CFG_DW0_CTBTRIGINOUT		0x610
#define PID_CTBTRIGINOUT			PID_SouthCommunity

#define GPIO_CTBTRIGOUT				149
#define R_PAD_CFG_DW0_CTBTRIGOUT			0x618
#define PID_CTBTRIGOUT				PID_SouthCommunity

#define GPIO_TAP1_TMS				150
#define R_PAD_CFG_DW0_TAP1_TMS			0x620
#define PID_TAP1_TMS				PID_SouthCommunity

#define GPIO_TAP1_TDI				151
#define R_PAD_CFG_DW0_TAP1_TDI			0x628
#define PID_TAP1_TDI				PID_SouthCommunity

#define GPIO_TAP1_TDO				152
#define R_PAD_CFG_DW0_TAP1_TDO			0x630
#define PID_TAP1_TDO				PID_SouthCommunity

//#define GPIO_GPIO_PMC
#define GPIO_SUSPWRDNACK			79
#define R_PAD_CFG_DW0_SUSPWRDNACK			0x638
#define PID_SUSPWRDNACK				PID_SouthCommunity

#define GPIO_PMU_SUSCLK				80
#define R_PAD_CFG_DW0_PMU_SUSCLK			0x640
#define PID_PMU_SUSCLK				PID_SouthCommunity
#define GPIO_PMU_SUSCLK				80

#define GPIO_PMU_AC_PRESENT			81
#define R_PAD_CFG_DW0_PMU_AC_PRESENT		0x648
#define PID_PMU_AC_PRESENT			PID_SouthCommunity

#define GPIO_PMU_SLP_S45_N			82
#define R_PAD_CFG_DW0_PMU_SLP_S45_N		0x650
#define PID_PMU_SLP_S45_N			PID_SouthCommunity

#define GPIO_PMU_SLP_S3_N			83
#define R_PAD_CFG_DW0_PMU_SLP_S3_N		0x658
#define PID_PMU_SLP_S3_N			PID_SouthCommunity

#define GPIO_PMU_WAKE_N				84
#define R_PAD_CFG_DW0_PMU_WAKE_N			0x660
#define PID_PMU_WAKE_N				PID_SouthCommunity

#define GPIO_PMU_PWRBTN_N			85
#define R_PAD_CFG_DW0_PMU_PWRBTN_N		0x668
#define PID_PMU_PWRBTN_N			PID_SouthCommunity

#define GPIO_PMU_RESETBUTTON_N		86
#define R_PAD_CFG_DW0_PMU_RESETBUTTON_N	0x670
#define PID_PMU_RESETBUTTON_N		PID_SouthCommunity

#define GPIO_PMU_PLTRST_N			87
#define R_PAD_CFG_DW0_PMU_PLTRST_N		0x678
#define PID_PMU_PLTRST_N			PID_SouthCommunity

#define GPIO_SUS_STAT_N				88
#define R_PAD_CFG_DW0_SUS_STAT_N			0x680
#define PID_SUS_STAT_N				PID_SouthCommunity

#define GPIO_SLP_S0IX_N				89
#define R_PAD_CFG_DW0_SLP_S0IX_N			0x688
#define PID_SLP_S0IX_N				PID_SouthCommunity

//GPIO_SPI
#define GPIO_SPI_CS0_N				72
#define R_PAD_CFG_DW0_SPI_CS0_N			0x690
#define PID_SPI_CS0_N				PID_SouthCommunity

#define GPIO_SPI_CS1_N				73
#define R_PAD_CFG_DW0_SPI_CS1_N			0x698
#define PID_SPI_CS1_N				PID_SouthCommunity

#define GPIO_SPI_MOSI_IO0			74
#define R_PAD_CFG_DW0_SPI_MOSI_IO0		0x6A0
#define PID_SPI_MOSI_IO0			PID_SouthCommunity

#define GPIO_SPI_MISO_IO1			75
#define R_PAD_CFG_DW0_SPI_MISO_IO1		0x6A8
#define PID_SPI_MISO_IO1			PID_SouthCommunity

#define GPIO_SPI_IO2				76
#define R_PAD_CFG_DW0_SPI_IO2				0x6B0
#define PID_SPI_IO2					PID_SouthCommunity

#define GPIO_SPI_IO3				77
#define R_PAD_CFG_DW0_SPI_IO3				0x6B8
#define PID_SPI_IO3					PID_SouthCommunity

#define GPIO_SPI_CLK				78
#define R_PAD_CFG_DW0_SPI_CLK				0x6C0
#define PID_SPI_CLK					PID_SouthCommunity


#define GPIO_SPI_CLK_LOOPBK			0xFFD
#define PID_SPI_CLK_LOOPBK		    PID_SouthCommunity
#define R_PAD_CFG_DW0_SPI_CLK_LOOPBK		0x6C8


//GPIO_ESPI_LPC
#define GPIO_ESPI_IO0				64
#define R_PAD_CFG_DW0_ESPI_IO0			0x6D0
#define PID_ESPI_IO0				PID_SouthCommunity

#define GPIO_ESPI_IO1				65
#define R_PAD_CFG_DW0_ESPI_IO1			0x6D8
#define PID_ESPI_IO1				PID_SouthCommunity

#define GPIO_ESPI_IO2				66
#define R_PAD_CFG_DW0_ESPI_IO2			0x6E0
#define PID_ESPI_IO2				PID_SouthCommunity

#define GPIO_ESPI_IO3				67
#define R_PAD_CFG_DW0_ESPI_IO3			0x6E8
#define PID_ESPI_IO3				PID_SouthCommunity

#define GPIO_ESPI_CS0_N				68
#define R_PAD_CFG_DW0_ESPI_CS0_N			0x6F0
#define PID_ESPI_CS0_N				PID_SouthCommunity

#define GPIO_ESPI_CLK				69
#define R_PAD_CFG_DW0_ESPI_CLK			0x6F8
#define PID_ESPI_CLK				PID_SouthCommunity

#define GPIO_ESPI_RST_N				70
#define R_PAD_CFG_DW0_ESPI_RST_N			0x700
#define PID_ESPI_RST_N				PID_SouthCommunity

#define GPIO_ESPI_ALRT0_N			71
#define R_PAD_CFG_DW0_ESPI_ALRT0_N		0x708
#define PID_ESPI_ALRT0_N			PID_SouthCommunity

#define GPIO_ESPI_CS1_N   			10
#define R_PAD_CFG_DW0_ESPI_CS1_N   		0x710
#define PID_ESPI_CS1_N   			PID_SouthCommunity

#define GPIO_ESPI_ALRT1_N			11
#define R_PAD_CFG_DW0_ESPI_ALRT1_N		0x718
#define PID_ESPI_ALRT1_N			PID_SouthCommunity

#define GPIO_ESPI_CLK_LOOPBK 		0xffe
#define PID_ESPI_CLK_LOOPBK			PID_SouthCommunity
#define R_PAD_CFG_DW0_ESPI_CLK_LOOPBK		0x720
//EMMC
#define GPIO_EMMC_CMD				123
#define R_PAD_CFG_DW0_EMMC_CMD			0x728
#define PID_EMMC_CMD				PID_SouthCommunity

#define GPIO_EMMC_STROBE			124
#define R_PAD_CFG_DW0_EMMC_STROBE			0x730
#define PID_EMMC_STROBE				PID_SouthCommunity

#define GPIO_EMMC_CLK				125
#define R_PAD_CFG_DW0_EMMC_CLK			0x738
#define PID_EMMC_CLK				PID_SouthCommunity

#define GPIO_EMMC_D0				126
#define R_PAD_CFG_DW0_EMMC_D0				0x740
#define PID_EMMC_D0					PID_SouthCommunity

#define GPIO_EMMC_D1				127
#define R_PAD_CFG_DW0_EMMC_D1				0x748
#define PID_EMMC_D1					PID_SouthCommunity

#define GPIO_EMMC_D2				128
#define R_PAD_CFG_DW0_EMMC_D2				0x750
#define PID_EMMC_D2					PID_SouthCommunity

#define GPIO_EMMC_D3				129
#define R_PAD_CFG_DW0_EMMC_D3				0x758
#define PID_EMMC_D3					PID_SouthCommunity

#define GPIO_EMMC_D4				130
#define R_PAD_CFG_DW0_EMMC_D4				0x760
#define PID_EMMC_D4					PID_SouthCommunity

#define GPIO_EMMC_D5				131
#define R_PAD_CFG_DW0_EMMC_D5				0x768
#define PID_EMMC_D5					PID_SouthCommunity

#define GPIO_EMMC_D6				132
#define R_PAD_CFG_DW0_EMMC_D6				0x770
#define PID_EMMC_D6					PID_SouthCommunity

#define GPIO_EMMC_D7				133
#define R_PAD_CFG_DW0_EMMC_D7				0x778
#define PID_EMMC_D7					PID_SouthCommunity

#define GPIO_GPIO_3					3
#define R_PAD_CFG_DW0_GPIO_3				0x780
#define PID_GPIO_3					PID_SouthCommunity

#endif
