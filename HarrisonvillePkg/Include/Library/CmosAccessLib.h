//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/** @file
  This library class provides CMOS access functions.

Copyright (c) 2010 - 2013, Intel Corporation. All rights reserved.<BR>
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

**/

#ifndef _CMOS_ACCESS_LIB_H_
#define _CMOS_ACCESS_LIB_H_

/**
  Returns the value from a CMOS location. 

  If the passed address is beyond the max address return RETURN_NOT_FOUND.
  If the function completed successfully return RETURN_SUCCESS
  
  @param [in]    CMOSAddress  Location to read from CMOS
  @param [out]   Data         The Value read from CMOS location

  @return Status.
**/
RETURN_STATUS
EFIAPI
ReadCmos (
  IN  UINT8 CMOSAddress,
  OUT UINT8 *Data
  );

/**
  Write the value from a CMOS location.

  If the passed address is beyond the max address return RETURN_NOT_FOUND.
  If the function completed successfully return RETURN_SUCCESS
  
  @param [in]  CMOSAddress  Location to write to CMOS
  @param [in]  Data         Value to be written to the CMOS

  @return Status.
**/
RETURN_STATUS
EFIAPI
WriteCmos (
  IN UINT8 CMOSAddress,
  IN UINT8 *Data
  );

/**
  Returns the 1 Byte value from a CMOS location. 

  If the passed address is beyond the max address return 0.
  
  @param [in]    CMOSAddress  Location to read from CMOS

  @return The Value read from CMOS location
**/
UINT8
EFIAPI
ReadCmos8 (
  IN  UINT8  CMOSAddress
  );

/**
  Write the 1 Byte value from a CMOS location.

  If the passed address is beyond the max address return RETURN_NOT_FOUND.
  If the function completed successfully return RETURN_SUCCESS
  
  @param [in]  CMOSAddress  Location to write to CMOS
  @param [in]  Data         Value to be written to the CMOS

  @return Status.
**/
RETURN_STATUS
EFIAPI
WriteCmos8 (
  IN UINT8  CMOSAddress,
  IN UINT8  Data
  );

/**
  Returns the 2 Bytes value from a CMOS location. 

  If the passed address is beyond the max address return 0.
  
  @param [in]    CMOSAddress  Location to read from CMOS

  @return The Value read from CMOS location
**/
UINT16
EFIAPI
ReadCmos16 (
  IN  UINT8  CMOSAddress
  );

/**
  Write the 2 Bytes value from a CMOS location.

  If the passed address is beyond the max address return RETURN_NOT_FOUND.
  If the function completed successfully return RETURN_SUCCESS
  
  @param [in]  CMOSAddress  Location to write to CMOS
  @param [in]  Data         Value to be written to the CMOS

  @return Status.
**/
RETURN_STATUS
EFIAPI
WriteCmos16 (
  IN UINT8  CMOSAddress,
  IN UINT16 Data
  );

/**
  Returns the 4 Bytes value from a CMOS location. 

  If the passed address is beyond the max address return 0.
  
  @param [in]    CMOSAddress  Location to read from CMOS
  @return The Value read from CMOS location
**/
UINT32
EFIAPI
ReadCmos32 (
  IN  UINT8  CMOSAddress
  );

/**
  Write the 4 Bytes value from a CMOS location.

  If the passed address is beyond the max address return RETURN_NOT_FOUND.
  If the function completed successfully return RETURN_SUCCESS
  
  @param [in]  CMOSAddress  Location to write to CMOS
  @param [in]  Data         Value to be written to the CMOS

  @return Status.
**/
RETURN_STATUS
EFIAPI
WriteCmos32 (
  IN UINT8  CMOSAddress,
  IN UINT32 Data
  );

/**
  Funtion to Initialize the CMOS.

  Checks the presence of CMOS battery, else it initialize CMOS to default.
  Perform a checksum computation and verify if the checksum is correct.
  If the input parameter ForceInit is TRUE, initialize all the CMOS 
  location to their default values 

  @param [in]  ForceInit  A boolean variable to initialize the CMOS to its default
                          without checking the RTC_PWR_STS or verifying the checksum.

  @param [out]  DefaultsRestored  A boolean variable to indicate if the defaults were restored

  @return Status.
**/
RETURN_STATUS
EFIAPI
InitCmos (
  IN  BOOLEAN     ForceInit,
  OUT BOOLEAN     *DefaultsRestored
  );

#endif // _CMOS_ACCESS_LIB_H_
