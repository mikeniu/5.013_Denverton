/** @file
  Pch information string library.

  All function in this library is available for PEI, DXE, and SMM,
  But do not support UEFI RUNTIME environment call.

@copyright
 Copyright (c) 2016 - 2017 Intel Corporation. All rights reserved
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
#include <Base.h>
#include <Uefi/UefiBaseType.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/MmPciLib.h>
#include <Library/PchInfoLib.h>
#include <PchAccess.h>

/**
  Structure for PCH stepping string mapping
**/
struct PCH_STEPPING_STRING {
  PCH_STEPPING  Stepping;
  CHAR8         *String;
};

GLOBAL_REMOVE_IF_UNREFERENCED
struct PCH_STEPPING_STRING mSteppingStrs[] = {
  {PchHA0,  "A0"},
  {PchHA1,  "A1"},
  {PchHB0,  "B0"},
  {PchHB1,  "B1"},
  {PchHC0,  "C0"},
  {PchHD0,  "D0"},
  {PchHD1,  "D1"},
#ifdef SIMICS_FLAG
  {PchLpA0, "A0"},
#endif
  {PchLpB0, "B0"},
  {PchLpB1, "B1"},
  {PchLpC0, "C0"},
  {PchLpC1, "C1"},
  {PchSteppingMax, NULL}
};

/**
  Structure for PCH series string mapping
**/
struct PCH_SERIES_STRING {
  PCH_SERIES    Series;
  CHAR8         *String;
};

GLOBAL_REMOVE_IF_UNREFERENCED
struct PCH_SERIES_STRING mSeriesStrs[] = {
  {PchH,  "SKL PCH-H"},
  {PchLp, "SKL PCH-LP"},
  {PchUnknownSeries, NULL}
};

/**
  Structure for PCH sku string mapping
**/
struct PCH_SKU_STRING {
  UINT16        Id;
  CHAR8         *String;
};

GLOBAL_REMOVE_IF_UNREFERENCED
struct PCH_SKU_STRING mSkuStrs[] = {
  //
  // SKL PCH H Desktop LPC Device IDs
  //
  {V_PCH_H_LPC_DEVICE_ID_DT_SUPER_SKU, "Super SKU"},
  {V_PCH_H_LPC_DEVICE_ID_DT_0, "Super SKU (locked)"},
  {V_PCH_H_LPC_DEVICE_ID_DT_1, "H110"},
  {V_PCH_H_LPC_DEVICE_ID_DT_2, "H170"},
  {V_PCH_H_LPC_DEVICE_ID_DT_3, "Z170"},
  {V_PCH_H_LPC_DEVICE_ID_DT_4, "Q170"},
  {V_PCH_H_LPC_DEVICE_ID_DT_5, "Q150"},
  {V_PCH_H_LPC_DEVICE_ID_DT_6, "B150"},
  {V_PCH_H_LPC_DEVICE_ID_UNFUSE, "Unfused SKU"},
  //
  // SKL PCH H Server/WS LPC Device IDs
  //
  {V_PCH_H_LPC_DEVICE_ID_SVR_0, "C236"},
  {V_PCH_H_LPC_DEVICE_ID_SVR_1, "C232"},
  {V_PCH_H_LPC_DEVICE_ID_SVR_2, "CM236"},
  {V_PCH_H_LPC_DEVICE_ID_A14B, "Super SKU (Unlocked)"},
  //
  // SKL PCH H Mobile LPC Device IDs
  //
  {V_PCH_H_LPC_DEVICE_ID_MB_0, "QM170"},
  {V_PCH_H_LPC_DEVICE_ID_MB_1, "HM170"},
  {V_PCH_H_LPC_DEVICE_ID_MB_2, "QMS170"},
  {V_PCH_H_LPC_DEVICE_ID_MB_SUPER_SKU, "Super SKU"},
  //
  // SKL PCH LP Mobile LPC Device IDs
  //
  {V_PCH_LP_LPC_DEVICE_ID_UNFUSE, "Unfused SKU"},
  {V_PCH_LP_LPC_DEVICE_ID_MB_SUPER_SKU, "Super SKU"},
  {V_PCH_LP_LPC_DEVICE_ID_MB_0, "Super SKU (locked)"},
  {V_PCH_LP_LPC_DEVICE_ID_MB_1, "(U) Base SKU"},
  {V_PCH_LP_LPC_DEVICE_ID_MB_2, "(Y) Premium SKU"},
  {V_PCH_LP_LPC_DEVICE_ID_MB_3, "(U) Premium  SKU"},
  {0xFFFF, NULL}
};

/**
  Get PCH stepping ASCII string
  The return string is zero terminated.

  @param [in]      PchStep              Pch stepping
  @param [out]     Buffer               Output buffer of string
  @param [in,out]  BufferSize           Size of input buffer,
                                        and return required string size when buffer is too small.

  @retval EFI_SUCCESS                   String copy successfully
  @retval EFI_INVALID_PARAMETER         The stepping is not supported, or parameters are NULL
  @retval EFI_BUFFER_TOO_SMALL          Input buffer size is too small
**/
EFI_STATUS
PchGetSteppingStr (
  IN     PCH_STEPPING                   PchStep,
  OUT    CHAR8                          *Buffer,
  IN OUT UINT32                         *BufferSize
  )
{
  UINTN  Index;
  UINT32 StrLength;
  CHAR8  *Str;

  if ((Buffer == NULL) || (BufferSize == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  if (*BufferSize > 0) {
    Buffer[0] = 0;
  }

  Str = NULL;
  StrLength = 0;
  for (Index = 0; mSteppingStrs[Index].Stepping != PchSteppingMax; Index++) {
    if (PchStep == mSteppingStrs[Index].Stepping) {
      StrLength = (UINT32) AsciiStrLen (mSteppingStrs[Index].String);
      Str       = mSteppingStrs[Index].String;
      break;
    }
  }
  if (StrLength == 0) {
    // Unsupported Stepping
    // ASSERT (FALSE);
    return EFI_INVALID_PARAMETER;
  }
  if (*BufferSize <= StrLength) {
    *BufferSize = StrLength + 1;
    return EFI_BUFFER_TOO_SMALL;
  }
  AsciiStrCpyS(Buffer, *BufferSize, Str);
  return EFI_SUCCESS;
}

/**
  Get PCH series ASCII string
  The return string is zero terminated.

  @param [in]      PchSeries            Pch series
  @param [out]     Buffer               Output buffer of string
  @param [in,out]  BufferSize           Size of input buffer,
                                        and return required string size when buffer is too small.

  @retval EFI_SUCCESS                   String copy successfully
  @retval EFI_INVALID_PARAMETER         The series is not supported, or parameters are NULL
  @retval EFI_BUFFER_TOO_SMALL          Input buffer size is too small
**/
EFI_STATUS
PchGetSeriesStr (
  IN     PCH_SERIES                     PchSeries,
  OUT    CHAR8                          *Buffer,
  IN OUT UINT32                         *BufferSize
  )
{
  UINTN  Index;
  UINT32 StrLength;
  CHAR8  *Str;

  if ((Buffer == NULL) || (BufferSize == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  if (*BufferSize > 0) {
    Buffer[0] = 0;
  }

  Str = NULL;
  StrLength = 0;
  for (Index = 0; mSeriesStrs[Index].Series != PchUnknownSeries; Index++) {
    if (PchSeries == mSeriesStrs[Index].Series) {
      StrLength = (UINT32) AsciiStrLen (mSeriesStrs[Index].String);
      Str       = mSeriesStrs[Index].String;
      break;
    }
  }
  if (StrLength == 0) {
    // Unsupported Series
    // ASSERT (FALSE);
    return EFI_INVALID_PARAMETER;
  }
  if (*BufferSize <= StrLength) {
    *BufferSize = StrLength + 1;
    return EFI_BUFFER_TOO_SMALL;
  }
  AsciiStrCpyS(Buffer, *BufferSize, Str);
  return EFI_SUCCESS;
}

/**
  Get PCH Sku ASCII string
  The return string is zero terminated.

  @param [in]      LpcDid               LPC device id
  @param [out]     Buffer               Output buffer of string
  @param [in,out]  BufferSize           Size of input buffer,
                                        and return required string size when buffer is too small.

  @retval EFI_SUCCESS                   String copy successfully
  @retval EFI_INVALID_PARAMETER         The series is not supported, or parameters are NULL
  @retval EFI_BUFFER_TOO_SMALL          Input buffer size is too small
**/
EFI_STATUS
PchGetSkuStr (
  IN     UINT16                         LpcDid,
  OUT    CHAR8                          *Buffer,
  IN OUT UINT32                         *BufferSize
  )
{
  UINTN  Index;
  UINT32 StrLength;
  CHAR8  *Str;

  if ((Buffer == NULL) || (BufferSize == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  if (*BufferSize > 0) {
    Buffer[0] = 0;
  }

  Str = NULL;
  StrLength = 0;
  for (Index = 0; mSkuStrs[Index].Id != 0xFFFF; Index++) {
    if (LpcDid == mSkuStrs[Index].Id) {
      StrLength = (UINT32) AsciiStrLen (mSkuStrs[Index].String);
      Str       = mSkuStrs[Index].String;
    }
  }
  if (StrLength == 0) {
    // Unsupported Sku
    // ASSERT (FALSE);
    return EFI_INVALID_PARAMETER;
  }
  if (*BufferSize <= StrLength) {
    *BufferSize = StrLength + 1;
    return EFI_BUFFER_TOO_SMALL;
  }
  AsciiStrCpyS(Buffer, *BufferSize, Str);
  return EFI_SUCCESS;
}
