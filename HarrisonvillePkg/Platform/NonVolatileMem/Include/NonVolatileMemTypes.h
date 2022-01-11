/** @file
  Defines types used my multiple NonVolatile files.

@copyright
  Copyright (c) 2014 - 2014 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/*
 * NonVolatileMemTypes.h
 *
 *  Created on: Apr 15, 2014
 */

#ifndef NONVOLATILEMEMTYPES_H_
#define NONVOLATILEMEMTYPES_H_

typedef enum {
    NONVOL_DISABLED = 0,
    NONVOL_ADR      = 1,
    NONVOL_ADR_C2F  = 2,
    NONVOL_SMI_C2F  = 3,
    NONVOL_NVDIMM   = 4,
    NONVOL_INVALID  = 5, //This value and all larger values are invalide modes
} NonVolMemModes;

#define NONVOL_MAX_DIMMS 4
#define NONVOL_C2F_MODE(mode) ((NONVOL_ADR_C2F==mode) || (NONVOL_SMI_C2F==mode))




#endif /* NONVOLATILEMEMTYPES_H_ */
