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

  Dimm.c

Abstract:

  PPI for reading DIMM SPDs.

--*/


#include "PlatformStage2.h"

//
// Total number of DIMM sockets allowed on the platform
//
#define DIMM_SOCKETS       4

//
// Total number of Segments Per DIMM
//
#define DIMM_SEGMENTS      1

//
// Total number of memory channels populated on the system board
//
#define MEMORY_CHANNELS    2

//
// Prototypes
//

EFI_STATUS
EFIAPI
GetDimmState (
  IN      EFI_PEI_SERVICES        **PeiServices,
  IN      PEI_PLATFORM_DIMM_PPI   *This,
  IN      UINT8                   Dimm,
  OUT     PEI_PLATFORM_DIMM_STATE *State
  );

EFI_STATUS
EFIAPI
SetDimmState (
  IN      EFI_PEI_SERVICES        **PeiServices,
  IN      PEI_PLATFORM_DIMM_PPI   *This,
  IN      UINT8                   Dimm,
  IN      PEI_PLATFORM_DIMM_STATE *State
  );

EFI_STATUS
EFIAPI
ReadSpd (
  IN      EFI_PEI_SERVICES      **PeiServices,
  IN      PEI_PLATFORM_DIMM_PPI *This,
  IN      UINT8                 Dimm,
  IN      UINT8                 Offset,
  IN      UINTN                 Count,
  IN OUT  UINT8                 *Buffer
  );

static PEI_PLATFORM_DIMM_PPI mPlatformDimmPpi = {
  DIMM_SOCKETS,
  DIMM_SEGMENTS,
  MEMORY_CHANNELS,
  GetDimmState,
  SetDimmState,
  ReadSpd
};

static EFI_PEI_PPI_DESCRIPTOR mPpiPlatformDimm = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gPeiPlatformDimmPpiGuid,
  &mPlatformDimmPpi
};


//
// Functions
//

EFI_STATUS
EFIAPI
GetDimmState (
  IN      EFI_PEI_SERVICES        **PeiServices,
  IN      PEI_PLATFORM_DIMM_PPI   *This,
  IN      UINT8                   Dimm,
  OUT     PEI_PLATFORM_DIMM_STATE *State
  )
/*++

Routine Description:

  This function returns the current state of a single DIMM.  Present indicates
  that the DIMM slot is physically populated.  Disabled indicates that the DIMM
  should not be used.

Arguments:

  PeiServices - PEI services table pointer
  This        - PPI pointer
  Dimm        - DIMM to read from
  State       - Pointer to a return buffer to be updated with the current state of the DIMM

Returns:

  EFI_SUCCESS - The function completed successfully.

--*/
{
  //
  // Add setup options to allow the user enable/disable DIMM slots.
  // Currently, just initialize the state of the DIMM.
  //
  State->Disabled = 0;
  State->Present  = 0;
  State->Reserved = 0;

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
SetDimmState (
  IN      EFI_PEI_SERVICES        **PeiServices,
  IN      PEI_PLATFORM_DIMM_PPI   *This,
  IN      UINT8                   Dimm,
  IN      PEI_PLATFORM_DIMM_STATE *State
  )
/*++

Routine Description:

  This function updates the state of a single DIMM.

Arguments:

  PeiServices - PEI services table pointer
  This        - PPI pointer
  Dimm        - DIMM to set state for
  State       - Pointer to the state information to set.

Returns:

  EFI_SUCCESS       - The function completed successfully.
  EFI_UNSUPPORTED   - The function is not supported.

--*/
{
  return EFI_UNSUPPORTED;
}

EFI_STATUS
EFIAPI
ReadSpd (
  IN      EFI_PEI_SERVICES      **PeiServices,
  IN      PEI_PLATFORM_DIMM_PPI *This,
  IN      UINT8                 Dimm,
  IN      UINT8                 Offset,
  IN      UINTN                 Count,
  IN OUT  UINT8                 *Buffer
  )
/*++

Routine Description:

  This function reads SPD information from a DIMM.

Arguments:

  PeiServices   - PEI services table pointer
  This          - PPI pointer
  Dimm          - DIMM to read from
  Offset        - Offset in DIMM
  Count         - Number of bytes
  Buffer        - Return buffer

Returns:

  EFI_SUCCESS           - The function completed successfully.
  EFI_DEVICE_ERROR      - The DIMM being accessed reported a device error,
                          does not have an SPD module, or is not installed in the system.
  EFI_TIMEOUT           - Time out trying to read the SPD module.
  EFI_INVALID_PARAMETER - A parameter was outside the legal limits.

--*/
{
  EFI_STATUS                Status;
  PEI_SMBUS_PPI             *Smbus;
  UINTN                     Index;
  EFI_SMBUS_DEVICE_ADDRESS  SlaveAddress;
  EFI_SMBUS_DEVICE_COMMAND  Command;
  UINTN                     Length;
  UINTN                     Index1;

  PLATFORM_INFO_HOB         *PlatformInfoHob;
  Status = (**PeiServices).LocatePpi (
                             PeiServices,
                             &gPeiSmbusPpiGuid,
                             0,
                             NULL,
                             &Smbus
                             );
  ASSERT_EFI_ERROR ( Status);

  Status = GetPlatformInfo (PeiServices, &PlatformInfoHob);
  ASSERT_EFI_ERROR ( Status);
#ifdef ERB_FLAG
  if (!EFI_ERROR(Status) 
	  && ((PlatformInfoHob->BoardId == BoardIdHaddockCreek) && (PlatformInfoHob->BoardRev == 0)
	  )) {
			if (Dimm == 0) {
				SlaveAddress.SmbusDeviceAddress = SMBUS_ADDR_CH_A_2 >> 1;
			} else if (Dimm == 1) {
				SlaveAddress.SmbusDeviceAddress = SMBUS_ADDR_CH_A_1 >> 1;
			} else  if (Dimm == 2) {
				SlaveAddress.SmbusDeviceAddress = SMBUS_ADDR_CH_B_2 >> 1;
			} else if (Dimm == 3) {
				SlaveAddress.SmbusDeviceAddress = SMBUS_ADDR_CH_B_1 >> 1;
			} else return EFI_INVALID_PARAMETER;
		}
#endif
  else {
	  if (Dimm == 0) {
	    SlaveAddress.SmbusDeviceAddress = SMBUS_ADDR_CH_A_1 >> 1;
	  } else if (Dimm == 1) {
	    SlaveAddress.SmbusDeviceAddress = SMBUS_ADDR_CH_A_2 >> 1;
	  } else  if (Dimm == 2) {
	    SlaveAddress.SmbusDeviceAddress = SMBUS_ADDR_CH_B_1 >> 1;
	  } else if (Dimm == 3) {
	    SlaveAddress.SmbusDeviceAddress = SMBUS_ADDR_CH_B_2 >> 1;
	  } else return EFI_INVALID_PARAMETER;
  } // end else for HC

  Index = Count % 4;
  if (Index != 0) {
    //
    // read the first serveral bytes to speed up following reading
    //
    for (Index1 = 0; Index1 < Index; Index1++) {
      Length = 1;
      Command = Offset + Index1;
      Status = Smbus->Execute (
                          PeiServices,
                          Smbus,
                          SlaveAddress,
                          Command,
                          EfiSmbusReadByte,
                          FALSE,
                          &Length,
                          &Buffer[Index1]
                          );
      if (EFI_ERROR(Status)) {
        return Status;
      }
    }
  }

  //
  // Now collect all the remaining bytes on 4 bytes block
  //
  for (; Index < Count; Index += 2) {
    Command = Index + Offset;
    Length = 2;
    Status = Smbus->Execute (
                      PeiServices,
                      Smbus,
                      SlaveAddress,
                      Command,
                      EfiSmbusReadWord,
                      FALSE,
                      &Length,
                      &Buffer[Index]
                      );
    if (EFI_ERROR(Status)) {
      return Status;
    }

    Index += 2;
    Command = Index + Offset;
    Length = 2;
    Status = Smbus->Execute (
                      PeiServices,
                      Smbus,
                      SlaveAddress,
                      Command,
                      EfiSmbusReadWord,
                      FALSE,
                      &Length,
                      &Buffer[Index]
                      );
    if (EFI_ERROR(Status)) {
      return Status;
    }
  }

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
PeimInitializeDimm (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *SmbusPpi
  )
/*++

Routine Description:

  This function initializes the PEIM.  It simply installs the DIMM PPI.

Arguments:

  PeiServices       - Pointer to PEI services table.
  NotifyDescriptor  - PPI notify descriptor pointer.
  SmbusPpi          - SmBus PPI instance pointer.

Returns:

  EFI_SUCCESS   - The function completed successfully.

--*/
{
  EFI_STATUS    Status;

  Status = (**PeiServices).InstallPpi (PeiServices, &mPpiPlatformDimm);
  ASSERT_EFI_ERROR ( Status);

  return EFI_SUCCESS;
}

