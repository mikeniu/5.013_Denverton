//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************

/** @file SbGeneric.c
    This file contains south bridge related code that is needed
    for both PEI & DXE stage. To avoid code duplication this
    file is made as a library and linked both in PEI & DXE
    south bridge FFS.

    @note  MAKE SURE NO PEI OR DXE SPECIFIC CODE IS NEEDED

*/


// Module specific Includes
#include "Efi.h"
#include "token.h"
#include <AmiPeiLib.h>
#include <AmiDxeLib.h>
#include <AmiCspLib.h>
#include <Library/PcdLib.h>
//#include <Hpet.h>
// Denverton Aptiov Override Start - EIP#210294
#include <PchAccess.h>
// Denverton Aptiov Override End - EIP#210294
#include <Guid\Rtc.h> // Denverton Aptiov override - EIP#362871
extern VOID CPULib_DisableInterrupt();
extern VOID CPULib_EnableInterrupt();
extern BOOLEAN CPULib_GetInterruptState();

// Denverton AptioV Override Start- EIP#280221  
VOID SwitchAlternateAccessMode (
    IN BOOLEAN      Switch
);
// Denverton AptioV Override End- EIP#280221  

//----------------------------------------------------------------------------
// Start OF CSM Related Porting Hooks
//----------------------------------------------------------------------------
#if     CSM_SUPPORT
//----------------------------------------------------------------------------

#include <Protocol/PciRootBridgeIo.h>
#include <Protocol/LegacyInterrupt.h>

typedef struct {
    UINT64                  Address;
    EFI_BOOT_SCRIPT_WIDTH   Width;
} BOOT_SCRIPT_PCI_REGISTER_SAVE;

/* PORTING NOTE **
    The following table contains the information regarding the PIRQ routing
    registers that need to be restored during the S3 wakeup. Mention all
    register address (bus, device, function, register) and specify the
    size of the register also.
 */
BOOT_SCRIPT_PCI_REGISTER_SAVE gPirqRegistersSave[] = {
    0x001f0060,  EfiBootScriptWidthUint32,      // PORTING REQUIRED
    0x001f0068,  EfiBootScriptWidthUint32       // PORTING REQUIRED
};

#define MAX_PIRQS        8

UINT8   bMaxPIRQ        = MAX_PIRQS;                // For CSM
UINT8   bRouterBus      = 0;// PORTING REQUIRED (Use appropriate Equate)
UINT8   bRouterDevice   = 7;// PORTING REQUIRED (Use appropriate Equate)
UINT8   bRouterFunction = 3;// PORTING REQUIRED (Use appropriate Equate)
UINT8   RRegs[MAX_PIRQS]= {0x60, 0x61, 0x62, 0x63, 0x68, 0x69, 0x6A, 0x6B}; // Porting required

// Local variable
static UINT8                        PirqShift[MAX_PIRQS]        = {0x00,0x04,0x08,0x0C, 0, 0, 0, 0};
                                        // PORTING REQUIRED for PirqShift
static UINT8                        PirqRegBuffer[MAX_PIRQS]    = {0,0,0,0,0,0,0,0};
static EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL      *gPciRootBridgeIo;


/**
    This function is clears the routing registers to default values

    @param PciRBIo Root bridge IO protocol pointer

    @retval EFI_STATUS Returned from PCI read call

**/
EFI_STATUS SbGenInitializeRouterRegisters (
    IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL *PciRBIo
)
{
    EFI_STATUS  Status = EFI_SUCCESS;

/** PORTING REQUIRED. Program chipset to disable all routing
    UINT8       Idx;
    UINT8       Data;


    for (Idx = 0; Idx < MAX_PIRQS; Idx++)
    {
        Data = 0;
        Status = PciRBIo->Pci.Write(
                    PciRBIo,
                    EfiPciWidthUint8,
                    LPC_BUS_DEV_FUNC | RRegs[Idx],
                    1,
                    &Data);
    }

 **/

    gPciRootBridgeIo = PciRBIo;         // Save RB IO value for later use
    return Status;
}

/**
    This function returns the 0 based PIRQ index (PIRQ0, 1 etc)
    based on the PIRQ register number specified in the routing table

    @param PIRQRegister - Register number of the PIR

    @retval PIRQIndex 0 - (MAX_PIRQ -1)
            0xFF if the register is invalid

    @note  Here is the control flow of this function:
              1. If Invalid PirqNumber, return EFI_INVALID_PARAMETER.
              2. Read into *PriqData from PIRQ register for Pirq requested.
              3. Return EFI_SUCCESS.
**/
UINT8 SBGen_GetPIRQIndex (
    UINT8 PIRQRegister
)
{
// Denverton AptioV Override Start - EIP#239027
	//
	// In Denverton, PIRQ registers no longer reside at 60h, 61h etc in LPC device.
	// So LNKA to LNKH reg values in SB.sdl are changed to 01h -08h.
	// Csm driver expects zero based Pirq register to pass as argument to Legacy Interrupt protocol.
	// Convert one based index value to zero based index and return #PIRQ.
	//   
	return --PIRQRegister;
// Denverton AptioV Override End - EIP#239027
}

/**
    This function reads the IRQ associated with the PIRQ from
    the chipset register

    @param This Pointer to Legacy interrupt protocol
    @param PirqNumber PIRQ number to read
    @param PirqData IRQ programmed for this PIRQ (BIT7 will be
           set if the PIRQ is not programmed)

    @retval EFI_STATUS
            EFI_SUCCESS On successfully IRQ value return
            EFI_INVALID_PARAMETER   If PirqNumber is greater than max PIRQs

    @note  Here is the control flow of this function:
              1. If Invalid PirqNumber, return EFI_INVALID_PARAMETER.
              2. Read into *PriqData from PIRQ register for PIRQ requested.
              3. Return EFI_SUCCESS.
**/
EFI_STATUS SbGenReadPirq (
    IN EFI_LEGACY_INTERRUPT_PROTOCOL    *This,
    IN UINT8                            PirqNumber,
    OUT UINT8                           *PirqData
)
{

    if (PirqNumber > MAX_PIRQS) return EFI_INVALID_PARAMETER;
    *PirqData = PirqRegBuffer[PirqNumber];
    *PirqData |= (PirqRegBuffer[PirqNumber]) ? 0x00 : 0x80;

    //If Pirq is not routed, bit 7 is set, however specification does not specify a return error for
    //this condition.

    return EFI_SUCCESS;
}

/**
    This function writes an IRQ value allocated for the PIRQ by
    programming the chipset register

    @param This - Pointer to Legacy interrupt protocol
    @param PirqNumber - PIRQ number to read
    @param PirqData - IRQ to be programmed

    @retval EFI_STATUS
            EFI_SUCCESS On successfully IRQ value return
            EFI_INVALID_PARAMETER If PirqNumber is greater than
                                  max PIRQs or PirqData is greater
                                  than 15 (MAX IRQ)

    @note  Here is the control flow of this function:
              1. If Invalid PirqNumber, return EFI_INVALID_PARAMETER.
              2. Write PirqData to PIRQ register for PIRQ requested.
              3. Return EFI_SUCCESS.
**/
EFI_STATUS SbGenWritePirq (
    IN EFI_LEGACY_INTERRUPT_PROTOCOL    *This,
    IN UINT8                            PirqNumber,
    IN UINT8                            PirqData
)
{
//  UINT64      Address;
//  EFI_STATUS  Status;
//  UINT16      PirqValue = PirqData & 0x0F;

    if (PirqNumber > MAX_PIRQS) return EFI_INVALID_PARAMETER;
    if (PirqData > 15) return EFI_INVALID_PARAMETER;


/** PORTING REQUIRED. Program chipset for proper routing
    Address = LPC_BUS_DEV_FUNC | RRegs[PirqNumber];

    Status = gPciRootBridgeIo->Pci.Write(
        gPciRootBridgeIo,
        EfiPciWidthUint8,
        Address,
        1,
        &PirqValue  // clear upper nibble
        );
    if (EFI_ERROR(Status)) return Status;
 **/

    PirqRegBuffer[PirqNumber] = PirqData;   // Update router register buffer

    return EFI_SUCCESS;
}

/**
    This writes the PIRQ to boot script before booting.

    @param BootScriptSave - Pointer to boot script protocol

    @retval EFI_STATUS

**/
EFI_STATUS SbGenWriteBootScript (
    IN EFI_S3_SAVE_STATE_PROTOCOL     *BootScriptSave
)
{
    UINTN       i;
    UINT32      Value;

    //Porting required: Write Boot Script

    for (i = 0; i < sizeof(gPirqRegistersSave)/ sizeof(BOOT_SCRIPT_PCI_REGISTER_SAVE); ++i) {
        gPciRootBridgeIo->Pci.Read(
            gPciRootBridgeIo,
            gPirqRegistersSave[i].Width,
            gPirqRegistersSave[i].Address,
            1,
            &Value
        );
        BOOT_SCRIPT_S3_PCI_CONFIG_WRITE_MACRO(
            BootScriptSave,
            gPirqRegistersSave[i].Width,
            gPirqRegistersSave[i].Address,
            1,
            &Value
        );
    }

/***/

    return EFI_SUCCESS;
}

//----------------------------------------------------------------------------
#endif          // END OF CSM Related Porting Hooks
//----------------------------------------------------------------------------

/**
    This function Shuts the system down (S5)

    @param VOID

    @retval VOID

**/
VOID SbLibShutdown (
    VOID
)
{
/**  CHIPSET SPECIFIC PORTING REQUIRED 
    UINT16          Value;

    //Check if Sleep SMI is enabled we will disable it.
    Value = IoRead16(PM_BASE_ADDRESS+0x30);
    Value&=(~BIT04);
    IoWrite16(PM_BASE_ADDRESS+0x30,Value);

    //Clear All PM  Statuses
    Value = IoRead16(PM_BASE_ADDRESS);
    IoWrite16(PM_BASE_ADDRESS,Value);

    //Go to S5
    Value = IoRead16(PM_BASE_ADDRESS + 4);
    IoWrite16(PM_BASE_ADDRESS + 4,(UINT16)(Value | (0xf << 10)));
 **/
}

/**
    This function is the extended reset call interface function
    provided by SB.

    @param ResetType - The extended type of reset to be generated

    @retval SYSTEM RESET
**/

VOID SbLibExtResetSystem (
    IN SB_EXT_RESET_TYPE  ResetType
)
{
/**  CHIPSET SPECIFIC PORTING REQUIRED 
    switch (ResetType) {
      case SbResetFull:
      case SbResetGlobal:
      case SbResetGlobalWithEc:
  
        break;
  
      default:
        return;
    }
  
    // We should never get this far
    EFI_DEADLOOP();
**/
}


/**
    This function is the reset call interface function published
    by the reset PPI

    @param ResetType Type of reset to be generated

    @retval VOID

**/
VOID SBLib_ResetSystem (
    IN  EFI_RESET_TYPE  ResetType
)
{
    // Denverton AptioV Override Start - EIP#326406
    UINT16  PchPmioBase;
    UINT16  Data16;
    UINT32  Data32;


	// Make use of ResetSystemLib functions to perform the reset opeartions
    switch (ResetType) {
    case EfiResetWarm:
	    AsmWbinvd ();
        IoWrite8 (R_PCH_RST_CNT, V_PCH_RST_CNT_SOFTSTARTSTATE);
        IoWrite8 (R_PCH_RST_CNT, V_PCH_RST_CNT_SOFTRESET);
        break;
    case EfiResetCold:
        IoWrite8 (R_PCH_RST_CNT, V_PCH_RST_CNT_HARDSTARTSTATE);
        IoWrite8 (R_PCH_RST_CNT, V_PCH_RST_CNT_HARDRESET);
        break;
    case EfiResetShutdown:
        //
        // Firstly, ACPI decode must be enabled
        //
        PciOr8 (
          PCI_LIB_ADDRESS (DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_PMC, PCI_FUNCTION_NUMBER_PCH_PMC, R_PCH_PMC_ACPI_CNT),
          (UINT8) (B_PCH_PMC_ACPI_CNT_ACPI_EN)
          );

        PchPmioBase = (UINT16) (PciRead16 (PCI_LIB_ADDRESS (DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_PMC, PCI_FUNCTION_NUMBER_PCH_PMC, R_PCH_PMC_ACPI_BASE)) & ~BIT0);

        //
        // Then, GPE0_EN should be disabled to avoid any GPI waking up the system from S5
        //
        Data16 = 0;
        IoWrite16 (
          (UINTN)(PchPmioBase + R_PCH_ACPI_GPE0_EN_127_96),
          (UINT16)Data16
          );

        //
        // Secondly, PwrSts register must be cleared
        //
        // Write a "1" to bit[8] of power button status register at
        // (PM_BASE + PM1_STS_OFFSET) to clear this bit
        //
        Data16 = B_PCH_ACPI_PM1_STS_PWRBTN;
        IoWrite16 (
          (UINTN)(PchPmioBase + R_PCH_ACPI_PM1_STS),
          (UINT16)Data16
          );

        //
        // Disable SMI on S5 sleep state. This allows ayatem to go to S5.
        //
        
        // Denverton Aptiov Override Start - EIP#364364
        // To avoid disabling of SMI on S5 sleep state and thus avoiding system entering into S5 state
        // This mechanism allows all the handlers registered for SMI to be serviced and then system is 
        // put into S5 sleep state.
        
        // IoWrite32 (PchPmioBase + R_PCH_SMI_EN,(UINT32)(IoRead32 (PchPmioBase + R_PCH_SMI_EN) & (~B_PCH_SMI_EN_ON_SLP_EN)));
        // Denverton Aptiov Override End - EIP#364364
        //
        // Finally, transform system into S5 sleep state
        //
        Data32 = IoRead32 ((UINTN) (PchPmioBase + R_PCH_ACPI_PM1_CNT));

        Data32 = (UINT32) ((Data32 & ~(B_PCH_ACPI_PM1_CNT_SLP_TYP + B_PCH_ACPI_PM1_CNT_SLP_EN)) | V_PCH_ACPI_PM1_CNT_S5);

        IoWrite32 (
          (UINTN)(PchPmioBase + R_PCH_ACPI_PM1_CNT),
          (UINT32)Data32
          );

        Data32 = Data32 | B_PCH_ACPI_PM1_CNT_SLP_EN;

        IoWrite32 (
          (UINTN)(PchPmioBase + R_PCH_ACPI_PM1_CNT),
          (UINT32)Data32
          ); 
      break;
    
    default:
      break;
    }

    DEBUG((DEBUG_INFO, "Resetting system...\n"));
    EFI_DEADLOOP();
    // This should not get here
	// Denverton AptioV Override End - EIP#326406
}


/**
    This function determines if the system should boot with the
    default configuration. 

    @param PeiServices Pointer to the PEI services table
    @param ReadVariablePpi Pointer to the Read Variable#2 PPI
                           (The pointer can be used to read and 
                           enumerate existing NVRAM variables)

    @retval TRUE Firmware will boot with default configuration.

    @note  1. If boot with default configuration is detected, default
                 values for NVRAM variables are used.
              2. Normally we have to check RTC power status or CMOS clear
                 jumper status to determine whether the system should boot
                 with the default configuration.
**/
BOOLEAN SbIsDefaultConfigMode (
    IN EFI_PEI_SERVICES                 **PeiServices,
    IN EFI_PEI_READ_ONLY_VARIABLE2_PPI  *ReadVariablePpi )
{
    UINT8           Buffer8;
    UINT8           Value8;
    BOOLEAN         RtcPwrStatus;              // Denverton Aptiov override - EIP#362871
    
    // GEN_PMCON_B = [B:0, D:31, F:2] + A4h, if Bit is 'Set' indicates a weak or missing battery.
    Buffer8 = READ_PCI8(0, PCI_DEVICE_NUMBER_PCH_PMC, PCI_FUNCTION_NUMBER_PCH_PMC, R_PCH_PMC_GEN_PMCON_B);
    
    // Diagnostic Status Register : 0x0E, Bit 6 & 7 corresponds to CMOS Bad Checksum and RTC Power loss respectively
    IoWrite8 (R_PCH_RTC_INDEX, 0x0E);  
    Value8 = IoRead8 (R_PCH_RTC_TARGET);

    // Denverton Aptiov override start - EIP#362871

    RtcPwrStatus = ((Buffer8 & B_PCH_PMC_GEN_PMCON_B_RTC_PWR_STS) || (Value8  & (BIT6 + BIT7))) ? TRUE : FALSE;   // if RTC_PWR_STS is set or CMOS Bad,boot with default settings

#if defined(CLEAR_TIME_BAD_CMOS) && (CLEAR_TIME_BAD_CMOS != 0)
    if (RtcPwrStatus == TRUE){ //If RTC_PWR_STS is set or CMOS Bad
        // Make CMOS Time incorrect, so that it will be reset to Default setting
        // Invalidating Date
        IoWrite8 (R_PCH_RTC_INDEX, RTC_MONTH_REG);
        IoWrite8 (R_PCH_RTC_TARGET, 0);
        
        // Invalidating Time
        IoWrite8 (R_PCH_RTC_INDEX, RTC_HOURS_REG);
        IoWrite8 (R_PCH_RTC_TARGET, 0xFF);        
    }
#endif
    
    return RtcPwrStatus;

    // Denverton Aptiov override end - EIP#362871
}

// Denverton Aptiov override start - EIP#210294
//#if SB_STALL_PPI_SUPPORT

/**
    This function delays for the number of micro seconds passed in

    @param DelayTime Number of microseconds(us) to delay
    @param BaseAddr The I/O base address of the ACPI registers

    @retval EFI_STATUS based on errors that occurred while waiting
                       for time to expire.

**/
EFI_STATUS CountTime (
    IN  UINTN   DelayTime,
    IN  UINT16  BaseAddr // only needs to be 16 bit for I/O address
)
{
// The following code is to generate delay for specified amount of micro seconds using ACPI timer.
    UINTN           TicksNeeded;
    UINT32          TimerValue, NewTimerValue;
    UINTN           OverFlow;
    UINTN           TheRest, EndValue;

    // Set up timer to point to the ACPI Timer register
/** PORTING NEEDED 
    BaseAddr += <Fill Appropriate PM Base Offset for the Timer>;            
 **/
    // Denverton Aptiov override start - EIP#210294
    BaseAddr += R_PCH_ACPI_PM1_TMR;      
	// Denverton Aptiov override end - EIP#210294      
    // There are 3.58 ticks per us, so we have to convert the number of us passed
    //  in to the number of ticks that need to pass before the timer has expired
    // Convert us to Ticks, don't loose significant figures or as few as possible
    //  do integer math in ticks/tens of ns and then divide by 100 to get ticks
    //  per us
    OverFlow = 0;
/* Workaround for timer value overflow
If the delay is 12 seconds, the result of multiplication (12000000 * 358) is 0x1000FC200.
Since this is 32 bit arithmetic, the most significant values are lost. We can use 64 arithmetic
throughout this function. But the PEI code size will increase further and we may not be able to
successfully build in debug mode.
To overcome these issues, the multiplication is broken into pieces so that we can have upto
19 minutes delay without any problem. I think that is large enough for anyone in POST.

**** If the delay is longer than 1199 seconds, the result of multiplication will overflow *****
*/
//  TheRest = TicksNeeded = (DelayTime * 358) /100;
    TicksNeeded = DelayTime * 3;            //(DelayTime * 3)
    TicksNeeded += (DelayTime) / 2;         //(DelayTime * 5)/10
    TicksNeeded += (DelayTime * 2) / 25;        //(DelayTime * 8)/100
    TheRest = TicksNeeded;

    // 32 bits corresponds to approximate 71 minutes no delay should be that long
    // otherwise
    // Get the number of times the counter will have to overflow to delay as long
    //  as needed
    if (NUM_BITS_IN_ACPI_TIMER < MAX_ACPI_TIMER_BITS)
    {
        OverFlow = TicksNeeded / (1 << NUM_BITS_IN_ACPI_TIMER);
        TheRest = TicksNeeded % (1 << NUM_BITS_IN_ACPI_TIMER);
    }


    // read ACPI Timer
  TimerValue = IoRead32(BaseAddr);

// need to adjust the values based off of the start time
    EndValue = TheRest + TimerValue;

    // check for overflow on addition.  possibly a problem
    if (EndValue < TimerValue)
    {
        OverFlow++;
    }
    // here make sure that EndValue is less than the max value
    //  of the counter
    else if (NUM_BITS_IN_ACPI_TIMER < MAX_ACPI_TIMER_BITS)
    {
        OverFlow += EndValue / (1 << NUM_BITS_IN_ACPI_TIMER);
        EndValue = EndValue % (1 << NUM_BITS_IN_ACPI_TIMER);
    }

    // let the timer wrap around as many times as calculated
    while (OverFlow)
    {
        // read timer and look to see if the new value read is less than
        //  the current timer value.  if this happens the timer overflowed
    NewTimerValue = IoRead32(BaseAddr);

        if (NewTimerValue < TimerValue)
            OverFlow--;

        TimerValue = NewTimerValue;
    }


    // now wait for the correct number of ticks that need to occur after
    //  all the needed overflows
  while (EndValue > TimerValue){
    NewTimerValue = IoRead32(BaseAddr);

        // check to see if the timer overflowed.  if it did then
        //  the time has elapsed. Because EndValue should be greater than TimerValue
        if (NewTimerValue < TimerValue)
            break;

        TimerValue = NewTimerValue;
    }

    return EFI_SUCCESS;
}

//#endif
// Denverton Aptiov override end - EIP#210294

//Generic Flash part porting hooks

/**
    This function is invoked to do any chipset specific operations 
    that are necessary when enabling the Flash Part for writing

    @param VOID

    @retval VOID

**/
// Denverton AptioV Override Start - EIP#257386
VOID ChipsetFlashDeviceWriteEnable(
    VOID
)
{
	
    UINT32	Reg32 = 0;	// Denverton AptioV Override - EIP#270220
	  
    IoWrite32 (0xCF8, (UINT32)SB_PCI_CFG_ADDRESS(DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_SPI, PCI_FUNCTION_NUMBER_PCH_SPI, R_PCH_SPI_BC));
    IoOr32(0xCFC, (UINT32) (B_PCH_SPI_BC_SYNC_SS));
	  
// Denverton AptioV Override Start - EIP#235471
    IoWrite32 (0xCF8, (UINT32)SB_PCI_CFG_ADDRESS(DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_SPI, PCI_FUNCTION_NUMBER_PCH_SPI, R_PCH_SPI_BC));
    IoOr32(0xCFC, (UINT32) (B_PCH_SPI_BC_WPD));
// Denverton AptioV Override End - EIP#235471

// Denverton AptioV Override Start - EIP#270220
// Disable Prefetch and Cache
    IoWrite32 (0xCF8, (UINT32)SB_PCI_CFG_ADDRESS(DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_SPI, PCI_FUNCTION_NUMBER_PCH_SPI, R_PCH_SPI_BC));
    Reg32 = IoRead32(0xCFC);
    Reg32 &= ((UINT32)(~B_PCH_SPI_BC_SRC));
    IoWrite32 (0xCF8, (UINT32)SB_PCI_CFG_ADDRESS(DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_SPI, PCI_FUNCTION_NUMBER_PCH_SPI, R_PCH_SPI_BC));
    IoWrite32 (0xCFC, Reg32 | (UINT32)(V_PCH_SPI_BC_SRC_PREF_DIS_CACHE_DIS << N_PCH_SPI_BC_SRC));

    Reg32 = IoRead32(PM_BASE_ADDRESS+R_PCH_SMI_STS);
    if(Reg32 & B_PCH_SMI_STS_TCO) {
    	// Clear TCO SMI Status bit
    	IoWrite32(PM_BASE_ADDRESS+R_PCH_SMI_STS, B_PCH_SMI_STS_TCO);
    }
// Denverton AptioV Override End - EIP#270220
    
    return;
}


/**
    This function is invoked to do any chipset specific operations 
    that are necessary when disabling the Flash Part for writing

    @param VOID

    @retval VOID

**/
VOID ChipsetFlashDeviceWriteDisable(
    VOID
)
{
// Denverton AptioV Override Start - EIP#270220	
	UINT32	Reg32 = 0;
	
// Enable Prefetch and Cache  
	IoWrite32 (0xCF8, (UINT32)SB_PCI_CFG_ADDRESS(DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_SPI, PCI_FUNCTION_NUMBER_PCH_SPI, R_PCH_SPI_BC));
	Reg32 = IoRead32(0xCFC);
	Reg32 &= ((UINT32)(~B_PCH_SPI_BC_SRC));
	IoWrite32 (0xCF8, (UINT32)SB_PCI_CFG_ADDRESS(DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_SPI, PCI_FUNCTION_NUMBER_PCH_SPI, R_PCH_SPI_BC));
	IoWrite32 (0xCFC, Reg32 | (UINT32)(V_PCH_SPI_BC_SRC_PREF_EN_CACHE_EN << N_PCH_SPI_BC_SRC));
// Denverton AptioV Override End - EIP#270220
	
// Denverton AptioV Override Start - EIP#235471
    IoWrite32 (0xCF8, (UINT32)SB_PCI_CFG_ADDRESS(DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_SPI, PCI_FUNCTION_NUMBER_PCH_SPI, R_PCH_SPI_BC));
    IoAnd32(0xCFC, (UINT32) (~B_PCH_SPI_BC_WPD));
// Denverton AptioV Override End - EIP#235471
    return;
}
// Denverton AptioV Override End - EIP#257386

#if BIOS_LOCK_ENABLE
/**
    The procedure re-enable the BIOS Write Enable bit before
    Write/Erase procedures if BIOS_LOCK_ENABLE is set. 

    @param SwSmiNum - A S/W SMI number, this value must be 0x20.
    @param Buffer   - A 64bit buffer uses for communicating between
                      caller and this routine.

    @retval VOID

**/

VOID BiosLockEnableSmiFlashHook (
    IN UINT8            SwSmiNum,
    IN OUT UINT64       Buffer
)
{
    if (SwSmiNum != 0x20) return;

/**** PORTING REQUIRED ****
    // Clear BIOSWR_STS
    IoWrite16(PM_BASE_ADDRESS + 0x64, 0x100);
    // Clear TCO_STS
    IoWrite32(PM_BASE_ADDRESS + 0x34, BIT13);
****/

}    
#endif

// End Generic Flash part porting hooks

#if CMOS_MANAGER_SUPPORT
/**
    This function is used to access addresses in the CMOS
    register range (0x80-0xff), for PEI and DXE boot phases.

    @param PeiServices - PEI Services table pointer (NULL in DXE phase)
    @param AccessType - ReadType or WriteType to specify the
                          type of access
    @param CmosRegister - The CMOS register to access
    @param CmosParameterValue - Pointer to the data variable to be
                                accessed

    @retval EFI_STATUS (return value) 
            EFI_SUCCESS The access operation was successfully.
            Otherwise A valid EFI error code is returned.

    @note  This function is used when a translation from logical address
              to index port value is required.
**/

EFI_STATUS ReadWriteCmosBank2 (
    IN EFI_PEI_SERVICES         **PeiServices,  // NULL in DXE phase
    IN CMOS_ACCESS_TYPE         AccessType,
    IN UINT16                   CmosRegister,
    IN OUT UINT8                *CmosParameterValue )
{
/**** PORTING REQUIRED ****
    if ((CmosRegister < 0x80) || (CmosRegister > 0xff))
        return EFI_INVALID_PARAMETER;

    // Some chipsets require translation from the logical CMOS address to a
    // physical CMOS index port value. However, other chipsets do not require
    // a translation and the index/data port can be directly used for 
    // accessing the second bank.

    IoWrite8( CMOS_IO_EXT_INDEX, (UINT8)CmosRegister );

    if (AccessType == ReadType) {
        *CmosParameterValue = IoRead8( CMOS_IO_EXT_DATA );
    } else {
        IoWrite8( CMOS_IO_EXT_DATA, *CmosParameterValue );
    }
****/
    return EFI_SUCCESS;
}

/**
    This function is checked CMOS battery is good or not.

    @param PeiServices - PEI Services table pointer (NULL in DXE phase)

    @retval BOOLEAN  
            TRUE The CMOS is battery is good.
            FALSE The CMOS is battery is bad.
**/
BOOLEAN SbGetRtcPowerStatus (
    IN EFI_PEI_SERVICES     **PeiServices )
{
/**** PORTING REQUIRED ****
    return (READ_PCI8_SB(SB_REG_GEN_PMCON_3) & 4) ? FALSE : TRUE; // 0xA4
****/
    return TRUE;
}

#endif  // #if CMOS_MANAGER_SUPPORT

// Begin Generic RTC SMM library porting hooks

/**
    This function reads one byte from CMOS register addressed by Index

    @param Index - CMOS Index

    @return The value is read by the index.

**/
UINT8 ReadCmos(
    IN UINT8 Index
)
{
    UINT8       NMI = 0;      // IoRead8(0x70) & 0x80;   //Read bit 7 (NMI setting). // Denverton AptioV Override - EIP#280221  
    UINT8       volatile Value;
    UINT8       Reg;
//------------------
    
    // Denverton AptioV Override Start- EIP#280221      
    SwitchAlternateAccessMode(TRUE);
    
    NMI = IoRead8(0x70) & 0x80;   //Read bit 7 (NMI setting).
    
    SwitchAlternateAccessMode(FALSE);
    // Denverton AptioV Override End- EIP#280221  

    if(Index<0x80) Reg=0x70;
    else Reg=0x72;

    IoWrite8(Reg, Index | NMI);
    Value = IoRead8(Reg+1);               //Read register.

    return (UINT8)Value;
}

/**
    This function writes value to CMOS register addressed by Index

    @param Index - CMOS register index
    @param Value - Value to write

    @retval VOID

**/
VOID WriteCmos(
    IN UINT8 Index, 
    IN UINT8 Value
)
{
    UINT8       NMI = 0;	// IoRead8(0x70) & 0x80;   //Read bit 7 (NMI setting).     // Denverton AptioV Override - EIP#280221
    UINT8       Reg;
//------------------
    
    // Denverton AptioV Override Start- EIP#280221
    SwitchAlternateAccessMode(TRUE);
    
    NMI = IoRead8(0x70) & 0x80;   //Read bit 7 (NMI setting).
    
    SwitchAlternateAccessMode(FALSE);
    // Denverton AptioV Override Start- EIP#280221

    if(Index<0x80) Reg=0x70;
    else Reg=0x72;

    IoWrite8(Reg, Index | NMI);
    IoWrite8(Reg+1, Value);                  //Write Register.
}

/**
    This function returns SMI state
              
    @param VOID

    @retval TRUE SMI enabled, FALSE - otherwise

    @note  This function must work at runtime. Do not use boot time services/protocols

**/
BOOLEAN SbLib_GetSmiState(
    VOID
)
{
#if SMM_SUPPORT
/** Porting required
    UINT32  SmiCtl = IoRead32 (PM_BASE_ADDRESS + 0x30) ;
//----------------------------------
    return ((SmiCtl & BIT00)!= 0);
**/
    return FALSE;
#else
    return FALSE;
#endif
}

/**
    This function disables SMI
              
    @param VOID

    @retval VOID

    @note  This function should be used ONLY in critical parts of code
           This function must work at runtime. Do not use boot time services/protocols

**/
VOID SbLib_SmiDisable(
    VOID
)
{
#if SMM_SUPPORT
/** Porting required
    UINT32  SmiCtl = IoRead32 (PM_BASE_ADDRESS + 0x30) ;
//----------------------------------
    SmiCtl &= (~BIT01); //Mask OFF EOS bit
    IoWrite32 ( PM_BASE_ADDRESS + 0x30, SmiCtl & (~BIT00));
**/
#endif
}

/**
    This function enables SMI
              
    @param VOID

    @retval VOID

    @note  This function should be used ONLY in critical parts of code
           This function must work at runtime. Do not use boot time services/protocols

**/
VOID SbLib_SmiEnable(
    VOID
)
{
#if SMM_SUPPORT
/** Porting required

    UINT32  SmiCtl = IoRead32 (PM_BASE_ADDRESS + 0x30) ;
//----------------------------------
    SmiCtl &= (~BIT01); //Mask OFF EOS bit
    IoWrite32 ( PM_BASE_ADDRESS + 0x30, SmiCtl | BIT00);

**/
#endif
}

//---------------------------------------------------------------------------
#if SMM_SUPPORT
//---------------------------------------------------------------------------

/**
    This hook is called in the very SMI entry and exit.
    Save/Restore chipset data if needed.

    @param Save - TRUE = Save / FALSE = Restore

    @retval EFI_SUCCESS
**/

EFI_STATUS SbSmmSaveRestoreStates (
    IN BOOLEAN                      Save )
{
/**** Porting Required
    static volatile UINT8   StoreCMOS;
    static volatile UINT8   StoreExtCMOS;
    static volatile UINT32  StoreCF8;
    static volatile UINT32  AltAcc;
    static volatile UINT8   RtcRegA;

    if (Save) {
      StoreCF8 = IoRead32(0xcf8);    // Store CF8 (PCI index)
  
      // Save Alternate access bit. (Intel Only)
      AltAcc = READ_MEM32_RCRB(RCRB_MMIO_GCS) & 16;
      // Enable Alternate access mode, let port 70h can be readable.
      SET_MEM32_RCRB(RCRB_MMIO_GCS, 0x10);
      while ((READ_MEM32_RCRB(RCRB_MMIO_GCS) & 16) == 0);
  
      StoreCMOS = IoRead8(RTC_IO_INDEX); // Save 0x70
  
      // Disable Alternate access mode. (Intel Only)
      RESET_MEM32_RCRB(RCRB_MMIO_GCS, 0x10);
      while (READ_MEM32_RCRB(RCRB_MMIO_GCS) & 16);
  
      StoreExtCMOS = IoRead8(CMOS_IO_EXT_INDEX);  // Save 0x72
    } else {
      do {
          RtcRegA = READ_IO8_RTC(0x8a);
      } while (RtcRegA & 0x80);
  
      IoWrite8(RTC_IO_INDEX, StoreCMOS);     // Restore 0x70
      IoWrite8(CMOS_IO_EXT_INDEX, StoreExtCMOS); // Restore 0x72
  
      // Restore Alternate access bit. (Intel Only)
      RW_MEM32_RCRB(RCRB_MMIO_GCS, AltAcc, 0x10);
      while ((READ_MEM32_RCRB(RCRB_MMIO_GCS) & 16) != AltAcc);
  
      IoWrite32(0xcf8, StoreCF8);    // Restore 0xCF8 (PCI index)
    }
****/
    return EFI_SUCCESS;
}

//---------------------------------------------------------------------------
#endif  // END OF SMM Related Porting Hooks
//---------------------------------------------------------------------------

/**
    This function is PM Specific function to check and Report to
    the System Status Code - CMOS Battery and Power Supply Power 
    loss/failure. Also it responsible of clearing PM Power Loss
    Statuses

    @param VOID

    @retval VOID

**/
VOID CspLibCheckPowerLoss(
    VOID
)
{
/** Porting required
    GEN_PMCON2_REG  pm2;
    GEN_PMCON3_REG  pm3;
    RTC_REG_D       RegD;
    BOOLEAN         RtcLostPower = FALSE;
//------------------
    //b00|d00|f1f|reg 0xa2
    pm2.GEN_PMCON2=*((UINT8*)PCIE_CFG_ADDR(0,0x1f,0,0xa2));
    //b00|d00|f1f|reg 0xa4      
    pm3.GEN_PMCON3=*((UINT8*)PCIE_CFG_ADDR(0,0x1f,0,0xa4));

    //Read RTC Power Register
    RegD.REG_D = ReadRtcIndex(RTC_REG_D_INDEX);

//TODO//TODO//TODO//TODO//TODO//TODO//TODO//TODO
//Set the correct Code # here
    //Clear Power Failure/Status bits   
    //-------------------------------------------------------
    //this bit means it was a power loss during S0 or S1,
    //cleared by writing 1 to it
//  if(pm2.PwrOkFail) ERROR_CODE(0x00,  EFI_ERROR_MAJOR);

    //-------------------------------------------------------
    //this bit means it was a power loss on CPU VRM well 
    // during S0 or S1, cleared by writing 0 to it
    if(pm2.CpuPwrFail){ 
//      ERROR_CODE(0x01,  EFI_ERROR_MAJOR);
        pm2.CpuPwrFail=0;
    }

    //-------------------------------------------------------
    //this bit means that S4 signal assertion was not long 
    //enough, cleared by writing 1 to it
//  if(pm2.SlpS4Fail) ERROR_CODE(0x02,  EFI_ERROR_MINOR);

    //-------------------------------------------------------
    //this bit means that a power was shut down because of 
    //CPU Power Trip, cleared by writing 1 to it
    if(pm2.ThermTripSts) ERROR_CODE(GENERIC_CPU_THERMAL_ERROR, EFI_ERROR_MAJOR);
    
    //-------------------------------------------------------
    //this bit means that RESET# was active 
    //cleared by writing 1 to it
//  if(pm2.ResetSts) ERROR_CODE(0x04,  EFI_ERROR_MINOR);

    //-------------------------------------------------------
    //this bit means that Battery Bad or was removed
    //cleared by writing 1 to it
//  if(pm3.PwrFail) ERROR_CODE(0x05,  EFI_ERROR_MAJOR);

    //-------------------------------------------------------
    //this bit means that RTC Battery Bad or was removed
    //cleared by writing 0 to it
    if(pm3.RtcPwrFail) {
    //-------------------------------------------------
        pm3.RtcPwrFail=0;
        RtcLostPower=TRUE;
    }   

    if(RegD.DataValid==0) RtcLostPower=TRUE;

    //Write back Cleared Statuses
    *((UINT8*)PCIE_CFG_ADDR(0,0x1f,0,0xa2))=pm2.GEN_PMCON2;
    *((UINT8*)PCIE_CFG_ADDR(0,0x1f,0,0xa4))=pm3.GEN_PMCON3;

    if(RtcLostPower){
        #if defined(SB_BAD_BATTERY_ERR_CODE_SUPPORT) && (SB_BAD_BATTERY_ERR_CODE_SUPPORT != 0)
        ERROR_CODE(DXE_SB_BAD_BATTERY,  EFI_ERROR_MAJOR);
        #endif
        InitRtc();
    }
**/
}

/**
    This function sets LPC Bridge Device Decoding
              
    @param LpcPciIo Pointer to LPC PCI IO Protocol
    @param Base I/O base address, if Base is 0 means disabled the
           decode of the device 
    @param DevUid The device Unique ID
    @param Type Device Type, please refer to AMISIO.h

    @retval EFI_STATUS
            EFI_SUCCESS Set successfully.
            EFI_UNSUPPORTED There is not proper Device Decoding 
                            register for the device UID.
**/
EFI_STATUS SbLibSetLpcDeviceDecoding (
    IN EFI_PCI_IO_PROTOCOL      *LpcPciIo,
    IN UINT16                   Base,
    IN UINT8                    DevUid,
    IN SIO_DEV_TYPE             Type )
{
    EFI_STATUS              Status = EFI_UNSUPPORTED;

/** Porting Required

    UINT16                  ComRange[] = { 0x3f8, 0x2f8, 0x220, 0x228,
                                           0x238, 0x2e8, 0x338, 0x3e8, 0};
    UINT16                  LptRange[] = { 0x378, 0x278, 0x3bc, 0};
    UINT16                  FpcRange[] = { 0x3f0, 0x370, 0};
    UINT16                  IoRangeMask16 = 0xffff;
    UINT16                  IoRangeSet16 = 0;
    UINT16                  IoEnMask16 = 0xffff;
    UINT16                  IoEnSet16 = 0;
    UINT8                   i;    

    switch (Type) {
        // FDC Address Range
        case (dsFDC) :
            if (Base == 0) IoEnMask16 &= ~BIT03;
            else {
                for (i = 0; (FpcRange[i] != 0) && (FpcRange[i] != Base); i++);
                if (FpcRange[i]) {
                    IoEnSet16 |= BIT03;
                    IoRangeMask16 &= ~BIT12;
                    IoRangeSet16 |= (i << 12);
                }
                else return EFI_UNSUPPORTED;
            }
            break;

        // LPT Address Range
        case (dsLPT) :
            if (Base == 0) IoEnMask16 &= ~BIT02;
            else {
                for (i = 0; (LptRange[i] != 0) && (LptRange[i] != Base); i++);
                if (LptRange[i]) {
                    IoEnSet16 |= BIT02;
                    IoRangeMask16 &= ~(BIT09 | BIT08);
                    IoRangeSet16 |= (i << 8);
                } else return EFI_UNSUPPORTED;
            }
            break;

        // ComA Address Range
        case (dsUART) :
            if (Base == 0) {
                if (DevUid) IoEnMask16 &= ~BIT01;
                else IoEnMask16 &= ~BIT00;
            } else {
                for (i = 0; (ComRange[i] != 0) && (ComRange[i] != Base); i++);
                if (ComRange[i]) {
                    if (DevUid) {
                        IoEnSet16 |= BIT01;
                        IoRangeMask16 &= ~(BIT06 | BIT05 | BIT04);
                        IoRangeSet16 |= (i << 4);
                    } else {
                        IoEnSet16 |= BIT00;
                        IoRangeMask16 &= ~(BIT02 | BIT01 | BIT00);
                        IoRangeSet16 |= i;
                    }
                } else return EFI_UNSUPPORTED;
            }
            break;

        // KBC Address Enable
        case (dsPS2K) :
        case (dsPS2M) :
        case (dsPS2CK) :
        case (dsPS2CM) :
            if (Base == 0) IoEnMask16 &= ~BIT10;
            else IoEnSet16 |= BIT10;
            break;

        // Game Port Address Enable
        case (dsGAME) :
            if (Base == 0) IoEnMask16 &= ~(BIT09 | BIT08);
            else {
                if (Base == 0x200) {
                    IoEnSet16 |= BIT08;
                } else {
                    if (Base == 0x208) IoEnSet16 |= BIT09;
                    else return EFI_UNSUPPORTED;
                }
            }
            break;

        // LPC CFG Address Enable
        case (0xff) :
            if (Base == 0x2e) IoEnSet16 |= BIT12;
            else {
                if (Base == 0x4e) IoEnSet16 |= BIT13;
                else {
                    if (Base == 0x62) IoEnSet16 |= BIT11;
                    else {
                    if (Base) SbLibSetLpcGenericDecoding( LpcPciIo,
                                                           Base ,
                                                           4,
                                                           TRUE );
                        else return EFI_UNSUPPORTED;
                    }
                }
            }
            break;

        default :
            return EFI_UNSUPPORTED;
    }

    if(LpcPciIo == NULL)
    {
      MmPci16(0, 0x1F, 0, ICH_REG_LPC_IODEC) &= ~(IoRangeMask16);  // 0x80
      MmPci16(0, 0x1F, 0, ICH_REG_LPC_IODEC) |= IoRangeSet16;

      MmPci16(0, 0x1F, 0, ICH_REG_LPC_EN) &= ~(IoEnMask16);  // 0x82
      MmPci16(0, 0x1F, 0, ICH_REG_LPC_EN) |= IoEnSet16;
    }
    else
    {
        UINT16 Data16;
    //-----------------
        Status = LpcPciIo->Pci.Read(LpcPciIo, EfiPciIoWidthUint16, ICH_REG_LPC_IODEC, 1, &Data16); 
        Data16 &= ~(IoRangeMask16);
        Data16 |= IoRangeSet16;
        Status = LpcPciIo->Pci.Write(LpcPciIo, EfiPciIoWidthUint16, ICH_REG_LPC_IODEC, 1, &Data16); 

        Status = LpcPciIo->Pci.Read(LpcPciIo, EfiPciIoWidthUint16, ICH_REG_LPC_EN, 1, &Data16); 
        Data16 &= ~(IoEnMask16);
        Data16 |= IoEnSet16;
        Status = LpcPciIo->Pci.Write(LpcPciIo, EfiPciIoWidthUint16, ICH_REG_LPC_EN, 1, &Data16); 
    }
Porting End **/

    return EFI_SUCCESS;
}

/**
    This function set LPC Bridge Generic Decoding
              
    @param LpcPciIo Pointer to LPC PCI IO Protocol
    @param Base I/O base address
    @param Length I/O Length
    @param Enabled Enable/Disable the generic decode range register

    @retval EFI_STATUS
            EFI_SUCCESS Set successfully.
            EFI_UNSUPPORTED This function is not implemented or the
                            Length more than the maximum supported
                            size of generic range decoding.
            EFI_INVALID_PARAMETER the Input parameter is invalid.
            EFI_OUT_OF_RESOURCES There is not available Generic
                                 Decoding Register.
            EFI_NOT_FOUND the generic decode range will be disabled
                          is not found.
**/
EFI_STATUS SbLibSetLpcGenericDecoding (
    IN EFI_PCI_IO_PROTOCOL      *LpcPciIo,
    IN UINT16                   Base,
    IN UINT16                   Length,
    IN BOOLEAN                  Enable )
{
/** Porting Required
    UINT32                  IoGenDecode32;
    UINT16                  IoGenDecIndex;
    UINT16                  Buffer16;
    UINT8                   Bsf8 = 0;
    UINT8                   Bsr8 = 0;

    if (Length > 0x100) return EFI_UNSUPPORTED;

    if (Length == 0) return EFI_INVALID_PARAMETER;

    if (Length < 4) Length = 4;

    // Read I/O Generic Decodes Register.
    for (IoGenDecIndex = 0; IoGenDecIndex < 4; IoGenDecIndex++) {
        IoGenDecode32 = MmPci32(0, 0x1F, 0, (ICH_REG_LPC_GEN1_DEC + IoGenDecIndex * 4));
        if (Enable) {
            if ((IoGenDecode32 & 1) == 0) break;
        } else {
            if (((IoGenDecode32 & 0xfffc) == Base) && (IoGenDecode32 & 1)) {
                IoGenDecode32 = 0; // Disable & clear the base/mask fields
                break;
            }
        }
    }

    if (Enable) {
        if (IoGenDecIndex == 4) return EFI_OUT_OF_RESOURCES;

        Buffer16 = Length;
        while ((Buffer16 % 2) == 0) {
            Buffer16 /= 2;
            Bsf8++;
        }

        while (Length) {
            Length >>= 1;
            Bsr8++;
        }

        if (Bsf8 == (Bsr8 - 1)) Bsr8--;
    
        Length = (1 << Bsr8) - 1 ;

        Base &= (~Length);

        IoGenDecode32 = Base | (UINT32)((Length >> 2) << 18) | 1;

    } else {
        if (IoGenDecIndex == 4) return EFI_NOT_FOUND;
    }

    if(LpcPciIo == NULL)
        MmPci32(0, 0x1F, 0, (ICH_REG_LPC_GEN1_DEC + IoGenDecIndex * 4)) = IoGenDecode32; // 0x84
    else
        Status = LpcPciIo->Pci.Write(LpcPciIo, 
                                     EfiPciIoWidthUint32, 
                                     (ICH_REG_LPC_GEN1_DEC + IoGenDecIndex * 4), 
                                     1, 
                                     &IoGenDecode32); 
Porting End **/

    return EFI_SUCCESS;
}

#if defined(HPET_APIC_INTERRUPT_MODE) && (HPET_APIC_INTERRUPT_MODE != 0)
//----------------------------------------------------------------------------
// Generic IO APIC routine.
//----------------------------------------------------------------------------
UINT8 gBspLocalApicID = 0;

/**
    This routine returns a structure pointer to the I/O APIC.

    @param VOID

    @retval IO_APIC structure.
**/
struct IO_APIC* IoApicBase( VOID )
{
	static UINT32 IoApicAddr = 0;
	if (IoApicAddr == 0 || IoApicAddr == -1) {
        // This value may need to read from chipset registers.
		IoApicAddr = APCB;
	}

	return (struct IO_APIC*)IoApicAddr;
}

/**
    This function writes a 32bits data to the register of
    I/O APIC.

    @param Reg - The register offset to be written.
    @param Value - A 32bits data will be written to the register
                   of I/O APIC.

    @retval EFI_SUCCESS
**/
EFI_STATUS IoApicWrite (
    IN UINT8        Reg,
    IN UINT32       Value )
{
	struct IO_APIC *IoApicStruct = IoApicBase();

	MMIO_WRITE8((UINT64)&IoApicStruct->Index, Reg);
	MMIO_WRITE32((UINT64)&IoApicStruct->Data, Value);

	return EFI_SUCCESS;
}

/**
    This function reads a 32bits data from the register of
    I/O APIC.

    @param Reg - the register offset to be read.

    @return A 32bits data read from the register of I/O APIC.
**/
UINT32 IoApicRead (
    IN UINT8        Reg )
{
	struct IO_APIC *IoApicStruct = IoApicBase();

	MMIO_WRITE8((UINT64)&IoApicStruct->Index, Reg);
	return MMIO_READ32((UINT64)&IoApicStruct->Data);
}

/**
    This function enables the specific interrupt pin of I/O APIC.

    @param Irq - The pin number of I/O APIC
    @param LevelTriggered - Trigger mechanism (level or edge)
                            for this INT pin.
    @param Polarity - Specifies the polarity of the INT pin.
                      (Active High or Active Low)
    @retval EFI_SUCCESS
**/
EFI_STATUS IoApicEnableIrq (
    IN UINT8        Irq,
    IN BOOLEAN      LevelTriggered,
    IN BOOLEAN      Polarity )
{
	IO_APIC_ROUTE_ENTRY	ApicEntry;
	union ENTRY_UNION			Eu = {{0, 0}};

	ApicEntry.DestMode = 0; // 0: physical
	ApicEntry.Mask = 0;		 // 0 : enable
	ApicEntry.Dest = gBspLocalApicID; // suppose the BSP handle interrupt.
	ApicEntry.DeliveryMode = 0;      // 000: FIXED
	ApicEntry.Polarity = (Polarity) ? 1 : 0;
	ApicEntry.Trigger = (LevelTriggered) ? 1 : 0;
	ApicEntry.Vector = MASTER_INTERRUPT_BASE + Irq;

	Eu.Entry = ApicEntry;
	IoApicWrite(IO_APIC_REDIR_TABLE_HIGH + 2 * Irq, Eu.W2);
	IoApicWrite(IO_APIC_REDIR_TABLE_LOW + 2 * Irq, Eu.W1);
	return EFI_SUCCESS;
}

/**
    This function disables the specific interrupt pin of I/O APIC.

    @param Irq - The pin number of I/O APIC

    @retval EFI_SUCCESS
**/
EFI_STATUS IoApicDisableIrq (
    IN UINT8        Irq )
{
	union ENTRY_UNION Eu = {{0, 0}};

	Eu.W1 = IoApicRead(IO_APIC_REDIR_TABLE_LOW + 2 * Irq);
	Eu.W2 = IoApicRead(IO_APIC_REDIR_TABLE_HIGH + 2 * Irq);
	Eu.Entry.Mask = 1;
	IoApicWrite(IO_APIC_REDIR_TABLE_LOW + 2 * Irq, Eu.W1);
	IoApicWrite(IO_APIC_REDIR_TABLE_HIGH + 2 * Irq, Eu.W2);
	IoApicEoi(Irq);

	return EFI_SUCCESS;
}

/**
    This routine masks the specific interrupt pin of I/O APIC.

    @param Irq - The pin number of I/O APIC

    @retval EFI_SUCCESS
**/
EFI_STATUS IoApicMaskIrq (
    IN UINT8        Irq )
{
	union ENTRY_UNION Eu = {{0, 0}};

	Eu.W1 = IoApicRead(IO_APIC_REDIR_TABLE_LOW + 2 * Irq);
	Eu.Entry.Mask = 1;
	IoApicWrite(IO_APIC_REDIR_TABLE_LOW + 2 * Irq, Eu.W1);

	return EFI_SUCCESS;
}

/**
    This routine unmasks the specific interrupt pin of I/O APIC.

    @param Irq - The pin number of I/O APIC

    @retval EFI_SUCCESS
**/
EFI_STATUS IoApicUnmaskIrq (
    IN UINT8        Irq )
{
	union ENTRY_UNION Eu = {{0, 0}};

	Eu.W1 = IoApicRead(IO_APIC_REDIR_TABLE_LOW + 2 * Irq);
	Eu.Entry.Mask = 0;
	IoApicWrite(IO_APIC_REDIR_TABLE_LOW + 2 * Irq, Eu.W1);

	return EFI_SUCCESS;
}

/**
    This routine issues an EOI to the specific pin of I/O APIC.

    @param Irq - The pin number of I/O APIC

    @retval EFI_SUCCESS
**/
EFI_STATUS IoApicEoi (
    IN UINT8        Irq )
{
	EFI_STATUS 	Status = EFI_SUCCESS;
	UINT32		Vector = MASTER_INTERRUPT_BASE + Irq;
	struct IO_APIC *IoApicStruct = IoApicBase();
	UINT32		Isr = 0;
	union ENTRY_UNION Eu = {{0, 0}};

	do {
		MMIO_WRITE32((UINT64)&IoApicStruct->Eoi, Vector);
		Eu.W1 = IoApicRead(IO_APIC_REDIR_TABLE_LOW + 2 * Irq);
	} while (Eu.Entry.Irr);

	do {
		MMIO_WRITE32(LOCAL_APIC_BASE + APIC_EOI_REGISTER, Vector);
		Isr = MMIO_READ32(LOCAL_APIC_BASE + ISR_REG (Vector));
	} while (Isr & ISR_BIT(Vector));

	return EFI_SUCCESS;
}

/**
    This routine gets the pin number of I/O APIC for HPET.

    @param VOID

    @return Irq - The pin number of I/O APIC for HPET.
**/
UINT8 GetHpetApicPin (VOID)
{
	EFI_STATUS 	Status = EFI_SUCCESS;
	UINT8		Irq = 0;

  volatile HPET_TIMER_CONFIGURATION_REGISTER   TimerConfiguration;

  TimerConfiguration.Uint64 = MMIO_READ64( HPET_BASE_ADDRESS + HPET_TIMER_CONFIGURATION_OFFSET + HPET_OFFSET * HPET_TIMER_STRIDE );
	Irq = TimerConfiguration.Bits.InterruptRoute;

	return Irq;
}

/**
    This routine checks the pin of I/O APIC for HPET is enabled or
    not.

    @param VOID

    @retval TRUE The pin of I/O APIC for HPET is enabled 
    @retval FALSE The pin of I/O APIC for HPET is disabled 
**/
BOOLEAN IsHpetApicEnable (VOID)
{
	union ENTRY_UNION Eu = {{0, 0}};
	UINT8 Irq;

	Irq = GetHpetApicPin();

	Eu.W1 = IoApicRead(IO_APIC_REDIR_TABLE_LOW + 2 * Irq);

	return (Eu.Entry.Mask) ? FALSE : TRUE;
}
#endif

/**
    This function checks if TopSwap (A16, A17, A18 address lines
    inversion) is on
              
    @param VOID

    @retval TRUE TopSwap is ON
    @retval FALSE TopSwap is OFF

    @note  Intel Chipsets are porting required
**/
BOOLEAN IsTopSwapOn( VOID )
{
  return FALSE;
/*

  volatile UINT8* Address8;

  Address8 = (UINT8 *)(SB_RCBA + SB_BACKED_UP_CONTROL_REGISTER);

  return ((*Address8) & 1) ? TRUE : FALSE;
*/
}

/**
    This function sets TopSwap (A16, A17, A18 address lines 
    inversion) to ON or OFF.

    @param BOOLEAN On - if TRUE, set TopSwap to ON, if FALSE - set to OFF

    @retval VOID

    @note  Intel Chipsets are porting required
**/
VOID  SetTopSwap(
    IN BOOLEAN      On )
{
/*
  volatile UINT8* Address8;

  Address8 = (UINT8 *)(SB_RCBA + SB_BACKED_UP_CONTROL_REGISTER);

  if (On) {
    *Address8 |= 1;
  } else {
    *Address8 &= 0xFE;
  }
*/
}

#if SMBIOS_SUPPORT && SB_WAKEUP_TYPE_FN
/**
    Detect and return SMBIOS wake-up type

    @param VOID

    @retval 0x01 = Other
            0x02 = Unknown
            0x03 = APM Timer
            0x04 = Modem Ring
            0x05 = LAN Remote
            0x06 = Power Switch
            0x07 = PCI PME#
            0x08 = AC Power Restored

    @note  Porting required 
**/
UINT8 getWakeupTypeForSmbios (
    VOID
)
{
    UINT8   WakeupTypeForSmbios = PcdGet8(PcdSbWakeType);

    if (WakeupTypeForSmbios >= SbWakeupTypeSmBiosMax) {
      WakeupTypeForSmbios = DEFAULT_SYSTEM_WAKEUP_TYPE;
    }
    
    return WakeupTypeForSmbios;
}
#endif

/**
    Enable or disable Alternate Access Mode.

    @param Switch TRUE: Enable Alternate Access Mode
                  FALSE: Disable Alternate Access Mode

    @retval VOID
**/
VOID SwitchAlternateAccessMode (
    IN BOOLEAN      Switch
)
{
  /** PORTING REQUIRED **
  UINT32  Rcrb;

  Rcrb = PcdGet32 (PcdRcrbBaseAddress);

  if (Switch) { 
    do { 
      // Enable Alternate Access Mode
      MmioOr32 (Rcrb + R_PCH_RCRB_GCS, B_PCH_RCRB_GCS_AME);
    } while((MmioRead32 (Rcrb + R_PCH_RCRB_GCS) & B_PCH_RCRB_GCS_AME) == 0);
  } else {
    do {
      // Disable Alternate Access Mode
      MmioAnd32 (Rcrb + R_PCH_RCRB_GCS, ~B_PCH_RCRB_GCS_AME);
    } while(MmioRead32 (Rcrb + R_PCH_RCRB_GCS) & B_PCH_RCRB_GCS_AME);
  }
  **/
  // Denverton AptioV Override Start- EIP#280221
  UINT32	Data32 = 0;
  
  Data32 = IoRead32((UINT64) PCH_PCR_ADDRESS (PID_ITSS, R_PCH_PCR_ITSS_GIC));
  
  if(Switch)
  {
    Data32 |= B_PCH_PCR_ITSS_GIC_AME;
  } else {
    Data32 &= ~B_PCH_PCR_ITSS_GIC_AME;

  }

  IoWrite32((UINT64) PCH_PCR_ADDRESS (PID_ITSS, R_PCH_PCR_ITSS_GIC), Data32);
  // Denverton AptioV Override Start- EIP#280221
}

/**
    Read port 70h.

    @param VOID

    @return Data of port 70h.
**/
UINT8 ReadPort70h (
    VOID
)
{
  /** PORTING REQUIRED **
  UINT8  Port70h;

  SwitchAlternateAccessMode (TRUE);

  Port70h = IoRead8(RTC_INDEX_REG);

  SwitchAlternateAccessMode (FALSE);

  return Port70h;
  **/
}

/**
    Enable or disable SB common functions.

    @param SbDevType - The type of SB device, refer to SB_DEV_TYPE
    @param Enable - TRUE: Enable / FALSE: Disable

    @retval VOID

**/
VOID SbEnableDisableFunctions (
    IN SB_DEV_TYPE  SbDevType,
    IN BOOLEAN      Enable
)
{
  /** PORTING REQUIRED **
  UINT32  Rcrb;

  Rcrb = PcdGet32 (PcdRcrbBaseAddress);
  
  if (Enable) {
    MmioAnd32 (Rcrb + R_PCH_RCRB_FUNC_DIS, (UINT32) ~(1 << PchDevType));
  } else {
    MmioOr32 (Rcrb + R_PCH_RCRB_FUNC_DIS, (UINT32) (1 << PchDevType));
  }
  **/
}

/**
    Save or Restore All Hardware SMIs

    @param Save TRUE  : Save all HW SMIs
                FALSE : Restore saved HW SMIs

    @retval VOID

**/
VOID SbSaveRestoreAllHwSmi (
    IN BOOLEAN      Save
)
{
  /** PORTING REQUIRED **
  static UINT16 Pm1EnSave;
  static UINT32 SmiEnSave;
  static UINT32 Gpe0aEnSave;
  static UINT16 AltGpiSmiEnSave;
         UINT32 Data32;
         UINT16 PmBase;
         UINT16 GpioBase;

  PmBase    = PcdGet16 (PcdAcpiIoPortBaseAddress);
  GpioBase  = PcdGet16 (PcdGpioPortBaseAddress);
           
  if (Save) {
    Pm1EnSave = IoRead16 (PmBase + R_PCH_ACPI_PM1_EN);
    SmiEnSave = IoRead32 (PmBase + R_PCH_SMI_EN) & ~(B_PCH_SMI_EN_GBL_SMI | B_PCH_SMI_EN_EOS | B_PCH_SMI_EN_APMC);
    Gpe0aEnSave     = IoRead32 (PmBase + R_PCH_ACPI_GPE0a_EN);
    AltGpiSmiEnSave = IoRead16 (GpioBase + R_PCH_ALT_GP_SMI_EN);
  } else {
    IoWrite16 (PmBase + R_PCH_ACPI_PM1_EN, Pm1EnSave);
    Data32 = IoRead32 (PmBase + R_PCH_SMI_EN);
    Data32 &= (B_PCH_SMI_EN_GBL_SMI | B_PCH_SMI_EN_EOS | B_PCH_SMI_EN_APMC);
    IoWrite32 (PmBase + R_PCH_SMI_EN, (Data32 | SmiEnSave));
    IoWrite32 (PmBase + R_PCH_ACPI_GPE0a_EN, Gpe0aEnSave);
    IoWrite16 (GpioBase + R_PCH_ALT_GP_SMI_EN, AltGpiSmiEnSave);
  }
  **/
}

/**
    Disable all HW SMIs

    @param VOID

    @retval VOID

**/
VOID SbDisableAllHwSmi (
    VOID
)
{
  /** PORTING REQUIRED **
  UINT16 PmBase;
  UINT16 GpioBase;

  PmBase    = PcdGet16 (PcdAcpiIoPortBaseAddress);
  GpioBase  = PcdGet16 (PcdGpioPortBaseAddress);

  // Clear PM1_EN
  IoAnd16 (PmBase + R_PCH_ACPI_PM1_EN, 0);
  // Clear SMI_EN except B_PCH_SMI_EN_GBL_SMI, B_PCH_SMI_EN_EOS and B_PCH_SMI_EN_APMC.
  IoAnd32 (PmBase + R_PCH_SMI_EN, (B_PCH_SMI_EN_GBL_SMI | B_PCH_SMI_EN_EOS | B_PCH_SMI_EN_APMC));
  // Clear GPE0_EN.
  IoAnd32 (PmBase + R_PCH_ACPI_GPE0a_EN, 0);
  // Clear ALT_GPI_EN.
  IoAnd16 (GpioBase + R_PCH_ALT_GP_SMI_EN, 0);
  **/
}

/**
    Check NMI bit

    @param VOID

    @retval TRUE  : NMI is enabled.
            FALSE : NMI is disabled.
**/
BOOLEAN SbCheckNmiStatus (
    VOID
)
{
  /** PORTING REQUIRED **
  if (ReadPort70h() & RTC_NMI_MASK) {
    // NMI is disabled.
    return FALSE;
  } else {
    // NMI is Enabled.
    return TRUE;
  }
  **/
  return FALSE;
}

/**
    Enable / Disable NMI Bit

    @param Enable - TRUE  : Enable NMI
                    FALSE : Disable NMI

    @retval TRUE  : NMI is enabled
            FALSE : NMI is disabled
**/
BOOLEAN SbEnableDisableNmi (
    IN BOOLEAN      Enable
)
{
  /** PORTING REQUIRED **
  BOOLEAN IntState = CPULib_GetInterruptState();
  static UINT8   Data8;

  CPULib_DisableInterrupt();
  
  Data8 = IoRead8 (CMOS_IO_STD_INDEX);
  
  if (Enable)
    Data8 &= ~RTC_NMI_MASK;
  else {
    Data8 |= RTC_NMI_MASK;
  }
  IoWrite8 (CMOS_ADDR_PORT, Data8);

  if (IntState) CPULib_EnableInterrupt();
  
  return SbCheckNmiStatus();
  **/
  return FALSE;
}

/**
    Save / Restore NMI bit

    @param Save TRUE  : Save NMI
                FALSE : Restore NMI

    @retval VOID

**/
VOID SbSaveRestoreNmi (
    IN BOOLEAN      Save
)
{
  /** PORTING REQUIRED **
  static  UINT8   NmiMask;
          BOOLEAN IntState = CPULib_GetInterruptState();
          UINT8   Data8;

  CPULib_DisableInterrupt();

  if (Save)
    // Save current NMI_EN.
    NmiMask = ReadPort70h() & RTC_NMI_MASK;
  else {
    Data8 = IoRead8 (CMOS_IO_STD_INDEX) & ~(RTC_NMI_MASK);
    IoWrite8 (CMOS_ADDR_PORT, Data8 | NmiMask);    
  }
  if (IntState) CPULib_EnableInterrupt();
  **/
}

/**
    Trigger SW SMI and adding extra I/O instructions around
    the SMI port write reduces the failure rate.

    @param SwSmi - The number of Software SMI

    @retval VOID

**/
VOID SbSwSmiTrigger (
    IN UINT8        SwSmi
)
{
  /** PORTING REQUIRED **
  IoWrite8 (0x81, 0xEF);
  IoWrite8 (0x81, 0xEF);
  IoWrite8 (R_PCH_APM_CNT, SwSmi);
  IoWrite8 (0x81, 0xEF);
  IoWrite8 (0x81, 0xEF);
  **/
}

/**
    Trigger SW SMI and return the S/W SMI data from the S/W SMI data port.

    @param SwSmi The number of Software SMI
    @param Data Pointer to the data to be written to the S/W SMI data port.

    @return Data Pointer to the data read from the S/W SMI data port.

**/
VOID SbSwSmiIo (
    IN     UINT8    SwSmi,
    IN OUT UINT8    *Data
)
{
  /** PORTING REQUIRED **
  IoWrite8 (R_PCH_APM_STS, *Data);
  SbSwSmiTrigger (SwSmi);
  *Data = IoRead8 (R_PCH_APM_STS);
  **/
}

/**
    Get data from S/W SMI I/O port and return the address of S/W SMI 
    I/O port.

    @param DataValue Pointer to the data buffer for receiving the S/W SMI
                     number read from S/W SMI I/O port.

    @return DataValue Pointer to the data buffer contained the S/W SMI
                      number read from S/W SMI I/O port.

**/
UINT16 SbGetSwSmi (
    IN OUT UINT32   *DataValue
)
{
  /** PORTING REQUIRED **
  *DataValue = (UINT32)(IoRead8(R_PCH_APM_CNT));
  return R_PCH_APM_CNT;
  **/
  return 0xFFFF;
}

/**
    Get Intel TCO2 Status address.(Intel Chipset Only)

    @param AcpiBaseAddr ACPI Base address, it is defined by PM_BASE_ADDRESS
                        normally.

    @return The address of TCO2 status register.
            0xFFFF - not supported.

**/
UINT16 SbGetTco2StsAddress (
    IN UINT16       AcpiBaseAddr
)
{
  /** PORTING REQUIRED **
  return AcpiBaseAddr + PCH_TCO_BASE + R_PCH_TCO2_STS;
  **/
  return 0xFFFF;
}

/**
    Get Intel TCO2 Control address. (Intel Chipset Only)

    @param AcpiBaseAddr ACPI Base address, it is defined by PM_BASE_ADDRESS
                        normally.

    @retval The address of TCO2 control register.
            0xFFFF - not supported.

**/
UINT16 SbGetTco2CntAddress (
    IN UINT16       AcpiBaseAddr
)
{
  /** PORTING REQUIRED **
  return AcpiBaseAddr + PCH_TCO_BASE + R_PCH_TCO2_CNT;
  **/
  return 0xFFFF;
}

/**
    Set After G3 bit.

    @param Set
           TRUE  = Enable G3 Bit
           FALSE = Disable G3 Bit

    @retval VOID

**/
VOID SetAfterG3Bit (
    IN BOOLEAN      Set
)
{
  /** PORTING REQUIRED **
  if (Set)
    SET_PCI8_SB (R_PCH_LPC_GEN_PMCON_3, B_PCH_LPC_GEN_PMCON_AFTERG3_EN);
  else
    RESET_PCI8_SB (R_PCH_LPC_GEN_PMCON_3, B_PCH_LPC_GEN_PMCON_AFTERG3_EN);
  **/
}

/**
    Check After G3 bit.

    @param VOID

    @retval TRUE:  G3 bit is set
            FALSE: G3 bit is not set

**/
BOOLEAN SbCheckAfterG3 (
    VOID
)
{
  /** PORTING REQUIRED **
  if (READ_PCI8_SB ( R_PCH_LPC_GEN_PMCON_3 ) & B_PCH_LPC_GEN_PMCON_AFTERG3_EN)
    return TRUE;
  else
    return FALSE;
  **/
  return FALSE;
}

/**
    Disable LPC Super I/O Devices

    @param SioType - The type of SIO device, refer to SB_LPC_SIO_TYPE.

    @retval VOID

**/
VOID SbDisableLpcDevices (
    IN SB_LPC_SIO_TYPE  SioType
)
{
  /** PORTING REQUIRED **
  UINT16 Data16;

  Data16 = READ_PCI16_SB (R_PCH_LPC_ENABLES);
  switch (SioType) {
    case SbComA:
      Data16 &= ~B_PCH_LPC_ENABLES_COMA_EN;
      break;
    case SbComB:
      Data16 &= ~B_PCH_LPC_ENABLES_COMB_EN;
      break;
    case SbLpt:
      Data16 &= ~B_PCH_LPC_ENABLES_LPT_EN;
      break;
    case SbFdd:
      Data16 &= ~B_PCH_LPC_ENABLES_FDD_EN;
      break;
    case SbGameL:
      Data16 &= ~B_PCH_LPC_ENABLES_GAMEL_EN;
      break;
    case SbGameH:
      Data16 &= ~B_PCH_LPC_ENABLES_GAMEH_EN;
      break;
    case SbKbc:
      Data16 &= ~B_PCH_LPC_ENABLES_KBC_EN;
      break;
    case SbMc:
      Data16 &= ~B_PCH_LPC_ENABLES_MC_EN;
      break;
    case SbCnf1:
      Data16 &= ~B_PCH_LPC_ENABLES_CNF1_EN;
      break;
    case SbCnf2:
      Data16 &= ~B_PCH_LPC_ENABLES_CNF2_EN;
      break;
    default:
      break;
  }
  WRITE_PCI16_SB (R_PCH_LPC_ENABLES, Data16);
  **/
}

/**
    Enable EHCI USB SMI

    @param VOID

    @retval VOID

**/
VOID SbEnableEhciSmi (
    VOID
)
{
  /** PORTING REQUIRED **
  UINT16 PmBase;

  PmBase    = PcdGet16 (PcdAcpiIoPortBaseAddress);
  IoOr32 (PmBase + R_PCH_SMI_EN, B_PCH_SMI_EN_LEGACY_USB2);
  **/
}

/**
    Disable EHCI USB SMI

    @param VOID

    @retval VOID

**/
VOID SbDisableEhciSmi (
    VOID
)
{
  /** PORTING REQUIRED **
  UINT16 PmBase;

  PmBase    = PcdGet16 (PcdAcpiIoPortBaseAddress);
  IoAnd32 (PmBase + R_PCH_SMI_EN, ~B_PCH_SMI_EN_LEGACY_USB2);
  IoWrite32 (PmBase + R_PCH_SMI_STS, B_PCH_SMI_STS_LEGACY_USB2);
  **/
}

/**
    Check power button pressed.

    @param VOID

    @retval TRUE:  Pressed
            FALSE: No Pressed

**/
BOOLEAN IsPowerButtonPressed (
    VOID
)
{  
  /** PORTING REQUIRED **
  UINT16 PmBase;

  PmBase    = PcdGet16 (PcdAcpiIoPortBaseAddress);

  if (IoRead16 (PmBase + R_PCH_ACPI_PM1_STS) & B_PCH_ACPI_PM1_STS_PWRBTN) {
    return TRUE;
  } else {
    return FALSE;
  }
  **/
  return FALSE;
}

/**
    Enable PCI PME bit.

    @param PciAddress The address got from PCI_LIB_ADDRESS macro in
                      PciLib.h
        #define PCI_LIB_ADDRESS(Bus,Device,Function,Register) \
        (((Register) & 0xfff) | (((Function) & 0x07) << 12) | \
        (((Device) & 0x1f) << 15) | (((Bus) & 0xff) << 20))

    @retval VOID

**/
VOID SbEnablePciPme (
    IN UINTN        PciAddress
)
{
  /** PORTING REQUIRED **
  UINT16 CapPtr;
  UINTN  PcieAddress;
  
  PcieAddress = CSP_PCIE_CFG_ADDRESS(
                  (PciAddress >> 20) & 0xFF,
                  (PciAddress >> 15) & 0x1F,
                  (PciAddress >> 12) & 0x07,
                  0);  
  
  CapPtr = FindCapPtr (PcieAddress, 0x01);
  // CapPtr is found
  if (CapPtr != 0) {
    // Set PMEE.
    MmioOr16 (PcieAddress + CapPtr + 4, BIT08);
  }
  **/
}

/**
    Disable PCI PME bit.

    @param PciAddress The address got from PCI_LIB_ADDRESS macro in
                      PciLib.h
        #define PCI_LIB_ADDRESS(Bus,Device,Function,Register) \
        (((Register) & 0xfff) | (((Function) & 0x07) << 12) | \
        (((Device) & 0x1f) << 15) | (((Bus) & 0xff) << 20))

    @retval VOID

**/
VOID SbDisablePciPme (
    IN UINTN        PciAddress
)
{
  /** PORTING REQUIRED **
  UINT16 CapPtr;
  UINTN  PcieAddress;
  
  PcieAddress = CSP_PCIE_CFG_ADDRESS(
                  (PciAddress >> 20) & 0xFF,
                  (PciAddress >> 15) & 0x1F,
                  (PciAddress >> 12) & 0x07,
                  0);  
  
  CapPtr = FindCapPtr (PcieAddress, 0x01);
  // CapPtr is found
  if (CapPtr != 0) {
    // Disable PMEE.
    MmioAnd16 (PcieAddress + CapPtr + 4, ~BIT08);
    // Clear PMES
    MmioWrite16 (PcieAddress + CapPtr + 4, BIT15);
  }
  **/
}

/**
    Program the related registers for WOL enabled.

    @param VOID

    @retval VOID

**/
VOID SbEnableWolPmConfg (
    VOID
)
{
  /** PORTING REQUIRED **/
}

/**
    Program the related registers for WOL disabled.

    @param VOID

    @retval VOID

**/
VOID SbDisableWolPmConfg (
    VOID
)
{
  /** PORTING REQUIRED **/
}

/**
    Get I/O Trap Info

    @param TrappedIoData Pointer to a 32bit data buffer for receiving the
                         data of I/O trap information.

    @return TrappedIoData Pointer to a 32bit data buffer contained the
                          data of I/O trap information.
            I/O Trap address

**/
UINT16 SbGetIoTrapInfo (
    IN OUT UINT32   *TrappedIoData 
)
{
  /** PORTING REQUIRED **
  if (READ_MEM16_RCRB (R_PCH_RCRB_TRCR) & B_PCH_RCRB_TRCR_RWI)
    //  Trapped cycle was a read cycle
    *TrappedIoData = 0;
  else
    //  Trapped cycle was a read cycle
    *TrappedIoData = READ_MEM32_RCRB (R_PCH_RCRB_TRWDR);

  // Return Trapped I/O Address
  return READ_MEM16_RCRB(R_PCH_RCRB_TRCR) & B_PCH_RCRB_TRCR_TIOA;
  **/
  return  0xFFFF;
}

/**
    Get ACPI Base Address

    @param VOID

    @return The address of ACPI Base.

**/
UINT16 SbGetAcpiBaseAddress (
    VOID
)
{
  return PcdGet16 (PcdAcpiIoPortBaseAddress);
}

/**
    Get PM1 Control Register Offset

    @param VOID

    @return The offset of PM1 Control Register

**/
UINT16 SbGetPm1CntOffset (
    VOID
)
{
  /** PORTING REQUIRED **
  return SbGetAcpiBaseAddress() + R_PCH_ACPI_PM1_CNT;
  **/
  return  0xFFFF;
}

/**
    Get Intel RCBB Address (Intel Chipset Only)

    @param VOID

    @return The address of RCRB

**/
UINT32 SbGetRcrbAddress (
    VOID
)
{
  return PcdGet32 (PcdRcrbBaseAddress);
}

/**
    Check RTC power status

    @param VOID

    @retval TRUE:  Valid
            FALSE: Invalid

**/
BOOLEAN SbIsRtcPwrValid (
    VOID
)
{
  /** PORTING REQUIRED **
  if (READ_PCI8_SB (R_PCH_LPC_GEN_PMCON_3) & B_PCH_LPC_GEN_PMCON_RTC_PWR_STS)
    return FALSE;
  else
    return TRUE;
  **/
  return TRUE;
}

/**
    Get the enable bit setting in GPE0 per Gpe0Type

    @param Gpe0Type	SB_GPE0_TYPE, like TYPE_HOT_PLUG .. etc.

    @retval TRUE = Enabled / FALSE = Disabled

**/
BOOLEAN SbGetGpe0En (
    IN SB_GPE0_TYPE Gpe0Type
)
{
  /** PORTING REQUIRED **/
  return FALSE;
}

/**
    Set the enable bit in GPE0 per Gpe0Type

    @param Gpe0Type	SB_GPE0_TYPE, like TYPE_HOT_PLUG .. etc.

    @retval VOID

**/
VOID SbSetGpe0En (
    IN SB_GPE0_TYPE Gpe0Type
)
{
  /** PORTING REQUIRED **/
}

/**
    Reset the enable bit in GPE0 per Gpe0Type

    @param Gpe0Type	SB_GPE0_TYPE, like TYPE_HOT_PLUG .. etc.

    @retval VOID

**/
VOID SbResetGpe0En (
    IN SB_GPE0_TYPE Gpe0Type
)
{
  /** PORTING REQUIRED **/
}

/**
    Save/Restore/ClearAll GPE0 register per Operation

    @param Operation	GPE0_EN_OP. GPE0_EN_OP_SAVE, GPE0_EN_OP_RESTORE,
                      GPE0_EN_OP_CLEAR_ALL.

    @retval VOID

**/
VOID SbGpe0Operation (
    IN GPE0_EN_OP   Operation
)
{
  /** PORTING REQUIRED **/
}

/**
    Get the status bit setting in GPE0 per GPE0 Type

    @param Gpe0Type	SB_GPE0_TYPE, like TYPE_HOT_PLUG .. etc.

    @retval TRUE = Set / FALSE= Clear

**/
BOOLEAN SbGetGpe0Sts (
    IN SB_GPE0_TYPE Gpe0Type
)
{
  /** PORTING REQUIRED **/
  return FALSE;
}

/**
    Clear the status bit in GPE0 per GPE0 Type

    @param Gpe0Type	SB_GPE0_TYPE, like TYPE_HOT_PLUG .. etc.

    @retval VOID 

**/
VOID SbClearGpe0Sts (
    IN SB_GPE0_TYPE Gpe0Type
)
{
  /** PORTING REQUIRED **/
}

/**
    Set GPI Pin Enable bit in Gpe0

    @param Gpio - Define the group and Pin# of the GPIO

    @retval VOID

**/
VOID SbSetGpe0GpinEn (
    IN AMI_OEM_GPIO Gpio
)
{
  /** PORTING REQUIRED **/
}

/**
    Get GPI Pin Enable bit setting in Gpe0

    @param Gpio - Define the group and Pin# of the GPIO

    @retval TRUE = Enabled / FALSE= Disabled

**/
BOOLEAN SbGetGpe0GpinEn (
    IN AMI_OEM_GPIO Gpio
)
{
  /** PORTING REQUIRED **/
  return FALSE;
}

/**
    Reset GPI Pin Enable bit in Gpe0

    @param Gpio - Define the group and Pin# of the GPIO

    @retval VOID

**/
VOID SbResetGpe0GpinEn (
    IN AMI_OEM_GPIO Gpio
)
{
  /** PORTING REQUIRED **/
}

/**
    Get GPI Pin Status bit setting in Gpe0

    @param Gpio - Define the group and Pin# of the GPIO

    @retval VOID

**/
BOOLEAN SbGetGpe0GpinSts (
    IN AMI_OEM_GPIO Gpio
)
{
  /** PORTING REQUIRED **/
  return FALSE;
}

/**
    Clear GPI Pin Status bit in Gpe0

    @param Gpio - Define the group and Pin# of the GPIO

    @retval VOID

**/
VOID SbClearGpe0GpinSts (
    IN AMI_OEM_GPIO Gpio
)
{
  /** PORTING REQUIRED **/
}

/**
    Get GPIO Use Setting

    @param Gpio - Define the group and Pin# of the GPIO

    @retval TRUE = GPIO MODE / FALSE= NATIVE MODE

**/
BOOLEAN SbGetGpioUseSel (
    IN AMI_OEM_GPIO Gpio
)
{
  /** PORTING REQUIRED **
  UINT16  GpioBase;
  UINTN   UseRegister;
  UINT32  BitNum;
  UINT32  Buffer32;

  GpioBase = PcdGet16 (PcdGpioPortBaseAddress);
  
  if (Gpio.PinNum > 31) {
    UseRegister = R_PCH_GPIO_USE_SEL2;
    BitNum = Gpio.PinNum - 32;
  } else {
    UseRegister = R_PCH_GPIO_USE_SEL;
    BitNum = Gpio.PinNum;
  }
  
  Buffer32 = IoRead32 (GpioBase + UseRegister);
  
  if (Buffer32 & (UINT32) (1 << Gpio.PinNum)) {
    return TRUE;
  } else {
    return FALSE;
  }
  **/
  return FALSE;
}

/**
    Program GPIO Use Setting

    @param Gpio - Define the group and Pin# of the GPIO
    @param GpioMode	- TRUE = GPIO MODE / FALSE= NATIVE MODE
    @param MulFunc - While this pin is a multi-function pin and GpioMode
                     is Native Mode, BIOS will refer to MulFunc to
                     determine which native function this pin will be
                     used.

    @retval VOID

**/
VOID SbProgGpioUseSel (
    IN AMI_OEM_GPIO Gpio,
    IN BOOLEAN      GpioMode,
    IN UINT8        MulFunc
)
{
  /** PORTING REQUIRED **
  UINT16  GpioBase;
  UINTN   UseRegister;
  UINT32  BitNum;
  UINT32  Buffer32;

  GpioBase = PcdGet16 (PcdGpioPortBaseAddress);
  
  if (Gpio.PinNum > 31) {
    UseRegister = R_PCH_GPIO_USE_SEL2;
    BitNum = Gpio.PinNum - 32;
  } else {
    UseRegister = R_PCH_GPIO_USE_SEL;
    BitNum = Gpio.PinNum;
  }
  
  Buffer32 = IoRead32 (GpioBase + UseRegister);
  if (GpioMode) {
    Buffer32 |= (UINT32) (1 << Gpio.PinNum);
  } else {
    Buffer32 &= (UINT32) ~(1 << Gpio.PinNum);
  }
  IoWrite32 (GpioBase + UseRegister, Buffer32);
  **/
}

/**
    Get GPIO I/O Setting

    @param Gpio - Define the group and Pin# of the GPIO

    @retval TRUE : Input Mode / FALSE : Output Mode

**/
BOOLEAN SbGetGpioIoSel (
    IN AMI_OEM_GPIO Gpio
)
{
  /** PORTING REQUIRED **
  UINT16  GpioBase;
  UINTN   IoRegister;
  UINT32  BitNum;
  UINT32  Buffer32;

  GpioBase = PcdGet16 (PcdGpioPortBaseAddress);
  
  if (Gpio.PinNum > 31) {
    IoRegister = R_PCH_GPIO_IO_SEL2;
    BitNum = Gpio.PinNum - 32;
  } else {
    IoRegister = R_PCH_GPIO_IO_SEL;
    BitNum = Gpio.PinNum;
  }
  
  Buffer32 = IoRead32 (GpioBase + IoRegister);
  
  if (Buffer32 & (UINT32) (1 << Gpio.PinNum)) {
    return TRUE;
  } else {
    return FALSE;
  }
  **/
  return FALSE;
}

/**
    Set GPIO I/O Setting

    @param Gpio - Define the group and Pin# of the GPIO
    @param InputMode - TRUE : Input Mode / FALSE : Output Mode

    @retval VOID

**/
VOID SbSetGpioIoSel (
    IN AMI_OEM_GPIO Gpio,
    IN BOOLEAN      InputMode
)
{
  /** PORTING REQUIRED **
  UINT16  GpioBase;
  UINTN   IoRegister;
  UINT32  BitNum;
  UINT32  Buffer32;

  GpioBase = PcdGet16 (PcdGpioPortBaseAddress);
  
  if (Gpio.PinNum > 31) {
    IoRegister = R_PCH_GPIO_IO_SEL2;
    BitNum = Gpio.PinNum - 32;
  } else {
    IoRegister = R_PCH_GPIO_IO_SEL;
    BitNum = Gpio.PinNum;
  }
  
  Buffer32 = IoRead32 (GpioBase + IoRegister);
  if (InputMode) {
    Buffer32 |= (UINT32) (1 << Gpio.PinNum);
  } else {
    Buffer32 &= (UINT32) ~(1 << Gpio.PinNum);
  }
  IoWrite32 (GpioBase + IoRegister, Buffer32);
  **/
}

/**
    Get GPIO Level Setting

    @param Gpio - Define the group and Pin# of the GPIO

    @retval TRUE: High level / FALSE: Low level

**/
BOOLEAN SbGetGpioLvlSel (
    IN AMI_OEM_GPIO Gpio
)
{
  /** PORTING REQUIRED **
  UINT16  GpioBase;
  UINTN   LvlRegister;
  UINT32  BitNum;
  UINT32  Buffer32;

  GpioBase = PcdGet16 (PcdGpioPortBaseAddress);
  
  if (Gpio.PinNum > 31) {
    LvlRegister = R_PCH_GPIO_LVL2;
    BitNum = Gpio.PinNum - 32;
  } else {
    LvlRegister = R_PCH_GPIO_LVL;
    BitNum = Gpio.PinNum;
  }
  
  Buffer32 = IoRead32 (GpioBase + LvlRegister);
  
  if (Buffer32 & (UINT32) (1 << Gpio.PinNum)) {
    return TRUE;
  } else {
    return FALSE;
  }
  **/
  return FALSE;
}

/**
    Set GPIO Level

    @param Gpio - Define the group and Pin# of the GPIO
    @param High - TRUE: Set to High level / FALSE: Set to Low level

    @retval VOID

**/
VOID SbSetGpioLvlSel (
    IN AMI_OEM_GPIO Gpio,
    IN BOOLEAN      High
)
{
  /** PORTING REQUIRED **
  UINT16  GpioBase;
  UINTN   LvlRegister;
  UINT32  BitNum;
  UINT32  Buffer32;

  GpioBase = PcdGet16 (PcdGpioPortBaseAddress);
  
  if (Gpio.PinNum > 31) {
    LvlRegister = R_PCH_GPIO_LVL2;
    BitNum = Gpio.PinNum - 32;
  } else {
    LvlRegister = R_PCH_GPIO_LVL;
    BitNum = Gpio.PinNum;
  }
  
  Buffer32 = IoRead32 (GpioBase + LvlRegister);
  if (High) {
    Buffer32 |= (UINT32) (1 << Gpio.PinNum);
  } else {
    Buffer32 &= (UINT32) ~(1 << Gpio.PinNum);
  }
  IoWrite32 (GpioBase + LvlRegister, Buffer32);
  **/
}

/**
    Read Alternate GPI SMI Status Register

    @param VOID

    @return The value read from Alternate GPI SMI Status Register

**/
UINT32 SbReadAltGpiSmiSts (
    VOID
)
{
  /** PORTING REQUIRED **
  UINT16  PmBase;

  PmBase = PcdGet16 (PcdAcpiIoPortBaseAddress);
  return IoRead16 (PmBase + R_PCH_ALT_GP_SMI_STS);
  **/
  return 0;
}

/**
    Clear Alternate GPI SMI Status

    @param Gpio - Define the group and Pin# of the GPIO

    @retval VOID

**/
VOID SbClearAltGpiSmiSts (
    IN AMI_OEM_GPIO Gpio
)
{
  /** PORTING REQUIRED **
  UINT16  PmBase;

  PmBase = PcdGet16 (PcdAcpiIoPortBaseAddress);
  IoWrite16 (PmBase + R_PCH_ALT_GP_SMI_STS, (1 << Gpio.PinNum));
  **/
}

/**
    Program Alternate GPI SMI Register

    @param Gpio - Define the group and Pin# of the GPIO
    @param Set - TRUE: Set / FALSE: Clear corresponding Alternate GPI SMI
                 Enable bit.

    @retval VOID

**/
VOID SbProgramAltGpiSmi (
    IN AMI_OEM_GPIO Gpio,
    IN BOOLEAN      Set
)
{
  /** PORTING REQUIRED **
  UINT16  Buffer16;

  Buffer16 = SbReadAltGpiSmi();

  if (Set) {
    Buffer16 |= (1 << Gpio.PinNum);
  } else {
    Buffer16 &= ~(1 << Gpio.PinNum);
  }

  SbWriteAltGpiSmi (Buffer16);
  **/
}

/**
    Program GPIO Rout

    @param Gpio - Define the group and Pin# of the GPIO
    @param Mode - NO_EFFECT/SMI_MODE/SCI_MODE/NMI_MODE

    @retval VOID

**/
VOID SbProgramGpioRout (
    IN AMI_OEM_GPIO Gpio,
    IN UINT8        Mode
)
{
  /** PORTING REQUIRED **
  UINT32      GpioRout;

  GpioRout = SbReadGpioRout (Gpio.PinNum);
  GpioRout &= ~(0x03 << (Gpio.PinNum * 2));
  
  if (Mode == 0) // No effect
    GpioRout |= (0x00 << (Gpio.PinNum * 2));
  if (Mode == 1) // SMI#
    GpioRout |= (0x01 << (Gpio.PinNum * 2));
  if (Mode == 2) // SCI#
    GpioRout |= (0x02 << (Gpio.PinNum * 2));
  if (Mode == 3) // NMI
    GpioRout |= (0x03 << (Gpio.PinNum * 2));

  SbWriteGpioRout (Gpio.PinNum, GpioRout);
  **/
}

/**
    Program GPIO Register

    @param Gpio - Define the group and Pin# of the GPIO
    @param RegType - The register type which is going to read
    @param AndData - The value to AND with read value from the GPIO register.
    @param OrData - The value to OR with the result of the AND operation.

    @retval VOID

**/
VOID SbProgramGpioRegister (
    IN AMI_OEM_GPIO   Gpio,
    IN GPIO_REG_TYPE  RegType,
    IN UINT32         AndData,
    IN UINT32         OrData
)
{
  /** PORTING REQUIRED **
  UINTN       IoBaseAddress;
  
  switch (Gpio.Group) {
    case GPIO_SC:
      IoBaseAddress = 0xFED0C000;
      break;
    case GPIO_NC:
      IoBaseAddress = 0xFED0D000;
      break;
    case GPIO_SUS:
      IoBaseAddress = 0xFED0E000;
      break;
    default: 
      return;
  }

  MmioAndThenOr32(IoBaseAddress + Gpio.PinNum * 0x10 + RegType * 4, AndData, OrData);
  **/
}

/**
    Read GPIO Register

    @param Gpio - Define the group and Pin# of the GPIO
    @param RegType - The register type which is going to read

    @retval 0xFFFFFFFF - Failed to read the register
    @retval Others - The data read from the register

**/
UINT32 SbReadGpioRegister (
    IN AMI_OEM_GPIO   Gpio,
    IN GPIO_REG_TYPE  RegType
)
{
  /** PORTING REQUIRED **
  UINTN       IoBaseAddress;
  
  switch (Gpio.Group) {
    case GPIO_SC:
      IoBaseAddress = 0xFED0C000;
      break;
    case GPIO_NC:
      IoBaseAddress = 0xFED0D000;
      break;
    case GPIO_SUS:
      IoBaseAddress = 0xFED0E000;
      break;
    default: 
      return 0xFFFFFFFF;
  }

  return MmioRead32(IoBaseAddress + Gpio.PinNum * 0x10 + RegType * 4);
  **/
  return 0xFFFFFFFF;
}

/**
    Get South Bridge Miscellaneous BIT Status

    @param SbMiscType   Please check SB_MISC_TYPE for the details
    @param BitStatus    The value of the Miscellaneous BIT

    @retval EFI_UNSUPPORTED   This Miscellaneous BIT is not supported
    @retval EFI_SUCCESS       Success to get the value of the Miscellaneous BIT

**/
EFI_STATUS SbGetMiscBitStatus(
    IN SB_MISC_TYPE         SbMiscType,
    IN OUT UINT8            *BitStatus
)
{
/**** PORTING REQUIRED ****
    // Please check the datasheet of the chipset for the register to know
    // how to get the value of the Miscellaneous BIT indicated by SbMiscType.
    // If the type is not supported by the chip, please return EFI_UNSUPPORTED.
    switch (SbMiscType) {
      case CPU_THERMAL_TRIP_STATUS:
      case AFTERG3_EN:
      case PWR_FLR:
        return EFI_SUCCESS;
        break;
      default: 
        break;
    }
**** PORTING REQUIRED ****/    
    return EFI_UNSUPPORTED;
}

/**
    Program South Bridge Miscellaneous BIT

    @param SbMiscType   Please check SB_MISC_TYPE for the details
    @param Set          Set/Clear the value of the Miscellaneous BIT

    @retval EFI_UNSUPPORTED   This Miscellaneous BIT is not supported
    @retval EFI_SUCCESS       Success to Set/Clear the value of the Miscellaneous BIT

**/
EFI_STATUS SbProgramMiscBit(
    IN SB_MISC_TYPE         SbMiscType,
    IN BOOLEAN              Set
)
{
/**** PORTING REQUIRED ****
    // Please check the datasheet of the chipset for the register to know
    // how to program the value of the Miscellaneous BIT indicated by SbMiscType.
    // If the type is not supported by the chip, please return EFI_UNSUPPORTED.
    switch (SbMiscType) {
      case CPU_THERMAL_TRIP_STATUS:
      case AFTERG3_EN:
      case PWR_FLR:
        return EFI_SUCCESS;
        break;
      default: 
        break;
    }
**** PORTING REQUIRED ****/
    return EFI_UNSUPPORTED;
}

/**
    Check if any USB port is routed to XHCI controller

    @param VOID

    @return TRUE  There is
    @return FALSE There is not

**/
BOOLEAN SbIsXhciRouting ( 
    VOID 
)
{
/**** PORTING REQUIRED ****
    Please check the datasheet of the chipset for the register to know
    if any USB port is routed to XHCI controller.
**** PORTING REQUIRED ****/
    return  TRUE;
}

/**
    Get the Mac Address of the LAN inside the chipset

    @param  MacAddress        Pointer to the buffer used to store the Mac Address

    @retval EFI_UNSUPPORTED   This function is not supported
    @retval EFI_SUCCESS       Success to get the Mac Address

**/
EFI_STATUS SbGetChipLanMacAddress ( 
    IN OUT UINT8            *MacAddress
)
{
/**** PORTING REQUIRED ****
    Please check the datasheet of the chipset for the register to know
    how to get the Mac Address of the LAN inside the chipset.
    For Intel chip, you also can get MAC address from Gbe Region of 
    the flash.
    If it is not supported by the chip or there is no LAN inside the 
    chip, please return EFI_UNSUPPORTED.
**** PORTING REQUIRED ****/
    return EFI_UNSUPPORTED;
}

/**
    Read SPI Descriptor

    @param  SectionSelect     Data written to Flash Descriptor Section Select bits
    @param  SectionIndex      Data written to Flash Descriptor Section Index bits
    @param  *SectionData      Pointer to Flash Descriptor Section Data

    @retval EFI_UNSUPPORTED   This function is not supported
    @retval EFI_SUCCESS       Success to read SPI Descriptor
    @retval EFI_DEVICE_ERROR  Failed to read SPI Descriptor
    @retval EFI_NOT_FOUND     Invalid Flash Valid Signature

**/
EFI_STATUS ReadSpiDescriptor (
    IN UINT8                SectionSelect,
    IN UINT8                SectionIndex,
    OUT UINT32              *SectionData
)
{
/**** PORTING REQUIRED ****
    // Please check the datasheet of the chipset for the register to know
    // how to read SPI Descriptor region.
    // If Descriptor region is not supported by the chip, please return EFI_UNSUPPORTED.

    //
    // Check if it is valid to use hardware sequencing regs
    //
    if ((MmioRead16 (SB_RCBA + SPI_BASE_ADDRESS + R_SB_SPI_HSFS) & B_SB_SPI_HSFS_FDV) == B_SB_SPI_HSFS_FDV) {
      //
      // Check Flash Valid Signature
      //
      MmioAndThenOr32 (
        SB_RCBA + SPI_BASE_ADDRESS + R_SB_SPI_FDOC,
        (UINT32) (~(B_SB_SPI_FDOC_FDSS_MASK | B_SB_SPI_FDOC_FDSI_MASK)),
        (UINT32) (V_SB_SPI_FDOC_FDSS_FSDM | R_SB_SPI_FDBAR_FLVALSIG)
        );
      if ((MmioRead32 (SB_RCBA + SPI_BASE_ADDRESS + R_SB_SPI_FDOD)) == SPI_FDBAR_FLVALSIG) {
        MmioAndThenOr32 (
          SB_RCBA + SPI_BASE_ADDRESS + R_SB_SPI_FDOC,
          (UINT32) (~(B_SB_SPI_FDOC_FDSS_MASK | B_SB_SPI_FDOC_FDSI_MASK)),
          ((UINT32)SectionSelect << 12) | ((UINT32)SectionIndex << 2)
          );
        *SectionData = MmioRead32 (SB_RCBA + SPI_BASE_ADDRESS + R_SB_SPI_FDOD);
        return EFI_SUCCESS;
      } else {
        return EFI_NOT_FOUND;
      }
    } else {
      return EFI_DEVICE_ERROR;
    }
**** PORTING REQUIRED ****/
    return EFI_UNSUPPORTED;
}

#if defined(AMI_COMPATIBILITY_PKG_VERSION)&&(AMI_COMPATIBILITY_PKG_VERSION > 18)
#if KBC_SUPPORT
/**
    This function detects if the Ctrl + Home key press has been made.
    Is function is only included if KBC_SUPPORT is enabled.  It enables
    the keyboard checks for the Ctrl + Home key press and then disables
    the keyboard

        
    @param PeiServices pointer to the PeiServices Table

    @retval TRUE - Ctrl + Home was pressed
            FALSE - Ctrl + Home not detected

    @note  Please make sure the following items have been done before
           calling IsCtrlHomePressed.
           - The decode of Port 60/64 has been enabled.
           - KBC controller has been enabled in SIO/EC device.
  
**/
BOOLEAN IsCtrlHomePressed (
    EFI_PEI_SERVICES **PeiServices
)
{
  UINT8               Index;
  UINT8               KeyValue = 0;
  UINT8               KeyScan = 0;
  UINT8               LoopCount = CTRL_HOME_CHECK_LOOP;
  UINT8               KbcSts = 0;
  UINT8               KbcSts1 = 0;
  UINT8               KbcSts2 = 0;
  BOOLEAN             KbcOBF = TRUE;
  BOOLEAN             KbcIBF = TRUE;
  BOOLEAN             KbcIBF1 = TRUE;
  UINT8               KbcRes = 0;

  //
  // Enable KBD
  //
  KbcSts = IoRead8 (0x64);
  //
  // Check KeyBoard Presence
  //
  if (KbcSts != 0xFF) {
    //
    // Save the Sys Bit for later use
    //
    KbcRes = KbcSts & 0x04;
    IoWrite8 (0x64, 0x60);  
    //
    // Wait for Input Buffer Empty
    //
    while(KbcIBF) {
      KbcIBF = FALSE;
      KbcSts1 = IoRead8 (0x64);
      if ((KbcSts1 & 0x02) == 0x02) {
        KbcIBF = TRUE;
      }
    }
    //
    // Keyboard enable, Mouse disable
    //
    IoWrite8 (0x60, KbcRes | 0x61);          
    //  
    // Wait for Input Buffer Empty
    //
    while(KbcIBF1) {
      KbcIBF1 = FALSE;
      KbcSts2 = IoRead8 (0x64);
      if ((KbcSts2 & 0x02) == 0x02) {
        KbcIBF1 = TRUE;
      }
    } 
  }  
  //
  // Loop for Key scan
  //
  for (Index = 0 ; Index < LoopCount ; Index++) {
    //
    // Wait at least for 6ms
    //
    CountTime(6000, PM_BASE_ADDRESS);
    //
    // Read Key stroke
    //
    if (IoRead8 (0x64) & 0x01) {
      KeyValue = IoRead8 (0x60);
    }
    //
    // Check for valid key
    //
    if (KeyValue == 0x1d) {
      //
      // Set BIT0 if it is CTRL key
      //
      KeyScan |= (1 << 0);
    }
    
    if (KeyValue == 0x47) {
      //
      // Set BIT1 if it is HOME key
      //
      KeyScan |= (1 << 1);
    }
    //
    // Check for CTRL_HOME Combination
    //
    if ((KeyScan & 0x03) == 0x03) {
      //
      // Disable KeyBoard Controller
      //
      IoWrite8 (0x64, 0x60);
      IoWrite8 (0x60, 0x30);
      return TRUE;
    }
  } // for (Index = 0 ; Index < LoopCount ; Index++)
  //
  // Disable KeyBoard Controller
  //
  IoWrite8 (0x64, 0x60);
  IoWrite8 (0x60, 0x30);
  //
  // No valid key is pressed
  //
  return FALSE;
}
#endif        // KBC_SUPPORT
#endif        // AMI_COMPATIBILITY_PKG_VERSION

/**
    Get the SPI region base, based on the enum type.
        
    @param RegionType - Region type to query for the base address
    @param LimitAddress - This address is left shifted by 12 bits to 
                          represent bits 26:12 for the Region 'n' Limit.
    @param BaseAddress - This address is left shifted by 12 bits to 
                         represent bits 26:12 for the Region 'n' Base.

    @retval EFI_SUCCESS - Read success
            EFI_INVALID_PARAMETER - Invalid region type given

**/
/*
EFI_STATUS
EFIAPI
GetSpiRegionAddresses (
  IN   AMI_PCH_SPI_REGION_TYPE  RegionType,
  OUT  UINT32                   *LimitAddress,
  OUT  UINT32                   *BaseAddress
  )
{
    BOOLEAN               AdjustRange;
    UINTN                 RegionTypeReg;
    UINT32                ReadValue;
    UINT32                RegionRangeBase;

    if (!((RegionType >= AmiUndefinedType) && (RegionType <= AmiTxeType))) {
      return EFI_INVALID_PARAMETER;
    }

    AdjustRange = FALSE;

    if (RegionType == AmiUndefinedType) {
      return EFI_SUCCESS;
    } else if (RegionType == AmiDescriptorType) {
      RegionTypeReg = R_SB_SPI_FREG0_FLASHD;
    } else if (RegionType == AmiBiosType) {
      AdjustRange = TRUE;
      RegionTypeReg = R_SB_SPI_FREG1_BIOS;
    } else if (RegionType == AmiTxeType) {
      RegionTypeReg = R_SB_SPI_FREG2_SEC;
    }

    ReadValue = MmioRead32 (SPI_BASE_ADDRESS + RegionTypeReg);
    RegionRangeBase  = (ReadValue & B_SB_SPI_FREGX_BASE_MASK) << 12;

    if (AdjustRange == TRUE) {
      *LimitAddress += RegionRangeBase;
      *BaseAddress  += RegionRangeBase;
    } else {
      *LimitAddress = (ReadValue & B_SB_SPI_FREGX_LIMIT_MASK) >> 4;
      *BaseAddress  = RegionRangeBase;
    }
    
    return EFI_SUCCESS;
}
*/

/**
    Get total Flash ROM size by reading SPI Descriptor.
        
    @param VOID

    @return Flash ROM Size

**/
/*
UINT32 GetTotalFlashRomSize ( VOID )
{
    UINT32  TotalRomSize = 0;
    UINT32  CtrlData;
    UINT8   NumSpi = 0;

    //
    // Select to Flash Map 0 Register to get the number of flash Component
    //
    CtrlData = MmioRead32((SPI_BASE_ADDRESS + R_SB_SPI_FDOC));
    CtrlData &= (UINT32)(~(B_SB_SPI_FDOC_FDSS_MASK | B_SB_SPI_FDOC_FDSI_MASK));
    CtrlData |= (UINT32)(V_SB_SPI_FDOC_FDSS_FSDM | R_SB_SPI_FDBAR_FLASH_MAP0);
    MmioWrite32((SPI_BASE_ADDRESS + R_SB_SPI_FDOC), CtrlData);

    switch ( MmioRead16 (SPI_BASE_ADDRESS + R_SB_SPI_FDOD) & B_SB_SPI_FDBAR_NC ) {
        case V_SB_SPI_FDBAR_NC_1:
            NumSpi = 1;
            break;
        case V_SB_SPI_FDBAR_NC_2:
            NumSpi = 2;
            break;
        default:
            break;
    }

    if (NumSpi == 0) return TotalRomSize;

    //
    // Select to Flash Components Register to get the Component 1 Density
    //
    CtrlData = MmioRead32((SPI_BASE_ADDRESS + R_SB_SPI_FDOC));
    CtrlData &= (UINT32)(~(B_SB_SPI_FDOC_FDSS_MASK | B_SB_SPI_FDOC_FDSI_MASK));
    CtrlData |= (UINT32)(V_SB_SPI_FDOC_FDSS_COMP | R_SB_SPI_FCBA_FLCOMP);
    MmioWrite32((SPI_BASE_ADDRESS + R_SB_SPI_FDOC), CtrlData);

    //
    //  Get Component 1 Density
    //
    switch ( (UINT8) MmioRead32 (SPI_BASE_ADDRESS + R_SB_SPI_FDOD) & B_SB_SPI_FLCOMP_COMP1_MASK ) {
        case V_SB_SPI_FLCOMP_COMP1_512KB:
            TotalRomSize += (UINT32) (512 << KBShift);
            break;
        case V_SB_SPI_FLCOMP_COMP1_1MB:
            TotalRomSize += (UINT32) (1 << MBShift);
            break;
        case V_SB_SPI_FLCOMP_COMP1_2MB:
            TotalRomSize += (UINT32) (2 << MBShift);
            break;
        case V_SB_SPI_FLCOMP_COMP1_4MB:
            TotalRomSize += (UINT32) (4 << MBShift);
            break;
        case V_SB_SPI_FLCOMP_COMP1_8MB:
            TotalRomSize += (UINT32) (8 << MBShift);
            break;
        case V_SB_SPI_FLCOMP_COMP1_16MB:
            TotalRomSize += (UINT32) (16 << MBShift);
            break;
        default:
            break;
    } // end of switch

    //
    // Get Component 2 Density
    //
    if (NumSpi == 2) {
        switch ( (UINT8) MmioRead32 (SPI_BASE_ADDRESS + R_SB_SPI_FDOD) & B_SB_SPI_FLCOMP_COMP2_MASK ) {
            case V_SB_SPI_FLCOMP_COMP2_512KB:
                TotalRomSize += (UINT32) (512 << KBShift);
                break;
            case V_SB_SPI_FLCOMP_COMP2_1MB:
                TotalRomSize += (UINT32) (1 << MBShift);
                break;
            case V_SB_SPI_FLCOMP_COMP2_2MB:
                TotalRomSize += (UINT32) (2 << MBShift);
                break;
            case V_SB_SPI_FLCOMP_COMP2_4MB:
                TotalRomSize += (UINT32) (4 << MBShift);
                break;
            case V_SB_SPI_FLCOMP_COMP2_8MB:
                TotalRomSize += (UINT32) (8 << MBShift);
                break;
            case V_SB_SPI_FLCOMP_COMP2_16MB:
                TotalRomSize += (UINT32) (16 << MBShift);
                break;
            default:
                break;
        } // end of switch
    }// end of if

    return TotalRomSize;
}
*/

/**
    This routine provides H/W read/write-protection of the BIOS region.
    If chipset supports "SPI Flash Protected Range Registers", then program
    them to protect BIOS region in SPI Flash.
        
    @param VOID

    @retval EFI_STATUS
            EFI_SUCCESS - Set successfully.
            EFI_INVALID_PARAMETER - The parameter of input is invalid
            EFI_UNSUPPORTED - Chipset or H/W is not supported.

    Note:   Porting required

**/
EFI_STATUS SbFlashProtectedRange ( VOID )
{
    return EFI_UNSUPPORTED;
/*
    AMI_SPI_PROTECTED_RANGE_CONIFG  SpiProtectedRange[] = { SPI_PROTECTED_RANGE_0,
                                                            SPI_PROTECTED_RANGE_1,
                                                            SPI_PROTECTED_RANGE_2,
                                                            SPI_PROTECTED_RANGE_3,
                                                            SPI_PROTECTED_RANGE_4 };
    AMI_PCH_SPI_REGION_TYPE         AmiSpiRegionType;
    BOOLEAN                         WriteProtectionEnable;
    BOOLEAN                         ReadProtectionEnable;
    UINT32                          ProtectedRangeBase;
    UINT32                          ProtectedRangeLimit;
    UINT32                          RegIndex;
    UINT32                          TotalRomSize;
    volatile UINT32                 Value32;

    //
    // Get Total ROM size first
    //
    TotalRomSize = GetTotalFlashRomSize();

    if (TotalRomSize == 0) return EFI_INVALID_PARAMETER;

    //
    // Program Protected Range Registers per SPI_PROTECTED_RANGE_X tokens
    //
    for (RegIndex = 0; RegIndex < 5; RegIndex++) {
      AmiSpiRegionType      = SpiProtectedRange[RegIndex].AmiPchSpiRegionType;
      WriteProtectionEnable = SpiProtectedRange[RegIndex].WriteProtectionEnable;
      ReadProtectionEnable  = SpiProtectedRange[RegIndex].ReadProtectionEnable;
      ProtectedRangeBase    = SpiProtectedRange[RegIndex].ProtectedRangeBase;
      ProtectedRangeLimit   = SpiProtectedRange[RegIndex].ProtectedRangeBase + SpiProtectedRange[RegIndex].ProtectedRangeLength - 1;
      
      if ((WriteProtectionEnable == 0) && (ReadProtectionEnable == 0)) continue;
      
      //
      // Get RangeBase and RangeLimit per SpiRegion
      //
      GetSpiRegionAddresses (AmiSpiRegionType, &ProtectedRangeBase, &ProtectedRangeLimit);
      //
      // Exceed the address of protected range base.
      //
      if (ProtectedRangeBase >= TotalRomSize) continue;
      //
      // Exceed the address of protected range limit.
      //
      if (ProtectedRangeLimit >= TotalRomSize) continue;

      Value32 = *(UINT32 *)(SPI_BASE_ADDRESS + R_SB_SPI_PR0 + (RegIndex * 4));
      if (Value32 == 0) {
        Value32 = (ProtectedRangeBase & 0x1FFF000) >> 12;
        Value32 += ((ProtectedRangeLimit & 0x1FFF000) << 4);
        if (ReadProtectionEnable) Value32 |= B_SB_SPI_PRx_RPE;
        if (WriteProtectionEnable) Value32 |= B_SB_SPI_PRx_WPE;
        *(UINT32 *)(SPI_BASE_ADDRESS + R_SB_SPI_PR0 + (RegIndex * 4)) = Value32;
      } // if register is not use yet
    } // for loop

    return EFI_SUCCESS;
*/    
}

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
