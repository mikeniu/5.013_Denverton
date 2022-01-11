/*++
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
--*/
/** @file
  System controller comunication library header file

  Copyright (c) 2010-2015, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

**/
#ifndef SYS_CTRL_COM_LIB
#define SYS_CTRL_COM_LIB


//
#include "PlatformDefinitions.h"
#include "PchAccess.h"
#include "CpuRegs.h"
//#include "Platform.h"
#include <Ppi/Stall.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/MmPciLib.h>
//

//
// Uncoment if wants to disable FPGA comunication
//
//#define SYS_CTRL_COM_DISABLED

#define VF_SC_SMBUS_ADDRESS        0x82
#define VF_SC_OP_WRITE             0x50
#define VF_SC_OP_READ              0x51

#define R_SYSCTRL_BIOSVERSION        0x30
#define R_SYSCTRL_FPGAVERSION_BUILD  0x0C
#define R_SYSCTRL_TRIDENT_ID_A       0x80
#define R_SYSCTRL_TRIDENT_ID_B       0xA0
#define R_SYSCTRL_CFIO_FUNC_SEL      0x310
#define R_SYSCTRL_GPIO_EXP           0x36018

#define SYSCTL_CFIO_FUNC_LENGTH      18

EFI_STATUS
SysCtrlReadWord(UINT32 Offset,
                UINT8 *Data);

EFI_STATUS
SysCtrlReadDword(UINT32 Offset,
                 UINT32 *Data);

EFI_STATUS
SysCtrlWriteWord(UINT32 Offset,
                 UINT16 Data);

#endif
