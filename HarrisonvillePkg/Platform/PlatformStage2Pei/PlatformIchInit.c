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

  PlatformIchInit.c

Abstract:

  Do Platform Stage ICH initialization.

--*/


#include "PlatformStage2.h"
#include <Library/PchInfoLib.h>
#include <Library/PchCycleDecodingLib.h>

EFI_STATUS
PlatformPchInit (
  IN CONST EFI_PEI_SERVICES    **PeiServices
  )
/*++

Routine Description:

  Initialize the GPIO IO selection, GPIO USE selection, and GPIO signal inversion registers

Arguments:

  PeiServices   General purpose services available to every PEIM.

Returns:

  EFI_SUCCESS   Operation success.

--*/
{
  UINT16                          Data16;
  UINT8                           Data8;
  EFI_PEI_CPU_IO_PPI              *CpuIo;
  EFI_STATUS                      Status;
//  UINT32                          Index;   // Denverton AptioV override  -  Comment unnecessary programming of GENx_DEC Registers in RC code.
  EFI_PEI_READ_ONLY_VARIABLE2_PPI *VariableServices;
  SYSTEM_CONFIGURATION            SystemConfiguration;
  UINTN                           VarSize;
  UINTN                           LpcBaseAddress;
  PLATFORM_INFO_HOB           *PlatformInfoHob;
  DEBUG ((EFI_D_INFO, __FUNCTION__" : Entry\n"));
  LpcBaseAddress = MmPciBase (
                     DEFAULT_PCI_BUS_NUMBER_PCH,
                     PCI_DEVICE_NUMBER_PCH_LPC,
                     PCI_FUNCTION_NUMBER_PCH_LPC
                     );

  //
  // Locate system configuration variable
  //
  Status = (*PeiServices)->LocatePpi (
             PeiServices,
             &gEfiPeiReadOnlyVariable2PpiGuid, // GUID
             0,                            // INSTANCE
             NULL,                         // EFI_PEI_PPI_DESCRIPTOR
             &VariableServices             // PPI
             );
  ASSERT_EFI_ERROR (Status);

  VarSize = sizeof (SYSTEM_CONFIGURATION);
  Status = VariableServices->GetVariable (
                               VariableServices,
                               PLATFORM_SETUP_VARIABLE_NAME,
                               &mSystemConfigurationGuid,
                               NULL,
                               &VarSize,
                               &SystemConfiguration
                               );
  if (Status == EFI_SUCCESS) {

  }

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
  MmioOr32 (PCH_PCR_ADDRESS(PID_SMB, R_PCH_PCR_SMBUS_GC), (UINT32)B_PCH_PCR_SMBUS_GC_NR);
  DEBUG ((EFI_D_INFO, "\tGC = 0x%08x\tValue = 0x%08x\n",
    PCH_PCR_ADDRESS(PID_SMB, R_PCH_PCR_SMBUS_GC),
    MmioRead32 (PCH_PCR_ADDRESS(PID_SMB, R_PCH_PCR_SMBUS_GC))));

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

  // Denverton AptioV override Start -  Comment unnecessary programming of GENx_DEC Registers in RC code.
  /*
  //
  // Enable the LPC I/O decoding for 0x6A0~0x6A7 as EC's extra I/O port, where 0x6A0 is the Data port
  // and 0x6A4 is the Command/Status port.
  //
  for (Index = R_PCH_LPC_GEN4_DEC; Index >= R_PCH_LPC_GEN1_DEC; Index-= 4) {
    if ((MmioRead32 (LpcBaseAddress + Index) & B_PCH_LPC_GENX_DEC_EN) != B_PCH_LPC_GENX_DEC_EN) {
      MmioAndThenOr32 (
        LpcBaseAddress + Index,
        (UINT32) (~(B_PCH_LPC_GENX_DEC_IOBAR | B_PCH_LPC_GENX_DEC_IODRA)),
        (UINT32) (BIT19 | BIT18 | EC_EXTRA_IO_PORT_1 | B_PCH_LPC_GENX_DEC_EN)
        );
      DEBUG ((EFI_D_INFO, "\tLGIR%d = 0x%08x\tValue = 0x%08x\n",(((Index-R_PCH_LPC_GEN1_DEC)/4)+1),LpcBaseAddress + Index,MmioRead32 (LpcBaseAddress + Index)));
      break;
    }
  }
  */
  // Denverton AptioV override End 

  //
  // Obtain Platform Info from HOB.
  //
  Status = GetPlatformInfo (&PlatformInfoHob);
  ASSERT_EFI_ERROR ( Status);
  if (PlatformInfoHob == NULL) {
    return Status;
  }
  // Denverton AptioV override Start - EIP#267875
  // This programming will be done by AMI IPMI module.
  /*
  //Enable BMC KCS address,0xCA2~0xCA5 in a LPC Generic I/O range reg.
  if(PlatformInfoHob->BoardId == BOARD_ID_CormorantLake){
    for (Index = R_PCH_LPC_GEN4_DEC; Index >= R_PCH_LPC_GEN1_DEC; Index-= 4) {
      if ((MmioRead32 (LpcBaseAddress + Index) & B_PCH_LPC_GENX_DEC_EN) != B_PCH_LPC_GENX_DEC_EN) {
        MmioAndThenOr32 (
          LpcBaseAddress + R_PCH_LPC_GEN3_DEC,
          (UINT32) (~(B_PCH_LPC_GENX_DEC_IOBAR | B_PCH_LPC_GENX_DEC_IODRA)),
          (UINT32) (BIT18 | BMC_KCS_IO_PORT_1 | B_PCH_LPC_GENX_DEC_EN)
          );
        DEBUG ((DEBUG_ERROR, "%a()-KCS enabled in LGIR%d %x:%x\n",__FUNCTION__,(((Index-R_PCH_LPC_GEN1_DEC)/4)+1), (LpcBaseAddress + R_PCH_LPC_GEN3_DEC), (MmioRead32 (LpcBaseAddress + Index) & ~B_PCH_LPC_GENX_DEC_EN) ));
        break;
      }
    }
  }

  // Report that no free LGIRs were found
  if (!(Index >= R_PCH_LPC_GEN1_DEC)) {
    DEBUG ((DEBUG_ERROR, "%a()- ERROR: KCS failed enabling, no LGIRs free\n",__FUNCTION__ ));
  }
  */
  // Denverton AptioV override End - EIP#267875

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
  DEBUG ((EFI_D_INFO, __FUNCTION__" : Exit\n"));
  return EFI_SUCCESS;

}
