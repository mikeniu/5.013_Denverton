//**********************************************************************//
//**********************************************************************//
//**                                                                  **//
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **//
//**                                                                  **//
//**                       All Rights Reserved.                       **//
//**                                                                  **//
//**       5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093       **//
//**                                                                  **//
//**                       Phone: (770)-246-8600                      **//
//**                                                                  **//
//**********************************************************************//
//**********************************************************************//

//<AMI_FHDR_START>
//----------------------------------------------------------------------------
//
// Name: RtErrorLogWhea.c
//
// Description:	
// 
//----------------------------------------------------------------------------
//<AMI_FHDR_END>

#include "RtErrorLogWhea.h"

BOOLEAN						mWheaLogStarted = FALSE;

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure: LogPciErrorToWhea
//
// Description: This function logs pci error to Whea
//
// Input:
//  UINT8    Bus
//  UINT8    Dev
//  UINT8    Func
//  BOOLEAN  IsBridge
//  UINT16   SecondaryStatus
//  UINT8    PcieOffset
//  UINT16   PcieDeviceStatus
//  UINT16   AerOffset
//  UINT32   UncorrErrStatus
//  UINT32   CorrErrStatus
//  UINT32   RootErrStatus
//
// Output: EFI_STATUS - EFI_SUCCESS
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
// Denverton AptioV Override Start - EIP#275412
EFI_STATUS
LogPciErrorToWhea(
	IN	  UINT8       ErrorSource,
	IN	  UINT8       ErrorType,
	IN	  UINT8       Segment,
	IN	  UINT8       Bus,
	IN	  UINT8       Device,
	IN	  UINT8       Function)
{
	PCIE_PCI_DEV_INFO PciDev;
	UINT16            WheaErrType;

	PciDev.Segment      = Segment;
	PciDev.Bus          = Bus;
	PciDev.Device       = Device;
	PciDev.Function     = Function;

	  switch (ErrorType) {

	    case SERROR_TYPE:
	      WheaErrType             = GEN_ERR_SEV_PCI_DEV_FATAL;
	      break;

	    case PERROR_TYPE:
	      WheaErrType             = GEN_ERR_SEV_PCI_DEV_FATAL;
	      break;

	    case COR_ERROR_TYPE:
	      WheaErrType             = GEN_ERR_SEV_PCIE_CORRECTED;
	      break;

	    case NONFATAL_ERROR_TYPE:
	        WheaErrType             = GEN_ERR_SEV_PCIE_RECOVERABLE;
	      break;

	    case FATAL_ERROR_TYPE:
	      WheaErrType             = GEN_ERR_SEV_PCIE_FATAL;
	      break;

	    default:
	      return EFI_SUCCESS;
	  }

	  if (mWheaPlatformSupportAvailable == TRUE) {
	    if (WheaErrType == FATAL_ERROR_TYPE || WheaErrType == GEN_ERR_SEV_PCI_DEV_CORRECTED || \
	        WheaErrType == GEN_ERR_SEV_PCI_DEV_FATAL || WheaErrType == GEN_ERR_SEV_PCI_DEV_INFORMATION) {
	       mWheaPlatformSupport->WheaPlatformElogPciDev (mWheaPlatformSupport, WheaErrType, &PciDev);
	    } else {
	       mWheaPlatformSupport->WheaPlatformElogPcieRootDevBridge (mWheaPlatformSupport, WheaErrType, &PciDev);
	    }
	  }

    return EFI_SUCCESS;
}
// Denverton AptioV Override End - EIP#275412
EFI_STATUS
LogMemErrorToWhea(
    IN UINT8    Type,
    MEMORY_DEV_INFO  *MemInfo
 )
/*++

Routine Description:
    This function modifies the Uncorrectable and correctable error block entries for status
    Error severity 

Arguments:
  ErrorType - The type of error that needs to be injected

Returns:
	None 	

--*/
{
	if( mWheaPlatformSupportAvailable == TRUE) {
        //
		// Start Whea error record log
		//
		if(mWheaLogStarted == FALSE)	{
		  mWheaPlatformSupport->WheaPlatformRestartAllElog (mWheaPlatformSupport);
		  mWheaLogStarted = TRUE;
		}

		if((mErrLogSetupData.WheaLogging == TRUE) && (Type == COR_ERROR_TYPE)) {
			// prepare the error status block in HEST table.
			mWheaPlatformSupport->WheaPlatformElogMemory (
				mWheaPlatformSupport,
				GEN_ERR_SEV_PLATFORM_MEMORY_CORRECTED,
                MemInfo
			    );
		}

		if((mErrLogSetupData.WheaLogging == TRUE) && (Type == RECOVERABLE_ERROR_TYPE)) {
			// prepare the error status block in HEST table.
			mWheaPlatformSupport->WheaPlatformElogMemory (
				mWheaPlatformSupport,
				GEN_ERR_SEV_PLATFORM_MEMORY_RECOVERABLE,
                MemInfo
				);
		}

		if((mErrLogSetupData.WheaLogging == TRUE) && (Type == FATAL_ERROR_TYPE)) {
			// prepare the error status block in HEST table.
			mWheaPlatformSupport->WheaPlatformElogMemory (
				mWheaPlatformSupport,
				GEN_ERR_SEV_PLATFORM_MEMORY_FATAL,
                MemInfo
			    );
		}
			// EndWheaElog generates NMI for uncorrectable errors
			// mWheaPlatformSupport->WheaPlatformEndElog (mWheaPlatformSupport); // Denverton AptioV Override - EIP#275412

	}

    return EFI_SUCCESS;
}



//**********************************************************************//
//**********************************************************************//
//**                                                                  **//
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **//
//**                                                                  **//
//**                       All Rights Reserved.                       **//
//**                                                                  **//
//**        5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093      **//
//**                                                                  **//
//**                       Phone: (770)-246-8600                      **//
//**                                                                  **//
//**********************************************************************//
//**********************************************************************//
