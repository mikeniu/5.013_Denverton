/** @file
  Mailbox.c
  This file contain functions for PUnit Mailboxing
 
  Copyright (c) 2005-2015 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation. 

**/

#include "Mailbox.h"
#include "Regs.h"
#include "McFunc.h"

#if PUNIT_MAILBOX

UINT8 mErrorEncodingMailbox[mailbox_cc_maxerror][40] = {
  "mailbox_cc_success                  ",  
  "mailbox_cc_illegal_cmd              ",
  "mailbox_cc_illegal_data             ",
  "mailbox_cc_timeout                  ",
  "mailbox_cc_cmd_locked               ",
  "mailbox_cc_illegal_addr             ",
  "mailbox_cc_illegal_event_busy       ",
  "mailbox_cc_invalid_telem_event_ctl  ",
  "mailbox_cc_resource_demand_exceeded ",
  "mailbox_cc_svid_error               ",
  "mailbox_cc_illegal_vr_id            ",
  "mailbox_cc_vr_interface_locked      ",
  "mailbox_cc_vr_error                 ",
  "mailbox_runbusybit_never_cleared    ",
  "mailbox_cc_illegal_channel          ",
  "mailbox_cc_unknown                  "
};

/**
  This function waits for Punit to be ready

  @param[in, out]  MrcData      Host structure for all data related to MMRC.
  @param[in]       MaxTry       Number of pooling attempts before calling it fail
  @param[in]       Command      Punit Mailbox Command
**/  
MMRC_STATUS
WaitBusy (
  IN  OUT   MMRC_DATA   *MrcData,
  IN        UINT32       MaxTry,
  IN        UINT32       Command
  )
{
  INTERFACE_REGISTER  InterfaceRegister;
  REGISTER_ACCESS     Register;

  Register.ShiftBit = 0;

  if (Command != MBX_CMD_MEM_INIT_DONE) {
    Register.Offset = P_CR_BIOS_MAILBOX_INTERFACE_0_0_0_MCHBAR_PUNITSA_REG;
  } else {
    Register.Offset = P_CR_MC_BIOS_REQ_0_0_0_MCHBAR_PUNITSA_REG;
  }
  Register.Mask = 0xFFFFFFFF;
  
  //
  // Poll the Interface.RunBusy until '0'
  //
  do {
    InterfaceRegister.Data = (UINT32) MemRegRead (PUNITSA, 0, 0, Register);
    MaxTry--;
  } while ((InterfaceRegister.Bits.run_busy == 1) && (MaxTry != 0));

  if (MaxTry == 0) {
    return MMRC_FAILURE;
  }
  return MMRC_SUCCESS;
}

/**
  This function writes data through Punit Mailbox

  @param[in, out]  MrcData      Host structure for all data related to MMRC.
  @param[in]       Command      Punit Mailbox Command
  @param[in]       Address      Punit Mailbox Address
**/  
MMRC_STATUS
WriteDataB2P (
  IN  OUT   MMRC_DATA   *MrcData,
  IN        UINT32       Command,
  IN        UINT32       Address,
  IN        UINT32       Data
  )
{
  INTERFACE_REGISTER  InterfaceRegister;
  MMRC_STATUS         Status;
  REGISTER_ACCESS     Register;
  UINT8               ch;
  P_CR_MC_BIOS_REQ_0_0_0_MCHBAR_PUNITSA_STRUCT  PcrMcBiosreq;
  Register.Mask     = 0xFFFFFFFF;
  Register.ShiftBit = 0;

  MspDebugPrint((MSP_DBG_TST, "Command = %x Address = %x Data = %x\n", Command, Address, Data));

  MspDebugPrint((MSP_DBG_TST, "Polling Busy Bit\n"));
  Status = WaitBusy (MrcData, TRY_MAX_COUNTER, Command);
  if (Status != MMRC_SUCCESS) {
    return mailbox_runbusybit_never_cleared;
  }
  //
  // Write Command, Address and set the RunBusy bit or Vsafe parameters
  //
  switch (Command) {
  case MBX_CMD_MEM_INIT_DONE:
    ch = FindActiveChannel (MrcData);
    if (ch == 0xFF) {
      return mailbox_cc_illegal_channel;
    }
    PcrMcBiosreq.Data = 0;
    PcrMcBiosreq.Bits.ch0_enabled      = MrcData->MrcParameters.ChEnabled[0];
    PcrMcBiosreq.Bits.ch1_enabled      = MrcData->MrcParameters.ChEnabled[1];
    //
    //  ch_dimms is 1 bit field related to ch_enabled if it's enabled then there is 1 or 2 dimms encoded on 1 bit
    //
    PcrMcBiosreq.Bits.ch0_dimms        = (MrcData->DimmCount[0] > 0) ? (MrcData->DimmCount[0] - 1) : 0;
    PcrMcBiosreq.Bits.ch1_dimms        = (MrcData->DimmCount[1] > 0) ? (MrcData->DimmCount[1] - 1) : 0;
    PcrMcBiosreq.Bits.ddr_type_voltage = (MrcData->MrcParameters.DramType[ch] == TypeDdr4) ? 1 : 0;
    PcrMcBiosreq.Bits.mem_init_done    = 1; 
    PcrMcBiosreq.Bits.run_busy         = 1;
    Register.Offset = P_CR_MC_BIOS_REQ_0_0_0_MCHBAR_PUNITSA_REG;
    InterfaceRegister.Data = (UINT32) PcrMcBiosreq.Data;
    break;
  default:
    //
    // Write data to B2PD
    //
    Register.Offset = P_CR_BIOS_MAILBOX_DATA_0_0_0_MCHBAR_PUNITSA_REG;
    MspDebugPrint((MSP_DBG_TST, "Writing Data register %x = %x\n", Register.Offset, Data));
    MemRegWrite(PUNITSA, 0, 0, Register, Data, 0xF);
    InterfaceRegister.Data          = 0;
    InterfaceRegister.Bits.command  = Command;
    InterfaceRegister.Bits.address  = Address;
    InterfaceRegister.Bits.run_busy = 1;
    Register.Offset = P_CR_BIOS_MAILBOX_INTERFACE_0_0_0_MCHBAR_PUNITSA_REG;
    break;
  }
  MspDebugPrint((MSP_DBG_TST, "Writing Interface register %x = %x\n", Register.Offset, InterfaceRegister.Data));
  MemRegWrite (PUNITSA, 0, 0, Register, InterfaceRegister.Data, 0xF);

  McStall (1000);
  Status = WaitBusy (MrcData, TRY_MAX_COUNTER, Command);
  if (Status != MMRC_SUCCESS) {
    return mailbox_runbusybit_never_cleared;
  }
  //
  // Process any errors. BIOS_MAILBOX_INTERFACE returns success or error code, MC_BIOS_REQ does not.
  //
  if (Command != MBX_CMD_MEM_INIT_DONE) {
    InterfaceRegister.Data = (UINT32) MemRegRead (PUNITSA, 0, 0, Register);
  } else {
    InterfaceRegister.Bits.command = 0;
  }
  
  return (MMRC_STATUS)InterfaceRegister.Bits.command;
}

/**
  This function reads data through Punit Mailbox

  @param[in, out]  MrcData      Host structure for all data related to MMRC.
  @param[in]       Command      Punit Mailbox Command
  @param[in]       Address      Punit Mailbox Address
  @param[in]       DataIn       Data In
  @param[out]      DataIn       Data Out
**/  
MMRC_STATUS
ReadDataB2P (
  IN  OUT   MMRC_DATA   *MrcData,
  IN        UINT32       Command,
  IN        UINT32       Address,
  IN        UINT32       DataIn,
      OUT   UINT32       *DataOut
  )
{
  INTERFACE_REGISTER  InterfaceRegister;
  MMRC_STATUS         Status;
  REGISTER_ACCESS     Register;

  Register.ShiftBit = 0;
  Register.Offset   = P_CR_BIOS_MAILBOX_DATA_0_0_0_MCHBAR_PUNITSA_REG;

  Status = WaitBusy (MrcData, TRY_MAX_COUNTER, Command);
  if (Status != MMRC_SUCCESS) {
    return mailbox_runbusybit_never_cleared;
  }
  //
  // Write data to B2PD
  //
  MemRegWrite (PUNITSA, 0, 0, Register, DataIn, 0xF);
  //
  // Write Command, Address and set the RunBusy bit
  //
  InterfaceRegister.Data = 0;
  InterfaceRegister.Bits.command = Command;
  InterfaceRegister.Bits.address = Address;
  InterfaceRegister.Bits.run_busy = 1;
  Register.Offset = P_CR_BIOS_MAILBOX_INTERFACE_0_0_0_MCHBAR_PUNITSA_REG;
  MemRegWrite (PUNITSA, 0, 0, Register, InterfaceRegister.Data, 0xF);
  //
  // Poll the Interface.RunBusy until '0'
  //
  McStall (100);
  Status = WaitBusy (MrcData, TRY_MAX_COUNTER, Command);
  if (Status != MMRC_SUCCESS) {
    return mailbox_runbusybit_never_cleared;
  }
  //
  // Process any errors
  //
  InterfaceRegister.Data = (UINT32) MemRegRead (PUNITSA, 0, 0, Register);
  //
  // Read Data
  //
  Register.Offset = P_CR_BIOS_MAILBOX_DATA_0_0_0_MCHBAR_PUNITSA_REG;
  *DataOut = (UINT32) MemRegRead (PUNITSA, 0, 0, Register);
  
  return (MMRC_STATUS)InterfaceRegister.Bits.command;
}

/**
  This function sends 'MBX_CMD_VDD_RAMP' command to Punit
  which sends VID_OFFSET register (0x33) on the VR13 when bit 31 is set
  the offset is in 5 mv steps around the VDDQ value set by the vsafe

  @param[in, out]  MrcData      Host structure for all data related to MMRC.
  @retval        MMRC_SUCCESS   Dimms are set to desired voltage  
**/ 
MMRC_STATUS
SetVoltageOffset(
  IN  OUT   MMRC_DATA   *MrcData
  )
{
  UINT32 ErrorCode;
  UINT32 Data;
  INT8   VoltageOffset;

  VoltageOffset = 0;
  ErrorCode = MMRC_SUCCESS;

  if (MrcData->MrcParameters.SaveRestore.InputFineDdrVoltage != VDDQ_CENTER) {

    VoltageOffset = ((MrcData->MrcParameters.SaveRestore.InputFineDdrVoltage - VDDQ_CENTER) / VDDQ_GRANULARITY);
    MspDebugPrint((MSP_DBG_MED, "VDDQ voltage (5mV) steps : %d\n", VoltageOffset));
    //
    //  Punit interprets MBX_CMD_VDD_RAMP as VID_OFFSET register (0x33) on the VR13
    //
    Data = (VoltageOffset & 0xff) | BIT31;

    ErrorCode = (UINT32)WriteDataB2P(MrcData, MBX_CMD_VDD_RAMP, DDR_VRID, Data);
    if (ErrorCode >= mailbox_cc_maxerror) {
      ErrorCode = mailbox_cc_unknown;
    }
    MspDebugPrint((MSP_DBG_MIN, "Message code[%d]: %s\n", ErrorCode, mErrorEncodingMailbox[ErrorCode]));
  }

  return ErrorCode;
}



/**
This function sends 'MBX_CMD_WRITE_DDR_BW_SCALE' command to Punit
to set the RAPL calculated value based on dimm population

@param[in, out]  MrcData      Host structure for all data related to MMRC.
@retval        MMRC_SUCCESS   Dimms are set to desired voltage  
**/ 
MMRC_STATUS
SetRaplBandWidthScale(
IN  OUT   MMRC_DATA   *MrcData
)
{
  UINT32 ErrorCode;
  UINT32 Data;

  ErrorCode = MMRC_SUCCESS;

  MspDebugPrint((MSP_DBG_MED, "RAPL Band Width Scale: %d\n", MrcData->DramRaplParameters.BandWidthScale));
  //
  //  MBX_CMD_WRITE_DDR_BW_SCALE to set on the Punit
  //
  Data = MrcData->DramRaplParameters.BandWidthScale;
  if (Data != 0){
    ErrorCode = (UINT32)WriteDataB2P(MrcData, MBX_CMD_WRITE_DDR_BW_SCALE, 0, Data);
    if (ErrorCode >= mailbox_cc_maxerror) {
      ErrorCode = mailbox_cc_unknown;
    }
    MspDebugPrint((MSP_DBG_MIN, "Message code[%d]: %s\n", ErrorCode, mErrorEncodingMailbox[ErrorCode]));
  } else {
    MspDebugPrint((MSP_DBG_MIN, "Warning!!! MrcData->DramRaplParameters.BandWidthScale found to be 0\n"));
  }

  return ErrorCode;
}



/**
  Vsafe flow. Populate the DDR type, channel and dimm population to the Punit.
  Vsafe flow sets the VDD to 1.35 or 1.2V depending on the DRAM type. Also calls
  SetVoltageOffset to margin VDDQ in 5mV steps

  @param[in, out]  MrcData      Host structure for all data related to MMRC.
  @retval       MMRC_SUCCESS    Command sent successfully.
**/ 
MMRC_STATUS
VSafe (
  IN  OUT   MMRC_DATA     *MrcData
  )
{
  UINT32 Status;
  UINT32 Data;
  UINT8  Channel;
  UINT8  DebugBackUp;
#ifdef SIM
  return MMRC_SUCCESS;
#endif

  DebugBackUp = MrcData->MspData.DebugMsgLevel;
  MrcData->MspData.DebugMsgLevel |= MSP_DBG_REG_ACCESS | MSP_DBG_VERBOSE | MSP_DBG_TST;
  Channel = FindActiveChannel(MrcData);
  if (Channel == 0xFF) {
    Status = mailbox_cc_illegal_channel;
    MspDebugPrint((MSP_DBG_MIN, "VSafe message code[%d]: %s\n", Status, mErrorEncodingMailbox[Status]));
    MrcData->MspData.DebugMsgLevel = DebugBackUp;
    return Status;
  }
  if (MrcData->MrcParameters.DramType[Channel] == TypeDdr4) {
    Data = VDDQ_DDR4;
    MspDebugPrint((MSP_DBG_MIN, "VSafe VDDQ_DDR4 \n"));
  } else {
    Data = VDDQ_DDR3L;
    MspDebugPrint((MSP_DBG_MIN, "VSafe VDDQ_DDR3L \n"));
  }
  Status = (UINT32)WriteDataB2P(MrcData, MBX_CMD_VDD_RAMP, DDR_VRID, Data);
  if (Status != MMRC_SUCCESS) {
    if (Status >= mailbox_cc_maxerror) {
      Status = mailbox_cc_unknown;
    }
    MspDebugPrint((MSP_DBG_MIN, "VSafe message code[%d]: %s\n", Status, mErrorEncodingMailbox[Status]));
    MrcData->MspData.DebugMsgLevel = DebugBackUp;
    return Status;
  }
  //
  // Margin VDDQ if required 
  //
  Status = SetVoltageOffset(MrcData);
  MrcData->MspData.DebugMsgLevel = DebugBackUp;
  return Status;
}

/**
  PunitMemInitDone. Signals the Pcode that the MRC has completed. The Pcode then process
  the number of channels and dimms populated for power management operations.

  @param[in, out]  MrcData      Host structure for all data related to MMRC.
  @retval          Status       MMRC_SUCCESS or mailbox_runbusybit_never_cleared
**/
MMRC_STATUS
PunitMemInitDone(
  IN  OUT   MMRC_DATA     *MrcData
)
{
  UINT32 Status;
  Status = MMRC_SUCCESS;
#ifdef SIM
  return Status;
#endif

  Status = (UINT32)WriteDataB2P(MrcData, MBX_CMD_MEM_INIT_DONE, 0, 0);
  //
  //  Vsafe MC_BIOS_REQ pcode command does not return error codes on DNV
  //
  if (Status == mailbox_runbusybit_never_cleared) {
    MspDebugPrint((MSP_DBG_MIN, "PunitMemInitDone message code[%d]: %s\n", Status, mErrorEncodingMailbox[Status]));
  }

  return Status;
}

#endif // PUNIT_MAILBOX
