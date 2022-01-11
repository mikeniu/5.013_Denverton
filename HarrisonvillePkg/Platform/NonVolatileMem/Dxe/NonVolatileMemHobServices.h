//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//
/*++

@copyright
  Copyright (c) 2014 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may be modified by the user, subject to
  the additional terms of the license agreement


Module Name:

 NonVolatileMemHobServices.h

Abstract:

  common retrieve function for NonVolatile Memory HOB

--*/


#ifndef NONVOLATILEMEMHOBSERVICES_H_
#define NONVOLATILEMEMHOB_H_

#include <Base.h>
#include <Guid/NonVolatileMemoryHob.h>

VOID GetNonVolatileMemHob(NON_VOLATILE_MEM_HOB **NonVolatileMemHob);

#endif /* NONVOLATILEMEMHOBSEERVICES_H_ */
