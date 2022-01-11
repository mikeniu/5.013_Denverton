/** @file
  Pei/Dxe/Smm TraceHub Init Lib.

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
#include <Library/TraceHubInitLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/MmPciLib.h>
#include <Library/PchCycleDecodingLib.h>
#include <Library/PchPcrLib.h>
#include <Include/SimicsDebug.h>
#include <Library/CpuPlatformLib.h>
#include <Library/TimerLib.h>
#include <Library/BaseLib.h>
#include "PpmCommon.h"



/**
  This function performs basic initialization for TraceHub
  This routine will consume address range 0xFE0C0000 - 0xFE3FFFFF for BARs usage.
  Although controller allows access to a 64bit address resource, PEI phase is a 32bit env,
  addresses greater than 4G is not allowed by CPU address space.
  So, the addresses must be limited to below 4G and UBARs should be set to 0.
  If this routine is called by platform code, it is expected EnableMode is passed in as PchTraceHubModeDisabled, 
  relying on the Intel TH debugger to enable it through the "cratchpad0 bit [24]".
  By this practice, it gives the validation team the capability to use single debug BIOS 
  to validate the early trace functionality and code path that enable/disable Intel TH using BIOS policy.

  @param[in] EnableMode                 Trace Hub Enable Mode
  @param[in] Destination                Trace Hub Trace Destination

**/
VOID
ConfigureTraceRoute (
  IN  UINT8          Destination,
  IN  UINT32         CsrMtbBar
)
{
  UINT8              SwDestCounter;
  UINT8              SwDestOffset;
  // Program Switching Destination Registers by setting the General Software Trace Destination (GSWDEST @0x88h)
  if(Destination == TraceHubTraceDestPti) {
    //
    // PTI
    //
    //  IF SIMICS
    //    SW_DEST 0x09 = PTI_BINARY
    //            0x0A = PTI_Text // Regular Boot
    // Masters 0:15 P2 0 SMU; 1 ODLA; 2 SoCHAP ; 12 RTIT
    // Masters 16:255 P2 16:18 ME; 23 PMC; 24 AET; 28 FTH; 29 PUINT ??IE
    if (PEI_IN_SIMICS) {
      for (SwDestCounter = 0;SwDestCounter <= V_PCH_TRACE_HUB_MTB_SWDEST_NUMBER; SwDestCounter++) {
        SwDestOffset = SwDestCounter * V_PCH_TRACE_HUB_MTB_SWDEST_WITH;
        MmioWrite32 ((UINT32) (CsrMtbBar + R_PCH_TRACE_HUB_MTB_SWDEST_0 + SwDestOffset), 0x99999999);
        DEBUG ((DEBUG_INFO, "MTB_SWDEST_%02d = 0x%08x, Value = 0x%08x\n",SwDestCounter,CsrMtbBar + R_PCH_TRACE_HUB_MTB_SWDEST_0 + SwDestOffset,MmioRead32 ((UINT32) (CsrMtbBar + R_PCH_TRACE_HUB_MTB_SWDEST_0 + SwDestOffset))));
      }
      MmioWrite32 ((UINT32) (CsrMtbBar + R_PCH_TRACE_HUB_MTB_GSWDEST), 0x09);
      DEBUG ((DEBUG_INFO, "MTB_GSWDEST = 0x%08x, Value = 0x%08x\n",CsrMtbBar + R_PCH_TRACE_HUB_MTB_GSWDEST,MmioRead32 ((UINT32) (CsrMtbBar + R_PCH_TRACE_HUB_MTB_GSWDEST))));
    } else {
      for (SwDestCounter = 0;SwDestCounter <= V_PCH_TRACE_HUB_MTB_SWDEST_NUMBER; SwDestCounter++) {
        SwDestOffset = SwDestCounter * V_PCH_TRACE_HUB_MTB_SWDEST_WITH;
        MmioWrite32 ((UINT32) (CsrMtbBar + R_PCH_TRACE_HUB_MTB_SWDEST_0 + SwDestOffset), 0xAAAAAAAA);
        DEBUG ((DEBUG_INFO, "MTB_SWDEST_%02d = 0x%08x, Value = 0x%08x\n",SwDestCounter,CsrMtbBar + R_PCH_TRACE_HUB_MTB_SWDEST_0 + SwDestOffset,MmioRead32 ((UINT32) (CsrMtbBar + R_PCH_TRACE_HUB_MTB_SWDEST_0 + SwDestOffset))));
      }
      MmioWrite32 ((UINT32) (CsrMtbBar + R_PCH_TRACE_HUB_MTB_GSWDEST), 0x0A);
      DEBUG ((DEBUG_INFO, "MTB_GSWDEST = 0x%08x, Value = 0x%08x\n",CsrMtbBar + R_PCH_TRACE_HUB_MTB_GSWDEST,MmioRead32 ((UINT32) (CsrMtbBar + R_PCH_TRACE_HUB_MTB_GSWDEST))));
    }
  }else{
    //
    // Memory OR USB (EXI) OR DCI OR BSSB
    //
    // Masters 0:15 P0 0 SMU; 1 ODLA; 2 SoCHAP ; 12 RTIT
    // Masters 16:255 P0 16:18 ME; 23 PMC; 24 AET; 28 FTH; 29 PUINT ??IE
    for (SwDestCounter = 0;SwDestCounter <= V_PCH_TRACE_HUB_MTB_SWDEST_NUMBER; SwDestCounter++) {
      SwDestOffset = SwDestCounter * V_PCH_TRACE_HUB_MTB_SWDEST_WITH;
      MmioWrite32 ((UINT32) (CsrMtbBar + R_PCH_TRACE_HUB_MTB_SWDEST_0 + SwDestOffset), 0x88888888);
      DEBUG ((DEBUG_INFO, "MTB_SWDEST_%02d = 0x%08x, Value = 0x%08x\n",SwDestCounter,CsrMtbBar + R_PCH_TRACE_HUB_MTB_SWDEST_0 + SwDestOffset,MmioRead32 ((UINT32) (CsrMtbBar + R_PCH_TRACE_HUB_MTB_SWDEST_0 + SwDestOffset))));
    }
    MmioWrite32 ((UINT32) (CsrMtbBar + R_PCH_TRACE_HUB_MTB_GSWDEST), 0x08);
    DEBUG ((DEBUG_INFO, "MTB_GSWDEST = 0x%08x, Value = 0x%08x\n",CsrMtbBar + R_PCH_TRACE_HUB_MTB_GSWDEST,MmioRead32 ((UINT32) (CsrMtbBar + R_PCH_TRACE_HUB_MTB_GSWDEST))));
  }
  return;
}

/**
  The function disables all messages between Punit and TraceHub (NPK).

  @param[in]      None
  @param[in out]  None

  @retval         EFI_INVALID_PARAMETER   Intended address to access is not a valid address
                  EFI_UNSUPPORTED         Command is not supported by the B2P mailbox or the PUnit
                  EFI_SUCCESS             Punit to TraceHub messages are disabled
                  EFI_NOT_READY           B2P mailbox is not available
                  EFI_TIMEOUT             Punit completion acknowledge was not received
**/
EFI_STATUS
TraceHubDisablePunitMgs(
  )
{
  B2P_MAILBOX_INTERFACE B2PMailbox;
  UINT16                PunitRamAddress = 0x000;
  UINT64                PtrMCHBAR = 0;
  UINT16                Counter = 500;

  DEBUG ((EFI_D_INFO, __FUNCTION__" : Entry\n"));

  // Get MCHBAR
  PtrMCHBAR = PciRead32( PCI_LIB_ADDRESS( MCHBAR_BUS, MCHBAR_DEV, MCHBAR_FUN, REGISTER_MCHBAR_HI ) );
  PtrMCHBAR = LShiftU64(PtrMCHBAR, MCHBAR64_SHIFT_HI);
  PtrMCHBAR |= (UINT64) PciRead32( PCI_LIB_ADDRESS( MCHBAR_BUS, MCHBAR_DEV, MCHBAR_FUN, REGISTER_MCHBAR_LO ) );
  // Clear the less significative bit, because only indicate that this register is active
  PtrMCHBAR &=(UINT64 )(~BIT0);

  // Wait until B2P Mailbox is available (not busy).
  do
  {
    B2PMailbox.Data = (UINT32)MmioRead32( (UINTN) (PtrMCHBAR + BIOS_MAILBOX_INTERFACE_MCHBAR) );
    if (DXE_IN_SIMICS) {
      B2PMailbox.Bits.RunBusy = 0;
    } else {
      MicroSecondDelay(1000000);
      Counter--;
    }
  } while (B2PMailbox.Bits.RunBusy);

  if (B2PMailbox.Bits.RunBusy) {
    DEBUG ((DEBUG_INFO, "\tWarning: Timeout waiting for B2P Mailbox to be available\n"));
    DEBUG ((EFI_D_INFO, __FUNCTION__" : Exit\n"));
    return EFI_NOT_READY;
  }

  B2PMailbox.Data = 0;
  MmioWrite32 ((UINTN)(PtrMCHBAR + BIOS_MAILBOX_DATA_MCHBAR), B2PMailbox.Data);
  DEBUG ((EFI_D_INFO, "\tBIOS_MAILBOX_DATA_MCHBAR = 0x%08x \tValue = 0x%08x\n",(UINT32)(PtrMCHBAR + BIOS_MAILBOX_DATA_MCHBAR), MmioRead32 ( (UINTN) (PtrMCHBAR + BIOS_MAILBOX_INTERFACE_MCHBAR) )));
  B2PMailbox.Data = 0;

  B2PMailbox.Bits.Command = 15;
  B2PMailbox.Bits.Address = PunitRamAddress;
  MmioWrite32 ((UINTN)(PtrMCHBAR + BIOS_MAILBOX_INTERFACE_MCHBAR), B2PMailbox.Data);
  DEBUG ((EFI_D_INFO, "\tBIOS_MAILBOX_INTERFACE_MCHBAR = 0x%08x \tValue = 0x%08x\n",(UINT32)(PtrMCHBAR + BIOS_MAILBOX_DATA_MCHBAR), MmioRead32 ( (UINTN) (PtrMCHBAR + BIOS_MAILBOX_INTERFACE_MCHBAR) )));

  B2PMailbox.Bits.RunBusy = 1;
  MmioWrite32 ((UINTN)(PtrMCHBAR + BIOS_MAILBOX_INTERFACE_MCHBAR), B2PMailbox.Data);
  DEBUG ((DEBUG_INFO, "\tSetting RUN_BUSY\n"));

  do {
    B2PMailbox.Data = (UINT32)MmioRead32( (UINTN) (PtrMCHBAR + BIOS_MAILBOX_INTERFACE_MCHBAR) );
    if (DXE_IN_SIMICS) {
      B2PMailbox.Bits.RunBusy = 0;
    } else {
      MicroSecondDelay(100);
      Counter--;
    }
  } while (B2PMailbox.Bits.RunBusy && (Counter));
  DEBUG ((EFI_D_INFO, "\tBIOS_MAILBOX_INTERFACE_MCHBAR = 0x%08x \tValue = 0x%08x\n",(UINT32)(PtrMCHBAR + BIOS_MAILBOX_DATA_MCHBAR), MmioRead32 ( (UINTN) (PtrMCHBAR + BIOS_MAILBOX_INTERFACE_MCHBAR) )));

  if (B2PMailbox.Bits.RunBusy) {
    DEBUG ((DEBUG_INFO, "\tWarning: Timeout waiting for Punit completion acknowledge\n"));
    DEBUG ((EFI_D_INFO, __FUNCTION__" : Exit\n"));
    return EFI_TIMEOUT;
  }

  DEBUG ((EFI_D_INFO, __FUNCTION__" : Exit\n"));

  return EFI_SUCCESS;
}

VOID
TraceHubInitialize (
  IN  PCH_POLICY_PPI  *PchPolicyPpi
 )
{
  UINTN                   TraceHubBaseAddress;
  UINT32                  PchPwrmBase;
  UINTN                   PmcRegBase;
  UINT32                  PmConA;

  DEBUG ((EFI_D_INFO, __FUNCTION__" : Entry\n"));

  //
  // Skip TraceHub Configuration if coming from Warm Boot
  //
  PmcRegBase = MmPciBase (
                   DEFAULT_PCI_BUS_NUMBER_PCH,
                   PCI_DEVICE_NUMBER_PCH_PMC,
                   PCI_FUNCTION_NUMBER_PCH_PMC
                   );
  PmConA     = MmioRead32 (PmcRegBase + R_PCH_PMC_GEN_PMCON_A);
  DEBUG ((EFI_D_INFO, "\tPmcRegBase = 0x%08x\n",PmcRegBase));
  DEBUG ((EFI_D_INFO, "\tGEN_PMCON_A = 0x%08x\tValue = 0x%08x\n",PmcRegBase + R_PCH_PMC_GEN_PMCON_A,MmioRead32 (PmcRegBase + R_PCH_PMC_GEN_PMCON_A)));
  //
  // Power Gate TraceHub If Disabled
  //
  PchPwrmBaseGet (&PchPwrmBase);
  DEBUG ((EFI_D_INFO, "\tPchPwrmBase = 0x%08x\n",PchPwrmBase));
  DEBUG ((EFI_D_INFO, "\tGBLRST_CAUSE0 = 0x%08x\tValue = 0x%08x\n",PchPwrmBase + R_PCH_PWRM_124,MmioRead32 (PchPwrmBase + R_PCH_PWRM_124)));
  DEBUG ((EFI_D_INFO, "\tGBLRST_CAUSE1 = 0x%08x\tValue = 0x%08x\n",PchPwrmBase + R_PCH_PWRM_128,MmioRead32 (PchPwrmBase + R_PCH_PWRM_128)));
  DEBUG ((EFI_D_INFO, "\tHSWPGCR1 = 0x%08x\tValue = 0x%08x\n",PchPwrmBase + R_PCH_PWRM_HSWPGCR1,MmioRead32 (PchPwrmBase + R_PCH_PWRM_HSWPGCR1)));
  MmioAndThenOr32((PchPwrmBase + R_PCH_PWRM_HSWPGCR1), 0xFFFFFFFE, (0x1 & PchPolicyPpi->PchTraceHubConfig.TraceHubEnable));
  if (!PchPolicyPpi->PchTraceHubConfig.TraceHubEnable) {
    DEBUG ((DEBUG_INFO, "\tTraceHub Power Gated\n"));
    return;
  }
  //
  // Get TraceHub Base Address
  //
  TraceHubBaseAddress = MmPciBase (
                          DEFAULT_PCI_BUS_NUMBER_PCH,
                          PCI_DEVICE_NUMBER_PCH_TRACE_HUB,
                          PCI_FUNCTION_NUMBER_PCH_TRACE_HUB
                          );
  if (MmioRead16 (TraceHubBaseAddress) == 0xFFFF) {
    DEBUG ((DEBUG_INFO, "\tTraceHubBaseAddress Invalid\n"));
    return;
  }
  DEBUG ((DEBUG_INFO, "\tTraceHubBaseAddress = 0x%08x\n",TraceHubBaseAddress));
  //
  // Program the MMIO Bars (MTB, SW, RTIT, FW) on TraceHub
  //
  if (PchPolicyPpi->PchTraceHubConfig.TraceHubFwEnable) {
    // MTB BAR
    MmioWrite32 ((UINT32) TraceHubBaseAddress + R_PCH_TRACE_HUB_CSR_MTB_UBAR, 0);
    MmioAndThenOr32 (
        TraceHubBaseAddress + R_PCH_TRACE_HUB_CSR_MTB_LBAR,
        B_PCH_TRACE_HUB_CSR_MTB_RBAL,
        (UINT32) V_PCH_TRACE_HUB_CSR_MTB_BAR
        );
    DEBUG ((DEBUG_INFO, "\tMTB_LBAR = 0x%08x, Value = 0x%08x\n",
        TraceHubBaseAddress + R_PCH_TRACE_HUB_CSR_MTB_LBAR,
        MmioRead32 (TraceHubBaseAddress + R_PCH_TRACE_HUB_CSR_MTB_LBAR)
        ));
    // SW BAR
    MmioWrite32 ((UINT32) TraceHubBaseAddress + R_PCH_TRACE_HUB_SW_UBAR, 0);
    MmioAndThenOr32 (
        TraceHubBaseAddress + R_PCH_TRACE_HUB_SW_LBAR,
        B_PCH_TRACE_HUB_SW_RBAL,
        (UINT32) V_PCH_TRACE_HUB_SW_BAR
        );
    DEBUG ((DEBUG_INFO, "\tSW_LBAR = 0x%08x, Value = 0x%08x\n",
        TraceHubBaseAddress + R_PCH_TRACE_HUB_SW_LBAR,
        MmioRead32 (TraceHubBaseAddress + R_PCH_TRACE_HUB_SW_LBAR)
        ));
    // RTIT BAR
    MmioWrite32 ((UINT32) TraceHubBaseAddress + R_PCH_TRACE_HUB_RTIT_UBAR, 0);
    MmioAndThenOr32 (
        TraceHubBaseAddress + R_PCH_TRACE_HUB_RTIT_LBAR,
        B_PCH_TRACE_HUB_RTIT_RBAL,
        (UINT32) V_PCH_TRACE_HUB_RTIT_BAR
        );
    DEBUG ((DEBUG_INFO, "\tRTIT_LBAR = 0x%08x, Value = 0x%08x\n",
        TraceHubBaseAddress + R_PCH_TRACE_HUB_RTIT_LBAR,
        MmioRead32(TraceHubBaseAddress + R_PCH_TRACE_HUB_RTIT_LBAR)
        ));
    // FW BAR
    MmioWrite32 ((UINT32) TraceHubBaseAddress + R_PCH_TRACE_HUB_FW_UBAR, 0);
    MmioAndThenOr32 (
        TraceHubBaseAddress + R_PCH_TRACE_HUB_FW_LBAR,
        B_PCH_TRACE_HUB_FW_RBAL,
        (UINT32) V_PCH_TRACE_HUB_FW_BARL
        );
    DEBUG ((DEBUG_INFO, "\tFW_LBAR = 0x%08x, Value = 0x%08x\n",
        TraceHubBaseAddress + R_PCH_TRACE_HUB_FW_LBAR,
        MmioRead32(TraceHubBaseAddress + R_PCH_TRACE_HUB_FW_LBAR)
        ));
    //
    // Manually write 1 to MSEN, PCR[PSF1] + TraceHub base, offset 0x01C[1] to activate the shadow.
    //
    PchPcrAndThenOr8 (PID_PSF1, R_PCH_PCR_PSF1_T0_SHDW_TRACE_HUB_REG_BASE + R_PCH_PCR_PSFX_T0_SHDW_PCIEN, 0xFF, B_PCH_PCR_PSFX_T0_SHDW_PCIEN_MEMEN);
    //
    // Program PCR[PSF1] + TraceHub ACPI BASE, offset 0x00 (BAR0) and offset 0x04 (BAR1) to TraceHub base address.
    //
    PchPcrWrite32 (PID_PSF1, R_PCH_PCR_PSF1_T0_SHDW_TRACE_HUB_ACPI_REG_BASE + R_PCH_PCR_PSFX_T0_SHDW_BAR0, V_PCH_TRACE_HUB_FW_BARL);
    PchPcrWrite32 (PID_PSF1, R_PCH_PCR_PSF1_T0_SHDW_TRACE_HUB_ACPI_REG_BASE + R_PCH_PCR_PSFX_T0_SHDW_BAR1, 0);
    //
    // Manually write 1 to MSEN, PCR[PSF1] + TraceHub ACPI base, offset 0x01C[1] to activate the shadow.
    //
    PchPcrAndThenOr8 (PID_PSF1, R_PCH_PCR_PSF1_T0_SHDW_TRACE_HUB_ACPI_REG_BASE + R_PCH_PCR_PSFX_T0_SHDW_PCIEN, 0xFF, B_PCH_PCR_PSFX_T0_SHDW_PCIEN_MEMEN);
    //
    // Enable Memory Space. Set bit 1(MSE) in the PCI CMD Register (Offset 0x04h). Don't set BME yet.
    //
    MmioAndThenOr8 (
          TraceHubBaseAddress + PCI_COMMAND_OFFSET,
          (UINT8) ~(EFI_PCI_COMMAND_BUS_MASTER),
          (UINT8) (EFI_PCI_COMMAND_MEMORY_SPACE)
          );
    DEBUG ((DEBUG_INFO, "\tPCI CMD OFFSET = 0x%08x, Value = 0x%08x\n",TraceHubBaseAddress + PCI_COMMAND_OFFSET,EFI_PCI_COMMAND_MEMORY_SPACE));
    //
    // Skip the rest of TraceHub Configuration if coming from Warm Boot
    //
    if ( (PmConA & B_PCH_PMC_GEN_PMCON_A_MEM_SR) == B_PCH_PMC_GEN_PMCON_A_MEM_SR ) {
      //
      // Enable Bus Master. Set bit 2 (BME) in the PCI CMD Register (Offset 0x04h)
      //
      MmioOr8 (TraceHubBaseAddress + PCI_COMMAND_OFFSET,(UINT8) (EFI_PCI_COMMAND_BUS_MASTER));
      DEBUG ((DEBUG_INFO, "\tPCI CMD OFFSET = 0x%08x, Value = 0x%08x\n",TraceHubBaseAddress + PCI_COMMAND_OFFSET,EFI_PCI_COMMAND_MEMORY_SPACE));
      DEBUG ((EFI_D_INFO, __FUNCTION__" : Exit\n"));
      return;
    }
    //
    // Configure TraceHub Destination
    //
    if (PchPolicyPpi->PchTraceHubConfig.TraceHubFwDestination == TraceHubTraceDestPti) {
      // PTI
      DEBUG ((DEBUG_INFO, "\tTraceHub Destination: PTI\n"));
      //ConfigureTraceRoute(TraceHubTraceDestPti, V_PCH_TRACE_HUB_CSR_MTB_BAR);
      MmioOr32 ((UINT32) (V_PCH_TRACE_HUB_CSR_MTB_BAR + R_PCH_TRACE_HUB_MTB_PTI_CTL), B_PCH_TRACE_HUB_MTB_PTI_EN);
      MmioOr32 ((UINT32) (V_PCH_TRACE_HUB_CSR_MTB_BAR + R_PCH_TRACE_HUB_MTB_PTI_CTL), PchPolicyPpi->PchTraceHubConfig.TraceHubPtiMode << 4);
      MmioOr32 ((UINT32) (V_PCH_TRACE_HUB_CSR_MTB_BAR + R_PCH_TRACE_HUB_MTB_PTI_CTL), PchPolicyPpi->PchTraceHubConfig.TraceHubPtiSpeed << 16);
      MmioOr32 ((UINT32) (V_PCH_TRACE_HUB_CSR_MTB_BAR + R_PCH_TRACE_HUB_MTB_PTI_CTL), PchPolicyPpi->PchTraceHubConfig.TraceHubPtiTraining << 20);
      DEBUG ((DEBUG_INFO, "\tMTB_PTI_CTL = 0x%08x, Value = 0x%08x\n",V_PCH_TRACE_HUB_CSR_MTB_BAR + R_PCH_TRACE_HUB_MTB_PTI_CTL,MmioRead32 ((UINT32) (V_PCH_TRACE_HUB_CSR_MTB_BAR + R_PCH_TRACE_HUB_MTB_PTI_CTL))));
    } else {
      // EXI Or MEM. Both have the same route config. Only the MSC config changes.
      //ConfigureTraceRoute(TraceHubTraceDestMem, V_PCH_TRACE_HUB_CSR_MTB_BAR);
      if (PchPolicyPpi->PchTraceHubConfig.TraceHubFwDestination == TraceHubTraceDestUsb) {
        DEBUG ((DEBUG_INFO, "\tTraceHub Destination: USB (EXI)\n"));
        //
        // If Memory is the trace destination, BIOS must configure the MSCs for DCI mode to take advantage of the full
        // 4kB of buffering present in TraceHub MSCs (specifically the two 2kB MTBs) for storing pre-MRC trace data.
        //

        //
        // a.    For each MSC, set the mode to DCI mode, and enable the MSC by setting the MSCnEN bit.
        //
        MmioWrite8 ((UINT32) (V_PCH_TRACE_HUB_CSR_MTB_BAR + R_PCH_TRACE_HUB_MTB_MSC0CTL), V_PCH_TRACE_HUB_MTB_MSCNMODE_DCI << N_PCH_TRACE_HUB_MTB_MSCNMODE);
        MmioOr32 ((UINT32) (V_PCH_TRACE_HUB_CSR_MTB_BAR + R_PCH_TRACE_HUB_MTB_MSC0CTL), B_PCH_TRACE_HUB_MTB_MSCNEN);
        DEBUG ((DEBUG_INFO, "\tMTB_MSC0CTL = 0x%08x, Value = 0x%08x\n",V_PCH_TRACE_HUB_CSR_MTB_BAR + R_PCH_TRACE_HUB_MTB_MSC0CTL,MmioRead32 ((UINT32) (V_PCH_TRACE_HUB_CSR_MTB_BAR + R_PCH_TRACE_HUB_MTB_MSC0CTL))));
        MmioWrite8 ((UINT32) (V_PCH_TRACE_HUB_CSR_MTB_BAR + R_PCH_TRACE_HUB_MTB_MSC1CTL), V_PCH_TRACE_HUB_MTB_MSCNMODE_DCI << N_PCH_TRACE_HUB_MTB_MSCNMODE);
        MmioOr8 ((UINT32) (V_PCH_TRACE_HUB_CSR_MTB_BAR + R_PCH_TRACE_HUB_MTB_MSC1CTL), B_PCH_TRACE_HUB_MTB_MSCNEN);
        DEBUG ((DEBUG_INFO, "\tMTB_MSC1CTL = 0x%08x, Value = 0x%08x\n",V_PCH_TRACE_HUB_CSR_MTB_BAR + R_PCH_TRACE_HUB_MTB_MSC1CTL,MmioRead32 ((UINT32) (V_PCH_TRACE_HUB_CSR_MTB_BAR + R_PCH_TRACE_HUB_MTB_MSC1CTL))));
        //
        // b.    In the TraceHub DCI Handler (TraceHub) block, set the STREAMCFG1.ENABLE bit (bit 28).
        //       The default mode is DbC. Trace, so no other bits need to be touched
        //
        MmioOr32 ((UINT32) (V_PCH_TRACE_HUB_CSR_MTB_BAR + R_PCH_TRACE_HUB_MTB_STREAMCFG1), B_PCH_TRACE_HUB_MTB_STREAMCFG1_ENABLE);
        DEBUG ((DEBUG_INFO, "\tMTB_STREAMCFG1 = 0x%08x, Value = 0x%08x\n",V_PCH_TRACE_HUB_CSR_MTB_BAR + R_PCH_TRACE_HUB_MTB_STREAMCFG1,MmioRead32 ((UINT32) (V_PCH_TRACE_HUB_CSR_MTB_BAR + R_PCH_TRACE_HUB_MTB_STREAMCFG1))));
      } else {
        DEBUG ((DEBUG_INFO, "\tTraceHub Destination: MEM\n"));
        //
        // Configure the MSCs for Internal Buffer mode (Debug) and Enable desired MSC(0 or 1 or both)
        //
        MmioWrite8 ((UINT32) (V_PCH_TRACE_HUB_CSR_MTB_BAR + R_PCH_TRACE_HUB_MTB_MSC0CTL), V_PCH_TRACE_HUB_MTB_MSCNMODE_DEBUG << N_PCH_TRACE_HUB_MTB_MSCNMODE);
        MmioOr32 ((UINT32) (V_PCH_TRACE_HUB_CSR_MTB_BAR + R_PCH_TRACE_HUB_MTB_MSC0CTL), B_PCH_TRACE_HUB_MTB_MSCNEN);
        DEBUG ((DEBUG_INFO, "\tMTB_MSC0CTL = 0x%08x, Value = 0x%08x\n",V_PCH_TRACE_HUB_CSR_MTB_BAR + R_PCH_TRACE_HUB_MTB_MSC0CTL,MmioRead32 ((UINT32) (V_PCH_TRACE_HUB_CSR_MTB_BAR + R_PCH_TRACE_HUB_MTB_MSC0CTL))));
        MmioWrite8 ((UINT32) (V_PCH_TRACE_HUB_CSR_MTB_BAR + R_PCH_TRACE_HUB_MTB_MSC1CTL), V_PCH_TRACE_HUB_MTB_MSCNMODE_DEBUG << N_PCH_TRACE_HUB_MTB_MSCNMODE);
        MmioOr32 ((UINT32) (V_PCH_TRACE_HUB_CSR_MTB_BAR + R_PCH_TRACE_HUB_MTB_MSC1CTL), B_PCH_TRACE_HUB_MTB_MSCNEN);
        DEBUG ((DEBUG_INFO, "\tMTB_MSC1CTL = 0x%08x, Value = 0x%08x\n",V_PCH_TRACE_HUB_CSR_MTB_BAR + R_PCH_TRACE_HUB_MTB_MSC1CTL,MmioRead32 ((UINT32) (V_PCH_TRACE_HUB_CSR_MTB_BAR + R_PCH_TRACE_HUB_MTB_MSC1CTL))));
      }
    }
    // Set Storage Overrides & Enable Trigger Overrides
    //
    // Set bits 16, 17 & 20 (SMU (StoreEnOvrd0), TSCU (StoreEnOvrd1) & STH (StoreEnOvrd4)) in the SCR Register (Offset 0xC8h)
    //
    MmioWrite32 ((UINT32) (V_PCH_TRACE_HUB_CSR_MTB_BAR + R_PCH_TRACE_HUB_MTB_SCR), 0x130000);
    DEBUG ((DEBUG_INFO, "\tMTB_SCR = 0x%08x, Value = 0x%08x\n",V_PCH_TRACE_HUB_CSR_MTB_BAR + R_PCH_TRACE_HUB_MTB_SCR,MmioRead32 ((UINT32) (V_PCH_TRACE_HUB_CSR_MTB_BAR + R_PCH_TRACE_HUB_MTB_SCR))));
    // At this point, TraceHub should be configured to log tracing to either memory or PTI ports.
    //
    // TraceHub supports a full-bypass mode, in which the input VISA Cross Bar (XBAR) data is directly connected to the data bus of
    // the output ports which means that all of the functional blocks will be bypassed. The bypass mode is on by default, to support
    // FROM-POWER-ON debug functionality with little-to-no configuration requirements.
    // Clear bit 0 (BYP) in the DSC Register (Offset 0x80h) to disable bypass if it is needed.
    //
    //MmioAnd32 (TraceHubBaseAddress + R_PCH_TRACE_HUB_DSC,(UINT32)(~B_PCH_TRACE_HUB_BYP));
    DEBUG ((DEBUG_INFO, "\tDSC = 0x%08x, Value = 0x%08x\n",TraceHubBaseAddress + R_PCH_TRACE_HUB_DSC,MmioRead32 ((UINT32) (TraceHubBaseAddress + R_PCH_TRACE_HUB_DSC))));
    //
    // Enable Bus Master. Set bit 2 (BME) in the PCI CMD Register (Offset 0x04h)
    //
    MmioOr8 (TraceHubBaseAddress + PCI_COMMAND_OFFSET,(UINT8) (EFI_PCI_COMMAND_BUS_MASTER));
    DEBUG ((DEBUG_INFO, "\tPCI CMD OFFSET = 0x%08x, Value = 0x%08x\n",TraceHubBaseAddress + PCI_COMMAND_OFFSET,MmioRead32 ((UINT32) (TraceHubBaseAddress + PCI_COMMAND_OFFSET))));
  }
  DEBUG ((EFI_D_INFO, __FUNCTION__" : Exit\n"));

  return;
}

