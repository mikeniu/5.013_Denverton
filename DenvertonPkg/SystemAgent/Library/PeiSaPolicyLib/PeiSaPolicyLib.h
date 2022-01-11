/** @file
  Header file for the PeiSaPolicy library.

@copyright
 Copyright (c) 2014 Intel Corporation. All rights reserved
 This software and associated documentation (if any) is furnished
 under a license and may only be used or copied in accordance
 with the terms of the license. Except as permitted by the
 license, no part of this software or documentation may be
 reproduced, stored in a retrieval system, or transmitted in any
 form or by any means without the express written consent of
 Intel Corporation.
 This file contains an 'Intel Peripheral Driver' and is uniquely
 identified as "Intel Reference Module" and is licensed for Intel
 CPUs and chipsets under the terms of your license agreement with
 Intel or your vendor. This file may be modified by the user, subject
 to additional terms of the license agreement.

@par Specification Reference:
**/
#ifndef _PEI_SA_POLICY_LIB_H_
#define _PEI_SA_POLICY_LIB_H_

#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PcdLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/MmPciLib.h>
#include <Ppi/SaPolicy.h>
#include <SaDataHob.h>
#include <SaAccess.h>
#include <CpuRegs.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/SmbusLib.h>

#define SaIoRead8    IoRead8
#define SaIoRead16   IoRead16
#define SaIoRead32   IoRead32
#define SaIoWrite8   IoWrite8
#define SaIoWrite16  IoWrite16
#define SaIoWrite32  IoWrite32
#define SaCopyMem    CopyMem
#define SaSetMem     SetMem
#define SaLShiftU64  LShiftU64
#define SaRShiftU64  RShiftU64
#define SaMultU64x32 MultU64x32

#define RTC_INDEX_REGISTER        (0x70)
#define RTC_TARGET_REGISTER       (0x71)
#define R_PCH_RTC_INDEX_ALT       (0x74)
#define R_PCH_RTC_TARGET_ALT      (0x75)
#define R_PCH_RTC_EXT_INDEX_ALT   (0x76)
#define R_PCH_RTC_EXT_TARGET_ALT  (0x77)

#define RTC_INDEX_MASK            (0x7F)
#define RTC_BANK_SIZE             (0x80)

#define RTC_SECONDS               (0x00)
#define RTC_MINUTES               (0x02)
#define RTC_HOURS                 (0x04)
#define RTC_DAY_OF_MONTH          (0x07)
#define RTC_MONTH                 (0x08)
#define RTC_YEAR                  (0x09)
#define CMOS_REGA                 (0x0A)
#define CMOS_REGB                 (0x0B)
#define CMOS_REGC                 (0x0C)
#define CMOS_REGD                 (0x0D)

#define RTC_UPDATE_IN_PROGRESS    (0x80)
#define RTC_HOLD                  (0x80)
#define RTC_MODE_24HOUR           (0x02)
#define RTC_CLOCK_DIVIDER         (0x20)
#define RTC_RATE_SELECT           (0x06)

#define BCD2BINARY(A)             (((((A) >> 4) & 0xF) * 10) + ((A) & 0xF))
#define CENTURY_OFFSET            (2000)

#endif // _PEI_SA_POLICY_LIB_H_
