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

/** @file IpmiIeSelfTestLogProtocol.h
    IPMI Self test log protocol definitions

**/

#ifndef _IPMI_IE_SELFTEST_H_
#define _IPMI_IE_SELFTEST_H_

#include <Include/IpmiIeNetFnAppDefinitions.h>

//
// Computing Unit Firmware Processor Subclass Error Code definitions.
//
#define EFI_CU_FP_EC_HARD_FAIL                        (EFI_SUBCLASS_SPECIFIC | 0x00000000)
#define EFI_CU_FP_EC_SOFT_FAIL                        (EFI_SUBCLASS_SPECIFIC | 0x00000001)
#define EFI_CU_FP_EC_COMM_ERROR                       (EFI_SUBCLASS_SPECIFIC | 0x00000002)
#define EFI_CU_FP_EC_SEL_NOT_ACCESSIBLE               (EFI_SUBCLASS_SPECIFIC | 0x00000003)
#define EFI_CU_FP_EC_SDR_REPOSITORY_NOT_ACCESSABLE    (EFI_SUBCLASS_SPECIFIC | 0x00000004)
#define EFI_CU_FP_EC_IE_FRU_NOT_ACCESSIBLE            (EFI_SUBCLASS_SPECIFIC | 0x00000005)
#define EFI_CU_FP_EC_IPMB_NOT_RESPONDING              (EFI_SUBCLASS_SPECIFIC | 0x00000006)
#define EFI_CU_FP_EC_SDR_EMPTY                        (EFI_SUBCLASS_SPECIFIC | 0x00000007)
#define EFI_CU_FP_EC_IE_FRU_CORRUPTED                 (EFI_SUBCLASS_SPECIFIC | 0x00000008)
#define EFI_CU_FP_EC_BB_CORRUPTED                     (EFI_SUBCLASS_SPECIFIC | 0x00000009)
#define EFI_CU_FP_EC_FIRMWARE_CORRUPTED               (EFI_SUBCLASS_SPECIFIC | 0x0000000a)
#define EFI_CU_FP_EC_FORCE_UPDATE_MODE                (EFI_SUBCLASS_SPECIFIC | 0x0000000b)  // APTIO 3.0 Features into 3.5 Changes

#define BST_LOG_SIZE     20

#endif

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
