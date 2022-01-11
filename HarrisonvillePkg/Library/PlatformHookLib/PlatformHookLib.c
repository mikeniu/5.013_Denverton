/** @file
  Platform Hook Library instance.

  Copyright (c) 2010 - 2014, Intel Corporation. All rights 
  reserved.<BR> This program and the accompanying materials 
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Base.h>
#include <Library/PlatformHookLib.h>
#include <Library/PciExpressLib.h>
#include <Library/IoLib.h>

#include <PchAccess.h>

#include "SocRegs.h"

/**
  Performs platform specific initialization required for the CPU to access
  the hardware associated with a SerialPortLib instance.  This function does
  not intiailzie the serial port hardware itself.  Instead, it initializes 
  hardware devices that are required for the CPU to access the serial port 
  hardware.  This function may be called more than once.

  @retval RETURN_SUCCESS       The platform specific initialization succeeded.
  @retval RETURN_DEVICE_ERROR  The platform specific initialization could not be completed.
 
**/
RETURN_STATUS
EFIAPI
PlatformHookSerialPortInitialize (
  VOID
  )
{
  UINT32     Data32;

  Data32 = MmioRead32(PCH_PCR_ADDRESS(PID_GPIOCOM1, R_PAD_CFG_DW0_UART0_RXD));
  if (( (Data32 & B_PCH_GPIO_PAD_MODE) >> N_PCH_GPIO_PAD_MODE) != V_PCH_GPIO_PAD_MODE_NAT_1) {
    Data32 |= (UINT32)(V_PCH_GPIO_PAD_MODE_NAT_1 << N_PCH_GPIO_PAD_MODE);
    MmioWrite32(PCH_PCR_ADDRESS(PID_GPIOCOM1, R_PAD_CFG_DW0_UART0_RXD), Data32);
  }

  Data32 = MmioRead32(PCH_PCR_ADDRESS(PID_GPIOCOM1, R_PAD_CFG_DW0_UART0_TXD));
  if (( (Data32 & B_PCH_GPIO_PAD_MODE) >> N_PCH_GPIO_PAD_MODE) != V_PCH_GPIO_PAD_MODE_NAT_1) {
	Data32 |= (UINT32)(V_PCH_GPIO_PAD_MODE_NAT_1 << N_PCH_GPIO_PAD_MODE);
	MmioWrite32(PCH_PCR_ADDRESS(PID_GPIOCOM1, R_PAD_CFG_DW0_UART0_TXD), Data32);
  }



  //Setup GPIO for HSUART0 signals
  Data32 = MmioRead32(PCH_PCR_ADDRESS(PID_GPIOCOM1, R_PAD_CFG_DW0_SMB3_CLTT_CLK));
  if (( (Data32 & B_PCH_GPIO_PAD_MODE) >> N_PCH_GPIO_PAD_MODE) != V_PCH_GPIO_PAD_MODE_NAT_2 ) {
    Data32 |= (UINT32)(V_PCH_GPIO_PAD_MODE_NAT_2 << N_PCH_GPIO_PAD_MODE);
    MmioWrite32(PCH_PCR_ADDRESS(PID_GPIOCOM1, R_PAD_CFG_DW0_SMB3_CLTT_CLK), Data32);
  }

  Data32 = MmioRead32(PCH_PCR_ADDRESS(PID_GPIOCOM1, R_PAD_CFG_DW0_PCIE_CLKREQ5_N));
  if (( (Data32 & B_PCH_GPIO_PAD_MODE) >> N_PCH_GPIO_PAD_MODE) != V_PCH_GPIO_PAD_MODE_NAT_3 ) {
    Data32 |= (UINT32)(V_PCH_GPIO_PAD_MODE_NAT_3 << N_PCH_GPIO_PAD_MODE);
    MmioWrite32(PCH_PCR_ADDRESS(PID_GPIOCOM1, R_PAD_CFG_DW0_PCIE_CLKREQ5_N), Data32);
  }

/* RC - OLD - Updated code above
  //Setup GPIO for HSUART0 signals
  Data32 = MmioRead32(PCH_PCR_ADDRESS(PID_GPIOCOM0, R_PAD_CFG_DW0_NCSI_TXD1));
  if (( (Data32 & B_PCH_GPIO_PAD_MODE) >> N_PCH_GPIO_PAD_MODE) != V_PCH_GPIO_PAD_MODE_NAT_3 ) {
    Data32 |= (UINT32)(V_PCH_GPIO_PAD_MODE_NAT_3 << N_PCH_GPIO_PAD_MODE);
    MmioWrite32(PCH_PCR_ADDRESS(PID_GPIOCOM0, R_PAD_CFG_DW0_NCSI_TXD1), Data32);
  }

  Data32 = MmioRead32(PCH_PCR_ADDRESS(PID_GPIOCOM0, R_PAD_CFG_DW0_NCSI_ARB_OUT));
  if (( (Data32 & B_PCH_GPIO_PAD_MODE) >> N_PCH_GPIO_PAD_MODE) != V_PCH_GPIO_PAD_MODE_NAT_3 ) {
    Data32 |= (UINT32)(V_PCH_GPIO_PAD_MODE_NAT_3 << N_PCH_GPIO_PAD_MODE);
    MmioWrite32(PCH_PCR_ADDRESS(PID_GPIOCOM0, R_PAD_CFG_DW0_NCSI_ARB_OUT), Data32);
  }
*/
  return RETURN_SUCCESS;
}

