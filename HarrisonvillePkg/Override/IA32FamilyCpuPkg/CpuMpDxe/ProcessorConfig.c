/*++
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
--*/
/** @file
  Code for processor configuration.

  Copyright (c) 2006 - 2016, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  Module Name:  ProcessorConfig.c

**/

#include "MpService.h"
#include "Cpu.h"
#include "Microcode.h"
#include "X2Apic.h"
#include "MchkInit.h"
#include "ThermalManagement.h"
#include "CState.h"
#include "Vt.h"
#include "Misc.h"
// OVERRIDE_4757982_Begin
  #include "SetupVariable.h"
  #include "Guid/SetupVariable.h"
// OVERRIDE_4757982_End

// Denverton AptioV Override Start - EIP#360629
#include <Protocol/Timer.h>
#include <Protocol/Cpu.h>
// Denverton AptioV Override End - EIP#360629

MP_SYSTEM_DATA                      mMPSystemData;
CPU_CONFIG_CONTEXT_BUFFER           mCpuConfigContextBuffer;
EFI_PHYSICAL_ADDRESS                mStartupVector;
UINT8                               mPlatformType;
ACPI_CPU_DATA                       *mAcpiCpuData;
EFI_HANDLE                          mHandle = NULL;
MTRR_SETTINGS                       *mMtrrSettings;
EFI_EVENT                           mSmmConfigurationNotificationEvent;
EFI_HANDLE                          mImageHandle;
EFI_TIMER_ARCH_PROTOCOL             *mTimer;
UINTN                               mLocalApicTimerDivisor;
UINT32                              mLocalApicTimerInitialCount;
//OVERRIDE_4757168_BEGINS
STATIC UINT64                       *RtitMemoryBlocks;
STATIC UINT64                        *TopaMemArray;
UINT64                              MemRegionSize;
BOOLEAN                             ProcTraceSupport = FALSE;
BOOLEAN                             IsTopaSupported = FALSE;
BOOLEAN                             IsSingleRangeSupported = FALSE;

//VOID                                *mMicrocodeBaseAddress;

/**
  Detect if Processor Trace Feature is supported or not

  @retval PROC_TRACE_SUPPORT if supported or 0 if not supported
**/
BOOLEAN
IsProcTraceSupported (
  VOID
  )
{
  EFI_CPUID_REGISTER CpuidRegisters;

  AsmCpuidEx (
          CPUID_FUNCTION_7,
          0,
          &CpuidRegisters.RegEax,
          &CpuidRegisters.RegEbx,
          &CpuidRegisters.RegEcx,
          &CpuidRegisters.RegEdx
          );
  if ((CpuidRegisters.RegEbx & B_CPUID_FUNCTION7_EBX_RTIT_SUPPORT) != 0) {
    return TRUE;
  }
  return FALSE;
};


/**
  Detect which PT output schemes are supported.

  @retval BIT0 Indicates support for ToPA,
  @retval BIT2 Indicates support for Single Range Output.

**/
UINT8
ProcTraceSchemesSupported (
  VOID
  )
{
  EFI_CPUID_REGISTER  CpuidRegisters;

  AsmCpuidEx (
          CPUID_FUNCTION_20,
          0,
          &CpuidRegisters.RegEax,
          &CpuidRegisters.RegEbx,
          &CpuidRegisters.RegEcx,
          &CpuidRegisters.RegEdx
          );

  return ((UINT8) CpuidRegisters.RegEcx);
};

// RTIT_BASE_MSR(LP) =  RTIT_BAR * 64 * LP * CLM
//  LP Processor # ; 64 Cache line size; CLM = 2 for Denverton (Cache Lines Per Master)
//  Total RTIT size is 2048b on a 16 CPU system
//OVERRIDE_4757168_ENDS
/**
  Prepares memory region for processor configuration.
  
  This function prepares memory region for processor configuration.

**/
VOID
PrepareMemoryForConfiguration (
  VOID
  )
{
  UINTN                NumberOfProcessors;
  UINTN                Index;
  MONITOR_MWAIT_DATA   *MonitorData;
// OVERRIDE_4757168_BEGINS
  UINTN                RtitBase = PcdGet32(PcdCpuRtitTraceHubBar);
  UINTN                AlignedAddress;
  UINTN                Pages;
  UINTN                Alignment;
  UINT8                SupportedSchemes;
// OVERRIDE_4757168_ENDS
  UINTN                MaxNumberOfCpus;
  EFI_STATUS           Status;

  NumberOfProcessors = mCpuConfigContextBuffer.NumberOfProcessors;

  //
  // Allocate buffer for arrays in MP_SYSTEM_DATA
  //
  mMPSystemData.CpuList = (BOOLEAN *)AllocateZeroPool (
                                       sizeof (BOOLEAN) * NumberOfProcessors
                                       );
  ASSERT (mMPSystemData.CpuList != NULL);

  mMPSystemData.CpuData = (CPU_DATA_BLOCK *)AllocateZeroPool (
                                              sizeof (CPU_DATA_BLOCK) * NumberOfProcessors
                                              );
  ASSERT (mMPSystemData.CpuData != NULL);

  mMPSystemData.DisableCause = (EFI_CPU_STATE_CHANGE_CAUSE *)AllocateZeroPool (
                                                               sizeof (EFI_CPU_STATE_CHANGE_CAUSE) * NumberOfProcessors
                                                               );
  ASSERT (mMPSystemData.DisableCause != NULL);

  mMPSystemData.CpuHealthy = (BOOLEAN *)AllocateZeroPool (
                                          sizeof (BOOLEAN) * NumberOfProcessors
                                          );
  ASSERT (mMPSystemData.CpuHealthy != NULL);

// OVERRIDE_4757168_BEGINS  
  MemRegionSize = (UINT32) (1 << (PcdGet32 (PcdCpuRtitSize) + 12));
// OVERRIDE_4757168_ENDS

  //
  // Initialize Spin Locks for system
  //
  InitializeSpinLock (&mMPSystemData.APSerializeLock);
  InitializeSpinLock (&mMPSystemData.MsrLock);
  for (Index = 0; Index < NumberOfProcessors; Index++) {
    InitializeSpinLock (&mMPSystemData.CpuData[Index].CpuDataLock);
  }

  //
  // Claim memory for AP stack. It will be used for both POST path and S3 path.
  // If support CPU hot plug, we need to allocate stack for possibly hot-added processors on S3 path.
  //
  if (FeaturePcdGet (PcdCpuHotPlugSupport)) {
    MaxNumberOfCpus = PcdGet32 (PcdCpuMaxLogicalProcessorNumber);
  } else {
    MaxNumberOfCpus = NumberOfProcessors;
  }
  mExchangeInfo->StackStart = AllocateAcpiNvsMemoryBelow4G (MaxNumberOfCpus * PcdGet32 (PcdCpuApStackSize));
  mExchangeInfo->StackSize  = PcdGet32 (PcdCpuApStackSize);

  //
  // Initialize the Monitor Data structure in APs' stack
  //
  for (Index = 0; Index < NumberOfProcessors; Index++) {
    MonitorData = GetMonitorDataAddress (Index);
    MonitorData->ApLoopMode = ApInHltLoop;
  }

  //
  // Initialize data for CPU configuration context buffer
  //
  mCpuConfigContextBuffer.CollectedDataBuffer  = AllocateZeroPool (sizeof (CPU_COLLECTED_DATA) * NumberOfProcessors);
  mCpuConfigContextBuffer.FeatureLinkListEntry = AllocateZeroPool (sizeof (LIST_ENTRY) * NumberOfProcessors);

  //
  // Initialize Processor Feature List for all logical processors.
  //
  for (Index = 0; Index < NumberOfProcessors; Index++) {
    InitializeListHead (&mCpuConfigContextBuffer.FeatureLinkListEntry[Index]);
  }

  mCpuConfigContextBuffer.RegisterTable   = AllocateAcpiNvsMemoryBelow4G (
                                            (sizeof (CPU_REGISTER_TABLE) + sizeof (UINTN)) * NumberOfProcessors
                                            );
  mCpuConfigContextBuffer.PreSmmInitRegisterTable = AllocateAcpiNvsMemoryBelow4G (
                                                     (sizeof (CPU_REGISTER_TABLE) + sizeof (UINTN)) * NumberOfProcessors
                                                     );

  mCpuConfigContextBuffer.SettingSequence = (UINTN *) (((UINTN) mCpuConfigContextBuffer.RegisterTable) + (sizeof (CPU_REGISTER_TABLE) * NumberOfProcessors));
  for (Index = 0; Index < NumberOfProcessors; Index++) {
    mCpuConfigContextBuffer.SettingSequence[Index] = Index;
  }

  //
  // Set the value for PcdCpuConfigContextBuffer.
  //
  mCpuConfigLibConfigContextBuffer = &mCpuConfigContextBuffer;
  Status = PcdSet64S (PcdCpuConfigContextBuffer, (UINT64) (UINTN) mCpuConfigLibConfigContextBuffer);
  ASSERT_EFI_ERROR (Status);

  //
  // Read the platform type from PCD
  //
  mPlatformType = PcdGet8 (PcdPlatformType);

  //
  // Allocate buffers used for feature configuration.
  //

  if (FeaturePcdGet (PcdCpuMachineCheckFlag)) {
    MachineCheckAllocateBuffer ();
  }

  if (FeaturePcdGet (PcdCpuThermalManagementFlag)) {
    ThermalManagementAllocateBuffer ();
  }

  if (FeaturePcdGet (PcdCpuCStateFlag)) {
    CStateAllocateBuffer ();
  }

  if (FeaturePcdGet (PcdCpuMicrocodeUpdateFlag)) {
    MicrocodeAllocateBuffer ();
  }

  if (FeaturePcdGet (PcdCpuVtLtFlag)) {
    VtLtAllocateBuffer ();
  }

// OVERRIDE_4757168_BEGINS  
  ProcTraceSupport = IsProcTraceSupported();
  //
  // RTIT Support
  //
  if(ProcTraceSupport && (PcdGet32 (PcdCpuRtitSetupOption) & B_PROC_TRACE_ENABLE)){
    SupportedSchemes = ProcTraceSchemesSupported();
    IsTopaSupported = (SupportedSchemes & B_CPUID_FUNCTION20_ECX_TOPA_SUPPORT) ? TRUE : FALSE;
    IsSingleRangeSupported = (SupportedSchemes & B_CPUID_FUNCTION20_ECX_SINGLE_RANGE_SUPPORT) ? TRUE : FALSE;

    if (!(IsTopaSupported || IsSingleRangeSupported)) {
      ProcTraceSupport = FALSE;
      return;
    }

    RtitMemoryBlocks = (UINT64 *) AllocatePool (NumberOfProcessors * sizeof(UINTN *));
    if (RtitMemoryBlocks != NULL) {
      for (Index = 0; Index < NumberOfProcessors; Index++) {
        if(PcdGet32 (PcdCpuRtitSetupOption) & B_PROC_TRACE_TRACE_HUB){
          //
          // TraceHub
          //
       	  MemRegionSize = 0x80;
       	  RtitMemoryBlocks[Index] = RtitBase + (Index * MemRegionSize);
          IsTopaSupported = FALSE;
        }else{
          //
          // Memory: Single Range output or ToPA
          //
          Pages = EFI_SIZE_TO_PAGES ((UINTN)MemRegionSize);
          Alignment = (UINTN)MemRegionSize;
          AlignedAddress = (UINTN) AllocateAlignedReservedPages (Pages, Alignment);
          if (AlignedAddress == 0) {
            DEBUG ((DEBUG_ERROR, "ProcTrace: Out of mem, allocated only for %d threads\n", Index));
            if (Index == 0) {
              FreePool ((VOID *) RtitMemoryBlocks);
              RtitMemoryBlocks = NULL;
              ProcTraceSupport = FALSE;
              return;
            }
            break;
          }
          RtitMemoryBlocks[Index] = AlignedAddress;
          DEBUG ((DEBUG_INFO, "ProcTrace: PT MemRegionBaseAddr(aligned) for thread %d: 0x%llX \n", Index, (UINTN) RtitMemoryBlocks[Index]));
          if(IsTopaSupported && (PcdGet32 (PcdCpuRtitSetupOption) & B_PROC_TRACE_ToPA)) {
            /**
              Create ToPA structure aligned at 4KB for each logical thread
              with at least 2 entries by 8 bytes size each. The first entry
              should have the trace output base address in bits 47:12, 6:9
              for Size, bits 4,2 and 0 must be cleared. The second entry
              should have the base address of the table location in bits
              47:12, bits 4 and 2 must be cleared and bit 0 must be set.
            **/
       	    if (TopaMemArray == NULL) {
              TopaMemArray = (UINT64 *) AllocatePool (NumberOfProcessors * sizeof(UINTN *));
              if (TopaMemArray == NULL){
                ProcTraceSupport = FALSE;
                DEBUG ((DEBUG_ERROR, "ProcTrace: Allocate mem for ToPA Failed\n"));
                return;
              }
            }
            Pages = EFI_SIZE_TO_PAGES (sizeof (PROC_TRACE_TOPA_TABLE));
            Alignment = 0x1000;
            AlignedAddress = (UINTN) AllocateAlignedReservedPages (Pages, Alignment);
            if (AlignedAddress == 0) {
              DEBUG ((DEBUG_ERROR, "ProcTrace: Out of mem, allocated ToPA mem only for %d threads\n", Index));
              if (Index == 0) {
                FreePool ((VOID *) TopaMemArray);
                FreePool ((VOID *) RtitMemoryBlocks);
                RtitMemoryBlocks = NULL;
                TopaMemArray = NULL;
                ProcTraceSupport = FALSE;
                return;
              }
              break;
            }
            TopaMemArray[Index] = AlignedAddress;
            DEBUG ((DEBUG_INFO, "ProcTrace: Topa table address(aligned) for thread %d is 0x%llX \n", Index,  (UINT64) TopaMemArray[Index]));
          }else{
            IsTopaSupported = FALSE;
          }
        }
      }
    }
  }else{
    ProcTraceSupport = FALSE;
  }
// OVERRIDE_4757168_ENDS
}

/**
  The post phase MP initialization after SMM initialization.
 
**/
VOID
PostProcessorConfiguration (
  VOID
  )
{
  EFI_STATUS    Status;
  VOID          *Registration;
  //
  // Wakeup APs. Collect data of all processors. BSP polls to
  // wait for APs' completion.
  //
  DataCollectionPhase ();
  //
  // With collected data, BSP analyzes processors'configuration
  // according to user's policy.
  //
  AnalysisPhase ();

  //
  // Wakeup APs. Program registers of all processors, according to
  // the result of Analysis phase. BSP polls to wait for AP's completion.
  //
  SettingPhase ();

  //
  // Select least-feature procesosr as BSP
  //
  if (FeaturePcdGet (PcdCpuSelectLfpAsBspFlag)) {
    SelectLfpAsBsp ();
  }

  //
  // Add SMBIOS Processor Type and Cache Type tables for the CPU.
  //
  AddCpuSmbiosTables ();

  //
  // Save CPU S3 data
  //
  SaveCpuS3Data (mImageHandle);

  Status = gBS->SetTimer (
                  mMPSystemData.CheckAPsEvent,
                  TimerPeriodic,
                  10000 * MICROSECOND
                  );
  ASSERT_EFI_ERROR (Status);

  //
  // Setup notification on Legacy BIOS Protocol to reallocate AP wakeup
  //
  EfiCreateProtocolNotifyEvent (
    &gEfiLegacyBiosProtocolGuid,
    TPL_CALLBACK,
    ReAllocateMemoryForAP,
    NULL,
    &Registration
    );
}

/**
  Event notification that is fired every time a gEfiSmmConfigurationProtocol installs.

  This function configures all logical processors with three-phase architecture.

  @param  Event                 The Event that is being processed, not used.
  @param  Context               Event Context, not used.

**/
VOID
EFIAPI
SmmConfigurationEventNotify (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  EFI_STATUS    Status;
  EFI_SMM_CONFIGURATION_PROTOCOL  *SmmConfiguration;

  //
  // Make sure this notification is for this handler
  //
  Status = gBS->LocateProtocol (&gEfiSmmConfigurationProtocolGuid, NULL, (VOID **)&SmmConfiguration);
  if (EFI_ERROR (Status)) {
    return;
  }

  PostProcessorConfiguration ();
}

/**
  Event notification for gEfiDxeSmmReadyToLockProtocolGuid event.

  This function performs security locks before any 3rd party 
  code can execute. 

  @param  Event                 The Event that is being processed, not used.
  @param  Context               Event Context, not used.

**/
VOID
EFIAPI
SmmReadyToLockCallback (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  EFI_STATUS                Status;
  VOID                      *Interface;

  // OVERRIDE_4757982_Begin
  UINTN                     VariableSize;
  SYSTEM_CONFIGURATION      SetupData;
  EFI_GUID                  mSystemConfigurationGuid = SYSTEM_CONFIGURATION_GUID;

  VariableSize = sizeof (SYSTEM_CONFIGURATION);
  Status = gRT->GetVariable (
                            L"IntelSetup",             // Denverton AptioV Override
                            &mSystemConfigurationGuid,
                            NULL,
                            &VariableSize,
                            &SetupData
                            );
  // Denverton Aptiov override Start - EIP#337909
  //ASSERT_EFI_ERROR (Status);
  if (!EFI_ERROR(Status) && SetupData.RelaxSecConf){
	  return;
  }
  // Denverton Aptiov override End - EIP#337909
  // OVERRIDE_4757982_End

  // OVERRIDE_4757803_Begin
  Status = gBS->LocateProtocol (&gEfiDxeSmmReadyToLockProtocolGuid, NULL, &Interface);
  if (EFI_ERROR (Status)) {
    return;
  }
  // OVERRIDE_4757803_End

  //
  //Call SetIaUntrusted on the BSP
  //
  SetIaUntrusted(&mMpService);
   if (mCpuConfigContextBuffer.NumberOfProcessors > 1) {
    //
    //Call SetIaUntrusted function for each AP
    //
    Status = mMpService.StartupAllAPs (
                          &mMpService,
                          (EFI_AP_PROCEDURE)SetIaUntrusted,
                          FALSE,
                          NULL,
                          0,
                          &mMpService,
                          NULL
                          );
    ASSERT_EFI_ERROR (Status);
  }
}

/**
  Early MP Initialization.
  
  This function does early MP initialization, including MTRR sync and first time microcode load.

  @param  ProcessorNumber    Handle number of specified logical processor.

**/
VOID
EarlyMpInit (
  IN UINTN  ProcessorNumber
  )
{
// OVERRIDE_4757168_BEGINS
  UINT64  MsrValue;
  PROC_TRACE_TOPA_TABLE  *TopaTable =NULL;
// OVERRIDE_4757168_ENDS
  //
  // first time microcode load.
  // Microcode patch must be applied before collecting capability
  // of processor features.
  //
  if (FeaturePcdGet (PcdCpuMicrocodeUpdateFlag)) {
    MicrocodeDetect (ProcessorNumber, TRUE);
  }

  MtrrSetAllMtrrs (mMtrrSettings);

  CollectBasicProcessorData (ProcessorNumber);

  //
  // If matching microcode patch has been found for this processor, then it
  // has already been applied at earlier phase. So here simply check if
  // microcode has been applied and collect the version.
  //
  if (FeaturePcdGet (PcdCpuMicrocodeUpdateFlag)) {
    CollectMicrocodeRevision (ProcessorNumber);
  }

// OVERRIDE_4757168_BEGINS
  if(ProcTraceSupport){

    //
    // Clear MSR_IA32_RTIT_CTL[0] and IA32_RTIT_STS only if MSR_IA32_RTIT_CTL[0]==1b
    //
    MsrValue = AsmReadMsr64 (MSR_IA32_RTIT_CTL);
    if (MsrValue & B_TRACE_EN) {
      //
      // Clear bit 0 in MSR IA32_RTIT_CTL (570)
      //
      MsrValue &= ~ B_TRACE_EN;
      AsmWriteMsr64(MSR_IA32_RTIT_CTL, MsrValue);

      //
      // Clear MSR IA32_RTIT_STS (571h) to all zeros
      //
      MsrValue = AsmReadMsr64 (MSR_IA32_RTIT_STATUS);
      MsrValue &= 0x0;
      AsmWriteMsr64 (MSR_IA32_RTIT_STATUS, MsrValue);
    }
    if(IsTopaSupported){
      //
      // ToPA
      //
   	  MsrValue = TopaMemArray[ProcessorNumber];
   	  DEBUG ((DEBUG_ERROR, "RTIT Memory MsrValue %x \n", MsrValue));
   	  DEBUG ((DEBUG_ERROR, "RtitMemoryBlocks %x \n",       RtitMemoryBlocks[ProcessorNumber]));
      TopaTable = (PROC_TRACE_TOPA_TABLE *) (UINTN)MsrValue;
      TopaTable->TopaEntry[0] = (UINT64) ((RtitMemoryBlocks[ProcessorNumber]) | (PcdGet32 (PcdCpuRtitSize) << 6)) & ~BIT0;
      TopaTable->TopaEntry[1] = (UINT64) MsrValue | BIT0;

      //
      // Program the MSR IA32_RTIT_OUTPUT_BASE (0x560) bits[47:12] with ToPA base
      //
      AsmWriteMsr64 (MSR_IA32_RTIT_OUTPUT_BASE, MsrValue);

      //
      // Set the MSR IA32_RTIT_OUTPUT_MASK (0x561) bits[63:7] to 0
      //
      AsmWriteMsr64 (MSR_IA32_RTIT_OUTPUT_MASK, 0x7f);

      //
      // Enable ToPA output scheme by enabling MSR IA32_RTIT_CTL (0x570) ToPA (Bit 8)
      //
      MsrValue = AsmReadMsr64 (MSR_IA32_RTIT_CTL);
      MsrValue |= B_ToPA_EN;
      AsmWriteMsr64 (MSR_IA32_RTIT_CTL, MsrValue);

      //
      // Enable the Processor Trace feature from MSR IA32_RTIT_CTL (570h)
      //
      MsrValue = AsmReadMsr64 (MSR_IA32_RTIT_CTL);
      MsrValue |=  B_TRACE_EN + B_TRACE_OS_CPL0 + B_TRACE_OS_CPLX + B_BRANCH_EN;
      AsmWriteMsr64 (MSR_IA32_RTIT_CTL, MsrValue);

    }else{
      //
      // Single Range  -- TraceHub or Memory use the populate addrs
      //
      if(RtitMemoryBlocks[ProcessorNumber]){
        //
        // Clear MSR IA32_RTIT_CTL (0x570) ToPA (Bit 8)
        //
        MsrValue = AsmReadMsr64 (MSR_IA32_RTIT_CTL);
        MsrValue &= ~B_ToPA_EN;
        AsmWriteMsr64 (MSR_IA32_RTIT_CTL, MsrValue);

        //
        // Program MSR IA32_RTIT_OUTPUT_BASE (0x560) bits[47:12] with the allocated Memory Region
        //
        // Update the addrs in the membuffer and  also the size.
        // If tracehub the size is always 128b ~ 0x80h per thread if all 16 threads are present
   	    DEBUG ((DEBUG_ERROR, "EarlyMpInit :RtitMemoryBlocks %x \n",       RtitMemoryBlocks[ProcessorNumber]));
        MsrValue = (UINT64) RtitMemoryBlocks[ProcessorNumber];
        AsmWriteMsr64 (MSR_IA32_RTIT_OUTPUT_BASE, MsrValue);

        //
        // Program the Mask bits for the Memory Region to MSR IA32_RTIT_OUTPUT_MASK_PTRS (561h)
        //
        MsrValue = (UINT64) MemRegionSize - 1;
        AsmWriteMsr64 (MSR_IA32_RTIT_OUTPUT_MASK, MsrValue);
        //
        // Enable the Processor Trace feature from MSR IA32_RTIT_CTL (570h)
        //
        MsrValue = AsmReadMsr64 (MSR_IA32_RTIT_CTL);
        MsrValue |=  B_TRACE_EN + B_TRACE_OS_CPL0 + B_TRACE_OS_CPLX + B_BRANCH_EN;
        AsmWriteMsr64 (MSR_IA32_RTIT_CTL, MsrValue);
      }
    }
// OVERRIDE_4757168_ENDS	
  }
}

/**
  First phase MP initialization before SMM initialization.
  
  @retval EFI_SUCCESS      First phase MP initialization was done successfully.
  @retval EFI_UNSUPPORTED  There is legacy APIC ID conflict and can't be rsolved in xAPIC mode.

**/
EFI_STATUS
ProcessorConfiguration (
  VOID
  )
{
  EFI_STATUS    Status;

// Denverton AptioV Override Start - EIP#360629

  UINT64  TimerPeriod;
  EFI_TIMER_ARCH_PROTOCOL         *gTimer = NULL;
  EFI_CPU_ARCH_PROTOCOL           *gCpu = NULL;  

   //
   // Closed the EFI timer event for events overlap
   //
   Status = gBS->LocateProtocol (&gEfiTimerArchProtocolGuid, NULL, (VOID **) &gTimer); 
   ASSERT_EFI_ERROR (Status);  

   Status = gBS->LocateProtocol (&gEfiCpuArchProtocolGuid, NULL, (VOID **) &gCpu);
   ASSERT_EFI_ERROR (Status);
   
   gTimer->GetTimerPeriod (gTimer, &TimerPeriod);
   gTimer->SetTimerPeriod (gTimer, 0);

   // Disable CPU interrupts 
   gCpu->DisableInterrupt (gCpu);

// Denverton AptioV Override End - EIP#360629

  //
  // Wakeup APs for the first time, BSP stalls for arbitrary
  // time for APs' completion. BSP then collects the number
  // and BIST information of APs.
  //
  WakeupAPAndCollectBist ();
  //
  // Sort APIC ID of all processors in asending order. Processor number
  // is assigned in this order to ease MP debug. SMBIOS logic also depends on it.
  //
  SortApicId ();

  //
  // Prepare data in memory for processor configuration
  //
  PrepareMemoryForConfiguration ();

  //
  // Early MP initialization
  //
  mMtrrSettings = (MTRR_SETTINGS *)(UINTN)PcdGet64 (PcdCpuMtrrTableAddress);
  if (mMtrrSettings == NULL) {
    //
    // MtrrSettings is not set. Set it here. 
    // Allocate memory for fixed MTRRs, variable MTRRs and MTRR_DEF_TYPE
    //
    mMtrrSettings = AllocateAcpiNvsMemoryBelow4G (sizeof (MTRR_SETTINGS));
    Status = PcdSet64S (PcdCpuMtrrTableAddress, (UINT64) (UINTN) mMtrrSettings);
    ASSERT_EFI_ERROR (Status);
  }
  MtrrGetAllMtrrs (mMtrrSettings);

  DispatchAPAndWait (
    TRUE,
    0,
    EarlyMpInit
    );

  EarlyMpInit (mCpuConfigContextBuffer.BspNumber);

  DEBUG_CODE (
    //
    // Verify that all processors have same APIC ID topology. New APIC IDs
    // were constructed based on this assumption.
    //
    UINTN Index;
    UINT8 PackageIdBitOffset;

    PackageIdBitOffset = mCpuConfigContextBuffer.CollectedDataBuffer[0].PackageIdBitOffset;
    for (Index = 1; Index < mCpuConfigContextBuffer.NumberOfProcessors; Index++) {
      if (PackageIdBitOffset != mCpuConfigContextBuffer.CollectedDataBuffer[Index].PackageIdBitOffset) {
        ASSERT (FALSE);
      }
    }
  );

  //
  // Check if there is legacy APIC ID conflict among all processors.
  //
  Status = CheckApicId ();
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Assign Package BSP for package scope programming later.
  //
  AssignPackageBsp ();

  //
  // Produce pre-SMM-init register table.
  //
  ProducePreSmmInitRegisterTable ();

  //
  // Early MP initialization step 2
  //
  DispatchAPAndWait (
    TRUE,
    0,
    SetPreSmmInitProcessorRegister
    );

  SetPreSmmInitProcessorRegister (mCpuConfigContextBuffer.BspNumber);

  //
  // Re-program Local APIC for virtual wire mode if socket ID for the BSP has been changed.
  //
  ReprogramVirtualWireMode ();

  //
  // If socket IDs were reassigned, update APIC ID related info collected before.
  //
  UpdateApicId ();

  MeasureMicrocode ();

  //
  // Locate Timer Arch Protocol
  //
  Status = gBS->LocateProtocol (&gEfiTimerArchProtocolGuid, NULL, (VOID **) &mTimer);
  ASSERT_EFI_ERROR (Status);

  //
  // Install MP Services Protocol
  //
  InstallMpServicesProtocol ();
  
  
// Denverton AptioV Override Start - EIP#360629

  gTimer->SetTimerPeriod (gTimer, TimerPeriod);
  gCpu->EnableInterrupt (gCpu);

// Denverton AptioV Override End - EIP#360629
  
  return EFI_SUCCESS;
}

/**
  Installs MP Services Protocol and register related timer event.
  
  This function installs MP Services Protocol and register related timer event.

**/
VOID
InstallMpServicesProtocol (
  VOID
  )
{
  EFI_STATUS    Status;

  //
  // Create timer event to check AP state for non-blocking execution.
  //
  Status = gBS->CreateEvent (
                  EVT_TIMER | EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  CheckAPsStatus,
                  NULL,
                  &mMPSystemData.CheckAPsEvent
                  );
  ASSERT_EFI_ERROR (Status);

  //
  // Now install the MP services protocol.
  //
  Status = gBS->InstallProtocolInterface (
                  &mHandle,
                  &gEfiMpServiceProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mMpService
                  );
  ASSERT_EFI_ERROR (Status);
}

/**
  Callback function for idle events.
 
  @param  Event                 Event whose notification function is being invoked.
  @param  Context               The pointer to the notification function's context,
                                which is implementation-dependent.

**/
VOID
EFIAPI
IdleLoopEventCallback (
  IN EFI_EVENT                Event,
  IN VOID                     *Context
  )
{
  CpuSleep ();
}

/**
  Entrypoint of CPU MP DXE module.
  
  This function is the entrypoint of CPU MP DXE module.
  It initializes Multi-processor configuration and installs MP Services Protocol.

  @param  ImageHandle   The firmware allocated handle for the EFI image.
  @param  SystemTable   A pointer to the EFI System Table.
  
  @retval EFI_SUCCESS   The entrypoint always returns EFI_SUCCESS.

**/
EFI_STATUS
EFIAPI
MultiProcessorInitialize (
  IN EFI_HANDLE                            ImageHandle,
  IN EFI_SYSTEM_TABLE                      *SystemTable
  )
{
  EFI_STATUS  Status;
  VOID        *Registration;
  EFI_EVENT   IdleLoopEvent;
  EFI_EVENT   ExitBootServiceEvent;
  EFI_EVENT   LegacyToBootEvent;
  UINT32      RegEax;
  UINT32      FamilyId;
  UINT32      ModelId;

  mImageHandle = ImageHandle;
  FamilyId     = 0;
  ModelId      = 0;
  
//BDE Ovverride begin - copy the ucode to memory to speed up the ucode load time for S3
//details in Grantley 4986242
	//
	// Copy Microcode to reserved memory
	//
//  mMicrocodeBaseAddress = AllocateReservedPool(PcdGet32 (PcdFlashNvStorageMicrocodeSize));
//  CopyMem(mMicrocodeBaseAddress, (VOID*)(UINTN)PcdGet32 (PcdFlashNvStorageMicrocodeBase), (UINTN)PcdGet32 (PcdFlashNvStorageMicrocodeSize));
//  mMicrocodePatchAddress = (VOID *)((UINTN)mMicrocodeBaseAddress + (UINTN)(PcdGet64 (PcdCpuMicrocodePatchAddress) - PcdGet32 (PcdFlashNvStorageMicrocodeBase)));
//BDE Override end
  
  //
  // Configure processors with three-phase architecture
  //
  Status = ProcessorConfiguration ();
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (FeaturePcdGet (PcdCpuTunnelCreekFamilyFlag) || FeaturePcdGet (PcdCpuSilvermontFamilyFlag) ||
      FeaturePcdGet (PcdCpuGoldmontFamilyFlag)) {
    //
    // Get the current logic processor information
    //
    AsmCpuid (EFI_CPUID_VERSION_INFO, &RegEax, NULL, NULL, NULL);
    FamilyId = BitFieldRead32 (RegEax, 8, 11);
    ModelId  = BitFieldRead32 (RegEax, 4, 7);
    if (FamilyId == 0x06 || FamilyId == 0x0f) {
      ModelId += ((BitFieldRead32 (RegEax, 16, 19)) << 4);
    }
    if (FamilyId == 0x0f) {
      FamilyId += BitFieldRead32 (RegEax, 20, 27);
    }
  }

  if (FeaturePcdGet (PcdCpuTunnelCreekFamilyFlag) && IS_TUNNELCREEK_PROC (FamilyId, ModelId)) {
    //
    // If this processor is TunnelCreek, do the post phase MP initialization directly and needn't wait for 
    // SMM initialization.
    //
    PostProcessorConfiguration ();
  } else {
    //
    // Install notification callback on SMM Configuration Protocol
    //
    mSmmConfigurationNotificationEvent = EfiCreateProtocolNotifyEvent (
                                           &gEfiSmmConfigurationProtocolGuid,
                                           TPL_CALLBACK,
                                           SmmConfigurationEventNotify,
                                           NULL,
                                           &Registration
                                           );
  }

  if ((FeaturePcdGet (PcdCpuSilvermontFamilyFlag) && IS_AVOTON_PROC (FamilyId, ModelId)) ||
      (FeaturePcdGet (PcdCpuGoldmontFamilyFlag) && IS_DENVERTON_PROC (FamilyId, ModelId))) {
    if (FeaturePcdGet (PcdCpuEnableIaUntrustedModeFlag)) {
      //
      // Install notification callback on SMM ready to lock event
      //
      EfiCreateProtocolNotifyEvent  (
                                   &gEfiDxeSmmReadyToLockProtocolGuid,
                                   TPL_CALLBACK,
                                   SmmReadyToLockCallback,
                                   NULL,
                                   &Registration
                                   );
    }
  }

  //
  // Check if there is AP existed
  //
  if (mCpuConfigContextBuffer.NumberOfProcessors > 1) {
    //
    // Create EXIT_BOOT_SERIVES Event to set AP to suitable status
    //
    Status = gBS->CreateEventEx (
                    EVT_NOTIFY_SIGNAL,
                    TPL_NOTIFY,
                    ChangeApLoopModeCallBack,
                    NULL,
                    &gEfiEventExitBootServicesGuid,
                    &ExitBootServiceEvent
                    );
    ASSERT_EFI_ERROR (Status);
    //
    // Create an event to be signalled when Legacy Boot occurs to set AP to suitable status
    //
    Status = EfiCreateEventLegacyBootEx(
               TPL_NOTIFY,
               ChangeApLoopModeCallBack,
               NULL,
               &LegacyToBootEvent
               );
    ASSERT_EFI_ERROR (Status);
  }
  //
  // Setup a callback for idle events
  //
  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_NOTIFY,
                  IdleLoopEventCallback,
                  NULL,
                  &gIdleLoopEventGuid,
                  &IdleLoopEvent
                  );
  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}

/**
  Wakes up APs for the first time to count their number and collect BIST data.

  This function wakes up APs for the first time to count their number and collect BIST data.

**/
VOID
WakeupAPAndCollectBist (
  VOID
  )
{
  //
  // Save BSP's Local APIC Timer setting
  //
  GetApicTimerState (&mLocalApicTimerDivisor, NULL, NULL);
  mLocalApicTimerInitialCount = GetApicTimerInitCount ();

  //
  // Prepare code and data for APs' startup vector
  //
  PrepareAPStartupVector ();

  mCpuConfigContextBuffer.NumberOfProcessors = 1;
  mCpuConfigContextBuffer.BspNumber = 0;
  //
  // Item 0 of BistBuffer is for BSP.
  //
  mExchangeInfo->BistBuffer[0].ApicId = GetInitialApicId ();
  
  SendInitSipiSipiIpis (
    TRUE,
    0,
    NULL
    );

  //
  // Wait for task to complete and then exit.
  //
  MicroSecondDelay (PcdGet32 (PcdCpuApInitTimeOutInMicroSeconds));
  mExchangeInfo->InitFlag = 0;
}


/**
  Prepare ACPI NVS memory below 4G memory for use of S3 resume.
  
  This function allocates ACPI NVS memory below 4G memory for use of S3 resume,
  and saves data into the memory region.

  @param  Context   The Context save the info.

**/
VOID
SaveCpuS3Data (
  VOID    *Context
  )
{
  MP_CPU_SAVED_DATA       *MpCpuSavedData;

  //
  // Allocate ACPI NVS memory below 4G memory for use of S3 resume.
  //
  MpCpuSavedData = AllocateAcpiNvsMemoryBelow4G (sizeof (MP_CPU_SAVED_DATA));

  //
  // Set the value for CPU data
  //
  mAcpiCpuData                 = &(MpCpuSavedData->AcpiCpuData);
  mAcpiCpuData->GdtrProfile    = (EFI_PHYSICAL_ADDRESS) (UINTN) &(MpCpuSavedData->GdtrProfile);
  mAcpiCpuData->IdtrProfile    = (EFI_PHYSICAL_ADDRESS) (UINTN) &(MpCpuSavedData->IdtrProfile);
  mAcpiCpuData->StackAddress   = (EFI_PHYSICAL_ADDRESS) (UINTN) mExchangeInfo->StackStart;
  mAcpiCpuData->StackSize      = PcdGet32 (PcdCpuApStackSize);
  mAcpiCpuData->MtrrTable      = (EFI_PHYSICAL_ADDRESS) (UINTN) PcdGet64 (PcdCpuMtrrTableAddress);
  mAcpiCpuData->RegisterTable  = (EFI_PHYSICAL_ADDRESS) (UINTN) mCpuConfigContextBuffer.RegisterTable;

  mAcpiCpuData->PreSmmInitRegisterTable   = (EFI_PHYSICAL_ADDRESS) (UINTN) mCpuConfigContextBuffer.PreSmmInitRegisterTable;
  mAcpiCpuData->ApMachineCheckHandlerBase = mApMachineCheckHandlerBase;
  mAcpiCpuData->ApMachineCheckHandlerSize = mApMachineCheckHandlerSize;

  //
  // Copy GDTR and IDTR profiles
  //
  CopyMem ((VOID *) (UINTN) mAcpiCpuData->GdtrProfile, (VOID *) (UINTN) &mExchangeInfo->GdtrProfile, sizeof (IA32_DESCRIPTOR));
  CopyMem ((VOID *) (UINTN) mAcpiCpuData->IdtrProfile, (VOID *) (UINTN) &mExchangeInfo->IdtrProfile, sizeof (IA32_DESCRIPTOR));

  mAcpiCpuData->NumberOfCpus  = (UINT32) mCpuConfigContextBuffer.NumberOfProcessors;
  
  //
  // Set the base address of CPU S3 data to PcdCpuS3DataAddress
  //
  PcdSet64S (PcdCpuS3DataAddress, (UINT64)(UINTN)mAcpiCpuData);
}
