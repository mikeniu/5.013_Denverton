/*++
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
--*/

/*++

Copyright (c)  2015 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  HsuartIpRegs.h

Abstract:

  Driver for Hsuart IP Register definitions.

  Conventions:

  - Prefixes:
    Definitions beginning with "R_" are registers
    Definitions beginning with "B_" are bits within registers
    Definitions beginning with "V_" are meaningful values of bits within the registers
    Definitions beginning with "S_" are register sizes
    Definitions beginning with "N_" are the bit position
  - In general, HSURAT registers are denoted by "_HSUART_" in register names
  - Registers / bits that are different between HSUART generations are denoted by
    "_HSUART_<generation_name>_" in register/bit names. e.g., "_HSURAT_LPT_"
  - Registers / bits that are different between SKUs are denoted by "_<SKU_name>"
    at the end of the register/bit names
  - Registers / bits of new devices introduced in a HSUART generation will be just named
    as "_HSUART_" without <generation_name> inserted.

Revision History

--*/

#ifndef _HSUART_IP_REGS_H_
#define _HSURAT_IP_REGS_H_

#define V_DEFAULT_CLASS_CODE    0x07000200
#define V_HSUART_PID            0xCB 
#define V_HSUART_DEV_NUM        0x1A
#define V_HSUART_BUS_NUM        0x0
#define V_OPCODE_WRITE          0x5
#define V_SBI_FBE               0xE 
#define V_SBI_BAR               0
#define SHIFT_LEFT_3            3  

#define B_CL_MASK               ~BIT0
#define B_CL_BIT                BIT0

#define R_REVISION_ID           0x8 
#define R_PLKCTL                0x70

#define V_HSUART_INTERRUPT_INTR_PIN_INTA 0x01
#define V_HSUART_INTERRUPT_INTR_PIN_INTB 0x02
#define V_HSUART_INTERRUPT_INTR_PIN_INTC 0x03
#define V_HSUART_INTERRUPT_INTR_PIN_INTD 0x04

#endif // _HSUART_IP_REGS_H_
