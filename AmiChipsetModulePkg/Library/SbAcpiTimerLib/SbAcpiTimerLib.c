//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file SbAcpiTimerLib.c
    This file contains south bridge related TimeLib library
    instance

*/

// Module specific Includes
#include "Efi.h"
#include "SB.h"
#include "token.h"
#include <Library/IoLib.h>
#include <Library/BaseLib.h>


/**
    This function delays for the number of micro seconds passed in

    @param MicroSeconds Number of microseconds(us) to delay

    @retval Value passed in for microseconds delay

**/
UINTN
EFIAPI
MicroSecondDelay (
  IN      UINTN                     MicroSeconds
  )
{
/**** PORTING REQUIRED ****
    Stalls the CPU for at least the given number of microseconds
**** PORTING REQUIRED ****/
  return MicroSeconds;
}

/**
    This function delays for the number of nano seconds passed in

    @param MicroSeconds Number of nanoseconds(ns) to delay

    @retval Value passed in for nanoseconds delay

**/
UINTN
EFIAPI
NanoSecondDelay (
  IN      UINTN                     NanoSeconds
  )
{
/**** PORTING REQUIRED ****
    Stalls the CPU for at least the given number of nanoseconds
**** PORTING REQUIRED ****/
    return NanoSeconds;
}

/**
    Retrieves the current value of a 64-bit free running
    performance counter.

    @param VOID

    @retval The current value of the free running performance counter.

**/
UINT64
EFIAPI
GetPerformanceCounter (
  VOID
  )
{
/**** PORTING REQUIRED ****
    Please check the datasheet of the chipset for PM Timer register
**** PORTING REQUIRED ****/
    return (UINT64)IoRead32(PM_BASE_ADDRESS + 0x08);
}

/**
    Retrieves the 64-bit frequency in Hz and the range of
    performance counter values.

    @param StartValue - The value the performance counter starts with when
                        it rolls over.
    @param EndValue - The value that the performance counter ends with
                      before it rolls over.

    @retval The frequency in Hz.

**/
UINT64
EFIAPI
GetPerformanceCounterProperties (
  OUT      UINT64                    *StartValue,  OPTIONAL
  OUT      UINT64                    *EndValue     OPTIONAL
  )
{
/**** PORTING REQUIRED ****
    if (StartValue != NULL) {
      *StartValue = 0;
    }

    if (EndValue != NULL) {
      *EndValue = ACPI_TMR_COUNT_SIZE - 1;
    }
**** PORTING REQUIRED ****/
    return ACPI_TMR_FREQUENCY;

}

/**
    This function converts the elapsed ticks of running performance
    counter to time value in unit of nanoseconds.

    @param Ticks   The number of elapsed ticks of running performance
                   counter.

    @retval The elapsed time in nanoseconds.

**/
UINT64
EFIAPI
GetTimeInNanoSecond (
  IN      UINT64                     Ticks
  )
{
  UINT64  Frequency;
  UINT64  NanoSeconds;
  UINT64  Remainder;
  INTN    Shift;

  Frequency = GetPerformanceCounterProperties (NULL, NULL);

  //
  //          Ticks
  // Time = --------- x 1,000,000,000
  //        Frequency
  //
  NanoSeconds = MultU64x32 (DivU64x64Remainder (Ticks, Frequency, &Remainder), 1000000000u);

  //
  // Ensure (Remainder * 1,000,000,000) will not overflow 64-bit.
  // Since 2^29 < 1,000,000,000 = 0x3B9ACA00 < 2^30, Remainder should < 2^(64-30) = 2^34,
  // i.e. highest bit set in Remainder should <= 33.
  //
  Shift = MAX (0, HighBitSet64 (Remainder) - 33);
  Remainder = RShiftU64 (Remainder, (UINTN) Shift);
  Frequency = RShiftU64 (Frequency, (UINTN) Shift);
  NanoSeconds += DivU64x64Remainder (MultU64x32 (Remainder, 1000000000u), Frequency, NULL);

  return NanoSeconds;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
