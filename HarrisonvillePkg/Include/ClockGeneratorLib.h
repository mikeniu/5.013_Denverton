/*++
 This file contains an 'Intel Peripheral Driver' and is        
 licensed for Intel CPUs and chipsets under the terms of your  
 license agreement with Intel or your vendor.  This file may   
 be modified by the user, subject to additional terms of the   
 license agreement                                             
--*/
/*++

Copyright (c) 2006 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

Module Name:

  ClockGeneratorLib.h

Abstract:

  Clock Generator library definitions.

  This library provides basic clock generator configuration services.

  There may be different libraries for different environments (PEI, BS, RT, SMM).
  Make sure you meet the requirements for the library (protocol dependencies, use
  restrictions, etc).

--*/
#ifndef _CLOCK_GENERATOR_LIB_H_
#define _CLOCK_GENERATOR_LIB_H_

//
// Known clock generator types
//
typedef enum {
  ClockGeneratorCk410,
  ClockGeneratorCk505,
  ClockGeneratorMax
} CLOCK_GENERATOR_TYPE;

//
// Clock controller specific definitions
//

//
// CK410 Definitions
//
#define CK410_GENERATOR_ID                          0x65
#define CK410_GENERATOR_SPREAD_SPECTRUM_BYTE        1
#define CK410_GENERATOR_SPREAD_SPECTRUM_BIT         (1 << 1)
#define CK410_GENERATOR_CLOCK_FREERUN_BYTE          4
#define CK410_GENERATOR_CLOCK_FREERUN_BIT           (1 << 7)

//
// CK505 Definitions
//
#define CK505_GENERATOR_ID                          0x26
// Bit 0-1, Byte 4 is the Spread Spectrum enabling bits: 1 on; 0 off
#define CK505_GENERATOR_SPREAD_SPECTRUM_BYTE        4
#define CK505_GENERATOR_SPREAD_SPECTRUM_BIT         3         // Bit 0-1

//
// CK505 doesn't support CPU free-running operation,
// so we define the byte and bit as 0xFF
//
#define CK505_GENERATOR_CLOCK_FREERUN_BYTE          0xFF
#define CK505_GENERATOR_CLOCK_FREERUN_BIT           0xFF

//
// An arbitrary maximum length for clock generator buffers
//
#define MAX_CLOCK_GENERATOR_BUFFER_LENGTH           0x20

//
// Library functions
//

EFI_STATUS
ConfigureClockGenerator (
  IN CONST EFI_PEI_SERVICES            **PeiServices,
  IN     CLOCK_GENERATOR_TYPE          ClockType,
  IN     UINT8                         ClockAddress,
  IN     BOOLEAN                       EnableSpreadSpectrum,
  IN     UINTN                         ConfigurationTableLength,
  IN OUT UINT8                         *ConfigurationTable
  );
/*++

Routine Description:

  Configure the clock generator according to input.
  Enable spread spectrum simply provides an optimization to simplify caller.

Arguments:

  PeiServices               - For PEI implementations, this must contain PEI services pointer.
                              For other implementations, this must be NULL
  ClockType                 - CLock generator's model name
  ClockAddress              - SMBUS address of clock generator
  ConfigurationTableLength  - Length (in bytes) of configuration table,
                              must be <= MAX_CLOCK_GENERATOR_BUFFER_LENGTH
  ConfigurationTable        - Pointer of configuration table at least ConfigurationTableLength bytes in size

Returns:

  EFI_SUCCESS               - The function completed successfully

--*/

#endif