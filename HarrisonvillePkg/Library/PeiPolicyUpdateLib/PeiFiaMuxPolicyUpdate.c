/*++
  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may be modified by the user, subject to
  the additional terms of the license agreement
--*/
/*++

Copyright (c) 2015 - 2017 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

Module Name:

  PeiFiaMuxPolicyUpdate.c

Abstract:

  This file is SampleCode of the library for FIA MUX PEI Platform Policy initialization.

--*/
#include <Base.h>
#include <PeiFiaMuxPolicyUpdate.h>
#include <Guid/PlatformInfo.h>
#include <Library/PeiPlatformLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Guid/FiaMuxVariableGuid.h>
#include <Library/HobLib.h>
#include <FiaMuxPolicy.h>
#include <SetupVariable.h>
#include <Library/IoLib.h>
#include <Library/MmPciLib.h>
#include "SimicsDebug.h"

// Denverton AptioV override start - EIP#332934
// Macros for Bifurcation Control Options.
// It will be the initial value of MeFiaPcieRootPortsConfig 
#define BifCtl_0_x2x2x2x2         0x0F
#define BifCtl_0_x2x2x4           0x07
#define BifCtl_0_x4x2x2           0x0D
#define BifCtl_0_x4x4             0x05
#define BifCtl_0_x8               0x01

#define BifCtl_1_x2x2x2x2         0xF0
#define BifCtl_1_x2x2x4           0x70
#define BifCtl_1_x4x2x2           0xD0
#define BifCtl_1_x4x4             0x50
#define BifCtl_1_x8               0x10

// Macro definitions to disable the various RootPorts as per the  Bifurcation settings, if any of the LANE below the corresponding RP is not assigned as PCIE
//If PCIE ROOT PORT LINK WIDTH =   "X1"  =>: Don't disable if the RP's LANE[0], below it, is assigned as PCIE
// Disable RP0
#define DIS_RP0    \
            if (!((SystemConfig.FiaPcieRootPortLinkWidth[0] == ME_FIA_PCIE_ROOT_PORT_LINK_X1) && (SystemConfig.FiaLaneConfig[0] == 1))) \
                FiaMuxConfig.PcieRootPortsConfiguration.MeFiaPcieRpConfig.PcieRp0En = 0;

// Disable RP1
#define DIS_RP1    \
            if (!((SystemConfig.FiaPcieRootPortLinkWidth[1] == ME_FIA_PCIE_ROOT_PORT_LINK_X1) && (SystemConfig.FiaLaneConfig[2] == 1))) \
			    FiaMuxConfig.PcieRootPortsConfiguration.MeFiaPcieRpConfig.PcieRp1En = 0;

// Disable RP2
#define DIS_RP2    \
            if (!((SystemConfig.FiaPcieRootPortLinkWidth[2] == ME_FIA_PCIE_ROOT_PORT_LINK_X1) && (SystemConfig.FiaLaneConfig[4] == 1))) \
			    FiaMuxConfig.PcieRootPortsConfiguration.MeFiaPcieRpConfig.PcieRp2En = 0;

// Disable RP3
#define DIS_RP3    \
            if (!((SystemConfig.FiaPcieRootPortLinkWidth[3] == ME_FIA_PCIE_ROOT_PORT_LINK_X1) && (SystemConfig.FiaLaneConfig[6] == 1))) \
			    FiaMuxConfig.PcieRootPortsConfiguration.MeFiaPcieRpConfig.PcieRp3En = 0;

// Disable RP4
#define DIS_RP4    \
            if (!((SystemConfig.FiaPcieRootPortLinkWidth[4] == ME_FIA_PCIE_ROOT_PORT_LINK_X1) && (SystemConfig.FiaLaneConfig[8] == 1))) \
			    FiaMuxConfig.PcieRootPortsConfiguration.MeFiaPcieRpConfig.PcieRp4En = 0;

// Disable RP5
#define DIS_RP5    \
            if (!((SystemConfig.FiaPcieRootPortLinkWidth[5] == ME_FIA_PCIE_ROOT_PORT_LINK_X1) && (SystemConfig.FiaLaneConfig[10] == 1))) \
			    FiaMuxConfig.PcieRootPortsConfiguration.MeFiaPcieRpConfig.PcieRp5En = 0;

// Disable RP6
#define DIS_RP6    \
            if (!((SystemConfig.FiaPcieRootPortLinkWidth[6] == ME_FIA_PCIE_ROOT_PORT_LINK_X1) && (SystemConfig.FiaLaneConfig[12] == 1))) \
			    FiaMuxConfig.PcieRootPortsConfiguration.MeFiaPcieRpConfig.PcieRp6En = 0;

// Disable RP7
#define DIS_RP7    \
            if (!((SystemConfig.FiaPcieRootPortLinkWidth[7] == ME_FIA_PCIE_ROOT_PORT_LINK_X1) && (SystemConfig.FiaLaneConfig[14] == 1))) \
			    FiaMuxConfig.PcieRootPortsConfiguration.MeFiaPcieRpConfig.PcieRp7En = 0;

// If Bifurcation is "x8" for Cluster 0, disable RootPort 0
#define IF_BIF_x8_DIS_RP0 \
            if (SystemConfig.BifCtl[0] == BifCtl_0_x8) \
                DIS_RP0
                     
// If Bifurcation is "x8" for Cluster 1, disable RootPort 4
#define IF_BIF_x8_DIS_RP4 \
            if (SystemConfig.BifCtl[1] == BifCtl_1_x8) \
                DIS_RP4

// If RP0 is x4 for Cluster 0, disable RootPort 0
#define IF_BIF_x4x2x2_Or_x4x4_DIS_RP0 \
            if (SystemConfig.BifCtl[1] == BifCtl_0_x4x2x2 || SystemConfig.BifCtl[0] == BifCtl_0_x4x4) \
               DIS_RP0

// If RP4 is x4 for Cluster 1, disable RootPort 4
#define IF_BIF_x4x2x2_Or_x4x4_DIS_RP4 \
            if (SystemConfig.BifCtl[1] == BifCtl_1_x4x2x2 || SystemConfig.BifCtl[1] == BifCtl_1_x4x4) \
                DIS_RP4
				
// If RP2 is x4 for Cluster 0, disable RootPort 2
#define IF_BIF_x2x2x4_Or_x4x4_DIS_RP2 \
            if (SystemConfig.BifCtl[0] == BifCtl_0_x2x2x4 || SystemConfig.BifCtl[0] == BifCtl_0_x4x4) \
                DIS_RP2

// If RP6 is x4 for Cluster 1, disable RootPort 6
#define IF_BIF_x2x2x4_Or_x4x4_DIS_RP6 \
            if (SystemConfig.BifCtl[1] == BifCtl_1_x2x2x4 || SystemConfig.BifCtl[1] == BifCtl_1_x4x4) \
                DIS_RP6
// Denverton AptioV override end - EIP#332934

EFI_STATUS
EFIAPI
UpdatePeiFiaMuxPolicy (
  IN OUT  FIA_MUX_GLOBAL_POLICY_PPI         *FiaMuxGlobalPolicyPpi
  )
/*++

Routine Description:

  This function performs FIA MUX PEI Platform Policy initialization.

Arguments:

  FiaMuxGlobalPolicyPpi            The FIA MUX Policy PPI instance

Returns:

  EFI_SUCCESS             The PPI is installed and initialized.
  EFI ERRORS              The PPI is not successfully installed.
  EFI_OUT_OF_RESOURCES    Do not have enough resources to initialize the driver

--*/
{
  EFI_STATUS                      Status;
  PLATFORM_INFO_HOB               *PlatformInfo;
  Status   = EFI_SUCCESS;
  PlatformInfo = NULL;

  DEBUG ((EFI_D_ERROR, "UpdatePeiFiaMuxPolicy\n"));
  //
  // Get the Platform default configuration
  //
  GetPlatformInfo(&PlatformInfo);
  ASSERT(PlatformInfo != NULL);
  if (PlatformInfo != NULL) {
     CopyMem(&FiaMuxGlobalPolicyPpi->FiaMuxConfig.FiaMuxConfigRequest, &PlatformInfo->FiaHsioInfo.FiaConfig, sizeof(ME_FIA_CONFIG));
  } else {
     DEBUG ((EFI_D_ERROR, "Platform Info Hob not Found - default Platform FIA configuration!!\n"));
  }

  if (PEI_IN_SIMICS) {
    DEBUG ((EFI_D_ERROR, "In SIMICs set FiaMuxConfig = FiaMuxConfigRequest\n"));  
    CopyMem(&FiaMuxGlobalPolicyPpi->FiaMuxConfig.FiaMuxConfig, &FiaMuxGlobalPolicyPpi->FiaMuxConfig.FiaMuxConfigRequest, sizeof(ME_FIA_CONFIG));   
  }
  //
  // Lastly, update the policy if there is a request to override current config request
  //
  SetupOverrideFiaMuxPolicy(FiaMuxGlobalPolicyPpi);
  
#if defined(SV_HOOKS)  || defined(CAR_LAKE_SUPPORT) 
  // For SVME build and Car Lake build, no support for FIA HECI messages, set the FIA Config equal to the FIA Config Request
  CopyMem(&FiaMuxGlobalPolicyPpi->FiaMuxConfig.FiaMuxConfig, &FiaMuxGlobalPolicyPpi->FiaMuxConfig.FiaMuxConfigRequest, sizeof(ME_FIA_CONFIG));
#endif // SV_HOOKS
  DEBUG((DEBUG_INFO, "SkuNumLanesAllowed: 0x%x\n", FiaMuxGlobalPolicyPpi->FiaMuxConfig.SkuNumLanesAllowed));
  DEBUG((DEBUG_INFO, "FiaMuxConfig.MuxConfiguration: 0x%lx\n", FiaMuxGlobalPolicyPpi->FiaMuxConfig.FiaMuxConfig.MuxConfiguration.MeFiaMuxLaneConfig));
  DEBUG((DEBUG_INFO, "FiaMuxConfig.SataLaneConfiguration: 0x%lx\n", FiaMuxGlobalPolicyPpi->FiaMuxConfig.FiaMuxConfig.SataLaneConfiguration.MeFiaSataLaneConfig));
  DEBUG((DEBUG_INFO, "FiaMuxConfig.PcieRootPortsConfiguration: 0x%lx\n", FiaMuxGlobalPolicyPpi->FiaMuxConfig.FiaMuxConfig.PcieRootPortsConfiguration.MeFiaPcieRootPortsConfig));
  DEBUG((DEBUG_INFO, "FiaMuxConfigRequest.MuxConfiguration: 0x%lx\n", FiaMuxGlobalPolicyPpi->FiaMuxConfig.FiaMuxConfigRequest.MuxConfiguration.MeFiaMuxLaneConfig));
  DEBUG((DEBUG_INFO, "FiaMuxConfigRequest.SataLaneConfiguration: 0x%lx\n", FiaMuxGlobalPolicyPpi->FiaMuxConfig.FiaMuxConfigRequest.SataLaneConfiguration.MeFiaSataLaneConfig));
  DEBUG((DEBUG_INFO, "FiaMuxConfigRequest.PcieRootPortsConfiguration: 0x%lx\n", FiaMuxGlobalPolicyPpi->FiaMuxConfig.FiaMuxConfigRequest.PcieRootPortsConfiguration.MeFiaPcieRootPortsConfig));
  DEBUG ((DEBUG_INFO, "FIA Mux Policy ready!!\n"));

  return Status;
}

EFI_STATUS
EFIAPI
SetupOverrideFiaMuxPolicy(
  IN OUT FIA_MUX_GLOBAL_POLICY_PPI *FiaMuxGlobalPolicyPpi
)
{
    EFI_STATUS                      Status;
    EFI_PEI_READ_ONLY_VARIABLE2_PPI *VariableServices;
    SYSTEM_CONFIGURATION            SystemConfig; 
    ME_FIA_CONFIG                   FiaMuxConfig;  
    UINTN                           BufferSize;
    UINT8                           Lane;
    Status   = EFI_SUCCESS;

    //
    // Retrieve FIA Mux configuration variable
    //
    Status = PeiServicesLocatePpi (
              &gEfiPeiReadOnlyVariable2PpiGuid, // GUID
              0,                                // INSTANCE
              NULL,                             // EFI_PEI_PPI_DESCRIPTOR
              &VariableServices                 // PPI
              );
    ASSERT_EFI_ERROR (Status);
    BufferSize = sizeof(SYSTEM_CONFIGURATION);
    Status = VariableServices->GetVariable (
              VariableServices,
              PLATFORM_SETUP_VARIABLE_NAME,
              &gSetupVariableGuid,
              NULL,
              &BufferSize,
              &SystemConfig
              );
    ASSERT_EFI_ERROR(Status);

    // Denverton AptioV override start - EIP#332934
    if (!EFI_ERROR(Status) && SystemConfig.FiaMuxOverride) {
        // If FIA MUX Override setup option is enabled follow the below steps:
        // STEP 1:  Initialize the LANE, SATA and PCIE Configuration structures to 0
        // STEP 2:  Preset the PCIE Configuration based on the Bifurcation settings from setup
        //          Note: Bifurcation control register also needs to be programmed accordingly.
		//                Updated the PcieIpClusterPolicy[Index].Bifurcation. in PeiPcieIpPolicyUpdate.c. 
        // STEP 3:  Parse through each LANE settings provided from the override setup options and do the following:
        //          (i) Program the FIA Sata Lane configuration
        //         (ii) Disable the RootPorts for which the LANES underneath are not assigned as PCIE.
        //          Note: Provide necessary considerations for Bifurcation settings while disabling the RootPorts
		//                Also takes care of RootPort Link Width. (Based on setup option) 
        // STEP 4: Update the FIA PCIE Root Port configuration from the Setup
        // STEP 5: Update the Override FIA Mux Policy HOB
        //-----------------------------------------------

        // STEP 1: Initialize the LANE, SATA and PCIE Configuration structures to 0

        FiaMuxConfig.MuxConfiguration.MeFiaMuxLaneConfig = 0;
        FiaMuxConfig.SataLaneConfiguration.MeFiaSataLaneConfig = 0;
        FiaMuxConfig.PcieRootPortsConfiguration.MeFiaPcieRootPortsConfig = 0;
        
        // STEP 2: Preset the PCIE Configuration based on the Bifurcation settings from setup
        
        // Based on Bifurcation Control 0 setup option: Initialize MeFiaPcieRootPortsConfig
        // After which, based on the LANE assignment (Disabled/PCIE/SATA), the corresponding RootPorts will be disabled
        //    =============================================================================================================================================
        //   |              Bifurcation            | BifCtl0 Value |   BifCtl1 Value |  RootPorts Enabled (for BifCtl0) | RootPorts Enabled (for BifCtl1)  |
        //    =============================================================================================================================================
        //   | PCI_BIF_CTRL_x2     | 000: x2x2x2x2 |  0x0F         | 0xF0            |      RP0, RP1, RP2, RP3          |      RP4, RP5, RP6, RP7          |
        //   | PCI_BIF_CTRL_x2x4   | 001: x2x2x4   |  0x07         | 0x70            |      RP0, RP1, RP2, --           |      RP4, RP5, RP6, --           |
        //   | PCI_BIF_CTRL_x4x2   | 010: x4x2x2   |  0x0D         | 0xD0            |      RP0, -- , RP2, RP3          |      RP4, -- , RP6, RP7          |
        //   | PCI_BIF_CTRL_x4     | 011: x4x4     |  0x05         | 0x50            |      RP0, -- , RP2, --           |      RP4, -- , RP6, --           |
        //   | PCI_BIF_CTRL_x8     | 100: x8       |  0x01         | 0x10            |      RP0, -- , -- , --           |      RP4, -- , -- , --           |
        //    =============================================================================================================================================
                
        // Check Bifurcation Control 0 setup option and program the MeFiaPcieRootPortsConfig initial value
        FiaMuxConfig.PcieRootPortsConfiguration.MeFiaPcieRootPortsConfig |= (UINT64)SystemConfig.BifCtl[0];
        
        // Check Bifurcation Control 1 setup option and program the MeFiaPcieRootPortsConfig initial value
        FiaMuxConfig.PcieRootPortsConfiguration.MeFiaPcieRootPortsConfig |= (UINT64)SystemConfig.BifCtl[1];

        // STEP 3: Parse through the LANE settings provided from the override setup options and do the following:
        //         - Program the FIA Sata Lane configuration
        //         - Disable the RootPorts for which the LANES underneath are not assigned as PCIE.
        //         Note: Provide necessary considerations for Bifurcation settings while disabling the RootPorts        

        for (Lane = 0; Lane < ME_FIA_MUX_LANE_NUM_MAX; Lane++) {
            
            // Update the FIA Lane Configuration
            FiaMuxConfig.MuxConfiguration.MeFiaMuxLaneConfig += (UINT64) LShiftU64 (SystemConfig.FiaLaneConfig[Lane], (2*Lane));

            // Program the FIA Sata Lane configuration
            // Loop through LANES that can be assigned to SATA
            if (Lane >= 4 && Lane <= 19){
                // Update the FIA SATA Lane configuration, check if LANE is assigned to SATA 
                if (SystemConfig.FiaLaneConfig[Lane] == ME_FIA_MUX_LANE_SATA){
                    FiaMuxConfig.SataLaneConfiguration.MeFiaSataLaneConfig += (UINT64) LShiftU64 (ME_FIA_SATA_CONTROLLER_LANE_ASSIGNED, (2*(Lane-4)));
                }
                else {
                      // Keep the SataLane as un-assigned if  LANE is not assigned to SATA
                      FiaMuxConfig.SataLaneConfiguration.MeFiaSataLaneConfig += (UINT64) LShiftU64 (ME_FIA_SATA_CONTROLLER_LANE_NOT_ASSIGNED, (2*(Lane-4)));
                }
            }

            // If PCIE ROOT PORT LINK WIDTH = "BICTRL" (Setup Default Case)=>: Disable the RP if ANY of the LANE below it is NOT assigned as PCIE
			// If PCIE ROOT PORT LINK WIDTH =   "X1"  =>: Do not disable the RP if its LANE[0], below it, is assigned as PCIE
            if (Lane < 16 && (SystemConfig.FiaLaneConfig[Lane] != ME_FIA_MUX_LANE_PCIE)){
                switch(Lane)
                {
                case 0:
                case 1:
                        DIS_RP0
                        break;
                case 2:
                case 3:
                        DIS_RP1
                        IF_BIF_x8_DIS_RP0
                        IF_BIF_x4x2x2_Or_x4x4_DIS_RP0
                        break;
                case 4:
                case 5:
                        DIS_RP2
                        IF_BIF_x8_DIS_RP0
                        break;
                case 6:
                case 7: 
                        DIS_RP3
                        IF_BIF_x8_DIS_RP0
                        IF_BIF_x2x2x4_Or_x4x4_DIS_RP2
                        break;
                case 8:
                case 9:
                        DIS_RP4
                        break;
                case 10:
                case 11:
                        DIS_RP5
                        IF_BIF_x8_DIS_RP4
                        IF_BIF_x4x2x2_Or_x4x4_DIS_RP4
                        break;
                case 12:
                case 13:
                        DIS_RP6
                        IF_BIF_x8_DIS_RP4
                        break;
                case 14:
                case 15:    
                        DIS_RP7
                        IF_BIF_x8_DIS_RP4
                        IF_BIF_x2x2x4_Or_x4x4_DIS_RP6
                        break;
                default:
                        break;
                }// End of "Switch"
           }// End of: "if (Lane < 16 && (SystemConfig.FiaLaneConfig[Lane] != 1))"
		
		// STEP 4: Update the FIA PCIE Root Port configuration from the Setup
        if (Lane < PCH_PCIE_MAX_ROOT_PORTS ){    
           FiaMuxConfig.PcieRootPortsConfiguration.MeFiaPcieRootPortsConfig += (UINT64) LShiftU64 (SystemConfig.FiaPcieRootPortLinkWidth[Lane], (8 + 4*Lane));
        } 

        } // End of: "for (Lane = 0; Lane < ME_FIA_MUX_LANE_NUM_MAX; Lane++)"
        

        //STEP 5: Update the Override FIA Mux Policy HOB
        DEBUG ((EFI_D_ERROR, "UpdateOverrideFiaMuxPolicy\n"));
        CopyMem(&FiaMuxGlobalPolicyPpi->FiaMuxConfig.FiaMuxConfigRequest.MuxConfiguration, &FiaMuxConfig.MuxConfiguration, sizeof(ME_FIA_MUX_CONFIG));

        // Update the Pcie and Sata configuration policy
        CopyMem(&FiaMuxGlobalPolicyPpi->FiaMuxConfig.FiaMuxConfigRequest.PcieRootPortsConfiguration, &FiaMuxConfig.PcieRootPortsConfiguration, sizeof(ME_FIA_PCIE_ROOT_PORTS_CONFIG));
        CopyMem(&FiaMuxGlobalPolicyPpi->FiaMuxConfig.FiaMuxConfigRequest.SataLaneConfiguration, &FiaMuxConfig.SataLaneConfiguration, sizeof(ME_FIA_SATA_CONFIG));

	} //End of : "if (!EFI_ERROR(Status) && SystemConfig.FiaMuxOverride)"
	// Denverton AptioV override end - EIP#332934
    return Status;
}
