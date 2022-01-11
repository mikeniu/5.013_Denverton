/*
 * GpioTableMinnehaha.h
 *
 *@copyright
  Copyright (c) 2015 Intel Corporation. All rights reserved
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
 */

#ifndef GPIOTABLEMINNEHAHA_H_
#define GPIOTABLEMINNEHAHA_H_
static GPIO_INIT_CONFIG mMinnehahaCfioInitTable [] = {
        // GBE0_SDP0 (GPIO_14)
        { NORTH_ALL_GBE0_SDP0           , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirInOut , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
        // GBE1_SDP0 (GPIO_15)
        { NORTH_ALL_GBE1_SDP0           , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirInOut , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
        // GBE2_I2C_CLK (GPIO_16)
        { NORTH_ALL_GBE0_SDP1           , {GpioPadModeNative2  , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
        // GBE2_I2C_DATA (GPIO_17)
        { NORTH_ALL_GBE1_SDP1           , {GpioPadModeNative2  , GpioHostOwnGpio, GpioDirInOut , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
        // GBE2_SDP0 (GPIO_18)
        { NORTH_ALL_GBE0_SDP2           , {GpioPadModeNative2  , GpioHostOwnGpio, GpioDirInOut , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
        // GBE3_SDP0 (GPIO_19)
        { NORTH_ALL_GBE1_SDP2           , {GpioPadModeNative2  , GpioHostOwnGpio, GpioDirInOut , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
        // GBE3_I2C_CLK (GPIO_20)
        { NORTH_ALL_GBE0_SDP3           , {GpioPadModeNative2  , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
        // GBE3_I2C_DATA (GPIO_21)
        { NORTH_ALL_GBE1_SDP3           , {GpioPadModeNative2  , GpioHostOwnGpio, GpioDirInOut , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
        // GBE2_LED0 (GPIO_22)
        { NORTH_ALL_GBE2_LED0           , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
        // GBE2_LED1 (GPIO_23)
        { NORTH_ALL_GBE2_LED1           , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
        // GBE0_I2C_CLK (GPIO_24)
        { NORTH_ALL_GBE0_I2C_CLK        , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
        // GBE0_I2C_DATA (GPIO_25)
        { NORTH_ALL_GBE0_I2C_DATA       , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirInOut , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
        // GBE1_I2C_CLK (GPIO_26)
        { NORTH_ALL_GBE1_I2C_CLK        , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
        // GBE1_I2C_DATA (GPIO_27)
        { NORTH_ALL_GBE1_I2C_DATA       , {GpioPadModeNative1  , GpioHostOwnGpio, GpioDirInOut , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
        // SPARE (GPIO_28)
        { NORTH_ALL_NCSI_RXD0           , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
        // SPARE (GPIO_29)
        { NORTH_ALL_NCSI_CLK_IN         , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
        // SPARE (GPIO_30)
        { NORTH_ALL_NCSI_RXD1           , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
        // SPARE (GPIO_31)
        { NORTH_ALL_NCSI_CRS_DV         , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
        // SPARE (GPIO_32)
        { NORTH_ALL_NCSI_ARB_IN         , {GpioPadModeGpio     , GpioHostOwnGpio, GpioDirOut   , GpioOutDefault , GpioIntDefault, GpioResetDeep, GpioTermDefault, GpioLockDefault}} ,
        // SPARE (GPIO_33)
};


#endif /* GPIOTABLEMINNEHAHA_H_ */
