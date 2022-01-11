/** @file
  Interface definition details between PEI and DXE phase for NonVolatile Memory.

@copyright
  Copyright (c) 2010 - 2015 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/*
 * NonVolatileMemoryHob.h
 *
 *  Created on: Apr 15, 2014
  */

#ifndef NONVOLATILEMEMORYHOB_H_
#define NONVOLATILEMEMORYHOB_H_

#include <NonVolatileMemTypes.h>

extern EFI_GUID gNonVolatileMemHobGuid;

typedef enum {
	NONV_FIXED = 0,
	NONV_PARTITION = 1
} NONV_LBA_LOC_METHOD;

typedef struct {
	UINT8   NonVolMemMode;
	UINT8   AdrRecoveryDetected;
	UINT8   NVDIMM_SMBUS_ADDR[NONVOL_MAX_DIMMS]; //SMBus address only if NVDIMM present
	UINT32  C2fLowStart;   //Address of sub 4G managed data; Zero if none
	UINT32  C2fLowLength;  //Size of sub 4G managed data; Zero if none
	UINT64  C2fHighStart;  //Address of above 4G managed data; Zero if none
	UINT64  C2fHighLength; //Size of >4G managed data; Zero if none
	NONV_LBA_LOC_METHOD  C2fLbaLocMethod;
    UINT32  C2fLba;

    UINT8   Revision;
} NON_VOLATILE_MEM_HOB;


#endif /* NONVOLATILEMEMORYHOB_H_ */
