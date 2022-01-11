//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
/** @file

GetVarFromStdDefault main file

**/

#include <AmiPeiLib.h>
#include <Setup.h>
#include "GetVarFromStdDefault.h"

EFI_STATUS
EFIAPI
GetVarMemoryDiscoveredCallback(
    IN  CONST EFI_PEI_SERVICES      **PeiServices,
    IN  EFI_PEI_NOTIFY_DESCRIPTOR   *NotifyDescriptor,
    IN  VOID                        *Ppi
);

static EFI_GUID mStdDefaultsGuid = STD_DEFAULTS_GUID;
static EFI_GUID mGetVariableFromStdDefaultGuid = GET_VARIABLE_FROM_STD_DEFAULT_GUID;
static EFI_GUID mFvBBStdDefaultFfsGuid = FV_BB_STD_DEFAULT_FFS_GUID;

static EFI_PEI_NOTIFY_DESCRIPTOR mMemDiscoveredNotifyList = {
    (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gEfiPeiMemoryDiscoveredPpiGuid,
    GetVarMemoryDiscoveredCallback
};

/**
    After memory is installed, cache as ram will be diabled.
    This callback will move StdDefault from cache to memory.

    @param PeiServices      An indirect pointer to the EFI_PEI_SERVICES table
                            published by the PEI Foundation
    @param NotifyDescriptor Address of the notification descriptor data structure
    @param Ppi              Address of the PPI that was installed

    @retval EFI_STATUS based on result

**/
EFI_STATUS
EFIAPI
GetVarMemoryDiscoveredCallback(
    IN  CONST EFI_PEI_SERVICES      **PeiServices,
    IN  EFI_PEI_NOTIFY_DESCRIPTOR   *NotifyDescriptor,
    IN  VOID                        *Ppi
)
{
    GET_VAR_FROM_STD_DEFAULT_HOB    *Hob;

    Hob = (GET_VAR_FROM_STD_DEFAULT_HOB*)GetFirstGuidHob( &mGetVariableFromStdDefaultGuid );
    if( Hob == NULL )
        return EFI_NOT_FOUND;

    Hob = (GET_VAR_FROM_STD_DEFAULT_HOB*)GET_GUID_HOB_DATA(Hob);

    Hob->StdDefaultInfo.NvramAddress = &Hob->StdDefaultData;
#if defined(NVRAM_VERSION) && (NVRAM_VERSION >= 7)
    NvInitInfoBuffer( &Hob->StdDefaultInfo, 0, NVRAM_STORE_FLAG_NON_VALATILE, NULL );
#else
    NvInitInfoBuffer( &Hob->StdDefaultInfo, 0, NVRAM_STORE_FLAG_NON_VALATILE );
#endif

    return EFI_SUCCESS;
}
/**
    This function searches for Var with specific GUID

    @param This         pointer to FI_PEI_READ_ONLY_VARIABLE2_PPI
    @param VariableName pointer to Var Name in Unicode
    @param VendorGuid   pointer to Var GUID
    @param Attributes   Pointer to memory where Attributes will be returned 
    @param DataSize     size of Var - if smaller than actual EFI_BUFFER_TOO_SMALL 
                        will be returned and DataSize will be set to actual size needed
    @param Data         Pointer to memory where Var will be returned

    @retval EFI_STATUS  based on result

**/
EFI_STATUS
EFIAPI
GetVarFromStdDefaultHook(
    IN      CONST EFI_PEI_READ_ONLY_VARIABLE2_PPI   *This,
    IN      CONST CHAR16                            *VariableName,
    IN      CONST EFI_GUID                          *VendorGuid,
    OUT     UINT32                                  *Attributes,
    IN  OUT UINTN                                   *DataSize,
    OUT     VOID                                    *Data
)
{
    GET_VAR_FROM_STD_DEFAULT_HOB    *Hob;
    EFI_STATUS                      Status;

    Hob = (GET_VAR_FROM_STD_DEFAULT_HOB*)GetFirstGuidHob( &mGetVariableFromStdDefaultGuid );
    if( Hob == NULL )
        return EFI_NOT_FOUND;
    Hob = (GET_VAR_FROM_STD_DEFAULT_HOB*)GET_GUID_HOB_DATA(Hob);

    Status = Hob->SavedPeiGetVariable(
                    This,
                    VariableName,
                    VendorGuid,
                    Attributes,
                    DataSize,
                    Data );
    if( Status != EFI_NOT_FOUND )
        return Status;

    if( Hob->Step != GetVarFromStdDefaultAvailiable )
        return EFI_NOT_FOUND;

	Status = NvGetVariable2(
				VariableName,
                VendorGuid,
                Attributes,
				DataSize,
                Data,
                1,
                &Hob->StdDefaultInfo );

    return Status;
}
/**
    This entry will determine the nvram region.
    If nvram region is not available, it will find StdDefault in FV_BB.
    And hook GetVariable() with GetVarFromStdDefaultHook().

    @param FileHandle   Handle of the file being invoked.
    @param PeiServices  Describes the list of possible PEI Services.

    @retval EFI_STATUS  based on result

**/
EFI_STATUS
GetVarFromStdDefaultPeiEntry(
    IN  EFI_PEI_FILE_HANDLE     FileHandle,
    IN  CONST EFI_PEI_SERVICES  **PeiServices
)
{
    EFI_STATUS                      Status;
    EFI_PEI_READ_ONLY_VARIABLE2_PPI *VariablePpi;
    EFI_PEI_FV_HANDLE               FvHandle;
    EFI_PEI_FILE_HANDLE             FfsHandle;
    EFI_FFS_FILE_HEADER             *FfsHeader;
    VOID                            *StdDeafultData;
    UINT32                          StdDeafultDataSize;
    NVRAM_STORE_INFO                StdDefaultArea;
    UINT32                          StdAttr;
    UINTN                           StdSize;
    VOID                            *StdDefault;
    GET_VAR_FROM_STD_DEFAULT_HOB    *Hob;

    Status = PeiServicesLocatePpi(
                &gEfiPeiReadOnlyVariable2PpiGuid,
                0,
                NULL,
                (VOID**)&VariablePpi );
    if( EFI_ERROR(Status) )
        return Status;

    //If Nvram region is available, just return.
#if defined(NVRAM_VERSION) && (NVRAM_VERSION >= 9)
    if( (((VARIABLE_PPI*)VariablePpi)->InfoCount != 0) )
#else
    if( (((VARIABLE_PPI*)VariablePpi)->InfoCount != 0) ||
        (((VARIABLE_PPI*)VariablePpi)->MainInfo != NULL) )
#endif
        return EFI_SUCCESS;

    //Find StdDefault ffs in FV_BB.
    Status = PeiServicesFfsFindNextVolume( 0, &FvHandle );
    if( EFI_ERROR(Status) )
        return Status;

    FfsHandle = NULL;
    while(TRUE)
    {
        Status = PeiServicesFfsFindNextFile(
                    EFI_FV_FILETYPE_RAW,
                    FvHandle,
                    &FfsHandle );
        if( EFI_ERROR(Status) ) break;
        FfsHeader = (EFI_FFS_FILE_HEADER*)FfsHandle;
        if( CompareGuid(&FfsHeader->Name, &mFvBBStdDefaultFfsGuid) ) break;
    }
    if( EFI_ERROR(Status) )
        return Status;

    StdDeafultData = FfsHeader + 1;
    StdDeafultDataSize = FFS_FILE_SIZE(FfsHeader) - sizeof(EFI_FFS_FILE_HEADER);

    //Init StdDefault region.
    MemSet( &StdDefaultArea, sizeof(StdDefaultArea), 0 );
    StdDefaultArea.NvramAddress = (UINT8*)StdDeafultData;
    StdDefaultArea.NvramSize = StdDeafultDataSize;
#if defined(NVRAM_VERSION) && (NVRAM_VERSION >= 7)
    NvInitInfoBuffer( &StdDefaultArea, 0, NVRAM_STORE_FLAG_NON_VALATILE, NULL );
#else
    NvInitInfoBuffer( &StdDefaultArea, 0, NVRAM_STORE_FLAG_NON_VALATILE );
#endif

    //Get StdDefault variable.
    StdAttr = 0;
    StdSize = 0;
    StdDefault = (VOID*)&StdDefault;
    Status = NvGetVariable2(
                L"StdDefaults",
                &mStdDefaultsGuid,
                &StdAttr,
                &StdSize,
                StdDefault,
                1,
                &StdDefaultArea );
    if( EFI_ERROR(Status) && (Status != EFI_BUFFER_TOO_SMALL) )
        return Status;

    Hob = NULL;
    Hob = (GET_VAR_FROM_STD_DEFAULT_HOB*)BuildGuidHob(
                                            &mGetVariableFromStdDefaultGuid,
                                            sizeof(GET_VAR_FROM_STD_DEFAULT_HOB) - sizeof(UINT8) + StdSize );
    if( Hob == NULL )
        return EFI_BUFFER_TOO_SMALL;

    Status = NvGetVariable2(
                L"StdDefaults",
                &mStdDefaultsGuid,
                &StdAttr,
                &StdSize,
                &Hob->StdDefaultData,
                1,
                &StdDefaultArea );
    if( EFI_ERROR(Status) )
        return Status;

    //Init StdDefault nvram region.
    MemSet( &StdDefaultArea, sizeof(StdDefaultArea), 0 );
    StdDefaultArea.NvramAddress = &Hob->StdDefaultData;
    StdDefaultArea.NvramSize = StdSize;
#if defined(NVRAM_VERSION) && (NVRAM_VERSION >= 7)
    NvInitInfoBuffer( &StdDefaultArea, 0, NVRAM_STORE_FLAG_NON_VALATILE, NULL );
#else
    NvInitInfoBuffer( &StdDefaultArea, 0, NVRAM_STORE_FLAG_NON_VALATILE );
#endif

    Hob->Step = GetVarFromStdDefaultFound;
    Hob->SavedPeiGetVariable = VariablePpi->GetVariable;
    MemCpy( &Hob->StdDefaultInfo, &StdDefaultArea, sizeof(StdDefaultArea) );

    VariablePpi->GetVariable = GetVarFromStdDefaultHook;
    Hob->Step = GetVarFromStdDefaultAvailiable;

    PeiServicesNotifyPpi( &mMemDiscoveredNotifyList );

    return EFI_SUCCESS;
}
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************