/*++

Copyright (c)  1999 - 2005 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

Module Name:

  CpuType.h

Abstract:

--*/

#ifndef _CPU_TYPE_H
#define _CPU_TYPE_H

#pragma pack(1)

typedef enum {
  EnumCpuUarchUnknown = 0,
  EnumNehalemUarch,
} EFI_CPU_UARCH;

typedef enum {
  EnumCpuPlatformUnknown = 0,
  EnumDesktop,
  EnumMobile,
  EnumServer,
  EnumNetTop
} EFI_CPU_PLATFORM;

typedef enum {
  EnumCpuTypeUnknown = 0,
  EnumAtom,
  EnumNehalemEx,
  EnumBloomfield,
  EnumGainestown,
  EnumHavendale,
  EnumLynnfield,
  EnumAuburndale,
  EnumClarksfield,
  EnumPineview,
  EnumCedarview,
  EnumValleyview,
  EnumClarkdale // Havendale 32nm
} EFI_CPU_TYPE;

typedef enum {
  EnumCpuFamilyUnknown = 0,
  EnumFamilyField,
  EnumFamilyDale
} EFI_CPU_FAMILY;

#pragma pack()

#endif
