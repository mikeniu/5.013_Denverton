//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

//*************************************************************************
/** @file NbPciDxeInitLib.c
    This file provide whole NB devices PCI initialized routine for PciBus
    call out.

    This template just provide two example.
    If you want append or modify please follow below naming rule.

    1. Append CspNbPciInit prior to your particular device name.
    for example : CspNbPciInitPciRoot, CspNbPciInitXXX

*/

#include <AmiDxeLib.h>
#include <Token.h>
#include <PciBus.h>
#include <PciHostBridge.h>
#include <Setup.h>
#include <Protocol\PciRootBridgeIo.h>
#include <Protocol\PciIo.h>
#include <Protocol\AmiBoardInitPolicy.h>
#include <Library\AmiPciBusLib.h>
#include <Nb.h>

/**
    This function is called by PCI Bus driver for NB PCI devices 
    initialization.

    @param This - Pointer to protocol AMI_BOARD_INIT_PROTOCOL.
    @param Function - Pointer to Function variable.
    @param ParameterBlock - Pointer to the standard parameter block.

    @retval EFI_STATUS

**/
EFI_STATUS CspNbPciInitPciRoot (
	IN AMI_BOARD_INIT_PROTOCOL				*This,
	IN UINTN								*Function,
	IN OUT AMI_BOARD_INIT_PARAMETER_BLOCK	*ParameterBlock
)
{
	// Update Standard parameter block
	AMI_BOARD_INIT_PARAMETER_BLOCK	*Args = ParameterBlock;
	PCI_INIT_STEP        			InitStep = (PCI_INIT_STEP)Args->InitStep;
    PCI_DEV_INFO   					*dev = (PCI_DEV_INFO*)Args->Param1;
    EFI_STATUS  					Status = EFI_UNSUPPORTED;
    EFI_PCI_IO_PROTOCOL 			*PciIo = &dev->PciIo;

    //---------------------------------
    // Check if parameters passed are VALID and
    if(Args->Signature != AMI_PCI_PARAM_SIG) return EFI_INVALID_PARAMETER;
    if(InitStep>=isPciMaxStep) return EFI_INVALID_PARAMETER;

	// ==========================
	// !!!	Porting required !!!!
    // ==========================
    if (InitStep == isPciGetOptionRom) {
    	// An example when PciBus call this device in "isPciGetOptionRom" case.
    	// ....
		//  This is a sample for On Board device.
		//	Just a sample looking for video ROM/GOP
    	if (dev->PciIo.RomImage == NULL) {
			// ==========================
			// !!!	Porting required !!!!
			// ==========================
			//   .....
			/*
			UINTN	i;

			for (i = 0; i < dev->SdlDevCount; i++) {
				// find legacy ROM.
				Status = PciBusReadNextEmbeddedRom(dev, i, &romSdlData, &RomFile, &RomSize);
				if(EFI_ERROR(Status)) break;
				//####ifdef AMI_MODULE_PKG_VERSION
				//####if AMI_MODULE_PKG_VERSION > 18
				if (romSdlData->PciDevFlags.Bits.UefiDriverBin == 1) continue;

				StdRomHdr = (PCI_STD_OPT_ROM_HEADER*)RomFile;
				//STD ROM Image... read VID/DID info from PCIR structure.
				PcirStruct = (PCI_DATA_STRUCTURE*)(((UINT8*)RomFile) + StdRomHdr->PcirOffset);
				// update DID in the PCIR header.
				PcirStruct->DeviceId = dev->DevVenId.DevId;

				dev->PciIo.RomImage = RomFile;
				dev->PciIo.RomSize = RomSize;
				//Update Capabilities...
				dev->Capab |= (EFI_PCI_IO_ATTRIBUTE_EMBEDDED_ROM);
				dev->Attrib  |= (EFI_PCI_IO_ATTRIBUTE_EMBEDDED_ROM);
				break;
			}*/
    	}
    	Status = EFI_SUCCESS;
    }

    	// For other case please refer to AmiBoardInitPolicy.h
		//	typedef enum {
		//		isPciNone			= 0,	//0
		//		isPciGetSetupConfig,		//1
		//		isPciSkipDevice,			//2
		//		isPciSetAttributes,			//3
		//		isPciProgramDevice,			//4
		//		isPcieInitLink,				//5
		//		isPcieSetAspm,				//6
		//		isPcieSetLinkSpeed,			//7
		//		isPciGetOptionRom,			//8
		//		isPciOutOfResourcesCheck,	//9
		//		isPciReadyToBoot,			//10
		//		isPciQueryDevice,			//11
		//		isHbBasicInit,				//12
		//		isRbCheckPresence,			//13
		//		isRbBusUpdate,				//14
		//		isPciMaxStep				//15...so far
		//	} PCI_INIT_STEP;

    // ....

    return Status;
}

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
