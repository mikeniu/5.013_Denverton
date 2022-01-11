/*++
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
--*/

/*++

Copyright (c)  2015 - 2017 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  PcieIpRegs.h

Abstract:

  Driver for Pcie IP Register definitions.

  Conventions:

  - Prefixes:
    Definitions beginning with "R_" are registers
    Definitions beginning with "B_" are bits within registers
    Definitions beginning with "V_" are meaningful values of bits within the registers
    Definitions beginning with "S_" are register sizes
    Definitions beginning with "N_" are the bit position
  - In general, PCIE registers are denoted by "_PCIE_" in register names
  - Registers / bits that are different between PCIE generations are denoted by
    "_PCIE_<generation_name>_" in register/bit names. e.g., "_PCIE_LPT_"
  - Registers / bits that are different between SKUs are denoted by "_<SKU_name>"
    at the end of the register/bit names
  - Registers / bits of new devices introduced in a PCIE generation will be just named
    as "_PCIE_" without <generation_name> inserted.

Revision History

--*/

#ifndef _PCIE_IP_REGS_H_
#define _PCIE_IP_REGS_H_

//
// PCIE code errors
//
#define V_ERROR_INVALID_ROOT_PORT_NUMBER        0
#define V_ERROR_PCI_DEVICE_IS_NOT_PRESENT       (~0)

#define R_PCIE_VENDOR_ID                        0x00
#define V_PCIE_NO_LANE_REVERSAL                 0
#define V_PCIE_LCTL2_DEEMPHASYS_6_DB_VALUE      0
#define V_PCIE_RELAXED_ORDERING_DEFAULT         0
#define V_PCIE_REVERSEVAL_DEFAULT               0
#define V_PCIE_PMSCI_DEFAULT                    0
#define V_PCIE_NO_SNOOP_DEFAULT                 0
#define V_PCIE_EXTENDED_SYNCH_DEFAULT           0
#define V_PCIE_EXT_TAG_FIELD_DEFAULT            0
#define V_PCIE_HOT_PLUG_DEFAULT                 0
#define V_PCIE_ASPM_L1_DEFAULT                  0
#define V_PCIE_UNSUPPORTED_REQUEST_DEFAULT      0
#define V_PCIE_LTR_ENABLE_DEFAULT               1
#define V_PCIE_DE_EMPHASIS_DEFAULT              0
#define V_PCIE_CLOCK_GATING_ENABLE_DEFAULT      0
#define V_PCIE_ERROR_INJECTION_DEFAULT          0
#define V_PCIE_DISABLE_MPHY_DEFAULT             0
#define V_PCIE_BIFURCATION_DEFAULT              0x02				//< default bifurcation: X2X2X2X2
#define V_PCIE_STRAP_DEFAULT                    0xAA000F			//< default strap value
#define V_PCIE_BIFURCATION_AUTO_OPTION          0xFF				//< value in setup option for detect this with trident mechanims
#define V_PCIE_REPLY_TIMER_TIMEOUT_DEFAULT      0x03
#define V_PCIE_COMPLETION_TIMEOUT_DEFAULT       0
#define V_PCIE_COMPLETION_TIMEOUT_RANGE_DEFAULT 0x0
#define V_PCIE_LCTL2_DEEMPHASYS_3P5_DB_VALUE    1
#define V_PCIE_LANE_REVERSAL                    1
#define V_PCIE_MAX_PAYLOAD_SIZE_DEFAULT			0x00
#define V_PCIE_DYNAMIC_LANE_REVERSAL            3
#define V_PCIE_MAXIMUM_LINK_SPEED_DEFAULT       3
#define N_PCIE_DCTL_MPS                         5
#define V_PCIE_MAX_READ_REQUEST_SIZE_DEFAULT    0x02
#define V_PCIE_SURPRISE_LINK_DEFAULT            0x1
#define N_PCIE_DCTL_ENS                         11
#define B_PCIE_DCTL_ETFE                        8    ///< Extended Tag Field Enable
#define B_PCIE_DCTL_ENRO                        4    ///< Enable Relaxed Ordering

#define R_PCIE_DCTL                             0x48
#define B_PCIE_DCTL_MRRS                        0x7000
#define B_PCIE_DCTL_MPS                         (BIT7 | BIT6 | BIT5)
#define B_PCIE_DCTL_ENS                         BIT11
#define V_PCIE_LOCK_DEFAULT                     0
#define V_PCIE_STOP_SCREAM_DEFAULT              0
#define V_PCIE_LANE_REVERSAL_DEFAULT            0

#define R_PCIE_LCTL                             0x50
#define B_PCIE_LCTL_ES                          BIT7

#define R_PCIE_DCTL2                            0x68
#define B_PCIE_DCTL2_CTD                        BIT4 ///< Completion Timeout Disable
#define B_PCIE_DCTL2_CTV                        (BIT3 | BIT2 | BIT1 | BIT0) ///< Completion Timeout Value
#define V_PCIE_DCTL2_CTV_DEFAULT                0x0
#define V_PCIE_DCTL2_CTV_50US_50MS              0x0
#define V_PCIE_DCTL2_CTV_50US_100US             0x1
#define V_PCIE_DCTL2_CTV_1MS_10MS               0x2
#define V_PCIE_DCTL2_CTV_16MS_55MS              0x5
#define V_PCIE_DCTL2_CTV_65MS_210MS             0x6
#define V_PCIE_DCTL2_CTV_260MS_900MS            0x9
#define V_PCIE_DCTL2_CTV_1S_3_5S                0xA
#define V_PCIE_DCTL2_CTV_4S_13S                 0xD
#define V_PCIE_DCTL2_CTV_17S_64S                0xE

#define R_PCIE_LCTL2                            0x70
#define B_PCIE_LCTL2_TLS                        (BIT3 | BIT2 | BIT1 | BIT0)
#define R_PCIE_PPD0                             0xD4
#define B_PCIE_PPD0_ILINKTRN0                   BIT3

#define R_PCIE_EBERMSK                          0x000B78
#define V_PCIE_EBERMSK                          0xFF

#define R_PCIE_SYMLCKCTL                        0xA00
#define V_PCIE_SYMLCKCTL                        0x1

//
// PPBIFCTL
//
#define R_PCIE_PCR_PPBIFCTLBIF                  0xC0
#define B_PCIE_PCR_PPBIFCTLBIF_CL_LOCK          BIT31
#define B_PCIE_PCR_PPBIFCTLBIF_BIFCTL0          (BIT0|BIT1|BIT2)

//
// PPBIFCTL_PRIV
//
#define R_PCIE_PPBIFCTL_PRIV                    0x10
#define R_PCIE_PPBIFCTL_PRIV_X2X2X2X2           0
#define R_PCIE_PPBIFCTL_PRIV_X2X2X4             0x001
#define R_PCIE_PPBIFCTL_PRIV_X4X2X2             0x010
#define R_PCIE_PPBIFCTL_PRIV_X4X4               0x011
#define R_PCIE_PPBIFCTL_PRIV_X8                 0x100
#define R_PCIE_PPBIFCTL_PRIV_CL                 BIT31
#define R_PCIE_EXPPTMBAR                        0x10
#define R_PCIE_COMMAND                          0x04

//
// PCIE_DEVHIDE_PRIV
//
#define R_PCIE_DEVHIDE_PRIV                    0x38
#define B_PCIE_DEVHIDE_X8                      (BIT0)
#define B_PCIE_DEVHIDE_X4X4                    (BIT0|BIT2)
#define B_PCIE_DEVHIDE_X4X2X2                  (BIT0|BIT1|BIT2)
#define B_PCIE_DEVHIDE_X2X2X4                  (BIT0|BIT2|BIT3)
#define B_PCIE_DEVHIDE_DHCL                    BIT31
#define B_PCIE_DEVHICE_ALL_RP_ACTIVE           (BIT0|BIT1|BIT2|BIT3)
#define B_PCIE_DEVHIDE_PCFGHP                  BIT4

//
// PCIE_PSF   block register for bifurcation
//
#define R_PCIE_PSF1                             0xA9
#define R_PCIE_PSF0                             0x24
#define R_PCIE_PSF1_RP0_OFFSET32                0x203C
#define R_PCIE_PSF1_RP0_OFFSET33                0x213C
#define R_PCIE_PSF1_RP0_OFFSET34                0x223C
#define R_PCIE_PSF1_RP0_OFFSET35                0x233C

#define R_PCIE_PSF1_RP1_OFFSET37                0x253C
#define R_PCIE_PSF1_RP1_OFFSET38                0x263C
#define R_PCIE_PSF1_RP1_OFFSET39                0x273C
#define R_PCIE_PSF1_RP1_OFFSET40                0x283C

#define B_PCIE_PSF_FUNDIS                      BIT8

//
// PCIE_PSF   RC OWNER RS0
//
#define R_PCIE_PSF1_RC_OWNER_RS0_OFFSET        0x4008
#define R_PCIE_PSF0_RC_OWNER_RS0_OFFSET        0x4008
#define V_PCIE_PSF1_RC_OWNER_RS0               0x40101
#define V_PCIE_PSF0_RC_OWNER_RS0               0x40101

//
// PCIE_SOFTSTRAPs
//
#define R_PCIE_STRAPCSR1                       0x08
#define R_PCIE_STRAPCSR2                       0x0C

///
/// INTXSWZCTL
///
#define R_PCIE_INTXSWZCTL                       0xF8
#define V_PCIE_INTXSWZCTL_SWIZZLE_MASK          (~0x7)
#define V_PCIE_INTXSWZCTL_INTA_INTA             0x0
#define V_PCIE_INTXSWZCTL_INTA_INTB             0x1
#define V_PCIE_INTXSWZCTL_INTA_INTC             0x2
#define V_PCIE_INTXSWZCTL_INTA_INTD             0x3

//
// Bifurcation
//
#define VALID_NUMBER_BIFURCATION_MODES 			5
#define BIFURCATION_METHOD_AUTOMATIC 			0xff

///
/// ICLK
///
#define R_PCIE_PCR_ICLK_DWORD2                  0x208
#define B_PCIE_1_PLLEN_HANDOFF                  BIT5
#define B_PCIE_1_PLLG2_EN                       BIT4
#define B_PCIE_1_PLLG3_EN                       BIT3
#define B_PCIE_0_PLLEN_HANDOFF                  BIT2
#define B_PCIE_0_PLLG2_EN                       BIT1
#define B_PCIE_0_PLLG3_EN                       BIT0

///
/// Clock Gating
///
#define R_PCIE_IOSFDEVCLKGCTL                   0x2E4
#define B_PCIE_IDLE_CLOCK_GATING_ENABLE         BIT15

#define R_PCIE_SBDEVCLKGCTL                     0x2E6
#define R_PCIE_IDFDEVCLKGCTL                    0x2E8
#define R_PCIE_PT0TXNCLKGCTL                    0x8E4
#define R_PCIE_PT1TXNCLKGCTL                    0x8E6
#define R_PCIE_PT2TXNCLKGCTL                    0x8E8
#define R_PCIE_PT3TXNCLKGCTL                    0x8EA
#define R_PCIE_PT0LNKCLKGCTL                    0x9E4
#define R_PCIE_PT1LNKCLKGCTL                    0x9E6
#define R_PCIE_PT2LNKCLKGCTL                    0x9E8
#define R_PCIE_PT3LNKCLKGCTL                    0x9EA
#define R_PCIE_CFGPCIECLKGCTL                   0x9EC
#define V_PCIE_OFFSET_A30H_LTSSMTRNCTL          0xA30
#define R_PCIE_PIPECTL2                         0xA5C
#define B_PCIE_PIPECTL2_L1P0_PSMAP              ( BIT4 | BIT5 | BIT6 )
#define B_PCIE_PIPECTL2_L1P0_PSMAP_POSITION     0x04
#define R_PCIE_PT0PHYCLKGCTL                    0xAE4
#define R_PCIE_PT1PHYCLKGCTL                    0xAE6
#define R_PCIE_PT2PHYCLKGCTL                    0xAE8
#define R_PCIE_PT3PHYCLKGCTL                    0xAEA
#define R_PCIE_PLLSTBYCTL                       0xBAC
#define B_PCIE_PLL_STANDBY_ENABLE               BIT0

//
// PS states to place PIPE interface into for L1.0.
//
#define PIPE_PS_STATE_2                         0x02

//
// PCIE Root port RTRYCTL register
//
#define R_PCIE_RTRYCTL								0x988 										///< RTRYCTL register offset
#define B_PCIE_RTRYCTL_RTTO							(BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0)  	///< Replay timer timeout start bit

///
/// The PCH PCI registers
///
#define DEFAULT_PCI_BUS_NUMBER_PCH              0
#define PCI_DEVICE_NUMBER_PCH_PMC               31
#define PCI_FUNCTION_NUMBER_PCH_PMC             2
#define R_PCH_PMC_PWRM_BASE                     0x48
#define B_PCH_PMC_PWRM_BASE_BAR                 0xFFFFF000                    ///< PWRM Base Bar is 4K aligned
#define R_PCH_PWRM_STPG_FUSE_SS_DIS_RD_2        0x644 ///< Fuse Disable Read 2 Register
#define B_PCH_PWRM_PCIE1_FUSE_SS_DIS            BIT5
#define  B_PCH_PWRM_PCIE0_FUSE_SS_DIS           BIT4
#define R_PCH_PMC_MTPMC_OFFSET                  0x20
#define R_PCH_PMC_MFPMC_OFFSET                  0x38
#define R_PCH_PMC_GPE0_EN_OFFSET                0x9C
#define R_PCH_PM_STS_OFFSET                     0x1C


///
/// The Phy Recal for PCIE root ports
///
#define R_PCH_PCIE_PHYRECAL                     0xBA8

//
// PCIE gen 3 configuration
//
#define R_PCIE_EQEVALCTL4                       0xBB8    //0x31B8
#define B_PCIE_EQEVALCTL4_FOMSEARCH_EN          BIT17
#define B_PCIE_EQEVALCTL4_RXHINTCOEFF_EN        BIT18
#define R_PCIE_EQEVALCTL2                       0xBD4    //0x31D4
#define V_PCIE_EQEVALCTL2_EVALTIME_VALUE        0xD8
#define V_PCIE_EQEVALCTL2_EVALATT_VALUE         0xFF
#define R_PCIE_EQEVALCTL3                       0xBD8        //0x31D8
#define B_PCIE_EQEVALCTL3_ERRTHRESH1            (BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0)
#define B_PCIE_EQEVALCTL3_ERRTHRESH0            (BIT15|BIT14|BIT13|BIT12|BIT11|BIT10|BIT9|BIT8)
#define R_PCIE_EQEVALCTL7                       0xB90     //0x3190
#define B_PCIE_EQEVALCTL7_FOMSTPLIM             0x20

#define R_PCIE_GEN3FARTXCOEFF0                  0x35C0
#define R_PCIE_GEN3FARTXCOEFF1                  0x35C4
#define R_PCIE_GEN3FARTXCOEFF2                  0x35C8
#define R_PCIE_GEN3FARTXCOEFF3                  0x35CC
#define R_PCIE_GEN3FARTXCOEFF4                  0x35D0
#define R_PCIE_GEN3FARTXCOEFF5                  0x35D4
#define R_PCIE_GEN3FARTXCOEFF6                  0x35D8
#define R_PCIE_GEN3FARTXCOEFF7                  0x35DC
#define N_PCIE_GEN3FARTXCOEFFX                  19
#define V_PCIE_GEN3FARTXCOEFF_D1                6
#define V_PCIE_GEN3FARTXCOEFF_D2                6
#define B_PCIE_GEN3FARTXCOEFF_FTYPE             BIT19
#define B_PCIE_GEN3FARTXCOEFF_LNOVRD            BIT31

#define R_PCIE_FOMHINTTBL0                      0x3A00
#define R_PCIE_FOMHINTTBL1                      0x3A04
#define R_PCIE_FOMHINTTBL2                      0x3A08
#define R_PCIE_FOMHINTTBL3                      0x3A0C
#define R_PCIE_FOMHINTTBL4                      0x3A10
#define R_PCIE_FOMHINTTBL5                      0x3A14
#define R_PCIE_FOMHINTTBL6                      0x3A18
#define R_PCIE_FOMHINTTBL7                      0x3A1C
#define R_PCIE_FOMHINTTBL8                      0x3A20
#define R_PCIE_FOMHINTTBL9                      0x3A24
#define R_PCIE_FOMHINTTBL10                     0x3A28
#define R_PCIE_FOMHINTTBL11                     0x3A2C

#define R_PCIE_FOMHINTTBL_ENABLE                BIT15
#define R_PCIE_FOMHINTTBL_CTLE                  BIT12
#define R_PCIE_FOMHINTTBL_ETM                   (BIT10|BIT11)

#define V_PCIE_FOMHINTTBL0                      0x0000b506
#define V_PCIE_FOMHINTTBL1                      0x0000b448
#define V_PCIE_FOMHINTTBL2                      0x0000b4ca
#define V_PCIE_FOMHINTTBL3                      0x0000b50c
#define V_PCIE_FOMHINTTBL4                      0x0000b44e
#define V_PCIE_FOMHINTTBL5_11                   0x000034c6

#define R_PCIE_EQEVALCTL                        0x000BD0
#define V_PCIE_EQEVALCTL                        0x00000000
#define V_PCIE_EQEVALCTL2                       0x00000ba0
#define V_PCIE_EQEVALCTL3                       0x33f00802
#define V_PCIE_EQEVALCTL4                       0x04b30004
#define R_PCIE_EQEVALCTL5                       0x000BBC
#define V_PCIE_EQEVALCTL5                       0x3a801013
#define R_PCIE_EQEVALCTL6                       0x000B8C
#define V_PCIE_EQEVALCTL6                       0x04904623
#define V_PCIE_EQEVALCTL7                       0x00000020
#define R_PCIE_FOMINCCTL                        0x000BA4
#define V_PCIE_FOMINCCTL                        0x0004ffff

#define R_PCH_PCIE_XPTDEF2                      0x8F8  // Downstream Relaxed Order Disable
#define V_PCH_PCIE_XPTDEF2_DNRODIS              BIT14  // DNRODIS

#define V_PCH_PCIE_LTSSMTRNCTL_ENFORCEDREC      BIT18  // LTSSMTRNCTL.ENFORCEDREC

#endif // _PCIE_IP_REGS_H_
