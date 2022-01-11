/*++        

Copyright (c)  2006 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

Module Name:

  DigitalThermalSensorLib.h

Abstract:

  Defines and prototypes for the DigitalThermalSensor SMM driver
  
--*/
#ifndef _DIGITAL_THERMAL_SENSOR_LIB_H_
#define _DIGITAL_THERMAL_SENSOR_LIB_H_

EFI_STATUS
InitializeDtsHookLib (
  VOID
)
/*++

Routine Description:

  Prepare data and protocol for Dts Hooe Lib

Arguments:

  None

Returns:

  EFI_SUCCESS - Initialize complete

--*/
;

VOID
PlatformHookBeforeGenerateSCI (
  VOID
)
/*++

Routine Description:

  Platform may need to register some data to private data structure before generate 
  software SMI or SCI.

Arguments:

  None

Returns:

  None
--*/
;

UINT8
ReadPlatformThermalDiode(
   VOID
)
/*++

Routine Description:

  Read CPU temperature from platform diode

Arguments:

  None

Returns:

  TemperatureOfDiode   -  Return the CPU temperature of platform diode

--*/
;

VOID
PlatformEventOutOfSpec (
  VOID
)
/*++

Routine Description:

  When system temperature out of specification, do platform specific programming to prevent 
  system damage.

Arguments:

  None

Returns:

  None

--*/
;

#endif