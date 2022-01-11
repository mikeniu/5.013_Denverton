/*++
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Mobile Silicon Support Module" and is
  licensed for Intel Mobile CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
--*/
/*++

Copyright (c)  1999 - 2014 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  SmmAccessDriver.h

Abstract:

  Header file for SMM Access Driver.

Revision History:

--*/

#ifndef _PEI_SMM_ACCESS_DRIVER_H
#define _PEI_SMM_ACCESS_DRIVER_H

#include "PiPei.h"
#include "Library/HobLib.h"
#include "Guid/SmramMemoryReserve.h"
#include "Ppi/SmmAccess.h"
#include "Library/DebugLib.h"
#include "Library/BaseMemoryLib.h"
#include "SaAccess.h"
#include "SaRegs.h"

//
// Chipset specific constants
//
#define MAX_SMRAM_RANGES 2

//
// Chipset register(s)
//
typedef UINT8 SMRAM;        // System Management RAM Control

//
// Private data structure
//
#define  SMM_ACCESS_PRIVATE_DATA_SIGNATURE SIGNATURE_32 ('i', 's', 'm', 'a')

typedef struct {
  UINTN                           Signature;
  EFI_HANDLE                      Handle;
  PEI_SMM_ACCESS_PPI              SmmAccess;
  //
  // Local Data for SMM Access interface goes here
  //
  UINTN                           NumberRegions;
  EFI_SMRAM_DESCRIPTOR            SmramDesc[MAX_SMRAM_RANGES];
} SMM_ACCESS_PRIVATE_DATA;

#define SMM_ACCESS_PRIVATE_DATA_FROM_THIS(a) \
  CR (a, SMM_ACCESS_PRIVATE_DATA, SmmAccess, SMM_ACCESS_PRIVATE_DATA_SIGNATURE)


//
// Prototypes
// Driver model protocol interface
//
EFI_STATUS
EFIAPI
SmmAccessDriverEntryPoint(
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
/*++

Routine Description:

  This is the standard EFI driver point that detects
  whether there is an proper chipset in the system
  and if so, installs an SMM Access Protocol.

Arguments:

  ImageHandle  -  Handle for the image of this driver.
  SystemTable  -  Pointer to the EFI System Table.

Returns:

  EFI_SUCCESS      -  Protocol successfully started and installed.
  EFI_UNSUPPORTED  -  Protocol can't be started.

--*/
;

EFI_STATUS
EFIAPI
Open (
  IN EFI_PEI_SERVICES        **PeiServices,
  IN PEI_SMM_ACCESS_PPI      *This,
  IN UINTN                   DescriptorIndex
  )
/*++

Routine Description:

  This routine accepts a request to "open" a region of SMRAM.  The
  region could be legacy ABSEG, HSEG, or TSEG near top of physical memory.
  The use of "open" means that the memory is visible from all boot-service
  and SMM agents.

Arguments:

  This             -  Pointer to the SMM Access Interface.
  DescriptorIndex  -  Region of SMRAM to Open.

Returns:

  EFI_SUCCESS            -  The region was successfully opened.
  EFI_DEVICE_ERROR       -  The region could not be opened because locked by
                            chipset.
  EFI_INVALID_PARAMETER  -  The descriptor index was out of bounds.

--*/
;

EFI_STATUS
EFIAPI
Close (
  IN EFI_PEI_SERVICES        **PeiServices,
  IN PEI_SMM_ACCESS_PPI      *This,
  IN UINTN                   DescriptorIndex
  )
/*++

Routine Description:

  This routine accepts a request to "close" a region of SMRAM.  This is valid for
  compatible SMRAM region.

Arguments:

  This             -  Pointer to the SMM Access Interface.
  DescriptorIndex  -  Region of SMRAM to Close.

Returns:

  EFI_SUCCESS            -  The region was successfully closed.
  EFI_DEVICE_ERROR       -  The region could not be closed because locked by
                            chipset.
  EFI_INVALID_PARAMETER  -  The descriptor index was out of bounds.

--*/
;

EFI_STATUS
EFIAPI
Lock (
  IN EFI_PEI_SERVICES        **PeiServices,
  IN PEI_SMM_ACCESS_PPI      *This,
  IN UINTN                   DescriptorIndex
  )
/*++

Routine Description:

  This routine accepts a request to "lock" SMRAM.  The
  region could be legacy AB or TSEG near top of physical memory.
  The use of "lock" means that the memory can no longer be opened
  to BS state..

Arguments:

  This             -  Pointer to the SMM Access Interface.
  DescriptorIndex  -  Region of SMRAM to Lock.

Returns:

  EFI_SUCCESS            -  The region was successfully locked.
  EFI_DEVICE_ERROR       -  The region could not be locked because at least
                            one range is still open.
  EFI_INVALID_PARAMETER  -  The descriptor index was out of bounds.

--*/
;

EFI_STATUS
EFIAPI
GetCapabilities (
  IN EFI_PEI_SERVICES            **PeiServices,
  IN PEI_SMM_ACCESS_PPI          *This,
  IN OUT   UINTN                       *SmramMapSize,
  IN OUT   EFI_SMRAM_DESCRIPTOR        *SmramMap
  )
/*++

Routine Description:

  This routine services a user request to discover the SMRAM
  capabilities of this platform.  This will report the possible
  ranges that are possible for SMRAM access, based upon the
  memory controller capabilities.

Arguments:

  This          -  Pointer to the SMRAM Access Interface.
  SmramMapSize  -  Pointer to the variable containing size of the
                   buffer to contain the description information.
  SmramMap      -  Buffer containing the data describing the Smram
                   region descriptors.

Returns:

  EFI_BUFFER_TOO_SMALL  -  The user did not provide a sufficient buffer.
  EFI_SUCCESS           -  The user provided a sufficiently-sized buffer.

--*/
;

#endif
