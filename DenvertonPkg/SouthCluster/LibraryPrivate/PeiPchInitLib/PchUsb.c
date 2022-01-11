/** @file
  Initializes PCH USB Controllers.

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
#include "PchUsb.h"
#include <Ppi/Spi.h>
#include <Library/PostCodeLib.h>
#ifdef FSP_FLAG
#include <StPgIpInit/Include/PchPmcStPg.h>
#endif    //FSP_FLAG

/**
  Check whether XHCI controller is present in this chipset SKU.

  @retval TRUE    XHCI is supported in this chipset
  @retval FALSE   XHCI is not supported
**/
BOOLEAN
EFIAPI
XhciIpIsXhciPresent (
  VOID
  )
{
  UINT32  PwrmBase;
  UINT32  FuseDis2State;
  PchPwrmBaseGet (&PwrmBase);
  FuseDis2State = MmioRead32 ((UINTN) (PwrmBase + R_PCH_PWRM_STPG_FUSE_SS_DIS_RD_2));
  return (FuseDis2State & B_PCH_PWRM_XHCI_FUSE_SS_DIS) == 0;
}

/**
  Check whether XHCI controller is enabled (in terms of
  static power gating).

  @retval TRUE    XHCI is enabled
  @retval FALSE   XHCI is statically power gated
**/
BOOLEAN
XhciIpIsXhciEnabled (
  VOID
)
{
  UINT32  PwrmBase;
  UINT32  STPGFDisState;
  PchPwrmBaseGet (&PwrmBase);
  STPGFDisState = MmioRead32 ((UINTN) (PwrmBase + R_PCH_PWRM_ST_PG_FDIS_PMC_1));
  return (STPGFDisState & B_PCH_PWRM_XHCI_FDIS_PMC) == 0;
}

/**
  Configures PCH xHCI device

  @param[in]  PchPolicy           The PCH Policy PPI instance
  @param[in]  TempMemBaseAddr     Temporary Memory Base Address for PCI
                                  devices to be used to initialize MMIO
                                  registers.

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_OUT_OF_RESOURCES    Do not have enough resources to initialize the driver
**/
EFI_STATUS
EFIAPI
ConfigureXhciPreMem (
  IN  PCH_POLICY_PPI            *PchPolicy,
  IN  UINT32                    TempMemBaseAddr
  )
{
  UINTN           XhciPciMmBase;
  UINT16          ABase;
  PCH_SERIES      PchSeries;
  PCH_USB_CONFIG  *UsbConfig;
  UINT32          XhciMmioBase;
#ifdef FSP_FLAG
  EFI_STATUS      Status;
  UINT32          FspData32And;
  UINT32          FspData32Or;
#endif //FSP_FLAG
  PCH_STEPPING    PchStep;
  PchStep     = PchStepping ();

  DEBUG ((EFI_D_INFO, __FUNCTION__" : Entry\n"));
  UsbConfig     = &PchPolicy->UsbConfig;
  XhciMmioBase  = TempMemBaseAddr;
  XhciPciMmBase   = MmPciBase (
                      DEFAULT_PCI_BUS_NUMBER_PCH,
                      PCI_DEVICE_NUMBER_PCH_XHCI,
                      PCI_FUNCTION_NUMBER_PCH_XHCI
                      );

  //
  // XHCI Static Power Gate Enable/Disable
  //
  if (XhciIpIsXhciPresent()) {
#ifdef FSP_FLAG
    DEBUG ((DEBUG_INFO, "XHCI is present\n"));

    if (UsbConfig->XhciEnable == 0) {

      MmioOr32 (
        XhciPciMmBase + R_PCH_XHCI_PWR_CNTL_STS,
        (UINT32) V_PCH_XHCI_PWR_CNTL_STS_PWR_STS_D3
        );

      MmioOr32 (
        XhciPciMmBase + R_PCH_XHCI_FC,
        (UINT32) (BIT0)
        );
      ///
      /// Disable xHCI function.
      /// Default value=0, When set, transactions targeting this PCI function
      /// will not be positively decoded.
      ///
      FspData32And = 0xFFFFFFFF;
      FspData32Or  = B_PCH_PCR_PSFX_T0_SHDW_PCIEN_FUNDIS;
      PchPcrAndThenOr32 (
        PID_PSF2, R_PCH_PCR_PSF2_T0_SHDW_XHCI_REG_BASE + R_PCH_PCR_PSFX_T0_SHDW_PCIEN,
        FspData32And,
        FspData32Or
        );

      Status = PmcStPgDisableFunction(PMC_ST_PG_XHCI);
      DEBUG ((DEBUG_INFO, "Disable XHCI device!!)\n"));
      ASSERT_EFI_ERROR (Status);

      DEBUG ((DEBUG_INFO, "xHCI: xHCI controller disabled\n"));

      return EFI_SUCCESS;
    }
#endif //FSP_FLAG
  }else{
    DEBUG ((DEBUG_INFO, "xHCI: xHCI controller not pressent\n"));
    return EFI_SUCCESS;
  }

  PchSeries     = GetPchSeries();



 //  1403979702 WA: USB.PORTPMSC.HLE = 0
  if (PchStep == PchHA0) {
    MmioAnd32 (XhciMmioBase + R_PCH_H_XHCI_PORTSC01USB2,~(UINT32) (B_PCH_H_XHCI_PORTSCXUSB2_HLE));
    MmioAnd32 (XhciMmioBase + R_PCH_H_XHCI_PORTSC02USB2,~(UINT32) (B_PCH_H_XHCI_PORTSCXUSB2_HLE));
    MmioAnd32 (XhciMmioBase + R_PCH_H_XHCI_PORTSC03USB2,~(UINT32) (B_PCH_H_XHCI_PORTSCXUSB2_HLE));
    MmioAnd32 (XhciMmioBase + R_PCH_H_XHCI_PORTSC04USB2,~(UINT32) (B_PCH_H_XHCI_PORTSCXUSB2_HLE));
  }


  PchAcpiBaseGet (&ABase);

  ///
  /// Assign memory resources
  ///
  XhciMemorySpaceOpen (
    UsbConfig,
    XhciMmioBase,
    XhciPciMmBase
    );

  ///
  /// Disable Compliance Mode
  /// It must be done before clock gating is configured.
  ///
  if (UsbConfig->DisableComplianceMode == TRUE){
    MmioOr32(
      XhciMmioBase + R_PCH_XHCI_HOST_CTRL_PORT_LINK_REG,
      (UINT32)(BIT0)
      );
  }

  ///
  /// Program Xhci Port Disable Override.
  ///
  XhciPortDisableOverride (
    UsbConfig,
    XhciMmioBase
    );

  XhciHcInit (
    PchPolicy,
    XhciMmioBase,
    XhciPciMmBase
    );

    ///
    /// Setup USB Over-Current Mapping.
    ///
    XhciOverCurrentMapping (
      UsbConfig,
      XhciPciMmBase
      );
  ///
  /// Clear memory resources
  ///
  XhciMemorySpaceClose (
    UsbConfig,
    XhciMmioBase,
    XhciPciMmBase
    );


  DEBUG ((EFI_D_INFO, __FUNCTION__" : Exit\n"));

  return EFI_SUCCESS;
}

/**
  Performs basic configuration of PCH USB3 (xHCI) controller.

  @param[in] PchPolicy            The PCH Policy PPI
  @param[in] XhciMmioBase         Memory base address of xHCI Controller
  @param[in] XhciPciMmBase        XHCI PCI Base Address

  @retval None
**/
VOID
XhciHcInit (
  IN  PCH_POLICY_PPI              *PchPolicy,
  IN  UINT32                      XhciMmioBase,
  IN  UINTN                       XhciPciMmBase
  )
{
  EFI_STATUS      Status;
  UINT32          Data32;
  UINT32          Data32Or;
  UINT32          Data32And;
  PCH_SERIES      PchSeries;
  PCH_STEPPING    PchStep;
  UINT32          HsPortCount;
  UINT32          SsPortCount;
  UINTN           PortSCxUsb3Base;
  EFI_BOOT_MODE   BootMode;
  BOOLEAN         DebugEnable;
  UINT32          DebugPort;
  UINT32          DebugPortSsIndex;
  UINT32          CapabilityPointer;
  UINT32          Capability;
  UINT32          UsbPort;
  UINT32          XhciUsb3Pdo;
  UINT32          Xhcc1;

  PchSeries   = GetPchSeries ();
  PchStep     = PchStepping ();
  HsPortCount = 0;
  SsPortCount = 0;

  DEBUG ((DEBUG_INFO, "Before XhciHcInit()\n"));
  DEBUG ((DEBUG_INFO, "POSTCODE << %x >>\n", PCH_HSIO_BEFORE_USB ));
  PostCode( PCH_HSIO_BEFORE_USB );
  DEBUG ((DEBUG_INFO, "Xhci Mmio Base = 0x%x\n", XhciMmioBase));
  DEBUG ((DEBUG_INFO, "Xhci Mmio Base + 0x0000 (Register 0x0000 to 0x001F) = \n"));
  PrintRegisters (XhciMmioBase + 0x0000, 0x20, 0);
  DEBUG ((DEBUG_INFO, "Xhci Mmio Base + 0x0080 (Register 0x0080 to 0x00BF) = \n"));
  PrintRegisters (XhciMmioBase + 0x0080, 0x40, 0x0080);
  DEBUG ((DEBUG_INFO, "Xhci Mmio Base + 0x0480 (Register 0x0480 to 0x05CF) = \n"));
  PrintRegisters (XhciMmioBase + 0x0480, 0x150,0x0480);
  DEBUG ((DEBUG_INFO, "Xhci Mmio Base + 0x8000 (Register 0x8000 to 0x833F) = \n"));
  PrintRegisters (XhciMmioBase + 0x8000, 0x340,0x8000);

  HsPortCount = (MmioRead32 (XhciMmioBase + R_PCH_XHCI_XECP_SUPP_USB2_2) & 0x0000FF00) >> 8;
  SsPortCount = (MmioRead32 (XhciMmioBase + R_PCH_XHCI_XECP_SUPP_USB3_2) & 0x0000FF00) >> 8;

  DEBUG ((DEBUG_INFO, "Max number of Super Speed Ports  = %d\n", SsPortCount));
  DEBUG ((DEBUG_INFO, "Max number of High Speed Ports   = %d\n", HsPortCount));

  //
  //  XHCC1 - XHC System Bus Configuration 1
  //  Address Offset: 40-43h
  //  Value: [21:19] 110b, [18] 1b, [8] 1b
  //  Writes to this registers needs to be performed per bytes to avoid touching bit 31
  //  Bit 31 is used to lock RW/L bits and can be writen once.
  //
  Xhcc1 = ((UINT32)PchPolicy->UsbConfig.IIL1e << B_IIL1E_BIT_OFFSET) & B_PCH_XHCI_XHCC1_IIL1E;
  if (PchPolicy->UsbConfig.XhciL1e) {
    Xhcc1 |= B_PCH_XHCI_XHCC1_XHCIL1E;
  } else {
	Xhcc1 &= (UINT32)~B_PCH_XHCI_XHCC1_XHCIL1E;
  }
  MmioAndThenOr32 (XhciPciMmBase + R_PCH_XHCI_XHCC1, 0xFFC3FFFF, Xhcc1);
  //Begin: USB3-WM Handshake WA for A0/A1 (HSD 1404757406). Fixed in B0
  //
  //[10:8] Need to be 0 so there is no count-down to avoid the pipe clkreq stalling the power gate entry
  //
  if ( PchStepping() == PchHA0 || PchStepping() == PchHA1 ) {
    MmioAnd32 (XhciPciMmBase + R_PCH_XHCI_XHCC1, (UINT32)~B_PCH_XHCI_XHCC1_L23HRAWC);
  }
  //End: USB3-WM Handshake WA for A0/A1 (HSD 1404757406). Fixed in B0
  DEBUG ((EFI_D_INFO, "\tXHCC1 = 0x%08x\tValue = 0x%08x\n",XhciPciMmBase + R_PCH_XHCI_XHCC1,MmioRead32 (XhciPciMmBase + R_PCH_XHCI_XHCC1)));

  //
  //  XHCC2 - XHC System Bus Configuration 2
  //  Address Offset: 44-47h
  //  Value: [19:14] 111111b, [10] 1b, [9:8] 10b, [7:6] 10b, [5:3] 001b, [2:0] 000b
  //
  MmioOr32 (
    XhciPciMmBase + R_PCH_XHCI_XHCC2,
    (UINT32) ((7<<N_PCH_XHCI_XHCC2_IDMA_RDREQSZCTRL) | (7<<N_PCH_XHCI_XHCC2_RDREQSZCTRL) |
             BIT25| BIT19 | BIT18 | BIT17 | BIT16 | BIT15 | BIT14 | BIT10 | BIT9 | BIT7 | BIT3)
    );
  DEBUG ((EFI_D_INFO, "\tXHCC2 = 0x%08x\tValue = 0x%08x\n",XhciPciMmBase + R_PCH_XHCI_XHCC2,MmioRead32 (XhciPciMmBase + R_PCH_XHCI_XHCC2)));

  //
  //  PCE - Power Control Enables
  //  Address Offset:   A2h - A3h
  //  Value: [3] 0b, [2] 1b,
  //
#ifdef PCH_PO_FLAG
  if (PchPolicy->PwrOnSafeMode.Bit.XhciSafeRegister == FALSE) {
#endif
  MmioAndThenOr16 (
    XhciPciMmBase + R_PCH_XHCI_PCE,
    (UINT16)~(BIT3 | BIT2),
    (UINT16) (BIT1)
    );
#ifdef PCH_PO_FLAG
  }
#endif
  DEBUG ((EFI_D_INFO, "\tPCE_REG = 0x%08x\tValue = 0x%08x\n",XhciPciMmBase + R_PCH_XHCI_PCE,(UINT16)MmioRead16 ( XhciPciMmBase + R_PCH_XHCI_PCE ) ));

  //
  //  XHCLKGTEN - Clock Gating
  //  Address Offset: 50-53h
  //  Value: 0x07CE6E5F
  //
#ifdef PCH_PO_FLAG
  if (PchPolicy->PwrOnSafeMode.Bit.XhciSafeRegister == FALSE) {
#endif
  Data32 = 0x07CE6E5F & ~BIT2;
  MmioWrite32 (
    XhciPciMmBase + R_PCH_XHCI_XHCLKGTEN,
    Data32
    );
  DEBUG ((EFI_D_INFO, "\tXHCLKGTEN = 0x%08x\tValue = 0x%08x\n",(XhciPciMmBase + R_PCH_XHCI_XHCLKGTEN),MmioRead32 (XhciPciMmBase + R_PCH_XHCI_XHCLKGTEN)));
#ifdef PCH_PO_FLAG
  }
#endif


  //
  // Sx WA
  //

  //
  // Put XHCI in D3
  //
  MmioOr16 (XhciPciMmBase + R_PCH_XHCI_PWR_CNTL_STS, (UINT16) V_PCH_XHCI_PWR_CNTL_STS_PWR_STS_D3);

  //
  // Delay of 20us (this to allow xHCI power gating.
  //
  MicroSecondDelay (20);

  //
  // Put XHCI in D0
  //
  MmioAnd16 (XhciPciMmBase + R_PCH_XHCI_PWR_CNTL_STS, (UINT16) ~(V_PCH_XHCI_PWR_CNTL_STS_PWR_STS_D3));

  ///
  /// The value in this field reflects the highest numbered port
  /// in the controller,not the actual count of the number of ports.
  /// This allows for gaps in the port numbering,
  /// between USB2 and USB3 protocol capabilities.
  ///
  Data32Or = (((MmioRead32 (XhciMmioBase + R_PCH_XHCI_XECP_SUPP_USB3_2) & 0x000000FF) + SsPortCount) - 1) << 24;
  MmioAndThenOr32 (
    (XhciMmioBase + R_PCH_XHCI_HCSPARAMS1),
    (UINT32) 0x00FFFFFF,
    (UINT32) Data32Or
    );

  //
  //  HSCFG1 - High Speed Configuration 1
  //  Address Offset: AC-AFh
  //  Value: [18] 1b
  //
#ifdef PCH_PO_FLAG
  if (PchPolicy->PwrOnSafeMode.Bit.XhciSafeRegister == FALSE) {
#endif
  MmioOr32 (
    XhciPciMmBase + R_PCH_XHCI_HSCFG1,
    (UINT32) (BIT18)
    );
#ifdef PCH_PO_FLAG
  }
#endif

  //
  //  HSCFG2 - High Speed Configuration 2
  //  Address Offset: A4-A7h
  //  [13] 0b
  //  [12:11] 11b
  //
  MmioAndThenOr32 (
    XhciPciMmBase + R_PCH_XHCI_HSCFG2,
    (UINT32)~(BIT13),
    (UINT32) (BIT12 | BIT11)
    );

  //
  //  SSCFG1 - SuperSpeed Configuration 1
  //  Address Offset: A8 - ABh
  //  Value: [17] 1b, [14] 1b
  //
#ifdef PCH_PO_FLAG
  if (PchPolicy->PwrOnSafeMode.Bit.XhciSafeRegister == FALSE) {
#endif
  MmioOr32 (
    XhciPciMmBase + R_PCH_XHCI_SSCFG1,
    (UINT32) (BIT17 | BIT14)
    );
#ifdef PCH_PO_FLAG
  }
#endif

  //
  //  MMIO Space
  //

  //
  //  HCSPARAMS3 - Structural Parameters 3
  //  Address Offset:   0x0C - 0x0F
  //  Set xHCIBAR + 0Ch[7:0] = 0Ah and [31:16] = 200h
  //
  MmioAndThenOr32 (
    (XhciMmioBase + R_PCH_XHCI_HCSPARAMS3),
    (UINT32) 0x0000FF00,
    (UINT32) 0x0200000A
    );

 //  Value: [11] 0b for A stepping
  if (PchStep == PchHA0) {
    MmioAnd32 (
      XhciMmioBase + R_PCH_XHCI_HCCPARAMS,
      ~(UINT32) (BIT11)
      );
  }

  //
  //  XECP_SUPP_USB2_2 - USB2 Supported Protocol Capability 2
  //  Address Offset: 0x8008 - 0x800B
  //  Value: [19] 0b
  //
  MmioAnd32 (
    XhciMmioBase + R_PCH_XHCI_XECP_SUPP_USB2_2,
    (UINT32)~(BIT19)
    );

  //
  //  PMCTRL - Power Management Control
  //  Address Offset: 80A4-80A7h
  //  Value: [30] 1b, [27] 1b [24] 1b, [23] 1b, [16] 0b, [15:8] 0x50, [7:4] 1001b, [3] 1b, [2] 1b
  //
#ifdef PCH_PO_FLAG
  if (PchPolicy->PwrOnSafeMode.Bit.XhciSafeRegister == FALSE) {
#endif
  MmioAndThenOr32 (
    (XhciMmioBase + R_PCH_XHCI_PMCTRL),
    (UINT32)~(BIT16 | BIT15 | BIT13 | BIT11 | BIT10 | BIT9  | BIT8),
    (UINT32) (BIT30 | BIT27 | BIT24 | BIT23 | BIT14 | BIT12 | BIT7 | BIT4 | BIT3 | BIT2)
    );
  DEBUG ((EFI_D_INFO, "\tPMCTRL = 0x%08x\tValue = 0x%08x\n",XhciMmioBase + R_PCH_XHCI_PMCTRL,MmioRead32 (XhciMmioBase + R_PCH_XHCI_PMCTRL)));
#ifdef PCH_PO_FLAG
  }
#endif

  DEBUG ((EFI_D_INFO, "\tPM_CS = 0x%08x\tValue = 0x%08x\n",XhciPciMmBase + R_PCH_XHCI_PWR_CNTL_STS,MmioRead32 (XhciPciMmBase + R_PCH_XHCI_PWR_CNTL_STS)));

  //Begin: USB3-WM Handshake WA for A0/A1 (HSD 1404757406). Fixed in B0
  //
  //  Host Controller Misc Reg 2
  //  Address Offset: 0x80B4 - 0x80B7
  //  Value: [2] 1b for Ax/B0 steppings
  //
  if ( PchStepping() == PchHA0 || PchStepping() == PchHA1 ) {
    MmioOr32 (
      (XhciMmioBase + R_PCH_XHCI_HOST_CTRL_MISC_REG_2),
      (UINT32) (BIT2)
      );
  }
  DEBUG ((EFI_D_INFO, "\tHOST_CTRL_MISC_REG2 = 0x%08x\tValue = 0x%08x\n",XhciMmioBase + R_PCH_XHCI_HOST_CTRL_MISC_REG_2,MmioRead32 (XhciMmioBase + R_PCH_XHCI_HOST_CTRL_MISC_REG_2)));
  //
  //  PGCBCTRL - PGCB Control
  //  Address Offset: 80A8-80ABh
  //  Value: [27] 1b, [26] 1b, [24] 1b
  //
  if ( PchStepping() != PchHA0 && PchStepping() != PchHA1 ) {
#ifdef PCH_PO_FLAG
    if (PchPolicy->PwrOnSafeMode.Bit.XhciSafeRegister == FALSE) {
#endif
      MmioOr32 (
        (XhciMmioBase + R_PCH_XHCI_PGCBCTRL),
        (UINT32) (BIT27 | BIT26 | BIT24)
        );
#ifdef PCH_PO_FLAG
    }
#endif
  }
  DEBUG ((EFI_D_INFO, "\tPGCBCTRL_REG = 0x%08x\tValue = 0x%08x\n",XhciMmioBase + R_PCH_XHCI_PGCBCTRL,MmioRead32 (XhciMmioBase + R_PCH_XHCI_PGCBCTRL)));
  //End: USB3-WM Handshake WA for A0/A1 (HSD 1404757406). Fixed in B0

  //
  //  AUX_CTRL_REG1 - AUX Power Management Control
  //  Address Offset: 80E0-80E3h
  //  Value: [22] 0b, [16] 0b, [9] 0b, [6] 1b
  //
#ifdef PCH_PO_FLAG
  if (PchPolicy->PwrOnSafeMode.Bit.XhciSafeRegister == FALSE) {
#endif
  //Begin: USB3 S4/S5 WA for A0/A1 (HSD 1404839189). Fixed in B0
  if ( PchStepping() == PchHA0 || PchStepping() == PchHA1 ) {
    MmioAndThenOr32 (
      (XhciMmioBase + R_PCH_XHCI_AUX_CTRL_REG1),
      (UINT32)~(BIT22 | BIT16 | BIT9),
      (UINT32) (BIT27 | BIT6)
      );
  } else {
  MmioAndThenOr32 (
    (XhciMmioBase + R_PCH_XHCI_AUX_CTRL_REG1),
    (UINT32)~(BIT22 | BIT16 | BIT9),
    (UINT32) (BIT6)
    );
  }
  DEBUG ((EFI_D_INFO, "\tAUX_CTRL_REG1 = 0x%08x\tValue = 0x%08x\n",XhciMmioBase + R_PCH_XHCI_AUX_CTRL_REG1,MmioRead32 (XhciMmioBase + R_PCH_XHCI_AUX_CTRL_REG1)));
  //End: USB3 S4/S5 WA for A0/A1 (HSD 1404839189). Fixed in B0
#ifdef PCH_PO_FLAG
  }
#endif

  //
  //  USB2_LINK_MGR_CTRL_REG1 - USB2 Port Link Control 1, 2, 3, 4
  //  Address Offset: 0x80F0 - 0x80FF [121] 1b, [20] 0b
  //  [127:96] is mapped to DW4 at offset 80FCh-80FFh [25] 1b
  //  [31:0]   is mapped to DW1 at offset 80F0h-80F3h [20] 0b
  //
#ifdef PCH_PO_FLAG
  if (PchPolicy->PwrOnSafeMode.Bit.XhciSafeRegister == FALSE) {
#endif
  MmioOr32 (
    XhciMmioBase + R_PCH_XHCI_USB2_LINK_MGR_CTRL_REG1_DW4,
    (UINT32) (BIT25)
    );
#ifdef PCH_PO_FLAG
  }
#endif

  MmioAnd32 (
    XhciMmioBase + R_PCH_XHCI_USB2_LINK_MGR_CTRL_REG1_DW1,
    (UINT32)~(BIT20)
    );

  //
  //  AUX_CTRL_REG2 - Aux PM Control Register 2
  //  Address Offset: 8154-8157h
  //  Value: [21] 0b, [13] 1b
  //
#ifdef PCH_PO_FLAG
  if (PchPolicy->PwrOnSafeMode.Bit.XhciSafeRegister == FALSE) {
#endif
  MmioAndThenOr32 (
    (XhciMmioBase + R_PCH_XHCI_AUX_CTRL_REG2),
    (UINT32)~(BIT21),
    (UINT32) (BIT13)
    );
#ifdef PCH_PO_FLAG
  }
#endif

  //
  //  xHCI Aux Clock Control Register
  //  Address Offset: 0x816C - 0x816F
  //  [13] 0b, [12] 0b, [11] 0b, [10] 0b, [9] 0b, [8] 0b
  //  [19] 1b, [18] 1b, [17] 1b, [14] 1b, [4] 1b, [3] 1b, [2] 1b,
  //
#ifdef PCH_PO_FLAG
  if (PchPolicy->PwrOnSafeMode.Bit.XhciSafeRegister == FALSE) {
#endif
  Data32And = (UINT32) ~(BIT13 | BIT12 | BIT11 | BIT10 | BIT9 | BIT8);
  Data32Or  = (UINT32) (BIT19 | BIT18 | BIT17 | BIT16 | BIT14 | BIT4 | BIT3 | BIT2);

  Data32And &= (UINT32) ~(BIT19 | BIT18);
  Data32Or  &= (UINT32) ~(BIT19 | BIT18);
  MmioAndThenOr32 (
    (XhciMmioBase + R_PCH_XHCI_AUXCLKCTL),
    Data32And,
    Data32Or
    );
  DEBUG ((EFI_D_INFO, "\tXHCI_AUX_CCR = 0x%08x\tValue = 0x%08x\n",XhciMmioBase + R_PCH_XHCI_AUXCLKCTL,MmioRead32 (XhciMmioBase + R_PCH_XHCI_AUXCLKCTL)));
#ifdef PCH_PO_FLAG
  }
#endif

  //
  //  HOST_IF_PWR_CTRL_REG0 - Power Scheduler Control 0
  //  Address Offset: 8140-8143h
  //  Value: [31:24] 0xFF, [11:0] 0x03C
  //         [23:12] 0x080 for SPT-LP A/B and SPT-H A/B/C steppings
  //         [23:12] 0x00F for all others steppings
  //
#ifdef PCH_PO_FLAG
  if (PchPolicy->PwrOnSafeMode.Bit.XhciSafeRegister == FALSE) {
#endif
  if (((PchStep <= PchLpB1) && (PchSeries == PchLp)) || ((PchStep <= PchHC0) && (PchSeries == PchH))) {
    Data32 = 0xFF08003C;
  } else {
    Data32 = 0xFF00F03C;
  }
  MmioWrite32 (
    XhciMmioBase + R_PCH_XHCI_HOST_IF_PWR_CTRL_REG0,
    Data32
    );
#ifdef PCH_PO_FLAG
  }
#endif
  //
  //  USB2PMCTRL - USB2 Power Management Control
  //  Address Offset:   0x81C4 - 0x81C7
  //  Value: [11] 1b, [8] 1b, [3:2] 0b, [1:0] 11b
  //
#ifdef PCH_PO_FLAG
  if (PchPolicy->PwrOnSafeMode.Bit.XhciSafeRegister == FALSE) {
#endif
  MmioAndThenOr32 (XhciMmioBase + R_PCH_XHCI_USB2PMCTRL,
    (UINT32) ~(BIT3 | BIT2 | BIT1 | BIT0),
    (UINT32) (BIT11 | BIT8)
    );
#ifdef PCH_PO_FLAG
  }
#endif

  //
  //  HOST_CTRL_SCH_REG - Host Control Scheduler
  //  Address Offset: 8094-8097h
  //  Value: [23] 1b, [22] 1b, [6] 1b
  //
  MmioOr32 (
    XhciMmioBase + R_PCH_XHCI_HOST_CTRL_SCH_REG,
    (UINT32) (BIT23 | BIT22 | BIT6)
    );

  //
  //  HOST_CTRL_IDMA_REG - Host Control IN DMA Register
  //  Address Offset: 809C-809Fh
  //  Value: [31] 0b
  //
  MmioAnd32 (
    XhciMmioBase + R_PCH_XHCI_HOST_CTRL_IDMA_REG,
    (UINT32)~(BIT31)
    );

  //
  //  Host Controller Misc Reg
  //  Address Offset: 0x80B0 - 0x80B3
  //  Value: [24] 0b, [23] 1b, [18:16] 000b
  //
  MmioAndThenOr32 (
    XhciMmioBase + R_PCH_XHCI_HOST_CTRL_MISC_REG,
    (UINT32)~(BIT24 | BIT18| BIT17 | BIT16),
    (UINT32) (BIT23)
    );

  //
  //  Host Controller Misc Reg 2
  //  Address Offset: 0x80B4 - 0x80B7
  //  Value: [2] 1b, [5] 1b for SPT-LP A/B step and SPT-H A/B/C steppings
  //         [2] 0b, [5] 0b for all others steppings
  //
  if (((PchStep <= PchLpB1) && (PchSeries == PchLp)) || ((PchStep <= PchHC0) && (PchSeries == PchH))) {
    MmioOr32 (XhciMmioBase + R_PCH_XHCI_HOST_CTRL_MISC_REG_2,
      (UINT32) (BIT5 | BIT2)
      );
  }

  //
  //  SSPE - Super Speed Port Enables
  //  Address Offset: 0x80B8 - 0x80BC
  //  Value: [30] 1b
  //
  MmioOr32 (XhciMmioBase + R_PCH_XHCI_SSPE,
    (UINT32) (BIT30)
    );

  //
  //  HOST_CTRL_PORT_LINK_REG - SuperSpeed Port Link Control
  //  Address Offset: 0x80EC - 0x80EF
  //  Value: [19] 1b, [17] 1b
  //
  MmioOr32 (
    XhciMmioBase + R_PCH_XHCI_HOST_CTRL_PORT_LINK_REG,
    (UINT32) (BIT19 | BIT17)
    );

  if (PchStep < PchHB0) {
	  MmioAnd32 (XhciMmioBase + R_PCH_XHCI_HOST_CTRL_PORT_LINK_REG,
			  	 (UINT32) (~BIT6));
  }

  //
  //  HOST_CTRL_TRM_REG2 - Host Controller Transfer Manager Control 2
  //  Address Offset: 8110-8113h
  //  Value: [2] 0b, [20] 1b, [11] 1b
  //
  MmioAndThenOr32 (
    (XhciMmioBase + R_PCH_XHCI_HOST_CTRL_TRM_REG2),
    (UINT32)~(BIT2),
    (UINT32) (BIT20 | BIT11)
    );

  //
  //  HOST_CTRL_BW_MAX_REG - USB Max Bandwidth Control 4
  //  Address Offset: 8128-812Fh
  //  Value: [47:36] 0x0FF
  //
  MmioAndThenOr64 (
    (XhciMmioBase + R_PCH_XHCI_HOST_CTRL_BW_MAX_REG),
    (UINT64)~(BIT47 | BIT46 | BIT45 | BIT44),
    (UINT64) (BIT43 | BIT42 | BIT41 | BIT40 | BIT39 | BIT38 | BIT37 | BIT36)
    );

  DEBUG ((EFI_D_INFO, "\tHOST_CTRL_BW_MAX_REG = 0x%08x\tValue = 0x%08x\n",(XhciMmioBase + R_PCH_XHCI_HOST_CTRL_BW_MAX_REG),MmioRead64 (XhciMmioBase + R_PCH_XHCI_HOST_CTRL_BW_MAX_REG)));

  //
  //  HOST_IF_PWR_CTRL_REG1 - Power Scheduler Control 1
  //  Address Offset: 8144-8147h
  //  Value: [8] 1b
  //
  MmioOr32 (XhciMmioBase + R_PCH_XHCI_HOST_IF_PWR_CTRL_REG1,
    (UINT32) (BIT8)
    );

  //
  //  USB2 PHY Power Management Control
  //  Address Offset: 8164-8167h
  //  Value: [7] 1b, [6] 1b, [5] 1b, [4] 1b, [3] 1b, [2] 1b,
  //
  MmioOr32 (XhciMmioBase + R_PCH_XHCI_USB2PHYPM,
    (UINT32) (BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2)
    );

  //
  //  USB_LPM_PARAM  - USB LPM Parameters
  //  Address Offset: 8170-8173h
  //  Value: [31:22] 0x32
  //
  MmioAndThenOr32 (
    (XhciMmioBase + R_PCH_XHCI_USB_LPM_PARAM),
    (UINT32)~(BIT31 | BIT30 | BIT29 | BIT28| BIT25 | BIT24 | BIT22 ),
    (UINT32) (BIT27 | BIT26 | BIT23)
    );
  DEBUG ((EFI_D_INFO, "\tUSB_LPM_PARAM = 0x%08x\tValue = 0x%08x\n",XhciMmioBase + R_PCH_XHCI_USB_LPM_PARAM,MmioRead32 (XhciMmioBase + R_PCH_XHCI_USB_LPM_PARAM)));

  //
  //  xHC Latency Tolerance Parameters - LTV Control
  //  Address Offset: 0x8174 - 0x8177
  //  Value: [24] 1b, [11:0] 0xC0A
  //
  MmioAndThenOr32 (XhciMmioBase + R_PCH_XHCI_XHCLTVCTL2,
    (UINT32)~0x00000FFF,
    0x01000C0A
    );

  //
  //  xHC Latency Tolerance Parameters - High Idle Time Control
  //  Address Offset: 0x817C - 0x817F
  //  Value: [28:16] 0x0332, [12:0] 0x00A3
  //
  MmioOr32 (XhciMmioBase + R_PCH_XHCI_LTVHIT,
    0x033200A3
    );

  //
  //  xHC Latency Tolerance Parameters - Medium Idle Time Control
  //  Address Offset: 0x8180 - 0x8183
  //  Value: [28:16] 0x00CB, [12:0] 0x0028
  //
  MmioOr32 (XhciMmioBase + R_PCH_XHCI_LTVMIT,
    0x00CB0028
    );

  //
  //  xHC Latency Tolerance Parameters - Low Idle Time Control
  //  Address Offset: 0x8184 - 0x8187
  //  Value: [28:16] 0x0064, [12:0] 0x001E
  //
  MmioOr32 (XhciMmioBase + R_PCH_XHCI_LTVLIT,
    0x0064001E
    );

  //
  //  XECP_CMDM_CTRL_REG1 - Command Manager Control 1
  //  Address Offset: 818C-818Fh
  //  Value: [8] 0b, [16] 1b
  //
  MmioAndThenOr32 (
    (XhciMmioBase + R_PCH_XHCI_XECP_CMDM_CTRL_REG1),
    (UINT32)~(BIT8),
    (UINT32) (BIT16)
    );

  //
  //  XECP_CMDM_CTRL_REG3 - Command Manager Control 3
  //  Address Offset: 8194-8197h
  //  Value: [25] 1b
  //
  MmioOr32 (
    XhciMmioBase + R_PCH_XHCI_XECP_CMDM_CTRL_REG3,
    (UINT32) (BIT25)
    );

  //
  //  THROTT - XHCI Throttle Control
  //  Address Offset: 819C-819Fh
  //  Value: [20] 1b, [16] 1b, [14] 1b, [13] 1b, [12] 1b, [11:8] 0x3, [7:4] 0x7, [3:0] 0xD
  //
  MmioAndThenOr32 (
    XhciMmioBase + R_PCH_XHCI_THROTT,
    (UINT32)~(BIT11 | BIT10 | BIT7 | BIT1),
    (UINT32) (BIT20 | BIT16 | BIT14 | BIT13 | BIT12 | BIT9 | BIT8 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT0)
    );

  //
  //  THROTT2 - XHCI Throttle Control2
  //  Address Offset: 81B4-81B7h
  //  Value: [9:0] 0x0
  //
  MmioAnd32 (
    XhciMmioBase + R_PCH_XHCI_THROTT2,
    (UINT32) ~(0x3FF)
    );

  //
  //  D0I2CTRL - D0I2 Control Register
  //  Address Offset: 81BC-81BFh
  //  Value: [31] 1b
  //
  MmioOr32 (
    XhciMmioBase + R_PCH_XHCI_D0I2CTRL,
    (UINT32) (BIT31)
    );

  //
  //  Force Disconnect upon DCE
  //  DBCCTL - DbC Control
  //  Address Offset: 0x8758 - 0x875C
  //  Value: [0] 1b
  //
  MmioOr32 (
    XhciMmioBase + R_PCH_XHCI_DBC_DBCCTL,
    (UINT32) (BIT0)
    );

  PortSCxUsb3Base = 0;

  switch (PchSeries) {
    case PchLp:
      PortSCxUsb3Base = R_PCH_LP_XHCI_PORTSC01USB3;
      break;

    case PchH:
      PortSCxUsb3Base = R_PCH_H_XHCI_PORTSC01USB3;
      break;

    default:
      ASSERT (FALSE);
      break;
  }

  //
  // Set 1 to enable Super Speed Ports terminations on enabled ports only (PDO = 0)
  // Required for Deep S3
  //
  if (PchSeries == PchLp) {
    XhciUsb3Pdo = MmioRead32 (XhciMmioBase + R_PCH_XHCI_USB3PDO) & B_PCH_XHCI_LP_USB3PDO_MASK;

    MmioAndThenOr32(
      XhciMmioBase + R_PCH_XHCI_SSPE,
      (UINT32) (~B_PCH_XHCI_LP_SSPE_MASK),
      (UINT32) ((~XhciUsb3Pdo) & B_PCH_XHCI_LP_SSPE_MASK)
      );
  } else {
    XhciUsb3Pdo = MmioRead32 (XhciMmioBase + R_PCH_XHCI_USB3PDO) & B_PCH_XHCI_H_USB3PDO_MASK;

    MmioAndThenOr32(
      XhciMmioBase + R_PCH_XHCI_SSPE,
      (UINT32) (~B_PCH_XHCI_H_SSPE_MASK),
      (UINT32) ((~XhciUsb3Pdo) & B_PCH_XHCI_H_SSPE_MASK)
      );
  }

  DEBUG ((EFI_D_INFO, "\tSSPE_REG = 0x%08x\tValue = 0x%08x\n",XhciMmioBase + R_PCH_XHCI_SSPE, MmioRead32(XhciMmioBase + R_PCH_XHCI_SSPE)));
  //
  // XHC Save/Restore function will be broken if reset during S3 resume.
  //
  Status = PeiServicesGetBootMode (&BootMode);
  if (!EFI_ERROR (Status)) {
    if (BootMode == BOOT_ON_S3_RESUME) {
      return;
    }
  }

  //
  // Perform WPR on USB3 port except for the port has DBC enabled.
  //
  //
  // Get debug enable status in order to skip some XHCI init which
  // may break XHCI debug
  //
  CapabilityPointer = (UINT32) (XhciMmioBase + (MmioRead32 (XhciMmioBase + R_PCH_XHCI_HCCPARAMS) >> 16) * 4);
  DebugEnable = FALSE;
  DebugPortSsIndex = 0xFFFFFFFF;
  Capability = MmioRead32 (CapabilityPointer);
  DEBUG ((DEBUG_ERROR, "PchUsbCommon XHCI Capability Pointer = 0x%x\n", CapabilityPointer));
  DEBUG ((DEBUG_INFO, "POSTCODE << %x >>\n", PCH_HSIO_AFTER_USB ));
  PostCode( PCH_HSIO_AFTER_USB );

  while (TRUE) {
    if ((Capability & 0xFF) == 0xA) {
      //
      // Check DCR bit in DCCTRL register (Debug Capability Base + 20h), if set, debug device is running
      //
      if ((MmioRead32 (CapabilityPointer + 0x20) & BIT0) != 0) {
        DebugEnable = TRUE;
        //
        // Get debug port number [24:31] in DCST register which starts from 1
        //
        DebugPort = (MmioRead32 (CapabilityPointer + 0x24) >> 24) - 1;
        for (UsbPort = 0; UsbPort < SsPortCount; UsbPort++) {
          if ((R_PCH_LP_XHCI_PORTSC01USB2 + DebugPort * 0x10) == (PortSCxUsb3Base + (UsbPort * 0x10))) {
            DebugPortSsIndex = UsbPort;
            DEBUG ((DEBUG_ERROR, "PchUsbCommon DebugPortSsIndex = ?%d\n", DebugPortSsIndex));
            break;
          }
        }
      }
      break;
    }
    if ((((Capability & 0xFF00) >> 8) & 0xFF) == 0) {
      //
      // Reach the end of list, quit
      //
      break;
    }
    CapabilityPointer += ((Capability & 0xFF00) >> 8) * 4;
    Capability = MmioRead32 (CapabilityPointer);
  }

  //
  // Initiate warm reset to all USB3 ports except for the USB3 port which has Dbc enabled
  //
  for (UsbPort = 0; UsbPort < SsPortCount; UsbPort++) {
    if ((DebugEnable) && (UsbPort == DebugPortSsIndex)) {
      continue;
    }
    MmioAndThenOr32 (
    XhciMmioBase + (PortSCxUsb3Base + (UsbPort * 0x10)),
      (UINT32)~ (B_PCH_XHCI_PORTSCXUSB3_PED),
      B_PCH_XHCI_PORTSCXUSB3_WPR
      );
  }
}


/**
  Program Xhci Port Disable Override

  @param[in] UsbConfig            The PCH Policy for USB configuration
  @param[in] XhciPciMmBase        XHCI PCI Base Address

  @retval None
**/
VOID
XhciPortDisableOverride (
  IN  CONST PCH_USB_CONFIG        *UsbConfig,
  IN  UINTN                       XhciMmioBase
  )
{
  UINT32          Index;
  UINT32          XhciSspe;
  UINT32          XhciUsb2Pdo;
  UINT32          XhciUsb3Pdo;
  UINT32          XhciUsb2PdoRd;
  UINT32          XhciUsb3PdoRd;
  PCH_SERIES      PchSeries;
  PCH_RESET_PPI   *PchResetPpi;

  PchSeries = GetPchSeries();

  ///
  /// XHCI PDO for HS
  ///
  if (PchSeries == PchLp) {
    XhciUsb2Pdo = MmioRead32 (XhciMmioBase + R_PCH_XHCI_USB2PDO) & B_PCH_XHCI_LP_USB2PDO_MASK;
  } else {
    XhciUsb2Pdo = MmioRead32 (XhciMmioBase + R_PCH_XHCI_USB2PDO) & B_PCH_XHCI_H_USB2PDO_MASK;
  }
  for (Index = 0; Index < GetPchUsbMaxPhysicalPortNum (); Index++) {
    if (UsbConfig->PortUsb20[Index].Enable == TRUE) {
      XhciUsb2Pdo &= (UINT32)~(B_PCH_XHCI_USB2PDO_DIS_PORT0 << Index);
    } else {
      XhciUsb2Pdo |= (UINT32) (B_PCH_XHCI_USB2PDO_DIS_PORT0 << Index); ///< A '1' in a bit position prevents the corresponding USB2 port from reporting a Device Connection to the XHC
    }
  }
  ///
  /// XHCI PDO for SS
  ///
  if (PchSeries == PchLp) {
    XhciUsb3Pdo = MmioRead32 (XhciMmioBase + R_PCH_XHCI_USB3PDO) & B_PCH_XHCI_LP_USB3PDO_MASK;
    XhciSspe = MmioRead32 (XhciMmioBase + R_PCH_XHCI_SSPE) & B_PCH_XHCI_LP_SSPE_MASK;
  } else {
    XhciUsb3Pdo = MmioRead32 (XhciMmioBase + R_PCH_XHCI_USB3PDO) & B_PCH_XHCI_H_USB3PDO_MASK;
    XhciSspe = MmioRead32 (XhciMmioBase + R_PCH_XHCI_SSPE) & B_PCH_XHCI_H_SSPE_MASK;
  }

  for (Index = 0; Index < GetPchXhciMaxUsb3PortNum (); Index++) {
    if (UsbConfig->PortUsb30[Index].Enable == TRUE) {
      XhciUsb3Pdo &= (UINT32)~(B_PCH_XHCI_USB3PDO_DIS_PORT0 << Index);
    } else {
      XhciUsb3Pdo |= (UINT32) (B_PCH_XHCI_USB3PDO_DIS_PORT0 << Index); ///< A '1' in a bit position prevents the corresponding USB3 port from reporting a Device Connection to the XHC
    }
  }
  ///
  /// USB2PDO and USB3PDO are Write-Once registers and bits in them are in the SUS Well.
  ///
  MmioWrite32 (XhciMmioBase + R_PCH_XHCI_USB2PDO, XhciUsb2Pdo);
  MmioWrite32 (XhciMmioBase + R_PCH_XHCI_USB3PDO, XhciUsb3Pdo);
  DEBUG ((EFI_D_INFO, "\tUSB2PDO = 0x%08x\tValue = 0x%08x\n",XhciMmioBase + R_PCH_XHCI_USB2PDO,MmioRead32 (XhciMmioBase + R_PCH_XHCI_USB2PDO)));
  DEBUG ((EFI_D_INFO, "\tUSB3PDO = 0x%08x\tValue = 0x%08x\n",XhciMmioBase + R_PCH_XHCI_USB3PDO,MmioRead32 (XhciMmioBase + R_PCH_XHCI_USB3PDO)));

  XhciUsb2PdoRd = MmioRead32 (XhciMmioBase + R_PCH_XHCI_USB2PDO);
  XhciUsb3PdoRd = MmioRead32 (XhciMmioBase + R_PCH_XHCI_USB3PDO);

  ///
  /// If USB2PDO and USB3PDO are not updated successfully perform Warm Reset to unlock RWO bits.
  ///
  if ((XhciUsb2Pdo != XhciUsb2PdoRd) || (XhciUsb3Pdo != XhciUsb3PdoRd)) {
    DEBUG ((DEBUG_ERROR, "xHCI: EfiResetWarm\n"));

    PeiServicesLocatePpi (
      &gPchResetPpiGuid,
      0,
      NULL,
      (VOID **)&PchResetPpi
      );

    PchResetPpi->Reset (PchResetPpi, WarmReset);
  }
}

/**
  Program and enable XHCI Memory Space

  @param[in] UsbConfig            The PCH Policy for USB configuration
  @param[in] XhciMmioBase         Memory base address of XHCI Controller
  @param[in] XhciPciMmBase        XHCI PCI Base Address

**/
VOID
XhciMemorySpaceOpen (
  IN  CONST PCH_USB_CONFIG        *UsbConfig,
  IN  UINT32                      XhciMmioBase,
  IN  UINTN                       XhciPciMmBase
  )
{
  ///
  /// Assign memory resources
  ///
  MmioWrite32 (XhciPciMmBase + R_PCH_XHCI_MEM_BASE, XhciMmioBase);

  MmioOr16 (
    XhciPciMmBase + PCI_COMMAND_OFFSET,
    (UINT16) (EFI_PCI_COMMAND_MEMORY_SPACE | EFI_PCI_COMMAND_BUS_MASTER)
    );

}

/**
  Clear and disable XHCI Memory Space

  @param[in] UsbConfig            The PCH Policy for USB configuration
  @param[in] XhciMmioBase         Memory base address of XHCI Controller
  @param[in] XhciPciMmBase        XHCI PCI Base Address

  @retval None
**/
VOID
XhciMemorySpaceClose (
  IN  CONST PCH_USB_CONFIG        *UsbConfig,
  IN  UINT32                      XhciMmioBase,
  IN  UINTN                       XhciPciMmBase
  )
{
  ///
  /// Clear memory resources
  ///
  MmioAnd16 (
    XhciPciMmBase + PCI_COMMAND_OFFSET,
    (UINT16)~(EFI_PCI_COMMAND_MEMORY_SPACE | EFI_PCI_COMMAND_BUS_MASTER)
    );

  MmioWrite32 ((XhciPciMmBase + R_PCH_XHCI_MEM_BASE), 0);

}

/**
  Lock USB registers before boot

  @param[in] UsbConfig      The PCH Policy for USB configuration
**/
VOID
UsbEndOfInit(
  IN  CONST PCH_USB_CONFIG    *UsbConfig
)
{
  UINTN                       XhciPciMmBase;
  UINT32    XhccCfg;

  DEBUG ((EFI_D_INFO, __FUNCTION__" : Entry\n"));
  if (!XhciIpIsXhciPresent() || !XhciIpIsXhciEnabled()) {
	  DEBUG ((DEBUG_INFO, "XHCI not present/enabled\n"));
	  return;
  }

  XhciPciMmBase = MmPciBase (
                      DEFAULT_PCI_BUS_NUMBER_PCH,
                      PCI_DEVICE_NUMBER_PCH_XHCI,
                      PCI_FUNCTION_NUMBER_PCH_XHCI
                      );

  ///
  /// PCH BIOS Spec Section 13.2.4 Locking xHCI Register Settings
  /// PCH BIOS Spec Locking xHCI Register settings
  /// After xHCI is initialized, BIOS should lock the xHCI configuration registers to RO.
  /// This prevent any unintended changes. There is also a lockdown feature for OverCurrent
  /// registers. BIOS should set these bits to lock down the settings prior to end of POST.
  /// 1. Set Access Control bit at XHCI PCI offset 40h[31] to 1b to lock xHCI register settings.
  /// 2. Set OC Configuration Done bit at XHCI PCI offset 44h[31] to lock overcurrent mappings from
  ///    further changes.
  ///
    MmioOr32 (XhciPciMmBase + R_PCH_XHCI_XHCC2, (UINT32) (BIT31));
    DEBUG ((EFI_D_INFO, "\tXHCC2 = 0x%08x\tValue = 0x%08x\n",XhciPciMmBase + R_PCH_XHCI_XHCC2,MmioRead32 (XhciPciMmBase + R_PCH_XHCI_XHCC2)));

  ///
  /// PCH BIOS Spec xHCI controller setup
  /// Note:
  /// XHCI PCI offset 40h is write once register.
  /// Unsupported Request Detected bit is write clear
  ///
  XhccCfg = MmioRead32 (XhciPciMmBase + R_PCH_XHCI_XHCC1);
  XhccCfg &= (UINT32) ~(B_PCH_XHCI_XHCC1_URD);
  XhccCfg |= (UINT32) (B_PCH_XHCI_XHCC1_ACCTRL);
  MmioWrite32 (XhciPciMmBase + R_PCH_XHCI_XHCC1, XhccCfg);

  DEBUG ((EFI_D_INFO, __FUNCTION__" : Exit\n"));
}

/**
  Configure PCH xHCI, post-mem phase

  @param[in]  PchPolicy           The PCH Policy PPI instance
  @param[in] TempMemBaseAddr      Temporary Memory Base Address for PCI
                                  devices to be used to initialize MMIO
                                  registers.

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_OUT_OF_RESOURCES    Do not have enough resources to initialize the driver
**/
EFI_STATUS
EFIAPI
ConfigureXhci (
  IN  PCH_POLICY_PPI            *PchPolicy,
  IN  UINT32                    TempMemBaseAddr
  )
{
  UINTN           XhciPciMmBase;
  PCH_SERIES      PchSeries;
  PCH_USB_CONFIG  *UsbConfig;
  UINT32          XhciMmioBase;


  if (!XhciIpIsXhciPresent()) {
	  DEBUG ((DEBUG_INFO, "XHCI not present\n"));
	  return EFI_SUCCESS;
  }
  if (!XhciIpIsXhciEnabled()) {
	  DEBUG ((DEBUG_INFO, "XHCI not enabled\n"));
	  return EFI_SUCCESS;
  }

  DEBUG ((DEBUG_INFO, "ConfigureXhci () - Start\n"));

  XhciMmioBase  = TempMemBaseAddr;
  UsbConfig     = &PchPolicy->UsbConfig;
  PchSeries     = GetPchSeries();

  XhciPciMmBase = MmPciBase (
                      0,
                      PCI_DEVICE_NUMBER_PCH_XHCI,
                      PCI_FUNCTION_NUMBER_PCH_XHCI
                      );

  //
  // Assign memory resources
  //
  MmioWrite32 (XhciPciMmBase + R_PCH_XHCI_MEM_BASE, XhciMmioBase);
  MmioOr16 (
    XhciPciMmBase + PCI_COMMAND_OFFSET,
    (UINT16) (EFI_PCI_COMMAND_MEMORY_SPACE | EFI_PCI_COMMAND_BUS_MASTER)
    );

  XhciInitPei (XhciMmioBase);

  //
  // Clear memory resources
  //
  MmioAnd16 (
    XhciPciMmBase + PCI_COMMAND_OFFSET,
    (UINT16)~(EFI_PCI_COMMAND_MEMORY_SPACE | EFI_PCI_COMMAND_BUS_MASTER)
    );
  MmioWrite32 ((XhciPciMmBase + R_PCH_XHCI_MEM_BASE), 0);

  DEBUG ((DEBUG_INFO, "ConfigureXhci () - End\n"));

  return EFI_SUCCESS;
}

/**
  Performs configuration of PCH USB3 (xHCI) controller.

  @param[in] XhciMmioBase        XHCI Mmio Base Address

  @retval None
**/
VOID
XhciInitPei (
  IN  UINT32    XhciMmioBase
  )
{
  PCH_SERIES      PchSeries;
  UINT32          SsPortCount;
  UINTN           PortSCxUsb3Base;
  UINT32          Data32;
  UINTN           Timeout;
  BOOLEAN         DebugEnable;
  UINT32          DebugPortSsIndex;
  UINT32          UsbPort;
  UINT32          DebugPort;
  UINT32          CapabilityPointer;
  UINT32          Capability;

  PortSCxUsb3Base = 0;
  PchSeries       = GetPchSeries();
  SsPortCount     = 0;
  SsPortCount     = (MmioRead32 (XhciMmioBase + R_PCH_XHCI_XECP_SUPP_USB3_2) & 0x0000FF00) >> 8;

  switch (PchSeries) {
    case PchLp:
      PortSCxUsb3Base = R_PCH_LP_XHCI_PORTSC01USB3;
      break;

    case PchH:
      PortSCxUsb3Base = R_PCH_H_XHCI_PORTSC01USB3;
      break;

    default:
      ASSERT (FALSE);
      break;
  }

  //
  // Perform WPR on USB3 port except for the port has DBC enabled.
  //
  //
  // Get debug enable status in order to skip some XHCI init which
  // may break XHCI debug
  //
  CapabilityPointer = (UINT32) (XhciMmioBase + (MmioRead32 (XhciMmioBase + R_PCH_XHCI_HCCPARAMS) >> 16) * 4);
  DebugEnable       = FALSE;
  DebugPortSsIndex  = 0xFFFFFFFF;
  Capability        = MmioRead32 (CapabilityPointer);

  DEBUG ((DEBUG_ERROR, "PchUsbPei XHCI Capability Pointer = 0x%x\n", CapabilityPointer));

  while (TRUE) {
    if ((Capability & 0xFF) == 0xA) {
      //
      // Check DCR bit in DCCTRL register (Debug Capability Base + 20h), if set, debug device is running
      //
      if ((MmioRead32 (CapabilityPointer + 0x20) & BIT0) != 0) {
        DebugEnable = TRUE;
        //
        // Get debug port number [24:31] in DCST register which starts from 1
        //
        DebugPort = (MmioRead32 (CapabilityPointer + 0x24) >> 24) - 1;
        for (UsbPort = 0; UsbPort < SsPortCount; UsbPort++) {
          if ((R_PCH_LP_XHCI_PORTSC01USB2 + DebugPort * 0x10) == (PortSCxUsb3Base + (UsbPort * 0x10))) {
            DebugPortSsIndex = UsbPort;
            DEBUG ((DEBUG_ERROR, "PchUsbCommon DebugPortSsIndex = ?%d\n", DebugPortSsIndex));
            break;
          }
        }
      }
      break;
    }
    if ((((Capability & 0xFF00) >> 8) & 0xFF) == 0) {
      //
      // Reach the end of list, quit
      //
      break;
    }
    CapabilityPointer += ((Capability & 0xFF00) >> 8) * 4;
    Capability = MmioRead32 (CapabilityPointer);
  }

  //
  // Poll for warm reset bit at steps #a to be cleared or timeout at 120ms
  //
  Timeout = 0;
  while (TRUE) {
    Data32 = 0;
    for (UsbPort = 0; UsbPort < SsPortCount; UsbPort++) {
      if ((DebugEnable) && (UsbPort == DebugPortSsIndex)) {
        continue;
      }
      Data32 |= MmioRead32 (XhciMmioBase + (PortSCxUsb3Base + (UsbPort * 0x10)));
    }
    if (((Data32 & B_PCH_XHCI_PORTSCXUSB3_PR) == 0) || (Timeout > PORT_RESET_TIMEOUT)) {
      break;
    }
    MicroSecondDelay (10);
    Timeout++;
  }

  //
  // Clear all the port's status by program xHCIBAR + 570h [23:17] to 1111111b.
  //
  for (UsbPort = 0; UsbPort < SsPortCount; UsbPort++) {
    if ((DebugEnable) && (UsbPort == DebugPortSsIndex)) {
      continue;
    }
    MmioAndThenOr32 (
      XhciMmioBase + (PortSCxUsb3Base + (UsbPort * 0x10)),
      (UINT32)~ (B_PCH_XHCI_PORTSCXUSB3_PED),
      B_PCH_XHCI_PORTSCXUSB3_CEC |
      B_PCH_XHCI_PORTSCXUSB3_PLC |
      B_PCH_XHCI_PORTSCXUSB3_PRC |
      B_PCH_XHCI_PORTSCXUSB3_OCC |
      B_PCH_XHCI_PORTSCXUSB3_WRC |
      B_PCH_XHCI_PORTSCXUSB3_PEC |
      B_PCH_XHCI_PORTSCXUSB3_CSC
      );
  }
}

/**
  Tune the USB 2.0 high-speed signals quality.

  @param[in] UsbConfig            The PCH Policy for USB configuration
  @param[in]  TempMemBaseAddr           The temporary memory base address for PMC device
  @param[in]  PmcBaseAddress            The PMC Base Address
**/
VOID
Usb2AfeProgramming (
  IN  CONST PCH_USB_CONFIG    *UsbConfig,
  IN  UINT32                  TempMemBaseAddr,
  IN  UINTN                   PmcBaseAddress
  )
{
  UINT8           Response;
  UINT32          Data32;
  UINT8           PpLane;
  UINT64          Offset;
  UINT8           MaxUsbPhyPortNum;
  PCH_SERIES      PchSeries;
  PCH_STEPPING    PchStep;
  UINT32          PchPwrmBase;

  if(!XhciIpIsXhciPresent())
   {
    DEBUG ((DEBUG_INFO, "xHCI: USB not pressent. Skiping AFE programing\n"));
    return;
   }

  DEBUG ((DEBUG_INFO, "xHCI: Usb2AfeProgramming Start\n"));

  PchSeries = GetPchSeries ();
  PchStep    = PchStepping ();
  PchPwrmBaseGet (&PchPwrmBase);
  //
  // USB2 PCR GLB ADP VBUS REG 0x402B[22:22] = 0x1
  // This register must access through Sideband interface.
  //
  PchSbiExecution (
    PID_USB2,
    R_PCH_PCR_USB2_GLB_ADP_VBUS_REG,
    PrivateControlRead,
  FALSE,
    &Data32,
    &Response
    );

  Data32 |= (BIT22);

  PchSbiExecution (
    PID_USB2,
    R_PCH_PCR_USB2_GLB_ADP_VBUS_REG,
    PrivateControlWrite,
    TRUE,
    &Data32,
    &Response
    );

  //
    // Set USB2 PLL feedback frequency to 96 Mhz
    // PLLDIVRATIOS_0 0xe000[31:24] = 0x14
    // This register must access through Sideband interface.
    //
    PchSbiExecution (
       PID_USB2,
       R_PCH_PCR_USB2_PLLDIVRATIOS_0,
       PrivateControlRead,
       FALSE,
       &Data32,
       &Response
       );

     Data32 &= 0x00FFFFFF;
     Data32 |= 0x14000000;

     PchSbiExecution (
       PID_USB2,
       R_PCH_PCR_USB2_PLLDIVRATIOS_0,
       PrivateControlWrite,
       TRUE,
       &Data32,
       &Response
       );


  //
  // USB2 GLOBAL PORT 0x4001[25:24] = 0x3
  // This register must access through Sideband interface.
  //
  PchSbiExecution (
    PID_USB2,
    R_PCH_PCR_USB2_GLOBAL_PORT,
    PrivateControlRead,
    FALSE,
    &Data32,
    &Response
    );

  //Enable the output HSIC clk480 to be dynamically gated and  enable USB PLL locking
  Data32 |= (BIT25 | BIT24);

  PchSbiExecution (
    PID_USB2,
    R_PCH_PCR_USB2_GLOBAL_PORT,
    PrivateControlWrite,
    TRUE,
    &Data32,
    &Response
    );



  //
  // USB2 PCR ADPLL 0x7034[31:0] = 0x50000000
  // This register must access through Sideband interface.
  //
  Data32 = 0x50000000;
  PchSbiExecution (
    PID_USB2,
    R_PCH_PCR_USB2_7034,
    PrivateControlWrite,
    TRUE,
    &Data32,
    &Response
    );

  //
  // USB2 PCR ADPLL 0x7038[31:0] = 0x0A70FCF6
  // This register must access through Sideband interface.
  //
  Data32 = 0x0A70FCF6;
  PchSbiExecution (
    PID_USB2,
    R_PCH_PCR_USB2_7038,
    PrivateControlWrite,
    TRUE,
    &Data32,
    &Response
    );

  //
  // USB2 PCR ADPLL 0x703C[31:0] = 0x1FBBC000
  // This register must access through Sideband interface.
  //
  Data32 = 0x1FBBC000;
  PchSbiExecution (
    PID_USB2,
    R_PCH_PCR_USB2_703C,
    PrivateControlWrite,
    TRUE,
    &Data32,
    &Response
    );

  //
  // Set USB2 COMPBG to 0x7F04 (Default Value 0x600)
  // This register must access through Sideband interface.
  //
  PchSbiExecution (
    PID_USB2,
    R_PCH_PCR_USB2_CFG_COMPBG,
    PrivateControlRead,
    FALSE,
    &Data32,
    &Response
    );
  Data32 &= ~(BIT14 | BIT13 | BIT9 | BIT8);
  Data32 |= (BIT15 | BIT10 | BIT7);
  PchSbiExecution (
    PID_USB2,
    R_PCH_PCR_USB2_CFG_COMPBG,
    PrivateControlWrite,
    TRUE,
    &Data32,
    &Response
    );

  MaxUsbPhyPortNum = GetPchUsbMaxPhysicalPortNum();
  for (PpLane = 1; PpLane <= MaxUsbPhyPortNum; PpLane++) {
    //
    // USB2 PER PORT
    // Address Offset: 0x0
    // Bit[14:8] according to recommendation for current board design
    // Bit[31:26] = 111111b
    //

    Offset = R_PCH_PCR_USB2_PP_LANE_BASE_ADDR | V_PCH_PCR_USB2_PER_PORT | (PpLane << 8);

    PchSbiExecution (
      PID_USB2,
      Offset,
      PrivateControlRead,
      FALSE,
      &Data32,
      &Response
      );

    Data32 &= ~(BIT14 | BIT13 | BIT12 | BIT11 | BIT10 | BIT9 | BIT8);
    Data32 |= (((UsbConfig->PortUsb20[PpLane - 1].Afe.Petxiset) & 0x7)  << 11) |
              (((UsbConfig->PortUsb20[PpLane - 1].Afe.Txiset) & 0x7)    << 8) |
              (((UsbConfig->PortUsb20[PpLane - 1].Afe.Pehalfbit) & 0x1) << 14);
    Data32 &= ~(BIT7 | BIT6 | BIT5 | BIT4);
    Data32 |= (BIT31 | BIT30 | BIT29 | BIT28 | BIT27 | BIT26 | BIT4);

    PchSbiExecution (
      PID_USB2,
      Offset,
      PrivateControlWrite,
      TRUE,
      &Data32,
      &Response
      );

    //
    // USB2 PER PORT 2
    // Address Offset: 0x26
    //
    Offset = R_PCH_PCR_USB2_PP_LANE_BASE_ADDR | V_PCH_PCR_USB2_PER_PORT_2 | (PpLane << 8);

    Data32 = 0;
    PchSbiExecution (
      PID_USB2,
      Offset,
      PrivateControlRead,
      FALSE,
      &Data32,
      &Response
      );

      Data32 &= ~(BIT24 | BIT23);
      Data32 |= (((UsbConfig->PortUsb20[PpLane - 1].Afe.Predeemp) & 0x3) << 23);

    PchSbiExecution (
      PID_USB2,
      Offset,
      PrivateControlWrite,
      TRUE,
      &Data32,
      &Response
      );

    //
    // UTMI MISC REG PER PORT
    // Address Offset: 0x08 (for each lane)
    // [13:12] 01b, [11] 1b [10:9] 01b, [7] 1b
    //
    Offset = R_PCH_PCR_USB2_PP_LANE_BASE_ADDR | V_PCH_PCR_USB2_UTMI_MISC_PER_PORT | (PpLane << 8);
    PchSbiExecution (
      PID_USB2,
      Offset,
      PrivateControlRead,
      FALSE,
      &Data32,
      &Response
      );

    Data32 &= ~(BIT13 | BIT10);
    Data32 |= (BIT12 | BIT11 | BIT9 | BIT7);

    PchSbiExecution (
      PID_USB2,
      Offset,
      PrivateControlWrite,
      TRUE,
      &Data32,
      &Response
      );
  }


  ///
  /// Please note: below programming must be done after USB2 GLOBAL PORT 2 programming
  /// PM_CFG (0x018h[29]) = 1, Allow 24MHz Crystal Oscillator Shutdown (ALLOW_24_OSC_SD)
  ///
    MmioOr32 (PchPwrmBase + R_PCH_PWRM_CFG, B_PCH_PWRM_CFG_ALLOW_24_OSC_SD);
  }

/**
  Setup XHCI Over-Current Mapping

@param[in] UsbConfig            The PCH Policy for USB configuration
  @param[in] XhciPciMmBase        XHCI PCI Base Address

  @retval None
**/
VOID
XhciOverCurrentMapping (
  IN  CONST PCH_USB_CONFIG        *UsbConfig,
  IN  UINTN                       XhciPciMmBase
  )
{
  ///
  ///  BIOS responsibility on Overcurrent protection.
  ///  ----------------------------------------------
  ///
  UINT32                   Index;
  PCH_GENERATION           PchGen;
  UINT32                   U2OCMBuf[V_PCH_XHCI_NUMBER_OF_OC_PINS];
  UINT32                   U3OCMBuf[V_PCH_XHCI_NUMBER_OF_OC_PINS];
  UINT32                   OCPin;
  DEBUG ((EFI_D_INFO, __FUNCTION__" : Entry\n"));
  ZeroMem (U2OCMBuf, sizeof (U2OCMBuf));
  ZeroMem (U3OCMBuf, sizeof (U3OCMBuf));

  PchGen    = GetPchGeneration();

  for (Index = 0; Index < GetPchXhciMaxUsb3PortNum (); Index++) {
    if (UsbConfig->PortUsb30[Index].OverCurrentPin == PchUsbOverCurrentPinSkip) {
      ///
      /// No OC pin assigned, skip this port
      ///
    } else {
      OCPin = UsbConfig->PortUsb30[Index].OverCurrentPin;
      ASSERT (OCPin < V_PCH_XHCI_NUMBER_OF_OC_PINS);
      U3OCMBuf[OCPin] |= (UINT32) (BIT0 << Index);
    }
  }

  for (Index = 0; Index < V_PCH_XHCI_NUMBER_OF_OC_PINS; Index++) {
    MmioWrite32 (XhciPciMmBase + R_PCH_XHCI_U3OCM + (Index * 4), U3OCMBuf[Index]);
    DEBUG ((EFI_D_INFO, "\tU3OCM%d = 0x%08x\tValue = 0x%08x\n",Index + 1,XhciPciMmBase + R_PCH_XHCI_U3OCM + (Index * 4),MmioRead32 (XhciPciMmBase + R_PCH_XHCI_U3OCM + (Index * 4))));
  }

  for (Index = 0; Index < GetPchXhciMaxUsb2PortNum (); Index++) {
    if (UsbConfig->PortUsb20[Index].OverCurrentPin == PchUsbOverCurrentPinSkip) {
      ///
      /// No OC pin assigned, skip this port
      ///
    } else {
      OCPin = UsbConfig->PortUsb20[Index].OverCurrentPin;
      ASSERT (OCPin < V_PCH_XHCI_NUMBER_OF_OC_PINS);
      U2OCMBuf[OCPin] |= (UINT32) (BIT0 << Index);
    }
  }

  for (Index = 0; Index < V_PCH_XHCI_NUMBER_OF_OC_PINS; Index++) {
    MmioWrite32 (XhciPciMmBase + R_PCH_XHCI_U2OCM + (Index * 4), U2OCMBuf[Index]);
    DEBUG ((EFI_D_INFO, "\tU2OCM%d = 0x%08x\tValue = 0x%08x\n",Index + 1,XhciPciMmBase + R_PCH_XHCI_U2OCM + (Index * 4),MmioRead32 (XhciPciMmBase + R_PCH_XHCI_U2OCM + (Index * 4))));
  }
  DEBUG ((EFI_D_INFO, __FUNCTION__" : Exit\n"));
}


/**
  Tune the USB 3.0 signals quality.

  @param[in]  UsbConfig                 The PCH Policy for USB configuration
**/
VOID
XhciUsb3Tune (
  IN  CONST PCH_USB_CONFIG    *UsbConfig
  )
{
  HSIO_LANE   HsioLane;
  UINT16      Offset;
  UINT8       Index;
  UINT8       PortId;
  UINT8       LaneOwner;
  UINT8       LaneNum;
  EFI_STATUS  Status;

  Offset = 0;

  DEBUG ((DEBUG_INFO, "XhciUsb3Tune() Start\n"));

  for (Index = 0; Index < GetPchXhciMaxUsb3PortNum (); Index++) {

    if ((UsbConfig->PortUsb30[Index].HsioTxDeEmphEnable == FALSE) &&
        (UsbConfig->PortUsb30[Index].HsioTxDownscaleAmpEnable == FALSE)) {
      continue;
    }

    Status = PchGetUsb3LaneNum (Index, &LaneNum);
    if (EFI_ERROR (Status)) {
      continue;
    }
    PchGetLaneInfo (LaneNum, &PortId, &LaneOwner);

    if (LaneOwner == V_PCH_PCR_FIA_LANE_OWN_USB3) {
      HsioGetLane (LaneNum, &HsioLane);
      //
      //Step 1: Make changes to any of the TX (Transmit) ModPHY Register Bit Fields
      //
      //
      // USB 3.0 TX Output -3.5dB De-Emphasis Adjustment Setting (ow2tapgen2deemph3p5)
      // HSIO_TX_DWORD5[21:16]
      //
      if (UsbConfig->PortUsb30[Index].HsioTxDeEmphEnable == TRUE) {
        Offset = HsioLane.Base + (UINT16) R_PCH_HSIO_TX_DWORD5;
        PchPcrAndThenOr32 (
          (PCH_SBI_PID) PortId,
          (UINT16) Offset,
          (UINT32) ~B_PCH_HSIO_TX_DWORD5_OW2TAPGEN2DEEMPH3P5_5_0,
          (UINT32) (UsbConfig->PortUsb30[Index].HsioTxDeEmph << N_PCH_HSIO_TX_DWORD5_OW2TAPGEN2DEEMPH3P5_5_0));
      }

      //
      // USB 3.0 TX Output Downscale Amplitude Adjustment (orate01margin)
      // HSIO_TX_DWORD8[21:16]
      //
      if (UsbConfig->PortUsb30[Index].HsioTxDownscaleAmpEnable == TRUE) {
        Offset = HsioLane.Base + (UINT16) R_PCH_HSIO_TX_DWORD8;
        PchPcrAndThenOr32 (
          (PCH_SBI_PID) PortId,
          (UINT16) Offset,
          (UINT32) ~B_PCH_HSIO_TX_DWORD8_ORATE01MARGIN_5_0,
          (UINT32) (UsbConfig->PortUsb30[Index].HsioTxDownscaleAmp << N_PCH_HSIO_TX_DWORD8_ORATE01MARGIN_5_0));
      }

      //
      // Step 2: Clear HSIO_TX_DWORD19 Bit[1] (o_calcinit bit) to 0b
      //
      PchPcrAndThenOr32 (
        (PCH_SBI_PID) PortId,
        (UINT16) Offset,
        (UINT32) ~(BIT1),
        (UINT32) (0));

      //
      // Step 3: Set HSIO_TX_DWORD19 Bit[1] (o_calcinit) to 1b
      //
      Offset = HsioLane.Base + (UINT16) R_PCH_HSIO_TX_DWORD19;
      PchPcrAndThenOr32 (
        (PCH_SBI_PID) PortId,
        (UINT16) Offset,
        (UINT32) ~(0),
        (UINT32) (BIT1));
    }
  }
}

