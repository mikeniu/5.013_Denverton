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

  PlatformStage1.c
  
Abstract:
  
  Do platform specific stage1 initializations.

--*/


#include "PlatformStage1.h"
#include "SmBiosV234.h"
#include "PlatformDefinitions.h"
#include "PlatformFlashMap.h"
#include <Include/SetupVariable.h>

#include <Library/PchCycleDecodingLib.h>
#include <Library/DebugPrintErrorLevelLib.h>
#include <Library/BiosIdLib.h>
#include <IndustryStandard/SmBios.h>
#include "SimicsDebug.h"
//
// If the debugger branch trace/data store debug feature is enabled
// This is an optional debug feature as enabling it will introduce
// significant changes in performance and system resources
//
#ifdef DEBUG_MODE
#include "DsAllocation.h"
#endif

#define    MASK_PM1_EN_STS_RW1C_BITS    0x32CE   //Denverton AptioV Override - EIP#288083

extern
EFI_STATUS
PrioritizeBootMode (
  IN OUT  EFI_BOOT_MODE     *CurrentBootMode,
  IN      EFI_BOOT_MODE     NewBootMode
  );

static EFI_GUID    mSystemConfigurationGuid  = SYSTEM_CONFIGURATION_GUID;


EFI_STATUS
PlatformInfoPeiInit (
  IN EFI_PEI_FILE_HANDLE       FileHandle,
  IN CONST EFI_PEI_SERVICES    **PeiServices
  );

VOID
SysControlConfigComunication();

VOID
GetSecPerformance (
  IN CONST EFI_PEI_SERVICES **PeiServices
  );
// Denverton AptioV override start - EIP#235962
// This function is used to load the setup defaults, In AMI we are doing this in the NVRAM driver
/*
EFI_STATUS
EFIAPI
PlatformStage1VariableInit (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Interface
  );
*/
// Denverton AptioV override End - EIP#235962
#define  HBPciRegBase            PCIEX_BASE_ADDRESS + (UINT32) (0 << 15) + (UINT32) (0 << 12)

//
// Value from PCCG BIOS
//
#define  SSVID_DID               0x19998086

static PEI_BASE_MEMORY_TEST_PPI mPeiBaseMemoryTestPpi = {
  BaseMemoryTest
};

static EFI_PEI_STALL_PPI mStallPpi = {
  PEI_STALL_RESOLUTION,
  Stall
};

static EFI_PEI_RESET_PPI mResetPpi = {
  PchReset
};

static EFI_PEI_PPI_DESCRIPTOR mInstallStallPpi = {
    EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
    &gEfiPeiStallPpiGuid,
    &mStallPpi 
};

static EFI_PEI_PPI_DESCRIPTOR mPpiList[] = {
  { 
    EFI_PEI_PPI_DESCRIPTOR_PPI,
    &gEfiPeiResetPpiGuid,
    &mResetPpi 
  },
  { 
    EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
    &gPeiBaseMemoryTestPpiGuid,
    &mPeiBaseMemoryTestPpi
  }
};
// Denverton AptioV override start - EIP#235962
// This function is used to load the setup defaults, In AMI we are doing this in the NVRAM driver
/*
static EFI_PEI_NOTIFY_DESCRIPTOR mVariableNotifyList = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiPeiReadOnlyVariable2PpiGuid,
  PlatformStage1VariableInit
};
*/
// Denverton AptioV override end - EIP#235962

EFI_STATUS
EFIAPI
PeiInitPlatformStage1 (
  IN EFI_PEI_FILE_HANDLE      FileHandle,
  IN CONST EFI_PEI_SERVICES   **PeiServices
  )
/*++

Routine Description:
  
  Platform specific initializations in stage1.
  
Arguments:
  
  FfsHeader         Pointer to the PEIM FFS file header.
  PeiServices       General purpose services available to every PEIM.
    
Returns:
  
  EFI_SUCCESS       Operation completed successfully.
  Otherwise         Platform initialization failed.

  Peform the boot mode determination logic

--*/  
{
  EFI_STATUS                    Status;
  EFI_PEI_CPU_IO_PPI            *CpuIo;
  UINT8                         FwConfig;
  // CHAR16                        Version[SMBIOS_STRING_MAX_LENGTH]; // Denverton AptioV override
  PLATFORM_INFO_HOB             *PlatformInfoHob;
  PCH_POLICY_PPI              *PchPolicyPpi;

  SetDebugPrintErrorLevel( 0x80400042 );

//
// Allocate an initial buffer from heap for debugger use
//
#ifdef DEBUG_MODE
  DsAllocation ((EFI_PEI_SERVICES **)PeiServices);  
#endif

  CpuIo    = (**PeiServices).CpuIo;

  //
  // program timer 1 as refresh timer
  //
  (CpuIo->IoWrite8) (
          PeiServices,
          CpuIo,
          0x43,
          0x54
          );

  (CpuIo->IoWrite8) (
          PeiServices,
          CpuIo,
          0x41,
          0x12
          );

  // Denverton Aptiov override Start
  //
  // Get BIOS ID
  //
  // Status = GetBiosVersionDateTime (Version, NULL, NULL); /*This call creates a HOB for use in DXE*/
  // ASSERT_EFI_ERROR (Status);
  // Denverton Aptiov override End

  //
  // Initialize platform PPIs
  //
  Status = (**PeiServices).InstallPpi (PeiServices, &mInstallStallPpi);
  ASSERT_EFI_ERROR (Status);

  //
  // Program HECI BAR
  //
  MmioWrite32 (CSME_HECI1PciRegBase + 0x10, HECIF0_BASE_ADDRESS | BIT0);
  MmioWrite32 (CSME_HECI1PciRegBase + 0x04, 0x06);
  
  MmioWrite32 (CSME_HECI2PciRegBase + 0x10, HECIF1_BASE_ADDRESS | BIT0);
  MmioWrite32 (CSME_HECI2PciRegBase + 0x04, 0x06);

  //
  // Program SSVID/SSID on various devices
  //
  MmioWrite32 (HBPciRegBase + 0x2C, SSVID_DID);

  //
  // Do Early PCH init 
  //
  EarlyPlatformPchInit (PeiServices);

  //
  // Get Board information as early as possible
  //
  Status = PlatformInfoPeiInit (FileHandle, PeiServices);
  ASSERT_EFI_ERROR ( Status);
  //
  // Initialize platform policies
  //
  FwConfig = FwConfigProduction;

// Denverton AptioV override start - EIP#235962
// This function is used to load the setup defaults, In AMI we are doing this in the NVRAM driver
/*
  //
  // Register notify to set default variable once variable service is ready.
  //
  Status = (**PeiServices).NotifyPpi (PeiServices, &mVariableNotifyList);
  ASSERT_EFI_ERROR ( Status);
*/
// Denverton AptioV override End - EIP#235962
  PeiPolicyInitPreMem (FwConfig);
  //
  // Platform specific programming
  //
  Status = PlatformInitStage1 (PeiServices);
  ASSERT_EFI_ERROR ( Status);

  //
  // Create the platform Flash Map
  //
  // Denverton AptioV Overrides Start - EIP#234259
  //Status = PeimInitializeFlashMap (FileHandle, PeiServices);
  //ASSERT_EFI_ERROR (Status);
  // Denverton AptioV Overrides End - EIP#234259

  
  DEBUG ((DEBUG_INFO, "GetWakeupEventAndSaveToHob ()\n"));
  GetWakeupEventAndSaveToHob();

  PlatformStage1InitBootMode (PeiServices);

  //
  // Move ResetEnd data to Hob
  //
  GetSecPerformance (PeiServices);

  //
  // Initialize platform PPIs
  //
  Status = (**PeiServices).InstallPpi (PeiServices, &mPpiList[0]);
  ASSERT_EFI_ERROR ( Status);

  Status = (**PeiServices).LocatePpi (
  (CONST EFI_PEI_SERVICES**)PeiServices,
  &gPchPolicyPpiGuid,
  0,
  NULL,
  (VOID *)&PchPolicyPpi
  );

  ASSERT_EFI_ERROR (Status);

  GetPlatformInfo (&PlatformInfoHob);
  if (PlatformInfoHob != NULL)
  {
      if(!PEI_IN_SIMICS && PlatformInfoHob->BoardId == BOARD_ID_OstrichBay)
      {
#ifndef PO_FLAG
          SysControlConfigComunication();
#endif //ifndef PO_FLAG
      }
  }

  return Status;
}

/**
  Get sleep type after wakeup

  @param[out] SleepType  Sleep type to be returned.

  @retval     TRUE       A wake event occured without power failure.
  @retval     FALSE      Power failure occured or not a wakeup.
**/
BOOLEAN
GetSleepTypeAfterWakeup (
  OUT UINT32            *SleepType
  )
{
  UINT16                Pm1Sts;
  UINT32                Pm1Cnt;

  ///
  /// Read the ACPI registers
  ///
  Pm1Sts  = IoRead16 (PCH_ACPI_BASE_ADDRESS + R_PCH_ACPI_PM1_STS);
  Pm1Cnt  = IoRead32 (PCH_ACPI_BASE_ADDRESS + R_PCH_ACPI_PM1_CNT);

  ///
  /// Get sleep type if a wake event occurred and there is no power failure and reset
  ///
  if ((Pm1Sts & B_PCH_ACPI_PM1_STS_WAK) != 0) {
    if((MmioRead16 (
          MmPciBase (
          DEFAULT_PCI_BUS_NUMBER_PCH,
          PCI_DEVICE_NUMBER_PCH_PMC,
          PCI_FUNCTION_NUMBER_PCH_PMC) + R_PCH_PMC_GEN_PMCON_B) & (B_PCH_PMC_GEN_PMCON_B_RTC_PWR_STS | B_PCH_PMC_GEN_PMCON_B_PWR_FLR)) == 0)
    {
      *SleepType = Pm1Cnt & B_PCH_ACPI_PM1_CNT_SLP_TYP;
      return  TRUE;
    }
    else {
      ///
      /// Clear Wake Status (WAK_STS) and Sleep Type (SLP_TYP)
      ///
      IoWrite16 (PCH_ACPI_BASE_ADDRESS + R_PCH_ACPI_PM1_STS, B_PCH_ACPI_PM1_STS_WAK);
      Pm1Cnt &= ~B_PCH_ACPI_PM1_CNT_SLP_TYP;
      IoWrite32 (PCH_ACPI_BASE_ADDRESS + R_PCH_ACPI_PM1_CNT, Pm1Cnt);
      return  FALSE;
    }
  }
  return  FALSE;
}

/**
  Parse the status registers for figuring out the wake-up event and save it into
  an GUID HOB which will be referenced later. However, modification is required
  to meet the chipset register definition and the practical hardware design. Thus,
  this is just an example.
**/
VOID
GetWakeupEventAndSaveToHob (
  VOID
  )
{
  UINT16             Pm1Sts;
  UINTN              Gpe0Sts;
  UINTN              Gpe0PmeSts;
  UINTN              Gpe0RiSts;
  UINTN              WakeEventData;

  Gpe0Sts    = 0;
  Gpe0RiSts  = 0;
  Gpe0PmeSts = 0;

  ///
  /// Read the ACPI registers
  ///
  Pm1Sts     = IoRead16 (PCH_ACPI_BASE_ADDRESS + R_PCH_ACPI_PM1_STS);
  Gpe0Sts    = IoRead32 (PCH_ACPI_BASE_ADDRESS + R_PCH_ACPI_GPE0_STS_127_96);
  Gpe0RiSts  = Gpe0Sts & B_PCH_ACPI_GPE0_STS_127_96_RI;
  Gpe0PmeSts = Gpe0Sts & B_PCH_ACPI_GPE0_STS_127_96_PME;

  DEBUG ((DEBUG_INFO, "Gpe0Sts is: %02x\n", Gpe0Sts));

  ///
  /// Figure out the wake-up event
  ///
  if ((Pm1Sts & B_PCH_ACPI_PM1_EN_PWRBTN) != 0 ) {
    WakeEventData = SMBIOS_WAKEUP_TYPE_POWER_SWITCH;
  } else if (((Pm1Sts & B_PCH_ACPI_PM1_STS_WAK) != 0) || (Gpe0PmeSts != 0)) {
    WakeEventData = SMBIOS_WAKEUP_TYPE_PCI_PME;
  } else if (Gpe0RiSts != 0) {
    WakeEventData = SMBIOS_WAKEUP_TYPE_MODEM_RING;
  } else if (Gpe0Sts != 0) {
    WakeEventData = SMBIOS_WAKEUP_TYPE_OTHERS;
  } else {
    WakeEventData = SMBIOS_WAKEUP_TYPE_UNKNOWN;
  }

  DEBUG ((DEBUG_INFO, "ACPI Wake Status Register: %04x\n", Pm1Sts));
}


VOID
clearPowerButtonSts (
  VOID
  )
/*++

Routine Description:

  Clears the power button status bit

Arguments:

Returns:

  None.

--*/
{
  UINT16 Data16;
  DEBUG ((EFI_D_INFO,"PM: Clearing power Button status...\n"));
  //
  // Read and check the ACPI registers
  //
  Data16 = IoRead16(PM_BASE_ADDRESS + R_PCH_ACPI_PM1_STS);
  
  // Denverton AptioV Override Start - EIP#288083
  Data16 = Data16 & MASK_PM1_EN_STS_RW1C_BITS;  //  Masking the Write 1 to Clear bits
  // Denverton AptioV Override End - EIP#288083
  
  Data16 |= B_PCH_ACPI_PM1_STS_PWRBTN;
  IoWrite16 (PM_BASE_ADDRESS + R_PCH_ACPI_PM1_STS, Data16);
  DEBUG ((EFI_D_INFO,"PM:0x%d: %x\n",Data16));

}

VOID
EarlyPlatformPchInit (
  IN CONST EFI_PEI_SERVICES    **PeiServices
  )
{
  UINT16                          Data16;
  UINT8                           Data8;
  EFI_PEI_CPU_IO_PPI              *CpuIo;
  UINTN                           LpcBaseAddress;
  UINTN                           PmcBaseAddress;
  UINTN                           P2sbBase;
  UINTN                           SpiBase;

  DEBUG ((EFI_D_INFO, __FUNCTION__" : Entry\n"));

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

  //
  // Program bar
  //
  P2sbBase = MmPciBase (
               DEFAULT_PCI_BUS_NUMBER_PCH,
               PCI_DEVICE_NUMBER_PCH_P2SB,
               PCI_FUNCTION_NUMBER_PCH_P2SB
               );

  MmioWrite32 (P2sbBase + R_PCH_P2SB_SBREG_BAR, PCH_PCR_BASE_ADDRESS);
  MmioOr8 (P2sbBase + PCI_COMMAND_OFFSET, EFI_PCI_COMMAND_MEMORY_SPACE);

  SpiBase  = MmPciBase (
               DEFAULT_PCI_BUS_NUMBER_PCH,
               PCI_DEVICE_NUMBER_PCH_P2SB,
               PCI_FUNCTION_NUMBER_PCH_SPI
               );

  MmioWrite32 (SpiBase + R_PCH_SPI_BAR0, PCH_SPI_BASE_ADDRESS);
  MmioOr8 (SpiBase + PCI_COMMAND_OFFSET, EFI_PCI_COMMAND_MEMORY_SPACE);

  ///
  /// Program PMBASE in PMC Device
  ///
  MmioAndThenOr32 (
    PmcBaseAddress + R_PCH_PMC_ACPI_BASE,
    (UINT32) (~B_PCH_PMC_ACPI_BASE_BAR),
    PCH_ACPI_BASE_ADDRESS
    );
  ///
  /// Enable PMBASE in PMC Device
  ///
  MmioOr8 (
    PmcBaseAddress + R_PCH_PMC_ACPI_CNT,
    B_PCH_PMC_ACPI_CNT_ACPI_EN
    );
  ///
  /// Program PWRMBASE in PMC Device
  ///
  PchPwrmBaseSet(PCH_PWRM_BASE_ADDRESS);

  ///
  /// Program TCO BASE
  ///
  PchTcoBaseSet (PCH_TCO_BASE_ADDRESS);

  ///
  /// LPC I/O Configuration
  ///
  PchLpcIoDecodeRangesSet (
    (V_PCH_LPC_IOD_LPT_378  << N_PCH_LPC_IOD_LPT)  |
    (V_PCH_LPC_IOD_COMB_2F8 << N_PCH_LPC_IOD_COMB) |
    (V_PCH_LPC_IOD_COMA_3F8 << N_PCH_LPC_IOD_COMA)
    );

  PchLpcIoEnableDecodingSet (
    B_PCH_LPC_IOE_ME2  |
    B_PCH_LPC_IOE_SE   |
    B_PCH_LPC_IOE_ME1  |
    B_PCH_LPC_IOE_KE   |
    B_PCH_LPC_IOE_HGE  |
    B_PCH_LPC_IOE_LGE  |
    B_PCH_LPC_IOE_FDE  |
    B_PCH_LPC_IOE_PPE
//    B_PCH_LPC_IOE_CBE  |
//    B_PCH_LPC_IOE_CAE
    );

  CpuIo  = (**PeiServices).CpuIo;

  //
  // Enable the upper 128-byte bank of RTC RAM
  //
  MmioOr32 (PCH_PCR_ADDRESS (PID_RTC, R_PCH_PCR_RTC_CONF), B_PCH_PCR_RTC_CONF_UCMOS_EN);

  //
  // Disable the Watchdog timer expiration from causing a system reset
  //
  //  MmioOr32 (PCH_PCR_ADDRESS (PID_ITSS, R_PCH_PCR_ITSS_GIC), B_PCH_PCR_ITSS_GIC_AME);      // Denverton AptioV Override Start- EIP#280221

  //
  // Halt the TCO timer 
  //
  Data16 = ((**PeiServices).CpuIo->IoRead16) (
                    PeiServices,
                    (**PeiServices).CpuIo,
                    PCH_TCO_BASE_ADDRESS + R_PCH_TCO1_CNT
                    );
  Data16 |= B_PCH_TCO_CNT_TMR_HLT;
  ((**PeiServices).CpuIo->IoWrite16) (
           PeiServices, 
           (**PeiServices).CpuIo, 
           PCH_TCO_BASE_ADDRESS + R_PCH_TCO1_CNT,
           Data16
           );
  
  //
  // Clear the Second TO status bit
  //
  Data8 = ((**PeiServices).CpuIo->IoRead8) (
                   PeiServices,
                   (**PeiServices).CpuIo,
                   PCH_TCO_BASE_ADDRESS + R_PCH_TCO2_STS
                   );
  Data8 |= B_PCH_TCO2_STS_SECOND_TO;
  Data8 &= ~(B_PCH_TCO2_STS_INTRD_DET || B_PCH_TCO2_STS_SMLINK_SLV_SMI);
  ((**PeiServices).CpuIo->IoWrite8) (
           PeiServices,
           (**PeiServices).CpuIo,
           PCH_TCO_BASE_ADDRESS + R_PCH_TCO2_STS,
           Data8
           );
  DEBUG ((EFI_D_INFO, "\tTSTS2 = 0x%08x\tValue = 0x%08x\n",
      PCH_TCO_BASE_ADDRESS + R_PCH_TCO2_STS,
    ((**PeiServices).CpuIo->IoRead8) (PeiServices,(**PeiServices).CpuIo,PCH_TCO_BASE_ADDRESS + R_PCH_TCO2_STS)));

  //
  // Disable SERR NMI and IOCHK# NMI in port 61
  //
  Data8 = ((**PeiServices).CpuIo->IoRead8) (
                   PeiServices,
                   (**PeiServices).CpuIo,
                   R_PCH_NMI_SC
                   );
  Data8 |= (B_PCH_NMI_SC_PCI_SERR_EN | B_PCH_NMI_SC_IOCHK_NMI_EN);
  ((**PeiServices).CpuIo->IoWrite8) (
           PeiServices, 
           (**PeiServices).CpuIo, 
           R_PCH_NMI_SC,
           Data8
           );

  clearPowerButtonSts();

  DEBUG ((EFI_D_INFO, __FUNCTION__" : Exit\n"));
}

// CMOS test
// 
BOOLEAN  IsCMOSBad(
 IN CONST EFI_PEI_SERVICES **PeiServices
 )
{
  UINT8           Nmi;
  volatile UINT32 Data32;
  EFI_PEI_CPU_IO_PPI *CpuIo;

  Nmi     = 0;
  Data32  = 0;
  CpuIo = (**PeiServices).CpuIo;

  //
  // Preserve NMI bet setting
  //
  Data32 = (CpuIo->MemRead32) (PeiServices, CpuIo, (UINT64) PCH_PCR_ADDRESS (PID_ITSS, R_PCH_PCR_ITSS_GIC)) | B_PCH_PCR_ITSS_GIC_AME;
  (CpuIo->MemWrite32) (PeiServices, CpuIo, (UINT64) PCH_PCR_ADDRESS (PID_ITSS, R_PCH_PCR_ITSS_GIC), Data32);
  Data32 = (CpuIo->MemRead32) (PeiServices, CpuIo, (UINT64) PCH_PCR_ADDRESS (PID_ITSS, R_PCH_PCR_ITSS_GIC));
  Nmi    = (CpuIo->IoRead8) (PeiServices, CpuIo, (UINT64) CMOS_ADDR_PORT) & 0x80;
  Data32 = (CpuIo->MemRead32) (PeiServices, CpuIo, (UINT64) PCH_PCR_ADDRESS (PID_ITSS, R_PCH_PCR_ITSS_GIC)) & ~B_PCH_PCR_ITSS_GIC_AME;
  (CpuIo->MemWrite32) (PeiServices, CpuIo, (UINT64) PCH_PCR_ADDRESS (PID_ITSS, R_PCH_PCR_ITSS_GIC), Data32);  

  (CpuIo->IoWrite8) (PeiServices, CpuIo, (UINT64) CMOS_ADDR_PORT, CMOS_BAD_REG | Nmi);
  if ((CpuIo->IoRead8) (PeiServices, CpuIo, (UINT64) CMOS_DATA_PORT) & (BIT6 + BIT7)) {
    return TRUE;
  } else {
    return FALSE;
  }
}

VOID
clearCmos0E(
  IN CONST EFI_PEI_SERVICES **PeiServices
)
{
  UINT8           Value;
  UINT8           Nmi;
  volatile UINT32 Data32;
  EFI_PEI_CPU_IO_PPI    *CpuIo;


  Nmi     = 0;
  Data32  = 0;
  CpuIo   = (**PeiServices).CpuIo;
  //
  // Preserve NMI bit setting
  //  
  Data32 = (CpuIo->MemRead32) (PeiServices, CpuIo, (UINT64) PCH_PCR_ADDRESS (PID_ITSS, R_PCH_PCR_ITSS_GIC)) | B_PCH_PCR_ITSS_GIC_AME;
  CpuIo->MemWrite32 (PeiServices, CpuIo, (UINT64) PCH_PCR_ADDRESS (PID_ITSS, R_PCH_PCR_ITSS_GIC), Data32);
  Data32 = (CpuIo->MemRead32) (PeiServices, CpuIo, (UINT64) PCH_PCR_ADDRESS (PID_ITSS, R_PCH_PCR_ITSS_GIC));
  Nmi    = (CpuIo->IoRead8) (PeiServices, CpuIo, (UINT64) CMOS_ADDR_PORT) & 0x80;
  Data32 = (CpuIo->MemRead32) (PeiServices, CpuIo, (UINT64) PCH_PCR_ADDRESS (PID_ITSS, R_PCH_PCR_ITSS_GIC)) & ~B_PCH_PCR_ITSS_GIC_AME;
  (CpuIo->MemWrite32) (PeiServices, CpuIo, (UINT64) PCH_PCR_ADDRESS (PID_ITSS, R_PCH_PCR_ITSS_GIC), Data32);  

  (CpuIo->IoWrite8) (PeiServices, CpuIo, (UINT64) CMOS_ADDR_PORT, CMOS_BAD_REG | Nmi);
  Value = (CpuIo->IoRead8) (PeiServices, CpuIo, (UINT64) CMOS_DATA_PORT);
  (CpuIo->IoWrite8) (PeiServices, CpuIo, (UINT64) CMOS_DATA_PORT, Value & 0x3F);
}
// Denverton AptioV override start - EIP#235962
// This function is used to load the setup defaults, In AMI we are doing this in the NVRAM driver
/*
EFI_STATUS
EFIAPI
PlatformStage1VariableInit (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Interface
  )
/*++

Routine Description:

  Perform the default variable initializations after variable service is ready.

Arguments:
  
  PeiServices      - General purpose services available to every PEIM.
  NotifyDescriptor - Pointer to Notify PPI descriptor.
  Interface        - Pointer to PPI.
    
Returns:

  EFI_SUCCESS  - Default setting is initialized into variable. 
  Other values - Can't find the matched default setting.

--
{
  EFI_STATUS                      Status;
  SYSTEM_CONFIGURATION            SystemConfiguration;
  EFI_GUID                        SystemConfigurationGuid = SYSTEM_CONFIGURATION_GUID;
  EFI_GUID                        DummyDefaultVariableHobGuid = DUMMY_DEFAULT_VARIABLE_HOB_GUID;
  UINTN                           DataSize;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI *VariableServices;
  PLATFORM_INFO_HOB               *PlatformInfoHob;
  VOID                            *HobList;
  EFI_BOOT_MODE                   BootMode;
  UINT8                           *Data8;
  UINTN                           Index;
  PCH_SETUP                       PchSetup;
  //
  // Find PlatformInfo HOB to get BoardId
  //
  (*PeiServices)->GetHobList (PeiServices, &HobList);
  PlatformInfoHob = NULL;
  PlatformInfoHob = GetNextGuidHob (&gPlatformInfoHobGuid, HobList);
  if (NULL == PlatformInfoHob) {
    return EFI_NOT_FOUND;
  }

  //
  // Manufacturing mode is set.
  // 
  if (IsManufacturingMode (PeiServices, PlatformInfoHob)) {
    return CreateDefaultVariableHob (EFI_HII_DEFAULT_CLASS_MANUFACTURING, PlatformInfoHob->BoardId);
  }

  //
  // Check the CMOS battery status. 
  //
  if (!CheckCmosBatteryStatus ()) {
    //
    // COMS power failure. Check whether the CMOS content is corrupted.
    //
    if (IsCMOSBad(PeiServices)) {
      DEBUG(( DEBUG_ERROR, "CMOS power failure. CMOS content is bad. Reset the CMOS 0Eh and setup variable.\n"));
      clearCmos0E (PeiServices);
    }
    return CreateDefaultVariableHob (EFI_HII_DEFAULT_CLASS_STANDARD, PlatformInfoHob->BoardId);
  }

  //
  // CMOS battery is ok. Check whether the CMOS content is corrupted.
  //
  if (IsCMOSBad(PeiServices)) {
    DEBUG(( DEBUG_INFO, "CMOS battery is healthy but CMOS content is bad. Reset the SMOS 0Eh and the Setup variable.\n"));
    clearCmos0E (PeiServices);
    return CreateDefaultVariableHob (EFI_HII_DEFAULT_CLASS_STANDARD, PlatformInfoHob->BoardId);
  }

  //
  // Check BootMode on Recovery boot or Boot with Default settings.
  //
  Status = (**PeiServices).GetBootMode (PeiServices, &BootMode);
  ASSERT_EFI_ERROR ( Status);
  if (BootMode == BOOT_IN_RECOVERY_MODE || BootMode == BOOT_WITH_DEFAULT_SETTINGS) {
    return CreateDefaultVariableHob (EFI_HII_DEFAULT_CLASS_STANDARD, PlatformInfoHob->BoardId);
  }

  //
  // Check whether Setup Variable does exist to know the first boot or not.
  //
  VariableServices = (EFI_PEI_READ_ONLY_VARIABLE2_PPI *) Interface;
  DataSize         = sizeof(SystemConfiguration);
  Status = VariableServices->GetVariable (
             VariableServices,
             PLATFORM_SETUP_VARIABLE_NAME,
             &SystemConfigurationGuid,
             NULL,
             &DataSize,
             &SystemConfiguration
             );
  //
  // Setup variable is not found. So, set the default setting.
  //
  if (Status == EFI_NOT_FOUND) {
	DEBUG(( EFI_D_ERROR, "UEFI SetupVariable was deleted or corrupted. Restoring default values.\n"));
    Status = CreateDefaultVariableHob (EFI_HII_DEFAULT_CLASS_STANDARD, PlatformInfoHob->BoardId);
    if (EFI_ERROR (Status)) {
      return Status;
    }
	Status = VariableServices->GetVariable (
               VariableServices,
               PLATFORM_SETUP_VARIABLE_NAME,
               &SystemConfigurationGuid,
               NULL,
               &DataSize,
               &SystemConfiguration
               );
    ASSERT_EFI_ERROR (Status);
    
    //
    // Print default Setup variable.
    //
    Data8 = (UINT8 *) &SystemConfiguration;
    for (Index = 0; Index < DataSize; Index ++) {
      if (Index % 0x10 == 0) {
        DEBUG((DEBUG_INFO, "\n%08X:", Index));
      }
      DEBUG((DEBUG_INFO, " %02X", *Data8++));
    }
    DEBUG((DEBUG_INFO, "\n"));
  } else {
    // For HOB memory allignment between first boot and the rest, reserve also the space(Dummy) for Default Variable Hob
    // Red Hat OS complains if memory map is different in S4 cycles.
    BuildGuidHob (&DummyDefaultVariableHobGuid, sizeof(SYSTEM_CONFIGURATION) + sizeof(PCH_SETUP));
  }
  
  //Check if Boot critical PCH_SETUP is in NVRAM
  DataSize = sizeof (PCH_SETUP);
  Status = VariableServices->GetVariable (
                               VariableServices,
                               L"PchSetup",
                               &gPchSetupVariableGuid,
                               NULL,
                               &DataSize,
                               &PchSetup
                               );
  if (Status == EFI_NOT_FOUND) {
	  //Variable was deleted or corrupted. Restore Setup and PchSetup variable with Default values
	  DEBUG(( EFI_D_ERROR, "UEFI PchSetup was deleted or corrupted. Restoring default values.\n"));
      Status = CreateDefaultVariableHob (EFI_HII_DEFAULT_CLASS_STANDARD, PlatformInfoHob->BoardId);
      if (EFI_ERROR (Status)) {
        return Status;
      }
  } else {
    // For HOB memory allignment between first boot and the rest, reserve also the space(Dummy) for Default Variable Hob
    // Red Hat OS complains if memory map is different in S4 cycles.
    BuildGuidHob (&DummyDefaultVariableHobGuid, sizeof(SYSTEM_CONFIGURATION) + sizeof(PCH_SETUP));
  }
  DataSize = sizeof (PCH_SETUP);
  Status = VariableServices->GetVariable (
                               VariableServices,
                               L"PchSetup",
                               &gPchSetupVariableGuid,
                               NULL,
                               &DataSize,
                               &PchSetup
                                );
   ASSERT_EFI_ERROR (Status);
   
  //
  // Check AdvDebug Jumper if set to Auto, otherwise, use Enabled/Disabled value
  //
  if ( SystemConfiguration.DfxAdvDebugJumper == ADV_DEBUG_AUTO) {
    DfxDebugJumperValue = PlatformInfoHob->GpioExp44.ADV_DBG_DFX;
  } else {
    DfxDebugJumperValue = SystemConfiguration.DfxAdvDebugJumper;
  }
  PcdSetBool(PcdDfxAdvDebugJumper, (BOOLEAN)DfxDebugJumperValue);
  DEBUG ((DEBUG_INFO, "PcdDfxAdvDebugJumper: %d\n", PcdGetBool(PcdDfxAdvDebugJumper)));

  return EFI_SUCCESS;
}
*/
// Denverton AptioV override end - EIP#235962

VOID
GetSecPerformance (
  IN CONST EFI_PEI_SERVICES **PeiServices
  )
/*++

Routine Description:

  Collect Sec Performance Data and build HOB.
  
Arguments:

  PeiServices   - General purpose services available to every PEIM.

--*/
{
  EFI_STATUS                  Status;
  PEI_SEC_PERFORMANCE_PPI     *SecPerf;
  FIRMWARE_SEC_PERFORMANCE    Performance;
  VOID *Hob;
  
  Status = (**PeiServices).LocatePpi (
                             PeiServices,
                             &gPeiSecPerformancePpiGuid,
                             0,
                             NULL,
                             (VOID **)&SecPerf
                             );
  if (!EFI_ERROR (Status)) {
    Status = SecPerf->GetPerformance (PeiServices, SecPerf, &Performance);
    if (!EFI_ERROR (Status)) {
      Hob = BuildGuidDataHob (
              &gEfiFirmwarePerformanceGuid,
              &Performance,
              sizeof (Performance)
              );

      DEBUG(( DEBUG_INFO, "FPDT: SEC Performance Hob ResetEnd = %ld\n", Performance.ResetEnd));
    }
  }
}

