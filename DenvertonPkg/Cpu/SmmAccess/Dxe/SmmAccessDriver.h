/*++
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
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

--*/

#ifndef _SMM_ACCESS_DRIVER_H_
#define _SMM_ACCESS_DRIVER_H_

#include <SaRegs.h>
#include <SaAccess.h>
#include <Protocol/SmmAccess2.h>
#include <Protocol/PciRootBridgeIo.h>

#define SMM_ACCESS_PRIVATE_DATA_SIGNATURE   SIGNATURE_32('4','5','s','a')

//
// Chipset specific constants
//

#define MAX_SMRAM_RANGES 2

// PCI "address" of chipset
#define PCI_BUS 0
#define PCI_DEV 0
#define PCI_FNC 0

// Chipset identifying registers
#define CHIPSET_VID       0x8086
#define CHIPSET_DID       0xA000

//
// Chipset register(s)
//

// SMM configuration register
typedef UINT8 SMRAM;        // System Management RAM Control
#define SMRAM_OFFSET        HUNIT_HSMMCTL


//
// Private data
//

typedef struct {
  UINTN                           Signature;
  EFI_HANDLE                      Handle;
  EFI_SMM_ACCESS2_PROTOCOL        SmmAccess;
  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL *PciRootBridgeIo;
  //
  // Local Data for SMM Access interface goes here
  //
  UINTN                           NumberRegions;
  EFI_SMRAM_DESCRIPTOR            SmramDesc[MAX_SMRAM_RANGES];
} SMM_ACCESS_PRIVATE_DATA;

#define SMM_ACCESS_PRIVATE_DATA_FROM_THIS(a) \
  CR(a, SMM_ACCESS_PRIVATE_DATA, SmmAccess, SMM_ACCESS_PRIVATE_DATA_SIGNATURE)

//
// Prototypes
// Driver model protocol interface
//

EFI_STATUS
EFIAPI
SmmAccessDriverEntryPoint(
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  );

EFI_STATUS
EFIAPI
Open (
  IN EFI_SMM_ACCESS2_PROTOCOL *This
  );

EFI_STATUS
EFIAPI
Close (
  IN EFI_SMM_ACCESS2_PROTOCOL *This
  );

EFI_STATUS
EFIAPI
Lock (
  IN EFI_SMM_ACCESS2_PROTOCOL *This
  );

EFI_STATUS
EFIAPI
GetCapabilities (
  IN CONST EFI_SMM_ACCESS2_PROTOCOL    *This,
  IN OUT UINTN                         *SmramMapSize, 
  IN OUT EFI_SMRAM_DESCRIPTOR          *SmramMap
  );

#endif