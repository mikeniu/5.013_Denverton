/** @file  
  Mailbox.h
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

#ifndef _MAILBOX_H_
#define _MAILBOX_H_

#include "Mrc.h"
#include "RegAccess.h"
#include "McFunc.h"
#include "Printf.h"

#if JTAG
#define TRY_MAX_COUNTER   0x100 // IDC Original Value 0x01
#else
#define TRY_MAX_COUNTER   0x100
#endif
//
// VID for DDR and VDDQ values for the VR13 Pcode will directly pass this values to the VR
// DDR4 is 1.2V and DDR3L is 1.35V the VR13 encoded values are on CDI 544905, 456098 and 543985
//
#define DDR_VRID         3
#define VDDQ_DDR4     0xBF
#define VDDQ_DDR3L    0xDD
//
// VDDQ Granularity would be 5mV per bit
//
#define VDDQ_GRANULARITY  5
//
// As the max delta that can be done is +-100mV
// We set it as out Center in order to start
// varying from that specifc point
//
#define VDDQ_CENTER       100
// 
// Commands
//
#define MBX_CMD_ZERO                                0
#define MBX_CMD_VR_INTERFACE                        1
#define MBX_CMD_READ_PCS                            2
#define MBX_CMD_WRITE_PCS                           3
#define MBX_CMD_READ_PCU_MISC_CONFIG                4
#define MBX_CMD_WRITE_PCU_MISC_CONFIG               5
#define MBX_CMD_READ_PL1_DUTY_CYCLE_SETTINGS        6
#define MBX_CMD_WRITE_PL1_DUTY_CYCLE_SETTINGS       7
#define MBX_CMD_VDD_RAMP                            8
#define MBX_CMD_MEM_INIT_DONE                       9
#define MBX_CMD_READ_DDR_BW_SCALE                  34
#define MBX_CMD_WRITE_DDR_BW_SCALE                 35
#define MBX_CMD_WRITE_DDR_REFRESH_MAILBOX          55

// 
// BIOS mailbox Completion Code encodings
//
typedef enum {
  mailbox_cc_success = 0,
  mailbox_cc_illegal_cmd,
  mailbox_cc_illegal_data,
  mailbox_cc_timeout,
  mailbox_cc_cmd_locked,
  mailbox_cc_illegal_addr,
  mailbox_cc_illegal_event_busy,
  mailbox_cc_invalid_telem_event_ctl,
  mailbox_cc_resource_demand_exceeded,
  mailbox_cc_svid_error,
  mailbox_cc_illegal_vr_id,
  mailbox_cc_vr_interface_locked,
  mailbox_cc_vr_error,
  mailbox_runbusybit_never_cleared,
  mailbox_cc_illegal_channel,
  mailbox_cc_unknown,
  mailbox_cc_maxerror
} MBX_BIOS_ERROR_ENUM;


#define INTERFACE_REGISTER P_CR_BIOS_MAILBOX_INTERFACE_0_0_0_MCHBAR_PUNITSA_STRUCT

/**
  This function sends 'MBX_CMD_VDD_RAMP' command to Punit
  which sends VID_OFFSET register (0x33) on the VR13 when bit 31 is set
  the offset is in 5 mv steps around the VDDQ value set by the vsafe

  @param[in, out]  MrcData      Host structure for all data related to MMRC.
  @retval        MMRC_SUCCESS   Dimms are set to desired voltage  
**/ 
MMRC_STATUS
SetVoltageOffset (
  IN  OUT   MMRC_DATA   *MrcData
  );

/**
This function sends 'MBX_CMD_WRITE_DDR_BW_SCALE' command to Punit
to set the RAPL calculated value based on dimm population

@param[in, out]  MrcData      Host structure for all data related to MMRC.
@retval        MMRC_SUCCESS   Dimms are set to desired voltage
**/
MMRC_STATUS
SetRaplBandWidthScale(
IN  OUT   MMRC_DATA   *MrcData
);

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
  );
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
  );
/**
PunitMemInitDone. Signals the Pcode that the MRC has completed. The Pcode then process
the number of channels and dimms populated for power management operations.

@param[in, out]  MrcData             Host structure for all data related to MMRC.
@param[in]       CapsuleStartIndex   Starting point within the CapsuleData structure where the Init should begin.
@param[in]       StringIndex         When provided, this is the index into the string table to show the register names.
@param[in]       Channel             Current Channel being examined.
@retval          Status              MMRC_SUCCESS or mailbox_runbusybit_never_cleared
**/
MMRC_STATUS
PunitMemInitDone(
  IN  OUT   MMRC_DATA     *MrcData
);

#endif
