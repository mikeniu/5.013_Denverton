//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file SbPciDxeInitLib.c
    This file provide whole SB devices PCI initialize routine for 
    PciBus call out.

    This template just provide two example.
    If you want append or modify please follow below naming rule.

    1. Append CspSbPciInit prior to your particular device name.
    for example : CspSbPciInitLpcBridge, CspSbPciInitEhci0

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
#include <Sb.h>
#include <SetupVariable.h>                // Denverton Aptiov override - EIP#379099
// Denverton AptioV override Start - EIP#235471
#include "Register\PchRegsSpi.h"

#define DEFAULT_PCI_BUS_NUMBER_PCH  0
// Denverton AptioV override End - EIP#235471
#include "Register\PchRegsTraceHub.h" // Denverton AptioV override - EIP#273570


static EFI_GUID                   mSystemConfigurationGuid = SYSTEM_CONFIGURATION_GUID;   // Denverton Aptiov override - EIP#379099

/**
    This function is called by PCI Bus driver for SB/LPC bridge  
    initialization.

    @param This - Pointer to protocol AMI_BOARD_INIT_PROTOCOL.
    @param Function - Pointer to Function variable.
    @param ParameterBlock - Pointer to the standard parameter block.

    @retval EFI_STATUS

**/
EFI_STATUS CspSbPciInitLpcBridge (
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
    if (InitStep == isPciProgramDevice) {
    	// An example when PciBus call this device in "isPciProgramDevice" case.
    	// ....
    	Status = EFI_SUCCESS;
    }

    if (InitStep == isPciGetSetupConfig) {
		// Like to set setup settings for ASPM to AUTO; Default was Disabled
		//###dev->PciExpress->Pcie1Setup.AspmMode = 0x55;
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

/**
    This function is called by PCI Bus driver for SB EHCI0 PCI controller   
    initialization.

    @param This - Pointer to protocol AMI_BOARD_INIT_PROTOCOL.
    @param Function - Pointer to Function variable.
    @param ParameterBlock - Pointer to the standard parameter block.

    @retval EFI_STATUS

**/
EFI_STATUS CspSbPciInitEhci0 (
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
    if (InitStep == isPciReadyToBoot) {
    	// An example when PciBus call this device in "isPciReadyToBoot" case.
    	// ....
    	Status = EFI_SUCCESS;
    }

    return Status;
}

// Denverton AptioV override End - EIP#235471
// To skip the device from PCI bus enumeration.

/**
    Skips the specified device from PCI enumeration. Bus, device and 
    function numbers of the device are used to skip device.

    @param This - Pointer to protocol AMI_BOARD_INIT_PROTOCOL.
    @param Function - Pointer to Function variable.
    @param ParameterBlock - Pointer to the standard parameter block.

    @retval EFI_STATUS

**/

EFI_STATUS PciPortSkipDeviceInit (
  IN      AMI_BOARD_INIT_PROTOCOL     *This,
  IN      UINTN                       *Function,
  IN OUT  VOID                        *ParameterBlock
)
{
    AMI_BOARD_INIT_PARAMETER_BLOCK  *Args = (AMI_BOARD_INIT_PARAMETER_BLOCK*)ParameterBlock;
    PCI_DEV_INFO                    *Dev = (PCI_DEV_INFO*)Args->Param1;
    
    if(Args->InitStep == isPciSkipDevice) {
        
        //
        //Skip SPI device from PCI Bus enumeration.
        //
        if ((Dev->Address.Addr.Bus == DEFAULT_PCI_BUS_NUMBER_PCH) && \
            (Dev->Address.Addr.Device == PCI_DEVICE_NUMBER_PCH_SPI) && \
            (Dev->Address.Addr.Function == PCI_FUNCTION_NUMBER_PCH_SPI)) {
            return EFI_SUCCESS;
        }
        // Denverton AptioV override Start - EIP#273570
        //
        // Skip Trace Hub device from PCI Bus enumeration.
        //
        if ((Dev->Address.Addr.Bus == DEFAULT_PCI_BUS_NUMBER_PCH) && \
            (Dev->Address.Addr.Device == PCI_DEVICE_NUMBER_PCH_TRACE_HUB) && \
            (Dev->Address.Addr.Function == PCI_FUNCTION_NUMBER_PCH_TRACE_HUB)) {
            return EFI_SUCCESS;
        }
        // Denverton AptioV override End - EIP#273570
    }
    return EFI_UNSUPPORTED;
}
// Denverton AptioV override End - EIP#235471

// Denverton Aptiov override Start- EIP#379099

/**
    Changing PCI Express Gen2 Link speed settings with respect to Intel Link speed setup value 
    when AMI Link speed setup value set as AUTO 

    @param This - Pointer to protocol AMI_BOARD_INIT_PROTOCOL.
    @param Function - Pointer to Function variable.
    @param ParameterBlock - Pointer to the standard parameter block.

    @retval EFI_STATUS

**/

EFI_STATUS InitPciPortSettings (
    IN AMI_BOARD_INIT_PROTOCOL              *This,
    IN UINTN                                *Function,
    IN OUT AMI_BOARD_INIT_PARAMETER_BLOCK   *ParameterBlock
)
{
    
    AMI_BOARD_INIT_PARAMETER_BLOCK  *Args    = ParameterBlock;
    PCI_INIT_STEP                   InitStep = (PCI_INIT_STEP)Args->InitStep;
    PCI_DEV_INFO                    *dev     = (PCI_DEV_INFO*)Args->Param1;
    EFI_STATUS                      Status;
    SYSTEM_CONFIGURATION            mSystemConfiguration;
    UINTN                           VariableSize = sizeof (SYSTEM_CONFIGURATION);
    UINTN                           idx;    
    PCI_TRACE((TRACE_ALWAYS,__FUNCTION__": Start \n"));
    
    if (InitStep == isPciGetSetupConfig) {
        Status = pRS->GetVariable (L"IntelSetup",
                                   &mSystemConfigurationGuid,
                                   NULL,
                                   &VariableSize,
                                   &mSystemConfiguration);
        if (EFI_ERROR(Status)){
            return EFI_UNSUPPORTED;
        }
        idx = (dev->Address.Addr.Device) - (0x09);
        if (dev->Address.Addr.Device > 0x0C){
            idx -= 1 ;
        }
        PCI_TRACE((TRACE_ALWAYS,"\n ROOT PORT %d \n", idx));
        if (dev->PciExpress->Pcie2->Pcie2Setup.LnkSpeed == PCI_SETUP_AUTO_VALUE)
            dev->PciExpress->Pcie2->Pcie2Setup.LnkSpeed = mSystemConfiguration.RootPortLinkSpeed[idx];
    }
    return EFI_UNSUPPORTED;    
}
// Denverton Aptiov override End- EIP#379099

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
