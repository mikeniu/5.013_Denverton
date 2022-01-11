/** @file
  Interface definition details between MRC and platform drivers during PEI phase.

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
 * NonVolatileMemPolicy.h
 *
 *  Created on: Apr 11, 2014
  */
#ifndef _NON_VOLATILE_MEM_POLICY_PPI_H_
#define _NON_VOLATILE_MEM_POLICY_PPI_H_

#include <NonVolatileMemTypes.h>
#include <Uefi.h>
#include <Library/SmbusLib.h>
 //#include <SaAccess.h>

//
// Extern the GUID for protocol users.
//
extern EFI_GUID gNonVolatileMemPpiGuid;


#define NON_VOLATILE_MEM_POLICY_PPI_REVISION   1
#define ADR_INTERNAL_SOURCE_COUNT 32

typedef struct {
    //
    // before MRC data
    //
    UINT8   NonVolMemMode;
    UINT8   NonVolMemInterleaving;
    UINT32  NonVolMemSize;
    UINT8   NonVolAdrSourcesSet;
    UINT32  NonVolAdrSources;
    UINT32  NonVolMemAdrStatusSrc;
    UINT8   AdrRecoveryDetected;
    UINT8   eraseArmNVDIMMS;
    UINT8   restoreNVDIMMS;
    UINT8   NonVolMemCacheFlushing;
    UINT8   NonVolMemC2fLbaLocMethod;
    UINT32  NonVolMemC2fLba;
    UINT8   NVDIMM_SMBUS_ADDR[NONVOL_MAX_DIMMS]; //filled in the MRC
    //
    // Post MRC data
    //
    UINT32  C2fLowStart;            //Address of sub 4G managed data; Zero if none
    UINT32  C2fLowLength;           //Size of sub 4G managed data; Zero if none
    UINT64  C2fHighStart;           //Address of above 4G managed data; Zero if none
    UINT64  C2fHighLength;          //Size of >4G managed data; Zero if none
    UINT64  LargestFreeblock;       //Address of largest free block of memory
    UINT64  LargestFreeblockLength; //length of largest free block

    UINT8   Revision;
    UINT8   NonVolMemTestMode;
} NON_VOLATILE_MEM_POLICY_PPI;

#endif //_NON_VOLATILE_MEM_POLICY_PPI_H_




