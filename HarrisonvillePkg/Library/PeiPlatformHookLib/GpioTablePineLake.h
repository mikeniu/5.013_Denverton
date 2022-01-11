/*
 * GpioTablePineLake.h
 *
 *@copyright
  Copyright (c) 2015 - 2017 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor. This file may
  be modified by the user, subject to additional terms of the
  license agreement.
 *  Created on: Mar Apr 3, 2015
 *      Author: jjdaly
 */

#ifndef GPIOTABLEPINELAKE_H_
#define GPIOTABLEPINELAKE_H_
static GPIO_INIT_CONFIG mPineLakeCfioInitTable [] = {
        // NC (GPIO_14)
        { NORTH_ALL_GBE0_SDP0           , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirNone  , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
        // SFP0_PRESENT (GPIO_15)
		{ NORTH_ALL_GBE1_SDP0           , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirIn    , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
        // NC (GPIO_16)
        { NORTH_ALL_GBE0_SDP1           , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirNone  , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
        // NC (GPIO_17)
        { NORTH_ALL_GBE1_SDP1           , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirNone  , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
        // NC (GPIO_18)
        { NORTH_ALL_GBE0_SDP2           , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirNone  , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
        // NC (GPIO_19)
        { NORTH_ALL_GBE1_SDP2           , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirNone  , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
        // NC (GPIO_20)
        { NORTH_ALL_GBE0_SDP3           , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirNone  , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
        // NC (GPIO_21)
        { NORTH_ALL_GBE1_SDP3           , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirNone  , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
        // NC (GPIO_22)
        { NORTH_ALL_GBE2_LED0           , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirNone  , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
        // NC (GPIO_23)
        { NORTH_ALL_GBE2_LED1           , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirNone  , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
        // NC (GPIO_24)
        { NORTH_ALL_GBE0_I2C_CLK        , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirNone  , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
        // NC (GPIO_25)
        { NORTH_ALL_GBE0_I2C_DATA       , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirNone  , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
        // LAN0_P1_I2C_CLK (GPIO_26)
        { NORTH_ALL_GBE1_I2C_CLK        , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
        // LAN0_P1_I2C_DATA (GPIO_27)
        { NORTH_ALL_GBE1_I2C_DATA       , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirInOut , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
        // NC (GPIO_28)
        { NORTH_ALL_NCSI_RXD0           , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirNone  , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
        // SOC_NCSI_CLK_IN PD (GPIO_29)
        { NORTH_ALL_NCSI_CLK_IN         , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirNone  , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
        // NC (GPIO_30)
        { NORTH_ALL_NCSI_RXD1           , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirNone  , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
        // NC (GPIO_31)
        { NORTH_ALL_NCSI_CRS_DV         , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirNone  , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
        // NC (GPIO_32)
        { NORTH_ALL_NCSI_ARB_IN         , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirNone  , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
        // SOC_NCSI_CLK_TX_EN PD (GPIO_33)
        { NORTH_ALL_NCSI_TX_EN          , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirNone  , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
        // SOC_NCSI_CLK_TXD0 PD (GPIO_34)
		{ NORTH_ALL_NCSI_TXD0           , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirNone  , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_NCSI_CLK_TXD1 PD (GPIO_35)
        { NORTH_ALL_NCSI_TXD1           , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirNone  , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// NC (GPIO_36)
        { NORTH_ALL_NCSI_ARB_OUT        , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirNone  , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// LAN0_P0_LED0 (GPIO_37)
        { NORTH_ALL_GBE0_LED0           , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// LAN0_P0_LED1 (GPIO_38)
        { NORTH_ALL_GBE0_LED1           , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// LAN0_P1_LED0 (GPIO_39)
        { NORTH_ALL_GBE1_LED0           , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// LAN0_P1_LED1 (GPIO_40)
        { NORTH_ALL_GBE1_LED1           , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// NC (GPIO_0)
        { NORTH_ALL_GPIO_0              , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirNone  , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// NC (GPIO_41)
        { NORTH_ALL_PCIE_CLKREQ0_N      , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirNone  , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_CLKREQ1_N (GPIO_42)
        { NORTH_ALL_PCIE_CLKREQ1_N      , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirIn    , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_CLKREQ2_N (GPIO_43)
        { NORTH_ALL_PCIE_CLKREQ2_N      , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirIn    , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// NC (GPIO_44)
        { NORTH_ALL_PCIE_CLKREQ3_N      , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirNone  , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// NC (GPIO_45)
        { NORTH_ALL_PCIE_CLKREQ4_N      , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirNone  , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// LAN_MDC (GPIO_1)
        { NORTH_ALL_GPIO_1              , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// LAN_MDIO (GPIO_2)
        { NORTH_ALL_GPIO_2              , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirInOut , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SVID_ALERT_N (GPIO_47)
        { NORTH_ALL_SVID_ALERT_N        , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirIn    , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SVID_DATA (GPIO_48)
        { NORTH_ALL_SVID_DATA           , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirInOut , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SVID_CLK (GPIO_49)
        { NORTH_ALL_SVID_CLK            , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_THERMTRIP_N (GPIO_50)
        { NORTH_ALL_THERMTRIP_N         , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_PROCHOT_N (GPIO_51)
        { NORTH_ALL_PROCHOT_N           , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirIn    , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_MEMHOT_N (GPIO_52)
        { NORTH_ALL_MEMHOT_N            , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirIn    , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// LTB_DFX_PORT_CLK0 (GPIO_53)
        { SOUTH_DFX_DFX_PORT_CLK0       , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// LTB_DFX_PORT_CLK1 (GPIO_54)
        { SOUTH_DFX_DFX_PORT_CLK1       , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_DFX_PORT0 (GPIO_55)
        { SOUTH_DFX_DFX_PORT0           , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirInOut , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_DFX_PORT1 (GPIO_56)
        { SOUTH_DFX_DFX_PORT1           , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirInOut , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_DFX_PORT2 (GPIO_57)
        { SOUTH_DFX_DFX_PORT2           , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirInOut , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_DFX_PORT3 (GPIO_58)
        { SOUTH_DFX_DFX_PORT3           , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirInOut , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_DFX_PORT4 (GPIO_59)
        { SOUTH_DFX_DFX_PORT4           , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirInOut , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_DFX_PORT5 (GPIO_60)
        { SOUTH_DFX_DFX_PORT5           , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirInOut , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_DFX_PORT6 (GPIO_61)
        { SOUTH_DFX_DFX_PORT6           , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirInOut , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_DFX_PORT7 (GPIO_62)
        { SOUTH_DFX_DFX_PORT7           , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirInOut , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_DFX_PORT8 (GPIO_63)
        { SOUTH_DFX_DFX_PORT8           , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirInOut , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_DFX_PORT9 (GPIO_134)
        { SOUTH_DFX_DFX_PORT9           , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirInOut , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_DFX_PORT10 (GPIO_135)
        { SOUTH_DFX_DFX_PORT10          , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirInOut , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_DFX_PORT11 (GPIO_136)
        { SOUTH_DFX_DFX_PORT11          , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirInOut , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_DFX_PORT12 (GPIO_137)
        { SOUTH_DFX_DFX_PORT12          , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirInOut , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_DFX_PORT13 (GPIO_138)
        { SOUTH_DFX_DFX_PORT13          , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirInOut , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_DFX_PORT14 (GPIO_139)
        { SOUTH_DFX_DFX_PORT14          , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirInOut , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_DFX_PORT15 (GPIO_140)
        { SOUTH_DFX_DFX_PORT15          , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirInOut , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
        // SPI_TPM_CS_N_GP12 (GPIO_12)
        { SOUTH_GROUP0_GPIO_12          , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
        // SMB_LAN_ALRT_N (GPIO_13)
        { SOUTH_GROUP0_SMB5_GBE_ALRT_N  , {GpioPadModeNative3  , GpioHostOwnGpio, GpioDirIn    , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// NC (GPIO_98)
        { SOUTH_GROUP0_PCIE_CLKREQ5_N   , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirNone  , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// NC (GPIO_99)
        { SOUTH_GROUP0_PCIE_CLKREQ6_N   , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirNone  , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// NC (GPIO_100)
        { SOUTH_GROUP0_PCIE_CLKREQ7_N   , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirNone  , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_UART0_RXD (GPIO_101)
        { SOUTH_GROUP0_UART0_RXD        , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirIn    , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_UART0_TXD (GPIO_102)
        { SOUTH_GROUP0_UART0_TXD        , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SMB_LAN_CLK (GPIO_103)
        { SOUTH_GROUP0_SMB5_GBE_CLK     , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SMB_LAN_DATA (GPIO_104)
        { SOUTH_GROUP0_SMB5_GBE_DATA    , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirInOut , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_ERROR_N2 (GPIO_105)
        { SOUTH_GROUP0_ERROR2_N         , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_ERROR_N1 (GPIO_106)
        { SOUTH_GROUP0_ERROR1_N         , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_ERROR_N0 (GPIO_107)
        { SOUTH_GROUP0_ERROR0_N         , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_IERR_N (GPIO_108)
        { SOUTH_GROUP0_IERR_N           , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_MCERR_N (GPIO_109)
        { SOUTH_GROUP0_MCERR_N          , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SMB_LEG_CLK (GPIO_110)
        { SOUTH_GROUP0_SMB0_LEG_CLK     , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SMB_LEG_DATA (GPIO_111)
        { SOUTH_GROUP0_SMB0_LEG_DATA    , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirInOut , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SMB_LEG_ALRT_N (GPIO_112)
        { SOUTH_GROUP0_SMB0_LEG_ALRT_N  , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirIn    , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SMB_HOST_DATA (GPIO_113)
//        { SOUTH_GROUP0_SMB1_HOST_DATA   , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirInOut , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SMB_HOST_CLK (GPIO_114)
//        { SOUTH_GROUP0_SMB1_HOST_CLK    , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SMB_PECI_DATA (GPIO_115)
        { SOUTH_GROUP0_SMB2_PECI_DATA   , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirInOut , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SMB_PECI_CLK (GPIO_116)
        { SOUTH_GROUP0_SMB2_PECI_CLK    , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// NC (GPIO_117)
 //       { SOUTH_GROUP0_SMB4_CSME0_DATA  , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirNone  , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// NC (GPIO_118)
 //       { SOUTH_GROUP0_SMB4_CSME0_CLK   , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirNone  , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// NC (GPIO_119)
 //       { SOUTH_GROUP0_SMB4_CSME0_ALRT_N, {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirNone  , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// USB_OC_N (GPIO_120)
        { SOUTH_GROUP0_USB_OC0_N        , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirIn    , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// NC (GPIO_121)
        { SOUTH_GROUP0_FLEX_CLK_SE0     , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirNone  , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// NC (GPIO_122)
        { SOUTH_GROUP0_FLEX_CLK_SE1     , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirNone  , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SW_MDC (GPIO_4)
        { SOUTH_GROUP0_GPIO_4           , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SW_MDIO (GPIO_5)
        { SOUTH_GROUP0_GPIO_5           , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirInOut , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// W_DISABLE_B_N (GPIO_6)
        { SOUTH_GROUP0_GPIO_6           , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// W_DISABLE_A_N (GPIO_7)
        { SOUTH_GROUP0_GPIO_7           , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_SATA0_LED_N (GPIO_90)
        { SOUTH_GROUP0_SATA0_LED_N      , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_SATA1_LED_N (GPIO_91)
        { SOUTH_GROUP0_SATA1_LED_N      , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_SATA_PDETECT0 PU (GPIO_92)
        { SOUTH_GROUP0_SATA_PDETECT0    , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirNone   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_SATA_PDETECT1 PU (GPIO_93)
        { SOUTH_GROUP0_SATA_PDETECT1    , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirNone   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// NC (GPIO_94)
        { SOUTH_GROUP0_SATA0_SDOUT      , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirNone  , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// NC (GPIO_95)
        { SOUTH_GROUP0_SATA1_SDOUT      , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirNone  , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_UART1_RXD (GPIO_96)
        { SOUTH_GROUP0_UART1_RXD        , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirIn    , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_UART1_TXD (GPIO_97)
        { SOUTH_GROUP0_UART1_TXD        , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_GPIO10 (GPIO_8)
//        { SOUTH_GROUP0_GPIO_8           , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirInOut , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,   //ADR_CFG_CLK
		// SW_INT_N (GPIO_9)
//        { SOUTH_GROUP0_GPIO_9           , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirIn    , GpioOutDefault , GpioIntLevel  , GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,  //ADR_CFG_DAT
		// LTB_SOC_TCK (GPIO_141)
        { SOUTH_GROUP0_TCK              , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirIn    , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// LTB_SOC_TRST_N (GPIO_142)
        { SOUTH_GROUP0_TRST_N           , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirIn    , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// LTB_SOC_TMS (GPIO_143)
        { SOUTH_GROUP0_TMS              , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirIn    , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// LTB_SOC_TDI (GPIO_144)
        { SOUTH_GROUP0_TDI              , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirIn    , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// LTB_SOC_TDO (GPIO_145)
        { SOUTH_GROUP0_TDO              , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// LTB_SOC_PRDY_N (GPIO_146)
        { SOUTH_GROUP0_CX_PRDY_N        , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// LTB_SOC_PREQ_N (GPIO_147)
        { SOUTH_GROUP0_CX_PREQ_N        , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirIn    , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// NC (GPIO_148)
        { SOUTH_GROUP0_CTBTRIGINOUT     , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirNone  , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// NC (GPIO_149)
        { SOUTH_GROUP0_CTBTRIGOUT       , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirNone  , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// NC (GPIO_150)
        { SOUTH_GROUP0_DFX_SPARE2       , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirNone  , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// NC (GPIO_151)
        { SOUTH_GROUP0_DFX_SPARE3       , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirNone  , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// NC (GPIO_152)
        { SOUTH_GROUP0_DFX_SPARE4       , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirNone  , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_PWRDNACK PU (GPIO_79)
        { SOUTH_GROUP1_SUSPWRDNACK      , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirNone  , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_SUSCLK (GPIO_80)
        { SOUTH_GROUP1_PMU_SUSCLK       , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
        // ADR_TRIGGER_N PD(GPIO_81)
        { SOUTH_GROUP1_ADR_TRIGGER      , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirNone  , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SLP_S45_N (GPIO_82)
        { SOUTH_GROUP1_PMU_SLP_S45_N    , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SLP_S3_N (GPIO_83)
        { SOUTH_GROUP1_PMU_SLP_S3_N     , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_WAKE_N (GPIO_84)
        { SOUTH_GROUP1_PMU_WAKE_N       , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirIn    , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_PWRBTN_N (GPIO_85)
        { SOUTH_GROUP1_PMU_PWRBTN_N     , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirIn    , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_RSTBUTTON_N (GPIO_86)
        { SOUTH_GROUP1_PMU_RESETBUTTON_N, {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirIn    , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// PLTRST_N (GPIO_87)
        { SOUTH_GROUP1_PMU_PLTRST_N     , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_SUS_STAT_N (GPIO_88)
        { SOUTH_GROUP1_SUS_STAT_N       , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// S0IX (GPIO_89)
        { SOUTH_GROUP1_SLP_S0IX_N       , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_SPI_CS0_N (GPIO_72)
        { SOUTH_GROUP1_SPI_CS0_N        , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_SPI_CS1_N (GPIO_73)
        { SOUTH_GROUP1_SPI_CS1_N        , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_SPI_MOSI (GPIO_74)
        { SOUTH_GROUP1_SPI_MOSI_IO0     , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirInOut , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_SPI_MISO (GPIO_75)
        { SOUTH_GROUP1_SPI_MISO_IO1     , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirInOut  , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_SPI_IO2 (GPIO_76)
        { SOUTH_GROUP1_SPI_IO2          , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirInOut  , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_SPI_IO3 (GPIO_77)
        { SOUTH_GROUP1_SPI_IO3          , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirInOut  , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_SPI_CLK (GPIO_78)
        { SOUTH_GROUP1_SPI_CLK          , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// LPC_LAD0 (GPIO_64)
        { SOUTH_GROUP1_ESPI_IO0         , {GpioPadModeNative2  , GpioHostOwnGpio, GpioDirInOut , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// LPC_LAD1 (GPIO_65)
        { SOUTH_GROUP1_ESPI_IO1         , {GpioPadModeNative2  , GpioHostOwnGpio, GpioDirInOut , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// LPC_LAD2 (GPIO_66)
        { SOUTH_GROUP1_ESPI_IO2         , {GpioPadModeNative2  , GpioHostOwnGpio, GpioDirInOut , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// LPC_LAD3 (GPIO_67)
        { SOUTH_GROUP1_ESPI_IO3         , {GpioPadModeNative2  , GpioHostOwnGpio, GpioDirInOut , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// LPC_FRAME_N (GPIO_68)
        { SOUTH_GROUP1_ESPI_CS0_N       , {GpioPadModeNative2  , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// LPC_CLKOUT_R (GPIO_69)
        { SOUTH_GROUP1_ESPI_CLK         , {GpioPadModeNative2  , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// NC (GPIO_70)
        { SOUTH_GROUP1_ESPI_RST_N       , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirNone  , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// ESPI_ALRT_N PU (GPIO_71)
        { SOUTH_GROUP1_ESPI_ALRT0_N     , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirNone  , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// MFG_MODE (GPIO_10)
        { SOUTH_GROUP1_GPIO_10          , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirIn    , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// LPC_SERIRQ (GPIO_11)
        { SOUTH_GROUP1_GPIO_11          , {GpioPadModeNative2  , GpioHostOwnGpio, GpioDirInOut , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// EMMC_CMD (GPIO_123)
        { SOUTH_GROUP1_EMMC_CMD         , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermWpu20K, GpioLockDefault}} ,
		// EMMC_STROBE (GPIO_124)
        { SOUTH_GROUP1_EMMC_STROBE      , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// EMMC_CLK (GPIO_125)
        { SOUTH_GROUP1_EMMC_CLK         , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermWpd20K, GpioLockDefault}} ,
		// EMMC_DATA0 (GPIO_126)
        { SOUTH_GROUP1_EMMC_D0          , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirInOut , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermWpu20K, GpioLockDefault}} ,
		// EMMC_DATA1 (GPIO_127)
        { SOUTH_GROUP1_EMMC_D1          , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirInOut , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermWpu20K, GpioLockDefault}} ,
		// EMMC_DATA2 (GPIO_128)
        { SOUTH_GROUP1_EMMC_D2          , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirInOut , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermWpu20K, GpioLockDefault}} ,
		// EMMC_DATA3 (GPIO_129)
        { SOUTH_GROUP1_EMMC_D3          , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirInOut , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermWpu20K, GpioLockDefault}} ,
		// EMMC_DATA4 (GPIO_130)
        { SOUTH_GROUP1_EMMC_D4          , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirInOut , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermWpu20K, GpioLockDefault}} ,
		// EMMC_DATA5 (GPIO_131)
        { SOUTH_GROUP1_EMMC_D5          , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirInOut , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermWpu20K, GpioLockDefault}} ,
		// EMMC_DATA6 (GPIO_132)
        { SOUTH_GROUP1_EMMC_D6          , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirInOut , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermWpu20K, GpioLockDefault}} ,
		// EMMC_DATA7 (GPIO_133)
        { SOUTH_GROUP1_EMMC_D7          , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirInOut , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermWpu20K, GpioLockDefault}} ,
		// NC (GPIO_3)
//        { SOUTH_GROUP1_GPIO_3           , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirNone  , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}}
};

static GPIO_INIT_CONFIG mPineLakeBCfioInitTable [] = {
        // SFP1_INT_N (GPIO_14)
        { NORTH_ALL_GBE0_SDP0           , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirIn    , GpioOutDefault , GpioIntLevel  , GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
        // SFP0_INT_N (GPIO_15)
		{ NORTH_ALL_GBE1_SDP0           , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirIn    , GpioOutDefault , GpioIntLevel  , GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
        // LAN1_P0_I2C_CLK (GPIO_16)
        { NORTH_ALL_GBE0_SDP1           , {GpioPadModeNative2  , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
        // LAN1_P0_I2C_DATA (GPIO_17)
        { NORTH_ALL_GBE1_SDP1           , {GpioPadModeNative2  , GpioHostOwnGpio, GpioDirInOut , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
        // SFP_LAN1_P0_PRESENT (GPIO_18)
        { NORTH_ALL_GBE0_SDP2           , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirIn    , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
        // SFP_LAN1_P1_PRESENT (GPIO_19)
        { NORTH_ALL_GBE1_SDP2           , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirIn    , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
        // LAN1_P1_I2C_CLK (GPIO_20)
        { NORTH_ALL_GBE0_SDP3           , {GpioPadModeNative2  , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
        // LAN1_P1_I2C_DATA (GPIO_21)
        { NORTH_ALL_GBE1_SDP3           , {GpioPadModeNative2  , GpioHostOwnGpio, GpioDirInOut , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
        // LAN1_P0_LED0 (GPIO_22)
        { NORTH_ALL_GBE2_LED0           , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
        // LAN1_P0_LED1 (GPIO_23)
        { NORTH_ALL_GBE2_LED1           , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
        // NC (GPIO_24)
        { NORTH_ALL_GBE0_I2C_CLK        , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirNone  , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
        // NC (GPIO_25)
        { NORTH_ALL_GBE0_I2C_DATA       , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirNone  , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
        // NC (GPIO_26)
        { NORTH_ALL_GBE1_I2C_CLK        , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
        // NC (GPIO_27)
        { NORTH_ALL_GBE1_I2C_DATA       , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirInOut , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
        // NC (GPIO_28)
        { NORTH_ALL_NCSI_RXD0           , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirNone  , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
        // SOC_NCSI_CLK_IN PD (GPIO_29)
        { NORTH_ALL_NCSI_CLK_IN         , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirNone  , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
        // NC (GPIO_30)
        { NORTH_ALL_NCSI_RXD1           , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirNone  , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
        // NC (GPIO_31)
        { NORTH_ALL_NCSI_CRS_DV         , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirNone  , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
        // NC (GPIO_32)
        { NORTH_ALL_NCSI_ARB_IN         , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirNone  , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
        // SOC_NCSI_CLK_TX_EN PD (GPIO_33)
        { NORTH_ALL_NCSI_TX_EN          , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirNone  , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
        // SOC_NCSI_CLK_TXD0 PD (GPIO_34)
		{ NORTH_ALL_NCSI_TXD0           , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirNone  , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_NCSI_CLK_TXD1 PD (GPIO_35)
        { NORTH_ALL_NCSI_TXD1           , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirNone  , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// NC (GPIO_36)
        { NORTH_ALL_NCSI_ARB_OUT        , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirNone  , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// LAN0_P0_LED0 (GPIO_37)
        { NORTH_ALL_GBE0_LED0           , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// LAN0_P0_LED1 (GPIO_38)
        { NORTH_ALL_GBE0_LED1           , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// NC (GPIO_39)
        { NORTH_ALL_GBE1_LED0           , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirNone  , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// NC (GPIO_40)
        { NORTH_ALL_GBE1_LED1           , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirNone  , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// NC (GPIO_0)
        { NORTH_ALL_GPIO_0              , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirNone  , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// NC (GPIO_41)
        { NORTH_ALL_PCIE_CLKREQ0_N      , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirNone  , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_CLKREQ1_N (GPIO_42)
        { NORTH_ALL_PCIE_CLKREQ1_N      , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirIn    , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_CLKREQ2_N (GPIO_43)
        { NORTH_ALL_PCIE_CLKREQ2_N      , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirIn    , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// NC (GPIO_44)
        { NORTH_ALL_PCIE_CLKREQ3_N      , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirNone  , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// NC (GPIO_45)
        { NORTH_ALL_PCIE_CLKREQ4_N      , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirNone  , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// LAN_MDC (GPIO_1)
        { NORTH_ALL_GPIO_1              , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// LAN_MDIO (GPIO_2)
        { NORTH_ALL_GPIO_2              , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirInOut , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SVID_ALERT_N (GPIO_47)
        { NORTH_ALL_SVID_ALERT_N        , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirIn    , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SVID_DATA (GPIO_48)
        { NORTH_ALL_SVID_DATA           , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirInOut , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SVID_CLK (GPIO_49)
        { NORTH_ALL_SVID_CLK            , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_THERMTRIP_N (GPIO_50)
        { NORTH_ALL_THERMTRIP_N         , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_PROCHOT_N (GPIO_51)
        { NORTH_ALL_PROCHOT_N           , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirIn    , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_MEMHOT_N (GPIO_52)
        { NORTH_ALL_MEMHOT_N            , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirIn    , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// LTB_DFX_PORT_CLK0 (GPIO_53)
        { SOUTH_DFX_DFX_PORT_CLK0       , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// LTB_DFX_PORT_CLK1 (GPIO_54)
        { SOUTH_DFX_DFX_PORT_CLK1       , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_DFX_PORT0 (GPIO_55)
        { SOUTH_DFX_DFX_PORT0           , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirInOut , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_DFX_PORT1 (GPIO_56)
        { SOUTH_DFX_DFX_PORT1           , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirInOut , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_DFX_PORT2 (GPIO_57)
        { SOUTH_DFX_DFX_PORT2           , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirInOut , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_DFX_PORT3 (GPIO_58)
        { SOUTH_DFX_DFX_PORT3           , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirInOut , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_DFX_PORT4 (GPIO_59)
        { SOUTH_DFX_DFX_PORT4           , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirInOut , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_DFX_PORT5 (GPIO_60)
        { SOUTH_DFX_DFX_PORT5           , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirInOut , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_DFX_PORT6 (GPIO_61)
        { SOUTH_DFX_DFX_PORT6           , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirInOut , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_DFX_PORT7 (GPIO_62)
        { SOUTH_DFX_DFX_PORT7           , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirInOut , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_DFX_PORT8 (GPIO_63)
        { SOUTH_DFX_DFX_PORT8           , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirInOut , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_DFX_PORT9 (GPIO_134)
        { SOUTH_DFX_DFX_PORT9           , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirInOut , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_DFX_PORT10 (GPIO_135)
        { SOUTH_DFX_DFX_PORT10          , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirInOut , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_DFX_PORT11 (GPIO_136)
        { SOUTH_DFX_DFX_PORT11          , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirInOut , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_DFX_PORT12 (GPIO_137)
        { SOUTH_DFX_DFX_PORT12          , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirInOut , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_DFX_PORT13 (GPIO_138)
        { SOUTH_DFX_DFX_PORT13          , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirInOut , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_DFX_PORT14 (GPIO_139)
        { SOUTH_DFX_DFX_PORT14          , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirInOut , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_DFX_PORT15 (GPIO_140)
        { SOUTH_DFX_DFX_PORT15          , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirInOut , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
        // SPI_TPM_CS_N_GP12 (GPIO_12)
        { SOUTH_GROUP0_GPIO_12          , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
        // SMB_LAN_ALRT_N (GPIO_13)
        { SOUTH_GROUP0_SMB5_GBE_ALRT_N  , {GpioPadModeNative3  , GpioHostOwnGpio, GpioDirIn    , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// NC (GPIO_98)
        { SOUTH_GROUP0_PCIE_CLKREQ5_N   , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirNone  , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// NC (GPIO_99)
        { SOUTH_GROUP0_PCIE_CLKREQ6_N   , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirNone  , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// LAN1_P1_LED0 (GPIO_100)
        { SOUTH_GROUP0_PCIE_CLKREQ7_N   , {GpioPadModeNative3  , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_UART0_RXD (GPIO_101)
        { SOUTH_GROUP0_UART0_RXD        , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirIn    , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_UART0_TXD (GPIO_102)
        { SOUTH_GROUP0_UART0_TXD        , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SMB_LAN_CLK (GPIO_103)
        { SOUTH_GROUP0_SMB5_GBE_CLK     , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SMB_LAN_DATA (GPIO_104)
        { SOUTH_GROUP0_SMB5_GBE_DATA    , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirInOut , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_ERROR_N2 (GPIO_105)
        { SOUTH_GROUP0_ERROR2_N         , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_ERROR_N1 (GPIO_106)
        { SOUTH_GROUP0_ERROR1_N         , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_ERROR_N0 (GPIO_107)
        { SOUTH_GROUP0_ERROR0_N         , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_IERR_N (GPIO_108)
        { SOUTH_GROUP0_IERR_N           , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_MCERR_N (GPIO_109)
        { SOUTH_GROUP0_MCERR_N          , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SMB_LEG_CLK (GPIO_110)
        { SOUTH_GROUP0_SMB0_LEG_CLK     , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SMB_LEG_DATA (GPIO_111)
        { SOUTH_GROUP0_SMB0_LEG_DATA    , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirInOut , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SMB_LEG_ALRT_N (GPIO_112)
        { SOUTH_GROUP0_SMB0_LEG_ALRT_N  , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirIn    , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SMB_HOST_DATA (GPIO_113)
        { SOUTH_GROUP0_SMB1_HOST_DATA   , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirInOut , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SMB_HOST_CLK (GPIO_114)
        { SOUTH_GROUP0_SMB1_HOST_CLK    , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SMB_PECI_DATA (GPIO_115)
        { SOUTH_GROUP0_SMB2_PECI_DATA   , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirInOut , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SMB_PECI_CLK (GPIO_116)
        { SOUTH_GROUP0_SMB2_PECI_CLK    , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// NC (GPIO_117)
        { SOUTH_GROUP0_SMB4_CSME0_DATA  , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirNone  , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// NC (GPIO_118)
        { SOUTH_GROUP0_SMB4_CSME0_CLK   , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirNone  , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// NC (GPIO_119)
        { SOUTH_GROUP0_SMB4_CSME0_ALRT_N, {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirNone  , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// USB_OC_N (GPIO_120)
        { SOUTH_GROUP0_USB_OC0_N        , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirIn    , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// NC (GPIO_121)
        { SOUTH_GROUP0_FLEX_CLK_SE0     , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirNone  , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// NC (GPIO_122)
        { SOUTH_GROUP0_FLEX_CLK_SE1     , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirNone  , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// LAN1_P1_LED1 (GPIO_4)
        { SOUTH_GROUP0_GPIO_4           , {GpioPadModeNative3  , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_GPIO5 NC (GPIO_5)
        { SOUTH_GROUP0_GPIO_5           , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirInOut , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// W_DISABLE_B_N (GPIO_6)
        { SOUTH_GROUP0_GPIO_6           , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// W_DISABLE_A_N (GPIO_7)
        { SOUTH_GROUP0_GPIO_7           , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_SATA0_LED_N (GPIO_90)
        { SOUTH_GROUP0_SATA0_LED_N      , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_SATA1_LED_N (GPIO_91)
        { SOUTH_GROUP0_SATA1_LED_N      , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_SATA_PDETECT0 PU (GPIO_92)
        { SOUTH_GROUP0_SATA_PDETECT0    , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirNone   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_SATA_PDETECT1 PU (GPIO_93)
        { SOUTH_GROUP0_SATA_PDETECT1    , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirNone   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// NC (GPIO_94)
        { SOUTH_GROUP0_SATA0_SDOUT      , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirNone  , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// NC (GPIO_95)
        { SOUTH_GROUP0_SATA1_SDOUT      , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirNone  , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_UART1_RXD (GPIO_96)
        { SOUTH_GROUP0_UART1_RXD        , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirIn    , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_UART1_TXD (GPIO_97)
        { SOUTH_GROUP0_UART1_TXD        , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_GPIO8 (GPIO_8)
        { SOUTH_GROUP0_GPIO_8           , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirInOut , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,   //ADR_CFG_CLK
		// SW_INT_N (GPIO_9)
        { SOUTH_GROUP0_GPIO_9           , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirIn    , GpioOutDefault , GpioIntLevel  , GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,  //ADR_CFG_DAT
		// LTB_SOC_TCK (GPIO_141)
        { SOUTH_GROUP0_TCK              , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirIn    , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// LTB_SOC_TRST_N (GPIO_142)
        { SOUTH_GROUP0_TRST_N           , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirIn    , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// LTB_SOC_TMS (GPIO_143)
        { SOUTH_GROUP0_TMS              , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirIn    , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// LTB_SOC_TDI (GPIO_144)
        { SOUTH_GROUP0_TDI              , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirIn    , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// LTB_SOC_TDO (GPIO_145)
        { SOUTH_GROUP0_TDO              , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// LTB_SOC_PRDY_N (GPIO_146)
        { SOUTH_GROUP0_CX_PRDY_N        , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// LTB_SOC_PREQ_N (GPIO_147)
        { SOUTH_GROUP0_CX_PREQ_N        , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirIn    , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// NC (GPIO_148)
        { SOUTH_GROUP0_CTBTRIGINOUT     , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirNone  , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// NC (GPIO_149)
        { SOUTH_GROUP0_CTBTRIGOUT       , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirNone  , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// NC (GPIO_150)
        { SOUTH_GROUP0_DFX_SPARE2       , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirNone  , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// NC (GPIO_151)
        { SOUTH_GROUP0_DFX_SPARE3       , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirNone  , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// NC (GPIO_152)
        { SOUTH_GROUP0_DFX_SPARE4       , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirNone  , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_PWRDNACK PU (GPIO_79)
        { SOUTH_GROUP1_SUSPWRDNACK      , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirNone  , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_SUSCLK (GPIO_80)
        { SOUTH_GROUP1_PMU_SUSCLK       , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
        // ADR_TRIGGER_N PD(GPIO_81)
        { SOUTH_GROUP1_ADR_TRIGGER      , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirNone  , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SLP_S45_N (GPIO_82)
        { SOUTH_GROUP1_PMU_SLP_S45_N    , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SLP_S3_N (GPIO_83)
        { SOUTH_GROUP1_PMU_SLP_S3_N     , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_WAKE_N (GPIO_84)
        { SOUTH_GROUP1_PMU_WAKE_N       , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirIn    , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_PWRBTN_N (GPIO_85)
        { SOUTH_GROUP1_PMU_PWRBTN_N     , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirIn    , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_RSTBUTTON_N (GPIO_86)
        { SOUTH_GROUP1_PMU_RESETBUTTON_N, {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirIn    , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// PLTRST_N (GPIO_87)
        { SOUTH_GROUP1_PMU_PLTRST_N     , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_SUS_STAT_N (GPIO_88)
        { SOUTH_GROUP1_SUS_STAT_N       , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// S0IX (GPIO_89)
        { SOUTH_GROUP1_SLP_S0IX_N       , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_SPI_CS0_N (GPIO_72)
        { SOUTH_GROUP1_SPI_CS0_N        , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_SPI_CS1_N (GPIO_73)
        { SOUTH_GROUP1_SPI_CS1_N        , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_SPI_MOSI (GPIO_74)
        { SOUTH_GROUP1_SPI_MOSI_IO0     , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirInOut , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_SPI_MISO (GPIO_75)
        { SOUTH_GROUP1_SPI_MISO_IO1     , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirInOut , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_SPI_IO2 (GPIO_76)
        { SOUTH_GROUP1_SPI_IO2          , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirInOut  , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_SPI_IO3 (GPIO_77)
        { SOUTH_GROUP1_SPI_IO3          , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirInOut  , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// SOC_SPI_CLK (GPIO_78)
        { SOUTH_GROUP1_SPI_CLK          , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// NC (GPIO_64)
        { SOUTH_GROUP1_ESPI_IO0         , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirNone  , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// NC (GPIO_65)
        { SOUTH_GROUP1_ESPI_IO1         , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirNone  , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// NC (GPIO_66)
        { SOUTH_GROUP1_ESPI_IO2         , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirNone  , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// NC (GPIO_67)
        { SOUTH_GROUP1_ESPI_IO3         , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirNone  , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// NC (GPIO_68)
        { SOUTH_GROUP1_ESPI_CS0_N       , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirNone  , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// NC (GPIO_69)
        { SOUTH_GROUP1_ESPI_CLK         , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirNone  , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// NC (GPIO_70)
        { SOUTH_GROUP1_ESPI_RST_N       , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirNone  , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// ESPI_ALRT_N PU (GPIO_71)
        { SOUTH_GROUP1_ESPI_ALRT0_N     , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirNone  , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// MFG_MODE (GPIO_10)
        { SOUTH_GROUP1_GPIO_10          , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirIn    , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// LPC_SERIRQ (GPIO_11)
        { SOUTH_GROUP1_GPIO_11          , {GpioPadModeNative2  , GpioHostOwnGpio, GpioDirInOut , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// EMMC_CMD (GPIO_123)
        { SOUTH_GROUP1_EMMC_CMD         , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermWpu20K, GpioLockDefault}} ,
		// EMMC_STROBE (GPIO_124)
        { SOUTH_GROUP1_EMMC_STROBE      , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
		// EMMC_CLK (GPIO_125)
        { SOUTH_GROUP1_EMMC_CLK         , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermWpu20K, GpioLockDefault}} ,
		// EMMC_DATA0 (GPIO_126)
        { SOUTH_GROUP1_EMMC_D0          , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirInOut , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermWpu20K, GpioLockDefault}} ,
		// EMMC_DATA1 (GPIO_127)
        { SOUTH_GROUP1_EMMC_D1          , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirInOut , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermWpu20K, GpioLockDefault}} ,
		// EMMC_DATA2 (GPIO_128)
        { SOUTH_GROUP1_EMMC_D2          , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirInOut , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermWpu20K, GpioLockDefault}} ,
		// EMMC_DATA3 (GPIO_129)
        { SOUTH_GROUP1_EMMC_D3          , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirInOut , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermWpu20K, GpioLockDefault}} ,
		// EMMC_DATA4 (GPIO_130)
        { SOUTH_GROUP1_EMMC_D4          , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirInOut , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermWpu20K, GpioLockDefault}} ,
		// EMMC_DATA5 (GPIO_131)
        { SOUTH_GROUP1_EMMC_D5          , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirInOut , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermWpu20K, GpioLockDefault}} ,
		// EMMC_DATA6 (GPIO_132)
        { SOUTH_GROUP1_EMMC_D6          , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirInOut , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermWpu20K, GpioLockDefault}} ,
		// EMMC_DATA7 (GPIO_133)
        { SOUTH_GROUP1_EMMC_D7          , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirInOut , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermWpu20K, GpioLockDefault}} ,
		// TOP_SWAP_STRAP (GPIO_3)
        { SOUTH_GROUP1_GPIO_3           , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirNone  , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}}
};

#endif /* GPIOTABLEPINELAKE_H_ */
