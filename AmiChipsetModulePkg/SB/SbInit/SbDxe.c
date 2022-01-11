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

/** @file SbDxe.c
    This file contains code for Template South bridge initialization
    in the DXE stage

*/

// Module specific Includes
#include <Efi.h>
#include <token.h>
#include <AmiLib.h>
#include <AmiDxeLib.h>
#include <AmiCspLib.h>
#include <PCI.h>
// Consumed Protocols
#include <Protocol/PciIo.h>
#include <Protocol/Cpu.h>
#include <Protocol/PciRootBridgeIo.h>

// Produced Protocols
#include <Protocol/RealTimeClock.h>
#include <Protocol/WatchdogTimer.h>
#include <Protocol/Legacy8259.h>
#include <Protocol/Timer.h>
#include <Protocol/SbSataProtocol.h>

#include <Sb.h>
#if defined AMIUSB_SUPPORT && AMIUSB_SUPPORT == 1
#include <Protocol/AmiUsbController.h>
#else
#define AMIUSB_SUPPORT 0
#endif

// Build flag adjustments
#ifndef     SMM_SUPPORT
#define     SMM_SUPPORT         0
#endif

// Constant definitions
//----------------------------------------------------------------------------
//          Timer Constants
//----------------------------------------------------------------------------
#define SYSTEM_TIMER_IRQ 0

// Cpu I/O space defines
#define TIMER_CTRL              0x43
#define TIMER_0_COUNT           0x40
#define TIMER_1_COUNT           0x41

// Timer Period
#define TIMER_TICK            838 //ns

// default duration is 0xffff ticks
#define DEFAULT_TICK_DURATION ((65535*838+50)/100)
#define MAX_TICK_DURATION DEFAULT_TICK_DURATION

//8259 PIC defines
#define ICW1            0x11    // Slave exists and ICW4 required.
#define ICW3_M          1 << 2  // IRQ 2 connects to slave
#define ICW3_S          2       // IRQ 2 connects to master
#define ICW4            1       // Bit 4 Normal Nested Mode
                                // Bit 3 Non-buffered Mode
                                // Bit 2 Unused with non-buffered mode
                                // Bit 1 Set manual EOI instead of automatic
                                // Bit 0 8086/8088 mode

#define OCW1_M          0xff    // Master Mask
#define OCW1_S          0xff    // Slave Mask

#define EOI_COMMAND     0x20    // EOI Command

#define INTERRUPTS_TRIGGER_REG    0x4d0   //Trigger for Interrupts (Edge or Level).
#define INTERRUPTS_EDGE_TRIGGER   0       //Set all interrupts at edge level.

//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
//          Variable Declaration
//----------------------------------------------------------------------------
#if defined SB_TIMER_ARCH_PROTOCOL_SUPPORT && SB_TIMER_ARCH_PROTOCOL_SUPPORT == 1
EFI_HANDLE  mTimerProtocolHandle        = NULL;
EFI_EVENT   gEvtTimerArch               = NULL;
VOID        *gRegTimerArch              = NULL;
UINT64      mProgrammedTimerValue;
EFI_TIMER_NOTIFY            mNotifyFunction;
EFI_LEGACY_8259_PROTOCOL    *mLegacy8259;
#endif
#if defined SB_WATCHDOG_TIMER_PROTOCOL_SUPPORT && SB_WATCHDOG_TIMER_PROTOCOL_SUPPORT == 1
EFI_EVENT   mWatchdogEvent;
UINT64      mWatchdogPeriod             = 0;
EFI_WATCHDOG_TIMER_NOTIFY   mWatchdogNotifyFunction = NULL;
#endif
EFI_EVENT   gEvtBiosSecurity = NULL;
EFI_EVENT   gEvtUsbProtocol = NULL;

// GUID Definitions

//This the number of days in a month - 1. (0 Based)
#if defined SB_8259_PROTOCOL_SUPPORT && SB_8259_PROTOCOL_SUPPORT == 1
UINT8       gMasterBase, 
            gSlaveBase;
UINT8       gMode                       = 1;                    //Initially in protected mode. (0 = Real, 1 = 32 bit protected)
UINT16      gIrqMask[2]                 = {0xffff,0xffff};      //Initially all Real IRQs masked, protected masked
UINT16      gIrqTrigger[2]              = {0,0};                //Initially all Real IRQs Edge, protected Edge.
#endif


EFI_RUNTIME_SERVICES        *gRT;
EFI_BOOT_SERVICES           *gBS;
EFI_S3_SAVE_STATE_PROTOCOL  *gBootScript = NULL;
VOID                        *gRegBiosSecurity = NULL;
VOID                        *gRegUsbProtocol = NULL;

#if AMIUSB_SUPPORT
EFI_USB_PROTOCOL            *gUsbProtocol = NULL;
#endif

#if defined(HPET_APIC_INTERRUPT_MODE) && (HPET_APIC_INTERRUPT_MODE != 0)
extern UINT8 gBspLocalApicID;
#endif

extern EFI_GUID gBdsConnectDriversProtocolGuid;

EFI_STATUS
GeneratePortNumber(	 
    IN    UINT8    DeviceNumber,
    IN    UINT8    FunctionNumber,
    IN    UINT32   IdeChannel,
    IN    UINT32   IdeDevice,
    OUT   UINT32   *PortNumber
);

SB_HDD_POLICY_PROTOCOL mSbHddPolicy = {	GeneratePortNumber };

//----------------------------------------------------------------------------

// Function Prototypes
EFI_STATUS SbDxeBoardInit (
    IN EFI_HANDLE                 ImageHandle,
    IN EFI_SYSTEM_TABLE           *SystemTable
);

EFI_STATUS CountTime(
    IN  UINTN   DelayTime, 
    IN  UINT16  BaseAddr
);

#if defined SB_WATCHDOG_TIMER_PROTOCOL_SUPPORT && SB_WATCHDOG_TIMER_PROTOCOL_SUPPORT == 1
EFI_STATUS WatchdogInit (
    IN EFI_HANDLE                 ImageHandle,
    IN EFI_SYSTEM_TABLE           *SystemTable
);

VOID EFIAPI WatchdogHandler (
    IN EFI_EVENT                  Event,
    IN VOID                       *Context
);

EFI_STATUS RegisterHandler(
    IN EFI_WATCHDOG_TIMER_ARCH_PROTOCOL  *This,
    IN EFI_WATCHDOG_TIMER_NOTIFY         NotifyFunction
);

EFI_STATUS WatchdogSetTimerPeriod(
    IN EFI_WATCHDOG_TIMER_ARCH_PROTOCOL  *This,
    IN UINT64                            TimerPeriod
);

EFI_STATUS WatchdogGetTimerPeriod(
    IN EFI_WATCHDOG_TIMER_ARCH_PROTOCOL  *This,
    IN UINT64                            *TimerPeriod
);
#endif

#if defined SB_8259_PROTOCOL_SUPPORT && SB_8259_PROTOCOL_SUPPORT == 1
EFI_STATUS Initialize8259 (
    IN EFI_HANDLE                 ImageHandle,
    IN EFI_SYSTEM_TABLE           *SystemTable
);
#endif

#if defined SMM_SUPPORT && SMM_SUPPORT == 1
EFI_STATUS SbSmmInit(
    IN EFI_HANDLE                 ImageHandle,
    IN EFI_SYSTEM_TABLE           *SystemTable
);
#endif

#if defined SB_TIMER_ARCH_PROTOCOL_SUPPORT && SB_TIMER_ARCH_PROTOCOL_SUPPORT == 1
VOID TimerInitWrapper(
    IN EFI_EVENT                Event,
    IN VOID                     *Context
);

EFI_STATUS TimerInit(
    VOID
);

EFI_STATUS TimerRegisterHandler(
    EFI_TIMER_ARCH_PROTOCOL     *This, 
    EFI_TIMER_NOTIFY            NotifyFunction
);

EFI_STATUS SetTimerPeriod(
    EFI_TIMER_ARCH_PROTOCOL     *This, 
    UINT64                      TimerPeriod
);

EFI_STATUS GetTimerPeriod(
    EFI_TIMER_ARCH_PROTOCOL     *This, 
    UINT64                      *TimerPeriod
);

EFI_STATUS GenerateSoftIntr(
    EFI_TIMER_ARCH_PROTOCOL     *This
);
#endif

EFI_STATUS InstallSbPolicyProtocol (VOID);

VOID InitSbRegsBeforeBoot (
    IN EFI_EVENT        Event,
    IN VOID             *Context
);

#if BIOS_LOCK_ENABLE
VOID SbCallbackBiosSecurity (
    IN EFI_EVENT        Event,
    IN VOID             *Context
); 
#endif

#if AMIUSB_SUPPORT
VOID SbUsbProtocolCallback (
    IN EFI_EVENT        Event,
    IN VOID             *Context
);
#endif

#if defined SB_WATCHDOG_TIMER_PROTOCOL_SUPPORT && SB_WATCHDOG_TIMER_PROTOCOL_SUPPORT == 1
// Architectural Protocol Definitions
EFI_WATCHDOG_TIMER_ARCH_PROTOCOL mWatchdog = {
  RegisterHandler,
  WatchdogSetTimerPeriod,
  WatchdogGetTimerPeriod
};
#endif

#if defined SB_TIMER_ARCH_PROTOCOL_SUPPORT && SB_TIMER_ARCH_PROTOCOL_SUPPORT == 1
EFI_TIMER_ARCH_PROTOCOL mTimerProtocol = {
  TimerRegisterHandler,
  SetTimerPeriod,
  GetTimerPeriod,
  GenerateSoftIntr
};
#endif

// Function Definitions

/**
    This function is the entry point for this DXE. This function
    initializes the chipset SB

    @param ImageHandle Image handle
    @param SystemTable Pointer to the system table

    @retval Return Status based on errors that occurred while waiting for
            time to expire.

**/
EFI_STATUS SbDxeInit (
    IN EFI_HANDLE                 ImageHandle,
    IN EFI_SYSTEM_TABLE           *SystemTable
)
{

    EFI_STATUS                          Status = EFI_SUCCESS;
    EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL     *PciRootBridgeIo;
    EFI_EVENT                           BeforeBootEvent;
//  UINT8                               ByteReg;
    //UINT32                              *pIOAPIC; // Denverton AptioV Override Start

#if defined(HPET_APIC_INTERRUPT_MODE) && (HPET_APIC_INTERRUPT_MODE != 0)
    CPUINFO_HOB                         *CpuInfoHob;
    EFI_GUID                            AmiCpuinfoHobGuid = AMI_CPUINFO_HOB_GUID;
#endif
    InitAmiLib(ImageHandle, SystemTable);

        //Report Progress code
        PROGRESS_CODE (DXE_SB_INIT);

    Status = pBS->LocateProtocol (&gEfiPciRootBridgeIoProtocolGuid, NULL, &PciRootBridgeIo);
        if (Status != EFI_SUCCESS) goto Init_Error;

    Status = pBS->LocateProtocol( &gEfiS3SaveStateProtocolGuid, NULL, &gBootScript );
        if (Status != EFI_SUCCESS) goto Init_Error;

#if defined(HPET_APIC_INTERRUPT_MODE) && (HPET_APIC_INTERRUPT_MODE != 0)
    //Find APIC ID HOB.
    CpuInfoHob = (CPUINFO_HOB*)GetEfiConfigurationTable( SystemTable,
                                                         &gEfiHobListGuid );
    if (CpuInfoHob == NULL) Status = EFI_UNSUPPORTED;
    else Status = FindNextHobByGuid( &AmiCpuinfoHobGuid, (VOID**)&CpuInfoHob);
    ASSERT_EFI_ERROR(Status);

    gBspLocalApicID = CpuInfoHob->Cpuinfo[0].ApicId;
#endif
    // Put the I/O APIC in Virtual wire mode

// CHIPSET PROGRAMMING TO ENABLE IOAPIC GOES HERE
// Denverton AptioV Override Start - Needs to be enabled once eds comes
/*
    pIOAPIC = (UINT32 *)APCB;
    *pIOAPIC = 0;                   // IO APIC index (0)
    *(pIOAPIC+4) = 0x2000000;       // New ID (at offset 0x10)
    *pIOAPIC = 0x10;                // Index 0x10
    *(pIOAPIC+4) = 0x700;           // INT0 - EXT INT unmasked
*/
// Denevrton AptioV Override End
    // Enable Top of 4GB flash ROM access

// CHIPSET PROGRAMMING TO ENABLE TOP of 4GB ROM ACCESS GOES HERE

    // Install 8259 services
#if defined SB_8259_PROTOCOL_SUPPORT && SB_8259_PROTOCOL_SUPPORT == 1
    Initialize8259(ImageHandle, SystemTable);
#endif

    // Install watchdog timer services
#if defined SB_WATCHDOG_TIMER_PROTOCOL_SUPPORT && SB_WATCHDOG_TIMER_PROTOCOL_SUPPORT == 1
    WatchdogInit(ImageHandle, SystemTable);
#endif

    // Install Legacy timer services
#if defined SB_TIMER_ARCH_PROTOCOL_SUPPORT && SB_TIMER_ARCH_PROTOCOL_SUPPORT == 1
    Status = pBS->LocateProtocol (&gEfiLegacy8259ProtocolGuid, NULL, &mLegacy8259);
    if (Status == EFI_SUCCESS) {
      TimerInit();
    } else {
      Status = RegisterProtocolCallback ( &gEfiLegacy8259ProtocolGuid,
                                          TimerInitWrapper,
                                          NULL,
                                          &gEvtTimerArch,
                                          &gRegTimerArch );
    }
#endif

#if     SMM_SUPPORT
    Status = SbSmmInit (ImageHandle, SystemTable);

    if (Status != EFI_SUCCESS) {
        TRACE ((TRACE_ALWAYS, "SbSmmInit() = %r\n", Status));
        goto Init_Error;
    }
#endif

    InstallSbPolicyProtocol();

    Status = SbDxeBoardInit (ImageHandle, SystemTable);

#if BIOS_LOCK_ENABLE
    Status = RegisterProtocolCallback ( &gBdsConnectDriversProtocolGuid,
                                        SbCallbackBiosSecurity,
                                        NULL,
                                        &gEvtBiosSecurity,
                                        &gRegBiosSecurity );
#endif

#if AMIUSB_SUPPORT
    Status = RegisterProtocolCallback( &gEfiUsbProtocolGuid,
                                       SbUsbProtocolCallback,
                                       NULL,
                                       &gEvtUsbProtocol,
                                       &gRegUsbProtocol );
#endif

    Status = CreateReadyToBootEvent( TPL_NOTIFY, 
                                     InitSbRegsBeforeBoot,
                                     NULL, 
                                     &BeforeBootEvent );

Init_Error:
    if (Status != EFI_SUCCESS) {
      //Report Error code
      ERROR_CODE (DXE_SB_ERROR, EFI_ERROR_MAJOR);
      ASSERT_EFI_ERROR(Status);
    }

    return Status;
}

/**
    This function can initialize any SB registers before DXE
    stage exiting.

    @param Event   - Event of callback
    @param Context - Context of callback.

    @retval VOID
**/
VOID InitSbRegsBeforeBoot (
    IN EFI_EVENT                  Event,
    IN VOID                       *Context
)
{
/**** PORTING REQUIRED ****

    if (gSetupData->Usb11 == 0) {   // If all USB controllers are disabled
        if (gUsbProtocol != NULL)
            gUsbProtocol->UsbRtShutDownLegacy();
        WRITE_PCI16_USB(USB_REG_PCICMD, 0);
        WRITE_PCI16_USB2(USB_REG_PCICMD, 0);
        WRITE_PCI16_USB3(USB_REG_PCICMD, 0);
        WRITE_PCI16_USB4(USB_REG_PCICMD, 0);
        WRITE_PCI16_EUSB(USB20_REG_PCICMD, 0);
    }

    // Disable & Hide all disabled USB controllers

****/
    pBS->CloseEvent(Event);
}

#if BIOS_LOCK_ENABLE
/**
    SB call back function before legacy boot.

    @param Event   - Event of callback   
    @param Context - Context of callback

    @retval VOID

**/
VOID SbCallbackBiosSecurity (
    IN EFI_EVENT            Event,
    IN VOID                 *Context
)
{
/**** PORTING REQUIRED ****
    UINT8   Buffer8 = 0;
    UINT32  Buffer32 = 0;                   // (P050509E)
    UINT32  D31F0Addr = (UINT32)(UINTN)SB_PCIE_CFG_ADDRESS(LPC_BUS, LPC_DEVICE, LPC_FUNC, ICH_REG_LPC_VID); // (P122208D)

    TRACE((TRACE_ALWAYS, "[[ SbPchCallbackBiosSecurity() Start. ]]\n"));
    // Enable BIOS Lock & SMM_BWP
    Buffer8 = MmPci8(LPC_BUS, LPC_DEVICE, LPC_FUNC, ICH_REG_LPC_BIOS_CNTL) |= (BIT01+BIT05);
    BOOT_SCRIPT_S3_MEM_WRITE_MACRO(gBootScriptSave,
                                   EfiBootScriptWidthUint8,
                                   (D31F0Addr + ICH_REG_LPC_BIOS_CNTL),
                                   1,
                                   &Buffer8);

    // Enable BIOS Interface Lock-Down
    Buffer32 = Mmio32(SB_RCBA, ICH_RCRB_GCS);  // SB_RCBA + 0x3410
    Buffer32 |= (BIT00);
    Mmio32(SB_RCBA, ICH_RCRB_GCS) = Buffer32;
    BOOT_SCRIPT_S3_MEM_WRITE_MACRO(gBootScriptSave,
                                   EfiBootScriptWidthUint32,
                                   (SB_RCBA + ICH_RCRB_GCS),
                                   1,
                                   &Buffer32);
                                        // <(P111009A)
    TRACE((TRACE_ALWAYS, "[[ SbPchCallbackBiosSecurity() Done. ]]\n"));
****/
    pBS->CloseEvent(Event);
}
#endif

#if AMIUSB_SUPPORT
/**
    This callback function is called after USB Protocol is 
    installed.

    @param Event   - Event of callback
    @param Context - Context of callback.

    @retval VOID
**/
VOID SbUsbProtocolCallback (
    IN EFI_EVENT                  Event,
    IN VOID                       *Context
)
{
    EFI_STATUS              Status = EFI_SUCCESS;
/**** PORTING REQUIRED ****
    // This example is skipped all USB mass storage initialization
    // If all USB controllers are disabled
    USB_SKIP_LIST SkipMassTable[] = { {1, 0, 0xff, 0, 0, 0x8},
                                      {0, 0, 0, 0, 0, 0}};
    
    if (gSetupData->Usb11 == 0) {   // If all USB controllers are disabled
        Status = pBS->LocateProtocol( &gEfiUsbProtocolGuid,
                                      NULL,
                                      &gUsbProtocol );

        gUsbProtocol->UsbCopySkipTable(SkipMassTable, sizeof(SkipMassTable)/sizeof (USB_SKIP_LIST));
    }
****/

    pBS->CloseEvent(Event);
}
#endif

/**
    This function returns the Port Number to which the Hard Disk 
    is connected.        

    @param DeviceNumber - PCI Device Number of the IDE/SATA Controller
    @param FunctionNumber - PCI Function Number of the IDE/SATA Controller
    @param IdeChannel -  IDE Channel
                         0 - Primary Channel
                         1 - Secondary Channel
    @param IdeDevice - IDE Device
                       0 - Master
                       1 - Salve
    @param PortNumber - Pointer to the Port Number returned by this function. 
    
    @retval EFI_SUCCESS       

**/
EFI_STATUS GeneratePortNumber (	 
    IN    UINT8    DeviceNumber,
    IN    UINT8    FunctionNumber,
    IN    UINT32   IdeChannel,
    IN    UINT32   IdeDevice,
    OUT   UINT32   *PortNumber
)
{
/** PORTING REQUIRED for the Chipset**/

    UINT32     SataPortIndex;

    SataPortIndex = (IdeChannel * 2) + IdeDevice;
 
    //
    // SINDX Serial ATA Index (D31:F2)
    //
    if (SataPortIndex == 1 || SataPortIndex == 2) {
        SataPortIndex ^= 3;
    }

    //
    //Port 4, 5 in SATA2
    //
//    if (FunctionNumber == SATA2_FUN) {
    if (FunctionNumber == 0) {
        SataPortIndex += 4;   
    }
 
    *PortNumber = SataPortIndex;

    return  EFI_SUCCESS;
}

//----------------------------------------------------------------------------
//   USUALLY NO PORTING REQUIRED FOR THE FOLLOWING ROUTINES
//----------------------------------------------------------------------------

#if defined SB_WATCHDOG_TIMER_PROTOCOL_SUPPORT && SB_WATCHDOG_TIMER_PROTOCOL_SUPPORT == 1
/**
    This function is called when the watchdog timer event is 
    signalled.  It calls the registered handler and then
    resets the system

    @param Event   - Watchdog event
    @param Context - Context pointer

    @retval VOID
**/
VOID EFIAPI WatchdogHandler (
    IN EFI_EVENT                  Event,
    IN VOID                       *Context
)
{
    //
    // Call the registered handler if there is one
    //
    if (mWatchdogNotifyFunction != NULL) {
        mWatchdogNotifyFunction (mWatchdogPeriod);
    }
    //
    // Reset the system
    //
    pRS->ResetSystem( EfiResetCold, EFI_TIMEOUT, 0, NULL );
}

/**
    This function registers a handler that is called when the Timer
    event has been signalled

    @param This - Pointer to the instance of the Architectural Protocol
    @param NotifyFunction - The function to call when the interrupt fires

    @retval EFI_STATUS
            EFI_SUCCESS   When new handle is registered
            EFI_ALREADY_STARTEd If notify function is already defined

**/
EFI_STATUS EFIAPI RegisterHandler (
    IN EFI_WATCHDOG_TIMER_ARCH_PROTOCOL *This,
    IN EFI_WATCHDOG_TIMER_NOTIFY        NotifyFunction
)
{
    //
    // only register the handler if it is still NULL
    //
    if (NotifyFunction && mWatchdogNotifyFunction) return EFI_ALREADY_STARTED;
    if (!NotifyFunction && !mWatchdogNotifyFunction) return EFI_INVALID_PARAMETER;

    mWatchdogNotifyFunction = NotifyFunction;

    return EFI_SUCCESS;
}

/**
    This function sets the timer period before the watchdog goes off
    every TimerPeriod number of 100ns intervals, if the period is set to 0 the
    timer event is cancelled

    @param This - Pointer to the instance of the Architectural Protocol
    @param TimerPeriod - The number of 100ns intervals to which the watchdog
                         will be programmed.

    @retval EFI_SUCCESS

**/
EFI_STATUS EFIAPI WatchdogSetTimerPeriod (
    IN EFI_WATCHDOG_TIMER_ARCH_PROTOCOL *This,
    IN UINT64                           TimerPeriod
)
{
    EFI_TIMER_DELAY  TimerDelayType;

    //
    // store new timer length
    //
    mWatchdogPeriod = TimerPeriod;
    //
    // cancel timer event if Timer Period is 0
    //
    TimerDelayType = (TimerPeriod) ? TimerRelative : TimerCancel;
    //
    // set the timer for the event
    //
    return pBS->SetTimer(mWatchdogEvent, TimerDelayType, mWatchdogPeriod);
}


/**
    This function returns the current watchdog timer period

    @param This - Pointer to the instance of the Architectural Protocol
    @param TimerPeriod - Pointer to a memory location to load the current 
                         Timer period into

    @retval TimerPeriod Current Timer Period if function returns EFI_SUCCESS

**/
EFI_STATUS EFIAPI WatchdogGetTimerPeriod(
    IN EFI_WATCHDOG_TIMER_ARCH_PROTOCOL *This,
    IN UINT64                           *TimerPeriod
)
{
    //
    // return the current Watchdog period
    //
    *TimerPeriod = mWatchdogPeriod;

    return EFI_SUCCESS;
}


/**
    This function installs the the Watchdog Timer protocol on its
    handle, and initializes the Watchdog timer.

    @param ImageHandle - ImageHandle of the loaded driver
    @param SystemTable - Pointer to the System Table

    @retval EFI_SUCCESS The Watchdog Timer protocol was
                        installed.
    @retval EFI_OUT_OF_RESOURCES Space for a new handle could not
                                 be allocated.
    @retval EFI_INVALID_PARAMETER One of the parameters has an 
                                  invalid value.
**/
EFI_STATUS WatchdogInit (
    IN EFI_HANDLE                 ImageHandle,
    IN EFI_SYSTEM_TABLE           *SystemTable
)
{
    EFI_STATUS  Status;
    EFI_HANDLE  WatchdogHandle = NULL;

    //
    // Use the Timer event to trigger the Watchdog.  No specific hardware
    // exists for this
    //
    Status = pBS->CreateEvent (
                    EVT_TIMER | EVT_NOTIFY_SIGNAL,
                    TPL_NOTIFY,
                    WatchdogHandler,
                    NULL,
                    &mWatchdogEvent
                    );
    //
    // Create a handle for the ArchProtocol and install Watchdog Arch
    // Protocol on the handle
    //
    Status = pBS->InstallProtocolInterface (
                    &WatchdogHandle,
                    &gEfiWatchdogTimerArchProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    &mWatchdog
                    );

    return Status;
}
#endif

#if defined(HPET_PROTOCOL_SUPPORT) && (HPET_PROTOCOL_SUPPORT != 0)
// Mask used for counter and comparator calculations to adjust for a 32-bit or 64-bit counter.
UINT64  gCounterMask;
// Cached state of the HPET General Capabilities register managed by this driver.
// Caching the state reduces the number of times the configuration register is read.
volatile HPET_GENERAL_CAPABILITIES_ID_REGISTER   gHpetGeneralCapabilities;
// Cached state of the HPET General Configuration register managed by this driver.
// Caching the state reduces the number of times the configuration register is read.
volatile HPET_GENERAL_CONFIGURATION_REGISTER     gHpetGeneralConfiguration;
// Cached state of the Configuration register for the HPET Timer managed by 
// this driver.  Caching the state reduces the number of times the configuration
// register is read.
volatile HPET_TIMER_CONFIGURATION_REGISTER       gTimerConfiguration;

EFI_EVENT                               gHpetLegacyBootEvent;

/**
    This function enables HPET register decode.

    @param VOID

    @retval VOID

    @note  Porting required.
**/
VOID EnableHpetInChipset( VOID )
{
    // Porting required.
//    WRITE_MEM32_RCRB_S3(gBootScript, RCRB_MMIO_HPTC, ((HPET_BASE_ADDRESS >> 12) & 3) | 0x80);
}

/**
    This function reads a 64-bit register from the HPET register.

    @param Offset - Specifies the offset of the HPET register to read.

    @retval The 64-bit value read from the HPET register specified by
            Offset.

    @note  No porting required.
**/
UINT64 HpetRead (
    IN UINTN        Offset )
{
    return MMIO_READ64( HPET_BASE_ADDRESS + Offset );
}

/**
    This function writes a 64-bit HPET register.

    @param Offset - Specifies the offSet of the HPET register to write.
    @param Value  - Specifies the value to write to the HPET register
                    specified by Offset.

    @retval The 64-bit value written to HPET register specified by Offset.

    @note  No porting required.
**/
UINT64 HpetWrite (
    IN UINTN        Offset,
    IN UINT64       Value )
{
    MMIO_WRITE64( HPET_BASE_ADDRESS + Offset, Value );
    return HpetRead( Offset );
}

/**
    This function enables or disables the main counter in the
    HPET Timer.

    @param Enable  TRUE  - Enable the main counter in the HPET Timer.
                   FALSE - Disable the main counter in the HPET Timer.
    @retval VOID

    @note  No porting required.
**/
VOID HpetEnable (
    IN BOOLEAN      Enable )
{
    gHpetGeneralConfiguration.Bits.MainCounterEnable = Enable ? 1 : 0;  
    HpetWrite( HPET_GENERAL_CONFIGURATION_OFFSET, 
                                          gHpetGeneralConfiguration.Uint64);
}

/**
    This function stops HPET counter & interrupt.

    @param Event   - Event of callback
    @param Context - Context of callback.

    @retval VOID
**/
VOID StopHpetBeforeLagecyBoot (
    IN EFI_EVENT        Event,
    IN VOID             *Context )
{
    // Disable HPET and Legacy Replacement Support.   
    HpetEnable (FALSE);
    CountTime ((HPET_DEFAULT_TICK_DURATION / 10) * 2, PM_BASE_ADDRESS);
    HpetWrite (HPET_TIMER_CONFIGURATION_OFFSET + HPET_OFFSET * HPET_TIMER_STRIDE, 0);

#if defined(HPET_APIC_INTERRUPT_MODE) && (HPET_APIC_INTERRUPT_MODE != 0)
    IoApicDisableIrq(HPET_APIC_INTERRUPT_PIN);
#else
    gHpetGeneralConfiguration.Bits.LegacyRouteEnable = 0;

    HpetEnable (FALSE);
#endif

    pBS->CloseEvent(Event);
}
#endif

#if defined SB_TIMER_ARCH_PROTOCOL_SUPPORT && SB_TIMER_ARCH_PROTOCOL_SUPPORT == 1
/**
    This function registers a handler that is called every time the
    timer interrupt fires

    @param This - Pointer to the instance of the 
                  Architectural Protocol
    @param NotifyFunction - The function to call when the interrupt fires

    @retval EFI_SUCCESS new handle registered
    @retval EFI_ALREADY_STARTED if Notify function is already defined

**/
EFI_STATUS TimerRegisterHandler(
    IN  EFI_TIMER_ARCH_PROTOCOL *This,
    IN  EFI_TIMER_NOTIFY        NotifyFunction
)
{
    // check to see if the handler has already been installed
  if (NotifyFunction != NULL && mNotifyFunction != NULL) {
    return EFI_ALREADY_STARTED;
  }

    // if not install it
    mNotifyFunction = NotifyFunction;
    return EFI_SUCCESS;
}

/**
    This function sets the timer to create an Interrupt on IRQ0
    every TimerPeriod number of 100ns intervals

    @param This - Pointer to the instance of the Architectural Protocol
    @param TimerPeriod - The number of 100ns intervals to which the timer
                         will be programmed. This value will be rounded up 
                         to the nearest timer interval

    @retval EFI_SUCCESS

**/
EFI_STATUS SetTimerPeriod (
    IN EFI_TIMER_ARCH_PROTOCOL      *This,
    IN UINT64                       TimerPeriod
)
{

#if defined(HPET_PROTOCOL_SUPPORT) && (HPET_PROTOCOL_SUPPORT != 0)
    UINTN                   Remainder;
    UINT64                  TimerCount;

    // Disable HPET timer when adjusting the timer period
    HpetEnable (FALSE);
#else
    UINT32					NumberOfTicks;
    UINT8                   Value8;
#endif

    // If timer period is 0 then disable the Timer interrupt
	if (TimerPeriod == 0) {
#if defined(HPET_APIC_INTERRUPT_MODE) && (HPET_APIC_INTERRUPT_MODE != 0)
        IoApicDisableIrq(HPET_APIC_INTERRUPT_PIN);
#endif        
        mLegacy8259->DisableIrq(mLegacy8259,SYSTEM_TIMER_IRQ);
        //DisableIrq(SYSTEM_TIMER_IRQ);

	} else {
#if defined(HPET_PROTOCOL_SUPPORT) && (HPET_PROTOCOL_SUPPORT != 0)
        // Convert TimerPeriod to femtoseconds and divide by the number if
        // femtoseconds per tick of the HPET counter to determine the number
        // of HPET counter ticks in TimerPeriod 100 ns units.
        TimerCount = Div64( Mul64( TimerPeriod, 100000000 ),
                     gHpetGeneralCapabilities.Bits.CounterClockPeriod,
                     &Remainder );

        // Reset Main Counter
        HpetWrite (HPET_MAIN_COUNTER_OFFSET, 0); 
        // ValueSetEnable must be set if the timer is set to periodic mode.
        gTimerConfiguration.Bits.ValueSetEnable = 1;
        HpetWrite (HPET_TIMER_CONFIGURATION_OFFSET + HPET_OFFSET * HPET_TIMER_STRIDE, gTimerConfiguration.Uint64);
        // Clear ValueSetEnable bit.
        gTimerConfiguration.Bits.ValueSetEnable = 0;
        HpetWrite (HPET_TIMER_COMPARATOR_OFFSET + HPET_OFFSET * HPET_TIMER_STRIDE, TimerCount);
#else
        if (TimerPeriod>MAX_TICK_DURATION) TimerPeriod=MAX_TICK_DURATION;

        // since TimerPeriod in 100ns units and TIMER_TICK in ns
        // we have to multiple TimerPeriod by 100
        // To round up result:

        NumberOfTicks = ((UINT32)TimerPeriod * 100 + TIMER_TICK / 2)
                                                                / TIMER_TICK;
        //write to port 0x43 to setup the timer
        IoWrite8 ( TIMER_CTRL, 0x36 );
        // write to port 0x40 to set the time
        IoWrite8 ( TIMER_0_COUNT, (UINT8)NumberOfTicks);
        IoWrite8 ( TIMER_0_COUNT, *(((UINT8*)&NumberOfTicks)+1) );
        Value8 = 0x36;
        BOOT_SCRIPT_S3_IO_WRITE_MACRO(gBootScript, EfiBootScriptWidthUint8, TIMER_CTRL, 1, &Value8);
        Value8 = (UINT8)NumberOfTicks;
        BOOT_SCRIPT_S3_IO_WRITE_MACRO(gBootScript, EfiBootScriptWidthUint8, TIMER_0_COUNT, 1, &Value8);
        Value8 = *(((UINT8*)&NumberOfTicks)+1);
        BOOT_SCRIPT_S3_IO_WRITE_MACRO(gBootScript, EfiBootScriptWidthUint8, TIMER_0_COUNT, 1, &Value8);
#endif
        // Now enable the interrupt
#if defined(HPET_APIC_INTERRUPT_MODE) && (HPET_APIC_INTERRUPT_MODE != 0)
        IoApicEnableIrq(HPET_APIC_INTERRUPT_PIN, FALSE, (HPET_INTERRUPT_POLARITY == 0) ? TRUE : FALSE);
#endif
        //EnableIrq(SYSTEM_TIMER_IRQ);
        mLegacy8259->EnableIrq(mLegacy8259,SYSTEM_TIMER_IRQ,FALSE);

#if defined(HPET_PROTOCOL_SUPPORT) && (HPET_PROTOCOL_SUPPORT != 0)        
        // Enable HPET Interrupt Generation
        gTimerConfiguration.Bits.InterruptEnable = 1;
        HpetWrite (HPET_TIMER_CONFIGURATION_OFFSET + HPET_OFFSET * HPET_TIMER_STRIDE, gTimerConfiguration.Uint64);

        // Enable the HPET counter once new timer period has been established
        // The HPET counter should run even if the HPET Timer interrupts are
        // disabled.  This is used to account for time passed while the interrupt
        // is disabled.
        HpetEnable (TRUE);
#endif
    }

    mProgrammedTimerValue = TimerPeriod;

    return EFI_SUCCESS;
}

/**
    This function returns the current timer period

    @param This - Pointer to the instance of the Architectural Protocol
    @param TimerPeriod - pointer to a memory location to load the current
                         Timer period into

    @retval EFI_STATUS

**/
EFI_STATUS GetTimerPeriod(
    IN      EFI_TIMER_ARCH_PROTOCOL *This,
    IN OUT  UINT64                  *TimerPeriod
)
{
    *TimerPeriod = mProgrammedTimerValue;

    return EFI_SUCCESS;
}

/**
    This function generates a soft timer interrupt

    @param This - Pointer to the instance of the Architectural Protocol

    @retval EFI_UNSUPPORTED

**/
EFI_STATUS GenerateSoftIntr(
    IN EFI_TIMER_ARCH_PROTOCOL      *This
)
{
    return EFI_UNSUPPORTED;
}

/**
    This function is called when the Timer reaches 0.  It raises
    the TPL and then calls the registered notify function

    @param InterruptType Interrupt type
    @param SystemContext System context

    @retval VOID

**/
VOID TimerInterruptHandler (
    IN EFI_EXCEPTION_TYPE   InterruptType,
    IN EFI_SYSTEM_CONTEXT   SystemContext
)
{
    EFI_TPL                 OldTpl;
    static volatile UINT32  StoreCF8;

    SaveRestoreRegisters(TRUE);

    StoreCF8 = IoRead32(0xcf8);    // Store CF8 (PCI index)

    OldTpl = pBS->RaiseTPL (TPL_HIGH_LEVEL);

#if defined(HPET_APIC_INTERRUPT_MODE) && (HPET_APIC_INTERRUPT_MODE != 0)
    IoApicEoi(GetHpetApicPin());
#if defined(HPET_INTERRUPT_TRIGGER) && (HPET_INTERRUPT_TRIGGER != 0)
    HpetWrite (HPET_GENERAL_INTERRUPT_STATUS_OFFSET, (BIT0 << HPET_OFFSET));
#endif
#else
    // clear the interrupt flag
    //SendEoi(SYSTEM_TIMER_IRQ);
    mLegacy8259->EndOfInterrupt(mLegacy8259,SYSTEM_TIMER_IRQ);
#endif

    // This checks for the existence of a registered notify function and if it exists
    //  it calls the function with the current programmed Timer Period
    if (mNotifyFunction)
    {
        mNotifyFunction (mProgrammedTimerValue);
    }

    pBS->RestoreTPL (OldTpl);

    IoWrite32(0xcf8, StoreCF8);    // Restore 0xCF8 (PCI index)

    SaveRestoreRegisters(FALSE);
}

#if defined(HPET_APIC_INTERRUPT_MODE) && (HPET_APIC_INTERRUPT_MODE != 0)
/**
    This function is called when the 8254 Timer 0 reaches 0.
    It raises the TPL and then calls the registered notify
    function.

    @param InterruptType - Interrupt type
    @param SystemContext - System context

    @retval VOID
**/
VOID Irq0InterruptHandler (
    IN EFI_EXCEPTION_TYPE   InterruptType,
    IN EFI_SYSTEM_CONTEXT   SystemContext )
{
    EFI_TPL                 OldTpl;

    OldTpl = pBS->RaiseTPL (TPL_HIGH_LEVEL);

    // Clear the interrupt flag
    mLegacy8259->EndOfInterrupt(mLegacy8259, SYSTEM_TIMER_IRQ);

    pBS->RestoreTPL (OldTpl);
}
#endif

/**
    Calls TimerInit when the Legacy8259 Protocol
    becomes available. 

    @param Event   - Event of callback
    @param Context - Context of callback.
    
    @retval VOID

**/    
VOID TimerInitWrapper(
    IN EFI_EVENT                Event,
    IN VOID                     *Context
)
{
    EFI_STATUS    Status;

    Status = pBS->LocateProtocol (&gEfiLegacy8259ProtocolGuid, NULL, &mLegacy8259);
    if(EFI_ERROR(Status)) return;

    TimerInit();

    //Kill the Event
    pBS->CloseEvent(Event);
}

/**
    This function installs the the timer protocol on its handle,
    and initializes the timer.

    @param ImageHandle ImageHandle of the loaded driver
    @param SystemTable Pointer to the System Table

    @retval EFI_STATUS

**/
EFI_STATUS TimerInit (
    VOID
)
{
    EFI_STATUS  Status;
    EFI_CPU_ARCH_PROTOCOL           *CpuArch;
    UINT8 Value;
    //TimerVector must be initialized to 0, since GetVector only modifies the lowest byte,
    //and RegisterInterruptHandler requires TimerVector to be 4 bytes.
    UINT32      TimerVector = 0;
    UINT32                  Irq0TimerVector = 0;

#if defined(HPET_PROTOCOL_SUPPORT) && (HPET_PROTOCOL_SUPPORT != 0)    
    // Enable HPET (0x3404)
    EnableHpetInChipset();

    // Retrieve HPET Capabilities and Configuration Information  
    gHpetGeneralCapabilities.Uint64  = HpetRead (HPET_GENERAL_CAPABILITIES_ID_OFFSET);
    gHpetGeneralConfiguration.Uint64 = HpetRead (HPET_GENERAL_CONFIGURATION_OFFSET);
 
    // If Revision is not valid, then ASSERT() and unload the driver because the HPET 
    // device is not present.  
    if (gHpetGeneralCapabilities.Uint64 == 0 || gHpetGeneralCapabilities.Uint64 == 0xFFFFFFFFFFFFFFFF) {
        TRACE((-1, "HPET device is not present.  Unload HPET driver.\n"));
        return EFI_DEVICE_ERROR;
    }

    HpetEnable (FALSE);
#if defined(HPET_APIC_INTERRUPT_MODE) && (HPET_APIC_INTERRUPT_MODE == 0)
    // Enable Legacy Interrupt
    gHpetGeneralConfiguration.Bits.LegacyRouteEnable = 1;
#endif
#endif

    // Disable timer, make sure no interrupt will be created
    Status = SetTimerPeriod (&mTimerProtocol, 0);
    ASSERT_EFI_ERROR(Status);

#if defined(HPET_PROTOCOL_SUPPORT) && (HPET_PROTOCOL_SUPPORT != 0) 
    // Configure the selected HPET Timer (Timer#0), clear InterruptEnable to keep 
    // interrupts disabled until full initialization is complete 
    // Enable PeriodicInterruptEnable to use periodic mode
    // Configure as a 32-bit counter
    gTimerConfiguration.Uint64 = HpetRead (HPET_TIMER_CONFIGURATION_OFFSET + HPET_OFFSET * HPET_TIMER_STRIDE);
    gTimerConfiguration.Bits.InterruptEnable         = 0;
    gTimerConfiguration.Bits.PeriodicInterruptEnable = 1;
    gTimerConfiguration.Bits.CounterSizeEnable       = 1;
    gTimerConfiguration.Bits.LevelTriggeredInterrupt = 0;
#if defined(HPET_APIC_INTERRUPT_MODE) && (HPET_APIC_INTERRUPT_MODE != 0)
    gTimerConfiguration.Bits.InterruptRoute = HPET_APIC_INTERRUPT_PIN;
    gTimerConfiguration.Bits.LevelTriggeredInterrupt = (HPET_INTERRUPT_TRIGGER != 0) ? 1 : 0;
#endif
    HpetWrite (HPET_TIMER_CONFIGURATION_OFFSET + HPET_OFFSET * HPET_TIMER_STRIDE, gTimerConfiguration.Uint64);

    // Read the HPET Timer Capabilities and Configuration register back again.
    // CounterSizeEnable will be read back as a 0 if it is a 32-bit only timer
    gTimerConfiguration.Uint64 = HpetRead (HPET_TIMER_CONFIGURATION_OFFSET + HPET_OFFSET * HPET_TIMER_STRIDE);
#if defined(HPET_APIC_INTERRUPT_MODE) && (HPET_APIC_INTERRUPT_MODE != 0)
    // If the interrupt pin isn't supported by the particular timer, then the value read back won't match that is written.
    if (gTimerConfiguration.Bits.InterruptRoute != HPET_APIC_INTERRUPT_PIN) {
        ASSERT_EFI_ERROR (EFI_UNSUPPORTED); 
        return EFI_UNSUPPORTED;
    }
#endif
    if ((gTimerConfiguration.Bits.CounterSizeEnable == 1) && (sizeof (UINTN) == sizeof (UINT64))) {
        // 64-bit BIOS can use 64-bit HPET timer
        gCounterMask = 0xffffffffffffffff;
        // Set timer back to 64-bit
        gTimerConfiguration.Bits.CounterSizeEnable = 0;
        HpetWrite (HPET_TIMER_CONFIGURATION_OFFSET + HPET_OFFSET * HPET_TIMER_STRIDE, gTimerConfiguration.Uint64);
    } else {
        gCounterMask = 0x00000000ffffffff;
    }
#endif
    // Find the CPU Arch Protocol
    Status = pBS->LocateProtocol (&gEfiCpuArchProtocolGuid, NULL, &CpuArch);
    ASSERT_EFI_ERROR(Status);

#if defined(HPET_APIC_INTERRUPT_MODE) && (HPET_APIC_INTERRUPT_MODE != 0)
    TimerVector = MASTER_INTERRUPT_BASE + HPET_APIC_INTERRUPT_PIN;

    Status = mLegacy8259->GetVector ( mLegacy8259,
                                      Efi8259Irq0,
                                      (UINT8 *)&Irq0TimerVector );
    ASSERT_EFI_ERROR(Status);

    Status = CpuArch->RegisterInterruptHandler ( CpuArch,
                                                 Irq0TimerVector,
                                                 Irq0InterruptHandler );
    ASSERT_EFI_ERROR(Status);
#else
    Status = mLegacy8259->GetVector ( mLegacy8259,
                                      Efi8259Irq0,
                                      (UINT8 *)&TimerVector );
    ASSERT_EFI_ERROR(Status);
#endif

    Status = CpuArch->RegisterInterruptHandler ( CpuArch,
                                                 TimerVector,
                                                 TimerInterruptHandler );
    ASSERT_EFI_ERROR(Status);
    // Initialize the handle pointer
    mNotifyFunction = NULL;

#if defined(HPET_PROTOCOL_SUPPORT) && (HPET_PROTOCOL_SUPPORT != 0)
    // Init default for Timer 1
	IoWrite8( TIMER_CTRL, 0x36 );
	IoWrite8( TIMER_0_COUNT, 0 );
	IoWrite8( TIMER_0_COUNT, 0 );
    // Add boot script programming
    Value = 0x36;
    BOOT_SCRIPT_S3_IO_WRITE_MACRO(gBootScript, EfiBootScriptWidthUint8, TIMER_CTRL, 1, &Value);
    Value = 0x0;
    BOOT_SCRIPT_S3_IO_WRITE_MACRO(gBootScript, EfiBootScriptWidthUint8, TIMER_0_COUNT, 1, &Value);
    BOOT_SCRIPT_S3_IO_WRITE_MACRO(gBootScript, EfiBootScriptWidthUint8, TIMER_0_COUNT, 1, &Value);
    // The default value of 10000 100 ns units is the same as 1 ms.
    Status = SetTimerPeriod ( &mTimerProtocol, HPET_DEFAULT_TICK_DURATION );
    Status = CreateLegacyBootEvent( TPL_CALLBACK,
                                    StopHpetBeforeLagecyBoot,
                                    NULL,
                                    &gHpetLegacyBootEvent ); 
#else    
    // Force the timer to be enabled at its default period
    Status = SetTimerPeriod (&mTimerProtocol, DEFAULT_TICK_DURATION);
#endif
    ASSERT_EFI_ERROR (Status);

    //Program Timer1 to pass certain customer's test
    IoWrite8( TIMER_CTRL, 0x54 );
    IoWrite8( TIMER_1_COUNT, 0x12 );
    //add boot script programming
    Value = 0x54;
    BOOT_SCRIPT_S3_IO_WRITE_MACRO(gBootScript, EfiBootScriptWidthUint8, TIMER_CTRL, 1, &Value);
    Value = 0x12;
    BOOT_SCRIPT_S3_IO_WRITE_MACRO(gBootScript, EfiBootScriptWidthUint8, TIMER_1_COUNT, 1, &Value);


  // Install the Timer Architectural Protocol onto a new handle
    Status = pBS->InstallProtocolInterface (
                    &mTimerProtocolHandle,
                    &gEfiTimerArchProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    &mTimerProtocol
                    );
    ASSERT_EFI_ERROR(Status);

  return Status;
}
#endif

#if defined SB_8259_PROTOCOL_SUPPORT && SB_8259_PROTOCOL_SUPPORT == 1
/**
    Program the IRQ Mask and Trigger.

    @param VOID

    @retval VOID

    @note  Here is the control flow of this function:
     1. Program Master IRQ Mask.
     2. Program Slave IRQ Mask.
     3. Program Trigger Level.

**/
VOID ProgramIrqMaskTrigger (
    VOID
)
{
    IoWrite8(0x21, (UINT8)gIrqMask[gMode]);
    IoWrite8(0xa1, (UINT8)(gIrqMask[gMode] >> 8));
    //
    // 4d0 can not be accessed as by IoWrite16, we have to split
    //
    IoWrite8(0x4d0, (UINT8)gIrqTrigger[gMode]);
    IoWrite8(0x4d1, (UINT8)(gIrqTrigger[gMode] >> 8));
}

/**
    Initializes the interrupt controller and program the IRQ
    Master and Slave Vector Base.

    @param This - Pointer to this object
    @param MasterBase - IRQ base for the master 8259 controller
    @param SlaveBase - IRQ base for the slave 8259 controller

    @retval EFI_SUCCESS Interrupt on the interrupt controllers was enabled.

    @note  Here is the control flow of this function:
           1. If Master base is changed, initialize master 8259 setting
              the interrupt offset.
           2. If Slave base is changed, initialize slave 8259 setting
              the interrupt offset.
           3. Return EFI_SUCCESS.
**/
EFI_STATUS SetVectorBase (
    IN EFI_LEGACY_8259_PROTOCOL   *This,
    IN UINT8                      MasterBase,
    IN UINT8                      SlaveBase
)
{
    //
    // 8259 Master
    //
    if (MasterBase != gMasterBase)
    {
        //
        // Start 8259 Master Initialization.
        //
        IoWrite8(LEGACY_8259_CONTROL_REGISTER_MASTER, ICW1);
        //
        // Set Interrupt Offset
        //
        IoWrite8(LEGACY_8259_MASK_REGISTER_MASTER, MasterBase);
        //
        // Set Slave IRQ.
        //
        IoWrite8(LEGACY_8259_MASK_REGISTER_MASTER, ICW3_M);
        //
        // Set 8259 mode. See ICW4 comments with #define.
        //
        IoWrite8(LEGACY_8259_MASK_REGISTER_MASTER, ICW4);
        gMasterBase = MasterBase;
    }
    //
    // 8259 Slave
    //
    if (SlaveBase != gSlaveBase)
    {
        //
        // Start 8259 Slave  Initialization.
        //
        IoWrite8(LEGACY_8259_CONTROL_REGISTER_SLAVE, ICW1);
        //
        // Set Interrupt Offset
        //
        IoWrite8(LEGACY_8259_MASK_REGISTER_SLAVE, SlaveBase);
        //
        // Set Slave IRQ.
        //
        IoWrite8(LEGACY_8259_MASK_REGISTER_SLAVE, ICW3_S);
        //
        // Set 8259 mode. See ICW4 comments with #define.
        //
        IoWrite8(LEGACY_8259_MASK_REGISTER_SLAVE, ICW4);
        gSlaveBase = SlaveBase;
    }

    return EFI_SUCCESS;
}

/**
    Get the Master/Slave IRQ Mask, IRQ Level for Legacy real
    mode and protected mode.

    @param This - Pointer to this object
    @param LegacyMask - Legacy mode interrupt mask
    @param LegacyEdgeLevel - Legacy mode edge/level trigger value
    @param ProtectedMask - Protected mode interrupt mask
    @param ProtectedEdgeLevel - Protected mode edge/level trigger value

    @retval EFI_SUCCESS Returned iRQ mask/level.

    @note  Here is the control flow of this function:
           1. If *LegacyMask not NULL, get legacy Mask.
           2. If *LegacyEdgeLevel not NULL, get legacy Trigger Level.
           3. If *ProtectedMask not NULL, get protected Mask.
           4. If *ProtectedEdgeLevel not NULL, get protected trigger level.
           5. Return EFI_SUCCESS.

**/
EFI_STATUS GetMask (
    IN EFI_LEGACY_8259_PROTOCOL   *This,
    OUT UINT16                    *LegacyMask   OPTIONAL,
    OUT UINT16                    *LegacyEdgeLevel OPTIONAL,
    OUT UINT16                    *ProtectedMask  OPTIONAL,
    OUT UINT16                    *ProtectedEdgeLevel OPTIONAL
)
{
    if (LegacyMask)   *LegacyMask   = gIrqMask[0];
    if (LegacyEdgeLevel) *LegacyEdgeLevel = gIrqTrigger[0];
    if (ProtectedMask)  *ProtectedMask  = gIrqMask[1];
    if (ProtectedEdgeLevel) *ProtectedEdgeLevel = gIrqTrigger[1];

    return EFI_SUCCESS;
}

/**
    Set the Master/Slave IRQ Mask, IRQ Level for Legacy real mode
    and protected mode.

    @param This - Pointer to this object
    @param LegacyMask - Legacy mode interrupt mask
    @param LegacyEdgeLevel - Legacy mode edge/level trigger value
    @param ProtectedMask - Protected mode interrupt mask
    @param ProtectedEdgeLevel - Protected mode edge/level trigger value

    @retval EFI_SUCCESS Set IRQ mask/level.

    @note  Here is the control flow of this function:
           1. If *LegacyMask not NULL, set legacy mask variable.
           2. If *LegacyEdgeLevel not NULL, set legacy Trigger Level variable.
           3. If *ProtectedMask not NULL, set protected mask variable.
           4. If *ProtectedEdgeLevel not NULL, set protected trigger level variable.
           5. Call function to program 8259 with mask/trigger of current mode.
           6. Return EFI_SUCCESS.

**/
EFI_STATUS SetMask (
    IN EFI_LEGACY_8259_PROTOCOL   *This,
    IN UINT16                     *LegacyMask   OPTIONAL,
    IN UINT16                     *LegacyEdgeLevel OPTIONAL,
    IN UINT16                     *ProtectedMask  OPTIONAL,
    IN UINT16                     *ProtectedEdgeLevel OPTIONAL
)
{
    if (LegacyMask)   gIrqMask[0]  = *LegacyMask;
    if (LegacyEdgeLevel) gIrqTrigger[0] = *LegacyEdgeLevel;
    if (ProtectedMask)  gIrqMask[1]  = *ProtectedMask;
    if (ProtectedEdgeLevel) gIrqTrigger[1] = *ProtectedEdgeLevel;

    ProgramIrqMaskTrigger();

    return EFI_SUCCESS;
}

/**
    Sets the interrupt mode operation to legacy or protected.
    New mask and edge/level status can be provided as input

    @param This - Pointer to this object
    @param Mode - Interrupt mode setting
    @param Mask - New interrupt mask for this mode
    @param EdgeLevel - New edge/level trigger value for this mode

    @retval EFI_SUCCESS Set mode was successful

    @note  Here is the control flow of this function:
           1. If invalid mode, return EFI_INVALID_PARAMETER.
           2. If *Mask not NULL, set mode mask variable.
           3. If *EdgeLevel not NULL, set mode trigger level variable.
           4. Call function to program 8259 with mask/trigger of current mode.
           5. Return EFI_SUCCESS.

**/
EFI_STATUS SetMode (
    IN EFI_LEGACY_8259_PROTOCOL   *This,
    IN EFI_8259_MODE              Mode,
    IN UINT16                     *Mask   OPTIONAL,
    IN UINT16                     *EdgeLevel OPTIONAL
)
{
    if (Mode >= Efi8259MaxMode) return EFI_INVALID_PARAMETER;

#if defined (HPET_INTERRUPT_TRIGGER) && (HPET_INTERRUPT_TRIGGER != 0)
    if (Mode == Efi8259LegacyMode)
        gTimerConfiguration.Bits.InterruptEnable = 0;
    else    // Efi8259ProtectedMode
        gTimerConfiguration.Bits.InterruptEnable = 1;
        HpetWrite (
          HPET_TIMER_CONFIGURATION_OFFSET + HPET_OFFSET * HPET_TIMER_STRIDE,
          gTimerConfiguration.Uint64
          );
#endif

    gMode = Mode;
    if (Mask) gIrqMask[Mode] = *Mask;
    if (EdgeLevel) gIrqTrigger[Mode] = *EdgeLevel;

    ProgramIrqMaskTrigger();

    return EFI_SUCCESS;
}

/**
    Returns the vector number for the requested IRQ

    @param This - Legacy8259 protocol object
    @param Irq - IRQ number for which vector is needed
    @param Vector - Vector value is returned in this pointer

    @retval EFI_SUCCESS Get IRQ Vector for IRQ.

    @note  Here is the control flow of this function:
           1. If invalid IRQ, return EFI_INVALID_PARAMETER.
           2. If Set *Vector to IRQ base + interrupt offset.
           3. Return EFI_SUCCESS.

**/
EFI_STATUS GetVector (
    IN EFI_LEGACY_8259_PROTOCOL   *This,
    IN EFI_8259_IRQ               Irq,
    OUT UINT8                     *Vector
)
{
    if ((UINT8)Irq >= (UINT8)Efi8259IrqMax) return EFI_INVALID_PARAMETER;
    *Vector = (Irq <= Efi8259Irq7) ? gMasterBase + Irq : gSlaveBase + Irq - 8;

    return EFI_SUCCESS;
}

/**
    Enable the Interrupt controllers to respond in a specific IRQ.

    @param This - Legacy8259 protocol object
    @param Irq - IRQ that has to be enabled
    @param LevelTriggered - Trigger mechanism (level or edge) for this IRQ

    @retval EFI_SUCCESS Interrupt on the interrupt controllers was enabled.
    @retval EFI_INVALID_PARAMETER Interrupt not existent. The 8259 
                                  master/slave supports IRQ 0-15.

    @note  Here is the control flow of this function:
           1. Check if IRQ is valid. If not, return EFI_INVALID_PARAMETER.
           2. Clear interrupt mask bit in variable of current mode.
           3. Set/Clear bit of trigger level variable of current mode.
           4. Program mask/trigger.
           5. Return EFI_SUCCESS.

**/
EFI_STATUS EnableIrq (
    IN EFI_LEGACY_8259_PROTOCOL   *This,
    IN EFI_8259_IRQ               Irq,
    IN BOOLEAN                    LevelTriggered
)
{
    if ((UINT8)Irq > (UINT8)15) return EFI_INVALID_PARAMETER;
    //
    // Clear mask for the IRQ.
    //
    gIrqMask[gMode] &= ~(1 << Irq);
    //
    // Mask IRQ to change.
    //
    gIrqTrigger[gMode] &= ~(1 << Irq);
    //
    // Change IRQ bit, 0 = edge, 1 = level.
    //
    if (LevelTriggered) gIrqTrigger[gMode] |= (1 << Irq);

    ProgramIrqMaskTrigger();
    return EFI_SUCCESS;
}


/**
    Disable the Interrupt controllers to stop responding to
    a specific IRQ.

    @param This - Legacy8259 protocol object
    @param Irq - IRQ that has to be disabled

    @retval EFI_SUCCESS Interrupt on the interrupt controllers was enabled.
    @retval EFI_INVALID_PARAMETER Interrupt not existent. The 8259 
                                  master/slave supports IRQ 0-15.

    @note  Here is the control flow of this function:
           1. Check if IRQ is valid. If not, return EFI_INVALID_PARAMETER.
           2. Set interrupt mask bit in variable of current mode.
           3. Program mask/trigger.
           4. Return EFI_SUCCESS.

**/
EFI_STATUS DisableIrq (
    IN EFI_LEGACY_8259_PROTOCOL   *This,
    IN EFI_8259_IRQ               Irq
)
{
    if ((UINT8)Irq > (UINT8)15) return EFI_INVALID_PARAMETER;
    //
    // Set mask for the Irq.
    //
    gIrqMask[gMode] |= 1 << Irq;

    ProgramIrqMaskTrigger();
    return EFI_SUCCESS;
}

/**
    Get IRQ vector assigned to PCI card.

    @param This - Pointer to this object
    @param PciHandle - PCI handle for this device
    @param Vector - Interrupt vector this device

    @retval EFI_SUCCESS Vector returned.
    @retval EFI_INVALID_PARAMETER Invalid PciHandle.

    @note  Here is the control flow of this function:
           1. Get Handle of PciIo installed on PciHandle.
           2. If PciIo not installed, return EFI_INVALID_DEVICE.
           3. Set *vector to IRQ vector programmed into card.
           4. Return EFI_SUCCESS.

**/
EFI_STATUS GetInterruptLine (
    IN EFI_LEGACY_8259_PROTOCOL   *This,
    IN EFI_HANDLE                 PciHandle,
    OUT UINT8                     *Vector
)
{
    EFI_STATUS    Status;
    EFI_PCI_IO_PROTOCOL  *PciIo;
    UINT8     InterruptLine;

    Status = pBS->HandleProtocol(
                    PciHandle,
                    &gEfiPciIoProtocolGuid,
                    &PciIo
                    );
    if (EFI_ERROR(Status)) return EFI_INVALID_PARAMETER;
    //
    // Read vector from card.
    //
    PciIo->Pci.Read(
            PciIo,
            EfiPciIoWidthUint8,
            PCI_INTLINE,
            1,
            &InterruptLine
            );

    *Vector = InterruptLine;

    return EFI_SUCCESS;
}

/**
    Send end of interrupt command to interrupt controller(s).

    @param This - Pointer to this object
    @param Irq - IRQ number for this EOI has to be sent

    @retval EFI_SUCCESS EOI command sent to controller(s)
    @retval EFI_INVALID_PARAMETER Interrupt not existent. The 8259 
                                  master/slave supports IRQ 0-15.

    @note  Here is the control flow of this function:
           1. Check if IRQ is valid. If not, return EFI_INVALID_PARAMETER.
           2. If IRQ >= 8, then Send EOI to slave controller.
           3. Send EOI to master controller. (This is for both master/slave IRQs)
           4. Return EFI_SUCCESS.

**/
EFI_STATUS EndOfInterrupt (
    IN EFI_LEGACY_8259_PROTOCOL   *This,
    IN EFI_8259_IRQ               Irq
)
{
    if (Irq > 15) return EFI_INVALID_PARAMETER;
    //
    // If Slave, send EOI to slave first.
    //
    if (Irq >= 8)
    {
        //
        // Send Slave EOI
        //
        IoWrite8(LEGACY_8259_CONTROL_REGISTER_SLAVE, EOI_COMMAND);
    }
    //
    // Send Master EOI
    //
    IoWrite8(LEGACY_8259_CONTROL_REGISTER_MASTER, EOI_COMMAND);

    return EFI_SUCCESS;
}

EFI_LEGACY_8259_PROTOCOL gLegacy8259Protocol =
{
    SetVectorBase,
    GetMask,
    SetMask,
    SetMode,
    GetVector,
    EnableIrq,
    DisableIrq,
    GetInterruptLine,
    EndOfInterrupt
};

/**
    Program the IRQ Mask and Trigger.

    @param ImageHandle  Image handle for this driver
    @param SystemTable  Pointer to the system table

    @retval EFI_STATUS

    @note  Here is the control flow of this function:
           1. Initialize AMI Library.
           2. Install Legacy 8259 Interface. If error, return error.
           3. Initialize the CPU setting vector bases.
           4. Set CPU Mode, mask, and trigger level.
           5. Return EFI_SUCCESS.

**/
EFI_STATUS Initialize8259 (
    IN EFI_HANDLE                 ImageHandle,
    IN EFI_SYSTEM_TABLE           *SystemTable
)
{
    EFI_STATUS    Status;
    UINT16        Mask  = 0xffff;     //Mask all interrupts.
    UINT16        EdgeLevel = 0x00;   //Set all edge.

    SetVectorBase (0, MASTER_INTERRUPT_BASE, SLAVE_INTERRUPT_BASE);
    SetMode (0, Efi8259ProtectedMode, &Mask, &EdgeLevel);

    Status = pBS->InstallMultipleProtocolInterfaces(
                    &ImageHandle,
                    &gEfiLegacy8259ProtocolGuid,
                    &gLegacy8259Protocol,
                    NULL
                    );

    return Status;
}
#endif

/**
    This Function Installs the SB HDD Policy Protocol      

    @param VOID
           
    @retval EFI_SUCCESS      

**/
EFI_STATUS
InstallSbPolicyProtocol()
{
    EFI_HANDLE  Handle=NULL;
    EFI_STATUS  Status;

    Status = pBS->InstallMultipleProtocolInterfaces (
                  &Handle,
                  &gAmiSbHddPolicyProtocolGuid,
                  &mSbHddPolicy,
                  NULL
                  );

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
