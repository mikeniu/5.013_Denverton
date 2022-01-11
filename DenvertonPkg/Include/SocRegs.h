//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//
/*++

Copyright (c) 1999 - 2015 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  SocRegs.h

Abstract:

  Register names for Denverton.
--*/
#ifndef _SOC_REGS_H_
#define _SOC_REGS_H_

//
// Include device register definitions
//
//
//

//
// System Agent
//
#define PCI_BUS_NUMBER_SA              0
#define PCI_DEVICE_NUMBER_SA           0
#define PCI_FUNCTION_NUMBER_SA         0

//
// High-speed UART
//
#define PCI_BUS_NUMBER_HSUART0         0
#define PCI_DEVICE_NUMBER_HSUART0      26
#define PCI_FUNCTION_NUMBER_HSUART0    0
#define PCI_BUS_NUMBER_HSUART1         0
#define PCI_DEVICE_NUMBER_HSUART1      26
#define PCI_FUNCTION_NUMBER_HSUART1    1
#define PCI_BUS_NUMBER_HSUART2         0
#define PCI_DEVICE_NUMBER_HSUART2      26
#define PCI_FUNCTION_NUMBER_HSUART2    2

typedef struct {
  UINT8 bus;
  UINT8 device;
  UINT8 function;
}HSUART_BDF;

#define DEBUG_PORT_HSUART_BUS          PCI_BUS_NUMBER_HSUART0
#define DEBUG_PORT_HSUART_DEV          PCI_DEVICE_NUMBER_HSUART0
#define DEBUG_PORT_HSUART_FUNC         PCI_FUNCTION_NUMBER_HSUART0

#define R_HSUART_PCICMD                0x4
#define B_HSUART_PCICMD_IOSE           BIT0
#define R_HSUART_IOBA                  0x10
#define B_HSUART_IOBA_EN               BIT0
#define B_HSUART_IOBA_BASE_ADDR        0xfffffff8
#define R_HSUART_INTERRUPT  	 	   0x3C
#define B_HSUART_INTERRUPT_INTR_LINE   0xFF
#define B_HSUART_INTERRUPT_INTR_PIN    0xFF00
#define V_HSUART_INTERRUPT_INTR_PIN_INTA 0x0100
#define V_HSUART_INTERRUPT_INTR_PIN_INTB 0x0200
#define V_HSUART_INTERRUPT_INTR_PIN_INTC 0x0300
#define V_HSUART_INTERRUPT_INTR_PIN_INTD 0x0400
#define R_HSUART_RDCFG                   0x74 //Function-Hide Register. Hiding is programmable per-function. Since hiding affects reads to FUNC_RDCFG_HIDE itself, this register is not shared in order to read-modify-write errors. Note: if function 0 is hidden, O/S will not discover function 1 and function 2.
#define B_HSUART_RDCFG_HIDE              BIT0

#endif
