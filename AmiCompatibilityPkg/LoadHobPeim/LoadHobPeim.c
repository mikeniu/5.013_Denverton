//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2011, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

#include <Library/DebugLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/HobLib.h>

#include <Ppi/LoadFile.h>

#include <AmiLib.h>
#include <AmiHobs.h>

EFI_STATUS
EFIAPI
LoadHobPeimEntry (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
	static EFI_GUID LoadHobGuid = AMI_PEIM_LOAD_HOB_GUID;
	PEIM_LOAD_HOB *LoadHob;
    EFI_PEI_LOAD_FILE_PPI                     *LoadFile;
    EFI_STATUS Status;

	Status = (*PeiServices)->GetHobList(PeiServices,&LoadHob);
    if (EFI_ERROR(Status)) return EFI_NOT_FOUND;
	// Load PEIMs
	while(!EFI_ERROR(FindNextHobByGuid(&LoadHobGuid, &LoadHob)))
	{
		EFI_PHYSICAL_ADDRESS Address,EntryPoint, ModuleDelta;
		UINT64 Size;
		// if file header is NULL, the entry is in the PEI Core binary
        if (LoadHob->pFfsHeader == NULL) continue;
        
        Status = (*PeiServices)->LocatePpi (PeiServices, &gEfiPeiLoadFilePpiGuid, 0, NULL, (VOID **) &LoadFile);
        //
        // There must exist an instance of EFI_PEI_LOAD_FILE_PPI to support to load file handle successfully.
        //
        ASSERT_EFI_ERROR (Status);
        if (EFI_ERROR(Status)) return EFI_NOT_FOUND;

        Status = LoadFile->LoadFile (
                             LoadFile,
                             LoadHob->pFfsHeader,
                             &Address,
                             &Size,
                             &EntryPoint,
                             NULL
                             );
        if (EFI_ERROR(Status)) continue;
		ModuleDelta = EntryPoint - (UINT64)LoadHob->EntryPoint;
        if (ModuleDelta==0) continue; // the module was not loaded
        
DEBUG_CODE_BEGIN();
    char sName[0x100];
    EFI_FFS_FILE_HEADER *PeimFileHeader = LoadHob->pFfsHeader;

    if (!GetName((VOID*)(UINTN)Address,sName))
        Sprintf_s(sName,sizeof(sName),"[%G]",&PeimFileHeader->Name);
    DEBUG((
        EFI_D_INIT, "%s.InMemEntry(%X)\n", sName, 
        (UINT64)LoadHob->InMemEntryPoint+ModuleDelta
    ));
DEBUG_CODE_END();

		((EFI_PEIM_ENTRY_POINT)(
			(UINT64)LoadHob->InMemEntryPoint+ModuleDelta
		))(LoadHob->pFfsHeader, PeiServices);		
	}
  
  return EFI_SUCCESS;
}   
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2011, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************