//
// This file contains a 'Sample Driver' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may be modified by the user, subject to  
// the additional terms of the license agreement               
//
/*++

Copyright (c)  1999 - 2002 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  BaseMemoryTest.h
   
Abstract:

  EFI 2.0 PEIM to provide a PEI memory test service.

--*/

#ifndef _BASE_MEMORY_TEST_H_
#define _BASE_MEMORY_TEST_H_

#define PEI_BASE_MEMORY_TEST_GUID \
  { \
    0xb6ec423c, 0x21d2, 0x490d, {0x85, 0xc6, 0xdd, 0x58, 0x64, 0xea, 0xa6, 0x74} \
  }

typedef struct _PEI_BASE_MEMORY_TEST_PPI PEI_BASE_MEMORY_TEST_PPI;

typedef enum {
  Ignore,
  Quick,
  Sparse,
  Extensive
} PEI_MEMORY_TEST_OP;

typedef
EFI_STATUS
(EFIAPI *PEI_BASE_MEMORY_TEST) (
  IN  EFI_PEI_SERVICES                   **PeiServices,
  IN PEI_BASE_MEMORY_TEST_PPI            * This,
  IN  EFI_PHYSICAL_ADDRESS               BeginAddress,
  IN  UINT64                             MemoryLength,
  IN  PEI_MEMORY_TEST_OP                 Operation,
  OUT EFI_PHYSICAL_ADDRESS               * ErrorAddress
  );

struct _PEI_BASE_MEMORY_TEST_PPI {
  PEI_BASE_MEMORY_TEST  BaseMemoryTest;
};

extern EFI_GUID gPeiBaseMemoryTestPpiGuid;

#endif
