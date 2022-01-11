/** @file
  MmrcHooks.h
  Header file for all external hooks used by modular MRC.
 
  Copyright (c) 2005-2015 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

**/

#ifndef _MMRC_HOOKS_H_
#define _MMRC_HOOKS_H_

#include "Printf.h"
#include "OemHooks.h"
#include "MmrcData.h"
#include "RegAccess.h"
#if SIM
#include "vpi_user.h"
#include "mrc_wrapper.h"
#endif

#define CHECKPOINT(CP)        (CheckpointSet(CP))

//
// Opcode index in the MrsCommandIndex matrix
//
#define MR0      0
#define MR1      1
#define MR2      2
#define MR3      3
#define MR4      4
#define MR5      5
#define MR6      6
#define RC00     18
#define RC01     19
#define RC02     20
#define RC03     21
#define RC04     22
#define RC05     23
#define RC06     32
#define RC08     24
#define RC09     25
#define RC0A     26
#define RC0B     27
#define RC0C     28
#define RC0D     29
#define RC0E     7
#define RC0F     8
#define RC3X     33
#define RC4X     34
#define RC6X     31

typedef struct {
  UINT16 DqSignal;
  UINT16 DqsSignal;
  UINT8  Skew;
} SKEW_CTL_STRUCT;

MMRC_STATUS
ProgSliceChannelHash (
  IN  OUT MMRC_DATA         *MrcData,
  IN      UINT16            CapsuleStartIndex,
  IN      UINT16            StringIndex, 
  IN      UINT8             Channel
  );

extern
VOID
McStallNanoSecond (
  IN      UINT32    DelayHPET
  );

extern
VOID
McStall (
  IN      UINT32    DelayHPET
  );

UINT32
ConvertLinearToPhysicalValue (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         Channel,
  IN        UINT16        DimIndex,
  IN        UINT32        LinearValue
  );

UINT32
ConvertPhysicalToLinearValue (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         Channel,
  IN        UINT16        DimIndex,
  IN        UINT32        PhysicalValue
  );

MMRC_STATUS
GetSetDimmVref (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         Socket,
  IN        UINT8         Channel,
  IN        UINT8         Dimm,
  IN        UINT8         Rank,
  IN        UINT8         Strobe,
  IN        UINT8         Bit,
  IN        UINT8         FrequencyIndex,
  IN        UINT8         IoLevel,
  IN        UINT8         Cmd,
  IN  OUT   UINT32        *Value
  );

MMRC_STATUS
Value2RxVrefRegs (
  IN        UINT32        Value,
  IN  OUT   UINT32        *RangeSel,
  IN  OUT   UINT32        *Ctrl
  );

MMRC_STATUS
RxVrefRegs2Value (
  IN  OUT   UINT32        *Value,
  IN        UINT32        RangeSel,
  IN        UINT32        Ctrl
  );

MMRC_STATUS
GetSetRxVref (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         Socket,
  IN        UINT8         Channel,
  IN        UINT8         Dimm,
  IN        UINT8         Rank,
  IN        UINT8         Strobe,
  IN        UINT8         Bit,
  IN        UINT8         FrequencyIndex,
  IN        UINT8         IoLevel,
  IN        UINT8         Cmd,
  IN  OUT   UINT32        *Value
  );

MMRC_STATUS
GetSetWrLvlSmp (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         Socket,
  IN        UINT8         Channel,
  IN        UINT8         Dimm,
  IN        UINT8         Rank,
  IN        UINT8         Strobe,
  IN        UINT8         Bit,
  IN        UINT8         FrequencyIndex,
  IN        UINT8         IoLevel,
  IN        UINT8         Cmd,
  IN  OUT   UINT32        *Value
  );

MMRC_STATUS
GetSetTxDqDelayGExt (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         Socket,
  IN        UINT8         Channel,
  IN        UINT8         Dimm,
  IN        UINT8         Rank,
  IN        UINT8         Strobe,
  IN        UINT8         Bit,
  IN        UINT8         FrequencyIndex,
  IN        UINT8         IoLevel,
  IN        UINT8         Cmd,
  IN  OUT   UINT32        *Value
);

MMRC_STATUS
GetSetWrLvlDelay (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         Socket,
  IN        UINT8         Channel,
  IN        UINT8         Dimm,
  IN        UINT8         Rank,
  IN        UINT8         Strobe,
  IN        UINT8         Bit,
  IN        UINT8         FrequencyIndex,
  IN        UINT8         IoLevel,
  IN        UINT8         Cmd,
  IN  OUT   UINT32        *Value
);

MMRC_STATUS
GetSetRxDqsTether (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         Socket,
  IN        UINT8         Channel,
  IN        UINT8         Dimm,
  IN        UINT8         Rank,
  IN        UINT8         Strobe,
  IN        UINT8         Bit,
  IN        UINT8         FrequencyIndex,
  IN        UINT8         IoLevel,
  IN        UINT8         Cmd,
  IN  OUT   UINT32        *Value
);

MMRC_STATUS
GetSetCmdVref (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         Socket,
  IN        UINT8         Channel,
  IN        UINT8         Dimm,
  IN        UINT8         Rank,
  IN        UINT8         Strobe,
  IN        UINT8         Bit,
  IN        UINT8         FrequencyIndex,
  IN        UINT8         IoLevel,
  IN        UINT8         Cmd,
  IN  OUT   UINT32        *Value
  );

MMRC_STATUS
GetSetTxEq (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         Socket,
  IN        UINT8         Channel,
  IN        UINT8         Dimm,
  IN        UINT8         Rank,
  IN        UINT8         Strobe,
  IN        UINT8         Bit,
  IN        UINT8         FrequencyIndex,
  IN        UINT8         IoLevel,
  IN        UINT8         Cmd,
  IN  OUT   UINT32        *Value
  );
MMRC_STATUS
GetSetCtle (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         Socket,
  IN        UINT8         Channel,
  IN        UINT8         Dimm,
  IN        UINT8         Rank,
  IN        UINT8         Strobe,
  IN        UINT8         Bit,
  IN        UINT8         FrequencyIndex,
  IN        UINT8         IoLevel,
  IN        UINT8         Cmd,
  IN  OUT   UINT32        *Value
  );

MMRC_STATUS
GetSetTxSwingB (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         Socket,
  IN        UINT8         Channel,
  IN        UINT8         Dimm,
  IN        UINT8         Rank,
  IN        UINT8         Strobe,
  IN        UINT8         Bit,
  IN        UINT8         FrequencyIndex,
  IN        UINT8         IoLevel,
  IN        UINT8         Cmd,
  IN  OUT   UINT32        *Value
  );
/**
  Overrides the settings for CMD/CTL/CLK

  @param[in, out]  MrcData         Host structure for all data related to MMRC. 
  @param[in]       CapsuleStartIndex 
  @param[in]       StringIndex 
  @param[in]       Channel 
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
CmdOverrideSettings (
  MMRC_DATA         *MrcData,
  UINT16            CapsuleStartIndex,
  UINT16            StringIndex,
  UINT8             Channel
  );

/**
  DisableChannel sends the disabled unused channel to the deepest power management suspend state

@param[in, out]  MrcData         Host structure for all data related to MMRC.
@param[in]       CapsuleStartIndex
@param[in]       StringIndex
@param[in]       Channel
@retval          MMRC_SUCCESS
**/
MMRC_STATUS
DisableChannel (
MMRC_DATA         *MrcData,
UINT16            CapsuleStartIndex,
UINT16            StringIndex,
UINT8             Channel
);

/**
  Enables parity check in the Aunit and Bunit through MSR's

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]   CapsuleStartIndex   Starting point within the CapsuleData structure
                                   where the Init should begin.
  @param[in]       StringIndex     When provided, this is the index into the string
                                   table to show the register names.
  @param[in]       Channel         Current Channel being examined.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
ProgramParityCheck (
  IN  OUT MMRC_DATA         *MrcData,
  IN      UINT16            CapsuleStartIndex,
  IN      UINT16            StringIndex, 
  IN      UINT8             Channel
  );

/**
  Enables Dunit MCI

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]   CapsuleStartIndex   Starting point within the CapsuleData structure
                                   where the Init should begin.
  @param[in]       StringIndex     When provided, this is the index into the string
                                   table to show the register names.
  @param[in]       Channel         Current Channel being examined.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
MciDunit (
  IN  OUT MMRC_DATA         *MrcData,
  IN      UINT16            CapsuleStartIndex,
  IN      UINT16            StringIndex,
  IN      UINT8             Channel
  );

/**
  Performs a test for PPR 
  It forces the sPPR sequence even when the dimms do not support 
  the feature, this will be helpful when verifying waveforms in 
  the SIM-RTL environment.

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Channel         Current Channel being examined. 
  @param[in]       SoftPpr         Indicates if method is sPPR
  @retval          None
**/
VOID 
ExecutePprTestCase (
  IN  OUT MMRC_DATA         *MrcData,
  IN      UINT8             Channel,
  IN      BOOLEAN           SoftPpr
);

/**
  Performs PPR

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]   CapsuleStartIndex   Starting point within the CapsuleData structure
                                   where the Init should begin.
  @param[in]       StringIndex     When provided, this is the index into the string
                                   table to show the register names.
  @param[in]       Channel         Current Channel being examined.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
PostPackageRepairEntry (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT16        CapsuleStartIndex, 
  IN        UINT16        StringIndex, 
  IN        UINT8         Channel
  );


VOID
WriteDimmVref (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         Channel,
  IN        UINT32        vref,
  IN        UINT8         Override
  );

VOID
WriteCmdVref (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         Channel,
  IN        UINT32        vref
);

MMRC_STATUS
VrefOverride (
  IN  OUT MMRC_DATA         *MrcData,
  IN      UINT16            CapsuleStartIndex,
  IN      UINT16            StringIndex, 
  IN      UINT8             Channel
  );

/**
  Sets write VREF prior to start all MRC training to a known value in order to
  avoid failures during early trainings

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Channel         Channel under examination
**/
MMRC_STATUS
EarlySetWriteVref (
  IN  OUT MMRC_DATA         *MrcData,
  IN      UINT16            CapsuleStartIndex,
  IN      UINT16            StringIndex, 
  IN      UINT8             Channel
  );

/**
  Receive enable exit

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Channel         Current Channel being examined.

**/
VOID
ReceiveEnableExit (
  IN    OUT   MMRC_DATA         *MrcData,
  IN          UINT8             Channel
);

/**
  Write Trainig exit

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Channel         Current Channel being examined.

**/
VOID
WriteTrainingExit (
  IN    OUT   MMRC_DATA         *MrcData,
  IN    OUT   UINT8             Channel
);

/**
  Configures a checkpoint to halt the CPU at any point in the MRC code, this
  function is used for debug, and never should be left in the code after debug
  is completed.
  This always halts where the function is placed, in order to continue developer
  must write a '0' into the SSKPD4_DUNIT_REG register

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Channel         Current Channel being examined.
  @param[in]       Name            Pointer with the Name of the Breakpoint.
  @retval          NONE
**/
VOID
BreakpointMrc (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         *Name
  );

/**
  Configures a checkpoint to halt the CPU in a specific memory 
  training step 

  @param[in, out]  MrcData         Host structure for all data related to MMRC. 
  @param[in]       CheckPoint      Specifies the code related to 
                                   an MRC step
  @retval          NONE
**/
VOID
HandleCheckpoint (
  IN        MMRC_DATA *MrcData,
  IN        UINT8      CheckPoint
  );

VOID
HandleAddressMirroring (
  IN  OUT   UINT32 *Data,
  IN        UINT8   AddressMirroringSpdByte,
  IN        UINT8   DramDeviceSpdByte,
  IN        UINT8   DramType, 
  IN        UINT8   DimmType,
  IN        UINT8   Side
  );

/**
  Toggle the MPR mode for an specific rank 

  @param[in, out]  MrcData         Host structure for all data related to MMRC. 
  @param[in]       Channel         Current Channel being examined.
  @param[in]       Rank            Current Rank being examined.
  @param[in]       Toggle          Value for the MRS 3 command.
  @retval          NONE
**/
VOID
ToggleMprMode (
  IN  OUT   MMRC_DATA       *MrcData,
  IN        UINT8           Channel,
  IN        UINT8           Rank,
  IN        UINT8           Toggle
  );

VOID
ToggleMprModeSide (
  IN  OUT   MMRC_DATA       *MrcData,
  IN        UINT8           Channel,
  IN        UINT8           Rank,
  IN        UINT8           Toggle,
  IN        UINT8           Side
  );

VOID
WriteDramCommandSideWrap(
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         Channel,
  IN        UINT8         Rank,
  IN        UINT32        MiscCommand
  );

VOID
WriteDramCommand (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         Channel,
  IN        UINT8         Rank,
  IN        UINT32        MiscCommand,
  IN        UINT8         Side
  );

VOID
MirrorMrsAndAssignToDynamicVars(
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         Channel,
  IN        UINT8         Rank,
  IN        UINT32        MiscCommand,
  IN        UINT16        DynamicVar,
  IN        UINT8         Side
  );

VOID
ConvertMrsDataToMirrored(
IN  OUT   MMRC_DATA     *MrcData,
IN        UINT8         Channel,
IN        UINT8         Rank,
IN  OUT   UINT32        *MiscCommand,
IN        UINT8         Side
  );

UINT32
ReadMrsCommand (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         Channel,
  IN        UINT8         Rank,
  IN        UINT8         Index
  );

VOID
DisablePatrolDemandScrubbing (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         Channel
  );

VOID
RestorePatrolDemandScrubbing (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         Channel
  );

VOID
UpdateSelfRefreshDelay (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         Channel,
  IN        UINTX         NewDelay
  );

/**
  Mirrors address bits

  @param[in]       data         Data for MRS write 
  @param[in]       DramType     Dram Type 
  @retval                       Mirrored data
**/
UINT32
MirrorAddrBits (
  IN    UINT32 Data, 
  IN    UINT8  DramType
  );

/**
  Mirrors bank address bits

  @param[in]       bank         Bank address to be mirrored 
  @retval                       Mirrored bank address
**/
UINT8
MirrorBankBits (
  UINT8 bank
  );

UINT32
GetAddressBit (
  IN  OUT   MMRC_DATA     *MrcData, 
  IN        UINT8         Channel, 
  IN        UINT8         Rank
  );

VOID
CpgcOptionsSetup (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         Channel
  );

/**
  Output a progress data to port 80 for debug purpose
  Could be safely overridden to send checkpoints elsewhere, such
  as port 0x84 or a serial port 

  @param[in]    Content     Hexadecimal value to be sent thru 
                            debug port
  @retval                   NONE
**/
VOID
CheckpointSet (
  IN        UINT16    Content
  );

MMRC_STATUS
MrcDelay (
  IN        UINT8   Type,
  IN        UINT32  Delay
  );

MMRC_STATUS
DumpCpgcRegisters (
  IN  OUT   MMRC_DATA *MrcData,
  IN        UINT8      Channel
  );

MMRC_STATUS
DumpRegs (
  IN  OUT   MMRC_DATA   *MrcData
  );

MMRC_STATUS
DumpDdrioRegisters (
  IN  OUT   MMRC_DATA *MrcData,
  IN        UINT8      Channel
  );

MMRC_STATUS
DumpDunitRegisters (
  IN  OUT   MMRC_DATA *MrcData,
  IN        UINT8      Channel
  );

MMRC_STATUS
DumpBunitRegisters (
  IN  OUT   MMRC_DATA *MrcData
  );

/**
  This routine attempts to acquire the SMBus

  @param[in]       SmbusBase       SMBUS Base Address
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
MrcAcquireBus (
  IN      UINT16    SmbusBase
  );

/**
  This routine reads SysCtl registers

  @param[in]       SmbusBase    SMBUS Base Address
  @param[in]       SlvAddr      Targeted Smbus Slave device address
  @param[in]       Operation    Which SMBus protocol will be used
  @param[in]       Offset       Offset of the register
  @param[in]       Length       Number of bytes
  @param[in]       Buffer       Buffer contains values read from registers
  @retval          MMRC_SUCCESS As Passed
  @retval         !MMRC_SUCCESS As Failed
**/
MMRC_STATUS
MrcSmbusExec (
  IN        UINT16    SmbusBase,
  IN        UINT8     SlvAddr,
  IN        UINT8     Operation,
  IN        UINT16    Offset,
  IN        UINT8     *Length,
  IN        UINT8     *Buffer
  );

MMRC_STATUS
BreakStrobeLoop (
  IN   MMRC_DATA    *MrcData,
  IN   UINT16       Dim1Index,
  IN   UINT8        *Strobe
  );

UINT32
SignalUpperBound (
  IN         MMRC_DATA    *MrcData,
  IN         UINT8        Channel,
  IN         UINT8        Rank,
  IN         UINT16       Dim1Index
  );

UINT32
MirrorAddrBitsPassGate (
  IN          MMRC_DATA     *MrcData,
  IN          UINT8         Channel,
  IN          UINT32        Address
  );

VOID
DelayForXActivates (
  IN          MMRC_DATA     *MrcData,
  IN          UINT8         Channel,
  IN          UINT32        Activations,
  IN          UINT32        *ReadCounter,
  IN          UINT32        *WriteCounter,
  IN          UINT32        *ActivateCounter
  );

/**
  Configures steps prior to execute Pass Gate Test
  Configure the DMAP register according with the Device Density and Data Width

  @param[in, out]  MrcData         Host structure for all data related to MMRC. 
  @param[in, out]  Channel         Current channel examined
  @retval          MMRC_SUCCESS    OEM information set correctly
**/
MMRC_STATUS
PassGateTestEntryHooks (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         Channel  
  );

/**
  Configures steps after execute Pass Gate Test

  @param[in, out]  MrcData         Host structure for all data related to MMRC. 
  @param[in, out]  Channel         Current channel examined
  @retval          MMRC_SUCCESS    
**/
MMRC_STATUS
PassGateTestExitHooks (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         Channel
  );

/**
  This function releases the board's activation of self refresh to the DIMMs

  @param[in, out]  MrcData         Host structure for all data related to MMRC. 
  @retval          MMRC_SUCCESS    OEM information set correctly
**/
MMRC_STATUS
ClearSelfRefreshClamp (
  IN  OUT MMRC_DATA         *MrcData,
  IN      UINT16            CapsuleStartIndex,
  IN      UINT16            StringIndex,
  IN      UINT8             Channel
  );

/**
  RxTxSkewCtl Enables the hook for PMO and memory vebdors to skew Rx or Tx Dq/Dqs training results

@param[in, out]  MrcData         Host structure for all data related to MMRC.
@retval          MMRC_SUCCESS    OEM information set correctly
**/
MMRC_STATUS
RxTxSkewCtl(
  IN  OUT MMRC_DATA         *MrcData,
  IN      UINT16            CapsuleStartIndex,
  IN      UINT16            StringIndex,
  IN      UINT8             Channel
  );

/**
  CA Parity Enable

  @param[in, out]  MrcData             Host structure for all data related to MMRC.
  @param[in]       CapsuleStartIndex   Starting point within the CapsuleData structure where the Init should begin.
  @param[in]       StringIndex         When provided, this is the index into the string table to show the register names.
  @param[in]       Channel             Current Channel being examined.

  @retval          Status              MMRC_SUCCESS
**/
MMRC_STATUS
CaParityEnable(
  IN  OUT MMRC_DATA         *MrcData,
  IN      UINT16            CapsuleStartIndex,
  IN      UINT16            StringIndex,
  IN      UINT8             Channel
);

/**
Rapl enables the time window and band width scale factors

@param[in, out]  MrcData         Host structure for all data related to MMRC.
@retval          MMRC_SUCCESS    OEM information set correctly
**/
MMRC_STATUS
RaplSupport(
IN  OUT MMRC_DATA         *MrcData,
IN      UINT16            CapsuleStartIndex,
IN      UINT16            StringIndex,
IN      UINT8             Channel
);

/**
DnvNibbleTrainingExitHook  switches the training granularity to x4/x8

@param[in, out]  MrcData             Host structure for all data related to MMRC.
@param[in]       CapsuleStartIndex   Starting point within the CapsuleData structure where the Init should begin.
@param[in]       StringIndex         When provided, this is the index into the string table to show the register names.
@param[in]       Channel             Current Channel being examined.
@retval          Status              MMRC_SUCCESS

**/
MMRC_STATUS
DnvNibbleTrainingExitHook (
IN  OUT MMRC_DATA         *MrcData,
IN      UINT16            CapsuleStartIndex,
IN      UINT16            StringIndex,
IN      UINT8             Channel
);


/**
DnvNibbleTrainingHook  switches the training granularity to x4 in case there is a x8 dimm

@param[in, out]  MrcData             Host structure for all data related to MMRC.
@param[in]       CapsuleStartIndex   Starting point within the CapsuleData structure where the Init should begin.
@param[in]       StringIndex         When provided, this is the index into the string table to show the register names.
@param[in]       Channel             Current Channel being examined.
@retval          Status              MMRC_SUCCESS

**/
MMRC_STATUS
DnvNibbleTrainingHook (
IN  OUT MMRC_DATA         *MrcData,
IN      UINT16            CapsuleStartIndex,
IN      UINT16            StringIndex,
IN      UINT8             Channel
);

/**
  This function restores any saved NVDIMM data and prepares for the next save

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @retval          MMRC_SUCCESS    OEM information set correctly
**/
MMRC_STATUS
RestoreArmNvDimms (
  IN  OUT MMRC_DATA         *MrcData,
  IN      UINT16            CapsuleStartIndex,
  IN      UINT16            StringIndex,
  IN      UINT8             Channel
  );


/**
  Wrap RxDqs training result may be different across DDR3/DDR4 HIP implementations

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Channel         Current Channel being examined.
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS 
DisplayOrRestoreTrainingResultsRxDqsWrapper (
  IN  OUT   MMRC_DATA     *MrcData,
  IN        UINT8         Channel,
  IN        BOOLEAN       FaultyPartProcess
  );

/**
  SignalPunitMemInitDone sends the MemInitDone punit mailbox command

  @param[in, out]  MrcData             Host structure for all data related to MMRC.
  @param[in]       CapsuleStartIndex   Starting point within the CapsuleData structure where the Init should begin.
  @param[in]       StringIndex         When provided, this is the index into the string table to show the register names.
  @param[in]       Channel             Current Channel being examined.
  @retval          Status              MMRC_SUCCESS

**/
MMRC_STATUS
SignalPunitMemInitDone(
IN  OUT MMRC_DATA         *MrcData,
IN      UINT16            CapsuleStartIndex,
IN      UINT16            StringIndex,
IN      UINT8             Channel
);

/**
Return the flags required for the Jedec Reset Type for Read Training (basic and advanced)

@retval  x    Bit flags for this training

**/
UINT16
GetJedecResetTypeForRd ();

/**
Return the flags required for the Jedec Reset Type for Write Training (advanced only)

@retval  x    Bit flags for this training

**/
UINT16
GetJedecResetTypeForWr ();

/**
Getx4FloorPlan returns the high logical strobe number that corresponds to the low strobe to support x4 configurations
               and siganls that have byte level granularity
@param[in]       Strobe             Strobe
@retval          Strobe             Strobe map or error

**/
UINT8
Getx4FloorPlan(
IN      UINT8             Strobe
);

/**
Check if ther eis x4 support per project basis

@param[in, out]  MrcData         Host structure for all data related to MMRC.

**/
MMRC_STATUS
CheckX4Support (
IN  OUT MMRC_DATA         *MrcData,
IN      UINT8             *function,
IN      UINT32            line
)
;

/**
Check if the Per bit should be executed per project basis

@param[in, out]  MrcData         Host structure for all data related to MMRC.

**/
MMRC_STATUS
CheckPerBitSupport (
IN  OUT MMRC_DATA         *MrcData,
IN      UINT8             *function,
IN      UINT32            line
)
;

/**
SetPbdLinearEntry on DNV the perbit delays need to be handled as x4 configuration so the Mmrc properlly
uses the right offsets

@param[in]       Strobe             Strobe
@retval          Strobe             Strobe map or error

**/
MMRC_STATUS
SetPbdLinearEntry(
IN  OUT MMRC_DATA         *MrcData
);

/**
SetPbdLinearExit restores the original MaxDq and MaxBit for all channels / ranks

@param[in]       Strobe             Strobe
@retval          Strobe             Strobe map or error

**/
MMRC_STATUS
SetPbdLinearExit(
IN  OUT MMRC_DATA         *MrcData
);

/**
  Indicates if the Dimm under examination should be initialized or not.

  Cases:
  Dimm is V                         - Dimm should be initialized
  Dimm is NV and data is NOT valid  - Dimm should be initialized
  Dimm is NV and data is valid      - Dimm should NOT be initialized

  Note,
    (V) : Volatile
    (NV): Non-Volatile

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Channel         Current Channel being examined.
  @param[in]       Dimm            Current Dimm being examined.

  @retval          TRUE            Only if initialization is required
**/
MMRC_STATUS
IsInitRequired (
  IN  OUT MMRC_DATA         *MrcData,
  IN      UINT8             Channel,
  IN      UINT8             Dimm
  )
;

/**
  Determines the right values for DMAP configuration according with the rank

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Channel         Current Channel being examined.
  @param[in]       Dimm            Current Dimm
  @param[in]       Rank            Current Rank
  @param[out]      RsOffset        Rank Offset
  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
DetermineRankMap (
  IN  OUT MMRC_DATA         *MrcData,
  IN      UINT8             Channel,
  IN      UINT8             Dimm,
  IN      UINT8             Rank,
  IN      UINT8             *RsOffset
)
;

/**
  Checks if ADR flow needs to be executed

  @param[in, out]  MrcData         Host structure for all data related to MMRC.

  @retval          MMRC_SUCCESS
**/
BOOLEAN
IsAdrFlow (
  IN  OUT MMRC_DATA         *MrcData
  );

/**
  Initializes the memory on an specific Dimm

  @param[in, out]  MrcData         Host structure for all data related to MMRC.
  @param[in]       Channel         Current Channel being examined.

  @retval          MMRC_SUCCESS
**/
MMRC_STATUS
MemInitPerDimm (
  IN  OUT MMRC_DATA         *MrcData,
  IN      UINT8             Channel
  )
;
MMRC_STATUS
PdaSequence(
IN  OUT   MMRC_DATA     *MrcData,
IN        UINT8         Socket,
IN        UINT8         Channel,
IN        UINT8         Rank,
IN        UINT8         Strobe,
IN  OUT   UINT32        *Value,
IN        UINT8         Side,
IN        BOOLEAN       PdaEnable
);

/**
SaveMrs  save MRS values from 0 to 6 in EXTBUF
@param[in, out]  MrcData             Host structure for all data related to MMRC.
@retval          Status              MMRC_SUCCESS

**/
MMRC_STATUS
SaveMrs (
IN  OUT MMRC_DATA         *MrcData,
IN      UINT8             Channel
);

/**
SaveRcw  save Rcw values in EXTBUF
@param[in, out]  MrcData             Host structure for all data related to MMRC.
@retval          Status              MMRC_SUCCESS

**/
MMRC_STATUS
SaveRcw (
IN  OUT MMRC_DATA         *MrcData,
IN      UINT8             Channel
);
/**
SetExtDefault  set EXTBUF to default values

@param[in, out]  MrcData             Host structure for all data related to MMRC.
@retval          Status              MMRC_SUCCESS

**/
MMRC_STATUS
SetExtDefault (
IN  OUT MMRC_DATA         *MrcData,
IN      UINT8             Channel
);

/**
JededDllOffToOnFlow  turns a DIMMs DLL on

@param[in, out]  MrcData             Host structure for all data related to MMRC.
@retval          Status              MMRC_SUCCESS

**/
VOID
JedecDllOffToOnFlow (
IN  OUT   MMRC_DATA     *MrcData,
IN        UINT8         Channel,
IN        UINT8         Dimm
);

#endif // _MMRC_HOOKS_H_
