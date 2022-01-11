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

CacheVariablePei main file

**/
#include <AmiPeiLib.h>
#include "CacheVariablePei.h"
#include "CachedVariableElink.h"
#include "Token.h"

VARIABLES_TO_BE_CACHED  VariableToBeCachedList[] = 
{
    VARIABLE_TO_BE_CACHED_LIST
    {NULL, {0,0,0,0,0,0,0,0,0,0,0}}
};

EFI_STATUS
EFIAPI
CacheVariableMemoryDiscoveredCallback(
    IN  CONST EFI_PEI_SERVICES      **PeiServices,
    IN  EFI_PEI_NOTIFY_DESCRIPTOR   *NotifyDescriptor,
    IN  VOID                        *Ppi
);

static EFI_GUID mCacheVariableHobGuid = CACHE_VARIABLE_HOB_GUID;

static EFI_PEI_NOTIFY_DESCRIPTOR mMemDiscoveredNotifyList = {
    (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gEfiPeiMemoryDiscoveredPpiGuid,
    CacheVariableMemoryDiscoveredCallback
};

/**
    After memory is installed, cache as ram will be diabled.
    This callback will re-initialize cached region on memory.

    @param PeiServices      An indirect pointer to the EFI_PEI_SERVICES table
                            published by the PEI Foundation
    @param NotifyDescriptor Address of the notification descriptor data structure
    @param Ppi              Address of the PPI that was installed

    @retval EFI_STATUS based on result

**/
EFI_STATUS
EFIAPI
CacheVariableMemoryDiscoveredCallback(
    IN  CONST EFI_PEI_SERVICES      **PeiServices,
    IN  EFI_PEI_NOTIFY_DESCRIPTOR   *NotifyDescriptor,
    IN  VOID                        *Ppi
)
{
    UINT8                           *Hob;
    EFI_STATUS                      Status;
    EFI_PEI_READ_ONLY_VARIABLE2_PPI *ReadVariablePpi;

    Hob = NULL;
    Hob = (UINT8*)GetFirstGuidHob( &mCacheVariableHobGuid );
    if( Hob == NULL )
        return EFI_NOT_FOUND;

    Hob = GET_GUID_HOB_DATA(Hob);

    Status = PeiServicesLocatePpi(
                &gEfiPeiReadOnlyVariable2PpiGuid,
                0,
                NULL,
                (VOID**)&ReadVariablePpi );
    if( EFI_ERROR(Status) )
        return Status;

    ((VARIABLE_PPI*)ReadVariablePpi)->NvramInfo[0].NvramAddress = Hob;
#if defined(NVRAM_VERSION) && (NVRAM_VERSION >= 7)
    NvInitInfoBuffer( &(((VARIABLE_PPI*)ReadVariablePpi)->NvramInfo[0]), 0, NVRAM_STORE_FLAG_NON_VALATILE, NULL );
#else
    NvInitInfoBuffer( &(((VARIABLE_PPI*)ReadVariablePpi)->NvramInfo[0]), 0, NVRAM_STORE_FLAG_NON_VALATILE );
#endif

    return EFI_SUCCESS;
}
/**
    This entry will cache specific variables to generate nvram region.

    @param FileHandle   Handle of the file being invoked.
    @param PeiServices  Describes the list of possible PEI Services.

    @retval EFI_STATUS  based on result

**/
EFI_STATUS
CacheVariablePeiEntry(
    IN  EFI_PEI_FILE_HANDLE     FileHandle,
    IN  CONST EFI_PEI_SERVICES  **PeiServices
)
{
    EFI_STATUS                      Status;
    EFI_PEI_READ_ONLY_VARIABLE2_PPI *ReadVariablePpi;
    UINTN                           CachedRegionSize;
    UINT32                          Index;
    UINT8                           *TempData;
    UINTN                           TempSize;
    UINT32                          TempAttr;
    UINT8                           *CachedRegion;
    GUID_NVAR                       *GuidNvarPtr;
    VARIABLE_PPI                    *VariablePpi;
    NVRAM_STORE_INFO                CachedRegionInfo;

    Status = PeiServicesLocatePpi(
                &gEfiPeiReadOnlyVariable2PpiGuid,
                0,
                NULL,
                (VOID**)&ReadVariablePpi );
    if( EFI_ERROR(Status) )
        return Status;

    // Calculate total size to store cached variables
    CachedRegionSize = 0;
    for( Index = 0; VariableToBeCachedList[Index].CachedVariableName != NULL; Index++ )
    {
        TempSize = 0;
        TempData = (UINT8*)&TempData;
        Status = ReadVariablePpi->GetVariable(
                                ReadVariablePpi,
                                VariableToBeCachedList[Index].CachedVariableName,
                                &VariableToBeCachedList[Index].CachedVariableGuid,
                                NULL,
                                &TempSize,
                                TempData );
        if( Status == EFI_BUFFER_TOO_SMALL )
        {
            CachedRegionSize += (sizeof(GUID_NVAR) - sizeof(GUID_NVAR_RAW_DATA));
            CachedRegionSize += (StrSize( VariableToBeCachedList[Index].CachedVariableName ) / sizeof(CHAR16));
            CachedRegionSize += TempSize;
        }
    }

    if( CachedRegionSize == 0 )
        return EFI_SUCCESS;

    CachedRegion = NULL;
    CachedRegion = (UINT8*)BuildGuidHob( &mCacheVariableHobGuid, CachedRegionSize );
    if( CachedRegion == NULL )
        return EFI_SUCCESS;

    GuidNvarPtr = (GUID_NVAR*)CachedRegion;
    for( Index = 0; VariableToBeCachedList[Index].CachedVariableName != NULL; Index++ )
    {
        TempData = ( &GuidNvarPtr->NvarRawData.CachedVariableName +
                    (StrSize( VariableToBeCachedList[Index].CachedVariableName ) / sizeof(CHAR16)) );
        TempSize = CachedRegionSize;
        Status = ReadVariablePpi->GetVariable(
                                ReadVariablePpi,
                                VariableToBeCachedList[Index].CachedVariableName,
                                &VariableToBeCachedList[Index].CachedVariableGuid,
                                &TempAttr,
                                &TempSize,
                                TempData );
        if( !EFI_ERROR(Status) )
        {
            // Initalize NVAR header
            GuidNvarPtr->Nvar.signature = NVAR_SIGNATURE;
            GuidNvarPtr->Nvar.size = 0;
            GuidNvarPtr->Nvar.size += (sizeof(GUID_NVAR) - sizeof(GUID_NVAR_RAW_DATA));
            GuidNvarPtr->Nvar.size += (StrSize( VariableToBeCachedList[Index].CachedVariableName ) / sizeof(CHAR16));
            GuidNvarPtr->Nvar.size += TempSize;
            GuidNvarPtr->Nvar.next = 0xFFFFFF;
            GuidNvarPtr->Nvar.flags = NVRAM_FLAG_VALID | NVRAM_FLAG_GUID | NVRAM_FLAG_ASCII_NAME;
            if( TempAttr & EFI_VARIABLE_RUNTIME_ACCESS )
                GuidNvarPtr->Nvar.flags |= NVRAM_FLAG_RUNTIME;

            CopyGuid( &GuidNvarPtr->NvarGuid, &VariableToBeCachedList[Index].CachedVariableGuid );

            UnicodeStrToAsciiStr( VariableToBeCachedList[Index].CachedVariableName, &GuidNvarPtr->NvarRawData.CachedVariableName );

            GuidNvarPtr = (GUID_NVAR*)(&GuidNvarPtr->NvarRawData.CachedVariableName + AsciiStrSize( &GuidNvarPtr->NvarRawData.CachedVariableName ) + TempSize);
        }
    }

    VariablePpi = (VARIABLE_PPI*)ReadVariablePpi;

    // Initialize cached variable region
    MemSet( &CachedRegionInfo, sizeof(CachedRegionInfo), 0 );
    CachedRegionInfo.NvramAddress = CachedRegion;
    CachedRegionInfo.NvramSize = CachedRegionSize;
#if defined(NVRAM_VERSION) && (NVRAM_VERSION >= 7)
    NvInitInfoBuffer( &CachedRegionInfo, 0, NVRAM_STORE_FLAG_NON_VALATILE, NULL );
#else
    NvInitInfoBuffer( &CachedRegionInfo, 0, NVRAM_STORE_FLAG_NON_VALATILE );
#endif

    // Add cached region to VARIABLE_PPI
    MemCpy( &VariablePpi->NvramInfo[2], &VariablePpi->NvramInfo[1], sizeof(NVRAM_STORE_INFO) );
    MemCpy( &VariablePpi->NvramInfo[1], &VariablePpi->NvramInfo[0], sizeof(NVRAM_STORE_INFO) );
    MemCpy( &VariablePpi->NvramInfo[0], &CachedRegionInfo, sizeof(NVRAM_STORE_INFO) );

    if( VariablePpi->InfoCount < 3 )
        VariablePpi->InfoCount++;

#if !defined(NVRAM_VERSION) || (defined(NVRAM_VERSION) && (NVRAM_VERSION < 9))
    VariablePpi->MainInfo = &VariablePpi->NvramInfo[0];
#endif

    PeiServicesNotifyPpi( &mMemDiscoveredNotifyList );

    return EFI_SUCCESS;
}