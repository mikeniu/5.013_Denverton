/** @file
  This code provides a initialization of intel VT-d (Virtualization Technology for Directed I/O).

@copyright
  Copyright (c) 1999 - 2015 Intel Corporation. All rights reserved
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

@par Specification Reference:
**/
#include "SaInit.h"
#include "VTd.h"
#include <CpuRegs.h>
#include <Library/HobLib.h>
#include <SaDataHob.h>

SA_DATA_HOB *mSaDataHob;
PCH_SERIES                      mPchSeries;

/**
For device that specified by Device Num and Function Num,
mDevEnMap is used to check device presence.
  0x80 means use Device ID to detemine presence

 The structure is used to check if device scope is valid when update DMAR table
**/
UINT16  mDevEnMap[][2] = {
  {
    0x1500,
    0x80
  }   ///< D21F0
};

BOOLEAN mInterruptRemappingSupport;

/**
    Get the corresponding device Enable/Disable bit according DevNum and FunNum

    @param[in] DevNum  - Device Number
    @param[in] FunNum  - Function Number

    @retval If the device is found, return disable/Enable bit in FD/Deven reigster
    @retval If not found return 0xFF
**/
UINT16
GetFunDisableBit (
  UINT8 DevNum,
  UINT8 FunNum
  )
{
  UINTN Index;

  for (Index = 0; Index < sizeof (mDevEnMap) / 4; Index++) {
    if (mDevEnMap[Index][0] == ((DevNum << 0x08) | FunNum)) {
      return mDevEnMap[Index][1];
    }
  }

  return 0xFF;
}

UINT8
GetIoApicID (
  VOID
  )
/**
    Get IOAPIC ID from LPC

    @retval APIC ID
**/
{
  UINT8 volatile  *IoapicIndex;
  UINT32 volatile *IoapicData;
  UINT32          IoApicIndexAddr;
  UINT32          IoApicDataAddr;
  UINT32          Data32;

  ///
  /// Get IOAPIC base
  ///
  PchIoApicBaseGet (&IoApicIndexAddr, &IoApicDataAddr);
  IoapicIndex = (UINT8 *) (UINTN) IoApicIndexAddr;
  IoapicData  = (UINT32 *) (UINTN) IoApicDataAddr;

  ///
  /// Get APIC ID from Identification Register (Index 0)
  ///
  *IoapicIndex  = 0;
  Data32        = (*IoapicData & 0x0F000000) >> 24;

  return (UINT8) Data32;
}

VOID
UpdateDRHD (
  IN OUT VOID *DrhdEnginePtr
  )
/**
    Update the second DRHD structure

    @param[in, out] DrhdEnginePtr       - A pointer to DRHD structure
**/
{
  UINT16                        Length;
  UINTN                         DeviceScopeNum;
  UINTN                         ValidDeviceScopeNum;
  UINT16                        Data16;
  UINT16                        Index;
  UINT8                         Bus;
  UINT8                         Path[2] = { 0, 0 };
  BOOLEAN                       NeedRemove;
  EFI_ACPI_DRHD_ENGINE2_STRUCT  *DrhdEngine;

  ///
  /// Convert DrhdEnginePtr to EFI_ACPI_DRHD_ENGINE2_STRUCT Pointer
  ///
  DrhdEngine      = (EFI_ACPI_DRHD_ENGINE2_STRUCT *) DrhdEnginePtr;

  Length          = DrhdEngine->Length;
  DeviceScopeNum  = (DrhdEngine->Length - EFI_ACPI_DRHD_ENGINE_HEADER_LENGTH) / sizeof (EFI_ACPI_DEV_SCOPE_STRUCTURE);
  Data16          = 0;
  Bus             = 0;
  ValidDeviceScopeNum = 0;

  for (Index = 0; Index < DeviceScopeNum; Index++) {
    NeedRemove = FALSE;
    /**
      For HPET and APIC, update device scope if Interrupt remapping is supported. remove device scope
      if interrupt remapping is not supported.
      - Index = 0 - IOAPIC
      - Index = 1 - HPET
    **/
    if (mInterruptRemappingSupport) {
      if (Index == 0) {
        ///
        /// Update source id for IoApic's device scope entry
        ///
        PchP2sbCfgGet16 (R_PCH_P2SB_IBDF, &Data16);
        Bus     = (UINT8) (Data16 >> 8);
        if (Bus != 0x00) {
          Path[0] = (UINT8) ((Data16 & 0xff) >> 3);
          Path[1] = (UINT8) (Data16 & 0x7);
        } else {
          //
          // BUGBUG: Here we just hardcode, because in this version, R_PCH_LPC_IOXAPIC is initialized AFTER Vtd run.
          // We can NOT get proper setting from PCH
          /// @todo check if code still needed
          //
          DEBUG ((DEBUG_WARN, "BUGBUG: UpdateApicHpet use hardcode value - To be fixed!\n"));
          Bus     = 0xF0;
          Path[0] = 0x1F;
          Path[1] = 0x0;
        }
        DrhdEngine->DeviceScope[Index].StartBusNumber = Bus;
        //
        // Update APIC ID
        //
        DrhdEngine->DeviceScope[Index].EnumId = GetIoApicID ();
      }
      if (Index == 1) {
        ///
        /// Update source id for HPET's device scope entry
        ///
        PchP2sbCfgGet16 (R_PCH_P2SB_HBDF, &Data16);
        Bus     = (UINT8) (Data16 >> 8);
        Path[0] = (UINT8) ((Data16 & 0xFF) >> 3);
        Path[1] = (UINT8) (Data16 & 0x7);
        DrhdEngine->DeviceScope[Index].StartBusNumber = Bus;
      }
    } else {
      if ((Index == 0) || (Index == 1)) {
        NeedRemove = TRUE;
      }
    }

    if ((Index >= 2) && (Index <= 8)){
      NeedRemove = TRUE;
    }

    CopyMem (
      &DrhdEngine->DeviceScope[ValidDeviceScopeNum],
      &DrhdEngine->DeviceScope[Index],
      sizeof (EFI_ACPI_DEV_SCOPE_STRUCTURE)
      );
    if (NeedRemove) {
      Length -= sizeof (EFI_ACPI_DEV_SCOPE_STRUCTURE);
    } else {
      ValidDeviceScopeNum++;
    }
  }
  ///
  /// If no devicescope is left, we set the structure length as 0x00
  ///
  if ((Length > EFI_ACPI_DRHD_ENGINE_HEADER_LENGTH) || (DrhdEngine->Flags == 0x01)) {
    DrhdEngine->Length = Length;
  } else {
    DrhdEngine->Length = 0;
  }
}

/**
    Update the RMRR structure

    @param[in, out] RmrrPtr             - A pointer to RMRR structure
**/
VOID
UpdateRMRR (
  IN OUT VOID *RmrrPtr
  )
{
  UINT16                  Length;
  UINT16                  DisableBit;
  UINTN                   DeviceScopeNum;
  UINTN                   ValidDeviceScopeNum;
  UINTN                   Index;
  BOOLEAN                 NeedRemove;
  EFI_ACPI_RMRR_USB_STRUC *Rmrr;

  ///
  /// To make sure all devicescope can be checked,
  /// we convert the RmrrPtr to EFI_ACPI_RMRR_USB_STRUC pointer
  ///
  Rmrr                = (EFI_ACPI_RMRR_USB_STRUC *) RmrrPtr;

  Length              = Rmrr->Length;
  ValidDeviceScopeNum = 0;
  DeviceScopeNum      = (Rmrr->Length - EFI_ACPI_RMRR_HEADER_LENGTH) / sizeof (EFI_ACPI_DEV_SCOPE_STRUCTURE);
  for (Index = 0; Index < DeviceScopeNum; Index++) {
    DisableBit = GetFunDisableBit (
                  Rmrr->DeviceScope[Index].PciPath[0],
                  Rmrr->DeviceScope[Index].PciPath[1]
                  );
    NeedRemove = FALSE;
    if ((DisableBit == 0xFF) ||
        ((DisableBit == 0x80) &&
        (MmioRead32 (MmPciBase (0, Rmrr->DeviceScope[Index].PciPath[0], Rmrr->DeviceScope[Index].PciPath[1]) + 0x00) == 0xFFFFFFFF))
        ){
      NeedRemove = TRUE;
    }
    CopyMem (
      &Rmrr->DeviceScope[ValidDeviceScopeNum],
      &Rmrr->DeviceScope[Index],
      sizeof (EFI_ACPI_DEV_SCOPE_STRUCTURE)
      );
    if (NeedRemove) {
      Length -= sizeof (EFI_ACPI_DEV_SCOPE_STRUCTURE);
    } else {
      ValidDeviceScopeNum++;
    }
  }
  ///
  /// If No deviceScope is left, set length as 0x00
  ///
  if (Length > EFI_ACPI_RMRR_HEADER_LENGTH) {
    Rmrr->Length = Length;
  } else {
    Rmrr->Length = 0;
  }
}

/**
  Update the DMAR table

  @param[in, out] TableHeader         - The table to be set
  @param[in, out] Version             - Version to publish
**/
VOID
DmarTableUpdate (
  IN OUT   EFI_ACPI_DESCRIPTION_HEADER       *TableHeader,
  IN OUT   EFI_ACPI_TABLE_VERSION            *Version
  )

{
  EFI_ACPI_DMAR_TABLE *DmarTable;
  EFI_ACPI_DMAR_TABLE TempDmarTable;
  UINTN               Offset;
  UINTN               StructureLen;
  UINTN               McD0BaseAddress;
  UINT64               MchBar;
  UINT32              IgdMemSize;
  UINT32              GttMemSize;

  IgdMemSize  = 0;
  GttMemSize  = 0;
  DmarTable   = (EFI_ACPI_DMAR_TABLE *) TableHeader;

  ///
  /// Set INTR_REMAP bit (BIT 0) if interrupt remapping is supported
  ///
  if (mInterruptRemappingSupport) {
    DmarTable->Flags |= BIT0;
  }

  McD0BaseAddress = MmPciBase (SA_MC_BUS, 0, 0);
  MchBar          = MmioRead32 (McD0BaseAddress + R_SA_MCHBAR_LO) &~BIT0;
  MchBar          += (UINT64)LShiftU64(MmioRead32 (McD0BaseAddress + R_SA_MCHBAR_HI), 32);

  DmarTable->RmrrUsb.RmrBaseAddress   = mSaDataHob->VtdData.RmrrUsbBaseAddress;
  DmarTable->RmrrUsb.RmrLimitAddress  = mSaDataHob->VtdData.RmrrUsbLimitAddress;

  ///
  /// Convert to 4KB alignment.
  ///
  DmarTable->RmrrUsb.RmrBaseAddress &= (EFI_PHYSICAL_ADDRESS) ~VTD_MASK_RMR;
  DmarTable->RmrrUsb.RmrLimitAddress &= (EFI_PHYSICAL_ADDRESS) ~VTD_MASK_RMR;
  DmarTable->RmrrUsb.RmrLimitAddress += 0x1000 - 1;

  DEBUG ((DEBUG_INFO, "RMRR Base  address USB %016lX\n", DmarTable->RmrrUsb.RmrBaseAddress));
  DEBUG ((DEBUG_INFO, "RMRR Limit address USB %016lX\n", DmarTable->RmrrUsb.RmrLimitAddress));

  if (DmarTable->RmrrUsb.RmrBaseAddress == 0) {
    DEBUG ((DEBUG_WARN, "WARNING:  RmrrUsb.RmrBaseAddress is 0.\n"));
  }
  ///
  /// Update DRHD structures of DmarTable
  ///
  ///
  /// DNV: Skip DRHD for IGD for Denverton
  ///

  DmarTable->DrhdEngine.RegisterBaseAddress = (MmioRead32 ((UINTN)(MchBar + R_SA_MCHBAR_DEFVTDBAR_OFFSET)) & ~B_SA_MCHBAR_DEFVTDBAR_DEFVTDBAREN);

  DEBUG ((DEBUG_INFO, "VTD base address %x\n", DmarTable->DrhdEngine.RegisterBaseAddress));

  ///
  /// copy DmarTable to TempDmarTable to be processed
  ///
  CopyMem (&TempDmarTable, DmarTable, sizeof (EFI_ACPI_DMAR_TABLE));

  ///
  /// Update DRHD structures of temp DMAR table
  ///
  UpdateDRHD (&TempDmarTable.DrhdEngine);

  ///
  /// Update RMRR structures of temp DMAR table
  ///
  UpdateRMRR ((VOID *) &TempDmarTable.RmrrUsb);

  ///
  /// Remove unused device scope or entire DRHD structures
  ///
  Offset = (UINTN) (&TempDmarTable.DrhdEngine);
  if (TempDmarTable.DrhdEngine.Length != 0) {
    StructureLen = TempDmarTable.DrhdEngine.Length;
    Offset += StructureLen;
  }
  ///
  /// Remove unused device scope or entire RMRR structures
  ///
  if (TempDmarTable.RmrrUsb.Length != 0) {
    StructureLen = TempDmarTable.RmrrUsb.Length;
    CopyMem ((VOID *) Offset, (VOID *) &TempDmarTable.RmrrUsb, TempDmarTable.RmrrUsb.Length);
    Offset += StructureLen;
  }

  Offset = Offset - (UINTN) &TempDmarTable;
  ///
  /// Re-caculate DMAR table check sum
  ///
  TempDmarTable.Header.Checksum = (UINT8) (TempDmarTable.Header.Checksum + TempDmarTable.Header.Length - Offset);
  ///
  /// Set DMAR table length
  ///
  TempDmarTable.Header.Length = (UINT32) Offset;
  ///
  /// Replace DMAR table with rebuilt table TempDmarTable
  ///
  CopyMem ((VOID *) DmarTable, (VOID *) &TempDmarTable, TempDmarTable.Header.Length);
}

VOID
UpdateDmarEndOfDxe (

  IN EFI_EVENT    Event,
  IN VOID         *Context
  )
/**
  EndOfDxe routine for update DMAR
**/
{
  EFI_STATUS                      Status;
  EFI_HANDLE                      *HandleBuffer;
  UINTN                           NumberOfHandles;
  EFI_FV_FILETYPE                 FileType;
  UINT32                          FvStatus;
  EFI_FV_FILE_ATTRIBUTES          Attributes;
  UINTN                           Size;
  UINTN                           i;
  INTN                            Instance;
  EFI_ACPI_TABLE_VERSION          Version;
  EFI_ACPI_COMMON_HEADER          *CurrentTable;
  UINTN                           AcpiTableHandle;
  EFI_FIRMWARE_VOLUME2_PROTOCOL   *FwVol;
  EFI_ACPI_TABLE_PROTOCOL         *AcpiTable;
  EFI_ACPI_DESCRIPTION_HEADER     *VtdAcpiTable;
  STATIC BOOLEAN                  Triggered = FALSE;

  if (Triggered) {
    return;
  }

  Triggered     = TRUE;

  FwVol         = NULL;
  AcpiTable     = NULL;
  VtdAcpiTable  = NULL;

  mPchSeries  = GetPchSeries();

  ///
  ///
  if ((mSaDataHob->VtdData.VtdEnable == FALSE) || (MmioRead32 (MmPciBase (SA_MC_BUS, 0, 0) + R_SA_MC_CAPID0_A) & BIT23)) {
      DEBUG ((DEBUG_INFO, "Vtd Disabled, skip DMAR Table install\n"));

      return;
  }

  ///
  /// Locate ACPI support protocol
  ///
  Status = gBS->LocateProtocol (&gEfiAcpiTableProtocolGuid, NULL, (VOID **) &AcpiTable);

  ///
  /// Locate protocol.
  /// There is little chance we can't find an FV protocol
  ///
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiFirmwareVolume2ProtocolGuid,
                  NULL,
                  &NumberOfHandles,
                  &HandleBuffer
                  );
  ASSERT_EFI_ERROR (Status);

  ///
  /// Looking for FV with ACPI storage file
  ///
  for (i = 0; i < NumberOfHandles; i++) {
    ///
    /// Get the protocol on this handle
    /// This should not fail because of LocateHandleBuffer
    ///
    Status = gBS->HandleProtocol (
                    HandleBuffer[i],
                    &gEfiFirmwareVolume2ProtocolGuid,
                    (VOID **) &FwVol
                    );
    ASSERT_EFI_ERROR (Status);

    ///
    /// See if it has the ACPI storage file
    ///
    Size      = 0;
    FvStatus  = 0;
    Status = FwVol->ReadFile (
                      FwVol,
                      &gSaAcpiTableStorageGuid,
                      NULL,
                      &Size,
                      &FileType,
                      &Attributes,
                      &FvStatus
                      );

    ///
    /// If we found it, then we are done
    ///
    if (Status == EFI_SUCCESS) {
      break;
    }
  }
  ///
  /// Our exit status is determined by the success of the previous operations
  /// If the protocol was found, Instance already points to it.
  ///
  ///
  /// Free any allocated buffers
  ///
  FreePool (HandleBuffer);

  ///
  /// Sanity check that we found our data file
  ///
  ASSERT (FwVol);
  if (FwVol == NULL) {
    return ;
  }
  ///
  /// By default, a table belongs in all ACPI table versions published.
  ///
  Version = EFI_ACPI_TABLE_VERSION_1_0B | EFI_ACPI_TABLE_VERSION_2_0 | EFI_ACPI_TABLE_VERSION_3_0;

  ///
  /// Read tables from the storage file.
  ///
  Instance      = 0;
  CurrentTable  = NULL;

  while (Status == EFI_SUCCESS) {
    Status = FwVol->ReadSection (
                      FwVol,
                      &gSaAcpiTableStorageGuid,
                      EFI_SECTION_RAW,
                      Instance,
                      (VOID **) &CurrentTable,
                      &Size,
                      &FvStatus
                      );

    if (!EFI_ERROR (Status)) {
      ///
      /// Check the Signature ID to modify the table
      ///
      switch (((EFI_ACPI_DESCRIPTION_HEADER *) CurrentTable)->Signature) {

      case EFI_ACPI_VTD_DMAR_TABLE_SIGNATURE:
        VtdAcpiTable = (EFI_ACPI_DESCRIPTION_HEADER *) CurrentTable;
        DmarTableUpdate (VtdAcpiTable, &Version);
        break;

      default:
        break;
      }
      ///
      /// Increment the instance
      ///
      Instance++;
      CurrentTable = NULL;
    }
  }
  ///
  /// Update the VTD table in the ACPI tables.
  ///
  AcpiTableHandle = 0;
  if (VtdAcpiTable != NULL) {
    Status = AcpiTable->InstallAcpiTable (
                          AcpiTable,
                          VtdAcpiTable,
                          VtdAcpiTable->Length,
                          &AcpiTableHandle
                          );
  }
}

EFI_STATUS
VtdInit (
  IN  SA_DATA_HOB    *SaDataHob
  )
/**
  Locate the VT-d ACPI tables data file and read ACPI SSDT tables.
  Publish the appropriate SSDT based on current configuration and capabilities.

  @param[in] SA Data Hob  -  SA Data Hob Structure

  @retval EFI_SUCCESS     - Vtd initialization complete
  @exception EFI_UNSUPPORTED - Vtd is not enabled by policy
**/
{
  UINTN        McD0BaseAddress;
  UINT64       MchBar;
  UINT32       VtdBase;
  UINT32       VtBarReg;
  EFI_EVENT    EndOfDxeEvent;
  EFI_STATUS   Status;

  mInterruptRemappingSupport  = FALSE;
  McD0BaseAddress             = MmPciBase (SA_MC_BUS, 0, 0);
  MchBar                      = MmioRead32 (McD0BaseAddress + R_SA_MCHBAR_LO) &~BIT0;
  MchBar                      += (UINTN)LShiftU64(MmioRead32 (McD0BaseAddress + R_SA_MCHBAR_HI), 32);
  VtBarReg                    = R_SA_MCHBAR_DEFVTDBAR_OFFSET;
  VtdBase                     = MmioRead32 ((UINTN)(MchBar + VtBarReg)) & ~BIT0;
  mSaDataHob                  = SaDataHob;

  if ((!mSaDataHob->VtdData.VtdEnable) || (MmioRead32 (McD0BaseAddress + R_SA_MC_CAPID0_A) & BIT23)) {
    DEBUG ((DEBUG_WARN, "VTd disabled or no capability!\n"));
    return EFI_UNSUPPORTED;
  }
  ///
  /// Check SA supports VTD and VTD is enabled in setup menu
  ///
  DEBUG ((DEBUG_INFO, "VTd enabled\n"));

  ///
  /// Skip if the VT bar is 0
  ///
  if ((VtdBase != 0) && (VtdBase != ~BIT0)) {
    ///
	/// Check IR status
	///
	if (MmioRead32 (VtdBase + VTD_ECAP_REG) & IR){

      if (mSaDataHob->VtdData.InterruptRemappingSupport){
  	    mInterruptRemappingSupport = TRUE;
  	    DEBUG ((DEBUG_WARN, "Interrupt Remapping enabled %x!\n", mInterruptRemappingSupport ));
      }

	}
  }

  ///
  /// Register Event for updating DMAR table
  ///

  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  UpdateDmarEndOfDxe,
                  NULL,
                  &gEfiEndOfDxeEventGroupGuid,
                  &EndOfDxeEvent
                  );
  return EFI_SUCCESS;
}
