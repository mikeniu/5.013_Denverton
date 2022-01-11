/*++
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
--*/

/*++

Copyright (c)  2015 - 2016, Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  PcieIpPlatform.h

Abstract:

  Driver for Pcie Platform specific.

Revision History

--*/

#ifndef _PCIE_IP_PLATFORM_H_
#define _PCIE_IP_PLATFORM_H_


typedef enum {
  PCIE_IP_PchH          = 1,
  PCIE_IP_PchLp,
  PCIE_IP_PchUnknownSeries
} PCIE_IP_SERIES;

#define PCIE_IP_STANDARD_ROOT_PORT      8				//< Standard root ports
#define PCIE_IP_VIRTUAL_ROOT_PORT       3				//< Virtual root ports

//
// PCH PCI Express Root Ports (D28:F0~7 & D29:F0~3)
//
#define V_DEVICE_NUMBER_PCIE_ROOT_PORT_0  9
#define V_DEVICE_NUMBER_PCIE_ROOT_PORT_1  10
#define V_DEVICE_NUMBER_PCIE_ROOT_PORT_2  11
#define V_DEVICE_NUMBER_PCIE_ROOT_PORT_3  12
#define V_DEVICE_NUMBER_PCIE_ROOT_PORT_4  14
#define V_DEVICE_NUMBER_PCIE_ROOT_PORT_5  15
#define V_DEVICE_NUMBER_PCIE_ROOT_PORT_6  16
#define V_DEVICE_NUMBER_PCIE_ROOT_PORT_7  17

#define V_DEVICE_NUMBER_PCIE_VIRTUAL_ROOT_PORT_0 6
#define V_DEVICE_NUMBER_PCIE_VIRTUAL_ROOT_PORT_1 22
#define V_DEVICE_NUMBER_PCIE_VIRTUAL_ROOT_PORT_2 23

#define V_PCIE_ROOT_PORT_FUNC		0

#define PCH_PCIE_ROOT_COMPLEX_PRIMARY_BUS         0
#define PCH_PCIE_ROOT_COMPLEX_FUNCTION            0

#define PCIE_DEVICE_RTF                           0xE
#define R_XHCC1                                   0x80
#define PCI_OFFSET_A30H_LTSSMTRNCTL               0xA30
#define STR_FORCE_LINK_OPERATE_NON_LANE_REVERSAL_VALUE 0
#define STR_STR_FORCE_LINK_OPERATE_LANE_REVERSAL_VALUE 1
#define STR_DYNAMIC_LANE_REVERSAL_VALUE                3

#define R_PCH_PCIE_PPD0                           0xD4
#define B_PCH_PCIE_PPD0_ILINKTRN0                 BIT3

#define PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS     28

#define PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_1  0
#define PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_2  1
#define PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_3  2
#define PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_4  3
#define PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_5  4
#define PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_6  5
#define PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_7  6
#define PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_8  7

#define PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_9  0
#define PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_10 1
#define PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_11 2
#define PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_12 3
#define PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_13 4
#define PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_14 5
#define PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_15 6
#define PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_16 7

#define PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_17 0
#define PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_18 1
#define PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_19 2
#define PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_20 3
#define R_PCH_PCIE_VENDOR_ID                      0x00
#define V_PCH_PCIE_VENDOR_ID                      V_PCH_INTEL_VENDOR_ID
#define R_PCH_PCIE_DEVICE_ID                      0x02

#define V_PCH_LPTH_PCIE_DEVICE_ID_PORT1            0x8C10  ///< PCI Express Root Port #1, LPT
#define V_PCH_LPTH_PCIE_DEVICE_ID_PORT2            0x8C12  ///< PCI Express Root Port #2, LPT
#define V_PCH_LPTH_PCIE_DEVICE_ID_PORT3            0x8C14  ///< PCI Express Root Port #3, LPT
#define V_PCH_LPTH_PCIE_DEVICE_ID_PORT4            0x8C16  ///< PCI Express Root Port #4, LPT
#define V_PCH_LPTH_PCIE_DEVICE_ID_PORT5            0x8C18  ///< PCI Express Root Port #5, LPT
#define V_PCH_LPTH_PCIE_DEVICE_ID_PORT6            0x8C1A  ///< PCI Express Root Port #6, LPT
#define V_PCH_LPTH_PCIE_DEVICE_ID_PORT7            0x8C1C  ///< PCI Express Root Port #7, LPT
#define V_PCH_LPTH_PCIE_DEVICE_ID_PORT8            0x8C1E  ///< PCI Express Root Port #8, LPT
#define V_PCH_LPTH_PCIE_DEVICE_ID_MB_SUBD          0x2448  ///< Mobile with subtractive decode enable
#define V_PCH_LPTH_PCIE_DEVICE_ID_DT_SUBD          0x244E  ///< Desktop with subtractive decode enable

#define V_PCH_LPTHR_PCIE_DEVICE_ID_PORT1           0x8C90  ///< LPT H Refresh PCI Express Root Port #1, LPT
#define V_PCH_LPTHR_PCIE_DEVICE_ID_PORT2           0x8C92  ///< LPT H Refresh PCI Express Root Port #2, LPT
#define V_PCH_LPTHR_PCIE_DEVICE_ID_PORT3           0x8C94  ///< LPT H Refresh PCI Express Root Port #3, LPT
#define V_PCH_LPTHR_PCIE_DEVICE_ID_PORT4           0x8C96  ///< LPT H Refresh PCI Express Root Port #4, LPT
#define V_PCH_LPTHR_PCIE_DEVICE_ID_PORT5           0x8C98  ///< LPT H Refresh PCI Express Root Port #5, LPT
#define V_PCH_LPTHR_PCIE_DEVICE_ID_PORT6           0x8C9A  ///< LPT H Refresh PCI Express Root Port #6, LPT
#define V_PCH_LPTHR_PCIE_DEVICE_ID_PORT7           0x8C9C  ///< LPT H Refresh PCI Express Root Port #7, LPT
#define V_PCH_LPTHR_PCIE_DEVICE_ID_PORT8           0x8C9E  ///< LPT H Refresh PCI Express Root Port #8, LPT

#define V_PCH_LPTLP_PCIE_DEVICE_ID_PORT1            0x9C10  ///< PCI Express Root Port #1, LPTLP PCIe Device ID bit[0] fuse = 0
#define V_PCH_LPTLP_PCIE_DEVICE_ID_PORT1_ALT        0x9C11  ///< PCI Express Root Port #1, LPTLP PCIe Device ID bit[0] fuse = 1
#define V_PCH_LPTLP_PCIE_DEVICE_ID_PORT2            0x9C12  ///< PCI Express Root Port #2, LPTLP PCIe Device ID bit[0] fuse = 0
#define V_PCH_LPTLP_PCIE_DEVICE_ID_PORT2_ALT        0x9C13  ///< PCI Express Root Port #2, LPTLP PCIe Device ID bit[0] fuse = 1
#define V_PCH_LPTLP_PCIE_DEVICE_ID_PORT3            0x9C14  ///< PCI Express Root Port #3, LPTLP PCIe Device ID bit[0] fuse = 0
#define V_PCH_LPTLP_PCIE_DEVICE_ID_PORT3_ALT        0x9C15  ///< PCI Express Root Port #3, LPTLP PCIe Device ID bit[0] fuse = 1
#define V_PCH_LPTLP_PCIE_DEVICE_ID_PORT4            0x9C16  ///< PCI Express Root Port #4, LPTLP PCIe Device ID bit[0] fuse = 0
#define V_PCH_LPTLP_PCIE_DEVICE_ID_PORT4_ALT        0x9C17  ///< PCI Express Root Port #4, LPTLP PCIe Device ID bit[0] fuse = 1
#define V_PCH_LPTLP_PCIE_DEVICE_ID_PORT5            0x9C18  ///< PCI Express Root Port #5, LPTLP PCIe Device ID bit[0] fuse = 0
#define V_PCH_LPTLP_PCIE_DEVICE_ID_PORT5_ALT        0x9C19  ///< PCI Express Root Port #5, LPTLP PCIe Device ID bit[0] fuse = 1
#define V_PCH_LPTLP_PCIE_DEVICE_ID_PORT6            0x9C1A  ///< PCI Express Root Port #6, LPTLP PCIe Device ID bit[0] fuse = 0
#define V_PCH_LPTLP_PCIE_DEVICE_ID_PORT6_ALT        0x9C1B  ///< PCI Express Root Port #6, LPTLP PCIe Device ID bit[0] fuse = 1

#define V_PCH_WPTLP_PCIE_DEVICE_ID_PORT1            0x9C90  ///< PCI Express Root Port #1, LPTLP PCIe Device ID bit[0] fuse = 0
#define V_PCH_WPTLP_PCIE_DEVICE_ID_PORT1_ALT        0x9C91  ///< PCI Express Root Port #1, LPTLP PCIe Device ID bit[0] fuse = 1
#define V_PCH_WPTLP_PCIE_DEVICE_ID_PORT2            0x9C92  ///< PCI Express Root Port #2, LPTLP PCIe Device ID bit[0] fuse = 0
#define V_PCH_WPTLP_PCIE_DEVICE_ID_PORT2_ALT        0x9C93  ///< PCI Express Root Port #2, LPTLP PCIe Device ID bit[0] fuse = 1
#define V_PCH_WPTLP_PCIE_DEVICE_ID_PORT3            0x9C94  ///< PCI Express Root Port #3, LPTLP PCIe Device ID bit[0] fuse = 0
#define V_PCH_WPTLP_PCIE_DEVICE_ID_PORT3_ALT        0x9C95  ///< PCI Express Root Port #3, LPTLP PCIe Device ID bit[0] fuse = 1
#define V_PCH_WPTLP_PCIE_DEVICE_ID_PORT4            0x9C96  ///< PCI Express Root Port #4, LPTLP PCIe Device ID bit[0] fuse = 0
#define V_PCH_WPTLP_PCIE_DEVICE_ID_PORT4_ALT        0x9C97  ///< PCI Express Root Port #4, LPTLP PCIe Device ID bit[0] fuse = 1
#define V_PCH_WPTLP_PCIE_DEVICE_ID_PORT5            0x9C98  ///< PCI Express Root Port #5, LPTLP PCIe Device ID bit[0] fuse = 0
#define V_PCH_WPTLP_PCIE_DEVICE_ID_PORT5_ALT        0x9C99  ///< PCI Express Root Port #5, LPTLP PCIe Device ID bit[0] fuse = 1
#define V_PCH_WPTLP_PCIE_DEVICE_ID_PORT6            0x9C9A  ///< PCI Express Root Port #6, LPTLP PCIe Device ID bit[0] fuse = 0
#define V_PCH_WPTLP_PCIE_DEVICE_ID_PORT6_ALT        0x9C9B  ///< PCI Express Root Port #6, LPTLP PCIe Device ID bit[0] fuse = 1

#define V_PCH_SPTH_PCIE_DEVICE_ID_PORT1            0x8C10  ///< PCI Express Root Port #1, LPT
#define V_PCH_SPTH_PCIE_DEVICE_ID_PORT2            0x8C12  ///< PCI Express Root Port #2, LPT
#define V_PCH_SPTH_PCIE_DEVICE_ID_PORT3            0x8C14  ///< PCI Express Root Port #3, LPT
#define V_PCH_SPTH_PCIE_DEVICE_ID_PORT4            0x8C16  ///< PCI Express Root Port #4, LPT
#define V_PCH_SPTH_PCIE_DEVICE_ID_PORT5            0x8C18  ///< PCI Express Root Port #5, LPT
#define V_PCH_SPTH_PCIE_DEVICE_ID_PORT6            0x8C1A  ///< PCI Express Root Port #6, LPT
#define V_PCH_SPTH_PCIE_DEVICE_ID_PORT7            0x8C1C  ///< PCI Express Root Port #7, LPT
#define V_PCH_SPTH_PCIE_DEVICE_ID_PORT8            0x8C1E  ///< PCI Express Root Port #8, LPT
#define V_PCH_SPTH_PCIE_DEVICE_ID_PORT9            0x8C1E  ///< PCI Express Root Port #8, LPT
#define V_PCH_SPTH_PCIE_DEVICE_ID_PORT10           0x8C1E  ///< PCI Express Root Port #8, LPT
#define V_PCH_SPTH_PCIE_DEVICE_ID_PORT11           0x8C1E  ///< PCI Express Root Port #8, LPT
#define V_PCH_SPTH_PCIE_DEVICE_ID_PORT12           0x8C1E  ///< PCI Express Root Port #8, LPT
#define V_PCH_SPTH_PCIE_DEVICE_ID_PORT13           0x8C1E  ///< PCI Express Root Port #8, LPT
#define V_PCH_SPTH_PCIE_DEVICE_ID_PORT14           0x8C1E  ///< PCI Express Root Port #8, LPT
#define V_PCH_SPTH_PCIE_DEVICE_ID_PORT15           0x8C1E  ///< PCI Express Root Port #8, LPT
#define V_PCH_SPTH_PCIE_DEVICE_ID_PORT16           0x8C1E  ///< PCI Express Root Port #8, LPT
#define V_PCH_SPTH_PCIE_DEVICE_ID_PORT17           0x8C1E  ///< PCI Express Root Port #8, LPT
#define V_PCH_SPTH_PCIE_DEVICE_ID_PORT18           0x8C1E  ///< PCI Express Root Port #8, LPT
#define V_PCH_SPTH_PCIE_DEVICE_ID_PORT19           0x8C1E  ///< PCI Express Root Port #8, LPT
#define V_PCH_SPTH_PCIE_DEVICE_ID_MB_SUBD          0x2448  ///< Mobile with subtractive decode enable
#define V_PCH_SPTH_PCIE_DEVICE_ID_DT_SUBD          0x244E  ///< Desktop with subtractive decode enable

#define V_PCH_SPTLP_PCIE_DEVICE_ID_PORT1            0x9C10  ///< PCI Express Root Port #1, SPTLP PCIe Device ID bit[0] fuse = 0
#define V_PCH_SPTLP_PCIE_DEVICE_ID_PORT1_ALT        0x9C11  ///< PCI Express Root Port #1, SPTLP PCIe Device ID bit[0] fuse = 1
#define V_PCH_SPTLP_PCIE_DEVICE_ID_PORT2            0x9C12  ///< PCI Express Root Port #2, SPTLP PCIe Device ID bit[0] fuse = 0
#define V_PCH_SPTLP_PCIE_DEVICE_ID_PORT2_ALT        0x9C13  ///< PCI Express Root Port #2, SPTLP PCIe Device ID bit[0] fuse = 1
#define V_PCH_SPTLP_PCIE_DEVICE_ID_PORT3            0x9C14  ///< PCI Express Root Port #3, SPTLP PCIe Device ID bit[0] fuse = 0
#define V_PCH_SPTLP_PCIE_DEVICE_ID_PORT3_ALT        0x9C15  ///< PCI Express Root Port #3, SPTLP PCIe Device ID bit[0] fuse = 1
#define V_PCH_SPTLP_PCIE_DEVICE_ID_PORT4            0x9C16  ///< PCI Express Root Port #4, SPTLP PCIe Device ID bit[0] fuse = 0
#define V_PCH_SPTLP_PCIE_DEVICE_ID_PORT4_ALT        0x9C17  ///< PCI Express Root Port #4, SPTLP PCIe Device ID bit[0] fuse = 1
#define V_PCH_SPTLP_PCIE_DEVICE_ID_PORT5            0x9C18  ///< PCI Express Root Port #5, SPTLP PCIe Device ID bit[0] fuse = 0
#define V_PCH_SPTLP_PCIE_DEVICE_ID_PORT5_ALT        0x9C19  ///< PCI Express Root Port #5, SPTLP PCIe Device ID bit[0] fuse = 1
#define V_PCH_SPTLP_PCIE_DEVICE_ID_PORT6            0x9C1A  ///< PCI Express Root Port #6, SPTLP PCIe Device ID bit[0] fuse = 0
#define V_PCH_SPTLP_PCIE_DEVICE_ID_PORT6_ALT        0x9C1B  ///< PCI Express Root Port #6, SPTLP PCIe Device ID bit[0] fuse = 1
#define V_PCH_SPTLP_PCIE_DEVICE_ID_PORT6            0x9C1A  ///< PCI Express Root Port #7, SPTLP PCIe Device ID bit[0] fuse = 0
#define V_PCH_SPTLP_PCIE_DEVICE_ID_PORT6_ALT        0x9C1B  ///< PCI Express Root Port #7, SPTLP PCIe Device ID bit[0] fuse = 1
#define V_PCH_SPTLP_PCIE_DEVICE_ID_PORT6            0x9C1A  ///< PCI Express Root Port #8, SPTLP PCIe Device ID bit[0] fuse = 0
#define V_PCH_SPTLP_PCIE_DEVICE_ID_PORT6_ALT        0x9C1B  ///< PCI Express Root Port #8, SPTLP PCIe Device ID bit[0] fuse = 1
#define V_PCH_SPTLP_PCIE_DEVICE_ID_PORT6            0x9C1A  ///< PCI Express Root Port #9, SPTLP PCIe Device ID bit[0] fuse = 0
#define V_PCH_SPTLP_PCIE_DEVICE_ID_PORT6_ALT        0x9C1B  ///< PCI Express Root Port #9, SPTLP PCIe Device ID bit[0] fuse = 1
#define V_PCH_SPTLP_PCIE_DEVICE_ID_PORT6            0x9C1A  ///< PCI Express Root Port #10, SPTLP PCIe Device ID bit[0] fuse = 0
#define V_PCH_SPTLP_PCIE_DEVICE_ID_PORT6_ALT        0x9C1B  ///< PCI Express Root Port #10, SPTLP PCIe Device ID bit[0] fuse = 1
#define V_PCH_SPTLP_PCIE_DEVICE_ID_PORT6            0x9C1A  ///< PCI Express Root Port #11, SPTLP PCIe Device ID bit[0] fuse = 0
#define V_PCH_SPTLP_PCIE_DEVICE_ID_PORT6_ALT        0x9C1B  ///< PCI Express Root Port #11, SPTLP PCIe Device ID bit[0] fuse = 1
#define V_PCH_SPTLP_PCIE_DEVICE_ID_PORT6            0x9C1A  ///< PCI Express Root Port #12, SPTLP PCIe Device ID bit[0] fuse = 0
#define V_PCH_SPTLP_PCIE_DEVICE_ID_PORT6_ALT        0x9C1B  ///< PCI Express Root Port #12, SPTLP PCIe Device ID bit[0] fuse = 1

#define R_PCIE_SOSCTL                               0x000AC8	///< Offset in PCI Express configuration to optimize performance
#define V_PCIE_SOSCTL                               0x2E4		///< Value PCI Express configuration to optimize performance (0x2E4)
#define V_PCIE_SOSCTL_MASK                          0x7FF	///< Mask for SOSCTL

#define R_PCIE_DEVCTL                               0x000048	///< Offset in PCI Express override to MPS and MRRS
#define V_PCIE_DEVCTL_MRRS_MASK					    (BIT14 | BIT13 | BIT12) 		///< Mask PCI Express DEVCTL MRRS field
#define V_PCIE_DEVCTL_MPS_MASK					    (BIT5 | BIT6 | BIT7) 		    ///< Mask PCI Express DEVCTL MPS field
#define V_PCIE_DEVCTL_MASK                          (V_PCIE_DEVCTL_MRRS_MASK|V_PCIE_DEVCTL_MPS_MASK)		///< Mask PCI Express override FOR DEVCTL
#define V_PCIE_DEVCTL                               0x5020

#define R_PCH_PCIE_PCICMD                         0x04
#define S_PCH_PCIE_PCICMD                         2
#define B_PCH_PCIE_PCICMD_ID                      BIT10
#define B_PCH_PCIE_PCICMD_FBE                     BIT9
#define B_PCH_PCIE_PCICMD_SEE                     BIT8
#define B_PCH_PCIE_PCICMD_WCC                     BIT7
#define B_PCH_PCIE_PCICMD_PER                     BIT6
#define B_PCH_PCIE_PCICMD_VPS                     BIT5
#define B_PCH_PCIE_PCICMD_PMWE                    BIT4
#define B_PCH_PCIE_PCICMD_SCE                     BIT3
#define B_PCH_PCIE_PCICMD_BME                     BIT2
#define B_PCH_PCIE_PCICMD_MSE                     BIT1
#define B_PCH_PCIE_PCICMD_IOSE                    BIT0
#define R_PCH_PCIE_PCISTS                         0x06
#define S_PCH_PCIE_PCISTS                         2
#define B_PCH_PCIE_PCISTS_DPE                     BIT15
#define B_PCH_PCIE_PCISTS_SSE                     BIT14
#define B_PCH_PCIE_PCISTS_RMA                     BIT13
#define B_PCH_PCIE_PCISTS_RTA                     BIT12
#define B_PCH_PCIE_PCISTS_STA                     BIT11
#define B_PCH_PCIE_PCISTS_DEV_STS                 (BIT10 | BIT9)
#define B_PCH_PCIE_PCISTS_DPED                    BIT8
#define B_PCH_PCIE_PCISTS_FB2BC                   BIT7
#define B_PCH_PCIE_PCISTS_66MHZ_CAP               BIT5
#define B_PCH_PCIE_PCISTS_CAP_LST                 BIT4
#define B_PCH_PCIE_PCISTS_INTR_STS                BIT3
#define R_PCH_PCIE_RID                            0x08
#define B_PCH_PCIE_RID                            0xFF
#define R_PCH_PCIE_PI                             0x09
#define B_PCH_PCIE_PI                             0xFF
#define R_PCH_PCIE_SCC                            0x0A
#define B_PCH_PCIE_SCC                            0xFF
#define V_PCH_PCIE_SCC_04                         0x04
#define V_PCH_PCIE_SCC_00                         0x00
#define R_PCH_PCIE_BCC                            0x0B
#define B_PCH_PCIE_BCC                            0xFF
#define R_PCH_PCIE_CLS                            0x0C
#define B_PCH_PCIE_CLS                            0xFF
#define R_PCH_PCIE_PLT                            0x0D
#define B_PCH_PCIE_PLT_LC                         0xF8
#define R_PCH_PCIE_HEADTYPE                       0x0E
#define B_PCH_PCIE_HEADTYPE_MFD                   BIT7
#define B_PCH_PCIE_HEADTYPE_CL                    0x7F
#define V_PCH_PCIE_HEADTYPE_CL_01                 0x01
#define V_PCH_PCIE_HEADTYPE_CL_00                 0x00
#define R_PCH_PCIE_BNUM                           0x18
#define B_PCH_PCIE_BNUM_SBBN                      0x00FF0000
#define B_PCH_PCIE_BNUM_SCBN                      0x0000FF00
#define B_PCH_PCIE_BNUM_PBN                       0x000000FF
#define R_PCH_PCIE_SLT                            0x1B
#define B_PCH_PCIE_SLT                            0xFF
#define R_PCH_PCIE_IOBL                           0x1C
#define B_PCH_PCIE_IOBL_IOLA                      0xF000
#define B_PCH_PCIE_IOBL_IOLC                      0x0F00
#define B_PCH_PCIE_IOBL_IOBA                      0x00F0
#define B_PCH_PCIE_IOBL_IOBC                      0x000F
#define R_PCH_PCIE_SSTS                           0x1E
#define S_PCH_PCIE_SSTS                           2
#define B_PCH_PCIE_SSTS_DPE                       BIT15
#define B_PCH_PCIE_SSTS_RSE                       BIT14
#define B_PCH_PCIE_SSTS_RMA                       BIT13
#define B_PCH_PCIE_SSTS_RTA                       BIT12
#define B_PCH_PCIE_SSTS_STA                       BIT11
#define B_PCH_PCIE_SSTS_SDTS                      (BIT10 | BIT9)
#define B_PCH_PCIE_SSTS_DPD                       BIT8
#define B_PCH_PCIE_SSTS_SFBC                      BIT7
#define B_PCH_PCIE_SSTS_SC66                      BIT5
#define R_PCH_PCIE_MBL                            0x20
#define B_PCH_PCIE_MBL_ML                         0xFFF00000
#define B_PCH_PCIE_MBL_MB                         0x0000FFF0
#define R_PCH_PCIE_PMBL                           0x24
#define B_PCH_PCIE_PMBL_PML                       0xFFF00000
#define B_PCH_PCIE_PMBL_I64L                      0x000F0000
#define B_PCH_PCIE_PMBL_PMB                       0x0000FFF0
#define B_PCH_PCIE_PMBL_I64B                      0x0000000F
#define R_PCH_PCIE_PMBU32                         0x28
#define B_PCH_PCIE_PMBU32                         0xFFFFFFFF
#define R_PCH_PCIE_PMLU32                         0x2C
#define B_PCH_PCIE_PMLU32                         0xFFFFFFFF
#define R_PCH_PCIE_CAPP                           0x34
#define B_PCH_PCIE_CAPP                           0xFF
#define R_PCH_PCIE_INTR                           0x3C
#define B_PCH_PCIE_INTR_IPIN                      0xFF00
#define B_PCH_PCIE_INTR_ILINE                     0x00FF
#define R_PCH_PCIE_INTPIN                         0x3D
#define R_PCH_PCIE_BCTRL                          0x3E
#define S_PCH_PCIE_BCTRL                          2
#define B_PCH_PCIE_BCTRL_DTSE                     BIT11
#define B_PCH_PCIE_BCTRL_DTS                      BIT10
#define B_PCH_PCIE_BCTRL_SDT                      BIT9
#define B_PCH_PCIE_BCTRL_PDT                      BIT8
#define B_PCH_PCIE_BCTRL_FBE                      BIT7
#define B_PCH_PCIE_BCTRL_SBR                      BIT6
#define B_PCH_PCIE_BCTRL_MAM                      BIT5
#define B_PCH_PCIE_BCTRL_V16                      BIT4
#define B_PCH_PCIE_BCTRL_VE                       BIT3
#define B_PCH_PCIE_BCTRL_IE                       BIT2
#define B_PCH_PCIE_BCTRL_SE                       BIT1
#define B_PCH_PCIE_BCTRL_PERE                     BIT0
#define R_PCH_PCIE_CLIST                          0x40
#define B_PCH_PCIE_CLIST_NEXT                     0xFF00
#define B_PCH_PCIE_CLIST_CID                      0x00FF
#define R_PCH_PCIE_XCAP                           0x42
#define S_PCH_PCIE_XCAP                           2
#define B_PCH_PCIE_XCAP_IMN                       0x3E00
#define B_PCH_PCIE_XCAP_SI                        BIT8
#define B_PCH_PCIE_XCAP_DT                        0x00F0
#define B_PCH_PCIE_XCAP_CV                        0x000F
#define R_PCH_PCIE_DCAP                           0x44
#define S_PCH_PCIE_DCAP                           4
#define B_PCH_PCIE_DCAP_CSPS                      0x0C000000
#define B_PCH_PCIE_DCAP_CSPV                      0x03FC0000
#define B_PCH_PCIE_DCAP_RBER                      BIT15
#define B_PCH_PCIE_DCAP_PIP                       BIT14
#define B_PCH_PCIE_DCAP_AIP                       BIT13
#define B_PCH_PCIE_DCAP_ABP                       BIT12
#define B_PCH_PCIE_DCAP_E1AL                      0x00000E00
#define B_PCH_PCIE_DCAP_E0AL                      0x000001C0
#define B_PCH_PCIE_DCAP_ETFS                      BIT5
#define B_PCH_PCIE_DCAP_PFS                       0x00000018
#define B_PCH_PCIE_DCAP_MPS                       0x00000007
#define R_PCH_PCIE_DCTL                           0x48
#define S_PCH_PCIE_DCTL                           2
#define B_PCH_PCIE_DCTL_MRRS                      0x7000
#define B_PCH_PCIE_DCTL_ENS                       BIT11

#define R_PCH_PCIE_DCAP2                          0x64

#define N_PCH_PCIE_DCTL_ENS                       11
#define N_PCH_PCIE_DCTL_MPS                       5
#define N_PCH_PCIE_DCTL_MRRS                      12
#define B_PCH_PCIE_LTSSMTRNCTL_SURLNKNEN   	      BIT20

#define B_PCH_PCIE_DCTL_APME                      BIT10
#define B_PCH_PCIE_DCTL_PFE                       BIT9
#define B_PCH_PCIE_DCTL_ETFE                      BIT8
#define B_PCH_PCIE_DCTL_MPS                       (BIT7 | BIT6 | BIT5)
#define B_PCH_PCIE_DCTL_ERO                       BIT4
#define B_PCH_PCIE_DCTL_URE                       BIT3
#define B_PCH_PCIE_DCTL_FEE                       BIT2
#define B_PCH_PCIE_DCTL_NFE                       BIT1
#define B_PCH_PCIE_DCTL_CEE                       BIT0
#define R_PCH_PCIE_DSTS                           0x4A
#define B_PCH_PCIE_DSTS_TDP                       BIT5
#define B_PCH_PCIE_DSTS_APD                       BIT4
#define B_PCH_PCIE_DSTS_URD                       BIT3
#define B_PCH_PCIE_DSTS_FED                       BIT2
#define B_PCH_PCIE_DSTS_NFED                      BIT1
#define B_PCH_PCIE_DSTS_CED                       BIT0
#define R_PCH_PCIE_LCAP                           0x4C
#define B_PCH_PCIE_LCAP_PN                        0xFF000000
#define V_PCH_PCIE_LCAP_PN1                       (1 << 24)
#define V_PCH_PCIE_LCAP_PN2                       (2 << 24)
#define V_PCH_PCIE_LCAP_PN3                       (3 << 24)
#define V_PCH_PCIE_LCAP_PN4                       (4 << 24)
#define V_PCH_PCIE_LCAP_PN5                       (5 << 24)
#define V_PCH_PCIE_LCAP_PN6                       (6 << 24)
#define V_PCH_PCIE_LCAP_PN7                       (7 << 24)
#define V_PCH_PCIE_LCAP_PN8                       (8 << 24)
#define B_PCH_PCIE_LCAP_LARC                      BIT20
#define B_PCH_PCIE_LCAP_EL1                       (BIT17 | BIT16 | BIT15)
#define B_PCH_PCIE_LCAP_EL0                       (BIT14 | BIT13 | BIT12)
#define B_PCH_PCIE_LCAP_APMS                      (BIT11 | BIT10)
#define V_PCH_PCIE_LCAP_APMS_L0S                  (1 << 10)
#define V_PCH_PCIE_LCAP_APMS_L1                   (2 << 10) // L1 Entry Supported
#define V_PCH_PCIE_LCAP_APMS_L1_DISABLE           ~( BIT11 | BIT10 )
#define V_PCH_PCIE_LCAP_APMS_L0S_L1               (3 << 10)
#define B_PCH_PCIE_LCAP_MLW                       0x000003F0
#define B_PCH_PCIE_LCAP_MLS                       0x0000000F
#define R_PCH_PCIE_LCTL                           0x50
#define B_PCH_PCIE_LCTL_HAWD                      BIT9
#define B_PCH_PCIE_LCTL_ES                        BIT7
#define B_PCH_PCIE_LCTL_CCC                       BIT6
#define B_PCH_PCIE_LCTL_RL                        BIT5
#define B_PCH_PCIE_LCTL_LD                        BIT4
#define B_PCH_PCIE_LCTL_RCBC                      BIT3
#define B_PCH_PCIE_LCTL_APMC                      (BIT1 | BIT0)
#define V_PCH_PCIE_LCTL_APMC_L0S                  1
#define V_PCH_PCIE_LCTL_APMC_L1                   2
#define V_PCH_PCIE_LCTL_APMC_L1_DISABLE           ~( BIT1 | BIT0 )
#define V_PCH_PCIE_LCTL_APMC_L0S_L1               3
#define R_PCH_PCIE_LSTS                           0x52
#define S_PCH_PCIE_LSTS                           2
#define B_PCH_PCIE_LSTS_DLLA                      BIT13
#define B_PCH_PCIE_LSTS_SCC                       BIT12
#define B_PCH_PCIE_LSTS_LT                        BIT11
#define B_PCH_PCIE_LSTS_LTE                       BIT10
#define B_PCH_PCIE_LSTS_NLW                       0x03F0
#define V_PCH_PCIE_LSTS_NLW_0                     0x0000
#define V_PCH_PCIE_LSTS_NLW_1                     0x0010
#define V_PCH_PCIE_LSTS_NLW_2                     0x0020
#define V_PCH_PCIE_LSTS_NLW_4                     0x0040
#define V_PCH_PCIE_LSTS_NLW_8                     0x0080
#define V_PCH_PCIE_LSTS_NLW_16                    0x0100
#define B_PCH_PCIE_LSTS_LS                        0x000F
#define V_PCH_PCIE_LSTS_LS_GEN1                   1
#define V_PCH_PCIE_LSTS_LS_GEN2                   2
#define V_PCH_PCIE_LSTS_LS_GEN3                   3
#define R_PCH_PCIE_SLCAP                          0x54
#define S_PCH_PCIE_SLCAP                          4
#define B_PCH_PCIE_SLCAP_PSN                      0xFFF80000
#define B_PCH_PCIE_SLCAP_SLS                      0x00018000
#define B_PCH_PCIE_SLCAP_SLV                      0x00007F80
#define B_PCH_PCIE_SLCAP_HPC                      BIT6
#define B_PCH_PCIE_SLCAP_HPS                      BIT5
#define B_PCH_PCIE_SLCAP_PIP                      BIT4
#define B_PCH_PCIE_SLCAP_AIP                      BIT3
#define B_PCH_PCIE_SLCAP_MSP                      BIT2
#define B_PCH_PCIE_SLCAP_PCP                      BIT1
#define B_PCH_PCIE_SLCAP_ABP                      BIT0
#define R_PCH_PCIE_SLCTL                          0x58
#define S_PCH_PCIE_SLCTL                          2
#define B_PCH_PCIE_SLCTL_LACE                     BIT12
#define B_PCH_PCIE_SLCTL_PCC                      BIT10
#define B_PCH_PCIE_SLCTL_HPE                      BIT5
#define B_PCH_PCIE_SLCTL_PDE                      BIT3
#define R_PCH_PCIE_SLSTS                          0x5A
#define S_PCH_PCIE_SLSTS                          2
#define B_PCH_PCIE_SLSTS_LASC                     BIT8
#define B_PCH_PCIE_SLSTS_PDS                      BIT6
#define B_PCH_PCIE_SLSTS_MS                       BIT5
#define B_PCH_PCIE_SLSTS_PDC                      BIT3
#define B_PCH_PCIE_SLSTS_MSC                      BIT2
#define B_PCH_PCIE_SLSTS_PFD                      BIT1
#define R_PCH_PCIE_RCTL                           0x5C
#define S_PCH_PCIE_RCTL                           2
#define B_PCH_PCIE_RCTL_PIE                       BIT3
#define B_PCH_PCIE_RCTL_SFE                       BIT2
#define B_PCH_PCIE_RCTL_SNE                       BIT1
#define B_PCH_PCIE_RCTL_SCE                       BIT0
#define R_PCH_PCIE_RSTS                           0x60
#define S_PCH_PCIE_RSTS                           4
#define B_PCH_PCIE_RSTS_PP                        BIT17
#define B_PCH_PCIE_RSTS_PS                        BIT16
#define B_PCH_PCIE_RSTS_RID                       0x0000FFFF
#define R_PCH_PCIE_DCAP2                          0x64
#define B_PCH_PCIE_DCAP2_CTDS                     BIT4
#define B_PCH_PCIE_DCAP2_CTRS                     0xF
#define V_PCH_PCIE_DCAP2_CTRS_UNSUPPORTED         0x0
#define V_PCH_PCIE_DCAP2_CTRS_RANGE_A             0x1
#define V_PCH_PCIE_DCAP2_CTRS_RANGE_B             0x2
#define V_PCH_PCIE_DCAP2_CTRS_RANGE_C             0x4
#define V_PCH_PCIE_DCAP2_CTRS_RANGE_D             0x8
#define R_PCH_PCIE_DCTL2                          0x68
#define B_PCH_PCIE_DCTL2_CTD                      BIT4
#define B_PCH_PCIE_DCTL2_CTV                      0xF
#define V_PCH_PCIE_DCTL2_CTV_DEFAULT              0x0
#define V_PCH_PCIE_DCTL2_CTV_40MS_50MS            0x5
#define V_PCH_PCIE_DCTL2_CTV_160MS_170MS          0x6
#define V_PCH_PCIE_DCTL2_CTV_400MS_500MS          0x9
#define V_PCH_PCIE_DCTL2_CTV_1P6S_1P7S            0xA
#define R_PCH_PCIE_LCTL2                          0x70
#define V_PCH_PCIE_LCTL2_DEEMPHASYS_6_DB_VALUE    0
#define V_PCH_PCIE_LCTL2_DEEMPHASYS_3P5_DB_VALUE  1
#define B_PCH_PCIE_LCTL2_TLS                      (BIT3 | BIT2 | BIT1 | BIT0)
#define R_PCH_PCIE_MID                            0x80
#define S_PCH_PCIE_MID                            2
#define B_PCH_PCIE_MID_NEXT                       0xFF00
#define B_PCH_PCIE_MID_CID                        0x00FF
#define R_PCH_PCIE_MC                             0x82
#define S_PCH_PCIE_MC                             2
#define B_PCH_PCIE_MC_C64                         BIT7
#define B_PCH_PCIE_MC_MME                         (BIT6 | BIT5 | BIT4)
#define B_PCH_PCIE_MC_MMC                         0x000E
#define B_PCH_PCIE_MC_MSIE                        BIT0
#define R_PCH_PCIE_MA                             0x84
#define S_PCH_PCIE_MA                             4
#define B_PCH_PCIE_MA_ADDR                        0xFFFFFFFC
#define R_PCH_PCIE_MD                             0x88
#define S_PCH_PCIE_MD                             2
#define B_PCH_PCIE_MD_DATA                        0xFFFF
#define R_PCH_PCIE_SVCAP                          0x90
#define S_PCH_PCIE_SVCAP                          2
#define B_PCH_PCIE_SVCAP_NEXT                     0xFF00
#define B_PCH_PCIE_SVCAP_CID                      0x00FF
#define R_PCH_PCIE_SVID                           0x94
#define S_PCH_PCIE_SVID                           4
#define B_PCH_PCIE_SVID_SID                       0xFFFF0000
#define B_PCH_PCIE_SVID_SVID                      0x0000FFFF
#define R_PCH_PCIE_PMCAP                          0xA0
#define S_PCH_PCIE_PMCAP                          2
#define B_PCH_PCIE_PMCAP_NEXT                     0xFF00
#define B_PCH_PCIE_PMCAP_CID                      0x00FF
#define R_PCH_PCIE_PMC                            0xA2
#define S_PCH_PCIE_PMC                            2
#define B_PCH_PCIE_PMC_PMES                       0xF800
#define B_PCH_PCIE_PMC_D2S                        BIT10
#define B_PCH_PCIE_PMC_D1S                        BIT9
#define B_PCH_PCIE_PMC_AC                         0x01C0
#define B_PCH_PCIE_PMC_DSI                        BIT5
#define B_PCH_PCIE_PMC_PMEC                       BIT3
#define B_PCH_PCIE_PMC_VS                         0x0007
#define R_PCH_PCIE_PMCS                           0xA4
#define S_PCH_PCIE_PMCS                           4
#define B_PCH_PCIE_PMCS_BPCE                      BIT23
#define B_PCH_PCIE_PMCS_B23S                      BIT22
#define B_PCH_PCIE_PMCS_PMES                      BIT15
#define B_PCH_PCIE_PMCS_PMEE                      BIT8
#define B_PCH_PCIE_PMCS_PS                        (BIT1 | BIT0)
#define V_PCH_PCIE_PMCS_D0                        0x00
#define V_PCH_PCIE_PMCS_D3H                       0x03
#define R_PCH_PCIE_CCFG                           0xD0
#define B_PCH_PCIE_CCFG_DCGEISMA                  BIT17
#define R_PCH_PCIE_SMICSR                         0xC8
#define S_PCH_PCIE_SMICSR                         4
#define B_PCH_PCIE_SMICSR_PMME                    BIT0
#define B_PCH_PCIE_SMICSR_HPME                    BIT1
#define N_PCH_PCIE_SMICSR_HPME                    1
#define B_PCH_PCIE_SMICSR_HPDLLSCS                BIT18
#define N_PCH_PCIE_SMICSR_HPDLLSCS                18
#define B_PCH_PCIE_SMICSR_HPPDCS                  BIT17
#define N_PCH_PCIE_SMICSR_HPPDCS                  17
#define R_PCH_PCIE_SCICSR                         0xCC
#define S_PCH_PCIE_SCICSR                         4
#define B_PCH_PCIE_SCICSR_PMCE                    BIT0
#define B_PCH_PCIE_SCICSR_HPCE                    BIT1
#define R_PCH_PCIE_MPC2                           0xD4
#define S_PCH_PCIE_MPC2                           4
#define B_PCH_PCIE_MPC2_PCME                      BIT5
#define B_PCH_PCIE_MPC2_ASPMCOEN                  BIT4
#define B_PCH_PCIE_MPC2_ASPMCO                    (BIT3 | BIT2)
#define V_PCH_PCIE_MPC2_ASPMCO_DISABLED           0
#define V_PCH_PCIE_MPC2_ASPMCO_L0S                1 << 2
#define V_PCH_PCIE_MPC2_ASPMCO_L1                 2 << 2
#define V_PCH_PCIE_MPC2_ASPMCO_L0S_L1             3 << 2
#define B_PCH_PCIE_MPC2_EOIFD                     BIT1
#define B_PCH_PCIE_MPC2_L1CTM                     BIT0
#define R_PCH_PCIE_MPC                            0xD8
#define S_PCH_PCIE_MPC                            4
#define B_PCH_PCIE_MPC_PMCE                       BIT31
#define B_PCH_PCIE_MPC_HPCE                       BIT30
#define B_PCH_PCIE_MPC_LHO                        BIT29
#define B_PCH_PCIE_MPC_ATE                        BIT28
#define B_PCH_PCIE_MPC_MMBNCE                     BIT27
#define B_PCH_PCIE_MPC_IRBNCE                     BIT26
#define B_PCH_PCIE_MPC_IRRCE                      BIT25
#define B_PCH_PCIE_MPC_BMERCE                     BIT24
#define B_PCH_PCIE_MPC_FORCEDET                   BIT22
#define B_PCH_PCIE_MPC_FCDL1E                     BIT21
#define B_PCH_PCIE_MPC_UCEL                       (BIT20 | BIT19 | BIT18)
#define B_PCH_PCIE_MPC_CCEL                       (BIT17 | BIT16 | BIT15)
#define B_PCH_PCIE_MPC_PAE                        BIT7
#define B_PCH_PCIE_MPC_MCTPSE                     BIT3
#define B_PCH_PCIE_MPC_BT                         BIT2
#define B_PCH_PCIE_MPC_HPME                       BIT1
#define N_PCH_PCIE_MPC_HPME                       1
#define B_PCH_PCIE_MPC_PMME                       BIT0
#define R_PCH_PCIE_SMSCS                          0xDC
#define S_PCH_PCIE_SMSCS                          4
#define B_PCH_PCIE_SMSCS_PMCS                     BIT31
#define B_PCH_PCIE_SMSCS_HPCS                     BIT30
#define B_PCH_PCIE_SMSCS_HPLAS                    BIT4
#define N_PCH_PCIE_SMSCS_HPLAS                    4
#define B_PCH_PCIE_SMSCS_HPCCM                    BIT3
#define B_PCH_PCIE_SMSCS_HPABM                    BIT2
#define B_PCH_PCIE_SMSCS_HPPDM                    BIT1
#define N_PCH_PCIE_SMSCS_HPPDM                    1
#define B_PCH_PCIE_SMSCS_PMMS                     BIT0
#define R_PCH_PCIE_RPDCGEN                        0xE1
#define S_PCH_PCIE_RPDCGEN                        1
#define B_PCH_PCIE_RPDCGEN_RPSCGEN                BIT7
#define B_PCH_PCIE_RPDCGEN_POCGE                  BIT6
#define B_PCH_PCIE_RPDCGEN_LCLKREQEN              BIT5
#define B_PCH_PCIE_RPDCGEN_BBCLKREQEN             BIT4
#define B_PCH_PCIE_RPDCGEN_SRDLCGEN               BIT3
#define B_PCH_PCIE_RPDCGEN_SRDBCGEN               BIT2
#define B_PCH_PCIE_RPDCGEN_RPDLCGEN               BIT1
#define B_PCH_PCIE_RPDCGEN_RPDBCGEN               BIT0
#define R_PCH_PCIE_RPPGEN                         0xE2
#define B_PCH_PCIE_RPPGEN_PTOTOP                  BIT6
#define B_PCH_PCIE_RPPGEN_LMSDOCGE                BIT5
#define B_PCH_PCIE_RPPGEN_SEOCGE                  BIT4
#define R_PCH_PCIE_PECR1                          0xE8
#define S_PCH_PCIE_PECR1                          4
#define B_PCH_PCIE_PECR1_FIELD_2                  BIT1
#define V_PCH_PCIE_PECR1_FIELD_3                  (BIT3 | BIT2)
#define R_PCH_PCIE_PECR3                          0xEC
#define R_PCH_PCIE_STRPFUSECFG                    0xFC

#define B_PCH_PCIE_STRPFUSECFG_PXIP               (BIT27 | BIT26 | BIT25 | BIT24)
#define N_PCH_PCIE_STRPFUSECFG_PXIP               24
#define B_PCH_PCIE_STRPFUSECFG_RPC                (BIT15 | BIT14)
#define V_PCH_PCIE_STRPFUSECFG_RPC_1_1_1_1        (0)
#define V_PCH_PCIE_STRPFUSECFG_RPC_2_1_1          (BIT14)
#define V_PCH_PCIE_STRPFUSECFG_RPC_2_2            (BIT15)
#define V_PCH_PCIE_STRPFUSECFG_RPC_4              (BIT15 | BIT14)
#define N_PCH_PCIE_STRPFUSECFG_RPC_4              14
#define B_PCH_PCIE_STRPFUSECFG_mPHYIOPMDIS        (BIT9)
#define B_PCH_PCIE_STRPFUSECFG_PLLSHTDWNDIS       (BIT8)
#define B_PCH_PCIE_STRPFUSECFG_STPGATEDIS         (BIT7)
#define B_PCH_PCIE_STRPFUSECFG_ASPMDIS            (BIT6)
#define B_PCH_PCIE_STRPFUSECFG_LDCGDIS            (BIT5)
#define B_PCH_PCIE_STRPFUSECFG_LTCGDIS            (BIT4)
#define B_PCH_PCIE_STRPFUSECFG_BDCGDIS            (BIT3)
#define B_PCH_PCIE_STRPFUSECFG_DESKTOPMOB         (BIT1)
#define R_PCH_PCIE_AECH                           0x100
#define R_PCH_PCIE_UES                            0x104
#define S_PCH_PCIE_UES                            4
#define B_PCH_PCIE_UES_URE                        BIT20
#define B_PCH_PCIE_UES_EE                         BIT19
#define B_PCH_PCIE_UES_MT                         BIT18
#define B_PCH_PCIE_UES_RO                         BIT17
#define B_PCH_PCIE_UES_UC                         BIT16
#define B_PCH_PCIE_UES_CA                         BIT15
#define B_PCH_PCIE_UES_CT                         BIT14
#define B_PCH_PCIE_UES_FCPE                       BIT13
#define B_PCH_PCIE_UES_PT                         BIT12
#define B_PCH_PCIE_UES_DLPE                       BIT4
#define B_PCH_PCIE_UES_TE                         BIT0
#define R_PCH_PCIE_UEM                            0x108
#define S_PCH_PCIE_UEM                            4
#define B_PCH_PCIE_UEM_URE                        BIT20
#define B_PCH_PCIE_UEM_EE                         BIT19
#define B_PCH_PCIE_UEM_MT                         BIT18
#define B_PCH_PCIE_UEM_RO                         BIT17
#define B_PCH_PCIE_UEM_UC                         BIT16
#define B_PCH_PCIE_UEM_CA                         BIT15
#define B_PCH_PCIE_UEM_CT                         BIT14
#define B_PCH_PCIE_UEM_FCPE                       BIT13
#define B_PCH_PCIE_UEM_PT                         BIT12
#define B_PCH_PCIE_UEM_DLPE                       BIT4
#define B_PCH_PCIE_UEM_TE                         BIT0
#define R_PCH_PCIE_UEV                            0x10C
#define S_PCH_PCIE_UEV                            4
#define B_PCH_PCIE_UEV_URE                        BIT20
#define B_PCH_PCIE_UEV_EE                         BIT19
#define B_PCH_PCIE_UEV_MT                         BIT18
#define B_PCH_PCIE_UEV_RO                         BIT17
#define B_PCH_PCIE_UEV_UC                         BIT16
#define B_PCH_PCIE_UEV_CA                         BIT15
#define B_PCH_PCIE_UEV_CT                         BIT14
#define B_PCH_PCIE_UEV_FCPE                       BIT13
#define B_PCH_PCIE_UEV_PT                         BIT12
#define B_PCH_PCIE_UEV_DLPE                       BIT4
#define B_PCH_PCIE_UEV_TE                         BIT0
#define R_PCH_PCIE_CES                            0x110
#define S_PCH_PCIE_CES                            4
#define B_PCH_PCIE_CES_ANFES                      BIT13
#define B_PCH_PCIE_CES_RTT                        BIT12
#define B_PCH_PCIE_CES_RNR                        BIT8
#define B_PCH_PCIE_CES_BD                         BIT7
#define B_PCH_PCIE_CES_BT                         BIT6
#define B_PCH_PCIE_CES_RE                         BIT0
#define R_PCH_PCIE_CEM                            0x114
#define S_PCH_PCIE_CEM                            4
#define B_PCH_PCIE_CEM_ANFEM                      BIT13
#define B_PCH_PCIE_CEM_RTT                        BIT12
#define B_PCH_PCIE_CEM_RNR                        BIT8
#define B_PCH_PCIE_CEM_BD                         BIT7
#define B_PCH_PCIE_CEM_BT                         BIT6
#define B_PCH_PCIE_CEM_RE                         BIT0
#define R_PCH_PCIE_AECC                           0x118
#define S_PCH_PCIE_AECC                           4
#define B_PCH_PCIE_AECC_ECE                       BIT8
#define B_PCH_PCIE_AECC_ECC                       BIT7
#define B_PCH_PCIE_AECC_EGE                       BIT6
#define B_PCH_PCIE_AECC_EGC                       BIT5
#define B_PCH_PCIE_AECC_FEP                       0x0000001F
#define R_PCH_PCIE_RES                            0x130
#define S_PCH_PCIE_RES                            4
#define B_PCH_PCIE_RES_AEMN                       0xF8000000
#define B_PCH_PCIE_RES_FEMR                       BIT6
#define B_PCH_PCIE_RES_NFEMR                      BIT5
#define B_PCH_PCIE_RES_FUF                        BIT4
#define B_PCH_PCIE_RES_MENR                       BIT3
#define B_PCH_PCIE_RES_ENR                        BIT2
#define B_PCH_PCIE_RES_MCR                        BIT1
#define B_PCH_PCIE_RES_CR                         BIT0
#define R_PCH_PCIE_PECR2                          0x320
#define S_PCH_PCIE_PECR2                          4
#define B_PCH_PCIE_PECR2_FIELD_1                  BIT21
#define V_PCH_PCIE_PECR2_LATGC                    (BIT8 | BIT7 | BIT6)
#define R_PCH_PCIE_PEETM                          0x324
#define S_PCH_PCIE_PEETM                          1
#define B_PCH_PCIE_PEETM_BAU                      BIT2
#define R_PCH_PCIE_PEC1                           0x330
#define S_PCH_PCIE_PEC1                           4
#define B_PCH_PCIE_PEC1_FIELD_1                   0xFF
#define R_PCH_PCIE_LTROVR                         0x400
#define R_PCH_PCIE_LTROVR2                        0x404
#define R_PCH_PCIE_L1SECH                         0x200
#define V_PCH_PCIE_L1SECH_L1SUBST_CAP_ID          0x1E
#define R_PCH_PCIE_L1SCAP                         0x204
#define R_PCH_PCIE_PCIEPMECTL                     0x420
#define B_PCH_PCIE_PCIEPMECTL_FDPGE               BIT31
#define B_PCH_PCIE_PCIEPMECTL_DLSULPGE            BIT30
#define B_PCH_PCIE_PCIEPMECTL_DLSULDLSD           BIT29
#define B_PCH_PCIE_PCIEPMECTL_L1LE                BIT17
#define V_PCH_PCIE_PCIEPMECTL_L1LTRTLV            (BIT13 | BIT12 | BIT11 | BIT10 | BIT9 | BIT8 | BIT7 | BIT6 | BIT5 | BIT4)

//
// Lock Key bit for all RW-L bits (capabilities, next capability pointer, SSID/SVID, slot register, etc)
//
#define R_PCH_PCIE_PLKCTL                         0xEA
#define B_PCH_PCIE_PLKCTL_CL                      BIT0
#define B_PCH_PCIE_PLKCTL_TLCL                    BIT1
#define B_PCH_PCIE_PLKCTL_LLCL                    BIT2
#define B_PCH_PCIE_PLKCTL_UNLOCK                  (~BIT0)

//
// Stop & scream
//
#define R_PCH_PCIE_CCSTL                           0xD0
#define B_PCH_PCIE_CCSTL_SSEN                      BIT0

//
// PCIE PCRs (PID:SPA SPB SPC SPD SPE)
//
#define R_PCH_PCR_SPX_PCD                         0x0000                  ///< Port configuration and disable
#define B_PCH_PCR_SPX_PCD_RP1FN                   (BIT2 | BIT1 | BIT0)    ///< Port 1 Function Number
#define B_PCH_PCR_SPX_PCD_RP1CH                   BIT3                    ///< Port 1 config hide
#define B_PCH_PCR_SPX_PCD_RP2FN                   (BIT6 | BIT5 | BIT4)    ///< Port 2 Function Number
#define B_PCH_PCR_SPX_PCD_RP2CH                   BIT7                    ///< Port 2 config hide
#define B_PCH_PCR_SPX_PCD_RP3FN                   (BIT10 | BIT9 | BIT8)   ///< Port 3 Function Number
#define B_PCH_PCR_SPX_PCD_RP3CH                   BIT11                   ///< Port 3 config hide
#define B_PCH_PCR_SPX_PCD_RP4FN                   (BIT14 | BIT13 | BIT12) ///< Port 4 Function Number
#define B_PCH_PCR_SPX_PCD_RP4CH                   BIT15                   ///< Port 4 config hide
#define S_PCH_PCR_SPX_PCD_RP_FIELD                4                       ///< 4 bits for each RP FN
#define B_PCH_PCR_SPX_PCD_P1D                     BIT16                   ///< Port 1 disable
#define B_PCH_PCR_SPX_PCD_P2D                     BIT17                   ///< Port 2 disable
#define B_PCH_PCR_SPX_PCD_P3D                     BIT18                   ///< Port 3 disable
#define B_PCH_PCR_SPX_PCD_P4D                     BIT19                   ///< Port 4 disable
#define B_PCH_PCR_SPX_PCD_SRL                     BIT31                   ///< Secured Register Lock

#define R_PCH_PCR_SPX_PCIEHBP                     0x0004                  ///< PCI Express high-speed bypass
#define B_PCH_PCR_SPX_PCIEHBP_PCIEHBPME           BIT0                    ///< PCIe HBP mode enable
#define B_PCH_PCR_SPX_PCIEHBP_PCIEGMO             (BIT2 | BIT1)           ///< PCIe gen mode override
#define B_PCH_PCR_SPX_PCIEHBP_PCIETIL0O           BIT3                    ///< PCIe transmitter-in-L0 override
#define B_PCH_PCR_SPX_PCIEHBP_PCIERIL0O           BIT4                    ///< PCIe receiver-in-L0 override
#define B_PCH_PCR_SPX_PCIEHBP_PCIELRO             BIT5                    ///< PCIe link recovery override
#define B_PCH_PCR_SPX_PCIEHBP_PCIELDO             BIT6                    ///< PCIe link down override
#define B_PCH_PCR_SPX_PCIEHBP_PCIESSM             BIT7                    ///< PCIe SKP suppression mode
#define B_PCH_PCR_SPX_PCIEHBP_PCIESST             BIT8                    ///< PCIe suppress SKP transmission
#define B_PCH_PCR_SPX_PCIEHBP_PCIEHBPPS           (BIT13 | BIT12)         ///< PCIe HBP port select
#define B_PCH_PCR_SPX_PCIEHBP_CRCSEL              (BIT15 | BIT14)         ///< CRC select
#define B_PCH_PCR_SPX_PCIEHBP_PCIEHBPCRC          0xFFFF0000              ///< PCIe HBP CRC

#define R_IOSFDEVCLKGCTL                          0x2E4                   // IOSF Device Clock Gate CTRL Register
#define R_SBDEVCLKGCTL                            0x2E6                   // Sideband Device Clock Gate CTRL Register
#define R_PT0TXNCLKGCTL                           0x8E4                   // Port 0 Transaction Clock Gate CTRL Register
#define R_PT1TXNCLKGCTL                           0x8E6                   // Port 1 Transaction Clock Gate CTRL Register
#define R_PT2TXNCLKGCTL                           0x8E8                   // Port 2 Transaction Clock Gate CTRL Register
#define R_PT3TXNCLKGCTL                           0x8EA                   // Port 3 Transaction Clock Gate CTRL Register
#define R_PT0LNKCLKGCTL                           0x9E4                   // Port 0 Link Clock Gate CTRL Register
#define R_PT1LNKCLKGCTL                           0x9E6                   // Port 1 Link Clock Gate CTRL Register
#define R_PT2LNKCLKGCTL                           0x9E8                   // Port 2 Link Clock Gate CTRL Register
#define R_PT3LNKCLKGCTL                           0x9EA                   // Port 3 Link Clock Gate CTRL Register
#define R_CFGPCIECLKGCTL                          0x9EC                   // Configuration PCIe Clock Gate CTRL Register
#define R_PT0PHYCLKGCTL                           0xAE4                   // Port 0 Physical Clock Gate CTRL Register
#define R_PT1PHYCLKGCTL                           0xAE6                   // Port 1 Physical Clock Gate CTRL Register
#define R_PT2PHYCLKGCTL                           0xAE8                   // Port 2 Physical Clock Gate CTRL Register
#define R_PT3PHYCLKGCTL                           0xAEA                   // Port 3 Physical Clock Gate CTRL Register
#define R_PLLSTBYCTL                              0xBAC                   // PLLSTBY Feature
#define V_PLLSTBYCTL_PSE                         (BIT0| BIT1| BIT2| BIT3) // Pll Standby Enable bitfield: one bit controls a  port
#define R_IDFDEVCLKGCTL                           0x2E8                   // enables clock gating to occur when the IP block is idle longer
#define B_IDLE_CLOCK_GATING_ENABLE                BIT15                   // This bit enables clock gating
#define B_PLL_STANDBY_ENABLE                      BIT0                    // PLL Standby Enable
#define R_PCH_PCIE_EINJCTL                        0x188                   // Error injection control register
#define V_PCH_PCIE_EINJDIS                        BIT0                    // Error injection disable

#define R_PCH_PCR_XPLDEF                          0x9A0                   // Register to control link state and enforce certain policies
#define B_PCH_PCR_XPLDEF_DIS_ASPM_L1              BIT1                    // for disabling entry into L1 regardless of ASPMCTL field in LNKCTL.
#define B_PCH_PCR_XPLDEF_DIS_L0                   BIT0                    // for disabling entry into L0s regardless of ASPMCTL field in LNKCTL.

#define  VALID_NUMBER_BIFURCATION_MODES           5
#define  BIFCTL0_LOCK                             BIT31
#define  BIFCTL0_X8                               4
#define  BIFCTL0_X4X4                             3
#define  BIFCTL0_X4X2X2                           2
#define  BIFCTL0_X2X2X4                           1
#define  BIFCTL0_X2X2X2X2                         0

#define R_PCH_LTSSMSTATEJMP                       0xA40
#define B_PCH_LTSSMSTATEJMP_HARDFORCE             BIT10                      //forces an immediate return to detect
#define B_PCH_LTSSMSTATEJMP_FORCEDETECT           BIT2                       //When released,upstream ports will follow the path as if it had a link down.

#define INVALID_ROOT_PORT_NUMBER    0
#define PCI_DEVICE_IS_NOT_PRESENT (~0)

//
// PID: PID_ICLK
//

#define R_PCH_PCR_ICLK_DWORD2                              0x208
#define PCIE1_PLLEN_HANDOFF                                BIT5  // Turn over PLL control to WM state machine
#define PCIE1_PLLG2_EN                                     BIT4  // Enable G2 PLL - only applies before handoff
#define PCIE1_PLLG3_EN                                     BIT3  // Enable G3 PLL - only applies before handoff
#define PCIE0_PLLEN_HANDOFF                                BIT2  // Turn over PLL control to WM state machine
#define PCIE0_PLLG2_EN                                     BIT1  // Enable G2 PLL - only applies before handoff
#define PCIE0_PLLG3_EN                                     BIT0  // Enable G3 PLL - only applies before handoff

#define R_PCH_PCR_ICLK_DWORD25                             0x264
#define B_PCH_PCR_ICLK_PCIE1_PLLG2_OK                      BIT3
#define B_PCH_PCR_ICLK_PCIE1_PLLG3_OK                      BIT2
#define B_PCH_PCR_ICLK_PCIE0_PLLG2_OK                      BIT1
#define B_PCH_PCR_ICLK_PCIE0_PLLG3_OK                      BIT0

#define R_PCH_PCR_PCIE_PPBIFCTLBIF                         0x10

//
// Hotplug
//
#define R_PCH_ACPI_GPE0_STS_127_96                0x8C
#define B_PCH_ACPI_GPE0_STS_127_96_HOT_PLUG       BIT1

//sighting (Lane EQ preset) = 1404805389
//soc.pcie0.port0.laneeqctl[all lanes].uptp=5
//soc.pcie1.port0.laneeqctl[all lanes].dptp=8
#define B_WM_LANEEQCTL_MASK                      (UINT16)~(BIT8|BIT9|BIT10|BIT11|BIT3|BIT2|BIT1|BIT0)
#define B_WM_LANEEQCTL_UPTP_DPTP                 (UINT16)(BIT8|BIT10|BIT3)

#endif   // _PCIE_IP_PLATFORM_H_
