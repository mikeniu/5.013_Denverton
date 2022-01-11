/** @file
  IchRegs.h
  This file include defination for ICH register.
 
  Copyright (c) 2005-2015 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

**/

#ifndef _ICHREGS_H_
#define _ICHREGS_H_

//
// Definitions begginning with "R_" are registers
// Definitions begginning with "B_" are bits within registers
// Definitions begginning with "V_" are meaningful values of bits within the
//   registers
//

#define R_ICH_LPC_GEN_PMCON_2                   0xA2
#define   B_ICH_LPC_GEN_PMCON_MIN_SLP_S4        0x04
#define   B_ICH_LPC_GEN_PMCON_DRAM_INIT         0x80
#define R_ICH_LPC_GEN_PMCON_3                   0xA4
#define R_ICH_LPC_ETR3                          0xAC
#define   B_ICH_LPC_ETR3_CF9GR                  (BIT20)

#endif
