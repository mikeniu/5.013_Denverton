//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************


/** @file WdatDxe.c 
    Entry point for WDAT initialization. 

**/

//---------------------------------------------------------------------------
// Include(s)
//---------------------------------------------------------------------------
#include <AmiDxeLib.h>
#include "Wdat.h"
#include <Protocol\AcpiSupport.h>

//---------------------------------------------------------------------------
// Constant, Macro and Type Definition(s)
//---------------------------------------------------------------------------
// Constant Definition(s)

// Macro Definition(s)

// Type Definition(s)

// Function Prototype(s)

//---------------------------------------------------------------------------
// Variable and External Declaration(s)
//---------------------------------------------------------------------------
// Variable Declaration(s)
static WDA_TABLE    *gWdatTable;
UINT8    ACPI_OEM_ID[6]     = CONVERT_TO_STRING(T_ACPI_OEM_ID);     //"A M I";      //add 0 at the end.OemId 6 bytes
UINT8    ACPI_OEM_TBL_ID[8] = CONVERT_TO_STRING(T_ACPI_OEM_TBL_ID); //"ALASKA"; //add 0 at the end.OemTableId  8 bytes

extern UINT32	ActionTableLen;

// GUID Definition(s)
EFI_GUID gAcpiSupportGuid = EFI_ACPI_SUPPORT_GUID;

// Protocol Definition(s)

// External Declaration(s)
EFI_STATUS WdatHeaderAndActionTableOverride(WDA_TABLE *pWdatTable);

// Function Definition(s)

//---------------------------------------------------------------------------                 

/**
    Setting the default WDAT's ACPI header data.
    
    @retval EFI_SUCCESS Always returns EFI_SUCCESS.  
    
**/
EFI_STATUS SetupAcpiHeaderData(VOID)
{
	UINTN		Index;

	gWdatTable->AcpiHeader.Signature = WDAT_SIG;
	gWdatTable->AcpiHeader.Length = sizeof(WDA_TABLE) + (ActionTableLen * sizeof(WDAT_INST_ENTRY));
	gWdatTable->AcpiHeader.Revision = 0x1;
	gWdatTable->AcpiHeader.Checksum = 0x00;
	gWdatTable->AcpiHeader.OemRev = ACPI_OEM_REV;
	gWdatTable->AcpiHeader.CreatorId = CREATOR_ID_MS;
	gWdatTable->AcpiHeader.CreatorRev = CREATOR_REV_MS;

	//Update OemdID & OemTableID
    for(Index=0;Index<6;Index++) 
		((UINT8 *)(gWdatTable->AcpiHeader.OemId))[Index]=ACPI_OEM_ID[Index];
    for(Index=0;Index<8;Index++)
		((UINT8 *)(gWdatTable->AcpiHeader.OemTblId))[Index]=ACPI_OEM_TBL_ID[Index];

	return EFI_SUCCESS;	
}

/**
    Entry point of the WDAT driver. Locate ACPI support protocol and 
    Call SetupAcpiHeaderData and WdatHeaderAndActionTableOverride hooks
    update in ACPI table.
    
    @param  ImageHandle The firmware allocated handle for the EFI image.  
    @param  SystemTable A pointer to the EFI System Table.  
    @retval Status
    
**/
EFI_STATUS WatchdogActionTableEntryPoint(
	IN EFI_HANDLE ImageHandle,
	IN EFI_SYSTEM_TABLE *SystemTable
)
{
	EFI_STATUS	Status = EFI_UNSUPPORTED;
#if ACPI_SUPPORT
	EFI_ACPI_SUPPORT_PROTOCOL	*AcpiSupport;
	UINTN		AsptTblHandle = 0;

	InitAmiLib(ImageHandle,SystemTable);

	Status = pBS->LocateProtocol (&gAcpiSupportGuid, NULL, &AcpiSupport);
	if (EFI_ERROR (Status)) return Status;
	
	Status = pBS->AllocatePool(EfiBootServicesData, sizeof(ACPI_HDR) + sizeof(WDAT_HDR) + (ActionTableLen*sizeof(WDAT_INST_ENTRY)), &gWdatTable);
	if (EFI_ERROR (Status)) return Status;
	
	Status = SetupAcpiHeaderData();
 	Status = WdatHeaderAndActionTableOverride(gWdatTable);
 
	Status = AcpiSupport->SetAcpiTable (AcpiSupport, gWdatTable, TRUE, EFI_ACPI_TABLE_VERSION_ALL, &AsptTblHandle);
	
	pBS->FreePool(gWdatTable);
	
#endif
	return Status;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
