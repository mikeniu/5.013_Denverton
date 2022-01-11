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
#include "Token.h"
#include "../NVRAM/NVRAM.h"
#include <Library/PeiServicesLib.h>
#include <Ppi/ReadOnlyVariable2.h>

EFI_STATUS
CacheVariablePeiEntry(
    IN  EFI_PEI_FILE_HANDLE     FileHandle,
    IN  CONST EFI_PEI_SERVICES  **PeiServices
);

EFI_STATUS
GetVarFromStdDefaultPeiEntry(
    IN  EFI_PEI_FILE_HANDLE     FileHandle,
    IN  CONST EFI_PEI_SERVICES  **PeiServices
);

typedef struct
{
    EFI_PEI_READ_ONLY_VARIABLE2_PPI Ppi;
    UINT32 InfoCount;
    UINT32 LastInfoIndex;
    NVRAM_STORE_INFO NvramInfo[3];
#if !defined(NVRAM_VERSION) || (defined(NVRAM_VERSION) && (NVRAM_VERSION < 9))
    NVRAM_STORE_INFO *MainInfo;
#endif
}VARIABLE_PPI;
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