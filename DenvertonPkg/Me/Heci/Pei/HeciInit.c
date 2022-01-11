/**
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
 **/
/**

Copyright (c) 2008-2017 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

@file:

  HeciInit.c

@brief:

  Framework PEIM to HECI.

**/
#include <PiPei.h>
#include <Uefi.h>
#include <Ppi/HeciPpi.h>
#include <Ppi/EndOfPeiPhase.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <library/PcdLib.h>
#include "Library/MeTypeLib.h"
#include <Library/PeiServicesLib.h>
#include <Library/TimerLib.h>
#if AMT_SUPPORT
#include <Library/HobLib.h>
#include "Library/AmtLibPei.h"
#endif // AMT_SUPPORT
#include <Register/PchRegsPmc.h>
#include <Library/PchCycleDecodingLib.h>
#include <Library/HeciCoreLib.h>
#include "MeAccess.h"
#include "HeciRegs.h"
#include "MeState.h"
#include "HeciInit.h"

//
// Function Declarations
//
static HECI_PPI mHeciPpi = {
  HeciSendwAck,
  HeciReceive,
  HeciSend,
  HeciInitialize,
  HeciGetMeStatus,
  HeciGetMeMode
};

static EFI_PEI_PPI_DESCRIPTOR mInstallHeciPpi = {
  EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
  &gHeciPpiGuid,
  &mHeciPpi
};

static PEI_HECI2_PPI mHeci2Ppi = {
  Heci2Initialize,
  MicroSecondDelay,
  Heci2GetMeStatus
};

static EFI_PEI_PPI_DESCRIPTOR  mInstallHeci2Ppi = {
  EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
  &gPeiHeci2PpiGuid,
  &mHeci2Ppi
};


///////////////////////////////////////////////////////////////////////////////
// Function Implementations
//

/*****************************************************************************
 @brief
  Internal function performing Heci platform PPIs init needed in PEI phase

 @retval EFI_SUCCESS             The function completed successfully.
 @retval EFI_DEVICE_ERROR        ME FPT is bad
**/
EFI_STATUS
EFIAPI
PeiInstallHeciPpi (
  VOID
  )
{
  EFI_STATUS  Status;

  //
  // Initialize Heci platform PPIs
  // Do not use EdkIIGlueLib here because PeiService pointer used in GlueLib
  // is not updated after memory installed.
  //
  Status = PeiServicesInstallPpi (&mInstallHeciPpi);
  ASSERT_EFI_ERROR (Status);

  Status = PeiServicesInstallPpi (&mInstallHeci2Ppi);
  ASSERT_EFI_ERROR (Status);

  return Status;
}


/*****************************************************************************
 @brief
  Internal function performing PM register initialization for Me

 @retval EFI_SUCCESS             Always return EFI_SUCCESS
**/
EFI_STATUS
EFIAPI
MePmInit (
  VOID
  )
{
  UINT16  PmBase;
  UINT16  Pm1Cnt;

  //
  // Before system memory initialization, BIOS should check the WAK_STS bit in PM1_STS[15] (PCH register PMBASE+00h)
  // to determine if Intel Management Engine has reset the system while the host was in a sleep state. If WAK_STS is
  // not set, BIOS should ensure a non-sleep exit path is taken. One way to accomplish this is by overwriting
  // PM1_CNT[12:10] (PCH register PMBASE+04h) to 111b to force an S5 exit path by the BIOS.
  //
  PchAcpiBaseGet(&PmBase);

  ASSERT (PmBase != 0);

  if (PmBase != 0) {
    if ((IoRead16 (PmBase + R_PCH_ACPI_PM1_STS) & B_PCH_ACPI_PM1_STS_WAK) == 0) {
      Pm1Cnt = IoRead16 (PmBase + R_PCH_ACPI_PM1_CNT) | V_PCH_ACPI_PM1_CNT_S5;
      IoWrite16 (PmBase + R_PCH_ACPI_PM1_CNT, Pm1Cnt);
      DEBUG ((EFI_D_ERROR, "Force an S5 exit path.\n"));
    }
  }

  return EFI_SUCCESS;
}


/*****************************************************************************
 @brief
  Internal function performing Me initialization.

  @retval EFI_SUCCESS             Heci initialization completed successfully.
  @retval All other error conditions encountered result in an ASSERT.
**/
EFI_STATUS
EFIAPI
MeInit (
  VOID
  )
{
  EFI_STATUS  Status;

  Status = MePmInit ();
  return Status;
}


/*****************************************************************************
 @bref
  The entry point of the HECI PEIM. Installs HECI PPI interface.

  @param  FileHandle  Handle of the file being invoked.
  @param  PeiServices Describes the list of possible PEI Services.

  @retval EFI_SUCCESS   Indicates that HECI initialization completed successfully.
**/
EFI_STATUS
HeciPeimEntryPoint (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS        Status;
  HECI_FWS_REGISTER MeFirmwareStatus;

  //
  // Performing Me initialization
  //
  Status = MeInit ();
  ASSERT_EFI_ERROR (Status);

  //
  // Install HECI PPI
  //
  MeFirmwareStatus.ul = HeciPciRead32 (R_FWSTATE);
  if ((MeFirmwareStatus.ul != 0xFFFFFFFF) &&
      (MeFirmwareStatus.r.CurrentState > ME_STATE_INIT)) {
    Status = PeiInstallHeciPpi();
    ASSERT_EFI_ERROR (Status);
  } else {
    DEBUG ((EFI_D_ERROR, "HECI PPI will not be installed\n"));
  }

  return Status;
} // PeimHeciInit()

