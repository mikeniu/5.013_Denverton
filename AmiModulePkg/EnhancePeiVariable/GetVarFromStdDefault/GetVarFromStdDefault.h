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

//E03E6451-297A-4FE9-B1F7-639B70327C52
#define GET_VARIABLE_FROM_STD_DEFAULT_GUID \
    { \
        0xE03E6451, 0x297A, 0x4FE9, {0xB1, 0xF7, 0x63, 0x9B, 0x70, 0x32, 0x7C, 0x52} \
    }

//77D3DC50-D42B-4916-AC80-8F469035D150
#define FV_BB_STD_DEFAULT_FFS_GUID \
    { \
        0x77D3DC50, 0xD42B, 0x4916, {0xAC, 0x80, 0x8F, 0x46, 0x90, 0x35, 0xD1, 0x50} \
    }

#define STD_DEFAULTS_GUID \
    { \
        0x4599D26F, 0x1A11, 0x49B8, {0xB9, 0x1F, 0x85, 0x87, 0x45, 0xCF, 0xF8, 0x24} \
    }

typedef enum
{
    GetVarFromStdDefaultNone = 0,
    GetVarFromStdDefaultFound,
    GetVarFromStdDefaultAvailiable
}GET_VAR_FROM_STD_DEFAULT_STEP;

#pragma pack(1)
typedef struct
{
    UINT8                   Step;
    NVRAM_STORE_INFO        StdDefaultInfo;
    EFI_PEI_GET_VARIABLE2   SavedPeiGetVariable;
    UINT8                   StdDefaultData;
}GET_VAR_FROM_STD_DEFAULT_HOB;
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