//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
//#*********************************************************************
//<AMI_FHDR_START>
//---------------------------------------------------------------------------
//
// Name:        RTCWakeup.C
//
// Description: Provide functions to register and handle RtcWakeup from S5
//              functionality.  This code is generic and as long as PM
//              base address SDL token is defined properly this should
//              work fine.
//
//---------------------------------------------------------------------------
//<AMI_FHDR_END>
//
#include <Token.h>
#include <AmiDxeLib.h>
#include<Protocol/SmmBase2.h>
#include <Protocol/SmmSxDispatch2.h>
#include <Setup.h>
#if RTCWAKEUP_POWERBUTTON_SUPPORT
#include <Protocol/SmmPowerButtonDispatch.h>
#endif


#define CMOS_INDEX  0x70
#define CMOS_DATA   0x71
#define RTC_SECONDS_ALARM   1
#define RTC_MIN_ALARM       3
#define RTC_HOUR_ALARM      5
#define RTC_STATUS_REG_A    0xA
#define RTC_STATUS_REG_B    0xB
#define RTC_STATUS_REG_C    0xC


EFI_GUID    gSetupGuid = SETUP_GUID;
SETUP_DATA  gSetupData ;

UINT8   RtcWakeType=0;
UINT8   RTCWakeupTimeHour;
UINT8   RTCWakeupTimeMinute;
UINT8   RTCWakeupTimeSecond;
UINT8   DynamicMinIncrease;


//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   DecToBCD
//
// Description: Converts a Decimal value to a BCD value.
//
// Input:       
//      IN UINT8 Dec - Decimal value
//
// Output:      
//      UINT8 - BCD value
//
// Notes:       
//      Only for 2 digit decimal.
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
UINT8 DecToBCD (
  IN UINT8 Dec )
{
    UINT8 FirstDigit = Dec % 10;
    UINT8 SecondDigit = Dec / 10;

    return (SecondDigit << 4) + FirstDigit;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   BCDToDec
//
// Description: Converts a BCD value to a Decimal value.
//
// Input:       
//      IN UINT8 BCD -- BCD value
//
// Output:      
//      UINT8 - decimal value
//
// Notes:       
//      Only for 2 digit BCD.
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
UINT8 BCDToDec (
  IN UINT8 BCD )
{
    UINT8 FirstDigit = BCD & 0xf;
    UINT8 SecondDigit = BCD >> 4;;

    return SecondDigit * 10  + FirstDigit;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   ReadRtcIndex
//
// Description: Read the RTC value at the given Index.
//
// Input:       Index       RTC Index
//
// Output:      RTC Value read from the provided Index
//
// Notes:       Here is the control flow of this function:
//              1. Read port 0x70 (RTC Index Register) to get bit 7.
//                  Bit 7 is the NMI bit-it should not be changed.
//              2. Output 0x70 with the Index and NMI bit setting.
//              3. Read 0x71 for Data. Getting Dec when appropriate.
//              4. Return the Data.
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

UINT8 ReadRtcIndex (
  IN UINT8 Index )
{
    UINT8 volatile Value=0;

    do {
        IoWrite8(CMOS_INDEX, RTC_STATUS_REG_A | 0x80);
        Value = IoRead8(CMOS_DATA);     
    } while (Value & 0x80); 

    IoWrite8(CMOS_INDEX, Index | 0x80);

    // Read register
    Value = IoRead8(CMOS_DATA);
    if (Index <= 9) { 
        Value = BCDToDec(Value);    
    }

    return (UINT8)Value;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   WriteRtcIndex
//
// Description: Write the RTC value at the given Index.
//
// Input:       Index   RTC Index
//              Data    RTC Index
//
// Output:      None
//
// Notes:       Here is the control flow of this function:
//              1. Read port 0x70 (RTC Index Register) to get bit 7.
//                 Bit 7 is the NMI bit-it should not be changed.
//              2. Output 0x70 with the Index. Switch to BCD when needed.
//              3. Write the data to 0x71.
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

void WriteRtcIndex (
  IN UINT8 Index, 
  IN UINT8 Value )
{
    IoWrite8(CMOS_INDEX,Index | 0x80);
    if (Index <= 9 ) { 
        Value = DecToBCD(Value);
    }

    // Write Register
    IoWrite8(CMOS_DATA,Value);

    return;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   SetWakeupTime
//
// Description: Set the alarm time to CMOS location and enable alarm interrupt
//
// Input:       
//      IN EFI_TIME *Time
//
// Output:      
//      VOID
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
void SetWakeupTime (
    IN EFI_TIME     *Time )
{
    UINT8 Value;

    WriteRtcIndex(RTC_HOUR_ALARM,Time->Hour);
    WriteRtcIndex(RTC_MIN_ALARM,Time->Minute);
    WriteRtcIndex(RTC_SECONDS_ALARM,Time->Second);

    //Set Enable
    Value = ReadRtcIndex(RTC_STATUS_REG_B);
    Value |= 1 << 5;
    WriteRtcIndex(RTC_STATUS_REG_B,Value);
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   S5SmiCallback
//
// Description: 
//
// Input:
//  IN EFI_HANDLE                      DispatchHandle
//  IN EFI_SMM_SX_DISPATCH_CONTEXT    *DispatchContext
//
// Output: VOID
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
EFIAPI
S5SmiCallback (
  IN EFI_HANDLE  DispatchHandle,
  IN CONST VOID  *Context         OPTIONAL,
  IN OUT VOID    *CommBuffer      OPTIONAL,
  IN OUT UINTN   *CommBufferSize  OPTIONAL )
{
    EFI_TIME      Time;
    
    if ( !(gSetupData.RtcWakeType) ) {
        return EFI_SUCCESS;
    }
    


    // If the Dynamic option selected, read the current time and increase
    // the alarm time by the Min selected in Setup
    if (RtcWakeType == 2) {
        RTCWakeupTimeHour = ReadRtcIndex(4);
        RTCWakeupTimeMinute = ReadRtcIndex(2);
        RTCWakeupTimeSecond = ReadRtcIndex(0);

        RTCWakeupTimeMinute += DynamicMinIncrease;
        if  (RTCWakeupTimeMinute >= 60) {
             RTCWakeupTimeMinute = 0;
             ++RTCWakeupTimeHour;
             if (RTCWakeupTimeHour == 24) {
                RTCWakeupTimeHour = 0;
             }
        }
    }

    Time.Hour = RTCWakeupTimeHour;
    Time.Minute = RTCWakeupTimeMinute;
    Time.Second = RTCWakeupTimeSecond;

    SetWakeupTime(&Time);

    //Clear RTC PM1 status
    IoWrite16(PM_BASE_ADDRESS , ( IoRead16(PM_BASE_ADDRESS) | (1 << 10) ));

    //set RTC_EN bit in PM1_EN to wake up from the alarm
    IoWrite16(PM_BASE_ADDRESS + 0x02, \
            ( IoRead16(PM_BASE_ADDRESS + 0x02) | (1 << 10) ));

    return EFI_SUCCESS;
}

#if RTCWAKEUP_POWERBUTTON_SUPPORT
//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   RTCPBtnSmiCallback
//
// Description: This function enter s5 callback if press power button
//
// Input:
//  IN EFI_HANDLE       DispatchHandle
//  IN EFI_SMM_POWER_BUTTON_DISPATCH_CONTEXT *DispatchContext
//
// Output: EFI_STATUS
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID RTCPBtnSmiCallback (
  IN EFI_HANDLE                               DispatchHandle,
  IN EFI_SMM_POWER_BUTTON_DISPATCH_CONTEXT    *DispatchContext )
{
    EFI_SMM_SX_REGISTER_CONTEXT  SxDispatchContext;

    if(((gSetupData.RtcWakeType != 1)&&(gSetupData.RtcWakeType != 2))) {
        //Clear RTC PM1 status
        IoWrite16(PM_BASE_ADDRESS , ( IoRead16(PM_BASE_ADDRESS) | (1 << 10) ));
        //Clear RTC_EN bit in PM1_EN to wake up from the alarm
        IoWrite32(PM_BASE_ADDRESS + 0x02, \
                ( IoRead32(PM_BASE_ADDRESS + 0x02) & ~(1 << 10) ));
    } else {
        S5SmiCallback( DispatchHandle , &SxDispatchContext, NULL , NULL );
    }
}
#endif
//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   InSmmFunction
//
// Description: This function is called from SMM during SMM registration.
//
// Input:
//  IN EFI_HANDLE       ImageHandle
//  IN EFI_SYSTEM_TABLE *SystemTable
//
// Output: EFI_STATUS
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS InSmmFunction (
  IN EFI_HANDLE        ImageHandle, 
  IN EFI_SYSTEM_TABLE  *SystemTable )
{
    EFI_STATUS                               Status;
    EFI_SMM_SX_DISPATCH2_PROTOCOL            *SxDispatchProtocol;
    EFI_SMM_SX_REGISTER_CONTEXT              SxDispatchContext;
    EFI_HANDLE                               SxDispatchHandle;
    UINT8                                    Value;
    EFI_SMM_BASE2_PROTOCOL                   *pSmmBase=NULL;
    EFI_SMM_SYSTEM_TABLE2                    *pSmst=NULL;
#if RTCWAKEUP_POWERBUTTON_SUPPORT
    // Power Button SMI
    EFI_SMM_POWER_BUTTON_DISPATCH_PROTOCOL   *PBtnDispatch;
    EFI_SMM_POWER_BUTTON_DISPATCH_CONTEXT    PBtnContext = {PowerButtonEntry};
#endif
    
    //Disable RTC alarm and clear RTC PM1 status
    Value = ReadRtcIndex(RTC_STATUS_REG_B);
    Value &= ~((UINT8)1 << 5);
    WriteRtcIndex(RTC_STATUS_REG_B,Value);
    //Clear Alarm Flag (AF) by reading RTC Reg C

    ReadRtcIndex(RTC_STATUS_REG_C); 
    IoWrite16(PM_BASE_ADDRESS , ( IoRead16(PM_BASE_ADDRESS) | (1 << 10) ));

    Status = pBS->LocateProtocol(&gEfiSmmBase2ProtocolGuid, NULL, &pSmmBase);
    if (EFI_ERROR(Status)) {
        return Status;
    }

    Status = pSmmBase->GetSmstLocation( pSmmBase, &pSmst );
    if (EFI_ERROR(Status)) {
        return Status;
    }

    Status = pSmst->SmmLocateProtocol(&gEfiSmmSxDispatch2ProtocolGuid,NULL,&SxDispatchProtocol);

    if (EFI_ERROR(Status)) { 
        return Status;
    }
    
    // Register the callback for S5 entry

    SxDispatchContext.Type  = SxS5;
    SxDispatchContext.Phase = SxEntry;
    Status = SxDispatchProtocol->Register(
        SxDispatchProtocol,
        S5SmiCallback,
        &SxDispatchContext,
        &SxDispatchHandle
        );
    ASSERT_EFI_ERROR (Status); 
   #if RTCWAKEUP_POWERBUTTON_SUPPORT
    // Register the callback for PowerButton event
    Status = pSmst->SmmLocateProtocol (
              &gEfiSmmPowerButtonDispatch2ProtocolGuid,
              NULL,
              &PBtnDispatch);
    if (EFI_ERROR(Status))
        PBtnDispatch = NULL;

    if (PBtnDispatch != NULL) {
        Status = PBtnDispatch->Register(
                  PBtnDispatch,
                  RTCPBtnSmiCallback,
                  &PBtnContext,
                  &SxDispatchHandle);
         if (EFI_ERROR(Status)) {
             ASSERT_EFI_ERROR (Status);  
             return Status;
         }
    }
#endif

    ASSERT_EFI_ERROR (Status);  

    return Status;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   NotInSmmFunction
//
// Description: Function is called from outside of SMM during SMM registration
//
// Input:
//  IN EFI_HANDLE           ImageHandle
//  IN EFI_SYSTEM_TABLE     *SystemTable
//
// Output: EFI_STATUS
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
/*
EFI_STATUS NotInSmmFunction(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
    return EFI_SUCCESS;
}
*/


//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   InitRTCWakeup
//
// Description: This is the entrypoint of the RTCWakeup driver.
//
// Input:
//  IN EFI_HANDLE       ImageHandle
//  IN EFI_SYSTEM_TABLE *SystemTable
//
// Output: EFI_STATUS
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
EFIAPI
InitRTCWakeup(
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable )
{
    EFI_STATUS  Status ;
    UINTN       Size = sizeof(SETUP_DATA) ;

    Status = InitAmiSmmLib( ImageHandle, SystemTable );
    if (EFI_ERROR(Status)) { 
        ASSERT_EFI_ERROR (Status);
        return Status;
    }

    Status = pRS->GetVariable( L"Setup", 
                               &gSetupGuid, 
                               NULL, 
                               &Size, 
                               &gSetupData );

    if(!EFI_ERROR(Status)){
        RtcWakeType=gSetupData.RtcWakeType;
        if (RtcWakeType == 0) {
            return EFI_NOT_FOUND;
        } else if (RtcWakeType == 1) {
            RTCWakeupTimeHour = gSetupData.FixedTimeHour ;
            RTCWakeupTimeMinute = gSetupData.FixedTimeMin ;
            RTCWakeupTimeSecond = gSetupData.FixedTimeSec ;
        } else if (RtcWakeType == 2) {
            DynamicMinIncrease= gSetupData.DynamicMinIncrease;
        }
    } else {
        return EFI_NOT_FOUND;
    }

    //Register RTC wakeup Handler
    Status = InitSmmHandler(ImageHandle, SystemTable, InSmmFunction, NULL);

    return Status;
}
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
