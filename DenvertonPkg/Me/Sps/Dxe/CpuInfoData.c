/**

Copyright (c) 2010 - 2016, Intel Corporation.

This source code and any documentation accompanying it ("Material") is furnished
under license and may only be used or copied in accordance with the terms of that
license.  No license, express or implied, by estoppel or otherwise, to any
intellectual property rights is granted to you by disclosure or delivery of these
Materials.  The Materials are subject to change without notice and should not be
construed as a commitment by Intel Corporation to market, license, sell or support
any product or technology.  Unless otherwise provided for in the license under which
this Material is provided, the Material is provided AS IS, with no warranties of
any kind, express or implied, including without limitation the implied warranties
of fitness, merchantability, or non-infringement.  Except as expressly permitted by
the license for the Material, neither Intel Corporation nor its suppliers assumes
any responsibility for any errors or inaccuracies that may appear herein.  Except
as expressly permitted by the license for the Material, no part of the Material
may be reproduced, stored in a retrieval system, transmitted in any form, or
distributed by any means without the express written consent of Intel Corporation.


  @file CpuInfoData.c

   This implements filling out the HECI Message responsible of passing
   CPU Info data.

**/
#include <Base.h>
#include <Uefi.h>
#include <PiDxe.h>

#include <Library/PcdLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>

#include <Library/UefiRuntimeServicesTableLib.h>
#include <Protocol/MpService.h>
#include <Library/CpuConfigLib.h>

#include <Guid/SpsInfoHobGuid.h>
#include <Protocol/HeciProtocol.h>
#include <Sps.h>
#include <CpuRegs.h>
#include "SpsDxe.h"


/*****************************************************************************
 * Local definitions
 *****************************************************************************/
#define CPU_TSTATES_NUM_STD    8
#define CPU_TSTATES_NUM_FG    15  // fine grained T-states


/*****************************************************************************
 @brief
  Gather and send to ME host configuration data needed by Node Manager

  @param[in] pSpsHob    Info from SPS PEI driver
  @param[in] MeNmConfig BIOS setup configuration from ME Policy

  @retval EFI_SUCCESS             MeMode copied
  @retval EFI_INVALID_PARAMETER   Pointer of MeMode is invalid
**/
EFI_STATUS
GatherCPUInfoData(
  SPS_DXE_CONTEXT       *pSpsContext)
{
  EFI_STATUS Status;
  UINT32     HeciMsgLen;
  UINT32     RspLen;
  UINT32     Reg;
  UINT32     Index;
  UINT32     ProcMask;
  UINT32     tCoreMask[1];
  union
  {
    MKHI_MSG_NM_HOST_CFG_REQ   NMHostCfgReq;
    MKHI_MSG_NM_HOST_CFG_RSP   NMHostCfgRsp;
  } HeciMsg;
  HECI_PROTOCOL            *pHeciProtocol;
  CPU_COLLECTED_DATA       *pCpuData;

  Status = gBS->LocateProtocol(&gHeciProtocolGuid, NULL, &pHeciProtocol);
  if (EFI_ERROR(Status))
  {
    DEBUG((EFI_D_ERROR, "[SPS] ERROR: Cannot locate HECI protocol (status: %r)\n", Status));
    return Status;
  }
  SetMem(&HeciMsg, sizeof(HeciMsg), 0);

  HeciMsg.NMHostCfgReq.Mkhi.Data = (NM_CMD_HOSTCFG << 8) | MKHI_GRP_NM;

  HeciMsg.NMHostCfgReq.Capabilities.Bits.TurboEn = pSpsContext->MePolicy->MeNmConfig.TurboMode;
  HeciMsg.NMHostCfgReq.Capabilities.Bits.PowerMsmt = pSpsContext->MePolicy->MeNmConfig.NmPowerMsmtSupport;
  HeciMsg.NMHostCfgReq.Capabilities.Bits.HwChange = pSpsContext->MePolicy->MeNmConfig.NmHwChangeStatus;
  //
  // ME wants PLATFORM_INFO MSRs sent raw.
  //
  HeciMsg.NMHostCfgReq.PlatformInfo = AsmReadMsr64(MSR_PLATFORM_INFO);
  HeciMsg.NMHostCfgReq.Altitude = (INT32)pSpsContext->MePolicy->MeNmConfig.SpsAltitude;

  ProcMask = 0;
  SetMem(&tCoreMask[0], sizeof(tCoreMask), 0);
  for (pCpuData = &mCpuConfigLibConfigContextBuffer->CollectedDataBuffer[0];
       pCpuData < &mCpuConfigLibConfigContextBuffer->CollectedDataBuffer[mCpuConfigLibConfigContextBuffer->NumberOfProcessors];
       pCpuData++)
  {
    ASSERT(pCpuData->ProcessorLocation.Package < sizeof(tCoreMask)/sizeof(tCoreMask[0]));
    if (!(ProcMask & 1 << pCpuData->ProcessorLocation.Package))
    {
      ProcMask |= 1 << pCpuData->ProcessorLocation.Package;
      HeciMsg.NMHostCfgReq.ProcNumber++;
    }
    if (!(tCoreMask[pCpuData->ProcessorLocation.Package] & (1 << pCpuData->ProcessorLocation.Core)))
    {
      tCoreMask[pCpuData->ProcessorLocation.Package] |= 1 << pCpuData->ProcessorLocation.Core;
      HeciMsg.NMHostCfgReq.ProcCoresEnabled++;
      HeciMsg.NMHostCfgReq.ProcThreadsEnabled++;
    }
#if 0
// this option is not used in DNV
    else
    { //
      // For some reason hyper-threads are listed in this library regardless whether
      // HT is enabled or not. Let's filter them using configuration data.
      //
      if (!ProcessorHyperThreadingDisable)
      {
        HeciMsg.NMHostCfgReq.ProcThreadsEnabled++;
      }
    }
#endif

    if (pCpuData->PackageBsp)
    {
      HeciMsg.NMHostCfgReq.PStatesNumber = (UINT8)pCpuData->CpuMiscData.NumberOfPStates;
      ASSERT(pCpuData->CpuMiscData.NumberOfPStates <= sizeof(HeciMsg.NMHostCfgReq.PStatesRatio)/sizeof(HeciMsg.NMHostCfgReq.PStatesRatio[0]));
      if (pCpuData->CpuMiscData.FvidTable != NULL)
      {
        for (Index = 0; Index < pCpuData->CpuMiscData.NumberOfPStates; Index++)
        {
          HeciMsg.NMHostCfgReq.PStatesRatio[Index] = (UINT8)pCpuData->CpuMiscData.FvidTable[Index].Ratio;
        }
      }
      if (pSpsContext->MePolicy->MeNmConfig.TStateEnable)
      {
        HeciMsg.NMHostCfgReq.TStatesNumber = CPU_TSTATES_NUM_STD;
        if (pCpuData->CpuidData.CpuIdLeaf != NULL &&
            pCpuData->CpuidData.NumberOfBasicCpuidLeafs > EFI_CPUID_POWER_MANAGEMENT_PARAMS &&
            (pCpuData->CpuidData.CpuIdLeaf[EFI_CPUID_POWER_MANAGEMENT_PARAMS].RegEax & EFI_FINE_GRAINED_CLOCK_MODULATION))
        {
          HeciMsg.NMHostCfgReq.TStatesNumber = CPU_TSTATES_NUM_FG;
        }
      }
    }
  } // for (pProcData...)
  //
  // Number of cores implemented can be found in CPU ID function 11 - core topology.
  // Core topology subfunction 1 gives the number of possible logical threads, subfunction 0
  // the number of threads per core. To calculate cores divide threads by threads per core.
  //
  AsmCpuidEx(EFI_CPUID_CORE_TOPOLOGY, 1, NULL, &Reg, NULL, NULL);
  HeciMsg.NMHostCfgReq.ProcCoresNumber = (UINT8)Reg;
  AsmCpuidEx(EFI_CPUID_CORE_TOPOLOGY, 0, NULL, &Reg, NULL, NULL);
  HeciMsg.NMHostCfgReq.ProcCoresNumber /= (UINT8)Reg;
  HeciMsg.NMHostCfgReq.ProcCoresNumber *= HeciMsg.NMHostCfgReq.ProcNumber;

  DEBUG((EFI_D_INFO, "[SPS] Sending host configuration to NM\n"));
  DEBUG((EFI_D_INFO, "[SPS]   Capabilities : 0x%04x\n", HeciMsg.NMHostCfgReq.Capabilities.Uint16));
  DEBUG((EFI_D_INFO, "[SPS]   P/T-states:    %d/%d\n",
                     HeciMsg.NMHostCfgReq.PStatesNumber, HeciMsg.NMHostCfgReq.TStatesNumber));
  DEBUG((EFI_D_INFO, "[SPS]   Min/max power: %d/%d\n",
                     HeciMsg.NMHostCfgReq.MinPower, HeciMsg.NMHostCfgReq.MaxPower));
  DEBUG((EFI_D_INFO, "[SPS]   Processor packages: %d\n", HeciMsg.NMHostCfgReq.ProcNumber));
  DEBUG((EFI_D_INFO, "[SPS]   Processor cores:    %d\n", HeciMsg.NMHostCfgReq.ProcCoresNumber));
  DEBUG((EFI_D_INFO, "[SPS]   Processor cores enabled:   %d\n", HeciMsg.NMHostCfgReq.ProcCoresEnabled));
  DEBUG((EFI_D_INFO, "[SPS]   processor threads enabled: %d\n", HeciMsg.NMHostCfgReq.ProcThreadsEnabled));
  DEBUG((EFI_D_INFO, "[SPS]   Platform info:     0x%08X%08X\n",
                     (UINT32)(HeciMsg.NMHostCfgReq.PlatformInfo >> 32), (UINT32)HeciMsg.NMHostCfgReq.PlatformInfo));
  DEBUG((EFI_D_INFO, "[SPS]   Altitude: %d\n", HeciMsg.NMHostCfgReq.Altitude));

  HeciMsgLen = sizeof(HeciMsg.NMHostCfgReq);
  if ((HeciMsg.NMHostCfgReq.PStatesNumber > 0))
  {
    DEBUG((EFI_D_INFO, "[SPS]   PStatesRatio[%d]: ", HeciMsg.NMHostCfgReq.PStatesNumber));
    for (Index = 0; Index < HeciMsg.NMHostCfgReq.PStatesNumber; Index++)
    {
      DEBUG((EFI_D_INFO, " %02X", HeciMsg.NMHostCfgReq.PStatesRatio[Index]));
    }
    DEBUG((EFI_D_INFO, "\n"));
  }
  RspLen = sizeof(HeciMsg.NMHostCfgRsp);
  Status = pHeciProtocol->SendwACK(HECI1_DEVICE, (UINT32*)&HeciMsg.NMHostCfgReq, HeciMsgLen,
                                  &RspLen,
                                  SPS_CLIENTID_BIOS, SPS_CLIENTID_ME_MKHI);
  if (EFI_ERROR(Status))
  {
    DEBUG((EFI_D_ERROR, "[SPS] ERROR: Cannot send host configuration info to NM (%r)\n", Status));
  }
  else
  {
    switch (HeciMsg.NMHostCfgRsp.Status)
    {
      case STATUS_NM_HOST_CFG_RSP_OK:
        DEBUG((EFI_D_INFO, "[SPS] Configuration info to NM was sent.\n"));
      break;
      case STATUS_NM_HOST_CFG_RSP_WRONG_MSG_FORMAT:
        DEBUG((EFI_D_ERROR, "[SPS] ERROR: Host configuration info response has wrong format\n"));
      break;
      case STATUS_NM_HOST_CFG_RSP_ALTITUDE_MISSING:
        DEBUG((EFI_D_ERROR, "[SPS] ERROR: Altitude was missing in host configuration info\n"));
      break;
      case STATUS_NM_HOST_CFG_RSP_PSTATESRATIO_MISSING:
        DEBUG((EFI_D_ERROR, "[SPS] ERROR: PStatesRatio was missing in host configuration info\n"));
      break;
      default:
        DEBUG((EFI_D_ERROR, "[SPS] ERROR: Unknown host configuration info status\n"));
      break;
    }
  }
  return Status;
} // GatherCPUInfoData()
