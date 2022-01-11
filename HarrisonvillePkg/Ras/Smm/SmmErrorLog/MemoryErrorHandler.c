/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement

Copyright (c)2009-2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  MemoryErrorHandler.c

Abstract:

  SMM Error Logging.

------------------------------------------------------------------*/
#include "CommonErrorHandlerIncludes.h"

VOID
LogMemError (
  IN  UINT8        Type, 
  IN  UINT8        SubType,
  IN  UINT8        Channel
  )
/*++

Routine Description:

    Creates the memory error record. 

Arguments:


Returns:

--*/
{
  UINT32                CorrErrLog;
  UINT32                UnCorrErrLog;
  UINT32                PhyAddress = 0;
  MEMORY_DEV_INFO		    MemInfo;
  REGISTER_ACCESS       Register;
  // Denverton Aptiov override Start - EIP#249150
  MEM_ERROR_LOG_INFO MemErrorLogInfo;
  
  ZeroMem (&MemErrorLogInfo, sizeof (MEM_ERROR_LOG_INFO)); 
  // Denverton Aptiov override End - EIP#249150

  ZeroMem (&MemInfo, sizeof (MEMORY_DEV_INFO));

  MemInfo.ValidBits = PLATFORM_MEM_NODE_VALID  | PLATFORM_MEM_CARD_VALID | PLATFORM_MEM_ERROR_TYPE_VALID; 

  MemInfo.UefiErrorRecordRevision = GENERIC_ERROR_SECTION_REVISION_UEFI231;

  if (Type == COR_ERROR_TYPE) {
    if (SubType == MEM_ECC_ERROR){
      MemInfo.ErrorType = PLATFORM_MEM_ERROR_SINGLEBIT_ECC;
    } else if (SubType == MEM_PATROL_SCRUB_ERROR) {
      MemInfo.ErrorType = PLATFORM_MEM_ERROR_SCRUB_COR;
    } else {
      MemInfo.ErrorType = PLATFORM_MEM_ERROR_UNKNOWN;
    }
  } else if (Type == FATAL_ERROR_TYPE) {
    if (SubType == MEM_ECC_ERROR) {
      MemInfo.ErrorType = PLATFORM_MEM_ERROR_MLTIBIT_ECC;
    } else if (SubType == MEM_DDR_PARITY_ERROR) {
      MemInfo.ErrorType = PLATFORM_MEM_ERROR_PARITY;
    } else if (SubType == MEM_PATROL_SCRUB_ERROR) {
      MemInfo.ErrorType = PLATFORM_MEM_ERROR_SCRUB_UNCOR; 
    } else {
      MemInfo.ErrorType = PLATFORM_MEM_ERROR_UNKNOWN;
    }
  } else {
    MemInfo.ErrorType = PLATFORM_MEM_ERROR_UNKNOWN;
  }

  MemInfo.Node   = 00; 
  MemInfo.Branch = 00;
  MemInfo.Channel = Channel; 

  if (Type == COR_ERROR_TYPE && SubType == MEM_ECC_ERROR) {
    Register.Offset = R_SBELOG;
    Register.Mask = 0xFFFFFFFF;
    Register.ShiftBit = 0;
    CorrErrLog = (UINT32)MemRegRead(DUNIT, Channel, 0, Register);
    MemInfo.Bank = (CorrErrLog & B_SBEBNK)>>16;
    MemInfo.Rank = (CorrErrLog & B_SBERNK)>>20;

    Register.Offset = R_SBEADDR;
    Register.Mask = 0xFFFFFFFF;
    Register.ShiftBit = 0;
    PhyAddress = (UINT32)MemRegRead(DUNIT, Channel, 0, Register);

    MemInfo.PhyAddr = (UINT64) (PhyAddress << 6);
    MemInfo.ValidBits |= PLATFORM_MEM_PHY_ADDRESS_VALID | PLATFORM_MEM_BANK_VALID | PLATFORM_RANK_NUMBER_VALID;
  }

  if (Type == FATAL_ERROR_TYPE && SubType == MEM_ECC_ERROR) {
    Register.Offset = R_UCELOG;
    Register.Mask = 0xFFFFFFFF;
    Register.ShiftBit = 0;
    UnCorrErrLog = (UINT32)MemRegRead(DUNIT, Channel, 0, Register);
    MemInfo.Bank = (UnCorrErrLog & B_UCEBNK)>>16;
    MemInfo.Rank = (UnCorrErrLog & B_UCERNK)>>20;

    Register.Offset = R_UCEADDR;
    Register.Mask = 0xFFFFFFFF;
    Register.ShiftBit = 0;
    PhyAddress = (UINT32)MemRegRead(DUNIT, Channel, 0, Register);
    MemInfo.PhyAddr = (UINT64) (PhyAddress << 6);
    MemInfo.ValidBits |= PLATFORM_MEM_PHY_ADDRESS_VALID | PLATFORM_MEM_BANK_VALID | PLATFORM_RANK_NUMBER_VALID;

    mUCEccDetected = TRUE;
  }

  //
  //Rank 0 and 2 From DIMM 0
  //Rank 1 and 3 From DIMM 1
  //
  if (SubType == MEM_ECC_ERROR) {
    MemInfo.Dimm = MemInfo.Rank & 0x01;
    MemInfo.ValidBits |= PLATFORM_MEM_MODULE_VALID;
  }

  // Denverton Aptiov override Start - EIP#249150
  
  MemErrorLogInfo.Source = MemSource;
  MemErrorLogInfo.Type = Type;
  MemErrorLogInfo.MemInfo = &MemInfo;
  
  //
  // Logging Error using Runtime Error Logging Listener
  //
  CommonErrorHandling((VOID *) &MemErrorLogInfo);
  
  /*
  if (mWheaPlatformSupportAvailable && mErrLogSetupData.WheaLogging) {
    if(Type == COR_ERROR_TYPE) {
      // prepare the error status block in HEST table.
      mWheaPlatformSupport->WheaPlatformElogMemory (
                                        mWheaPlatformSupport,
                                        GEN_ERR_SEV_PLATFORM_MEMORY_CORRECTED,
                                        &MemInfo
                                        );
    }

    if(Type == FATAL_ERROR_TYPE) {
      // prepare the error status block in HEST table.
      mWheaPlatformSupport->WheaPlatformElogMemory (
                                        mWheaPlatformSupport,
                                        GEN_ERR_SEV_PLATFORM_MEMORY_FATAL,
                                        &MemInfo
                                        );
    }
  */
  // Denverton Aptiov override End - EIP#249150
}


VOID
EnableElogDUnit(
	)
/*++

Routine Description:

  This function Enables or disables the Memory errors.

Returns:

  None

--*/
{
  UINT32                          DErrMskSev;
  REGISTER_ACCESS                 Register;
  UINT8                           ch;

  Register.Offset = R_DERRMSKSEV;
  Register.Mask = 0xFFFFFFFF;
  Register.ShiftBit = 0;

  for(ch=DUNIT_CH0; ch < MAX_CHANNELS; ch++) {

    DErrMskSev = (UINT32)MemRegRead(DUNIT, ch, 0, Register);

    //clear All mask
    DErrMskSev &= ~(B_MSK_SBE | B_MSK_UCE | B_MSK_WDBW | B_MSK_APE | B_MSK_UCEPAT | 
                    B_MSK_APE | B_MSK_UCEPAT);

    if ((mErrLogSetupData.SystemErrorEn != ENABLED) || (!mErrLogSetupData.MemoryErrorLog)) {
        //
        //Disable DUNIT memory error reporting.
        //
        DErrMskSev |= (B_MSK_SBE | B_MSK_UCE | B_MSK_WDBW | B_MSK_APE | B_MSK_UCEPAT | 
                       B_MSK_APE | B_MSK_UCEPAT);
    } else {
      //
      //Enable error reporting based on setup
      //
      if (!mErrLogSetupData.MemoryCorrErrorLog)
        DErrMskSev |= (B_MSK_SBE | B_MSK_UCE | B_MSK_WDBW | B_MSK_APE | B_MSK_UCEPAT);

      if (!mErrLogSetupData.MemoryUnCorrErrorLog)
        DErrMskSev |= ( B_MSK_APE | B_MSK_UCEPAT);
    }
  
    MemRegWrite(DUNIT, ch, 0, Register, DErrMskSev, 0xF);
  }
}

BOOLEAN
ElogMemoryErrorHandler 
(
  )
/*++

Routine Description:
    Memory SMI handler to handle Memory errors

Arguments:
    None

Returns:
    Error detected status

--*/ 
{
  BOOLEAN                         ErrorDetected = FALSE;
  UINT32                          DErrSts;
  REGISTER_ACCESS                 Register;
  UINT8                           ch;

  Register.Offset = R_DERRSTS;
  Register.Mask = 0xFFFFFFFF;
  Register.ShiftBit = 0;

  for(ch=DUNIT_CH0; ch < MAX_CHANNELS; ch++) {
    DErrSts = (UINT32)MemRegRead (DUNIT, ch, 0, Register);
    
    //
    //Correctable Single BIT ECC error
    //
    if (DErrSts & B_SBE) {
      LogMemError (COR_ERROR_TYPE, MEM_ECC_ERROR, ch);
      ErrorDetected = TRUE;
    }

    //
    //Un Correctable Multi BIT ECC error
    //
    if (DErrSts & B_UCE) {
      LogMemError (FATAL_ERROR_TYPE, MEM_ECC_ERROR, ch);
      ErrorDetected = TRUE;
    }

    //
    //Un Correctable Write Parity and DDR4 command/Address parity error
    //
    if ((DErrSts & B_WPE) || (DErrSts & B_APE)) {
      LogMemError (FATAL_ERROR_TYPE, MEM_DDR_PARITY_ERROR, ch);
      ErrorDetected = TRUE;
    }

    //
    //UnCorrectable Multi bit Patrol Scrub error
    //
    if (DErrSts & B_UCEPAT) {
      LogMemError (FATAL_ERROR_TYPE, MEM_PATROL_SCRUB_ERROR, 0);
      ErrorDetected = TRUE;
    }
  }
  
  return ErrorDetected;
}

VOID
ClearDUnitErrors(
  )
/*++

Routine Description:

  Clear Dunit errors before enabling them.

Arguments:
  
  None 

Returns:

  None

--*/
{  

  UINT32                          DErrSts;
  REGISTER_ACCESS                 Register;
  UINT8                           ch;

  Register.Offset = R_DERRSTS;
  Register.Mask = 0xFFFFFFFF;
  Register.ShiftBit = 0;

  for(ch=DUNIT_CH0; ch < MAX_CHANNELS; ch++) {
    DErrSts = (UINT32)MemRegRead (DUNIT, ch, 0, Register);
    MemRegWrite(DUNIT, ch, 0, Register, DErrSts, 0xF);
  }
}
