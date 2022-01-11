//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file SbIde.c
    Initialize and provide a protocol for the IDE controller.

*/

// Generic Include Files
#include <Efi.h>
#include <Token.h>
#include <PiDxe.h>
#include <AmiDxeLib.h>
#include <AmiCspLib.h>
#include <Setup.h>

// Protocol Include Files
#include <Protocol/PciIo.h>
#include <Protocol/PciRootBridgeIo.h>
#include <Protocol/DevicePath.h>
#include <Protocol/DriverBinding.h>
#include <protocol/BlockIo.h>
#include <protocol/PDiskInfo.h>
#include <Protocol/PIDEController.h>
#include <IndustryStandard/AmiAtaAtapi.h>
#include <Protocol/AmiIdeBus.h>
#include <Protocol/ComponentName2.h>
#include <Library/SbPolicy.h>

#define EFI_EVENT_SIGNAL_READY_TO_BOOT  0x00000203

// GUID Definitions
EFI_GUID    gSetupGuid = SETUP_GUID;
extern EFI_GUID gEfiIdeControllerProtocolGuid;

// Function prototypes
void        ZeroMemory (void    *Buffer, UINTN   Size);

EFI_STATUS  CheckSATADevice(EFI_PCI_IO_PROTOCOL *PciIo, UINT8 Channel);
EFI_STATUS  CheckPATAIDEController (UINT8 *PciConfig);
EFI_STATUS  CheckSATAIDEController (UINT8 *PciConfig);
VOID        CallbackBootScript(IN EFI_EVENT Event, IN VOID *Context);
EFI_STATUS  CheckIDEController (IN UINT8            *PciConfig);
EFI_STATUS  IdeGetControllerInfo (IN EFI_HANDLE     Controller,
                    IN OUT CONTROLLER_INFO      *ControllerInfo);
EFI_STATUS  InitIdeController (EFI_PCI_IO_PROTOCOL      *PciIo,
                    UINT8                   *PciConfig);
EFI_STATUS  GetIdeControllerName (IN OUT IDE_CONTROLLER_PROTOCOL        *IdeControllerInterfaceData,
                    IN UINT8                            *PciConfig);

EFI_STATUS  IdeControllerSupported (IN EFI_DRIVER_BINDING_PROTOCOL    *This,
                IN EFI_HANDLE                      Controller,
                IN EFI_DEVICE_PATH_PROTOCOL       *RemainingDevicePath);
EFI_STATUS  IdeControllerStart (IN EFI_DRIVER_BINDING_PROTOCOL    *This,
                IN EFI_HANDLE                     Controller,
                IN EFI_DEVICE_PATH_PROTOCOL       *RemainingDevicePath);
EFI_STATUS  IdeControllerStop (IN EFI_DRIVER_BINDING_PROTOCOL    *This,
                IN EFI_HANDLE                     Controller,
                IN UINTN                          NumberOfChildren,
                IN EFI_HANDLE                     *ChildHandleBuffer);
EFI_STATUS  IdeCtlDriverName(IN EFI_COMPONENT_NAME2_PROTOCOL  *This,
                IN CHAR8                        *Language,
                OUT CHAR16                      **DriverName);
EFI_STATUS  IdeCtlGetControllerName(IN EFI_COMPONENT_NAME2_PROTOCOL  *This,
                IN EFI_HANDLE                   ControllerHandle,
                IN EFI_HANDLE                   ChildHandle        OPTIONAL,
                IN CHAR8                        *Language,
                OUT CHAR16                      **ControllerName);

EFI_STATUS SbIde_BoardInit (
                IN EFI_HANDLE     ImageHandle,
                IN EFI_SYSTEM_TABLE   *SystemTable );

// Constant Definitions

/****  PORTING REQUIRED ****/
#define IDE_CONTROLLER_DRIVER_VERSION   0x01
#define VID_IDE                         0
#define IDE_DID                         0
#define IDE_ADDRESS                     0x1f0100    // Bus 0, Device 1f, Function 1
#define SATA_ADDRESS                    0x1f0200    // Bus 0, Device 1f, Function 2

//  Number of Channels in PATA
#define MAXIMUM_CHANNELS                0x02
#define SCC_IDE_DEVICE                  1       // Sub Class Code
#define BCC_MASS_STORAGE_DEVICE         1       // Base class code
#define IO_ENABLE                       1
#define BUS_MASTER_ENABLE               4
#define PCI_CFG_COMMAND_REG             0x04
#define PCI_CFG_PROG_IF                 0x09

// Define necessary chipset based constants here
/*** PORTING REQUIRED ***/
/***
#define IDE_INTEL_Vendor_ID             0x8086  // Intel
#define IDE_ICH7_PATA_Device_ID         0x27df
#define IDE_ICH7_SATA_Device_ID         0x27c0
 ***/

#define IDE_TIMP                        0x40
#define IDE_TIMS                        0x42
#define SLV_IDETIM                      0x44
#define SDMA_CNT                        0x48
#define SDMA_TIM                        0x4A
#define IDE_CONFIG                      0x54

#define PM_GPE0_STS                     0x28

#define PRI_CHANNEL_CABLE_GPI           0x09
#define SEC_CHANNEL_CABLE_GPI           0x00

// Structure definitions
typedef struct {
    UINT64                  Address;
    EFI_BOOT_SCRIPT_WIDTH   Width;
    UINT32                  Value;
} BOOT_SCRIPT_PCI_REGISTER_SAVE;

// Define variables
EFI_EVENT   gEvtBootScript;

// Define name strings
CHAR16  *gIdeDriverName             = L"AMI SB IDE Controller Driver";
CHAR16  *gIdeControllerName         = L"SB IDE Controller";
/*** PORTING REQUIRED - Change the name of the controller appropriately ***/
CHAR16  *gIdePataControllerName     = L"AMI Template PATA IDE Controller";
CHAR16  *gIdeSataControllerName     = L"AMI Template SATA IDE Controller";

// Table definitions
EFI_DRIVER_BINDING_PROTOCOL gIdeControllerDriverBinding = {
                IdeControllerSupported, 
                IdeControllerStart,     
                IdeControllerStop,      
                IDE_CONTROLLER_DRIVER_VERSION,      // version
                NULL,                               // ImageHandle
                NULL                                // DriverBindingHandle
};

EFI_COMPONENT_NAME2_PROTOCOL gIdeControllerDriverName = {
                IdeCtlDriverName,
                IdeCtlGetControllerName,
                "eng"
};

IDE_CONTROLLER_INIT_DATA IdeControllerPATAInitTable [] = {
//    Register,             AND_MASK  OR_MASK
    PCI_CFG_PROG_IF,        0x00,       0x00,
    0x00,                   0xFF,       0x00            // DUMMY VALUE REMOVE IT
/** PORTING REQUIRED - Include appropriate table entries below
    IDE_TIMP,                 0x00,   0x00,
    IDE_TIMP + 1,             0x00,   0x80,
    IDE_TIMS,                 0x00,   0x00,
    IDE_TIMS + 1,             0x00,   0x80,
    SDMA_CNT,                 0xF0,   0x00,
    SDMA_TIM,                 0x00,   0x00,
    IDE_CONFIG,               0x00,   0x00,
    IDE_CONFIG + 1,           0x0F,   0x00,
    IDE_CONFIG + 2,           0xF0,   0x00
 **/
};


IDE_CONTROLLER_INIT_DATA IdeControllerSATAInitTable [] = {
//    Register,             AND_MASK  OR_MASK
    PCI_CFG_PROG_IF,        0xF0,       0x05,
    0x00,                   0xFF,       0x00            // DUMMY VALUE REMOVE IT
/** PORTING REQUIRED - Include appropriate table entries below
    IDE_TIMP,                 0x00,   0x00,
    IDE_TIMP + 1,             0x00,   0x80,
    IDE_TIMS,                 0x00,   0x00,
    IDE_TIMS + 1,             0x00,   0x80,
    SDMA_CNT,                 0xF0,   0x00,
    SDMA_TIM,                 0x00,   0x00,
    IDE_CONFIG,               0x00,   0x00,
    IDE_CONFIG + 1,           0x0F,   0x00,
    IDE_CONFIG + 2,           0xF0,   0x00,
//  R_SATA_PCS,               0xF0,   0x0F
 **/
};

/** PORTING REQUIRED - Included appropriate table entries below based on your chipset **/
UINT8 UDMA_Reg_4A_4B [] = {
//  Value       UDMA MODE
    00,         // 0
    01,         // 1
    02,         // 2
    01,         // 3
    02,         // 4
    01          // 5
};

/** PORTING REQUIRED - Included appropriate table entries below based on your chipset **/
UINT8 PIOMode_IoRdy [] = {
//  Value       PIO Mode
    00,         // 0
    00,         // 1
    01,         // 2
    02,         // 3
    02,         // 4
};

/** PORTING REQUIRED - Included appropriate table entries below based on your chipset **/
UINT8 PIOMode_RCT [] = {
//  Value       PIO Mode
    00,         // 0
    00,         // 1
    00,         // 2
    01,         // 3
    03,         // 4
};

/** PORTING REQUIRED - Included appropriate table entries below based on your chipset **/
IDE_CONTROLLER_INIT_DATA PIOMode_Regs [] =  {
//  Reg                 AND         Shift Value
    IDE_TIMP + 1,       0xc0,       00,     // Primary Master
    SLV_IDETIM,         0xF0,       00,     // Primary Slave
    IDE_TIMS + 1,       0xc0,       00,     // Secondary Master 
    SLV_IDETIM,         0x0F,       04,     // Secondary Slave          
};

BOOT_SCRIPT_PCI_REGISTER_SAVE gPciRegistersSave[] = {
/** PORTING REQUIRED **/
  IDE_ADDRESS + 0x09,  EfiBootScriptWidthUint8,  0, //The register will be filled in later.
  IDE_ADDRESS + 0x40,  EfiBootScriptWidthUint32, 0,
  IDE_ADDRESS + 0x44,  EfiBootScriptWidthUint8,  0,
  IDE_ADDRESS + 0x48,  EfiBootScriptWidthUint8,  0,
  IDE_ADDRESS + 0x4a,  EfiBootScriptWidthUint16, 0,
  IDE_ADDRESS + 0x54,  EfiBootScriptWidthUint32, 0,
  IDE_ADDRESS + 0xc0,  EfiBootScriptWidthUint8,  0,

  SATA_ADDRESS + 0x09, EfiBootScriptWidthUint8,  0,
  SATA_ADDRESS + 0x40, EfiBootScriptWidthUint32, 0,
  SATA_ADDRESS + 0x44, EfiBootScriptWidthUint8,  0,
  SATA_ADDRESS + 0x48, EfiBootScriptWidthUint8,  0,
  SATA_ADDRESS + 0x4a, EfiBootScriptWidthUint16, 0,
  SATA_ADDRESS + 0x54, EfiBootScriptWidthUint32, 0,
  SATA_ADDRESS + 0x74, EfiBootScriptWidthUint16, 0,
  SATA_ADDRESS + 0x90, EfiBootScriptWidthUint8,  0,
  SATA_ADDRESS + 0x92, EfiBootScriptWidthUint16, 0
};

/**
    Detect whether 40/80 pin cable is connected.

    @param PciIo - PCI I/O protocol pointer
    @param Channel - Channel information

    @retval TRUE  80 Pin cable connected
            FALSE 40 Pin Cable connected

    @note  Make use of PciIO protocol to distinguish between 
           multiple IDE controllers.
**/
BOOLEAN
Get4080PinCableStatus (
    IN EFI_PCI_IO_PROTOCOL      *PciIo,
    IN UINT8                    Channel
  )
{

/*** Porting Required
    UINT16          Data16;

//  For SATA always return 80 Pin Cable
    if (CheckSATADevice(PciIo, Channel) == EFI_SUCCESS) return TRUE;

    Data16 = IoRead16(PM_BASE_ADDRESS + PM_GPE0_STS + 2);

    if (Channel == PRIMARY_CHANNEL) return !(Data16 & PRI_CHANNEL_CABLE_GPI);
    else return !(Data16 & SEC_CHANNEL_CABLE_GPI);
 ***/
    return FALSE;

}

/**
    Detect whether the drive connected is a SATA device

    @param PciIo - PCI I/O protocol pointer
    @param Channel - Channel information

    @retval EFI_SUCCESS if the device is a SATA device
            EFI_DEVICE_ERROR if the device is not a SATA device

    @note  This routine is specific to South Bridge. If multiple IDE controllers
           are not handled by the same IDE controller driver, then this routine is 
           not needed.
**/
EFI_STATUS
CheckSATADevice(
    IN EFI_PCI_IO_PROTOCOL  *PciIo,
    IN UINT8                Channel
 )
{
    UINT8                   PciConfig[256];
    EFI_STATUS              Status;

    // Read the PCI Configuration Space
    Status = PciIo->Pci.Read (PciIo, EfiPciIoWidthUint8, 
                                0, sizeof (PciConfig), PciConfig);

    if (CheckSATAIDEController (PciConfig) == EFI_SUCCESS) {
/** PORTING REQUIRED
        // Now Check whether the given channel is really in SATA mode
        // ICH6/ICH5 have different bit definitions. Port it accordingly.
        if ((PciConfig[R_SATA_MAP] & 03) == 0) return EFI_SUCCESS; // Non Combined mode
        if ((PciConfig[R_SATA_MAP] & 03) == 1 && (Channel == 1)) return EFI_SUCCESS; // SATA is Secondary
        if ((PciConfig[R_SATA_MAP] & 03) == 2 && (Channel == 0)) return EFI_SUCCESS; // SATA is Primary
 **/
    }

    return EFI_DEVICE_ERROR;
}

/**
    Check whether the IDE controller is PATA or not

    @param *PciConfig  Contains the PCI configuration map for the IDE controller

    @retval EFI_SUCCESS if the device is a PATA controller

    @note  This routine is specific to South Bridge. If multiple IDE controllers
           are not handled by the same IDE controller driver, then this routine is 
           not needed.
**/
EFI_STATUS
CheckPATAIDEController (
    UINT8           *PciConfig
)
{
/** PORTING REQUIRED **
    // Check whether it is Intel ICH7 IDE Controller or NOT?
    if (PciConfig[0xA] == SCC_IDE_DEVICE &&
        PciConfig[0xB] == BCC_MASS_STORAGE_DEVICE  &&
        PciConfig[0x0] == (IDE_INTEL_Vendor_ID & 0xff)  && 
        PciConfig[0x1] == (IDE_INTEL_Vendor_ID >>8 )    && 
        ((PciConfig[0x2] == (IDE_ICH7_PATA_Device_ID & 0xff) &&
            PciConfig[0x3] == (IDE_ICH7_PATA_Device_ID >>8 )) ))
        return EFI_SUCCESS;
    else
 **/
        return EFI_UNSUPPORTED; // This is not the device (PCI IDE controller) we are looking for   

}

/**
    Check whether the IDE controller is SATA or not

    @param PciConfig  Contains the PCI configuration map for the IDE controller

    @retval EFI_SUCCESS if the device is a SATA controller

    @note  This routine is specific to South Bridge. If multiple IDE controllers
           are not handled by the same IDE controller driver, then this routine is 
           not needed.
**/
EFI_STATUS
CheckSATAIDEController (
    IN UINT8            *PciConfig
)
{

/** PORTING REQUIRED **
    // Check whether it is Intel ICH7 IDE Controller or NOT?
    if (PciConfig[0xA] == SCC_IDE_DEVICE &&
        PciConfig[0xB] == BCC_MASS_STORAGE_DEVICE &&
        PciConfig[0x0] == (IDE_INTEL_Vendor_ID & 0xff) &&
        PciConfig[0x1] == (IDE_INTEL_Vendor_ID >>8 ) &&
        ((PciConfig[0x2] == (IDE_ICH7_SATA_Device_ID & 0xff) &&
            PciConfig[0x3] == (IDE_ICH7_SATA_Device_ID >>8 )) ))
        return EFI_SUCCESS;
    else
 **/
        return EFI_UNSUPPORTED; // This is not the device (PCI IDE controller) we are looking for

}

/**
    This routine is called to initialize the PATA IDE controller

    @param PciIo    PCI I/O protocol pointer

    @retval VOID

    @note  This routine should program the controller with default register
           values
**/
void
InitPATAIdeController (
    IN EFI_PCI_IO_PROTOCOL          *PciIo
)
{

    UINT8           PciData8;
    UINT8           Index;
    EFI_STATUS      Status;

//  Based on Platform policy initialize the IDE controller
//  Example: 1. Legacy/Native Mode. 2. How many channels need to be enabled? etc

    for (Index = 0; 
        Index < sizeof (IdeControllerPATAInitTable) / sizeof (IDE_CONTROLLER_INIT_DATA); 
        Index++) {
        Status = PciIo->Pci.Read (
                    PciIo,
                    EfiPciIoWidthUint8,     
                    IdeControllerPATAInitTable[Index].Register,
                    1,
                    &PciData8 );
        PciData8 &= IdeControllerPATAInitTable[Index].And_Mask;
        PciData8 |= IdeControllerPATAInitTable[Index].Or_Mask;
        Status = PciIo->Pci.Write(
                    PciIo,
                    EfiPciIoWidthUint8,     
                    IdeControllerPATAInitTable[Index].Register,
                    1,
                    &PciData8 );
    }
}   

/**
    This routine is called to initialize the SATA IDE controller

    @param PciIo    PCI I/O protocol pointer

    @retval VOID

    @note  This routine should program the controller with default register
           values
**/
void
InitSATAIdeController (
    IN EFI_PCI_IO_PROTOCOL      *PciIo
  )
{

    UINT8               PciData8;
    UINT8               Index;
    EFI_STATUS          Status;
    SB_SETUP_DATA       SetupData;

// Based on Platform policy initialize the IDE controller
// Example: 1. Legacy/Native Mode. 2. How many channels need to be enabled? etc

    for (Index = 0; 
        Index < sizeof (IdeControllerSATAInitTable) / sizeof (IDE_CONTROLLER_INIT_DATA); 
        Index++) {
        Status = PciIo->Pci.Read(
                            PciIo,
                            EfiPciIoWidthUint8,
                            IdeControllerSATAInitTable[Index].Register,
                            1,
                            &PciData8
                            );
        PciData8 &= IdeControllerSATAInitTable[Index].And_Mask;
        PciData8 |= IdeControllerSATAInitTable[Index].Or_Mask;
        Status = PciIo->Pci.Write(
                              PciIo,
                              EfiPciIoWidthUint8,
                              IdeControllerSATAInitTable[Index].Register,
                              1,
                              &PciData8 );
    }

    GetSbSetupData(pRS, &SetupData, FALSE);

/** PORTING REQUIRED ** program as per the setup options  **/
    if (!EFI_ERROR(Status)){
        if(SetupData.Sata == 1)
        {
/*          Status = PciIo->Pci.Read(
                              PciIo,
                              EfiPciIoWidthUint8,
                              R_SATA_PCS,
                              1,
                              &PciData8
                              );*/

            //
            // Disable the SATA Ports where devices are not present.
            //
            //PciData8 >>= 4;

            //
            // Disable SATA ports (P0-P3) depending upon the Setup
            //
            if(SetupData.Sata0 == 0){
                //PciData8 &= ~B_SATA_PORT_0_ENABLED; // Disable SATA Port 0
                ;
            }

            if(SetupData.Sata1 == 0){
                //PciData8 &= ~B_SATA_PORT_1_ENABLED; // Disable SATA Port 1
                ;
            }

            if(SetupData.Sata2 == 0){
                //PciData8 &= ~B_SATA_PORT_2_ENABLED; // Disable SATA Port 2
                ;
            }

            if(SetupData.Sata3 == 0){
                //PciData8 &= ~B_SATA_PORT_3_ENABLED; // Disable SATA Port 3
                ;
            }
/*
            Status = PciIo->Pci.Write(
                                PciIo,
                                EfiPciIoWidthUint8,
                                R_SATA_PCS,
                                1,
                                &PciData8
                                );
*/
        }
/*
        Status = PciIo->Pci.Read(
                            PciIo,
                            EfiPciIoWidthUint8,
                            R_SATA_MAP,
                            1,
                            &PciData8
                            );

        //
        // Clear the ATAPI/SATA Mode bits (Bits 7:6 & 1:0)
        //
        PciData8 &= 0x3C;

        PciData8 |= V_SATA_MAP_NON_COMBINED;// Native (Enhanced) Mode.
        PciData8 |= V_SATA_MAP_IDE;         // IDE is the only valid mode.
        Status = PciIo->Pci.Write(
                              PciIo,
                              EfiPciIoWidthUint8,
                              R_SATA_MAP,
                              1,
                              &PciData8
                              );
*/
    }
}

/**
    This routine is called to program IDE device with the provided
    PIO mode timing

    @param IdeDevice  Pointer to the IDE device interface structure

    @retval EFI_SUCCESS

    @note  This is Chipset dependent hook. Use PCI_IO_PROTOCOL to 
           differentiate between multiple IDE controllers.
             1. Program the timings for the given PIO mode into controller 
                if it is supported.
             2. If not, lower the PIO mode, program the timing and update 
                the input value.
**/
EFI_STATUS
IdeControllerSetPioModeTiming (
    IN OUT IDE_DEVICE_INTERFACE             *IdeDevice
  )
{

    UINT8                   *PioMode = &(IdeDevice->PIOMode);
//  UINT8                   Channel = IdeDevice->Channel;
//  UINT8                   Drive = IdeDevice->Device;
//  UINT8                   *SWDma = &(IdeDevice->SWDma);
//  UINT8                   *MWDma = &(IdeDevice->MWDma);
//  UINT8                   *UDmaMode  = &(IdeDevice->UDma);
//  UINT8                   *IORdy = &(IdeDevice->IORdy);
//  UINT8                   PciReg;
//  UINT8                   PciData8;
//  UINT8                   Value = 0;
//  EFI_STATUS              Status;
//  IDENTIFY_DATA           *pIdentifyData = &(IdeDevice->IdentifyData);
//  EFI_PCI_IO_PROTOCOL     *PciIo = IdeDevice->PciIO;


//  Max PioMode Supported is 4
    *PioMode = *PioMode > 4  ? 4 : *PioMode;

/** ** PORTING REQUIRED
//----------------------------------------------------------------------------
//  Program the IORDY and RCT bits based on PIO mode in Reg 40-43 in case of Drive 0
//  and Reg 44h in case of Drive 1
    PciReg = (Channel == PRIMARY_CHANNEL ? IDE_TIMP + 1 : IDE_TIMS + 1); // 0x40 0x42
    if (Drive == SLAVE_DRIVE) {
        PciReg = SLV_IDETIM;                // 0x44
    }

    
    Value = PIOMode_IoRdy[*PioMode];
    Value <<= (Drive == SLAVE_DRIVE ? 2 : 4);   // Left Shift IORDY bits by 2 in case of Slave else 4 for Master 
    Value |= PIOMode_RCT[*PioMode];

    Status = PciIo->Pci.Read (
                PciIo,
                EfiPciIoWidthUint8,     
                PciReg,         
                1,
                &PciData8);         

    PciData8 = PciData8 & (PIOMode_Regs[(Channel << 1) + Drive].And_Mask);
    Value = Value << (PIOMode_Regs[(Channel << 1) + Drive].Or_Mask);
    PciData8 |= Value;

    Status = PciIo->Pci.Write (
                PciIo,
                EfiPciIoWidthUint8,
                PciReg,
                1,
                &PciData8);

//----------------------------------------------------------------------------
//  Enable Drive 1 Timing register 
    if (Drive ==  SLAVE_DRIVE) {
        PciReg = Channel == PRIMARY_CHANNEL ? IDE_TIMP + 1: IDE_TIMS + 1;
        Status = PciIo->Pci.Read (
                    PciIo,
                    EfiPciIoWidthUint8,     
                    PciReg,         
                    1,
                    &PciData8);         
        PciData8 |= 0x40;
        Status = PciIo->Pci.Write (
                    PciIo,
                    EfiPciIoWidthUint8,     
                    PciReg,         
                    1,
                    &PciData8);                 

    }   

//----------------------------------------------------------------------------
//  Program the Fast Timing, IORDY Enable, Prefetch Posting and DMA Timing Enable bits
    PciReg = (Channel == PRIMARY_CHANNEL ? IDE_TIMP : IDE_TIMS); // 0x40 0x42
    Status = PciIo->Pci.Read(
                PciIo,
                EfiPciIoWidthUint8,     
                PciReg,         
                1,
                &PciData8);                 

//  Clear the data
    PciData8 &= (Drive == SLAVE_DRIVE ? 0x0f : 0xf0);
    Value = 0;

//  Enable Fast timing bit if PIO Mode > 0
    Value |= (*PioMode > 0 ? 1 : 0);

//  Enable IORdy sampling point
    Value |= (*IORdy == 1 ? 2 : 0);

//  Prefetch/Posting Enable
    if ((pIdentifyData->General_Config_0 & 0x8000) == 0) {  // Fixed HDD only               
        Value |= (*PioMode > 2 ? 4 : 0);    
    }

//  Shift Left by 4 in case of Slave
    Value <<= (Drive == SLAVE_DRIVE ? 4 : 0);

    PciData8 |= Value;
    Status = PciIo->Pci.Write(
                PciIo,
                EfiPciIoWidthUint8,     
                PciReg,         
                1,
                &PciData8);                     
//----------------------------------------------------------------------------
**/
    return EFI_SUCCESS;
}

/**
    This routine is called to program IDE device with the provided
    DMA mode timing

    @param IdeDevice  Pointer to the IDE device interface structure

    @retval EFI_SUCCESS

    @note  This is Chipset dependent hook. Use PCI_IO_PROTOCOL to 
           differentiate between multiple IDE controllers.
             1. Program the timings for the given DMA mode into controller 
                if it is supported.
             2. If not, lower the UDMA mode, program the timing and update 
                the input value.
             3. Set DMA capable bit in BUS Master Status Register
**/
EFI_STATUS
IdeControllerSetDmaModeTiming (
    IDE_DEVICE_INTERFACE                *IdeDevice
  )
{

/** PORTING REQUIRED
    EFI_PCI_IO_PROTOCOL     *PciIo = IdeDevice->PciIO;
    UINT8                   Channel = IdeDevice->Channel;
    UINT8                   Drive = IdeDevice->Device;
    UINT8                  *SWDma = &(IdeDevice->SWDma);
    UINT8                  *MWDma = &(IdeDevice->MWDma);
    UINT8                   *UDmaMode  = &(IdeDevice->UDma);

    UINT8                   PciReg;
    UINT8                   PciData8;
    UINT16                  PciData16;
    UINT8                   UDMA_Value;
    EFI_STATUS              Status;

    if (*UDmaMode != 0xff) {

//--------------------------------------------------------------//
//      Valid UDMA mode
        if (*UDmaMode > Max_UDMA) { *UDmaMode = Max_UDMA; } // Reduce the UDMA mode
        if ( (*UDmaMode > 2) && !Get4080PinCableStatus (PciIo, Channel)) {
                *UDmaMode = 2;                  // 40 Pin Cable detected. Reduce the UDMA Mode
            }

//      Program Registers 4A-4Bh
        if (Channel) {
            PciReg = SDMA_TIM + 1;      // Secondary 4B
        }                       
        else {
            PciReg = SDMA_TIM;          // Primary  4A
        }
        
        Status = PciIo->Pci.Read (
                    PciIo,
                    EfiPciIoWidthUint8,     
                    PciReg,         // 4A-4B
                    1,
                    &PciData8);

        UDMA_Value = UDMA_Reg_4A_4B[*UDmaMode];
        if  (Drive) {                   // Slave
            PciData8 &= 0x0f;
            PciData8 |= (UDMA_Value << 4);
        }
        else {                          // Master
            PciData8 &= 0xf0;
            PciData8 |= UDMA_Value;
        }

        Status = PciIo->Pci.Write(
                    PciIo,
                    EfiPciIoWidthUint8,     
                    PciReg,         // 4A-4B
                    1,
                    &PciData8);

//--------------------------------------------------------------//

//      Enable Synchronous DMA Mode in register 48h
        PciReg = SDMA_CNT;           
        Status = PciIo->Pci.Read (
                    PciIo,
                    EfiPciIoWidthUint8,     
                    PciReg,         // 48
                    1,
                    &PciData8);     
        if (Channel == 0 && Drive == 0) { PciData8 |= 1;}
        if (Channel == 0 && Drive == 1) { PciData8 |= 2;}
        if (Channel == 1 && Drive == 0) { PciData8 |= 4;}
        if (Channel == 1 && Drive == 1) { PciData8 |= 8;}

        Status = PciIo->Pci.Write(
                    PciIo,
                    EfiPciIoWidthUint8,     
                    PciReg,         // 48
                    1,
                    &PciData8);

//--------------------------------------------------------------//
//      Set 66/100Mhz Base clock.
        PciReg = IDE_CONFIG;                // 0x54
        Status = PciIo->Pci.Read (
                    PciIo,
                    EfiPciIoWidthUint16,        
                    PciReg,             // 54
                    1,
                    &PciData16);        
        if (*UDmaMode == 5) {                       // 100Mhz
        if (Channel == 0 && Drive == 0) { PciData16 |= 0x1000;}
        if (Channel == 0 && Drive == 1) { PciData16 |= 0x2000;}
        if (Channel == 1 && Drive == 0) { PciData16 |= 0x4000;}
        if (Channel == 1 && Drive == 1) { PciData16 |= 0x8000;}
        }
        if  (*UDmaMode > 2 && *UDmaMode < 5){       // 66Mhz
        if (Channel == 0 && Drive == 0) { PciData16 |= 0x01;}
        if (Channel == 0 && Drive == 1) { PciData16 |= 0x02;}
        if (Channel == 1 && Drive == 0) { PciData16 |= 0x04;}
        if (Channel == 1 && Drive == 1) { PciData16 |= 0x08;}
        }
        if (Get4080PinCableStatus (PciIo, Channel)) {       // Set 40/80 pin cable
            if (Channel == 0 && Drive == 0) { PciData16 |= 0x10;}
            if (Channel == 0 && Drive == 1) { PciData16 |= 0x20;}
            if (Channel == 1 && Drive == 0) { PciData16 |= 0x40;}
            if (Channel == 1 && Drive == 1) { PciData16 |= 0x80;}
        }
        Status = PciIo->Pci.Write(
                    PciIo,
                    EfiPciIoWidthUint16,        
                    PciReg,             // 54
                    1,
                    &PciData16);
    }
//--------------------------------------------------------------//

//  Enable Bus Master Bit in Command Register
    Status = PciIo->Pci.Read (
                PciIo,
                EfiPciIoWidthUint8,
                PCI_CFG_COMMAND_REG,                        // 4
                1,
                &PciData8);
    PciData8 |= 4;
    Status = PciIo->Pci.Write (
                PciIo,
                EfiPciIoWidthUint8,
                PCI_CFG_COMMAND_REG,                        // 4
                1,
                &PciData8);

//  Enable DMA capable bit in Bus Master Status Register
    PciIo->Io.Read (
            PciIo,
            EfiPciIoWidthUint8,
            4,                              // BarIndex
            Channel == 0 ? 2 : 0xA,         // Offset
            1,                              // Count
            &PciData8);

    if ((*UDmaMode != 0xff) ||  (*MWDma != 0xff)){
        PciData8 |= (Drive == 0 ? 0x20 : 0x40); // Master/Slave
    }
    else {
        PciData8 &= (Drive == 0 ? 0xDF : 0xBF); // Master/Slave
    }

    PciIo->Io.Write(
            PciIo,
            EfiPciIoWidthUint8,
            4,                              // BarIndex
            Channel == 0 ? 2 : 0xA,         // Offset
            1,                              // Count
            &PciData8);
//--------------------------------------------------------------//
 **/

    return EFI_SUCCESS;
}

/**
    This routine is used to find out the hot plug device change status

    @param PciIo - PCI I/O protocol pointer
    @param HpMask - Mask indicating which IDE devices are hot pluggable
    @param CurrentStatus - Indicates what devices are present (1 - present)
           BIT         Device Type
            0          Primary Master
            1          Primary Slave
            2          Secondary Master
            3          Secondary Slave

    @retval EFI_STATUS

    @note  This is Chipset dependent hook and requires porting.
           If GPI status is available to find the device insertion and 
           removal make use of it.
           This routine shouldn't take very long to complete the 
           Hot plug detection. 
**/
EFI_STATUS
HPCheckForDeviceChange (
    EFI_PCI_IO_PROTOCOL         *PciIo,
    UINT8                       HpMask,
    UINT8                       *CurrentStatus
)
{

#if IDE_HP_SUPPORT
/** PORTING REQUIRED
    UINT8                       PciData8;
    UINT8                       Temp;
    UINT8                       Current_Channel = 0;
    UINT8                       Current_Device = 0;
    UINT8                       PciConfig[0x20];
    UINT16                      Index, DevReg;
    EFI_STATUS                  Status;


//  Read the port Status
    Status = PciIo->Pci.Read (
                PciIo,
                EfiPciIoWidthUint8,     
                0x92,
                1,
                &Temp);         
    Temp >>= 4;

//  Enable all the Ports 
    PciData8 = 0x0f;
    Status = PciIo->Pci.Write(
                PciIo,
                EfiPciIoWidthUint8,     
                0x92,
                1,
                &PciData8 );

    pBS->Stall (1000);          // 1msec

    Status = PciIo->Pci.Read (
                PciIo,
                EfiPciIoWidthUint8,     
                0x92,
                1,
                &PciData8);     

    PciData8 >>= 4;
    
    if (PciData8 != Temp) {
        Temp = ~Temp;
        PciData8 = Temp & PciData8;
        if (PciData8 & 2) {Current_Channel = 1; Current_Device = 0;}
        if (PciData8 & 4) {Current_Channel = 0; Current_Device = 1;}
        if (PciData8 & 8) {Current_Channel = 1; Current_Device = 1;}

//      Read the PCI Configuration Space
        PciIo->Pci.Read (
                PciIo,
                EfiPciIoWidthUint8,     
                0,
                0x20,
                PciConfig );


        DevReg = 0x1f6;
        if (Current_Channel) DevReg = 0x176;    
//      Check for Legacy/Native Mode
        if (Current_Channel) PciConfig[9] &= 4; else PciConfig[9] &= 1;
        if (PciConfig[9]) {
//          Native Mode
            if (Current_Channel) DevReg = *(((UINT16 *)PciConfig) + 9); 
            else DevReg = *(((UINT16 *)PciConfig) + 8); 
            DevReg &= 0xfffc;
            DevReg += 6;
        }

        Temp = 0;               
        if (Current_Device) Temp = 0x10;
        for (Index = 0; Index < 500; Index++) {
            PciIo->Io.Write(PciIo, EfiPciIoWidthFifoUint8, EFI_PCI_IO_PASS_THROUGH_BAR,
                        DevReg, 1, &Temp);

            PciIo->Io.Read (PciIo, EfiPciIoWidthFifoUint8, EFI_PCI_IO_PASS_THROUGH_BAR,
                        DevReg + 1, 1, &PciData8);
            if (PciData8 != 0xFF || PciData8 != 0x7F) break;
            pBS->Stall(1000);
        }       

    }   
//  Disable the ports where devices are not connected
    Status = PciIo->Pci.Read (
                PciIo,
                EfiPciIoWidthUint8,     
                0x92,
                1,
                &PciData8);     
    PciData8 >>= 4;
    Status = PciIo->Pci.Write(
                PciIo,
                EfiPciIoWidthUint8,     
                0x92,
                1,
                &PciData8 );

//  For ICH6 port   P0:PM
//                  P1:SM
//                  P2:PS
//                  P3:SS
    *CurrentStatus = PciData8;
    *CurrentStatus &= 0x09;         // BIT0 and BIT 3 are correct. Swap BIT1 and 2
    Temp = PciData8;
    Temp &= 0x2;
    Temp <<= 1;
    *CurrentStatus = *CurrentStatus | Temp;
    Temp = PciData8;
    Temp &= 0x4;
    Temp >>= 1;
    *CurrentStatus = *CurrentStatus | Temp;
 **/
#endif
    return EFI_SUCCESS;
}

/**
    This routine is used to enable/disable a hot-plug channel

    @param PciIo - PCI I/O protocol pointer
    @param Channel - Channel that need to be enabled/disabled
    @param Device - Device in which the channel is present
    @param DisableEnable - Indicates whether to enable or disable

    @retval EFI_STATUS

    @note  This is Chipset dependent hook and requires porting.
           Used only for hot plug support
**/
EFI_STATUS
HPDisableEnableChannel(
    EFI_PCI_IO_PROTOCOL         *PciIo,
    UINT8                       Channel,
    UINT8                       Device,
    BOOLEAN                     DisableEnable
 )
{

#if IDE_HP_SUPPORT
/** PORTING REQUIRED
    UINT8           Mask = 1, bData;

//  Check for SATA/PATA
//Valid only for SATA 
    if (Channel) Mask = 2;
    if (Device) Mask <<= 2; 

    if (DisableEnable == FALSE) Mask = ~Mask;  // Ones Complement

    PciIo->Pci.Read (
                PciIo,
                EfiPciIoWidthUint8,     
                0x92,
                1,
                &bData);        

    if (DisableEnable == FALSE) bData &= Mask;
    else bData |= Mask;

    PciIo->Pci.Write(
                PciIo,
                EfiPciIoWidthUint8,     
                0x92,
                1,
                &bData );
 **/
#endif
    return EFI_SUCCESS;
}

/**
    This support function clears the buffer with zeros

    @param Buffer      Pointer to the buffer which has to be zero-ed
    @param Size        Number of bytes in the buffer to be initialized

    @retval VOID
**/
void
ZeroMemory (
    IN OUT void     *Buffer,
    IN UINTN        Size
)
{
    UINT8   *Ptr;
    Ptr = Buffer;
    while (Size--) {
        *(Ptr++) = 0;
    }

}

/**
    This routine is used to get the appropriate PIO and DMA mode
    for the drive based on its supported functionality and chipset
    support

    @param IdeDevice  Pointer to the IDE device interface structure

    @retval EFI_STATUS

    @note  Here is the control flow of this function:
             This is a generic routine. Most likely this algorithm 
             might work across chipsets, if not port it.
             1. This routine makes use of Identify command data to select best
                UDMA, MWDMA, SWDMA and PIO mode. It doesn't take into 
                consideration, the modes supported by the IDE controller.
             2. Check UDMA valid or not Word 53 Bit 2 
                If yes, get the best UDMA value from Word 88
             3. Check  if PIO, DMA valid bit is set for MWDMA and SWDMA.
                If yes, get the best MWDAM from Word 63 and DWDMA from 
                word 62. Extended PIO mode is indicated by Word 64
             4. IORDY support is indicated by Word 49 Bit 11
**/
EFI_STATUS
GetbestPioDmaMode(
    IDE_DEVICE_INTERFACE                *IdeDevice
  )
{

    UINT8                   Index;
    UINT8                   CalculatedPIOMode = 0;
    UINT8                   Channel = IdeDevice->Channel;
    UINT8                   Drive = IdeDevice->Device;
    UINT8                   *PIOMode = &(IdeDevice->PIOMode);
    UINT8                   *SWDmaMode = &(IdeDevice->SWDma);
    UINT8                   *MWDmaMode = &(IdeDevice->MWDma);
    UINT8                   *UDmaMode  = &(IdeDevice->UDma);
    UINT8                   *IORdy = &(IdeDevice->IORdy);
    UINT16                  Data16;
    IDENTIFY_DATA           *IdentifyData = &(IdeDevice->IdentifyData);
    EFI_PCI_IO_PROTOCOL     *PciIo = IdeDevice->PciIO;


    *UDmaMode = 0xff;                   // UDMA not supported
//  Check if UDMA validity bit is set or not
    if (IdentifyData->Valid_Bits_53 & 0x04) {
        Data16 = IdentifyData->UDMA_Mode_88;
        for (Index = 0; Index < 8; Index++) {
            if ( Data16 & 1) {
                *UDmaMode = Index;
            }
            Data16 >>= 1;
        }
    }

    *MWDmaMode = 0xff;                  // Multi Word DMA not supported
//  Check if SWDMA/MWDMA validity bit is set or not
    if (IdentifyData->Valid_Bits_53 & 0x02) {
        Data16 = IdentifyData->MultiWord_DMA_63;
        for (Index = 0; Index < 3; Index++) {
            if ( Data16 & 1) {
                *MWDmaMode = Index;
            }
            Data16 >>= 1;
        }
    }

    *SWDmaMode = 0xff;                  // Single Word DMA not supported
//  Check if SWDMA/MWDMA validity bit is set or not
    if (IdentifyData->Valid_Bits_53 & 0x02) {
        Data16 = IdentifyData->SingleWord_DMA_62;
        for (Index = 0; Index < 3; Index++) {
            if ( Data16 & 1) {
                *SWDmaMode = Index;
            }
            Data16 >>= 1;
        }
    }


    *PIOMode = (IdentifyData->PIO_Mode_51) >> 8;
//  Check if Advanced PIO mode Supported
    if (IdentifyData->Valid_Bits_53 & 0x02) {
        *IORdy = 1;
        if (IdentifyData->PIO_Mode_64 & 0x1 ) { *PIOMode = 3;}
        if (IdentifyData->PIO_Mode_64 & 0x2 ) { *PIOMode = 4;}
        if (IdentifyData->PIO_Mode_64 & 0x4 ) { *PIOMode = 5;}
        if (IdentifyData->PIO_Mode_64 & 0x8 ) { *PIOMode = 6;}
    }

//  Update IORDY Value
    *IORdy = 0;
    if (IdentifyData->Capabilities_49 & 0x800) { 
        *IORdy = 1;      // IORdy Supported
    }
    else {
        *PIOMode = *PIOMode > 2 ? 2 : *PIOMode; // If IORdy not supported, max PIOmode is 2
    }

    CalculatedPIOMode = *PIOMode;
//  Adjust the PIO Mode based on MWDMA and SWDMA
    if (*SWDmaMode == 2) {
        CalculatedPIOMode = 2;
    }

    switch (*MWDmaMode) {
        case 2:
            CalculatedPIOMode = 4;
            break;
        case 1:
            CalculatedPIOMode = 3;
            break;
        default:
            break;
    }

    if (CalculatedPIOMode > *PIOMode) { *PIOMode = 0;}

#ifdef Debug_Level_2
    TRACE ((-1,"Channel Drive %x:%x\n", Channel, Drive));
    TRACE ((-1,"  PioMode = %x\n",*PIOMode));
    TRACE ((-1,"  SWDmaMode = %x\n",*SWDmaMode));
    TRACE ((-1,"  MWDmaMode = %x\n",*MWDmaMode));
    TRACE ((-1,"  UDMAMode = %x\n",*UDmaMode));
#endif

    return EFI_SUCCESS;
}

/**
    This function retrieves a Unicode string that is the user
    readable name of the EFI Driver.

    @param This - A pointer to the EFI_COMPONENT_NAME2_PROTOCOL instance.
    @param Language - A pointer to a three character ISO 639-2 language 
                      identifier. This is the language of the driver name 
                      that that the caller is requesting, and it must match 
                      one of the languages specified in SupportedLanguages.
                      The number of languages supported by a driver is up to
                      the driver writer.
    @param DriverName - A pointer to the Unicode string to return.  This 
                        Unicode string is the name of the driver specified 
                        by This in the language specified by Language.

    @retval EFI_SUCCESS The Unicode string for the Driver specified by This
                        and the language specified by Language was returned
                        in DriverName.
    @retval EFI_INVALID_PARAMETER Language is NULL.
    @retval EFI_INVALID_PARAMETER DriverName is NULL.
    @retval EFI_UNSUPPORTED The driver specified by This does not support the
                            language specified by Language.

    @note  No porting required normally
**/
EFI_STATUS
IdeCtlDriverName(
    IN EFI_COMPONENT_NAME2_PROTOCOL *This,
    IN CHAR8                        *Language,
    OUT CHAR16                      **DriverName
)
{
    if(!Language || !DriverName) return EFI_INVALID_PARAMETER;
    if (MemCmp(Language, "eng", 3)) return EFI_UNSUPPORTED;
    *DriverName = gIdeDriverName;
    return EFI_SUCCESS;
}

/**
    This function retrieves a Unicode string corresponding to the
    IDE controller name

    @param IdeControllerInterfaceData  IDE controller Interface data pointer
    @param PciConfig                   IDE controller PCI configuration data

    @retval EFI_SUCCESS     Always

    @note  No porting required normally
**/
EFI_STATUS
GetIdeControllerName (
    IN OUT IDE_CONTROLLER_PROTOCOL      *IdeControllerInterfaceData,
    IN UINT8                            *PciConfig
)
{

    IdeControllerInterfaceData->Controllername = gIdePataControllerName;
    if (CheckSATAIDEController(PciConfig) == EFI_SUCCESS) 
        IdeControllerInterfaceData->Controllername = gIdeSataControllerName;

    return EFI_SUCCESS;
}


/**
    This function returns the controller capability information
    based on the platform initialization

    @param Controller      Handle to the controller
    @param ControllerInfo  Controller info data to be filled in

    @retval EFI_SUCCESS     Always

    @note  Make use of Controller Handle, if multiple IDE controllers 
           need to be handled in this routine. Open PCI_IO_PROTOCOL 
           and proceed from there.
**/
EFI_STATUS
IdeGetControllerInfo (
    IN EFI_HANDLE               Controller,
    IN OUT CONTROLLER_INFO      *ControllerInfo
)
{

#if IDE_HP_SUPPORT
    UINT8                   PciConfig[256];
    EFI_STATUS              Status;
    EFI_PCI_IO_PROTOCOL     *PciIo;
#endif

    ControllerInfo->PrimaryChannel = PRIMARY_CHANNEL_ENABLE;
    ControllerInfo->PrimaryMasterDevice = PRIMARY_MASTER_DRIVE_ENABLE;
    ControllerInfo->PrimarySlaveDevice = PRIMARY_SLAVE_DRIVE_ENABLE;
    ControllerInfo->SecondaryChannel = SECONDARY_CHANNEL_ENABLE;
    ControllerInfo->SecondaryMasterDevice = SECONDARY_MASTER_DRIVE_ENABLE;
    ControllerInfo->SecondarySlaveDevice = SECONDARY_SLAVE_DRIVE_ENABLE;
    ControllerInfo->BusMasterEnable = BUSMASTER_ENABLE;
    ControllerInfo->HPMask = 0;                 // Hot Plug Mask

#if IDE_HP_SUPPORT
    // Check whether PCI Protocol has been installed on this controller
    Status = pBS->HandleProtocol(Controller,  &gEfiPciIoProtocolGuid, (VOID **)&PciIo);

    // Read the PCI Configuration Space
    if (Status == EFI_SUCCESS) {
        Status = PciIo->Pci.Read (PciIo, EfiPciIoWidthUint8, 0,
                              sizeof (PciConfig), PciConfig);

    if (CheckSATAIDEController(PciConfig) == EFI_SUCCESS)
        ControllerInfo->HPMask = 0xf;       
    }
#endif    // End of IDE_HP_SUPPORT

    return EFI_SUCCESS;
}

/**
    This function retrieves a Unicode string that is the user 
    readable name of the controller that is being managed 
    by an EFI Driver.

    @param This - A pointer to the EFI_COMPONENT_NAME2_PROTOCOL instance.
    @param ControllerHandle - The handle of a controller that the driver 
                              specified by This is managing. This handle 
                              specifies the controller whose name is to be 
                              returned.
    @param ChildHandle - The handle of the child controller to retrieve the 
                         name of. This is an optional parameter that may be 
                         NULL. It will be NULL for device drivers. It will 
                         also be NULL for a bus drivers that wish to retrieve
                         the name of the bus controller. It will not be NULL 
                         for a bus driver that wishes to retrieve the name 
                         of a child controller.
    @param Language - A pointer to a three character ISO 639-2 language
                      identifier.  This is the language of the controller 
                      name that that the caller is requesting, and it must 
                      match one of the languages specified in 
                      SupportedLanguages. The number of languages supported 
                      by a driver is up to the driver writer.
    @param ControllerName - A pointer to the Unicode string to return.  This 
                            Unicode string is the name of the controller 
                            specified by ControllerHandle and ChildHandle in 
                            the language specified by Language from the 
                            point of view of the driver specified by This.

    @retval EFI_SUCCESS - The Unicode string for the user readable name 
                          in the language specified by Language for the 
                          driver specified by This was returned in DriverName.
            EFI_INVALID_PARAMETER - ControllerHandle is not a valid 
                                    EFI_HANDLE or ChildHandle is not NULL 
                                    and it is not a valid EFI_HANDLE or
                                    Language is NULL or when ControllerName 
                                    is NULL.
            EFI_UNSUPPORTED - The driver specified by This is not currently 
                              managing the controller specified by 
                              ControllerHandle and ChildHandle or when the 
                              driver specified by This does not support the 
                              language specified by Language.

    @note  No porting required normally
**/
EFI_STATUS
IdeCtlGetControllerName(
    IN EFI_COMPONENT_NAME2_PROTOCOL *This,
    IN EFI_HANDLE                   Controller,
    IN EFI_HANDLE                   ChildHandle        OPTIONAL,
    IN CHAR8                        *Language,
    OUT CHAR16                      **ControllerName
)
{
    if(!Language || !ControllerName) return EFI_INVALID_PARAMETER;
    if (MemCmp(Language, "eng", 3)) return EFI_UNSUPPORTED;
    *ControllerName = gIdeControllerName;
    return EFI_SUCCESS;

}

/**
    Check whether this controller is an IDE and also whether this
    driver needs to manage this controller or not

    @param PciConfig - Contains the PCI configuration map for the IDE 
                       controller.

    @retval EFI_SUCCESS if the Controller is the one to be handled
    @retval EFI_UNSUPPORTED if this controller is not supported

    @note  If multiple IDE controllers need to be managed, check for all.
            No porting required normally.
**/
EFI_STATUS
CheckIDEController (
    IN UINT8            *PciConfig
)
{
    EFI_STATUS          Status;

    Status = CheckPATAIDEController (PciConfig);
    if (EFI_ERROR(Status)) Status = CheckSATAIDEController (PciConfig);

    return Status;
}

/**
    This routine is called to initialize the IDE controller(s)

    @param PciIo - PCI I/O protocol pointer
    @param PciConfig - Contains the PCI configuration map for the IDE 
                       controller

    @retval VOID

    @note  This routine should program the controller(s) with default register
           values. No porting required normally
**/
EFI_STATUS
InitIdeController (
    EFI_PCI_IO_PROTOCOL         *PciIo,
    UINT8                       *PciConfig
)
{

    if (CheckPATAIDEController(PciConfig) == EFI_SUCCESS)
        InitPATAIdeController (PciIo);
    else 
        if (CheckSATAIDEController(PciConfig) == EFI_SUCCESS)
            InitSATAIdeController (PciIo);

    return EFI_SUCCESS;
}

/**
    This function saves the IDE registers that are needed
    for S3 ACPI support in boot script format

    @param Event       Pointer to the Event handler
    @param Context     Pointer to the context wrapping

    @retval VOID
**/
VOID 
CallbackBootScript(
    IN EFI_EVENT        Event, 
    IN VOID             *Context
)
{
    UINT8                                   i;
    EFI_STATUS                              Status;
    EFI_S3_SAVE_STATE_PROTOCOL              *BootScriptSave;
    EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL         *PciRootBridgeIo;
    
    static BOOLEAN BootScriptSaved          = FALSE;

    if (BootScriptSaved)
        return;

    Status = pBS->LocateProtocol(&gEfiS3SaveStateProtocolGuid, NULL,
                    &BootScriptSave);
    if (EFI_ERROR(Status)) return;

    Status = pBS->LocateProtocol(&gEfiPciRootBridgeIoProtocolGuid, NULL,
                    &PciRootBridgeIo);
    if (EFI_ERROR(Status)) return;

    for (i = 0; i < sizeof(gPciRegistersSave)/ sizeof(BOOT_SCRIPT_PCI_REGISTER_SAVE); ++i) {
        PciRootBridgeIo->Pci.Read(PciRootBridgeIo,
            gPciRegistersSave[i].Width,gPciRegistersSave[i].Address,
            1, &gPciRegistersSave[i].Value);

        BOOT_SCRIPT_S3_PCI_CONFIG_WRITE_MACRO(BootScriptSave,
            gPciRegistersSave[i].Width, gPciRegistersSave[i].Address,
            1, &gPciRegistersSave[i].Value);
    }

    BootScriptSaved = TRUE;
}

/**
    This routine is the entry point for the SBIDE driver. It
    Initializes various controllers and installs Driver Binding
    protocol 

    @param ImageHandle     Handle to this driver image
    @param SystemTable     Pointer to the system table

    @retval EFI_STATUS
**/
EFI_STATUS 
IdeControllerEntryPoint(
    IN EFI_HANDLE        ImageHandle,
    IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
    EFI_STATUS  Status;

    gIdeControllerDriverBinding.DriverBindingHandle=ImageHandle;
    gIdeControllerDriverBinding.ImageHandle=ImageHandle;

    InitAmiLib(ImageHandle, SystemTable);

    //Create event for boot script
    Status = pBS->CreateEvent(
        EFI_EVENT_SIGNAL_READY_TO_BOOT,
        TPL_NOTIFY,
        CallbackBootScript,
        NULL,
        &gEvtBootScript
    );
    ASSERT_EFI_ERROR(Status);

    Status = pBS->InstallMultipleProtocolInterfaces(
                &ImageHandle,
                &gEfiDriverBindingProtocolGuid, &gIdeControllerDriverBinding,
                &gEfiComponentName2ProtocolGuid, &gIdeControllerDriverName,
                NULL
                );

#ifdef Debug_Level_1
    TRACE((-1,"IdeControllerEntryPoint Status = %x\n", Status));
#endif

    SbIde_BoardInit (ImageHandle, SystemTable);

    return Status;
}

/**
    This Driver Binding Protocol function is used to check whether
    this driver can handle the controller passed to this routine

    @param This - Pointer to the driver binding protocol of this controller
    @param Controller - Handle to this controller
    @param RemainingDevicePath - Pointer to the device path protocol

    @retval EFI_STATUS

    @note  Generally no porting needed
**/
EFI_STATUS 
IdeControllerSupported (
    IN EFI_DRIVER_BINDING_PROTOCOL    *This,
    IN EFI_HANDLE                     Controller,
    IN EFI_DEVICE_PATH_PROTOCOL       *RemainingDevicePath )
{

    EFI_STATUS                  Status;
    EFI_PCI_IO_PROTOCOL         *PciIo;
    EFI_DEVICE_PATH_PROTOCOL    *ParentDevicePath;          
    UINT8                       PciConfig[256];

#ifdef Debug_Level_1
    EfiDebugPrint(-1,"Entring IdeControllerSupported\n");
#endif  

//  Check whether DeviePath Protocol has been installed on this controller  
    Status = pBS->OpenProtocol( Controller,
                                &gEfiDevicePathProtocolGuid,
                                (VOID **)&ParentDevicePath,
                                This->DriverBindingHandle,     
                                Controller,   
                                EFI_OPEN_PROTOCOL_BY_DRIVER );

#ifdef Debug_Level_3
    EfiDebugPrint(-1,"Open gEfiDevicePathProtocolGuid Status = %x\n", Status);
#endif  
    if  ( EFI_ERROR(Status) ) return Status;

//  Close DevicePathProtocol
    pBS->CloseProtocol (
        Controller,
        &gEfiDevicePathProtocolGuid,
        This->DriverBindingHandle,
        Controller                
        );

//  Check whether PCI Protocol has been installed on this controller
    Status = pBS->OpenProtocol( Controller,
                                &gEfiPciIoProtocolGuid,
                                (VOID **)&PciIo,
                                This->DriverBindingHandle,     
                                Controller,   
                                EFI_OPEN_PROTOCOL_BY_DRIVER );

    #ifdef Debug_Level_3
        EfiDebugPrint(-1,"Open gEfiPciIoProtocolGuid Status = %x\n", Status);
    #endif  
    if  ( EFI_ERROR(Status) ) return Status;


//  Read the PCI Configuration Space
    Status =    PciIo->Pci.Read (
                    PciIo,
                    EfiPciIoWidthUint8,     
                    0,
                    sizeof (PciConfig),
                    PciConfig );

//  Close PciIoProtocol. No further use.
    pBS->CloseProtocol (
            Controller,
            &gEfiPciIoProtocolGuid,
            This->DriverBindingHandle,
            Controller                
        );

    if  ( EFI_ERROR(Status) ) {
        return EFI_UNSUPPORTED;
    }

#ifdef Debug_Level_2
    EfiDebugPrint(-1," IDE Device ID = %x%x\n", PciConfig[0x3], PciConfig[0x2]);
    EfiDebugPrint(-1," IDE Vendor ID = %x%x\n", PciConfig[0x1], PciConfig[0x0]);
#endif  

    Status = CheckIDEController (PciConfig);

    if (EFI_ERROR(Status)) return EFI_UNSUPPORTED;      //This is not the device (PCI IDE controller) we are looking for    

//  Check if IDEControllerProtocol already installed
    Status = pBS->OpenProtocol( Controller,
                                &gEfiIdeControllerProtocolGuid,
                                NULL,                   // Optional
                                This->DriverBindingHandle,     
                                Controller,   
                                EFI_OPEN_PROTOCOL_TEST_PROTOCOL );

    if  ( Status == EFI_SUCCESS ) {
        return EFI_ALREADY_STARTED;
    }

#ifdef Debug_Level_1
    EfiDebugPrint(-1," IdeControllerSupported Exited Successfully\n");
#endif  

    return EFI_SUCCESS;
}


/**
    This Driver Binding Protocol function is used to start the 
    IDE controller and install appropriate controller protocol

    @param This - Pointer to the driver binding protocol of this controller
    @param Controller - Handle to this controller
    @param RemainingDevicePath - Pointer to the device path protocol

    @retval EFI_STATUS

    @note  Generally no porting needed
**/
EFI_STATUS 
IdeControllerStart (
    IN EFI_DRIVER_BINDING_PROTOCOL    *This,
    IN EFI_HANDLE                     Controller,
    IN EFI_DEVICE_PATH_PROTOCOL       *RemainingDevicePath )
{

    EFI_STATUS                  Status;
    EFI_PCI_IO_PROTOCOL         *PciIo;
    UINT8                       PciConfig[256];
    IDE_CONTROLLER_PROTOCOL     *IdeControllerInterfaceData;

#ifdef Debug_Level_1
    EfiDebugPrint(-1," IdeControllerStart Entered Successfully\n");
#endif  

//  Check whether PCI Protocol has been installed on this controller
    Status = pBS->OpenProtocol( Controller,
                                &gEfiPciIoProtocolGuid,
                                (VOID **)&PciIo,
                                This->DriverBindingHandle,     
                                Controller,   
                                EFI_OPEN_PROTOCOL_BY_DRIVER );

    if  ( EFI_ERROR(Status) ) return Status;


//  Install IDEController Protocol
    Status = pBS->AllocatePool (EfiBootServicesData,
                                sizeof(IDE_CONTROLLER_PROTOCOL),
                                (VOID**)&IdeControllerInterfaceData
                              );
    if (EFI_ERROR(Status)) {
        Status = pBS->CloseProtocol( Controller,
                        &gEfiPciIoProtocolGuid,
                        This->DriverBindingHandle,     
                        Controller);
        return EFI_OUT_OF_RESOURCES;
    }

    ZeroMemory (IdeControllerInterfaceData, sizeof(IDE_CONTROLLER_PROTOCOL));

#ifdef Debug_Level_3
    EfiDebugPrint(-1,"Before Ide Init\n");
#endif  

//  Read the PCI Config Space
    Status =    PciIo->Pci.Read (
                    PciIo,
                    EfiPciIoWidthUint8,     
                    0,
                    0x20,
                    PciConfig );

//  Initialize IDE controller
    InitIdeController (PciIo, PciConfig);

    #ifdef Debug_Level_3
        EfiDebugPrint(-1,"After Ide Init\n");
    #endif  

//  Initialize Data Area
    IdeControllerInterfaceData->PciIO = PciIo;
    IdeControllerInterfaceData->ControllerHandle = Controller;
    IdeControllerInterfaceData->IdeGetControllerInfo = IdeGetControllerInfo; 
    IdeControllerInterfaceData->IdeSetPioMode = IdeControllerSetPioModeTiming;
    IdeControllerInterfaceData->IdeSetDmaMode = IdeControllerSetDmaModeTiming;
    IdeControllerInterfaceData->GetbestPioDmaMode = GetbestPioDmaMode;
    IdeControllerInterfaceData->HPCheckForDeviceChange = HPCheckForDeviceChange;
    IdeControllerInterfaceData->HPDisableEnableChannel= HPDisableEnableChannel;
    IdeControllerInterfaceData->Max_Channel = MAXIMUM_CHANNELS;
    GetIdeControllerName (IdeControllerInterfaceData, PciConfig);
//  Install Protocol on the Controller Handle
    Status = pBS->InstallProtocolInterface (
                    &Controller,
                    &gEfiIdeControllerProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    IdeControllerInterfaceData);
    if (!EFI_ERROR(Status) && CheckSATAIDEController(PciConfig) == EFI_SUCCESS)
        Status = pBS->InstallProtocolInterface (
                    &Controller,
                    &gEfiSataControllerProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    NULL);

#ifdef Debug_Level_3
    EfiDebugPrint(-1,"gEfiIdeControllerProtocolGuid Status = %x\n", Status);
#endif  

    if (EFI_ERROR(Status)) {
        Status = pBS->FreePool (IdeControllerInterfaceData);
        Status = pBS->CloseProtocol (Controller,
                        &gEfiPciIoProtocolGuid,
                        This->DriverBindingHandle,     
                        Controller);
        return Status;
    }           

#ifdef Debug_Level_1
    EfiDebugPrint(-1," IdeControllerStart Exited Successfully\n");
#endif  

// Enable the Controller
    PciConfig[4] = IO_ENABLE | BUS_MASTER_ENABLE;
    Status =    PciIo->Pci.Write (
                    PciIo,
                    EfiPciIoWidthUint8,     
                    4,
                    1,
                    &(PciConfig[4]));

    return EFI_SUCCESS;
}

/**
    This Driver Binding Protocol function is used to stop the 
    IDE controller by uninstalling the controller protocol published

    @param This - Pointer to the driver binding protocol of this controller
    @param Controller - Handle to this controller
    @param RemainingDevicePath - Pointer to the device path protocol

    @retval EFI_STATUS

    @note  Generally no porting needed
**/
EFI_STATUS 
IdeControllerStop (
    IN EFI_DRIVER_BINDING_PROTOCOL    *This,
    IN EFI_HANDLE                     Controller,
    IN UINTN                          NumberOfChildren,
    IN EFI_HANDLE                     *ChildHandleBuffer
 )
{
    EFI_STATUS                      Status;
    IDE_CONTROLLER_PROTOCOL         *IdeControllerInterfaceData;

#ifdef Debug_Level_1
    EfiDebugPrint(-1,"IdeControllerStop Entered\n");
#endif  

    Status = pBS->OpenProtocol( Controller,
                                &gEfiIdeControllerProtocolGuid,
                                (VOID **)&IdeControllerInterfaceData,
                                This->DriverBindingHandle,     
                                Controller,   
                                EFI_OPEN_PROTOCOL_GET_PROTOCOL);

//  Uninstall protocol on the Controller Handle
    Status = pBS->UninstallProtocolInterface (
                    &Controller,
                    &gEfiIdeControllerProtocolGuid,
                    IdeControllerInterfaceData);


#ifdef Debug_Level_1
    EfiDebugPrint(-1,"Uninstall gEfiIdeControllerProtocolGuid status %x\n", Status);
#endif  

    if (EFI_ERROR(Status)) {
        return EFI_DEVICE_ERROR;
    }

    Status = pBS->CloseProtocol( Controller,
                                &gEfiPciIoProtocolGuid,
                                This->DriverBindingHandle,     
                                Controller);

    if (EFI_ERROR(Status)) {
        return EFI_DEVICE_ERROR;
    }

    Status = pBS->FreePool (IdeControllerInterfaceData);

#ifdef Debug_Level_1
    EfiDebugPrint(-1,"IdeControllerStop Exited successfully\n");
#endif  
    return Status;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
