/*++
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
--*/
/*++
  ACPI Platform Driver

Copyright (c) 2014 - 2017, Intel Corporation. All rights reserved. <BR>
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

--*/

#include <IndustryStandard/Pci22.h>
#include "AcpiPlatform.h"
#include "..\AcpiTables\Bdat\Bdat.h"
#define MRC_DATA_REQUIRED_FROM_OUTSIDE
#include "Bdat.h"
#include <PchAccess.h>
#include <SouthCluster/Include/Library/PchP2sbLib.h>
#include <SouthCluster/Include/Register/PchRegsP2sb.h>
#include <Include/Library/MmPciLib.h>
#include <IndustryStandard/DebugPort2Table.h>
// Denverton AptioV Override Start
//#include <Library/PeiDxeSmmPchSerialIoLib.h>
// Denverton AptioV Override End
#include <Library/HobLib.h>
#include <IncludePrivate/PchPolicyHob.h>
#include <AmiProtocol.h>

// Denverton AptioV Override Start - EIP#288690
#include <SocRegs.h>
#include <SaRegs.h>

#define RES_MEM64_36_LIMIT      0x01000000000 - 1           // 2^36
#define RES_MEM64_39_LIMIT      0x08000000000 - 1           // 2^39
// Denverton AptioV Override End - EIP#288690

#define DUNIT_PORT       0x10
// Denverton AptioV Override Start
// #define EFI_ACPI_OEM_DEBUG_PORT_2_TABLE_REVISION      0x00
// Denverton AptioV Override End

//
// Bit0 Legacy boot only. Bit 7 Legacy VGA.
// If legacy boot is set the legacy VGA is also set. 
// Only legacy VGA is an option to check, if it is going to be
// implemented then the bit defines can be changed.
//
#define LEGACY_BOOT      0x81   

//
// Global Variable
//
STATIC EFI_GLOBAL_NVS_AREA_PROTOCOL  mGlobalNvsArea;
EFI_ACPI_SDT_PROTOCOL                *mAcpiSdt;

EFI_ACPI_HANDLE mDsdtHandle = NULL;
EFI_ACPI_SERIAL_PORT_CONSOLE_REDIRECTION_TABLE *mSpcrTable;
EFI_GUID              mSystemConfigurationGuid = SYSTEM_CONFIGURATION_GUID;
SYSTEM_CONFIGURATION  mSystemConfiguration;
extern EFI_GUID gEfiGlobalVariableGuid;


EFI_STATUS
FindDsdtFadtTable (
		    IN   EFI_ACPI_TABLE_VERSION     Version,
		    IN   UINT32                     Signature,
		    OUT  EFI_ACPI_SDT_HEADER        **Table,
		    OUT  UINTN                      *TableKey
    )
/*++

  Routine Description:

  Returns specific ACPI table for the Signature and Version passed as inputs. Otherwise returns EFI_NOT_FOUND.

 Arguments:
 
    Version     - Acpi table version.
    Signature   - Signature of the Acpi table.
    **Table     - Contains a header of Acpi table found.
    *TableKey   - Contains handle of the Acpi table found.

 Returns:
     EFI_STATUS


--*/
{
    EFI_STATUS               Status = EFI_SUCCESS;
    UINTN                    Index = 0;
    EFI_ACPI_TABLE_VERSION   TableVersion;
      
    // If DSDT table is already preset, it must be deleted
    if (mAcpiSdt != NULL)
       {
        for (;Status == EFI_SUCCESS; Index++)
            {
             // Get the Handle of the previous DSDT table
             Status = mAcpiSdt->GetAcpiTable (
                                    Index,
                                    Table,
                                    &TableVersion,
                                    TableKey
                                    );
             if (EFI_ERROR(Status)) break;
                
             // Check if the obtained table is DSDT / FADT
             if (Signature == (*Table)->Signature) {
                 // We got a DSDT / FADT table. 
             
                         switch (Version) {
                             case EFI_ACPI_TABLE_VERSION_ALL:
                                 return EFI_SUCCESS;
                             case EFI_ACPI_TABLE_VERSION_X:
                                 if (TableVersion & EFI_ACPI_TABLE_VERSION_X) {
                                     return EFI_SUCCESS;
                                 }
                                 break;
                             default:
                                 if (Version == TableVersion) {
                                     return EFI_SUCCESS;
                                 }
                         }
                     }
        }
     }
     return EFI_SUCCESS;
}

VOID
UpdateAcpiTableIds(
  EFI_ACPI_DESCRIPTION_HEADER     *TableHeader
  )
{
//
// Update the OEMID, Creator ID, and Creator revision.
//
// Denverton AptioV Override Start - EIP#314064
// Update OemId and OemTableId from SDL Token (AMI Format)	
  /*
  *(UINT32 *) (TableHeader->OemId)      = 'I' + ('N' << 8) + ('T' << 16) + ('E' << 24);
  *(UINT16 *) (TableHeader->OemId + 4)  = 'L' + (' ' << 8); 
  */
  UINT64                        OemTableId;
  
  CopyMem (TableHeader->OemId, PcdGetPtr (PcdAcpiDefaultOemId), sizeof(TableHeader->OemId));    //Set OemId value from PcdAcpiDefaultOemId PCD
  
  OemTableId = PcdGet64 (PcdAcpiDefaultOemTableId);
  CopyMem ((UINT8 *)&TableHeader->OemTableId, &OemTableId, sizeof(TableHeader->OemTableId));    //Set OemTableId value from PcdAcpiDefaultOemTableId PCD
// Denverton AptioV Override End - EIP#314064

  TableHeader->CreatorId                = EFI_ACPI_CREATOR_ID;
  TableHeader->CreatorRevision          = EFI_ACPI_CREATOR_REVISION;
}

EFI_STATUS
LocateSupportProtocol (
  IN  EFI_GUID                       *Protocol,
  OUT VOID                           **Instance,
  IN  UINT32                         Type
  )
/*++

Routine Description:

  Locate the first instance of a protocol.  If the protocol requested is an
  FV protocol, then it will return the first FV that contains the ACPI table
  storage file.

Arguments:

  Protocol      The protocol to find.
  Instance      Return pointer to the first instance of the protocol

Returns:

  EFI_SUCCESS           The function completed successfully.
  EFI_NOT_FOUND         The protocol could not be located.
  EFI_OUT_OF_RESOURCES  There are not enough resources to find the protocol.

--*/
{
  EFI_STATUS              Status;
  EFI_HANDLE              *HandleBuffer;
  UINTN                   NumberOfHandles;
  EFI_FV_FILETYPE         FileType;
  UINT32                  FvStatus;
  EFI_FV_FILE_ATTRIBUTES  Attributes;
  UINTN                   Size;
  UINTN                   i;

  FvStatus = 0;

  //
  // Locate protocol.
  //
  Status = gBS->LocateHandleBuffer (
                   ByProtocol,
                   Protocol,
                   NULL,
                   &NumberOfHandles,
                   &HandleBuffer
                   );
  if (EFI_ERROR (Status)) {

    //
    // Defined errors at this time are not found and out of resources.
    //
    return Status;
  }



  //
  // Looking for FV with ACPI storage file
  //

  for (i = 0; i < NumberOfHandles; i++) {
    //
    // Get the protocol on this handle
    // This should not fail because of LocateHandleBuffer
    //
    Status = gBS->HandleProtocol (
                     HandleBuffer[i],
                     Protocol,
                     Instance
                     );
    ASSERT_EFI_ERROR (Status);

    if (!Type) {
      //
      // Not looking for the FV protocol, so find the first instance of the
      // protocol.  There should not be any errors because our handle buffer
      // should always contain at least one or LocateHandleBuffer would have
      // returned not found.
      //
      break;
    }

    //
    // See if it has the ACPI storage file
    //

    Status = ((EFI_FIRMWARE_VOLUME2_PROTOCOL*) (*Instance))->ReadFile (*Instance,
                                                              (EFI_GUID*)PcdGetPtr (PcdAcpiTableStorageFile),
                                                              NULL,
                                                              &Size,
                                                              &FileType,
                                                              &Attributes,
                                                              &FvStatus
                                                              );

    //
    // If we found it, then we are done
    //
    if (Status == EFI_SUCCESS) {
      break;
    }
  }

  //
  // Our exit status is determined by the success of the previous operations
  // If the protocol was found, Instance already points to it.
  //

  //
  // Free any allocated buffers
  //
  gBS->FreePool (HandleBuffer);

  return Status;
}


VOID
DsdtTableUpdate (
  IN OUT   EFI_ACPI_DESCRIPTION_HEADER  *TableHeader,
  IN OUT   EFI_ACPI_TABLE_VERSION       *Version
  )
/*++

  Routine Description:

    Update the DSDT table

  Arguments:

    Table   - The table to be set
    Version - Version to publish

  Returns:

    None

--*/
{

  UINT8      *CurrPtr;
  UINT8      *DsdtPointer;
  UINT32     *Signature;
  UINT8      *Operation;
  UINT32     *Address;
  UINT16     *Size;
  //
  // Loop through the ASL looking for values that we must fix up.
  //
  CurrPtr = (UINT8 *) TableHeader;
  for (DsdtPointer = CurrPtr;
       DsdtPointer <= (CurrPtr + ((EFI_ACPI_COMMON_HEADER *) CurrPtr)->Length);
       DsdtPointer++
      )
  {
    Signature = (UINT32 *) DsdtPointer;
    switch (*Signature) {
    //
    // MNVS operation region
    //
    case (SIGNATURE_32 ('M', 'N', 'V', 'S')):
      //
      // Conditional match.  For Region Objects, the Operator will always be the
      // byte immediately before the specific name.  Therefore, subtract 1 to check
      // the Operator.
      //
      Operation = DsdtPointer - 1;
      if (*Operation == AML_OPREGION_OP) {
        Address   = (UINT32 *) (DsdtPointer + 6);
        *Address  = (UINT32) (UINTN) mGlobalNvsArea.Area;
        Size      = (UINT16 *) (DsdtPointer + 11);
        *Size     = sizeof (EFI_GLOBAL_NVS_AREA);
      }
      break;

    default:
      break;
    }
  }
}


VOID
ApicTableUpdate (
  IN OUT   EFI_ACPI_DESCRIPTION_HEADER  *TableHeader,
  IN OUT   EFI_ACPI_TABLE_VERSION       *Version
  )
/*++

  Routine Description:

    Update the processors information in the APIC table

  Arguments:

    Table   - The table to be set
    Version - Version to publish

  Returns:

    None

--*/
{
  EFI_STATUS                 Status;
  EFI_MP_SERVICES_PROTOCOL   *MpService;
  UINT8                      *CurrPtr;
  UINT8                      *EndPtr;
  UINT8                      CurrProcessor;
  UINTN                      NumberOfCPUs;
  UINTN                      NumberOfEnabledCPUs;
  UINTN                      BufferSize;
  EFI_PROCESSOR_INFORMATION  MpContext;
  ACPI_APIC_STRUCTURE_PTR    *ApicPtr;


  CurrProcessor = 0;
  //
  // Find the MP Protocol. This is an MP platform, so MP protocol must be
  // there.
  //
  Status = gBS->LocateProtocol (
                  &gEfiMpServiceProtocolGuid,
                  NULL,
                  (VOID**)&MpService
                  );
  if (EFI_ERROR (Status)) {
    //
    // Failed to get MP information, doesn't publish the invalid table
    //
    *Version = EFI_ACPI_TABLE_VERSION_NONE;
    return;
  }

  //
  // Determine the number of processors
  //
  MpService->GetNumberOfProcessors (
               MpService,
               &NumberOfCPUs,
               &NumberOfEnabledCPUs
               );

  CurrPtr = (UINT8*) &(TableHeader[1]);
  CurrPtr = CurrPtr + 8; // Size of Local APIC Address & Flag
  EndPtr  = (UINT8*) TableHeader;
  EndPtr  = EndPtr + TableHeader->Length;

  while (CurrPtr < EndPtr) {

    ApicPtr = (ACPI_APIC_STRUCTURE_PTR*) CurrPtr;
    switch (ApicPtr->AcpiApicCommon.Type) {

    case EFI_ACPI_1_0_PROCESSOR_LOCAL_APIC:
      BufferSize = sizeof (EFI_PROCESSOR_INFORMATION);
      ApicPtr->AcpiLocalApic.Flags = 0;
      ApicPtr->AcpiLocalApic.ApicId = 0;
      Status = MpService->GetProcessorInfo (
                            MpService,
                            CurrProcessor,
                            &MpContext
                            );

      if (!EFI_ERROR (Status)) {
        if (MpContext.StatusFlag & PROCESSOR_ENABLED_BIT) {
          ApicPtr->AcpiLocalApic.Flags = EFI_ACPI_3_0_LOCAL_APIC_ENABLED;
        }
        ApicPtr->AcpiLocalApic.ApicId = (UINT8)MpContext.ProcessorId;
      }
      CurrProcessor++;
      break;

    case EFI_ACPI_1_0_IO_APIC:
      //
      // IO APIC entries can be patched here
      //
    default:
      break;
    };
    CurrPtr = CurrPtr + ApicPtr->AcpiApicCommon.Length;
  }
}


INT16
Crc16 (
  IN INT8  *Ptr,
  IN INT32 Count
)
/*++

Routine Description:

This function returns the CRC16 of the provided array.

Arguments:

  Ptr:    The array to be processed.
  Count:  Number of elements in the array.

Returns:

  CRC16 of the provided array.

--*/
{
  INT16 crc, i;
  crc = 0;
  while (--Count >= 0) {
    crc = crc ^ (INT16) (int) * Ptr++ << 8;
    for (i = 0; i < 8; ++i) {
      if (crc & 0x8000) {
        crc = crc << 1 ^ 0x1021;
      } else {
        crc = crc << 1;
      }
    }
  }
  return (crc & 0xFFFF);
}

VOID 
PrintBdatStructure (
  IN        BDAT_STRUCTURE                  *BdatStructure
)
{
  UINT8     TempValue8;
  UINT8     *PtrValue8;

  DEBUG ((EFI_D_ERROR, "Printing BDAT 4.0 Structure\n"));

  PtrValue8 = (UINT8 *) BdatStructure;
  DEBUG ((EFI_D_ERROR, "-BdatHeader\n"));
  for (TempValue8 = 0; TempValue8 < 8; TempValue8++) {
    DEBUG ((EFI_D_ERROR, "  BiosDataSignature[%d] = %c\n", TempValue8, PtrValue8[TempValue8]));
  }
  DEBUG ((EFI_D_ERROR, "  BiosDataStructSize = %d\n", BdatStructure->BdatHeader.BiosDataStructSize));
  DEBUG ((EFI_D_ERROR, "  Crc16              = 0x%04x\n", BdatStructure->BdatHeader.Crc16));
  DEBUG ((EFI_D_ERROR, "  PrimaryVersion     = %d\n", BdatStructure->BdatHeader.PrimaryVersion));
  DEBUG ((EFI_D_ERROR, "  SecondaryVersion   = %d\n", BdatStructure->BdatHeader.SecondaryVersion));
  DEBUG ((EFI_D_ERROR, "  OemOffset          = %d\n", BdatStructure->BdatHeader.OemOffset));

  DEBUG ((EFI_D_ERROR, "-BdatSchemas\n"));

  DEBUG ((EFI_D_ERROR, "  SchemaListLength = %d\n", BdatStructure->BdatSchemas.SchemaListLength));
  DEBUG ((EFI_D_ERROR, "  Year             = %d\n", BdatStructure->BdatSchemas.Year));
  DEBUG ((EFI_D_ERROR, "  Month            = %d\n", BdatStructure->BdatSchemas.Month));
  DEBUG ((EFI_D_ERROR, "  Day              = %d\n", BdatStructure->BdatSchemas.Day));
  DEBUG ((EFI_D_ERROR, "  Hour             = %d\n", BdatStructure->BdatSchemas.Hour));
  DEBUG ((EFI_D_ERROR, "  Minute           = %d\n", BdatStructure->BdatSchemas.Minute));
  DEBUG ((EFI_D_ERROR, "  Second           = %d\n", BdatStructure->BdatSchemas.Second));

  for (TempValue8 = 0; TempValue8 < BdatStructure->BdatSchemas.SchemaListLength; TempValue8++) {
    DEBUG ((EFI_D_ERROR, "  Schemas[%d]           = 0x%08x\n", TempValue8, BdatStructure->BdatSchemas.Schemas[TempValue8]));
  }
}

VOID 
PrintMemoryData2Structure (
  IN        BDAT_MEMORY_DATA_2_STRUCTURE    *BdatMemoryData2Structure
)
{
  UINT8     Node;
  UINT8     Channel;
  UINT8     Dimm;
  UINT8     Rank;
  UINT16    SpdElement;
  UINT8     Strobe;
  UINT8     DramDevice;
  UINT8     ModeRegister;
  UINT8     Dq;
  UINT8     TempValue8;
  UINT8     *PtrValue8;

  BDAT_SOCKET_2_STRUCTURE     *BdatSocket;
  BDAT_CHANNEL_2_STRUCTURE    *BdatChannel;
  BDAT_DIMM_2_STRUCTURE       *BdatDimm;
  BDAT_RANK_2_STRUCTURE       *BdatRank;
  BDAT_SPD_2_STRUCTURE        *BdatSpd;

  DEBUG ((EFI_D_ERROR, "Printing BDAT Memory Schema 2 Structure\n"));

  DEBUG ((EFI_D_ERROR, "-schemaHeader\n"));

  DEBUG ((EFI_D_ERROR, "  SchemaId = {"));
  PtrValue8 = (UINT8 *)&BdatMemoryData2Structure->schemaHeader.SchemaId;

  TempValue8 = 0;
  while (TempValue8 < sizeof (EFI_GUID)) {
    if (TempValue8 == 0) {
      DEBUG ((EFI_D_ERROR, "%08x, ", ((UINT32 *) PtrValue8)[TempValue8]));
      TempValue8 += 4;
    } else if (TempValue8 == 4 || TempValue8 == 6) {
      DEBUG ((EFI_D_ERROR, "%04x, ", ((UINT16 *) PtrValue8)[TempValue8]));
      TempValue8 += 2;
    } else {
      DEBUG ((EFI_D_ERROR, "%02x, ", ((UINT8 *) PtrValue8)[TempValue8]));
      TempValue8 += 1;
    }
  }
  DEBUG ((EFI_D_ERROR, "}\n"));

  DEBUG ((EFI_D_ERROR, "  DataSize = %02d\n", BdatMemoryData2Structure->schemaHeader.DataSize));
  DEBUG ((EFI_D_ERROR, "  Crc16    = %02d\n", BdatMemoryData2Structure->schemaHeader.Crc16));

  DEBUG ((EFI_D_ERROR, "refCodeRevision = 0x%08x\n", BdatMemoryData2Structure->refCodeRevision));
  DEBUG ((EFI_D_ERROR, "maxNode         = %02d\n", BdatMemoryData2Structure->maxNode));
  DEBUG ((EFI_D_ERROR, "maxCh           = %02d\n", BdatMemoryData2Structure->maxCh));
  DEBUG ((EFI_D_ERROR, "maxDimm         = %02d\n", BdatMemoryData2Structure->maxDimm));
  DEBUG ((EFI_D_ERROR, "maxRankDimm     = %02d\n", BdatMemoryData2Structure->maxRankDimm));
  DEBUG ((EFI_D_ERROR, "maxStrobe       = %02d\n", BdatMemoryData2Structure->maxStrobe));
  DEBUG ((EFI_D_ERROR, "maxDq           = %02d\n", BdatMemoryData2Structure->maxDq));
  DEBUG ((EFI_D_ERROR, "marginLoopCount = %02d\n", BdatMemoryData2Structure->marginLoopCount));

  for (Node = 0; Node < BDAT_MAX_NODES; Node++) {
    DEBUG ((EFI_D_ERROR, "-socketList[%d]\n", Node));
    BdatSocket = (BDAT_SOCKET_2_STRUCTURE *) &BdatMemoryData2Structure->socketList[Node].imcEnabled;
    
    DEBUG ((EFI_D_ERROR, "  imcEnabled = %02d\n", BdatSocket->imcEnabled));
    DEBUG ((EFI_D_ERROR, "  imcDid     = 0x%04x\n", BdatSocket->imcDid));
    DEBUG ((EFI_D_ERROR, "  imcRid     = 0x%02x\n", BdatSocket->imcRid));
    DEBUG ((EFI_D_ERROR, "  ddrFreq    = %05d\n", BdatSocket->ddrFreq));
    DEBUG ((EFI_D_ERROR, "  ddrVoltage = %04d\n", BdatSocket->ddrVoltage));
    DEBUG ((EFI_D_ERROR, "  rxVrefStep = %04d\n", BdatSocket->rxVrefStep));
    DEBUG ((EFI_D_ERROR, "  txVrefStep = %04d\n", BdatSocket->txVrefStep));
    DEBUG ((EFI_D_ERROR, "  caVrefStep = %04d\n", BdatSocket->caVrefStep));
    DEBUG ((EFI_D_ERROR, "  recvenStep = %02d\n", BdatSocket->recvenStep));
    DEBUG ((EFI_D_ERROR, "  wrLevStep  = %02d\n", BdatSocket->wrLevStep));
                                                
    for (Channel = 0; Channel < BDAT_MAX_CHANNELS; Channel++) {
      DEBUG ((EFI_D_ERROR, "  -channelList[%d]\n", Channel));
      BdatChannel = (BDAT_CHANNEL_2_STRUCTURE *) &BdatSocket->channelList[Channel].chEnabled;

      DEBUG ((EFI_D_ERROR, "    chEnabled   = %02d\n", BdatChannel->chEnabled));
      DEBUG ((EFI_D_ERROR, "    numDimmSlot = %02d\n", BdatChannel->numDimmSlot));
      //                              
      // Skip if channel is disabled  
      //                              
      if (BdatChannel->chEnabled == 0) {
        continue;
      }
      
      for (Dimm = 0; Dimm < BDAT_MAX_DIMM; Dimm++) {
        DEBUG ((EFI_D_ERROR, "    -dimmList[%d]\n", Dimm));
        BdatDimm  = (BDAT_DIMM_2_STRUCTURE *) &BdatChannel->dimmList[Dimm].dimmEnabled;

        DEBUG ((EFI_D_ERROR, "      dimmEnabled = %02d\n", BdatDimm->dimmEnabled));
        //
        // Skip if no DIMM present
        //
        if (BdatDimm->dimmEnabled == 0) {
          continue;
        }

        DEBUG ((EFI_D_ERROR, "      -spdBytes\n"));
        BdatSpd = (BDAT_SPD_2_STRUCTURE *) &BdatDimm->spdBytes.valid[0];

        for (SpdElement = 0; SpdElement < MAX_SPD_BYTE_512; SpdElement++) {
          if ((SpdElement % 8) == 0) {
            DEBUG ((EFI_D_ERROR, "        valid = 0x%02x\n", BdatSpd->valid[SpdElement / 8]));
          }
          if ((BdatSpd->valid[SpdElement / 8] & (1 << (SpdElement % 8))) == 0) {
            continue;  // There are a few values in the "SPD" table that do not actually exist in the SPD.
          }
          DEBUG ((EFI_D_ERROR, "        spdData[%03d] = 0x%02x\n", SpdElement, BdatSpd->spdData[SpdElement]));
        }

        for (Rank = 0; Rank < BDAT_MAX_RANK_DIMM; Rank++) {
          DEBUG ((EFI_D_ERROR, "      -rankList[%d]\n", Rank));
          BdatRank = (BDAT_RANK_2_STRUCTURE *) &BdatDimm->rankList[Rank].rankEnabled;

          DEBUG ((EFI_D_ERROR, "        rankEnabled       = %02d\n", BdatRank->rankEnabled));
          DEBUG ((EFI_D_ERROR, "        rankMarginEnabled = %02d\n", BdatRank->rankMarginEnabled));
          DEBUG ((EFI_D_ERROR, "        dqMarginEnabled   = %02d\n", BdatRank->dqMarginEnabled));
          //
          // Skip if no Rank or if it is mapped out
          //
          if (BdatRank->rankEnabled == 0) {
            continue;
          }

          DEBUG ((EFI_D_ERROR, "        -rankMargin\n"));
          DEBUG ((EFI_D_ERROR, "          rxDqLeft    = %02d\n", (INT8) BdatRank->rankMargin.rxDqLeft));
          DEBUG ((EFI_D_ERROR, "          rxDqRight   = %02d\n", (INT8) BdatRank->rankMargin.rxDqRight));
          DEBUG ((EFI_D_ERROR, "          txDqLeft    = %02d\n", (INT8) BdatRank->rankMargin.txDqLeft));
          DEBUG ((EFI_D_ERROR, "          txDqRight   = %02d\n", (INT8) BdatRank->rankMargin.txDqRight));
          DEBUG ((EFI_D_ERROR, "          cmdLeft     = %02d\n", (INT8) BdatRank->rankMargin.cmdLeft));
          DEBUG ((EFI_D_ERROR, "          cmdRight    = %02d\n", (INT8) BdatRank->rankMargin.cmdRight));
          DEBUG ((EFI_D_ERROR, "          recvenLeft  = %02d\n", (INT8) BdatRank->rankMargin.recvenLeft));
          DEBUG ((EFI_D_ERROR, "          recvenRight = %02d\n", (INT8) BdatRank->rankMargin.recvenRight));
          DEBUG ((EFI_D_ERROR, "          wrLevLeft   = %02d\n", (INT8) BdatRank->rankMargin.wrLevLeft));
          DEBUG ((EFI_D_ERROR, "          wrLevRight  = %02d\n", (INT8) BdatRank->rankMargin.wrLevRight));
          DEBUG ((EFI_D_ERROR, "          rxVrefLow   = %02d\n", (INT8) BdatRank->rankMargin.rxVrefLow));
          DEBUG ((EFI_D_ERROR, "          rxVrefHigh  = %02d\n", (INT8) BdatRank->rankMargin.rxVrefHigh));
          DEBUG ((EFI_D_ERROR, "          txVrefLow   = %02d\n", (INT8) BdatRank->rankMargin.txVrefLow));
          DEBUG ((EFI_D_ERROR, "          txVrefHigh  = %02d\n", (INT8) BdatRank->rankMargin.txVrefHigh));
          DEBUG ((EFI_D_ERROR, "          cmdVrefLow  = %02d\n", (INT8) BdatRank->rankMargin.cmdVrefLow));
          DEBUG ((EFI_D_ERROR, "          cmdVrefHigh = %02d\n", (INT8) BdatRank->rankMargin.cmdVrefHigh));

          if (BdatRank->dqMarginEnabled) {
            for (Dq = 0; Dq < BDAT_MAX_DQ; Dq++) {
              DEBUG ((EFI_D_ERROR, "        -dqMargin[%d]\n", Dq));
              DEBUG ((EFI_D_ERROR, "          rxDqLeft   = %02d\n", BdatRank->rankMargin.rxDqLeft));
              DEBUG ((EFI_D_ERROR, "          rxDqRight  = %02d\n", BdatRank->rankMargin.rxDqRight));
              DEBUG ((EFI_D_ERROR, "          txDqLeft   = %02d\n", BdatRank->rankMargin.txDqLeft));
              DEBUG ((EFI_D_ERROR, "          txDqRight  = %02d\n", BdatRank->rankMargin.txDqRight));
              DEBUG ((EFI_D_ERROR, "          rxVrefLow  = %02d\n", BdatRank->rankMargin.cmdLeft));
              DEBUG ((EFI_D_ERROR, "          rxVrefHigh = %02d\n", BdatRank->rankMargin.cmdRight));
              DEBUG ((EFI_D_ERROR, "          txVrefLow  = %02d\n", BdatRank->rankMargin.recvenLeft));
              DEBUG ((EFI_D_ERROR, "          txVrefHigh = %02d\n", BdatRank->rankMargin.recvenRight));
            }
          }

          DEBUG ((EFI_D_ERROR, "        -rankTraining\n"));
          for (Strobe = 0; Strobe < BDAT_MAX_STROBES; Strobe++) {
            DEBUG ((EFI_D_ERROR, "          recEnDelay[%02d] = %04d\n", Strobe, BdatRank->rankTraining.recEnDelay[Strobe]));
          } // Strobe loop ...
          for (Strobe = 0; Strobe < BDAT_MAX_STROBES; Strobe++) {
            DEBUG ((EFI_D_ERROR, "          wlDelay[%02d]    = %04d\n", Strobe, BdatRank->rankTraining.wlDelay[Strobe]));
          } // Strobe loop ...
          for (Strobe = 0; Strobe < BDAT_MAX_STROBES; Strobe++) {
            DEBUG ((EFI_D_ERROR, "          rxDqDelay[%02d]  = %02d\n", Strobe, BdatRank->rankTraining.rxDqDelay[Strobe]));
          } // Strobe loop ...
          for (Strobe = 0; Strobe < BDAT_MAX_STROBES; Strobe++) {
            DEBUG ((EFI_D_ERROR, "          txDqDelay[%02d]  = %02d\n", Strobe, BdatRank->rankTraining.txDqDelay[Strobe]));
          } // Strobe loop ...

          DEBUG ((EFI_D_ERROR, "          clkDelay  = %03d\n", BdatRank->rankTraining.clkDelay));
          DEBUG ((EFI_D_ERROR, "          ctlDelay  = %03d\n", BdatRank->rankTraining.ctlDelay));
          DEBUG ((EFI_D_ERROR, "          cmdDelay  = %03d : %03d : %03d\n", BdatRank->rankTraining.cmdDelay[0], BdatRank->rankTraining.cmdDelay[1], BdatRank->rankTraining.cmdDelay[2]));
          DEBUG ((EFI_D_ERROR, "          ioLatency = %03d\n", BdatRank->rankTraining.ioLatency));
          DEBUG ((EFI_D_ERROR, "          roundtrip = %03d\n", BdatRank->rankTraining.roundtrip));

          for (DramDevice = 0; DramDevice < MAX_DRAM_DEVICE; DramDevice++) {
            DEBUG ((EFI_D_ERROR, "        -rankMRS[%d]\n", DramDevice));
            for (ModeRegister = 0; ModeRegister < 4; ModeRegister++) {
              DEBUG ((EFI_D_ERROR, "          modeRegister[%d] = 0x%04x\n", ModeRegister, BdatRank->rankMRS[DramDevice].modeRegister[ModeRegister]));
            }
          } // DramDevice loop
        } // Rank loop
      } // Dimm loop
    } // Channel loop
  } // Node loop
}

VOID
BdatTableUpdate (
  IN OUT   EFI_ACPI_DESCRIPTION_HEADER  *TableHeader,
  IN OUT   EFI_ACPI_TABLE_VERSION       *Version
  )
/*++

  Routine Description:

    Update the BDAT ACPI table

  Arguments:

    Table   - The table to be set
    Version - Version to publish

  Returns:

    None.

--*/
{
  EFI_STATUS                        Status;
  EFI_PHYSICAL_ADDRESS              Address;
  BDAT_STRUCTURE                    *BdatStructure;
  BDAT_MEMORY_DATA_2_STRUCTURE      *BdatMemoryData2Structure;                        
  EFI_BDAT_ACPI_DESCRIPTION_TABLE   *BdatAcpiDescriptionTable;
  UINT8                             BdatSignature[] = "BDATHEAD";
  UINT32                            PtrIdx;
  EFI_HOB_GUID_TYPE                 *GuidHob;
  VOID                              *HobData;
  EFI_TIME                          EfiTime;
  UINT8                             Checksum;

  GuidHob = NULL;
  HobData = NULL;

  DEBUG ((EFI_D_INFO, "MMRC2P0_ENABLE = 1, BdatTableUpdate\n"));
  DEBUG ((EFI_D_ERROR, "BDAT_STRUCTURE = %d Bytes\n", sizeof(BDAT_STRUCTURE)));
  DEBUG ((EFI_D_ERROR, "BDAT_MEMORY_DATA_2_STRUCTURE = %d Bytes\n", sizeof(BDAT_MEMORY_DATA_2_STRUCTURE)));

  Address = 0xFFFFFFFF;
  Status  = gBS->AllocatePages (
                   AllocateMaxAddress,
                   EfiReservedMemoryType,
                   EFI_SIZE_TO_PAGES (sizeof (BDAT_STRUCTURE) + sizeof (BDAT_MEMORY_DATA_2_STRUCTURE)),
                   &Address
                 );
  
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Status 0x%08x\n", Status));
    return;
  }

  BdatStructure = (BDAT_STRUCTURE *)(UINTN) Address;
  ZeroMem ((VOID *)BdatStructure, sizeof (BDAT_STRUCTURE) + sizeof (BDAT_MEMORY_DATA_2_STRUCTURE));
  //
  // Fill BDAT_STRUCTURE
  //
  for (PtrIdx = 0; PtrIdx < 8; PtrIdx++) {
    BdatStructure->BdatHeader.BiosDataSignature[PtrIdx]  = BdatSignature[PtrIdx];
  }
  BdatStructure->BdatHeader.BiosDataStructSize = sizeof (BDAT_STRUCTURE);  
  BdatStructure->BdatHeader.PrimaryVersion     = 0x4;
  BdatStructure->BdatHeader.SecondaryVersion   = 0;
  BdatStructure->BdatHeader.OemOffset          = 0;
                 
  BdatStructure->BdatSchemas.SchemaListLength  = 1;
  Status  = gRT->GetTime (&EfiTime, NULL);
  if (!EFI_ERROR (Status)) {
    BdatStructure->BdatSchemas.Year   = EfiTime.Year;
    BdatStructure->BdatSchemas.Month  = EfiTime.Month;
    BdatStructure->BdatSchemas.Day    = EfiTime.Day;
    BdatStructure->BdatSchemas.Hour   = EfiTime.Hour;
    BdatStructure->BdatSchemas.Minute = EfiTime.Minute;
    BdatStructure->BdatSchemas.Second = EfiTime.Second;
  }
  BdatStructure->BdatSchemas.Schemas[0]        = ((UINT32) Address) + sizeof (BDAT_STRUCTURE);
  //
  // Update Crc16
  //
  BdatStructure->BdatHeader.Crc16              = Crc16 ((INT8 *) BdatStructure, sizeof (BDAT_STRUCTURE));
  //
  // Get data from BDAT_MEMORY_DATA_2_GUID's HOB and copy it to its final location
  //
  GuidHob = GetFirstGuidHob (&gEfiBdatMemoryData2Guid);
  ASSERT (GuidHob != NULL);
  HobData = GET_GUID_HOB_DATA (GuidHob);

  CopyMem ((VOID *)(((UINT32) Address) + sizeof (BDAT_STRUCTURE)), (VOID *) HobData, sizeof(BDAT_MEMORY_DATA_2_STRUCTURE));

  BdatMemoryData2Structure = (BDAT_MEMORY_DATA_2_STRUCTURE *) (((UINT32) Address) + sizeof (BDAT_STRUCTURE));
  //
  // Fill up the ACPI Table with the required information
  //
  BdatAcpiDescriptionTable = (EFI_BDAT_ACPI_DESCRIPTION_TABLE *) TableHeader;
  BdatAcpiDescriptionTable->BdatGas.Address = (UINT64) Address;
    //
  // Update checksum
  //
  BdatAcpiDescriptionTable->Header.Checksum = 0;
  Checksum = 0;
  for(PtrIdx = 0; PtrIdx < sizeof(EFI_BDAT_ACPI_DESCRIPTION_TABLE); PtrIdx++) {
    Checksum = Checksum + (UINT8) (((UINT8 *)(BdatAcpiDescriptionTable))[PtrIdx]);
  }
  BdatAcpiDescriptionTable->Header.Checksum = (UINT8) (0 - Checksum);

  DEBUG ((EFI_D_ERROR, "Address = 0x%08x\n", Address));
  DEBUG ((EFI_D_ERROR, "AddressBdatMemoryData2Structure = 0x%08x\n", BdatMemoryData2Structure));

  PrintBdatStructure (BdatStructure);
  PrintMemoryData2Structure (BdatMemoryData2Structure);

  return;
}

VOID
AcpiUpdateTable (
  IN OUT   EFI_ACPI_DESCRIPTION_HEADER  *TableHeader,
  IN OUT   EFI_ACPI_TABLE_VERSION       *Version
  )
/*++

  Routine Description:

    Set the correct table revision upon the setup value

  Arguments:

    Table   - The table to be set
    Version - Version to publish

  Returns:

    None

--*/

{
  if (TableHeader != NULL && Version != NULL) {
    // Denverton AptioV Override Start - EIP#221424
	// Version 1E is treated as invalid in AMI ACPI module
    // *Version = EFI_ACPI_TABLE_VERSION_1_0B | EFI_ACPI_TABLE_VERSION_2_0 | EFI_ACPI_TABLE_VERSION_3_0 | EFI_ACPI_TABLE_VERSION_4_0;
    *Version = EFI_ACPI_TABLE_VERSION_2_0;
	// Denverton AptioV Override End - EIP#221424
    //
    // Here we use all 3.0 signature because all version use same signature if they supported
    //
    switch (TableHeader->Signature) {
    //
    // "APIC" Multiple APIC Description Table
    //
    case EFI_ACPI_3_0_MULTIPLE_APIC_DESCRIPTION_TABLE_SIGNATURE:
      ApicTableUpdate (TableHeader, Version);
      break;
    //
    // "DSDT" Differentiated System Description Table
    //
    case EFI_ACPI_3_0_DIFFERENTIATED_SYSTEM_DESCRIPTION_TABLE_SIGNATURE:
      DsdtTableUpdate (TableHeader, Version);
      break;

    //
    // "FACP" Fixed ACPI Description Table
    //
    case EFI_ACPI_3_0_FIXED_ACPI_DESCRIPTION_TABLE_SIGNATURE:
      *Version = EFI_ACPI_TABLE_VERSION_NONE;
      if (TableHeader->Revision == EFI_ACPI_1_0_FIXED_ACPI_DESCRIPTION_TABLE_REVISION) {
      	*Version = EFI_ACPI_TABLE_VERSION_1_0B;
      } else if (TableHeader->Revision == EFI_ACPI_2_0_FIXED_ACPI_DESCRIPTION_TABLE_REVISION) {
      	*Version = EFI_ACPI_TABLE_VERSION_2_0;
      } else if (TableHeader->Revision == EFI_ACPI_3_0_FIXED_ACPI_DESCRIPTION_TABLE_REVISION) {
      	*Version = EFI_ACPI_TABLE_VERSION_3_0;
      } else if (TableHeader->Revision == EFI_ACPI_4_0_FIXED_ACPI_DESCRIPTION_TABLE_REVISION) {
      	*Version = EFI_ACPI_TABLE_VERSION_4_0;
      }
      break;
    //
    // "FACS" Firmware ACPI Control Structure
    //
    case EFI_ACPI_3_0_FIRMWARE_ACPI_CONTROL_STRUCTURE_SIGNATURE:
      break;
    //
    // "SSDT" Secondary System Description Table
    //
    case EFI_ACPI_3_0_SECONDARY_SYSTEM_DESCRIPTION_TABLE_SIGNATURE:
    break;
    //
    // "HPET" IA-PC High Precision Event Timer Table
    //
    case EFI_ACPI_3_0_HIGH_PRECISION_EVENT_TIMER_TABLE_SIGNATURE:
      UpdateAcpiTableIds (TableHeader);
      //
      // If HPET is disabled in setup, don't publish the table.
      //
      if (mGlobalNvsArea.Area->HpetEnable == 0) {
        *Version = EFI_ACPI_TABLE_VERSION_NONE;
      }
    break;
    //
    // "SPCR" Serial Port Console Redirection Table
    //
    case EFI_ACPI_3_0_SERIAL_PORT_CONSOLE_REDIRECTION_TABLE_SIGNATURE:
      break;
    //
    // BDAT  Table
    //

    case EFI_BDAT_TABLE_SIGNATURE:
      BdatTableUpdate (TableHeader, Version);
      break;
    //
    // "MCFG" PCI Express Memory Mapped Configuration Space Base Address Description Table
    //
    case EFI_ACPI_3_0_PCI_EXPRESS_MEMORY_MAPPED_CONFIGURATION_SPACE_BASE_ADDRESS_DESCRIPTION_TABLE_SIGNATURE:
      UpdateAcpiTableIds (TableHeader);
      break;
    // Lakeport platform doesn't support the following table
    /*
      //
    // "ECDT" Embedded Controller Boot Resources Table
        //
    case EFI_ACPI_3_0_EMBEDDED_CONTROLLER_BOOT_RESOURCES_TABLE_SIGNATURE:
      break;
        //
    // "PSDT" Persistent System Description Table
          //
    case EFI_ACPI_3_0_PERSISTENT_SYSTEM_DESCRIPTION_TABLE_SIGNATURE:
      break;
          //
    // "SBST" Smart Battery Specification Table
    //
    case EFI_ACPI_3_0_SMART_BATTERY_SPECIFICATION_TABLE_SIGNATURE:
          break;
    //
    // "SLIT" System Locality Information Table
    //
    case EFI_ACPI_3_0_SYSTEM_LOCALITY_INFORMATION_TABLE_SIGNATURE:
          break;
    //
    // "SRAT" Static Resource Affinity Table
    //
    case EFI_ACPI_3_0_STATIC_RESOURCE_AFFINITY_TABLE_SIGNATURE:
    break;
  //
    // "XSDT" Extended System Description Table
  //
    case EFI_ACPI_3_0_EXTENDED_SYSTEM_DESCRIPTION_TABLE_SIGNATURE:
      break;
  //
    // "BOOT" MS Simple Boot Spec
  //
    case EFI_ACPI_3_0_SIMPLE_BOOT_FLAG_TABLE_SIGNATURE:
      break;
  //
    // "CPEP" Corrected Platform Error Polling Table
  //
    case EFI_ACPI_3_0_CORRECTED_PLATFORM_ERROR_POLLING_TABLE_SIGNATURE:
      break;
  //
    // "DBGP" MS Debug Port Spec
  //
    case EFI_ACPI_3_0_DEBUG_PORT_TABLE_SIGNATURE:
      break;
    //
    // "ETDT" Event Timer Description Table
    //
    case EFI_ACPI_3_0_EVENT_TIMER_DESCRIPTION_TABLE_SIGNATURE:
      break;
    //
    // "SPMI" Server Platform Management Interface Table
    //
    case EFI_ACPI_3_0_SERVER_PLATFORM_MANAGEMENT_INTERFACE_TABLE_SIGNATURE:
      break;
    //
    // "TCPA" Trusted Computing Platform Alliance Capabilities Table
    //
    case EFI_ACPI_3_0_TRUSTED_COMPUTING_PLATFORM_ALLIANCE_CAPABILITIES_TABLE_SIGNATURE:
      break;
    */
    default:
      break;
    }
  }
}


EFI_STATUS
AcpiPlatformHooksIsActiveTable (
  IN OUT EFI_ACPI_COMMON_HEADER     *Table
  )
/*++

Routine Description:

  Called for every ACPI table found in the BIOS flash.
  Returns whether a table is active or not. Inactive tables
  are not published in the ACPI table list. This hook can be
  used to implement optional SSDT tables or enabling/disabling
  specific functionality (e.g. SPCR table) based on a setup
  switch or platform preference. In case of optional SSDT tables,
  the platform flash will include all the SSDT tables but will
  return EFI_SUCCESS only for those tables that need to be
  published.

Arguments:

  *Table - Pointer to the active table

Returns:

  Status  - EFI_SUCCESS if the table is active
  Status  - EFI_UNSUPPORTED if the table is not active
--*/
{
  EFI_ACPI_DESCRIPTION_HEADER     *TableHeader;

  if(Table == NULL) {
	DEBUG(( EFI_D_ERROR, "Pointer to table is NULL\n"));
	return EFI_ABORTED;
  }

  TableHeader   = (EFI_ACPI_DESCRIPTION_HEADER *) Table;
  if (TableHeader->Signature == EFI_ACPI_3_0_DIFFERENTIATED_SYSTEM_DESCRIPTION_TABLE_SIGNATURE) {
    // Check platform type here to install right DSDT table!
    DEBUG(( EFI_D_INFO, "AcpiPlatform Found DSDT table. \n"));
    return EFI_SUCCESS;
  } else if (TableHeader->Signature  ==  EFI_ACPI_2_0_SERIAL_PORT_CONSOLE_REDIRECTION_TABLE_SIGNATURE) {
    //
    // Spcr table not ready now, will update it before booting
    //
    mSpcrTable = (EFI_ACPI_SERIAL_PORT_CONSOLE_REDIRECTION_TABLE *) Table;
    return EFI_NOT_READY;
  }
  return EFI_SUCCESS;
}


BOOLEAN
IsTerminalDevicePath (
  IN  EFI_DEVICE_PATH_PROTOCOL *DevicePath,
  OUT UINT8                    *TerminalType
  )
/*++

Routine Description:
  Test whether DevicePath is a valid Terminal.

Arguments:
  DevicePath  -   DevicePath to be checked
  TerminalType-   If is terminal, give its type

Returns:
  TRUE        -   If DevicePath point to a Terminal
  FALSE

--*/
{
  UINT8                 *Ptr;
  VENDOR_DEVICE_PATH    *Vendor;

  Ptr = (UINT8 *) DevicePath;
  Ptr    = Ptr + GetDevicePathSize (DevicePath) -
           sizeof (VENDOR_DEVICE_PATH) - sizeof (EFI_DEVICE_PATH_PROTOCOL);
  Vendor = (VENDOR_DEVICE_PATH *) Ptr;

  if (Vendor->Header.Type == MESSAGING_DEVICE_PATH &&
      Vendor->Header.SubType == MSG_VENDOR_DP &&
      DevicePathNodeLength (&Vendor->Header) == sizeof(VENDOR_DEVICE_PATH)) {
    //
    // There are four kinds of Terminal types
    // check to see whether this devicepath
    // is one of that type
    //
    if (CompareGuid (&Vendor->Guid, &gEfiVT100Guid)) {
      *TerminalType = 0;
    } else if (CompareGuid (&Vendor->Guid, &gEfiVT100PlusGuid)) {
      *TerminalType = 1;
    } else if (CompareGuid (&Vendor->Guid, &gEfiVTUTF8Guid)) {
      *TerminalType = 2;
    } else if (CompareGuid (&Vendor->Guid, &gEfiPcAnsiGuid)) {
      *TerminalType = 3;
    } else {
      return FALSE;
    }

    return TRUE;
  }
  return FALSE;
}

/* // Denverton Aptiov override start
// DBG2 ACPI table is installed by "Debug Port Tables" module

BOOLEAN
InstallDbg2Table (
  IN  UINT8                                 OsDebugPort
  )
/*++

Routine Description:

  This function installs the DBG2 ACPI Table

Arguments:
  None

Returns:
  TRUE - DBG2 Table is updated and valid
--*/
/*
{
  UINTN                                         AcpiTableKey;
  EFI_STATUS                                    Status;
  EFI_ACPI_DEBUG_PORT_2_DESCRIPTION_TABLE       *DebugPortTable2;
  EFI_ACPI_DBG2_DEBUG_DEVICE_INFORMATION_STRUCT *Dbg2DeviceTablePtr;
  EFI_ACPI_5_0_GENERIC_ADDRESS_STRUCTURE        *Dbg2DeviceGasPtr;
  EFI_ACPI_TABLE_PROTOCOL      					*AcpiTableProtocol;
  UINT32                                        *Dbg2DeviceAdrSizePtr;
  UINT8                                         *Dbg2DeviceNameSpacePtr;
  UINT8                                         *Dbg2DeviceNameSpaceStr;
  UINT16                                        Dbg2DeviceNameSpaceStrSize;
  UINT32                                        DebugPortTable2Size;
  UINT32                                        Dbg2DevNum;
  UINT16                                        DebugDevInfoStructSize;
  UINT8                                         Checksum;
  UINT32                                        PtrIdx;

  AcpiTableKey      = 0;

  //
  // Locate ACPI support protocol
  //
  Status = gBS->LocateProtocol (&gEfiAcpiTableProtocolGuid, NULL, (VOID**)&AcpiTableProtocol);
  if (EFI_ERROR (Status)) {
    return FALSE;
  }

  //
  // Initialize the number of device for DBG2
  //
  Dbg2DevNum = 1;

  //
  // Initialize the DBG2 debug device's name space string and size
  //
  Dbg2DeviceNameSpaceStr = ACPI_DBG2_DEFAULT_NAME_SPACE;
  if (OsDebugPort == 0) {
    Dbg2DeviceNameSpaceStr = ACPI_DBG2_SERIALIO_UART0_NAME_SPACE;
  } else if (OsDebugPort == 1) {
    Dbg2DeviceNameSpaceStr = ACPI_DBG2_SERIALIO_UART1_NAME_SPACE;
  } else if (OsDebugPort == 2) {
    Dbg2DeviceNameSpaceStr = ACPI_DBG2_SERIALIO_UART2_NAME_SPACE;
  } else {
	  Dbg2DeviceNameSpaceStr = ACPI_DBG2_SERIALIO_UART0_NAME_SPACE;
  }

  Dbg2DeviceNameSpaceStrSize = (UINT16) AsciiStrSize(Dbg2DeviceNameSpaceStr);

  //
  // Initialize the DBG2 total size
  //
  DebugDevInfoStructSize = (UINT16)((sizeof(EFI_ACPI_DBG2_DEBUG_DEVICE_INFORMATION_STRUCT) +        // Debug Device Information Structure
                                     sizeof(EFI_ACPI_5_0_GENERIC_ADDRESS_STRUCTURE) +               // Base Address Register - Assume 1 BAR
                                     4 +                                                            // Address Size
                                     Dbg2DeviceNameSpaceStrSize +                                   // Name Space String
                                     0) *                                                           // Oem Data - Assume none
                                    Dbg2DevNum);
  DebugPortTable2Size = sizeof(EFI_ACPI_DEBUG_PORT_2_DESCRIPTION_TABLE) + DebugDevInfoStructSize;   // DBG2 table size

  //
  // Allocate memory for DBG2
  //
  DebugPortTable2 = AllocatePool (DebugPortTable2Size);
  ASSERT (DebugPortTable2 != NULL);

  if (DebugPortTable2 == NULL) return FALSE;

  //
  // Populate data for DBG2
  //
  DebugPortTable2->Header.Signature       = EFI_ACPI_5_0_DEBUG_PORT_2_TABLE_SIGNATURE;
  DebugPortTable2->Header.Length          = DebugPortTable2Size;
  DebugPortTable2->Header.Revision        = EFI_ACPI_DEBUG_PORT_2_TABLE_REVISION;
  DebugPortTable2->Header.Checksum        = 0x00;
  CopyMem (DebugPortTable2->Header.OemId, EFI_ACPI_OEM_ID, 6);
  DebugPortTable2->Header.OemTableId      = 0x00;
  DebugPortTable2->Header.OemRevision     = EFI_ACPI_OEM_DEBUG_PORT_2_TABLE_REVISION;
  DebugPortTable2->Header.CreatorId       = EFI_ACPI_CREATOR_ID;
  DebugPortTable2->Header.CreatorRevision = EFI_ACPI_CREATOR_REVISION;
  DebugPortTable2->OffsetDbgDeviceInfo    = sizeof(EFI_ACPI_DEBUG_PORT_2_DESCRIPTION_TABLE);
  DebugPortTable2->NumberDbgDeviceInfo    = Dbg2DevNum;

  //
  // Set pointer to DBG2 Device Info Structure Offset
  //
  Dbg2DeviceTablePtr = (EFI_ACPI_DBG2_DEBUG_DEVICE_INFORMATION_STRUCT *)((UINT8 *)DebugPortTable2 + DebugPortTable2->OffsetDbgDeviceInfo);

  //
  // Populate DBG2 Device Info Structure
  //
  Dbg2DeviceTablePtr[0].Revision                        = 0x00;
  Dbg2DeviceTablePtr[0].Length                          = DebugDevInfoStructSize;
  Dbg2DeviceTablePtr[0].NumberofGenericAddressRegisters = 1;
  Dbg2DeviceTablePtr[0].NameSpaceStringLength           = Dbg2DeviceNameSpaceStrSize;
  Dbg2DeviceTablePtr[0].NameSpaceStringOffset           = sizeof(EFI_ACPI_DBG2_DEBUG_DEVICE_INFORMATION_STRUCT) + sizeof(EFI_ACPI_5_0_GENERIC_ADDRESS_STRUCTURE) + 4;
  Dbg2DeviceTablePtr[0].OemDataLength                   = 0;
  Dbg2DeviceTablePtr[0].OemDataOffset                   = 0;
  Dbg2DeviceTablePtr[0].PortType                        = EFI_ACPI_DBG2_PORT_TYPE_SERIAL;
  Dbg2DeviceTablePtr[0].PortSubtype                     = EFI_ACPI_DBG2_PORT_SUBTYPE_SERIAL_FULL_16550;
  Dbg2DeviceTablePtr[0].Reserved[0]                     = 0;
  Dbg2DeviceTablePtr[0].Reserved[1]                     = 0;
  Dbg2DeviceTablePtr[0].BaseAddressRegisterOffset       = sizeof(EFI_ACPI_DBG2_DEBUG_DEVICE_INFORMATION_STRUCT);
  Dbg2DeviceTablePtr[0].AddressSizeOffset               = sizeof(EFI_ACPI_DBG2_DEBUG_DEVICE_INFORMATION_STRUCT) + sizeof(EFI_ACPI_5_0_GENERIC_ADDRESS_STRUCTURE);

  //
  // Set pointer to DBG2 Device Base Address Offset
  //
  Dbg2DeviceGasPtr = (EFI_ACPI_5_0_GENERIC_ADDRESS_STRUCTURE *)((UINT8 *)Dbg2DeviceTablePtr + Dbg2DeviceTablePtr[0].BaseAddressRegisterOffset);

  //
  // Set pointer to DBG2 Device Address Size Offset and populate value
  //
  Dbg2DeviceAdrSizePtr = (UINT32 *)((UINT8 *)Dbg2DeviceTablePtr + Dbg2DeviceTablePtr[0].AddressSizeOffset);
  Dbg2DeviceAdrSizePtr[0] = 32;

  //
  // Set pointer to DBG2 Device Name Space Offset and populate value
  //
  Dbg2DeviceNameSpacePtr = (UINT8 *)((UINT8 *)Dbg2DeviceTablePtr + Dbg2DeviceTablePtr[0].NameSpaceStringOffset);
  CopyMem (Dbg2DeviceNameSpacePtr, Dbg2DeviceNameSpaceStr, Dbg2DeviceNameSpaceStrSize);

  //
  // Check which port is used as Debug Port based on policy
  // case 0 : Legacy UART
  // case 1 : Serial IO UART 0
  // case 2 : Serial IO UART 1
  // case 3 : Serial IO UART 2
  //
  //
  // Update DBG2 Device Base Address
  //
  Dbg2DeviceGasPtr[0].AddressSpaceId = EFI_ACPI_5_0_SYSTEM_IO;
  Dbg2DeviceGasPtr[0].RegisterBitWidth = 8;
  Dbg2DeviceGasPtr[0].RegisterBitOffset = 0;
  Dbg2DeviceGasPtr[0].AccessSize = EFI_ACPI_5_0_BYTE;
  Dbg2DeviceGasPtr[0].Address = (UINT64)(FindSerialIoBar(OsDebugPort));

  //
  // Update checksum
  //
  DebugPortTable2->Header.Checksum = 0;
  Checksum = 0;
  for(PtrIdx = 0; PtrIdx < DebugPortTable2Size; PtrIdx++) {
    Checksum += (UINT8) (((UINT8 *)(DebugPortTable2))[PtrIdx]);
  }
  DebugPortTable2->Header.Checksum = (UINT8)(0-Checksum);

  Status = AcpiTableProtocol->InstallAcpiTable (AcpiTableProtocol, DebugPortTable2, DebugPortTable2->Header.Length, &AcpiTableKey);
  ASSERT_EFI_ERROR (Status);

  FreePool (DebugPortTable2);

  DEBUG((EFI_D_INFO, "DBG2 Table Installed\n"));

  return TRUE;

}
*/ //Denverton Aptiov override End

// Denverton Aptiov override start - EIP#235962
// SPCR table is being published by AMI`s terminal driver
/*
BOOLEAN
UpdateSpcrTable (
  )
/*++

Routine Description:

  This function updates SPCR table.

  Note that user may change CR settings via setup or other methods.
  The SPCR table must match.

Arguments:

  None

Returns:

  TRUE - SPCR Table updated and is valid.

--
{
  EFI_STATUS Status;
  UINT8      BaudRate;
  UINT8      TerminalType;
  UINT32     Control;
  UINTN      Com;
  UINTN      Size;
  EFI_SERIAL_IO_PROTOCOL   *SerialIo;
  EFI_DEVICE_PATH_PROTOCOL *DevicePath;
  EFI_DEVICE_PATH_PROTOCOL *TempDevicePath;
  EFI_DEVICE_PATH_PROTOCOL *DevicePathInstance;
  EFI_HANDLE               Handle;
  EFI_LPC_POLICY_PROTOCOL  *LpcPolicy;

  if (mSpcrTable == NULL) {
    return FALSE;
  }
  DEBUG(( EFI_D_ERROR, "SPCR: Update SPCR table!\n" ));

  Status = LocateSupportProtocol (&gEfiLpcPolicyProtocolGuid, &LpcPolicy, 0);
  ASSERT_EFI_ERROR (Status);

  if (LpcPolicy->PolicyCom.Enable[0] != TRUE) {
    return FALSE;
  }
  mSpcrTable->BaseAddress.Address = LpcPolicy->PolicyCom.Address[0];
  mSpcrTable->Irq = (UINT8) LpcPolicy->PolicyCom.Irq[0];
  mSpcrTable->GlobalSystemInterrupt = mSpcrTable->Irq;

  //
  // Update OEM ID
  //
  UpdateAcpiTableIds (&mSpcrTable->Header);

  //
  // Locate the COM1 terminal
  //

  DevicePath = GetVariable (
              L"ConIn",
              &gEfiGlobalVariableGuid
              );
  if (DevicePath == NULL) {
    return FALSE;
  }

  TerminalType = 0;
  TempDevicePath = DevicePath;
  do {
    DevicePathInstance = GetNextDevicePathInstance (&TempDevicePath, &Size);
    if (DevicePathInstance == NULL) {
      break;
    }

    if (IsTerminalDevicePath (DevicePathInstance, &TerminalType)) {
      break;
    }
    gBS->FreePool (DevicePathInstance);
    DevicePathInstance = NULL;
  } while (TempDevicePath != NULL);

  if (DevicePathInstance == NULL) {
    gBS->FreePool (DevicePath);
    return FALSE;
  }

  TempDevicePath = DevicePathInstance;
  Status = gBS->LocateDevicePath (
                  &gEfiSerialIoProtocolGuid,
                  &TempDevicePath,
                  &Handle
                  );
  gBS->FreePool (DevicePathInstance);
  if (EFI_ERROR (Status)) {
    return FALSE;
  }

  Status = gBS->HandleProtocol (
                  Handle,
                  &gEfiSerialIoProtocolGuid,
                  (VOID **) &SerialIo
                  );
  ASSERT_EFI_ERROR (Status);

  switch (SerialIo->Mode->BaudRate) {
  case 9600:
    BaudRate = 3;
    break;
  case 19200:
    BaudRate = 4;
    break;
  case 57600:
    BaudRate = 6;
    break;
  case 115200:
    BaudRate = 7;
    break;
  default:
    DEBUG(( EFI_D_ERROR, "\nSPCR: Unsupported baud rate!\n" ));
    goto UpdateSpcrExit;
    break;
  }
  mSpcrTable->BaudRate = BaudRate;
  if (SerialIo->Mode->Parity != NoParity) {
    DEBUG(( EFI_D_ERROR, "\nSPCR: Only support no parity!\n" ));
    goto UpdateSpcrExit;
  }
  if (SerialIo->Mode->DataBits != 8) {
    DEBUG(( EFI_D_ERROR, "\nSPCR: Only support 8 data bits!\n" ));
    goto UpdateSpcrExit;
  }
  if (SerialIo->Mode->StopBits != OneStopBit) {
    DEBUG(( EFI_D_ERROR, "\nSPCR: Only support 1 stop bit!\n" ));
    goto UpdateSpcrExit;
  }
  Status = SerialIo->GetControl (
                        SerialIo,
                        &Control
                        );
  ASSERT_EFI_ERROR (Status);

  if (Control & EFI_SERIAL_HARDWARE_FLOW_CONTROL_ENABLE) {
    mSpcrTable->FlowControl |= 2;
  }
  mSpcrTable->TerminalType = TerminalType;
  return TRUE;

UpdateSpcrExit:
  return FALSE;
}
*/
// Denverton Aptiov override end - EIP#235962
STATIC
VOID
EFIAPI
OnReadyToBoot (
  IN      EFI_EVENT                 Event,
  IN      VOID                      *Context
  )
{
  EFI_STATUS                    Status;
  // Denverton Aptiov override start - EIP#235962
  // UINTN                         TableKey;
  // EFI_ACPI_TABLE_PROTOCOL       *AcpiTableProtocol;
  // Denverton Aptiov override end - EIP#235962
  UINTN                         VariableSize;
  EFI_PEI_HOB_POINTERS          HobPtr;
  PCH_POLICY_HOB                *mPchPolicyHob;
  UINT8 Index;

  //
  // Update the ACPI parameter blocks finally
  //
  VariableSize = sizeof (SYSTEM_CONFIGURATION);
  // Denverton Aptiov override Start - EIP#217439
  Status = gRT->GetVariable (
                            L"IntelSetup",
                            &mSystemConfigurationGuid,
                            NULL,
                            &VariableSize,
                            &mSystemConfiguration
                            );
  // Denverton Aptiov override Start - EIP#217439
  ASSERT_EFI_ERROR (Status);
  //
  // Set the mGlobalNvsArea structure of variables used in the DSDT ASL code.
  //
  //mGlobalNvsArea.Area->TpmEnabled = mSystemConfiguration.TpmEnable;    // Denverton AptioV override - EIP#288065
  
  // Denverton AptioV Override Start - EIP#288690
  mGlobalNvsArea.Area->MMIOB = MmioRead32(MmPciBase(PCI_BUS_NUMBER_SA, PCI_DEVICE_NUMBER_SA, PCI_FUNCTION_NUMBER_SA) + R_SA_TOLUD) & B_SA_TOLUD_TOLUD_MASK;
  mGlobalNvsArea.Area->MMIOL = (MmioRead32(MmPciBase(PCI_BUS_NUMBER_SA, PCI_DEVICE_NUMBER_SA, PCI_FUNCTION_NUMBER_SA) + R_SA_PCIEXBAR) & B_SA_PCIEXBAR_PCIEXBAR_MASK) - 1;

  mGlobalNvsArea.Area->MMIHB = MmioRead32(MmPciBase(PCI_BUS_NUMBER_SA, PCI_DEVICE_NUMBER_SA, PCI_FUNCTION_NUMBER_SA) + R_SA_TOUUD_HI) & B_SA_TOUUD_HI_TOUUD_MASK;
  mGlobalNvsArea.Area->MMIHB = (MmioRead32(MmPciBase(PCI_BUS_NUMBER_SA, PCI_DEVICE_NUMBER_SA, PCI_FUNCTION_NUMBER_SA) + R_SA_TOUUD_LO) & B_SA_TOUUD_LO_TOUUD_MASK) + LShiftU64(mGlobalNvsArea.Area->MMIHB, 32);
  mGlobalNvsArea.Area->MMIHL = RES_MEM64_39_LIMIT;	
  
  mGlobalNvsArea.Area->TsegB = MmioRead32(MmPciBase(PCI_BUS_NUMBER_SA, PCI_DEVICE_NUMBER_SA, PCI_FUNCTION_NUMBER_SA) + R_SA_TSEG) & B_SA_TSEGMB_TSEGMB_MASK;
  mGlobalNvsArea.Area->TsegL = (UINT32)(mGlobalNvsArea.Area->MMIOB - mGlobalNvsArea.Area->TsegB);
  // Denverton AptioV Override End - EIP#288690

  if (!PcdGetBool(PcdUefiOptimizedBoot)) {
    mGlobalNvsArea.Area->Legacy = LEGACY_BOOT;
  } else {
    mGlobalNvsArea.Area->Legacy = 0;
  }
  // Denverton Aptiov override start - EIP#235962
  // SPCR table is being published by AMI`s terminal driver
  /*
  //
  // Find the AcpiSupport protocol
  //
  Status = LocateSupportProtocol (&gEfiAcpiTableProtocolGuid, &AcpiTableProtocol, 0);
  ASSERT_EFI_ERROR (Status);

  if (UpdateSpcrTable ()) {
    //
    // Add SPCR table
    //
    TableKey = 0;
    Status = AcpiTableProtocol->InstallAcpiTable (
                                             AcpiTableProtocol,
                                             mSpcrTable,
                                             mSpcrTable->Header.Length,
                                             &TableKey
                                             );
    ASSERT_EFI_ERROR (Status);
  }

  if (mSpcrTable != NULL) {
    gBS->FreePool (mSpcrTable);
  }
  */
  // Denverton Aptiov override end - EIP#235962
  

  //Install DBG2 Table

    //
    // Get PCH Policy HOB.
    //
  HobPtr.Guid   = GetFirstGuidHob (&gPchPolicyHobGuid);
  ASSERT (HobPtr.Guid != NULL);
  mPchPolicyHob = GET_GUID_HOB_DATA (HobPtr.Guid);

  mGlobalNvsArea.Area->GpioStatus = mPchPolicyHob->GpioStatus ? 0xF:0;
  
/* // Denverton Aptiov override start
// DBG2 ACPI table is installed by "Debug Port Tables" module

  if(mPchPolicyHob->HsUartMdpt == TRUE)
  {
  DEBUG((EFI_D_INFO, "Selected HSUART port for KD Debug is:%x\n", mPchPolicyHob->HsUartKDPort));

  Status = InstallDbg2Table (mPchPolicyHob->HsUartKDPort);
  ASSERT_EFI_ERROR (Status);
  }
*/ // Denverton Aptiov override End
  //
  //Configure _STA of Uart devices
  //

  //Set default Status in disabled
  mGlobalNvsArea.Area->HsUartStatus1 = 0x0;
  mGlobalNvsArea.Area->HsUartStatus2 = 0x0;
  mGlobalNvsArea.Area->HsUartStatus3 = 0x0;
  mGlobalNvsArea.Area->HsUartStatus4 = 0x0;

  for(Index = 0; Index < PCH_MAX_HSUART; Index++)
  {
	  //Enable ACPI devices that are legacy
	  if(mPchPolicyHob->HsUartConfig[Index].HsUartMode == HsUartModeLegacy)
	  {
		  switch(mPchPolicyHob->HsUartConfig[Index].HsUartCOM)
		  {
			  case HSUART_COM1_ADDRESSS:
				  mGlobalNvsArea.Area->HsUartStatus1 = 0x000F;
				  break;
			  case HSUART_COM2_ADDRESSS:
				  mGlobalNvsArea.Area->HsUartStatus2 = 0x000F;
				  break;
			  case HSUART_COM3_ADDRESSS:
				  mGlobalNvsArea.Area->HsUartStatus3 = 0x000F;
				  break;
			  case HSUART_COM4_ADDRESSS:
				  mGlobalNvsArea.Area->HsUartStatus4 = 0x000F;
				  break;
		  }
	  }
  }



  gBS->CloseEvent (Event);
}

/**
  Get HPET base address.
  This function will be reveal and hide P2SB once we get value of HPTC register

  @retval EFI_INVALID_PARAMETER         Invalid offset passed.
**/
UINT8
GetHpetBaseAddress (
  )
{
  UINTN     P2sbBase 		= 0;
  BOOLEAN	P2sbOrgStatus	= 0;
  UINT32 	RegHptc			= 0;

  P2sbBase      = MmPciBase (
                        DEFAULT_PCI_BUS_NUMBER_PCH,
						PCI_DEVICE_NUMBER_PCH_P2SB,
						PCI_FUNCTION_NUMBER_PCH_P2SB
                        );

  PchRevealP2sb (P2sbBase, &P2sbOrgStatus);
  RegHptc = MmioRead32(P2sbBase + R_PCH_P2SB_HPTC);

  if (!P2sbOrgStatus) {
      PchHideP2sb (P2sbBase);
  }

  return (RegHptc & 3);
}

//
// Description:
//    Entrypoint of Acpi Platform driver
// In:
//    ImageHandle
//    SystemTable
// Out:
//    EFI_SUCCESS
//    EFI_LOAD_ERROR
//    EFI_OUT_OF_RESOURCES
//

EFI_STATUS
AcpiPlatformEntryPoint (
  IN EFI_HANDLE          ImageHandle,
  IN EFI_SYSTEM_TABLE    *SystemTable
  )
{
  EFI_STATUS                    Status;
  EFI_STATUS                    AcpiStatus;
  EFI_ACPI_TABLE_PROTOCOL      *AcpiTableProtocol;
  EFI_FIRMWARE_VOLUME2_PROTOCOL *FwVol;
  INTN                          Instance;
  EFI_ACPI_COMMON_HEADER        *CurrentTable;
  UINTN                         TableKey;
  UINT32                        FvStatus;
  UINTN                         Size;
  EFI_ACPI_TABLE_VERSION        Version;
  EFI_HANDLE                    Handle;
//  UINTN                         Index;
//  PCI_DEVICE_INFO               *PciDeviceInfo;
//  EFI_ACPI_HANDLE               PciRootHandle;
//  BOOLEAN                       UpdatePRT;
//  BOOLEAN                       UpdatePRW;
  PLATFORM_INFO_HOB             *PlatformInfoHobData = NULL;
  PCI_DEVICE_SETTING            *mConfigData;
  EFI_EVENT                     Event;
  EFI_HOB_GUID_TYPE             *GuidHob = NULL;
  EFI_MP_SERVICES_PROTOCOL      *MpService = NULL;
  UINTN                         CpuCount;    
  UINTN                         NumberOfEnabledCPUs;
  UINT32                        Signature;
  UINTN                         TableNumber;

  //
  // Find the MpService Protocol
  //
  Status = gBS->LocateProtocol (&gEfiMpServiceProtocolGuid,
                                NULL, 
                                &MpService
                               );
  ASSERT_EFI_ERROR (Status);

  //
  // Get processor count from MP service.
  //
  Status = MpService->GetNumberOfProcessors (MpService, &CpuCount, &NumberOfEnabledCPUs);
  ASSERT_EFI_ERROR (Status);


  Instance     = 0;
  TableKey     = 0;
  CurrentTable = NULL;
  mConfigData  = NULL;

  //
  // Initialize the EFI Driver Library
  //

  //ASSERT (sizeof (EFI_GLOBAL_NVS_AREA) == 512);


  Status = gBS->AllocatePool (
                   EfiACPIMemoryNVS,
                   sizeof (EFI_GLOBAL_NVS_AREA),
                   (VOID**)&mGlobalNvsArea.Area
                   );

  ASSERT_EFI_ERROR (Status);

  Handle = NULL;
  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gEfiGlobalNvsAreaProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mGlobalNvsArea
                  );

  ASSERT_EFI_ERROR (Status);
  if (!EFI_ERROR (Status)) {
    SetMem (
      mGlobalNvsArea.Area,
      sizeof (EFI_GLOBAL_NVS_AREA),
      0
      );

  }

  //
  // Initialize the data.  Eventually, this will be controlled by setup options.
  //
  mGlobalNvsArea.Area->HpetEnable 		= PcdGetBool (PcdHpetEnable);
  mGlobalNvsArea.Area->NCpu 			= (UINT8)NumberOfEnabledCPUs;
  mGlobalNvsArea.Area->HpetBaseAddress 	= GetHpetBaseAddress();

  GuidHob       = GetFirstGuidHob (&gPlatformInfoHobGuid);
  if(GuidHob == NULL) {
	  DEBUG ((EFI_D_ERROR, "GuidHob is null\n"));
	  return EFI_NOT_FOUND;
  }

  PlatformInfoHobData  = GET_GUID_HOB_DATA (GuidHob);
  if(PlatformInfoHobData == NULL) {
	  DEBUG ((EFI_D_ERROR, "PlatformInfoHobGuid not found\n"));
	  return EFI_NOT_FOUND;
  }
  
  //
  // CPLD device is implemented but left as disabled because there's no driver for it on Windows or Linux.
  //
  mGlobalNvsArea.Area->CpldStatus = 0x00;

  switch (PlatformInfoHobData->BoardId) {
   case BOARD_ID_OstrichBay:
   case BOARD_ID_OstrichBayS:
    mGlobalNvsArea.Area->Usb3Port0Connectable = 0xFF;
    mGlobalNvsArea.Area->Usb3Port0Type = 0x03;
    mGlobalNvsArea.Area->Usb3Port0Visible = 1;
    mGlobalNvsArea.Area->Usb3Port0Group   = 1;

    mGlobalNvsArea.Area->Usb3Port1Connectable = 0xFF;
    mGlobalNvsArea.Area->Usb3Port1Type = 0x03;
    mGlobalNvsArea.Area->Usb3Port1Visible = 1;
    mGlobalNvsArea.Area->Usb3Port1Group   = 2;

    mGlobalNvsArea.Area->Usb3Port2Connectable = 0xFF;
    mGlobalNvsArea.Area->Usb3Port2Type = 0x03;
    mGlobalNvsArea.Area->Usb3Port2Visible = 1;
    mGlobalNvsArea.Area->Usb3Port2Group   = 3;

    mGlobalNvsArea.Area->Usb3Port3Connectable = 0xFF;
    mGlobalNvsArea.Area->Usb3Port3Type = 0x03;
    mGlobalNvsArea.Area->Usb3Port3Visible = 1;
    mGlobalNvsArea.Area->Usb3Port3Group   = 4;

    mGlobalNvsArea.Area->Usb2Port0Connectable = 0xFF;
    mGlobalNvsArea.Area->Usb2Port0Type = 0x03;
    mGlobalNvsArea.Area->Usb2Port0Visible = 1;
    mGlobalNvsArea.Area->Usb2Port0Group   = 5;

    mGlobalNvsArea.Area->Usb2Port1Connectable = 0xFF;
    mGlobalNvsArea.Area->Usb2Port1Type = 0x03;
    mGlobalNvsArea.Area->Usb2Port1Visible = 1;
    mGlobalNvsArea.Area->Usb2Port1Group   = 6;

    mGlobalNvsArea.Area->Usb2Port2Connectable = 0xFF;
    mGlobalNvsArea.Area->Usb2Port2Type = 0x03;
    mGlobalNvsArea.Area->Usb2Port2Visible = 1;
    mGlobalNvsArea.Area->Usb2Port2Group   = 7;

    mGlobalNvsArea.Area->Usb2Port3Connectable = 0xFF;
    mGlobalNvsArea.Area->Usb2Port3Type = 0x03;
    mGlobalNvsArea.Area->Usb2Port3Visible = 1;
    mGlobalNvsArea.Area->Usb2Port3Group   = 8;
    break;

   case BOARD_ID_AspenCove:
    mGlobalNvsArea.Area->Usb3Port0Connectable = 0xFF;
    mGlobalNvsArea.Area->Usb3Port0Type = 0x03;
    mGlobalNvsArea.Area->Usb3Port0Visible = 1;
    mGlobalNvsArea.Area->Usb3Port0Group   = 1;

    mGlobalNvsArea.Area->Usb3Port1Connectable = 0;
    mGlobalNvsArea.Area->Usb3Port1Type = 0xFF;
    mGlobalNvsArea.Area->Usb3Port0Visible = 0;
    mGlobalNvsArea.Area->Usb3Port0Group   = 2;

    mGlobalNvsArea.Area->Usb3Port2Connectable = 0;
    mGlobalNvsArea.Area->Usb3Port2Type = 0xFF;
    mGlobalNvsArea.Area->Usb3Port2Visible = 0;
    mGlobalNvsArea.Area->Usb3Port2Group   = 3;

    mGlobalNvsArea.Area->Usb3Port3Connectable = 0x0;
    mGlobalNvsArea.Area->Usb3Port3Type = 0xFF;
    mGlobalNvsArea.Area->Usb3Port3Visible = 0;
    mGlobalNvsArea.Area->Usb3Port3Group   = 4;

    mGlobalNvsArea.Area->Usb2Port0Connectable = 0xFF;
    mGlobalNvsArea.Area->Usb2Port0Type = 0x0;
    mGlobalNvsArea.Area->Usb2Port0Visible = 1;
    mGlobalNvsArea.Area->Usb2Port0Group   = 5;

    mGlobalNvsArea.Area->Usb2Port1Connectable = 0xFF;
    mGlobalNvsArea.Area->Usb2Port1Type = 0x03;
    mGlobalNvsArea.Area->Usb2Port1Visible = 1;
    mGlobalNvsArea.Area->Usb2Port1Group   = 1;

    mGlobalNvsArea.Area->Usb2Port2Connectable = 0x0;
    mGlobalNvsArea.Area->Usb2Port2Type = 0xFF;
    mGlobalNvsArea.Area->Usb2Port2Visible = 0;
    mGlobalNvsArea.Area->Usb2Port2Group   = 7;

    mGlobalNvsArea.Area->Usb2Port3Connectable = 0x00;
    mGlobalNvsArea.Area->Usb2Port3Type = 0xFF;
    mGlobalNvsArea.Area->Usb2Port3Visible = 0;
    mGlobalNvsArea.Area->Usb2Port3Group   = 8;

    break;

   case BOARD_ID_CormorantLake:
    mGlobalNvsArea.Area->Usb3Port0Connectable = 0;
    mGlobalNvsArea.Area->Usb3Port0Type = 0xFF;
    mGlobalNvsArea.Area->Usb3Port0Visible = 0;
    mGlobalNvsArea.Area->Usb3Port0Group   = 1;

    mGlobalNvsArea.Area->Usb3Port1Connectable = 0;
    mGlobalNvsArea.Area->Usb3Port1Type = 0xFF;
    mGlobalNvsArea.Area->Usb3Port1Visible = 0;
    mGlobalNvsArea.Area->Usb3Port1Group   = 2;

    mGlobalNvsArea.Area->Usb3Port2Connectable = 0;
    mGlobalNvsArea.Area->Usb3Port2Type = 0xFF;
    mGlobalNvsArea.Area->Usb3Port2Visible = 0;
    mGlobalNvsArea.Area->Usb3Port2Group   = 3;

    mGlobalNvsArea.Area->Usb3Port3Connectable = 0xFF;
    mGlobalNvsArea.Area->Usb3Port3Type = 0x03;
    mGlobalNvsArea.Area->Usb3Port3Visible = 1;
    mGlobalNvsArea.Area->Usb3Port3Group   = 4;

    mGlobalNvsArea.Area->Usb2Port0Connectable = 0xFF;
    mGlobalNvsArea.Area->Usb2Port0Type = 0x00;
    mGlobalNvsArea.Area->Usb2Port0Visible = 1;
    mGlobalNvsArea.Area->Usb2Port0Group   = 5;

    mGlobalNvsArea.Area->Usb2Port1Connectable = 0xFF;
    mGlobalNvsArea.Area->Usb2Port1Type = 0x00;
    mGlobalNvsArea.Area->Usb2Port1Visible = 1;
    mGlobalNvsArea.Area->Usb2Port1Group   = 6;

    mGlobalNvsArea.Area->Usb2Port2Connectable = 0x0;
    mGlobalNvsArea.Area->Usb2Port2Type = 0xFF;
    mGlobalNvsArea.Area->Usb2Port2Visible = 0;
    mGlobalNvsArea.Area->Usb2Port2Group   = 7;

    mGlobalNvsArea.Area->Usb2Port3Connectable = 0xFF;
    mGlobalNvsArea.Area->Usb2Port3Type = 0x03;
    mGlobalNvsArea.Area->Usb2Port3Visible = 1;
    mGlobalNvsArea.Area->Usb2Port3Group   = 8;

    break;
   case BOARD_ID_Harcuvar:
   case BOARD_ID_CarCreekModule:

    mGlobalNvsArea.Area->Usb3Port0Connectable = 0;
    mGlobalNvsArea.Area->Usb3Port0Type = 0xFF;
    mGlobalNvsArea.Area->Usb3Port0Visible = 0;
    mGlobalNvsArea.Area->Usb3Port0Group   = 0;

    mGlobalNvsArea.Area->Usb3Port1Connectable = 0;
    mGlobalNvsArea.Area->Usb3Port1Type = 0xFF;
    mGlobalNvsArea.Area->Usb3Port1Visible = 0;
    mGlobalNvsArea.Area->Usb3Port1Group   = 0;

    mGlobalNvsArea.Area->Usb3Port2Connectable = 0;
    mGlobalNvsArea.Area->Usb3Port2Type = 0xFF;
    mGlobalNvsArea.Area->Usb3Port2Visible = 0;
    mGlobalNvsArea.Area->Usb3Port2Group   = 0;

    mGlobalNvsArea.Area->Usb3Port3Connectable = 0x1;
    mGlobalNvsArea.Area->Usb3Port3Type = 0x03;
    mGlobalNvsArea.Area->Usb3Port3Visible = 1;
    mGlobalNvsArea.Area->Usb3Port3Group   = 2;

    mGlobalNvsArea.Area->Usb2Port0Connectable = 01;
    mGlobalNvsArea.Area->Usb2Port0Type = 0x00;
    mGlobalNvsArea.Area->Usb2Port0Visible = 1;
    mGlobalNvsArea.Area->Usb2Port0Group   = 1;

    mGlobalNvsArea.Area->Usb2Port1Connectable = 0x1;
    mGlobalNvsArea.Area->Usb2Port1Type = 0x03;
    mGlobalNvsArea.Area->Usb2Port1Visible = 1;
    mGlobalNvsArea.Area->Usb2Port1Group   = 2;

    mGlobalNvsArea.Area->Usb2Port2Connectable = 0x1;
    mGlobalNvsArea.Area->Usb2Port2Type = 0x00;
    mGlobalNvsArea.Area->Usb2Port2Visible = 1;
    mGlobalNvsArea.Area->Usb2Port2Group   = 3;

    mGlobalNvsArea.Area->Usb2Port3Connectable = 0x1;
    mGlobalNvsArea.Area->Usb2Port3Type = 0xFF;
    mGlobalNvsArea.Area->Usb2Port3Visible = 0;
    mGlobalNvsArea.Area->Usb2Port3Group   = 4;

    break;

   case BOARD_ID_Minnehaha:
    mGlobalNvsArea.Area->Usb3Port0Connectable = 0;
    mGlobalNvsArea.Area->Usb3Port0Type = 0xFF;
    mGlobalNvsArea.Area->Usb3Port0Visible = 0;
    mGlobalNvsArea.Area->Usb3Port0Group   = 1;

    mGlobalNvsArea.Area->Usb3Port1Connectable = 0;
    mGlobalNvsArea.Area->Usb3Port1Type = 0xFF;
    mGlobalNvsArea.Area->Usb3Port1Visible = 0;
    mGlobalNvsArea.Area->Usb3Port1Group   = 2;

    mGlobalNvsArea.Area->Usb3Port2Connectable = 0;
    mGlobalNvsArea.Area->Usb3Port2Type = 0xFF;
    mGlobalNvsArea.Area->Usb3Port2Visible = 0;
    mGlobalNvsArea.Area->Usb3Port2Group   = 3;

    mGlobalNvsArea.Area->Usb3Port3Connectable = 0xFF;
    mGlobalNvsArea.Area->Usb3Port3Type = 0x03;
    mGlobalNvsArea.Area->Usb3Port3Visible = 1;
    mGlobalNvsArea.Area->Usb3Port3Group   = 4;

    mGlobalNvsArea.Area->Usb2Port0Connectable = 0xFF;
    mGlobalNvsArea.Area->Usb2Port0Type = 0x03;
    mGlobalNvsArea.Area->Usb2Port0Visible = 1;
    mGlobalNvsArea.Area->Usb2Port0Group   = 4;

    mGlobalNvsArea.Area->Usb2Port1Connectable = 0x0;
    mGlobalNvsArea.Area->Usb2Port1Type = 0xFF;
    mGlobalNvsArea.Area->Usb2Port1Visible = 0;
    mGlobalNvsArea.Area->Usb2Port1Group   = 6;

    mGlobalNvsArea.Area->Usb2Port2Connectable = 0x0;
    mGlobalNvsArea.Area->Usb2Port2Type = 0xFF;
    mGlobalNvsArea.Area->Usb2Port2Visible = 0;
    mGlobalNvsArea.Area->Usb2Port2Group   = 7;

    mGlobalNvsArea.Area->Usb2Port3Connectable = 0x0;
    mGlobalNvsArea.Area->Usb2Port3Type = 0xFF;
    mGlobalNvsArea.Area->Usb2Port3Visible = 0;
    mGlobalNvsArea.Area->Usb2Port3Group   = 8;
    break;

   case BOARD_ID_PineLake:
   case BOARD_ID_PineLakeB:
    mGlobalNvsArea.Area->Usb3Port0Connectable = 0;
    mGlobalNvsArea.Area->Usb3Port0Type = 0xFF;
    mGlobalNvsArea.Area->Usb3Port0Visible = 1;
    mGlobalNvsArea.Area->Usb3Port0Group   = 1;

    mGlobalNvsArea.Area->Usb3Port1Connectable = 0;
    mGlobalNvsArea.Area->Usb3Port1Type = 0xFF;
    mGlobalNvsArea.Area->Usb3Port1Visible = 1;
    mGlobalNvsArea.Area->Usb3Port1Group   = 1;

    mGlobalNvsArea.Area->Usb3Port2Connectable = 0;
    mGlobalNvsArea.Area->Usb3Port2Type = 0xFF;
    mGlobalNvsArea.Area->Usb3Port2Visible = 1;
    mGlobalNvsArea.Area->Usb3Port2Group   = 1;

    mGlobalNvsArea.Area->Usb3Port3Connectable = 0xFF;
    mGlobalNvsArea.Area->Usb3Port3Type = 0x03;
    mGlobalNvsArea.Area->Usb3Port3Visible = 1;
    mGlobalNvsArea.Area->Usb3Port3Group   = 1;

    mGlobalNvsArea.Area->Usb2Port0Connectable = 0xFF;
    mGlobalNvsArea.Area->Usb2Port0Type = 0x03;
    mGlobalNvsArea.Area->Usb2Port0Visible = 1;
    mGlobalNvsArea.Area->Usb2Port0Group   = 1;

    mGlobalNvsArea.Area->Usb2Port1Connectable = 0xFF;
    mGlobalNvsArea.Area->Usb2Port1Type = 0x00;
    mGlobalNvsArea.Area->Usb2Port1Visible = 1;
    mGlobalNvsArea.Area->Usb2Port1Group   = 1;

    mGlobalNvsArea.Area->Usb2Port2Connectable = 0xFF;
    mGlobalNvsArea.Area->Usb2Port2Type = 0x00;
    mGlobalNvsArea.Area->Usb2Port2Visible = 1;
    mGlobalNvsArea.Area->Usb2Port2Group   = 1;

    mGlobalNvsArea.Area->Usb2Port3Connectable = 0x00;
    mGlobalNvsArea.Area->Usb2Port3Type = 0x02;
    mGlobalNvsArea.Area->Usb2Port3Visible = 1;
    mGlobalNvsArea.Area->Usb2Port3Group   = 1;
    break;

    case BOARD_ID_Nano:
      mGlobalNvsArea.Area->Usb3Port0Connectable = 0;
      mGlobalNvsArea.Area->Usb3Port0Type = 0xFF;

   	  mGlobalNvsArea.Area->Usb3Port1Connectable = 0;
      mGlobalNvsArea.Area->Usb3Port1Type = 0xFF;

  	  mGlobalNvsArea.Area->Usb3Port2Connectable = 0;
  	  mGlobalNvsArea.Area->Usb3Port2Type = 0xFF;

  	  mGlobalNvsArea.Area->Usb3Port3Connectable = 0xFF;
      mGlobalNvsArea.Area->Usb3Port3Type = 0x03;

      mGlobalNvsArea.Area->Usb2Port0Connectable = 0;
      mGlobalNvsArea.Area->Usb2Port0Type = 0xFF;

      mGlobalNvsArea.Area->Usb2Port1Connectable = 0;
      mGlobalNvsArea.Area->Usb2Port1Type = 0xFF;

      mGlobalNvsArea.Area->Usb2Port2Connectable = 0xFF;
      mGlobalNvsArea.Area->Usb2Port2Type = 0x03;

      mGlobalNvsArea.Area->Usb2Port3Connectable = 0x00;
      mGlobalNvsArea.Area->Usb2Port3Type = 0xFF;
      break;

   default:
    DEBUG ((EFI_D_ERROR, "Unknown BoardId. USB ports information has been not configured\n"));
    break;
  }

  //
  // Find the AcpiTable protocol
  //
  Status = gBS->LocateProtocol (&gEfiAcpiTableProtocolGuid, NULL, (VOID**)&AcpiTableProtocol);
  if (EFI_ERROR (Status)) {
    return EFI_ABORTED;
  }
  //
  // Initialize MADT table
  //
  Status = MadtTableInitialize (&CurrentTable, &Size);
  ASSERT_EFI_ERROR (Status);
  //
  // Perform any table specific updates.
  //
  AcpiUpdateTable ((EFI_ACPI_DESCRIPTION_HEADER *) CurrentTable, &Version);
  //
  // Add the table
  //
  TableKey = 0;
  Status = AcpiTableProtocol->InstallAcpiTable (
                          AcpiTableProtocol,
                          CurrentTable,
                          CurrentTable->Length,
                          &TableKey
                          );
  ASSERT_EFI_ERROR (Status);
  CurrentTable = NULL;
  //
  // Init Pci Device PRT PRW information structure from PCD
  //
  mConfigData = (PCI_DEVICE_SETTING *)AllocateZeroPool (sizeof (PCI_DEVICE_SETTING));
  if(mConfigData == NULL) {
	DEBUG ((EFI_D_ERROR, "Pool has not been allocated\n"));
    return EFI_ABORTED;
  }
  InitPciDeviceInfoStructure (mConfigData);
  //
  // Get the Acpi SDT protocol for manipulation on acpi table
  //
  Status = gBS->LocateProtocol (&gEfiAcpiSdtProtocolGuid, NULL, &mAcpiSdt);
  ASSERT_EFI_ERROR (Status);
  //
  // Locate the firmware volume protocol
  //
  Status = LocateSupportProtocol (&gEfiFirmwareVolume2ProtocolGuid, (VOID**)&FwVol, 1);
  if (EFI_ERROR (Status)) {
    return EFI_ABORTED;
  }
  //
  // Read tables from the storage file.
  //

  while (Status == EFI_SUCCESS) {
    Status = FwVol->ReadSection (
                      FwVol,
                      (EFI_GUID*)PcdGetPtr (PcdAcpiTableStorageFile),
                      EFI_SECTION_RAW,
                      Instance,
                      (VOID**)&CurrentTable,
                      &Size,
                      &FvStatus
                      );

    if (!EFI_ERROR(Status)) {

    	if(CurrentTable == NULL){
    	  DEBUG ((EFI_D_ERROR, "ReadSection returned NULL table\n"));
    	  return EFI_ABORTED;
    	}
    	
      //
      // Allow platform specific code to reject the table or update it
      //
      AcpiStatus = AcpiPlatformHooksIsActiveTable (CurrentTable);

      if (!EFI_ERROR (AcpiStatus)) {
        //
        // Perform any table specific updates.
        //
        AcpiUpdateTable ((EFI_ACPI_DESCRIPTION_HEADER *) CurrentTable, &Version);

        //
        // Add the table
        //
        TableKey = 0;

        Status = AcpiTableProtocol->InstallAcpiTable (
                                              AcpiTableProtocol,
                                              CurrentTable,
                                              CurrentTable->Length,
                                              &TableKey
                                              );
        if (EFI_ERROR(Status)) {
          return EFI_ABORTED;
        }

        if (CurrentTable != NULL) {
          gBS->FreePool (CurrentTable);
        }
        //
        // If this table is the DSDT table, then update the _PRT and _PRW based on
        // the settings from pcds
        //
/*        if (CurrentTable->Signature == EFI_ACPI_2_0_DIFFERENTIATED_SYSTEM_DESCRIPTION_TABLE_SIGNATURE) {
        //
        // Create the root handle for DSDT table
        //
        Status = mAcpiSdt->OpenSdt (TableHandle, &mDsdtHandle);
        ASSERT_EFI_ERROR (Status);

        PciRootHandle = NULL;
        PciRootHandle = SdtGetRootBridgeHandle (mAcpiSdt, mDsdtHandle);
        ASSERT (PciRootHandle != NULL);

        PciDeviceInfo = NULL;
        for (Index = 0; Index < mConfigData->PciDeviceInfoNumber; Index++) {
          PciDeviceInfo = &(mConfigData->PciDeviceInfo[Index]);

          //
          // Check whether this is a valid item
          //
          if ((PciDeviceInfo->BridgeAddress != 0xFFFFFFFF) && (PciDeviceInfo->DeviceAddress != 0xFFFFFFFF)) {

            //DEBUG ((EFI_D_ERROR, "Valid pci info structure: bridge address:0x%x, device address:0x%x\n", PciDeviceInfo->BridgeAddress, PciDeviceInfo->DeviceAddress));

            UpdatePRT = FALSE;
            UpdatePRW = FALSE;

  //            SdtCheckPciDeviceInfoChanged (PciDeviceInfo, &UpdatePRT, &UpdatePRW);
            //
            // Check whether there is any valid pci routing item
            //
            if (UpdatePRT) {
              //
              // Update the pci routing information
              //
              //DEBUG ((EFI_D_ERROR, "Update _PRT\n"));
              SdtUpdatePciRouting (mAcpiSdt, PciRootHandle, PciDeviceInfo);
            }
            //
            // Check whether there is any valid pci routing item
            //
            if (UpdatePRW) {
              //
              // Update the pci wakeup information
              //
              //DEBUG ((EFI_D_ERROR, "Update _PRW\n"));
              SdtUpdatePowerWake (mAcpiSdt, PciRootHandle, PciDeviceInfo);
            }
          }
        }
        Status = mAcpiSdt->Close (PciRootHandle);
        ASSERT_EFI_ERROR (Status);
        //
        // Mark the root handle as modified , let SDT protocol recaculate the checksum
        //
        ((EFI_AML_HANDLE *)mDsdtHandle)->Modified = TRUE;
        Status = mAcpiSdt->Close (mDsdtHandle);
        ASSERT_EFI_ERROR (Status);
      }
        */
      }
      //
      // Increment the instance
      //
      Instance++;
      CurrentTable = NULL;
    }
  }

  //
  // Getting and using the DSDT & FADT table already published by AMI ACPI module and perform platform specific updates
  //
  Signature = EFI_ACPI_3_0_DIFFERENTIATED_SYSTEM_DESCRIPTION_TABLE_SIGNATURE;
  Version = EFI_ACPI_TABLE_VERSION_ALL;
  
  for (TableNumber = 0 ; TableNumber < 2 ; TableNumber++){
	  
	  // Finds first DSDT Table followed by FADT Table using Signature and Version
	  Status = FindDsdtFadtTable(
			  Version,
			  Signature,
			  (EFI_ACPI_SDT_HEADER**) &CurrentTable,
			  &TableKey
			  );
	  if (!EFI_ERROR(Status)) {
		  //
		  // Allow platform specific code to reject the table or update it
		  //
		  AcpiStatus = AcpiPlatformHooksIsActiveTable (CurrentTable);
		  
		  if (!EFI_ERROR (AcpiStatus)) {
			  //
			  // Perform any table specific updates.
			  //
			  AcpiUpdateTable ((EFI_ACPI_DESCRIPTION_HEADER *) CurrentTable, &Version);
			  
			  if (!EFI_ERROR(AcpiStatus)){
				  // Uninstall the already available table and install updated table
				  Status = AcpiTableProtocol->UninstallAcpiTable(AcpiTableProtocol, TableKey);
				  if(!EFI_ERROR(Status)){
					  TableKey = 0;
					  Status = AcpiTableProtocol->InstallAcpiTable(
							  AcpiTableProtocol, 
							  CurrentTable,
							  CurrentTable->Length,
							  &TableKey
							  );
				  }
			  }
			  FreePool(CurrentTable); // Freeing memory allocated by GetAcpiTable.
		  }
	  }
	  Signature = EFI_ACPI_3_0_FIXED_ACPI_DESCRIPTION_TABLE_SIGNATURE;
	  Version = EFI_ACPI_TABLE_VERSION_1_0B;
  }
  
  Status = EfiCreateEventReadyToBootEx(
             TPL_NOTIFY,
             OnReadyToBoot,
             NULL,
             &Event
             );

  gBS->FreePool (mConfigData);
  return EFI_SUCCESS;
}
