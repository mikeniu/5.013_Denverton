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

  PeiClockGenerator.c
    
Abstract:

  Implements services to configure a clock generator in PEI phase
  using SMBUS services.

--*/

#include <Uefi/UefiBaseType.h>
#include <Pi/PiPeiCis.h>
#include <Ppi/Smbus.h>              // IntelFramework
#include <Library/DebugLib.h>

#include "ClockGeneratorLib.h"

//
// Clock generator details table
//
typedef struct {
  CLOCK_GENERATOR_TYPE      ClockType;
  UINT8                     ClockId;
  UINT8                     SpreadSpectrumByteOffset;
  UINT8                     SpreadSpectrumBitOffset;
} CLOCK_GENERATOR_DETAILS;

//
// Table providing details on clocks supported by this library
//
// It is critical that this table be properly constructed.
// The table entries must line up with the clock generatory types
//
CLOCK_GENERATOR_DETAILS   mSupportedClockGeneratorTable[] =
{
  { ClockGeneratorCk410, CK410_GENERATOR_ID , CK410_GENERATOR_SPREAD_SPECTRUM_BYTE, CK410_GENERATOR_SPREAD_SPECTRUM_BIT },
  { ClockGeneratorCk505, CK505_GENERATOR_ID , CK505_GENERATOR_SPREAD_SPECTRUM_BYTE, CK505_GENERATOR_SPREAD_SPECTRUM_BIT }
};

EFI_STATUS
ConfigureClockGenerator (
  IN CONST EFI_PEI_SERVICES          **PeiServices,
  IN     CLOCK_GENERATOR_TYPE          ClockType,
  IN     UINT8                         ClockAddress,
  IN     BOOLEAN                       EnableSpreadSpectrum,
  IN     UINTN                         ConfigurationTableLength,
  IN OUT UINT8                         *ConfigurationTable
  )
/*++

Routine Description:

  Configure the clock generator using the SMBUS PPI services.

  This function applies the desired spread spectrum configuration, performs
  a block write, and dumps debug information.

Arguments:

  PeiServices               - General purpose services available to every PEIM.
  ClockType                 - Clock generator's model name.
  ClockAddress              - SMBUS address of clock generator.
  EnableSpreadSpectrum      - Enable/Disable Spread Spectrum function.
  ConfigurationTableLength  - Length of configuration table.
  ConfigurationTable        - Pointer of configuration table.

Returns:

  EFI_SUCCESS - Operation success.

--*/
{
  EFI_STATUS                    Status;
  EFI_SMBUS_DEVICE_ADDRESS      SlaveAddress;
  UINT8                         Buffer[MAX_CLOCK_GENERATOR_BUFFER_LENGTH];
  UINTN                         Length;
  EFI_SMBUS_DEVICE_COMMAND      Command;
  EFI_PEI_SMBUS_PPI             *SmbusPpi;

  //
  // Locate SMBUS PPI
  //
  Status = (**PeiServices).LocatePpi (
    PeiServices,
    &gEfiPeiSmbusPpiGuid,
    0,                  // INSTANCE
    NULL,               // EFI_PEI_PPI_DESCRIPTOR
    &SmbusPpi           // PPI
    );
  ASSERT_EFI_ERROR (Status);

  //
  // Verify input arguments
  //
  //ASSERT_EFI_ERROR (ConfigurationTableLength >= 8);
  //ASSERT_EFI_ERROR (ConfigurationTableLength <= MAX_CLOCK_GENERATOR_BUFFER_LENGTH);
  //ASSERT_EFI_ERROR (ClockType < ClockGeneratorMax);
  //ASSERT_EFI_ERROR (ConfigurationTable != NULL);
  //
  // Read the clock generator
  //
  SlaveAddress.SmbusDeviceAddress = ClockAddress >> 1;
  Length = MAX_CLOCK_GENERATOR_BUFFER_LENGTH;
  Command = 0;
  Status = SmbusPpi->Execute (
    (EFI_PEI_SERVICES **)PeiServices,
    SmbusPpi,
    SlaveAddress,
    Command,
    EfiSmbusReadBlock,
    FALSE,
    &Length,
    Buffer
    );

  //
  // Sanity check that the requested clock type is present in our supported clocks table
  //
  //DEBUG ((DEBUG_ERROR | DEBUG_INFO, "Expected Clock Generator ID is %x, expecting %x\n", mSupportedClockGeneratorTable[ClockType].ClockId, Buffer[7]));


  //
  // Program Spread Spectrum function
  //
  if (EnableSpreadSpectrum) {
    ConfigurationTable[mSupportedClockGeneratorTable[ClockType].SpreadSpectrumByteOffset] |= mSupportedClockGeneratorTable[ClockType].SpreadSpectrumBitOffset;
  } else {
    ConfigurationTable[mSupportedClockGeneratorTable[ClockType].SpreadSpectrumByteOffset] &= ~(mSupportedClockGeneratorTable[ClockType].SpreadSpectrumBitOffset);
  }

  //
  // Program clock generator
  //
  Command = 0;
  Status = SmbusPpi->Execute (
    (EFI_PEI_SERVICES **)PeiServices,
    SmbusPpi,
    SlaveAddress,
    Command,
    EfiSmbusWriteBlock,
    FALSE,
    &ConfigurationTableLength,
    ConfigurationTable
    );


    //ASSERT_EFI_ERROR(Status);

  //
  // Dump contents after write
  //
  DEBUG_CODE (
    {

      SlaveAddress.SmbusDeviceAddress = ClockAddress >> 1;
      Length = MAX_CLOCK_GENERATOR_BUFFER_LENGTH;
      Command = 0;
      Status =  SmbusPpi->Execute (
        (EFI_PEI_SERVICES **) PeiServices,
        SmbusPpi,
        SlaveAddress,
        Command,
        EfiSmbusReadBlock,
        FALSE,
        &Length,
        Buffer
        );
      //ASSERT_EFI_ERROR (Status);


    }
  );
  return EFI_SUCCESS;
}
