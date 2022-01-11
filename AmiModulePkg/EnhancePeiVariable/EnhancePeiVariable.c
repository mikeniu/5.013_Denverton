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

EnhancePeiVariable main file

**/
#include <AmiPeiLib.h>
#include "EnhancePeiVariable.h"

/**
    Module entry point
    This entry will determine the nvram region.

    @param FileHandle   Handle of the file being invoked.
    @param PeiServices  Describes the list of possible PEI Services.

    @retval EFI_STATUS  based on result

**/
EFI_STATUS
EFIAPI
EnhancePeiVariableEntry(
    IN  EFI_PEI_FILE_HANDLE     FileHandle,
    IN  CONST EFI_PEI_SERVICES  **PeiServices
)
{
    EFI_STATUS                      Status;
    EFI_PEI_READ_ONLY_VARIABLE2_PPI *VariablePpi;

    Status = PeiServicesLocatePpi(
                &gEfiPeiReadOnlyVariable2PpiGuid,
                0,
                NULL,
                (VOID**)&VariablePpi );
    if( EFI_ERROR(Status) )
        return Status;

#if defined(NVRAM_VERSION) && (NVRAM_VERSION >= 9)
    if( (((VARIABLE_PPI*)VariablePpi)->InfoCount != 0) )
#else
    if( (((VARIABLE_PPI*)VariablePpi)->InfoCount != 0) ||
        (((VARIABLE_PPI*)VariablePpi)->MainInfo != NULL) )
#endif
    {
        // If Nvram region is available, cache specfic variables
        CacheVariablePeiEntry( FileHandle, PeiServices );
    }
    else
    {
        // If Nvram region is not available, try to find StdDefault in FV_BB
#if defined(GET_FROM_STD_DEFAULT) && (GET_FROM_STD_DEFAULT == 1)
        GetVarFromStdDefaultPeiEntry( FileHandle, PeiServices );
#endif
    }

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