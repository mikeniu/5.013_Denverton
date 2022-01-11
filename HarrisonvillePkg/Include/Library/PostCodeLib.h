//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/*++

Copyright (c) 2013 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  PostCodeLib.h

Abstract:

--*/

#ifndef _POST_CODE_LIB_H_
#define _POST_CODE_LIB_H_


/**
  Sends an 32-bit value to a POST card.

  Sends the 32-bit value specified by Value to a POST card, and returns Value.  
  Some implementations of this library function may perform I/O operations 
  directly to a POST card device.  Other implementations may send Value to 
  ReportStatusCode(), and the status code reporting mechanism will eventually 
  display the 32-bit value on the status reporting device.
  
  PostCode() must actively prevent recursion.  If PostCode() is called while 
  processing another any other Post Code Library function, then 
  PostCode() must return Value immediately.

  @param  Value  The 32-bit value to write to the POST card.

  @return The 32-bit value to write to the POST card.

**/
UINT32
EFIAPI
PostCode (
  IN UINT32  Value
  );

#endif
