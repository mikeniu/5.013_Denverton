/** @file
  PCH Init Smm module for PCH specific SMI handlers.

@copyright
 Copyright (c) 2013 - 2017 Intel Corporation. All rights reserved
 This software and associated documentation (if any) is furnished
 under a license and may only be used or copied in accordance
 with the terms of the license. Except as permitted by the
 license, no part of this software or documentation may be
 reproduced, stored in a retrieval system, or transmitted in any
 form or by any means without the express written consent of
 Intel Corporation.
 This file contains an 'Intel Peripheral Driver' and is uniquely
 identified as "Intel Reference Module" and is licensed for Intel
 CPUs and chipsets under the terms of your license agreement with
 Intel or your vendor. This file may be modified by the user, subject
 to additional terms of the license agreement.
**/
#include "PchInitSmm.h"

GLOBAL_REMOVE_IF_UNREFERENCED EFI_SMM_IO_TRAP_DISPATCH2_PROTOCOL        *mPchIoTrap;
GLOBAL_REMOVE_IF_UNREFERENCED EFI_SMM_SX_DISPATCH2_PROTOCOL             *mSxDispatch;

GLOBAL_REMOVE_IF_UNREFERENCED PCH_NVS_AREA                              *mPchNvsArea;
GLOBAL_REMOVE_IF_UNREFERENCED UINT16                                    mAcpiBaseAddr;

GLOBAL_REMOVE_IF_UNREFERENCED PCH_POLICY_HOB                            *mPchPolicyHob;
GLOBAL_REMOVE_IF_UNREFERENCED SI_POLICY_HOB                             *mSiPolicyHob;

//
// The reserved MMIO range to be used in Sx handler
//
GLOBAL_REMOVE_IF_UNREFERENCED EFI_PHYSICAL_ADDRESS                      mResvMmioBaseAddr;
GLOBAL_REMOVE_IF_UNREFERENCED UINTN                                     mResvMmioSize;
//
// The reserved MMIO range to be used by xHCI ASL code
//
GLOBAL_REMOVE_IF_UNREFERENCED EFI_PHYSICAL_ADDRESS                      mXhciMmioBaseAddr;

/**
  PCH Sx entry SMI handler.

  @param[in]     Handle          Handle of the callback
  @param[in]     Context         The dispatch context
  @param[in,out] CommBuffer      A pointer to a collection of data in memory that will
                                 be conveyed from a non-SMM environment into an SMM environment.
  @param[in,out] CommBufferSize  The size of the CommBuffer.

  @retval EFI_SUCCESS
**/
EFI_STATUS
EFIAPI
PchSxHandler (
  IN  EFI_HANDLE                   Handle,
  IN CONST VOID                    *Context OPTIONAL,
  IN OUT VOID                      *CommBuffer OPTIONAL,
  IN OUT UINTN                     *CommBufferSize OPTIONAL
  )
{
  DEBUG ((EFI_D_INFO, __FUNCTION__" : Entry\n"));
  PchLanSxCallback ();
  DEBUG ((EFI_D_INFO, __FUNCTION__" : Exit\n"));

  return EFI_SUCCESS;
}

/**
  Initialize PCH Sx entry SMI handler.

  @param[in] ImageHandle - Handle for the image of this driver
**/
VOID
InitializeSxHandler (
  IN EFI_HANDLE        ImageHandle
  )
{
  EFI_SMM_SX_REGISTER_CONTEXT               SxDispatchContext;
  EFI_HANDLE                                SxDispatchHandle;
  EFI_SLEEP_TYPE                            SxType;
  EFI_STATUS                                Status;

  DEBUG ((EFI_D_INFO, __FUNCTION__" : Entry\n"));

  //
  // Register the callback for S3/S4/S5 entry
  //
  SxDispatchContext.Phase = SxEntry;
  for (SxType = SxS3; SxType <= SxS5; SxType++) {
    SxDispatchContext.Type  = SxType;
    Status = mSxDispatch->Register (
                            mSxDispatch,
                            PchSxHandler,
                            &SxDispatchContext,
                            &SxDispatchHandle
                            );
    ASSERT_EFI_ERROR (Status);
  }

  DEBUG ((EFI_D_INFO, __FUNCTION__" : Exit\n"));
}

/**
  Allocates reserved MMIO for Sx SMI handler use.

  This function is only called from entry point therefore DXE/boot Services can be used here.
  Updates ACPI NVS location to reserve allocated MMIO range as system resource.
**/
VOID
AllocateReservedMmio (
  IN EFI_HANDLE        ImageHandle
  )
{
  PCH_NVS_AREA_PROTOCOL  *PchNvsAreaProtocol;
  EFI_STATUS             Status;
  UINTN Alignment;

  mResvMmioSize = EFI_PAGE_SIZE;
  Alignment = EFI_PAGE_SHIFT;

  if ( (PcdGet8(PcdEfiGcdAllocateType) == EfiGcdAllocateMaxAddressSearchBottomUp) || (PcdGet8(PcdEfiGcdAllocateType) == EfiGcdAllocateMaxAddressSearchTopDown) ){
    mResvMmioBaseAddr = 0xFFFFFFFF;
  }
  Status = gDS->AllocateMemorySpace (
                  PcdGet8(PcdEfiGcdAllocateType),
                  EfiGcdMemoryTypeMemoryMappedIo,
                  Alignment,
                  mResvMmioSize,
                  &mResvMmioBaseAddr,
                  ImageHandle,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  DEBUG ((DEBUG_INFO, "mResvMmioBaseAddr %x\n", mResvMmioBaseAddr));

  //
  // Locate the PCH shared data area and update reserved memory base address
  //
  Status = gBS->LocateProtocol (&gPchNvsAreaProtocolGuid, NULL, (VOID **) &PchNvsAreaProtocol);
  ASSERT_EFI_ERROR (Status);

  PchNvsAreaProtocol->Area->SxMemSize = (UINT32) mResvMmioSize;
  PchNvsAreaProtocol->Area->SxMemBase = (UINT32) mResvMmioBaseAddr;


  //
  // Install and initialize all the needed protocols
  //
  if ( (PcdGet8(PcdEfiGcdAllocateType) == EfiGcdAllocateMaxAddressSearchBottomUp) || (PcdGet8(PcdEfiGcdAllocateType) == EfiGcdAllocateMaxAddressSearchTopDown) ){
    mXhciMmioBaseAddr = 0xFFFFFFFF;
  }
  Status = gDS->AllocateMemorySpace (
                  PcdGet8(PcdEfiGcdAllocateType),
                  EfiGcdMemoryTypeMemoryMappedIo,
                  N_PCH_XHCI_MEM_ALIGN,
                  V_PCH_XHCI_MEM_LENGTH,
                  &mXhciMmioBaseAddr,
                  ImageHandle,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);
  DEBUG ((DEBUG_INFO, "mXhciMmioBaseAddr %x\n", mXhciMmioBaseAddr));

  //
  // Update XWMB, XHCI memory base address
  //
  PchNvsAreaProtocol->Area->XhciRsvdMemBase = (UINT32) mXhciMmioBaseAddr;

}

/**
  Initializes the PCH SMM handler for for PCIE hot plug support
  <b>PchInit SMM Module Entry Point</b>\n
  - <b>Introduction</b>\n
      The PchInitSmm module is a SMM driver that initializes the Intel Platform Controller Hub
      SMM requirements and services. It consumes the PCH_POLICY_HOB and SI_POLICY_HOB for expected
      configurations per policy.

  - <b>Details</b>\n
    This module provides SMI handlers to services PCIE HotPlug SMI, LinkActive SMI, and LinkEq SMI.
    And also provides port 0x61 emulation support, registers BIOS WP handler to process BIOSWP status,
    and registers SPI Async SMI handler to handler SPI Async SMI.
    This module also registers Sx SMI callback function to detail with GPIO Sx Isolation and LAN requirement.

  - @pre
    - PCH PCR base address configured
    - EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL
      - This is to ensure that PCI MMIO and IO resource has been prepared and available for this driver to allocate.
    - EFI_SMM_BASE2_PROTOCOL
    - EFI_SMM_IO_TRAP_DISPATCH2_PROTOCOL
    - EFI_SMM_SX_DISPATCH2_PROTOCOL
    - EFI_SMM_CPU_PROTOCOL
    - @link _PCH_SMM_IO_TRAP_CONTROL_PROTOCOL PCH_SMM_IO_TRAP_CONTROL_PROTOCOL @endlink
    - @link _PCH_SMI_DISPATCH_PROTOCOL PCH_SMI_DISPATCH_PROTOCOL @endlink
    - @link _PCH_PCIE_SMI_DISPATCH_PROTOCOL PCH_PCIE_SMI_DISPATCH_PROTOCOL @endlink
    - @link _PCH_TCO_SMI_DISPATCH_PROTOCOL PCH_TCO_SMI_DISPATCH_PROTOCOL @endlink
    - @link _PCH_ESPI_SMI_DISPATCH_PROTOCOL PCH_ESPI_SMI_DISPATCH_PROTOCOL @endlink

  - <b>References</b>\n
    - @link _PCH_POLICY PCH_POLICY_HOB @endlink.
    - @link _SI_POLICY_STRUCT SI_POLICY_HOB @endlink.

  - <b>Integration Checklists</b>\n
    - Verify prerequisites are met. Porting Recommendations.
    - No modification of this module should be necessary
    - Any modification of this module should follow the PCH BIOS Specification and EDS

  @param[in] ImageHandle - Handle for the image of this driver
  @param[in] SystemTable - Pointer to the EFI System Table

  @retval EFI_SUCCESS    - PCH SMM handler was installed
**/
EFI_STATUS
EFIAPI
PchInitSmmEntryPoint (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                                Status;
  PCH_NVS_AREA_PROTOCOL                     *PchNvsAreaProtocol;
  EFI_PEI_HOB_POINTERS                      HobPtr;

  DEBUG ((DEBUG_INFO, "PchInitSmmEntryPoint()\n"));
  Status = gSmst->SmmLocateProtocol (
                    &gEfiSmmSxDispatch2ProtocolGuid,
                    NULL,
                    (VOID**) &mSxDispatch
                    );
  ASSERT_EFI_ERROR (Status);

  Status = gBS->LocateProtocol (&gPchNvsAreaProtocolGuid, NULL, (VOID **) &PchNvsAreaProtocol);
  ASSERT_EFI_ERROR (Status);
  mPchNvsArea = PchNvsAreaProtocol->Area;

  HobPtr.Guid   = GetFirstGuidHob (&gPchPolicyHobGuid);
  ASSERT (HobPtr.Guid != NULL);
  mPchPolicyHob = GET_GUID_HOB_DATA (HobPtr.Guid);

  HobPtr.Guid   = GetFirstGuidHob (&gSiPolicyHobGuid);
  ASSERT (HobPtr.Guid != NULL);
  mSiPolicyHob  = GET_GUID_HOB_DATA (HobPtr.Guid);

  PchAcpiBaseGet (&mAcpiBaseAddr);

  AllocateReservedMmio (ImageHandle);

  InitializeSxHandler (ImageHandle);

  Status = InstallPchBiosWriteProtect (ImageHandle, SystemTable);
  ASSERT_EFI_ERROR (Status);

  Status = InstallPchSpiAsyncSmiHandler ();
  ASSERT_EFI_ERROR (Status);


  return EFI_SUCCESS;
}
