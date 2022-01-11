//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************


/** @file SdioController.c
    Functions to detect the SDIO device and read/write operation

**/

//----------------------------------------------------------------------
#include "SdioController.h"

//----------------------------------------------------------------------

extern SDIO_GLOBAL_DATA *gSdioData;
DMA_SUPPORT             gSDIO_Access=PIO;

//
// Setting the clock to 400kHz
//
UINT32      gClock = DEFAULT_SD_MMC_CLOCK;

/**
    Check if any device is connected to the slot

    @param  SdioDevInfo 
    @param  Port 

    @retval EFI_STATUS

**/
EFI_STATUS
CheckDevicePresenceInController (
    IN  SDIO_DEVICE_INFO    *SdioDevInfo,
    IN  UINT8               Port
)
{

    UINT64  SdioBaseAddr = SdioDevInfo->SdioBaseAddress;
    UINT32  PresentState;
    
    //
    // Bit 16 of present state register denote card presence
    //
    PresentState=SDIO_REG32(SdioBaseAddr,PSTATE);
#if SDMMC_VERBOSE_PRINT
    DEBUG((EFI_D_VERBOSE ,"SdMmc: Host Controller Version Register=0x%x \n",SDIO_REG16(SdioBaseAddr,HCVER)));
    DEBUG((EFI_D_VERBOSE ,"SdMmc: Host Capability Lower=0x%x \n",SDIO_REG32(SdioBaseAddr,CAP)));
    DEBUG((EFI_D_VERBOSE ,"SdMmc: Host Capability Upper =0x%x\n",SDIO_REG32(SdioBaseAddr,CAP+4)));
    DEBUG((EFI_D_VERBOSE ,"SdMmc: Host Present Register=0x%x \n",PresentState));
#endif
    if(PresentState &PSTATE_CardInsert) {
        SdioDevInfo->bState=CARDSTATUS_IDLE;
        SdioDevInfo->bHostControllerVersion= (UINT8)(0xff&(SDIO_REG16(SdioBaseAddr,HCVER)));
        return EFI_SUCCESS;
    }

    return EFI_NOT_FOUND;
}


/**
    Configure the Sdio device

    @param  SdioDevInfo 

    @retval EFI_STATUS

    @note  
      1. Initialize the controller
      2. Initialize the MMC card according to Jedec eMMC Electrical Standard

**/
EFI_STATUS
ConfigureSdMmcMemoryDevice (
    IN  SDIO_DEVICE_INFO    *SdioDevInfo,
    IN  UINT8               Port
)
{

    EFI_STATUS  Status;
    
    //
    // Initialize the HOST controller
    //
    Status= ControllerInitEnvironment(SdioDevInfo,Port);
    if(EFI_ERROR(Status)) {
        return Status;
    }

    //
    // Issues  CMD55+ACMD41. The MMC will fail on CMD55 and returns EFI_NOT_FOUND
    //
    Status= SdCardInit(SdioDevInfo,Port);
    if(Status == EFI_NOT_FOUND ) {
        //
        // After SD command, MMC card will be in error state. To bring it back idle sate
        // rest the controller. Software reset will reset the power and clock.
        // 
        Status= ControllerInitEnvironment(SdioDevInfo,Port);
        if(EFI_ERROR(Status)) {
            return Status;
        }
        
        Status= MmcCardInit(SdioDevInfo,Port);
        if(EFI_ERROR(Status)) {
            SdioDevInfo->DevEntryUsed = FALSE;
            return Status;
        }
    }
#if SDMMC_VERBOSE_PRINT
    DEBUG((EFI_D_VERBOSE,"SdMmc: Device MaxLBA=%x\n",SdioDevInfo->dMaxLBA));
    DEBUG((EFI_D_VERBOSE,"SdMmc: Device BlockSize=%x\n",SdioDevInfo->wBlockSize));
    DEBUG((EFI_D_VERBOSE ,"SdMmc: Card and Host Controller clock set =%d Hz\n",gClock));
    switch(SdioDevInfo->SdVoltageWindow) {
        case 0x300000:  DEBUG((EFI_D_VERBOSE ,"SdMmc: 3.3 Voltage set \n"));
                        break;
        case 0x60000:   DEBUG((EFI_D_VERBOSE ,"SdMmc: 3.0 Voltage set \n"));
                        break;
        default:        DEBUG((EFI_D_VERBOSE ,"SdMmc: 1.8 Voltage set \n"));
                        break;
    }
    if(SdioDevInfo->VoltageSwitch) {
        DEBUG((EFI_D_VERBOSE ,"SdMmc: 1.8V switch Successful in card \n"));
    }
    
    switch(SdioDevInfo->MmcBusWidth) {
        case MMC_4_BIT_BUS_WIDTH:       DEBUG((EFI_D_VERBOSE ,"SdMmc: Bus width set to 4-bit SDR \n"));
                                        break;
        case MMC_8_BIT_BUS_WIDTH:       DEBUG((EFI_D_VERBOSE ,"SdMmc: Bus width set to 8-Bit SDR \n"));
                                        break;
        case MMC_4_BIT_DDR_BUS_WIDTH:   DEBUG((EFI_D_VERBOSE ,"SdMmc: Bus width set to 4-Bit DDR \n"));
                                        break;
        case MMC_8_BIT_DDR_BUS_WIDTH:   DEBUG((EFI_D_VERBOSE ,"SdMmc: Bus width set to 8-Bit DDR \n"));
                                        break;
        default:                        DEBUG((EFI_D_VERBOSE ,"SdMmc: Bus width set to 1-Bit SDR \n"));
                                        break;
    }
    
    DEBUG((EFI_D_VERBOSE ,"SdMmc: Bus width set to %d bit\n",SdioDevInfo->MmcBusWidth));
    
    DEBUG((EFI_D_VERBOSE ,"SdMmc: Host Clock Register =0x%x \n",SDIO_REG16(SdioDevInfo->SdioBaseAddress,CLKCTL)));
    DEBUG((EFI_D_VERBOSE ,"SdMmc: Host Power control =0x%x \n",SDIO_REG8(SdioDevInfo->SdioBaseAddress,PWRCTL)));
    DEBUG((EFI_D_VERBOSE ,"SdMmc: Host Control 1 Register=0x%x \n",SDIO_REG8(SdioDevInfo->SdioBaseAddress,HOSTCTL)));
    DEBUG((EFI_D_VERBOSE ,"SdMmc: Host Control 2 Register=0x%x \n",SDIO_REG16(SdioDevInfo->SdioBaseAddress,HOSTCTL2)));
#endif
    return Status;
}

/**
    Set the clock in the controller

    @param  SdioDevInfo 
    @param  Port 
    @param  Clock Divident

    @retval EFI_STATUS
    
    Note: This function is implement as per SD Host Controller Simplified 
          Specification Version 3.00 section 3.2.3 SD Clock Frequency Change Sequence
          Procedure:
          1. Calculate a divisor for SD clock frequency
          2. Set SDCLK Frequency Select and Internal Clock Enable
          3. Check Internal Clock Stable
          4. Set SD Clock On.
          5. If Clock is set greater than 25Mhz and upto 52 Mhz set high speed enable. 
             For MMC device upto 52 Mhz is high speed mode. So take upper limit to 52Mhz
             (According to section 2.2.10 Host Control 1 Register)

**/
EFI_STATUS
ControllerSetClock (
    IN  SDIO_DEVICE_INFO    *SdioDevInfo,
    IN  UINT8               Port,
    IN  UINT32              RequiredClock
)
{
    UINT64  SdioBaseAddr = SdioDevInfo->SdioBaseAddress;
    UINT16  ClkControl=0;
    UINT16  ClkDivident;
    UINT32  CapValue;
    UINT32  MaxHstClk=0;
    UINT32  TimeOut;
    UINT16  ClkCtl =0;
    UINT8   Temp=0;
    
    // Set UHS-I mode in Host control 2 reg if 1.8V is set and DDR is not selected.
    if( SdioDevInfo->VoltageSwitch == TRUE && 
        (SDIO_REG8(SdioBaseAddr,HOSTCTL2) & UHS_MODE_SELECT) <= SDR104_UHS_SELECT ) {
        if ( RequiredClock >= 200000000 ) {
            SDIO_REG8_OR(SdioBaseAddr,HOSTCTL2, SDR104_UHS_SELECT);
        } else if ( RequiredClock >= 100000000 ) {
#if SDMMC_VERBOSE_PRINT
            DEBUG((EFI_D_VERBOSE ,"Set SDR50 mode...\n"));
#endif
            SDIO_REG8_OR(SdioBaseAddr,HOSTCTL2, SDR50_UHS_SELECT);
        } else if ( RequiredClock >= 50000000 ) {
#if SDMMC_VERBOSE_PRINT
            DEBUG((EFI_D_VERBOSE ,"Set SDR25 mode...\n"));
#endif
            SDIO_REG8_OR(SdioBaseAddr,HOSTCTL2, SDR25_UHS_SELECT);
        } 
        MicroSecondDelay (10);
    }
    
    //
    // Clear SD Clock Enable
    //
    SDIO_WRITE_REG16(SdioBaseAddr,CLKCTL,0);
    
    CapValue=SDIO_REG32(SdioBaseAddr,CAP);
    MaxHstClk= CapValue;
    
    if(SdioDevInfo->bHostControllerVersion >= HOST_CONTROLLER_VER3) {        
        MaxHstClk &= BASE_CLOCK_VER3;
    } else {
        MaxHstClk &= BASE_CLOCK;
    }
    //
    // Calculate the Maximmum clock supported by Host
    //
    MaxHstClk = (MaxHstClk >> 8);
    if(MaxHstClk) {
        MaxHstClk = MaxHstClk * 1000000;
    }
    
    //
    // Calculate the clock divident
    //
    if(RequiredClock >= MaxHstClk) {
        ClkDivident=0;
        RequiredClock=MaxHstClk;
    } else if (SdioDevInfo->bHostControllerVersion >= HOST_CONTROLLER_VER3) {
        
        //
        // As per Section 2.2.14 Clock Control Register (Offset 02Ch) of
        // SD Host Controller Simplified Specification Version 3.00, the 
        // version 3 controller supports 10 bit Divided Clock Mode
        // 
        if(MaxHstClk%(2*RequiredClock)) {
            ClkDivident =(MaxHstClk/(2*RequiredClock))+1;
       }else{
           ClkDivident =MaxHstClk/(2*RequiredClock);
       }
        ClkCtl = ((ClkDivident>>2)&0xc0); 
    } else {
        //
        // As per Section 2.2.14 Clock Control Register (Offset 02Ch) of
        // SD Host Controller Simplified Specification Version 3.00, the 
        // version 2 and 1 controller supports 8 bit Divided Clock Mode.
        // In 8 bit Divided Clock Mode only bit can be set in the clock 
        // divident.
        // 
        do {
            Temp++;
        }while((MaxHstClk/(2*(1<<Temp))> RequiredClock) && (Temp<8));
        ClkDivident = 1<<Temp;
    }
       
    ClkDivident &= 0xff;
    ClkDivident = ClkDivident<<8; 
    ClkCtl |= ClkDivident;
    
    //
    // Set the Clock to Required clock or less. Set the frequency select and enable the internal clock
    //
    SDIO_WRITE_REG16(SdioBaseAddr,CLKCTL,ClkCtl);
    SDIO_REG16_OR(SdioBaseAddr,CLKCTL,CLKCTL_INTERNAL_CLOCK_ENABLE);
    
    TimeOut = CLOCK_INTERNAL_STABLE_TIMEOUT;
    //
    // Loop until Internal clock is stable. Timeout to avoid dead loop
    //
    do {
        ClkControl=SDIO_REG16(SdioBaseAddr,CLKCTL);
        MicroSecondDelay (1);
    } while((!(ClkControl & CLKCTL_INTERNAL_CLOCK_STABLE))&& TimeOut--);
    
    if(!TimeOut) {
        return EFI_TIMEOUT;
    }
    
    //
    // Set the SD clock on
    //
    SDIO_REG16_OR(SdioBaseAddr,CLKCTL,CLKCTL_CLOCK_ENABLE);
    
    //
    // Enable the High speed in controller
    //
    if (( CapValue & HIGH_SPEED_SUPPORT)&&((RequiredClock >= 25000000)&&(RequiredClock<=52000000))) {
        SDIO_REG8_OR(SdioBaseAddr,HOSTCTL,HIGH_SPEED_ENABLE);
    }
    
    gClock= RequiredClock;
    return EFI_SUCCESS;
}

/**
    Initialize the Controller

    @param  SdioDevInfo 
    @param  Port 

    @retval EFI_STATUS
    
    Note: Host initialization is done reffering following spec:
        1.SD Host Controller Simplified Specification Version 3.00
        2.Jedec eMMC Electrical Standard 5.01(Sec A.6.1 Bus initialization 
          Step 1 to 3)
        3.Physical Layer Simplified Specification Version 4.10

**/
EFI_STATUS
ControllerInitEnvironment (
    IN  SDIO_DEVICE_INFO    *SdioDevInfo,
    IN  UINT8               Port
)
{

    UINT64      SdioBaseAddr = SdioDevInfo->SdioBaseAddress;
    UINT32      CapValue;
    UINT8       SoftRst;
    UINT32      TimeOut= SOFTWARE_RESET_TIMEOUT;
    EFI_STATUS  Status;

    //
    // Reset the controller. The CMD0 fails if reset is not given
    //
    SDIO_WRITE_REG8(SdioBaseAddr,SWRST,RESETALL);
    do {
        SoftRst=SDIO_REG8(SdioBaseAddr,SWRST);
        MicroSecondDelay (1);
    } while((SoftRst & BIT0)&&(TimeOut--));
    if(!TimeOut) {
        return EFI_TIMEOUT;
    }
    
    //
    // Enable the interrupts 
    //
    SDIO_WRITE_REG16(SdioBaseAddr,NINTEN,0xFF);
    SDIO_WRITE_REG16(SdioBaseAddr,ERINTEN,0xFF);
    SDIO_WRITE_REG16(SdioBaseAddr,NINTSIGEN,0xFF);
    SDIO_WRITE_REG16(SdioBaseAddr,ERINTSIGEN,0xFF);
    
    //
    // Set time out control register as per sec 2.2.15 Timeout Control
    // Register from Sd Host controller spec
    //
    SDCard_SetTimeout(SdioDevInfo,Port);
    
    //
    // Set the voltage to maximum supported value as per 
    // 3.3 SD Bus Power Control from Sd Host controller spec and
    // Section A.6.1 Bus initialization of Jedec Spec
    //
    CapValue=SDIO_REG32(SdioBaseAddr,CAP);
    
    if( CapValue & VOLTAGE_SUPPORT_33 ) {  
        SDIO_WRITE_REG8(SdioBaseAddr,PWRCTL,PWRCTL_33V);
        SdioDevInfo->SdVoltageWindow = VOLTAGE_WINDOW3_3V;
    } else if (CapValue & VOLTAGE_SUPPORT_30 ) {
        SDIO_WRITE_REG8(SdioBaseAddr,PWRCTL,PWRCTL_30V);
        SdioDevInfo->SdVoltageWindow = VOLTAGE_WINDOW3_0V;
    } else { 
        SDIO_WRITE_REG8(SdioBaseAddr,PWRCTL,PWRCTL_18V);
        SdioDevInfo->SdVoltageWindow =0;
    }
    SDIO_REG8_OR(SdioBaseAddr,PWRCTL,PWRCTL_POWER_ENABLE);

    //
    // Clock must be set to 400KHz or less as per
    // Section A.6.1 Bus initialization of Jedec Spec
    // 4.2.1 Card Reset Sd physical layer spec
    //
    Status = ControllerSetClock(SdioDevInfo,Port,400000);
    if(EFI_ERROR(Status)) {
        return Status;
    }
    //
    // Delay mentioned in A.6.1 Bus initialization of JEDEC Ver5.01
    //
    MicroSecondDelay (1000);  // 1Msec

    return EFI_SUCCESS;
}

/**
    Set the SD device timeout value

    @param  SdioDevInfo 
    @param  Port 

    @retval EFI_STATUS

**/
EFI_STATUS
SDCard_SetTimeout (
    IN  SDIO_DEVICE_INFO    *SdioDevInfo,
    IN  UINT8               Port
)
{

    UINT64  SdioBaseAddr = SdioDevInfo->SdioBaseAddress;

    //
    // Set the Maximum Timeout Value. The timeout set will depend upon the 
    // Timeout Clock Frequency and Timeout Clock Unit in the capability 
    // register
    //
    SDIO_WRITE_REG8(SdioBaseAddr,TOCTL,0x0E);
    return EFI_SUCCESS;
}

/**
    Send the Command to Sd or MMC device

    @param  SdioDevInfo 
    @param  Port 
    @param  CommandIndex 
    @param  CommandArgument 

    @retval EFI_STATUS

**/
EFI_STATUS
SdMmcCommand (
    IN  SDIO_DEVICE_INFO    *SdioDevInfo,
    IN  UINT8               Port,
    IN  UINT16              CommandIndex,
    IN  UINT32              CommandArgument
)
{

    UINT64      SdioBaseAddr = SdioDevInfo->SdioBaseAddress;
    EFI_STATUS  Status;
    UINT32      PresentState;
    UINT32      Timeout;

    //
    // As per Section 3.7.1.1 The Sequence to Issue a SD Command for SD commands the
    // command inhibit status should be checked
    //
    Timeout = 2000;
    do {
        PresentState = SDIO_REG32(SdioBaseAddr,PSTATE);
        MicroSecondDelay(1);
    } while ((Timeout-- > 0) && (PresentState & BIT0));
    
    if(!Timeout) {
        return EFI_TIMEOUT;
    }
    
    Timeout = 2000;
    do {
        PresentState = SDIO_REG32(SdioBaseAddr,PSTATE);
        MicroSecondDelay(1);
    } while ((Timeout-- >0) && (PresentState & BIT1));
    
    if(!Timeout) {
        return EFI_TIMEOUT;
    }
    
    SDIO_WRITE_REG32(SdioBaseAddr,CMDARG,CommandArgument);

    SDIO_WRITE_REG16(SdioBaseAddr,CMD,CommandIndex);

    Status=SdMmcWaitCMDComplete(SdioDevInfo,Port);
    
    if((CommandIndex & RESP_REG_BITS) == RESP1B_TYPE) {
        Timeout = BUSY_BIT_TIMEOUT;
        do {
            PresentState=SDIO_REG16(SdioBaseAddr,PSTATE);
            MicroSecondDelay (1);
        } while((!(PresentState & PSTATE_DAT0))&& Timeout--);
        if(!Timeout) {
            return EFI_TIMEOUT;
        }
    }
    return Status;

}

/**
    Waits Until command complete.

    @param  SdioDevInfo 
    @param  Port 

    @retval EFI_STATUS

**/
EFI_STATUS
SdMmcWaitCMDComplete (
    IN  SDIO_DEVICE_INFO    *SdioDevInfo,
    IN  UINT8               Port
)
{

    UINT64  SdioBaseAddr = SdioDevInfo->SdioBaseAddress;
    UINT16  Temp16;
    UINT16  TempValue;
    UINT16  i;

#if SDMMC_VERBOSE_PRINT
    DEBUG((EFI_D_VERBOSE ,"SdMmc: Input- CMD=%d ",(SDIO_REG16(SdioBaseAddr,CMD)>>8)& 0x3f));
    DEBUG((EFI_D_VERBOSE ,"RespType=0x%x ",SDIO_REG16(SdioBaseAddr,CMD)&0xff));
    DEBUG((EFI_D_VERBOSE ,"Arg=0x%x \n",SDIO_REG32(SdioBaseAddr,CMDARG)));
#endif
    for(i=0;i<=COMMAND_EXECUTION_TIMEOUT;i++) {

        Temp16=SDIO_REG16(SdioBaseAddr,NINTSTS);
        if(Temp16 & NINTSTS_CC) {
#if SDMMC_VERBOSE_PRINT
            DEBUG((EFI_D_VERBOSE ,"SdMmc: Output- RESP0=0x%x",SDIO_REG32(SdioBaseAddr,RESP0)));
            if((SDIO_REG16(SdioBaseAddr,CMD)&0xff)== RESP2_TYPE) {
                DEBUG((EFI_D_VERBOSE ,"RESP1=0x%x RESP2=0x%x RESP3=0x%x",
                                    SDIO_REG32(SdioBaseAddr,RESP1),
                                    SDIO_REG32(SdioBaseAddr,RESP2),
                                    SDIO_REG32(SdioBaseAddr,RESP3)));
            }
            DEBUG((EFI_D_VERBOSE ,"\n"));
#endif
            break;
        }
        if(Temp16 & NINTSTS_EI) {
            Temp16 = SDIO_REG16(SdioBaseAddr,ERINTSTS);
#if SDMMC_VERBOSE_PRINT
            DEBUG((EFI_D_VERBOSE ,"SdMmc: ERROR- Error Reg(0x32h)=0x%x \n",Temp16));
            DEBUG((EFI_D_VERBOSE ,"Command Status Device Error\n"));
#endif
            SDIO_WRITE_REG16(SdioBaseAddr,ERINTSTS,0xFFFF);
            SDIO_WRITE_REG16(SdioBaseAddr,NINTSTS,NINTSTS_EI);
            if(Temp16 & CMD_CRC_ERROR) {
                return EFI_CRC_ERROR;
            }
            return EFI_DEVICE_ERROR;
        }

        //
        //delay 1ms delay
        //
        MicroSecondDelay (1000);
    }

    if(i>COMMAND_EXECUTION_TIMEOUT) {
#if SDMMC_VERBOSE_PRINT
        DEBUG((EFI_D_VERBOSE ,"Command Status Timeout\n"));
#endif
        return EFI_TIMEOUT;
    }

    Temp16=SDIO_REG16(SdioBaseAddr,CMD);
    TempValue=Temp16;

    //
    // If the Respose type is R1b or R5b is the command with busy signal and
    // When busy signal is de-asserted Transfer complete bit will be set.
    //
    TempValue &= (BIT0+BIT1);
    if(TempValue == (BIT0+BIT1)) {
        SDIO_WRITE_REG16(SdioBaseAddr,NINTSTS,(NINTSTS_TC | NINTSTS_CC));
    } else {
        SDIO_WRITE_REG16(SdioBaseAddr,NINTSTS,NINTSTS_CC);
    }

#if SDMMC_VERBOSE_PRINT
    DEBUG((EFI_D_VERBOSE ,"Command Status Success\n"));
#endif
    return EFI_SUCCESS;
}

/**
    Set the Clock value for the Sd device

    @param  SdioDevInfo 
    @param  Port 

    @retval EFI_STATUS
    Note : This function follows :
    1. Jedec eMMC Electrical Standard 5.01
    2. SD Host Controller Simplified Specification Version 3.00 
    3. Physical Layer Simplified Specification Version 4.10
**/
VOID
CalculateMaxCardAndHostFreq (
    IN  SDIO_DEVICE_INFO    *SdioDevInfo,
    IN  UINT8               Port,
    OUT BOOLEAN             *HighSpeedSupport,
    OUT UINT32              *MaxCardClk,
    OUT UINT32              *MaxHstClk
) {
    UINT64  SdioBaseAddr = SdioDevInfo->SdioBaseAddress;
    UINT32  CapValue=0;
    UINT8   FreqUnitIndex;
    UINT8   MultFactIndex;
    UINT16  FreqUnit[] = {1, 10, 100, 1000};
    UINT8   MultFactor[] = {10, 12, 13, 15, 20, 26, 30, \
                            35, 40, 45, 52, 55, 60, 70, 80};
    
    //
    // Read the Capability register
    //
    CapValue=SDIO_REG32(SdioBaseAddr,CAP);
    
    //
    // Check if High Speed is supported
    //
    if ( CapValue & HIGH_SPEED_SUPPORT ) {
        *HighSpeedSupport = TRUE;
    }

    if(SdioDevInfo->bHostControllerVersion >= HOST_CONTROLLER_VER3) {        
        CapValue &= BASE_CLOCK_VER3;
    } else {
        CapValue &= BASE_CLOCK;
    }

    //
    // Calculate the Maximmum clock supported by Host
    //
    CapValue = (CapValue >> 8);
    CapValue &=0xFF; 
    if(CapValue) {
        *MaxHstClk = CapValue * 1000000;
    }

    //
    // Calculate the maximum card frequency from CSD register 
    // as per 7.3.5 TRAN_SPEED [103:96] from Jedec eMMC Electrical 
    // Standard 5.01and 5.3.1 CSD_STRUCTURE TRAN_SPEED [103:96] from
    // Physical Layer Simplified Specification Version 4.10
    //
    FreqUnitIndex = SdioDevInfo->bTranSpeed & 7;
    MultFactIndex = (SdioDevInfo->bTranSpeed >> 3) & 0xf;
    *MaxCardClk = 10000 * FreqUnit[FreqUnitIndex] * MultFactor[MultFactIndex-1];
#if SDMMC_VERBOSE_PRINT
    DEBUG((EFI_D_VERBOSE ,"SdMmc: Maximum Card clock Supported =%d \n",*MaxCardClk));
    DEBUG((EFI_D_VERBOSE ,"SdMmc: Maximum Host clock Supported =%d \n",*MaxHstClk));
#endif
    
    return;
}

/**
    Get the CID register

    @param  SdioDevInfo 
    @param  Port 

    @retval EFI_STATUS
    Note: For MMC: As per step 15-16 of Sec A.6.1 Bus initialization of 
    Jedec Electrical standard 5.01 
    For SD: As per Section 4.2.3 Card Initialization and Identification
     Process from Physical Layer Simplified Specification Version 4.10
    1. Send CMD2
    2. Receive R2, and get the Device’s CID
    
**/
EFI_STATUS
SdMmcCardGetCID (
    IN  SDIO_DEVICE_INFO    *SdioDevInfo,
    IN  UINT8               Port
)
{
    UINT64      SdioBaseAddr = SdioDevInfo->SdioBaseAddress;
    UINT16      CommandIndex;
    UINT32      CommandArgument;
    EFI_STATUS  Status;
    UINT8       i;
    UINT8       Temp8;

    //
    // Issue CMD2 for CID register and put the card to identification state.
    // CID register contain the card details like manufacture ID, Product 
    // name etc.
    //
    CommandIndex=((ALL_SEND_CID_CMD2 << 8) | RESP2_TYPE);
    CommandArgument=0;
    Status=SdMmcCommand(SdioDevInfo,Port,CommandIndex,CommandArgument);
    if(EFI_ERROR(Status)) {
        return Status;
    }

    SdioDevInfo->d4CID[0]=SDIO_REG32(SdioBaseAddr,RESP0);
    SdioDevInfo->d4CID[1]=SDIO_REG32(SdioBaseAddr,RESP1);
    SdioDevInfo->d4CID[2]=SDIO_REG32(SdioBaseAddr,RESP2);
    SdioDevInfo->d4CID[3]=SDIO_REG32(SdioBaseAddr,RESP3);

    //
    // We get bits 0-119 in response, shift left by one byte.
    //
    for(i=3; i>0; i--) {
        SdioDevInfo->d4CID[i] = (SdioDevInfo->d4CID[i] << 8);
        Temp8 = (SdioDevInfo->d4CID[i-1] >> 24) & 0xff;
        SdioDevInfo->d4CID[i] += Temp8;
    }

    SdioDevInfo->d4CID[i] = (SdioDevInfo->d4CID[i] << 8);

    SdioDevInfo->bState=CARDSTATUS_IDENT;

    return EFI_SUCCESS;
}

/**
    Get the CSD from Sd device

    @param  SdioDevInfo 
    @param  Port 

    @retval EFI_STATUS
    Note: For MMC: As per step 18-20 of Sec A.6.1 Bus initialization of 
    Jedec Electrical standard 5.01.
    For SD: As per Section 4.3 Data Transfer Mode of Physical Layer 
    Simplified Specification Version 4.10

**/
EFI_STATUS
SdMmcCardGetCSD (
    IN  SDIO_DEVICE_INFO    *SdioDevInfo,
    IN  UINT8               Port
)
{

    UINT64      SdioBaseAddr = SdioDevInfo->SdioBaseAddress;
    UINT32      Temp32;
    UINT16      CommandIndex;
    UINT32      CommandArgument;
    EFI_STATUS  Status;
    UINT8       Temp8;
    UINT8       i;


    if(SdioDevInfo->bState != CARDSTATUS_STBY) {
        return EFI_DEVICE_ERROR;
    }

    //
    // Issues CMD9 and Get CSD register. The Device-Specific Data (CSD) 
    // register provides information on how to access the Device contents.
    //

    CommandIndex=((SEND_CSD_CMD9 << 8) | RESP2_TYPE);
    CommandArgument=(UINT32)SdioDevInfo->wRCA;
    CommandArgument=CommandArgument << 16;
    Status=SdMmcCommand(SdioDevInfo,Port,CommandIndex,CommandArgument);
    if(EFI_ERROR(Status)) {
        return Status;
    }

    SdioDevInfo->d4CSD[0] = SDIO_REG32(SdioBaseAddr, RESP0);
    SdioDevInfo->d4CSD[1] = SDIO_REG32(SdioBaseAddr, RESP1);
    SdioDevInfo->d4CSD[2] = SDIO_REG32(SdioBaseAddr, RESP2);
    SdioDevInfo->d4CSD[3] = SDIO_REG32(SdioBaseAddr, RESP3);
    //
    // We get bits 0-119 in response, shift left by one byte.
    //
    for(i=3; i>0; i--) {
        SdioDevInfo->d4CSD[i] = (SdioDevInfo->d4CSD[i] << 8);
        Temp8 = (SdioDevInfo->d4CSD[i-1] >> 24) & 0xff;
        SdioDevInfo->d4CSD[i] += Temp8;
    }

    SdioDevInfo->d4CSD[i] = (SdioDevInfo->d4CSD[i] << 8);

    if(SdioDevInfo->d4CSD[0] & PARTIAL_BLOCKS_FOR_WRITE_ALLOWED) {
        SdioDevInfo->bWrite_Bl_Partial=TRUE;
    } else {
        SdioDevInfo->bWrite_Bl_Partial=FALSE;
    }

    Temp32= SdioDevInfo->d4CSD[0] & MAX_WRITE_BLOCK_LENGTH;
    Temp32=Temp32 >> 22;
    SdioDevInfo->bWrite_Bl_Len=(UINT8)Temp32;

    if(SdioDevInfo->d4CSD[2] & PARTIAL_BLOCKS_FOR_READ_ALLOWED) {
        SdioDevInfo->bRead_Bl_Partial=TRUE;
    } else {
        SdioDevInfo->bRead_Bl_Partial=FALSE;
    }

    Temp32= SdioDevInfo->d4CSD[2] & MAX_READ_BLOCK_LENGTH;
    Temp32=Temp32 >> 16;
    SdioDevInfo->bRead_Bl_Len=(UINT8)Temp32;

    Temp8 = (SdioDevInfo->d4CSD[3] >> 24) & 0xff;
    SdioDevInfo->bCsdStruct = (Temp8 & 0xc0) >> 6;
    SdioDevInfo->bSpecVers = (Temp8 & 0x3c) >> 2;
    SdioDevInfo->bTranSpeed = SdioDevInfo->d4CSD[3] & 0xff;
    
    return EFI_SUCCESS;
}

/**
    This function read/write from/to the card. 

    @param  SdioDevInfo 
    @param  Port 

    @retval EFI_STATUS

**/
EFI_STATUS
SdMmcReadWriteData (
    IN  SDIO_DEVICE_INFO *SdioDevInfo,
    IN  UINT8            Port,
    IN  UINT8            Command,
    IN  UINT32           CommandArgument,
    IN  UINT32           *TransferAddress,
    IN  UINT32           NumBlks,
    IN  UINT32           BlockSize,
    IN  UINT16           ResponseType,
    IN  BOOLEAN          AppCmd, 
    IN  BOOLEAN          Read
)
{

    UINT64      SdioBaseAddr = SdioDevInfo->SdioBaseAddress;
    EFI_STATUS  Status;
    UINT16      CommandIndex;
    UINT32      CommandArgument1;


    Status= SdMmcCardGetStatus(SdioDevInfo,Port);
    if(EFI_ERROR(Status)) {
        return Status;
    }

    //
    // All commands should be in tran state to trasfer data except for Cmd14
    // which will be in BTST state while transferring the data
    //
    if(Command != BUSTEST_READ_CMD14) {
        if((SdioDevInfo->bState != CARDSTATUS_TRAN)) {
            return EFI_DEVICE_ERROR;
        }
    } else {
        if((SdioDevInfo->bState != CARDSTATUS_BTST)) {
            return EFI_DEVICE_ERROR;
        }
    }
    //
    // Issue CMD55 if ACMD need to be sent.
    //
    if(AppCmd) {
        CommandIndex=((APP_CMD_CMD55 << 8) | RESP1_TYPE);
        CommandArgument1=(UINT32)SdioDevInfo->wRCA;
        CommandArgument1=CommandArgument1 << 16;
        Status=SdMmcCommand(SdioDevInfo,Port,CommandIndex,CommandArgument1);

        if(EFI_ERROR(Status)) {
            return Status;
        }
    }

    //
    // Configure the transfer mode register, Bloack size register and block
    // count register for the transfer
    //
    SDIO_WRITE_REG16(SdioBaseAddr,BLKSZ,BlockSize);
    SDIO_WRITE_REG16(SdioBaseAddr,BLKCNT,NumBlks);
    if(Read) {
        SDIO_WRITE_REG16(SdioBaseAddr,XFRMODE,DATA_TRANSFER_CARD2HOST);
    } else {
        SDIO_WRITE_REG16(SdioBaseAddr,XFRMODE,0);
    }
    
    //
    // Send the Command argument and command
    //
    CommandIndex = (Command << 8) | ResponseType+DATA_PRESENT_SELECT;
    Status=SdMmcCommand (SdioDevInfo,Port,CommandIndex,CommandArgument);
    if(EFI_ERROR(Status)) {
        return EFI_DEVICE_ERROR;
    }

    //
    // Wait for the transfer to complete
    //
    Status= SdMmcWaitXferComplete(SdioDevInfo,Port,&TransferAddress,BlockSize,Read);
    if(EFI_ERROR(Status)) {
        SdMmcRestoreStatus(SdioDevInfo,Port);
        return Status;
    }

    //
    // For Cmd19 restore status should not be called as it set the state of card to BTST.
    //
    if (Command != BUSTEST_WRITE_CMD19) {
        Status = SdMmcRestoreStatus(SdioDevInfo,Port);
    
        if(EFI_ERROR(Status)) {
            return EFI_DEVICE_ERROR;
        }
    }

    return Status;
}
/**
    To abort the IO transaction

    @param  SdioDevInfo 
    @param  Port 

    @retval EFI_STATUS

**/
EFI_STATUS
SdIoAbort(
    IN  SDIO_DEVICE_INFO    *SdioDevInfo,
    IN  UINT8               Slot
) {
    EFI_STATUS    Status;
    UINT32        CommandArgument;
    UINT32        DataBuffer;
   
    CommandArgument = MAKE_SDIO_RW(1)| MAKE_SDIO_OFFSET (IO_ABORT_REGISTER) | SdioDevInfo->FunctionNo;
   
    Status = SdIoSendCmd (SdioDevInfo,
                          Slot,
                          (IO_RW_DIRECT << 8) | RESP5_TYPE,
                          CommandArgument,
                          &DataBuffer,
                          RESP5_TYPE,
                          0);
   
    return Status;   
}

/**
    Send the command to IO device.

    @param  SdioDevInfo 
    @param  Port 
    @param  CommandIndex 
    @param  CommandArgument 
    @param  TransferAddress 
    @param  ResponseType 
    @param  Read
    
    @retval EFI_STATUS

**/
EFI_STATUS
SdIoSendCmd(
    IN  SDIO_DEVICE_INFO *SdioDevInfo,
    IN  UINT8            Slot,
    IN  UINT16           CommandIndex,
    IN  UINT32           CommandArgument,
    IN  UINT32           *TransferAddress,
    IN  UINT16           ResponseType,
    IN  BOOLEAN          Read
){    
    EFI_STATUS       Status;
    UINT32           SdioBaseAddr = (UINT32)(SdioDevInfo->SdioBaseAddress);
    UINT8            ResponseFlag=0;
    UINT32           ResponceData;

    Status = SdMmcCommand(SdioDevInfo,
                              Slot, 
                              CommandIndex, 
                              CommandArgument
                              );
        
    if(EFI_ERROR(Status)) {
        return Status;
    }
        
    // Get the Response Data 
    ResponceData = SDIO_REG32(SdioBaseAddr,RESP0);

    // Check for the Error.
    ResponseFlag = (UINT8)(ResponceData >> 8);
    if(ResponseFlag & (IO_OUT_OF_RANGE + IO_INVALID_FUNCTION_NO + IO_SDIO_DEVICE_ERROR +
                               IO_ILLEGAL_COMMAND + IO_COM_CRC_ERROR) ) {
        return EFI_DEVICE_ERROR;
    }
            
    if (Read) {
        *TransferAddress = ResponceData & 0x00FF;
    } 
        
    return EFI_SUCCESS;
    
}
/**
    Send IO read or write command to the device.

    @param  SdioDevInfo 
    @param  Port 
    @param  ReadWriteStartOffset 
    @param  NumberOfBytes 
    @param  ReadWrite 
    @param  DataBuffer 

    @retval EFI_STATUS

**/
EFI_STATUS
SdIoReadWriteCmd (
    IN  SDIO_DEVICE_INFO *SdioDevInfo,
    IN  UINT8            Slot,
    IN  UINT16           CommandIndex,
    IN  UINT32           CommandArgument,
    IN  UINT32           *TransferAddress,
    IN  UINT16           ResponseType,
    IN  BOOLEAN          Read
) {
    EFI_STATUS         Status = EFI_DEVICE_ERROR;
    UINT32             SdioBaseAddr = (UINT32)(SdioDevInfo->SdioBaseAddress);
    UINT32             NumBlksOrBytes;
    UINT32             BlockSize;
    BOOLEAN            BlockMode;
    UINT8              FuncNo;
    UINT16             TransferMode = 0;
    UINT32             CommandArg;
    UINT16             FnBlockSizeReg = 0;
    UINT32             DataBuffer;
    
    FuncNo = (CommandArgument >> 28) & 0x7;
    SdioDevInfo->FunctionNo = FuncNo;
    
    if (FuncNo > SdioDevInfo->IoFunctionCount) {
        return Status;
    }
    
    if (CommandIndex == IO_RW_DIRECT) {
        Status = SdIoSendCmd (SdioDevInfo,
                             Slot,
                            (CommandIndex << 8) | ResponseType,
                            CommandArgument,
                            TransferAddress,
                            RESP5_TYPE,
                            Read);
        return Status;
    } else if (CommandIndex == IO_RW_EXTENDED) {
        BlockMode = (CommandArgument >> 27) & 0x1;
        NumBlksOrBytes = CommandArgument & 0x1FF;
#if SDMMC_VERBOSE_PRINT
            DEBUG((EFI_D_VERBOSE,"SdMmc:BlockMode:%x,NumBlksOrBytes:%x",BlockMode,NumBlksOrBytes));
#endif
        // Read Bus Interface Control (CCCR 07h)
        CommandArg = MAKE_SDIO_OFFSET (BUS_INTERFACE_CONTROL);
        Status = SdIoSendCmd(SdioDevInfo,
                             Slot,
                             (IO_RW_DIRECT << 8) | ResponseType,
                             CommandArg,
                             &DataBuffer,
                             RESP5_TYPE,
                             1); 
        if (EFI_ERROR(Status)) {
            return Status;
        }
        // Card Detect Disable
        CommandArg = MAKE_SDIO_RW(1) | CommandArg | DataBuffer | CD_DISABLE;
        Status = SdIoSendCmd(SdioDevInfo,
                             Slot,
                             (IO_RW_DIRECT << 8) | ResponseType,
                             CommandArg,
                             &DataBuffer,
                             RESP5_TYPE,
                             0); 
        if (EFI_ERROR(Status)) {
            return Status;
        }
        if(BlockMode) {
            // Check whether Block mode is suppported or not 
            if(!(SdioDevInfo->IoCapabilityRegister & SMB)) {
                return EFI_DEVICE_ERROR;    
            }
            
            // Read the Block size from BlockSize registers
            FnBlockSizeReg = FN_BLOCK_SIZE_OFFSET | (FuncNo * 0x100);                  
            CommandArg = MAKE_SDIO_OFFSET (FnBlockSizeReg);
            Status = SdIoSendCmd(SdioDevInfo,
                                 Slot,
                                 (IO_RW_DIRECT << 8) | ResponseType,
                                 CommandArg,
                                 &DataBuffer,
                                 RESP5_TYPE,
                                 1);  
            if (EFI_ERROR(Status)) {
                return Status;
            }

            BlockSize = DataBuffer;
            FnBlockSizeReg ++;                        
            CommandArg = MAKE_SDIO_OFFSET (FnBlockSizeReg);
            Status = SdIoSendCmd(SdioDevInfo,
                                 Slot,
                                 (IO_RW_DIRECT << 8) | ResponseType,
                                 CommandArg,
                                 &DataBuffer,
                                 RESP5_TYPE,
                                 1);
            if (EFI_ERROR(Status)) {
                return Status;
            }
            BlockSize = (DataBuffer << 8) + BlockSize;
            TransferMode = BLOCK_COUNT_ENABLE + MULTI_BLOCK_SELECT; 
            
        } else {
            BlockSize = NumBlksOrBytes; 
            NumBlksOrBytes = 1;
        }
        
        // Configure the BlockSize and Block Count register
        SDIO_WRITE_REG16(SdioBaseAddr,BLKSZ,BlockSize);
        SDIO_WRITE_REG16(SdioBaseAddr,BLKCNT,NumBlksOrBytes);
        
        // Configure the transfer mode register, Block size register and block
        // count register for the transfer
        if(Read) {
            TransferMode = TransferMode | DATA_TRANSFER_CARD2HOST;
        } 
        
        SDIO_WRITE_REG16(SdioBaseAddr,XFRMODE,TransferMode);
        
        CommandIndex = (CommandIndex << 8) | ResponseType | DATA_PRESENT_SELECT;
        Status = SdIoSendCmd (SdioDevInfo,
                              Slot,
                              CommandIndex,
                              CommandArgument,
                              &DataBuffer,
                              RESP5_TYPE,
                              0);
        if (EFI_ERROR(Status)) {
            return Status;
        }
        
        Status= SdMmcWaitXferComplete(SdioDevInfo,
                                     Slot,
                                     &TransferAddress,
                                     (BlockMode)? BlockSize: BlockSize*NumBlksOrBytes,
                                     Read);

    }
    return Status;
}

/**
    Select the Device

    @param  SdioDevInfo 
    @param  Port 

    @retval EFI_STATUS
    Note: This function follows the Step of Section A.6.2 Switching to  
        high-speed mode of Jedec Electrical standard 5.01  

**/
EFI_STATUS
SdMmcCardSelect (
    IN  SDIO_DEVICE_INFO    *SdioDevInfo,
    IN  UINT8               Port
)
{

    UINT16      CommandIndex;
    UINT32      CommandArgument;
    EFI_STATUS  Status;

    if((SdioDevInfo->bState != CARDSTATUS_STBY) &&
        (SdioDevInfo->bState != CARDSTATUS_TRAN) &&
        (SdioDevInfo->bState != CARDSTATUS_DATA) &&
        (SdioDevInfo->bState != CARDSTATUS_PRG) &&
        (SdioDevInfo->bState != CARDSTATUS_DIS)
        ) {
        return EFI_DEVICE_ERROR;
    }

    //
    // Issue CMD7. This caommand select or de-select the card and 
    // put it to tran state.
    //
    CommandIndex=((SELECT_DESELECT_CARD_CMD7 << 8) | RESP1B_TYPE);
    CommandArgument=(UINT32)SdioDevInfo->wRCA;
    CommandArgument=CommandArgument << 16;
    Status=SdMmcCommand(SdioDevInfo,Port,CommandIndex,CommandArgument);
    if(EFI_ERROR(Status)) {
        return Status;
    }
    
    if(SdioDevInfo->IODevice == TRUE) {
        //
        //CMD 13 is not supported by IO device.
        //
        return EFI_SUCCESS;
    }

    Status=SdMmcCardGetStatus(SdioDevInfo,Port);
    if(EFI_ERROR(Status)) {
        return Status;
    }

    //
    // As per step 21 of Jedec Electrical standard 5.01 the cmd7 should put
    //the card in tran state 
    //
    if(SdioDevInfo->bState != CARDSTATUS_TRAN)  {
        return EFI_DEVICE_ERROR;
    }
    return EFI_SUCCESS;
}

/**
    Get the Sd device current Status

    @param  SdioDevInfo 
    @param  Port 

    @retval EFI_STATUS

**/
EFI_STATUS
SdMmcCardGetStatus (
    IN  SDIO_DEVICE_INFO    *SdioDevInfo,
    IN  UINT8               Port
)
{

    UINT64      SdioBaseAddr = SdioDevInfo->SdioBaseAddress;
    UINT32      Temp32;
    UINT16      CommandIndex;
    UINT32      CommandArgument;
    EFI_STATUS  Status;

    //
    //if auto cmd12 enable bit set, will return R1b response REP[96:127]
    //To prevent confusion, clear this bit before issue CMD13
    //
    SDIO_WRITE_REG16(SdioBaseAddr,XFRMODE,0);


    //
    //Issue CMD13. To get the state of the card
    //
    CommandIndex=((SEND_STATUS_CMD13 << 8) | RESP1_TYPE);
    CommandArgument=(UINT32)SdioDevInfo->wRCA;
    CommandArgument=CommandArgument << 16;
    Status=SdMmcCommand(SdioDevInfo,Port,CommandIndex,CommandArgument);

    if(EFI_ERROR(Status)) {
        return Status;
    }


    Temp32=SDIO_REG32(SdioBaseAddr,RESP0);
    SdioDevInfo->CurrentState=Temp32;
    Temp32 &=CURRENT_STATE;
    Temp32 = (Temp32 >> 9);

    SdioDevInfo->bState=(UINT8)Temp32;

    return EFI_SUCCESS;
}

/**
    Wait until Transfer Complete and gets the data from SD device

    @param  SdioDevInfo 
    @param  Port 
    @param  BufferAddress 
    @param  DmaAddress 

    @retval EFI_STATUS

**/
EFI_STATUS
SdMmcWaitXferComplete (
    IN  SDIO_DEVICE_INFO    *SdioDevInfo,
    IN  UINT8               Port,
    IN  UINT32              **BufferAddress,
    IN  UINT32              BlkSize,
    IN  BOOLEAN             SdioRead
)
{
    UINT64      SdioBaseAddr = SdioDevInfo->SdioBaseAddress;
    EFI_STATUS  Status;
    UINT16      Temp16;
    UINT16      ErrorInt16;
    UINT8       TEMP8=0;
    UINT32      TimeOut;

    Status=EFI_TIMEOUT;
    while(1){

        Temp16=SDIO_REG16(SdioBaseAddr,NINTSTS);
        if(Temp16 & NINTSTS_DMAINT) {
            SDIO_WRITE_REG16(SdioBaseAddr,NINTSTS,NINTSTS_DMAINT);
            SdMmcDmaIntHandle(SdioDevInfo,Port,BufferAddress);
        } else if(Temp16 & NINTSTS_BRR) {
            SDIO_WRITE_REG16(SdioBaseAddr,NINTSTS,NINTSTS_BRR);
            SdMmcBRINTHandle(SdioDevInfo,Port,BufferAddress,SdioRead,BlkSize);
        } else if(Temp16 & NINTSTS_BWR) {
            SDIO_WRITE_REG16(SdioBaseAddr,NINTSTS,NINTSTS_BWR);
            SdMmcBRINTHandle(SdioDevInfo,Port,BufferAddress,SdioRead,BlkSize);
        } else if(Temp16 & NINTSTS_TC) {
            Status=EFI_SUCCESS;
            
            //
            // According to section 2.2.17 Normal Interrupt Status Register 
            // (Offset 030h). The Transfer complete has higher priority than
            // data timeout. If both happens together execution oof it is 
            // completed. Clear the error both set togthered.
            //
            SDIO_WRITE_REG16(SdioBaseAddr,ERINTSTS,0xFFFF);
            SDIO_WRITE_REG16(SdioBaseAddr,NINTSTS,NINTSTS_EI);
            break;
        } else if(Temp16 & NINTSTS_EI) {
            ErrorInt16 = SDIO_REG16(SdioBaseAddr,ERINTSTS);
#if SDMMC_VERBOSE_PRINT
            DEBUG((EFI_D_VERBOSE,"SdMmc: DataXfer error register=%x \n",ErrorInt16));
#endif
            Status=EFI_DEVICE_ERROR;
            if( ErrorInt16 & ADMA_ERROR) {
                SDIO_WRITE_REG16(SdioBaseAddr,ERINTSTS,ADMA_ERROR);
                SDIO_WRITE_REG16(SdioBaseAddr,NINTSTS,NINTSTS_EI);
                SDMmcAsynchronousAbort(SdioDevInfo,Port);
            } else {
                if(ErrorInt16 & DATA_CRC_ERROR) {
                    Status=EFI_CRC_ERROR;
                }
                if(SdioDevInfo->IODevice) {
                    SdIoAbort(SdioDevInfo,Port);
                } 
                SDIO_WRITE_REG16(SdioBaseAddr,ERINTSTS,0xFFFF);
                SDIO_WRITE_REG16(SdioBaseAddr,NINTSTS,NINTSTS_EI);
                
                //
                // Reset the CMD and DAT line
                //
                SDIO_REG8_OR(SdioBaseAddr,SWRST,RESETCMD+RESETDAT);
                
                //
                // Wait till both the Software Reset For DAT Line and Software Reset
                // For CMD Line are cleared.
                //
                TimeOut = DATA_COMMAND_LINE_RESET_TIMEOUT;
                do {
                   TEMP8=SDIO_REG8(SdioBaseAddr,SWRST);
                   MicroSecondDelay (1);
                }while(((TEMP8 & RESETDAT)|| (TEMP8 & RESETCMD))&&(TimeOut--));
                if(!TimeOut) {
                    return EFI_TIMEOUT;
                }
            }
            
            break;
        }

        //
        //delay 1ms
        //
        MicroSecondDelay (1000);
    }

   SDIO_WRITE_REG16(SdioBaseAddr,NINTSTS,NINTSTS_TC+NINTSTS_DMAINT+ \
           NINTSTS_BWR+NINTSTS_BRR);
        

    return Status;
}

/**
    DMA interrupt handler

    @param  SdioDevInfo 
    @param  Port 

    @retval EFI_STATUS

**/
EFI_STATUS
SdMmcDmaIntHandle (
    IN  SDIO_DEVICE_INFO    *SdioDevInfo,
    IN  UINT8               Port,
    IN  UINT32              **BufferAddress
)
{

    UINT64  SdioBaseAddr = SdioDevInfo->SdioBaseAddress;
    UINT32  BytesToBeTransferd=BLOCK_BOUNTRY;

    *BufferAddress=(UINT32*)(((UINT8*)*BufferAddress)+BytesToBeTransferd);
    SDIO_WRITE_REG16(SdioBaseAddr,NINTSTS,NINTSTS_DMAINT);
    SDIO_WRITE_REG32(SdioBaseAddr,DMAADR,((UINT32)*BufferAddress));


    return EFI_SUCCESS;
}

/**
    BR interrupt Handler

    @param  SdioDevInfo 
    @param  Port 

    @retval EFI_STATUS

**/
EFI_STATUS
SdMmcBRINTHandle (
    IN  SDIO_DEVICE_INFO    *SdioDevInfo,
    IN  UINT8               Port,
    IN  UINT32              **TransferAddress,
    IN  BOOLEAN             Read,
    IN  UINT32              BlockSize
)
{

    UINT64  SdioBaseAddr = SdioDevInfo->SdioBaseAddress;

    BlockSize=BlockSize/4;
    if(Read) {
        //
        // Read
        //
        do {
            (**TransferAddress)=SDIO_REG32(SdioBaseAddr,BUFDATA);
            (*TransferAddress)++;
            BlockSize--;
        }while(BlockSize);

    } else {
        //
        // Write
        //
        do {
            SDIO_WRITE_REG32(SdioBaseAddr,BUFDATA,(**TransferAddress));
            (*TransferAddress)++;
            BlockSize--;
        }while(BlockSize);
    }
    return EFI_SUCCESS;
}

/**
    Restore the SD card Status

    @param  SdioDevInfo 
    @param  Port 

    @retval EFI_STATUS

**/
EFI_STATUS
SdMmcRestoreStatus (
    IN  SDIO_DEVICE_INFO    *SdioDevInfo,
    IN  UINT8               Port
)
{
    UINT16      CommandIndex;
    UINT32      CommandArgument;
    EFI_STATUS  Status;

    do {
        Status=SdMmcCardGetStatus(SdioDevInfo,Port);

        if(EFI_ERROR(Status)) {
            return Status;
        }

        if(SdioDevInfo->bState == CARDSTATUS_TRAN) {
            return EFI_SUCCESS;
        }

        if( (SdioDevInfo->bState == CARDSTATUS_DATA) ||
            (SdioDevInfo->bState == CARDSTATUS_PRG)||
            (SdioDevInfo->bState == CARDSTATUS_RCV) ) {

            //
            //Issue CMD12
            //

            CommandIndex=((STOP_TRANSMISSION_CMD12 << 8) | RESP1B_TYPE);
            CommandArgument=0;
            Status=SdMmcCommand(SdioDevInfo,Port,CommandIndex,CommandArgument);
            if(EFI_ERROR(Status)) {
                return Status;
            }
            continue;

        } else {
            return EFI_DEVICE_ERROR;
        }
    }while(1);

    return EFI_SUCCESS;
}

/**
    To abort the ADMA transaction

    @param  SdioDevInfo 
    @param  Port 

    @retval EFI_STATUS

**/
EFI_STATUS
SDMmcAsynchronousAbort (
    IN  SDIO_DEVICE_INFO    *SdioDevInfo,
    IN  UINT8               Port
)
{
    UINT16      CommandIndex;
    UINT32      CommandArgument;
    EFI_STATUS  Status;
    UINT64      SdioBaseAddr = SdioDevInfo->SdioBaseAddress;
    UINT8       TEMP8=0;
    UINT32      TimeOut;

    //
    //Issue CMD12 to stop the transaction
    //
    CommandIndex=((STOP_TRANSMISSION_CMD12 << 8) | ABORT_CMD |RESP1B_TYPE);
    CommandArgument=0;

    Status=SdMmcCommand(SdioDevInfo,Port,CommandIndex,CommandArgument);
    if(EFI_ERROR(Status)) {
        return Status;
    }
    //
    // Reset the CMD and DAT line
    //
    SDIO_REG8_OR(SdioBaseAddr,SWRST,RESETCMD+RESETDAT);
    
    //
    // Wait till both the Software Reset For DAT Line and Software Reset
    // For CMD Line are cleared.
    //
    TimeOut = DATA_COMMAND_LINE_RESET_TIMEOUT;
    do {
        TEMP8=SDIO_REG8(SdioBaseAddr,SWRST);
         MicroSecondDelay (1);
    }while(((TEMP8 & RESETDAT)|| (TEMP8 & RESETCMD))&&(TimeOut--));
    if(!TimeOut) {
        return EFI_TIMEOUT;
    }

    return EFI_SUCCESS;
}

/**
    Read the Data from Sd or Mmc card

    @param  SdioDevInfo 
    @param  Port 

    @retval EFI_STATUS

**/
EFI_STATUS
SdMmcRead(
    IN  SDIO_DEVICE_INFO    *SdioDevInfo,
    IN  UINT8               Port,
    IN  EFI_LBA             LBA,
    IN  UINT32              NumBlks,
    IN  VOID                *BufferAddress
)
{
    UINT64                  SdioBaseAddr = SdioDevInfo->SdioBaseAddress;
    EFI_STATUS              Status;
    BOOLEAN                 SdioRead=TRUE;
    SDIO_ADMA_DESCRIPTOR    *AdmaAddress=0;
#if SDMMC_VERBOSE_PRINT
    UINT8                       *TempAddress = (UINT8*)BufferAddress;
#endif
    //
    // if BufferAddress is not IoAlign by 4 in ADMA mode then select PIO mode or
    // if BufferAddress is not IoAlign by 4 in SDMA mode then select PIO mode or
    // if address is 64 bit 
    //
    if((((UINT64)BufferAddress) & (UPPER_64_BITS))||\
            ((SdioDevInfo->SDIO_Access == ADMA)&& (((UINTN)BufferAddress)%4))||\
            ((SdioDevInfo->SDIO_Access == SDMA)&& (((UINTN)BufferAddress)%4096))){
        gSDIO_Access =PIO;
        
    } else {
        gSDIO_Access = SdioDevInfo->SDIO_Access;
    }

    AdmaAddress = (SDIO_ADMA_DESCRIPTOR*)(gSdioData->AdmaDescriptorAddress);


    Status=SdMmcCardGetStatus(SdioDevInfo,Port);

    if(EFI_ERROR(Status)) {
        Status=EFI_DEVICE_ERROR;
        goto ReadError;
    }

    if(SdioDevInfo->bState != CARDSTATUS_TRAN) {
        Status=EFI_DEVICE_ERROR;
        goto ReadError;
    }


    //
    // Creating ADMA Descriptor entries if Access mode is ADMA
    // 
    if (gSDIO_Access == ADMA){
        Status = SdMmcADMADescriptorEntry(SdioDevInfo,NumBlks,BufferAddress,AdmaAddress);
        if(EFI_ERROR(Status)) {
            Status=EFI_DEVICE_ERROR;
            goto ReadError;
        }
    }

    //
    //Send the command to controller
    //
    Status=SDMmcRWCMDController(SdioDevInfo,Port,LBA, NumBlks, BufferAddress,TRUE);
    if(EFI_ERROR(Status)) {
        Status=EFI_DEVICE_ERROR;
        goto ReadError;
    }


    //
    //Wait for the transfer complete. Data copied to Buffer address
    //
    Status= SdMmcWaitXferComplete(SdioDevInfo,Port,(UINT32**)&BufferAddress,SdioDevInfo->wBlockSize,SDMMC_READ);
    if(EFI_ERROR(Status)) {
        SdMmcRestoreStatus(SdioDevInfo,Port);
        goto ReadError;
    }

    Status= SdMmcRestoreStatus(SdioDevInfo,Port);
    if(EFI_ERROR(Status)) {
        Status=EFI_DEVICE_ERROR;
        goto ReadError;
    }

ReadError:
#if SDMMC_VERBOSE_PRINT
        DEBUG((EFI_D_VERBOSE,"SdMmc: Read Lba=%x NumBlks=%x BufferAddress=%x Read Status= %r\n",LBA,NumBlks,TempAddress,Status));
#endif


    return Status;
}

/**
    Read the Data from Sd or Mmccard

    @param  SdioDevInfo 
    @param  Port 

    @retval EFI_STATUS

**/
EFI_STATUS
SdMmcWrite (
    IN  SDIO_DEVICE_INFO    *SdioDevInfo,
    IN  UINT8               Port,
    IN  EFI_LBA             LBA,
    IN  UINT32              NumBlks,
    IN  VOID                *BufferAddress
)
{
    UINT64                  SdioBaseAddr = SdioDevInfo->SdioBaseAddress;
    EFI_STATUS              Status;
    SDIO_ADMA_DESCRIPTOR    *AdmaAddress=0;
    UINT32                  *TempAddress = (UINT32*)BufferAddress;

    //
    // if BufferAddress is not IoAlign by 4 in ADMA mode then select PIO mode or
    // if BufferAddress is not IoAlign by 4 in SDMA mode then select PIO mode or
    // if address is 64 bit 
    //
    if((((UINT64)BufferAddress) & (UPPER_64_BITS))||\
            ((SdioDevInfo->SDIO_Access == ADMA)&& (((UINTN)BufferAddress)%4))||\
            ((SdioDevInfo->SDIO_Access == SDMA)&& (((UINTN)BufferAddress)%4096))){
            gSDIO_Access =PIO;
    } else {
        gSDIO_Access = SdioDevInfo->SDIO_Access;
    }
    AdmaAddress = (SDIO_ADMA_DESCRIPTOR*)(gSdioData->AdmaDescriptorAddress);

    SDIO_WRITE_REG16(SdioBaseAddr,NINTEN,0x1FF);
    SDIO_WRITE_REG16(SdioBaseAddr,ERINTEN,0x3FF);
    SDIO_WRITE_REG16(SdioBaseAddr,NINTSIGEN,0x0);
    SDIO_WRITE_REG16(SdioBaseAddr,ERINTSIGEN,0x0);

    Status=SdMmcCardGetStatus(SdioDevInfo,Port);

    if(EFI_ERROR(Status)) {
        Status=EFI_DEVICE_ERROR;
        goto WriteError;
    }

    if(SdioDevInfo->bState != CARDSTATUS_TRAN) {
        Status=EFI_DEVICE_ERROR;
        goto WriteError;
    }

    //
    // Creating ADMA Descriptor entries if Access mode is ADMA
    // 
    if (gSDIO_Access == ADMA){
        Status = SdMmcADMADescriptorEntry(SdioDevInfo,NumBlks,BufferAddress,AdmaAddress);
        if(EFI_ERROR(Status)) {
            Status=EFI_DEVICE_ERROR;
            goto WriteError;
        }
    }

    //
    //Send the command to controller
    //
    Status = SDMmcRWCMDController(SdioDevInfo,Port,LBA, NumBlks, BufferAddress,FALSE);

    if(EFI_ERROR(Status)) {
        Status=EFI_DEVICE_ERROR;
        goto WriteError;
    }

    //
    //Wait for the transfer complete and move the data from Input buffer to Dma buffer.
    //
    Status= SdMmcWaitXferComplete(SdioDevInfo,Port,(UINT32**)&BufferAddress,SdioDevInfo->wBlockSize,SDMMC_WRITE);
    if(EFI_ERROR(Status)) {
        SdMmcRestoreStatus(SdioDevInfo,Port);
        goto WriteError;
    }

    Status= SdMmcRestoreStatus(SdioDevInfo,Port);

    if(EFI_ERROR(Status)) {
        Status=EFI_DEVICE_ERROR;
        goto WriteError;
    }

WriteError:
#if SDMMC_VERBOSE_PRINT
        DEBUG((EFI_D_VERBOSE,"SdMmc: Write Lba=%x NumBlks=%x BufferAddress=%x Write Status= %r\n",LBA,NumBlks,TempAddress,Status));
#endif
    //
    //Clear the status.
    //
    SDIO_WRITE_REG16(SdioBaseAddr,NINTEN,0xFF);
    SDIO_WRITE_REG16(SdioBaseAddr,ERINTEN,0x2FF);
    SDIO_WRITE_REG16(SdioBaseAddr,NINTSIGEN,0xFF);
    SDIO_WRITE_REG16(SdioBaseAddr,ERINTSIGEN,0x2FF);

    return Status;
}

/**
    Sends the Read or Write command to the SD controller

    @param  SdioDevInfo 
    @param  Port 

    @retval EFI_STATUS

**/
EFI_STATUS
SDMmcRWCMDController (
    IN  SDIO_DEVICE_INFO    *SdioDevInfo,
    IN  UINT8               Port,
    IN  EFI_LBA             LBA,
    IN  UINT32              NumBlks,
    IN  VOID                *BufferAddress,
    BOOLEAN                 ReadWrite
)
{
    UINT64      SdioBaseAddr = SdioDevInfo->SdioBaseAddress;
    UINT32      BlockLength=0;
    UINT16      TransferMode=0;
    UINT16      CmdData=0;
    UINT32      BlockSize=0;
    
    //
    // Set SDMA Register
    //
    if(gSDIO_Access == SDMA) {
        SDIO_WRITE_REG32(SdioBaseAddr,DMAADR,BufferAddress);
    }

    //
    // Set Argument Register (read write address from card)
    //
    if(!((SdioDevInfo->bMode == SD_HIGH_CAP) || 
           (SdioDevInfo->bMode == MMC_HIGH_CAP)) ) {
        if(LBA >= 0x400000) {
            return EFI_DEVICE_ERROR;
        }
        LBA=(UINT32)LBA * SdioDevInfo->wBlockSize;
    }

    //
    // Set Block Size Register, Default 4KB block
    //

    if(SdioDevInfo->bMode & (SD_STAND_CAP+SD_HIGH_CAP+\
            MMC_HIGH_CAP+MMC_STAND_CAP)) {
        BlockLength=SdioDevInfo->wBlockSize;
    }

    //
    // Right now we support only 4KB DMA buffer
    //
    BlockSize=(SDIO_Transfer_Buffer << 12) + BlockLength;
    SDIO_WRITE_REG16(SdioBaseAddr,BLKSZ,BlockSize);


    //
    // Set Block Count Register
    //
    if(NumBlks == 0) {
        return EFI_DEVICE_ERROR;
    }

    SDIO_WRITE_REG16(SdioBaseAddr,BLKCNT,NumBlks);

    if(ReadWrite) {
        //
        //Read
        //
        if(NumBlks == 1) {
            //
            // Single block read
            // 
            TransferMode=DATA_TRANSFER_CARD2HOST;
            CmdData=(READ_SINGLE_BLOCK_CMD17 << 8);
        } else {
            //
            // Multiple block read
            //
            TransferMode= (DATA_TRANSFER_CARD2HOST  + \
                    BLOCK_COUNT_ENABLE + \
                    MULTI_BLOCK_SELECT + AUTO_CMD12_ENABLE);
            CmdData=(READ_MULTIPLE_BLOCK_CMD18 << 8);
        }
        
    } else {
        //
        //Write
        //
        if(NumBlks == 1) {
            //
            // Single block write
            // 
            CmdData=(WRITE_SIGLE_BLOCK_CMD24 << 8);
        } else {
            TransferMode= (BLOCK_COUNT_ENABLE + \
                    MULTI_BLOCK_SELECT + AUTO_CMD12_ENABLE);
            CmdData=(WRITE_MULTIPLE_BLOCK_CMD25 << 8);
        }

    }

    //
    // Enable DMA mode and DMA transfer mode. HostCtl Bit 03-04 determines
    // the DMA transfer mode. If 00 then SDMA and if 10 the ADMA32 mode.
    //
    if(gSDIO_Access == ADMA) {
        SDIO_REG8_AND_OR( SdioBaseAddr, HOSTCTL, ~(DMA_SELECT) ,ADMA32_SELECT);
        TransferMode|=DMA_ENABLE;
    }else if(gSDIO_Access == SDMA) {
        TransferMode|=DMA_ENABLE;
    }

    SDIO_WRITE_REG16(SdioBaseAddr,XFRMODE,TransferMode);

    //
    // Set CMD Index
    //
    CmdData=CmdData + RESP1_TYPE+DATA_PRESENT_SELECT;

    return SdMmcCommand(SdioDevInfo, Port,CmdData,(UINT32)LBA);

}

/**
    Reset the SD or Mmc card

    @param  SdioDevInfo 
    @param  Port 

    @retval EFI_STATUS

**/
EFI_STATUS
SdMmcResetCard (
    IN  SDIO_DEVICE_INFO    *SdioDevInfo,
    IN  UINT8               Port
)
{

    UINT64  SdioBaseAddr = SdioDevInfo->SdioBaseAddress;

    //
    //Release DAT line
    //
    SDIO_WRITE_REG8(SdioBaseAddr,SWRST,RESETDAT);

    //
    //release CMD 
    //
    SDIO_WRITE_REG8(SdioBaseAddr,SWRST,RESETCMD);


    return EFI_SUCCESS;
}

/**
    This function will create ADMA descriptor entries in ADMA mode.

    @param  SdioDevInfo  - Pointer to SDIO_DEVICE_INFO structure
    @param  NumBlks       - Number of block
    @param  BufferAddress - Buffer address.
    @param  AdmaDescrip   - ADMA Descriptor pointer

    @retval EFI_SUCCESS
    Note: This funtion is following 1.13 Advanced DMA

**/
EFI_STATUS
SdMmcADMADescriptorEntry (
    IN  SDIO_DEVICE_INFO        *SdioDevInfo,
    IN  UINT32                  NumBlks,
    IN  VOID                    *BufferAddress,
    IN  SDIO_ADMA_DESCRIPTOR    *AdmaDescrip
)
{
    UINTN   TotalBytes=0;
    UINT32  TotalDescEntries=0;
    UINT32  i; 
    UINT64  SdioBaseAddr = SdioDevInfo->SdioBaseAddress;
    
    //
    // Total Length 
    //
    TotalBytes =(NumBlks)* SdioDevInfo->wBlockSize;

    //
    // Calculation of total number of ADMA Entries
    //
    TotalDescEntries = (UINT32)(TotalBytes / MAXIMUM_ADMA_LENGTH);
    if(TotalBytes % MAXIMUM_ADMA_LENGTH) {
        TotalDescEntries= TotalDescEntries+1;
    }

    //
    // Currently only 512 entries can be stored
    //
    if (TotalDescEntries > 512) {
        return EFI_DEVICE_ERROR;
    }

    for(i=0; i< TotalDescEntries;i++) {
        AdmaDescrip[i].EntryAddress = (UINT32)((UINT32*)BufferAddress)+(i*MAXIMUM_ADMA_LENGTH);
        AdmaDescrip[i].Attribute.ValidBit =1;
        AdmaDescrip[i].Attribute.EndBit =0;
        AdmaDescrip[i].Attribute.IntBit =0;
        AdmaDescrip[i].Attribute.Act1Bit =0;
        AdmaDescrip[i].Attribute.Act2Bit =1;
        AdmaDescrip[i].Attribute.AttReserved =0;
        AdmaDescrip[i].Attribute.Reserved1 =0;
        AdmaDescrip[i].Attribute.Reserved =0;
        AdmaDescrip[i].Attribute.Length =0;
   
    }

    //
    // if TotalBytes is not evenly divisible(no remainder) by  MAXIMUM_ADMA_LENGTH(64kb) 
    // then the last entry will hold remainder.
    //
    if (TotalBytes % MAXIMUM_ADMA_LENGTH) { 
        AdmaDescrip[i-1].Attribute.Length = (TotalBytes % MAXIMUM_ADMA_LENGTH);
    } 

    //
    // The endBit of the last entry should be set to 1
    //
    AdmaDescrip[i-1].Attribute.EndBit =1;
    SDIO_WRITE_REG32(SdioBaseAddr,ADMAADR,AdmaDescrip);
    
    //
    // Enable the error interrupt for ADMA and auto CMD
    //
    SDIO_WRITE_REG16(SdioBaseAddr,ERINTEN,0x3FF);
    SDIO_WRITE_REG16(SdioBaseAddr,ERINTSIGEN,0x2FF);
    return EFI_SUCCESS;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
