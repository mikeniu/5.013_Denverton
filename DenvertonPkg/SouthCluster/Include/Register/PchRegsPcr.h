/** @file
  Register names for PCH private chipset register

  Conventions:

  - Prefixes:
    Definitions beginning with "R_" are registers
    Definitions beginning with "B_" are bits within registers
    Definitions beginning with "V_" are meaningful values within the bits
    Definitions beginning with "S_" are register sizes
    Definitions beginning with "N_" are the bit position
  - In general, PCH registers are denoted by "_PCH_" in register names
  - Registers / bits that are different between PCH generations are denoted by
    "_PCH_[generation_name]_" in register/bit names.
  - Registers / bits that are specific to PCH-H denoted by "_H_" in register/bit names.
    Registers / bits that are specific to PCH-LP denoted by "_LP_" in register/bit names.
    e.g., "_PCH_H_", "_PCH_LP_"
    Registers / bits names without _H_ or _LP_ apply for both H and LP.
  - Registers / bits that are different between SKUs are denoted by "_[SKU_name]"
    at the end of the register/bit names
  - Registers / bits of new devices introduced in a PCH generation will be just named
    as "_PCH_" without [generation_name] inserted.

@copyright
 Copyright (c) 2013 - 2016 Intel Corporation. All rights reserved
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
#ifndef _PCH_REGS_PCR_H_
#define _PCH_REGS_PCR_H_

///
/// Definition for PCR base address (defined in PchReservedResources.h)
///
//#define PCH_PCR_BASE_ADDRESS            0xFD000000
//#define PCH_PCR_MMIO_SIZE               0x01000000
/**
  Definition for PCR address
  The PCR address is used to the PCR MMIO programming
**/
#define PCH_PCR_ADDRESS(Pid, Offset)    (PCH_PCR_BASE_ADDRESS | ((UINT8)(Pid) << 16) | (UINT16)(Offset))

///
/// Definition for SBI PID
/// The PCH_SBI_PID defines the PID for PCR MMIO programming and PCH SBI programming as well.
///
typedef UINT8 PCR_PID;

typedef enum {
  PID_ESPISPI                           = 0x93,
  PID_ICLK                              = 0x99,
  PID_PMC                               = 0x82,
  PID_XHCI                              = 0xA2,
  PID_SATA0                             = 0xB5,
  PID_SATA1                             = 0xB6,
  PID_FIA                               = 0xB0,
  PID_USB2                              = 0xA7,
  PID_LPC                               = 0xD2,
  PID_SMB                               = 0xCF,
  PID_P2S                               = 0xD4,
  PID_ITSS                              = 0xD0,
  PID_RTC                               = 0xD1,
  PID_PSF0                              = 0x24,
  PID_PSF1                              = 0xA9,
  PID_PSF2                              = 0xA0,
  PID_PSF3                              = 0xC6,
  PID_PSFS0                             = 0xB7,
  PID_PSFS1                             = 0xB8,
  PID_MEPSF                             = 0xE3,
  PID_MEPTIO                            = 0xE4,
  PID_IEPSF                             = 0xF3,
  PID_IEPTIO                            = 0xF4,
  PID_DCI                               = 0xA8,
  PID_NPK                               = 0x88,
  PID_GPIOCOM0                          = 0xC2,
  PID_GPIOCOM1                          = 0xC5,
  PID_MODPHY0                           = 0xB9,
  PID_MODPHY1                           = 0xBA,
  PID_MODPHY2                           = 0xBB,
  PID_TUNIT0                            = 0x50,
  PID_TUNIT1                            = 0x51,
  PID_TUNIT2                            = 0x52,
  PID_AUNIT0                            = 0x4D,
  PID_AUNIT1                            = 0x4F,
  PID_BUNIT0                            = 0x4C,
  PID_BUNIT1                            = 0x4B,
  PID_CUNIT                             = 0x41,
  PID_DUNIT                             = 0x16,
  PID_DUNIT0                            = 0x10,
  PID_DUNIT1                            = 0x12,
  PID_PUNIT0                            = 0x46,
  PID_PUNIT1                            = 0x47,
  PID_PUNIT2                            = 0x48,
  PID_PUNIT3                            = 0x49,
  PID_PUNIT4                            = 0x4A,
  PID_SCC                               = 0xD6,
  PID_PCIE0                             = 0xB4,
  PID_PCIE1                             = 0xB3,
  PID_GLM0                              = 0x01,
  PID_GLM1                              = 0x02,
  PID_GLM2                              = 0x03,
  PID_GLM3                              = 0x04,
  PID_GLM4                              = 0x05,
  PID_GLM5                              = 0x06,
  PID_GLM6                              = 0x07,
  PID_GLM7                              = 0x08,
  PID_PML                               = 0x0D,
  PID_THERMGLM0                         = 0x9C,
  PID_THERMGLM1                         = 0x9D,
  PID_THERMGLM2                         = 0x9E,
  PID_THERMGLM3                         = 0x9F,
  PID_THERMGLM4                         = 0xDC,
  PID_THERMGLM5                         = 0xDD,
  PID_THERMGLM6                         = 0xDE,
  PID_THERMGLM7                         = 0xDF,
  PID_THERMDDR0                         = 0xBE,
  PID_THERMDDR1                         = 0x0F,
  PID_THERMWM8                          = 0xBC,
  PID_THERMWM16                         = 0xBD,
  PID_THERMGPIO                         = 0xBF  
} PCH_SBI_PID;
#endif
