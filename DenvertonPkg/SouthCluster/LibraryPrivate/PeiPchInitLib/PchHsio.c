/** @file
  PCH HSIO Initialization file

@copyright
 Copyright (c) 2014 - 2017 Intel Corporation. All rights reserved
 This software and associated documentation (if any) is furnished
 under a license and may only be used or copied in accordance
 with the terms of the license. Except as permitted by the
 license, no part of this software or documentation may be
 reproduced, stored in a retrieval system, or transmitted in any
 form or by any means without the express written consent of
 Intel Corporation.
 This file contains an 'Intel Peripheral Driver' and is uniquely
 identified as "Intel Reference Module" and is licensed for Intel
 CPUs and chipsets under the terms of your license agreement with
 Intel or your vendor. This file may be modified by the user, subject
 to additional terms of the license agreement.

@par Specification Reference:
**/

#include <Library/HeciMsgLib.h>
#include <Library/PostCodeLib.h>
#include <PcieIpPlatformLib.h>
#include <BupMsgs.h>
#include "PchInitPei.h"
#include "PchHsioLaneLib.h"
#include <Library/PeiServicesTablePointerLib.h>
#include <IncludePrivate/PchHsio.h>
#include <IncludePrivate/PchChipsetInfoHob.h>
#include <Library/ConfigBlockLib.h>
#include <Ppi/SaPolicy.h>
#include <Library/PchHsioLib.h>

#define PCH_HSIO_BEFORE_RECIPE		0xC2
#define PCH_HSIO_AFTER_RECIPE		0xC3
#define PCH_HSIO_BEFORE_SATA		0xC5
#define PCH_HSIO_AFTER_SATA			0xC6

#define MAX_SATA_CTRL           2


/**
  The function is used to detemine if a ChipsetInitSync with ME is required and syncs with ME if required.

  @param[in] MeSync               Sync ME Settings

  @retval EFI_SUCCESS             BIOS and ME ChipsetInit settings are in sync
  @retval EFI_UNSUPPORTED         BIOS and ME ChipsetInit settings are not in sync
**/
EFI_STATUS
PchHsioChipsetInitProg (
  IN  BOOLEAN                      MeSync
  )
{
  EFI_STATUS                  Status;
#ifdef ME_SUPPORT_FLAG //(SERVER_BIOS_FLAG)
  UINT16                      BiosChipInitCrc;
  PEI_ME_PLATFORM_POLICY_PPI  *PeiMePolicyPpi;
  CHIPSET_INIT_INFO_HOB       *ChipsetInitHob;
#endif // ME_SUPPORT_FLAG (SERVER_BIOS_FLAG)
  EFI_BOOT_MODE               BootMode;
  UINT32                      PchChipsetInitTableLength;
  UINT8                       *PchChipsetInitTable;
#ifdef ME_SUPPORT_FLAG //(SERVER_BIOS_FLAG)
#endif //ME_SUPPORT_FLAG
  ///
  /// Step 1
  /// GetBoodMode, do not perform ChipsetInit Sync check on S3 RESUME
  ///
  Status = PeiServicesGetBootMode (&BootMode);
  if(BootMode == BOOT_ON_S3_RESUME) {
    return EFI_SUCCESS;
  }

  switch (PchStepping ()) {
    case PchHA0:
    case PchHA1:
      PchChipsetInitTable = PchHChipsetInitTable_A0;
      PchChipsetInitTableLength   = sizeof (PchHChipsetInitTable_A0);
	  break;
    case PchHB0:
    case PchHB1:
  	  PchChipsetInitTable = PchHChipsetInitTable_B0;
  	  PchChipsetInitTableLength   = sizeof (PchHChipsetInitTable_B0);
  	  break;
    default:
      DEBUG ((DEBUG_INFO, "Unkown PCH Stepping %x. Setting default Table Bx\n", PchStepping () ));
      PchChipsetInitTable = PchHChipsetInitTable_B0;
      PchChipsetInitTableLength   = sizeof (PchHChipsetInitTable_B0);
  }

  ///
  /// Step 3
  /// Send the HECI HSIO Message
  ///
#ifdef ME_SUPPORT_FLAG //(SERVER_BIOS_FLAG)
  ///
  /// Get Policy settings through the MePolicy PPI
  ///
  Status = PeiServicesLocatePpi (
                            &gPeiMePlatformPolicyPpiGuid,
                            0,
                            NULL,
                            (VOID **) &PeiMePolicyPpi
                            );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "MePolicyPpi not located! Error: %r\n", Status));
  } else if (PeiMePolicyPpi->HsioEnableMessaging == 0) {
    DEBUG ((DEBUG_INFO, "PeiMePolicyPpi->HsioEnableMessaging = 0, do not perform HSIO sync. \n"));
    return EFI_SUCCESS;
  }

  Status         = EFI_SUCCESS;
  ChipsetInitHob = GetFirstGuidHob (&gPchChipsetInitHobGuid);
  if (ChipsetInitHob == NULL) {

    UINT16  MeChipInitCrc = 0;

    DEBUG ((DEBUG_INFO, "(Hsio) ChipsetInitHob not found\n"));
    Status = PeiHeciHsioMsg (HSIO_REPORT_VERSION_REQ, &MeChipInitCrc);
    if (!EFI_ERROR(Status)) {
      DEBUG ((DEBUG_INFO, "(Hsio) Creating HOB to adjust Hsio settings in PchInit, if required\n"));
    Status = PeiServicesCreateHob (
                EFI_HOB_TYPE_GUID_EXTENSION,
                sizeof (CHIPSET_INIT_INFO_HOB),
                (VOID**) &ChipsetInitHob
                );
    if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "(Hsio) ChipsetInitHob could not be created\n"));
        ASSERT_EFI_ERROR (Status);
      return EFI_ABORTED;
    }
    //
    // Initialize ChipsetInitHob
    //
      ChipsetInitHob->MeChipInitCrc = MeChipInitCrc;
    ChipsetInitHob->Header.Name              = gPchChipsetInitHobGuid;
    }
  }

  if (Status == EFI_SUCCESS) {
    DEBUG ((DEBUG_INFO, "(Hsio) ME Reported CRC=0x%04X\n", ChipsetInitHob->MeChipInitCrc));
    //
    // Get ChipsetInit table indentifier from the one found in the code
    //
    if(PchChipsetInitTable != NULL) {
      BiosChipInitCrc = *((UINT16*)PchChipsetInitTable);
      DEBUG ((DEBUG_INFO, "(Hsio) BIOS Hsio CRC=0x%04X\n", BiosChipInitCrc));
      if (ChipsetInitHob->MeChipInitCrc != BiosChipInitCrc) {
        if(MeSync == TRUE) {
          Status = PeiHeciChipsetInitSyncMsg (PchChipsetInitTable, PchChipsetInitTableLength);
          if (EFI_ERROR (Status)) {
            DEBUG ((DEBUG_ERROR, "ChipsetInit Sync Error: %r\n", Status));
            if (Status == EFI_UNSUPPORTED || Status == EFI_DEVICE_ERROR) {
              DEBUG ((DEBUG_ERROR, "Current Me Bios boot path doesn't support Chipset Init Sync message, continue to boot.\n", Status));
            } else {
              ASSERT_EFI_ERROR (Status);
              return EFI_UNSUPPORTED;
            }
          }
        } else {
          return EFI_UNSUPPORTED;
        }
      }
    }
  } else {
    DEBUG ((DEBUG_INFO, "(Hsio) Syncing ChipsetInit with ME failed! Error: %r\n", Status));
  }
#endif // ME_SUPPORT_FLAG (SERVER_BIOS_FLAG)
  return EFI_SUCCESS;
}

// DNV_TODO: check PchLosPhyModeCheck()

/**
  The function indicates if at least one of the data lanes attached
  to a specific common lane is of a specific phymode.

  @param[in] Pid               Common Lane End Point ID
  @param[in] PhyMode           Phymode that needs to be checked
  @param[in] PchSeries         Indicates the Pch Series
  @param[in] Los1              Lane Owner Status 1 Value
  @param[in] Los2              Lane Owner Status 2 Value
  @param[in] Los3              Lane Owner Status 3 Value
  @param[in] Los4              Lane Owner Status 4 Value

  @retval EFI_SUCCESS          Phymode exists.
          EFI_UNSUPPORTED      Phymode does not exist.
**/
EFI_STATUS
PchLosPhyModeCheck (
  IN  PCH_SBI_PID                       Pid,
  IN  UINT8                             PhyMode,
  IN PCH_SERIES                         PchSeries,
  IN UINT32                             Los1,
  IN UINT32                             Los2,
  IN UINT32                             Los3,
  IN UINT32                             Los4
  )
{
  UINT8                       Shift;

  switch (Pid) {
    case PID_MODPHY1:
      if (PchSeries == PchLp) {
        for(Shift = PCH_MODPHY1_LP_LOS1_LANE_START; Shift <= PCH_MODPHY1_LP_LOS1_LANE_END; Shift++) {
          if(((Los1 >> (Shift * 4)) & 0xF) == PhyMode) {
            return EFI_SUCCESS;
          }
        }
        for(Shift = PCH_MODPHY1_LP_LOS2_LANE_START; Shift <= PCH_MODPHY1_LP_LOS2_LANE_END; Shift++) {
          if(((Los2 >> (Shift * 4)) & 0xF) == PhyMode) {
            return EFI_SUCCESS;
          }
        }
      } else if (PchSeries == PchH) {
        for(Shift = PCH_MODPHY1_H_LOS1_LANE_START; Shift <= PCH_MODPHY1_H_LOS1_LANE_END; Shift++) {
          if(((Los1 >> (Shift * 4)) & 0xF) == PhyMode) {
            return EFI_SUCCESS;
          }
        }
        for(Shift = PCH_MODPHY1_H_LOS2_LANE_START; Shift <= PCH_MODPHY1_H_LOS2_LANE_END; Shift++) {
          if(((Los2 >> (Shift * 4)) & 0xF) == PhyMode) {
            return EFI_SUCCESS;
          }
        }
      }
      break;
    case PID_MODPHY2:
      if (PchSeries == PchLp) {
        for(Shift = PCH_MODPHY2_LP_LOS2_LANE_START; Shift <= PCH_MODPHY2_LP_LOS2_LANE_END; Shift++) {
          if(((Los2 >> (Shift * 4)) & 0xF) == PhyMode) {
            return EFI_SUCCESS;
          }
        }
      } else if (PchSeries == PchH) {
        for(Shift = PCH_MODPHY2_H_LOS3_LANE_START; Shift <= PCH_MODPHY2_H_LOS3_LANE_END; Shift++) {
          if(((Los3 >> (Shift * 4)) & 0xF) == PhyMode) {
            return EFI_SUCCESS;
          }
        }
      }
      break;
    default:
      return EFI_SUCCESS;
      break;
  }
  return EFI_UNSUPPORTED;
}

/**
  The function programs HSIO Rx\Tx Eq policy registers.

  @param[in]  PchPolicyPpi        The PCH Policy PPI instance
  @param[in] TempMemBaseAddr      Temporary memory base address for PMC device
  @param[in]  PmcBaseAddress      The PMC Base Address
**/
VOID
PchHsioRxTxEqPolicyProg (
  IN  PCH_POLICY_PPI      *PchPolicyPpi
  )
{
  HSIO_LANE                   HsioLane;
  UINT16                      Offset;
  UINT8                       Index;
  UINT8                       PortId;
  UINT8                       LaneOwner;
  UINT8                       MaxSataPorts;
  UINT8                       MaxPciePorts;
  UINT8                       LaneNum;
  UINT32                      Data32;
  UINT32                      Data32And;
  UINT32                      Data32Or;
  UINT8                       IndexClusters;
  EFI_STATUS                  Status;

  DEBUG ((DEBUG_INFO, "POSTCODE << %x >>\n", PCH_HSIO_BEFORE_SATA ));
  PostCode( PCH_HSIO_BEFORE_SATA );
  MaxSataPorts = GetPchMaxSataPortNum ();
  MaxPciePorts = PcieIPGetPchStandardPciePortNum ();                //using new PCIE library

  for( IndexClusters = 0; IndexClusters < MAX_SATA_CTRL ; IndexClusters++ ) {
    for (Index = 0; Index < MaxSataPorts; Index++) {
      Status = PchGetSataLaneNum (Index + (IndexClusters * MaxSataPorts ), &LaneNum);
      if(Status != EFI_SUCCESS) {
    	DEBUG ((DEBUG_INFO, "LaneNum unsupported\n"));
        continue;
      }
      Status = PchGetLaneInfo (LaneNum, &PortId, &LaneOwner);
      if (LaneOwner == V_PCH_PCR_FIA_LANE_OWN_SATA && Status == EFI_SUCCESS ) {
        HsioGetLane (LaneNum, &HsioLane);

        if ((PchPolicyPpi->HsioSataConfig.PortLane[Index].HsioRxGen1EqBoostMagEnable == TRUE) ||
            (PchPolicyPpi->HsioSataConfig.PortLane[Index].HsioRxGen2EqBoostMagEnable == TRUE)){
          Offset = HsioLane.Base + (UINT16) R_PCH_HSIO_RX_DWORD21;
          Data32Or  = 0;
          Data32And = 0xFFFFFFFF;
          if (PchPolicyPpi->HsioSataConfig.PortLane[Index].HsioRxGen1EqBoostMagEnable == TRUE) {
            Data32Or |= (UINT32) (PchPolicyPpi->HsioSataConfig.PortLane[Index].HsioRxGen1EqBoostMag << N_PCH_HSIO_RX_DWORD21_ICFGCTLEDATATAP_QUATRATE_5_0);
            Data32And &= (UINT32)~B_PCH_HSIO_RX_DWORD21_ICFGCTLEDATATAP_QUATRATE_5_0;
          }

          if (PchPolicyPpi->HsioSataConfig.PortLane[Index].HsioRxGen2EqBoostMagEnable == TRUE) {
            Data32Or |= (UINT32) (PchPolicyPpi->HsioSataConfig.PortLane[Index].HsioRxGen2EqBoostMag << N_PCH_HSIO_RX_DWORD21_ICFGCTLEDATATAP_HALFRATE_5_0);
            Data32And &= (UINT32)~B_PCH_HSIO_RX_DWORD21_ICFGCTLEDATATAP_HALFRATE_5_0;
          }

          PchPcrAndThenOr32 (
            (PCH_SBI_PID) PortId,
            (UINT16) Offset,
            Data32And,
            Data32Or);
          PchPcrRead32 ((PCH_SBI_PID) PortId, Offset, &Data32);
          DEBUG ((DEBUG_INFO, "HSIO : PortID = 0x%02x, Offset = 0x%04x, Value = 0x%08x\n", (PCH_SBI_PID) PortId, (UINT16) Offset, (UINT32) Data32));
        }

        if (PchPolicyPpi->HsioSataConfig.PortLane[Index].HsioRxGen3EqBoostMagEnable == TRUE){
          DEBUG ((DEBUG_INFO, "sata port %x in lane %x \n", Index + (IndexClusters * MaxSataPorts ),LaneNum  ));
          if ( PchStepping() == PchHA0 || PchStepping() == PchHA1 ) {
            Offset = HsioLane.Base + (UINT16) R_PCH_HSIO_RX_DWORD26;
            PchPcrAndThenOr32 (
              (PCH_SBI_PID) PortId,
              (UINT16) Offset,
              (UINT32) ~0,
              (UINT32) B_PCH_HSIO_RX_DWORD26_SATA_EQ_DIS );
            PchPcrRead32 ((PCH_SBI_PID) PortId, Offset, &Data32);
            DEBUG ((DEBUG_INFO, "SATA index %x = HSIO : PortID = 0x%02x, Offset = 0x%04x, SATA_EQ_DISABLE = 0x%08x\n", Index + (IndexClusters * MaxSataPorts ),
              (PCH_SBI_PID) PortId,
              (UINT16) Offset,
               Data32 ));
          }
          Offset = HsioLane.Base + (UINT16) R_PCH_HSIO_RX_DWORD20;
          PchPcrAndThenOr32 (
            (PCH_SBI_PID) PortId,
            (UINT16) Offset,
            (UINT32) ~B_PCH_HSIO_RX_DWORD20_ICFGCTLEDATATAP_FULLRATE_5_0,
            (UINT32) (PchPolicyPpi->SataConfig[IndexClusters].PortSettings[Index].HsioRxEqBoostMagAd << N_PCH_HSIO_RX_DWORD20_ICFGCTLEDATATAP_FULLRATE_5_0));
          PchPcrRead32 ((PCH_SBI_PID) PortId, Offset, &Data32);\
          DEBUG ((DEBUG_INFO, "SATA = HSIO : PortID = 0x%02x, Offset = 0x%04x, HsioRxEqBoostMagAd = 0x%08x\n", (PCH_SBI_PID) PortId, (UINT16) Offset, (UINT32) PchPolicyPpi->SataConfig[IndexClusters].PortSettings[Index].HsioRxEqBoostMagAd));
        }

        if ((PchPolicyPpi->HsioSataConfig.PortLane[Index].HsioTxGen1DownscaleAmpEnable == TRUE) ||
            (PchPolicyPpi->HsioSataConfig.PortLane[Index].HsioTxGen2DownscaleAmpEnable == TRUE) ||
            (PchPolicyPpi->HsioSataConfig.PortLane[Index].HsioTxGen3DownscaleAmpEnable == TRUE)) {

          Data32Or  = 0;
          Data32And = 0xFFFFFFFF;
          if (PchPolicyPpi->HsioSataConfig.PortLane[Index].HsioTxGen1DownscaleAmpEnable == TRUE) {
            Data32Or |= (UINT32) (PchPolicyPpi->HsioSataConfig.PortLane[Index].HsioTxGen1DownscaleAmp << N_PCH_HSIO_TX_DWORD8_ORATE00MARGIN_5_0);
            Data32And &= (UINT32)~B_PCH_HSIO_TX_DWORD8_ORATE00MARGIN_5_0;
          }

          if (PchPolicyPpi->HsioSataConfig.PortLane[Index].HsioTxGen2DownscaleAmpEnable == TRUE) {
            Data32Or |= (UINT32) (PchPolicyPpi->HsioSataConfig.PortLane[Index].HsioTxGen2DownscaleAmp << N_PCH_HSIO_TX_DWORD8_ORATE01MARGIN_5_0);
            Data32And &= (UINT32)~B_PCH_HSIO_TX_DWORD8_ORATE01MARGIN_5_0;
          }

          if (PchPolicyPpi->HsioSataConfig.PortLane[Index].HsioTxGen3DownscaleAmpEnable == TRUE) {
            Data32Or |= (UINT32) (PchPolicyPpi->HsioSataConfig.PortLane[Index].HsioTxGen3DownscaleAmp << N_PCH_HSIO_TX_DWORD8_ORATE10MARGIN_5_0);
            Data32And &= (UINT32)~B_PCH_HSIO_TX_DWORD8_ORATE10MARGIN_5_0;
          }

          Offset = HsioLane.Base + (UINT16) R_PCH_HSIO_TX_DWORD8;
          PchPcrAndThenOr32 (
            (PCH_SBI_PID) PortId,
            (UINT16) Offset,
            Data32And,
            Data32Or);
          PchPcrRead32 ((PCH_SBI_PID) PortId, Offset, &Data32);
          DEBUG ((DEBUG_INFO, "HSIO : PortID = 0x%02x, Offset = 0x%04x, Value = 0x%08x\n", (PCH_SBI_PID) PortId, (UINT16) Offset, (UINT32) Data32));
        }

        if ((PchPolicyPpi->HsioSataConfig.PortLane[Index].HsioTxGen1DeEmphEnable == TRUE) ||
            (PchPolicyPpi->HsioSataConfig.PortLane[Index].HsioTxGen2DeEmphEnable == TRUE) ||
            (PchPolicyPpi->HsioSataConfig.PortLane[Index].HsioTxGen3DeEmphEnable == TRUE)) {
          Offset = HsioLane.Base + (UINT16) R_PCH_HSIO_TX_DWORD6;
          Data32Or  = 0;
          Data32And = 0xFFFFFFFF;
          if (PchPolicyPpi->HsioSataConfig.PortLane[Index].HsioTxGen1DeEmphEnable == TRUE) {
            Data32Or |= (UINT32) (PchPolicyPpi->HsioSataConfig.PortLane[Index].HsioTxGen1DeEmph);
            Data32And &= (UINT32) ~B_PCH_HSIO_TX_DWORD6_OW2TAPGEN1DEEMPH6P0_5_0;
          }

          if (PchPolicyPpi->HsioSataConfig.PortLane[Index].HsioTxGen2DeEmphEnable == TRUE) {
            Data32Or |= (UINT32) (PchPolicyPpi->HsioSataConfig.PortLane[Index].HsioTxGen2DeEmph << N_PCH_HSIO_TX_DWORD6_OW2TAPGEN2DEEMPH6P0_5_0);
            Data32And &= (UINT32) ~B_PCH_HSIO_TX_DWORD6_OW2TAPGEN2DEEMPH6P0_5_0;
          }

          if (PchPolicyPpi->HsioSataConfig.PortLane[Index].HsioTxGen3DeEmphEnable == TRUE) {
            Data32Or |= (UINT32) (PchPolicyPpi->HsioSataConfig.PortLane[Index].HsioTxGen3DeEmph << N_PCH_HSIO_TX_DWORD6_OW2TAPGEN3DEEMPH6P0_5_0);
            Data32And &= (UINT32) ~B_PCH_HSIO_TX_DWORD6_OW2TAPGEN3DEEMPH6P0_5_0;
          }

          PchPcrAndThenOr32 (
            (PCH_SBI_PID) PortId,
            (UINT16) Offset,
            Data32And,
            Data32Or);
          PchPcrRead32 ((PCH_SBI_PID) PortId, Offset, &Data32);
          DEBUG ((DEBUG_INFO, "HSIO : PortID = 0x%02x, Offset = 0x%04x, Value = 0x%08x\n", (PCH_SBI_PID) PortId, (UINT16) Offset, (UINT32) Data32));
        }
      }
    }
  }

  for( IndexClusters = 0; IndexClusters < PCH_MAX_PCIE_CLUSTERS ; IndexClusters++ ) {
	for (Index = 0; Index < MaxPciePorts; Index++) {
	  Status = PchGetPcieLaneNum (Index, &LaneNum);
	  if(Status != EFI_SUCCESS) {
        DEBUG ((DEBUG_INFO, "LaneNum unsupported\n"));
        continue;
      }
	  Status = PchGetLaneInfo (LaneNum, &PortId, &LaneOwner);
	  if (LaneOwner == V_PCH_PCR_FIA_LANE_OWN_PCIEDMI && Status == EFI_SUCCESS) {
	      HsioGetLane (LaneNum, &HsioLane);
	      if (PchPolicyPpi->PcieConfig[ IndexClusters ].RootPort[Index].HsioRxSetCtleEnable == TRUE) {
	        Offset = HsioLane.Base + (UINT16) R_PCH_HSIO_RX_DWORD25;
	        PchPcrAndThenOr32 (
	          (PCH_SBI_PID) PortId,
	          (UINT16) Offset,
	          (UINT32) ~B_PCH_HSIO_RX_DWORD25_CTLE_ADAPT_OFFSET_CFG_4_0,
	          (UINT32) (PchPolicyPpi->PcieConfig[ IndexClusters ].RootPort[Index].HsioRxSetCtle << N_PCH_HSIO_RX_DWORD25_CTLE_ADAPT_OFFSET_CFG_4_0));
	        DEBUG ((DEBUG_INFO, "PCIE = HSIO : PortID = 0x%02x, Offset = 0x%04x, HsioRxSetCtle = 0x%08x\n", (PCH_SBI_PID) PortId, (UINT16) Offset, (UINT32) PchPolicyPpi->PcieConfig[ IndexClusters ].RootPort[Index].HsioRxSetCtle));
	        PchPcrRead32 ((PCH_SBI_PID) PortId, Offset, &Data32);
	      }

	    //TODO jambriz 27/27/2015 This property  HsioTxGen1DownscaleAmpEnable is not used in SPT, pending activate this lines
//	    if ((PchPolicyPpi->HsioPcieConfig.Lane[Index].HsioTxGen1DownscaleAmpEnable == TRUE) || (PchPolicyPpi->HsioPcieConfig.Lane[Index].HsioTxGen1DeEmphEnable == TRUE)) {
//        MmioOr8 (RpBase + R_PCH_PCIE_LCTL, B_PCIE_LCTL_LD);
//      }

	    if ((PchPolicyPpi->HsioPcieConfig[ IndexClusters ].Lane[Index].HsioTxGen1DownscaleAmpEnable == TRUE) ||
		  (PchPolicyPpi->HsioPcieConfig[ IndexClusters ].Lane[Index].HsioTxGen2DownscaleAmpEnable == TRUE) ||
		  (PchPolicyPpi->HsioPcieConfig[ IndexClusters ].Lane[Index].HsioTxGen3DownscaleAmpEnable == TRUE)) {

		  Data32Or  = 0;
		  Data32And = 0xFFFFFFFF;
		  if (PchPolicyPpi->HsioPcieConfig[ IndexClusters ].Lane[Index].HsioTxGen1DownscaleAmpEnable == TRUE) {
		    Data32Or |= (UINT32) (PchPolicyPpi->HsioPcieConfig[ IndexClusters ].Lane[Index].HsioTxGen1DownscaleAmp << N_PCH_HSIO_TX_DWORD8_ORATE00MARGIN_5_0);
		    Data32And &= (UINT32)~B_PCH_HSIO_TX_DWORD8_ORATE00MARGIN_5_0;
		  }

		  if (PchPolicyPpi->HsioPcieConfig[ IndexClusters ].Lane[Index].HsioTxGen2DownscaleAmpEnable == TRUE) {
		    Data32Or |= (UINT32) (PchPolicyPpi->HsioPcieConfig[ IndexClusters ].Lane[Index].HsioTxGen2DownscaleAmp << N_PCH_HSIO_TX_DWORD8_ORATE01MARGIN_5_0);
		    Data32And &= (UINT32)~B_PCH_HSIO_TX_DWORD8_ORATE01MARGIN_5_0;
		  }

		  if (PchPolicyPpi->HsioPcieConfig[ IndexClusters ].Lane[Index].HsioTxGen3DownscaleAmpEnable == TRUE) {
		    Data32Or |= (UINT32) (PchPolicyPpi->HsioPcieConfig[ IndexClusters ].Lane[Index].HsioTxGen3DownscaleAmp << N_PCH_HSIO_TX_DWORD8_ORATE10MARGIN_5_0);
		    Data32And &= (UINT32)~B_PCH_HSIO_TX_DWORD8_ORATE10MARGIN_5_0;
		  }

		  Offset = HsioLane.Base + (UINT16) R_PCH_HSIO_TX_DWORD8;
		  PchPcrAndThenOr32 (
		    (PCH_SBI_PID) PortId,
		    (UINT16) Offset,
		    Data32And,
		    Data32Or);
		  PchPcrRead32 ((PCH_SBI_PID) PortId, Offset, &Data32);
		  DEBUG ((DEBUG_INFO, "HSIO : PortID = 0x%02x, Offset = 0x%04x, Value = 0x%08x\n", (PCH_SBI_PID) PortId, (UINT16) Offset, (UINT32) Data32));
	    }
        if ((PchPolicyPpi->HsioPcieConfig[ IndexClusters ].Lane[Index].HsioTxGen1DeEmphEnable == TRUE) ||
          (PchPolicyPpi->HsioPcieConfig[ IndexClusters ].Lane[Index].HsioTxGen2DeEmph3p5Enable == TRUE)) {

          Data32Or  = 0;
          Data32And = 0xFFFFFFFF;
          if (PchPolicyPpi->HsioPcieConfig[ IndexClusters ].Lane[Index].HsioTxGen1DeEmphEnable == TRUE) {
            Data32Or |= (UINT32) (PchPolicyPpi->HsioPcieConfig[ IndexClusters ].Lane[Index].HsioTxGen1DeEmph << N_PCH_HSIO_TX_DWORD5_OW2TAPGEN1DEEMPH3P5_5_0);
            Data32And &= (UINT32)~B_PCH_HSIO_TX_DWORD5_OW2TAPGEN1DEEMPH3P5_5_0;
          }

          if (PchPolicyPpi->HsioPcieConfig[ IndexClusters ].Lane[Index].HsioTxGen2DeEmph3p5Enable == TRUE) {
            Data32Or |= (UINT32) (PchPolicyPpi->HsioPcieConfig[ IndexClusters ].Lane[Index].HsioTxGen2DeEmph3p5 << N_PCH_HSIO_TX_DWORD5_OW2TAPGEN2DEEMPH3P5_5_0);
            Data32And &= (UINT32)~B_PCH_HSIO_TX_DWORD5_OW2TAPGEN2DEEMPH3P5_5_0;
          }

          Offset = HsioLane.Base + (UINT16) R_PCH_HSIO_TX_DWORD5;
          PchPcrAndThenOr32 (
            (PCH_SBI_PID) PortId,
            (UINT16) Offset,
            Data32And,
            Data32Or);
          PchPcrRead32 ((PCH_SBI_PID) PortId, Offset, &Data32);
          DEBUG ((DEBUG_INFO, "HSIO : PortID = 0x%02x, Offset = 0x%04x, Value = 0x%08x\n", (PCH_SBI_PID) PortId, (UINT16) Offset, (UINT32) Data32));
        }

        if (PchPolicyPpi->HsioPcieConfig[ IndexClusters ].Lane[Index].HsioTxGen2DeEmph6p0Enable == TRUE) {
          Offset = HsioLane.Base + (UINT16) R_PCH_HSIO_TX_DWORD6;
          PchPcrAndThenOr32 (
            (PCH_SBI_PID) PortId,
            (UINT16) Offset,
            (UINT32) ~B_PCH_HSIO_TX_DWORD6_OW2TAPGEN2DEEMPH6P0_5_0,
            (UINT32) (PchPolicyPpi->HsioPcieConfig[ IndexClusters ].Lane[Index].HsioTxGen2DeEmph6p0 << N_PCH_HSIO_TX_DWORD6_OW2TAPGEN2DEEMPH6P0_5_0));
          PchPcrRead32 ((PCH_SBI_PID) PortId, Offset, &Data32);
          DEBUG ((DEBUG_INFO, "HSIO : PortID = 0x%02x, Offset = 0x%04x, Value = 0x%08x\n", (PCH_SBI_PID) PortId, (UINT16) Offset, (UINT32) Data32));
        }

	    //TODO jambriz 27/27/2015 This property  HsioTxGen1DownscaleAmpEnable is not used in SPT, pending activate this lines
//      if ((PchPolicyPpi->HsioPcieConfig.Lane[Index].HsioTxGen1DownscaleAmpEnable == TRUE) || (PchPolicyPpi->HsioPcieConfig.Lane[Index].HsioTxGen1DeEmphEnable == TRUE)) {
//        MmioAnd8 (RpBase + R_PCH_PCIE_LCTL, (UINT8) ~(B_PCIE_LCTL_LD));
	  }
	}
  }

  DEBUG ((DEBUG_INFO, "POSTCODE << %x >>\n", PCH_HSIO_AFTER_SATA ));
  PostCode( PCH_HSIO_AFTER_SATA );
}

VOID
PchHsioEnablePcieComplianceMode (
  IN UINT8 IndexClusters
  )
{
  UINT32     MaxPcieLanes;
  UINT32     PcieIndex;
  UINT8      HsioLaneNo;
  HSIO_LANE  HsioLane;

  MaxPcieLanes = PcieIPGetPchStandardPciePortNum();
  for (PcieIndex = 0; PcieIndex < MaxPcieLanes; ++PcieIndex) {
    if (PchGetPcieLaneNum (PcieIndex + ( IndexClusters * MaxPcieLanes ), &HsioLaneNo) == EFI_SUCCESS) {
      DEBUG ((DEBUG_INFO, "PCIe compliance mode for PCIe%d, HSIO%d\n", PcieIndex + ( IndexClusters * MaxPcieLanes ), HsioLaneNo));
      HsioGetLane (HsioLaneNo, &HsioLane);
      HsioLaneAndThenOr32 (&HsioLane, R_PCH_HSIO_PCS_DWORD4, (UINT32) ~BIT23, 0);
    }
  }
}

/**
  The function programs common HSIO registers.

  @param[in] PchPolicyPpi The PCH Policy PPI instance

  @retval EFI_SUCCESS             THe HSIO settings have been programmed correctly
**/
EFI_STATUS
PchHsioCommonProg (
  IN  PCH_POLICY_PPI      *PchPolicyPpi
  )
{
  PCH_SBI_HSIO_TABLE_STRUCT   *PchHsioTable;
  UINT8                       PhyModeNum;
  PCH_SERIES                  PchSeries;
  UINT16                      HsioSize;
  UINT16                      Entry;
  UINT32                      Los1;
  UINT32                      Los2;
  UINT32                      Los3;
  UINT32                      Los4;
  UINT8                       IndexClusters;
  EFI_STATUS                  status = 0;
  UINT8 PortId;
  UINT16 Offset;
  UINT32 Data32;
  UINT32 Data32And;
  UINT32 Data32Or;

  PchHsioTable = NULL;
  PhyModeNum = 0;
  HsioSize = 0;
  Entry = 0;
  Los3 = 0;
  Los4 = 0;

  PchSeries = GetPchSeries ();

  ///
  /// Step 1
  /// Read the MIPI Rate Register and LOS Registers
  ///
  PchPcrRead32 (PID_FIA, R_PCH_PCR_FIA_LOS1_REG_BASE, &Los1);
  DEBUG ((DEBUG_INFO, "FIA LOS1 = %08x\n", Los1));
  PchPcrRead32 (PID_FIA, R_PCH_PCR_FIA_LOS2_REG_BASE, &Los2);
  DEBUG ((DEBUG_INFO, "FIA LOS2 = %08x\n", Los2));
  if (PchSeries == PchH) {
    PchPcrRead32 (PID_FIA, R_PCH_PCR_FIA_LOS3_REG_BASE, &Los3);
    DEBUG ((DEBUG_INFO, "FIA LOS3 = %08x\n", Los3));
  }

  ///
  /// Step 2
  /// Assign the appropriate BIOS HSIO table
  ///
  switch (PchStepping ()) {
    case PchHA0:
      DEBUG ((DEBUG_INFO, "Was detected stepping %X\n", PchHA0));
	  PchHsioTable = PchHHsio_A0;
	  HsioSize = (UINT16) (sizeof (PchHHsio_A0) / sizeof (PCH_SBI_HSIO_TABLE_STRUCT));
	  break;
    case PchHB0:
      DEBUG ((DEBUG_INFO, "Was detected stepping %X\n", PchHB0));
	  PchHsioTable = PchHHsio_B0;
	  HsioSize = (UINT16) 0;
	  break;
    case PchHB1:
      DEBUG ((DEBUG_INFO, "Was detected stepping %X\n", PchHB1));
	  PchHsioTable = PchHHsio_B0;
	  HsioSize = (UINT16) 0;
	  break;
    default:
      DEBUG ((DEBUG_INFO, "was not detected any stepping, this was detected %X, was applied PchHA0\n", PchStepping () ));
  PchHsioTable = PchHHsio_A0;
  HsioSize = (UINT16) (sizeof (PchHHsio_A0) / sizeof (PCH_SBI_HSIO_TABLE_STRUCT));
  }

  ///
  /// Step 3
  /// Iterate through the HSIO table
  ///
  for(Entry = 0; Entry < HsioSize; Entry++){
    if(((PchHsioTable[Entry].Offset & B_PCH_HSIO_ACCESS_TYPE) == V_PCH_HSIO_ACCESS_TYPE_MULCAST) &&
      ((PchHsioTable[Entry].Offset & B_PCH_HSIO_LANE_GROUP_NO) !=  PCH_HSIO_LANE_GROUP_COMMON_LANE)) {
        if (PchLosPhyModeCheck ((PCH_SBI_PID) PchHsioTable[Entry].PortId, PchHsioTable[Entry].LanePhyMode, PchSeries, Los1, Los2, Los3, Los4) == EFI_SUCCESS) {
          ///
          /// Step 3a
          /// If the HSIO offset is targeted to all the phymode specific data lanes attached to the common lane (MULCAST)
        /// then program the values only if the PhyModes actually exist on the platform based on FIA LOS
          ///
          DEBUG ((DEBUG_INFO, "Programming HSIO MULCAST : PortID = %02x, Offset = %04x, Value = %08x, LanePhyMode = %02x\n", (PCH_SBI_PID) PchHsioTable[Entry].PortId, (UINT16) PchHsioTable[Entry].Offset, (UINT32) PchHsioTable[Entry].Value, PchHsioTable[Entry].LanePhyMode));
          PchPcrWrite32 ((PCH_SBI_PID) PchHsioTable[Entry].PortId, (UINT16) PchHsioTable[Entry].Offset, (UINT32) PchHsioTable[Entry].Value);
          if ( PchPolicyPpi->SataConfig[0].TestMode == 1 &&
        	   PchHsioTable[Entry].Offset == 0x8B50 &&
        	   PchHsioTable[Entry].LanePhyMode == V_PCH_PCR_FIA_LANE_OWN_SATA) {
            DEBUG ((DEBUG_INFO, "SATA TESTMODE ENABLED in cluster 0!!\n" ));
        	Data32 = 0;
            PortId = 0xBA;
            Offset = 0x8B50 ;
            PchPcrRead32 ( (PCH_SBI_PID) PortId , Offset , &Data32);
            DEBUG ((DEBUG_INFO, "HSIO : PortID = 0x%02x, Offset = 0x%04x, Value = 0x%08x\n", (PCH_SBI_PID) PortId, (UINT16) Offset, (UINT32) Data32));
            Data32And = 0xC0FFF;
            Data32Or = 0x6;
            Data32Or <<= 24;
            PchPcrAndThenOr32 ((PCH_SBI_PID) PortId, (UINT16) Offset,Data32And, Data32Or);
          }
          if ( PchPolicyPpi->SataConfig[1].TestMode == 1 &&
           	   PchHsioTable[Entry].Offset == 0x8B50 &&
           	   PchHsioTable[Entry].LanePhyMode == V_PCH_PCR_FIA_LANE_OWN_SATA) {
            DEBUG ((DEBUG_INFO, "SATA TESTMODE ENABLED in cluster 1!!\n" ));
        	Data32 = 0;
            PortId = 0xBB;
            Offset = 0x8B50 ;
            PchPcrRead32 ( (PCH_SBI_PID) PortId , Offset , &Data32);
            DEBUG ((DEBUG_INFO, "HSIO : PortID = 0x%02x, Offset = 0x%04x, Value = 0x%08x\n", (PCH_SBI_PID) PortId, (UINT16) Offset, (UINT32) Data32));
            Data32And = 0xC0FFF;
            Data32Or = 0x6;
            Data32Or <<= 24;
            PchPcrAndThenOr32 ((PCH_SBI_PID) PortId,(UINT16) Offset,Data32And,Data32Or);
          }
        } else {
          DEBUG ((DEBUG_INFO, "Skipping HSIO MULCAST : PortID = %02x, Offset = %04x, Value = %08x, LanePhyMode = %02x\n", (PCH_SBI_PID) PchHsioTable[Entry].PortId, (UINT16) PchHsioTable[Entry].Offset, (UINT32) PchHsioTable[Entry].Value, PchHsioTable[Entry].LanePhyMode));
        }
    } else {
      ///
      /// Step 3b
      /// If the HSIO offset is targeted to all the data lanes attached to the common lane (BDCAST)
      /// or the offset target is the common lane then program without any checks
      ///
      status = PchPcrWrite32 ((PCH_SBI_PID) PchHsioTable[Entry].PortId, (UINT16) PchHsioTable[Entry].Offset, (UINT32) PchHsioTable[Entry].Value);
      DEBUG ((DEBUG_INFO, "HSIO : PortID = %02x, Offset = %04x, Value = %08x, status 0x%X\n", (PCH_SBI_PID) PchHsioTable[Entry].PortId, (UINT16) PchHsioTable[Entry].Offset, (UINT32) PchHsioTable[Entry].Value, status));
    }
  }

  for( IndexClusters = 0; IndexClusters < PCH_MAX_PCIE_CLUSTERS ; IndexClusters++ ) {
    if (PchPolicyPpi->PcieConfig[ IndexClusters ].ComplianceTestMode) {
      PchHsioEnablePcieComplianceMode ( IndexClusters );
    }
    PortId = 0xBA;
    if ( PchPolicyPpi->PcieConfig[ IndexClusters ].ComplianceTestMode == 1 &&
  		   PchLosPhyModeCheck((PCH_SBI_PID) PortId, V_PCH_PCR_FIA_LANE_OWN_PCIEDMI, PchSeries, Los1, Los2, Los3, Los4) == EFI_SUCCESS )
    {
      DEBUG ((DEBUG_INFO, "PCIE TESTMODE ENABLED in cluster %X!!\n", IndexClusters ));
      Data32 = 0;
      Offset = 0x8350 ;
      PchPcrRead32 ( (PCH_SBI_PID) PortId , Offset , &Data32);
      DEBUG ((DEBUG_INFO, "PCIE TESTMODE ENABLED - HSIO : PortID = 0x%02x, Offset = 0x%04x, Value = 0x%08x\n",
        (PCH_SBI_PID) PortId, (UINT16) Offset, (UINT32) Data32));
      Data32And = 0xC0FFF;
      Data32Or = 0x5;
      Data32Or <<= 24;
      PchPcrAndThenOr32 ((PCH_SBI_PID) PortId, (UINT16) Offset,Data32And, Data32Or);
    }
  }

  return EFI_SUCCESS;
}

/**
  The function programs the Pcie Pll SSC registers.

  @param[in]  PchPolicyPpi        The PCH Policy PPI instance
**/
VOID
PchPciePllSscProg (
  IN  PCH_POLICY_PPI      *PchPolicyPpi
  )
{
  UINT8                       PciePllSsc;
  UINT32                      Data32;
  UINT8 PciePllSscTable[] = {
          0x0,  // 0%
          0x6,  // 0.1%
          0xD,  // 0.2%
          0x14, // 0.3%
          0x1A, // 0.4%
          0x21, // 0.5%
          0x28, // 0.6%
          0x2E, // 0.7%
          0x35, // 0.8%
          0x3C, // 0.9%
          0x42, // 1.0%
          0x49, // 1.1%
          0x50, // 1.2%
          0x56, // 1.3%
          0x5D, // 1.4%
          0x64, // 1.5%
          0x6A, // 1.6%
          0x71, // 1.7%
          0x78, // 1.8%
          0x7E, // 1.9%
          0x85  // 2.0%
          };

  if (PchPolicyPpi->PmConfig.PciePllSsc == PCIE_PLL_SSC_AUTO) {
    return;
  }

  PciePllSsc = PchPolicyPpi->PmConfig.PciePllSsc;
  if (PciePllSsc > MAX_PCIE_PLL_SSC_PERCENT) {
    PciePllSsc = MAX_PCIE_PLL_SSC_PERCENT;
  }

  ///
  /// Step 1:  Clear i_sscen_h to 0b
  ///
  PchPcrAndThenOr32 (
    PID_MODPHY1,
    R_PCH_HSIO_PLL_SSC_DWORD2,
    (UINT32) ~B_PCH_HSIO_PLL_SSC_DWORD2_SSCSEN,
    0
    );

  ///
  /// Step 2:  Write the desired Down Spread % bit values in i_sscstepsize_7_0
  ///
  PchPcrAndThenOr32 (
    PID_MODPHY1,
    R_PCH_HSIO_PLL_SSC_DWORD2,
    (UINT32) ~B_PCH_HSIO_PLL_SSC_DWORD2_SSCSTEPSIZE_7_0,
    (UINT32) (PciePllSscTable[PciePllSsc] << N_PCH_HSIO_PLL_SSC_DWORD2_SSCSTEPSIZE_7_0)
    );

  ///
  /// Step 3:  Set i_ssc_propagate_h to 1b in the LCPLL_CFG1 register
  ///
  PchPcrAndThenOr32 (
    PID_MODPHY1,
    R_PCH_HSIO_PLL_SSC_DWORD3,
    (UINT32) ~B_PCH_HSIO_PLL_SSC_DWORD3_SSC_PROPAGATE,
    (UINT32) B_PCH_HSIO_PLL_SSC_DWORD3_SSC_PROPAGATE
    );

  ///
  /// Step 4:  Set i_sscen_h to 1b
  ///
//  PchPcrAndThenOr32 (
//    PID_MODPHY1,
//    R_PCH_HSIO_PLL_SSC_DWORD2,
//    (UINT32) ~B_PCH_HSIO_PLL_SSC_DWORD2_SSCSEN,
//    (UINT32) B_PCH_HSIO_PLL_SSC_DWORD2_SSCSEN
//    );

  PchPcrRead32 (PID_MODPHY1, R_PCH_HSIO_PLL_SSC_DWORD2, &Data32);
  DEBUG ((DEBUG_INFO, "HSIO : PortID = 0x%02x, Offset = 0x%04x, PciePllSsc = 0x%08x\n", PID_MODPHY1, R_PCH_HSIO_PLL_SSC_DWORD2, (UINT32) Data32));
}


/**
  The function program HSIO registers.

  @param[in] SiPolicyPpi          The Silicon Policy PPI instance
  @param[in] PchPolicyPpi         The PCH Policy PPI instance
  @param[in] PmcBaseAddress       The PMC Base Address

  @retval EFI_SUCCESS             THe HSIO settings have been programmed correctly
**/
EFI_STATUS
PchHsioBiosProg (
  IN  PCH_POLICY_PPI    *PchPolicyPpi
  )
{
  DEBUG ((DEBUG_INFO, "PchHsioBiosProg() Start\n"));
  DEBUG ((DEBUG_INFO, "PostCode <<%X>>\n", PCH_HSIO_BEFORE_RECIPE ));
  PostCode( PCH_HSIO_BEFORE_RECIPE );
  PchHsioCommonProg (PchPolicyPpi);
  PchHsioRxTxEqPolicyProg (PchPolicyPpi );
  DEBUG ((DEBUG_INFO, "PchHsioBiosProg() End\n"));
  DEBUG ((DEBUG_INFO, "PostCode <<%X>>\n", PCH_HSIO_AFTER_RECIPE ));
  PostCode( PCH_HSIO_AFTER_RECIPE );
  return EFI_SUCCESS;
}
