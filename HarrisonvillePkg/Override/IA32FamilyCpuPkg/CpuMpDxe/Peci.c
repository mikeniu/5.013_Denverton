/*++
  This file contains an 'Intel Peripheral Driver' and is        
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/
/** @file

  Code for PECI Feature

  Copyright (c) 2006 - 2016, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  Module Name:  Peci.c

**/

#include "Peci.h"

BOOLEAN    mEnablePeci = FALSE;
BOOLEAN    mEnablePeciDownstreamWrite = FALSE;

/**
  Detect capability of PECI feature for specified processor.
  
  This function detects capability of PECI feature for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.

**/
VOID
PeciDetect (
  UINTN   ProcessorNumber
  )
{
  UINT32    FamilyId;
  UINT32    ModelId;
  UINT32    SteppingId;
  UINT64    MsrValue;

  GetProcessorVersionInfo (ProcessorNumber, &FamilyId, &ModelId, &SteppingId, NULL);

  if (FamilyId == 0x06) {
    if (ModelId == 0x0F) {
      MsrValue = AsmReadMsr64 (MSR_EXT_CONFIG);
      if (SteppingId >= 0x02 && BitFieldRead64 (MsrValue, 22, 22) != 0) {
        SetProcessorFeatureCapability (ProcessorNumber, Peci, NULL);
      }
    } else if (ModelId == 0x17 || ModelId == 0x16) {
      MsrValue = AsmReadMsr64 (MSR_EXT_CONFIG);
      if (BitFieldRead64 (MsrValue, 22, 22) != 0) {
        SetProcessorFeatureCapability (ProcessorNumber, Peci, NULL);
      }
    } else if (IS_NEHALEM_PROC(FamilyId, ModelId)) {
      SetProcessorFeatureCapability (ProcessorNumber, Peci, NULL);
    }
  }
}

/**
  Configures Processor Feature Lists for PECI feature for all processors.
  
  This function configures Processor Feature Lists for PECI feature for all processors.

**/
VOID
PeciConfigFeatureList (
  VOID
  )
{
  UINTN                 ProcessorNumber;
  EFI_STATUS            Status;

  if ((PcdGet32 (PcdCpuProcessorFeatureUserConfiguration) & PCD_CPU_PECI_BIT) != 0) {
    mEnablePeci = TRUE;
  }

  for (ProcessorNumber = 0; ProcessorNumber < mCpuConfigContextBuffer.NumberOfProcessors; ProcessorNumber++) {
    //
    // Check whether this logical processor supports PECI
    //
    if (GetProcessorFeatureCapability (ProcessorNumber, Peci, NULL)) {
      if (ProcessorNumber == mCpuConfigContextBuffer.BspNumber) {
        //
        // Set the bit of PECI capability according to BSP capability.
        //
        Status = PcdSet32S (PcdCpuProcessorFeatureCapability, PcdGet32 (PcdCpuProcessorFeatureCapability) | PCD_CPU_PECI_BIT);
        ASSERT_EFI_ERROR (Status);
        //
        // Set the bit of PECI setting according to BSP setting.
        //
        if (mEnablePeci) {
          Status = PcdSet32S (PcdCpuProcessorFeatureSetting, PcdGet32 (PcdCpuProcessorFeatureSetting) | PCD_CPU_PECI_BIT);
          ASSERT_EFI_ERROR (Status);
        }
      }

      //
      // If this logical processor supports PECI, then add feature into feature list for operation
      // on corresponding register.
      //
      AppendProcessorFeatureIntoList (ProcessorNumber, Peci, &mEnablePeci);
    }
  }
}

/**
  Produces entry of PECI feature in Register Table for specified processor.
  
  This function produces entry of PECI feature in Register Table for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.
  @param  Attribute         Pointer to the attribute

**/
VOID
PeciReg (
  UINTN      ProcessorNumber,
  VOID       *Attribute
  )
{
  BOOLEAN   *Enable;
  UINT64    Value;
  UINT32    FamilyId;
  UINT32    ModelId;

  GetProcessorVersionInfo (ProcessorNumber, &FamilyId, &ModelId, NULL, NULL);
  
  //
  // If Attribute is TRUE, then write 1 to MSR_PECI_CONTROL[0].
  // Otherwise, write 0 to the bit.
  //
  Enable = (BOOLEAN *) Attribute;
  Value  = 0;
  if (*Enable) {
    Value = 1;
  }

  if (!IS_NEHALEM_PROC(FamilyId, ModelId)) {
    WriteRegisterTable (ProcessorNumber, Msr, MSR_PECI_CONTROL, 0, 1, Value);
  }
}

/**
  Detect capability of PECI Downstream for specified processor.

  This function detects capability of PECI Downstream for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.

**/
VOID
PeciDownstreamDetect (
  UINTN   ProcessorNumber
  )
{
  UINT32    FamilyId;
  UINT32    ModelId;
  UINT32    SteppingId;

  GetProcessorVersionInfo (ProcessorNumber, &FamilyId, &ModelId, &SteppingId, NULL);

  if ((FeaturePcdGet (PcdCpuKnightsLandingFamilyFlag) /* && IS_KNIGHTSLANDING_PROC (FamilyId, ModelId)*/ )
      ) {
    SetProcessorFeatureCapability (ProcessorNumber, PeciDownstreamWrite, NULL);
  }
}

/**
  Configures Processor Feature Lists for PECI Downstream for all processors.

  This function configures Processor Feature Lists for PECI Downstream for all processors.

**/
VOID
PeciDownstreamFeatureList (
  VOID
  )
{
  UINTN                 ProcessorNumber;
  EFI_STATUS            Status;

  if ((PcdGet32 (PcdCpuProcessorFeatureUserConfigurationEx1) & PCD_CPU_PECI_DOWNSTREAM_WRITE_BIT) != 0) {
    mEnablePeciDownstreamWrite = TRUE;
  }

  for (ProcessorNumber = 0; ProcessorNumber < mCpuConfigContextBuffer.NumberOfProcessors; ProcessorNumber++) {
    //
    // Check whether this logical processor supports PeciDownstreamWrite
    //
    if (GetProcessorFeatureCapability (ProcessorNumber, PeciDownstreamWrite, NULL)) {
      if (ProcessorNumber == mCpuConfigContextBuffer.BspNumber) {
        //
        // Set the bit of PeciDownstreamWrite capability according to BSP capability.
        //
        Status = PcdSet32S (PcdCpuProcessorFeatureCapabilityEx1, PcdGet32 (PcdCpuProcessorFeatureCapabilityEx1) | PCD_CPU_PECI_DOWNSTREAM_WRITE_BIT);
        ASSERT_EFI_ERROR (Status);
        //
        // Set the bit of PeciDownstreamWrite setting according to BSP setting.
        //
        if (mEnablePeciDownstreamWrite) {
          Status = PcdSet32S (PcdCpuProcessorFeatureSettingEx1, PcdGet32 (PcdCpuProcessorFeatureSettingEx1) | PCD_CPU_PECI_DOWNSTREAM_WRITE_BIT);
          ASSERT_EFI_ERROR (Status);
        }
      }

      //
      // If this logical processor supports PeciDownstreamWrite, then add feature into feature list for operation
      // on corresponding register.
      //
      AppendProcessorFeatureIntoList (ProcessorNumber, PeciDownstreamWrite, &mEnablePeciDownstreamWrite);
    }
  }
}

/**
  Produces entry of PECI Downstream feature in Register Table for specified processor.

  This function produces entry of PECI Downstream feature in Register Table for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.
  @param  Attribute         Pointer to the attribute

**/
VOID
PeciDownstreamReg (
  UINTN      ProcessorNumber,
  VOID       *Attribute
  )
{
  BOOLEAN   *Enable;
  UINT64    Value;

  //
  // If Attribute is TRUE, then write 1 to MSR_PECI_DNSTRM_WRITE_EN[0].
  // Otherwise, write 0 to the bit.
  //
  Enable = (BOOLEAN *) Attribute;
  Value  = 0;
  if (*Enable) {
    Value = 1;
  }

  WriteRegisterTable (ProcessorNumber,
                      Msr,
                      MSR_PECI_DNSTRM_WRITE_EN,
                      0,
                      1,
                      Value);
}
