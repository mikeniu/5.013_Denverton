/** @file
  This file is a library for PCIE Ip specific Initalization.

@copyright
 Copyright (c) 2016-2017 Intel Corporation. All rights reserved
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

#include <PeiPcieIpInitLib.h>
#include <PchAccess.h>
#include <PciAccess.h>
#include <PcieIpPlatformLib.h>
#include <PeiPcieIpPolicyLib.h>
#include <Library/PciLib.h>
#include <Library/MmPciLib.h>
#include <Library/IoLib.h>
#include <Library/PostCodeLib.h>
#include <PcieIpRegs.h>

/* *********************************** private function  *********************************** */

VOID
PeiPcieDevHide(
 IN UINT8 clusterPcie,
 IN UINT32 pcieStrap,
 IN UINT8 bifcode
  )
{
  UINT32 hidevalue = 0;
  //only the straps for hidding RP
  pcieStrap &= (UINT32) 0xF;
  if ( ( bifcode != X2X2X2X2 ) ||
        (pcieStrap != (UINT32)(0x0F)  ) ) {
    MmioOr32 (
	  (UINTN) (PCH_PCR_ADDRESS (clusterPcie,R_PCIE_DEVHIDE_PRIV) ),
	  B_PCIE_DEVHIDE_PCFGHP );
  }
  switch ( bifcode )
  {
	  case X2X2X2X2:
		  hidevalue = (UINT32)(B_PCIE_DEVHICE_ALL_RP_ACTIVE) & pcieStrap ;
		  break;
	  case X2X2X4:
		  hidevalue = (UINT32) (B_PCIE_DEVHIDE_X2X2X4) & pcieStrap ;
		  break;
	  case X4X2X2:
		  hidevalue = (UINT32) (B_PCIE_DEVHIDE_X4X2X2) & pcieStrap ;
		  break;
	  case X4X4:
		  hidevalue = (UINT32) (B_PCIE_DEVHIDE_X4X4) & pcieStrap ;
		  break;
	  case X8:
		  hidevalue = (UINT32) (B_PCIE_DEVHIDE_X8) & pcieStrap ;
		  break;
  }
  MmioOr32 (
	(UINTN) (PCH_PCR_ADDRESS (clusterPcie,R_PCIE_DEVHIDE_PRIV) ),
	( ~hidevalue & (UINT32) (BIT3|BIT2|BIT1|BIT0) ) );

  MmioOr32 (
	(UINTN) (PCH_PCR_ADDRESS (clusterPcie,R_PCIE_DEVHIDE_PRIV) ),
	(UINT32) (B_PCIE_DEVHIDE_DHCL) );

  DEBUG ((EFI_D_INFO, "Cluster %X Strap %X hide %X\n" , clusterPcie, pcieStrap , ( ~hidevalue & (UINT32) (BIT3|BIT2|BIT1|BIT0) ) ));
}

VOID
PcieIpLaneEQPresetFeature (
  UINT8         		BifPcieCntl0,
  UINT8         		BifPcieCntl1,
  PCIE_IP_PORT_POLICY   PcieIpPortPolicy[ MAX_PCIE_ROOT_PORTS ]
  )
/**
  Lane EQ preset default value should be changed to 5 for PCIE

  Arguments:
  @param[in] BifPcieCntl0 	- bifurcation of PCIE controller 0
  @param[in] BifPcieCntl1 	- bifurcation of PCIE controller 1

  Returns:
  None
**/
{
//  UINT16      Data16 = 0;
  UINT32      Offsets[] = { 0x00020C, 0x00020E, 0x000210, 0x000212, 0x000214, 0x000216, 0x000218, 0x00021A };
  UINT8       Index, OffsetIndex;
  UINTN       RPBase = (UINTN)PCI_DEVICE_IS_NOT_PRESENT ;

  DEBUG ((EFI_D_INFO, "PcieIpLaneEQPresetFeature bifurcation %X\n" , BifPcieCntl0 ));
  switch ( BifPcieCntl0  ) {
    case X2X2X2X2: //X2X2X2X2
  	      OffsetIndex = 0;
		  for( Index = 0 ;  Index < (PCIE_IP_STANDARD_ROOT_PORT/MAX_PCIE_CLUSTERS) ; Index++, OffsetIndex++ ) {
	    	  RPBase = PcieIpPortPolicy[ Index ].RPBase;
	    	  DEBUG ((EFI_D_INFO, "RP base %X\n" , RPBase ));
	    	  if ( RPBase == (UINTN)PCI_DEVICE_IS_NOT_PRESENT ) {
	    		  continue;
	    	  }

	    	  MmioAndThenOr16(RPBase +  Offsets[ OffsetIndex ] , B_WM_LANEEQCTL_MASK , B_WM_LANEEQCTL_UPTP_DPTP  );
	    	  DEBUG ((EFI_D_INFO, "RP %X offset %X\n" , Index, Offsets[ OffsetIndex ] ));

			  OffsetIndex++;
	    	  MmioAndThenOr16(RPBase +  Offsets[ OffsetIndex ] , B_WM_LANEEQCTL_MASK , B_WM_LANEEQCTL_UPTP_DPTP  );
	    	  DEBUG ((EFI_D_INFO, "RP %X offset %X\n" , Index, Offsets[ OffsetIndex ] ));
		  }

    	break;
    case X2X2X4: //X2X2X4
	      OffsetIndex = 0;
		  for( Index = 0 ;  Index < (PCIE_IP_STANDARD_ROOT_PORT/MAX_PCIE_CLUSTERS) ; Index++, OffsetIndex++ ) {
			  if ( Index ==0 || Index == 2 || Index ==3 ) {
	    	    RPBase = PcieIpPortPolicy[ Index ].RPBase;
		    	DEBUG ((EFI_D_INFO, "RP %X base memory: %X \t" , Index, RPBase ));
			  }
	    	  if ( RPBase == (UINTN)PCI_DEVICE_IS_NOT_PRESENT ) {
	    		  continue;
	    	  }

	    	  MmioAndThenOr16(RPBase +  Offsets[ OffsetIndex ] , B_WM_LANEEQCTL_MASK , B_WM_LANEEQCTL_UPTP_DPTP  );
	    	  DEBUG ((EFI_D_INFO, "offset %X \t" , Offsets[ OffsetIndex ] ));

			  OffsetIndex++;
	    	  MmioAndThenOr16(RPBase +  Offsets[ OffsetIndex ] , B_WM_LANEEQCTL_MASK , B_WM_LANEEQCTL_UPTP_DPTP  );
	    	  DEBUG ((EFI_D_INFO, "offset %X\n" , Offsets[ OffsetIndex ] ));
		  }
    	break;
    case X4X2X2: //X4X2X2
	      OffsetIndex = 0;
		  for( Index = 0 ;  Index < (PCIE_IP_STANDARD_ROOT_PORT/MAX_PCIE_CLUSTERS) ; Index++, OffsetIndex++ ) {
			  if ( Index <= 2 ) {
	    	    RPBase = PcieIpPortPolicy[ Index ].RPBase;
		    	  DEBUG ((EFI_D_INFO, "RP %X base memory: %X \t" , Index, RPBase ));
			  }
	    	  if ( RPBase == (UINTN)PCI_DEVICE_IS_NOT_PRESENT ) {
	    		  continue;
	    	  }

	    	  MmioAndThenOr16(RPBase +  Offsets[ OffsetIndex ] , B_WM_LANEEQCTL_MASK , B_WM_LANEEQCTL_UPTP_DPTP  );
	    	  DEBUG ((EFI_D_INFO, "offset %X \t" , Offsets[ OffsetIndex ] ));

			  OffsetIndex++;
	    	  MmioAndThenOr16(RPBase +  Offsets[ OffsetIndex ] , B_WM_LANEEQCTL_MASK , B_WM_LANEEQCTL_UPTP_DPTP  );
	    	  DEBUG ((EFI_D_INFO, "offset %X\n" , Offsets[ OffsetIndex ] ));
		  }
    	break;
    case X4X4: //X4X4
	      OffsetIndex = 0;
		  for( Index = 0 ;  Index < (PCIE_IP_STANDARD_ROOT_PORT/MAX_PCIE_CLUSTERS) ; Index++, OffsetIndex++ ) {
			  if ( Index == 0 || Index == 2 ) {
	    	    RPBase = PcieIpPortPolicy[ Index ].RPBase;
		    	  DEBUG ((EFI_D_INFO, "RP %X base memory: %X \t" , Index, RPBase ));
			  }
	    	  if ( RPBase == (UINTN)PCI_DEVICE_IS_NOT_PRESENT ) {
	    		  continue;
	    	  }

	    	  MmioAndThenOr16(RPBase +  Offsets[ OffsetIndex ] , B_WM_LANEEQCTL_MASK , B_WM_LANEEQCTL_UPTP_DPTP  );
	    	  DEBUG ((EFI_D_INFO, "offset %X \t" , Offsets[ OffsetIndex ] ));

			  OffsetIndex++;
	    	  MmioAndThenOr16(RPBase +  Offsets[ OffsetIndex ] , B_WM_LANEEQCTL_MASK , B_WM_LANEEQCTL_UPTP_DPTP  );
	    	  DEBUG ((EFI_D_INFO, "offset %X\n" , Offsets[ OffsetIndex ] ));
		  }
    	break;
    case X8: //X8
	      OffsetIndex = 0;
		  for( Index = 0 ;  Index < (PCIE_IP_STANDARD_ROOT_PORT/MAX_PCIE_CLUSTERS) ; Index++, OffsetIndex++ ) {
			  if ( Index == 0 ) {
	    	    RPBase = PcieIpPortPolicy[ Index ].RPBase;
		    	  DEBUG ((EFI_D_INFO, "RP %X base memory: %X \t" , Index, RPBase ));
			  }
	    	  if ( RPBase == (UINTN)PCI_DEVICE_IS_NOT_PRESENT ) {
	    		  continue;
	    	  }

	    	  MmioAndThenOr16(RPBase +  Offsets[ OffsetIndex ] , B_WM_LANEEQCTL_MASK , B_WM_LANEEQCTL_UPTP_DPTP  );
	    	  DEBUG ((EFI_D_INFO, "offset %X \t" , Offsets[ OffsetIndex ] ));

			  OffsetIndex++;
	    	  MmioAndThenOr16(RPBase +  Offsets[ OffsetIndex ] , B_WM_LANEEQCTL_MASK , B_WM_LANEEQCTL_UPTP_DPTP  );
	    	  DEBUG ((EFI_D_INFO, "offset %X\n" , Offsets[ OffsetIndex ] ));
		  }
    	break;
  }
  DEBUG ((EFI_D_INFO, "PcieIpLaneEQPresetFeature bifurcationt %X\n" , BifPcieCntl1 ));
  switch ( BifPcieCntl1  ) {
    case X2X2X2X2: //X2X2X2X2
  	      OffsetIndex = 0;
		  for( Index = (PCIE_IP_STANDARD_ROOT_PORT/MAX_PCIE_CLUSTERS) ;  Index < PCIE_IP_STANDARD_ROOT_PORT ; Index++, OffsetIndex++ ) {
	    	  RPBase = PcieIpPortPolicy[ Index ].RPBase;
	    	  DEBUG ((EFI_D_INFO, "RP base %X\n" , RPBase ));
	    	  if ( RPBase == (UINTN)PCI_DEVICE_IS_NOT_PRESENT ) {
	    		  continue;
	    	  }

	    	  MmioAndThenOr16(RPBase +  Offsets[ OffsetIndex ] , B_WM_LANEEQCTL_MASK , B_WM_LANEEQCTL_UPTP_DPTP  );
	    	  DEBUG ((EFI_D_INFO, "RP %X offset %X\n" , Index, Offsets[ OffsetIndex ] ));

			  OffsetIndex++;
	    	  MmioAndThenOr16(RPBase +  Offsets[ OffsetIndex ] , B_WM_LANEEQCTL_MASK , B_WM_LANEEQCTL_UPTP_DPTP  );
	    	  DEBUG ((EFI_D_INFO, "RP %X offset %X\n" , Index, Offsets[ OffsetIndex ] ));
		  }

    	break;
    case X2X2X4: //X2X2X4
	      OffsetIndex = 0;
		  for( Index = (PCIE_IP_STANDARD_ROOT_PORT/MAX_PCIE_CLUSTERS) ;  Index < PCIE_IP_STANDARD_ROOT_PORT ; Index++, OffsetIndex++ ) {
			  if ( Index == 4 || Index == 6 || Index == 7 ) {
	    	    RPBase = PcieIpPortPolicy[ Index ].RPBase;
		    	DEBUG ((EFI_D_INFO, "RP %X base memory: %X \t" , Index, RPBase ));
			  }
	    	  if ( RPBase == (UINTN)PCI_DEVICE_IS_NOT_PRESENT ) {
	    		  continue;
	    	  }

	    	  MmioAndThenOr16(RPBase +  Offsets[ OffsetIndex ] , B_WM_LANEEQCTL_MASK , B_WM_LANEEQCTL_UPTP_DPTP  );
	    	  DEBUG ((EFI_D_INFO, "offset %X \t" , Offsets[ OffsetIndex ] ));

			  OffsetIndex++;
	    	  MmioAndThenOr16(RPBase +  Offsets[ OffsetIndex ] , B_WM_LANEEQCTL_MASK , B_WM_LANEEQCTL_UPTP_DPTP  );
	    	  DEBUG ((EFI_D_INFO, "offset %X\n" , Offsets[ OffsetIndex ] ));
		  }
    	break;
    case X4X2X2: //X4X2X2
	      OffsetIndex = 0;
		  for( Index = (PCIE_IP_STANDARD_ROOT_PORT/MAX_PCIE_CLUSTERS) ;  Index < PCIE_IP_STANDARD_ROOT_PORT ; Index++, OffsetIndex++ ) {
			  if ( Index <= 6 ) {
	    	    RPBase = PcieIpPortPolicy[ Index ].RPBase;
		    	  DEBUG ((EFI_D_INFO, "RP %X base memory: %X \t" , Index, RPBase ));
			  }
	    	  if ( RPBase == (UINTN)PCI_DEVICE_IS_NOT_PRESENT ) {
	    		  continue;
	    	  }

	    	  MmioAndThenOr16(RPBase +  Offsets[ OffsetIndex ] , B_WM_LANEEQCTL_MASK , B_WM_LANEEQCTL_UPTP_DPTP  );
	    	  DEBUG ((EFI_D_INFO, "offset %X \t" , Offsets[ OffsetIndex ] ));

			  OffsetIndex++;
	    	  MmioAndThenOr16(RPBase +  Offsets[ OffsetIndex ] , B_WM_LANEEQCTL_MASK , B_WM_LANEEQCTL_UPTP_DPTP  );
	    	  DEBUG ((EFI_D_INFO, "offset %X\n" , Offsets[ OffsetIndex ] ));
		  }
    	break;
    case X4X4: //X4X4
	      OffsetIndex = 0;
		  for( Index = (PCIE_IP_STANDARD_ROOT_PORT/MAX_PCIE_CLUSTERS) ;  Index < PCIE_IP_STANDARD_ROOT_PORT ; Index++, OffsetIndex++ ) {
			  if ( Index == 4 || Index == 6 ) {
	    	    RPBase = PcieIpPortPolicy[ Index ].RPBase;
		    	DEBUG ((EFI_D_INFO, "RP %X base memory: %X \t" , Index, RPBase ));
			  }
	    	  if ( RPBase == (UINTN)PCI_DEVICE_IS_NOT_PRESENT ) {
	    		  continue;
	    	  }

	    	  MmioAndThenOr16(RPBase +  Offsets[ OffsetIndex ] , B_WM_LANEEQCTL_MASK , B_WM_LANEEQCTL_UPTP_DPTP  );
	    	  DEBUG ((EFI_D_INFO, "offset %X \t" , Offsets[ OffsetIndex ] ));

			  OffsetIndex++;
	    	  MmioAndThenOr16(RPBase +  Offsets[ OffsetIndex ] , B_WM_LANEEQCTL_MASK , B_WM_LANEEQCTL_UPTP_DPTP  );
	    	  DEBUG ((EFI_D_INFO, "offset %X\n" , Offsets[ OffsetIndex ] ));
		  }
    	break;
    case X8: //X8
	      OffsetIndex = 0;
		  for( Index = (PCIE_IP_STANDARD_ROOT_PORT/MAX_PCIE_CLUSTERS) ;  Index < PCIE_IP_STANDARD_ROOT_PORT ; Index++, OffsetIndex++ ) {
			  if ( Index == 4 ) {
	    	    RPBase = PcieIpPortPolicy[ Index ].RPBase;
		    	DEBUG ((EFI_D_INFO, "RP %X base memory: %X \t" , Index, RPBase ));
			  }
	    	  if ( RPBase == (UINTN)PCI_DEVICE_IS_NOT_PRESENT ) {
	    		  continue;
	    	  }

	    	  MmioAndThenOr16(RPBase +  Offsets[ OffsetIndex ] , B_WM_LANEEQCTL_MASK , B_WM_LANEEQCTL_UPTP_DPTP  );
	    	  DEBUG ((EFI_D_INFO, "offset %X \t" , Offsets[ OffsetIndex ] ));

			  OffsetIndex++;
	    	  MmioAndThenOr16(RPBase +  Offsets[ OffsetIndex ] , B_WM_LANEEQCTL_MASK , B_WM_LANEEQCTL_UPTP_DPTP  );
	    	  DEBUG ((EFI_D_INFO, "offset %X\n" , Offsets[ OffsetIndex ] ));
		  }
    	break;
  }
}

VOID
PeiPcieWACpl(
  IN UINTN RPBase
  )
/*++
Routine Description:
  Disables the ability of downstream relaxed ordered completions from passing posted requests in the same direction

Arguments:
  RPBase    Root port base address.
--*/
{
  UINT32 Data32 = 0;
  Data32 = MmioRead32( RPBase + R_PCH_PCIE_XPTDEF2 );

  Data32 |= V_PCH_PCIE_XPTDEF2_DNRODIS;

  MmioWrite32 (RPBase + R_PCH_PCIE_XPTDEF2, Data32);
}

VOID
PeiPcieWAEBErrors(
  IN UINTN RPBase
  )
/*++
Routine Description:
  Disables the ability of downstream relaxed ordered completions from passing posted requests in the same direction

Arguments:
  RPBase    Root port base address.
--*/
{
  UINT32 Data32 = 0;
  Data32 = MmioRead32( RPBase + V_PCIE_OFFSET_A30H_LTSSMTRNCTL );

  Data32 &= (UINT32) ~V_PCH_PCIE_LTSSMTRNCTL_ENFORCEDREC;

  MmioWrite32 (RPBase + V_PCIE_OFFSET_A30H_LTSSMTRNCTL, Data32);
}

VOID
PeiPcieSurpriseLinkDownControl (
	IN UINT8 Enable,
	IN UINTN RPBase

  )
/*++

Routine Description:

  Enable/Disable Surprise Link Down

Arguments:

  RPBase    Root port base address.
  Enable    Flag used to indicate if enable or disable SURLNK

--*/
{
  UINT32 Data32;

  Data32 = MmioRead32 (RPBase + PCI_OFFSET_A30H_LTSSMTRNCTL);

  if (Enable == TRUE) {
    Data32 |= B_PCH_PCIE_LTSSMTRNCTL_SURLNKNEN;
  } else {
	Data32 &= ~B_PCH_PCIE_LTSSMTRNCTL_SURLNKNEN;
  }

  MmioWrite32 (RPBase + PCI_OFFSET_A30H_LTSSMTRNCTL, Data32);
}

VOID
PeiPcieSetASPML1(
  IN UINT8 Enable,
  IN UINTN RPBase
		  )
/*++

Routine Description:

  Enable/ Disable ASPM L1

Arguments:

  RPBase    Root port base address.
  Enable    Flag used to indicate if enable or disable ASPM L1

--*/
{
  UINT16 Data16 = 0;

  Data16 = MmioRead16 (RPBase + R_PCH_PCIE_LCTL);
  DEBUG ((EFI_D_INFO, "B PCIE ASPM LINK CTL %X\n" , (Data16 & B_PCH_PCIE_LCTL_APMC) ));

  if (Enable == 2 ) {   //10b: L1 Supported
    Data16 |= V_PCH_PCIE_LCTL_APMC_L1;
  } else {
	Data16 &= V_PCH_PCIE_LCTL_APMC_L1_DISABLE;
  }
  MmioWrite16 (RPBase + R_PCH_PCIE_LCTL, Data16);

  Data16 = MmioRead16 (RPBase + R_PCH_PCIE_LCTL);
  DEBUG ((EFI_D_INFO, "A PCIE ASPM LINK CTL %X\n" , (Data16 & B_PCH_PCIE_LCTL_APMC) ));

  Data16 = MmioRead16 (RPBase + R_PCH_PCIE_LCAP);
  DEBUG ((EFI_D_INFO, "B PCIE ASPM LINK CAP %X\n" , (Data16 & B_PCH_PCIE_LCAP_APMS) ));

  if (Enable == 2 ) {   //10b: L1 Supported
    Data16 |= V_PCH_PCIE_LCAP_APMS_L1;
  } else {
	Data16 &= V_PCH_PCIE_LCAP_APMS_L1_DISABLE;
  }
  MmioWrite16 (RPBase + R_PCH_PCIE_LCAP, Data16);

  Data16 = MmioRead16 (RPBase + R_PCH_PCIE_LCAP);
  DEBUG ((EFI_D_INFO, "A PCIE ASPM LINK CAP %X\n" , (Data16 & B_PCH_PCIE_LCAP_APMS) ));
}

VOID
PeiPcieSetLock(
  IN UINT8 Enable,
  IN UINTN RPBase
		  )
/*++

Routine Description:

  Lock / Unlock PCIE registers

Arguments:

  RPBase    Root port base address.
  Enable    Flag used to indicate if lock or unlock PCIE registers

--*/
{
  UINT16 Data16;

  Data16 = MmioRead16 (RPBase + R_PCH_PCIE_PLKCTL);

  if (Enable == 1 ) {
    Data16 |= B_PCH_PCIE_PLKCTL_CL;
    Data16 |= B_PCH_PCIE_PLKCTL_TLCL;
    Data16 |= B_PCH_PCIE_PLKCTL_LLCL;
  } else {
	Data16 &= ~B_PCH_PCIE_PLKCTL_CL;
	Data16 &= ~B_PCH_PCIE_PLKCTL_TLCL;
	Data16 &= ~B_PCH_PCIE_PLKCTL_LLCL;
  }
  MmioWrite16 (RPBase + R_PCH_PCIE_PLKCTL, Data16);
}

VOID
PeiPcieSetRTTO(
		UINTN		RPBase,
		IN UINT8	TimeoutValue
)
/*++

Routine Description:

  Set Replay timer value

Arguments:

  RPBase    	RTRYCTL regiser base address
  TimeoutValue	Reply timer timeout vaule

--*/
{
	DEBUG ((EFI_D_INFO, "B RTRYCTL %X\n" ,	 (UINT8)B_PCIE_RTRYCTL_RTTO & MmioRead8(RPBase + R_PCIE_RTRYCTL) ));
	MmioAndThenOr8(RPBase + R_PCIE_RTRYCTL,  (UINT8)(~B_PCIE_RTRYCTL_RTTO), TimeoutValue);
	DEBUG ((EFI_D_INFO, "A RTRYCTL %X\n" , 	 (UINT8)B_PCIE_RTRYCTL_RTTO & MmioRead8(RPBase + R_PCIE_RTRYCTL) ));
}

VOID
PeiPcieDevCap2(
  IN UINTN RPBase
		  )
/*++

Routine Description:

  Clean DEVCAP2 PCIE register

Arguments:

  RPBase    Root port base address.

--*/
{
  UINT32 Data32 = 0;
  Data32 = MmioRead32( RPBase + R_PCH_PCIE_DCAP2 );

  Data32 |= B_PCH_PCIE_DCAP2_CTDS;
  Data32 |= V_PCH_PCIE_DCAP2_CTRS_RANGE_A;
  Data32 |= V_PCH_PCIE_DCAP2_CTRS_RANGE_B;
  Data32 |= V_PCH_PCIE_DCAP2_CTRS_RANGE_C;

  MmioWrite32 (RPBase + R_PCH_PCIE_DCAP2, Data32);
}

VOID
PeiPciePhyRecal(
  IN UINTN RPBase
		  )
/*++

Routine Description:

  Clean PHYRECAL register

Arguments:

  RPBase    Root port base address.

--*/
{
  UINT32 Data32 = 0;

  MmioWrite32 (RPBase + R_PCH_PCIE_PHYRECAL, Data32);
}

VOID
PeiPcieStopScream(
  IN BOOLEAN Enable,
  IN UINTN RPBase
		  )
/*++

Routine Description:

  Enable/Disable Stop & Scream  functionality

Arguments:

  RPBase    Root port base address.
  Enable    Flag used to indicate if enable or disable stop & Scream functionality

--*/
 {
  UINT16 Data16;

  Data16 = MmioRead16 (RPBase + R_PCH_PCIE_CCSTL);

  if (Enable == TRUE) {
    Data16 |= B_PCH_PCIE_CCSTL_SSEN;
  } else {
	Data16 &= ~B_PCH_PCIE_CCSTL_SSEN;
  }
  MmioWrite16 (RPBase + R_PCH_PCIE_CCSTL, Data16);
}

EFI_STATUS
PeiPcieSelectDeEmphasis
(
  IN UINT8  deEmphasis,
  IN UINTN  RPBase
)
/*++
Routine Description:
  Configure De-emphasis Level for root ports

  Arguments:
  @param[in] deEmphasis 		- De-emphasis value.
  @param[in] RPBase 			- Root port base address.

  Returns:
  @retval EFI_SUCCESS 				- The function completed successfully.
  @retval RETURN_INVALID_PARAMETER 	- invalid parameter
--*/
{
  EFI_STATUS Status;
  if ( (deEmphasis != V_PCIE_LCTL2_DEEMPHASYS_6_DB_VALUE && deEmphasis != V_PCIE_LCTL2_DEEMPHASYS_3P5_DB_VALUE ) || RPBase == (UINTN) NULL ) {
    DEBUG ((EFI_D_INFO, "Error: Bad parameter for Pcie De-emphasis: %x \n",deEmphasis));
    Status = RETURN_INVALID_PARAMETER ;
  } else {
    if( deEmphasis == V_PCIE_LCTL2_DEEMPHASYS_6_DB_VALUE) {
	  DEBUG ((EFI_D_INFO, "PCIE DeEmphasis = -6dB\n"));
      MmioAnd16 (RPBase + R_PCIE_LCTL2 , (UINT16) ~BIT6);
    } else if (deEmphasis == V_PCIE_LCTL2_DEEMPHASYS_3P5_DB_VALUE ) {
	  DEBUG ((EFI_D_INFO, "PCIE DeEmphasis = -3.5dB\n"));
      MmioOr16 (RPBase + R_PCIE_LCTL2 , (UINT16) BIT6);
    }
    Status = EFI_SUCCESS;
  }
  return Status;
}


EFI_STATUS
PeiPcieCtlLaneReversal
(
    UINT8 LaneReversal,
    UINTN RPBase
)
/**
  Set lane reversal settings for a root ports

  Arguments:
  @param[in] LaneReversal 		- LaneReversal option
  @param[in] RPBase 			- Address base root port


  Returns:
  @retval EFI_SUCCESS 				- The function completed successfully.
  @retval RETURN_INVALID_PARAMETER 	- invalid parameter
**/
{
    EFI_STATUS Status;
    UINT32 Data32Or;
    Status = EFI_SUCCESS;
    if ( RPBase == (UINTN) NULL ) {
    	Status = RETURN_INVALID_PARAMETER ;
    } else {
	  // Lane Reversal
      //Activate  bit 5  LTSSMTRNCTL
	  //Bus: 0 Device: 1 Function: 0 Offset: A30h ; (Root ports)
	  Data32Or = MmioRead32 ( RPBase + V_PCIE_OFFSET_A30H_LTSSMTRNCTL);
	  switch ( LaneReversal ) {
		case V_PCIE_LANE_REVERSAL:
		  // Force reverse
		  Data32Or |=   BIT5    ;            //REVERSEOVVR: Disable Dynamic Lane Reversal, use REVERSEVAL instead
		  Data32Or |=   BIT6    ; 	         //REVERSEVAL, force operate in reverse
		  MmioWrite32(  RPBase +  V_PCIE_OFFSET_A30H_LTSSMTRNCTL  ,  Data32Or ) ;
		  DEBUG ((EFI_D_INFO, "Non dynamic Link Reversal Enabled : Addr %x \n",RPBase +  V_PCIE_OFFSET_A30H_LTSSMTRNCTL));
		  break;
		case V_PCIE_NO_LANE_REVERSAL:
		  //Force non reverse
		  Data32Or |=    BIT5;
		  Data32Or &=  ~(BIT6);		//REVERSEVAL, force operate in non-reverse
		  MmioWrite32(  RPBase +  V_PCIE_OFFSET_A30H_LTSSMTRNCTL  ,  Data32Or ) ;
		  DEBUG ((EFI_D_INFO, "No Link Reversal : Addr %x \n",RPBase +  V_PCIE_OFFSET_A30H_LTSSMTRNCTL));
		  break;
		case V_PCIE_DYNAMIC_LANE_REVERSAL:
		  // Dynamic Lane Reversal
		  Data32Or &= ~(BIT5);
		  MmioWrite32(  RPBase +  V_PCIE_OFFSET_A30H_LTSSMTRNCTL  ,  Data32Or ) ;
		  DEBUG ((EFI_D_INFO, "Dynamic Link Reversal is set: Addr %x \n",RPBase +  V_PCIE_OFFSET_A30H_LTSSMTRNCTL));
		  break;
		default:
		  Status = RETURN_INVALID_PARAMETER ;
	  }
    }
    return Status;
}


EFI_STATUS
PeiPcieLnkCtl (
  IN PCIE_IP_PORT_POLICY 	*PcieIpPortPolicy,
  IN UINT8      			PortIndex
  )
/**
  configure Link Ctl register in PCIe

  Arguments:
  @param[in] PcieIpPortPolicy 	- This is the Port policy
  @param[in] PortIndex 			- Root number port

  Returns:
  @retval EFI_SUCCESS 				- The function completed successfully.
  @retval RETURN_INVALID_PARAMETER 	- invalid parameter
**/
{
  EFI_STATUS Status;
  UINT16 Data16 = 0;
  DEBUG ((DEBUG_INFO, "PEI PcieIp Link Ctl Start\n"));
  if ( PcieIpPortPolicy == NULL ) {
    Status = RETURN_INVALID_PARAMETER ;
  } else {
    ///
	/// Configure Extended Synch
	///
	if ( PcieIpPortPolicy[ PortIndex ].ExtSync ) {
	  Data16 = (UINT16) B_PCH_PCIE_LCTL_ES;
	  MmioOr16 ( PcieIpPortPolicy[ PortIndex ].RPBase + R_PCH_PCIE_LCTL, Data16);
	  DEBUG ((DEBUG_ERROR, "PCIE_LCTL.ExtSynch.Enabled :: PCIE_LCTL = 0x%x\n",
			  MmioRead16 ( PcieIpPortPolicy[ PortIndex ].RPBase + R_PCH_PCIE_LCTL)));
	} else {
	  Data16 = (UINT16) ~B_PCH_PCIE_LCTL_ES;
	  MmioAnd16 ( PcieIpPortPolicy[ PortIndex ].RPBase + R_PCH_PCIE_LCTL,Data16);
	  DEBUG ((DEBUG_ERROR, "PCIE_LCTL.ExtSynch.Disabled :: PCIE_LCTL = 0x%x\n",
			  MmioRead16 ( PcieIpPortPolicy[ PortIndex ].RPBase + R_PCH_PCIE_LCTL)));
	}
	Status = EFI_SUCCESS;
  }
  DEBUG ((DEBUG_INFO, "PEI PcieIpInit End\n"));
  return Status;
}

EFI_STATUS
PeiPcieDevCtlInit (
  IN PCIE_IP_PORT_POLICY 	*PcieIpPortPolicy,
  IN UINT8      			PortIndex
  )
/**
  configure Dev Ctl register in PCIe

  Arguments:
  @param[in] PcieIpPortPolicy 	- This is the Port policy
  @param[in] PortIndex 			- Root number port

  Returns:
  @retval EFI_SUCCESS 				- The function completed successfully.
  @retval RETURN_INVALID_PARAMETER 	- invalid parameter
**/
{
  EFI_STATUS Status;
  UINT16 Data16 = 0;

  DEBUG ((DEBUG_INFO,"PeiPcieDevCtlInit Start\n"));

  if ( PcieIpPortPolicy == NULL ) {
    Status = RETURN_INVALID_PARAMETER ;
  } else {
	///
	/// Clear Bits
	///
	Data16 = MmioRead16 (PcieIpPortPolicy[ PortIndex ].RPBase + R_PCIE_DCTL);
	Data16 &= (UINT16) (~(B_PCIE_DCTL_MRRS | B_PCIE_DCTL_MPS ));	//< Apply MASK for MRRS and MPS
	///
	/// Write register according to policy data
	///
	if ( PcieIpPortPolicy[ PortIndex ].DeviceCtlConfig.MaxReadRequest == 55 ) {
	  PcieIpPortPolicy[ PortIndex ].DeviceCtlConfig.MaxReadRequest = 5;
	}
	Data16 |= (UINT16) (PcieIpPortPolicy[ PortIndex ].DeviceCtlConfig.MaxReadRequest << N_PCH_PCIE_DCTL_MRRS); /// Max Read Request Size
	if ( PcieIpPortPolicy[ PortIndex ].DeviceCtlConfig.MaxPayload == 55 ) {
	  PcieIpPortPolicy[ PortIndex ].DeviceCtlConfig.MaxPayload = 1 ;
	}
	Data16 |= (UINT16) (PcieIpPortPolicy[ PortIndex ].DeviceCtlConfig.MaxPayload << N_PCIE_DCTL_MPS);      /// Max Payload in DXE
	Data16 |= (UINT16) (PcieIpPortPolicy[ PortIndex ].DeviceCtlConfig.RelaxedOrdering << B_PCIE_DCTL_ENRO);      /// Enable Relaxed Ordering
	Data16 |= (UINT16) (PcieIpPortPolicy[ PortIndex ].ExtTagField << B_PCIE_DCTL_ETFE);      /// Extended Tag Field Enable
	MmioWrite16 ( PcieIpPortPolicy[ PortIndex ].RPBase + R_PCIE_DCTL, Data16);

	DEBUG ((DEBUG_ERROR, "PCIE_DCTL = 0x%X\n", MmioRead16 ( PcieIpPortPolicy[ PortIndex ].RPBase + R_PCIE_DCTL)));

	Status = EFI_SUCCESS;
  }

  DEBUG ((DEBUG_INFO, "PeiPcieDevCtlInit End\n"));
  return Status;
}

EFI_STATUS
PeiPcieDevCtl2Init (
  IN PCIE_IP_PORT_POLICY 	*PcieIpPortPolicy,
  IN UINT8      			PortIndex
  )
/**
  configure Dev Ctl 2 register in PCIe

  Arguments:
  @param[in] PcieIpPortPolicy 	- This is the Port policy
  @param[in] PortIndex 			- Root number port

  Returns:
  @retval EFI_SUCCESS 				- The function completed successfully.
  @retval RETURN_INVALID_PARAMETER 	- invalid parameter
**/
{
  EFI_STATUS Status;
  UINT16     Data16 = 0;

  DEBUG ((DEBUG_INFO,"PeiPcieDevCtl2Init Start\n"));

  if ( PcieIpPortPolicy == NULL ) {
    Status = RETURN_INVALID_PARAMETER ;
  } else {
	Data16 = MmioRead16 (PcieIpPortPolicy[ PortIndex ].RPBase + R_PCIE_DCTL2);

	///
	/// Check if Completion Timeout is Enable
	///
    Data16 &= ~( B_PCIE_DCTL2_CTD | B_PCIE_DCTL2_CTV);
	if ( !PcieIpPortPolicy[ PortIndex ].CompletionTimeout ) {
      switch ( PcieIpPortPolicy[ PortIndex ].CompletionTimeoutRange ){

      case PcieCompletionTO_50us_50ms:
        Data16 |= V_PCIE_DCTL2_CTV_50US_50MS;
        break;

      case PcieCompletionTO_50us_100us:
        Data16 |= V_PCIE_DCTL2_CTV_50US_100US;
        break;

      case PcieCompletionTO_1ms_10ms:
        Data16 |= V_PCIE_DCTL2_CTV_1MS_10MS;
        break;

      case PcieCompletionTO_16ms_55ms:
        Data16 |= V_PCIE_DCTL2_CTV_16MS_55MS;
        break;

      case PcieCompletionTO_65ms_210ms:
        Data16 |= V_PCIE_DCTL2_CTV_65MS_210MS;
        break;

      case PcieCompletionTO_260ms_900ms:
        Data16 |= V_PCIE_DCTL2_CTV_260MS_900MS;
        break;

      case PcieCompletionTO_1s_3_5s:
        Data16 |= V_PCIE_DCTL2_CTV_1S_3_5S;
        break;

      case PcieCompletionTO_4s_13s:
        Data16 |= V_PCIE_DCTL2_CTV_4S_13S;
        break;

      case PcieCompletionTO_17s_64s:
        Data16 |= V_PCIE_DCTL2_CTV_17S_64S;
        break;

      case PcieCompletionTO_Default:
      default:
        Data16 |= V_PCIE_DCTL2_CTV_65MS_210MS;
        break;
      }
	} else {
      Data16 |= B_PCIE_DCTL2_CTD;
 	}

	MmioWrite16 ( PcieIpPortPolicy[ PortIndex ].RPBase + R_PCIE_DCTL2, Data16);
	DEBUG ((DEBUG_ERROR, "PCIE_DCTL2 = 0x%04x\n", MmioRead16 ( PcieIpPortPolicy[ PortIndex ].RPBase + R_PCIE_DCTL2)));
	Status = EFI_SUCCESS;
  }

  DEBUG ((DEBUG_INFO, "PeiPcieDevCtlInit End\n"));
  return Status;
}

VOID
PcieIpSetPIPEtoPSState2(
  IN PCIE_IP_GLOBAL_POLICY_PPI  *PcieIpGlobaIpolicy
    )
/**
  DNV BIOS WorkAround regarding sighting 1404103991 for stepping A0
  Sends PIPE interface to PS State 2 for L1.0

  Arguments:
  @param[in] PcieIpGlobalPolicy - This is the global PCIE policy

  Returns:
  NONE
**/
{
  UINT8 Index;
  UINT32 Data32;

  //
  // Set PS State 2 for all active root ports
  //
  for( Index=0 ; Index < PCIE_IP_STANDARD_ROOT_PORT ; Index++){
    if ( MmPci32 ( 0,0, PcieIpGlobaIpolicy->PcieIpPortBdfMap[ Index ].DevNum ,0, 0) == (UINT32 )~0 ) {
    // Root Port Not Active, Skip It
    continue;
    }
    Data32 = MmPci32 ( 0,0,PcieIpGlobaIpolicy->PcieIpPortBdfMap[ Index ].DevNum,0, R_PCIE_PIPECTL2 );
    Data32 &= ~B_PCIE_PIPECTL2_L1P0_PSMAP;
    Data32 |= (PIPE_PS_STATE_2 << B_PCIE_PIPECTL2_L1P0_PSMAP_POSITION);
    MmPci32 ( 0,0,PcieIpGlobaIpolicy->PcieIpPortBdfMap[ Index ].DevNum,0, R_PCIE_PIPECTL2 ) = Data32 ;
  }
}

VOID
PcieIpInitLinkTraining(
  IN PCIE_IP_GLOBAL_POLICY_PPI  *PcieIpGlobaIpolicy		//is an array of cluster
		)
/**
  Initialize Link Training

  Arguments:
  @param[in] PcieIpGlobalPolicy - This is the global PCIE policy

  Returns:
  NONE
**/
{
  UINT8 Index;
  UINT32 Data32;
  UINT16 Data16;

  //
  // Initiate link training for all root ports
  //
  for( Index=0 ; Index < PCIE_IP_STANDARD_ROOT_PORT ; Index++){
    if ( MmPci32 ( 0,0, PcieIpGlobaIpolicy->PcieIpPortBdfMap[ Index ].DevNum ,0, 0) == (UINT32 )~0 ) {
	  // Root Port Not Active, skip it
	  continue;
    }
    // set Elastic buffer, feature: 1404736062
    // soc.pcies.ports.eberrmsk=0xFF  , offset: 0x000B78
    Data32 = MmPci32 ( 0,0,PcieIpGlobaIpolicy->PcieIpPortBdfMap[ Index ].DevNum,0, R_PCIE_EBERMSK );
    Data32 |= ( UINT32 )V_PCIE_EBERMSK;
    MmPci32 ( 0,0,PcieIpGlobaIpolicy->PcieIpPortBdfMap[ Index ].DevNum,0, R_PCIE_EBERMSK ) = Data32 ;

    //soc.pcies.ports.symlckctl=0x1, offset 0xA00
    Data16 = MmPci16 ( 0,0,PcieIpGlobaIpolicy->PcieIpPortBdfMap[ Index ].DevNum,0, R_PCIE_SYMLCKCTL );
    Data16 &= ( UINT16 )~(BIT0|BIT1);
    Data16 |= ( UINT16 )V_PCIE_SYMLCKCTL;
    MmPci16 ( 0,0,PcieIpGlobaIpolicy->PcieIpPortBdfMap[ Index ].DevNum,0, R_PCIE_SYMLCKCTL ) = Data16 ;

    DEBUG ((EFI_D_INFO, "Selected Gen speed %d  RP %d : 0,%d,0 \n"
		 ,PcieIpGlobaIpolicy->PcieIpPortPolicy[ Index ].PcieSpeed
		 ,Index
		 ,PcieIpGlobaIpolicy->PcieIpPortBdfMap[ Index ].DevNum ));

    Data16  =  MmPci16 ( 0,0,PcieIpGlobaIpolicy->PcieIpPortBdfMap[ Index ].DevNum,0, R_PCIE_LCTL2 );
    if (   (Data16 &  B_PCIE_LCTL2_TLS) != (PcieIpGlobaIpolicy->PcieIpPortPolicy[ Index ].PcieSpeed ) ){
	  ///
	  ///  Change Tarjet Speed only if Setup option selected  Gen1 or Gen2
	  ///
	  DEBUG ((EFI_D_INFO, "Forcing Gen %d on RP %d : 0,%d,0 \n"
		,PcieIpGlobaIpolicy->PcieIpPortPolicy[ Index ].PcieSpeed
			,Index
			,PcieIpGlobaIpolicy->PcieIpPortBdfMap[ Index ].DevNum));

	  Data16 &= ~B_PCIE_LCTL2_TLS;
	  Data16 |=  (UINT16) PcieIpGlobaIpolicy->PcieIpPortPolicy[ Index ].PcieSpeed;
	  MmPci16 ( 0,0,PcieIpGlobaIpolicy->PcieIpPortBdfMap[ Index ].DevNum,0, R_PCIE_LCTL2 )= Data16;
    }

    Data32 = MmPci32 ( 0,0,PcieIpGlobaIpolicy->PcieIpPortBdfMap[ Index ].DevNum,0, R_PCIE_PPD0 );
    Data32 |= B_PCIE_PPD0_ILINKTRN0;
    MmPci32 ( 0,0,PcieIpGlobaIpolicy->PcieIpPortBdfMap[ Index ].DevNum,0, R_PCIE_PPD0 ) = Data32 ;
    DEBUG ((EFI_D_INFO, "Starting link training for PCie RP %d : 0,%d,0 \n",Index,PcieIpGlobaIpolicy->PcieIpPortBdfMap[ Index ].DevNum));
  }
}

VOID
PcieIpConfigureHWEQ_Step3(
  IN UINT8 Device
		)
{
	UINT32 temporalData = 0;

//	eqevalctl       = 0x00000000, offset: 0x000BD0
    temporalData = MmPci32 ( 0,0, Device ,0, R_PCIE_EQEVALCTL );
    temporalData = V_PCIE_EQEVALCTL;
    DEBUG ((EFI_D_INFO, "0x000BD0 %x\n", temporalData ));
    MmPci32 ( 0,0,Device,0, R_PCIE_EQEVALCTL ) = temporalData ;

//	eqevalctl2      = 0x00000ba0, offset: 0x000BD4
    temporalData = MmPci32 ( 0,0,Device,0, R_PCIE_EQEVALCTL2 );
    temporalData = V_PCIE_EQEVALCTL2;
    DEBUG ((EFI_D_INFO, "R_PCIE_EQEVALCTL2 %x\n", temporalData ));
    MmPci32 ( 0,0,Device,0, R_PCIE_EQEVALCTL2 ) = temporalData;

//	eqevalctl3      = 0x33f00802, offset: 0x000BD8
    temporalData = MmPci32 ( 0,0,Device,0, R_PCIE_EQEVALCTL3 );
    temporalData = V_PCIE_EQEVALCTL3;
    DEBUG ((EFI_D_INFO, "R_PCIE_EQEVALCTL3 %x\n", temporalData ));
    MmPci32 ( 0,0,Device,0, R_PCIE_EQEVALCTL3 ) = temporalData;

//	eqevalctl4      = 0x04b30004, offset: 0x000BB8		-> 0x4b20004L wrote
    temporalData = MmPci32 ( 0,0,Device,0, R_PCIE_EQEVALCTL4 );
    temporalData = V_PCIE_EQEVALCTL4;
    DEBUG ((EFI_D_INFO, "R_PCIE_EQEVALCTL4 %x\n", temporalData ));
    MmPci32 ( 0,0,Device,0, R_PCIE_EQEVALCTL4 ) = temporalData;

//	eqevalctl5      = 0x3a801013, offset: 0x000BBC
    temporalData = MmPci32 ( 0,0,Device,0, R_PCIE_EQEVALCTL5 );
    temporalData = V_PCIE_EQEVALCTL5;
    DEBUG ((EFI_D_INFO, "R_PCIE_EQEVALCTL5 %x\n", temporalData ));
    MmPci32 ( 0,0,Device,0, R_PCIE_EQEVALCTL5 ) = temporalData;

//	eqevalctl6      = 0x04904623, offset: 0x000B8C
	temporalData = MmPci32 ( 0,0,Device,0, R_PCIE_EQEVALCTL6 );
	temporalData = V_PCIE_EQEVALCTL6;
	DEBUG ((EFI_D_INFO, "R_PCIE_EQEVALCTL6 %x\n", temporalData ));
	MmPci32 ( 0,0,Device,0, R_PCIE_EQEVALCTL6 ) = temporalData;

//	eqevalctl7      = 0x00000020, offset: 0x000B90
	temporalData = MmPci32 ( 0,0,Device,0, R_PCIE_EQEVALCTL7 );
    temporalData = V_PCIE_EQEVALCTL7;
    DEBUG ((EFI_D_INFO, "R_PCIE_EQEVALCTL7 %x\n", temporalData ));
    MmPci32 ( 0,0,Device,0, R_PCIE_EQEVALCTL7 ) = temporalData;

//	fomincctl       = 0x0004ffff, offset: 0x000BA4
	temporalData = MmPci32 ( 0,0,Device,0, R_PCIE_FOMINCCTL );
    temporalData = V_PCIE_FOMINCCTL;
    DEBUG ((EFI_D_INFO, "fomincctl %x\n", temporalData ));
    MmPci32 ( 0,0,Device,0, R_PCIE_FOMINCCTL ) = temporalData;
}

VOID
PcieIpConfigureHWEQ_Step4(
  IN UINT8 Device,
  IN UINTN Expptmbar
		)
{
  UINT32 temporalData = 0;
  UINT8 IndexOffset;
  UINT32 offsets[] = {
	R_PCIE_FOMHINTTBL0,
	R_PCIE_FOMHINTTBL1,
	R_PCIE_FOMHINTTBL2,
	R_PCIE_FOMHINTTBL3,
	R_PCIE_FOMHINTTBL4,
	R_PCIE_FOMHINTTBL5,
	R_PCIE_FOMHINTTBL6,
	R_PCIE_FOMHINTTBL7,
	R_PCIE_FOMHINTTBL8,
	R_PCIE_FOMHINTTBL9,
	R_PCIE_FOMHINTTBL10,
	R_PCIE_FOMHINTTBL11
	  };
  for( IndexOffset= 0 ; IndexOffset < 12 ; IndexOffset++  ) {
	switch ( IndexOffset ) {
	  case 0: temporalData = V_PCIE_FOMHINTTBL0; break;
	  case 1: temporalData = V_PCIE_FOMHINTTBL1; break;
	  case 2: temporalData = V_PCIE_FOMHINTTBL2; break;
	  case 3: temporalData = V_PCIE_FOMHINTTBL3; break;
	  case 4: temporalData = V_PCIE_FOMHINTTBL4; break;
	  default: temporalData = V_PCIE_FOMHINTTBL5_11; break;
	}
	MmioWrite32 (  Expptmbar + offsets[ IndexOffset ], temporalData);
    DEBUG ((EFI_D_INFO, "device %X, offset %X data %X\n", Device, Expptmbar + offsets[ IndexOffset ], temporalData ));

  }
}

VOID
PcieIpConfigureHWEQ_Step5(
  IN UINTN Expptmbar
		)
{
  UINT32 temporalData = 0;
  UINT32 mask32= 0;
  UINT8 IndexOffset;
  UINT32 offsets[] = {
	R_PCIE_GEN3FARTXCOEFF0,
	R_PCIE_GEN3FARTXCOEFF1,
	R_PCIE_GEN3FARTXCOEFF2,
	R_PCIE_GEN3FARTXCOEFF3,
	R_PCIE_GEN3FARTXCOEFF4,
	R_PCIE_GEN3FARTXCOEFF5,
	R_PCIE_GEN3FARTXCOEFF6,
	R_PCIE_GEN3FARTXCOEFF7
	  };
  for( IndexOffset= 0 ; IndexOffset < 8 ; IndexOffset++  ) {
    MmioAndThenOr32( Expptmbar + offsets[ IndexOffset ],  mask32, temporalData  );
    DEBUG ((EFI_D_INFO, "base extmem R_PCIE_GEN3FARTXCOEFFx: %X data %X\n", Expptmbar + offsets[ IndexOffset ], temporalData ));
  }
}

VOID
PcieIpConfigureHWEQ(
  IN PCIE_IP_PORT_POLICY  *PcieIpPortPolicy
		)
/**
  PCIE link configuration for Gen 3. Done after biffurcation

  Arguments:
  @param[in] PcieIpGlobalPolicy - This is the global PCIE policy

  Returns: none
**/
{
  UINT8 RPindex;
  UINT32 extenderPCIEbar = PEIPCIE_EXPPTMBAR;
  UINT16 Command = PEIPCIE_COMMAND;
  UINT8  devices[]={
	   V_DEVICE_NUMBER_PCIE_ROOT_PORT_0
	  ,V_DEVICE_NUMBER_PCIE_ROOT_PORT_1
	  ,V_DEVICE_NUMBER_PCIE_ROOT_PORT_2
	  ,V_DEVICE_NUMBER_PCIE_ROOT_PORT_3
	  ,V_DEVICE_NUMBER_PCIE_ROOT_PORT_4
	  ,V_DEVICE_NUMBER_PCIE_ROOT_PORT_5
	  ,V_DEVICE_NUMBER_PCIE_ROOT_PORT_6
	  ,V_DEVICE_NUMBER_PCIE_ROOT_PORT_7
  };
  for( RPindex = 0 ; RPindex < PCIE_IP_STANDARD_ROOT_PORT ; RPindex++ ) {
	  if ( PcieIpPortPolicy[ RPindex ].RPBase == (UINT32) (~0) ) {
		  continue;
	  }
	  extenderPCIEbar = PEIPCIE_EXPPTMBAR;
	  Command = PEIPCIE_COMMAND;

	  DEBUG ((EFI_D_INFO, "PCIE GEN3 configuration sequence, - begin for rp %X\n" , RPindex));
	  PcieIpConfigureHWEQ_Step3( devices[ RPindex ]);
	  //configure extender bar
	  MmPci32( 0,0, devices[ RPindex ], 0, R_PCIE_EXPPTMBAR ) = extenderPCIEbar;
	  MmPci32( 0,0, devices[ RPindex ], 0, R_PCIE_COMMAND ) = Command;

	  extenderPCIEbar= MmPci32( 0,0, devices[ RPindex ], 0, R_PCIE_EXPPTMBAR );
	  Command= MmPci16( 0,0, devices[ RPindex ], 0, R_PCIE_COMMAND );
  	  DEBUG ((EFI_D_INFO, "bar %X - command %X\n" , extenderPCIEbar , Command ));
  	  PcieIpConfigureHWEQ_Step4( devices[ RPindex ], (UINT32) PEIPCIE_EXPPTMBAR  );
  	  PcieIpConfigureHWEQ_Step5( (UINT32) PEIPCIE_EXPPTMBAR );

	  //clean extender bar
	  MmPci32( 0,0, devices[ RPindex ], 0, R_PCIE_COMMAND ) = 0;
	  MmPci32( 0,0, devices[ RPindex ], 0, R_PCIE_EXPPTMBAR ) = 0;

	  DEBUG ((EFI_D_INFO, "PCIE GEN3 configuration sequence - end \n" ));
  }
}

VOID
PcieIpRC_Onwer_PSF(
		)
/**
  Define the owner of RS0

  Arguments:
  none

  Returns: none
**/
{
  UINT32 mask = 0;
  UINT32 value = V_PCIE_PSF1_RC_OWNER_RS0;
  DEBUG ((EFI_D_INFO, "soc.psf.psf_1_rc_owner_rs0= 0x40101\n" ));
  //soc.psf.psf_1_rc_owner_rs0= 0x40101
  MmioAndThenOr32 (
	(UINTN) (PCH_PCR_ADDRESS (R_PCIE_PSF1,R_PCIE_PSF1_RC_OWNER_RS0_OFFSET) ),
	mask,
	value);
  //soc.psf.psf_0_rc_owner_rs0= 0x40101
  value = V_PCIE_PSF0_RC_OWNER_RS0;
  MmioAndThenOr32 (
	(UINTN) (PCH_PCR_ADDRESS (R_PCIE_PSF0,R_PCIE_PSF0_RC_OWNER_RS0_OFFSET) ),
	mask,
	value);
}

VOID
PcieIpBifurcationPSF(
  IN PCIE_IP_GLOBAL_POLICY_PPI  *PcieIpGlobaIpolicy		//is an array of cluster
		)
/**
  Bifurcation process

  Arguments:
  @param[in] PcieIpGlobalPolicy - This is the global PCIE policy

  Returns: none
**/
{
  UINT8 bifcode=0 ;
  bifcode = PcieIpGlobaIpolicy->PcieIpClusterPolicy[0].Bifurcation;
  DEBUG ((DEBUG_INFO, "PSF PcieIpBifurcationPSF %x\n", bifcode ));
  switch ( bifcode )
  {
	  case X2X2X2X2:
		  break;
	  case X2X2X4:
		  //blocking port 1
		  MmioOr32 (
			(UINTN) (PCH_PCR_ADDRESS (R_PCIE_PSF1,R_PCIE_PSF1_RP0_OFFSET33) ),
			(UINT32) (B_PCIE_PSF_FUNDIS) );
		  break;
	  case X4X2X2:
		  //blocking port 3
		  MmioOr32 (
			(UINTN) (PCH_PCR_ADDRESS (R_PCIE_PSF1,R_PCIE_PSF1_RP0_OFFSET35) ),
			(UINT32) (B_PCIE_PSF_FUNDIS) );
		  break;
	  case X4X4:
		  //blocking port 1
		  MmioOr32 (
			(UINTN) (PCH_PCR_ADDRESS (R_PCIE_PSF1,R_PCIE_PSF1_RP0_OFFSET33) ),
			(UINT32) (B_PCIE_PSF_FUNDIS) );
		  DEBUG ((DEBUG_INFO, "PSF blocking 1\n" ));
		  //blocking port 3
		  MmioOr32 (
			(UINTN) (PCH_PCR_ADDRESS (R_PCIE_PSF1,R_PCIE_PSF1_RP0_OFFSET35) ),
			(UINT32) (B_PCIE_PSF_FUNDIS) );
		  DEBUG ((DEBUG_INFO, "PSF blocking 3\n" ));
		  break;
	  case X8:
		  //blocking port 1
		  MmioOr32 (
			(UINTN) (PCH_PCR_ADDRESS (R_PCIE_PSF1,R_PCIE_PSF1_RP0_OFFSET33) ),
			(UINT32) (B_PCIE_PSF_FUNDIS) );
		  //blocking port 2
		  MmioOr32 (
			(UINTN) (PCH_PCR_ADDRESS (R_PCIE_PSF1,R_PCIE_PSF1_RP0_OFFSET34) ),
			(UINT32) (B_PCIE_PSF_FUNDIS) );
		  //blocking port 3
		  MmioOr32 (
			(UINTN) (PCH_PCR_ADDRESS (R_PCIE_PSF1,R_PCIE_PSF1_RP0_OFFSET35) ),
			(UINT32) (B_PCIE_PSF_FUNDIS) );
		  break;
  }

  bifcode = PcieIpGlobaIpolicy->PcieIpClusterPolicy[1].Bifurcation;
  switch ( bifcode )
  {
	  case X2X2X2X2:
		  break;
	  case X2X2X4:
		  //blocking port 1
		  MmioOr32 (
			(UINTN) (PCH_PCR_ADDRESS (R_PCIE_PSF1,R_PCIE_PSF1_RP1_OFFSET38) ),
			(UINT32) (B_PCIE_PSF_FUNDIS) );
		  break;
	  case X4X2X2:
		  //blocking port 3
		  MmioOr32 (
			(UINTN) (PCH_PCR_ADDRESS (R_PCIE_PSF1,R_PCIE_PSF1_RP1_OFFSET40) ),
			(UINT32) (B_PCIE_PSF_FUNDIS) );
		  break;
	  case X4X4:
		  //blocking port 1
		  MmioOr32 (
			(UINTN) (PCH_PCR_ADDRESS (R_PCIE_PSF1,R_PCIE_PSF1_RP1_OFFSET38) ),
			(UINT32) (B_PCIE_PSF_FUNDIS) );
		  //blocking port 3
		  MmioOr32 (
			(UINTN) (PCH_PCR_ADDRESS (R_PCIE_PSF1,R_PCIE_PSF1_RP1_OFFSET40) ),
			(UINT32) (B_PCIE_PSF_FUNDIS) );
		  break;
	  case X8:
		  //blocking port 1
		  MmioOr32 (
			(UINTN) (PCH_PCR_ADDRESS (R_PCIE_PSF1,R_PCIE_PSF1_RP1_OFFSET38) ),
			(UINT32) (B_PCIE_PSF_FUNDIS) );
		  //blocking port 2
		  MmioOr32 (
			(UINTN) (PCH_PCR_ADDRESS (R_PCIE_PSF1,R_PCIE_PSF1_RP1_OFFSET39) ),
			(UINT32) (B_PCIE_PSF_FUNDIS) );
		  //blocking port 3
		  MmioOr32 (
			(UINTN) (PCH_PCR_ADDRESS (R_PCIE_PSF1,R_PCIE_PSF1_RP1_OFFSET40) ),
			(UINT32) (B_PCIE_PSF_FUNDIS) );
		  break;
  }
}

VOID
PcieIpBifurcation(
  IN PCIE_IP_GLOBAL_POLICY_PPI  *PcieIpGlobaIpolicy		//is an array of cluster
		)
/**
  Bifurcation process

  Arguments:
  @param[in] PcieIpGlobalPolicy - This is the global PCIE policy

  Returns:
  @retval EFI_SUCCESS - The function completed successfully.
**/
{
  UINT8 bifcode=0 ;

  bifcode = PcieIpGlobaIpolicy->PcieIpClusterPolicy[0].Bifurcation;
  if ( bifcode < VALID_NUMBER_BIFURCATION_MODES  ){
      MmioAndThenOr32 (
			(UINTN) (PCH_PCR_ADDRESS (PID_PCIE0,R_PCIE_PPBIFCTL_PRIV) ),
			(UINT32) ( ~(BIT0|BIT1|BIT2) ),
			(UINT32) ( bifcode ) );
      MmioAndThenOr32 (
			(UINTN) (PCH_PCR_ADDRESS (PID_PCIE0,R_PCIE_PPBIFCTL_PRIV) ),
			(UINT32) ( ~0 ),
			(UINT32) ( B_PCIE_PCR_PPBIFCTLBIF_CL_LOCK ) );

	  DEBUG((DEBUG_INFO, "Configure RC0 : bifurcation code %x \n", bifcode ) );
	  //hide root ports that are not enabled
	  PeiPcieDevHide( PID_PCIE0,  PcieIpGlobaIpolicy->PcieIpClusterPolicy[0].Straps , bifcode);
  } else {
    DEBUG((DEBUG_ERROR, "Wrong bifurcation code %x in cluster 0 \n", bifcode ) );
	ASSERT( FALSE );
  }

  bifcode= PcieIpGlobaIpolicy->PcieIpClusterPolicy[1].Bifurcation;
  if( bifcode < VALID_NUMBER_BIFURCATION_MODES ){
	  MmioAndThenOr32 (
			(UINTN) (PCH_PCR_ADDRESS ( PID_PCIE1,R_PCIE_PPBIFCTL_PRIV) ),
			(UINT32) ( ~(BIT0|BIT1|BIT2) ),
			(UINT32) ( bifcode ) );
	  MmioAndThenOr32 (
			(UINTN) (PCH_PCR_ADDRESS ( PID_PCIE1,R_PCIE_PPBIFCTL_PRIV) ),
			(UINT32) ( ~0 ),
			(UINT32) ( B_PCIE_PCR_PPBIFCTLBIF_CL_LOCK));
	  DEBUG((DEBUG_INFO, "Configure RC1 : bifurcation code %x \n", bifcode ) );
	  //hide root ports that are not enabled
	  PeiPcieDevHide( PID_PCIE1, PcieIpGlobaIpolicy->PcieIpClusterPolicy[1].Straps, bifcode );
  } else {
	DEBUG((DEBUG_ERROR, "Wrong bifurcation code %x in cluster 1 \n", bifcode ) );
	ASSERT( FALSE );
  }
}

EFI_STATUS
PcieIpInitCluster (
  IN PCIE_IP_GLOBAL_POLICY_PPI  *PcieIpGlobaIpolicy,		//is an array of cluster
  IN PCIE_IP_INIT_PHASE         PcieIpInitPhase
  )
/**
  Initialize all clusters configuration, as the bifurcation.

  Arguments:
  @param[in] PcieIpGlobalPolicy - This is the global PCIE policy
  @param[in] PcieIpInitPhase - Define the initialize phase

  Returns:
  @retval EFI_SUCCESS - The function completed successfully.
**/
{
  UINT8      Cluster;
  EFI_STATUS Status;

  Cluster = 0;
  Status = EFI_SUCCESS;

  if ( PcieIpInitPhase == 0 ) {
	DEBUG ((DEBUG_INFO, "PcieIpInitCluster Start, Opcode <<%X >>\n", PEIPCIE_OPCODE_BEFORE_BIFURCATION ));
    PostCode( PEIPCIE_OPCODE_BEFORE_BIFURCATION );
	PcieIpBifurcation( PcieIpGlobaIpolicy );			//< bifurcation in phase 0
	PcieIpBifurcationPSF( PcieIpGlobaIpolicy );
	PcieIpRC_Onwer_PSF() ;
    Status = EFI_SUCCESS;
  }
  else {
    if ( PcieIpInitPhase == 1 ) {
   	  DEBUG ((DEBUG_INFO, "Before link trainning, Opcode <<%X >>\n", PEIPCIE_OPCODE_BEFORE_LINK_TRAINNING ));
      PostCode( PEIPCIE_OPCODE_BEFORE_LINK_TRAINNING );
      //
      // Get FIA configuration for disable lanes, in this moment FIA configuration and trident
      // are equal
      //
      PcieIpPlatformFIA(PcieIpGlobaIpolicy);
      //
      // DNV BIOS WorkAround regarding sighting 1404103991 for stepping A0. Sends PIPE interface to PS State 2 for L1.0
      //
      PcieIpSetPIPEtoPSState2( PcieIpGlobaIpolicy );
      //
      // Init WMP - White Mountain Phy
      //
      PcieIpInitLinkTraining( PcieIpGlobaIpolicy );		//< link training in phase 1
      Status = EFI_SUCCESS;
    } else {
	  Status = RETURN_INVALID_PARAMETER;
	}
  }
  DEBUG ((DEBUG_INFO, "PcieIpInitCluster End\n"));
  return Status;
}

EFI_STATUS
EFIAPI
PcieIpInitSwzCtl (
  IN PCIE_IP_BDF_MAP_ENTRY   *PcieIpPortBdfMap,
  IN UINT8      			  PortIndex
		  )
/*++
  Routine Description:  Program the INTXSWZCTL

  Arguments:
  @param[in] PcieIpPortBdfMap 	- Mapping root port BDF structure array
  @param[in] PortIndex 			- root port number

  Returns:
  EFI_SUCCESS             The function completes successfully
  EFI_OUT_OF_RESOURCES    Insufficient resources to create database
  --*/
{
  EFI_STATUS Status;
  UINT8 	 Values[] = {
		  V_PCIE_INTXSWZCTL_INTA_INTA,
          V_PCIE_INTXSWZCTL_INTA_INTB,
		  V_PCIE_INTXSWZCTL_INTA_INTC,
		  V_PCIE_INTXSWZCTL_INTA_INTD,
		  V_PCIE_INTXSWZCTL_INTA_INTA,
		  V_PCIE_INTXSWZCTL_INTA_INTB,
		  V_PCIE_INTXSWZCTL_INTA_INTC,
		  V_PCIE_INTXSWZCTL_INTA_INTD
  	  };

  if ( PcieIpPortBdfMap == NULL ) {
	  Status = RETURN_INVALID_PARAMETER;
	  return Status;
  }
  DEBUG ((EFI_D_INFO, "PcieIpInitSwzCtl  start\n"));
  MmPci8AndThenOr(0, 0,PcieIpPortBdfMap[ PortIndex ].DevNum , 0, R_PCIE_INTXSWZCTL, ~V_PCIE_INTXSWZCTL_SWIZZLE_MASK, Values[ PortIndex ] );

  //
  // This executes after Root Ports are out of reset.
  //
  //
  // DNV Interrupt Routing
  //Device	|DevNum |PIR-Register|	ITSS-Reg-Offset| PIR-INTA|	 PIR-INTB|	 PIR-INTC|	           PIR-INTD|  IntA   |IntB |	IntC|	IntD
  //PCIE0	  D9F0	      PIR7	       0x314e	             PIRQA	PIRQB	PIRQC	   PIRQD	 INTXSWZCTL	INTA->INTA	PIRQA	  PIRQB	PIRQC	PIRQD
  //PCIE0A	D10F0	      PIR7	       0x314e	             PIRQA	PIRQB	PIRQC	   PIRQD	 INTXSWZCTL	INTA->INTB	PIRQB	  PIRQC	PIRQD	PIRQA
  //PCIE0B	D11F0	      PIR7	       0x314e	             PIRQA	PIRQB	PIRQC	   PIRQD	 INTXSWZCTL	INTA->INTC	PIRQC	  PIRQD	PIRQA	PIRQB
  //PCIE0C	D12F0	      PIR7	       0x314e	             PIRQA	PIRQB	PIRQC	   PIRQD	 INTXSWZCTL	INTA->INTD	PIRQD	  PIRQA	PIRQB	PIRQC
  //PCIE1	  D14F0	      PIR8	       0x3150	             PIRQE	PIRQF	PIRQG	   PIRQH	 INTXSWZCTL	INTA->INTA	PIRQE	  PIRQF	PIRQG	PIRQH
  //PCIE1A	D15F0	      PIR8	       0x3150	             PIRQE	PIRQF	PIRQG	   PIRQH	 INTXSWZCTL	INTA->INTB	PIRQF	  PIRQG	PIRQH	PIRQE
  //PCIE1B	D16F0	      PIR8	       0x3150	             PIRQE	PIRQF	PIRQG	   PIRQH	 INTXSWZCTL	INTA->INTC	PIRQG	  PIRQH	PIRQE	PIRQF
  //PCIE1C	D17F0	      PIR8	       0x3150	             PIRQE	PIRQF	PIRQG	   PIRQH	 INTXSWZCTL	INTA->INTD	PIRQH	  PIRQE	PIRQF	PIRQG
  //
  return EFI_SUCCESS;
}

VOID
PcieIpClockGatingControl (
  IN UINT8      PortIndex,
  IN UINT8      ClockGatingEnFlag,
  IN UINT8      PortType,
  IN UINTN      RPBase
)
/**
  This is the function to enable the clock gating for PCI Express Virtual Root Ports.

  Arguments:
  @param[in] PortIndex            Root Port Index Number.
  @param[in] ClockGatingEnFlag    Enable or Disable flag consumed by this function.
  @param[in] PortType             Identifies Port as Root Port or Virtual Root Port.
  @param[in] RPBase               Root port base address

  Returns:
  NONE
**/
{
  ///
  /// do nothing if rp is not in use
  ///
  if ( (RPBase == (UINTN)INVALID_ROOT_PORT_NUMBER) || (RPBase == (UINTN)PCI_DEVICE_IS_NOT_PRESENT)){
    DEBUG ((EFI_D_INFO, "Invalid Root Port Number or Virtual Root Port : %d\n",PortIndex) );
    return ;
  }

    if (ClockGatingEnFlag == 0) {
      DEBUG ((EFI_D_INFO, "Disabling Clock Gating settings \n"));

      ///
      /// Same settings needs to be disable for both RP and VRP
      ///
      MmioAnd16 ( RPBase +  R_CFGPCIECLKGCTL ,  (UINT16) ~B_IDLE_CLOCK_GATING_ENABLE );
      MmioAnd16 ( RPBase +  R_IOSFDEVCLKGCTL ,  (UINT16) ~B_IDLE_CLOCK_GATING_ENABLE );
//      MmioAnd16 ( RPBase +  R_SBDEVCLKGCTL   ,  (UINT16) ~B_IDLE_CLOCK_GATING_ENABLE );
      MmioAnd16 ( RPBase +  R_PT0TXNCLKGCTL  ,  (UINT16) ~B_IDLE_CLOCK_GATING_ENABLE );
      MmioAnd16 ( RPBase +  R_PT0LNKCLKGCTL  ,  (UINT16) ~B_IDLE_CLOCK_GATING_ENABLE );

	  if (PortType == StandardRootPort ){
		///
		/// PT{1,2,3}TX|LNK  is only needed for Root Ports
		///
		MmioAnd16 ( RPBase +  R_PT1TXNCLKGCTL  ,  (UINT16) ~B_IDLE_CLOCK_GATING_ENABLE );
		MmioAnd16 ( RPBase +  R_PT2TXNCLKGCTL  ,  (UINT16) ~B_IDLE_CLOCK_GATING_ENABLE );
		MmioAnd16 ( RPBase +  R_PT3TXNCLKGCTL  ,  (UINT16) ~B_IDLE_CLOCK_GATING_ENABLE );

		MmioAnd16 ( RPBase +  R_PT1LNKCLKGCTL  ,  (UINT16) ~B_IDLE_CLOCK_GATING_ENABLE );
		MmioAnd16 ( RPBase +  R_PT2LNKCLKGCTL  ,  (UINT16) ~B_IDLE_CLOCK_GATING_ENABLE );
		MmioAnd16 ( RPBase +  R_PT3LNKCLKGCTL  ,  (UINT16) ~B_IDLE_CLOCK_GATING_ENABLE );

		//
		// phy apply if is a root port
		//
		MmioAnd16 ( RPBase +  R_PT0PHYCLKGCTL  ,  (UINT16) ~B_IDLE_CLOCK_GATING_ENABLE );
		MmioAnd16 ( RPBase +  R_PT1PHYCLKGCTL  ,  (UINT16) ~B_IDLE_CLOCK_GATING_ENABLE );
		MmioAnd16 ( RPBase +  R_PT2PHYCLKGCTL  ,  (UINT16) ~B_IDLE_CLOCK_GATING_ENABLE );
		MmioAnd16 ( RPBase +  R_PT3PHYCLKGCTL  ,  (UINT16) ~B_IDLE_CLOCK_GATING_ENABLE );

		///
		///Only bit 0 is valid in the PSE field
		///
		MmioAnd32 ( RPBase +  R_PLLSTBYCTL     , (UINT32) ~B_PLL_STANDBY_ENABLE );
	  }else {
	    //
		// this only apply  for Virtual Root Ports
		//
		MmioAnd16 ( RPBase +  R_IDFDEVCLKGCTL  , (UINT16) ~B_IDLE_CLOCK_GATING_ENABLE );
	  }

    }else {
      //
      //ENABLE Clock gating, common settings for RP and VRP
      //
      DEBUG ((EFI_D_INFO, "Enabling Clock Gating settings \n"));

      MmioOr16 ( RPBase +  R_IOSFDEVCLKGCTL ,  (UINT16) B_IDLE_CLOCK_GATING_ENABLE );
      MmioOr16 ( RPBase +  R_SBDEVCLKGCTL   ,  (UINT16) B_IDLE_CLOCK_GATING_ENABLE );
      MmioAnd16 ( RPBase +  R_PT0TXNCLKGCTL  ,  (UINT16) ~B_IDLE_CLOCK_GATING_ENABLE );
      MmioOr16 ( RPBase +  R_PT0LNKCLKGCTL  ,  (UINT16) B_IDLE_CLOCK_GATING_ENABLE );
      MmioOr16 ( RPBase +  R_CFGPCIECLKGCTL ,  (UINT16) B_IDLE_CLOCK_GATING_ENABLE );

	  if ( PortType == StandardRootPort ){
		///
		/// PT{1,2,3}TX|LNK  is only needed for Root Ports
		///
		MmioAnd16 ( RPBase +  R_PT1TXNCLKGCTL  ,  (UINT16) ~B_IDLE_CLOCK_GATING_ENABLE );
		MmioAnd16 ( RPBase +  R_PT2TXNCLKGCTL  ,  (UINT16) ~B_IDLE_CLOCK_GATING_ENABLE );
		MmioAnd16 ( RPBase +  R_PT3TXNCLKGCTL  ,  (UINT16) ~B_IDLE_CLOCK_GATING_ENABLE );

		MmioOr16 ( RPBase +  R_PT1LNKCLKGCTL  ,  (UINT16) B_IDLE_CLOCK_GATING_ENABLE );
		MmioOr16 ( RPBase +  R_PT2LNKCLKGCTL  ,  (UINT16) B_IDLE_CLOCK_GATING_ENABLE );
		MmioOr16 ( RPBase +  R_PT3LNKCLKGCTL  ,  (UINT16) B_IDLE_CLOCK_GATING_ENABLE );

		MmioOr16 ( RPBase +  R_PT0PHYCLKGCTL  ,  (UINT16) B_IDLE_CLOCK_GATING_ENABLE );
		MmioOr16 ( RPBase +  R_PT1PHYCLKGCTL  ,  (UINT16) B_IDLE_CLOCK_GATING_ENABLE );
		MmioOr16 ( RPBase +  R_PT2PHYCLKGCTL  ,  (UINT16) B_IDLE_CLOCK_GATING_ENABLE );
		MmioOr16 ( RPBase +  R_PT3PHYCLKGCTL  ,  (UINT16) B_IDLE_CLOCK_GATING_ENABLE );

		///
		///Only bit 0 is valid in the PSE field
		///
		MmioOr32 ( RPBase +  R_PLLSTBYCTL     , (UINT32) B_PLL_STANDBY_ENABLE );
	  }else {
	    MmioOr16 ( RPBase +  R_IDFDEVCLKGCTL  , (UINT16) B_IDLE_CLOCK_GATING_ENABLE );
	  }
    }
//
  ;
}

EFI_STATUS
PcieIpConnectSlotWithRootPort (
  UINTN         			RPBase
  )
/**
  Indicate that this root port has really connected a slot.

  Arguments:
  @param[in] PcieIpPortPolicy 	- This is the global PCIE policy
  @param[in] PortIndex 			- Port number
  @param[in] RPBase 			- root port base address

  Returns:
  @retval EFI_SUCCESS - The function completed successfully.
**/
{
  EFI_STATUS Status;
  UINT16      Data16 = 0;

  Status = EFI_SUCCESS;

  Data16 = MmioRead16( RPBase +  R_PCH_PCIE_XCAP );
  Data16 &= (UINT16) ~B_PCH_PCIE_XCAP_SI;
  Data16 |= (UINT16) B_PCH_PCIE_XCAP_SI;
  MmioWrite16( RPBase +  R_PCH_PCIE_XCAP , Data16 );

  return Status;
}

EFI_STATUS
PcieIpInitL2State (
  UINTN         			RPBase
  )
/**
  Configure each root port to L2 sate

  Arguments:
  @param[in] RPBase 			- root port base address

  Returns:
  @retval EFI_SUCCESS - The function completed successfully.
**/
{
  EFI_STATUS Status;
  UINT32      Data32 = 0;

  Status = EFI_SUCCESS;

  Data32 = MmioRead32( RPBase +  R_PCIE_PIPECTL2 ) ;
  Data32 &= (UINT32) ~(BIT28|BIT29|BIT30);
  Data32 |= (UINT32) BIT29;
  MmioWrite32( RPBase +  R_PCIE_PIPECTL2 , Data32 );

  return Status;
}

EFI_STATUS
PcieIpInitSingleRootPort (
  IN PCIE_IP_PORT_POLICY    *PcieIpPortPolicy,
  IN UINT8      			PortIndex,
  UINTN         			RPBase
  )
/**
  Configuration each one single root port

  Arguments:
  @param[in] PcieIpPortPolicy 	- This is the global PCIE policy
  @param[in] PortIndex 			- Port number
  @param[in] RPBase 			- root port base address

  Returns:
  @retval EFI_SUCCESS - The function completed successfully.
**/
{
  EFI_STATUS Status;
  UINT32      Data32 = 0;

  Status = EFI_SUCCESS;

  Data32 = MmioRead32( RPBase +  R_PCIE_SOSCTL ) ;
  Data32 &= (UINT32) ~V_PCIE_SOSCTL_MASK;
  Data32 |= (UINT32) V_PCIE_SOSCTL;
  MmioWrite32( RPBase +  R_PCIE_SOSCTL , Data32 );

  if ( PcieIpPortPolicy[ PortIndex ].VirtualRootPort == 0 ) {
	  PcieIpClockGatingControl( PortIndex , PcieIpPortPolicy[ PortIndex ].ClockGatingEn, StandardRootPort , RPBase );
  } else {
	  PcieIpClockGatingControl( PortIndex , 0, VirtualRootPort , RPBase );
  }
  return Status;
}

EFI_STATUS
PcieIpInitPhase0(
  IN PCIE_IP_GLOBAL_POLICY_PPI *PcieIpGlobaIpolicy
)
/**
  PCIE init Phase 0

  Arguments:
  @param[in] PcieIpGlobalPolicy - This is the global PCIE policy

  Returns:
  @retval EFI_SUCCESS - The function completed successfully.
**/
{
  EFI_STATUS Status;
  Status = PcieIpInitCluster( PcieIpGlobaIpolicy, Phase0 );
  ASSERT_EFI_ERROR( Status  );
  return Status;
}

EFI_STATUS
PcieIpInitPhase1(
  IN PCIE_IP_GLOBAL_POLICY_PPI *PcieIpGlobaIpolicy
)
/**
  PCIE init Phase 1

  Arguments:
  @param[in] PcieIpGlobalPolicy - This is the global PCIE policy

  Returns:
  @retval EFI_SUCCESS - The function completed successfully.
**/
{
  EFI_STATUS Status;
  UINT8 Index;

  Status = EFI_SUCCESS;
  if ( PcieIpGlobaIpolicy == NULL ) {
	Status = RETURN_INVALID_PARAMETER;
    ASSERT_EFI_ERROR( Status  );
    return Status;
  }
  PcieIpLaneEQPresetFeature(
    PcieIpGlobaIpolicy->PcieIpClusterPolicy[0].Bifurcation,
    PcieIpGlobaIpolicy->PcieIpClusterPolicy[1].Bifurcation,
	PcieIpGlobaIpolicy->PcieIpPortPolicy
	);
  //Initialize root port
  for ( Index = 0; Index < PCIE_IP_STANDARD_ROOT_PORT ; Index++ ) {
	  if ( PcieIpGlobaIpolicy->PcieIpPortBdfMap[ Index ].RPBase == (UINTN)PCI_DEVICE_IS_NOT_PRESENT ) {
		  continue;
	  }
	  PcieIpInitSingleRootPort( PcieIpGlobaIpolicy->PcieIpPortPolicy , Index , PcieIpGlobaIpolicy->PcieIpPortBdfMap[ Index ].RPBase );
	  PcieIpInitL2State( PcieIpGlobaIpolicy->PcieIpPortBdfMap[ Index ].RPBase );
	  //intswzz
	  PcieIpInitSwzCtl( PcieIpGlobaIpolicy->PcieIpPortBdfMap, Index );
	  //PcieIpConnectSlotWithRootPort( PcieIpGlobaIpolicy->PcieIpPortBdfMap[ Index ].RPBase );
  }
  PcieIpInitCluster( PcieIpGlobaIpolicy, Phase1 );
  return Status;
}

UINTN
PcieIpGetMmBase(
  PCIE_IP_PORT_POLICY *PcieIpPortPolicy
  )
{
  return 0;
}

VOID
PeiIpConfigure (
  IN PCIE_IP_GLOBAL_POLICY_PPI *PcieIpGlobaIpolicy
)
/**
  set PCIE configuration options

  Arguments:
  @param[in] PcieIpGlobaIpolicy - This is the global PCIE policy

  Returns:
  @retval EFI_SUCCESS - The function completed successfully.
**/{
  UINT8 Index;
  EFI_STATUS Status;
  BOOLEAN FlagLtrEnable = FALSE;
  //General Configuration by cluster
  //Set PCIE root port replay timer timeout
  //The ports 1,2 and 3 are alias of port 0
  PeiPcieSetRTTO(
    PcieIpGlobaIpolicy->PcieIpPortPolicy[ 0 ].RPBase,
  	PcieIpGlobaIpolicy->PcieIpClusterPolicy[ 0 ].ReplyTimerTimeout );
    DEBUG ((DEBUG_INFO, "Root port replay timer 0 -> %X \n" , PcieIpGlobaIpolicy->PcieIpClusterPolicy[ 0 ].ReplyTimerTimeout ));
  //The ports 5,6 and 7 are alias of port 4
  PeiPcieSetRTTO(
  	PcieIpGlobaIpolicy->PcieIpPortPolicy[ 4 ].RPBase,
  	PcieIpGlobaIpolicy->PcieIpClusterPolicy[ 1 ].ReplyTimerTimeout );
    DEBUG ((DEBUG_INFO, "Root port replay timer 4 -> %X \n" , PcieIpGlobaIpolicy->PcieIpClusterPolicy[ 1 ].ReplyTimerTimeout));

//for each root port set setup options
  for ( Index = 0; Index < PCIE_IP_STANDARD_ROOT_PORT ; Index++ ) {
	if ( PcieIpGlobaIpolicy->PcieIpPortPolicy[ Index ].RPBase == V_ERROR_PCI_DEVICE_IS_NOT_PRESENT ) {
		continue;
	}
	//Disables the ability of downstream relaxed ordered completions from passing posted requests in the same direction
	PeiPcieWACpl( PcieIpGlobaIpolicy->PcieIpPortPolicy[ Index ].RPBase );

	//LTSSMTRNCTL.ENFORCEDREC = 0, EB errors don't cause a recovery entry
	PeiPcieWAEBErrors( PcieIpGlobaIpolicy->PcieIpPortPolicy[ Index ].RPBase );

	DEBUG ((DEBUG_INFO, "root port lock %d -> %X \n", Index , PcieIpGlobaIpolicy->PcieIpPortPolicy[ Index ].Lock ));

	//	Clean DEVCAP2 register ( with 0's )
    PeiPcieDevCap2( PcieIpGlobaIpolicy->PcieIpPortPolicy[ Index ].RPBase );
    //	Clean PHYRECAL register ( with 0's )
    PeiPciePhyRecal( PcieIpGlobaIpolicy->PcieIpPortPolicy[ Index ].RPBase );
	DEBUG ((DEBUG_INFO, "aspm value %d \n", PcieIpGlobaIpolicy->PcieIpPortPolicy[ Index ].Aspm ));
	PeiPcieSetASPML1(
      PcieIpGlobaIpolicy->PcieIpPortPolicy[ Index ].Aspm,
      PcieIpGlobaIpolicy->PcieIpPortPolicy[ Index ].RPBase);
	PeiPcieStopScream(
	  (BOOLEAN) PcieIpGlobaIpolicy->PcieIpPortPolicy[ Index ].StopScream,
	  PcieIpGlobaIpolicy->PcieIpPortPolicy[ Index ].RPBase);
//  configure Dev Ctl register ( DxePcieDevCtlInit)
	Status = PeiPcieDevCtlInit ( PcieIpGlobaIpolicy->PcieIpPortPolicy, Index );
	ASSERT_EFI_ERROR( Status  );
//  configure Dev Ctl register ( DxePcieDevCtlInit)
	Status = PeiPcieDevCtl2Init ( PcieIpGlobaIpolicy->PcieIpPortPolicy, Index );
	ASSERT_EFI_ERROR( Status  );
//  Configure Extended Synch ( DxePcieLnkCtl)
	Status = PeiPcieLnkCtl (PcieIpGlobaIpolicy->PcieIpPortPolicy, Index );
	ASSERT_EFI_ERROR( Status  );
//  Lane Reversal, Activate  bit 5  LTSSMTRNCTL  ( DxePcieCtlLaneReversal)
	Status = PeiPcieCtlLaneReversal(
      PcieIpGlobaIpolicy->PcieIpPortPolicy[ Index ].CtlLaneReversal ,
	  PcieIpGlobaIpolicy->PcieIpPortPolicy[ Index ].RPBase );
	ASSERT_EFI_ERROR( Status  );
//      PCIE DeEmphasis ( DxePcieSelectDeEmphasis)
	Status = PeiPcieSelectDeEmphasis (
	  PcieIpGlobaIpolicy->PcieIpPortPolicy[ Index ].DeEmphasis,
	  PcieIpGlobaIpolicy->PcieIpPortPolicy[ Index ].RPBase	);
	ASSERT_EFI_ERROR( Status  );
	PeiPcieSurpriseLinkDownControl(
	  PcieIpGlobaIpolicy->PcieIpPortPolicy[ Index ].DeviceCtlConfig.EnableSLD,
	  PcieIpGlobaIpolicy->PcieIpPortPolicy[ Index ].RPBase);
	FlagLtrEnable = ( BOOLEAN )PcieIpGlobaIpolicy->PcieIpPortPolicy[ Index ].DeviceCtlConfig.LtrEnable;
	PcieIpPlatformLTR( PcieIpGlobaIpolicy->PcieIpPortPolicy[ Index ].RPBase, FlagLtrEnable );
	if ( PcieIpGlobaIpolicy->PcieIpPortBdfMap[ Index ].Enabled ) {
	  PcieIpPlatformSlotImplemented(PcieIpGlobaIpolicy->PcieIpPortPolicy[ Index ].RPBase,
			PcieIpGlobaIpolicy->PcieIpPortBdfMap[ Index ].PortNumber,
			(BOOLEAN) PcieIpGlobaIpolicy->PcieIpPortPolicy[ Index ].HotPlug );
	}
	else {
	  DEBUG ((DEBUG_INFO, "This root port %d is not active\n", Index ));
	}
	if ( PcieIpGlobaIpolicy->PcieIpPortPolicy[ Index ].HotPlug == 1 ) {
	  PcieIpPlatformHotPlug( PcieIpGlobaIpolicy->PcieIpPortPolicy[ Index ].RPBase );
	}
	else {
	  DEBUG ((DEBUG_INFO, "This root port %d is not active with HotPlug\n", Index ));
	}
	//lock pcie registers
	PeiPcieSetLock(
	  PcieIpGlobaIpolicy->PcieIpPortPolicy[ Index ].Lock,
	  PcieIpGlobaIpolicy->PcieIpPortPolicy[ Index ].RPBase);
  }
}

/* *********************************** public function  *********************************** */

EFI_STATUS
PcieIpInit (
  IN PCIE_IP_GLOBAL_POLICY_PPI *PcieIpGlobaIpolicy,
  IN PCIE_IP_INIT_PHASE         PcieIpInitPhase
  )
/**
  Initialize Pcie IP

  Arguments:
  @param[in] PcieIpGlobalPolicy - This is the global PCIE policy
  @param[in] PcieIpInitPhase - Define the initialize phase

  Returns:
  @retval EFI_SUCCESS 				- The function completed successfully.
  @retval RETURN_INVALID_PARAMETER 	- invalid parameter
**/
{
  EFI_STATUS Status;

  DEBUG ((DEBUG_INFO, "PcieIpInit Start\n"));
  Status = EFI_SUCCESS;

  if ( PcieIpInitPhase == 0 ) {
    Status = PcieIpInitPhase0( PcieIpGlobaIpolicy );
    ASSERT_EFI_ERROR( Status  );
	Status = PcieIpPlatformPortBdfMap( PcieIpGlobaIpolicy );
	ASSERT_EFI_ERROR( Status  );
	PcieIpConfigureHWEQ( PcieIpGlobaIpolicy->PcieIpPortPolicy);		//< Enable HW EQ
    DEBUG ((EFI_D_INFO, "PcieIpConfigureLinkSequence end\n"  ));
  }
  else {
	if ( PcieIpInitPhase == 1 ) {
	  Status = PcieIpInitPhase1( PcieIpGlobaIpolicy );
	  PeiIpConfigure( PcieIpGlobaIpolicy );
	}
	else {
	  Status = RETURN_INVALID_PARAMETER;
	}
  }

  DEBUG ((DEBUG_INFO, "PcieIpInit End\n"));
  return Status;
}


