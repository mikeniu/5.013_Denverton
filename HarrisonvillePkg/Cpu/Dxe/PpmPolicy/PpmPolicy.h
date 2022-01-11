/*++

Copyright (c)  1999 - 2010 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

Module Name:

  PpmPolicy.h

Abstract:

  Header file for the PpmPolicyInitDxe Driver.

--*/
#include <PiDxe.h>
//
// Driver Produced Protocol Prototypes
//
#include <Protocol/PpmPlatformPolicy.h>

PPM_PLATFORM_POLICY_PROTOCOL    mDxePlatformPpmPolicy;

// Function Definition
#define  ICH_DEVICE_ENABLE       1
#define  ICH_DEVICE_DISABLE      0

EFI_BOOT_SERVICES     *gBS;
EFI_BOOT_SERVICES     *pBS;
EFI_RUNTIME_SERVICES  *pRS;

#define POWER_STATE_SWITCH_SMI                       43
#define ENABLE_C_STATE_IO_REDIRECTION_SMI            70
#define DISABLE_C_STATE_IO_REDIRECTION_SMI           71
#define ENABLE_SMI_C_STATE_COORDINATION_SMI          72
#define DISABLE_SMI_C_STATE_COORDINATION_SMI         73
#define ENABLE_P_STATE_HARDWARE_COORDINATION_SMI     74
#define DISABLE_P_STATE_HARDWARE_COORDINATION_SMI    75
#define S3_RESTORE_MSR_SW_SMI                        48
#define ENABLE_C6_RESIDENCY_SMI                      76

#define CSTATE_DISABLE                               0
#define C4_STATE                                     1
#define C6_NO_SHRINK                                 2
#define C6_INCREMENTAL_SHRINK                        3
#define C6_FULL_SHRINK                               4

#define CUNIT                                        0x08
#define CUNIT_OPCODE_READ                            0x10

#define	CUNIT_MANUFACTURING_ID_SB	                 0x003E             // offset for sideband transactions
#define CUNIT_MANUFACTURING_ID_BIT_7_0_MASK          0x000000FF
#define AVT_A0_STEPPING                              0x00
#define AVT_A1_STEPPING                              0x01
#define AVT_B0_STEPPING                              0x02

