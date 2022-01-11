/** @file
  This file is SampleCode of the library for Intel PCH PEI Policy initialization.

@copyright
 Copyright (c) 2004 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
 with the terms of the license. Except as permitted by the
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.
 This file contains a 'Sample Driver' and is licensed as such
 under the terms of your license agreement with Intel or your
 vendor. This file may be modified by the user, subject to
 the additional terms of the license agreement.

@par Specification Reference:
**/

#include <PeiPchPolicyUpdate.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/HobLib.h>
#include <Guid/GlobalVariable.h>
#include <Library/PchInfoLib.h>
#include <Library/PchPcrLib.h>
#include <Library/PchHsioLib.h>
#include <Ppi/PchHsioPtssTable.h>
#include "PchHHsioPtssTablesBx.h"
#include <Ppi/Spi.h>
#include <Library/PcdLib.h>
#include <PcieIpPlatformLib.h>
#include <Library/PeiDxeSmmFiaMuxHelperLib.h>
// Denverton AptioV override Start

#ifdef HPET_SUPPORT
#undef HPET_SUPPORT
#undef PASSWORD_MAX_SIZE
#undef PCI_EXPRESS_SUPPORT
#endif
#include <Token.h>
// Denverton AptioV override End
//
// Sawtooth Peak
// Single SPD EEPROM at 0xA2 serves both C0D0 and C1D0 (LPDDR is 1DPC only)
//
#define DIMM_SMB_SPD_P0C0D0_STP 0xA2
#define DIMM_SMB_SPD_P0C0D1_STP 0xA0
#define DIMM_SMB_SPD_P0C1D0_STP 0xA2
#define DIMM_SMB_SPD_P0C1D1_STP 0xA0

GLOBAL_REMOVE_IF_UNREFERENCED UINT8 mSmbusSTPRsvdAddresses[] = {
  DIMM_SMB_SPD_P0C0D0_STP,
  DIMM_SMB_SPD_P0C0D1_STP,
  DIMM_SMB_SPD_P0C1D0_STP,
  DIMM_SMB_SPD_P0C1D1_STP
};


VOID
InstallPlatformHsioPtssTable (
  IN          UINT16             BoardId,
  IN          PCH_SETUP         *PchSetup,
  IN OUT      PCH_POLICY_PPI    *PchPolicy
  )
{
  HSIO_PTSS_TABLES            *PtssTables;
  UINT8                       PtssTableIndex;
  UINT32                      TableSize;
  UINT32                      Entry;
  UINT8                       LaneNum;
  UINT8                       Index;
  UINT8                       MaxSataPorts;
  UINT8                       MaxPciePorts;
  UINT8                       PcieTopologyReal[ MAX_PCIE_LANES ];
    DEBUG ((DEBUG_INFO, "InstallPlatformHsioPtssTable begin\n"));
  switch (PchStepping ()) {
    case PchHA0:
    //Break omitted on purpose
    case PchHB0:
        DEBUG ((DEBUG_INFO, "PTSS not being initialized for this stepping \n"));
        PtssTables = PchHHsioPtss_Bx;
        TableSize = PchHHsioPtss_Bx_Size;
        return;
    default:
      PtssTables = NULL;
      TableSize = 0;
      DEBUG ((DEBUG_ERROR, "Unsupported PCH Stepping\n"));
  }
  PtssTableIndex = 0;
  MaxSataPorts = GetPchMaxSataPortNum () * MAX_SATA_CTRL;
  MaxPciePorts = PcieIPGetPchStandardPciePortNum () * MAX_PCIE_CLUSTERS;
  ZeroMem (PcieTopologyReal, sizeof (PcieTopologyReal));
  //Populate PCIe topology based on lane configuration
  for (Index = 0; Index < MaxPciePorts; Index++) {
    PcieTopologyReal[Index] = PchSetup->PcieTopology[Index];
    DEBUG ((DEBUG_INFO, "PCIE lane %d Topology %d \n", Index, PcieTopologyReal[Index] ));
  }
  //Case 1: BoardId is known, Topology is known/unknown
  //Case 1a: SATA
  for (Index = 0; Index < MaxSataPorts; Index++) {
    if (PchGetSataLaneNum (Index, &LaneNum) == EFI_SUCCESS) {
      for (Entry = 0; Entry < TableSize; Entry++) {
        if ((LaneNum == PtssTables[Entry].PtssTable.LaneNum) &&
          (PtssTables[Entry].PtssTable.PhyMode == V_PCH_PCR_FIA_LANE_OWN_SATA) &&
          (PchSetup->SataTopology[Index] == PtssTables[Entry].Topology) &&
          (BoardId == PtssTables[Entry].BoardId)) {
          PtssTableIndex++;
          if ((PtssTables[Entry].PtssTable.Offset == (UINT32) R_PCH_HSIO_RX_DWORD20) &&
            (((UINT32) ~PtssTables[Entry].PtssTable.BitMask & B_PCH_HSIO_RX_DWORD20_ICFGCTLEDATATAP_FULLRATE_5_0) == (UINT32) B_PCH_HSIO_RX_DWORD20_ICFGCTLEDATATAP_FULLRATE_5_0)) {
        	if ( Index > 7 ) {
              PchPolicy->SataConfig[1].PortSettings[Index - PCH_MAX_SATA_PORTS ].HsioRxEqBoostMagAdEnable = TRUE;
              PchPolicy->SataConfig[1].PortSettings[Index - PCH_MAX_SATA_PORTS].HsioRxEqBoostMagAd = (PtssTables[Entry].PtssTable.Value & (UINT32) ~PtssTables[Entry].PtssTable.BitMask) >> N_PCH_HSIO_RX_DWORD20_ICFGCTLEDATATAP_FULLRATE_5_0;
        	} else {
              PchPolicy->SataConfig[0].PortSettings[Index].HsioRxEqBoostMagAdEnable = TRUE;
              PchPolicy->SataConfig[0].PortSettings[Index].HsioRxEqBoostMagAd = (PtssTables[Entry].PtssTable.Value & (UINT32) ~PtssTables[Entry].PtssTable.BitMask) >> N_PCH_HSIO_RX_DWORD20_ICFGCTLEDATATAP_FULLRATE_5_0;
        	}
          } else if ((PtssTables[Entry].PtssTable.Offset == (UINT32) R_PCH_HSIO_TX_DWORD8) &&
            (((UINT32) ~PtssTables[Entry].PtssTable.BitMask & (UINT32) B_PCH_HSIO_TX_DWORD8_ORATE00MARGIN_5_0) == (UINT32) B_PCH_HSIO_TX_DWORD8_ORATE00MARGIN_5_0)) {
        	if ( Index > 7 ) {
              PchPolicy->SataConfig[1].PortSettings[Index - PCH_MAX_SATA_PORTS ].HsioTxGen1DownscaleAmpEnable = TRUE;
              PchPolicy->SataConfig[1].PortSettings[Index - PCH_MAX_SATA_PORTS ].HsioTxGen1DownscaleAmp = (PtssTables[Entry].PtssTable.Value & (UINT32) B_PCH_HSIO_TX_DWORD8_ORATE00MARGIN_5_0) >> N_PCH_HSIO_TX_DWORD8_ORATE00MARGIN_5_0;
        	} else {
              PchPolicy->SataConfig[0].PortSettings[Index].HsioTxGen1DownscaleAmpEnable = TRUE;
              PchPolicy->SataConfig[0].PortSettings[Index].HsioTxGen1DownscaleAmp = (PtssTables[Entry].PtssTable.Value & (UINT32) B_PCH_HSIO_TX_DWORD8_ORATE00MARGIN_5_0) >> N_PCH_HSIO_TX_DWORD8_ORATE00MARGIN_5_0;
        	}
          } else if ((PtssTables[Entry].PtssTable.Offset == (UINT32) R_PCH_HSIO_TX_DWORD8) &&
            (((UINT32) ~PtssTables[Entry].PtssTable.BitMask & (UINT32) B_PCH_HSIO_TX_DWORD8_ORATE01MARGIN_5_0) == (UINT32) B_PCH_HSIO_TX_DWORD8_ORATE01MARGIN_5_0)) {
        	if ( Index > 7 ) {
              PchPolicy->SataConfig[1].PortSettings[Index - PCH_MAX_SATA_PORTS ].HsioTxGen2DownscaleAmpEnable = TRUE;
              PchPolicy->SataConfig[1].PortSettings[Index - PCH_MAX_SATA_PORTS ].HsioTxGen2DownscaleAmp = (PtssTables[Entry].PtssTable.Value & (UINT32) B_PCH_HSIO_TX_DWORD8_ORATE01MARGIN_5_0) >> N_PCH_HSIO_TX_DWORD8_ORATE01MARGIN_5_0;
        	} else {
              PchPolicy->SataConfig[0].PortSettings[Index].HsioTxGen2DownscaleAmpEnable = TRUE;
              PchPolicy->SataConfig[0].PortSettings[Index].HsioTxGen2DownscaleAmp = (PtssTables[Entry].PtssTable.Value & (UINT32) B_PCH_HSIO_TX_DWORD8_ORATE01MARGIN_5_0) >> N_PCH_HSIO_TX_DWORD8_ORATE01MARGIN_5_0;
        	}
          } else {
            ASSERT (FALSE);
          }
        }
      }
    }
  }
  //Case 1b: PCIe
  for (Index = 0; Index < MaxPciePorts; Index++) {
    if (PchGetPcieLaneNum (Index, &LaneNum) == EFI_SUCCESS) {
      for (Entry = 0; Entry < TableSize; Entry++) {
        if ((LaneNum == PtssTables[Entry].PtssTable.LaneNum) &&
          (PtssTables[Entry].PtssTable.PhyMode == V_PCH_PCR_FIA_LANE_OWN_PCIEDMI) &&
          (PcieTopologyReal[Index] == PtssTables[Entry].Topology) &&
          (BoardId == PtssTables[Entry].BoardId)) {
          PtssTableIndex++;
          if ((PtssTables[Entry].PtssTable.Offset == (UINT32) R_PCH_HSIO_RX_DWORD25) &&
            (((UINT32) ~PtssTables[Entry].PtssTable.BitMask & B_PCH_HSIO_RX_DWORD25_CTLE_ADAPT_OFFSET_CFG_4_0) == (UINT32) B_PCH_HSIO_RX_DWORD25_CTLE_ADAPT_OFFSET_CFG_4_0)) {
        	if ( Index > 7 ) {
              PchPolicy->PcieConfig[1].RootPort[Index - PCIE_IP_STANDARD_ROOT_PORT].HsioRxSetCtleEnable = TRUE;
              PchPolicy->PcieConfig[1].RootPort[Index - PCIE_IP_STANDARD_ROOT_PORT].HsioRxSetCtle = (PtssTables[Entry].PtssTable.Value & (UINT32) ~PtssTables[Entry].PtssTable.BitMask) >> N_PCH_HSIO_RX_DWORD25_CTLE_ADAPT_OFFSET_CFG_4_0;
        	} else {
                PchPolicy->PcieConfig[0].RootPort[Index].HsioRxSetCtleEnable = TRUE;
                PchPolicy->PcieConfig[0].RootPort[Index].HsioRxSetCtle = (PtssTables[Entry].PtssTable.Value & (UINT32) ~PtssTables[Entry].PtssTable.BitMask) >> N_PCH_HSIO_RX_DWORD25_CTLE_ADAPT_OFFSET_CFG_4_0;
        	}
          } else {
            ASSERT (FALSE);
          }
        }
      }
    }
  }
  //Case 2: BoardId is unknown, Topology is known/unknown
  if (PtssTableIndex == 0) {
    DEBUG ((DEBUG_INFO, "PTSS Settings for unknown board will be applied\n"));
    for (Index = 0; Index < MaxSataPorts; Index++) {
      if (PchGetSataLaneNum (Index, &LaneNum) == EFI_SUCCESS) {
        for (Entry = 0; Entry < TableSize; Entry++) {
          if ((LaneNum == PtssTables[Entry].PtssTable.LaneNum) &&
            (PtssTables[Entry].PtssTable.PhyMode == V_PCH_PCR_FIA_LANE_OWN_SATA) &&
            (PchSetup->SataTopology[Index] == PtssTables[Entry].Topology) &&
            (PtssTables[Entry].BoardId == BoardIdUnknown1)) {
            if ((PtssTables[Entry].PtssTable.Offset == (UINT32) R_PCH_HSIO_RX_DWORD20) &&
              (((UINT32) ~PtssTables[Entry].PtssTable.BitMask & B_PCH_HSIO_RX_DWORD20_ICFGCTLEDATATAP_FULLRATE_5_0) == (UINT32) B_PCH_HSIO_RX_DWORD20_ICFGCTLEDATATAP_FULLRATE_5_0)) {
        	if ( Index > 7 ) {
              PchPolicy->SataConfig[1].PortSettings[Index - PCH_MAX_SATA_PORTS ].HsioRxEqBoostMagAdEnable = TRUE;
              PchPolicy->SataConfig[1].PortSettings[Index - PCH_MAX_SATA_PORTS ].HsioRxEqBoostMagAd = (PtssTables[Entry].PtssTable.Value & (UINT32) ~PtssTables[Entry].PtssTable.BitMask) >> N_PCH_HSIO_RX_DWORD20_ICFGCTLEDATATAP_FULLRATE_5_0;
        	} else {
              PchPolicy->SataConfig[0].PortSettings[Index].HsioRxEqBoostMagAdEnable = TRUE;
              PchPolicy->SataConfig[0].PortSettings[Index].HsioRxEqBoostMagAd = (PtssTables[Entry].PtssTable.Value & (UINT32) ~PtssTables[Entry].PtssTable.BitMask) >> N_PCH_HSIO_RX_DWORD20_ICFGCTLEDATATAP_FULLRATE_5_0;
        	}
            } else if ((PtssTables[Entry].PtssTable.Offset == (UINT32) R_PCH_HSIO_TX_DWORD8) &&
              (((UINT32) ~PtssTables[Entry].PtssTable.BitMask & (UINT32) B_PCH_HSIO_TX_DWORD8_ORATE00MARGIN_5_0) == (UINT32) B_PCH_HSIO_TX_DWORD8_ORATE00MARGIN_5_0)) {
        	if ( Index > 7 ) {
              PchPolicy->SataConfig[1].PortSettings[Index - PCH_MAX_SATA_PORTS ].HsioTxGen1DownscaleAmpEnable = TRUE;
              PchPolicy->SataConfig[1].PortSettings[Index - PCH_MAX_SATA_PORTS ].HsioTxGen1DownscaleAmp = (PtssTables[Entry].PtssTable.Value & (UINT32) B_PCH_HSIO_TX_DWORD8_ORATE00MARGIN_5_0) >> N_PCH_HSIO_TX_DWORD8_ORATE00MARGIN_5_0;
        	} else {
              PchPolicy->SataConfig[0].PortSettings[Index].HsioTxGen1DownscaleAmpEnable = TRUE;
              PchPolicy->SataConfig[0].PortSettings[Index].HsioTxGen1DownscaleAmp = (PtssTables[Entry].PtssTable.Value & (UINT32) B_PCH_HSIO_TX_DWORD8_ORATE00MARGIN_5_0) >> N_PCH_HSIO_TX_DWORD8_ORATE00MARGIN_5_0;
        	}
            } else if ((PtssTables[Entry].PtssTable.Offset == (UINT32) R_PCH_HSIO_TX_DWORD8) &&
              (((UINT32) ~PtssTables[Entry].PtssTable.BitMask & (UINT32) B_PCH_HSIO_TX_DWORD8_ORATE01MARGIN_5_0) == (UINT32) B_PCH_HSIO_TX_DWORD8_ORATE01MARGIN_5_0)) {
        	if ( Index > 7 ) {
              PchPolicy->SataConfig[1].PortSettings[Index - PCH_MAX_SATA_PORTS ].HsioTxGen2DownscaleAmpEnable = TRUE;
              PchPolicy->SataConfig[1].PortSettings[Index - PCH_MAX_SATA_PORTS ].HsioTxGen2DownscaleAmp = (PtssTables[Entry].PtssTable.Value & (UINT32) B_PCH_HSIO_TX_DWORD8_ORATE01MARGIN_5_0) >> N_PCH_HSIO_TX_DWORD8_ORATE01MARGIN_5_0;
        	} else {
              PchPolicy->SataConfig[0].PortSettings[Index].HsioTxGen2DownscaleAmpEnable = TRUE;
              PchPolicy->SataConfig[0].PortSettings[Index].HsioTxGen2DownscaleAmp = (PtssTables[Entry].PtssTable.Value & (UINT32) B_PCH_HSIO_TX_DWORD8_ORATE01MARGIN_5_0) >> N_PCH_HSIO_TX_DWORD8_ORATE01MARGIN_5_0;
        	}
            } else {
              ASSERT (FALSE);
            }
          }
        }
      }
    }
    for (Index = 0; Index < MaxPciePorts; Index++) {
      if (PchGetPcieLaneNum (Index, &LaneNum) == EFI_SUCCESS) {
        for (Entry = 0; Entry < TableSize; Entry++) {
          if ((LaneNum == PtssTables[Entry].PtssTable.LaneNum) &&
            (PtssTables[Entry].PtssTable.PhyMode == V_PCH_PCR_FIA_LANE_OWN_PCIEDMI) &&
            (PcieTopologyReal[Index] == PtssTables[Entry].Topology) &&
            (PtssTables[Entry].BoardId == BoardIdUnknown1)) {
            if ((PtssTables[Entry].PtssTable.Offset == (UINT32) R_PCH_HSIO_RX_DWORD25) &&
              (((UINT32) ~PtssTables[Entry].PtssTable.BitMask & B_PCH_HSIO_RX_DWORD25_CTLE_ADAPT_OFFSET_CFG_4_0) == (UINT32) B_PCH_HSIO_RX_DWORD25_CTLE_ADAPT_OFFSET_CFG_4_0)) {
        	if ( Index > 7 ) {
              PchPolicy->PcieConfig[1].RootPort[Index - PCIE_IP_STANDARD_ROOT_PORT ].HsioRxSetCtleEnable = TRUE;
              PchPolicy->PcieConfig[1].RootPort[Index - PCIE_IP_STANDARD_ROOT_PORT ].HsioRxSetCtle = (PtssTables[Entry].PtssTable.Value & (UINT32) ~PtssTables[Entry].PtssTable.BitMask) >> N_PCH_HSIO_RX_DWORD25_CTLE_ADAPT_OFFSET_CFG_4_0;
        	} else {
              PchPolicy->PcieConfig[0].RootPort[Index].HsioRxSetCtleEnable = TRUE;
              PchPolicy->PcieConfig[0].RootPort[Index].HsioRxSetCtle = (PtssTables[Entry].PtssTable.Value & (UINT32) ~PtssTables[Entry].PtssTable.BitMask) >> N_PCH_HSIO_RX_DWORD25_CTLE_ADAPT_OFFSET_CFG_4_0;
        	}
            } else {
              ASSERT (FALSE);
            }
          }
        }
      }
    }
  }
}


VOID
UpdatePeiPchPlatformPolicySata(UINT8 ControllerIndex, PCH_SATA_CONFIG *SataConfig,
                               PCH_SETUP *PchSetup)
{
  UINT8                           Index;
  UINT8                           MaxSataPorts;
  UINT8                           SetupPortIndex;
  //DNV TODO: here update depending the type of board. Need to check Platform info (enable disable port)

  //
  // Controller settings
  //
  SataConfig->Enable          = PchSetup->PchSata[ControllerIndex];
  SataConfig->SataMode        = PchSataModeAhci; //PchSetup->SataInterfaceMode[ControllerIndex]; //Raid not supported
  SataConfig->SalpSupport     = PchSetup->SataSalp[ControllerIndex];
  SataConfig->LpmSupport      = PchSetup->SataLpm[ControllerIndex];
  SataConfig->SpeedLimit      = PchSetup->SataSpeedLimit[ControllerIndex];
  SataConfig->PortMultiplier  = PchSetup->PortMultiplierSupported[ControllerIndex];
  SataConfig->eSATASpeedLimit = FALSE;
  SataConfig->Rst.HddUnlock                          = FALSE; //PchSetup->SataHddlk;
  SataConfig->Rst.LedLocate                          = FALSE; //PchSetup->SataLedl;



  //
  //Port settings
  //
  MaxSataPorts = GetPchMaxSataPortNum ();
  for (Index = 0; Index < MaxSataPorts; Index++) {

    SetupPortIndex = Index + (ControllerIndex * MaxSataPorts);

    if (PchSetup->SataTestMode[ControllerIndex] == TRUE) {
      SataConfig->PortSettings[Index].Enable = TRUE;
    } else {
      SataConfig->PortSettings[Index].Enable = PchSetup->SataPort[SetupPortIndex];
    }
    if(SataConfig->LpmSupport == TRUE)
    {
      SataConfig->PortSettings[Index].HotPlug          = FALSE;
    }else
    {
      SataConfig->PortSettings[Index].HotPlug          = PchSetup->SataHotPlug[SetupPortIndex];
    }
    SataConfig->PortSettings[Index].SpinUp           = PchSetup->SataSpinUp[SetupPortIndex];
    SataConfig->PortSettings[Index].External         = FALSE;
    SataConfig->PortSettings[Index].DevSlp           = FALSE;  //DNV TODO: PchSetup->PxDevSlp[SetupPortIndex];
    SataConfig->PortSettings[Index].EnableDitoConfig = FALSE;  //DNV TODO: PchSetup->EnableDitoConfig[SetupPortIndex];
    SataConfig->PortSettings[Index].DmVal            = 15;     //DNV TODO: PchSetup->DmVal[SetupPortIndex];
    SataConfig->PortSettings[Index].DitoVal          = 625;    //DNV TODO: PchSetup->DitoVal[SetupPortIndex];
    //DNV TODO: Need to port Policy consumption of SolidStateDrive from:
    //SkylakeSiPkg\Pch\LibraryPrivate\PeiPchInitLib\PchThermal.c
    SataConfig->PortSettings[Index].SolidStateDrive  = FALSE; //DNV TODO: PchSetup->SataType[SetupPortIndex];
    SataConfig->PortSettings[Index].InterlockSw      = FALSE; //DNV TODO: PchSetup->SataMechanicalSw[SetupPortIndex];
    SataConfig->PortSettings[Index].InterlockSw      = PchSetup->SataMechanicalSw[SetupPortIndex];

  }
}

/**
  This function performs Sata disabling according to FIA config

  @param[in] ControllerIndex    PCH policy controller index
  @param[in] PlatformInfo       PlatfomrInfo Hob
  @param[in] PchPolicy          PCH policy

  @retval None

**/
VOID
FiaSataDisable(UINT8 ControllerIndex,
               PCH_POLICY_PPI* PchPolicy)
{
  EFI_STATUS Status = EFI_SUCCESS;
  UINT8 Start = 0;
  UINT8 Index = 0;
  UINT8 DisableSata = TRUE;
  ME_FIA_MUX_LANE_CONFIG LaneQuery;

  DEBUG ((EFI_D_INFO, "------------------ PCH SATA Config %d Policy Override ------------------\n",ControllerIndex));
  //Calculate offset in FIA mux structure of the selected controller
  if(ControllerIndex == POLICY_SATA0_INDEX)
  {
    Start = ME_FIA_MUX_LANE_SATA0_BEGING;
  }else
  if(ControllerIndex == POLICY_SATA1_INDEX)
  {
    Start = ME_FIA_MUX_LANE_SATA1_BEGING;
  }
  else
  {
    DEBUG((EFI_D_INFO,"WARNING: Wrong SATA controller tried to be disable: 0x%x\n",ControllerIndex));
    ASSERT(FALSE);
    return;
  }

  //If controller is disable by setup menu then skip overwrite
  if(PchPolicy->SataConfig[ControllerIndex].Enable == FALSE)
  {
      DEBUG ((EFI_D_INFO,"\t Controller 0x%x disabled by setup menu: \n",ControllerIndex));
      return;
  }

  Status = FiaMuxConfigLanesQuerySingleMatch(Start, (Start + PCH_MAX_SATA_PORTS - 1), ME_FIA_MUX_LANE_SATA);
  if (Status == EFI_SUCCESS)
  {
    //If one lane is used don't disable the controller
    DisableSata = FALSE;
  }

  //Disable if not FIA lane is used for this controller
  if (DisableSata)
  {
    DEBUG((EFI_D_INFO,"Sata controller %x is not on FIA config\n",ControllerIndex));
    PchPolicy->SataConfig[ControllerIndex].Enable = FALSE;
  }
  else
  {
    DEBUG ((EFI_D_INFO,"FIA: SATA controller %d is enabled\n",ControllerIndex));
    //Controller is enabled. Check what ports are being used
    for(Index = 0; Index < PCH_MAX_SATA_PORTS; Index++)
    {

      //If port is disabled by setup menu don't overwrite it
      if(PchPolicy->SataConfig[ControllerIndex].PortSettings[Index].Enable == FALSE)
      {
        DEBUG ((EFI_D_INFO,"\t Port 0x%x disabled by setup menu\n",Index));
        continue;
      }

      Status = FiaMuxConfigLaneQuery((Index+Start), &LaneQuery);
      ASSERT_EFI_ERROR(Status);
      if(Status == EFI_SUCCESS)
      {
        if(LaneQuery != ME_FIA_MUX_LANE_SATA)
        {
          PchPolicy->SataConfig[ControllerIndex].PortSettings[Index].Enable = FALSE;
          DEBUG ((EFI_D_INFO,"\t Disabling port: 0x%x\n",Index));
        }
      }
      else
      {
        DEBUG ((EFI_D_INFO,"ERROR: Wrong SATA lane query: 0x%x\n",(Index+Start)));
      }
    }

  }
}

/**
  This function performs XHCI disabling according to FIA config

  @param[in] ControllerIndex    PCH policy controller index
  @param[in] PchPolicy          PCH policy

  @retval None

**/
VOID
FiaXhciDisable(
  PCH_POLICY_PPI* PchPolicy
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  UINT8 Start = FIA_LANE16;
  UINT8 Index;
  ME_FIA_MUX_LANE_CONFIG LaneQuery;

  DEBUG ((EFI_D_INFO, "------------------ PCH USB Config Policy Override ------------------\n"));
  //Skip overwrite if all XCHI (USB3) ports are disabled by setup menu.
  for (Index = 0; Index < GetPchXhciMaxUsb3PortNum(); Index++) {
    if(PchPolicy->UsbConfig.PortUsb30[Index].Enable == TRUE) {
      break;
    } else if (Index == (GetPchXhciMaxUsb3PortNum() - 1) ) {
      DEBUG ((EFI_D_INFO,"\tAll XHCI ports were disabled by setup menu\n"));
      return;
    }
  }

  //Disable XHCI (USB3) controller if none FIA lanes are assigned to it.
  Status = FiaMuxConfigLanesQuerySingleMatch(Start, (Start + GetPchXhciMaxUsb3PortNum() - 1), ME_FIA_MUX_LANE_XHCI);
  if (Status != EFI_SUCCESS) {
    DEBUG((EFI_D_INFO,"\tNone FIA lanes are assigned to XHCI controller\n"));
    for (Index = 0; Index < GetPchXhciMaxUsb3PortNum(); Index++) {
      PchPolicy->UsbConfig.PortUsb30[Index].Enable = FALSE;
    }
  } else {
    DEBUG ((EFI_D_INFO,"FIA: XHCI controller is enabled\n"));
    //Controller is enabled. Check what ports are being used
    for(Index = 0; Index < GetPchXhciMaxUsb3PortNum(); Index++) {
      //If a port is disabled by setup menu don't overwrite it
      if(PchPolicy->UsbConfig.PortUsb30[Index].Enable == FALSE) {
        DEBUG ((EFI_D_INFO,"\tXCHI Port 0x%x is disabled by setup menu\n",Index));
        continue;
      }
      Status = FiaMuxConfigLaneQuery((Start + Index), &LaneQuery);
      ASSERT_EFI_ERROR(Status);
      if(Status == EFI_SUCCESS) {
        if(LaneQuery != ME_FIA_MUX_LANE_XHCI) {
          PchPolicy->UsbConfig.PortUsb30[Index].Enable = FALSE;
          // OC pin unmapping of the XHCI (USB3) Ports
          PchPolicy->UsbConfig.PortUsb30[Index].OverCurrentPin = PchUsbOverCurrentPinSkip;
          DEBUG ((EFI_D_INFO,"\tXCHI Port 0x%x is being disabled\n",Index));
        } else {
          DEBUG ((EFI_D_INFO,"\tXCHI Port 0x%x is enabled\n",Index));
        }
      } else {
        DEBUG ((EFI_D_INFO,"\tERROR: Wrong XHCI lane query: 0x%x\n",(Start + Index)));
      }
    }
  }
}

/**
  This function performs PCH PEI Policy initialization.

  @param[in, out] PchPolicy       The PCH Policy PPI instance

  @retval EFI_SUCCESS             The PPI is installed and initialized.
  @retval EFI ERRORS              The PPI is not successfully installed.
  @retval EFI_OUT_OF_RESOURCES    Do not have enough resources to initialize the driver
**/
EFI_STATUS
EFIAPI
UpdatePeiPchPolicy (
  IN OUT      PCH_POLICY_PPI     *PchPolicy
  )
{
  EFI_STATUS                      Status;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI *VariableServices;
  UINTN                           VariableSize;
  PCH_SETUP                       PchSetup;
  SYSTEM_CONFIGURATION            SetupVariables;
  UINT8                           Index;
  PLATFORM_INFO_HOB               *PlatformInfo;
  UINTN                           LpcBaseAddress;
  UINT8                           *SmBusReservedTable;
  UINT8                           SmBusReservedNum;
  UINT16                          BoardId;
#ifndef FSP_FLAG //SERVER_FSP
  // Denverton Aptiov override start - EIP#235962
  // Remove unreferenced code
  /*
  PCH_SPI_PPI                     *SpiPpi;
  UINT32                          BaseAddr;
  UINT32                          RegionSize;
  */
  // Denverton Aptiov override End - EIP#235962
#endif //!FSP_FLAG  SERVER_FSP
  BOOLEAN                         FlashProtectionEnabled;

  LpcBaseAddress = MmPciBase (
                            DEFAULT_PCI_BUS_NUMBER_PCH,
                     PCI_DEVICE_NUMBER_PCH_LPC,
                     PCI_FUNCTION_NUMBER_PCH_LPC
                     );

  PlatformInfo=NULL;
  Status = GetPlatformInfo (&PlatformInfo);

  ASSERT_EFI_ERROR (Status);
  ASSERT (PlatformInfo != NULL);
  if (PlatformInfo == NULL) {
    DEBUG ((EFI_D_ERROR, "\nERROR: UpdatePeiPchPolicy () - Attempting to use a NULL pointer.\n"));
    return EFI_NOT_READY;
  }
  BoardId = PlatformInfo->BoardId;

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

  VariableSize = sizeof (SYSTEM_CONFIGURATION);
  // Denverton Aptiov override Start - EIP#217439
  Status = VariableServices->GetVariable (
                              VariableServices,
                              L"IntelSetup",
                               &gSetupVariableGuid,
                              NULL,
                              &VariableSize,
                              &SetupVariables
                              );
  // Denverton Aptiov override End - EIP#217439
  ASSERT_EFI_ERROR (Status);


  VariableSize = sizeof (PCH_SETUP);
  Status = VariableServices->GetVariable (
                               VariableServices,
                               L"PchSetup",
                               &gPchSetupVariableGuid,
                               NULL,
                               &VariableSize,
                               &PchSetup
                               );
  ASSERT_EFI_ERROR (Status);


  PchPolicy->SmbusConfig.Enable         = TRUE;
  PchPolicy->SmbusConfig.DynamicPowerGating    = FALSE;
  PchPolicy->SmbusConfig.SMBusIOSFClockGating  = SetupVariables.SMBusIOSFClockGating;
  PchPolicy->SmbusConfig.SMBusHostSpeedConfig  = SetupVariables.SmbusSpeed;
  // Denverton AptioV Override Start - EIP#235760
  /*
  PchPolicy->PchConfig.DebugLevel		= SetupVariables.DebugPrintErrorLevel
                       | (SetupVariables.DebugPrintInit ? DEBUG_INIT : 0)
                       | (SetupVariables.DebugPrintWarn ? DEBUG_WARN : 0)
                       | (SetupVariables.DebugPrintLoad ? DEBUG_LOAD : 0)
                       | (SetupVariables.DebugPrintFS ? DEBUG_FS : 0)
                       | (SetupVariables.DebugPrintPool ? DEBUG_POOL : 0)
                       | (SetupVariables.DebugPrintPage ? DEBUG_PAGE : 0)
                       | (SetupVariables.DebugPrintInfo ? DEBUG_INFO : 0)
                       | (SetupVariables.DebugPrintDispatch ? DEBUG_DISPATCH : 0)
                       | (SetupVariables.DebugPrintVariable ? DEBUG_VARIABLE : 0)
                       | (SetupVariables.DebugPrintBM ? DEBUG_BM : 0)
                       | (SetupVariables.DebugPrintBlkIO ? DEBUG_BLKIO : 0)
                       | (SetupVariables.DebugPrintNet ? DEBUG_NET : 0)
                       | (SetupVariables.DebugPrintUNDI ? DEBUG_UNDI : 0)
                       | (SetupVariables.DebugPrintLoadfile ? DEBUG_LOADFILE : 0)
                       | (SetupVariables.DebugPrintEvent ? DEBUG_EVENT : 0)
                       | (SetupVariables.DebugGCD ? DEBUG_GCD : 0)
                       | (SetupVariables.DebugCache ? DEBUG_CACHE : 0)
                       | (SetupVariables.DebugPrintVerbose ? DEBUG_VERBOSE : 0)
                       | (SetupVariables.DebugPrintError ? DEBUG_ERROR : 0);
    SetDebugPrintErrorLevel( PchPolicy->PchConfig.DebugLevel );
    DEBUG ((DEBUG_INFO, "policy %X\n", PchPolicy->PchConfig.DebugLevel ));
    */
    // Denverton AptioV Override Start - EIP#235760

  //
  //Sata update
  //
  for (Index = 0; Index < GetPchMaxSataControllerNum(); Index++) {
    UpdatePeiPchPlatformPolicySata(Index, &PchPolicy->SataConfig[Index],
                         &PchSetup);
    //Disable controller if is not mapped in FIA mux
    FiaSataDisable(Index, PchPolicy);
  }
  //
  // SMBUS reserved addresses
  //
  SmBusReservedTable = NULL;
  SmBusReservedNum   = 0;
  PchPolicy->SmbusConfig.SmbusIoBase = SMBUS_BASE_ADDRESS; // Denverton AptioV TODO
  SmBusReservedTable = mSmbusSTPRsvdAddresses;
  SmBusReservedNum   = sizeof (mSmbusSTPRsvdAddresses);

  if (SmBusReservedTable != NULL) {
    PchPolicy->SmbusConfig.NumRsvdSmbusAddresses = SmBusReservedNum;
    CopyMem (
      PchPolicy->SmbusConfig.RsvdSmbusAddressTable,
      SmBusReservedTable,
      SmBusReservedNum
      );
  }



  PchPolicy->PcieConfig[0].ComplianceTestMode            = PchSetup.PcieComplianceTestMode;
  PchPolicy->PcieConfig[1].ComplianceTestMode            = PchSetup.PcieComplianceTestMode;

  if (SetupVariables.RelaxSecConf) {
    PchPolicy->LockDownConfig.BiosLock = FALSE;
    PchPolicy->LockDownConfig.SpiEiss  = FALSE;
    PchPolicy->PchHideP2sbEnable = FALSE;
    PchPolicy->P2sbConfig.SbiUnlock = TRUE;
    FlashProtectionEnabled = FALSE;
  } else {
    //
    //Set in true as there is not CpuSetup.BiosGuard policy
    //
    PchPolicy->LockDownConfig.BiosLock = TRUE;
    PchPolicy->LockDownConfig.SpiEiss  = TRUE;
    PchPolicy->PchHideP2sbEnable = TRUE;
    PchPolicy->P2sbConfig.SbiUnlock = FALSE;
    FlashProtectionEnabled = TRUE;
  }

  //
  // PMC
  //
  PchPolicy->PmConfig.StateAfterG3        = PchSetup.StateAfterG3;
  PchPolicy->PmConfig.ActiveCoreCount     = SetupVariables.ActiveCoreCount;


  //
  //HSUART
  //
  // Denverton AptioV override start - EIP#288063
  // Use the settings based on SDL tokens instead of setup option from RC
  for(Index = 0; Index < PCH_MAX_HSUART; Index++)
  {
    PchPolicy->HsUartConfig[Index].HsUartMode = HSUART_MODE; 
    PchPolicy->HsUartConfig[Index].HsUartCOM = PchSetup.HsUartCOM[Index];
  }
#if VirtualSerial_SUPPORT
  PchPolicy->HsUartConfig[0].HsUartCOM = VIRTUAL_SERIAL_DEVICE1_BASE_ADDRESS;
  PchPolicy->HsUartConfig[1].HsUartCOM = VIRTUAL_SERIAL_DEVICE2_BASE_ADDRESS;
  PchPolicy->HsUartConfig[2].HsUartCOM = VIRTUAL_SERIAL_DEVICE3_BASE_ADDRESS;
#endif
  // Denverton AptioV override end - EIP#288063
// Denverton Aptiov override start
// DBG2 ACPI table is installed by "Debug Port Tables" module  
//  PchPolicy->HsUartKDPort = PchSetup.HsUartKDPort;
// Denverton Aptiov override End
  PchPolicy->GpioStatus = PchSetup.GpioStatus;
  //
  // IOAPIC Config
  //
  PchPolicy->IoApicConfig.IoApicEntry24_119 = PchSetup.PchIoApic24119Entries;
  PchPolicy->IoApicConfig.BdfValid          = 1;
  PchPolicy->IoApicConfig.BusNumber         = 0xF0;
  PchPolicy->IoApicConfig.DeviceNumber      = 0x1F;
  PchPolicy->IoApicConfig.FunctionNumber    = 0;

  //
  // Misc PM Config
  //
    PchPolicy->PmConfig.PwrBtnOverridePeriod              = 0;   // 4sec

  //
  // DefaultSvidSid Config
  //
  PchPolicy->PchConfig.SubSystemVendorId = V_PCH_INTEL_VENDOR_ID;
  PchPolicy->PchConfig.SubSystemId       = V_PCH_DEFAULT_SID;

  //
  // Initialize Serial IRQ Config
  //
  PchPolicy->SerialIrqConfig.SirqEnable       = TRUE;
  PchPolicy->SerialIrqConfig.StartFramePulse  = PchSfpw4Clk;
  if (PchSetup.PchSirqMode == 0) {
    PchPolicy->SerialIrqConfig.SirqMode = PchQuietMode;
  } else {
    PchPolicy->SerialIrqConfig.SirqMode = PchContinuousMode;
  }
  //
  // Update Pch Usb Config
  //
  UpdatePchUsbConfig (
    &PchPolicy->UsbConfig,
    &PchSetup,
    PlatformInfo->PlatformType,
    PlatformInfo->PlatformFlavor,
    PlatformInfo->BoardType,
    PlatformInfo->BoardId
    );

  //Override Pch Usb Config according FIA mux
  FiaXhciDisable(PchPolicy);




  //
  // Update TraceHub config based on setup options
  //
  PchSetup.TraceHubEnable                             = SetupVariables.TraceHubEnable;
  PchSetup.TraceHubFwDestination                      = 1;
  PchPolicy->PchTraceHubConfig.TraceHubEnable         = PchSetup.TraceHubEnable;
  PchPolicy->PchTraceHubConfig.TraceHubFwEnable       = PchSetup.TraceHubFwEnable;
  PchPolicy->PchTraceHubConfig.TraceHubFwDestination  = PchSetup.TraceHubFwDestination;
  PchPolicy->PchTraceHubConfig.TraceHubMemBaseRegion0 = PchSetup.TraceHubMemBaseRegion0;
  PchPolicy->PchTraceHubConfig.TraceHubMemBaseRegion1 = PchSetup.TraceHubMemBaseRegion1;
  PchPolicy->PchTraceHubConfig.TraceHubPtiMode        = PchSetup.TraceHubPtiMode;
  PchPolicy->PchTraceHubConfig.TraceHubPtiTraining    = PchSetup.TraceHubPtiTraining;
  PchPolicy->PchTraceHubConfig.TraceHubPtiSpeed       = PchSetup.TraceHubPtiSpeed;
  PchPolicy->PchTraceHubConfig.TraceHubMemoryEnabled  = PchSetup.TraceHubMemoryEnabled;
  PchPolicy->PchTraceHubConfig.EnableMode = PchSetup.TraceHubEnableMode;

  switch (PchSetup.MemRegion0BufferSize) {
    case 0:
       PchPolicy->PchTraceHubConfig.MemReg0Size = 0; // No memory
       break;
    case 1:
       PchPolicy->PchTraceHubConfig.MemReg0Size = 0x100000;  // 1MB
       break;
    case 2:
       PchPolicy->PchTraceHubConfig.MemReg0Size = 0x800000;  // 8MB
       break;
    case 3:
       PchPolicy->PchTraceHubConfig.MemReg0Size = 0x4000000; // 64MB
       break;
    case 4:
       PchPolicy->PchTraceHubConfig.MemReg0Size = 0x8000000; // 128MB
       break;
    case 5:
       PchPolicy->PchTraceHubConfig.MemReg0Size = 0x10000000; // 256MB
       break;
    case 6:
       PchPolicy->PchTraceHubConfig.MemReg0Size = 0x20000000; // 512MB
       break;
    case 7:
       PchPolicy->PchTraceHubConfig.MemReg0Size = 0x40000000; // 1GB
       break;
  }

  switch (PchSetup.MemRegion1BufferSize) {
    case 0:
       PchPolicy->PchTraceHubConfig.MemReg1Size = 0; // No memory
       break;
    case 1:
       PchPolicy->PchTraceHubConfig.MemReg1Size = 0x100000;  // 1MB
       break;
    case 2:
       PchPolicy->PchTraceHubConfig.MemReg1Size = 0x800000;  // 8MB
       break;
    case 3:
       PchPolicy->PchTraceHubConfig.MemReg1Size = 0x4000000; // 64MB
       break;
    case 4:
       PchPolicy->PchTraceHubConfig.MemReg1Size = 0x8000000; // 128MB
       break;
    case 5:
       PchPolicy->PchTraceHubConfig.MemReg1Size = 0x10000000; // 256MB
       break;
    case 6:
       PchPolicy->PchTraceHubConfig.MemReg1Size = 0x20000000; // 512MB
       break;
    case 7:
       PchPolicy->PchTraceHubConfig.MemReg1Size = 0x40000000; // 1GB
       break;
  }



  //
  // Enabling Flash Protection Range Registers
  // Enable FPRR policy and set up ranges on non-Capsule Update flow with Flash Wear-Out Protection enabled
  // This MSR is only available on SKL ULX/ULT CPU stepping C0 and greater and Skylake DT/Halo steppings Q0 and greater
  // Note: CPU steppings are not enumerated alphabetically
  // PchPrintFlashProtectionConfig() dumps FPRR information during PchPrintPolicyPpi()
  //
  if (FlashProtectionEnabled) {
    //
    // Flash Protection Range Register initialization
    //
    for (Index = 0; Index < PCH_FLASH_PROTECTED_RANGES; Index++) {
      PchPolicy->FlashProtectConfig.ProtectRange[Index].WriteProtectionEnable  = TRUE;
      PchPolicy->FlashProtectConfig.ProtectRange[Index].ReadProtectionEnable   = FALSE;
    }

#ifndef FSP_FLAG //SERVER_FSP
    // Denverton Aptiov override start - EIP#235962
    // Remove unreferenced code
    /*
    Status = PeiServicesLocatePpi (
               &gPchSpiPpiGuid,
               0,
               NULL,
               (VOID **)&SpiPpi
               );
    if (EFI_ERROR (Status)) {
      return EFI_DEVICE_ERROR;
    }
    //
    // Find the base address for flash
    //
    Status = SpiPpi->GetRegionAddress (SpiPpi, FlashRegionBios, &BaseAddr, &RegionSize);
    ASSERT_EFI_ERROR (Status);
    */
    // Denverton Aptiov override End - EIP#235962
#endif //!FSP_FLAG SERVER_FSP

    //
    // Assign FPRR ranges
    //
#ifndef FSP_FLAG
    // Denverton AptioV TODO Start
    // Need to verify DMI edit and Recovery functionalities once board is available.
    // Porting for AMI`s ROM layout and protecting MAIN and BB
    /*
    PchPolicy->FlashProtectConfig.ProtectRange[0].ProtectedRangeBase    = (UINT16)((BaseAddr + FixedPcdGet32 (PcdFlashFvMainOffset)) >> 12);
    PchPolicy->FlashProtectConfig.ProtectRange[0].ProtectedRangeLimit   = (UINT16)((BaseAddr +
                                                                          FixedPcdGet32 (PcdFlashFvMainOffset) +
                                                                          FixedPcdGet32 (PcdFlashFvMainSize)+
                                                                          FixedPcdGet32 (PcdFlashFvLegacySize)+
                                                                          FixedPcdGet32 (PcdFlashFvLegacyFreeSize)- 1) >> 12);

    PchPolicy->FlashProtectConfig.ProtectRange[0].WriteProtectionEnable  = TRUE;
    PchPolicy->FlashProtectConfig.ProtectRange[0].ReadProtectionEnable   = FALSE;

    PchPolicy->FlashProtectConfig.ProtectRange[1].ProtectedRangeBase    = (UINT16)((BaseAddr + FixedPcdGet32 (PcdFlashFvOemLogoOffset)) >> 12);
    PchPolicy->FlashProtectConfig.ProtectRange[1].ProtectedRangeLimit   = (UINT16)((BaseAddr + FixedPcdGet32 (PcdFlashFvOemLogoOffset) +
                                                                          FixedPcdGet32 (PcdFlashFvOemLogoSize)+
                                                                          FixedPcdGet32 (PcdFlashNvStorageVariableSize)- 1) >> 12);
    PchPolicy->FlashProtectConfig.ProtectRange[1].WriteProtectionEnable  = TRUE;
    PchPolicy->FlashProtectConfig.ProtectRange[1].ReadProtectionEnable   = FALSE;


    PchPolicy->FlashProtectConfig.ProtectRange[2].ProtectedRangeBase    = (UINT16)((BaseAddr + FixedPcdGet32 (PcdFlashFvMrcNormalOffset)) >> 12);
    PchPolicy->FlashProtectConfig.ProtectRange[2].ProtectedRangeLimit   = (UINT16)((BaseAddr + FixedPcdGet32 (PcdFlashFvMrcNormalOffset) +
                                                                          FixedPcdGet32 (PcdFlashFvMrcNormalSize)+
                                                                          FixedPcdGet32 (PcdFlashFvSecPeiSize)- 1) >> 12);
    PchPolicy->FlashProtectConfig.ProtectRange[2].WriteProtectionEnable  = TRUE;
    PchPolicy->FlashProtectConfig.ProtectRange[2].ReadProtectionEnable   = FALSE;
    */
    PchPolicy->FlashProtectConfig.ProtectRange[0].ProtectedRangeBase    = (UINT16)(FV_MAIN_BASE >> 12);
    PchPolicy->FlashProtectConfig.ProtectRange[0].ProtectedRangeLimit   = (UINT16)((FV_MAIN_BASE + FV_MAIN_SIZE -1) >> 12);
    PchPolicy->FlashProtectConfig.ProtectRange[0].WriteProtectionEnable  = TRUE;
    PchPolicy->FlashProtectConfig.ProtectRange[0].ReadProtectionEnable   = FALSE;

    PchPolicy->FlashProtectConfig.ProtectRange[1].ProtectedRangeBase    = (UINT16)(FV_BB_BASE >> 12);
    PchPolicy->FlashProtectConfig.ProtectRange[1].ProtectedRangeLimit   = (UINT16)(((UINT64)FV_BB_BASE + (UINT64)FV_BB_SIZE -1) >> 12);
    PchPolicy->FlashProtectConfig.ProtectRange[1].WriteProtectionEnable  = TRUE;
    PchPolicy->FlashProtectConfig.ProtectRange[1].ReadProtectionEnable   = FALSE;
    
    PchPolicy->FlashProtectConfig.ProtectRange[2].ProtectedRangeBase    = 0;
    PchPolicy->FlashProtectConfig.ProtectRange[2].ProtectedRangeLimit   = 0;
    PchPolicy->FlashProtectConfig.ProtectRange[2].WriteProtectionEnable  = FALSE;
    PchPolicy->FlashProtectConfig.ProtectRange[2].ReadProtectionEnable   = FALSE;
    // Denverton AptioV TODO End

    //
    //Unused ranges
    //
    PchPolicy->FlashProtectConfig.ProtectRange[3].ProtectedRangeBase    = 0;
    PchPolicy->FlashProtectConfig.ProtectRange[3].ProtectedRangeLimit   = 0;
    PchPolicy->FlashProtectConfig.ProtectRange[3].WriteProtectionEnable  = TRUE;
    PchPolicy->FlashProtectConfig.ProtectRange[3].ReadProtectionEnable   = FALSE;

    PchPolicy->FlashProtectConfig.ProtectRange[4].ProtectedRangeBase    = 0;
    PchPolicy->FlashProtectConfig.ProtectRange[4].ProtectedRangeLimit   = 0;
    PchPolicy->FlashProtectConfig.ProtectRange[4].WriteProtectionEnable  = TRUE;
    PchPolicy->FlashProtectConfig.ProtectRange[4].ReadProtectionEnable   = FALSE;
#endif //!FSP_FLAG

  }

  //
  // HPET
  //
  PchPolicy->HpetConfig.Enable = PchSetup.Hpet;
  // Denverton AptioV Override Start - EIP#235760
  //DEBUG ((DEBUG_INFO, "call PTSS table %X\n", PchPolicy->PchConfig.DebugLevel ));
  // Denverton AptioV Override Start - EIP#235760
  InstallPlatformHsioPtssTable (PlatformInfo->BoardId, &PchSetup, PchPolicy);

  return Status;
}

