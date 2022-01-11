/** @file
  MrcFunc.c
  Utility MRC function.
 
  Copyright (c) 2005-2017 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

**/

#include "McFunc.h"
#include "MrcFunc.h"
#include "ConfigMem.h"
#include "OemHooks.h"

/**
  Returns the position of the least significant bit set in the input parameter.
  If the input value is zero, the output is undefined.

  @param[in]       Input         The value to be scanned 
  @retval                        Bit position
**/
#pragma intrinsic(_BitScanForward)
UINT8
BitScanForward8 (
  IN        UINT8   Input
  )
{
#ifdef SIM
  UINT8 i;
  for (i = 0; (i < 8) && ((Input & (1 << i) ) == 0); i++);
  return i;
#else
  UINT32 Index;
  _BitScanForward (&Index, Input);
  return (UINT8) Index;
#endif
}

/**
  Returns the position of the most significant bit set in the input parameter.
  If the input value is zero, the output is undefined.

  @param[in]       Input         The value to be scanned 
  @retval                        Bit position
**/
#pragma intrinsic(_BitScanReverse)
INT8
BitScanReverse32 (
  IN        UINT32   Input
  )
{
#ifdef SIM
  INT8 i;
  for (i = 31; (i >= 0) && ((Input & (1 << i) ) == 0); i--);
  return i;
#else
  UINT32 Index;
  _BitScanReverse (&Index, Input);
  return (UINT8) Index;
#endif
}

MMRC_STATUS
FillFastBootDataStructure (
  IN  OUT   MMRC_DATA   *MrcData
)
{
  UINT8     CurrentDimm;
  UINT8     Channel;

  for (Channel = 0; Channel < MAX_CHANNELS; Channel++) {
    for (CurrentDimm = 0; CurrentDimm < MAX_DIMMS; CurrentDimm++) {
      if (MrcData->MrcParameters.DramType[Channel] <= TypeDdr3All) {
        MrcData->FastBootDataCurrent.ManuIDlo[Channel][CurrentDimm]      = MrcData->MrcParameters.SpdData[Channel][CurrentDimm].Buffer[SPD_DDR3_MANUFACTURER_ID_LO  ];
        MrcData->FastBootDataCurrent.ManuIDhi[Channel][CurrentDimm]      = MrcData->MrcParameters.SpdData[Channel][CurrentDimm].Buffer[SPD_DDR3_MANUFACTURER_ID_HI  ];
        MrcData->FastBootDataCurrent.ManuLoc[Channel][CurrentDimm]       = MrcData->MrcParameters.SpdData[Channel][CurrentDimm].Buffer[SPD_DDR3_MANUFACTURE_LOCATION];
        MrcData->FastBootDataCurrent.ManuDateLO[Channel][CurrentDimm]    = MrcData->MrcParameters.SpdData[Channel][CurrentDimm].Buffer[SPD_DDR3_MANUFACTURE_DATE_LO ];
        MrcData->FastBootDataCurrent.ManuDateHI[Channel][CurrentDimm]    = MrcData->MrcParameters.SpdData[Channel][CurrentDimm].Buffer[SPD_DDR3_MANUFACTURE_DATE_HI ];
        MrcData->FastBootDataCurrent.SerialNumber1[Channel][CurrentDimm] = MrcData->MrcParameters.SpdData[Channel][CurrentDimm].Buffer[SPD_DDR3_SERIAL_NUMBER_1     ];
        MrcData->FastBootDataCurrent.SerialNumber2[Channel][CurrentDimm] = MrcData->MrcParameters.SpdData[Channel][CurrentDimm].Buffer[SPD_DDR3_SERIAL_NUMBER_2     ];
        MrcData->FastBootDataCurrent.SerialNumber3[Channel][CurrentDimm] = MrcData->MrcParameters.SpdData[Channel][CurrentDimm].Buffer[SPD_DDR3_SERIAL_NUMBER_3     ];
        MrcData->FastBootDataCurrent.SerialNumber4[Channel][CurrentDimm] = MrcData->MrcParameters.SpdData[Channel][CurrentDimm].Buffer[SPD_DDR3_SERIAL_NUMBER_4     ];
        MrcData->FastBootDataCurrent.SdramWidth[Channel][CurrentDimm]    = MrcData->MrcParameters.SpdData[Channel][CurrentDimm].Buffer[SPD_DDR3_ORG                 ];
      } else if (MrcData->MrcParameters.DramType[Channel] == TypeDdr4) {
        MrcData->FastBootDataCurrent.ManuIDlo[Channel][CurrentDimm]      = MrcData->MrcParameters.SpdData[Channel][CurrentDimm].Buffer[SPD_DDR4_MMIDL   ];
        MrcData->FastBootDataCurrent.ManuIDhi[Channel][CurrentDimm]      = MrcData->MrcParameters.SpdData[Channel][CurrentDimm].Buffer[SPD_DDR4_MMIDH   ];
        MrcData->FastBootDataCurrent.ManuLoc[Channel][CurrentDimm]       = MrcData->MrcParameters.SpdData[Channel][CurrentDimm].Buffer[SPD_DDR4_MMLOC   ];
        MrcData->FastBootDataCurrent.ManuDateLO[Channel][CurrentDimm]    = MrcData->MrcParameters.SpdData[Channel][CurrentDimm].Buffer[SPD_DDR4_MMDATE  ];
        MrcData->FastBootDataCurrent.ManuDateHI[Channel][CurrentDimm]    = MrcData->MrcParameters.SpdData[Channel][CurrentDimm].Buffer[SPD_DDR4_MMDATE  ];
        MrcData->FastBootDataCurrent.SerialNumber1[Channel][CurrentDimm] = MrcData->MrcParameters.SpdData[Channel][CurrentDimm].Buffer[SPD_DDR4_MODULESN];
        MrcData->FastBootDataCurrent.SerialNumber2[Channel][CurrentDimm] = MrcData->MrcParameters.SpdData[Channel][CurrentDimm].Buffer[SPD_DDR4_MODULEPN];
        MrcData->FastBootDataCurrent.SerialNumber3[Channel][CurrentDimm] = MrcData->MrcParameters.SpdData[Channel][CurrentDimm].Buffer[SPD_DDR4_MODULERC];
        MrcData->FastBootDataCurrent.SerialNumber4[Channel][CurrentDimm] = MrcData->MrcParameters.SpdData[Channel][CurrentDimm].Buffer[SPD_DDR4_REV     ];
        MrcData->FastBootDataCurrent.SdramWidth[Channel][CurrentDimm]    = MrcData->MrcParameters.SpdData[Channel][CurrentDimm].Buffer[SPD_DDR4_ORG     ];
      }
    } // CurrentDimm loop ...
    MrcData->FastBootDataCurrent.DimmCount[Channel]                      = MrcData->DimmCount[Channel];
    MrcData->FastBootDataCurrent.DdrFrequency[Channel]                   = MrcData->MrcParameters.DdrFrequency;

  } // Channel loop ...

  return MMRC_SUCCESS;
}

MMRC_STATUS
FillOutputStructure (
  IN  OUT   MMRC_DATA   *MrcData
)
{
  UINT8 FastBootParameter;
  UINT8 *CurrentFastBootData;
  UINT8 *NextFastBootData;

  CurrentFastBootData = (UINT8 *) &MrcData->FastBootDataCurrent;
  NextFastBootData    = (UINT8 *) &MrcData->MrcParameters.SaveRestore.FastBootData;

  for (FastBootParameter = 0; FastBootParameter < sizeof (FASTBOOTDATA); FastBootParameter++) {
    *(NextFastBootData + FastBootParameter) = *(CurrentFastBootData + FastBootParameter);
  } // FastBootParameter loop ...
  return MMRC_SUCCESS;
}

