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

  SmmErrorHandler.c

Abstract:

  SMM Error Logging.

------------------------------------------------------------------*/
#include "CommonErrorHandlerIncludes.h"
// Denverton AptioV Override Start - EIP#275412
#include "CpuRegs.h"
#include "Token.h"
#include <Protocol/SmmRtProtocol.h>

EFI_SMM_RUNTIME_PROTOCOL                *mSmmRT = NULL;
static EFI_RUNTIME_SERVICES             *mRT;
ERROR_LOG_SETUP_DATA                    mErrLogSetupData;
#if MCA_ERROR_LOGGING
MC_DEV_INFO mcInfo;
BOOLEAN  mCsiErrDetected = FALSE;
BOOLEAN  mAllogCsiErrLogging = TRUE;

extern EFI_STATUS CommonErrorHandling ( VOID  *ErrStruc);
#endif
// Denverton AptioV Override End - EIP#275412
//
// General Global values required in the Runtime
//
BOOLEAN                                  mPciSerrDetected                  = FALSE;
BOOLEAN                                  mPciPerrDetected                  = FALSE;
BOOLEAN                                  mPcieFatalErrDetected             = FALSE;
BOOLEAN                                  mNumCorrectablePcieErrs           = 0;
BOOLEAN                                  mUCEccDetected                    = FALSE;
BOOLEAN                                  mUCNonStdErrDetected              = FALSE;
BOOLEAN                                  mAllowClearingOfPcieDeviceStatus  = TRUE;
BOOLEAN                                  mAllowClearingOfUEStatus          = TRUE;    // CV requirement
UINT8                                    mPciXPUnsupportedRequest          = 0;

EFI_HANDLE                               mImageHandle;

static VOID                              *mInitSmmErrorLogNotifyReg = NULL;
static VOID                              *mWheaEventReg = NULL;
static EFI_EVENT                         mWheaEvent;
BOOLEAN                                  mWheaPlatformSupportAvailable = FALSE;
EFI_WHEA_PLATFORM_SUPPORT_PROTOCOL       *mWheaPlatformSupport = NULL;

static EFI_SMM_BASE2_PROTOCOL            *mSmmBase2;
EFI_SMM_SYSTEM_TABLE2*                   mSmst = NULL;

VOID
ClearSystemErrors(
  VOID
  )
/*++

Routine Description:

  Clear all system errors before enabling them.

Arguments:
  
  None 

Returns:

  none

--*/
{  
  ClearNorthComplexErrors();
  ClearSouthComplexErrors();
  // Denverton AptioV Override Start - EIP#275412
#if MCA_ERROR_LOGGING
  // Do not clear the Mc bank registers, if Machine check error has been detected in the SMI handler
  // Since this data will be used to log the error in next boot
  if(!(mAllogCsiErrLogging==FALSE && mCsiErrDetected==TRUE))
  {
	  ClearMCErrors();
  }
#endif
// Denverton AptioV Override End - EIP#275412
}

EFI_STATUS
ErrorHandlerMain 
(
  IN  EFI_HANDLE    DispatchHandle,
  IN CONST VOID     *Context         OPTIONAL,
  IN OUT VOID       *CommunicationBuffer,
  IN OUT UINTN      *SourceSize
)
/*++

Routine Description:
    Main SMI handler to handle system errors

Arguments:

Returns:
    Status

--*/ 
{
  BOOLEAN          ErrorDetected = FALSE;
  // Denverton AptioV Override Start - EIP#275412
#if MCA_ERROR_LOGGING
  mCsiErrDetected = FALSE;
#endif
// Denverton AptioV Override End - EIP#275412
  
  if(!CheckForSystemError()) {
    return EFI_SUCCESS;
  }

  if (mWheaPlatformSupportAvailable && mErrLogSetupData.WheaLogging) {
    mWheaPlatformSupport->WheaPlatformRestartAllElog (mWheaPlatformSupport);
  }

  ErrorDetected |= ElogNorthComplexErrorHandler ();
  ErrorDetected |= ElogSouthComplexErrorHandler ();
// Denverton AptioV Override Start - EIP#275412
  // PCI and Memory errors are not detected. Check for CSI errors
#if MCA_ERROR_LOGGING
  if (!ErrorDetected)
  {
	  ErrorDetected |= ELogCSIErrorHandler();
  }
#endif
// Denverton AptioV Override End - EIP#275412
  ClearSystemErrors ();

  if (ErrorDetected) {
    
    if (mWheaPlatformSupportAvailable && mErrLogSetupData.WheaLogging) {
      // EndWheaElog generates SCI for this Correctable error.
      mWheaPlatformSupport->WheaPlatformEndElog (mWheaPlatformSupport );
    }
    //
    // NMI if a fatal error occurred.  
    // We treat PCI parity errors and PCIe Fatal errors since
    // data corruption may have occurred.
    //
	// Denverton AptioV Override Start - EIP#275412
	#if MCA_ERROR_LOGGING
    if ((mPciSerrDetected && mErrLogSetupData.PCIeSystemErrEnable) || (mPciPerrDetected && mErrLogSetupData.PCIeParityErrEnable) ||
         mPcieFatalErrDetected  || mUCEccDetected || mUCNonStdErrDetected || mCsiErrDetected) {
	#else
    if ((mPciSerrDetected && mErrLogSetupData.PCIeSystemErrEnable) || (mPciPerrDetected && mErrLogSetupData.PCIeParityErrEnable) ||
         mPcieFatalErrDetected  || mUCEccDetected || mUCNonStdErrDetected || mCsiErrDetected) {
	#endif
	// Denverton AptioV Override End - EIP#275412
      ElogGenerateNmiNow ();
    }
  }

  return EFI_SUCCESS;
}

EFI_STATUS
RegisterErrorSMIHandler (
  VOID
  )
/*++

Routine Description:
    This function registers a SMI callback function for processor error checking and processing

Arguments:
    None

Returns:
    Nothing

--*/
{
  EFI_STATUS            Status = EFI_SUCCESS;

  if(mErrLogSetupData.SystemErrorEn == 1) {

    Status = gSmst->SmiHandlerRegister (
                      ErrorHandlerMain,
                      NULL,
                      &mImageHandle
                      );
    ASSERT_EFI_ERROR (Status);

  }
  return Status;
}


VOID
ProgramErrorSignalAndSeverity(
  VOID
	)
/*++

Routine Description:

  This function Enables or disables the Memory PCIe and Processor errors.

Arguments:

    None

Returns:

    None

--*/
{
  //
  // Program North Complex Error Signals
  //
  ProgramNorthComplexErrorSignals();

  //
  // Program South Complex Error Signals
  //
  ProgramSouthComplexErrorSignals();
}

VOID
EFIAPI
WheaPlatformCallback (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
/*++

Routine Description:

  Call back function to locate the mWheaPlatformSupport protocol

Arguments:

  Event    - Event type
  *Context - Context for the event

Returns:

  None

--*/
{
  EFI_STATUS        Status;

  mWheaPlatformSupportAvailable = FALSE;    
  mWheaPlatformSupport = NULL;    
  Status = mSmst->SmmLocateProtocol(&gEfiWheaPlatformSupportProtocolGuid, NULL, &mWheaPlatformSupport);
  if (Status == EFI_SUCCESS) {
    mWheaPlatformSupportAvailable = TRUE;
  }
}
// Denverton AptioV Override Start - EIP#275412
#if MCA_ERROR_LOGGING
VOID
EFIAPI
ClearMCErrorsPerCore (
  IN  VOID *Buffer
  )
/*++
Routine Description:

  This function is used to clear the MC bank register of the given core

Arguments:
  
  Buffer

Returns:

  none
--*/
{
    UINT32 RegEdx, Index;

    AsmCpuid (EFI_CPUID_VERSION_INFO, NULL, NULL, NULL, &RegEdx);
    if(RegEdx & (B_EFI_CPUID_VERSION_INFO_EDX_MCA | B_EFI_CPUID_VERSION_INFO_EDX_MCE)) 
    {
        // Reading Global Machinge Check Control MSRs
        mcInfo.McgCapMsr.Data  = AsmReadMsr64 (EFI_MSR_IA32_MCG_CAP);
        // Reading Machine check error reporting MSRs for all banks
        for(Index=0; Index < mcInfo.McgCapMsr.Bits.bank_count; Index++)
        {
            AsmWriteMsr64 (EFI_MSR_IA32_MC0_STATUS + Index*4 , 0);
            // Denverton does not have Mc7_Addr_MSR & Mc8_Addr_MSR, Writing this will generate a General Exception 
            if(!((Index == 7) || (Index == 8)))
                AsmWriteMsr64 (EFI_MSR_IA32_MC0_ADDR + Index*4 , 0);
        }
    }
}

VOID
ClearMCErrors(
  )
/*++

Routine Description:

  Clear all Machine check errors

Arguments:
  
  none

Returns:

  none

--*/
{  
	UINT8 Index;
    EFI_STATUS     Status;
    
	ClearMCErrorsPerCore(NULL); // Clear the MCbank registers in BSP
    for (Index = 1; Index < (UINT8)mSmst->NumberOfCpus; Index++) {
        Status = mSmst->SmmStartupThisAp (ClearMCErrorsPerCore, Index, NULL);
        ASSERT_EFI_ERROR (Status);
    }
}

VOID
EFIAPI
DetectLastBootMCError (
  IN  VOID *Buffer
  )
/*++
Routine Description:

  This function is used to Detects, if any machine check error has 
  occurred on the current processor core

Arguments:
  
  Buffer

Returns:

  none
--*/
{
    UINT32 RegEdx, Index;
    UINT8 CpuIndex = *(UINT8*)Buffer;
    MC_ERROR_LOG_INFO McErrLogInfo;

    mcInfo.CpuIndex = CpuIndex;
    // Check whether Machine check is supported or not
    AsmCpuid (EFI_CPUID_VERSION_INFO, NULL, NULL, NULL, &RegEdx);
    if(RegEdx & (B_EFI_CPUID_VERSION_INFO_EDX_MCA | B_EFI_CPUID_VERSION_INFO_EDX_MCE)) 
    {
        // Reading Global Machinge Check Control MSRs
        mcInfo.McgCapMsr.Data  = AsmReadMsr64 (EFI_MSR_IA32_MCG_CAP);
        mcInfo.McgStatusMsr.Data = AsmReadMsr64 (EFI_MSR_IA32_MCG_STATUS);
        
        // Reading Machine check error reporting MSRs for all banks
        for(Index=0; Index < mcInfo.McgCapMsr.Bits.bank_count; Index++)
        {
            // Read the MCi_STATUS MSR and check for occurrence of error
            mcInfo.MciStatusMsr.Data = AsmReadMsr64 (EFI_MSR_IA32_MC0_STATUS + Index*4 );   
            if(mcInfo.MciStatusMsr.Bits.val && mcInfo.MciStatusMsr.Bits.uc && mcInfo.MciStatusMsr.Bits.en)
            // Added for testing >>>
            // if(mcInfo.MciStatusMsr.Bits.val  && mcInfo.MciStatusMsr.Bits.uc  /* && mcInfo.MciStatusMsr.Bits.en*/)
            // Added for testing <<<
            {
            	mCsiErrDetected = TRUE;
                if(mAllogCsiErrLogging)
                {
					mcInfo.BankIndex = Index;
					mcInfo.MciCtlMsr = AsmReadMsr64 (EFI_MSR_IA32_MC0_CTL + Index*4 );
					// Denverton does not have Mc7_Addr_MSR & Mc8_Addr_MSR, Writing this will generate a General Exception 
					if(!((Index == 7) || (Index == 8)))
					    mcInfo.MciAddrMsr = AsmReadMsr64 (EFI_MSR_IA32_MC0_ADDR + Index*4 );
					McErrLogInfo.Source = CsiSource;
					McErrLogInfo.Type = MC_ERROR_TYPE;
					McErrLogInfo.McDevInfo = &mcInfo;
					CommonErrorHandling((VOID *) &McErrLogInfo);
                } else {
                    return;
                }
            }
        }
    }
}

BOOLEAN
EFIAPI
ELogCSIErrorHandler (
  VOID
  )
/*++
Routine Description:

  This function is used to Check the occurance of any CSI errors

Arguments:
  
  none

Returns:

  ErrorDetected status
--*/
{
    EFI_STATUS     Status;
    UINT8          Index=0;
    BOOLEAN        ErrorDetected = FALSE;
    UINT64         SmmMcaControlMsr;
    
    //
    // Check if any Machine Check error has occured in the last boot
    //
    DetectLastBootMCError(&Index); // Check for errors in the BSP
    for (Index = 1; (Index < (UINT8)mSmst->NumberOfCpus) && !(mAllogCsiErrLogging==FALSE && mCsiErrDetected==TRUE); Index++) {
        Status = mSmst->SmmStartupThisAp (DetectLastBootMCError, Index, &Index);
        ASSERT_EFI_ERROR (Status);
    }

    ErrorDetected |= mCsiErrDetected;
    
    // If Machine check error is detected, We must mask the SMI generation for other processors
    if (mAllogCsiErrLogging==FALSE && mCsiErrDetected == TRUE)
    {
        SmmMcaControlMsr = AsmReadMsr64 (SMM_MCA_CONTROL);
        SmmMcaControlMsr &= (UINT64)~BIT9;  // Reset the PEND_SMI_ON_MCA Bit
        AsmWriteMsr64 (SMM_MCA_CONTROL, SmmMcaControlMsr);
    }
    
    return ErrorDetected;
}

VOID
EFIAPI
HandleLastBootErrors (
  VOID
  )
/*++
Routine Description:

  This function is used to Collect information from the last boot 
  and record if any error has occured

Arguments:
  
  none

Returns:

  none
--*/
{
	//
	// Check for occurrence of any CSI errors
	//
	ELogCSIErrorHandler();
  
}
#endif
// Denverton AptioV Override End - EIP#275412

EFI_STATUS
LocateWheaPlatformProtocol (
  VOID
  )
/*++

Routine Description:

  This function is used to locate the mWheaPlatformSupportProtocol protocol

Arguments:
  
  ErrorLogSetupData : 

Returns:

  none

--*/
{
  EFI_STATUS Status;

  //
  // Locate the WHEA Platform driver and initialize the module globals.
  //
  mWheaPlatformSupportAvailable = FALSE;
  mWheaPlatformSupport		  = NULL;
 
  Status = mSmst->SmmLocateProtocol(&gEfiWheaPlatformSupportProtocolGuid, NULL, &mWheaPlatformSupport);
   
  if (Status == EFI_SUCCESS && mWheaPlatformSupport != NULL) {
    mWheaPlatformSupportAvailable = TRUE;
  } else {
 
    //
    // Find Whea Platform protocol instance
    //
    mWheaPlatformSupport = NULL;

    Status = mSmst->SmmRegisterProtocolNotify(
                &gEfiWheaPlatformSupportProtocolGuid,
                (EFI_SMM_NOTIFY_FN)WheaPlatformCallback,
                &mWheaEventReg
                );
    if(EFI_ERROR(Status))  return Status;
  }

  return Status;
}


EFI_STATUS
InitializeEDKProtocols()
{
  EFI_STATUS  Status;

  //
  // Locate WheaPlatform Protocol
  //
  Status = LocateWheaPlatformProtocol ();

  return Status;
}

EFI_STATUS
SetupInterface (
  )
  /*++

Routine Description:

 Access setup variables and updates in the local structures to be used in the error log driver

Arguments:
  
  ErrorLogSetupData : 

Returns:

  none

--*/
{
  EFI_STATUS                        Status = EFI_SUCCESS;
  struct RAS_GLOBAL_POLICY          *RasPolicy;
  EFI_PEI_HOB_POINTERS              HobPtr;
  EFI_GLOBAL_NVS_AREA_PROTOCOL      *GlobalNvsArea;

  // Clear the ErrrorLogSetUpData structure.
  ZeroMem (&mErrLogSetupData, sizeof (ERROR_LOG_SETUP_DATA));

  //
  // Get PCH Policy HOB.
  //
  HobPtr.Guid   = GetFirstGuidHob (&gRasPolicyHobGuid);
  ASSERT (HobPtr.Guid != NULL);
  RasPolicy = GET_GUID_HOB_DATA (HobPtr.Guid);

  //
  //System Error Log
  //
  mErrLogSetupData.SystemErrorEn = RasPolicy->SystemErrorEn;
  mErrLogSetupData.ErrorCloakingEnable = RasPolicy->ErrorCloakingEnable;

  if (mErrLogSetupData.SystemErrorEn == 1) {
    
    mErrLogSetupData.MemoryErrorLog = RasPolicy->MemoryErrorLog;
    if (mErrLogSetupData.MemoryErrorLog) {
      mErrLogSetupData.MemoryCorrErrorLog = RasPolicy->MemoryCorrErrorLog;
      mErrLogSetupData.MemoryUnCorrErrorLog = RasPolicy->MemoryUnCorrErrorLog;
    }

    mErrLogSetupData.PCIeErrorLog = RasPolicy->PCIeErrorLog;
    if (mErrLogSetupData.PCIeErrorLog) {
      mErrLogSetupData.PCIeCorrErrorLog = RasPolicy->PCIeCorrErrorLog;
      mErrLogSetupData.PCIeNFErrorLog = RasPolicy->PCIeNFErrorLog;
      mErrLogSetupData.PCIeFatalErrorLog = RasPolicy->PCIeFatalErrorLog;
      mErrLogSetupData.PCIeSystemErrEnable = RasPolicy->PCIeSystemErrEnable;
      mErrLogSetupData.PCIeParityErrEnable = RasPolicy->PCIeParityErrEnable;
    }

    mErrLogSetupData.RCECErrorLog = RasPolicy->RCECErrorLog;
    mErrLogSetupData.BUnitErrorLog = RasPolicy->BUnitErrorLog;

  }

  //
  //Whea Logging
  //
  mErrLogSetupData.WheaSupport = RasPolicy->WheaSupport;
  if (mErrLogSetupData.WheaSupport) {
    mErrLogSetupData.WheaLogging = RasPolicy->WheaLogging;
  }


  Status = gBS->LocateProtocol (
                  &gEfiGlobalNvsAreaProtocolGuid,
                  NULL,
                  &GlobalNvsArea
                  );

  ASSERT_EFI_ERROR(Status);

  return Status;
}

EFI_STATUS
InitializeUserConfiguration(
  )
/*
Routine Description:

  This function updates the ERROR_LOG_SETUP_DATA with setup and other platform needed information

Arguments:
  
  ErrorLogSetupData : 

Returns:

  EFI_STATUS


*/
{
  EFI_STATUS  Status;
// Denverton AptioV Override Start - EIP#275412
#if MCA_ERROR_LOGGING 
  UINT64         SmmMcaControlMsr;
#endif
  //
  // setup Varibale update
  //
  Status = SetupInterface();

  if( EFI_ERROR(Status)) 
    return Status;

  //
  // Update EDKII proitocols used.
  //
  Status = InitializeEDKProtocols();

#if MCA_ERROR_LOGGING 
  //
  // Enable PEND_SMI_ON_MCA Bit to generated SMI when MCERR occurs
  //
  SmmMcaControlMsr = AsmReadMsr64 (SMM_MCA_CONTROL);
  SmmMcaControlMsr |= BIT9;  
  AsmWriteMsr64 (SMM_MCA_CONTROL, SmmMcaControlMsr);
#endif
// Denverton AptioV Override End - EIP#275412
  return Status;

}

BOOLEAN
EfiInManagementInterrupt (
  VOID
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:


Returns:

  GC_TODO: add return values

--*/
{
   
  BOOLEAN InSmm;

  if (mSmmBase2 == NULL) {
    InSmm = FALSE;
  } else {
    mSmmBase2->InSmm (mSmmBase2, &InSmm);
    mSmmBase2->GetSmstLocation (mSmmBase2, &mSmst);
  }

  return InSmm;
}


EFI_STATUS
InitSmmErrorLogCallback (
  IN CONST EFI_GUID                *Protocol,
  IN VOID                          *Interface,
  IN EFI_HANDLE                    Handle
  )
/*++

Routine Description:

  Callback function to Initialise Error Signals and register error Handler

Arguments:

  Event                                       - Event on which the callback is called
  Context                                     - The context of the Callback

Returns:

  EFI_STATUS

--*/
{

  ClearSystemErrors();

  //
  //  Enables or Disables Errors
  //
  ProgramErrorSignalAndSeverity();

  //
  // Register SMI handler to handle CORR/NON FATAL/FATAL errros
  //
  RegisterErrorSMIHandler();

  //
  // Enable SMI Logic such that an SMI is generated upon any of the anticipated errors.
  //
  EnableAsyncSmiLogic();

  return EFI_SUCCESS;
}

//
// Enable Error Cloaking
//
EFI_STATUS
UpdateErrorCloaking (
  VOID
  )
{
  UINT8                     ErrorCloaking   = 0;
  UINT32                    McaControl      = 0;

  if (DXE_IN_SIMICS) {
    return EFI_SUCCESS;
  }

  ErrorCloaking = mErrLogSetupData.ErrorCloakingEnable;
  McaControl    = AsmReadMsr32 (MSR_B_SMM_MCA_CTL);

  if (ErrorCloaking == ERROR_CLOAKING_DISABLED) {
    McaControl &= ~B_ERROR_CLOAKING;
  } else if (ErrorCloaking == ERROR_CLOAKING_ENABLED) {
	McaControl |= B_ERROR_CLOAKING;
  }
  AsmWriteMsr32(MSR_B_SMM_MCA_CTL, McaControl);

  return EFI_SUCCESS;
}

//
// Driver entry point
//
EFI_STATUS
InitializeSmmErrorLog (
  IN EFI_HANDLE             ImageHandle,
  IN EFI_SYSTEM_TABLE       *SystemTable
  )
{
  EFI_STATUS				Status;

  Status = gBS->LocateProtocol (
                  &gEfiSmmBase2ProtocolGuid,
                  NULL,
                  (VOID **) &mSmmBase2
                  );

  if (EfiInManagementInterrupt ()) {
    //
    // Updates the ERROR_LOG_SETUP_DATA with setup and other platform RELATED information
    //
    Status = InitializeUserConfiguration();
    if (EFI_ERROR(Status))
      return Status;

    //
    // If SystemErrorEn is 00 -> disble error enabling, 01 -> Error Enabling; 02 -> skip error log driver ERROR enbaling or disabling code;
    // Skip the driver if auto option is selected.
    //
    if(mErrLogSetupData.SystemErrorEn == 2)   
      return EFI_SUCCESS;
// Denverton AptioV Override Start - EIP#275412
#if MCA_ERROR_LOGGING
    //
    // Handle the errors occured in the last boot
    //
    HandleLastBootErrors();  
#endif
    ClearSystemErrors();
#if MCA_ERROR_LOGGING
    mAllogCsiErrLogging = FALSE;
#endif
// Denverton AptioV Override End - EIP#275412
    Status = gSmst->SmmRegisterProtocolNotify(
                     &gEfiSmmReadyToLockProtocolGuid, 
                     InitSmmErrorLogCallback, 
                     &mInitSmmErrorLogNotifyReg
                     );
    ASSERT_EFI_ERROR (Status);
  
  }
  UpdateErrorCloaking ();

  return Status;
}



