/*++
  This file contains an 'Intel Peripheral Driver' and is        
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/
/*++

Copyright (c) 2009-2016, Intel Corporation.  All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license.  Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

Module Name:
    WheaPlatformHooks.c
   
Abstract:
    Whea Platform hook functions and platform data, which needs to be 
    ported.

--*/


#include "Library\WheaPlatformHooksLib.h"
#include <PchAccess.h>
#include <Library\PciLib.h>
#include <Include\Library\IoLib.h>
#include <Library\DebugLib.h>
#include <Library\PchCycleDecodingLib.h>

//
// Data definitions & structures
//

#define EFI_ACPI_CREATOR_ID                 0x4C544E49  // "INTL"
#define EFI_ACPI_CREATOR_REVISION           0x00000001
#define EFI_ACPI_OEM_TABLE_ID               SIGNATURE_64 ('D', 'N', 'V', ' ') // OEM table id 8 bytes long
#define EFI_WHEA_OEM_REVISION               0x0001;
#define BOARD_ID                            0x444E56    // DNV
#define NMI_STS_CNT                         0x61
                                            
//////////////////////////////////////////////////////////////////////////
//                     PLATFORM HOOK DATA & TABLES                      //
//                     PLATFORM HOOK DATA & TABLES                      //
//                     PLATFORM HOOK DATA & TABLES                      //
//////////////////////////////////////////////////////////////////////////

//
// Error Sources that are supported in the platform & chipset
//

// HW Error notification method used for FW first model
#define     ERROR_STATUS_GAS_DATADEF        {0, 64, 0, 0, 0x0000}
#define     SCI_ERROR_NOTIFY_DATADEF        {ERR_NOTIFY_SCI, sizeof(HW_NOTIFY_METHOD), 0, 0, 0, 0, 0, 0, 0}
#define     NMI_ERROR_NOTIFY_DATADEF        {ERR_NOTIFY_NMI, sizeof(HW_NOTIFY_METHOD), 0, 0, 0, 0, 0, 0, 0}

//
// Generic error HW error sources - Tip: list all FW First error sources
// Note: Used 1KB Error Status block for each error source
//
//  RelSrcId,   MaxErrData, ErrStsAddr,                Notify method,           Max ErrSts block Size
GENERIC_HW_ERROR_SOURCE         PlatformCeSrc = \
    {0xffff,    4096,       ERROR_STATUS_GAS_DATADEF,  SCI_ERROR_NOTIFY_DATADEF,  4096};
GENERIC_HW_ERROR_SOURCE         PlatformUeSrc = \
    {0xffff,    4096,       ERROR_STATUS_GAS_DATADEF,  NMI_ERROR_NOTIFY_DATADEF,  4096};

SYSTEM_GENERIC_ERROR_SOURCE         SysGenErrSources[] = {
//  Type,                     SrcId,  Flags,	NumOfRecords   MaxSecPerRecord     SourceInfo
    {GENERIC_ERROR_CORRECTED, 0,		0,      1,     1,                  &PlatformCeSrc},
    {GENERIC_ERROR_FATAL,     0,		0,      1,     1,                  &PlatformUeSrc},
};
UINTN NumSysGenericErrorSources     = (sizeof(SysGenErrSources)/sizeof(SYSTEM_GENERIC_ERROR_SOURCE));

//
// Native error sources - Tip: Errors that can be handled by OS directly
// Note: Used 1KB Error Status block for each error source
//
HW_NOTIFY_METHOD NotifyMthd ={0,sizeof(HW_NOTIFY_METHOD),0,0,0,0x9,0,0x9,0};

SYSTEM_NATIVE_ERROR_SOURCE          SysNativeErrSources[] = {
00
};


//UINTN NumNativeErrorSources         = (sizeof(SysNativeErrSources)/sizeof(SYSTEM_NATIVE_ERROR_SOURCE));
UINTN NumNativeErrorSources         = 00;

// To be done
#define EFI_DIMM_ADDRESS_TO_GLDN(SlotNumber,Node,Branch,Channel,Dimm)  SlotNumber = \
                                                                (((PLATFORM_MAX_SOCKETS - 1 - Node) << 4) + \
                                                                ((PLATFORM_MAX_BRANCHES - 1 - Branch) << 3) + \
                                                                ((PLATFORM_MAX_CHANNELS - 1 - Channel) << 2) + \
                                                                Dimm)

#define PLATFORM_MAX_SOCKETS      1
#define PLATFORM_MAX_BRANCHES     2
#define PLATFORM_MAX_CHANNELS     2
//////////////////////////////////////////////////////////////////////////
//                      PLATFORM HOOK FUNCTIONS                         //
//                      PLATFORM HOOK FUNCTIONS                         //
//                      PLATFORM HOOK FUNCTIONS                         //
//////////////////////////////////////////////////////////////////////////

EFI_STATUS
InitWheaPlatformHookLib (void)
{
  return EFI_SUCCESS;
}

//
// Update Platform & OEM ID's in Whea ACPI tables.
//
VOID
UpdateAcpiTableIds(
  EFI_ACPI_DESCRIPTION_HEADER     *TableHeader
  )
{
//
// Update the OEMID, Creator ID, and Creator revision.
//
  *(UINT32 *) (TableHeader->OemId)      = 'I' + ('N' << 8) + ('T' << 16) + ('E' << 24);
  *(UINT16 *) (TableHeader->OemId + 4)  = 'L' + (' ' << 8);
  TableHeader->CreatorId                = EFI_ACPI_CREATOR_ID;
  TableHeader->CreatorRevision          = EFI_ACPI_CREATOR_REVISION;

//
// Update OEM revision and OEM TABLE ID based on the platform/SKU
//
  TableHeader->OemRevision = EFI_WHEA_OEM_REVISION;
  TableHeader->OemTableId = BOARD_ID;
}
EFI_STATUS
WheaGetMemoryFruInfo (
  IN MEMORY_DEV_INFO    *MemInfo,
  OUT EFI_GUID          *FruId,
  OUT CHAR8             *FruString,
  OUT UINT16            *Card 
)
{
  CHAR8                     SlotStr[] = {'S', 'l', 'o', 't', ' ', ' ', ' ', 0};
  UINT16                    DimmInstance;
  EFI_STATUS                Status;

  EFI_DIMM_ADDRESS_TO_GLDN (DimmInstance, MemInfo->Node, MemInfo->Branch, MemInfo->Channel, MemInfo->Dimm);

// *** Changed it for now - implement properly
  SlotStr[5] = (CHAR8)(((DimmInstance & 0x00f0) >> 4) + '0');
  SlotStr[6] = (CHAR8)((DimmInstance & 0x000f) + '0');
  Status = AsciiStrCpyS(FruString, 64, SlotStr);
  ASSERT_EFI_ERROR(Status);

  *Card = 0; //BUG BUG: keep Card as 0 for now.
  return EFI_SUCCESS;
}

EFI_STATUS
TriggerWheaSci (
  EFI_SMM_BASE2_PROTOCOL         *SmmBase,
  EFI_SMM_SYSTEM_TABLE2          *Smst
  )
{
  UINT16      AcpiBase;
  UINT8       Data;
  UINT32      Data32;
  EFI_STATUS  Status;
  //
  // Get the PM Base Address
  //
  AcpiBase = PciRead16 (PCI_LIB_ADDRESS (DEFAULT_PCI_BUS_NUMBER_PCH,
                                         PCI_DEVICE_NUMBER_PCH_PMC,
                                         PCI_FUNCTION_NUMBER_PCH_PMC, 
                                         R_PCH_PMC_ACPI_BASE));

  //
  // Base Address should be in bits 15:7 of ACPIBASE register 
  // so they use the whole 16 bit
  //
  AcpiBase = (UINT16) (AcpiBase & B_PCH_PMC_ACPI_BASE_BAR);
   //
   // Trigger SCI
   //

 // Set the SWGPE_CTRL bit to 1
   Status = Smst->SmmIo.Io.Read (&Smst->SmmIo, SMM_IO_UINT32, (AcpiBase + R_PCH_ACPI_GPE_CNTL), 1, &Data32);
   Data32 |= B_PCH_ACPI_GPE_CNTL_SWGPE_CTRL;
   Status = Smst->SmmIo.Io.Write (&Smst->SmmIo, SMM_IO_UINT32, (AcpiBase + R_PCH_ACPI_GPE_CNTL), 1, &Data32);

      //
      // Status Check
      //
      while(TRUE) {
        Smst->SmmIo.Io.Read (&Smst->SmmIo, SMM_IO_UINT8, (AcpiBase + R_PCH_ACPI_GPE0_STS_127_96), 1, &Data );
        if(Data & B_PCH_ACPI_GPE0_STS_127_96_SWGPE)  {
          Data = 0xFF;
          while (Data--);
          //
          // Set the SWGPE_CTRL bit to 0
          //
          Status = Smst->SmmIo.Io.Read (&Smst->SmmIo, SMM_IO_UINT32, (AcpiBase + R_PCH_ACPI_GPE_CNTL), 1, &Data32);
          Data32 &= ~B_PCH_ACPI_GPE_CNTL_SWGPE_CTRL;
          Status = Smst->SmmIo.Io.Write (&Smst->SmmIo, SMM_IO_UINT32, (AcpiBase + R_PCH_ACPI_GPE_CNTL), 1, &Data32);
        break;
        }
      }

  return EFI_SUCCESS;

}

EFI_STATUS
TriggerWheaNmi (
  EFI_SMM_BASE2_PROTOCOL         *SmmBase,
  EFI_SMM_SYSTEM_TABLE2          *Smst
  )
{

  UINT16          TcoBaseAddr;
  UINT8           Data;
  UINT8           Save_Nmi2Smi_En;
  UINT8           Save_Port70;
  UINT16          Data16;
  EFI_STATUS      Status;

  DEBUG ((DEBUG_INFO,"Generating NMI Now\n"));
  TcoBaseAddr = 0;

  //
  // Get the TCO Base Address
  // TcoBase was moved to SMBUS device in PCH
  //
  if(EFI_ERROR(PchTcoBaseGet(&TcoBaseAddr))) {
      return RETURN_NOT_FOUND;
  }

  // Read the NMI2SMI_EN bit, save it for future restore
  Status = Smst->SmmIo.Io.Read (&Smst->SmmIo, SMM_IO_UINT8, (TcoBaseAddr + R_PCH_TCO1_CNT + 1), 1, &Save_Nmi2Smi_En);

  // Set the NMI2SMI_EN bit to 0
  // This also clears NMI_NOW if it is already set
  Data = (UINT8)(Save_Nmi2Smi_En & 0xFD);
  Status = Smst->SmmIo.Io.Write (&Smst->SmmIo, SMM_IO_UINT8, (TcoBaseAddr + R_PCH_TCO1_CNT + 1), 1, &Data);

  // Enable NMI_EN
  Status = Smst->SmmIo.Io.Read (&Smst->SmmIo, SMM_IO_UINT8, 0x74, 1, &Save_Port70);
  Data = (UINT8)(Save_Port70 & 0x7F);
  Status = Smst->SmmIo.Io.Write (&Smst->SmmIo, SMM_IO_UINT8, R_PCH_NMI_EN, 1, &Data);

  // Set NMI_NOW = 1
  Status = Smst->SmmIo.Io.Read (&Smst->SmmIo, SMM_IO_UINT8, (TcoBaseAddr + R_PCH_TCO1_CNT + 1), 1, &Data);
  Data = (UINT8) (Data | 0x01);
  Status = Smst->SmmIo.Io.Write (&Smst->SmmIo, SMM_IO_UINT8, (TcoBaseAddr + R_PCH_TCO1_CNT + 1), 1, &Data);

  //
  // Clear NMI_NOW = 0 by writing 1 to NMI_NOW bit
  //
  Status = Smst->SmmIo.Io.Read (&Smst->SmmIo, SMM_IO_UINT8, (TcoBaseAddr + R_PCH_TCO1_CNT + 1), 1, &Data);
  Data |= 0x01;
  Status = Smst->SmmIo.Io.Write (&Smst->SmmIo, SMM_IO_UINT8, (TcoBaseAddr + R_PCH_TCO1_CNT + 1), 1, &Data);

  // Clear the CPUSERR_STS/DMISERR_STS bit, bit 12
  Status = Smst->SmmIo.Io.Read (&Smst->SmmIo, SMM_IO_UINT16, (TcoBaseAddr + R_PCH_TCO1_STS), 1, &Data16);
  Data16 = (Data16 | B_PCH_TCO1_STS_DMISERR);
  Status = Smst->SmmIo.Io.Write (&Smst->SmmIo, SMM_IO_UINT16, (TcoBaseAddr + R_PCH_TCO1_STS), 1, &Data16);

  // Clear the NMI2SMI_STS bit if set
  Status = Smst->SmmIo.Io.Read (&Smst->SmmIo, SMM_IO_UINT16, (TcoBaseAddr + R_PCH_TCO1_STS), 1, &Data16);
  if (Data16 & 0x0001) {
    // check port 0x61
    Status = Smst->SmmIo.Io.Read (&Smst->SmmIo, SMM_IO_UINT8, NMI_STS_CNT, 1, &Data);
    if (Data & 0x80) {
      Status = Smst->SmmIo.Io.Read (&Smst->SmmIo, SMM_IO_UINT8, NMI_STS_CNT, 1, &Data);
      Data = (UINT8) (Data | 0x04);
      Data = (UINT8) (Data & 0x0F);
      Status = Smst->SmmIo.Io.Write (&Smst->SmmIo, SMM_IO_UINT8, NMI_STS_CNT, 1, &Data);
      Data = (UINT8) (Data & 0x0B);
      Status = Smst->SmmIo.Io.Write (&Smst->SmmIo, SMM_IO_UINT8, NMI_STS_CNT, 1, &Data);
    }
  }

  // Restore NMI_EN
  Status = Smst->SmmIo.Io.Write (&Smst->SmmIo, SMM_IO_UINT8, R_PCH_NMI_EN, 1, &Save_Port70);

  return EFI_SUCCESS;

}

EFI_STATUS
ProgramWheaSignalsHook (
  BOOLEAN                     EnableWhea,
  EFI_SMM_BASE2_PROTOCOL       *SmmBase,
  EFI_SMM_SYSTEM_TABLE2        *Smst
  )
{
  UINT8                       Data;
  UINT16                      PmBase;
  EFI_STATUS                  Status;

  //
  // Get the PM Base Address
  //
  PmBase = PciRead16 (PCI_LIB_ADDRESS (DEFAULT_PCI_BUS_NUMBER_PCH,
                                      PCI_DEVICE_NUMBER_PCH_PMC,
                                      PCI_FUNCTION_NUMBER_PCH_PMC, 
                                      R_PCH_PMC_ACPI_BASE)
                                      );

  //
  // Base Address should be in bits 15:7 of PMBASE register 
  // so they use the whole 16 bit
  //
  PmBase = (UINT16) (PmBase & B_PCH_PMC_ACPI_BASE_BAR);
  
  // Set SWGPE_EN to cause a SCI 
  //
  Status = Smst->SmmIo.Io.Write (&Smst->SmmIo, SMM_IO_UINT8, PmBase + R_PCH_ACPI_GPE0a_EN, 1, &Data);
  Data |=  BIT2;
  Status = Smst->SmmIo.Io.Write (&Smst->SmmIo, SMM_IO_UINT8, PmBase + R_PCH_ACPI_GPE0a_EN, 1, &Data);


  return EFI_SUCCESS;
}



/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) 2009-2010 Intel Corporation.  All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license.  Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

-----------------------------------------------------------------------------*/
