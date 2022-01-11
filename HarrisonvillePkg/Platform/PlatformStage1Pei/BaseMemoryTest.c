/*++
  This file contains an 'Intel Peripheral Driver' and is        
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/
/*++

Copyright (c)  1999 - 2015 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:
  
  BaseMemoryTest.c
   
Abstract:
  
  Produce MemoryTest Ppi.
  
--*/


#include "PlatformStage1.h"

EFI_STATUS
EFIAPI
BaseMemoryTest (
  IN  CONST EFI_PEI_SERVICES             **PeiServices,
  IN  PEI_BASE_MEMORY_TEST_PPI           *This, 
  IN  EFI_PHYSICAL_ADDRESS               BeginAddress,
  IN  UINT64                             MemoryLength,
  IN  PEI_MEMORY_TEST_OP                 Operation,
  OUT EFI_PHYSICAL_ADDRESS               *ErrorAddress
  )
/*++

Routine Description:

  This function checks the memory range in PEI. 

Arguments:

  PeiServices     Pointer to PEI Services.
  This            Pei memory test PPI pointer.
  BeginAddress    Beginning of the memory address to be checked.
  MemoryLength    Bytes of memory range to be checked.
  Operation       Type of memory check operation to be performed.
  ErrorAddress    Return the address of the error memory address.
    
Returns:
  
  EFI_SUCCESS         The operation completed successfully.
  EFI_DEVICE_ERROR    Memory test failed. It's not safe to use this range of memory.

--*/  
{
#ifdef EFI_NO_MEMORY_TEST
  return EFI_SUCCESS;
#else
  UINT32                                 TestPattern;
  UINT32                                 TestMask;
  UINT32                                 SpanSize;
  EFI_PHYSICAL_ADDRESS                   TempAddress;

  (*PeiServices)->ReportStatusCode (
                    PeiServices,
                    EFI_PROGRESS_CODE,
                    EFI_COMPUTING_UNIT_MEMORY + EFI_CU_MEMORY_PC_TEST,   
                    0,
                    NULL,
                    NULL
                    );

  TestPattern = MEMORY_TEST_PATTERN;
  TestMask = 0;
  SpanSize = 0;

  //
  // Make sure we don't try and test anything above the max physical address range
  //
  ASSERT_EFI_ERROR (BeginAddress + MemoryLength < MAX_ADDRESS);

  switch (Operation) {
    case Extensive:
      SpanSize = 0x4;
      break;
    case Sparse:
    case Quick:
      SpanSize = MEMORY_TEST_COVER_SPAN;
      break;
    case Ignore:
      goto Done;
      break;
  }
  
  //
  // Write the test pattern into memory range
  //
  TempAddress = BeginAddress;
  while (TempAddress < BeginAddress + MemoryLength) {
    (*(UINT32*)(UINTN)TempAddress) = TestPattern;
    TempAddress += SpanSize;
  }
  
  //
  // Read pattern from memory and compare it
  //
  TempAddress = BeginAddress;
  while (TempAddress < BeginAddress + MemoryLength){
    if ((*(UINT32*)(UINTN)TempAddress) != TestPattern) {
      *ErrorAddress = TempAddress;
      (*PeiServices)->ReportStatusCode (
        PeiServices,
        EFI_ERROR_CODE + EFI_ERROR_UNRECOVERED,
        EFI_COMPUTING_UNIT_MEMORY + EFI_CU_MEMORY_EC_UNCORRECTABLE,
        0,
        NULL,
        NULL
        );

      return EFI_DEVICE_ERROR;
    }
    TempAddress += SpanSize;
  }

Done:
  return EFI_SUCCESS;
#endif
}
