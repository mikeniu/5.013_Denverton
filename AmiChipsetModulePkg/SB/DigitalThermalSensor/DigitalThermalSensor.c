//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file DigitalThermalSensor.c
    Digital Thermal Sensor (DTS) driver.
    This SMM driver configures and supports the Digital Thermal Sensor features
    for the platform.
*/


#include "DigitalThermalSensor.h"

//
// Globals used by the reference code
//
EFI_GLOBAL_NVS_AREA       *mGlobalNvsAreaPtr = 0;

//
// Globals used by the DigitalThermalSensor code
//
DTS_EVENT_TYPE                      mBspDigitalThermalSensorThermalEvent = DtsEventNone;
DTS_EVENT_TYPE                      mApDigitalThermalSensorThermalEvent  = DtsEventNone;
UINT8                               mDtsTjMax;

//
// Function implementations
//

/**
  Issue a Shutdown command
**/
VOID
PerformShutdown (
  VOID
)
{
    EFI_RUNTIME_SERVICES 	*SmmRuntimeVar = NULL;
    UINT8 Index;
    
    for (Index = 0; Index < gSmst->NumberOfTableEntries; ++Index) {
        if (guidcmp(&gSmst->SmmConfigurationTable[Index].VendorGuid, \
                            &SmmRsTableGuid) == 0) {
            break;
        }
    }
    if (Index != gSmst->NumberOfTableEntries) {
         SmmRuntimeVar =(EFI_RUNTIME_SERVICES *) gSmst->SmmConfigurationTable[Index].VendorTable;
    }
    if(SmmRuntimeVar){
       SmmRuntimeVar->ResetSystem(EfiResetShutdown, EFI_SUCCESS, 0, NULL);
    }
}

/**
  Read the temperature data per core/thread.
  This function must be AP safe.

  @param[in] Buffer        Pointer to UINT8 to update with the current temperature
**/

VOID
EFIAPI
DigitalThermalSensorUpdateTemperature (
  VOID *Buffer
  )
{
  MSR_REGISTER       MsrData;

  //
  // Read the temperature
  //
  MsrData.Qword = AsmReadMsr64 (EFI_MSR_IA32_THERM_STATUS);

  //
  // Find the DTS temperature.
  //
  *(UINT8 *)Buffer = mDtsTjMax - (MsrData.Bytes.ThirdByte & OFFSET_MASK);
  return;
}

/**
  SMI handler to handle DigitalThermalSensor CPU Local APIC SMI 
  for thermal threshold interrupt 

  @param DispatchHandle  The unique handle assigned to this handler by SmiHandlerRegister().
  @param Context         Points to an optional handler context which was specified when the
                         handler was registered.
  @param CommBuffer      A pointer to a collection of data in memory that will
                         be conveyed from a non-SMM environment into an SMM environment.
  @param CommBufferSize  The size of the CommBuffer.

  @retval EFI_SUCCESS Command is handled successfully.

**/
EFI_STATUS
EFIAPI
DtsSmiCallback (
  IN EFI_HANDLE             DispatchHandle,
  IN     CONST VOID         *Context,         OPTIONAL
  IN OUT VOID               *CommBuffer,      OPTIONAL
  IN OUT UINTN              *CommBufferSize   OPTIONAL
  )
{
  UINTN                       Index;
  UINT8                       Temperature = 0;

  //
  // We enable the Thermal interrupt on the AP's prior to the event check
  // for the case where the AP has gone through the INIT-SIPI-SIPI sequence
  // and does not have the interrupt enabled.  (This allows the AP thermal
  // interrupt to be re-enabled due to chipset-based SMIs without waiting
  // to receive a DTS event on the BSP.)
  //
  for (Index = 1; Index < gSmst->NumberOfCpus; Index++) {
	  gSmst->SmmStartupThisAp (DigitalThermalSensorEnableSmi, Index, NULL);
  }

  //
  // Check is this a DTS SMI event or the flag of update DTS temperature and threshold value in every SMI   
  //
  if (DigitalThermalSensorEventCheck() || UPDATE_DTS_EVERY_SMI) {
    //
    // Disable Local APIC SMI before programming the threshold  
    // 
    DigitalThermalSensorDisableSmi (NULL);
    for (Index = 1; Index < gSmst->NumberOfCpus; Index++) {
      gSmst->SmmStartupThisAp (DigitalThermalSensorDisableSmi, Index, NULL);
    }

    //
    // Handle OutOfSpec Events
    //
    if ((mBspDigitalThermalSensorThermalEvent == DtsEventOutOfSpec)|| (mApDigitalThermalSensorThermalEvent == DtsEventOutOfSpec)) {
      PerformShutdown();
    }
    
    //
    // Update the GlobalNvs Temperature values
    //
    
    //
    // Note:  We only save the highest temperature of each die in
    // the NVS area when more than two logical processors are
    // present as only the highest DTS reading is actually used by
    // the current ASL solution.
    //
    mGlobalNvsAreaPtr->BspDigitalThermalSensorTemperature = 0;
    mGlobalNvsAreaPtr->ApDigitalThermalSensorTemperature  = 0;
    
    DigitalThermalSensorUpdateTemperature(&mGlobalNvsAreaPtr->BspDigitalThermalSensorTemperature);
    for (Index = 1; Index < gSmst->NumberOfCpus; Index++) {
      gSmst->SmmStartupThisAp (DigitalThermalSensorUpdateTemperature, Index, &Temperature);
      if(Temperature > mGlobalNvsAreaPtr->ApDigitalThermalSensorTemperature)
    	  mGlobalNvsAreaPtr->ApDigitalThermalSensorTemperature = Temperature;
    }
    
    //
    // Signals SWGPE only in DTS event, else it would cause wake event
    // Set SWGPE Status to generate an SCI if we had any events
    //   
    if (mBspDigitalThermalSensorThermalEvent || mApDigitalThermalSensorThermalEvent) {
      //
      // Set SWGPE Status to generate an SCI if we had any events. trigger GPE._L0B, which notifies TZ01
      //  
      DigitalThermalSensorSetSwGpeSts (); 
    }

    //
    // Enable Local APIC SMI
    // 
    for (Index = 1; Index < gSmst->NumberOfCpus; Index++) {
      gSmst->SmmStartupThisAp (DigitalThermalSensorEnableSmi, Index, NULL);
    }
    DigitalThermalSensorEnableSmi (NULL);
  }

  return EFI_SUCCESS;
}


/**
  Generates a _GPE._L02 SCI to an ACPI OS.
**/
VOID
DigitalThermalSensorSetSwGpeSts (
  VOID
  )
{
  UINT16      AcpiBase;
  UINT8       Data;
  UINT32      Data32;
  EFI_STATUS  Status;
  //
  // Get the PM Base Address
  //
  AcpiBase = PciRead16 (
                  PCI_LIB_ADDRESS (0,
                  PCI_DEVICE_NUMBER_PCH_LPC,
                  PCI_FUNCTION_NUMBER_PCH_PMC,	
                  R_PCH_LPC_ACPI_BASE)
                  );
  //
  // Base Address should be in bits 15:7 of ACPIBASE register 
  // so they use the whole 16 bit
  //
  AcpiBase = (UINT16) (AcpiBase & B_PCH_LPC_ACPI_BASE_BAR);
  
  
  // Set the SWGPE_EN bit to 1
  Status = gSmst->SmmIo.Io.Read (&gSmst->SmmIo, SMM_IO_UINT32, (AcpiBase + R_PCH_ACPI_GPE0_EN_127_96), 1, &Data32);
  Data32 |= B_PCH_ACPI_GPE0_EN_127_96_SWGPE; 
  Status = gSmst->SmmIo.Io.Write (&gSmst->SmmIo, SMM_IO_UINT32, (AcpiBase + R_PCH_ACPI_GPE0_EN_127_96), 1, &Data32);
  
  //
  // Check SCI enable
  //
  Status = gSmst->SmmIo.Io.Read (&gSmst->SmmIo, SMM_IO_UINT32, (AcpiBase + R_PCH_ACPI_PM1_CNT), 1, &Data32);
  if ((Data32 & B_PCH_ACPI_PM1_CNT_SCI_EN) != 0) {
    //
    // Trigger SCI
    //
  
    // Set the SWGPE_CTRL bit to 1
    Status = gSmst->SmmIo.Io.Read (&gSmst->SmmIo, SMM_IO_UINT32, (AcpiBase + R_PCH_ACPI_GPE_CNTL), 1, &Data32);
    Data32 |= B_PCH_ACPI_GPE_CNTL_SWGPE_CTRL; 
    Status = gSmst->SmmIo.Io.Write (&gSmst->SmmIo, SMM_IO_UINT32, (AcpiBase + R_PCH_ACPI_GPE_CNTL), 1, &Data32);
  
    //
    // Status Check
    //
    while(TRUE) {
    	gSmst->SmmIo.Io.Read (&gSmst->SmmIo, SMM_IO_UINT8, AcpiBase + R_PCH_ACPI_GPE0_STS_127_96, 1, &Data );
      if(Data & B_PCH_ACPI_GPE0_STS_127_96_SWGPE)  {
        Data = 0xff;
        while (Data--);
        //
        // Set the SWGPE_CTRL bit to 0
        //
        Status = gSmst->SmmIo.Io.Read (&gSmst->SmmIo, SMM_IO_UINT32, (AcpiBase + R_PCH_ACPI_GPE_CNTL), 1, &Data32);
        Data32 &= ~B_PCH_ACPI_GPE_CNTL_SWGPE_CTRL; 
        Status = gSmst->SmmIo.Io.Write (&gSmst->SmmIo, SMM_IO_UINT32, (AcpiBase + R_PCH_ACPI_GPE_CNTL), 1, &Data32);
        break;
      }
    }
  }
}
/**
  Checks for a Core Thermal Event on any processor


  @retval TRUE means this is a DTS Thermal event
  @retval FALSE means this is not a DTS Thermal event.
**/
BOOLEAN
DigitalThermalSensorEventCheck (
  VOID
  )
/*++

Routine Description:

  Checks for a Core Thermal Event on any processor

Arguments:
  
  None

Returns:

  TRUE means this is a DTS Thermal event 
  FALSE means this is not a DTS Thermal event.

--*/
{
  //
  // Clear event status
  //
  mBspDigitalThermalSensorThermalEvent = DtsEventNone;
  mApDigitalThermalSensorThermalEvent  = DtsEventNone;
  //
  // Check BSP thermal event
  //   
  DigitalThermalSensorEventCheckMsr (&mBspDigitalThermalSensorThermalEvent);  

  //
  // Check AP thermal event
  //  
  gSmst->SmmStartupThisAp (DigitalThermalSensorEventCheckMsr, 1, &mApDigitalThermalSensorThermalEvent);

  if (mBspDigitalThermalSensorThermalEvent > DtsEventNone || mApDigitalThermalSensorThermalEvent > DtsEventNone) {
    return TRUE;
  }
  return FALSE;
}  

/**
  Checks for a Core Thermal Event by reading MSR.

  This function must be MP safe.

  @param[in] Buffer    Pointer to DTS_EVENT_TYPE
**/
VOID
EFIAPI
DigitalThermalSensorEventCheckMsr (
  IN  VOID        *Buffer
  )
{
  MSR_REGISTER                MsrData;

  //
  // Read thermal status
  //
  MsrData.Qword = AsmReadMsr64 (EFI_MSR_IA32_THERM_STATUS);

  //
  // Check for out of spec status
  //
  if (MsrData.Qword & B_OUT_OF_SPEC_STATUS_LOG) {
    *(DTS_EVENT_TYPE*) Buffer = DtsEventOutOfSpec;
    return;
  }

  //
  // Check thresholds 
  //    
  if (MsrData.Qword & B_THERMAL_THRESHOLD_1_STATUS_LOG) {
    *(DTS_EVENT_TYPE*) Buffer = DtsEventThreshold1;
    return;
  }
  if (MsrData.Qword & B_THERMAL_THRESHOLD_2_STATUS_LOG) {
    *(DTS_EVENT_TYPE*) Buffer = DtsEventThreshold2;
    return;
  }
}

/**
  Enables the Thermal Interrupt in the core Local APIC.

  @param[in] Buffer        Unused

  @retval EFI_SUCCESS   Enable Local APIC to generate a SMI successfully
**/
VOID
EFIAPI
DigitalThermalSensorEnableSmi (
  IN VOID          *Buffer
  )
{
  UINT32  ApicThermalValue;
  BOOLEAN x2ApicEnabled;
  x2ApicEnabled = (BOOLEAN) (((AsmReadMsr64 (EFI_MSR_IA32_APIC_BASE)) & (BIT11 + BIT10)) == BIT11 + BIT10);
  //
  // Configure the Local APIC to generate an SMI on Thermal events.  First,
  // Clear BIT16, BIT10-BIT8, BIT7-BIT0.  Then, set BIT9 (delivery mode).
  // Don't enable the interrupt if it's already enabled
  //
  if (x2ApicEnabled) {
    ApicThermalValue = (UINT32) AsmReadMsr64 (EFI_MSR_EXT_XAPIC_LVT_THERM);
  } else {
    ApicThermalValue = *(UINT32 *) (UINTN) LOCAL_APIC_THERMAL_DEF;
  }

  if ((ApicThermalValue & (B_INTERRUPT_MASK | B_DELIVERY_MODE | B_VECTOR)) != V_MODE_SMI) {
    ApicThermalValue = (ApicThermalValue &~(B_INTERRUPT_MASK | B_DELIVERY_MODE | B_VECTOR)) | V_MODE_SMI;
    if (x2ApicEnabled) {
      AsmWriteMsr64 (EFI_MSR_EXT_XAPIC_LVT_THERM, ApicThermalValue);
    } else {
      *(UINT32 *) (UINTN) (LOCAL_APIC_THERMAL_DEF) = (UINT32) ApicThermalValue;
    }
  }

  return;
}
  
/**
  Disables the Thermal Interrupt in the core Local APIC.

  @param[in] Buffer        Unused

  @retval EFI_SUCCESS   Disable Local APIC to generate a SMI successfully
**/
VOID
EFIAPI
DigitalThermalSensorDisableSmi (
  IN  VOID          *Buffer
  )
{
  UINT32  ApicThermalValue;
  BOOLEAN x2ApicEnabled;
  x2ApicEnabled = (BOOLEAN) (((AsmReadMsr64 (EFI_MSR_IA32_APIC_BASE)) & (BIT11 + BIT10)) == BIT11 + BIT10);

  if (x2ApicEnabled) {
    ApicThermalValue = (UINT32) AsmReadMsr64 (EFI_MSR_EXT_XAPIC_LVT_THERM);
  } else {
    ApicThermalValue = *(UINT32 *) (UINTN) LOCAL_APIC_THERMAL_DEF;
  }

  //
  // Following descriptions were from SSE BIOS
  // We set the interrupt mode at the same time as the interrupt is disabled to
  // avoid the "Received Illegal Vector" being set in the Error Status Register.
  //  and eax, 0FFFEF800h
  //  or  eax, 000010200h     ; Clear Mask, Set Delivery
  //
  ApicThermalValue = (ApicThermalValue &~(B_INTERRUPT_MASK | B_DELIVERY_MODE | B_VECTOR)) | (B_INTERRUPT_MASK | V_MODE_SMI);
  if (x2ApicEnabled) {
    AsmWriteMsr64 (EFI_MSR_EXT_XAPIC_LVT_THERM, ApicThermalValue);
  } else {
    *(UINT32 *) (UINTN) (LOCAL_APIC_THERMAL_DEF) = (UINT32) ApicThermalValue;
  }

  return;
}

/**
  Perform first time initialization of the Digital Thermal Sensor

  @retval EFI_SUCCESS  Init Digital Thermal Sensor successfully
**/
EFI_STATUS
DigitalThermalSensorInit (
  VOID
  )
{
  MSR_REGISTER                   MsrData;
  UINTN                          Index;
  
  //
  // Get the TCC Activation Temperature and use it for TjMax.
  //
  MsrData.Qword         = AsmReadMsr64 (EFI_MSR_CPU_THERM_TEMPERATURE);
  mDtsTjMax             = (MsrData.Bytes.ThirdByte);
  
  // The Default Critical temperature is dependent 
  // upon on the processor.
  mGlobalNvsAreaPtr->CriticalThermalTripPoint = mDtsTjMax;
  
  //
  // Enable the Local APIC SMI on all logical processors
  // 
  for (Index = 1; Index < gSmst->NumberOfCpus; Index++) {
    gSmst->SmmStartupThisAp (DigitalThermalSensorEnableSmi, Index, NULL);
  }
  DigitalThermalSensorEnableSmi (NULL);
  
  //
  // Set DigitalThermalSensor flag in ACPI NVS
  //
  mGlobalNvsAreaPtr->EnableDigitalThermalSensor = 1;
  
  return EFI_SUCCESS;
}

/**
  Digital Thermal Sensor (DTS) SMM driver entry point function.

  @param[in] ImageHandle   Image handle for this driver image
  @param[in] SystemTable   Pointer to the EFI System Table

  @retval EFI_SUCCESS   Driver initialization completed successfully
**/
EFI_STATUS
InstallDigitalThermalSensor (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
  EFI_STATUS                     Status = EFI_SUCCESS;
  EFI_GLOBAL_NVS_AREA_PROTOCOL   *GlobalNvsAreaProtocol;
  EFI_HANDLE                     DispatchHandle;

  InitAmiLib(ImageHandle, SystemTable);

  //
  // Locate our shared data area
  //
  Status = pBS->LocateProtocol (&gEfiGlobalNvsAreaProtocolGuid, NULL, &GlobalNvsAreaProtocol);
  ASSERT_EFI_ERROR (Status);
  
  mGlobalNvsAreaPtr = GlobalNvsAreaProtocol->Area;

  //
  // Register a callback function to handle DigitalThermalSensor SMIs.
  //
  Status  = gSmst->SmiHandlerRegister (DtsSmiCallback, NULL,  &DispatchHandle);
  ASSERT_EFI_ERROR (Status);  
  
  DigitalThermalSensorInit();

  return Status;
}

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
