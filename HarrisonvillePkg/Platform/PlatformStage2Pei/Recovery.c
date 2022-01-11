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

  Recovery.c

Abstract:

  PEIM to provide the platform recovery functionality.

--*/


#include "PlatformStage2.h"
#include <Ppi/RecoveryModule.h>
#include <Ppi/Stall.h>
#include <Ppi/DeviceRecoveryModule.h>
#include <Ppi/AtaController.h>
//#include <Ppi/PchUsbPolicy.h>
//#include <Ppi/PchInit.h>
#include <Library/HobLib.h>
//
// Required Service
//
EFI_STATUS
EFIAPI
PlatformRecoveryModule (
  IN EFI_PEI_SERVICES                   **PeiServices,
  IN EFI_PEI_RECOVERY_MODULE_PPI        *This
  );

EFI_STATUS
EnableAtaChannel (
  IN EFI_PEI_SERVICES       **PeiServices,
  IN PEI_ATA_CONTROLLER_PPI *This,
  IN UINT8                  ChannelIndex
  );

UINT32
GetIdeRegsBaseAddr (
  IN  EFI_PEI_SERVICES       **PeiServices,
  IN  PEI_ATA_CONTROLLER_PPI *This,
  OUT IDE_REGS_BASE_ADDR     *IdeRegsBaseAddr
  );

//
// Module globals
//
static EFI_PEI_RECOVERY_MODULE_PPI mRecoveryPpi = {
  PlatformRecoveryModule
};


static PEI_ATA_CONTROLLER_PPI mAtaControllerPpi = {
  EnableAtaChannel,
  GetIdeRegsBaseAddr
};

static EFI_PEI_PPI_DESCRIPTOR mRecoveryPpiList[2] = {
  {
    (EFI_PEI_PPI_DESCRIPTOR_PPI),
    &gEfiPeiRecoveryModulePpiGuid,
    &mRecoveryPpi
  },
  {
    (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gPeiAtaControllerPpiGuid,
    &mAtaControllerPpi
  }
};

//
// IDE registers' fixed address
//
static IDE_REGS_BASE_ADDR   mAtaChannelIoSpace[4] = {
  {PCH_SATA_PRIMARY_CMD_BAR, PCH_SATA_PRIMARY_CTL_BAR},
  {PCH_SATA_SECONDARY_CMD_BAR, PCH_SATA_SECONDARY_CTL_BAR},
  {PCH_SATA2_PRIMARY_CMD_BAR, PCH_SATA2_PRIMARY_CTL_BAR + 2},
  {PCH_SATA2_SECONDARY_CMD_BAR, PCH_SATA2_SECONDARY_CTL_BAR + 2}
};

EFI_STATUS
EnableAtaChannel (
  IN EFI_PEI_SERVICES               **PeiServices,
  IN PEI_ATA_CONTROLLER_PPI         *This,
  IN UINT8                          ChannelMask
  )
/*++

Routine Description:

  Initializes the Ata Channel

Arguments:

  PeiServices     - The PEI core services table.
  This            - PEI_ATA_CONTROLLER_PPI pointer.
  ChannelMask     - Indicate primary or secondary channel.

Returns:

  EFI_SUCCESS
--*/
{
  EFI_PEI_STALL_PPI  *StallPpi;
  EFI_STATUS     Status;
  PCH_SERIES     PchSeries;
  UINTN          Sata1BaseAddress;

  Sata1BaseAddress = MmPciBase (
                       DEFAULT_PCI_BUS_NUMBER_PCH,
                       PCI_DEVICE_NUMBER_PCH_SATA,
                       PCI_FUNCTION_NUMBER_PCH_SATA
                       );

  PchSeries = GetPchSeries();
  //
  // Config SATA controller D31:F2 Legacy BAR and SIDPBA
  //
  MmioWrite32 (Sata1BaseAddress + R_PCH_SATA_BUS_MASTER_BAR, PCH_SATA_LEGACY_BAR);
  MmioWrite32 (Sata1BaseAddress + R_PCH_SATA_AHCI_BAR, PCH_SATA_SIDPBA);

  if (PchSeries == PchLp) {
    //
    // Set SATA controller D31:F2 to IDE legacy mode
    // Leave SATA controller D31:F5 to IDE native mode by default
    //
    MmioAnd8 (
      Sata1BaseAddress + R_PCH_SATA_PI_REGISTER,
      (UINT8)~(BIT2+BIT0)                 // Clear bits (AND data)
      );
  }

  //
  // Enabled IO Space
  //
  MmioAndThenOr16 (
    Sata1BaseAddress + R_PCH_SATA_COMMAND,
    (UINT16)~(B_PCH_SATA_COMMAND_IOSE | B_PCH_SATA_COMMAND_BME | B_PCH_SATA_COMMAND_MSE),
    (UINT16)B_PCH_SATA_COMMAND_IOSE | B_PCH_SATA_COMMAND_BME
    );
  //
  // Enable Port0/Port1/Port2/Port3 on D31:F2
  //
  MmioOr16 (
    Sata1BaseAddress + R_PCH_SATA_PCS,
    B_PCH_SATA_PCS_PORT0_EN | B_PCH_SATA_PCS_PORT1_EN | B_PCH_SATA_PCS_PORT2_EN);

  if (PchSeries == PchH) {
    //
    // Enable Port4/Port5 on D31:F5
    //
    //MmioOr16 (
    //  Sata2BaseAddress + R_PCH_SATA_PCS,
    //  B_PCH_SATA2_PCS_PORT4_EN | B_PCH_SATA2_PCS_PORT5_EN
    //  );
  }

  //
  // To ensure FvMain can be found, stall 15S
  //
  Status = (**PeiServices).LocatePpi (
                           PeiServices,
                           &gEfiPeiStallPpiGuid,
                           0,
                           NULL,
                           &StallPpi
                           );
  if (!EFI_ERROR (Status)) {
    StallPpi->Stall (
                PeiServices,
                StallPpi,
                15000000
                );
  }

   return EFI_SUCCESS;
}

UINT32
GetIdeRegsBaseAddr (
  IN  EFI_PEI_SERVICES       **PeiServices,
  IN  PEI_ATA_CONTROLLER_PPI *This,
  OUT IDE_REGS_BASE_ADDR     *IdeRegsBaseAddr
  )
/*++

Routine Description:

  Gets the Ata Channel's I/O space

Arguments:

  PeiServices     - The PEI core services table.
  This            - PEI_ATA_CONTROLLER_PPI pointer.
  IdeRegsBaseAddr - Ata Channel's I/O space

Returns:

  The number of command/control pair registers.

--*/
{
  ((*PeiServices)->CopyMem) (IdeRegsBaseAddr, mAtaChannelIoSpace, sizeof (mAtaChannelIoSpace));
  return sizeof (mAtaChannelIoSpace) / sizeof (IDE_REGS_BASE_ADDR);
}

EFI_STATUS
EFIAPI
PeimInitializeRecovery (
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
/*++

Routine Description:

  Provide the functionality of the Recovery Module.

Arguments:

  PeiServices - General purpose services available to every PEIM.

Returns:

  Status -  EFI_SUCCESS if the interface could be successfully
            installed

--*/
{
  EFI_STATUS  Status;

  Status = (*PeiServices)->InstallPpi (PeiServices, &mRecoveryPpiList[0]);

  return Status;
}

EFI_STATUS
EFIAPI
PlatformRecoveryModule (
  IN EFI_PEI_SERVICES                       **PeiServices,
  EFI_PEI_RECOVERY_MODULE_PPI       *This
  )
/*++

Routine Description:

  Provide the functionality of the Ea Recovery Module.

Arguments:

  PeiServices - General purpose services available to every PEIM.

Returns:

  Status -  EFI_SUCCESS if the interface could be successfully
            installed

--*/
{
  EFI_STATUS                             Status;
  EFI_PEI_DEVICE_RECOVERY_MODULE_PPI     *DeviceRecoveryModule;
  UINTN                                  NumberOfImageProviders;
  BOOLEAN                                ProviderAvailable;
  UINTN                                  NumberRecoveryCapsules;
  UINTN                                  RecoveryCapsuleSize;
  EFI_GUID                               DeviceId;
  BOOLEAN                                ImageFound;
  EFI_PHYSICAL_ADDRESS                   Address;
  VOID                                   *Buffer;
  EFI_PEI_HOB_POINTERS                   Hob;
  EFI_PEI_HOB_POINTERS                   HobOld;
  BOOLEAN                                HobUpdate;

  Status = EFI_SUCCESS;
  HobUpdate = FALSE;

  ProviderAvailable = TRUE;
  ImageFound        = FALSE;
  NumberOfImageProviders = 0;

  DeviceRecoveryModule = NULL;

  DEBUG((DEBUG_INFO | DEBUG_LOAD, "Recovery Entry\n"));

  //
  // Search the platform for some recovery capsule if the DXE IPL
  // discovered a recovery condition and has requested a load.
  //
  while (ProviderAvailable) {
    Status = (*PeiServices)->LocatePpi (
                               PeiServices,
                               &gEfiPeiDeviceRecoveryModulePpiGuid,
                               NumberOfImageProviders,
                               NULL,
                               &DeviceRecoveryModule
                               );


    if (!EFI_ERROR (Status)) {
      DEBUG((DEBUG_INFO | DEBUG_LOAD, "Device Recovery PPI located\n"));
      NumberOfImageProviders++;
      Status = DeviceRecoveryModule->GetNumberRecoveryCapsules (
                                                      PeiServices,
                                                      DeviceRecoveryModule,
                                                      &NumberRecoveryCapsules
                                                      );

      DEBUG((DEBUG_INFO | DEBUG_LOAD, "Number Of Recovery Capsules: %d\n", NumberRecoveryCapsules));

      if (NumberRecoveryCapsules != 0) {
        break;
      }
    } else {
      ProviderAvailable = FALSE;
    }
  }

  //
  // If there is an image provider, get the capsule ID
  //
  if (ProviderAvailable) {
    RecoveryCapsuleSize = 0;

    Status = DeviceRecoveryModule->GetRecoveryCapsuleInfo (
                                                    PeiServices,
                                                    DeviceRecoveryModule,
                                                    0,
                                                    &RecoveryCapsuleSize,
                                                    &DeviceId
                                                  );

    if (EFI_ERROR(Status)) {
      return Status;
    }

    DEBUG((DEBUG_INFO | DEBUG_LOAD, "Recovery Capsule Size: %d\n", RecoveryCapsuleSize));

    //
    // Only support the 2 capsule types known
    // Future enhancement is to rank-order the selection
    //
    //
    // Bugbug here
    //
    /*
    if ((!CompareGuid (&DeviceId, &gPeiCapsuleOnFatIdeDiskGuid)) &&
        (!CompareGuid (&DeviceId, &gPeiCapsuleOnFatFloppyDiskGuid)) &&
        (!CompareGuid (&DeviceId, &gPeiCapsuleOnDataCDGuid)) &&
        (!CompareGuid (&DeviceId, &gPeiCapsuleOnFatUsbDiskGuid))
        ) {
      return EFI_UNSUPPORTED;
    }
    */
    Buffer = NULL;
    Status = (*PeiServices)->AllocatePages (
                PeiServices,
                EfiBootServicesCode,
                (RecoveryCapsuleSize - 1) / 0x1000 + 1,
                &Address
                );

    DEBUG((DEBUG_INFO | DEBUG_LOAD, "AllocatePage Returns: %r\n", Status));

    if (EFI_ERROR(Status)) {
      return Status;
    }

    Buffer = (UINT8*)(UINTN)Address;

    Status = DeviceRecoveryModule->LoadRecoveryCapsule (
                                                    PeiServices,
                                                    DeviceRecoveryModule,
                                                    0,
                                                    Buffer
                                                    );

    DEBUG((DEBUG_INFO | DEBUG_LOAD, "LoadRecoveryCapsule Returns: %r\n", Status));

    if (EFI_ERROR (Status)) {
      return Status;
    }

    //
    // Update FV Hob if found
    //
    Status = (*PeiServices)->GetHobList (PeiServices, &Hob.Raw);
    HobOld.Raw = Hob.Raw;
    while (!END_OF_HOB_LIST (Hob)) {
      if (Hob.Header->HobType == EFI_HOB_TYPE_FV) {
        DEBUG((DEBUG_INFO | DEBUG_LOAD, "Hob FV Length: %x\n", Hob.FirmwareVolume->Length));
        if (Hob.FirmwareVolume->Length > 0x50000) {
          HobUpdate = TRUE;
          //
          // This looks like the Hob we are interested in
          //
          DEBUG((DEBUG_INFO | DEBUG_LOAD, "Hob Updated\n"));
          Hob.FirmwareVolume->BaseAddress = (UINTN)Buffer;
          Hob.FirmwareVolume->Length = RecoveryCapsuleSize;
        }
      }
      Hob.Raw = GET_NEXT_HOB (Hob);
    }

    //
    // build FV Hob if it is not built before
    //
    if (!HobUpdate) {
      DEBUG((DEBUG_INFO | DEBUG_LOAD, "FV Hob is not found, Build FV Hob then..\n" ));

      BuildFvHob (
        (UINTN)Buffer,
        RecoveryCapsuleSize
      );

    }
  }
  DEBUG((DEBUG_INFO | DEBUG_LOAD, "Recovery Module Returning: %r\n", Status));
  return Status;
}
