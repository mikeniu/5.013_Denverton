/*++
  This file contains an 'Intel Peripheral Driver' and is        
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/
/*++

Copyright (c)  1999 - 2017 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:
  
  PlatformStage2.c

Abstract:

  Do platform specific stage2 initializations. 

--*/

#include "PlatformStage2.h"
#include "PlatformBoardId.h"
#include <Library/PeiPlatformHookLib.h>
#include <Library/GpioLib.h>
#include <Library/PchCycleDecodingLib.h>


#define ME_SUPPORT 1
#define    MASK_PM1_EN_STS_RW1C_BITS    0x32CE   //Denverton AptioV Override - EIP#288083

static PEI_PLATFORM_MEMORY_SIZE_PPI  mMemoryMemorySizePpi = {
  GetPlatformMemorySize
};

static PEI_PLATFORM_MEMORY_RANGE_PPI mPlatformMemoryRangePpi = {
  ChoosePlatformMemoryRange
};

static EFI_PEI_PPI_DESCRIPTOR mPpiList[] = {

  {
    EFI_PEI_PPI_DESCRIPTOR_PPI,
    &gPeiPlatformMemorySizePpiGuid,
    &mMemoryMemorySizePpi 
  },
  {
    (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gPeiPlatformMemoryRangePpiGuid,
    &mPlatformMemoryRangePpi 
  },
};

static EFI_PEI_NOTIFY_DESCRIPTOR mNotifyList[] = {
/*++
  {
    EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK,
    &gPeiSmbusPpiGuid,
    PeimInitializeDimm 
  },
  {
    EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK,
    &gEfiPeiSmbusPpiGuid,
    ConfigurePlatformClocks
  },
--*/
  {
    EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK,
    &gEfiPeiSmbusPpiGuid,
    ConfigureIoExpander
  },
  {
    EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK,
    &gEfiPeiMemoryDiscoveredPpiGuid,
    InitIoExpander
  },
  {
    (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gEfiPeiMemoryDiscoveredPpiGuid,
    MemoryDiscoveredPpiNotifyCallback 
  }
};

EFI_GUID mSystemConfigurationGuid = SYSTEM_CONFIGURATION_GUID;

EFI_STATUS
EFIAPI
PeiInitPlatformStage2 (
  IN EFI_PEI_FILE_HANDLE    FfsHeader,
  IN CONST EFI_PEI_SERVICES **PeiServices
  )
/*++

Routine Description:

  Perform the platform spefific initializations.

Arguments:
  
  FfsHeader   - FFS file header pointer of this driver.
  PeiServices - General purpose services available to every PEIM.
    
Returns:

  Status -  EFI_SUCCESS if the interface could be successfully installed.
            EFI_NOT_FOUND if required PPI not found.

--*/
{
  EFI_STATUS                  Status;
  EFI_BOOT_MODE               BootMode;
  EFI_PEI_CPU_IO_PPI          *CpuIo;
  UINT16                      ABase;
  UINT16                      Pm1Sts;
  UINT32                      Pm1Cnt;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI *VariableServices;
  PCH_SERIES                  PchSeries;
  UINTN                       LpcBaseAddress;
  UINTN                       PmcBaseAddress;
  UINT64                      MemSize;
  PCH_POLICY_PPI 			  *PchPolicyPpi;

  //
  // Locate ReadOnly variable Services
  //
  Status = (*PeiServices)->LocatePpi (
             PeiServices,
             &gEfiPeiReadOnlyVariable2PpiGuid,
             0,
             NULL,
             &VariableServices
             );
  ASSERT_EFI_ERROR ( Status);
  //
  // Make sure we have the PPI, if not, just return.
  //
  if ((!VariableServices) || (EFI_ERROR(Status))) {
    return EFI_NOT_FOUND;
  }

  LpcBaseAddress = MmPciBase (
                     DEFAULT_PCI_BUS_NUMBER_PCH,
                     PCI_DEVICE_NUMBER_PCH_LPC,
                     PCI_FUNCTION_NUMBER_PCH_LPC
                     );
  PmcBaseAddress = MmPciBase (
                     DEFAULT_PCI_BUS_NUMBER_PCH,
                     PCI_DEVICE_NUMBER_PCH_PMC,
                     PCI_FUNCTION_NUMBER_PCH_PMC
                     );
  PchSeries             = GetPchSeries();

  //  
  // Produce SA policy PPI here (Only Switchable Graphics)
  //
//DNV_TODO  Status = PlatformSaInit (PeiServices);
//DNV_TODO  ASSERT_EFI_ERROR ( Status);

  //
  // Do basic PCH init 
  //
  Status = PlatformPchInit (PeiServices);
  ASSERT_EFI_ERROR ( Status);

  Status = ClearIchSmiAndWake ();
  ASSERT_EFI_ERROR (Status);

  //
  // Check "ForceResetAfterAcRemoval" variable
  //
  CheckForceResetAfterAcRemoval (PeiServices, VariableServices);

  CpuIo = (**PeiServices).CpuIo;

  Status = (*PeiServices)->GetBootMode (PeiServices, &BootMode);
  ASSERT_EFI_ERROR ( Status);

  mMemoryMemorySizePpi.GetPlatformMemorySize(PeiServices, &mMemoryMemorySizePpi, &MemSize);

  //
  // Platform specific programming. It may have Chipset, KSC or some platform specific
  // programming here.
  //
  Status = PlatformInitStage2 (PeiServices);
  ASSERT_EFI_ERROR ( Status);

  ///
  /// Clear all pending SMI. On S3 clear power button enable so it will not generate an SMI.
  ///
  IoWrite16 (PCH_ACPI_BASE_ADDRESS + R_PCH_ACPI_PM1_EN, 0);
  IoWrite32 (PCH_ACPI_BASE_ADDRESS + R_PCH_ACPI_GPE0_EN_127_96, 0);
  ///----------------------------------------------------------------------------------
  ///
  /// BIOS should check the WAK_STS bit in PM1_STS[15] (PCH register ABASE+00h) before memory
  /// initialization to determine if ME has reset the system while the host was in a sleep state.
  /// If WAK_STS is not set, BIOS should ensure a non-sleep exit path is taken by overwriting
  /// PM1_CNT[12:10] (PCH register ABASE+04h) to 111b to force an s5 exit.
  ///
  PchAcpiBaseGet (&ABase);
  Pm1Sts = IoRead16 (ABase + R_PCH_ACPI_PM1_STS);
  if ((Pm1Sts & B_PCH_ACPI_PM1_STS_WAK) == 0) {
    Pm1Cnt = IoRead32 (ABase + R_PCH_ACPI_PM1_CNT);
    Pm1Cnt |= V_PCH_ACPI_PM1_CNT_S5;
    IoWrite32 (ABase + R_PCH_ACPI_PM1_CNT, Pm1Cnt);
  }

  //
  // Install Platform PPI, in Napa platform, there is a notify register to
  // gPeiPlatformMemoryRangePpiGuid, it will InstallEfiMemory after the PPI 
  // installed except S3 resume boot path. The installation should arranged
  // after GraphicsInit, otherwise, unexpect result will occur.
  //
  Status = (**PeiServices).InstallPpi (PeiServices, mPpiList);
  ASSERT_EFI_ERROR ( Status);

  //
  // After code reorangized, memorycallback will run because the PPI is already 
  // installed when code run to here, it is supposed that the InstallEfiMemory is 
  // done before.
  //
  Status = (**PeiServices).NotifyPpi (PeiServices, mNotifyList);
  ASSERT_EFI_ERROR ( Status);

  //
  // Set what state (S0/S5) to go to when power is re-applied after a power failure (G3 state)
  //
  Status = (**PeiServices).LocatePpi (
    (CONST EFI_PEI_SERVICES**)PeiServices,
    &gPchPolicyPpiGuid,
    0,
    NULL,
    (VOID *)&PchPolicyPpi
    );
  ASSERT_EFI_ERROR (Status);
  SetTheStateToGoAfterG3 ( PchPolicyPpi->PmConfig.StateAfterG3 );
  
 return Status;
}

/**
  Clear any SMI status or wake status left from boot.

  @retval     EFI_SUCCESS       The function completed successfully.
**/
EFI_STATUS
ClearIchSmiAndWake (
  VOID
  )
{
  EFI_STATUS          Status;
  EFI_BOOT_MODE       BootMode;
  UINT16              ABase;
  UINT16              Pm1Sts;
  UINT32              Pm1Cnt;
  UINT32              Data32;
  UINT32              Gpe0Sts;
  UINT32              SmiSts;
  UINT16              DevActSts;
  UINT16              TcoBase;
  UINT16              Tco1Sts;
  UINT16              LpcDeviceId;
  UINTN               LpcBaseAddress;
  DEBUG ((EFI_D_INFO, __FUNCTION__" : Entry\n"));
  Status = PeiServicesGetBootMode (&BootMode);
  ASSERT_EFI_ERROR (Status);
  if (BootMode == BOOT_ON_S3_RESUME) {
    return EFI_SUCCESS;
  }

  LpcBaseAddress = MmPciBase (
                     DEFAULT_PCI_BUS_NUMBER_PCH,
                     PCI_DEVICE_NUMBER_PCH_LPC,
                     PCI_FUNCTION_NUMBER_PCH_LPC
                     );
  PchAcpiBaseGet (&ABase);

  PchTcoBaseGet (&TcoBase);

  //
  // Read the ACPI registers
  //
  Pm1Sts      = IoRead16 (ABase + R_PCH_ACPI_PM1_STS);
  // Denverton AptioV Override Start - EIP#288083
  Pm1Sts = Pm1Sts & MASK_PM1_EN_STS_RW1C_BITS;  //  Masking the Write 1 to Clear bits
  // Denverton AptioV Override End - EIP#288083
  Pm1Cnt      = IoRead32 (ABase + R_PCH_ACPI_PM1_CNT);

  Gpe0Sts = IoRead32 (ABase + R_PCH_ACPI_GPE0_STS_127_96);
  SmiSts  = IoRead32 (ABase + R_PCH_SMI_STS);

  Data32 = 0;

  LpcDeviceId = MmioRead16 (LpcBaseAddress + PCI_DEVICE_ID_OFFSET);
  //
  // Clear AF bit in RegisterC (RTC) by reading it prior to clear RTC_STS bit in PM1_EN_STS (PMC)
  //
  IoWrite8 (R_PCH_RTC_INDEX, (UINT8) R_PCH_RTC_REGC);
  IoRead8 (R_PCH_RTC_TARGET);
  //
  // Clear any SMI or wake state from the boot
  //
  Pm1Sts |=
    (
      //B_PCH_ACPI_PM1_STS_WAK |  // Denverton AptioV Override - EIP#288083 To prevent the WAK_STS bit from getting cleared while checking bootmode after MRC reboot
      B_PCH_ACPI_PM1_STS_PRBTNOR |
      B_PCH_ACPI_PM1_STS_RTC |
      B_PCH_ACPI_PM1_STS_PWRBTN |
      B_PCH_ACPI_PM1_STS_GBL |
      B_PCH_ACPI_PM1_STS_TMROF
    );

  Gpe0Sts |=
    (
      B_PCH_ACPI_GPE0_STS_127_96_PME_B0 |
      B_PCH_ACPI_GPE0_STS_127_96_PME |
      B_PCH_ACPI_GPE0_STS_127_96_PCI_EXP |
      B_PCH_ACPI_GPE0_STS_127_96_RI |
      B_PCH_ACPI_GPE0_STS_127_96_SMB_WAK |
      B_PCH_ACPI_GPE0_STS_127_96_TC0SCI |
      B_PCH_ACPI_GPE0_STS_127_96_HOT_PLUG |
      B_PCH_ACPI_GPE0_STS_127_96_LAN_WAKE
    );

  //
  // PCH Desktop
  //
  if (IS_PCH_LPC_DEVICE_ID_MOBILE (LpcDeviceId)) {
    Gpe0Sts |= B_PCH_ACPI_GPE0_STS_127_96_BATLOW;
  }

  SmiSts |=
    (
      B_PCH_SMI_STS_SMBUS |
      B_PCH_SMI_STS_PERIODIC |
      B_PCH_SMI_STS_TCO |
      B_PCH_SMI_STS_MCSMI |
      B_PCH_SMI_STS_SWSMI_TMR |
      B_PCH_SMI_STS_APM |
      B_PCH_SMI_STS_ON_SLP_EN |
      B_PCH_SMI_STS_BIOS
    );
  //
  // Write them back
  //
  IoWrite16 (ABase + R_PCH_ACPI_PM1_STS,  Pm1Sts);
  DEBUG ((EFI_D_INFO, "\tPM1_EN_STS = 0x%08x\tValue = 0x%08x\n",
    ABase + R_PCH_ACPI_PM1_STS,
    (IoRead16 (ABase + R_PCH_ACPI_PM1_EN) << 16) | IoRead16 (ABase + R_PCH_ACPI_PM1_STS)));
  IoWrite32 (ABase + R_PCH_ACPI_GPE0_STS_127_96, Gpe0Sts);
  DEBUG ((EFI_D_INFO, "\tGPE0_STS_127_96 = 0x%08x\tValue = 0x%08x\n",
    (ABase + R_PCH_ACPI_GPE0_STS_127_96),
    IoRead32 (ABase + R_PCH_ACPI_GPE0_STS_127_96)));
  IoWrite32 (ABase + R_PCH_SMI_STS,  SmiSts);
  
  DevActSts = IoRead16 (ABase + R_PCH_DEVACT_STS);
  Tco1Sts = IoRead16 (TcoBase + R_PCH_TCO1_STS);
  
  DevActSts |=
    (
      B_PCH_DEVACT_STS_KBC |
      B_PCH_DEVACT_STS_PIRQDH |
      B_PCH_DEVACT_STS_PIRQCG |
      B_PCH_DEVACT_STS_PIRQBF |
      B_PCH_DEVACT_STS_PIRQAE
    );
  Tco1Sts |=
    (
      B_PCH_TCO1_STS_DMISERR |
      B_PCH_TCO1_STS_DMISMI |
      B_PCH_TCO1_STS_DMISCI |
      B_PCH_TCO1_STS_BIOSWR |
      B_PCH_TCO1_STS_NEWCENTURY |
      B_PCH_TCO1_STS_TIMEOUT |
      B_PCH_TCO1_STS_TCO_INT |
      B_PCH_TCO1_STS_SW_TCO_SMI
    );
  
  //
  // clear GPI SMI STS
  //
  GpioClearAllGpiSmiSts ();

  IoWrite16 (TcoBase + R_PCH_TCO1_STS, Tco1Sts);

  //
  // We do not want to write 1 to clear INTRD_DET bit.
  //
  IoWrite16 ((UINTN) (TcoBase + R_PCH_TCO2_STS), (UINT16) ~B_PCH_TCO2_STS_INTRD_DET);
  DEBUG ((EFI_D_INFO, "\tTSTS2 = 0x%08x\tValue = 0x%08x\n",TcoBase + R_PCH_TCO2_STS,IoRead16 ((UINTN) (TcoBase + R_PCH_TCO2_STS))));

  IoWrite16 (ABase + R_PCH_DEVACT_STS, DevActSts);
  
  //
  // Clear Power Failure (PWR_FLR), It was used to check S3_Resume State.
  //
  // MmioOr8 (
  //   LpcBaseAddress + R_PCH_PMC_GEN_PMCON_B,
  //   B_PCH_PMC_GEN_PMCON_B_PWR_FLR
  //   );
  DEBUG ((EFI_D_INFO, __FUNCTION__" : Exit\n"));
  return EFI_SUCCESS;
}

EFI_STATUS
SetTheStateToGoAfterG3 (
  IN UINT8 		StateAfterG3
  )
{
    UINT32                          PmconData = 0;
    UINTN                           PmcBaseAddress;
    EFI_STATUS                      Status = EFI_SUCCESS;

    DEBUG ((EFI_D_INFO, __FUNCTION__" : Entry\n"));
    PmcBaseAddress = MmPciBase (
                     DEFAULT_PCI_BUS_NUMBER_PCH,
                     PCI_DEVICE_NUMBER_PCH_PMC,
                     PCI_FUNCTION_NUMBER_PCH_PMC
                     );
    PmconData = MmioRead32 (PmcBaseAddress + R_PCH_PMC_GEN_PMCON_B);
    switch (StateAfterG3) {
      case S0_AFTER_G3:
        //AfterG3 = S0
        PmconData &= (UINT8)~B_PCH_PMC_GEN_PMCON_B_AFTERG3_EN;
        DEBUG ((EFI_D_INFO, "\t'AfterG3' set to 0: System will go to S0\n"));
      break;

      case S5_AFTER_G3:
        //AfterG3 = S5
        PmconData |= B_PCH_PMC_GEN_PMCON_B_AFTERG3_EN;
        DEBUG ((EFI_D_INFO, "\t'AfterG3' set to 1: System will go to S5\n"));
      break;

      case SS_AFTER_G3:
        //AfterG3 = Follow soft strap
        DEBUG ((EFI_D_INFO, "\t'AfterG3' set to 2: System will follow soft strap configuration\n"));
      break;

      default:
        //AfterG3 = Follow soft strap
        DEBUG ((EFI_D_INFO, "\tError on 'AfterG3' setup option\n"));
      break;
    }
    MmioWrite32 (PmcBaseAddress + R_PCH_PMC_GEN_PMCON_B, PmconData);
    DEBUG ((EFI_D_INFO, "\tGEN_PMCON_B = 0x%08x\tValue = 0x%08x\n",PmcBaseAddress + R_PCH_PMC_GEN_PMCON_B,MmioRead32 (PmcBaseAddress + R_PCH_PMC_GEN_PMCON_B)));
    DEBUG ((EFI_D_INFO, __FUNCTION__" : Exit\n"));

  return Status;
}



EFI_STATUS
CheckForceResetAfterAcRemoval (
  IN CONST EFI_PEI_SERVICES                **PeiServices,
  IN       EFI_PEI_READ_ONLY_VARIABLE2_PPI *VariableServices
  )
{
  EFI_PEI_STALL_PPI               *StallPpi;
  EFI_PEI_CPU_IO_PPI              *CpuIo;
  PLATFORM_INFO_HOB               *PlatformInfoHob;
  EFI_STATUS                      Status;
#ifdef ERB_FLAG
  UINTN                           VariableSize;
  UINT16                          DataUint16;
  UINT8                           ForceResetAfterAcRemovalVar;
  UINTN                           PmcBaseAddress;
#endif
  //
  // Locate Variable Ppi
  //
  CpuIo  = (**PeiServices).CpuIo;
  Status = (*PeiServices)->LocatePpi (
                             PeiServices,
                             &gEfiPeiStallPpiGuid,
                             0,
                             NULL,
                             &StallPpi);
  ASSERT_EFI_ERROR (Status);

  //
  // Obtain Platform Info from PlatformInfoHOB.
  //
  Status = GetPlatformInfo (&PlatformInfoHob);
  if (PlatformInfoHob == NULL) {
    return FALSE;
  }
  //
  // Get Setup Variable
  //
#ifdef ERB_FLAG
  if ((PlatformInfoHob->BoardId == BoardIdHarrisBeachFfrd) || (PlatformInfoHob->BoardId == BoardIdWilsonBeachSds)) {
    VariableSize = sizeof (UINT8);
		
    Status = VariableServices->GetVariable ( 
                                  VariableServices, 
                                  L"ForceResetAfterAcRemoval", 
                                  &gEfiGlobalVariableGuid,
                                  NULL,
                                  &VariableSize,
                                  &ForceResetAfterAcRemovalVar
                                  );

    if (!EFI_ERROR (Status)) {
      //
      // Set AC Removal reset in EC if necessary.
      // ForceResetAfterAcRemovalVar == 0 : Clear AC Removal reset bit in EC
      //                                1 : Set AC Removal reset bit in EC
      //
      EcForceResetAfterAcRemoval(PeiServices, CpuIo, StallPpi, ForceResetAfterAcRemovalVar);

      if (ForceResetAfterAcRemovalVar != 0) {
        // Set AfterG3 = S0
        PmcBaseAddress = MmPciBase (
                           DEFAULT_PCI_BUS_NUMBER_PCH,
                           PCI_DEVICE_NUMBER_PCH_PMC,
                           PCI_FUNCTION_NUMBER_PCH_PMC
                           );
        DataUint16 = MmioRead16 (PmcBaseAddress + R_PCH_PMC_GEN_PMCON_3);
        DataUint16 &= ~B_PCH_PMC_GEN_PMCON_AFTERG3_EN;
        MmioWrite16 (PmcBaseAddress + R_PCH_PMC_GEN_PMCON_3, DataUint16);
      }
    }
  }
#endif
  return EFI_SUCCESS;
}

EFI_STATUS
ConfigureIoExpander (
  IN CONST EFI_PEI_SERVICES             **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR          *NotifyDescriptor,
  IN VOID                               *SmbusPpi
  )
/*++

Routine Description:

  Configure the IO Expander smbus device to program Bit0 and Bit1 as output.

Arguments:

  PeiServices             General purpose services available to every PEIM.
  NotifyDescriptor        The notification structure this PEIM registered on install.
  SmbusPpi                Smbus PPI.

Returns:

  EFI_SUCCESS    The function completed successfully.

--*/
{
  UINTN                     Length;
  EFI_SMBUS_DEVICE_COMMAND  Command;
  EFI_SMBUS_DEVICE_ADDRESS  SlaveAddress;
  UINT8                     Data;
  EFI_STATUS                Status;
  PLATFORM_INFO_HOB         *PlatformInfoHob;

  //
  // Obtain Platform Info from HOB.
  //
  Status = GetPlatformInfo (&PlatformInfoHob);
  ASSERT_EFI_ERROR ( Status);
  if (PlatformInfoHob == NULL) {
    return Status;
  }

  //
  // Configure IO Expander device for PPV boards
  //
  if (!EFI_ERROR(Status) 
      && ((PlatformInfoHob->BoardType == BoardTypeCrb)      
      )) {
    SlaveAddress.SmbusDeviceAddress = SMBUS_IO_EXPANDER_ADDRESS >> 1;
    Length = 1;

    Command = 0x3;
    Data = 0xFC; 

    //
    // Configure BIOS_DO_NOT_DISTURB and BIOS_DONE (BIT0, BIT1) as output.
    // As default output value of IO Expander is 0, BIOS_DO_NOT_DISTURB and BIOS_DONE bits will be driven low.
    //
    Status = ((EFI_PEI_SMBUS_PPI*)SmbusPpi)->Execute (
        (EFI_PEI_SERVICES **)PeiServices,
        SmbusPpi,
        SlaveAddress,
        Command,
        EfiSmbusWriteByte,
        FALSE,
        &Length,
        &Data
        );
    ASSERT_EFI_ERROR ( Status);
  }

  return EFI_SUCCESS;
}

EFI_STATUS
InitIoExpander (
  IN CONST EFI_PEI_SERVICES             **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR          *NotifyDescriptor,
  IN VOID                               *MemoryInstalled
  )
/*++

Routine Description:

  Set BIOS_DO_NOT_DISTURB bit of IO Expander to high once MRC is done.

Arguments:

  PeiServices             General purpose services available to every PEIM.
  NotifyDescriptor        The notification structure this PEIM registered on install.
  MemoryInstalled         The memory discovered PPI.  Not used.

Returns:

  EFI_SUCCESS    The function completed successfully.

--*/
{
  EFI_SMBUS_DEVICE_COMMAND  Command;
  EFI_PEI_SMBUS_PPI         *SmbusPpi;
  EFI_SMBUS_DEVICE_ADDRESS  SlaveAddress;
  PLATFORM_INFO_HOB         *PlatformInfoHob;
  UINTN                     Length;
  UINT8                     Data;
  EFI_STATUS                Status;

  //
  // Obtain Platform Info from HOB.
  //
  Status = GetPlatformInfo (&PlatformInfoHob);
  ASSERT_EFI_ERROR ( Status);
  if (PlatformInfoHob == NULL) {
    return Status;
  }

  //
  // Initialize IO Expander device for PPV boards
  //
  if (!EFI_ERROR(Status) 
      && ((PlatformInfoHob->BoardType == BoardTypeCrb)      
      )) {
    //
    // Locate SMBUS PPI
    //
    Status = (**PeiServices).LocatePpi (
                            PeiServices,
                            &gEfiPeiSmbusPpiGuid,
                            0,
                            NULL,
                            &SmbusPpi
                            );

    if (!EFI_ERROR(Status)) {

      SlaveAddress.SmbusDeviceAddress = SMBUS_IO_EXPANDER_ADDRESS >> 1;
      Length = 1;

      //
      // Read Input port register of IO Expander.
      //
      Command = 0x0;
      Status =  SmbusPpi->Execute (
        (EFI_PEI_SERVICES **)PeiServices,
        SmbusPpi,
        SlaveAddress,
        Command,
        EfiSmbusReadByte,
        FALSE,
        &Length,
        &Data
        );
      ASSERT_EFI_ERROR ( Status);

      //
      // Set BIOS_DO_NOT_DISTURB bit to high (Bit0) of IO Expander.
      //
      Data |= BIT0; // BIOS_DO_NOT_DISTURB
      Command = 0x1;
      Status = SmbusPpi->Execute (
        (EFI_PEI_SERVICES **)PeiServices,
        SmbusPpi,
        SlaveAddress,
        Command,
        EfiSmbusWriteByte,
        FALSE,
        &Length,
        &Data
        );
      ASSERT_EFI_ERROR ( Status);
    }
  }

  return EFI_SUCCESS;
}

