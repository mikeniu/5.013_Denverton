/**
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c) 2015 - 2017 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

@file:
  MeFiaMuxLib.c

@brief:
  Me FIA MUX configuration interface implementation.

**/

#include <Uefi.h>
#include <Library/PciLib.h>
#include <Library/MmPciLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MeFiaMuxLib.h>
#include <Library/MeTypeLib.h>
#include <Ppi/HeciPpi.h>
#include <Sps.h>
#include <MeAccess.h>
// Denverton AptioV override Start - EIP#337485
#include <Library/TimerLib.h>

#define SPS_INIT_TIMEOUT        2000000 // [us] Timeout when waiting for InitComplete
#define STALL_1MS               1000
// Denverton AptioV override End - EIP#337485

EFI_STATUS
EFIAPI
MeFiaMuxConfigSet(IN ME_FIA_CONFIG *pConfig)
{
  EFI_STATUS                      Status;
  UINT32                          CommandSize;
  SPS_MEFS1                       MeFs1;
  SPS_MEFS2                       MeFs2;
  HECI_PPI                        *pPpiHeci;
  UINT32                          SpsInitTimeout =  SPS_INIT_TIMEOUT / STALL_1MS; // Denverton AptioV override - EIP#337485

  union {
    ICC_MSG_SET_FIA_MUX_CONFIG_REQ Req;
    ICC_MSG_SET_FIA_MUX_CONFIG_RSP Rsp;
  } IccMsg;


  DEBUG ((EFI_D_INFO, "[SPS] (ICC) MeFiaMuxConfigSet\n"));

  if (!MeTypeIsSps() || MeTypeIsSpsInRecovery()) {
    DEBUG ((DEBUG_INFO, "[SPS] (ICC) MeFiaMuxConfigSet() only supported in SPS Operational mode\n"));
    return EFI_UNSUPPORTED;
  }

  Status = PeiServicesLocatePpi(&gHeciPpiGuid, 0, NULL, &pPpiHeci);
  if (EFI_ERROR(Status)) {
    DEBUG ((DEBUG_ERROR, "[SPS] ERROR: (ICC) MeFiaMuxConfigSet: Cannot locate HECI PPI\n"));
    return EFI_DEVICE_ERROR;
  }

  // Denverton AptioV override Start - EIP#337485
  // Wait till ME is ready. Timeout is 2 seconds.
  MeFs1.UInt32 = HeciPciRead32(SPS_REG_MEFS1);
  while (!MeFs1.Bits.InitComplete){
         if (!SpsInitTimeout) {
    	    DEBUG((DEBUG_ERROR, "[SPS] ERROR: Timeout when waiting for ME init complete\n"));
            return EFI_DEVICE_ERROR;
         }
         SpsInitTimeout--;
		 MicroSecondDelay(STALL_1MS);
		 MeFs1.UInt32 = HeciPciRead32(SPS_REG_MEFS1);
  }

  // If END_OF_POST is set, then perform Global Platform Reset to rectify
  MeFs2.UInt32 = HeciPciRead32(SPS_REG_MEFS2);
  if (MeFs2.Bits.EopStatus){
      DEBUG ((DEBUG_ERROR, "[SPS] ERROR: (ICC) MeFiaMuxConfigSet: invalid after END_OF_POST (MEFS1: %08X, MEFS2: %08X).\n",
        MeFs1.UInt32, MeFs2.UInt32));
      return EFI_NOT_READY;
  }
  // Denverton AptioV override End - EIP#337485
  ZeroMem(&IccMsg, sizeof(IccMsg));

  IccMsg.Req.Header.ApiVersion   = ICC_API_4_0;
  IccMsg.Req.Header.IccCommand   = ICC_CMD_SET_FIA_MUX_CONFIG;
  IccMsg.Req.Header.IccStatus    = ICC_STS_SUCCESS;
  IccMsg.Req.Invalidate          = (NULL == pConfig) ? 1 : 0;
  IccMsg.Req.Header.BufferLength = sizeof(IccMsg.Req) - sizeof(IccMsg.Req.Header);
  CommandSize                    = sizeof(IccMsg.Req);

  if (NULL != pConfig)
  {
    CopyMem(IccMsg.Req.MuxConfiguration, (UINT8*)(&pConfig->MuxConfiguration), sizeof(IccMsg.Req.MuxConfiguration));
    DEBUG ((DEBUG_INFO, "[SPS] (ICC) MeFiaMuxConfigSet: Requested FIA Mux Lanes Configuration = 0x%lx\n", pConfig->MuxConfiguration.MeFiaMuxLaneConfig));

    CopyMem(IccMsg.Req.SataLaneConfiguration, (UINT8*)(&pConfig->SataLaneConfiguration), sizeof(IccMsg.Req.SataLaneConfiguration));
    DEBUG ((DEBUG_INFO, "[SPS] (ICC) MeFiaMuxConfigSet: Requested SATA Lanes Configuration = 0x%lx\n", pConfig->SataLaneConfiguration.MeFiaSataLaneConfig));

    CopyMem(IccMsg.Req.PcieRootPortsConfiguration, (UINT8*)(&pConfig->PcieRootPortsConfiguration), sizeof(IccMsg.Req.PcieRootPortsConfiguration));
    DEBUG ((DEBUG_INFO, "[SPS] (ICC) MeFiaMuxConfigSet: Requested PCIE Root Ports Configuration = 0x%lx\n", pConfig->PcieRootPortsConfiguration.MeFiaPcieRootPortsConfig));
  }

  Status = pPpiHeci->SendwAck(HECI1_DEVICE,
                              (UINT32*)&IccMsg.Req,
                              CommandSize, NULL,
                              SPS_CLIENTID_BIOS, SPS_CLIENTID_ME_ICC);
  if (EFI_ERROR(Status)) {
    DEBUG ((DEBUG_ERROR, "[SPS] ERROR: (ICC) MeFiaMuxConfigSet: Message failed! EFI_STATUS = %r\n", Status));
    return EFI_DEVICE_ERROR;
  }

  switch (IccMsg.Rsp.Header.IccStatus) {
    case ICC_STS_SUCCESS:
      DEBUG ((DEBUG_INFO, "[SPS] (ICC) MeFiaMuxConfigSet: FIA configuration set successfully\n"));
      Status = EFI_SUCCESS;
      break;

    case ICC_STS_FIA_MUX_CONFIG_SKU_MISMATCH:
      DEBUG ((DEBUG_ERROR, "[SPS] ERROR: (ICC) MeFiaMuxConfigSet: SKU mismatch\n"));
      Status = EFI_OUT_OF_RESOURCES;
      break;

    case ICC_STS_INVALID_PARAMETERS:
      DEBUG ((DEBUG_ERROR, "[SPS] ERROR: (ICC) MeFiaMuxConfigSet: FIA configuration invalid\n"));
      Status = EFI_ABORTED;
      break;

    default:
      DEBUG ((DEBUG_ERROR, "[SPS] ERROR: (ICC) MeFiaMuxConfigSet: Error in response! ICC Status = 0x%x\n", IccMsg.Rsp.Header.IccStatus));
      ASSERT(IccMsg.Rsp.Header.IccStatus == ICC_STS_SUCCESS || IccMsg.Rsp.Header.IccStatus == ICC_STS_FIA_MUX_CONFIG_SKU_MISMATCH);
      return EFI_PROTOCOL_ERROR;
      break;
  }

  DEBUG ((DEBUG_INFO, "[SPS] (ICC) MeFiaMuxConfigSet: End - %r\n", Status));
  return Status;
} // MeFiaMuxConfigSet()

EFI_STATUS
EFIAPI
MeFiaMuxConfigGet(OUT ME_FIA_CONFIG *pConfig, OUT UINT32 *pLanesAllowed)
{
  EFI_STATUS                    Status;
  UINT32                        CommandSize;
  UINT32                        ResponseSize;
  SPS_MEFS1                     MeFs1;
  SPS_MEFS2                     MeFs2;
  HECI_PPI                      *pPpiHeci;
  BOOLEAN                       MeFiaConfigRcvd = FALSE;
  UINT32                          SpsInitTimeout =  SPS_INIT_TIMEOUT / STALL_1MS; // Denverton AptioV override - EIP#337485

  union {
    ICC_MSG_GET_FIA_MUX_CONFIG_REQ Req;
    ICC_MSG_GET_FIA_MUX_CONFIG_RSP Rsp;
  } IccMsg;

  DEBUG ((DEBUG_INFO, "[SPS] (ICC) MeFiaMuxConfigGet\n"));

  if (pConfig == NULL && pLanesAllowed == NULL) {
    DEBUG ((DEBUG_ERROR, "[SPS] ERROR: (ICC) MeFiaMuxConfigGet: both pConfig and pLanesAllowed cannot be NULL\n"));
    ASSERT(pConfig != NULL || pLanesAllowed != NULL);
    return EFI_INVALID_PARAMETER;
  }

  if (!MeTypeIsSps()) {
    DEBUG ((DEBUG_INFO, "[SPS] (ICC) MeFiaMuxConfigGet() only supported in SPS FW\n"));
    return EFI_UNSUPPORTED;
  }

  Status = PeiServicesLocatePpi(&gHeciPpiGuid, 0, NULL, &pPpiHeci);
  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_ERROR, "[SPS] ERROR: (ICC) MeFiaMuxConfigGet: Cannot locate HECI PPI\n"));
    return EFI_DEVICE_ERROR;
  }

  MeFs1.UInt32 = HeciPciRead32(SPS_REG_MEFS1);
  MeFs2.UInt32 = HeciPciRead32(SPS_REG_MEFS2);

  // Denverton AptioV override Start - EIP#337485
  // Wait till ME is ready. Timeout is 2 seconds.
  MeFs1.UInt32 = HeciPciRead32(SPS_REG_MEFS1);
  while (!MeFs1.Bits.InitComplete){
         if (!SpsInitTimeout) {
    	    DEBUG((DEBUG_ERROR, "[SPS] ERROR: Timeout when waiting for ME init complete\n"));
            return EFI_DEVICE_ERROR;
         }
         SpsInitTimeout--;
		 MicroSecondDelay(STALL_1MS);
		 MeFs1.UInt32 = HeciPciRead32(SPS_REG_MEFS1);
  }
  
  // Return error if END_OF_POST
  MeFs2.UInt32 = HeciPciRead32(SPS_REG_MEFS2);
  if (MeFs2.Bits.EopStatus){
      DEBUG ((DEBUG_ERROR, "[SPS] ERROR: (ICC) MeFiaMuxConfigGet: invalid after END_OF_POST (MEFS1: %08X, MEFS2: %08X).\n",
        MeFs1.UInt32, MeFs2.UInt32));
      return EFI_NOT_READY;
  }
  // Denverton AptioV override End - EIP#337485


  ZeroMem(&IccMsg, sizeof(IccMsg));

  IccMsg.Req.Header.ApiVersion   = ICC_API_4_0;
  IccMsg.Req.Header.IccCommand   = ICC_CMD_GET_FIA_MUX_CONFIG;
  IccMsg.Req.Header.IccStatus    = ICC_STS_SUCCESS;
  IccMsg.Req.Header.BufferLength = sizeof(IccMsg.Req) - sizeof(IccMsg.Req.Header);

  CommandSize                    = sizeof(IccMsg.Req);
  ResponseSize                   = sizeof(IccMsg.Rsp);

  Status = pPpiHeci->SendwAck(HECI1_DEVICE,
                              (UINT32*)&IccMsg.Req,
                              CommandSize, &ResponseSize,
                              SPS_CLIENTID_BIOS, SPS_CLIENTID_ME_ICC);
  if (EFI_ERROR(Status)) {
    DEBUG ((DEBUG_ERROR, "[SPS] ERROR: (ICC) MeFiaMuxConfigGet: Message failed! EFI_STATUS = %r\n", Status));
    return EFI_DEVICE_ERROR;
  }

  switch (IccMsg.Rsp.Header.IccStatus) {
    case ICC_STS_SUCCESS:
      DEBUG((DEBUG_INFO, "[SPS] (ICC) MeFiaMuxConfigGet: FIA Mux configuration retrieved, number of lanes allowed: (%d)\n", IccMsg.Rsp.LanesAllowed));
      MeFiaConfigRcvd = TRUE;
      Status = EFI_SUCCESS;
      break;

    case ICC_STS_FIA_MUX_NO_CONFIG_FOUND:
      DEBUG ((DEBUG_INFO, "[SPS] (ICC) MeFiaMuxConfigGet: no FIA Mux configuration in ME Flash\n"));
      DEBUG((DEBUG_INFO, "[SPS] (ICC) MeFiaMuxConfigGet: Softstraps Configuration will be used\n"));
      MeFiaConfigRcvd = TRUE;
      Status = EFI_SUCCESS;
      break;

    default:
      DEBUG ((DEBUG_ERROR, "[SPS] ERROR: (ICC) MeFiaMuxConfigGet: Error in response! ICC Status = 0x%x\n", IccMsg.Rsp.Header.IccStatus));
      ASSERT(IccMsg.Rsp.Header.IccStatus == ICC_STS_SUCCESS || IccMsg.Rsp.Header.IccStatus == ICC_STS_FIA_MUX_NO_CONFIG_FOUND);
      return EFI_PROTOCOL_ERROR;
      break;
  }

  if (IccMsg.Rsp.LanesAllowed == 0) {
    DEBUG ((DEBUG_ERROR, "[SPS] ERROR: (ICC) MeFiaMuxConfigGet: ME responded with LanesAllowed = 0!\n"));
    ASSERT(IccMsg.Rsp.LanesAllowed != 0);
    return EFI_PROTOCOL_ERROR;
  }

  if (pLanesAllowed != NULL) {
    DEBUG ((DEBUG_INFO, "[SPS] (ICC) MeFiaMuxConfigGet: Received  number of Lanes allowed = 0x%x\n", IccMsg.Rsp.LanesAllowed));
    *pLanesAllowed = IccMsg.Rsp.LanesAllowed;
  }

  if (pConfig != NULL && MeFiaConfigRcvd) {

    pConfig->MuxConfiguration.MeFiaMuxLaneConfig = 0;
    CopyMem((UINT8*)(&pConfig->MuxConfiguration), IccMsg.Rsp.MuxConfiguration, sizeof(IccMsg.Rsp.MuxConfiguration));
    DEBUG ((DEBUG_INFO, "[SPS] (ICC) MeFiaMuxConfigGet: Received FIA Mux Lanes Configuration = 0x%lx\n", pConfig->MuxConfiguration.MeFiaMuxLaneConfig));


    pConfig->SataLaneConfiguration.MeFiaSataLaneConfig = 0;
    CopyMem((UINT8*)(&pConfig->SataLaneConfiguration), IccMsg.Rsp.SataLaneConfiguration, sizeof(IccMsg.Rsp.SataLaneConfiguration));
    DEBUG ((DEBUG_INFO, "[SPS] (ICC) MeFiaMuxConfigGet: Received SATA Lanes Configuration = 0x%lx\n", pConfig->SataLaneConfiguration.MeFiaSataLaneConfig));


    pConfig->PcieRootPortsConfiguration.MeFiaPcieRootPortsConfig = 0;
    CopyMem((UINT8*)(&pConfig->PcieRootPortsConfiguration), IccMsg.Rsp.PcieRootPortsConfiguration, sizeof(IccMsg.Rsp.PcieRootPortsConfiguration));
    DEBUG ((DEBUG_INFO, "[SPS] (ICC) MeFiaMuxConfigGet: Received PCIE Root Ports Configuration = 0x%lx\n", pConfig->PcieRootPortsConfiguration.MeFiaPcieRootPortsConfig));
  }

  DEBUG ((DEBUG_INFO, "[SPS] (ICC) MeFiaMuxConfigGet: End - %r\n", Status));
  return Status;
} // MeFiaMuxConfigGet()
