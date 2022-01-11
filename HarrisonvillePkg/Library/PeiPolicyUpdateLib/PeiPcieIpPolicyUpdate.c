/*++
  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may be modified by the user, subject to
  the additional terms of the license agreement
--*/
/*++

Copyright (c) 2016 - 2017 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

Module Name:

  PchPlatformPolicyUpdatePei.c

Abstract:

  This file is SampleCode of the library for Intel PCH PEI Platform Policy initialzation.

--*/
#include <PeiPcieIpPolicyUpdate.h>
#include <Guid/PlatformInfo.h>
#include <PchReservedResources.h>
#include <Register/PchRegsPcr.h>
#include <Library/PeiPlatformLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Guid/GlobalVariable.h>
#include <Library/DebugLib.h>
#include <PcieIpRegs.h>

#ifdef ECP_FLAG
EFI_GUID gEfiGlobalVariableGuid = EFI_GLOBAL_VARIABLE_GUID;
#endif

UINT32
UpdatePeiPcieClusterSoftStrap (
  IN UINT8 clusterPcie
  )
/*++

Routine Description:

  Get straps

Arguments:
  @param[in] clusterPcie 			- Cluster number

Returns:
  @retval bifurcation
--*/
{
  UINT32 pcieStrap = 0;
  if ( clusterPcie == 0 ) {
	  pcieStrap = MmioRead32( (UINTN) (PCH_PCR_ADDRESS(PID_PCIE0, R_PCIE_STRAPCSR1) ) );
  } else {
	  if ( clusterPcie == 1 ) {
	    pcieStrap = MmioRead32( (UINTN) (PCH_PCR_ADDRESS(PID_PCIE1, R_PCIE_STRAPCSR1) ) );
	  } else {
		  DEBUG ((EFI_D_ERROR, "PCIE cluster invalid\n"));
		  ASSERT( FALSE );
	  }
  }
  return pcieStrap;
}

UINT8
UpdatePeiPcieBifurcationFromPlatformInfo (
  IN UINT8 ClusterNumber
  )
/*++

Routine Description:

  get bifurcation from a Trident. Request this information from platform

Arguments:
  @param[in] Cluster 			- number cluster

Returns:
  @retval bifurcation
--*/
{
	PLATFORM_INFO_HOB *PlatformInfo = NULL;
	EFI_STATUS        Status;

	Status = GetPlatformInfo (&PlatformInfo);
	ASSERT_EFI_ERROR (Status);

	ASSERT (PlatformInfo != NULL);
	if (PlatformInfo == NULL) {
	  DEBUG ((EFI_D_ERROR, "\nERROR: UpdatePeiPcieBifurcationFromPlatformInfo () - Attempting to use a NULL pointer.\n"));
	  return 0;
	}

	return ( PlatformInfo->FiaHsioInfo.PcieBifCtr[ ClusterNumber ] );
}

EFI_STATUS
EFIAPI
UpdatePeiPcieIpPolicy (
  IN OUT      PCIE_IP_GLOBAL_POLICY_PPI       *PcieIpGlobalPolicyPpi
  )
/*++

Routine Description:

  This function performs PCIE IP PEI Platform Policy initialzation.

Arguments:

  PcieIpPolicy            The Pcie Ip Platform Policy PPI instance

Returns:

  EFI_SUCCESS             The PPI is installed and initialized.
  EFI ERRORS              The PPI is not successfully installed.
  EFI_OUT_OF_RESOURCES    Do not have enough resources to initialize the driver

--*/
{

  EFI_PEI_READ_ONLY_VARIABLE2_PPI *VariableServices;
  UINTN                           VariableSize;
  EFI_GUID                        SystemConfigurationGuid = SYSTEM_CONFIGURATION_GUID;
  SYSTEM_CONFIGURATION            SetupVariables;
  UINT8                           Index;
  EFI_STATUS					  Status = EFI_SUCCESS;

  VariableSize = sizeof (SYSTEM_CONFIGURATION);

//
// Retrieve Setup variable
//
  Status = PeiServicesLocatePpi (
                            &gEfiPeiReadOnlyVariable2PpiGuid, // GUID
                            0,                            // INSTANCE
                            NULL,                         // EFI_PEI_PPI_DESCRIPTOR
                            &VariableServices             // PPI
                            );
  ASSERT_EFI_ERROR (Status);
  // Denverton Aptiov override Start - EIP#217439
  Status = VariableServices->GetVariable (
                              VariableServices,
                              L"IntelSetup",
                              &SystemConfigurationGuid,
                              NULL,
                              &VariableSize,
                              &SetupVariables
                              );
  // Denverton Aptiov override End - EIP#217439
  ASSERT_EFI_ERROR (Status);

  //
  // PCI express config, endpoints
  //
  // Denverton AptioV Override Start - EIP#235760
  /*
  PcieIpGlobalPolicyPpi->PcieIPEndPoints.DeviceCtlConfig.MaxPayload                 = SetupVariables.PcieEPMaxPayload;
  PcieIpGlobalPolicyPpi->PcieIPEndPoints.DeviceCtlConfig.MaxReadRequest             = SetupVariables.PcieEPMaxReadRequest;
  PcieIpGlobalPolicyPpi->PcieIPEndPoints.DeviceCtlConfig.ExtTagField                = SetupVariables.PcieEPExtTagField;
  PcieIpGlobalPolicyPpi->PcieIPEndPoints.DeviceCtlConfig.RelaxedOrdering            = SetupVariables.PcieEPRelaxedOrdering;
  PcieIpGlobalPolicyPpi->PcieIPEndPoints.ExtSync                                    = SetupVariables.PcieEPExtSync;
  PcieIpGlobalPolicyPpi->PcieIPEndPoints.Aspm                                       = SetupVariables.PcieEPAspm;
  */
  // Denverton AptioV Override End - EIP#235760

  //
  // PCI express config, root ports
  //
  for(Index = 0; Index < PCIE_IP_STANDARD_ROOT_PORT; Index++) {
	PcieIpGlobalPolicyPpi->PcieIpPortPolicy[Index].PcieSpeed                       = SetupVariables.RootPortLinkSpeed[Index];
	PcieIpGlobalPolicyPpi->PcieIpPortPolicy[ Index ].ClockGatingEn                 = SetupVariables.RootPortClockGating[Index];
    PcieIpGlobalPolicyPpi->PcieIpPortPolicy[Index].DeviceCtlConfig.MaxPayload      = SetupVariables.RootPortMaxPayload[Index];
    PcieIpGlobalPolicyPpi->PcieIpPortPolicy[Index].DeviceCtlConfig.MaxReadRequest  = SetupVariables.RootPortMaxReadRequest[Index];
    PcieIpGlobalPolicyPpi->PcieIpPortPolicy[ Index ].VirtualRootPort = FALSE;
	PcieIpGlobalPolicyPpi->PcieIpPortPolicy[Index].ExtTagField                     = SetupVariables.RootPortExtTagField[Index];
    PcieIpGlobalPolicyPpi->PcieIpPortPolicy[Index].DeviceCtlConfig.RelaxedOrdering = SetupVariables.RootPortRelaxedOrdering[Index];
    PcieIpGlobalPolicyPpi->PcieIpPortPolicy[Index].HotPlug                         = SetupVariables.PcieRootPortHPE[Index];
    PcieIpGlobalPolicyPpi->PcieIpPortPolicy[Index].Aspm                            = SetupVariables.PcieRootPortAspm[Index];
    PcieIpGlobalPolicyPpi->PcieIpPortPolicy[Index].DeviceCtlConfig.EnableSLD       = SetupVariables.PcieRootPortSLD[Index];
    PcieIpGlobalPolicyPpi->PcieIpPortPolicy[ Index ].Lock                          = SetupVariables.CapabilityLock[Index];
    PcieIpGlobalPolicyPpi->PcieIpPortPolicy[Index].ExtSync                         = SetupVariables.RootPortExtSync[Index];
	PcieIpGlobalPolicyPpi->PcieIpPortPolicy[Index].DeEmphasis                      = SetupVariables.RootPortDeEmphasis[Index];
    PcieIpGlobalPolicyPpi->PcieIpPortPolicy[Index].CtlLaneReversal                 = SetupVariables.RootPortLaneReversal[Index];
	PcieIpGlobalPolicyPpi->PcieIpPortPolicy[Index].CompletionTimeout               = SetupVariables.RootPortCTO[Index];
	PcieIpGlobalPolicyPpi->PcieIpPortPolicy[Index].CompletionTimeoutRange          = SetupVariables.RootPortCTORange[Index];
	PcieIpGlobalPolicyPpi->PcieIpPortPolicy[Index].StopScream                      = SetupVariables.RootPortStopScream[Index];

    PcieIpGlobalPolicyPpi->PcieIpPortPolicy[Index].WheaPcieErrInj                  = SetupVariables.WheaPcieErrInj;
	if ( SetupVariables.PcieRootPortEn[Index] == 1 ){
	  PcieIpGlobalPolicyPpi->PcieIpPortBdfMap[Index].Enabled = TRUE;
	}else{
	  PcieIpGlobalPolicyPpi->PcieIpPortBdfMap[Index].Enabled = FALSE;
	}

	PcieIpGlobalPolicyPpi->PcieIpPortBdfMap[Index].SlotImplemented 	               = SetupVariables.PcieRootPortEn[Index];
	PcieIpGlobalPolicyPpi->PcieIpPortPolicy[Index].PmSci 				           = SetupVariables.PcieRootPortPMCE[Index];
	PcieIpGlobalPolicyPpi->PcieIpPortPolicy[Index].UnsupportedRequestReport        = SetupVariables.PcieRootPortURE[Index];
  }

  PcieIpGlobalPolicyPpi->EnableMphyMsg  = SetupVariables.EnableMphyMsg;

  for( Index = PCIE_IP_STANDARD_ROOT_PORT ; Index < MAX_PCIE_ROOT_PORTS ; Index++ ) {
    PcieIpGlobalPolicyPpi->PcieIpPortPolicy[ Index ].VirtualRootPort = TRUE;
  }

  for (Index = 0; Index < PCH_MAX_PCIE_CLUSTERS; Index++) {
//	PcieIpGlobalPolicyPpi->PcieIpClusterPolicy[Index].ReplyTimerTimeout               = SetupVariables.RootPortRTTO;	// Denverton AptioV Override
	if ( SetupVariables.bifurcation[Index] != V_PCIE_BIFURCATION_AUTO_OPTION ) {
      PcieIpGlobalPolicyPpi->PcieIpClusterPolicy[ Index ].Bifurcation = SetupVariables.bifurcation[Index];
	} else {
      PcieIpGlobalPolicyPpi->PcieIpClusterPolicy[ Index ].Bifurcation = UpdatePeiPcieBifurcationFromPlatformInfo( Index );
	}
    PcieIpGlobalPolicyPpi->PcieIpClusterPolicy[ Index ].Straps = UpdatePeiPcieClusterSoftStrap( Index );

    // Denverton AptioV override start - EIP#332934
    // Modify bifurcation, if FIA MUX Override option is enabled
    if (SetupVariables.FiaMuxOverride){
        switch (SetupVariables.BifCtl[Index]){
            case 0x01:
            case 0x10:
                PcieIpGlobalPolicyPpi->PcieIpClusterPolicy[Index].Bifurcation = X8;
                break;
            case 0x05:
            case 0x50:
                PcieIpGlobalPolicyPpi->PcieIpClusterPolicy[Index].Bifurcation = X4X4;
                break;
            case 0x0D:
            case 0xD0:
                PcieIpGlobalPolicyPpi->PcieIpClusterPolicy[Index].Bifurcation = X4X2X2;
                break;
            case 0x07:
            case 0x70:
                PcieIpGlobalPolicyPpi->PcieIpClusterPolicy[Index].Bifurcation = X2X2X4;
                break;
            case 0x0F:
            case 0xF0:
                PcieIpGlobalPolicyPpi->PcieIpClusterPolicy[Index].Bifurcation = X2X2X2X2;
                break;
            default:
                break;
        }
    }
    // Denverton AptioV override end - EIP#332934
  }

  DEBUG ((DEBUG_INFO, "Pcie IP Policy ready!!\n"));
  

  return Status;
}
