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
#include "../NVRAM/NVRAM.h"
#include "EnhancePeiVariable.h"
#include <Library/HobLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiLib.h>
#include <Library/PeiServicesLib.h>
#include <Ppi/ReadOnlyVariable2.h>

//E5C61161-6BF4-49D0-810D-C7287BBB35E6
#define CACHE_VARIABLE_HOB_GUID \
    { \
        0xE5C61161, 0x6BF4, 0x49D0, {0x81, 0x0D, 0xC7, 0x28, 0x7B, 0xBB, 0x35, 0xE6} \
    }

typedef struct
{
    CHAR16      *CachedVariableName;
    EFI_GUID    CachedVariableGuid;
}VARIABLES_TO_BE_CACHED;

#pragma pack(1)
typedef union
{
    CHAR8       CachedVariableName;
    UINT8       CachedVariableData;
}GUID_NVAR_RAW_DATA;

typedef struct
{
    NVAR                        Nvar;
    EFI_GUID                    NvarGuid;
    GUID_NVAR_RAW_DATA          NvarRawData;
}GUID_NVAR;
#pragma pack()
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