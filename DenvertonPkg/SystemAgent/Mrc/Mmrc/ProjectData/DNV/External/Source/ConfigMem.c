/** @file 
  ConfigMem.c
  Memory Configuration

  Copyright (c) 2005-2017 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

**/

#include "ConfigMem.h"
#include "ConfigMemData.h"
#include "IchRegs.h"

#include "OemHooks.h"
#include "McFunc.h"
#include "DetectDimms.h"
#include "MmrcHooks.h"
#include "Mailbox.h"
#include "MmrcLibraries.h"
#include "Regs.h"
#include "PassGate.h"
#ifdef MINIBIOS
#ifndef CAR
#include "Uart.h"
#endif
#endif
#ifdef SIM
#include "time.h"
#endif
#include "NvDimmHooks.h"


extern UINT32 mDmapRegisters[MAX_DIMM_CONFIGS][MAX_DMAP_REGISTERS];

//
// Patrol Scrub Definitions
//
UINT32 ScrubIndex2SecondsTable[] = {
  86400,   // 24 hours
  36000,   // 10 hours
  14400,   // 4 hours
  3600     // 1 hour
};

//
// DClks in usec
//
UINT32 DClksUsec[] = {
  2500, 
  2143,
  1875,
  1666,
  1500
};



//
// CAS Latency to DDR4 MRS0 CAS latency index
//
UINT8 CasLat2Ddr4Mrs0CasLatIdx[] = {    //A6 A5 A4 A2 CAS Latency
  0,                                        //0 0 0 0 9
  1,                                        //0 0 0 1 10
  2,                                        //0 0 1 0 11
  3,                                        //0 0 1 1 12
  4,                                        //0 1 0 0 13
  5,                                        //0 1 0 1 14
  6,                                        //0 1 1 0 15
  7,                                        //0 1 1 1 16
  13,                                       //1 1 0 1 17
  8,                                        //1 0 0 0 18
  14,                                       //1 1 1 0 19
  9,                                        //1 0 0 1 20
  15,                                       //1 1 1 1 21
  10,                                       //1 0 1 0 22
  0,
  11,                                       //1 0 1 1 24
  12                                        //1 1 0 0 Reserved
 };                                 

//
// Used to match the rank select on DRAM_CMD to generate the MRS of the register control words
//
UINT32 DramCmdRankMask[] = {
  0<<26,
  1<<26,
  2<<26,
  3<<26
};
UINT32 LeakyBucketDivisor[C_MAXDDR] = {
  10240000,
  8781824,
  7684096,
  6823936,
  6144000
};

/**
  Write to CMOS.

  @param[in]   Index      Location to write to CMOS
  @param[in]   Value      Value to be written to the CMOS
  @retval      NONE
**/  
VOID
WriteCmosMrc (
  IN  UINT8   Index,
  IN  UINT8   Value
  )
{
  if (Index < 0x80) {
    IoOut8 (CMOS_UPPER, Index);
    IoOut8 (CMOS_UPPER + 1, Value);
  }
}

/**
  Read From CMOS.

  @param[in]   Index       Location to read from CMOS
  @retval      Value       Contains the value read from the CMOS
**/
UINT8
ReadCmosMrc (
  IN  UINT8   Index
  )
{
  UINT8  Value;

  Value = 0;

  if (Index < 0x80) {
    IoOut8 (CMOS_UPPER, Index);
    Value = (UINT8) IoIn8 (CMOS_UPPER + 1);
  }
  return (UINT8) Value;
}

/**
  Initializes all dynamic variables used in JEDEC init for DDR3 
  or DDR4 

  @param[in, out]  MrcData       Host structure for all data related to MMRC.
  @param[in]       Channel       Current Channel being examined.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
InitializeJedecVariables (
  IN  OUT   MMRC_DATA   *MrcData,
  IN        UINT8        Channel
  )
{
  UINT8   Rank;
  UINT8   MrsCommand;
  if (MrcData->MrcParameters.DramType[Channel] <= TypeDdr3All) {
    InitializeDdr3JedecVariables (MrcData, Channel);
  } else {
    InitializeDdr4JedecVariables (MrcData, Channel);
  }
  //
  // Next section will override the MRS in case we are in any restore pad
  // This will be required to restore correctly the ODT's when the power
  // training was executed during the coldboot path and the values are different
  // from the default ones
  //
  for (Rank = 0; Rank < MAX_RANKS; Rank++) {
    //
    // Skip to next populated rank
    //
    if (MrcData->MrcParameters.RankPresent[Channel][Rank] == 0) {
      continue;
    }
    if (MrcData->RestorePath && MrcData->MrcParameters.SaveRestore.MrsCommandSx[Channel][Rank].MrsValid) {
      MspDebugPrint ((MSP_DBG_MIN, "Restoring MRS - Rank[%d]\n", Rank));
      for (MrsCommand = 0; MrsCommand < MR_COUNT; MrsCommand++) {
        MrcData->MspData.DynamicVars[Channel][MrsCommandIndex[Rank][MrsCommand]] = MrcData->MrcParameters.SaveRestore.MrsCommandSx[Channel][Rank].MrsValue[MrsCommand];
        MspDebugPrint ((MSP_DBG_MIN, "  Mrs[%d]: 0x%08x\n", MrsCommand, (UINT32) MrcData->MspData.DynamicVars[Channel][MrsCommandIndex[Rank][MrsCommand]]));
      }
      continue;
    } else if (MrcData->RestorePath && !MrcData->MrcParameters.SaveRestore.MrsCommandSx[Channel][Rank].MrsValid) {
      MspDebugPrint ((MSP_DBG_MIN, "Invalid  MRS - Rank[%d]\n", Rank));
    }
  }

  return MMRC_SUCCESS;
}
/**
  Initializes all dynamic variables used in JEDEC init for DDR3.

  @param[in, out]  MrcData       Host structure for all data related to MMRC.
  @param[in]       Channel       Current Channel being examined.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
InitializeDdr3JedecVariables (
  IN  OUT   MMRC_DATA   *MrcData,
  IN        UINT8        Channel
  )
{
  UINT8             Rank;
  UINT8             Dimm;
  UINT8             CurrentFrequency;
  UINT8             DramType;
  UINT8             SpdAddMap;
  UINT8             SpdDeviceOrg;
  UINT8             DimmType;
  UINT32            MrsValue[MAX_RANKS][MR_COUNT];
  DramInitDDR3MRS0  Mrs0Command;
  DramInitDDR3EMR1  Mrs1Command;
  DramInitDDR3EMR2  Mrs2Command;
  DramInitDDR3EMR3  Mrs3Command;

  PRINT_FUNCTION_INFO_MAX;

  MmrcMemset (MrsValue, 0, MAX_RANKS * MR_COUNT * sizeof (UINT32));

#if ODT_TABLE
  //
  // Set ODT Matrix
  //
  SetOdtMatrix (MrcData, 0, Channel, MrsValue);
#endif

  for (Dimm = 0; Dimm < MAX_DIMMS; Dimm++) {
    //
    // Skip to next populated rank
    //
    if (!MrcData->MrcParameters.DimmPresent[Channel][Dimm]) {
      continue;
    }

    for (Rank = 0; Rank < MAX_RANKS_PER_DIMM; Rank++) {
      //
      // Skip to next populated rank
      //
      if (MrcData->MrcParameters.RankPresent[Channel][(2 * Dimm) + Rank] == 0) {
        continue;
      }

      Mrs2Command.Data              = MrsValue[(2 * Dimm) + Rank][2];
      Mrs2Command.Bits.bankAddress  = 2;
      Mrs2Command.Bits.CWL          = MrcData->MrcParameters.TimingData[Channel][TimingTwcl][FreqA] - 5;

      //
      // DDR3 Configuring Normal/Extended Operating Temperatures
      // MR2 (A7) -- Self-Refresh Temperature Range (SRT)
      // MR2 (A6) -- Auto Self-Refresh mode (ASR)
      // 
      // ---------------------------------
      // MR@ A7/A6:
      //    "00" = Normal (0 to 85C)
      //    "10" = SRT Normal and Extended (0 to 95C)
      //    "11" = ASR Normal and Extended (0 to 95C)
      //
      Mrs2Command.Bits.ASR = 0;
      Mrs2Command.Bits.SRT = 0;

      if (!MrcData->PowerOn) {
        if ((MrcData->MrcParameters.SpdData[Channel][Dimm].Buffer[SPD_DDR3_STROPT] & BIT0) == 0x01) {
          Mrs2Command.Bits.SRT = 1;
        } else if (((MrcData->MrcParameters.SpdData[Channel][Dimm].Buffer[SPD_DDR3_STROPT] & BIT2) >> 2) == 0x01) {
          Mrs2Command.Bits.ASR = 1;

        } else {
          MspDebugPrint ((MSP_DBG_MAX, "Extended temperature range not supported in the Dimm...\n"));
        }
      }
      MspDebugPrint ((MSP_DBG_MAX, "Mrs2Command.Bits.SRT = 0x%x, Mrs2Command.Bits.ASR = 0x%x\n", Mrs2Command.Bits.SRT, Mrs2Command.Bits.ASR));

      //
      // Shadow MRS2
      //
      if (Rank == 0) {
        MrcData->MspData.DynamicVars[Channel][BITS7_3_DIMM0] = (Mrs2Command.Bits.CWL | 
                                                        (Mrs2Command.Bits.ASR << 3) | 
                                                        (Mrs2Command.Bits.SRT << 4));
        MrcData->MspData.DynamicVars[Channel][BITS12_9_DIMM0] = Mrs2Command.Bits.rtt_WR;                      
      }
      if (Rank == 2) {
        MrcData->MspData.DynamicVars[Channel][BITS7_3_DIMM1] = (Mrs2Command.Bits.CWL |
                                                        (Mrs2Command.Bits.ASR << 3) | 
                                                        (Mrs2Command.Bits.SRT << 4));
        MrcData->MspData.DynamicVars[Channel][BITS12_9_DIMM1] = Mrs2Command.Bits.rtt_WR;
      }
      //
      // Setup for MR3
      //
      Mrs3Command.Data              = 0;
      Mrs3Command.Bits.bankAddress  = 3;
      //
      // Setup for MR1
      // SetOdtMatrix initializes MR1
      //
      Mrs1Command.Data              = MrsValue[(2 * Dimm) + Rank][1];
      Mrs1Command.Bits.bankAddress  = 1;
      Mrs1Command.Bits.dllEnabled   = 0;
      Mrs1Command.Bits.DIC0         = 1;
#if DM_PINS_TIED_GROUND == 1
      Mrs1Command.Bits.TDQS         = 0;
#else
      //
      // TDQS should be 1 for x8 and 0 for x16
      //
      if (MrcData->D_DataWidth[Channel][Dimm] == 2) {
        Mrs1Command.Bits.TDQS         = 0;
      } else {
        Mrs1Command.Bits.TDQS         = 1;
      }

#endif
      //
      // Setup for MR0
      //
      Mrs0Command.Data              = 0;
      Mrs0Command.Bits.bankAddress  = 0;
      Mrs0Command.Bits.dllReset     = 1;
      Mrs0Command.Bits.BL           = DQ_BURSTLEN_8;
      Mrs0Command.Bits.PPD          = 1;
      Mrs0Command.Bits.RBT          = 0;

      if (MrcData->MspData.DynamicVars[Channel][SLOWEXIT] == 1) {
        Mrs0Command.Bits.PPD = 0;
      }

      Mrs0Command.Bits.casLatency = MrcData->MrcParameters.TimingData[Channel][TimingTcl][FreqA] - 4;
      Mrs0Command.Bits.CL = ((MrcData->MrcParameters.TimingData[Channel][TimingTcl][FreqA] - 4) >> 3);
      Mrs0Command.Bits.reserved1 = 0;

      //
      // Consider making writeRecovery a formula based on
      // MrcData->MrcParameters.TimingData[Channel][TimingTwr]
      //
      switch (MrcData->MrcParameters.DdrFrequency) {
      case MMRC_1600:
        Mrs0Command.Bits.writeRecovery = 6;
        break;
      case MMRC_1866:
        Mrs0Command.Bits.writeRecovery = 7;
        break;
      default:
        Mrs0Command.Bits.writeRecovery = 6;
      } 
      //
      // Use DDR 800 to index zero
      //
      CurrentFrequency = MrcData->MrcParameters.DdrFrequency - MINDDR;

      Mrs0Command.Bits.rankSelect = ((2 * Dimm) + Rank);
      Mrs1Command.Bits.rankSelect = ((2 * Dimm) + Rank);
      Mrs2Command.Bits.rankSelect = ((2 * Dimm) + Rank);
      Mrs3Command.Bits.rankSelect = ((2 * Dimm) + Rank);
      //
      // DDR3 address mirroring
      //
      DramType = MrcData->MrcParameters.DramType[Channel];
      SpdAddMap = MrcData->MrcParameters.SpdData[Channel][Dimm].Buffer[SPD_DDR3_ADD_MAPPING];
      SpdDeviceOrg = MrcData->MrcParameters.SpdData[Channel][Rank / MAX_RANKS_PER_DIMM].Buffer[SPD_DDR3_ORG];
      DimmType = MrcData->D_Type[Channel][Dimm];
      HandleAddressMirroring (&Mrs0Command.Data, SpdAddMap, SpdDeviceOrg, DramType, DimmType, SIDE_A);
      HandleAddressMirroring (&Mrs1Command.Data, SpdAddMap, SpdDeviceOrg, DramType, DimmType, SIDE_A);
      HandleAddressMirroring (&Mrs2Command.Data, SpdAddMap, SpdDeviceOrg, DramType, DimmType, SIDE_A);
      HandleAddressMirroring (&Mrs3Command.Data, SpdAddMap, SpdDeviceOrg, DramType, DimmType, SIDE_A);
      //
      // Save MRS settings for later use
      //
      MrcData->MspData.DynamicVars[Channel][MrsCommandIndex[(2 * Dimm) + Rank][0]] = Mrs0Command.Data;
      MrcData->MspData.DynamicVars[Channel][MrsCommandIndex[(2 * Dimm) + Rank][1]] = Mrs1Command.Data;
      MrcData->MspData.DynamicVars[Channel][MrsCommandIndex[(2 * Dimm) + Rank][2]] = Mrs2Command.Data;
      MrcData->MspData.DynamicVars[Channel][MrsCommandIndex[(2 * Dimm) + Rank][3]] = Mrs3Command.Data;
    } // end for (Rank = 0; Rank < MAX_RANKS; Rank++)
  } // end for (Dimm = 0; Dimm < MAX_DIMMS; Dimm++)

  return MMRC_SUCCESS;
}

UINT8   RegisterBacksideDriveStrength[MAX_RANKS][2] = {
  // x8,   x4
  {  0x0,  0x5}, // 1R
  {  0x5,  0xA}, // 2R
  {  0x5,  0xA}, // 3R
  {  0x5,  0xA}  // 4R
};

/**
  Initializes Register Control Words.

  @param[in, out]   MrcData   Host structure for all MRC global data
  @param[in]        Channel   Channel being examinated
  @param[in]        Dimm      Dimm
  @param[in]        Rank      Rank
  @param[in, out]   MrsValue  Variables to save the register 
                              control word values
  @retval           Status 
**/ 
MMRC_STATUS
RegisterControlWordSetup (
  IN  OUT MMRC_DATA   *MrcData,
  IN      UINT8       Channel,
  IN      UINT8       Dimm, 
  IN      UINT8       Rank, 
  IN  OUT UINT32      MrsValue[MAX_RANKS][MR_COUNT]
  )
{
  UINT8   ChannelRankNum;
  UINT8   CwData;
  UINT8   TempData;
  UINT8   Bank7Command;
  UINT8   DimmX4;
  UINT16  RegisterManufacturerId;

  ChannelRankNum  = (2 * Dimm) + Rank;
  Bank7Command    =  (7 << 4) | (8 << 0);

  RegisterManufacturerId  = MrcData->MrcParameters.SpdData[Channel][Dimm].Buffer[SPD_DDR4_RMIDL] & 0x7F;
  RegisterManufacturerId |= MrcData->MrcParameters.SpdData[Channel][Dimm].Buffer[SPD_DDR4_RMIDH] << 8;
  //
  // Rdimm and CA parity register control words
  //
  if (MrcData->MrcParameters.DramType[Channel] == TypeDdr4) {
    if (MrcData->MspData.DynamicVars[Channel][DIMMTYPE] == SPD_DDR_RDIMM) {
      //
      // Detect if it is x4 Dimm
      //
      if (MrcData->D_DataWidth[Channel][Dimm] == 0) {
        DimmX4 = 1;
      } else {
        DimmX4 = 0;
      }
      //
      // RC0 is index 18 MrsValue
      // Each MRS Command to modify the Register Control in the RDIMM will have the next format:
      // RC0x[19:00] = {Register Address[7:0], Data[3:0], Bank[3:0], Command[3:0]}
      // Bank is always     = 7
      // Command is always  = 8
      //
      //
      // RC00 bits:
      // 0 - Inversion Disabled
      // 1 - Weak Drive Enable
      // 2 - A outputs disabled
      // 3 - B outputs disabled
      //
      MrsValue[ChannelRankNum][18] = (0 << 12) | (0 << 8) | Bank7Command;
      //
      // RC01 bits:
      // x - Yx_t/Y0_c clock
      //
      MrsValue[ChannelRankNum][19] = (1 << 12) | (0 << 8) | Bank7Command;
      //
      // RC02 bits:
      // 0 - DA17 Input bus termination Disabled
      // 1 - DPAR Input bus termination Disabled
      // 2 - Transparent mode enabled
      // 3 - Frequency Band Select (Test Mode)
      //
      MrsValue[ChannelRankNum][20] = (2 << 12) | (0 << 8) | Bank7Command;
      //
      // RC03 bits:
      // 1,0 - Address/Command Drive (Light, Moderate, Strong, Very Strong)
      // 3,2 - QxCSy_n Outputs (Light, Moderate, Strong, Very Strong)
      //
      TempData  = (MrcData->MrcParameters.SpdData[Channel][Dimm].Buffer[SPD_DDR4_ODS_CTL] >> 4) & 0x0f;
      if (MrcData->MrcParameters.SpdData[Channel][Dimm].Buffer[SPD_DDR4_SPD_REVISION] < 7) {
        //
        // Override in case the SPD version is below 7
        //
        TempData = RegisterBacksideDriveStrength[MrcData->D_Ranks[Channel][Dimm]][DimmX4];
      }
      MrsValue[ChannelRankNum][21] = (0x3 << 12) | (TempData << 8) | Bank7Command;
      //
      // RC04 bits:
      // 1,0 - QxODT0..QxODT1 Outputs (Light, Moderate, Strong, Very Strong)
      // 3,2 - QxCKE0..QxCKE1 Outputs (Light, Moderate, Strong, Very Strong)
      //
      TempData  = (MrcData->MrcParameters.SpdData[Channel][Dimm].Buffer[SPD_DDR4_ODS_CTL] >> 2) & 0x03;
      CwData    = (MrcData->MrcParameters.SpdData[Channel][Dimm].Buffer[SPD_DDR4_ODS_CTL] << 2) & 0x0C;
      TempData |= CwData;
      if (MrcData->MrcParameters.SpdData[Channel][Dimm].Buffer[SPD_DDR4_SPD_REVISION] < 7) {
        //
        // Override in case the SPD version is below 7
        //
        TempData = RegisterBacksideDriveStrength[MrcData->D_Ranks[Channel][Dimm]][DimmX4];
      }
      MrsValue[ChannelRankNum][22] = (0x4 << 12) | (TempData << 8) | Bank7Command;

      if (RegisterManufacturerId == RID_INPHI) {
        if (Dimm == 0) {
          MrcData->MspData.DynamicVars[Channel][INPHI_REGISTER_0] = 1;
        } else {
          MrcData->MspData.DynamicVars[Channel][INPHI_REGISTER_1] = 1;
        }
        //
        // RC4x Bits:
        // 7-->0: {1'bx, 1'bx, 1'bx, A12:A8}
        //
        TempData = 2 << 5;
        MrsValue[ChannelRankNum][30] = (0x40 << 12) | (TempData << 8) | Bank7Command;
        //
        // RC6x Bits:
        // 7-->0: A7:A0
        //
        TempData = 2;
        MrsValue[ChannelRankNum][31] = (0x60 << 12) | (TempData << 8) | Bank7Command;
        //
        // RC06 Bits:
        // 3,2,1,0: CMD 0-->15
        // 5: CW Write Operation
        //
        TempData = 5;
        MrsValue[ChannelRankNum][32] = (0x06 << 12) | (TempData << 8) | Bank7Command;
      }

      //
      // RC05 bits:
      // 1,0 - Clock Y1_t, Y1_c, Y3_t & Y3_c Outputs (Light, Moderate, Strong, Very Strong)
      // 3,2 - Clock Y0_t, Y0_c, Y2_t & Y2_c Outputs (Light, Moderate, Strong, Very Strong)
      //
      TempData  = (MrcData->MrcParameters.SpdData[Channel][Dimm].Buffer[SPD_DDR4_ODS_CK] >> 2) & 0x03;
      CwData    = (MrcData->MrcParameters.SpdData[Channel][Dimm].Buffer[SPD_DDR4_ODS_CK] << 2) & 0x0C;
      TempData |= CwData;
      if (MrcData->MrcParameters.SpdData[Channel][Dimm].Buffer[SPD_DDR4_SPD_REVISION] < 7) {
        //
        // Override in case the SPD version is below 7
        //
        TempData = RegisterBacksideDriveStrength[MrcData->D_Ranks[Channel][Dimm]][DimmX4];
      }
      MrsValue[ChannelRankNum][23] = (0x5 << 12) | (TempData << 8) | Bank7Command;
      //
      // RC4x Bits:
      // 7-->0: {1'bx, 1'bx, 1'bx, A12:A8}
      //
      TempData = 0xF;
      MrsValue[ChannelRankNum][34] = (0x40 << 12) | (TempData << 8) | Bank7Command;
      //
      // RC8 Bits:
      // 1,0: QxC[2:0] enabled, QxC2 Disabled, QxC0 Disabled and QxC[2:0] Disabled
      // 2: QxPAR Disabled
      // 3: DA17 and QxA17 Disabled
      //
      if ((MrcData->MrcParameters.SpdData[Channel][Dimm].Buffer[SPD_DDR4_TYPE] & 0x3) == SPD_DDR4_3DS) {
        TempData = 0xB;
      } else if (MrcData->D_Ranks[Channel][Dimm] == MAX_RANKS - 1) {
        TempData = 0x9;
      } else {
        TempData = 0xB;
      }
      MrsValue[ChannelRankNum][24] = (0x08 << 12) | (TempData << 8) | Bank7Command;
      //
      // RC9 Bits:
      // 0: DCS1 Input Bus Termination Disable
      // 1: DCS1 Input Buffer & QxCS1 Disable
      // 3,2: CKE Power Down Modes ('3'=0 to Disable)
      //
      if (MrcData->DimmCount[Channel] > 1) {
        TempData = 0x8;
      } else {
        TempData = 0xC;
      }
      MrsValue[ChannelRankNum][25] = (0x09 << 12) | (TempData << 8) | Bank7Command;
      //
      // RCA Bits:
      // 2,1,0: 1600->3200
      // 3: Context 2 operation
      //
      switch (MrcData->MrcParameters.DdrFrequency) {
        case MMRC_1600:
          TempData = 0;
          break;
        case MMRC_1866:
          TempData = 1;
          break;
        case MMRC_2133:
          TempData = 2;
          break;
        case MMRC_2400:
          TempData = 3;
          break;
        case MMRC_2666:
          TempData = 4;
          break;
        default:
          TempData = 0;
      }
      MrsValue[ChannelRankNum][26] = (0xA << 12) | (TempData << 8) | Bank7Command;
      //
      // RCB Bits:
      // 2,1,0: Internal Vref configurations
      // 3: External VrefCA
      //
      MrsValue[ChannelRankNum][27] = (0xB << 12) | (BIT3 << 8) | Bank7Command;
      //
      // RCC Bits:
      // 2,1,0: Normal, ClocktoCA, DCS0, DCS1, DCKE0, DCKE1, DODT0, DODT1
      // 3: Reserved
      //
      MrsValue[ChannelRankNum][28] = (0xC << 12) | (0 << 8) | Bank7Command;
      //
      // RCD Bits:
      // 1,0: CS Mode
      // 2: RDIMM
      // 3: Address Mirroring for MRS
      //
      TempData = 0;
      if (MrcData->MrcParameters.SpdData[Channel][Dimm].Buffer[SPD_DDR4_ADD_MAPPING] & 1){
        TempData = BIT3;
      }
      TempData |= BIT2;
      MrsValue[ChannelRankNum][29] = (0xD << 12) | (TempData << 8) | Bank7Command;
      //
      // RCE Bits:
      // 0: Parity Enable
      // 1: Reserved
      // 2: Alert_n pulse width
      // 3: Parity checking is re-enabled
      //
      MrsValue[ChannelRankNum][7] = (0xE << 12) | (0 << 8) | Bank7Command;
      //
      // RCF Bits:
      // 2,1,0: Latencies
      // 3: Reserved
      //
      // As parity will not be enabled not extra latency is required
      // 
      TempData = BIT2;
      if (MrcData->MrcParameters.DdrFrequency <= MMRC_2400) {
      } else {
      }
      MrsValue[ChannelRankNum][8] = (0xF << 12) | (TempData << 8) | Bank7Command;
      if (MrcData->CommandAddressParity) {
        //
        // RC0F
        // 1 cycle(s) of command latency for speeds up to DDR4 2400. Set RC0F.DA[3:0] = bx000
        // 2 cycle(s) of command latency for speeds up to DDR4 2667. Set RC0F.DA[3:0] = bx001
        //
        if (MrcData->MrcParameters.DdrFrequency == MMRC_2666) {
          TempData = BIT0;
          MrsValue[ChannelRankNum][8] = (0xF << 12) | (TempData << 8) | Bank7Command;
        } else {
          TempData = 0;
          MrsValue[ChannelRankNum][8] = (0xF << 12) | (TempData << 8) | Bank7Command;
        }
//        MrsValue[ChannelRankNum][8] = 0xED78;
      }
      //
      // RC3x Bits:
      // 6-->0: Fine Granularity RDIMM Operating Speed
      // 7: Reserved
      //
      switch (MrcData->MrcParameters.DdrFrequency) {
        case MMRC_1600:
          TempData = 0x12;
          break;
        case MMRC_1866:
          TempData = 0x1F;
          break;
        case MMRC_2133:
          TempData = 0x27;
          break;
        case MMRC_2400:
          TempData = 0x3A;
          break;
        case MMRC_2666:
          TempData = 0x47;
          break;
        default:
          TempData = 0;
      }
      MrsValue[ChannelRankNum][33] = (0x30 << 12) | (TempData << 8) | Bank7Command;
      //
      // Write RC3x only if different that Vendor TI and Register Rev == 0xA
      //
      if (MrcData->MrcParameters.SpdData[Channel][Dimm].Buffer[SPD_DDR4_REGREV] == 0xA &&
          RegisterManufacturerId == RID_TI) {
        if (Dimm == 0) {
          MrcData->MspData.DynamicVars[Channel][RC3X_0]   = 0;
        } else {
          MrcData->MspData.DynamicVars[Channel][RC3X_1]   = 0;
        }
      } else {
        if (Dimm == 0) {
          MrcData->MspData.DynamicVars[Channel][RC3X_0]   = 1;
        } else {
          MrcData->MspData.DynamicVars[Channel][RC3X_1]   = 1;
        }
      }

      //
      // Set the rank select for the MRS RC
      //
      MrsValue[ChannelRankNum][18] |= DramCmdRankMask[ChannelRankNum];
      MrsValue[ChannelRankNum][19] |= DramCmdRankMask[ChannelRankNum];
      MrsValue[ChannelRankNum][20] |= DramCmdRankMask[ChannelRankNum];
      MrsValue[ChannelRankNum][21] |= DramCmdRankMask[ChannelRankNum];
      MrsValue[ChannelRankNum][22] |= DramCmdRankMask[ChannelRankNum];
      MrsValue[ChannelRankNum][23] |= DramCmdRankMask[ChannelRankNum];
      MrsValue[ChannelRankNum][24] |= DramCmdRankMask[ChannelRankNum];
      MrsValue[ChannelRankNum][25] |= DramCmdRankMask[ChannelRankNum];
      MrsValue[ChannelRankNum][26] |= DramCmdRankMask[ChannelRankNum];
      MrsValue[ChannelRankNum][27] |= DramCmdRankMask[ChannelRankNum];
      MrsValue[ChannelRankNum][28] |= DramCmdRankMask[ChannelRankNum];
      MrsValue[ChannelRankNum][29] |= DramCmdRankMask[ChannelRankNum];
      MrsValue[ChannelRankNum][30] |= DramCmdRankMask[ChannelRankNum];
      MrsValue[ChannelRankNum][31] |= DramCmdRankMask[ChannelRankNum];
      MrsValue[ChannelRankNum][32] |= DramCmdRankMask[ChannelRankNum];
      MrsValue[ChannelRankNum][33] |= DramCmdRankMask[ChannelRankNum];
      MrsValue[ChannelRankNum][34] |= DramCmdRankMask[ChannelRankNum];
      MrsValue[ChannelRankNum][7] |= DramCmdRankMask[ChannelRankNum];
      MrsValue[ChannelRankNum][8] |= DramCmdRankMask[ChannelRankNum];
    }
  }
  return MMRC_SUCCESS;
}
/**
  Initializes all dynamic variables used in JEDEC init for DDR4.

  @param[in, out]  MrcData       Host structure for all data related to MMRC.
  @param[in]       Channel       Current Channel being examined.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
InitializeDdr4JedecVariables (
  IN  OUT   MMRC_DATA   *MrcData,
  IN        UINT8        Channel
  )
{
  UINT8             Rank;
  UINT8             Dimm;
  UINT8             CurrentFrequency;
  UINT8             DramType;
  UINT8             SpdAddMap;
  UINT8             SpdDeviceOrg;
  UINT8             DimmType;
  UINT32            MrsValue[MAX_RANKS][MR_COUNT];
  DramInitDDR4MRS0  Mrs0Command;
  DramInitDDR4EMR1  Mrs1Command;
  DramInitDDR4EMR2  Mrs2Command;
  DramInitDDR4EMR3  Mrs3Command;
  DramInitDDR4EMR4  Mrs4Command;
  DramInitDDR4EMR5  Mrs5Command;
  DramInitDDR4EMR6  Mrs6Command;
  DramInitDDR4EMR6  Mrs6CommandVrefd;
  UINT16            Temp;
  UINT8             ChRankNum;

  PRINT_FUNCTION_INFO_MAX;

  MmrcMemset (MrsValue, 0, MAX_RANKS * MR_COUNT * sizeof (UINT32));

#if ODT_TABLE
  //
  // Set ODT Matrix
  //
  SetOdtMatrix (MrcData, 0, Channel, MrsValue);
#endif

  for (Dimm = 0; Dimm < MAX_DIMMS; Dimm++) {
    //
    // Skip to next populated rank
    //
    if (!MrcData->MrcParameters.DimmPresent[Channel][Dimm]) {
      continue;
    }

    for (Rank = 0; Rank < MAX_RANKS_PER_DIMM; Rank++) {
      ChRankNum = (2 * Dimm) + Rank;
      //
      // Skip to next populated rank
      //
      if (MrcData->MrcParameters.RankPresent[Channel][ChRankNum] == 0) {
        continue;
      }
      //
      // Setup for MR2. SetOdtMatrix initializes Rtt_WR MR1.
      //
      Mrs2Command.Data              = MrsValue[ChRankNum][2];
      Mrs2Command.Bits.bankAddress  = 2;
      Mrs2Command.Bits.bankGroup    = 0;
      Mrs2Command.Bits.ACT_N        = 1;
      Temp = MrcData->MrcParameters.TimingData[Channel][TimingTwcl][FreqA];

      if (Temp < 14) {
        Mrs2Command.Bits.CWL          = Temp - 9;
      } else {
        Mrs2Command.Bits.CWL          = Temp - ((Temp - 14) / 2) - 10;
      }
      //
      // All DDR4 Devices support ASR Mode.
      // ----------------------------------------------
      // 2b'11(A7:A6) = ASR Mode (Auto Self Refresh)
      //
      if (MrcData->PowerOn) {
        Mrs2Command.Bits.LPASR        = 0;
        MspDebugPrint ((MSP_DBG_MAX, "DDR4 ASR Extended temperature range disabled at PO...\n"));
      } else {
        Mrs2Command.Bits.LPASR        = 3;
        MspDebugPrint ((MSP_DBG_MAX, "DDR4 ASR Extended temperature range enabled...\n"));
      }
      //
      // Shadow MRS2    
      //                
      if (Rank == 0) {
        MrcData->MspData.DynamicVars[Channel][BITS7_3_DIMM0] = Mrs2Command.Bits.CWL | (Mrs2Command.Bits.LPASR << 3);
        MrcData->MspData.DynamicVars[Channel][BITS12_9_DIMM0] = Mrs2Command.Bits.RTT_WR | (Mrs2Command.Bits.writeCRC << 3);                      
      }
      if (Rank == 2) {
        MrcData->MspData.DynamicVars[Channel][BITS7_3_DIMM1] = Mrs2Command.Bits.CWL | (Mrs2Command.Bits.LPASR<<3);
        MrcData->MspData.DynamicVars[Channel][BITS12_9_DIMM1] = Mrs2Command.Bits.RTT_WR | (Mrs2Command.Bits.writeCRC<<3);
      }
      //
      // Setup for MR3
      //
      Mrs3Command.Data              = 0;
      Mrs3Command.Bits.bankAddress  = 3;
      Mrs3Command.Bits.PDA          = 0;
      Mrs3Command.Bits.ACT_N        = 1;
      switch (MrcData->MrcParameters.DdrFrequency) {
       case MMRC_1600:
         Mrs3Command.Bits.WL_crcdm  = 0;
         break;
       case MMRC_1866:
       case MMRC_2133:
       case MMRC_2400:
       case MMRC_2666:
         Mrs3Command.Bits.WL_crcdm  = 1;
         break;
       default:
         Mrs3Command.Bits.WL_crcdm  = 0;
       }
      //
      // SetOdtMatrix initializes RTT_NOM MR1.
      //
      Mrs1Command.Data              = MrsValue[ChRankNum][1];
      Mrs1Command.Bits.bankAddress  = 1;
      Mrs1Command.Bits.bankGroup    = 0;
      Mrs1Command.Bits.dllEnabled   = 1;
      if ((MrcData->DimmCount[Channel] == MAX_DIMMS) || (MrcData->OemMrcData.EnableMemoryDown[Channel][0]) || (MrcData->OemMrcData.EnableMemoryDown[Channel][1])) {
        Mrs1Command.Bits.DIC0         = 0;
      } else {
        Mrs1Command.Bits.DIC0         = 1;
        MspDebugPrint ((MSP_DBG_MAX, "setting RON to 48...\n"));
      }
      Mrs1Command.Bits.ACT_N        = 1;
      //
      // TDQS should be 1 for x8 and 0 for x4 and x16
      //
      Mrs1Command.Bits.TDQS         = 1;
      if ((MrcData->D_DataWidth[Channel][Dimm] == 0) || (MrcData->D_DataWidth[Channel][Dimm] == 2)) {
        Mrs1Command.Bits.TDQS         = 0;
      }
      //
      // Setup for MR0
      //
      Mrs0Command.Data              = 0;
      Mrs0Command.Bits.bankAddress  = 0;
      Mrs0Command.Bits.bankGroup    = 0;
      Mrs0Command.Bits.dllReset     = 1;
      Mrs0Command.Bits.BurstLength  = DQ_BURSTLEN_8;
      Mrs0Command.Bits.readBurstType= 0;
      Mrs0Command.Bits.ACT_N        = 1;

      //
      // RDIMM
      //
      Temp = MrcData->MrcParameters.TimingData[Channel][TimingTcl][FreqA];
      if (MrcData->TimingDataOverrides[Channel][TimingTcl][FreqA] != 0xFF) {
        Temp = MrcData->TimingDataOverrides[Channel][TimingTcl][FreqA];
      }
      if (Temp < 9) {
        Temp = 9;
      }
      if (Temp > 25) {
        Temp = 25;
      }
      Mrs0Command.Bits.CL = CasLat2Ddr4Mrs0CasLatIdx[Temp - 9] & 0x1;
      Mrs0Command.Bits.casLatency = CasLat2Ddr4Mrs0CasLatIdx[Temp - 9] >> 1;

      //
      // Consider making writeRecovery a formula based on
      // Roundup (MrcData->MrcParameters.TimingData[Channel][TimingTwr] / Tckmin)
      //
      switch (MrcData->MrcParameters.DdrFrequency) {
      case MMRC_1600:
        Mrs0Command.Bits.writeRecovery = 1;
        break;
      case MMRC_1866:
        Mrs0Command.Bits.writeRecovery = 2;
        break;
      case MMRC_2133:           
        Mrs0Command.Bits.writeRecovery = 3;
        break;
      case MMRC_2400:
        Mrs0Command.Bits.writeRecovery = 4;
        break; 
      case MMRC_2666:           
        Mrs0Command.Bits.writeRecovery = 5;
        break;
      default:
        Mrs0Command.Bits.writeRecovery = 1;
      } 
      //
      // Use DDR 800 to index zero
      //
      CurrentFrequency = MrcData->MrcParameters.DdrFrequency - MINDDR;
      //
      // Setup for MR4
      //
      Mrs4Command.Data              = 0;
      Mrs4Command.Bits.bankAddress  = 0;
      Mrs4Command.Bits.bankGroup    = 1;
      Mrs4Command.Bits.ACT_N        = 1;
      if (MrcData->MspData.DynamicVars[Channel][TWO_CLK_RD_PREAMBLE] == 1) {
        Mrs4Command.Bits.ReadPream = 1;
      }
      if (MrcData->MspData.DynamicVars[Channel][TWO_CLK_WR_PREAMBLE] == 1) {
        Mrs4Command.Bits.WritePream = 1;
      }
      //
      // Setup for MR5. SetOdtMatrix initializes RttPrk MR5
      //
      Mrs5Command.Data = MrsValue[ChRankNum][5];
      Mrs5Command.Bits.Odt_inbuff   = 0;
      Mrs5Command.Bits.bankAddress  = 1;
      Mrs5Command.Bits.bankGroup    = 1;
      Mrs5Command.Bits.ACT_N        = 1;
      Mrs5Command.Bits.CA_paritylat = 0;
      /*
      This is not needed. DNV does not support parity at the DRAMs, only at Register level. TODO clean
      if (MrcData->CommandAddressParity && MrcData->MspData.DynamicVars[Channel][DIMMTYPE] !=  SPD_DDR_RDIMM) {
        switch (MrcData->MrcParameters.DdrFrequency)  {
        case MMRC_1600:
        case MMRC_1866:
        case MMRC_2133:
          Mrs5Command.Bits.CA_paritylat = 1;
          break;
        case MMRC_2400:
        case MMRC_2666:
          Mrs5Command.Bits.CA_paritylat = 2;
          break;
        default :
          Mrs5Command.Bits.CA_paritylat = 0;
          break;
        }
      }
      */
#if defined SIM
      MspDebugPrint ((
        MSP_DBG_VERBOSE, 
        "TB Override MR5:%d - %d\n",
        Mrs5Command.Bits.RTT_PARK,
        ((MrcData->TbMr5[Channel] >> 6) & 7)
        ));
      Mrs5Command.Bits.RTT_PARK = (MrcData->TbMr5[Channel] >> 6) & 7;
#endif
      //
      // Setup for MR6. SetOdtMatrix initializes VrefDQ Training Value MR6
      //
      Mrs6Command.Data = MrsValue[ChRankNum][6];
      Mrs6Command.Bits.bankAddress  = 2;
      Mrs6Command.Bits.bankGroup    = 1;
      Mrs6Command.Bits.Vrefdq_Enable= 0;
      Mrs6Command.Bits.ACT_N        = 1;
      Mrs6Command.Bits.tCCD_L       = (UINT32) MrcData->MspData.DynamicVars[Channel][TCCD_L_WR];

      Mrs0Command.Bits.rankSelect = (ChRankNum);
      Mrs1Command.Bits.rankSelect = (ChRankNum);
      Mrs2Command.Bits.rankSelect = (ChRankNum);
      Mrs3Command.Bits.rankSelect = (ChRankNum);
      Mrs4Command.Bits.rankSelect = (ChRankNum);
      Mrs5Command.Bits.rankSelect = (ChRankNum);
      Mrs6Command.Bits.rankSelect = (ChRankNum);
      Mrs6CommandVrefd.Data = Mrs6Command.Data;
      Mrs6CommandVrefd.Bits.Vrefdq_Enable = 1;
      //
      // DDR4 address mirroring and inversion. Inversion only affects RDIMMS
      //
      DramType = MrcData->MrcParameters.DramType[Channel];
      SpdAddMap = MrcData->MrcParameters.SpdData[Channel][Dimm].Buffer[SPD_DDR4_ADD_MAPPING];
      SpdDeviceOrg = MrcData->MrcParameters.SpdData[Channel][Rank / MAX_RANKS_PER_DIMM].Buffer[SPD_DDR4_ORG];
      DimmType = MrcData->D_Type[Channel][Dimm];
      HandleAddressMirroring(&Mrs0Command.Data, SpdAddMap, SpdDeviceOrg, DramType, DimmType, SIDE_A);
      HandleAddressMirroring(&Mrs1Command.Data, SpdAddMap, SpdDeviceOrg, DramType, DimmType, SIDE_A);
      HandleAddressMirroring(&Mrs2Command.Data, SpdAddMap, SpdDeviceOrg, DramType, DimmType, SIDE_A);
      HandleAddressMirroring(&Mrs3Command.Data, SpdAddMap, SpdDeviceOrg, DramType, DimmType, SIDE_A);
      HandleAddressMirroring(&Mrs4Command.Data, SpdAddMap, SpdDeviceOrg, DramType, DimmType, SIDE_A);
      HandleAddressMirroring(&Mrs5Command.Data, SpdAddMap, SpdDeviceOrg, DramType, DimmType, SIDE_A);
      HandleAddressMirroring(&Mrs6Command.Data, SpdAddMap, SpdDeviceOrg, DramType, DimmType, SIDE_A);
      HandleAddressMirroring(&Mrs6CommandVrefd.Data, SpdAddMap, SpdDeviceOrg, DramType, DimmType, SIDE_A);
      //
      // Save MRS settings for later use
      //
      MrcData->MspData.DynamicVars[Channel][MrsCommandIndex[ChRankNum][0]] = Mrs0Command.Data;
      MrcData->MspData.DynamicVars[Channel][MrsCommandIndex[ChRankNum][1]] = Mrs1Command.Data;
      MrcData->MspData.DynamicVars[Channel][MrsCommandIndex[ChRankNum][2]] = Mrs2Command.Data;
      MrcData->MspData.DynamicVars[Channel][MrsCommandIndex[ChRankNum][3]] = Mrs3Command.Data;
      MrcData->MspData.DynamicVars[Channel][MrsCommandIndex[ChRankNum][4]] = Mrs4Command.Data;
      MrcData->MspData.DynamicVars[Channel][MrsCommandIndex[ChRankNum][5]] = Mrs5Command.Data;
      MrcData->MspData.DynamicVars[Channel][MrsCommandIndex[ChRankNum][6]] = Mrs6Command.Data;
      //
      // Turn on Vref Dq training
      //
      MrcData->MspData.DynamicVars[Channel][MrsCommandIndex[ChRankNum][9]] = Mrs6CommandVrefd.Data;
      //
      // RDIMM's Side B devices
      //
      if (MrcData->MspData.DynamicVars[Channel][DIMMTYPE] == SPD_DDR_RDIMM) {
        Mrs0Command.Data = ReadMrsCommand(MrcData, Channel, ChRankNum, 0);
        Mrs1Command.Data = ReadMrsCommand(MrcData, Channel, ChRankNum, 1);
        Mrs2Command.Data = ReadMrsCommand(MrcData, Channel, ChRankNum, 2);
        Mrs3Command.Data = ReadMrsCommand(MrcData, Channel, ChRankNum, 3);
        Mrs4Command.Data = ReadMrsCommand(MrcData, Channel, ChRankNum, 4);
        Mrs5Command.Data = ReadMrsCommand(MrcData, Channel, ChRankNum, 5);
        Mrs6Command.Data = ReadMrsCommand(MrcData, Channel, ChRankNum, 6);
        Mrs6CommandVrefd.Data = ReadMrsCommand(MrcData, Channel, ChRankNum, 6);
        Mrs6CommandVrefd.Bits.Vrefdq_Enable = 1;
        HandleAddressMirroring(&Mrs0Command.Data, SpdAddMap, SpdDeviceOrg, DramType, DimmType, SIDE_B);
        HandleAddressMirroring(&Mrs1Command.Data, SpdAddMap, SpdDeviceOrg, DramType, DimmType, SIDE_B);
        HandleAddressMirroring(&Mrs2Command.Data, SpdAddMap, SpdDeviceOrg, DramType, DimmType, SIDE_B);
        HandleAddressMirroring(&Mrs3Command.Data, SpdAddMap, SpdDeviceOrg, DramType, DimmType, SIDE_B);
        HandleAddressMirroring(&Mrs4Command.Data, SpdAddMap, SpdDeviceOrg, DramType, DimmType, SIDE_B);
        HandleAddressMirroring(&Mrs5Command.Data, SpdAddMap, SpdDeviceOrg, DramType, DimmType, SIDE_B);
        HandleAddressMirroring(&Mrs6Command.Data, SpdAddMap, SpdDeviceOrg, DramType, DimmType, SIDE_B);
        HandleAddressMirroring(&Mrs6CommandVrefd.Data, SpdAddMap, SpdDeviceOrg, DramType, DimmType, SIDE_B);
        //
        // Save MRS settings for later use RDIMM's Side B devices
        //
        MrcData->MspData.DynamicVars[Channel][MrsCommandIndex[ChRankNum][10]] = Mrs0Command.Data;
        MrcData->MspData.DynamicVars[Channel][MrsCommandIndex[ChRankNum][11]] = Mrs1Command.Data;
        MrcData->MspData.DynamicVars[Channel][MrsCommandIndex[ChRankNum][12]] = Mrs2Command.Data;
        MrcData->MspData.DynamicVars[Channel][MrsCommandIndex[ChRankNum][13]] = Mrs3Command.Data;
        MrcData->MspData.DynamicVars[Channel][MrsCommandIndex[ChRankNum][14]] = Mrs4Command.Data;
        MrcData->MspData.DynamicVars[Channel][MrsCommandIndex[ChRankNum][15]] = Mrs5Command.Data;
        MrcData->MspData.DynamicVars[Channel][MrsCommandIndex[ChRankNum][16]] = Mrs6Command.Data;
        //
        // Turn on Vref Dq training
        //
        MrcData->MspData.DynamicVars[Channel][MrsCommandIndex[ChRankNum][17]] = Mrs6CommandVrefd.Data;
      }
      //
      // Register Control Words Setup only needs side A
      //
      RegisterControlWordSetup (MrcData, Channel, Dimm, Rank, MrsValue);
      HandleAddressMirroring(&MrsValue[ChRankNum][18], SpdAddMap, SpdDeviceOrg, DramType, DimmType, SIDE_A);
      HandleAddressMirroring(&MrsValue[ChRankNum][19], SpdAddMap, SpdDeviceOrg, DramType, DimmType, SIDE_A);
      HandleAddressMirroring(&MrsValue[ChRankNum][20], SpdAddMap, SpdDeviceOrg, DramType, DimmType, SIDE_A);
      HandleAddressMirroring(&MrsValue[ChRankNum][21], SpdAddMap, SpdDeviceOrg, DramType, DimmType, SIDE_A);
      HandleAddressMirroring(&MrsValue[ChRankNum][22], SpdAddMap, SpdDeviceOrg, DramType, DimmType, SIDE_A);
      HandleAddressMirroring(&MrsValue[ChRankNum][23], SpdAddMap, SpdDeviceOrg, DramType, DimmType, SIDE_A);
      HandleAddressMirroring(&MrsValue[ChRankNum][24], SpdAddMap, SpdDeviceOrg, DramType, DimmType, SIDE_A);
      HandleAddressMirroring(&MrsValue[ChRankNum][25], SpdAddMap, SpdDeviceOrg, DramType, DimmType, SIDE_A);
      HandleAddressMirroring(&MrsValue[ChRankNum][26], SpdAddMap, SpdDeviceOrg, DramType, DimmType, SIDE_A);
      HandleAddressMirroring(&MrsValue[ChRankNum][27], SpdAddMap, SpdDeviceOrg, DramType, DimmType, SIDE_A);
      HandleAddressMirroring(&MrsValue[ChRankNum][28], SpdAddMap, SpdDeviceOrg, DramType, DimmType, SIDE_A);
      HandleAddressMirroring(&MrsValue[ChRankNum][29], SpdAddMap, SpdDeviceOrg, DramType, DimmType, SIDE_A);
      HandleAddressMirroring(&MrsValue[ChRankNum][30], SpdAddMap, SpdDeviceOrg, DramType, DimmType, SIDE_A);
      HandleAddressMirroring(&MrsValue[ChRankNum][31], SpdAddMap, SpdDeviceOrg, DramType, DimmType, SIDE_A);
      HandleAddressMirroring(&MrsValue[ChRankNum][32], SpdAddMap, SpdDeviceOrg, DramType, DimmType, SIDE_A);
      HandleAddressMirroring(&MrsValue[ChRankNum][33], SpdAddMap, SpdDeviceOrg, DramType, DimmType, SIDE_A);
      HandleAddressMirroring(&MrsValue[ChRankNum][34], SpdAddMap, SpdDeviceOrg, DramType, DimmType, SIDE_A);
      HandleAddressMirroring(&MrsValue[ChRankNum][7], SpdAddMap, SpdDeviceOrg, DramType, DimmType, SIDE_A);
      HandleAddressMirroring(&MrsValue[ChRankNum][8], SpdAddMap, SpdDeviceOrg, DramType, DimmType, SIDE_A);
      MrcData->MspData.DynamicVars[Channel][MrsCommandIndex[ChRankNum][18]] = MrsValue[ChRankNum][18];
      MrcData->MspData.DynamicVars[Channel][MrsCommandIndex[ChRankNum][19]] = MrsValue[ChRankNum][19];
      MrcData->MspData.DynamicVars[Channel][MrsCommandIndex[ChRankNum][20]] = MrsValue[ChRankNum][20];
      MrcData->MspData.DynamicVars[Channel][MrsCommandIndex[ChRankNum][21]] = MrsValue[ChRankNum][21];
      MrcData->MspData.DynamicVars[Channel][MrsCommandIndex[ChRankNum][22]] = MrsValue[ChRankNum][22];
      MrcData->MspData.DynamicVars[Channel][MrsCommandIndex[ChRankNum][23]] = MrsValue[ChRankNum][23];
      MrcData->MspData.DynamicVars[Channel][MrsCommandIndex[ChRankNum][24]] = MrsValue[ChRankNum][24];
      MrcData->MspData.DynamicVars[Channel][MrsCommandIndex[ChRankNum][25]] = MrsValue[ChRankNum][25];
      MrcData->MspData.DynamicVars[Channel][MrsCommandIndex[ChRankNum][26]] = MrsValue[ChRankNum][26];
      MrcData->MspData.DynamicVars[Channel][MrsCommandIndex[ChRankNum][27]] = MrsValue[ChRankNum][27];
      MrcData->MspData.DynamicVars[Channel][MrsCommandIndex[ChRankNum][28]] = MrsValue[ChRankNum][28];
      MrcData->MspData.DynamicVars[Channel][MrsCommandIndex[ChRankNum][29]] = MrsValue[ChRankNum][29];
      MrcData->MspData.DynamicVars[Channel][MrsCommandIndex[ChRankNum][30]] = MrsValue[ChRankNum][30];
      MrcData->MspData.DynamicVars[Channel][MrsCommandIndex[ChRankNum][31]] = MrsValue[ChRankNum][31];
      MrcData->MspData.DynamicVars[Channel][MrsCommandIndex[ChRankNum][32]] = MrsValue[ChRankNum][32];
      MrcData->MspData.DynamicVars[Channel][MrsCommandIndex[ChRankNum][33]] = MrsValue[ChRankNum][33];
      MrcData->MspData.DynamicVars[Channel][MrsCommandIndex[ChRankNum][34]] = MrsValue[ChRankNum][34];
      MrcData->MspData.DynamicVars[Channel][MrsCommandIndex[ChRankNum][7]] = MrsValue[ChRankNum][7];
      MrcData->MspData.DynamicVars[Channel][MrsCommandIndex[ChRankNum][8]] = MrsValue[ChRankNum][8];

      MspDebugPrint ((MSP_DBG_MAX, "\n"));
    } // Rank Loop ..
  } // Dimm Loop ..

  //
  // If in simulation, just stall for 1us after all ranks to save time.
  //
#ifdef SIM
  MySimStall (1000);
#endif
  return MMRC_SUCCESS;
}

/**
  This configures the checkpoints using the Port: 01h Offset: 4Bh Sticky Scratch Pad register 1 (SSKPD1)
 
  @param[in, out]  MrcData       Host structure for all data related to MMRC.
  @retval          MMRC_SUCCESS  Checkpoint configured correctly
**/
MMRC_STATUS
CheckpointConfigure (
  IN        MMRC_DATA   *MrcData
  )
{
  UINT32                          CheckpointCode;
  REGISTER_ACCESS                 Register;
  //
  // Overridecheckpoints = 0 (Auto) normal behavior, warm reset can be executed without issues
  // Overridecheckpoints = 1 (Enabled) warm reset can't be executed, only cold-reset, so a breakpoint is set,
  //                                   in order to configure a checkpoint
  // Overridecheckpoints = 2 (Disabled) no need for checkpoints thru this register
  //
  Register.Offset = SSKPD1_DUNIT_COMMON_REG;
  Register.Mask = 0xFFFFFFFF;
  if (MrcData->OverrideCheckpoints == 1) {
    //
    // Clear register
    //
    MemRegWrite (DUNIT, 0, 0, Register, 0, 0xF);
    CheckpointCode = (UINT32) MemRegRead (DUNIT, 0, 0, Register);
    MspDebugPrint ((MSP_DBG_MIN, "Waiting to checkpoint configuration..."));
    //
    // Waits until a checkpoint is configured
    //
    while (CheckpointCode == 0) {
      CheckpointCode = (UINT32) MemRegRead (DUNIT, 0, 0, Register);
    }
  } else if (MrcData->OverrideCheckpoints == 2) {
    MemRegWrite (DUNIT, 0, 0, Register, 0, 0xF);
  }
  return MMRC_SUCCESS;
}

/**
  Executes the SPD-Only reset when needed

  @param[in, out]  MrcData       Host structure for all data related to MMRC.
  @retval          TRUE          If a reset is required due to a change in the frequency
  @retval          FALSE         Otherwise

  BIOS can use the following bits to determine where to extract the iCLK DDR Limit set by PMC (Fuse vs Sofstrap)
  PCH_PM_STS2[SPD_RESET_PCODE ]	PCH_PM_STS[ SPD_BIOS_RESET ]	BIOS Action
  0	                            0	                            Use existing method to get DDR limit
                                                              (i.e., Read from fuses)
  0	                            1	                            Illegal state:
                                                              Use existing method to get DDR limit
                                                              (i.e., Read from fuses)
  1	                            0	                            Booting from G3 or Warm Reset (not SPD Reset)
                                                              Read DDR limit from
                                                              PCH_PM_STS2[CURRENT_SPD_SPEED ]
  1	                            1	                            Booting from an SPD Reset initiated by BIOS.
                                                              Read DDR limit from
                                                              PCH_PM_STS2[CURRENT_SPD_SPEED ]
**/
BOOLEAN
SpdResetNeeded (
  IN  OUT   MMRC_DATA   *MrcData
  )
{
  UINT8                                            SpdSpeedRequested;
  UINT8                                            SpdSpeedCurrentHw;
  BOOLEAN                                          SpdResetRequired;
  UINT8                                            Channel;
  MMRC_STATUS                                      Status;
#ifdef SIM
  return FALSE;
#endif
  //
  // Find an active channel that have read the fuses and current DDR freq
  //
  Channel = FindActiveChannel (MrcData);

  SpdResetRequired     = FALSE;
  //
  // SpdResetNeededEntry reads the Spd_Reset_Status from the Punit and the Mrc_Flow_Status from SSKP0
  //
  Status = MmrcExecuteTask(MrcData, SpdResetNeededEntry, 0, Channel);

  MspDebugPrint ((MSP_DBG_MED, "MrcFlowStatus           = 0x%08x\n", MrcData->MspData.DynamicVars[Channel][MRC_FLOW_STATUS]));
  MspDebugPrint ((MSP_DBG_MED, "SpdResetStatus (Fuse)   = 0x%08x\n", MrcData->MspData.DynamicVars[Channel][SPD_RESET_STATUS]));
  MspDebugPrint((MSP_DBG_MED,  "SPD_RESET_PCODE (Soft Strap)   = 0x%08x\n", MrcData->MspData.DynamicVars[Channel][SPD_RESET_PCODE]));
  MspDebugPrint((MSP_DBG_MED,  "SPD_BIOS_RESET                 = 0x%08x\n", MrcData->MspData.DynamicVars[Channel][SPD_BIOS_RESET]));
  //
  // SPD_SPEED = CurrentFrequency - MINDDR  due to translate from MMRC defines to PMC register field values:
  // MMRC_1600   0x03       000=ddr 1600
  // MMRC_1866   0x04       001=ddr 1867
  // MMRC_2133   0x05       010=ddr 2133
  // MMRC_2400   0x06       011=ddr 2400
  // MMRC_2666   0x07       100=ddr 2667
  //
  SpdSpeedRequested = MrcData->MrcParameters.DdrFrequency - MINDDR;
  
  if (MrcData->MrcParameters.DdrFrequency < MMRC_1600 || MrcData->MrcParameters.DdrFrequency > MMRC_2666) {
    MspDebugPrint ((MSP_DBG_MIN, "CurrentFrequency: %d not Supported. Skipping SPD Reset\n", MrcData->MrcParameters.DdrFrequency));
    return FALSE;
  }
  //
  // First detect if the system is using Soft Strap SPD indications based on the table above to descide were to gather information
  //
  if (MrcData->MspData.DynamicVars[Channel][SPD_RESET_PCODE] == 1){
    //
    // Clear bit 3 when using the soft strap indication that bit means valid value
    //
    SpdSpeedCurrentHw = (7 & MrcData->MspData.DynamicVars[Channel][CURRENT_SPD_SPEED]);
  } else {
    //
    // Use the Fuse Value
    //
    SpdSpeedCurrentHw = (UINT8) MrcData->MspData.DynamicVars[Channel][DDR_CURRENT_FREQ];
  }
  MspDebugPrint((MSP_DBG_MED, "SpdSpeedCurrentHw  = 0x%08x\n", SpdSpeedCurrentHw));

  if ((MrcData->MspData.DynamicVars[Channel][SPD_RESET_STATUS] || MrcData->MspData.DynamicVars[Channel][SPD_BIOS_RESET]) == 1) {
    Status = MmrcExecuteTask(MrcData, ClearSpdResetFlag, 0, Channel);
    if ((SpdSpeedCurrentHw != SpdSpeedRequested) &&
        (MrcData->MspData.DynamicVars[Channel][MRC_FLOW_STATUS] != MmrcNotStarted)) {
        //
        // Mrc finish at least once or left in the middle of training, but the desired frequency doesn't match the SPD
        // reset frequency, so a power cycle needs to be issued in order to train again
        //
        MspDebugPrint ((
          MSP_DBG_MIN,
          "SPD reset is needed and MRC already completed at least once or it was interrupted in the middle, means that previous "\
          " training was done with different DDR frequency. Let's force a power cycle and train again with the new frequency\n"
        ));
        if (MrcData->MspData.CurrentPlatform[0] != P_SIMICS && MrcData->MspData.CurrentPlatform[0] != P_EMU && MrcData->MspData.CurrentPlatform[0] != P_JTAG) {
          IoOut8(0xCF9, 0x0);
          IoOut8(0xCF9, 0xE);
#ifndef SIM
          _asm hlt
#endif
          MrcData->ErrorCode = MrcErrWarmResetError;
          MrcDeadLoop();
        }
    }
    SpdResetRequired = FALSE;
  } else if (SpdSpeedCurrentHw != SpdSpeedRequested) {
    SpdResetRequired = TRUE;
  } else {
    SpdResetRequired = FALSE;
  }
  //
  // Update the Spd Reset Parameters (SPD_DDR_SPEED, SPD_BIOS_RESET) on the PMC PCH_PM_STS and update the SSKPD0
  //
  if (SpdResetRequired == TRUE) {
    //
    // Setting PCH_PM_STS[SPD_DDR_SPEED] DynamicVars to 0 since PMC_PCI_MMR BoxType has one Instance PerChannel only and PMC_PCI_MMR is Channel independent.
    //
    MrcData->MspData.DynamicVars[0][SPD_DDR_SPEED] = SpdSpeedRequested; 
    MrcData->MspData.DynamicVars[0][MRC_SPD_FREQUENCY] = MrcData->MrcParameters.DdrFrequency;
    MspDebugPrint ((
              MSP_DBG_MIN,
              "SpdSpeedRequested %d DdrFrequency = %d\n",
              SpdSpeedRequested, MrcData->MrcParameters.DdrFrequency
            ));
    Status = MmrcExecuteTask(MrcData, SetSpdResetPmc, 0, Channel);
  }

  return SpdResetRequired;
}

/**
  Verifies if a reset is required due to change in frequency, voltage and/or channels.

  @param[in, out]  MrcData       Host structure for all data related to MMRC.
  @retval          NONE
**/
VOID
VerifyResetRequests (
  IN  OUT   MMRC_DATA         *MrcData
  )
{
  MRC_RESET_FLAGS   MrcResetFlags;
#ifdef SIM
  return;
#endif

  MmrcMemset (&MrcResetFlags, 0, sizeof (MRC_RESET_FLAGS));
  //
  // Verify if an spd reset is required
  //
  MrcResetFlags.FrequencyResetRequired = SpdResetNeeded (MrcData);

  if (MrcResetFlags.FrequencyResetRequired) {
    MspDebugPrint ((MSP_DBG_MIN, "RESET required: change of frequency\n"));
    //
    // Do regular warm Reset
    //
    if (MrcData->MspData.CurrentPlatform[0] != P_SIMICS && MrcData->MspData.CurrentPlatform[0] != P_EMU && MrcData->MspData.CurrentPlatform[0] != P_JTAG) {
      IoOut8(0xCF9, 0x00);
      IoOut8(0xCF9, 0x06);

#if __GNUC__
      //asm("cli":::); no rests on sim
      //asm("hlt":::);
#else
        _asm cli
        _asm hlt
#endif
      MrcData->ErrorCode = MrcErrWarmResetError;
      MrcDeadLoop(); 
    }
  }
}

/**
  Configure the memory thru several training steps

  @param[in, out]  MrcData       Host structure for all data related to MMRC.
  @retval          Status
**/
MMRC_STATUS
ConfigureMemory (
  IN  OUT   MMRC_DATA   *MrcData
  )
{
  MMRC_STATUS        Status;
#ifdef MINIBIOS
  //
  // All default parameters should be initialized here. This routine is called from 3 places:
  // 1) On MiniBIOS build, it is called here.
  // 2) On full BIOS build, it is called from MemoryInit.c.
  // 3) On RTL simulation build it is called from mrc_wrapper.c in Linux.
  //
  MrcInitializeParameters (MrcData);
#endif

  //
  // Configure SMBus SPD Write Disable function
  //
  Status = ConfigureSMBusSpdWriteDisable (MrcData);

#if defined SIM || defined JTAG
#if defined SIM
  time_t curtime;
  struct tm *loctime;
#endif
  gLogFile = MrcData->LogFile;
#endif



#ifdef SIM
  //
  // Get the current time. 
  //
  curtime = time ((time_t *)NULL);
  //
  // Convert it to local time representation.
  //
  loctime = localtime (&curtime);
  //
  // Print out the date and time in the standard format.
  //
  MspDebugPrint ((MSP_DBG_MIN, "MRC Start: %s\n", asctime (loctime)));
#endif
  MspDebugPrint ((MSP_DBG_MIN, "MRC VERSION: 0x%x\n", MRC_VERSION));
  MspDebugPrint ((MSP_DBG_MIN, "MRCDATA Size: %x\n", sizeof (MMRC_DATA)));
  MspDebugPrint ((MSP_DBG_MIN, "MRC_SAVE_RESTORE Size: %x\n", sizeof (MRC_SAVE_RESTORE)));
  MspDebugPrint ((MSP_DBG_MIN, "SocStepping: %d\n",   MrcData->SocStepping));
  //
  // Populate all input parameters that MMRC needs to execute.
  //
  GetInputParameters (MrcData);
  //
  // Convert input parameters to actual values which will be written
  // into registers. For example, if TCL of 11 needs a value of 6 
  // written to the DUNIT, this routine will change TCL to 6.
  //
  Status = ConvertInputParameters (MrcData);
  if (Status != MMRC_SUCCESS) {
    MrcData->ErrorCode = Status;
    MrcDeadLoop();
  }
  //
  // Single entry point to MMRC to execute the entire flow.
  //
  //
  // Checkpoint Configuring
  //
  CheckpointConfigure (MrcData);

  Status = MmrcEntry (MrcData, EntireMmrc, (HANDSHAKE_PARAMETERS *)NULL);

  if (Status != MMRC_SUCCESS) {
    MrcData->ErrorCode = Status;
    MrcDeadLoop();
  }

  FillOutputStructure (MrcData);
  //
  // If we're in the MiniBIOS, MRC has completed. Now need to deadloop and optionally
  // display all delays and dump all registers.
  //
#ifdef MINIBIOS
  MrcData->ErrorCode = MrcErrOk;
  MrcDeadLoop();
#endif

  return Status;
}

/**
  Mark MRC as started so that if a reset occurs during the middle of MRC,
  we know the memory subsystem is partially initialized. In this case, we
  need to force a powergood reset to get the DDRIO back into a reset state
  where the registers are at default values.

  @param[in, out]  MrcData       Host structure for all data related to MMRC. 
  @retval          NONE 
**/
VOID
RecordMrcStart (
  IN  OUT   MMRC_DATA   *MrcData
  )
{
  MRC_FLOW_FLAGS      MrcFlowFlags;
  REGISTER_ACCESS     Register;

  Register.Offset   = SSKPD0_DUNIT_COMMON_REG;
  Register.Mask     = 0xFFFFFFFF;
  Register.ShiftBit = 0;
  MrcFlowFlags.Data = (UINT32) MemRegRead (DUNIT_COMMON, 0, 0, Register);
  //
  // SSKPD0_DUNIT_REG[1:0]= 1 MRC started; 2 MRC finished
  // Lets restart the value to MRC_STARTED in case a warm reset is executed during the training
  //
  MrcFlowFlags.Bits.MrcFlowStatus = MmrcStarted;
  MemRegWrite (DUNIT_COMMON, 0, 0, Register, MrcFlowFlags.Data, 0xF);
}

/**
  Routine to initialize other pieces of silicon which may be required
  for MRC to successfully initialize memory. High precision event timer,
  PUNIT mailbox, etc.

  @param   MrcData       Host structure for all data related to MMRC.
  @retval  Status        
**/
MMRC_STATUS
ExternalSiliconInit (
  IN  OUT   MMRC_DATA   *MrcData
  )
{
  UINT8 i;
#if !defined SIM && !defined JTAG
#ifdef MINIBIOS
#if DEBUG_MSG
#ifndef CAR
  Init16550Uart ();
#endif
#endif
#endif
  
  if (GetPlatformSettings (MrcData) == MMRC_FAILURE) {
    MrcData->ErrorCode = MrcErrPlatformSettings;
    MrcDeadLoop ();
  }
  //
  // Initialize the EcBase Properly
  //
  MspInit (&MrcData->MspData, MrcData, &SetMspData);

#else
  MrcData->SbRegBase = 0xFD000000;
  MrcData->MchBar    = 0xFED10000;  
  MrcData->EcBase    = EC_BASE;
  MrcData->PmcBar    = 0xFE000000;
  //
  // Initialize the EcBase Properly
  //
  MspInit (&MrcData->MspData, MrcData, &SetMspData);

#endif // SIM && JTAG
  //
  // Assign Register Access Dynamic Variables
  //
  for (i=0 ; i < MAX_CHANNELS; i++) {
    MrcData->MspData.DynamicVars[i][P2SBBAR] = MrcData->SbRegBase;
    MrcData->MspData.DynamicVars[i][MCHBAR]  = MrcData->MchBar;
    MrcData->MspData.DynamicVars[i][PMCBAR]  = MrcData->PmcBar;

    MspDebugPrint ((MSP_DBG_MAX, "P2SBBAR[%d] = 0x%08x\n", i, MrcData->MspData.DynamicVars[i][P2SBBAR]));
    MspDebugPrint ((MSP_DBG_MAX, "MCHBAR[%d]  = 0x%08x\n", i, MrcData->MspData.DynamicVars[i][MCHBAR]));
    MspDebugPrint ((MSP_DBG_MAX, "PMCBAR[%d]  = 0x%08x\n", i, MrcData->MspData.DynamicVars[i][PMCBAR]));
  }

  return MMRC_SUCCESS;
}

/**
  This routine is responsible to initialize ALL data required for
  MMRC to initialize memory. Typical flow for server would be:

  1) DIMM detection
  2) Timing calculations
  3) Variable initialization
  4) BIOS Setup overrides

  Typical phone/tablet flow would be:

  1) Timing variable population via lookup table
  2) BIOS Setup overrides

  This routine can be very complex in server segments or extremely 
  lightweight in phone/tablet segments, depending on code size and
  MRC execution time requirements.

  @param[in, out]  MrcData       Host structure for all data related to MMRC.
  @retval          Status
**/
MMRC_STATUS
GetInputParameters (
  IN  OUT   MMRC_DATA   *MrcData
  )
{
#if DETECT_DIMMS
  DetectDimms (MrcData);
#endif

  //
  // Update BootMode if the NonVolatile Memory path is needed
  //
  {
    UINT32 ModeChange = PlatformUpdateBootMode(MrcData);

    if (ModeChange){
      if (NVDIMM_ADR_RECOVERY == ModeChange) {
        MrcData->MrcParameters.BootMode = ADR;
      }
    }
  }

  //
  // Select to use the restore path here we decide here after DetermineBootMode 
  // and DetectDimms, this may change for memory down we decide here in case a define
  // prevents DetermineBootMode or DetectDimms functions to execute
  //
  if (MrcData->MrcParameters.BootMode != S5) {
    MrcData->RestorePath = TRUE;
    MspDebugPrint ((MSP_DBG_MIN, "Restore Path\n"));
  } else {
    MrcData->RestorePath = FALSE;
    MspDebugPrint ((MSP_DBG_MIN, "Normal Path\n"));
  }
  //
  // Get timing parameters requested by user.
  //
  TimingParams (MrcData);
#ifdef SIM  
  //
  // Override the timing settings with the 
  // ones provided by the SIM environment
  //
  OverrideTimingParams (MrcData);  
#endif  
  //
  // Initialize the modular MRC prior to use.
  //
  FillModMrcDataStructure (MrcData);
  //
  // Initialize the Msp Data
  //
  MspInit (&MrcData->MspData, MrcData, &SetMspData);
  //
  // Mark MRC as started.
  //
  RecordMrcStart (MrcData);

  return MMRC_SUCCESS;
}

UINT32 
ChannelSizeFusetoMBSize (
  IN UINTX FuseValue
  ) 
{
  //
  // Reserverd value
  //
  if (FuseValue == 6)
    return 0;
  //
  //  No limit
  //
  if (FuseValue == 7)
    return 0xFFFFFFFF;
  //
  // 2GB to 64GB
  //
  return (1 << (FuseValue + 11));
}

/**
  This routine is responsible of configuring: Interleaving Mode, Slice hash 
  function and Asymmetric Configurations.
  Interleaving Modes are the following:
  0) Disable
  1) Mode 0
  2) Mode 1
  3) Mode 2
  4) HVM Mode

  Asymmetric Configurations are autodetected and configured depending on
  the Interleaving Mode selected. The Hardware accounts for the MMIO and substracts accordingly.
  There are two cases:

  1) One Asymmetric Memory Region
    Address between 0 and BASE0 - Fully interleaved.
    Memory above BASE0 - Non Interleaved
  2) Two Asymmetric Memory Regions
    BASE0 is always 0.
    BASE1 always > 0
    All memory is Non-Interleaved

  @param[in, out]  MrcData       Host structure for all data related to MMRC.
**/
VOID
ConfigureInterleaving (
  IN  OUT   MMRC_DATA   *MrcData
  )
{
#if INTERLEAVING_SUPPORT
  UINT32 AsymBase;
  UINT32 AsymLimit;
  AsymBase = 0;
  AsymLimit = 0;

  MrcData->MspData.DynamicVars[0][HVM_MODE] = 0;
  //
  // Set HVM Mode. If set the rest of the Slice config is a don't care.
  //
  if (MrcData->InterleaveMode == HVM) {
    MrcData->MspData.DynamicVars[0][HVM_MODE] = 1;
    return;
  }  

  if (MrcData->MrcParameters.ChEnabled[0] && MrcData->MrcParameters.ChEnabled[1]) {
    if (NVDIMM_MODE == MrcData->NonVolMemMode) {
      UINT8 NvDimmPesentCh0 = (0 != (MrcData->NvDimmType[0][0] || MrcData->NvDimmType[0][1]));
      UINT8 NvDimmPesentCh1 = (0 != (MrcData->NvDimmType[1][0] || MrcData->NvDimmType[1][1]));

      //
      // Interleaving between Standard DIMMs and NVDIMMs should not occur.
      // Interleaving can occure if the same DIMM type is in the same slot of each channel
      //
      if (NvDimmPesentCh0 || NvDimmPesentCh1) {
        UINT8 Slot0DimmTypesMatch = ( (MrcData->NvDimmType[0][0] == MrcData->NvDimmType[1][0]));
        UINT8 Slot1DimmTypesMatch = ( (MrcData->NvDimmType[0][1] == MrcData->NvDimmType[1][1]));
        //        if (MrcData->MrcParameters.DimmPresent[0][Dimm] && MrcData->MrcParameters.DimmPresent[1][Dimm]) {
        if ( ( (MrcData->TotalDimm <= 2)  && (!NvDimmPesentCh0 || !NvDimmPesentCh1) )
             || ( (MrcData->TotalDimm == 3) && !Slot0DimmTypesMatch )
             || ( (MrcData->TotalDimm == 4) && (!Slot0DimmTypesMatch || !Slot1DimmTypesMatch) )
           ) {
          MrcData->InterleaveMode = NoInterleave;
          MspDebugPrint ((MSP_DBG_MIN, "[NVDIMM] No Interleave because of mixed types\n"));
        }
      }

      if (MrcData->NonVolMemInterleaving) {
        MrcData->InterleaveMode = NoInterleave;
        MspDebugPrint ((MSP_DBG_MIN, "[NVDIMM] No Interleave\n"));
      }
    }
    if (MrcData->InterleaveMode == NoInterleave) {
      //
      // Interleaving is Disabled and Memory is populated in both channels: Two Asym Config.
      //
      MrcData->MspData.DynamicVars[0][SLICE0_ASYM_ENABLE]  = 1;
      MrcData->MspData.DynamicVars[0][SLICE0_ASYM_BASE]    = 0;
      MrcData->MspData.DynamicVars[0][SLICE0_ASYM_LIMIT]   = (MrcData->MrcParameters.TotalMem[0] >> 11) + (MrcData->OemMrcData.MmioSize >> 11) - 1;


      MrcData->MspData.DynamicVars[0][SLICE1_ASYM_ENABLE]  = 1;
      MrcData->MspData.DynamicVars[0][SLICE1_ASYM_BASE]    = (MrcData->MrcParameters.TotalMem[0] >> 11) + (MrcData->OemMrcData.MmioSize >> 11);
      MrcData->MspData.DynamicVars[0][SLICE1_ASYM_LIMIT]   = ((MrcData->MrcParameters.TotalMem[1] + MrcData->MrcParameters.TotalMem[0]) >> 11) + (MrcData->OemMrcData.MmioSize >> 11) - 1;

      if (MrcData->MrcParameters.TotalMem[0] < 0x1000) {
        MrcData->MspData.DynamicVars[0][SLICE0_ASYM_LIMIT]   = (MrcData->MrcParameters.TotalMem[0] >> 11) - 1;
        MrcData->MspData.DynamicVars[0][SLICE1_ASYM_BASE]    = (MrcData->MrcParameters.TotalMem[0] >> 11);
        MrcData->MspData.DynamicVars[0][SLICE1_ASYM_LIMIT]   = ((MrcData->MrcParameters.TotalMem[1] + MrcData->MrcParameters.TotalMem[0]) >> 11) + (MrcData->OemMrcData.MmioSize >> 11) - 1;
      }

      if (NVDIMM_MODE == MrcData->NonVolMemMode) {
        UINT8 NvDimmPesentCh0 = (0 != (MrcData->NvDimmType[0][0] || MrcData->NvDimmType[0][1]));
        UINT8 NvDimmPesentCh1 = (0 != (MrcData->NvDimmType[1][0] || MrcData->NvDimmType[1][1]));

        if (NvDimmPesentCh0 || NvDimmPesentCh1) {
          UINTX Slice0Base = 0;
          UINTX Slice0Limit;
          UINTX Slice1Base;
          UINTX Slice1Limit;
          UINTX MmioAddrSizeAddr = (MrcData->OemMrcData.MmioSize >> 11);

          //
          // NVDIMM must be placed in the highest end of the memory space
          // If there is only an NVDIMM in channel 0 then channel 1 should map to the lowest addresses
          //
          if (NvDimmPesentCh1) {
            if (MrcData->MrcParameters.TotalMem[0] < 0x1000) {
              MmioAddrSizeAddr = 0;
            }
            Slice0Base = 0;
            Slice0Limit = (MrcData->MrcParameters.TotalMem[0] >> 11) + MmioAddrSizeAddr - 1;
            Slice1Base = Slice0Limit + 1;
            Slice1Limit = Slice1Base + (MrcData->MrcParameters.TotalMem[1] >> 11) - 1;     
            if (MrcData->MrcParameters.TotalMem[0] < 0x1000) {
              Slice1Limit  = Slice1Limit + (MrcData->OemMrcData.MmioSize >> 11);
            }
          } else {
            if (MrcData->MrcParameters.TotalMem[1] < 0x1000) {
              MmioAddrSizeAddr = 0;
            }
            Slice1Base = 0;
            Slice1Limit = (MrcData->MrcParameters.TotalMem[1] >> 11) + MmioAddrSizeAddr - 1;
            Slice0Base = Slice1Limit + 1;
            Slice0Limit = Slice0Base + (MrcData->MrcParameters.TotalMem[0] >> 11) - 1;
            if (MrcData->MrcParameters.TotalMem[1] < 0x1000) {
              Slice0Limit = Slice0Limit + (MrcData->OemMrcData.MmioSize >> 11);
            }
          }
          MrcData->MspData.DynamicVars[0][SLICE0_ASYM_BASE]    = Slice0Base;
          MrcData->MspData.DynamicVars[0][SLICE0_ASYM_LIMIT]   = Slice0Limit;
          MrcData->MspData.DynamicVars[0][SLICE1_ASYM_BASE]    = Slice1Base;
          MrcData->MspData.DynamicVars[0][SLICE1_ASYM_LIMIT]   = Slice1Limit;
        }
      }

      MspDebugPrint ((MSP_DBG_MIN, "No Interleave\n"));
      MspDebugPrint ((MSP_DBG_MIN, "Mmio 0x%x\n", (MrcData->OemMrcData.MmioSize >> 11)));
      MspDebugPrint ((MSP_DBG_MIN, "SLICE0_ASYM_BASE 0x%x\n", MrcData->MspData.DynamicVars[0][SLICE0_ASYM_BASE]));
      MspDebugPrint ((MSP_DBG_MIN, "SLICE0_ASYM_LIMIT 0x%x\n", MrcData->MspData.DynamicVars[0][SLICE0_ASYM_LIMIT]));
      MspDebugPrint ((MSP_DBG_MIN, "SLICE1_ASYM_BASE 0x%x\n", MrcData->MspData.DynamicVars[0][SLICE1_ASYM_BASE]));
      MspDebugPrint ((MSP_DBG_MIN, "SLICE1_ASYM_LIMIT 0x%x\n", MrcData->MspData.DynamicVars[0][SLICE1_ASYM_LIMIT]));
    } else {
      //
      // Interleaving Enabled
      //
      MrcData->MspData.DynamicVars[0][INTERLEAVE_MODE] = MrcData->InterleaveMode - 1;
      MrcData->MspData.DynamicVars[0][SLICE_HASH_MASK] = (BIT4 << (MrcData->InterleaveMode - 1)) | 0x2004;
      MspDebugPrint ((MSP_DBG_MIN, "Interleave Mode %d\n", MrcData->InterleaveMode - 1));

      if (MrcData->MrcParameters.TotalMem[0] != MrcData->MrcParameters.TotalMem[1]) {
        //
        // One Asymmetric Memory Config.
        //        
        if (MrcData->MrcParameters.TotalMem[0] > MrcData->MrcParameters.TotalMem[1]) {
          AsymBase = MrcData->MrcParameters.TotalMem[1] * 2;
          if (AsymBase < 0x1000) {
           AsymLimit = AsymBase + MrcData->OemMrcData.MmioSize + (MrcData->MrcParameters.TotalMem[0] - MrcData->MrcParameters.TotalMem[1]);
          } else {
           //
           // If Above 4G account for the MMIO
           //
           AsymBase += MrcData->OemMrcData.MmioSize;
           AsymLimit = AsymBase + (MrcData->MrcParameters.TotalMem[0] - MrcData->MrcParameters.TotalMem[1]);
          }
          MspDebugPrint ((MSP_DBG_MIN, "Asymmetric Ch0 Interleave\n"));
          MrcData->MspData.DynamicVars[0][SLICE0_ASYM_ENABLE] = 1;
          MrcData->MspData.DynamicVars[0][SLICE0_ASYM_BASE]   = AsymBase >> 11;
          MrcData->MspData.DynamicVars[0][SLICE0_ASYM_LIMIT]  = (AsymLimit >> 11) - 1;
    	  } else {
    	    AsymBase = MrcData->MrcParameters.TotalMem[0] * 2;
          if (AsymBase < 0x1000) {
           AsymLimit = AsymBase + MrcData->OemMrcData.MmioSize + (MrcData->MrcParameters.TotalMem[1] - MrcData->MrcParameters.TotalMem[0]);
          } else {
           AsymBase += MrcData->OemMrcData.MmioSize;
           AsymLimit = AsymBase + (MrcData->MrcParameters.TotalMem[1] - MrcData->MrcParameters.TotalMem[0]);
          }
          MspDebugPrint ((MSP_DBG_MIN, "Asymmetric Ch1 Interleave\n"));
          MrcData->MspData.DynamicVars[0][SLICE1_ASYM_ENABLE] = 1;
          MrcData->MspData.DynamicVars[0][SLICE1_ASYM_BASE]   = AsymBase >> 11;
          MrcData->MspData.DynamicVars[0][SLICE1_ASYM_LIMIT]  = (AsymLimit >> 11) - 1;
        }
        MspDebugPrint ((MSP_DBG_MIN, "SLICE0_ASYM_BASE 0x%x\n", MrcData->MspData.DynamicVars[0][SLICE0_ASYM_BASE]));
        MspDebugPrint ((MSP_DBG_MIN, "SLICE0_ASYM_LIMIT 0x%x\n", MrcData->MspData.DynamicVars[0][SLICE0_ASYM_LIMIT]));
        MspDebugPrint ((MSP_DBG_MIN, "SLICE1_ASYM_BASE 0x%x\n", MrcData->MspData.DynamicVars[0][SLICE1_ASYM_BASE]));
        MspDebugPrint ((MSP_DBG_MIN, "SLICE1_ASYM_LIMIT 0x%x\n", MrcData->MspData.DynamicVars[0][SLICE1_ASYM_LIMIT]));
      } else {
        //
        // Same amount of Memory across Channels. All Interleaved.
        //
        MspDebugPrint ((MSP_DBG_MIN, "All Interleave\n"));
      }
    }
  }
#endif
}

/**
  This routine programs DRAM Rapl as per following equation:
  TW = Time Unit * (1 + (x/4))*2^y
  Value provided is divided by the closest power of 2 that does not exceed the value.
  Remainder should be a number between 1 and 2. Variable x is programmed to the closest value: 
  (1 -> 0, 1.25 -> 1, 1.5->2, 1.75-> 3)

  @param[in, out]  MrcData       Host structure for all data related to MMRC.
**/
#if DRAM_RAPL == 1
VOID
ProgDramRapl (
  IN  OUT   MMRC_DATA   *MrcData
  )
{
  UINT8  Index;
  UINT64 TimeWindowY;
  
  if (MrcData->DramRaplParameters.PowerLimitEnable == 1) {
    TimeWindowY = MrcData->DramRaplParameters.TimeWindow;
    MspDebugPrint ((MSP_DBG_MAX, "Dram Rapl - Input Time Window = 0x%08x", (UINT32) (MrcData->DramRaplParameters.TimeWindow >> 32)));
    MspDebugPrint ((MSP_DBG_MAX, "%08x\n", (UINT32) (MrcData->DramRaplParameters.TimeWindow)));
    //
    // Variable Y is found by dividing or shifting until remainder is <= 200
    //
    for (Index = 0; TimeWindowY > 200; Index++) {
      TimeWindowY = (UINT64) ((MrcData->DramRaplParameters.TimeWindow * 100) >> Index);
      MrcData->MspData.DynamicVars[0][TWY] = Index;
      MspDebugPrint ((MSP_DBG_MAX, "Index %d, TW = 0x%08x%08x\n", Index, (UINT32)(TimeWindowY >> 32), (UINT32)(TimeWindowY)));
    }

    //
    // Variable X is programmed to the closest value.
    // (1 -> 0, 1.25 -> 1, 1.5->2, 1.75-> 3)
    //
    if (TimeWindowY >= 100 && TimeWindowY <= 112) {
      MrcData->MspData.DynamicVars[0][TWX] = 0;
    } else if (TimeWindowY >= 113 && TimeWindowY <= 137) {
      MrcData->MspData.DynamicVars[0][TWX] = 1;
    } else if (TimeWindowY >= 138 && TimeWindowY <= 162) {
      MrcData->MspData.DynamicVars[0][TWX] = 2;
    } else if (TimeWindowY >= 163 && TimeWindowY <= 200) {
      MrcData->MspData.DynamicVars[0][TWX] = 3;
    } else {
      MspDebugPrint ((MSP_DBG_MIN, "Dram Rapl - Invalid Reminder for TWX = %d. Range Value should be between 100 and 200\n", TimeWindowY));
    }
    MspDebugPrint ((MSP_DBG_MAX, "Dram Rapl - TWY = %d, TWX = %d\n", (UINT32) (MrcData->MspData.DynamicVars[0][TWY]), (UINT32) (MrcData->MspData.DynamicVars[0][TWX])));

    MrcData->MspData.DynamicVars[0][POWER_LIMIT_EN] = MrcData->DramRaplParameters.PowerLimitEnable;
    MrcData->MspData.DynamicVars[0][POWER_LIMIT] = MrcData->DramRaplParameters.PowerLimit / POWER_LIMIT_RESOLUTION;
  } else {
    MrcData->MspData.DynamicVars[0][POWER_LIMIT] = 0;
  }
  MrcData->MspData.DynamicVars[0][POWER_LIMIT_LOCK] = MrcData->DramRaplParameters.PowerLimitLock;
  MspDebugPrint ((MSP_DBG_MAX, "Dram Rapl - Power Limit 1 - Enable: %d Value: %d\n", (UINT32) MrcData->MspData.DynamicVars[0][POWER_LIMIT_EN] , (UINT32) MrcData->MspData.DynamicVars[0][POWER_LIMIT]));
}
#endif

/**
  This routine programs Leaky Bucket
  For each rank Leak Rate is calculated as follows:
    Leak Ticks = Leak Rate / (8192 * dram_clk_tick)

  @param[in, out]  MrcData       Host structure for all data related to MMRC.
  @param[in]       Channel       Current Channel being examinated
**/
#if LEAKY_BUCKET == 1
VOID
ProgLeakyBucket (
  IN  OUT   MMRC_DATA   *MrcData,
  IN        UINT8       Channel
  )
{
  UINT32 Divisor = 0;
  MspDebugPrint ((MSP_DBG_MED, "LeakRate - 0x%08x", MrcData->LeakyBucketParameters.Rate[Channel][0] >> 32)); 
  MspDebugPrint ((MSP_DBG_MED, "%08X\n", MrcData->LeakyBucketParameters.Rate[Channel][0]));
  Divisor = LeakyBucketDivisor[MrcData->MrcParameters.DdrFrequency - MINDDR];
    if (Divisor == 0) {
      Divisor = 1;
    }
  //
  // Convert Leak Rate to ns*1000, picoseconds.
  //
  if (MrcData->LeakyBucketParameters.Rate[Channel][0] == 0) {
    MrcData->MspData.DynamicVars[Channel][LEAK_RATE_R0] = 0xFFFFFFFFFF;
  } else {
    MrcData->MspData.DynamicVars[Channel][LEAK_RATE_R0] = ((MrcData->LeakyBucketParameters.Rate[Channel][0] * MrcData->LeakyBucketParameters.RateUnits) / Divisor);
  }
  if (MrcData->LeakyBucketParameters.Rate[Channel][1] == 0) {
    MrcData->MspData.DynamicVars[Channel][LEAK_RATE_R1] = 0xFFFFFFFFFF;
  } else {
    MrcData->MspData.DynamicVars[Channel][LEAK_RATE_R1] = ((MrcData->LeakyBucketParameters.Rate[Channel][1] * MrcData->LeakyBucketParameters.RateUnits) / Divisor);
  }
  if (MrcData->LeakyBucketParameters.Rate[Channel][2] == 0) {
    MrcData->MspData.DynamicVars[Channel][LEAK_RATE_R2] = 0xFFFFFFFFFF;
  } else {
    MrcData->MspData.DynamicVars[Channel][LEAK_RATE_R2] = ((MrcData->LeakyBucketParameters.Rate[Channel][2] * MrcData->LeakyBucketParameters.RateUnits) / Divisor);
  }
  if (MrcData->LeakyBucketParameters.Rate[Channel][3] == 0) {
    MrcData->MspData.DynamicVars[Channel][LEAK_RATE_R3] = 0xFFFFFFFFFF;
  } else {
    MrcData->MspData.DynamicVars[Channel][LEAK_RATE_R3] = ((MrcData->LeakyBucketParameters.Rate[Channel][3] * MrcData->LeakyBucketParameters.RateUnits) / Divisor);
  }
  MspDebugPrint ((MSP_DBG_DEBUG, "LEAK_RATE_R0 - 0x%08x", (UINT32) (MrcData->MspData.DynamicVars[Channel][LEAK_RATE_R0] >> 32)));
  MspDebugPrint ((MSP_DBG_DEBUG, "%08X\n", (UINT32) (MrcData->MspData.DynamicVars[Channel][LEAK_RATE_R1])));
  MspDebugPrint ((MSP_DBG_DEBUG, "LEAK_RATE_R1 - 0x%08x", (UINT32) (MrcData->MspData.DynamicVars[Channel][LEAK_RATE_R1] >>32)));
  MspDebugPrint ((MSP_DBG_DEBUG, "%08X\n", (UINT32) (MrcData->MspData.DynamicVars[Channel][LEAK_RATE_R1])));
  MspDebugPrint ((MSP_DBG_DEBUG, "LEAK_RATE_R2 - 0x%08x", (UINT32) (MrcData->MspData.DynamicVars[Channel][LEAK_RATE_R2]>> 32)));
  MspDebugPrint ((MSP_DBG_DEBUG, "%08X\n", (UINT32) (MrcData->MspData.DynamicVars[Channel][LEAK_RATE_R2])));
  MspDebugPrint ((MSP_DBG_DEBUG, "LEAK_RATE_R3 - 0x%08x", (UINT32) (MrcData->MspData.DynamicVars[Channel][LEAK_RATE_R3]>>32)));
  MspDebugPrint ((MSP_DBG_DEBUG, "%08X\n", (UINT32) (MrcData->MspData.DynamicVars[Channel][LEAK_RATE_R3])));

  if (MrcData->LeakyBucketParameters.CorrectableErrorThreshold[Channel][0] == 0) {
    MrcData->MspData.DynamicVars[Channel][CORR_ERR_THR_R0] = 0x7FFF;
  } else {
    MrcData->MspData.DynamicVars[Channel][CORR_ERR_THR_R0] = MrcData->LeakyBucketParameters.CorrectableErrorThreshold[Channel][0];
  }
  if (MrcData->LeakyBucketParameters.CorrectableErrorThreshold[Channel][1] == 0) {
    MrcData->MspData.DynamicVars[Channel][CORR_ERR_THR_R1] = 0x7FFF;
  } else {
    MrcData->MspData.DynamicVars[Channel][CORR_ERR_THR_R1] = MrcData->LeakyBucketParameters.CorrectableErrorThreshold[Channel][1];
  }
  if (MrcData->LeakyBucketParameters.CorrectableErrorThreshold[Channel][2] == 0) {
    MrcData->MspData.DynamicVars[Channel][CORR_ERR_THR_R2] = 0x7FFF;
  } else {
    MrcData->MspData.DynamicVars[Channel][CORR_ERR_THR_R2] = MrcData->LeakyBucketParameters.CorrectableErrorThreshold[Channel][2];
  }
  if (MrcData->LeakyBucketParameters.CorrectableErrorThreshold[Channel][3] == 0) {
    MrcData->MspData.DynamicVars[Channel][CORR_ERR_THR_R3] = 0x7FFF;
  } else {
    MrcData->MspData.DynamicVars[Channel][CORR_ERR_THR_R3] = MrcData->LeakyBucketParameters.CorrectableErrorThreshold[Channel][3];
  }

}
#endif
/**
  This routine programs Patrol and Demand Scrub

  @param[in, out]  MrcData       Host structure for all data related to MMRC.
  @param[in]       Channel       Current Channel being examinated
**/
VOID
ProgPatrolDemandScrub (
  IN  OUT   MMRC_DATA   *MrcData,
  IN        UINT8       Channel
  )
{
  UINT8  ColAddr;
  UINT8  RowAddr;
  UINT8  Dimm;
  UINT64 ScrubOps;
  UINT64 ScrubTimer;
  UINT64 ScrubResult;
  //
  // If either patrol scrub or demand scrub are enabled, set SERREN in DUNIT.
  //
  MrcData->MspData.DynamicVars[Channel][SERR_EN] = MrcData->DemandScrub | MrcData->PatrolScrubEnable;
  if (MrcData->PatrolScrubEnable) {
    MrcData->MspData.DynamicVars[Channel][PATROL_SCRUB] = 1;
    MspDebugPrint ((MSP_DBG_MAX, "Patrol Scrub Config\n"));
    //
    // Patrol Scrub Operations = Memory Size (GB) / 64 (B/trans) 
    // Patrol Scrub Timer = #hours (ns) / Patrol Scrub Operations
    // DCLK tick = Dram Speed / 4
    // Patrol Scrub Timer in Dlcks = Patrol Scrub Timer / DCLK Tick
    // Formula definition from SCRUBPER register in SSA registers.
    //
    MspDebugPrint ((MSP_DBG_MAX, "PatrolScrubPeriod: %d\n", (ScrubIndex2SecondsTable[MrcData->PatrolScrubPeriod])));

    ScrubOps = MrcData->MrcParameters.TotalMem[Channel] * 1024 / 64 * 1024;
    MspDebugPrint ((MSP_DBG_MAX, "Scrub Ops: 0x%08x", (UINT32) (ScrubOps >> 32)));
    MspDebugPrint ((MSP_DBG_MAX, "%08x.\n", (UINT32) (ScrubOps)));

    ScrubTimer = ((UINT64) ScrubIndex2SecondsTable[MrcData->PatrolScrubPeriod]) * (UINT64)1000000000;
    MspDebugPrint ((MSP_DBG_MAX, "Scrub Timer: 0x%08x", (UINT32) (ScrubTimer >> 32)));
    MspDebugPrint ((MSP_DBG_MAX, "%08x.\n", (UINT32) (ScrubTimer)));

    ScrubResult = (UINT64)ScrubTimer /(UINT64)ScrubOps;
    ScrubResult = (UINT64)(ScrubResult * 1000) / (UINT64)DClksUsec[MrcData->MrcParameters.DdrFrequency - MINDDR];

    MspDebugPrint ((MSP_DBG_MAX, "Scrub Result: 0x%08x", (UINT32) (ScrubResult >>32)));
    MspDebugPrint ((MSP_DBG_MAX, "%08x.\n", (UINT32) (ScrubResult)));

    MrcData->MspData.DynamicVars[Channel][SCRUB_TIMER] = ScrubResult;
    //
    // Set Maximum Row Address and total number of columns.
    //
    for (Dimm = 0; Dimm < MAX_DIMMS; Dimm++) {
      if (MrcData->MrcParameters.DimmPresent[Channel][Dimm]) {
        ColAddr = MrcData->MrcParameters.SpdData[Channel][Dimm].Buffer[SPD_DDR3_ADDRESS] & 0x7;
        RowAddr = MrcData->MrcParameters.SpdData[Channel][Dimm].Buffer[SPD_DDR3_ADDRESS] >> 3 & 0x7;
        //
        // DDR3 Dimms that use 8Gb parts have 11 column address bits. For every other size of memory there are 10 column address bits. 
        //
        if (ColAddr == 2 && MrcData->MrcParameters.DramType[Channel] <= TypeDdr3All) {
          if (Dimm == 0) {
            MrcData->MspData.DynamicVars[Channel][DIMM0_COL_ADDR_BITS] = 1;
          } else {
            MrcData->MspData.DynamicVars[Channel][DIMM1_COL_ADDR_BITS] = 1;
          }
        }
        //
        // Total Row Address Bits - Max Row Address
        // 15 - 0x7
        // 16 - 0xF
        // ..
        //
        if (Dimm == 0) {
          MrcData->MspData.DynamicVars[Channel][DIMM0_MAX_ROW_ADDR] = (1 << RowAddr) - 1;
        } else {
          MrcData->MspData.DynamicVars[Channel][DIMM1_MAX_ROW_ADDR] = (1 << RowAddr) - 1;
        }
      }
    }
    if (MrcData->MaxScrubDebitCount != 0) {
      MrcData->MspData.DynamicVars[Channel][MAX_PATSCRUB_DEBIT_CNT] = MrcData->MaxScrubDebitCount;
    } else {
      MrcData->MspData.DynamicVars[Channel][MAX_PATSCRUB_DEBIT_CNT] =0xFF;
    }
  } else {
    MrcData->MspData.DynamicVars[Channel][PATROL_SCRUB] = 0;
    MspDebugPrint ((MSP_DBG_MIN, "Patrol scrub disabled.\n"));
  }

  if (MrcData->DemandScrub == 1) {
    MrcData->MspData.DynamicVars[Channel][DEMAND_SCRUB] = 1;
    MspDebugPrint ((MSP_DBG_MIN, "Demand scrub enabled.\n"));
  } else {
    MrcData->MspData.DynamicVars[Channel][DEMAND_SCRUB] = 0;
    MspDebugPrint ((MSP_DBG_MIN, "Demand scrub disabled.\n"));
  }
}

/**
  This routine initializes Dynamic Variables for Phy Init


  @param[in, out]  MrcData       Host structure for all data related to MMRC.
**/
#if DDRIO_PHY_INIT == 1
VOID
SetPhyInitDynamicVariables (
  IN  OUT   MMRC_DATA   *MrcData
  )
{
  UINT8  Channel;
  UINT8  x4bit_memory[MAX_CHANNELS];
  UINT8  Rank;
  DramInitDDR4EMR5  Mrs5Command;
  Rank = 0;
  x4bit_memory[0] = 1;
  x4bit_memory[1] = 1;
  Mrs5Command.Data = 0;
  
  for (Channel = 0; Channel < MAX_CHANNELS; Channel++) {  
    if (!MrcData->MrcParameters.ChEnabled[Channel]) {
      continue;
    }
    x4bit_memory[Channel] = MrcData->MspData.DynamicVars[Channel][DIMMDWID] & BIT0;
  }

  for (Channel = 0; Channel < MAX_CHANNELS; Channel++) {       
    if ( MrcData->MspData.DynamicVars[0][DIMMTYPE] ==  SPD_DDR_RDIMM) {
      MrcData->MspData.DynamicVars[Channel][RDIMM_EN_0] = 1;
    } else {
      MrcData->MspData.DynamicVars[Channel][RDIMM_EN_0] = 0;
    }

    if ( MrcData->MspData.DynamicVars[1][DIMMTYPE] ==  SPD_DDR_RDIMM) {
      MrcData->MspData.DynamicVars[Channel][RDIMM_EN_1] = 1;
    } else {
      MrcData->MspData.DynamicVars[Channel][RDIMM_EN_1] = 0;
    }

    MrcData->MspData.DynamicVars[Channel][RL0] = MrcData->MrcParameters.TimingData[0][TimingTcl][FreqA] + MrcData->MspData.DynamicVars[0][ADDLAT] + MrcData->MspData.DynamicVars[0][RDIMM_EN_0];
    MrcData->MspData.DynamicVars[Channel][RL1] = MrcData->MrcParameters.TimingData[1][TimingTcl][FreqA] + MrcData->MspData.DynamicVars[1][ADDLAT] + MrcData->MspData.DynamicVars[0][RDIMM_EN_1];
    MrcData->MspData.DynamicVars[Channel][WL0] = MrcData->MrcParameters.TimingData[0][TimingTwcl][FreqA] + MrcData->MspData.DynamicVars[0][ADDLAT] + MrcData->MspData.DynamicVars[0][RDIMM_EN_0];
    MrcData->MspData.DynamicVars[Channel][WL1] = MrcData->MrcParameters.TimingData[1][TimingTwcl][FreqA] + MrcData->MspData.DynamicVars[1][ADDLAT] + MrcData->MspData.DynamicVars[0][RDIMM_EN_1];

    MrcData->MspData.DynamicVars[Channel][wr_pream0] = MrcData->TwoClkWrPreamble[0] + 1;
    MrcData->MspData.DynamicVars[Channel][wr_pream1] = MrcData->TwoClkWrPreamble[1] + 1;

#ifdef SIM
    MrcData->MspData.DynamicVars[Channel][EARLY_DQ_EN_0] = MrcData->WriteDataEarlyEnable[0];
    MrcData->MspData.DynamicVars[Channel][EARLY_DQ_EN_1] = MrcData->WriteDataEarlyEnable[1]; 
#else
    MrcData->MspData.DynamicVars[Channel][EARLY_DQ_EN_0] = 1;
    MrcData->MspData.DynamicVars[Channel][EARLY_DQ_EN_1] = 1;
#endif

    MrcData->MspData.DynamicVars[Channel][X8X8] = 0;
    MrcData->MspData.DynamicVars[Channel][X4X4] = 0;
    MrcData->MspData.DynamicVars[Channel][X8X4] = 0;
    MrcData->MspData.DynamicVars[Channel][X4X8] = 0;
 
    switch ( (x4bit_memory[1] << 1) | x4bit_memory[0] ){
      case 0:
        MrcData->MspData.DynamicVars[Channel][X8X8] = 1;
        break;
      case 1:
        MrcData->MspData.DynamicVars[Channel][X8X4] = 1;
        break;
      case 2:
        MrcData->MspData.DynamicVars[Channel][X4X8] = 1;
        break;
      default:
        MrcData->MspData.DynamicVars[Channel][X4X4] = 1;
        break;
    }

    MrcData->MspData.DynamicVars[Channel][IDLE_PULLUP_ENABLE_0] = 0;
    if (MrcData->MrcParameters.ChEnabled[0]) {
      for (Rank = 0; Rank < MAX_RANKS; Rank += 2) {
        if (MrcData->MrcParameters.RankPresent[0][Rank] == 1) {
          Mrs5Command.Data = ReadMrsCommand(MrcData, 0, Rank, 5);
          break;
        }
      }
      if ((MrcData->MrcParameters.DramType[0] == TypeDdr4) && (Mrs5Command.Bits.RTT_PARK != 0)) {
        MrcData->MspData.DynamicVars[Channel][IDLE_PULLUP_ENABLE_0] = 1;
      }
    }
 
    MrcData->MspData.DynamicVars[Channel][IDLE_PULLUP_ENABLE_1] = 0;
    if (MrcData->MrcParameters.ChEnabled[1]) {
      for (Rank = 0; Rank < MAX_RANKS; Rank += 2) {
        if (MrcData->MrcParameters.RankPresent[1][Rank] == 1) {
          Mrs5Command.Data = ReadMrsCommand(MrcData, 1, Rank, 5);
          break;
        }
      }
      if ((MrcData->MrcParameters.DramType[1] == TypeDdr4) && (Mrs5Command.Bits.RTT_PARK != 0)) {
        MrcData->MspData.DynamicVars[Channel][IDLE_PULLUP_ENABLE_1] = 1;
      }
    }
    //
    // On DNV B0 the DDRIO preserves it's configuration when Dunit register SBONUS_MISC_B0.DDRIO_RST_BLOCK = 1
    //
    MrcData->MspData.DynamicVars[Channel][PHY_INIT_RUN] = 1;
    if ((MrcData->MrcParameters.BootMode == S0) && (MrcData->SocStepping == DNV_B0_STEP)) {
      MrcData->MspData.DynamicVars[Channel][PHY_INIT_RUN] = 0;
    }
    MspDebugPrint((MSP_DBG_MAX, "DDR PHY Settings\n"));
    MspDebugPrint((MSP_DBG_MAX, "RL0 %d\n", MrcData->MspData.DynamicVars[Channel][RL0]));
    MspDebugPrint((MSP_DBG_MAX, "RL1 %d\n", MrcData->MspData.DynamicVars[Channel][RL1]));
    MspDebugPrint((MSP_DBG_MAX, "WL0 %d\n", MrcData->MspData.DynamicVars[Channel][WL0]));
    MspDebugPrint((MSP_DBG_MAX, "WL1 %d\n", MrcData->MspData.DynamicVars[Channel][WL1]));
    MspDebugPrint((MSP_DBG_MAX, "wr_pream0 %d\n", MrcData->MspData.DynamicVars[Channel][wr_pream0]));
    MspDebugPrint((MSP_DBG_MAX, "wr_pream1 %d\n", MrcData->MspData.DynamicVars[Channel][wr_pream1]));

    MspDebugPrint((MSP_DBG_MAX, "EARLY_DQ_EN_0 %d\n", MrcData->MspData.DynamicVars[Channel][EARLY_DQ_EN_0]));
    MspDebugPrint((MSP_DBG_MAX, "EARLY_DQ_EN_1 %d\n", MrcData->MspData.DynamicVars[Channel][EARLY_DQ_EN_1]));
    MspDebugPrint((MSP_DBG_MAX, "ADDLAT 0 %d\n", MrcData->MspData.DynamicVars[0][ADDLAT]));
    MspDebugPrint((MSP_DBG_MAX, "ADDLAT 1 %d\n", MrcData->MspData.DynamicVars[1][ADDLAT]));

    MspDebugPrint((MSP_DBG_MAX, "RDIMM_EN_0 %d\n", MrcData->MspData.DynamicVars[Channel][RDIMM_EN_0]));
    MspDebugPrint((MSP_DBG_MAX, "RDIMM_EN_1 %d\n", MrcData->MspData.DynamicVars[Channel][RDIMM_EN_1]));

    MspDebugPrint((MSP_DBG_MAX, "X4X4 %d\n", MrcData->MspData.DynamicVars[Channel][X4X4]));
    MspDebugPrint((MSP_DBG_MAX, "X8X4 %d\n", MrcData->MspData.DynamicVars[Channel][X8X4]));
    MspDebugPrint((MSP_DBG_MAX, "X4X8 %d\n", MrcData->MspData.DynamicVars[Channel][X4X8]));
    MspDebugPrint((MSP_DBG_MAX, "X8X8 %d\n", MrcData->MspData.DynamicVars[Channel][X8X8]));

    MspDebugPrint((MSP_DBG_MAX, "IDLE_PULLUP_ENABLE_0 %d\n", MrcData->MspData.DynamicVars[Channel][IDLE_PULLUP_ENABLE_0]));
    MspDebugPrint((MSP_DBG_MAX, "IDLE_PULLUP_ENABLE_1 %d\n", MrcData->MspData.DynamicVars[Channel][IDLE_PULLUP_ENABLE_1]));
    MspDebugPrint((MSP_DBG_MAX, "PHY_INIT_RUN %d\n", MrcData->MspData.DynamicVars[Channel][PHY_INIT_RUN]));
  }
}
#endif

/**
  This routine initializes Dynamic Variables for Memory Thermal 
  Throttling 

  @param[in, out]  MrcData       Host structure for all data related to MMRC.
**/
VOID
SetMemThermThrotVariables(
  IN  OUT   MMRC_DATA   *MrcData
  )
{
  UINT8 Channel;

  for (Channel = 0; Channel < MAX_CHANNELS; Channel++) {
    if (MrcData->SelectRefreshRate == 1) {
      MspDebugPrint((MSP_DBG_MAX, "Refresh rate set to 1x. Forcing Memory Thermal to Enabled \n"));
      MrcData->MspData.DynamicVars[Channel][MEM_THERM_EN]   = 1;
      MrcData->MemoryThermalEnable = 1;
    } else if (MrcData->MemoryThermalEnable == 2) {
      if (MrcData->D_Type[Channel][0] == SPD_DDR_RDIMM || MrcData->D_Type[Channel][0] == SPD_DDR3_UNBUFFERED || MrcData->D_Type[Channel][1] == SPD_DDR_RDIMM || MrcData->D_Type[Channel][1] == SPD_DDR3_UNBUFFERED) {
        MrcData->MspData.DynamicVars[Channel][MEM_THERM_EN]   = 1;
        MspDebugPrint((MSP_DBG_MAX, "Enabling CLTT for RDIMM/UDIMM \n"));
      } else {
        MrcData->MspData.DynamicVars[Channel][MEM_THERM_EN]   = 0;
      }
	} else {
      MrcData->MspData.DynamicVars[Channel][MEM_THERM_EN]   = MrcData->MemoryThermalEnable;
    }
    if (MrcData->SelectRefreshRate == 1) {
    	MrcData->MspData.DynamicVars[Channel][MEM_THERM_TYPE] = 1;
    	MspDebugPrint((MSP_DBG_MAX, "Refresh rate set to 1x. Forcing Thermal Throttling to OLTT \n"));
    	MrcData->ThermalThrottlingType = 1;
    } else {
      MrcData->MspData.DynamicVars[Channel][MEM_THERM_TYPE] = MrcData->ThermalThrottlingType;
    }
    MrcData->MspData.DynamicVars[Channel][MEM_THERM_TYPE] = MrcData->ThermalThrottlingType;
    MrcData->MspData.DynamicVars[Channel][CLTT_MODE]      = MrcData->ClttMode;
    MrcData->MspData.DynamicVars[Channel][TEMP_HI]        = MrcData->TempHi;
    MrcData->MspData.DynamicVars[Channel][TEMP_MID]       = MrcData->TempMid;
    MrcData->MspData.DynamicVars[Channel][TEMP_LO]        = MrcData->TempLo;
    MrcData->MspData.DynamicVars[Channel][THRT_CRIT]      = (100 - MrcData->BwLevelOnThrtCrit);
    MrcData->MspData.DynamicVars[Channel][THRT_HI]        = (100 - MrcData->BwLevelOnThrtHi);
    MrcData->MspData.DynamicVars[Channel][THRT_MID]       = (100 - MrcData->BwLevelOnThrtMid);
    MrcData->MspData.DynamicVars[Channel][OLTT_THRT_BW]   = MrcData->OlttThrtBw;

    MspDebugPrint((MSP_DBG_MAX, "MEM_THERM_EN[%d] = %d\n", Channel, MrcData->MspData.DynamicVars[Channel][MEM_THERM_EN]));
    MspDebugPrint((MSP_DBG_MAX, "MEM_THERM_TYPE[%d] = %d\n", Channel, MrcData->MspData.DynamicVars[Channel][MEM_THERM_TYPE]));
    MspDebugPrint((MSP_DBG_MAX, "CLTT_MODE[%d] = %d\n", Channel, MrcData->MspData.DynamicVars[Channel][CLTT_MODE]));
    MspDebugPrint((MSP_DBG_MAX, "TEMP_HI[%d] = %d\n", Channel, MrcData->MspData.DynamicVars[Channel][TEMP_HI]));
    MspDebugPrint((MSP_DBG_MAX, "TEMP_MID[%d] = %d\n", Channel, MrcData->MspData.DynamicVars[Channel][TEMP_MID]));
    MspDebugPrint((MSP_DBG_MAX, "TEMP_LO[%d] = %d\n", Channel, MrcData->MspData.DynamicVars[Channel][TEMP_LO]));
    MspDebugPrint((MSP_DBG_MAX, "THRT_CRIT[%d] = %d\n", Channel, MrcData->MspData.DynamicVars[Channel][THRT_CRIT]));
    MspDebugPrint((MSP_DBG_MAX, "THRT_HI[%d] = %d\n", Channel, MrcData->MspData.DynamicVars[Channel][THRT_HI]));
    MspDebugPrint((MSP_DBG_MAX, "THRT_MID[%d] = %d\n", Channel, MrcData->MspData.DynamicVars[Channel][THRT_MID]));
    MspDebugPrint((MSP_DBG_MAX, "OLTT_THRT_BW[%d] = %d\n", Channel, MrcData->MspData.DynamicVars[Channel][OLTT_THRT_BW]));
  }

  if (MrcData->MemoryThermalEnable == 1) {
    MspDebugPrint((MSP_DBG_MAX, "Memory Thermal Throttling Enabled... \n"));
    if (MrcData->ThermalThrottlingType == 1) {
      MspDebugPrint((MSP_DBG_MAX, "OLTT Mode Enabled... \n"));
    } else {
      MspDebugPrint((MSP_DBG_MAX, "CLTT Mode Enabled... \n"));
      if (MrcData->ClttMode == 1) {
        MspDebugPrint((MSP_DBG_MAX, "CLTT Passthru Mode Enabled... \n"));
      } else {
        MspDebugPrint((MSP_DBG_MAX, "CLTT Normal Mode Enabled... \n"));
      }
    }
  } else {
    MspDebugPrint((MSP_DBG_MAX, "Memory Thermal Throttling Disabled... \n"));
  }
}

/**
  This routine initializes Dynamic Variables for MemHot feature
   
  MEMHOT 
  ________________________________________________________________
  0h - 97% DEFAULT_CRIT_THRT_BW -- Critical %BW to be throttled 
  1h - 90% DEFAULT_HI_THRT_BW   -- High %BW to be throttled 
  2h - 0% DEFAULT_MID_THRT_BW   -- Medium %BW to be throttled 
   
  MEMHOT_DISABLED = 0h -- MEMHOT disabled 
  MEMHOT_MID      = 1h -- MEMHOT using THRT_MID register value 
  MEMHOT_HI       = 2h -- MEMHOT using THRT_HI register value 
  MEMHOT_CRIT     = 3h -- MEMHOT using THRT_CRIT register value
   
  Arguments: 
   
  @param[in, out]  MrcData       Host structure for all data related to MMRC. 
  
**/
VOID
SetMemHotVariables(
  IN  OUT   MMRC_DATA   *MrcData
  )
{
  UINT8 Channel;
  //
  // Enable Slave ID if dimm is present
  //
  if (MrcData->MrcParameters.DimmPresent[0][0]) {
      MrcData->MspData.DynamicVars[0][VALID_C0_D0] = 1;
      MrcData->MspData.DynamicVars[0][SLAVE_ID_C0_D0] = 0x18;
  }
  if (MrcData->MrcParameters.DimmPresent[0][1]) {
      MrcData->MspData.DynamicVars[0][VALID_C0_D1] = 1;
      MrcData->MspData.DynamicVars[0][SLAVE_ID_C0_D1] = 0x19;
  }
  if (MrcData->MrcParameters.DimmPresent[1][0]) {
      MrcData->MspData.DynamicVars[0][VALID_C1_D0] = 1;
      MrcData->MspData.DynamicVars[0][SLAVE_ID_C1_D0] = 0x1A;
  }
  if (MrcData->MrcParameters.DimmPresent[1][1]) {
      MrcData->MspData.DynamicVars[0][VALID_C1_D1] = 1;
      MrcData->MspData.DynamicVars[0][SLAVE_ID_C1_D1] = 0x1B;
  }

  for (Channel = 0; Channel < MAX_CHANNELS; Channel++) {
    MrcData->MspData.DynamicVars[Channel][MEMHOT_THROT_LVL] = MrcData->MemHotThrotLvl;
    if (MrcData->MemoryThermalEnable == 0) {
      MspDebugPrint((MSP_DBG_MIN, "Setting MEMHOT THRT Crit/Hi/Med levels to customer defaults... \n"));
      MrcData->MspData.DynamicVars[Channel][MEMHOT_SET_DEFAULTS] = 1;
    } else {
      MspDebugPrint((MSP_DBG_MIN, "MEMHOT using THRT Crit/Hi/Med levels as CLTT or OLTT is set... \n"));
      MrcData->MspData.DynamicVars[Channel][MEMHOT_SET_DEFAULTS] = 0;
    }
  }

  if (MrcData->MemHotThrotLvl == MEMHOT_DISABLED) {
    MspDebugPrint((MSP_DBG_MIN, "MEMHOT is Disabled... \n"));
  } else if (MrcData->MemHotThrotLvl == MEMHOT_MID) {
    MspDebugPrint((MSP_DBG_MIN, "MEMHOT set to Mid Level... \n"));
  } else if (MrcData->MemHotThrotLvl == MEMHOT_HI) {
    MspDebugPrint((MSP_DBG_MIN, "MEMHOT set to Hi Level... \n"));
  } else if (MrcData->MemHotThrotLvl == MEMHOT_CRIT) {
    MspDebugPrint((MSP_DBG_MIN, "MEMHOT set to Critical Level... \n"));
  }
}

/**
  This routine initializes Dynamic Variables for MemTrip feature
   
  MEMTRIP 
  ______________ 
  0h - Enable 
  1h - Disable
   
  Arguments: 
   
  @param[in, out]  MrcData       Host structure for all data related to MMRC. 
  
**/
VOID
SetMemTripVariables(
  IN  OUT   MMRC_DATA   *MrcData
  )
{
  UINT8 Channel;

  for (Channel = 0; Channel < MAX_CHANNELS; Channel++) {
    MrcData->MspData.DynamicVars[Channel][EN_MEMTRIP]   = MrcData->EnMemTrip;
    MrcData->MspData.DynamicVars[Channel][TEMP_MEMTRIP] = MrcData->TempMemTrip;
  }

  if (MrcData->MemoryThermalEnable == 0) {
    MspDebugPrint((MSP_DBG_MIN, "Disabling MEMTRIP because MTM is disabled... \n"));
  } else {
    if (MrcData->ThermalThrottlingType == 1) {
      MspDebugPrint((MSP_DBG_MIN, "Disabling MEMTRIP because CLTT is disabled... \n"));
    } else {
      if (MrcData->EnMemTrip == 0) {
        MspDebugPrint((MSP_DBG_MIN, "MEMTRIP is Disabled... \n"));
      } else {
        MspDebugPrint((MSP_DBG_MIN, "MEMTRIP is Enabled... \n"));
      }
    }
  }
}

UINT8
A0DllWorkAroundEnabled ()
{
  UINT8                          CheckpointCode;
  REGISTER_ACCESS                 Register;
  //
  // SSKPD7_DUNIT_COMMON_REG bit 0 used to enable the DLL WA  
  //
  Register.Offset = SSKPD7_DUNIT_COMMON_REG;
  Register.Mask = 0xFFFFFFFF;

  CheckpointCode = (UINT8)MemRegRead(DUNIT_COMMON, 0, 0, Register);
  CheckpointCode &= 1;
  return CheckpointCode;
}

/**
  This routine assumes all input parameters to the MMRC have been
  populated by GetInputParameters(). Converting all timing variables
  to the correct values for DUNIT programming is the responsibility
  of this function. For example, CAS latency might be 11 DRAM clocks,
  however the DUNIT field for tCL has an offset of 5 as in the table 
  below:

  tCL
  ___________________
  0h 5 DRAM Clocks
  1h 6 DRAM Clocks
  2h 7 DRAM Clocks
  3h 8 DRAM Clocks
  4h 9 DRAM Clocks
  5h 10 DRAM Clocks
  6h 11 DRAM Clocks 
  7h 12 DRAM Clocks 
  8h 13 DRAM clocks 
  9h 14 DRAM Clocks 
  Ah 15 DRAM Clocks 
  Bh 16 DRAM Clocks

  This routine must translate tCL of 11 to 6 in order for MMRC to
  program the correct value into the DUNIT.

  Arguments:

  @param[in, out]  MrcData       Host structure for all data related to MMRC.
  @retval          Status
**/
MMRC_STATUS
ConvertInputParameters (
  IN  OUT   MMRC_DATA   *MrcData
  )
{
  UINT8 Channel;
  UINT8 Twtr;
  UINT8 Tcl;
  UINT8 Twl;
//  UINT8 ROdtStart;
  UINT8 Tcmd;
//  UINT8 WOdtStart;
//  UINT8 WOdtStop;
  DIMM_VOLTAGE_FLAGS  *DimmVoltageFlags;
  UINT32 MmioAllocation;
  UINT32 TOM;
  UINT32 LowMemory;
  BOOLEAN ReclaimEn;
  UINT8  BitLimit32;
  UINT32 HighMemory;
  UINT16 TSegSize;
  UINT32 TSegBase;
  UINT8  Dimm;
  UINT8  BusWidth;
  UINT8  DeviceWidth;
  UINT16 DramCapacity;
  UINT32 MemSizeToUse;
  UINT8  CurrentDimm;
  UINT8  DetermineCommandMode;
  UINT8  MirrorSpd;
  UINT8  SmbCltt[MAX_CHANNELS][MAX_DIMMS] = { { 0x30, 0x32 }, { 0x34, 0x36 } };
  BOOLEAN BackupEnable;
  UINT8   MaxNMode;
  UINT32 Data;
  UINT32 ErrorCode;


  BackupEnable = MrcData->MspData.Enabled[0];

  MaxNMode     = 0;
  DramCapacity = 256; //Mb
  DeviceWidth  = 4;
  BusWidth     = 8;
  DimmVoltageFlags = (DIMM_VOLTAGE_FLAGS *) &MrcData->DimmVoltageFlags;

  for (Channel = 0; Channel < MAX_CHANNELS; Channel++) {
    if (MrcData->DimmCount[Channel] == 0 || !MrcData->MrcParameters.ChEnabled[Channel]) {
      MrcData->MrcParameters.ChEnabled[Channel] = FALSE;
      continue;
    }
    //
    // OneX Clocks
    //
    //
    // RDIMMs
    //
    if ((MrcData->D_Type[Channel][0] == SPD_DDR_RDIMM || MrcData->D_Type[Channel][1] == SPD_DDR_RDIMM) && MrcData->CommandAddressParity == 1){
      //
      // TODO. Enable CAP for RDIMM only and keep Alert_rxen with default value of 1.
      // WA is to disable for now.
      //
      MrcData->MspData.DynamicVars[Channel][CAPERR] = 1;
      //
      // TODO. Program the REGISTER on the dimm to enable CAP
      //
    } else {
      //
      // Disable CAPERR for UDIMM and SODIMM. And Disable ALERT pin on both channels.
      //
      MrcData->MspData.DynamicVars[Channel][CAPERR] = 0;
    }
    if (MrcData->EnableParityCheck) {
      MrcData->MspData.DynamicVars[Channel][PARITY_CHECK] = 1;
    } else {
      MrcData->MspData.DynamicVars[Channel][PARITY_CHECK] = 0;
    }

    MrcData->MspData.DynamicVars[Channel][WRCMD2DQSSTART_LATENCY] = MrcData->MrcParameters.TimingData[Channel][TimingTwcl][FreqA] - 6;
#if SIM
    MrcData->MspData.DynamicVars[Channel][RDCMD2RCVEN_LATENCY]  = MrcData->MrcParameters.TimingData[Channel][TimingTcl][FreqA] - 8;
#else
    MrcData->MspData.DynamicVars[Channel][RDCMD2RCVEN_LATENCY]  = MrcData->MrcParameters.TimingData[Channel][TimingTcl][FreqA] - 6;
#endif
    
    if (MrcData->MrcParameters.DramType[Channel] == TypeDdr4) {
      MirrorSpd = SPD_DDR4_ADD_MAPPING;
    } else {
      MirrorSpd = SPD_DDR3_ADD_MAPPING;
    }
    if (MrcData->MrcParameters.BootMode == S0 && MrcData->MemoryPreservation) {
      MrcData->MspData.DynamicVars[0][DESTROY_CONTENT_S0] = 0;
      MrcData->MspData.DynamicVars[1][DESTROY_CONTENT_S0] = 0;
      MrcData->MspData.DynamicVars[0][PMOP_LONG_VAR] = 0;
      MrcData->MspData.DynamicVars[1][PMOP_LONG_VAR] = 0;
    } else {
      MrcData->MspData.DynamicVars[0][DESTROY_CONTENT_S0] = 1;
      MrcData->MspData.DynamicVars[1][DESTROY_CONTENT_S0] = 1;
      MrcData->MspData.DynamicVars[0][PMOP_LONG_VAR] = 1;
      MrcData->MspData.DynamicVars[1][PMOP_LONG_VAR] = 1;
    }
    MspDebugPrint ((
      MSP_DBG_MIN, "DESTROY_CONTENT_S0: %d:%d\n",
      (UINT32) MrcData->MspData.DynamicVars[0][DESTROY_CONTENT_S0],
      (UINT32) MrcData->MspData.DynamicVars[1][DESTROY_CONTENT_S0]
    ));
    // 
    // Look at both DIMMs and fill in DIMM-specific variables. Also add up total memory
    // per channel and system-wide.
    //
    if (MrcData->MrcParameters.DimmPresent[Channel][0]) {
      //
      // Set x4, x8 or x16 config.
      //
      switch (MrcData->D_DataWidth[Channel][0]) {
      case 0:   // x4
        MrcData->MspData.DynamicVars[Channel][DIMMDWID] = 1;
        break;
      case 1:   // x8
        MrcData->MspData.DynamicVars[Channel][DIMMDWID] = 0;
        break;
      case 2:   // x16
        MrcData->MspData.DynamicVars[Channel][DIMMDWID] = 2;
        MrcData->MspData.DynamicVars[Channel][DMAP_X16] = 1;
        break;
      default:
        MrcData->MspData.DynamicVars[Channel][DIMMDWID] = 0;
        break;
      }
      MrcData->MspData.DynamicVars[Channel][DIMM0MIRR] = MrcData->MrcParameters.SpdData[Channel][0].Buffer[MirrorSpd] & 1;
      if (MrcData->Dimm_1Gb[Channel] == 1) {
        MrcData->MspData.DynamicVars[Channel][DIMMDDEN0] = 0;
        MrcData->MrcParameters.SlotMem[Channel][0]     =  ((DramCapacity << (MrcData->D_Size[Channel][0])) / 8 *
                                                       (BusWidth << MrcData->D_BusWidth[Channel][0]) /
                                                       (DeviceWidth << (MrcData->D_DataWidth[Channel][0])) *
                                                       (MrcData->D_Ranks[Channel][0] + 1));
      } else {
        MrcData->MspData.DynamicVars[Channel][DIMMDDEN0] = MrcData->D_Size[Channel][0];
        MrcData->MrcParameters.SlotMem[Channel][0]     =  ((DramCapacity << (MrcData->D_Size[Channel][0] + 3)) / 8 *
                                                       (BusWidth << MrcData->D_BusWidth[Channel][0]) /
                                                       (DeviceWidth << (MrcData->D_DataWidth[Channel][0])) *
                                                       (MrcData->D_Ranks[Channel][0] + 1));
      }
      if (MrcData->HalfWidthEnable == 1) {
    	  MrcData->MrcParameters.SlotMem[Channel][0] = (MrcData->MrcParameters.SlotMem[Channel][0] / 2);
      }
      MspDebugPrint ((MSP_DBG_MIN, "DIMM0 Memory Size: %dMB\n", MrcData->MrcParameters.SlotMem[Channel][0]));
      MrcData->MrcParameters.TotalMem[Channel]           += MrcData->MrcParameters.SlotMem[Channel][0];
      MrcData->MrcParameters.SystemMemorySize                    += MrcData->MrcParameters.SlotMem[Channel][0];
      if (MrcData->D_Type[Channel][0] == SPD_DDR_RDIMM) {
        MrcData->MspData.DynamicVars[Channel][DIMMTYPE] =  1;
        MrcData->MrcParameters.SpdData[Channel][0].Buffer[SPD_DDR4_ADD_MAPPING] = MrcData->MrcParameters.SpdData[Channel][0].Buffer[SPD_DDR4_ADD_MAPPINGR];
      } else {
        MrcData->MspData.DynamicVars[Channel][DIMMTYPE] =  0;
      }
    }
    if (MrcData->MrcParameters.DimmPresent[Channel][1]) {
      //
      // Set x4, x8 or x16 config.
      //
      switch (MrcData->D_DataWidth[Channel][1]) {
      case 0:   // x4
        MrcData->MspData.DynamicVars[Channel][DIMMDWID] = 1;
        break;
      case 1:   // x8
        MrcData->MspData.DynamicVars[Channel][DIMMDWID] = 0;
        break;
      case 2:   // x16
        MrcData->MspData.DynamicVars[Channel][DIMMDWID] = 2;
        MrcData->MspData.DynamicVars[Channel][DMAP_X16] = 1;
        break;
      default:
        MrcData->MspData.DynamicVars[Channel][DIMMDWID] = 0;
        break;
      }
      MspDebugPrint ((
        MSP_DBG_MIN, "DMAP_X16: %d\n",
        (UINT32) MrcData->MspData.DynamicVars[Channel][DMAP_X16]
      ));
      MrcData->MspData.DynamicVars[Channel][DIMM1MIRR] = MrcData->MrcParameters.SpdData[Channel][1].Buffer[MirrorSpd] & 1;
      if (MrcData->Dimm_1Gb[Channel] == 1) {
        MrcData->MspData.DynamicVars[Channel][DIMMDDEN1] = 0;
        MrcData->MrcParameters.SlotMem[Channel][1]     =  ((DramCapacity << (MrcData->D_Size[Channel][1])) / 8 *
                                                       (BusWidth << MrcData->D_BusWidth[Channel][1]) /
                                                       (DeviceWidth << (MrcData->D_DataWidth[Channel][1])) *
                                                       (MrcData->D_Ranks[Channel][1] + 1));
      } else {
        MrcData->MspData.DynamicVars[Channel][DIMMDDEN1] = MrcData->D_Size[Channel][1];
        MrcData->MrcParameters.SlotMem[Channel][1]     =  ((DramCapacity << (MrcData->D_Size[Channel][1] + 3)) / 8 *
                                                       (BusWidth << MrcData->D_BusWidth[Channel][1]) /
                                                       (DeviceWidth << (MrcData->D_DataWidth[Channel][1])) *
                                                       (MrcData->D_Ranks[Channel][1] + 1));
      }
      if (MrcData->HalfWidthEnable == 1) {
    	  MrcData->MrcParameters.SlotMem[Channel][1] = (MrcData->MrcParameters.SlotMem[Channel][0] / 2);
      }
      MspDebugPrint ((MSP_DBG_MIN, "DIMM1 Memory Size: %dMB\n", MrcData->MrcParameters.SlotMem[Channel][1]));
      MrcData->MrcParameters.TotalMem[Channel]       += MrcData->MrcParameters.SlotMem[Channel][1];
      MrcData->MrcParameters.SystemMemorySize                += MrcData->MrcParameters.SlotMem[Channel][1];
      if (MrcData->D_Type[Channel][1] == SPD_DDR_RDIMM) {
        MrcData->MspData.DynamicVars[Channel][DIMMTYPE] =  1;
        MrcData->MrcParameters.SpdData[Channel][1].Buffer[SPD_DDR4_ADD_MAPPING] = MrcData->MrcParameters.SpdData[Channel][1].Buffer[SPD_DDR4_ADD_MAPPINGR];
      } else {
        MrcData->MspData.DynamicVars[Channel][DIMMTYPE] =  0;
      }
    }

    MrcData->MspData.DynamicVars[Channel][RANK0_ENABLED] = MrcData->MrcParameters.RankPresent[Channel][0];
    MrcData->MspData.DynamicVars[Channel][RANK0_RDIMM] = 0;
    if (MrcData->MrcParameters.RankPresent[Channel][0] && MrcData->D_Type[Channel][0] == SPD_DDR_RDIMM) {
      MrcData->MspData.DynamicVars[Channel][RANK0_RDIMM] = 1;
    }
    MrcData->MspData.DynamicVars[Channel][RANK1_ENABLED] = MrcData->MrcParameters.RankPresent[Channel][1];
    MrcData->MspData.DynamicVars[Channel][RANK1_RDIMM] = 0;
    if (MrcData->MrcParameters.RankPresent[Channel][1] && MrcData->D_Type[Channel][0] == SPD_DDR_RDIMM) {
      MrcData->MspData.DynamicVars[Channel][RANK1_RDIMM] = 1;
    }
    MrcData->MspData.DynamicVars[Channel][RANK2_ENABLED] = MrcData->MrcParameters.RankPresent[Channel][2];
    MrcData->MspData.DynamicVars[Channel][RANK2_RDIMM] = 0;
    if (MrcData->MrcParameters.RankPresent[Channel][2] && MrcData->D_Type[Channel][1] == SPD_DDR_RDIMM) {
      MrcData->MspData.DynamicVars[Channel][RANK2_RDIMM] = 1;
    }
    MrcData->MspData.DynamicVars[Channel][RANK3_ENABLED] = MrcData->MrcParameters.RankPresent[Channel][3];
    MrcData->MspData.DynamicVars[Channel][RANK3_RDIMM] = 0;
    if (MrcData->MrcParameters.RankPresent[Channel][3] && MrcData->D_Type[Channel][1] == SPD_DDR_RDIMM) {
      MrcData->MspData.DynamicVars[Channel][RANK3_RDIMM] = 1;
    }
    MspDebugPrint ((MSP_DBG_MIN, "CH%d\n", Channel));
    MspDebugPrint ((MSP_DBG_MIN, "R[0]: %d / %d (RDIMM)\n",
        (UINT32)MrcData->MspData.DynamicVars[Channel][RANK0_ENABLED],
        (UINT32)MrcData->MspData.DynamicVars[Channel][RANK0_RDIMM]
    ));
    MspDebugPrint ((MSP_DBG_MIN, "R[1]: %d / %d (RDIMM)\n",
        (UINT32)MrcData->MspData.DynamicVars[Channel][RANK1_ENABLED],
        (UINT32)MrcData->MspData.DynamicVars[Channel][RANK1_RDIMM]
    ));
    MspDebugPrint ((MSP_DBG_MIN, "R[2]: %d / %d (RDIMM)\n",
        (UINT32)MrcData->MspData.DynamicVars[Channel][RANK2_ENABLED],
        (UINT32)MrcData->MspData.DynamicVars[Channel][RANK2_RDIMM]
    ));
    MspDebugPrint ((MSP_DBG_MIN, "R[3]: %d / %d (RDIMM)\n",
        (UINT32)MrcData->MspData.DynamicVars[Channel][RANK3_ENABLED],
        (UINT32)MrcData->MspData.DynamicVars[Channel][RANK3_RDIMM]
    ));

    MemSizeToUse = MrcData->MrcParameters.SlotMem[Channel][0];
    MrcData->MspData.DynamicVars[Channel][DIMMFLIP] = 0;
    if (MrcData->MrcParameters.SlotMem[Channel][1] > MrcData->MrcParameters.SlotMem[Channel][0]) {
      MrcData->MspData.DynamicVars[Channel][DIMMFLIP] = 1;
      MemSizeToUse = MrcData->MrcParameters.SlotMem[Channel][1];
    }
    //
    // Verify the size id not smaller than 256 MB or bigger than 65536MB
    //
    if (MemSizeToUse < (1 << 8) || MemSizeToUse > (1 << 16)) {
       MspDebugPrint ((MSP_DBG_MIN, "Invalid Memory Size found: %dMB\n", MemSizeToUse));
       MrcData->ErrorCode = MrcErrInvalidMemorySize;
       MrcDeadLoop ();
    }
    //
    // Verify Max Memory Size per Channel
    //
  if (MrcData->MspData.CurrentPlatform[0] != P_SIMICS && MrcData->MspData.CurrentPlatform[0] != P_EMU && MrcData->MspData.CurrentPlatform[0] != P_EMU_DUNIT) {
    MemSizeToUse = MrcData->MrcParameters.SlotMem[Channel][0] + MrcData->MrcParameters.SlotMem[Channel][1];
    if (ChannelSizeFusetoMBSize (MrcData->MspData.DynamicVars[Channel][MAX_MEM_SUPPORTED]) == 0) {
      MspDebugPrint ((MSP_DBG_MIN, "Invalid fuse for memory size per channel %d\n", MrcData->MspData.DynamicVars[Channel][MAX_MEM_SUPPORTED]));
      MrcData->ErrorCode = MrcErrInvalidMemorySize;
      return MrcErrInvalidMemorySize;
    }

    if (MemSizeToUse > ChannelSizeFusetoMBSize (MrcData->MspData.DynamicVars[Channel][MAX_MEM_SUPPORTED])) {
      MspDebugPrint ((
        MSP_DBG_MIN,
        "Invalid Memory Size per channel found: %dMB fuse support:  %dMB\n",
        MemSizeToUse,
        ChannelSizeFusetoMBSize (MrcData->MspData.DynamicVars[Channel][MAX_MEM_SUPPORTED])
        ));
      MrcData->ErrorCode = MrcErrInvalidMemorySize;
      return MrcErrInvalidMemorySize;
    }
  }
    MrcData->MspData.DynamicVars[Channel][MAP0] = mDmapRegisters[MrcData->DimmConfig[Channel]][0];
    MrcData->MspData.DynamicVars[Channel][MAP1] = mDmapRegisters[MrcData->DimmConfig[Channel]][1];
    MrcData->MspData.DynamicVars[Channel][MAP2] = mDmapRegisters[MrcData->DimmConfig[Channel]][2];
    MrcData->MspData.DynamicVars[Channel][MAP3] = mDmapRegisters[MrcData->DimmConfig[Channel]][3];
    MrcData->MspData.DynamicVars[Channel][MAP4] = mDmapRegisters[MrcData->DimmConfig[Channel]][4];
    MrcData->MspData.DynamicVars[Channel][MAP5] = mDmapRegisters[MrcData->DimmConfig[Channel]][5];
    MrcData->CpgcOptions.VicPattern     = CPGC_LFSR_VICTIM_SEED;   
    MrcData->CpgcOptions.Pattern        = CPGC_LFSR_AGGRESSOR_SEED;   
//    MrcData->MspData.DynamicVars[Channel][VICTIM_PATTERN]     = CPGC_LFSR_VICTIM_SEED;
//    MrcData->MspData.DynamicVars[Channel][AGGRESSOR_PATTERN]  = CPGC_LFSR_AGGRESSOR_SEED;

  //
  // Set/Unset Segment directed to DRAM
  //
  MrcData->MspData.DynamicVars[0][SET_ABSEGINDRAM]                = MrcData->AbSegToDram;
  MrcData->MspData.DynamicVars[0][SET_READ_FSEG_FROM_DRAM]        = MrcData->FSegToDram;
  MrcData->MspData.DynamicVars[0][SET_READ_ESEG_FROM_DRAM]        = MrcData->ESegToDram;
  // 
  // Memory Mapping
  //
  TSegSize       = MrcData->OemMrcData.TsegSize;
  MmioAllocation = MrcData->OemMrcData.MmioSize;
  TOM            = MrcData->MrcParameters.SystemMemorySize;
  LowMemory      = MIN (0x1000 - MmioAllocation, TOM);

  ReclaimEn = FALSE;

  if (TOM > LowMemory) {
    ReclaimEn = TRUE;
  }

#ifdef SIM
  BitLimit32 = 1;
#else
  _asm {
    pushad
    mov    eax, 80000008h
    cpuid
    cmp    al, 20h
    je     BitLimited
    mov    BitLimit32, 0
    jmp    _Exit
    BitLimited:
    mov    BitLimit32, 1
    _Exit:
    popad
  }
#endif
  //
  // Only 32 bits for address? then we can't reclaim
  //
  if (BitLimit32 != 0) {
    ReclaimEn = FALSE;
    TOM = LowMemory;
  } 

  if (ReclaimEn) {
    HighMemory = TOM - LowMemory + 0x1000;
  } else {
    HighMemory = 0x1000;
  }

  MspDebugPrint ((MSP_DBG_MIN, "TSegSize:       0x%08x\n", TSegSize));
  MspDebugPrint ((MSP_DBG_MIN, "MmioAllocation: 0x%08x\n", MmioAllocation));
  MspDebugPrint ((MSP_DBG_MIN, "TOM:            0x%08x\n", TOM));
  MspDebugPrint ((MSP_DBG_MIN, "LowMemory:      0x%08x\n", LowMemory));
  MspDebugPrint ((MSP_DBG_MIN, "HighMemory:     0x%08x\n", HighMemory));
  //
  // Determine TSEG base, which is calculated by the value of TOLUD minus TSEG size.
  //
  TSegBase = LowMemory - TSegSize;
  //
  // TOLUD is aligned to 1MB
  //
  MrcData->MspData.DynamicVars[0][TOLUD]    = (UINT32)(LowMemory << 20);
  //
  // TOUUD is aligned to 1MB
  //
  MrcData->MspData.DynamicVars[0][TOUUD_LO]  = (UINT32) (HighMemory &  0x0FFF) << 20;
  MrcData->MspData.DynamicVars[0][TOUUD_HI]  = (UINT32) (((HighMemory & 0x7F000) >> 12));
  MrcData->MspData.DynamicVars[0][TSEGMB]    = (UINT32) (TSegBase << 20);
  //
  // End of Memory Mapping
  //
  //
  // IPROCTRIM
  //
  switch (MrcData->MspData.DynamicVars[Channel][IPROCTRIM]){
  case MMRC_TYPICAL:
    switch (MrcData->MrcParameters.DdrFrequency){
      case MMRC_1600:
      case MMRC_1866:
        //
        // Initialize only Channel 0 because this is used by DDRIO which only has one instace
        //
        MrcData->MspData.DynamicVars[0][CPTRIM] = 2;
        MrcData->MspData.DynamicVars[0][DRVSEL] = 0;
        MrcData->MspData.DynamicVars[0][PIDRVSEL] = 0;
        break;
      case MMRC_2133:
        MrcData->MspData.DynamicVars[0][CPTRIM] = 3;
        MrcData->MspData.DynamicVars[0][DRVSEL] = 2;
        MrcData->MspData.DynamicVars[0][PIDRVSEL] = 5;
        break;
      case MMRC_2400:
        MrcData->MspData.DynamicVars[0][CPTRIM] = 5;
        MrcData->MspData.DynamicVars[0][DRVSEL] = 3;
        MrcData->MspData.DynamicVars[0][PIDRVSEL] = 6;
        break;
      case MMRC_2666:
        MrcData->MspData.DynamicVars[0][CPTRIM] = 7;
        MrcData->MspData.DynamicVars[0][DRVSEL] = 5;
        MrcData->MspData.DynamicVars[0][PIDRVSEL] = 7;
        break;
      default:
        // Freq error not supported
        break;
    }
    break;
  case MMRC_SLOW:
    switch (MrcData->MrcParameters.DdrFrequency){
    case MMRC_1600:
    case MMRC_1866:
      MrcData->MspData.DynamicVars[0][CPTRIM] = 3;
      MrcData->MspData.DynamicVars[0][DRVSEL] = 1;
      MrcData->MspData.DynamicVars[0][PIDRVSEL] = 3;
      break;
    case MMRC_2133:
      MrcData->MspData.DynamicVars[0][CPTRIM] = 4;
      MrcData->MspData.DynamicVars[0][DRVSEL] = 3;
      MrcData->MspData.DynamicVars[0][PIDRVSEL] = 6;
      break;
    case MMRC_2400:
      MrcData->MspData.DynamicVars[0][CPTRIM] = 6;
      MrcData->MspData.DynamicVars[0][DRVSEL] = 4;
      MrcData->MspData.DynamicVars[0][PIDRVSEL] = 7;
      break;
    case MMRC_2666:
      MrcData->MspData.DynamicVars[0][CPTRIM] = 7;
      MrcData->MspData.DynamicVars[0][DRVSEL] = 6;
      MrcData->MspData.DynamicVars[0][PIDRVSEL] = 7;
      break;
    default:
      // Freq error not supported
      break;
    }
    break;
  case MMRC_FAST:
    switch (MrcData->MrcParameters.DdrFrequency){
    case MMRC_1600:
    case MMRC_1866:
      MrcData->MspData.DynamicVars[0][CPTRIM] = 1;
      MrcData->MspData.DynamicVars[0][DRVSEL] = 0;
      MrcData->MspData.DynamicVars[0][PIDRVSEL] = 1;
      break;
    case MMRC_2133:
      MrcData->MspData.DynamicVars[0][CPTRIM] = 2;
      MrcData->MspData.DynamicVars[0][DRVSEL] = 1;
      MrcData->MspData.DynamicVars[0][PIDRVSEL] = 4;
      break;
    case MMRC_2400:
      MrcData->MspData.DynamicVars[0][CPTRIM] = 4;
      MrcData->MspData.DynamicVars[0][DRVSEL] = 2;
      MrcData->MspData.DynamicVars[0][PIDRVSEL] = 5;
      break;
    case MMRC_2666:
      MrcData->MspData.DynamicVars[0][CPTRIM] = 6;
      MrcData->MspData.DynamicVars[0][DRVSEL] = 4;
      MrcData->MspData.DynamicVars[0][PIDRVSEL] = 6;
      break;
    default:
      // Freq error not supported
      break;
    }

    break;
  case MMRC_UNUSED:
  default:
    //
    //For B0 PO this should be treated as Typical but after PO this should be treated as an error
    //
    switch (MrcData->MrcParameters.DdrFrequency){
    case MMRC_1600:
    case MMRC_1866:
      MrcData->MspData.DynamicVars[0][CPTRIM] = 2;
      MrcData->MspData.DynamicVars[0][DRVSEL] = 0;
      MrcData->MspData.DynamicVars[0][PIDRVSEL] = 0;
      break;
    case MMRC_2133:
      MrcData->MspData.DynamicVars[0][CPTRIM] = 3;
      MrcData->MspData.DynamicVars[0][DRVSEL] = 2;
      MrcData->MspData.DynamicVars[0][PIDRVSEL] = 5;
      break;
    case MMRC_2400:
      MrcData->MspData.DynamicVars[0][CPTRIM] = 5;
      MrcData->MspData.DynamicVars[0][DRVSEL] = 3;
      MrcData->MspData.DynamicVars[0][PIDRVSEL] = 6;
      break;
    case MMRC_2666:
      MrcData->MspData.DynamicVars[0][CPTRIM] = 7;
      MrcData->MspData.DynamicVars[0][DRVSEL] = 5;
      MrcData->MspData.DynamicVars[0][PIDRVSEL] = 7;
      break;
    default:
      // Freq error not supported
      break;
    }
    break;
  }

  } // Channel loop ...
  //
  // Dynamic Input Parameters required for MEMINIT of MMRC.
  //
  if (!MrcData->MrcParameters.ChEnabled[0] && !MrcData->MrcParameters.ChEnabled[1]) {
    return MrcErrChannelsDisabled;
  } else {
    MrcData->MspData.DynamicVars[0][CH0_EN] = MrcData->MrcParameters.ChEnabled[0];
    MrcData->MspData.DynamicVars[0][CH1_EN] = MrcData->MrcParameters.ChEnabled[1];
    MrcData->MspData.DynamicVars[1][CH0_EN] = MrcData->MrcParameters.ChEnabled[0];
    MrcData->MspData.DynamicVars[1][CH1_EN] = MrcData->MrcParameters.ChEnabled[1];
    MrcData->MspData.DynamicVars[0][CH0_DIS] = !MrcData->MrcParameters.ChEnabled[0];
    MrcData->MspData.DynamicVars[0][CH1_DIS] = !MrcData->MrcParameters.ChEnabled[1];
    MrcData->MspData.DynamicVars[1][CH0_DIS] = !MrcData->MrcParameters.ChEnabled[0];
    MrcData->MspData.DynamicVars[1][CH1_DIS] = !MrcData->MrcParameters.ChEnabled[1];
    MspDebugPrint ((MSP_DBG_MIN, "Channel 0 state: %d\n", MrcData->MrcParameters.ChEnabled[0]));
    MspDebugPrint ((MSP_DBG_MIN, "Channel 1 state: %d\n", MrcData->MrcParameters.ChEnabled[1]));
  }
  MspDebugPrint ((
    MSP_DBG_MAX, "CH0_EN: %d:%d CH0_DIS: %d:%d \n",
    (UINT32) MrcData->MspData.DynamicVars[0][CH0_EN],  (UINT32) MrcData->MspData.DynamicVars[1][CH0_EN],
    (UINT32) MrcData->MspData.DynamicVars[0][CH0_DIS], (UINT32) MrcData->MspData.DynamicVars[1][CH0_DIS]
  ));
  MspDebugPrint ((
    MSP_DBG_MAX, "CH1_EN: %d:%d CH1_DIS: %d:%d \n",
    (UINT32) MrcData->MspData.DynamicVars[0][CH1_EN],  (UINT32) MrcData->MspData.DynamicVars[1][CH1_EN],
    (UINT32) MrcData->MspData.DynamicVars[0][CH1_DIS], (UINT32) MrcData->MspData.DynamicVars[1][CH1_DIS]
  ));

  MrcData->MspData.DynamicVars[0][MCI_IEH_N]      = MrcData->MciIehN;
  MrcData->MspData.DynamicVars[1][MCI_IEH_N]      = MrcData->MciIehN;

  ConfigureInterleaving (MrcData);
  
  for (Channel = 0; Channel < MAX_CHANNELS; Channel++) {
    //
    // Variables that must be initialize for both channels
    // This case applies to NCBB (Non-Channel Based BoxType)
    //
    MrcData->MspData.DynamicVars[Channel][PAGE_CLOSE_TIMEOUT]     = MrcData->OpenPolicyTimer;
    MrcData->MspData.DynamicVars[Channel][OOODIS]                 = MrcData->OutOfOrderDisable;
    MrcData->MspData.DynamicVars[Channel][NEW_REQ_BYPASS_DISABLE] = MrcData->NewRequestBypassDisable;
    if (!MrcData->ZqcEnable) {
      MrcData->MspData.DynamicVars[Channel][ZQCINT] = 0;
    } else {
      switch (MrcData->MrcParameters.DdrFrequency) {
      case MMRC_1600:
        if (MrcData->MrcParameters.DramType[Channel] <= TypeDdr3All) {
          MrcData->MspData.DynamicVars[Channel][ZQCINT] = 1562;
        } else if (MrcData->MrcParameters.DramType[Channel] == TypeDdr4) {
          MrcData->MspData.DynamicVars[Channel][ZQCINT] = 1562;
        }
        break;
      case MMRC_1866:
        if (MrcData->MrcParameters.DramType[Channel] <= TypeDdr3All) {
          MrcData->MspData.DynamicVars[Channel][ZQCINT] = 1822;
        } else if (MrcData->MrcParameters.DramType[Channel] == TypeDdr4) {
          MrcData->MspData.DynamicVars[Channel][ZQCINT] = 1822;
        }
        break;
      case MMRC_2133:
      case MMRC_2400:
      case MMRC_2666:
        MrcData->MspData.DynamicVars[Channel][ZQCINT] = 2047;
        break;
      default:
        MrcData->MspData.DynamicVars[Channel][ZQCINT] = 0;
        break;
      } // switch
    } // if condition

    MrcData->MspData.DynamicVars[Channel][CKE_POWER_DOWN_DIS] = MrcData->CkePowerDownDisable;

    Data = 0;
    ErrorCode = MMRC_SUCCESS;
    if (MrcData->SelectRefreshRate != Rate0x) {
      MrcData->MspData.DynamicVars[Channel][TREFI_SHIFT]  = AntiLogBase2 (MrcData->SelectRefreshRate);
      if (MrcData->SelectRefreshRate == 4) {
        Data = 1;
      } else {
        Data = 0;
      }
      if (MrcData->SocStepping > DNV_A1_STEP) {
        //
        //write to punit
        //
        ErrorCode = (UINT32)WriteDataB2P(MrcData, MBX_CMD_WRITE_DDR_REFRESH_MAILBOX, 0, Data);
        if (ErrorCode != MMRC_SUCCESS) {
          if (ErrorCode >= mailbox_cc_maxerror) {
            ErrorCode = mailbox_cc_unknown;
          }
          return ErrorCode;
        }
      }
    } else {
      MrcData->MspData.DynamicVars[Channel][TREFI_SHIFT]  = 30;
    }

    if (!MrcData->MrcParameters.ChEnabled[Channel]) {
      continue;
    }
#ifdef SIM
    //
    // When Rdimms are used the test bench provides increased value for Tcl and Twl. So we have to drecrease
    //
    if (MrcData->MspData.DynamicVars[Channel][DIMMTYPE] == SPD_DDR_RDIMM) {
      MrcData->MrcParameters.TimingData[Channel][TimingTcl][FreqA]--;
      MrcData->MrcParameters.TimingData[Channel][TimingTwcl][FreqA]--;
    }

#endif
    Tcl      = (UINT8) (MrcData->MrcParameters.TimingData[Channel][TimingTcl][FreqA]);
    Twl      = (UINT8) (MrcData->MrcParameters.TimingData[Channel][TimingTwcl][FreqA]);
    //
    // The controller side needs to have Tcl and Twl increased by 1 tick when we use RDIMMs
    //
    if (MrcData->MspData.DynamicVars[Channel][DIMMTYPE] == SPD_DDR_RDIMM) {
      Tcl++;
      Twl++;
    }
    Twtr     = (UINT8) (MrcData->MrcParameters.TimingData[Channel][TimingTwtr][FreqA]);
    Tcmd     = (UINT8) (MrcData->MrcParameters.TimingData[Channel][TimingTcmd][FreqA]) - 1;

    if (MrcData->MrcParameters.DdrFrequency >= MMRC_2400) {
      MrcData->MspData.DynamicVars[Channel][TWO_CLK_RD_PREAMBLE] = MrcData->TwoClkRdPreamble[Channel];
      MrcData->MspData.DynamicVars[Channel][TWO_CLK_WR_PREAMBLE] = MrcData->TwoClkWrPreamble[Channel];
      MspDebugPrint ((MSP_DBG_MAX, "TWO_CLK_RD_PREAMBLE: %d\n", MrcData->MspData.DynamicVars[Channel][TWO_CLK_RD_PREAMBLE]));
      MspDebugPrint ((MSP_DBG_MAX, "TWO_CLK_WR_PREAMBLE: %d\n", MrcData->MspData.DynamicVars[Channel][TWO_CLK_WR_PREAMBLE]));
    } else {
      MrcData->MspData.DynamicVars[Channel][TWO_CLK_RD_PREAMBLE] = 0;
      MrcData->MspData.DynamicVars[Channel][TWO_CLK_WR_PREAMBLE] = 0;
    }
    if (MrcData->TimingDataOverrides[Channel][TimingTcl][FreqA] != 0xFF) {
    	MrcData->MspData.DynamicVars[Channel][TCL]     = MrcData->TimingDataOverrides[Channel][TimingTcl][FreqA] - 7;
    } else {
      MrcData->MspData.DynamicVars[Channel][TCL]     = (UINT8)(Tcl  - 7);
    }
    MrcData->MspData.DynamicVars[Channel][TRP]     = (UINT8)(MrcData->MrcParameters.TimingData[Channel][TimingTrp][FreqA]  - 7);
    MrcData->MspData.DynamicVars[Channel][TRCD_RD] = (UINT8)(MrcData->MrcParameters.TimingData[Channel][TimingTrcd][FreqA] - 7);
    MrcData->MspData.DynamicVars[Channel][TRCD_WR] = MrcData->MspData.DynamicVars[Channel][TRCD_RD];
    MrcData->MspData.DynamicVars[Channel][TWCL]    = (UINT8)(Twl - 7);
    MrcData->MspData.DynamicVars[Channel][TCMD]    = (UINT8) Tcmd;
    MrcData->MspData.DynamicVars[Channel][TWR]     = (UINT8)(MrcData->MrcParameters.TimingData[Channel][TimingTwr][FreqA]  - 10);
    if (MrcData->MspData.DynamicVars[Channel][TWO_CLK_WR_PREAMBLE] == 1) {
      MrcData->MspData.DynamicVars[Channel][TWR] += 2;
    }
    MrcData->MspData.DynamicVars[Channel][TRAS]    = (UINT8)(MrcData->MrcParameters.TimingData[Channel][TimingTras][FreqA] - 24);
    MrcData->MspData.DynamicVars[Channel][TRTP]    = (UINT8)(MrcData->MrcParameters.TimingData[Channel][TimingTrtp][FreqA] -  5);
    MrcData->MspData.DynamicVars[Channel][TRRDR]   = (UINT8)((4 + 2 + TimingGuardbands[Trrdr][MrcData->MrcParameters.DdrFrequency - MINDDR]) - 4);
    MrcData->MspData.DynamicVars[Channel][TRRDD]   = (UINT8)((4 + 2 + TimingGuardbands[Trrdd][MrcData->MrcParameters.DdrFrequency - MINDDR]) - 4);
    MrcData->MspData.DynamicVars[Channel][TWWDR]   = (UINT8)((4 + 4 + TimingGuardbands[Twwdr][MrcData->MrcParameters.DdrFrequency - MINDDR]) - 4);
    MrcData->MspData.DynamicVars[Channel][TWWDD]   = (UINT8)((4 + 4 + TimingGuardbands[Twwdd][MrcData->MrcParameters.DdrFrequency - MINDDR]) - 4);
    MrcData->MspData.DynamicVars[Channel][TRWDR]   = (UINT8)((4 + 4 + TimingGuardbands[Trwdr][MrcData->MrcParameters.DdrFrequency - MINDDR]) - 6);
    MrcData->MspData.DynamicVars[Channel][TRWDD]   = (UINT8)((4 + 4 + TimingGuardbands[Trwdd][MrcData->MrcParameters.DdrFrequency - MINDDR]) - 6);
    MrcData->MspData.DynamicVars[Channel][TWRDR]   = (UINT8) (MAX(((4 + 4 + Twl - Tcl)), 6) + TimingGuardbands[Twrdr][MrcData->MrcParameters.DdrFrequency - MINDDR] - 6);
    MrcData->MspData.DynamicVars[Channel][TWRDD]   = (UINT8)(MAX(((4 + 4 + Twl - Tcl)), 6) + TimingGuardbands[Twrdd][MrcData->MrcParameters.DdrFrequency - MINDDR] - 6);
    MrcData->MspData.DynamicVars[Channel][TRWSR]   = (UINT8)((4 + TimingGuardbands[Trwsrl][MrcData->MrcParameters.DdrFrequency - MINDDR]) - 6);
    MrcData->MspData.DynamicVars[Channel][BIBLIM]  = 0;

    //
    // tXP is calculated based on cmd timing and frequency.
    // TRRD_S is based on density and frequency. (Applies for DDR3/DDR4, for DDR4 is always '0')
    // TRRD_L is based on density and frequency. (Applies for DDR4 Only)
    //
    MrcData->MspData.DynamicVars[Channel][TRRD_S] = 0;
    MrcData->MspData.DynamicVars[Channel][TRRD_L] = 0;
    switch (MrcData->MrcParameters.DdrFrequency) {
    case MMRC_1600:
      MrcData->MspData.DynamicVars[Channel][TXP] = 2;
      if (MrcData->MrcParameters.DramType[Channel] <= TypeDdr3All) {
        MrcData->MspData.DynamicVars[Channel][TRRD_S] = 1;
        if (MrcData->D_Size[Channel][0] == 2 || MrcData->D_Size[Channel][1] == 2) {
          MrcData->MspData.DynamicVars[Channel][TRRD_S]++;
        }
      } else if (MrcData->MrcParameters.DramType[Channel] == TypeDdr4){
        MrcData->MspData.DynamicVars[Channel][TRRD_L] = 1;
      }
      MrcData->MspData.DynamicVars[Channel][TCCD_L_WR] = 1;
      MrcData->MspData.DynamicVars[Channel][TCCD_L_RD] = 1;
      MrcData->MspData.DynamicVars[Channel][TWTR_L]    = 0;
      MrcData->MspData.DynamicVars[Channel][TCKE]      = 0;
      break;
    case MMRC_1866:
      MrcData->MspData.DynamicVars[Channel][TXP] = 2;
      if (MrcData->MrcParameters.DramType[Channel] <= TypeDdr3All) {
        MrcData->MspData.DynamicVars[Channel][TRRD_S] = 2;
        if (MrcData->D_Size[Channel][0] == 2 || MrcData->D_Size[Channel][1] == 2) {
          MrcData->MspData.DynamicVars[Channel][TRRD_S]++;
        }
      } else if (MrcData->MrcParameters.DramType[Channel] == TypeDdr4) {
        MrcData->MspData.DynamicVars[Channel][TRRD_L] = 1;
      }
      MrcData->MspData.DynamicVars[Channel][TCCD_L_WR] = 1; 
      MrcData->MspData.DynamicVars[Channel][TCCD_L_RD] = 1;
      MrcData->MspData.DynamicVars[Channel][TCKE]      = 1;
      MrcData->MspData.DynamicVars[Channel][TWTR_L]    = 1;
      break;
    case MMRC_2133:
      MrcData->MspData.DynamicVars[Channel][TRRD_L]     = 2;
      MrcData->MspData.DynamicVars[Channel][TXP]        = 3;
      MrcData->MspData.DynamicVars[Channel][TCCD_L_WR]  = 2;  
      MrcData->MspData.DynamicVars[Channel][TCCD_L_RD]  = 2;
      MrcData->MspData.DynamicVars[Channel][TCKE]       = 2;
      MrcData->MspData.DynamicVars[Channel][TWTR_L]     = 2;
      break;
    case MMRC_2400:
      MrcData->MspData.DynamicVars[Channel][TRRD_L]    = 2;
      MrcData->MspData.DynamicVars[Channel][TXP]       = 4;
      MrcData->MspData.DynamicVars[Channel][TCCD_L_WR] = 2; 
      MrcData->MspData.DynamicVars[Channel][TCCD_L_RD] = 2;
      MrcData->MspData.DynamicVars[Channel][TCKE]      = 2;
      MrcData->MspData.DynamicVars[Channel][TWTR_L]    = 3;
      break;
    case MMRC_2666:
      MrcData->MspData.DynamicVars[Channel][TRRD_L]    = 3;
      MrcData->MspData.DynamicVars[Channel][TCCD_L_WR] = 3;
      MrcData->MspData.DynamicVars[Channel][TCCD_L_RD] = 3;
      MrcData->MspData.DynamicVars[Channel][TCKE]      = 3;
      MrcData->MspData.DynamicVars[Channel][TXP]       = 5;
      MrcData->MspData.DynamicVars[Channel][TWTR_L]    = 4;
      //
      // Test Bench WA the SPD for 2666 is still TBD state
      //
      MrcData->MspData.DynamicVars[Channel][TRAS]++;
      break;
    default:
      //
      // Safe settings.
      //
      MrcData->MspData.DynamicVars[Channel][TCCD_L_WR] = 3;
      MrcData->MspData.DynamicVars[Channel][TCCD_L_RD] = 3;
      MrcData->MspData.DynamicVars[Channel][TRWSR]     = 5;
      MrcData->MspData.DynamicVars[Channel][TWTR_L]    = 6;
      break;
    }
    MrcData->MspData.DynamicVars[Channel][TWTR_L] += TimingGuardbands[Twrl][MrcData->MrcParameters.DdrFrequency - MINDDR];
    //
    // DEBUG DDR4 Corner case. Rule49 only for DDR4
    //
    if ((MrcData->MspData.DynamicVars[Channel][TRWSR]+2) < MrcData->MspData.DynamicVars[Channel][TCCD_L_RD]) {
      MrcData->MspData.DynamicVars[Channel][TRWSR] = MrcData->MspData.DynamicVars[Channel][TCCD_L_RD] + 2;
    }
    //
    // Relaxed value required by the testbench and used as a relaxed PO setting
    //
    if (MrcData->PowerOn) {
#ifdef SIM
      MrcData->MspData.DynamicVars[Channel][TCCD_L_RD] = 3;
      MrcData->MspData.DynamicVars[Channel][TCCD_L_WR] = 3;
      MrcData->MspData.DynamicVars[Channel][TCCD_WR]   = 0;
      MrcData->MspData.DynamicVars[Channel][TCCD_RD]   = 0;
      MrcData->MspData.DynamicVars[Channel][TWTR_L]    = 6;
      MrcData->MspData.DynamicVars[Channel][TRWSR]     = 5;
      //
      // Different rank
      //
      MrcData->MspData.DynamicVars[Channel][TRRDR]     = 9;  
      MrcData->MspData.DynamicVars[Channel][TWWDR]     = 9;
      MrcData->MspData.DynamicVars[Channel][TRWDR]     = 8;
      MrcData->MspData.DynamicVars[Channel][TWRDR]     = 6;  
      //
      // Different dimm
      //
      MrcData->MspData.DynamicVars[Channel][TRRDD]     = 9;  
      MrcData->MspData.DynamicVars[Channel][TWWDD]     = 9;  
      MrcData->MspData.DynamicVars[Channel][TRWDD]     = 8;  
      MrcData->MspData.DynamicVars[Channel][TWRDD]     = 7;
#endif
    }

    if (MrcData->MspData.DynamicVars[Channel][TWO_CLK_WR_PREAMBLE] == 1) {
      if (MrcData->MspData.DynamicVars[Channel][TWTR_L] < 5) {
        MrcData->MspData.DynamicVars[Channel][TWTR_L] += 2;
      }
    }
    if (MrcData->MspData.DynamicVars[Channel][TWO_CLK_RD_PREAMBLE] == 1) {
      MrcData->MspData.DynamicVars[Channel][TRRD_L] += 1;
    }
    //
    // ADDLAT
    //
    if (MrcData->MrcParameters.DramType[Channel] == TypeDdr4 && MrcData->CommandAddressParity) {
      MspDebugPrint ((MSP_DBG_MIN, "CommandAddressParity: %d\n", MrcData->CommandAddressParity));
      if (MrcData->MspData.DynamicVars[Channel][DIMMTYPE] ==  SPD_DDR_RDIMM) {
        if (MrcData->MrcParameters.DdrFrequency >= MMRC_1600 && MrcData->MrcParameters.DdrFrequency <= MMRC_2400) {
          MrcData->MspData.DynamicVars[Channel][ADDLAT] = 1;
        }
        if (MrcData->MrcParameters.DdrFrequency == MMRC_2666) {
          MrcData->MspData.DynamicVars[Channel][ADDLAT] = 2;
        }
      } else {
        //
        // CA Parity is only enabled for RDIMM, for SODIMM and UDIMM we leave disabled.
        //
        MrcData->MspData.DynamicVars[Channel][ADDLAT] = 0;
      }
      MrcData->MspData.DynamicVars[Channel][RDCMD2RCVEN_LATENCY] += MrcData->MspData.DynamicVars[Channel][ADDLAT];
      MrcData->MspData.DynamicVars[Channel][WRCMD2DQSSTART_LATENCY] += MrcData->MspData.DynamicVars[Channel][ADDLAT];
    } else {
      MrcData->MspData.DynamicVars[Channel][ADDLAT] = 0;
    }

    if (MrcData->MspData.DynamicVars[Channel][DIMMTYPE] ==  SPD_DDR_RDIMM) {
      MrcData->MspData.DynamicVars[Channel][PARITY_DLY] = 1;
    } else {
      MrcData->MspData.DynamicVars[Channel][PARITY_DLY] = 0;
    }
    //
    // From testbench or setup option on DTR5
    //
    if (MrcData->MrcParameters.DdrFrequency > MMRC_1600) {
      MrcData->MspData.DynamicVars[Channel][TIMING_DLY] = 1;
    }
    //
    // CPGC request data size always full cache lines on DNV.
    //
    MrcData->MspData.DynamicVars[Channel][BUS_WIDTH] = 1;
    //
    // 2N mode as default
    //
    MrcData->CurrentMode[Channel] = 2;
    if (MrcData->CommandMode == 0) {
      DetermineCommandMode = 0;
      for (Dimm = 0; Dimm < MAX_DIMMS; Dimm++) {
        if (MrcData->MrcParameters.DimmPresent[Channel][Dimm]) {
          if (MrcData->D_Type[Channel][Dimm] == SPD_DDR_RDIMM && MrcData->MrcParameters.DramType[Channel] == TypeDdr4) {
            //
            // RDIMM will always operate at 1N
            //
            DetermineCommandMode = 1;
          } else {
            //
            // Any other type could operate at 1N or 2N
            //
            DetermineCommandMode = 0;
            break;
          }
        } // MrcData->MrcParameters.DimmPresent[Channel][Dimm]
      } // Dimm loop ...

      if (DetermineCommandMode == 1 || (MrcData->DimmCount[Channel] < 2)) {
        //
        // if all dimms on current channel are RDIMM and DDR4 operate at 1N
        // or if second Dimm is not installed
        //
        if (MrcData->MspData.DynamicVars[Channel][TIMING_1N_SUPPORTED] == 0) {
          MspDebugPrint ((MSP_DBG_MIN, "1N Mode NOT Supported\n"));
        } else {
          //
          // 1N mode
          //
          MrcData->CurrentMode[Channel] = 1;
        }
      }
    } else if (MrcData->CommandMode == 1) {
      if (MrcData->MspData.DynamicVars[Channel][TIMING_1N_SUPPORTED] == 0) {
        MspDebugPrint ((MSP_DBG_MIN, "1N Mode NOT Supported\n"));
      } else {
        //
        // 1N mode
        //
        MrcData->CurrentMode[Channel] = 1;
      }
    }
    MspDebugPrint ((MSP_DBG_MIN, "C[%d] at %dN\n", Channel, MrcData->CurrentMode[Channel]));
    if (MaxNMode < MrcData->CurrentMode[Channel]) {
      MaxNMode = MrcData->CurrentMode[Channel];
    }
    MspDebugPrint ((MSP_DBG_MIN, "Common at %dN\n", MaxNMode));
    //
    // Increase TRRD_S if any of the DIMMs on the channel have 8GB density.
    //
    for (Dimm = 0; Dimm < MAX_DIMMS; Dimm++) {
      if (MrcData->MrcParameters.DimmPresent[Channel][Dimm]) {
        if (MrcData->D_Size[Channel][Dimm] == 2) {
          //
          // Now if we've found an 8Gb density device on DDR3, need to change tFAW as well.
          //
          if (MrcData->MrcParameters.DramType[Channel] <= TypeDdr3All ) {
            MrcData->MspData.DynamicVars[Channel][TRRD_S]++;
            MrcData->MrcParameters.TimingData[Channel][TimingTfaw][FreqA] = mFawTableDdr3[MrcData->MrcParameters.DdrFrequency - MINDDR][1];
          }
        }
        if (MrcData->D_DataWidth[Channel][Dimm] == 1 || MrcData->D_DataWidth[Channel][Dimm] == 2) {
          //
          // Now if we've found an x8 or x16 width device on DDR4, need to change tFAW as well.
          //
          if (MrcData->MrcParameters.DramType[Channel] == TypeDdr4) {
            MrcData->MrcParameters.TimingData[Channel][TimingTfaw][FreqA] = mFawTableDdr4[MrcData->MrcParameters.DdrFrequency - MINDDR][MrcData->D_DataWidth[Channel][Dimm]];
            if (MrcData->D_DataWidth[Channel][Dimm] == 2) {
              //
              // TRRD_S and TRRD_L need to be re-calculated when x16 devices
              // the general assumption is TRRD_S and TRRD_L  are equal to 4nCK ('0')
              // TRRD_S:
              // 1600 : RoundUp(6.0ns/1.250ns) = 5
              // 1866 : RoundUp(5.3ns/1.071ns) = 5
              // 2133 : RoundUp(5.3ns/0.937ns) = 6
              // 2400 : RoundUp(5.3ns/0.833ns) = 7
              // 2666 : RoundUp(5.3ns/0.750ns) = 8
              //
              // TRRD_L:
              //
              // 1600 : RoundUp(7.5ns/1.250ns) = 6
              // 1866 : RoundUp(6.4ns/1.071ns) = 6
              // 2133 : RoundUp(6.4ns/0.937ns) = 7
              // 2400 : RoundUp(6.4ns/0.833ns) = 7
              // 2666 : RoundUp(6.4ns/0.750ns) = 9
              //
              switch (MrcData->MrcParameters.DdrFrequency) {
              case MMRC_1600:
                MrcData->MspData.DynamicVars[Channel][TRRD_S] = 1;
                MrcData->MspData.DynamicVars[Channel][TRRD_L] = 2;
                break;
              case MMRC_1866:
                MrcData->MspData.DynamicVars[Channel][TRRD_S] = 1;
                MrcData->MspData.DynamicVars[Channel][TRRD_L] = 2;
                break;
              case MMRC_2133:
                MrcData->MspData.DynamicVars[Channel][TRRD_S] = 2;
                MrcData->MspData.DynamicVars[Channel][TRRD_L] = 3;
                break;
              case MMRC_2400:
                MrcData->MspData.DynamicVars[Channel][TRRD_S] = 3;
                MrcData->MspData.DynamicVars[Channel][TRRD_L] = 3;
                break;
              case MMRC_2666:
                MrcData->MspData.DynamicVars[Channel][TRRD_S] = 3;
                MrcData->MspData.DynamicVars[Channel][TRRD_L] = 4;
                MspDebugPrint ((MSP_DBG_MIN, "Warning: x16 devices at 2667 may not work\n"));
                break;
              default:
                break;
              }
            }
          }
        }
      }
    }
    MspDebugPrint ((
      MSP_DBG_MIN,
      "C%d TRRD_S: %d TRRD_L: %d\n",
      Channel,
      (UINT32)MrcData->MspData.DynamicVars[Channel][TRRD_S],
      (UINT32)MrcData->MspData.DynamicVars[Channel][TRRD_L]
    ));

    MrcData->MspData.DynamicVars[Channel][TFAW]   = (UINT8)(MrcData->MrcParameters.TimingData[Channel][TimingTfaw][FreqA] - 16);
    //
    // Overrides for non-SPD based values
    //
    if (MrcData->TimingDataOverrides[Channel][TimingTfaw][FreqA] != MMRC_NA) {
      MrcData->MspData.DynamicVars[Channel][TFAW] = MrcData->TimingDataOverrides[Channel][TimingTfaw][FreqA] - 16;
    }
    if (MrcData->TimingDataOverrides[Channel][TimingTrrd][FreqA] != MMRC_NA) {
      MrcData->MspData.DynamicVars[Channel][TRRD_S] = MrcData->MrcParameters.TimingData[Channel][TimingTrrd][FreqA] - 4;
      MrcData->MspData.DynamicVars[Channel][TRRD_L] = MrcData->MrcParameters.TimingData[Channel][TimingTrrd][FreqA] - 4;
    }
    if (MrcData->TimingDataOverrides[Channel][TimingTccd][FreqA] != MMRC_NA) {
      MrcData->MspData.DynamicVars[Channel][TCCD_RD]    = MrcData->MrcParameters.TimingData[Channel][TimingTccd][FreqA] - 4;
      MrcData->MspData.DynamicVars[Channel][TCCD_WR]    = MrcData->MrcParameters.TimingData[Channel][TimingTccd][FreqA] - 4;
      MrcData->MspData.DynamicVars[Channel][TCCD_L_WR]  = MrcData->MrcParameters.TimingData[Channel][TimingTccd][FreqA] - 4 + 2;
      MrcData->MspData.DynamicVars[Channel][TCCD_L_RD]  = MrcData->MrcParameters.TimingData[Channel][TimingTccd][FreqA] - 4 + 2;
    }
    if (MrcData->TimingDataOverrides[Channel][TimingTwcl][FreqA]!= MMRC_NA) {
      MrcData->MspData.DynamicVars[Channel][TWCL] = MrcData->TimingDataOverrides[Channel][TimingTwcl][FreqA] - 7;
    }
    //
    // 2666 WA SPD and Jedec still in TBD
    //
    if (MrcData->MrcParameters.DdrFrequency == MMRC_2666) {
      MrcData->MspData.DynamicVars[Channel][TFAW] -= 4;
    }
    //
    // Change fields that are different for various DDR types.
    //
    if (MrcData->MrcParameters.DramType[Channel] <= TypeDdr3All) {
      MrcData->MspData.DynamicVars[Channel][TCCD_L_WR] = 0;
      MrcData->MspData.DynamicVars[Channel][TCCD_L_RD] = 0;
    }

    //
    // DNV TODO:
    // TRFC is per-dimm on DNV, but we are doing a global TRFC per channel. Need to ask Steve if 
    // this really needs to be unique per-DIMM.
    //
    MrcData->MspData.DynamicVars[Channel][TRFC0MINUS3]  = MrcData->MrcParameters.TimingData[Channel][TimingTrfc][FreqA] - 3;
    MrcData->MspData.DynamicVars[Channel][TRFC1MINUS3]  = MrcData->MrcParameters.TimingData[Channel][TimingTrfc][FreqA] - 3;
    //
    // tXS is based on TRFC and tXS is on Dunit Common so channel 0 dynamic var has to be used 
    //
    MrcData->MspData.DynamicVars[0][TRFC]         = MrcData->MrcParameters.TimingData[Channel][TimingTrfc][FreqA];
    MrcData->MspData.DynamicVars[1][TRFC]         = MrcData->MrcParameters.TimingData[Channel][TimingTrfc][FreqA];

    MrcData->MspData.DynamicVars[Channel][DYN_SR] = MrcData->DynSR;
    MspDebugPrint ((MSP_DBG_MIN, "Channel[%x] Self Refresh = %d\n", Channel, (UINT32) MrcData->MspData.DynamicVars[Channel][DYN_SR]));

    //
    // Setting and calculating dpmc0[sredly] Self Refresh Delay in uS.
    // dpmc0[sredly] = [customer input in us] * DDR Speed (MHz) / 32
    // Customer specifies a latency in microseconds
    // Default value = 30us 
    // 
    // In case of customer selects in BIOS menu higher value than allowed by DDR Speed
    // MRC fixes those ranges as:
    //      - Max us possible at 1600 is 1310 us, rounding down to 1300us
    //      - Max us possible at 1867 is 1123 us, rounding down to 1100us
    //      - Max us possible at 2133 is 983  us, rounding down to 950us
    //      - Max us possible at 2400 is 874  us, rounding down to 850us
    //      - Max us possible at 2667 is 786  us, rounding down to 750us
    // 
    if (MrcData->MrcParameters.SaveRestore.SreDly > mDdrMaxSreDly[(MrcData->MrcParameters.DdrFrequency - MINDDR)]) {
      MrcData->MrcParameters.SaveRestore.SreDly = mDdrMaxSreDly[(MrcData->MrcParameters.DdrFrequency - MINDDR)];
      MspDebugPrint ((MSP_DBG_MIN, "Setting Max DDR Self Refreshed Delay = %dus\n", (MrcData->MrcParameters.SaveRestore.SreDly)));
    }
    MrcData->MspData.DynamicVars[Channel][SR_DELAY] = ((MrcData->MrcParameters.SaveRestore.SreDly * (2000000/(mFrequencyMultiplier[MrcData->MrcParameters.DdrFrequency - MINDDR]))) / 32);
    MspDebugPrint ((MSP_DBG_MIN, "Channel[%x] using %dus (0x%08x) Self Refreshed Delay for DDR ", Channel, MrcData->MrcParameters.SaveRestore.SreDly, (MrcData->MspData.DynamicVars[Channel][SR_DELAY])));
    MspDebugPrint ((MSP_DBG_MIN, "%dMHz\n", (2000000/(mFrequencyMultiplier[MrcData->MrcParameters.DdrFrequency - MINDDR]))));

    //
    // PMOP Levels  (dpmc0[dynpmop])
    //  Slow
    //    - PMOP = 0 for RDIMMs
    //    - PMOP = 6 for SODIMMs
    //  Fast = Default
    //    - PMOP = 0 for RDIMMs
    //    - PMOP = 3 for SODIMMs
    //
    if (MrcData->MspData.DynamicVars[Channel][DIMMTYPE] == SPD_DDR_RDIMM) {
      MrcData->MspData.DynamicVars[Channel][DYN_PMOP] = 0;
    } else if (MrcData->DynPmop == PMOP_FAST_EXIT) {
      MrcData->MspData.DynamicVars[Channel][DYN_PMOP] = 3;
    } else {
      MrcData->MspData.DynamicVars[Channel][DYN_PMOP] = 6;
    }
    MspDebugPrint ((MSP_DBG_MIN, "Channel %x PMOP Level = %d\n", Channel, MrcData->MspData.DynamicVars[Channel][DYN_PMOP]));

    //MrcData->MspData.DynamicVars[Channel][OOO_AGE_THRESHOLD] = MrcData->OutOfOrderAgingThreshold;

    MrcData->MspData.DynamicVars[Channel][TXPDLL_PD] = 0;


/*    MrcData->MspData.DynamicVars[Channel][MEMORY_THERMAL] = MrcData->MemoryThermalEnable;
    MrcData->MspData.DynamicVars[Channel][R2R_SWITCHING] = MrcData->Rk2RkEn;*/
#if LEAKY_BUCKET == 1
    ProgLeakyBucket (MrcData, Channel);
#endif
    ProgPatrolDemandScrub (MrcData, Channel);

    //
    // Set the initial and final address for this channel to be used for ECC init and memtest.
    //
    MrcData->MspData.DynamicVars[Channel][MEM_LOW_ADDRESS]   = 0;
#if SIM
    MrcData->MspData.DynamicVars[Channel][TOTAL_CACHE_LINES] = 4;
#else
    MrcData->MspData.DynamicVars[Channel][TOTAL_CACHE_LINES] = (MrcData->MrcParameters.TotalMem[Channel] << 14) - 1;
#endif


    MrcData->MspData.DynamicVars[Channel][BXOR] = MrcData->BankXorMapEnable;

    if (MrcData->MrcParameters.EccEnabled[Channel] && !MrcData->MspData.DynamicVars[Channel][ECC_SUPPORTED]) {
      MrcData->MrcParameters.EccEnabled[Channel] = 0;
      MspDebugPrint ((MSP_DBG_MIN, "Warning: ECC fuse disabled\n"));
    }
    MspDebugPrint ((MSP_DBG_MIN, "CH %d RK2RK: %d\n", Channel, MrcData->Rk2Rk[Channel]));
    if (MrcData->Rk2Rk[Channel]) {
      MrcData->MspData.DynamicVars[Channel][DISABLE_RK2RK]  = 0;
    } else {
      MrcData->MspData.DynamicVars[Channel][DISABLE_RK2RK]  = 1;
    }
    if (MrcData->MrcParameters.EccEnabled[Channel]) {
      MrcData->MspData.DynamicVars[Channel][ECC_MASK]     = 0xFF;
      MrcData->MspData.DynamicVars[Channel][ECC_ENABLED]  = 1;
      MrcData->MspData.DynamicVars[Channel][ECC_DISABLED] = 0;
      MrcData->MspData.DynamicVars[Channel][UCE_FILTER] = 0x03;
    } else {
      MrcData->MspData.DynamicVars[Channel][ECC_MASK]     = 0;
      MrcData->MspData.DynamicVars[Channel][ECC_ENABLED]  = 0;
      MrcData->MspData.DynamicVars[Channel][ECC_DISABLED] = 1;
      MrcData->MspData.DynamicVars[Channel][UCE_FILTER] = 0;
    }
    SetScrambler (MrcData, Channel);
    SetSlowFastExit (MrcData, Channel);

    InitializeJedecVariables (MrcData, Channel);
#if MITIGATION_FEATURE == 1
    //
    // The mitigation feature will be applied only when using DDR3 Memory Types, there is no 
    // need to add an if statement as this will be controlled by the PFCT in the SS 
    //
    if (MrcData->MitigationProbability != 0) {
      switch (MrcData->MitigationProbability) {
      case 2048:
        MrcData->MspData.DynamicVars[Channel][LFSR_MASK0] = 0xF000;
        MrcData->MspData.DynamicVars[Channel][THRESHOLD0] = 0x0036;
        MrcData->MspData.DynamicVars[Channel][LFSR_MASK1] = 0x00C9;
        MrcData->MspData.DynamicVars[Channel][THRESHOLD1] = 0x0F00;
        break;
      case 4096:
        MrcData->MspData.DynamicVars[Channel][LFSR_MASK0] = 0xE000;
        MrcData->MspData.DynamicVars[Channel][THRESHOLD0] = 0x0034;
        MrcData->MspData.DynamicVars[Channel][LFSR_MASK1] = 0x00C8;
        MrcData->MspData.DynamicVars[Channel][THRESHOLD1] = 0x0E00;
        break;
      case 8192:
        MrcData->MspData.DynamicVars[Channel][LFSR_MASK0] = 0xC000;
        MrcData->MspData.DynamicVars[Channel][THRESHOLD0] = 0x0024;
        MrcData->MspData.DynamicVars[Channel][LFSR_MASK1] = 0x0048;
        MrcData->MspData.DynamicVars[Channel][THRESHOLD1] = 0x0C00;
        break;
      case 16384:
        MrcData->MspData.DynamicVars[Channel][LFSR_MASK0] = 0x8000;
        MrcData->MspData.DynamicVars[Channel][THRESHOLD0] = 0x0004;
        MrcData->MspData.DynamicVars[Channel][LFSR_MASK1] = 0x0400;
        MrcData->MspData.DynamicVars[Channel][THRESHOLD1] = 0x0800;
        break;
      default:
        break;
      }
      MrcData->MspData.DynamicVars[Channel][SEED2]            = 0x1234;
      MrcData->MspData.DynamicVars[Channel][SEED1]            = 0x5678;
      MrcData->MspData.DynamicVars[Channel][RHP_ENABLE]       = 1;
      MrcData->MspData.DynamicVars[Channel][RH_EXIT]          = 12 + MrcData->MrcParameters.TimingData[Channel][TimingTrp][FreqA];
      MrcData->MspData.DynamicVars[Channel][HALF_TRAS]        = MrcData->MrcParameters.TimingData[Channel][TimingTras][FreqA] / 2;
      MrcData->MspData.DynamicVars[Channel][SWIZZLED_DIMM]    = 0;
      for (CurrentDimm = 0; CurrentDimm < MAX_DIMMS; CurrentDimm++) {
        //
        // Skip dimm if not present
        //
        if (!MrcData->MrcParameters.DimmPresent[Channel][CurrentDimm]) {
          continue;
        }
        MspDebugPrint ((MSP_DBG_MAX, "SPD_DDR3_DRAM_MANUFACTURER_ID_HIGH: 0x%04x\n", MrcData->MrcParameters.SpdData[Channel][CurrentDimm].Buffer[SPD_DDR3_DRAM_MANUFACTURER_ID_HIGH]));
        if (MrcData->MrcParameters.SpdData[Channel][CurrentDimm].Buffer[SPD_DDR3_DRAM_MANUFACTURER_ID_HIGH] == SAMSUNG_ID) {
          MrcData->MspData.DynamicVars[Channel][SWIZZLED_DIMM]  |= (UINTX)(1 << CurrentDimm);
        }
      } // CurrentDimm loop ...
    } // MitigationProbability statement ...
#endif // MITIGATION_FEATURE
    for (CurrentDimm = 0; CurrentDimm < MAX_DIMMS; CurrentDimm++) {
      MspDebugPrint ((MSP_DBG_MAX, "C%dD%d: %d:%d\n", Channel, CurrentDimm, SmbusAddressToClttTable[Channel][CurrentDimm][0], SmbusAddressToClttTable[Channel][CurrentDimm][1]));
      if (MrcData->MrcParameters.DimmPresent[Channel][CurrentDimm]) {
        MrcData->MspData.DynamicVars[Channel][SmbusAddressToClttTable[Channel][CurrentDimm][0]] = 1;
        MrcData->MspData.DynamicVars[Channel][SmbusAddressToClttTable[Channel][CurrentDimm][1]] = SmbCltt[Channel][CurrentDimm] >> 1;
      } else {                                                                          
        MrcData->MspData.DynamicVars[Channel][SmbusAddressToClttTable[Channel][CurrentDimm][0]] = 0;
        MrcData->MspData.DynamicVars[Channel][SmbusAddressToClttTable[Channel][CurrentDimm][1]] = 0;
      }
    }
  } // Channel loop ...
  for (Channel = 0; Channel < MAX_CHANNELS; Channel++) {
    if (MrcData->DimmCount[Channel] == 0 || !MrcData->MrcParameters.ChEnabled[Channel]) {
      continue;
    }
    MspDebugPrint ((
      MSP_DBG_MIN,
      "C%d TCCD_RD: %d TCCD_WR: %d TCCD_L_RD: %d TCCD_L_WR: %d \n",
      Channel,
      (UINT32)MrcData->MspData.DynamicVars[Channel][TCCD_RD],
      (UINT32)MrcData->MspData.DynamicVars[Channel][TCCD_WR],
      (UINT32)MrcData->MspData.DynamicVars[Channel][TCCD_L_WR],
      (UINT32)MrcData->MspData.DynamicVars[Channel][TCCD_L_RD]
    ));
    MspDebugPrint ((
      MSP_DBG_MIN,
      "C%d\nTWTR_L: %d\n TRWSR: %d\n TRRDR: %d\n TWWDR: %d\n TRWDR: %d\n TWRDR: %d\n TRRDD: %d\n TWWDD: %d\n TRWDD: %d\n TWRDD: %d\n",
      Channel,
      (UINT32) MrcData->MspData.DynamicVars[Channel][TWTR_L],
      (UINT32) MrcData->MspData.DynamicVars[Channel][TRWSR] ,
      //                                           ,
      // Different rank                            ,
      //                                           ,
      (UINT32) MrcData->MspData.DynamicVars[Channel][TRRDR] ,
      (UINT32) MrcData->MspData.DynamicVars[Channel][TWWDR] ,
      (UINT32) MrcData->MspData.DynamicVars[Channel][TRWDR] ,
      (UINT32) MrcData->MspData.DynamicVars[Channel][TWRDR] ,
      //                                           ,
      // Different dimm                            ,
      //                                           ,
      (UINT32) MrcData->MspData.DynamicVars[Channel][TRRDD] ,
      (UINT32) MrcData->MspData.DynamicVars[Channel][TWWDD] ,
      (UINT32) MrcData->MspData.DynamicVars[Channel][TRWDD] ,
      (UINT32) MrcData->MspData.DynamicVars[Channel][TWRDD]

    ));
    MrcData->MspData.DynamicVars[Channel][DLL_WA_ENABLE] = A0DllWorkAroundEnabled();
    MspDebugPrint((
      MSP_DBG_MIN,
      "C%d %d A0DllWorkAroundEnabled:%d \n",
      Channel, MrcData->MspData.DynamicVars[Channel][DLL_WA_ENABLE]));
    //
    // CPGC STOP_ON_ERR_CONTROL set to 2 (stop on first error) for most of the CPGC use on CPGC_ENABLE sequence
    //
    MrcData->MspData.DynamicVars[Channel][ADVANCED_RD_WR] = 0;
  } // Channel loop ...
#if DDRIO_PHY_INIT == 1
  SetPhyInitDynamicVariables(MrcData);
#endif
  MrcData->CurrentMode[0] = MaxNMode;
  MrcData->CurrentMode[1] = MaxNMode;

  SetMemThermThrotVariables(MrcData);
  SetMemHotVariables(MrcData);
  SetMemTripVariables(MrcData);

  MrcData->MspData.Enabled[0] = TRUE;
  MmrcExecuteTask (MrcData, SmbusAddressToCltt, 0, 0);
  MrcData->MspData.Enabled[0] = BackupEnable;
#if DRAM_RAPL == 1
  if (MrcData->DramRaplParameters.PowerLimitEnable == 1) {
    CalcRaplBandWidthScale(MrcData);
  }
  ProgDramRapl (MrcData);
#endif // DRAM_RAPL
  
  return MMRC_SUCCESS;
}

