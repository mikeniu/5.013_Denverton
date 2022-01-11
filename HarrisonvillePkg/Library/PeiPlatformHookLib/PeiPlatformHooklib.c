/**
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**
@copyright
Copyright (c) 1999 - 2017 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

Module Name:

  PeiPlatformHookLib.c

Abstract:

  PEI Library Functions
**/

#include <PiPei.h>
#include <Guid/PlatformInfo.h>
#include <Register/PchRegsLpc.h>
#include <PlatformDefinitions.h>
#include <PlatformBoardId.h>
#include <PchAccess.h>
//#include <Sb.h>
#include <Ppi/ReadOnlyVariable2.h>
//#include <Ppi/SaPlatformPolicy.h>
#include <Ppi/CpuIo.h>
#include <Ppi/Stall.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/IoLib.h>
#include <Library/HobLib.h>
#include <Library/PcdLib.h>
//#include <Library/PeiKscLib.h>
#include <Library/PeiPlatformLib.h>
#include <Library/PciLib.h>

//
// NOTE: Use SetupVariable for and only for platformstage2
//
#include "Include/SetupVariable.h"
#include <CmosMap.h>
#include <Library/MmPciLib.h>
#include <Library/PciLib.h>
#include <Library/GpioLib.h>
#include <Library/PeiServicesLib.h>
#include <GpioPinsSklH.h>
#include "GpioTableDvp.h"
#include "GpioTableHarcuvar.h"
#include "GpioTableCarCreekModule.h"
#include "GpioTableCarCreekModule2.h"
#include "GpioTableBigBugCreek.h"
#include "GpioTableCormorantLake.h"
#include "GpioTableMinnehaha.h"
#include "GpioTablePineLake.h"
#include "GpioTableAspenCove.h"
#include "GpioTableOstrichBay.h"
#include "GpioTableNano.h"
#include "GpioTableDelphiModule.h"
#include <Ppi/PchPolicy.h>
#include "GpioTable.h"    // Denverton AptioV Override - EIP#281657

#define SIO_BASE_ADDRESS                          0x0680

EFI_STATUS
GetPlatformInfo (
  OUT PLATFORM_INFO_HOB                 **PlatformInfoHob
  )
{
  EFI_STATUS                            Status;
  EFI_PEI_HOB_POINTERS                  Hob;

  *PlatformInfoHob = NULL;
  //
  // Find the PlatformInfo HOB
  //
  Status = PeiServicesGetHobList (&Hob.Raw);
  while (!END_OF_HOB_LIST (Hob)) {
    if (Hob.Header->HobType == EFI_HOB_TYPE_GUID_EXTENSION && (CompareMem (&Hob.Guid->Name, &gPlatformInfoHobGuid, sizeof(gPlatformInfoHobGuid))==0)) {
      *PlatformInfoHob = (PLATFORM_INFO_HOB*) (Hob.Raw + sizeof (EFI_HOB_GENERIC_HEADER) + sizeof (EFI_GUID));
      break;
    }
    Hob.Raw = GET_NEXT_HOB (Hob);
  }

  //
  // PlatformInfo PEIM should provide this HOB data, if not ASSERT and return error.
  //
  ASSERT (*PlatformInfoHob != NULL);
  if (!(*PlatformInfoHob)) {
    Status = EFI_NOT_FOUND;
    return Status;
  }

  return Status;
}

/**
  Configures GPIO

  @param[in]  GpioTable       Point to Platform Gpio table
  @param[in]  GpioTableCount  Number of Gpio table entries

**/
VOID
ConfigureGpio (
  IN GPIO_INIT_CONFIG                 *GpioDefinition,
  IN UINT16                           GpioTableCount
  )
{
  EFI_STATUS          Status;
  PCH_POLICY_PPI     *PchPolicy;
  DEBUG ((DEBUG_INFO, "ConfigureGpio() Start\n"));

  //
  // Locate installed PCH Policy PPI
  //
  Status = PeiServicesLocatePpi (
             &gPchPolicyPpiGuid,
             0,
             NULL,
             (VOID **) &PchPolicy
             );
  if (Status != EFI_SUCCESS) {
    //
    // PCH_POLICY_PPI must be installed at this point
    //
    ASSERT (FALSE);
  }
  Status = GpioConfigurePads (GpioTableCount, GpioDefinition, (UINT8)PchPolicy->LockDownConfig.GpioLockDown);
  DEBUG ((EFI_D_INFO, "ConfigureGpio() End\n"));
}

/**
Routine Description:

  Initialize the GPIO IO selection, GPIO USE selection, and GPIO signal inversion registers

Arguments:

  PeiServices   - General purpose services available to every PEIM.
  GpioTable     - Point to Platform Gpio table
  GpioTableCount - Number of Gpio table entries

Returns:
**/

/**
  Configure GPIO

  @param[in]  PlatformInfo
**/
VOID
GpioInit (
  IN PLATFORM_INFO_HOB         *PlatformInfo
  )
{
// Denverton AptioV Override Start - EIP#281657 - Assigning the values from automatically generated header file
  GPIO_INIT_CONFIG gpioCfioInitTable[] = {GPIO_TABLE};

  switch (PlatformInfo->BoardId) {
    case BoardIdCormorantLake:
      ConfigureGpio (gpioCfioInitTable, sizeof (gpioCfioInitTable) / sizeof (GPIO_INIT_CONFIG));
      break;
    case BoardIdHarcuvar:
      ConfigureGpio (gpioCfioInitTable, sizeof (gpioCfioInitTable) / sizeof (GPIO_INIT_CONFIG));
      break;
    case BoardIdMinnehaha:
      ConfigureGpio (mMinnehahaCfioInitTable, sizeof (mMinnehahaCfioInitTable) / sizeof (GPIO_INIT_CONFIG));
      break;
    case BoardIdPineLake:
      ConfigureGpio (mPineLakeCfioInitTable, sizeof (mPineLakeCfioInitTable) / sizeof (GPIO_INIT_CONFIG));
      break;
    case BoardIdAspenCove:
      ConfigureGpio (gpioCfioInitTable, sizeof (gpioCfioInitTable) / sizeof (GPIO_INIT_CONFIG));
// Denverton AptioV Override End - EIP#281657
      break;
    case BoardIdOstrichBay:
      ConfigureGpio (mOstrichBayCfioInitTable, sizeof (mOstrichBayCfioInitTable) / sizeof (GPIO_INIT_CONFIG));
      break;
    case BoardIdCarCreekModule:
      ConfigureGpio (mCarCreekModuleCfioInitTable, sizeof (mCarCreekModuleCfioInitTable) / sizeof (GPIO_INIT_CONFIG));
      break;
    case BoardIdCarCreekModule2:
		ConfigureGpio (mCarCreekModuleCfioInitTable2, sizeof (mCarCreekModuleCfioInitTable2) / sizeof (GPIO_INIT_CONFIG));
	break;	
    case BoardIdDelphi:
		ConfigureGpio (mDelphiModuleCfioInitTable, sizeof (mDelphiModuleCfioInitTable) / sizeof (GPIO_INIT_CONFIG));
	break;		
    case BoardIdBigBugCreek:
      ConfigureGpio (mBigBugCreekCfioInitTable, sizeof (mBigBugCreekCfioInitTable) / sizeof (GPIO_INIT_CONFIG));
      break;
    case BoardIdPineLakeB:
      ConfigureGpio (mPineLakeBCfioInitTable, sizeof (mPineLakeBCfioInitTable) / sizeof (GPIO_INIT_CONFIG));
      break;
    case BoardIdNano:
      ConfigureGpio (mNanoCfioInitTable, sizeof (mNanoCfioInitTable) / sizeof (GPIO_INIT_CONFIG));
      break;
    default:
      ConfigureGpio (mCfioInitTable, sizeof (mCfioInitTable) / sizeof (GPIO_INIT_CONFIG));
      break;
  }

  return;
}

/**
Routine Description:

    Program PCH registers for platform specific requirement. For example, decode region
    program, GPIO program


Arguments:

  PeiServices   General purpose services available to every PEIM.

Returns:

  EFI_SUCCESS   Operation success.
**/
EFI_STATUS
PlatformInitStage1 (
  IN CONST EFI_PEI_SERVICES       **PeiServices
  )
{
  EFI_PEI_CPU_IO_PPI              *CpuIo;
  EFI_STATUS                      Status;
  PLATFORM_INFO_HOB               *PlatformInfoHob;
#ifdef ERB_FLAG
  PCH_SERIES                      PchSeries;
#endif 


  CpuIo  = (**PeiServices).CpuIo;


  //
  // Obtain Platform Info from HOB.
  //
  Status = GetPlatformInfo (&PlatformInfoHob);
  ASSERT_EFI_ERROR (Status);
  if (PlatformInfoHob == NULL) {
    return Status;
  }

  DEBUG((DEBUG_INFO, "Board ID: %x\n",PlatformInfoHob->BoardId));

  GpioInit (PlatformInfoHob);

  //
  // We should not depend on SerialPortLib to initialize KBC for legacy USB
  // So initialize KBC for legacy USB driver explicitly here.
  // After we find how to enable mobile KBC, we will add enabling code for mobile then.
  //
  CpuIo    = (**PeiServices).CpuIo;

  if (PlatformInfoHob->PlatformFlavor == FlavorMicroserver) {
  // Denverton AptioV override Start -  Comment unnecessary programming of GENx_DEC Registers in RC code.
  /*
    //
    // Program and Enable Default Super IO Configuration Port Addresses and range
    //
    MmioWrite32 (
      MmPciBase (
        DEFAULT_PCI_BUS_NUMBER_PCH,
        PCI_DEVICE_NUMBER_PCH_LPC,
        0)
        + R_PCH_LPC_GEN2_DEC,
      0x3 << 18 | (LPC_SIO_CONFIG_DEFAULT_PORT & (~0xF)) | BIT0
    );

    //
    // 128 Byte Boundary and SIO Runtime Register Range is 0x0 to 0xF;
    //
    MmioWrite32 (
      MmPciBase (
        DEFAULT_PCI_BUS_NUMBER_PCH,
        PCI_DEVICE_NUMBER_PCH_LPC,
        0)
        + R_PCH_LPC_GEN1_DEC,
      0x3 << 18 | (SIO_BASE_ADDRESS  & (~0x7F)) | BIT0
    );
  */
  // Denverton AptioV override End 
  
    //
    // Enable KBC for National PC8374 SIO on Sugar Bay
    //
    (CpuIo->IoWrite8) (
            PeiServices,
            CpuIo,
            DESKTOP_LPC_SIO_INDEX_DEFAULT_PORT,
            0x07
            );

    (CpuIo->IoWrite8) (
            PeiServices,
            CpuIo,
            DESKTOP_LPC_SIO_DATA_DEFAULT_PORT,
            0x06
            );

    (CpuIo->IoWrite8) (
            PeiServices,
            CpuIo,
            DESKTOP_LPC_SIO_INDEX_DEFAULT_PORT,
            0x30
            );

    (CpuIo->IoWrite8) (
            PeiServices,
            CpuIo,
            DESKTOP_LPC_SIO_DATA_DEFAULT_PORT,
            0x01
            );
  }

   //
   //4373474-SIO support on Basking Ridge
   //
  if ((PlatformInfoHob->PlatformFlavor == FlavorMobile) ||
      (PlatformInfoHob->PlatformFlavor == FlavorEmbedded)){
#ifdef ERB_FLAG
    if ((PlatformInfoHob->BoardId == BoardIdBaskingRidge) ||
        (PlatformInfoHob->BoardId == BoardIdWalnutCanyon)) {

      //
      // ENTER CONFIG MODE
      //
      (CpuIo->IoWrite8) (
             PeiServices,
             CpuIo,
             DESKTOP_LPC_SIO_CONFIG_DEFAULT_PORT,
             0x55
             );
      //
      // program base address
      //
      (CpuIo->IoWrite8) (
             PeiServices,
             CpuIo,
             DESKTOP_LPC_SIO_INDEX_DEFAULT_PORT,
             0x30
             );
      (CpuIo->IoWrite8) (
             PeiServices,
             CpuIo,
             DESKTOP_LPC_SIO_DATA_DEFAULT_PORT,
             0x68
             );
      //
      //Enable access to Runtime Registers
      //
      (CpuIo->IoWrite8) (
             PeiServices,
             CpuIo,
             DESKTOP_LPC_SIO_INDEX_DEFAULT_PORT,
             0x3A
             );
      (CpuIo->IoWrite8) (
             PeiServices,
             CpuIo,
             DESKTOP_LPC_SIO_DATA_DEFAULT_PORT,
             0x02
             );
      //
      //GP17, GP16, GP14, are o/p
      //
      (CpuIo->IoWrite8) (
             PeiServices,
             CpuIo,
             DESKTOP_LPC_SIO_INDEX_DEFAULT_PORT,
             0x31
             );
      (CpuIo->IoWrite8) (
             PeiServices,
             CpuIo,
             DESKTOP_LPC_SIO_DATA_DEFAULT_PORT,
             BIT7 | BIT6| BIT4
             );
      //
      //invert GP13, GP11, GP10 to trigger during falling edge.
      //
      (CpuIo->IoWrite8) (
             PeiServices,
             CpuIo,
             DESKTOP_LPC_SIO_INDEX_DEFAULT_PORT,
             0x32
             );
      (CpuIo->IoWrite8) (
             PeiServices,
             CpuIo,
             DESKTOP_LPC_SIO_DATA_DEFAULT_PORT,
             BIT3 | BIT1| BIT0
             );
      //
      // GP10-GP17 O/P Buffer to Push Pull
      //
      (CpuIo->IoWrite8) (
             PeiServices,
             CpuIo,
             DESKTOP_LPC_SIO_INDEX_DEFAULT_PORT,
             0x33
             );
      (CpuIo->IoWrite8) (
             PeiServices,
             CpuIo,
             DESKTOP_LPC_SIO_DATA_DEFAULT_PORT,
             0x00
             );
      //
      //Alternate Function enable - GP12 to nIO_SMI
      //
      (CpuIo->IoWrite8) (
             PeiServices,
             CpuIo,
             DESKTOP_LPC_SIO_INDEX_DEFAULT_PORT,
             0x34
             );
      (CpuIo->IoWrite8) (
             PeiServices,
             CpuIo,
             DESKTOP_LPC_SIO_DATA_DEFAULT_PORT,
             BIT5
             );
      //
      // Drive GPIO17 and GPIO14 high
      //
      (CpuIo->IoWrite8) (
             PeiServices,
             CpuIo,
             Smsc1007_PME_BASE_ADDRESS + 0x0C,
             0x90
             );
      //
      //GP37, 36, 35, 34, 33, 32 AS O/P
      //
      (CpuIo->IoWrite8) (
             PeiServices,
             CpuIo,
             DESKTOP_LPC_SIO_INDEX_DEFAULT_PORT,
             0x35
             );
      (CpuIo->IoWrite8) (
             PeiServices,
             CpuIo,
             DESKTOP_LPC_SIO_DATA_DEFAULT_PORT,
             BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2
             );
      //
      //GP30-37 non-invert
      //
      (CpuIo->IoWrite8) (
             PeiServices,
             CpuIo,
             DESKTOP_LPC_SIO_INDEX_DEFAULT_PORT,
             0x36
             );
      (CpuIo->IoWrite8) (
             PeiServices,
             CpuIo,
             DESKTOP_LPC_SIO_DATA_DEFAULT_PORT,
             0x00
             );
      //
      //GP30-37 no-pullup
      //
      (CpuIo->IoWrite8) (
             PeiServices,
             CpuIo,
             DESKTOP_LPC_SIO_INDEX_DEFAULT_PORT,
             0x37
             );
      (CpuIo->IoWrite8) (
             PeiServices,
             CpuIo,
             DESKTOP_LPC_SIO_DATA_DEFAULT_PORT,
             0x00
             );
      //
      //GP10-17 no-pullup
      //
      (CpuIo->IoWrite8) (
             PeiServices,
             CpuIo,
             DESKTOP_LPC_SIO_INDEX_DEFAULT_PORT,
             0x38
             );
      (CpuIo->IoWrite8) (
             PeiServices,
             CpuIo,
             DESKTOP_LPC_SIO_DATA_DEFAULT_PORT,
             0x00
             );
      //
      // GP30-GP37 O/P Buffer to Push Pull
      //
      (CpuIo->IoWrite8) (
             PeiServices,
             CpuIo,
             DESKTOP_LPC_SIO_INDEX_DEFAULT_PORT,
             0x39
             );
      //
      //Drive GP37-32 high
      //
      (CpuIo->IoWrite8) (
             PeiServices,
             CpuIo,
             DESKTOP_LPC_SIO_DATA_DEFAULT_PORT,
             0x00
             );
       (CpuIo->IoWrite8) (
             PeiServices,
             CpuIo,
             Smsc1007_PME_BASE_ADDRESS + 0x0E,
             0x0E0
             );
       //
       //EN_SMI - route nSMI to IO_SMI# pin
       //
       (CpuIo->IoWrite8) (
             PeiServices,
             CpuIo,
             Smsc1007_PME_BASE_ADDRESS + 0x0B,
             0x80
             );
     //
     // EXIT CONFIG MODE
     //
     (CpuIo->IoWrite8) (
             PeiServices,
             CpuIo,
             DESKTOP_LPC_SIO_CONFIG_DEFAULT_PORT,
             0xAA
             );
    } else if (PlatformInfoHob->BoardId == BoardIdPearlValley) {

      //
      // ENTER CONFIG MODE
      //
      (CpuIo->IoWrite8) (
             PeiServices,
             CpuIo,
             LPC_SIO_CONFIG_DEFAULT_PORT,
             0x55
             );
      //
      // program base address
      //
      (CpuIo->IoWrite8) (
             PeiServices,
             CpuIo,
             LPC_SIO_INDEX_DEFAULT_PORT,
             0x30
             );
      (CpuIo->IoWrite8) (
             PeiServices,
             CpuIo,
             LPC_SIO_DATA_DEFAULT_PORT,
             0x68
             );
      //
      //Enable access to Runtime Registers
      //
      (CpuIo->IoWrite8) (
             PeiServices,
             CpuIo,
             LPC_SIO_INDEX_DEFAULT_PORT,
             0x3A
             );
      (CpuIo->IoWrite8) (
             PeiServices,
             CpuIo,
             LPC_SIO_DATA_DEFAULT_PORT,
             0x02
             );
      //
      //GP34, 33, 32, 31 AS O/P
      //
      (CpuIo->IoWrite8) (
             PeiServices,
             CpuIo,
             LPC_SIO_INDEX_DEFAULT_PORT,
             0x35
             );
      (CpuIo->IoWrite8) (
             PeiServices,
             CpuIo,
             LPC_SIO_DATA_DEFAULT_PORT,
             BIT4 | BIT3 | BIT2 | BIT1
             );

      //
      //GP30-37 non-invert
      //
      (CpuIo->IoWrite8) (
             PeiServices,
             CpuIo,
             LPC_SIO_INDEX_DEFAULT_PORT,
             0x36
             );
      (CpuIo->IoWrite8) (
             PeiServices,
             CpuIo,
             LPC_SIO_DATA_DEFAULT_PORT,
             0x00
             );
      //
      //GP30-37 no-pullup
      //
      (CpuIo->IoWrite8) (
             PeiServices,
             CpuIo,
             LPC_SIO_INDEX_DEFAULT_PORT,
             0x37
             );
      (CpuIo->IoWrite8) (
             PeiServices,
             CpuIo,
             LPC_SIO_DATA_DEFAULT_PORT,
             0x10
             );
      //
      // GP30-GP37 O/P Buffer to Push Pull
      //
      (CpuIo->IoWrite8) (
             PeiServices,
             CpuIo,
             LPC_SIO_INDEX_DEFAULT_PORT,
             0x39
             );
      (CpuIo->IoWrite8) (
             PeiServices,
             CpuIo,
             LPC_SIO_DATA_DEFAULT_PORT,
             0x10
             );

      //
      //Drive GP31-33 low and GP34 high
      //
      (CpuIo->IoWrite8) (
             PeiServices,
             CpuIo,
             LPC_SIO_DATA_DEFAULT_PORT,
             0x00
             );
       (CpuIo->IoWrite8) (
             PeiServices,
             CpuIo,
             Smsc1007_PME_BASE_ADDRESS + 0x0E,
             0x10
             );

     //
     // EXIT CONFIG MODE
     //
     (CpuIo->IoWrite8) (
             PeiServices,
             CpuIo,
             LPC_SIO_CONFIG_DEFAULT_PORT,
             0xAA
             );
    }
#endif 
  }

  return EFI_SUCCESS;
}

/**
Routine Description:

  Detect Recovery Mode.

Arguments:

  PeiServices - General purpose services available to every PEIM.

Returns:

  EFI_SUCCESS     - System in Recovery Mode
  EFI_UNSUPPORTED - System doesn't support Recovery Mode
  EFI_NOT_FOUND   - System is not in Recovery Mode
**/
EFI_STATUS
IsRecoveryMode (
  IN CONST EFI_PEI_SERVICES **PeiServices
  )
{
  return EFI_NOT_FOUND;
}

/**
  Description :
    Do platform specific programming. For example, KSC init, Chipset programming

  Parameters:
    PeiSercies  - Point to Pei Services structure

  Returns:
**/
EFI_STATUS
PlatformInitStage2 (
  IN CONST EFI_PEI_SERVICES    **PeiServices
  )
{
  EFI_STATUS                      Status;
  EFI_BOOT_MODE                   BootMode;
//  SA_PLATFORM_POLICY_PPI          *SaPlatformPolicyPpi;
  PLATFORM_INFO_HOB               *PlatformInfoHob;
  BOOLEAN                         SetupValid;
  EFI_PEI_CPU_IO_PPI              *CpuIo;

  CpuIo  = (**PeiServices).CpuIo;


  SetupValid = FALSE;

  Status = (*PeiServices)->GetBootMode (PeiServices, &BootMode);
  ASSERT_EFI_ERROR (Status);

  //
  // Obtain Platform Info from HOB.
  //
  Status = GetPlatformInfo (&PlatformInfoHob);
  ASSERT_EFI_ERROR (Status);
  if (PlatformInfoHob == NULL) {
    return Status;
  }

  if (BootMode == BOOT_ON_FLASH_UPDATE) {
    //
    // Get platform policy settings through the SaPlatformPolicy PPI
    //
//    Status = (*PeiServices)->LocatePpi (
//                               PeiServices,
//                               &gSaPlatformPolicyPpiGuid,
//                               0,
//                               NULL,
//                               &SaPlatformPolicyPpi
//                               );
//    ASSERT_EFI_ERROR (Status);
    //
    // Set SSKPD before memory initialize when BOOT_ON_FLASH_UPDATE
    //
//    MmioWrite8 (SaPlatformPolicyPpi->PlatformData->MchBar + 0x5D10, 1);
  }
  // Denverton AptioV override start - EIP#235962
  // AMI Romlayout module takes care of publishing FV hobs
  /*
  BuildFvHob (
      PcdGet32 (PcdFlashFvSecPeiBase),
      PcdGet32 (PcdFlashFvSecPeiSize)
    );
	
  BuildFvHob (
      PcdGet32 (PcdFlashFvOemLogoBase),
      PcdGet32 (PcdFlashFvOemLogoSize)
    );
	
  if (PcdGetBool(PcdUefiOptimizedBoot)) {
    BuildFvHob (
      PcdGet32 (PcdFlashFvLegacyFreeBase),
      PcdGet32 (PcdFlashFvLegacyFreeSize)
    );
  } else {
    BuildFvHob (
      PcdGet32 (PcdFlashFvLegacyBase),
      PcdGet32 (PcdFlashFvLegacySize)
    );
  }
  */
  // Denverton AptioV override End - EIP#235962
  return EFI_SUCCESS;
}

BOOLEAN
IsManufacturingMode (
  IN CONST EFI_PEI_SERVICES           **PeiServices,
  PLATFORM_INFO_HOB                   *PlatformInfoHob
  )

/*++
Routine Description:
  This function returns the state of the MFG_MODE jumper.

Arguments:
  PeiServices     - General purpose services available to every PEIM.
  PlatformInfoHob - Record current platform type and BoardId.

Returns:
  TRUE - The system is booting in manufacturing mode
  FALSE - Normal Boot
--*/

{
  BOOLEAN               ManufacturingMode;
  EFI_PEI_CPU_IO_PPI    *CpuIo;
  UINT16                GpioValue;
#ifdef ERB_FLAG
  UINT8                 CmosValue;
  UINT8                 MfgModeGpio;
#endif 
  ManufacturingMode = FALSE;
  CpuIo = (**PeiServices).CpuIo;

  if (PlatformInfoHob->PlatformType == TypeTrad) {
   CpuIo = (**PeiServices).CpuIo;
    if (PlatformInfoHob->PlatformFlavor == FlavorUpServer) {
      GpioValue = (CpuIo->IoRead16) (PeiServices, CpuIo, GPIO_BASE_ADDRESS + 0x48);
      ManufacturingMode = (BOOLEAN) ((GpioValue & 0x0100) == 0);
    } else {
      GpioValue = (CpuIo->IoRead16) (PeiServices, CpuIo, GPIO_BASE_ADDRESS + 0x38);
      ManufacturingMode = (BOOLEAN) ((GpioValue & 0x0040) == 0);
    }
  } 
  
  if  (PlatformInfoHob->PlatformType == TypeUlt) {
#ifdef ERB_FLAG 
    if(PlatformInfoHob->BoardId == BoardIdHarrisBeachFfrd){
    (CpuIo->IoWrite8) (PeiServices, CpuIo, 0x70, CMOS_MANUFACTURING_USED_REG);
    CmosValue = (CpuIo->IoRead8) (PeiServices, CpuIo, 0x71);
      if(CmosValue != 0){
        ManufacturingMode = TRUE;
        (CpuIo->IoWrite8) (PeiServices, CpuIo, 0x71, 0);
      }
    }
    else{
      MfgModeGpio = (CpuIo->IoRead8) (PeiServices, CpuIo, SIO_BASE_ADDRESS + 0x0C);
      ManufacturingMode = (BOOLEAN) ((MfgModeGpio & 0x02) == 0);
    }
#endif 
  }

  return ManufacturingMode;
}
