/** @file
  GPIO pins for SKL-PCH-H,

@copyright
 Copyright (c) 2015 Intel Corporation. All rights reserved
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
#ifndef _GPIO_PINS_SKL_H_H_
#define _GPIO_PINS_SKL_H_H_
///
/// This header file should be used together with
/// PCH GPIO lib in C and ASL. All defines used
/// must match both ASL/C syntax
///
///
/// SKL H GPIO Groups
/// Use below for functions from PCH GPIO Lib which
/// require GpioGroup as argument
///
#define GPIO_DNV_GROUP_NC     0x0100
#define GPIO_DNV_GROUP_SC_DFX 0x0101
#define GPIO_DNV_GROUP_SC0    0x0102
#define GPIO_DNV_GROUP_SC1    0x0103
#define GPIO_DNV_GROUP_MIN    GPIO_DNV_GROUP_NC
#define GPIO_DNV_GROUP_MAX    GPIO_DNV_GROUP_SC1
#define NORTH_ALL_GBE0_SDP0            0x01000000
#define NORTH_ALL_GBE1_SDP0            0x01000001
#define NORTH_ALL_GBE0_SDP1            0x01000002
#define NORTH_ALL_GBE1_SDP1            0x01000003
#define NORTH_ALL_GBE0_SDP2            0x01000004
#define NORTH_ALL_GBE1_SDP2            0x01000005
#define NORTH_ALL_GBE0_SDP3            0x01000006
#define NORTH_ALL_GBE1_SDP3            0x01000007
#define NORTH_ALL_GBE2_LED0            0x01000008
#define NORTH_ALL_GBE2_LED1            0x01000009
#define NORTH_ALL_GBE0_I2C_CLK         0x0100000A
#define NORTH_ALL_GBE0_I2C_DATA        0x0100000B
#define NORTH_ALL_GBE1_I2C_CLK         0x0100000C
#define NORTH_ALL_GBE1_I2C_DATA        0x0100000D
#define NORTH_ALL_NCSI_RXD0            0x0100000E
#define NORTH_ALL_NCSI_CLK_IN          0x0100000F
#define NORTH_ALL_NCSI_RXD1            0x01000010
#define NORTH_ALL_NCSI_CRS_DV          0x01000011
#define NORTH_ALL_NCSI_ARB_IN          0x01000012
#define NORTH_ALL_NCSI_TX_EN           0x01000013
#define NORTH_ALL_NCSI_TXD0            0x01000014
#define NORTH_ALL_NCSI_TXD1            0x01000015
#define NORTH_ALL_NCSI_ARB_OUT         0x01000016
#define NORTH_ALL_GBE0_LED0            0x01000017
#define NORTH_ALL_GBE0_LED1            0x01000018
#define NORTH_ALL_GBE1_LED0            0x01000019
#define NORTH_ALL_GBE1_LED1            0x0100001A
#define NORTH_ALL_GPIO_0               0x0100001B
#define NORTH_ALL_PCIE_CLKREQ0_N       0x0100001C
#define NORTH_ALL_PCIE_CLKREQ1_N       0x0100001D
#define NORTH_ALL_PCIE_CLKREQ2_N       0x0100001E
#define NORTH_ALL_PCIE_CLKREQ3_N       0x0100001F
#define NORTH_ALL_PCIE_CLKREQ4_N       0x01000020
#define NORTH_ALL_GPIO_1               0x01000021
#define NORTH_ALL_GPIO_2               0x01000022
#define NORTH_ALL_SVID_ALERT_N         0x01000023
#define NORTH_ALL_SVID_DATA            0x01000024
#define NORTH_ALL_SVID_CLK             0x01000025
#define NORTH_ALL_THERMTRIP_N          0x01000026
#define NORTH_ALL_PROCHOT_N            0x01000027
#define NORTH_ALL_MEMHOT_N             0x01000028
#define SOUTH_DFX_DFX_PORT_CLK0        0x01010000
#define SOUTH_DFX_DFX_PORT_CLK1        0x01010001
#define SOUTH_DFX_DFX_PORT0            0x01010002
#define SOUTH_DFX_DFX_PORT1            0x01010003
#define SOUTH_DFX_DFX_PORT2            0x01010004
#define SOUTH_DFX_DFX_PORT3            0x01010005
#define SOUTH_DFX_DFX_PORT4            0x01010006
#define SOUTH_DFX_DFX_PORT5            0x01010007
#define SOUTH_DFX_DFX_PORT6            0x01010008
#define SOUTH_DFX_DFX_PORT7            0x01010009
#define SOUTH_DFX_DFX_PORT8            0x0101000A
#define SOUTH_DFX_DFX_PORT9            0x0101000B
#define SOUTH_DFX_DFX_PORT10           0x0101000C
#define SOUTH_DFX_DFX_PORT11           0x0101000D
#define SOUTH_DFX_DFX_PORT12           0x0101000E
#define SOUTH_DFX_DFX_PORT13           0x0101000F
#define SOUTH_DFX_DFX_PORT14           0x01010010
#define SOUTH_DFX_DFX_PORT15           0x01010011
#define SOUTH_GROUP0_SMB3_CLTT_DATA    0x01020000
#define SOUTH_GROUP0_SMB3_CLTT_CLK     0x01020001
#define SOUTH_GROUP0_GPIO_12           0x01020000
#define SOUTH_GROUP0_SMB5_GBE_ALRT_N   0x01020001
#define SOUTH_GROUP0_PCIE_CLKREQ5_N    0x01020002
#define SOUTH_GROUP0_PCIE_CLKREQ6_N    0x01020003
#define SOUTH_GROUP0_PCIE_CLKREQ7_N    0x01020004
#define SOUTH_GROUP0_UART0_RXD         0x01020005
#define SOUTH_GROUP0_UART0_TXD         0x01020006
#define SOUTH_GROUP0_SMB5_GBE_CLK      0x01020007
#define SOUTH_GROUP0_SMB5_GBE_DATA     0x01020008
#define SOUTH_GROUP0_ERROR2_N          0x01020009
#define SOUTH_GROUP0_ERROR1_N          0x0102000A
#define SOUTH_GROUP0_ERROR0_N          0x0102000B
#define SOUTH_GROUP0_IERR_N            0x0102000C
#define SOUTH_GROUP0_MCERR_N           0x0102000D
#define SOUTH_GROUP0_SMB0_LEG_CLK      0x0102000E
#define SOUTH_GROUP0_SMB0_LEG_DATA     0x0102000F
#define SOUTH_GROUP0_SMB0_LEG_ALRT_N   0x01020010
#define SOUTH_GROUP0_SMB1_HOST_DATA    0x01020011
#define SOUTH_GROUP0_SMB1_HOST_CLK     0x01020012
#define SOUTH_GROUP0_SMB2_PECI_DATA    0x01020013
#define SOUTH_GROUP0_SMB2_PECI_CLK     0x01020014
#define SOUTH_GROUP0_SMB4_CSME0_DATA   0x01020015
#define SOUTH_GROUP0_SMB4_CSME0_CLK    0x01020016
#define SOUTH_GROUP0_SMB4_CSME0_ALRT_N 0x01020017
#define SOUTH_GROUP0_USB_OC0_N         0x01020018
#define SOUTH_GROUP0_FLEX_CLK_SE0      0x01020019
#define SOUTH_GROUP0_FLEX_CLK_SE1      0x0102001A
#define SOUTH_GROUP0_GPIO_4            0x0102001B
#define SOUTH_GROUP0_GPIO_5            0x0102001C
#define SOUTH_GROUP0_GPIO_6            0x0102001D
#define SOUTH_GROUP0_GPIO_7            0x0102001E
#define SOUTH_GROUP0_SATA0_LED_N       0x0102001F
#define SOUTH_GROUP0_SATA1_LED_N       0x01020020
#define SOUTH_GROUP0_SATA_PDETECT0     0x01020021
#define SOUTH_GROUP0_SATA_PDETECT1     0x01020022
#define SOUTH_GROUP0_SATA0_SDOUT       0x01020023
#define SOUTH_GROUP0_SATA1_SDOUT       0x01020024
#define SOUTH_GROUP0_UART1_RXD         0x01020025
#define SOUTH_GROUP0_UART1_TXD         0x01020026
#define SOUTH_GROUP0_GPIO_8            0x01020027
#define SOUTH_GROUP0_GPIO_9            0x01020028
#define SOUTH_GROUP0_TCK               0x01020029
#define SOUTH_GROUP0_TRST_N            0x0102002A
#define SOUTH_GROUP0_TMS               0x0102002B
#define SOUTH_GROUP0_TDI               0x0102002C
#define SOUTH_GROUP0_TDO               0x0102002D
#define SOUTH_GROUP0_CX_PRDY_N         0x0102002E
#define SOUTH_GROUP0_CX_PREQ_N         0x0102002F
#define SOUTH_GROUP0_CTBTRIGINOUT      0x01020030
#define SOUTH_GROUP0_CTBTRIGOUT        0x01020031
#define SOUTH_GROUP0_DFX_SPARE2        0x01020032
#define SOUTH_GROUP0_DFX_SPARE3        0x01020033
#define SOUTH_GROUP0_DFX_SPARE4        0x01020034
#define SOUTH_GROUP1_SUSPWRDNACK       0x01030000
#define SOUTH_GROUP1_PMU_SUSCLK        0x01030001
#define SOUTH_GROUP1_ADR_TRIGGER       0x01030002
#define SOUTH_GROUP1_PMU_AC_PRESENT    0x01030002
#define SOUTH_GROUP1_PMU_SLP_S45_N     0x01030003
#define SOUTH_GROUP1_PMU_SLP_S3_N      0x01030004
#define SOUTH_GROUP1_PMU_WAKE_N        0x01030005
#define SOUTH_GROUP1_PMU_PWRBTN_N      0x01030006
#define SOUTH_GROUP1_PMU_RESETBUTTON_N 0x01030007
#define SOUTH_GROUP1_PMU_PLTRST_N      0x01030008
#define SOUTH_GROUP1_SUS_STAT_N        0x01030009
#define SOUTH_GROUP1_SLP_S0IX_N        0x0103000A
#define SOUTH_GROUP1_SPI_CS0_N         0x0103000B
#define SOUTH_GROUP1_SPI_CS1_N         0x0103000C
#define SOUTH_GROUP1_SPI_MOSI_IO0      0x0103000D
#define SOUTH_GROUP1_SPI_MISO_IO1      0x0103000E
#define SOUTH_GROUP1_SPI_IO2           0x0103000F
#define SOUTH_GROUP1_SPI_IO3           0x01030010
#define SOUTH_GROUP1_SPI_CLK           0x01030011
#define SOUTH_GROUP1_SPI_CLK_LOOPBK    0x01030012
#define SOUTH_GROUP1_ESPI_IO0          0x01030013
#define SOUTH_GROUP1_ESPI_IO1          0x01030014
#define SOUTH_GROUP1_ESPI_IO2          0x01030015
#define SOUTH_GROUP1_ESPI_IO3          0x01030016
#define SOUTH_GROUP1_ESPI_CS0_N        0x01030017
#define SOUTH_GROUP1_ESPI_CLK          0x01030018
#define SOUTH_GROUP1_ESPI_RST_N        0x01030019
#define SOUTH_GROUP1_ESPI_ALRT0_N      0x0103001A
#define SOUTH_GROUP1_GPIO_10           0x0103001B
#define SOUTH_GROUP1_GPIO_11           0x0103001C
#define SOUTH_GROUP1_ESPI_CLK_LOOPBK   0x0103001D
#define SOUTH_GROUP1_EMMC_CMD          0x0103001E
#define SOUTH_GROUP1_EMMC_STROBE       0x0103001F
#define SOUTH_GROUP1_EMMC_CLK          0x01030020
#define SOUTH_GROUP1_EMMC_D0           0x01030021
#define SOUTH_GROUP1_EMMC_D1           0x01030022
#define SOUTH_GROUP1_EMMC_D2           0x01030023
#define SOUTH_GROUP1_EMMC_D3           0x01030024
#define SOUTH_GROUP1_EMMC_D4           0x01030025
#define SOUTH_GROUP1_EMMC_D5           0x01030026
#define SOUTH_GROUP1_EMMC_D6           0x01030027
#define SOUTH_GROUP1_EMMC_D7           0x01030028
#define SOUTH_GROUP1_GPIO_3            0x01030029

///
/// SKL H GPIO pins
/// Use below for functions from PCH GPIO Lib which
/// require GpioPad as argument. Encoding used here
/// has all information required by library functions
///
#define GPIO_SKL_H_GPP_A0       0x01000000
#define GPIO_SKL_H_GPP_A1       0x01000001
#define GPIO_SKL_H_GPP_A2       0x01000002
#define GPIO_SKL_H_GPP_A3       0x01000003
#define GPIO_SKL_H_GPP_A4       0x01000004
#define GPIO_SKL_H_GPP_A5       0x01000005
#define GPIO_SKL_H_GPP_A6       0x01000006
#define GPIO_SKL_H_GPP_A7       0x01000007
#define GPIO_SKL_H_GPP_A8       0x01000008
#define GPIO_SKL_H_GPP_A9       0x01000009
#define GPIO_SKL_H_GPP_A10      0x0100000A
#define GPIO_SKL_H_GPP_A11      0x0100000B
#define GPIO_SKL_H_GPP_A12      0x0100000C
#define GPIO_SKL_H_GPP_A13      0x0100000D
#define GPIO_SKL_H_GPP_A14      0x0100000E
#define GPIO_SKL_H_GPP_A15      0x0100000F
#define GPIO_SKL_H_GPP_A16      0x01000010
#define GPIO_SKL_H_GPP_A17      0x01000011
#define GPIO_SKL_H_GPP_A18      0x01000012
#define GPIO_SKL_H_GPP_A19      0x01000013
#define GPIO_SKL_H_GPP_A20      0x01000014
#define GPIO_SKL_H_GPP_A21      0x01000015
#define GPIO_SKL_H_GPP_A22      0x01000016
#define GPIO_SKL_H_GPP_A23      0x01000017
#define GPIO_SKL_H_GPP_B0       0x01010000
#define GPIO_SKL_H_GPP_B1       0x01010001
#define GPIO_SKL_H_GPP_B2       0x01010002
#define GPIO_SKL_H_GPP_B3       0x01010003
#define GPIO_SKL_H_GPP_B4       0x01010004
#define GPIO_SKL_H_GPP_B5       0x01010005
#define GPIO_SKL_H_GPP_B6       0x01010006
#define GPIO_SKL_H_GPP_B7       0x01010007
#define GPIO_SKL_H_GPP_B8       0x01010008
#define GPIO_SKL_H_GPP_B9       0x01010009
#define GPIO_SKL_H_GPP_B10      0x0101000A
#define GPIO_SKL_H_GPP_B11      0x0101000B
#define GPIO_SKL_H_GPP_B12      0x0101000C
#define GPIO_SKL_H_GPP_B13      0x0101000D
#define GPIO_SKL_H_GPP_B14      0x0101000E
#define GPIO_SKL_H_GPP_B15      0x0101000F
#define GPIO_SKL_H_GPP_B16      0x01010010
#define GPIO_SKL_H_GPP_B17      0x01010011
#define GPIO_SKL_H_GPP_B18      0x01010012
#define GPIO_SKL_H_GPP_B19      0x01010013
#define GPIO_SKL_H_GPP_B20      0x01010014
#define GPIO_SKL_H_GPP_B21      0x01010015
#define GPIO_SKL_H_GPP_B22      0x01010016
#define GPIO_SKL_H_GPP_B23      0x01010017
#define GPIO_SKL_H_GPP_C0       0x01020000
#define GPIO_SKL_H_GPP_C1       0x01020001
#define GPIO_SKL_H_GPP_C2       0x01020002
#define GPIO_SKL_H_GPP_C3       0x01020003
#define GPIO_SKL_H_GPP_C4       0x01020004
#define GPIO_SKL_H_GPP_C5       0x01020005
#define GPIO_SKL_H_GPP_C6       0x01020006
#define GPIO_SKL_H_GPP_C7       0x01020007
#define GPIO_SKL_H_GPP_C8       0x01020008
#define GPIO_SKL_H_GPP_C9       0x01020009
#define GPIO_SKL_H_GPP_C10      0x0102000A
#define GPIO_SKL_H_GPP_C11      0x0102000B
#define GPIO_SKL_H_GPP_C12      0x0102000C
#define GPIO_SKL_H_GPP_C13      0x0102000D
#define GPIO_SKL_H_GPP_C14      0x0102000E
#define GPIO_SKL_H_GPP_C15      0x0102000F
#define GPIO_SKL_H_GPP_C16      0x01020010
#define GPIO_SKL_H_GPP_C17      0x01020011
#define GPIO_SKL_H_GPP_C18      0x01020012
#define GPIO_SKL_H_GPP_C19      0x01020013
#define GPIO_SKL_H_GPP_C20      0x01020014
#define GPIO_SKL_H_GPP_C21      0x01020015
#define GPIO_SKL_H_GPP_C22      0x01020016
#define GPIO_SKL_H_GPP_C23      0x01020017
#define GPIO_SKL_H_GPP_D0       0x01030000
#define GPIO_SKL_H_GPP_D1       0x01030001
#define GPIO_SKL_H_GPP_D2       0x01030002
#define GPIO_SKL_H_GPP_D3       0x01030003
#define GPIO_SKL_H_GPP_D4       0x01030004
#define GPIO_SKL_H_GPP_D5       0x01030005
#define GPIO_SKL_H_GPP_D6       0x01030006
#define GPIO_SKL_H_GPP_D7       0x01030007
#define GPIO_SKL_H_GPP_D8       0x01030008
#define GPIO_SKL_H_GPP_D9       0x01030009
#define GPIO_SKL_H_GPP_D10      0x0103000A
#define GPIO_SKL_H_GPP_D11      0x0103000B
#define GPIO_SKL_H_GPP_D12      0x0103000C
#define GPIO_SKL_H_GPP_D13      0x0103000D
#define GPIO_SKL_H_GPP_D14      0x0103000E
#define GPIO_SKL_H_GPP_D15      0x0103000F
#define GPIO_SKL_H_GPP_D16      0x01030010
#define GPIO_SKL_H_GPP_D17      0x01030011
#define GPIO_SKL_H_GPP_D18      0x01030012
#define GPIO_SKL_H_GPP_D19      0x01030013
#define GPIO_SKL_H_GPP_D20      0x01030014
#define GPIO_SKL_H_GPP_D21      0x01030015
#define GPIO_SKL_H_GPP_D22      0x01030016
#define GPIO_SKL_H_GPP_D23      0x01030017
#define GPIO_SKL_H_GPP_E0       0x01040000
#define GPIO_SKL_H_GPP_E1       0x01040001
#define GPIO_SKL_H_GPP_E2       0x01040002
#define GPIO_SKL_H_GPP_E3       0x01040003
#define GPIO_SKL_H_GPP_E4       0x01040004
#define GPIO_SKL_H_GPP_E5       0x01040005
#define GPIO_SKL_H_GPP_E6       0x01040006
#define GPIO_SKL_H_GPP_E7       0x01040007
#define GPIO_SKL_H_GPP_E8       0x01040008
#define GPIO_SKL_H_GPP_E9       0x01040009
#define GPIO_SKL_H_GPP_E10      0x0104000A
#define GPIO_SKL_H_GPP_E11      0x0104000B
#define GPIO_SKL_H_GPP_E12      0x0104000C
#define GPIO_SKL_H_GPP_F0       0x01050000
#define GPIO_SKL_H_GPP_F1       0x01050001
#define GPIO_SKL_H_GPP_F2       0x01050002
#define GPIO_SKL_H_GPP_F3       0x01050003
#define GPIO_SKL_H_GPP_F4       0x01050004
#define GPIO_SKL_H_GPP_F5       0x01050005
#define GPIO_SKL_H_GPP_F6       0x01050006
#define GPIO_SKL_H_GPP_F7       0x01050007
#define GPIO_SKL_H_GPP_F8       0x01050008
#define GPIO_SKL_H_GPP_F9       0x01050009
#define GPIO_SKL_H_GPP_F10      0x0105000A
#define GPIO_SKL_H_GPP_F11      0x0105000B
#define GPIO_SKL_H_GPP_F12      0x0105000C
#define GPIO_SKL_H_GPP_F13      0x0105000D
#define GPIO_SKL_H_GPP_F14      0x0105000E
#define GPIO_SKL_H_GPP_F15      0x0105000F
#define GPIO_SKL_H_GPP_F16      0x01050010
#define GPIO_SKL_H_GPP_F17      0x01050011
#define GPIO_SKL_H_GPP_F18      0x01050012
#define GPIO_SKL_H_GPP_F19      0x01050013
#define GPIO_SKL_H_GPP_F20      0x01050014
#define GPIO_SKL_H_GPP_F21      0x01050015
#define GPIO_SKL_H_GPP_F22      0x01050016
#define GPIO_SKL_H_GPP_F23      0x01050017
#define GPIO_SKL_H_GPP_G0       0x01060000
#define GPIO_SKL_H_GPP_G1       0x01060001
#define GPIO_SKL_H_GPP_G2       0x01060002
#define GPIO_SKL_H_GPP_G3       0x01060003
#define GPIO_SKL_H_GPP_G4       0x01060004
#define GPIO_SKL_H_GPP_G5       0x01060005
#define GPIO_SKL_H_GPP_G6       0x01060006
#define GPIO_SKL_H_GPP_G7       0x01060007
#define GPIO_SKL_H_GPP_G8       0x01060008
#define GPIO_SKL_H_GPP_G9       0x01060009
#define GPIO_SKL_H_GPP_G10      0x0106000A
#define GPIO_SKL_H_GPP_G11      0x0106000B
#define GPIO_SKL_H_GPP_G12      0x0106000C
#define GPIO_SKL_H_GPP_G13      0x0106000D
#define GPIO_SKL_H_GPP_G14      0x0106000E
#define GPIO_SKL_H_GPP_G15      0x0106000F
#define GPIO_SKL_H_GPP_G16      0x01060010
#define GPIO_SKL_H_GPP_G17      0x01060011
#define GPIO_SKL_H_GPP_G18      0x01060012
#define GPIO_SKL_H_GPP_G19      0x01060013
#define GPIO_SKL_H_GPP_G20      0x01060014
#define GPIO_SKL_H_GPP_G21      0x01060015
#define GPIO_SKL_H_GPP_G22      0x01060016
#define GPIO_SKL_H_GPP_G23      0x01060017
#define GPIO_SKL_H_GPP_H0       0x01070000
#define GPIO_SKL_H_GPP_H1       0x01070001
#define GPIO_SKL_H_GPP_H2       0x01070002
#define GPIO_SKL_H_GPP_H3       0x01070003
#define GPIO_SKL_H_GPP_H4       0x01070004
#define GPIO_SKL_H_GPP_H5       0x01070005
#define GPIO_SKL_H_GPP_H6       0x01070006
#define GPIO_SKL_H_GPP_H7       0x01070007
#define GPIO_SKL_H_GPP_H8       0x01070008
#define GPIO_SKL_H_GPP_H9       0x01070009
#define GPIO_SKL_H_GPP_H10      0x0107000A
#define GPIO_SKL_H_GPP_H11      0x0107000B
#define GPIO_SKL_H_GPP_H12      0x0107000C
#define GPIO_SKL_H_GPP_H13      0x0107000D
#define GPIO_SKL_H_GPP_H14      0x0107000E
#define GPIO_SKL_H_GPP_H15      0x0107000F
#define GPIO_SKL_H_GPP_H16      0x01070010
#define GPIO_SKL_H_GPP_H17      0x01070011
#define GPIO_SKL_H_GPP_H18      0x01070012
#define GPIO_SKL_H_GPP_H19      0x01070013
#define GPIO_SKL_H_GPP_H20      0x01070014
#define GPIO_SKL_H_GPP_H21      0x01070015
#define GPIO_SKL_H_GPP_H22      0x01070016
#define GPIO_SKL_H_GPP_H23      0x01070017
#define GPIO_SKL_H_GPP_I0       0x01080000
#define GPIO_SKL_H_GPP_I1       0x01080001
#define GPIO_SKL_H_GPP_I2       0x01080002
#define GPIO_SKL_H_GPP_I3       0x01080003
#define GPIO_SKL_H_GPP_I4       0x01080004
#define GPIO_SKL_H_GPP_I5       0x01080005
#define GPIO_SKL_H_GPP_I6       0x01080006
#define GPIO_SKL_H_GPP_I7       0x01080007
#define GPIO_SKL_H_GPP_I8       0x01080008
#define GPIO_SKL_H_GPP_I9       0x01080009
#define GPIO_SKL_H_GPP_I10      0x0108000A
#define GPIO_SKL_H_GPD0         0x01090000
#define GPIO_SKL_H_GPD1         0x01090001
#define GPIO_SKL_H_GPD2         0x01090002
#define GPIO_SKL_H_GPD3         0x01090003
#define GPIO_SKL_H_GPD4         0x01090004
#define GPIO_SKL_H_GPD5         0x01090005
#define GPIO_SKL_H_GPD6         0x01090006
#define GPIO_SKL_H_GPD7         0x01090007
#define GPIO_SKL_H_GPD8         0x01090008
#define GPIO_SKL_H_GPD9         0x01090009
#define GPIO_SKL_H_GPD10        0x0109000A
#define GPIO_SKL_H_GPD11        0x0109000B


#include "GpioConfig.h"
static GPIO_PAD mDenvertonGpioPads[] = {
    NORTH_ALL_GBE0_SDP0           ,
    NORTH_ALL_GBE1_SDP0           ,
    NORTH_ALL_GBE0_SDP1           ,
    NORTH_ALL_GBE1_SDP1           ,
    NORTH_ALL_GBE0_SDP2           ,
    NORTH_ALL_GBE1_SDP2           ,
    NORTH_ALL_GBE0_SDP3           ,
    NORTH_ALL_GBE1_SDP3           ,
    NORTH_ALL_GBE2_LED0           ,
    NORTH_ALL_GBE2_LED1           ,
    NORTH_ALL_GBE0_I2C_CLK        ,
    NORTH_ALL_GBE0_I2C_DATA       ,
    NORTH_ALL_GBE1_I2C_CLK        ,
    NORTH_ALL_GBE1_I2C_DATA       ,
    NORTH_ALL_NCSI_RXD0           ,
    NORTH_ALL_NCSI_CLK_IN         ,
    NORTH_ALL_NCSI_RXD1           ,
    NORTH_ALL_NCSI_CRS_DV         ,
    NORTH_ALL_NCSI_ARB_IN         ,
    NORTH_ALL_NCSI_TX_EN          ,
    NORTH_ALL_NCSI_TXD0           ,
    NORTH_ALL_NCSI_TXD1           ,
    NORTH_ALL_NCSI_ARB_OUT        ,
    NORTH_ALL_GBE0_LED0           ,
    NORTH_ALL_GBE0_LED1           ,
    NORTH_ALL_GBE1_LED0           ,
    NORTH_ALL_GBE1_LED1           ,
    NORTH_ALL_GPIO_0              ,
    NORTH_ALL_PCIE_CLKREQ0_N      ,
    NORTH_ALL_PCIE_CLKREQ1_N      ,
    NORTH_ALL_PCIE_CLKREQ2_N      ,
    NORTH_ALL_PCIE_CLKREQ3_N      ,
    NORTH_ALL_PCIE_CLKREQ4_N      ,
    NORTH_ALL_GPIO_1              ,
    NORTH_ALL_GPIO_2              ,
    NORTH_ALL_SVID_ALERT_N        ,
    NORTH_ALL_SVID_DATA           ,
    NORTH_ALL_SVID_CLK            ,
    NORTH_ALL_THERMTRIP_N         ,
    NORTH_ALL_PROCHOT_N           ,
    NORTH_ALL_MEMHOT_N            ,
    SOUTH_DFX_DFX_PORT_CLK0       ,
    SOUTH_DFX_DFX_PORT_CLK1       ,
    SOUTH_DFX_DFX_PORT0           ,
    SOUTH_DFX_DFX_PORT1           ,
    SOUTH_DFX_DFX_PORT2           ,
    SOUTH_DFX_DFX_PORT3           ,
    SOUTH_DFX_DFX_PORT4           ,
    SOUTH_DFX_DFX_PORT5           ,
    SOUTH_DFX_DFX_PORT6           ,
    SOUTH_DFX_DFX_PORT7           ,
    SOUTH_DFX_DFX_PORT8           ,
    SOUTH_DFX_DFX_PORT9           ,
    SOUTH_DFX_DFX_PORT10          ,
    SOUTH_DFX_DFX_PORT11          ,
    SOUTH_DFX_DFX_PORT12          ,
    SOUTH_DFX_DFX_PORT13          ,
    SOUTH_DFX_DFX_PORT14          ,
    SOUTH_DFX_DFX_PORT15          ,
    SOUTH_GROUP0_SMB3_CLTT_DATA   ,
    SOUTH_GROUP0_SMB3_CLTT_CLK    ,
    SOUTH_GROUP0_PCIE_CLKREQ5_N   ,
    SOUTH_GROUP0_PCIE_CLKREQ6_N   ,
    SOUTH_GROUP0_PCIE_CLKREQ7_N   ,
    SOUTH_GROUP0_UART0_RXD        ,
    SOUTH_GROUP0_UART0_TXD        ,
    SOUTH_GROUP0_SMB5_GBE_CLK     ,
    SOUTH_GROUP0_SMB5_GBE_DATA    ,
    SOUTH_GROUP0_ERROR2_N         ,
    SOUTH_GROUP0_ERROR1_N         ,
    SOUTH_GROUP0_ERROR0_N         ,
    SOUTH_GROUP0_IERR_N           ,
    SOUTH_GROUP0_MCERR_N          ,
    SOUTH_GROUP0_SMB0_LEG_CLK     ,
    SOUTH_GROUP0_SMB0_LEG_DATA    ,
    SOUTH_GROUP0_SMB0_LEG_ALRT_N  ,
    SOUTH_GROUP0_SMB1_HOST_DATA   ,
    SOUTH_GROUP0_SMB1_HOST_CLK    ,
    SOUTH_GROUP0_SMB2_PECI_DATA   ,
    SOUTH_GROUP0_SMB2_PECI_CLK    ,
    SOUTH_GROUP0_SMB4_CSME0_DATA  ,
    SOUTH_GROUP0_SMB4_CSME0_CLK   ,
    SOUTH_GROUP0_SMB4_CSME0_ALRT_N,
    SOUTH_GROUP0_USB_OC0_N        ,
    SOUTH_GROUP0_FLEX_CLK_SE0     ,
    SOUTH_GROUP0_FLEX_CLK_SE1     ,
    SOUTH_GROUP0_GPIO_4           ,
    SOUTH_GROUP0_GPIO_5           ,
    SOUTH_GROUP0_GPIO_6           ,
    SOUTH_GROUP0_GPIO_7           ,
    SOUTH_GROUP0_SATA0_LED_N      ,
    SOUTH_GROUP0_SATA1_LED_N      ,
    SOUTH_GROUP0_SATA_PDETECT0    ,
    SOUTH_GROUP0_SATA_PDETECT1    ,
    SOUTH_GROUP0_SATA0_SDOUT      ,
    SOUTH_GROUP0_SATA1_SDOUT      ,
    SOUTH_GROUP0_UART1_RXD        ,
    SOUTH_GROUP0_UART1_TXD        ,
    SOUTH_GROUP0_GPIO_8           ,
    SOUTH_GROUP0_GPIO_9           ,
    SOUTH_GROUP0_TCK              ,
    SOUTH_GROUP0_TRST_N           ,
    SOUTH_GROUP0_TMS              ,
    SOUTH_GROUP0_TDI              ,
    SOUTH_GROUP0_TDO              ,
    SOUTH_GROUP0_CX_PRDY_N        ,
    SOUTH_GROUP0_CX_PREQ_N        ,
    SOUTH_GROUP0_CTBTRIGINOUT     ,
    SOUTH_GROUP0_CTBTRIGOUT       ,
    SOUTH_GROUP0_DFX_SPARE2       ,
    SOUTH_GROUP0_DFX_SPARE3       ,
    SOUTH_GROUP0_DFX_SPARE4       ,
    SOUTH_GROUP1_SUSPWRDNACK      ,
    SOUTH_GROUP1_PMU_SUSCLK       ,
    SOUTH_GROUP1_PMU_AC_PRESENT   ,
    SOUTH_GROUP1_PMU_SLP_S45_N    ,
    SOUTH_GROUP1_PMU_SLP_S3_N     ,
    SOUTH_GROUP1_PMU_WAKE_N       ,
    SOUTH_GROUP1_PMU_PWRBTN_N     ,
    SOUTH_GROUP1_PMU_RESETBUTTON_N,
    SOUTH_GROUP1_PMU_PLTRST_N     ,
    SOUTH_GROUP1_SUS_STAT_N       ,
    SOUTH_GROUP1_SLP_S0IX_N       ,
    SOUTH_GROUP1_SPI_CS0_N        ,
    SOUTH_GROUP1_SPI_CS1_N        ,
    SOUTH_GROUP1_SPI_MOSI_IO0     ,
    SOUTH_GROUP1_SPI_MISO_IO1     ,
    SOUTH_GROUP1_SPI_IO2          ,
    SOUTH_GROUP1_SPI_IO3          ,
    SOUTH_GROUP1_SPI_CLK          ,
    SOUTH_GROUP1_SPI_CLK_LOOPBK   ,
    SOUTH_GROUP1_ESPI_IO0         ,
    SOUTH_GROUP1_ESPI_IO1         ,
    SOUTH_GROUP1_ESPI_IO2         ,
    SOUTH_GROUP1_ESPI_IO3         ,
    SOUTH_GROUP1_ESPI_CS0_N       ,
    SOUTH_GROUP1_ESPI_CLK         ,
    SOUTH_GROUP1_ESPI_RST_N       ,
    SOUTH_GROUP1_ESPI_ALRT0_N     ,
    SOUTH_GROUP1_GPIO_10          ,
    SOUTH_GROUP1_GPIO_11          ,
    SOUTH_GROUP1_ESPI_CLK_LOOPBK  ,
    SOUTH_GROUP1_EMMC_CMD         ,
    SOUTH_GROUP1_EMMC_STROBE      ,
    SOUTH_GROUP1_EMMC_CLK         ,
    SOUTH_GROUP1_EMMC_D0          ,
    SOUTH_GROUP1_EMMC_D1          ,
    SOUTH_GROUP1_EMMC_D2          ,
    SOUTH_GROUP1_EMMC_D3          ,
    SOUTH_GROUP1_EMMC_D4          ,
    SOUTH_GROUP1_EMMC_D5          ,
    SOUTH_GROUP1_EMMC_D6          ,
    SOUTH_GROUP1_EMMC_D7          ,
    SOUTH_GROUP1_GPIO_3
};

#endif
