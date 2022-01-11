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

  GpioCtrlrLib.c

Abstract:

  This file provides functions for configuring an ADR capable platform's CPLD.

--*/
/*
 * GpioCtrlrLib.c
 *
 *  Created on: May 28, 2014
 */
#include "GpioCtrlrLib.h"
#include <PlatformDefinitions.h>
#include <Library/PcdLib.h>
#include <Library/PciLib.h>
#include <Library/DebugLib.h>


UINTN SBREGbase(){
  UINTN const P2sbMmio = PCIEX_BASE_ADDRESS | PCI_LIB_ADDRESS( 0, 31, 1, 0 );
  UINTN SBREG_BAR = ((*(UINT32*)(P2sbMmio+0x10)) & ~(BIT2));
  UINTN SBREG_BARH = ((*(UINT32*)(P2sbMmio+0x14)) & ~(BIT2));

  ASSERT(SBREG_BAR!=0); //SBREG should have been setup by now
  if (0 != SBREG_BARH) {
    DEBUG ((DEBUG_ERROR, "%a() - P2SB ID:%x SBREG:%x:%x\n",__FUNCTION__,(*(UINT32*)P2sbMmio), SBREG_BARH, SBREG_BAR));
    ASSERT(SBREG_BARH == 0); //SBREG_BAR assumed to be below 4G
  }

  return SBREG_BAR;
}

UINTN gpioPadCfgBaseOf(gpio_name GPIO){
	UINTN base = gpioCtrlBase(GPIO);
	UINTN offset;

	if (gpioInNorth(GPIO)) {
		offset = gpioPadCfgOff + (gpioPadCfgSize * gpioNorthIndex(GPIO));
	} else {
		offset = gpioPadCfgOff + (gpioPadCfgSize * gpioSouthIndex(GPIO));
	}

	//DEBUG ((DEBUG_ERROR, "%a() - PAD_CFG:%x\n",__FUNCTION__,base+offset));
	return (base+offset);
}



