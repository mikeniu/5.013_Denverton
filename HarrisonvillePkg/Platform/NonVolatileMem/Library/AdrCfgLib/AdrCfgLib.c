/*++
 This file contains an 'Intel Peripheral Driver' and uniquely
 identified as "Intel Reference Module" and is
 licensed for Intel CPUs and chipsets under the terms of your
 license agreement with Intel or your vendor.  This file may
 be modified by the user, subject to additional terms of the
 license agreement
 --*/

/*++

 Copyright (c)  1999 - 2016 Intel Corporation. All rights reserved
 This software and associated documentation (if any) is furnished
 under a license and may only be used or copied in accordance
 with the terms of the license. Except as permitted by such
 license, no part of this software or documentation may be
 reproduced, stored in a retrieval system, or transmitted in any
 form or by any means without the express written consent of
 Intel Corporation.


 Module Name:

 AdrCfgLib.c

 Abstract:

 This file provides functions for configuring an ADR capable platform's CPLD.

 --*/
/*
 * AdrCfgLib.c
 *
 *  Created on: May 28, 2014
 */
#include "AdrCfgLib.h"
#include <Library/GpioCtrlrLib.h>
#include <Library/DebugLib.h>

typedef struct {
  UINT8 AddrCmd;
  UINT8 Data;
} CPLD_MESSAGE;

#define cpldReadCmd  (1)
#define cpldWriteCmd (0)
#define BitAt(data,pos) (0<((data)&(1<<pos)))
//Pin name - SPARE_8/FAN_PWM1/SMB6_CSME1_DATA
#define ADR_CFG_CLK (gpio_TDO)
//Pin name - SPARE_9/FAN_PWM2/SMB6_CSME1_CLK
#define ADR_CFG_DAT (gpio_CX_PRDY_N)
#define ADR_CONFIG_STATUS_MCU_INIT (1<<4)
#define ADR_CONFIG_STATUS_ADR_STATUS_CLR (1<<6)

static UINTN GPIObar(void);
static UINT8 CPLDread(UINT8 addr);
static void CPLDwrite(UINT8 addr, UINT8 Data);

UINT8 AdrCfg_boardID(void);
UINT8 AdrCfg_CPLDversion(void);
UINT8 AdrCfg_VREGread(void);
void AdrCfg_VREGwrite( UINT8);
void AdrCfg_ConfigWrite(ADR_CONFIG_STATUS config);
void AdrCfg_Arm(CPLD_OP_MODES mode);
UINT8 AdrCfg_Active(void);
void AdrCfg_Deactivate(void);
void reset_CPLD(UINT32 CLK, UINT32 DATA);

static void loadData(UINTN DAT, UINTN CLK, UINT8 data, UINT8 Bits) {
  UINT8 b;

  // Loading MSB..LSB+1 {7..1 or 0}
  for (b = 7; 0 < Bits; b--, Bits--) { //
    gpioSetOutput(DAT, BitAt(data,b));
    gpioSetOutput(CLK, 1);
    gpioSetOutput(CLK, 0);
    //DEBUG ((DEBUG_ERROR, "D%x",BitAt(data,b)));
  }
}

static void loadCmd(UINTN DAT, UINTN CLK, UINT8 Cmd) {

  // Load op=read, CLK, reverse DATA direction, !CLK
  gpioSetOutput(DAT, Cmd);
  gpioSetOutput(CLK, 1);
  //DEBUG ((DEBUG_ERROR, "D%x",Cmd));
  if (cpldReadCmd == Cmd) {
    gpioSetDirection(DAT, gpioIn);
    //DEBUG ((DEBUG_ERROR, " DATA:%x ",*(UINT32*)DAT));
    //DEBUG ((DEBUG_ERROR, "Di"));

  }

  gpioSetOutput(CLK, 0);

}

static UINT8 PullData(UINTN DAT, UINTN CLK, UINT8 Bits) {
  UINT8 b;
  UINT8 result = 0;

  // pull data in MSB..LSB {7..1 or 0}
  for (b = 7; 0 < Bits; b--, Bits--) {
    gpioSetOutput(CLK, 1);
    result |= (gpioGetInput(DAT) << b);
    //DEBUG ((DEBUG_ERROR, "D%x",gpioGetInput(DAT)));
    gpioSetOutput(CLK, 0);
  }

  return result;
}

static UINT8 CPLDread(UINT8 addr) {
  UINT32 CLK = gpioPadCfgBaseOf(ADR_CFG_CLK);
  UINT32 DATA = gpioPadCfgBaseOf(ADR_CFG_DAT);
  UINT8 result;

  DEBUG(
      (DEBUG_ERROR, "%a(addr:%x) CLK:%x:%x DATA:%x:%x =>",__FUNCTION__,addr,CLK,*(UINT32*)CLK,DATA,*(UINT32*)DATA));

  gpioSetMode(CLK, gpioGPIO);
  gpioSetMode(DATA, gpioGPIO);
  gpioSetDirection(CLK, gpioOut);
  gpioSetDirection(DATA, gpioOut);
  //DEBUG ((DEBUG_ERROR, " CLK-DW0:%x DATA-DW0:%x\n CoDo",*(UINT32*)CLK,*(UINT32*)DATA));

  reset_CPLD(CLK, DATA);
  //send addr - all but LSB
  loadData(DATA, CLK, addr, 7);
  // Load op=read, CLK, reverse DATA direction, !CLK
  loadCmd(DATA, CLK, cpldReadCmd);
  result = PullData(DATA, CLK, 8);
  gpioSetDirection(DATA, gpioOut);
  DEBUG((DEBUG_ERROR, " %x \n",result));

  return result;
}

static void CPLDwrite(UINT8 addr, UINT8 Data) {
  UINTN CLK = gpioPadCfgBaseOf(ADR_CFG_CLK);
  UINTN DATA = gpioPadCfgBaseOf(ADR_CFG_DAT);
  DEBUG(
      (DEBUG_ERROR, "%a(addr:%x,Data:%x) CLK:%x:%x DATA:%x:%x\n",__FUNCTION__,addr,Data,CLK,*(UINT32*)CLK,DATA,*(UINT32*)DATA));

  gpioSetMode(CLK, gpioGPIO);
  gpioSetMode(DATA, gpioGPIO);
  gpioSetDirection(CLK, gpioOut);
  gpioSetDirection(DATA, gpioOut);

  reset_CPLD(CLK, DATA);

  //send addr - all but LSB
  loadData(DATA, CLK, addr, 7);
  // Load op=read, CLK, reverse DATA direction, !CLK
  loadCmd(DATA, CLK, cpldWriteCmd);
  //send addr - all but LSB
  loadData(DATA, CLK, Data, 8);
  //DEBUG ((DEBUG_ERROR, "\n"));
}

UINT8 AdrCfg_boardID(void) {
  UINT8 ID = CPLDread(CPLD_REG_BOARD_IDENTIFICATION);
  return ID;
}

UINT8 AdrCfg_CPLDversion(void) {
  UINT8 ver = CPLDread(CPLD_REG_CPLD_VERSION);
  return ver;
}

UINT8 AdrCfg_VREGread(void) {
  UINT8 ver = CPLDread(CPLD_REG_VREG_ENABLES);
  return ver;
}

void AdrCfg_VREGwrite(UINT8 data) {
  CPLDwrite(CPLD_REG_VREG_ENABLES, data);
}

void AdrCfg_ConfigWrite(ADR_CONFIG_STATUS config) {
  CPLDwrite(CPLD_REG_ADR_CONFIG_STATUS, config.data);
}

void AdrCfg_Arm(CPLD_OP_MODES mode) {
  ADR_CONFIG_STATUS config = {0};

  DEBUG(
      (DEBUG_ERROR, "%a(%x) BrdID:%x CPLD:%x\n",__FUNCTION__,mode,AdrCfg_boardID(),AdrCfg_CPLDversion()));

  // Only set the mode and write zero to the upper nibble
  // to avoid clearing ADR_STATUS or generating MCU_INIT
  config.fld.c2f_config = mode;
  DEBUG((DEBUG_ERROR, " ADR Mode:%x (%x)\n", mode, config.data));
  //config.fld.mcu_init = (NVM_DISABLE != mode); //only init if an active mode requested
  //config.fld.status_clr = 0;
  CPLDwrite(CPLD_REG_ADR_CONFIG_STATUS, config.data);
  DEBUG(
      (DEBUG_ERROR, " After Config & Status:%x \n", CPLDread(CPLD_REG_ADR_CONFIG_STATUS)));

}

UINT8 AdrCfg_Active(void) {
  ADR_CONFIG_STATUS config;
  config.data = CPLDread(CPLD_REG_ADR_CONFIG_STATUS);
  DEBUG(
      (DEBUG_ERROR, "AdrCfg_Active: Config & Status:%x (%x)\n", ((config.data)&0x80), config.data));
  return (((config.data) & 0x80));

}

void AdrCfg_Deactivate(void) {
  ADR_CONFIG_STATUS config;

  config.data = CPLDread(CPLD_REG_ADR_CONFIG_STATUS);
  if (0 != (config.data&0x80)){
    //The CPLD can only handle ADR status clear and MCU_INT to be done when ADR status is set
    config.data = config.data | (ADR_CONFIG_STATUS_MCU_INIT | ADR_CONFIG_STATUS_ADR_STATUS_CLR);
    DEBUG(
      (DEBUG_ERROR, "AdrCfg_Deactivate: Clear ADR Status & trigger MCU INIT (%x)\n", config.data));
    CPLDwrite(CPLD_REG_ADR_CONFIG_STATUS, config.data);
    //The CPLD also want MCU_INIT to be cleared after setting
    config.data = config.data & ~(ADR_CONFIG_STATUS_MCU_INIT | ADR_CONFIG_STATUS_ADR_STATUS_CLR);
    CPLDwrite(CPLD_REG_ADR_CONFIG_STATUS, config.data);
  }
  DEBUG(
      (DEBUG_ERROR, "AdrCfg_Deactivate:: Config & Status:%x \n",CPLDread(CPLD_REG_ADR_CONFIG_STATUS)));
}

//Command: Reset CPLD
//CLK: ----------------- 
//         _____       |   ____
//DATA:    |   |       |   |   |
//         |   |       |   |   |
//    -----     -------|---|   |------
//                     |
//                     ---------------
void reset_CPLD(UINT32 CLK, UINT32 DATA) {

  gpioSetOutput(CLK, 1);
  gpioSetOutput(DATA, 0);
  gpioSetOutput(DATA, 1);
  gpioSetOutput(DATA, 0);

  gpioSetOutput(CLK, 0);
  gpioSetOutput(DATA, 0);
  gpioSetOutput(DATA, 1);
  gpioSetOutput(DATA, 0);

}

