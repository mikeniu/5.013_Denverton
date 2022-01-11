//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/** @file
  This library class provides DebugProperty action.

Copyright (c) 2010 - 2011, Intel Corporation. All rights reserved.<BR>
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

**/

#ifndef _DEBUG_PROPERTY_LIB_H_
#define _DEBUG_PROPERTY_LIB_H_

/**
  This interface receive a UINT32 code and check if there is some action need
  to be taken, like DeadLoop(). Most likely the comsumer is PostCodeLib.

  @param  Value  The 32-bit code.
**/
VOID
EFIAPI
DebugPropertyAction (
  IN UINT32  Value
  );

#endif
