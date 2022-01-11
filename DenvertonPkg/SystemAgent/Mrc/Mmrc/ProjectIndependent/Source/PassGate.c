/**  @file
  PassGate.c
  Pass Gate libraries used throughout the MMRC, these files are
  independent of the project.

  Copyright (c) 1999 - 2015 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

**/

#include "PassGate.h"


UINT8 
AntiLogBase2 (
  IN          UINT32    num
  ) 
{
  UINT8 PowerOf2;

  PowerOf2 = 0;

  if (num == 0) {
    return 0xff;
  }

  while ((num >>= 1) > 0) {
    PowerOf2++;
  }

  return PowerOf2;
}

#if PASS_GATE_TEST == 1
MMRC_STATUS
InitInternalCommonVariables (
  IN  OUT     MMRC_DATA         *MrcData,
  IN          UINT8             Channel
  )
{
  MMRC_STATUS     MmrcStatus;
  //
  // Get the information related with the Dimm
  //
  MmrcStatus = GetDimmGeometry (MrcData, Channel);
  if (MmrcStatus != MMRC_SUCCESS) {
    return MrcErrSetupErrorPassGate;
  }
  //
  // Internal variables to adjust delay during the characterization reads
  //
  MrcData->PassGateParameters.ActivateAdjust          = 0;
  //
  // Overrides depending on the Type (DDR3/DDR4)
  //
  switch (MrcData->MrcParameters.DramType[Channel]) {
  case TypeDdr3:
  case TypeDdr3L:
  case TypeDdr3U:
  case TypeDdr3All:
    MrcData->PassGateParameters.BitsPerColumns        = 10;
    MrcData->PassGateParameters.BitsPerBank           = 3;
    MrcData->PassGateParameters.BitsPerBankGroup      = 0;
    MrcData->PassGateParameters.Burst                 = 128;
    //
    // Overrides depending on the Density
    //
    switch (MrcData->PassGateParameters.DeviceDensity) {
    case 0:
      // 
      // 1Gb Devices 
      //
      MrcData->PassGateParameters.BitsPerRow          = 14;
      break;
    case 1:
      // 
      // 2Gb Devices 
      //
      MrcData->PassGateParameters.BitsPerRow          = 15;
      break;
    case 2:
      // 
      // 4Gb Devices 
      //
      MrcData->PassGateParameters.BitsPerRow          = 16;
      break;
    case 3:
      //
      // 8Gb Devices
      //
      MrcData->PassGateParameters.BitsPerColumns      = 11;
      MrcData->PassGateParameters.BitsPerRow          = 16;
      MrcData->PassGateParameters.Burst               = 256;
      break;
    default:
      break;
    } // MrcData->PassGateParameters.DeviceDensity
    //
    // Overrides Data Width 
    //
    switch (MrcData->PassGateParameters.DeviceDataWidth) {
    case 0:
      //
      // x4 Devices
      //
      MrcData->PassGateParameters.BitsPerColumns      += 1;
      MrcData->PassGateParameters.Burst               = MrcData->PassGateParameters.Burst << 2;
      break;
    case 1:
      //
      // x8 Devices
      //
      break;
    default:
      MspDebugPrint ((MSP_DBG_MIN, "Device Width is not supported\n"));
      return MrcErrSetupErrorPassGate;
      break;

    } // MrcData->PassGateParameters.DeviceDataWidth
    break;
  case TypeDdr4:
    MrcData->PassGateParameters.BitsPerColumns        = 10;
    MrcData->PassGateParameters.BitsPerBank           = 2;
    MrcData->PassGateParameters.BitsPerBankGroup      = 2;
    MrcData->PassGateParameters.Burst                 = 128;
    //
    // Overrides depending on the Density
    //
    switch (MrcData->PassGateParameters.DeviceDensity) {
    case 1:
      // 
      // 2Gb Devices 
      //
      MrcData->PassGateParameters.BitsPerRow          = 14;
      break;
    case 2:
      // 
      // 4Gb Devices 
      //
      MrcData->PassGateParameters.BitsPerRow          = 15;
      break;
    case 3:
      //
      // 8Gb Devices
      //
      MrcData->PassGateParameters.BitsPerRow          = 16;
      break;
    case 4:
      //
      // 16Gb Devices
      //
      MrcData->PassGateParameters.BitsPerRow          = 17;
      break;
    default:
      break;
    }
    //
    // Overrides Data Width 
    //
    switch (MrcData->PassGateParameters.DeviceDataWidth) {
    case 0:
      //
      // x4 Devices
      //
      MrcData->PassGateParameters.BitsPerRow         += 1;
      break;
    case 1:
      //
      // x8 Devices
      //
      break;
    default:
      MspDebugPrint ((MSP_DBG_MIN, "Device Width is not supported\n"));
      return MrcErrSetupErrorPassGate;
      break;
    }
    break;
  default:
    MspDebugPrint ((MSP_DBG_MIN, "Wrong DDR Type\n"));
    return MrcErrSetupErrorPassGate;
    break;
  }

  MrcData->PassGateParameters.RowMask               = ((1 << MrcData->PassGateParameters.BitsPerRow) - 1);
  MrcData->PassGateParameters.RowBankTrackerMask    = ((1 << (MrcData->PassGateParameters.BitsPerBank + MrcData->PassGateParameters.BitsPerBankGroup + MrcData->PassGateParameters.BitsPerRow)) - 1);
  MrcData->PassGateParameters.RowBankTrackerMask   &= ~(MrcData->PassGateParameters.SpeedSelect - 1);

  MrcData->PassGateParameters.BankGroupOffset       = MrcData->PassGateParameters.BitsPerColumns + 3;
  MrcData->PassGateParameters.BankOffset            = MrcData->PassGateParameters.BankGroupOffset + MrcData->PassGateParameters.BitsPerBankGroup;
  MrcData->PassGateParameters.RowOffset             = MrcData->PassGateParameters.BankOffset + MrcData->PassGateParameters.BitsPerBank;
  MrcData->PassGateParameters.RankOffset            = MrcData->PassGateParameters.RowOffset + MrcData->PassGateParameters.BitsPerRow;

  MrcData->PassGateParameters.Burst                 = MrcData->PassGateParameters.Burst << AntiLogBase2 (MrcData->PassGateParameters.SpeedSelect);

#if SIM
  MrcData->PassGateParameters.Burst                 = 2;
#endif // SIM

  return MMRC_SUCCESS;
}

/**
  Gets the Row Bank Mapping

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Channel         Current Channel being examined.
  @param[in]       RowBankTracker  Current Bank & Row under test.
  @param[out]      Row             Pointer to return the value of the Row.
  @param[out]      Bank            Pointer to return the value of the Bank.
  @param[out]      BankGroup       Pointer to return the value of the Bank Group.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
GetRowBankMapping (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         Channel,
  IN        UINT32        RowBankTracker,
      OUT   UINT32        *Row,
      OUT   UINT8         *Bank,
      OUT   UINT8         *BankGroup
  )
{
  
  if (BankGroup != NULL && MrcData->PassGateParameters.BitsPerBankGroup != 0) {
    *BankGroup  = RowBankTracker & ((1 << (MrcData->PassGateParameters.BitsPerBankGroup)) - 1);
  }
  if (Bank != NULL) {
    *Bank       = RowBankTracker & ((1 << (MrcData->PassGateParameters.BitsPerBankGroup + MrcData->PassGateParameters.BitsPerBank)) - 1);
    *Bank       = *Bank >> MrcData->PassGateParameters.BitsPerBankGroup;
  }
  if (Row != NULL) {
    *Row        = (RowBankTracker & MrcData->PassGateParameters.RowBankTrackerMask) >> (MrcData->PassGateParameters.BitsPerBank + MrcData->PassGateParameters.BitsPerBankGroup);
    *Row        = *Row & MrcData->PassGateParameters.RowMask;
  }

  return MMRC_SUCCESS;
}


/**
  Adds/Removes swizzle from an specific address
  if BIT3 and Dimm is swizzle:
    BIT2 = BIT3 ^ BIT2
    BIT1 = BIT3 ^ BIT1

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Address         Current Channel being examined.
  @param[in]       Dimm            Current Dimm being examined.

  @retval          ToggleAddress
**/
UINT32 
ToggleDramDimmAddress (
  IN        MMRC_DATA     *MrcData,
  IN        UINT32        Address,
  IN        UINT8         Dimm
  )
{
  UINT32 ToggleAddress;

  ToggleAddress = Address;
  if ((Address & BIT3) && (MrcData->PassGateParameters.DimmSwizzle[Dimm])) {
    ToggleAddress  = Address & (~0x6);
    ToggleAddress |= (~Address) & 0x6;
  }

  return ToggleAddress;
}

UINT32 
GetCompositeAddress (
  IN        MMRC_DATA     *MrcData,
  IN        UINT8         Dimm,
  IN        UINT8         Rank,
  IN        UINT32        Row,
  IN        UINT8         Bank,
  IN        UINT8         BankGroup
  )
{
  UINT32  Address;

  Address     = (Bank << MrcData->PassGateParameters.BankOffset);
  if (MrcData->PassGateParameters.BitsPerBankGroup != 0) {
    Address  |= (BankGroup << (MrcData->PassGateParameters.BankGroupOffset));
  }
  Address    |= (((Dimm << 1) | (Rank << 0)) << MrcData->PassGateParameters.RankOffset);
  Address    |= (Row << MrcData->PassGateParameters.RowOffset);

  return Address;
}

/**
  Gets the Cpgc Addresses for the aggressor and 2 victims in the DRAM device
  - Get DIMM ADDR from Logical ADDR for aggressor (optional mirrored)
  - Get DRAM ADDR from DIMM ADDR [optional Swizzled]
  - Get DRAM ADDR +/- 1 (Victims)
  - Get DIMM ADDR from victims DRAM ADDR (Remove swizzle if needed)
  - Get Logical ADDR for victims (optional mirrored)

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Channel         Current Channel being examined.
  @param[in]       Dimm            Current Dimm being examined.
  @param[in]       Rank            Current Rank being examined.
  @param[in]       RowBankTracker  Current Bank & Row under test.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
AddressSetup (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         Channel,
  IN        UINT8         Dimm,
  IN        UINT8         Rank,
  IN        UINT32        RowBankTracker
  )
{
  UINT32  Row;
  UINT8   Bank;
  UINT8   BankGroup;

  UINT32  RowDimm;

  UINT32  RowDram;
  UINT32  RowDramPlus1;
  UINT32  RowDramMinus1;

  UINT32  RowDimmPlus1;
  UINT32  RowDimmMinus1;

  UINT32  RowPlus1;
  UINT32  RowMinus1;

  UINT32  CpgcAggressorAddress;
  UINT32  CpgcVictimAddress[2];
  
  //
  // Get the logical Row (Aggressor) and 
  // Get the mirrored address for the Aggressor (only if required)
  //
  GetRowBankMapping (MrcData, Channel, RowBankTracker, &Row, &Bank, &BankGroup);

  if (MrcData->PassGateParameters.DimmMirrored[Dimm] == 1 && (Rank % 2) != 0) {
    RowDimm = MirrorAddrBitsPassGate (MrcData, Channel, Row);
  } else {
    RowDimm = Row;
  }
  // 
  // Get the dram address, first we need to determine if it could be swizzled or not
  //
  RowDram = ToggleDramDimmAddress (MrcData, RowDimm, Dimm);
  // 
  // Get the 2 victim addresses at the dram (+-1)
  //
  RowDramPlus1  = (RowDram + 1) & MrcData->PassGateParameters.RowMask;
  RowDramMinus1 = (RowDram - 1) & MrcData->PassGateParameters.RowMask;
  //
  // Get the 2 victim addresses at the Dimm (remove swizzle mapping if required)
  //
  RowDimmPlus1  = ToggleDramDimmAddress (MrcData, RowDramPlus1, Dimm);
  RowDimmMinus1 = ToggleDramDimmAddress (MrcData, RowDramMinus1, Dimm);
  //
  // Get the 2 logical victim addresses
  //
  if (MrcData->PassGateParameters.DimmMirrored[Dimm] == 1 && (Rank % 2) != 0) {
    RowPlus1  = MirrorAddrBitsPassGate (MrcData, Channel, RowDimmPlus1);
    RowMinus1 = MirrorAddrBitsPassGate (MrcData, Channel, RowDimmMinus1);
  } else {
    RowPlus1  = RowDimmPlus1;
    RowMinus1 = RowDimmMinus1;
  }

  CpgcAggressorAddress  = GetCompositeAddress (MrcData, Dimm, Rank, Row, Bank, BankGroup);
  CpgcVictimAddress[0]  = GetCompositeAddress (MrcData, Dimm, Rank, RowPlus1, Bank, BankGroup);
  CpgcVictimAddress[1]  = GetCompositeAddress (MrcData, Dimm, Rank, RowMinus1, Bank, BankGroup);

  MrcData->MspData.DynamicVars[Channel][PG_AGGRESSOR_ADDRESS] = CpgcAggressorAddress;
  MrcData->MspData.DynamicVars[Channel][PG_VICTIM_ADDRESS_0]  = CpgcVictimAddress[0];
  MrcData->MspData.DynamicVars[Channel][PG_VICTIM_ADDRESS_1]  = CpgcVictimAddress[1];

  MspDebugPrint ((
    MSP_DBG_TST,
    "CpgcAddr             = 0x%08x\nCpgcAddrPlus1        = 0x%08x\nCpgcAddrMinus1       = 0x%08x\n",
    CpgcAggressorAddress,
    CpgcVictimAddress[0],
    CpgcVictimAddress[1]
  ));

  return MMRC_SUCCESS;
}

/**
  Pass Gate Agressor, Victim0, Victim1 Writes

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Channel         Current Channel being examined.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
AgressorVictimWrites (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         Channel
  )
{
  UINT32  CpgcErrorsDetected;
  UINT32  ActivateCounter;
  UINT32  ReadCounter;
  UINT32  WriteCounter;

  MspDebugPrint ((MSP_DBG_DEBUG, "\n  Starting Victim0, Victim1 Writes\n"));
  //
  // Configure as Writes Only
  // 
  //
  MrcData->MspData.DynamicVars[Channel][PG_SUBSEQ_TYPE] = 1;
  //
  // Configures the 2 sequences for the victims
  //
  MmrcExecuteTask (MrcData, PASS_GATE_CPGC_VICTIMS_SETUP, NO_PRINT, Channel);
  //
  // Starts the test and checks for errors
  //
  CpgcExecuteSingleTest (MrcData, Channel, 0, &CpgcErrorsDetected, NULL, NULL);

  if (CpgcErrorsDetected != 0) {
    MspDebugPrint ((MSP_DBG_MIN, "    1. Cpgc Errors Detected: %08x\n", CpgcErrorsDetected));
  }

  MmrcExecuteTask (MrcData, PASS_GATE_READ_RAPL_COUNTERS, NO_PRINT, Channel);
  ActivateCounter = (UINT32) MrcData->MspData.DynamicVars[Channel][PG_ACTIVATE_CNT];
  ReadCounter     = (UINT32) MrcData->MspData.DynamicVars[Channel][PG_READ_CNT];
  WriteCounter    = (UINT32) MrcData->MspData.DynamicVars[Channel][PG_WRITE_CNT];

  
  if (ActivateCounter != (UINT32)(MrcData->PassGateParameters.SpeedSelect << 1) && !MrcData->PassGateParameters.EnablePeriodicSelfRefresh) {
    MspDebugPrint ((MSP_DBG_MIN, "    2. Wrong Activate counter: %08x\n", ActivateCounter));
  }

  if (ReadCounter != 0) {
    MspDebugPrint ((MSP_DBG_MIN, "    3. Wrong Read counter: %08x\n", ReadCounter));
  }

  if (WriteCounter != (UINT32)(MrcData->PassGateParameters.Burst << 1)) {
    MspDebugPrint ((MSP_DBG_MIN, "    4. Wrong Write counter: %08x\n", WriteCounter));
  }

  MspDebugPrint ((MSP_DBG_DEBUG, "\n  Starting Aggressor Writes\n"));

  MmrcExecuteTask (MrcData, PASS_GATE_CPGC_AGG_WRITES_SETUP, NO_PRINT, Channel);
  MmrcExecuteTask (MrcData, PASS_GATE_CPGC_AGGRESSOR_SETUP, NO_PRINT, Channel);
  //
  // Starts the test and checks for errors
  //
  CpgcExecuteSingleTest (MrcData, Channel, 0, &CpgcErrorsDetected, NULL, NULL);

  if (CpgcErrorsDetected != 0) {
    MspDebugPrint ((MSP_DBG_MIN, "    5. Cpgc Errors Detected: %08x\n", CpgcErrorsDetected));
  }

  MmrcExecuteTask (MrcData, PASS_GATE_READ_RAPL_COUNTERS, NO_PRINT, Channel);
  ActivateCounter = (UINT32) MrcData->MspData.DynamicVars[Channel][PG_ACTIVATE_CNT];
  ReadCounter     = (UINT32) MrcData->MspData.DynamicVars[Channel][PG_READ_CNT];
  WriteCounter    = (UINT32) MrcData->MspData.DynamicVars[Channel][PG_WRITE_CNT];

  if (ActivateCounter != MrcData->PassGateParameters.SpeedSelect && !MrcData->PassGateParameters.EnablePeriodicSelfRefresh) {
    MspDebugPrint ((MSP_DBG_MIN, "    6. Wrong Activate counter: %08x\n", ActivateCounter));
  }

  if (ReadCounter != 0) {
    MspDebugPrint ((MSP_DBG_MIN, "    7. Wrong Read counter: %08x\n", ReadCounter));
  }

  if (WriteCounter != MrcData->PassGateParameters.Burst) {
    MspDebugPrint ((MSP_DBG_MIN, "    8. Wrong Write counter: %08x\n", WriteCounter));
  }

  return MMRC_SUCCESS;
}

/**
  Pass Gate Characterization test

  @param[in, out]  MrcData           Host structure for all data related to MMRC.
  @param[in]       Channel           Current Channel being examined.
  @param[in]       ActivationsCount  Pass Gate counter to stress a single row/bank.
  @param[out]      RaplCounter       Return the Rapl counter.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
CharacterizationReads (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         Channel,
  IN        UINT32        ActivationsCount,
      OUT   UINT32        *RaplCounter
  )
{
  UINT32  CpgcErrorsDetected;
  UINT32  ActivateCounter;
  UINT32  ReadCounter;
  UINT32  WriteCounter;
 
  ActivateCounter = 0;
  ReadCounter = 0;
  WriteCounter = 0;
  //
  // Configure as Reads Only
  //
  MrcData->MspData.DynamicVars[Channel][PG_SUBSEQ_TYPE] = 0;

  MmrcExecuteTask (MrcData, PASS_GATE_CPGC_AGG_READS_SETUP, NO_PRINT, Channel);
  MmrcExecuteTask (MrcData, PASS_GATE_CPGC_AGGRESSOR_SETUP, NO_PRINT, Channel);

  MspDebugPrint ((MSP_DBG_DEBUG, "\n  Characterization Reads Start\n"));
  //
  // Starts the test, delay and then stop the test and check for errors
  //
  MmrcExecuteTask (MrcData, PASS_GATE_START_TEST,  NO_PRINT, Channel);
  //
  // Do a 100ns delay per Activation
  // 
  if ((MrcData->SkipOptions & SKIP_DELAY) == 0) {
    DelayForXActivates (MrcData, Channel, ActivationsCount, &ReadCounter, &WriteCounter, &ActivateCounter);
  }
  //
  // Stops the test. Check for errors
  //
  MmrcExecuteTask (MrcData, PASS_GATE_STOP_TEST,  NO_PRINT, Channel);

  CpgcErrorsDetected = (UINT32) MrcData->MspData.DynamicVars[Channel][BYTEGRP_ERR_STAT];
  if (MrcData->MrcParameters.EccEnabled[Channel]) {
    CpgcErrorsDetected |= ((UINT32) MrcData->MspData.DynamicVars[Channel][ECC_ERR_STAT] << 8);
  }

  if (CpgcErrorsDetected != 0) {
    MspDebugPrint ((MSP_DBG_MIN, "    20. Cpgc Errors Detected: %08x\n", CpgcErrorsDetected));
  }

  MmrcExecuteTask (MrcData, CPGC_CLEAR_ERRORS,  NO_PRINT, Channel);

  MmrcExecuteTask (MrcData, PASS_GATE_READ_RAPL_COUNTERS, NO_PRINT, Channel);
  ActivateCounter += ((UINT32) MrcData->MspData.DynamicVars[Channel][PG_ACTIVATE_CNT]);
  ReadCounter     += ((UINT32) MrcData->MspData.DynamicVars[Channel][PG_READ_CNT]);
  WriteCounter    += ((UINT32) MrcData->MspData.DynamicVars[Channel][PG_WRITE_CNT]);

  ActivateCounter /= MrcData->PassGateParameters.SpeedSelect;
  ReadCounter     /= MrcData->PassGateParameters.SpeedSelect;
  WriteCounter    /= MrcData->PassGateParameters.SpeedSelect;
  *RaplCounter = ActivateCounter;

  if (WriteCounter != 0) {
    MspDebugPrint ((MSP_DBG_MIN, "    21. Wrong Write counter: %d\n", WriteCounter));
  }
  if (!MrcData->PassGateParameters.EnablePeriodicSelfRefresh) {
    if ((ActivateCounter >= (ReadCounter + 20)) || (ActivateCounter <= (ReadCounter - 20))) {
      MspDebugPrint ((MSP_DBG_MIN, "    22. Activate and read counts not equal  %d : %d\n", ActivateCounter, ReadCounter));
    }
  }

  return MMRC_SUCCESS;
}

/**
  Pass Gate to do the victim reads test

  @param[in, out]  MrcData             Host structure for all data related to MMRC.
  @param[in]       Channel             Current Channel being examined.
  @param[out] ErrorPassGateCorruption  Return if error is detected during test.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
VictimReads (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         Channel,
      OUT   BOOLEAN       *ErrorPassGateCorruption
  )
{
  UINT32  CpgcErrorsDetected;
  UINT32  ActivateCounter;
  UINT32  ReadCounter;
  UINT32  WriteCounter;

  MspDebugPrint ((MSP_DBG_DEBUG, "\n  Starting Victim Reads\n"));
  //
  // Configure as Reads Only
  // 
  //
  MrcData->MspData.DynamicVars[Channel][PG_SUBSEQ_TYPE] = 0;
  //
  // Configures the 2 sequences for the victims
  //
  MmrcExecuteTask (MrcData, PASS_GATE_CPGC_VICTIMS_SETUP, NO_PRINT, Channel);
  //
  // Starts the test and checks for errors
  //
  CpgcExecuteSingleTest (MrcData, Channel, 0, &CpgcErrorsDetected, NULL, NULL);

  if (CpgcErrorsDetected != 0) {
    MspDebugPrint ((MSP_DBG_MIN, "    30. Cpgc Errors Detected: %08x\n", CpgcErrorsDetected));
    *ErrorPassGateCorruption = TRUE;
  }

  MmrcExecuteTask (MrcData, PASS_GATE_READ_RAPL_COUNTERS, NO_PRINT, Channel);
  ActivateCounter = (UINT32) MrcData->MspData.DynamicVars[Channel][PG_ACTIVATE_CNT];
  ReadCounter     = (UINT32) MrcData->MspData.DynamicVars[Channel][PG_READ_CNT];
  WriteCounter    = (UINT32) MrcData->MspData.DynamicVars[Channel][PG_WRITE_CNT];

  if (ActivateCounter != (UINT32)(MrcData->PassGateParameters.SpeedSelect << 1) && !MrcData->PassGateParameters.EnablePeriodicSelfRefresh) {
    MspDebugPrint ((MSP_DBG_MIN, "    31. Wrong Activate counter: %08x\n", ActivateCounter));
    *ErrorPassGateCorruption = TRUE;
  }
  if (ReadCounter != (UINT32)(MrcData->PassGateParameters.Burst << 1)) {
    MspDebugPrint ((MSP_DBG_MIN, "    32. Wrong Read counter:     %08x\n", ReadCounter));
  }
  if (WriteCounter != 0) {
    MspDebugPrint ((MSP_DBG_MIN, "    33. Wrong Write counter:    %08x\n", WriteCounter));
  }

  return MMRC_SUCCESS;
}
#endif

/**
  Pass Gate Characterization Test
   
  @param[in, out]  MrcData         Host structure for all data related to MMRC. 
  @param[in]   CapsuleStartIndex   Starting point within the 
                                   CapsuleData structure where
                                   the Init should begin.
  @param[in]       StringIndex     When provided, this is the index into the string
                                   table to show the register names.   
  @param[in]       Channel         Current Channel being examined.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
PassGateTest (
  IN  OUT     MMRC_DATA         *MrcData,
  IN          UINT16            CapsuleStartIndex,
  IN          UINT16            StringIndex,
  IN          UINT8             Channel
  )
{
#if PASS_GATE_TEST == 1
  UINT8   Dimm;
  UINT8   Rank;

  UINT32  Row;
  UINT8   Bank;
  UINT8   BankGroup;
  
  UINT32  IterationCounter;
  UINT32  RowBankTracker;
  UINT32  RowBankTrackerMask;
  UINT32  MaxRowBank;
  UINT32  MinRowBank;

  UINT32  ActivationNumberAtFailure;
  UINT32  PreviousErrorList[MAX_FAILURE_LIST];
  UINT32  CurrentErrorList[MAX_FAILURE_LIST];
  UINT8   PreviousErrorListPtr;
  UINT8   CurrentErrorListPtr;

  BOOLEAN GoNextRow;
  BOOLEAN ErrorPassGateCorruption;
  BOOLEAN PreviousErrorListFull;
  BOOLEAN MonteCarloDone;
  BOOLEAN AllMemorySwept;
  BOOLEAN SavedOneValuePerIteration;
  BOOLEAN PassGateTestFailed;

  UINT8   CurrentMaxFailures;
  UINT32  CurrentNumberOfActivations;
  UINT32  MontecarloDecrementValue;
  UINT32  FinalActivationNumber[MAX_RANKS];

  MMRC_STATUS MmrcStatus;
  
  Bank      = 0;
  BankGroup = 0;
  Row       = 0;
  MspDebugPrint ((MSP_DBG_MIN, "Pass Gate\n"));
  if (!MrcData->PassGateParameters.EnableTest) {
    MspDebugPrint ((MSP_DBG_MIN, "Test Not Enabled\n"));
    return MMRC_SUCCESS;
  }
  MmrcStatus  = InitInternalCommonVariables (MrcData, Channel);
  MmrcStatus |= PassGateTestEntryHooks (MrcData, Channel);
  if (MmrcStatus != MMRC_SUCCESS) {
    return MmrcStatus;
  }

  PassGateTestFailed = FALSE;
  for (Dimm = 0; Dimm < MAX_DIMMS; Dimm++) {
    for (Rank = 0; Rank < MAX_RANKS_PER_DIMM; Rank++) {
      FinalActivationNumber[(2 * Dimm) + Rank] = 0;
      //
      // Skip to next populated rank
      //
      if (!MrcData->RankEnabled[Channel][(2 * Dimm) + Rank]) {
        continue;
      }
      if (!MrcData->PassGateParameters.RankTest[Channel][(2 * Dimm) + Rank]){
        MspDebugPrint ((
          MSP_DBG_MIN,
          "\n\n*********Dimm = %d Rank = %d Disabled for testing*********\n\n",
          Dimm,
          Rank
          ));
        continue;
      }

      CurrentMaxFailures  = MrcData->PassGateParameters.MontecarloMaxFailures;
      MspDebugPrint ((
        MSP_DBG_MIN,
        "\n\n*********Dimm = %d Rank = %d*********\n\n",
        Dimm,
        Rank
        ));

      if (MrcData->PassGateParameters.EnablePeriodicSelfRefresh && (MrcData->PassGateParameters.IterationOnRow <= 2)) {
        //
        // This is the minimum number (3) of iterations the algorithm should 
        // execute to get the errors when the Periodic Self Refresh is enabled
        //
#ifndef SIM
        MrcData->PassGateParameters.IterationOnRow = 3;
#else
        MrcData->PassGateParameters.IterationOnRow = 1;
#endif
      }

      MspDebugPrint ((
        MSP_DBG_MIN,
        "PassGateEn = %d\nPassGateRefresh = %d\nDirection = %d\nActivations = %d\nIteration On Row = %d\nSwizzle = %d\nAgressor Pattern = %d's\nTarget Pattern = %d's\nSpeed = %dx\n",
        MrcData->PassGateParameters.EnableTest,
        MrcData->PassGateParameters.EnablePeriodicSelfRefresh,
        MrcData->PassGateParameters.Direction,
        MrcData->PassGateParameters.ActivationsCount,
        MrcData->PassGateParameters.IterationOnRow,
        MrcData->PassGateParameters.DimmSwizzle[Dimm],
        MrcData->PassGateParameters.Pattern,
        MrcData->PassGateParameters.TargetPattern,
        MrcData->PassGateParameters.SpeedSelect
        ));

      MspDebugPrint ((
        MSP_DBG_MIN,
        "PassGatePartial = %d\nBankRowMin  = 0x%04x\nBankRowMax = 0x%04x\n\n",
        MrcData->PassGateParameters.EnablePartialTest,
        MrcData->PassGateParameters.PartialRowBankMin,
        MrcData->PassGateParameters.PartialRowBankMax
        ));

      MspDebugPrint ((
        MSP_DBG_MIN,
        "MonteCarloEnable = %d\nCurrentMaxFailures = %d\nStartupRepetition = %d\nDecrement %d\n",
        MrcData->PassGateParameters.EnableMontecarloSearch,
        CurrentMaxFailures,
        MrcData->PassGateParameters.MontecarloStartRepetition,
        MrcData->PassGateParameters.MontecarloDecrement
        ));

      MaxRowBank  = MrcData->PassGateParameters.PartialRowBankMax & MrcData->PassGateParameters.RowBankTrackerMask;
      MinRowBank  = MrcData->PassGateParameters.PartialRowBankMin & MrcData->PassGateParameters.RowBankTrackerMask;
      if (MaxRowBank <= MinRowBank) {
        MspDebugPrint ((MSP_DBG_MIN, "Invalid value: MaxRowBank <= MinRowBank\n"));
      }

      if (!MrcData->PassGateParameters.EnablePartialTest || MaxRowBank <= MinRowBank) {
        MaxRowBank = MrcData->PassGateParameters.RowBankTrackerMask;
        MinRowBank = 0;
      }
      MspDebugPrint ((MSP_DBG_MIN, "MinRowBank: 0x%08x <-> MaxRowBank: 0x%08x\n", MinRowBank, MaxRowBank));

      RowBankTrackerMask = MrcData->PassGateParameters.RowBankTrackerMask;
      if (MrcData->PassGateParameters.Direction == 1) {
        //
        // Highest -> Lowest
        //
        RowBankTracker = MaxRowBank;
      } else {
        //
        // Lowest -> Highest
        //
        RowBankTracker = MinRowBank;
      }

      if (MrcData->PassGateParameters.EnableMontecarloSearch) {
        CurrentNumberOfActivations  = MrcData->PassGateParameters.MontecarloStartRepetition;
        MontecarloDecrementValue    = MrcData->PassGateParameters.MontecarloDecrement;
      } else {
        CurrentNumberOfActivations  = MrcData->PassGateParameters.ActivationsCount;
        MontecarloDecrementValue    = 0;
      }

      PreviousErrorListFull = FALSE;
      AllMemorySwept        = FALSE;
      MmrcMemset (&PreviousErrorList[0], 0xFF,  MAX_FAILURE_LIST * sizeof (UINT32));

      do { // Montecarlo loop ...
        if (MrcData->PassGateParameters.EnableMontecarloSearch && MrcData->PassGateParameters.EnablePartialTest) {
          //
          // if Montecarlo and Partial Test are both enable, next test
          // will be done with the 'new number of activates', but
          // within the range specified between MaxRowBank & MinRowBank
          //
          if (MrcData->PassGateParameters.Direction == 1) {
            RowBankTracker = MaxRowBank & MrcData->PassGateParameters.RowBankTrackerMask;
          } else {
            RowBankTracker = MinRowBank & MrcData->PassGateParameters.RowBankTrackerMask;
          }
        }

        MspDebugPrint ((MSP_DBG_MIN, "\n***CurrentNumberOfActivations: %08d\n", CurrentNumberOfActivations));
        FinalActivationNumber[(2 * Dimm) + Rank] = CurrentNumberOfActivations;
        MonteCarloDone        = FALSE;
        PreviousErrorListPtr  = 0;
        CurrentErrorListPtr   = 0;
        MmrcMemset (&CurrentErrorList[0], 0xFF,  MAX_FAILURE_LIST * sizeof (UINT32));

        do { // Direction loop ...
          if (MrcData->PassGateParameters.EnableMontecarloSearch && PreviousErrorListFull && (PreviousErrorListPtr < CurrentMaxFailures)) {
            RowBankTracker = PreviousErrorList[PreviousErrorListPtr];
            MspDebugPrint ((MSP_DBG_MIN, "  Retrieving RowBankTracker = 0x%04x from PreviousErrorList[%d]\n", RowBankTracker, PreviousErrorListPtr));
            PreviousErrorListPtr++;
          }
          GetRowBankMapping (MrcData, Channel, RowBankTracker, &Row, &Bank, &BankGroup);
          AddressSetup (MrcData, Channel, Dimm, Rank, RowBankTracker);

          SavedOneValuePerIteration = FALSE;
          for (IterationCounter = 0; IterationCounter < MrcData->PassGateParameters.IterationOnRow; IterationCounter++) {
            ErrorPassGateCorruption = FALSE;
            //
            // Read the RAPL registers in order to clear them
            //
            MmrcExecuteTask (MrcData, PASS_GATE_READ_RAPL_COUNTERS, NO_PRINT, Channel);
            
            if (IterationCounter == 0 || MrcData->PassGateParameters.EnablePeriodicSelfRefresh) {
              //
              // If this is the first test execute the 3 tests:
              //  - Agressor, Victim0, Victim1
              //  - Pass Gate Characterization Writes
              //  - Victim Reads
              //
              // For the next test we only want to execute tests 2 and 3
              //
              AgressorVictimWrites (MrcData, Channel);
            }
            CharacterizationReads (MrcData, Channel, CurrentNumberOfActivations, &ActivationNumberAtFailure);
            VictimReads (MrcData, Channel, &ErrorPassGateCorruption);

            if (ErrorPassGateCorruption) {
              PassGateTestFailed = TRUE;
              MspDebugPrint ((MSP_DBG_MIN, "    Row = 0x%04x Bank = 0x%02x Bank Group = %d Counter = %08d\n",
                Row, Bank, BankGroup, ActivationNumberAtFailure
                ));
              if (MrcData->PassGateParameters.EnableMontecarloSearch && !SavedOneValuePerIteration) {
                SavedOneValuePerIteration = TRUE;

                MspDebugPrint ((MSP_DBG_MIN, "    Saving RowBankTracker = 0x%04x in CurrentErrorList[%d]\n",
                  RowBankTracker, CurrentErrorListPtr
                  ));
                CurrentErrorList[CurrentErrorListPtr] = RowBankTracker;
                CurrentErrorListPtr++;
              }
            }
          } // IterationCounter loop ...

          GoNextRow = TRUE;
          if (MrcData->PassGateParameters.Direction == 0) {
            RowBankTracker += MrcData->PassGateParameters.SpeedSelect;
            if ((RowBankTracker & RowBankTrackerMask) == ((MaxRowBank + MrcData->PassGateParameters.SpeedSelect) & RowBankTrackerMask)) {
              GoNextRow = FALSE;
            }
          } else if (MrcData->PassGateParameters.Direction == 1) {
            RowBankTracker -= MrcData->PassGateParameters.SpeedSelect;
            if ((RowBankTracker & RowBankTrackerMask) == ((MinRowBank - MrcData->PassGateParameters.SpeedSelect) & RowBankTrackerMask)) {
              GoNextRow = FALSE;
            }
          }
          if (MrcData->PassGateParameters.EnableMontecarloSearch) {
            if (CurrentErrorListPtr == 0 && !GoNextRow) {
              //
              // No errors are detected and we've reached the limit, 
              // so there are no more rows/banks to test.
              //
              MonteCarloDone  = TRUE;
              AllMemorySwept  = TRUE;
              GoNextRow       = FALSE;
              MspDebugPrint ((MSP_DBG_MIN, "\n    Montecarlo Test is Done\n\n"));
            } else if (CurrentErrorListPtr == 0 && PreviousErrorListPtr == CurrentMaxFailures && AllMemorySwept) {
              //
              // We previously reached the limit with errors, and start testing the
              // rows/banks in the stored in the fifo without corruption errors.
              //
              MonteCarloDone  = TRUE;
              AllMemorySwept  = TRUE;
              GoNextRow       = FALSE;
              MspDebugPrint ((MSP_DBG_MIN, "\n    Montecarlo Test is Done with AllMemorySwept\n\n"));
            } else if (CurrentErrorListPtr > 0 && PreviousErrorListPtr == CurrentMaxFailures && AllMemorySwept) {
              //
              // We previously reached the limit with errors, and start testing the
              // rows/banks in the stored in the fifo with corruption errors, so we
              // have to copy the results from the current error list to the previous
              // one, in order to re-test again the fifo with an smaller number of activations.
              // 
              // The number of errors in the current error list could be the same as MAX_FAILURES or smaller
              //
              MspDebugPrint ((MSP_DBG_MIN, "    Copying partial values from CurrentErrorList to PreviousErrorList after AllMemorySwept\n\n"));
              for (PreviousErrorListPtr = 0; PreviousErrorListPtr < CurrentErrorListPtr; PreviousErrorListPtr++) {
                PreviousErrorList[PreviousErrorListPtr] = CurrentErrorList[PreviousErrorListPtr];
              }

              CurrentMaxFailures          = CurrentErrorListPtr;
              PreviousErrorListFull       = TRUE;
              GoNextRow                   = FALSE;
              CurrentNumberOfActivations -= MontecarloDecrementValue;
            } else if (CurrentErrorListPtr > 0 && !GoNextRow && !AllMemorySwept) {
              AllMemorySwept = TRUE;
              //
              // We previously reached the limit with errors, so we have to copy the results from 
              // the current error list to the previous one.
              // 
              // The number of errors in the current error list could be the same as MAX_FAILURES or smaller
              //
              MspDebugPrint ((MSP_DBG_MIN, "    Copying partial values from CurrentErrorList to PreviousErrorList\n\n"));
              for (PreviousErrorListPtr = 0; PreviousErrorListPtr < CurrentErrorListPtr; PreviousErrorListPtr++) {
                PreviousErrorList[PreviousErrorListPtr] = CurrentErrorList[PreviousErrorListPtr];
              }
              CurrentMaxFailures          = CurrentErrorListPtr;
              PreviousErrorListFull       = TRUE;
              CurrentNumberOfActivations -= MontecarloDecrementValue;
            } else if (CurrentErrorListPtr == CurrentMaxFailures) {
              //
              // Stop looping when the first 'CurrentMaxFailures' are gotten in the test
              //
              GoNextRow = FALSE;
              //
              // Copy the results in the NewList to the OldList
              //
              MspDebugPrint ((MSP_DBG_MIN,  "    Copying values from CurrentErrorList to PreviousErrorList\n"));
              for (CurrentErrorListPtr = 0; CurrentErrorListPtr < CurrentMaxFailures; CurrentErrorListPtr++) {
                PreviousErrorList[CurrentErrorListPtr] = CurrentErrorList[CurrentErrorListPtr];
              }
              PreviousErrorListFull       = TRUE;
              CurrentNumberOfActivations -= MontecarloDecrementValue;
            }
          } // if Montecarlo Enable statement ...
        } while (GoNextRow);
      } while (MrcData->PassGateParameters.EnableMontecarloSearch && !MonteCarloDone);
    } // Rank loop ...
  } // Dimm loop ...

  PassGateTestExitHooks (MrcData, Channel);

  MspDebugPrint ((MSP_DBG_MIN, "\n-- Final Results--\n"));
  for (Dimm = 0; Dimm < MAX_DIMMS; Dimm++) {
    for (Rank = 0; Rank < MAX_RANKS_PER_DIMM; Rank++) {
      if (!MrcData->RankEnabled[Channel][(2 * Dimm) + Rank]) {
        continue;
      }
      MspDebugPrint ((MSP_DBG_MIN, "Dimm = %d Rank = %d --> ", Dimm, Rank));
      if (MrcData->PassGateParameters.RankTest[Channel][(2 * Dimm) + Rank]) {
        MspDebugPrint ((MSP_DBG_MIN, "Max PG Counter = %d\n", FinalActivationNumber[(2 * Dimm) + Rank]));
      } else {
        MspDebugPrint ((MSP_DBG_MIN, "Not tested\n"));
      }
    } // Rank loop ...
  } // Dimm loop ...

  if (PassGateTestFailed) {
    MspDebugPrint ((MSP_DBG_MIN, "TEST FAILED: PASS GATE ISSUES FOUND\n"));
  } else {
    MspDebugPrint ((MSP_DBG_MIN, "TEST PASSED\n"));
  }

#endif
  return MMRC_SUCCESS;
}

