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

/** @file SbRuntimeServiceLib.c
    This file contains code for South Bridge runtime protocol

*/


// Module specific Includes
#include "Efi.h"
#include "token.h"
#include <AmiLib.h>
#include <AmiDxeLib.h>
#include <AmiCspLib.h>
#include <Guid/Rtc.h>
#include <Library/AmiChipsetRuntimeServiceLib.h>
#include <SbElinks.h>

#include <Library\ResetSystemLib.h> // Denverton AptioV override - EIP#238625
// Produced Protocols
#include <Protocol/Reset.h>
#include <Protocol/RealTimeClock.h>
#include <Protocol/Timer.h>

extern VOID CPULib_DisableInterrupt();
extern VOID CPULib_EnableInterrupt();
extern BOOLEAN CPULib_GetInterruptState();
//----------------------------------------------------------------------------
//          Variable Declaration
//----------------------------------------------------------------------------
EFI_EVENT ResetEvent;
static BOOLEAN Runtime = FALSE;
static BOOLEAN  gTimeOut = FALSE;
static UINT16 gTimeZone;
static UINT8  gDaylight;

//This the number of days in a month - 1. (0 Based)
UINT8       DaysInMonth[] = {30, 27, 30, 29, 30,\
                                29, 30, 30, 29, 30, 29, 30};


// Function Prototypes
typedef VOID (SB_RESET_CALLBACK) (
    IN EFI_RESET_TYPE           ResetType,
    IN EFI_STATUS               ResetStatus,
    IN UINTN                    DataSize,
    IN VOID                     *ResetData OPTIONAL
);

// Denverton AptioV Override Start- EIP#280221
VOID SwitchAlternateAccessMode (
    IN BOOLEAN      Switch
);
// Denverton AptioV Override End- EIP#280221

// Function Definitions
extern SB_RESET_CALLBACK SB_RESET_CALLBACK_LIST EndOfList;
SB_RESET_CALLBACK* SbResetCallbackList[] = { SB_RESET_CALLBACK_LIST NULL };

//----------------------------------------------------------------------------
//   USUALLY NO PORTING REQUIRED FOR THE FOLLOWING ROUTINES
//----------------------------------------------------------------------------


/**
    This function is the interface for the reset function.  In
    the future, this may allow for a shared library for DXE and PEI.

    @param ResetType - Type of reset to perform
    @param ResetStatus - System status that caused the reset.  if part
                         of normal operation then this should be
                         EFI_SUCCESS, Otherwise it should reflect the
                         state of the system that caused it
    @param DataSize - Size in bytes of the data to be logged
    @param ResetData - Pointer to the data buffer that is to be logged

    @retval None, Even though it should never get that far

**/
#if SB_RUNTIME_SUPPORT // Denverton Aptiov Override - EIP#210294 
EFI_STATUS
AmiChipsetResetSystem (
    IN EFI_RESET_TYPE   ResetType,
    IN EFI_STATUS       ResetStatus,
    IN UINTN            DataSize,
    IN CHAR16           *ResetData OPTIONAL
    )
{
    UINT32              Index;

    for (Index = 0; SbResetCallbackList[Index] != NULL; Index++) {
      SbResetCallbackList[Index](ResetType, ResetStatus, DataSize, ResetData);
    }
    // Denverton AptioV override start - EIP#238625
	// Make use of ResetSystemLib functions to perform the reset opeartions
    switch (ResetType) {
    case EfiResetWarm:
      AsmWbinvd ();    // Denverton AptioV Override - EIP#326406
      ResetWarm ();
      break;
    case EfiResetCold:
      ResetCold ();
      break;
    case EfiResetShutdown:
      ResetShutdown ();
      break;
    default:
      break;
    }

    TRACE((TRACE_ALWAYS, "Resetting system...\n"));
    EFI_DEADLOOP();
	// Denverton AptioV override end - EIP#238625
    // This should not get here
    return EFI_SUCCESS;
}
#endif // Denverton Aptiov Override - EIP#210294 
/**
    This function converts data from DEC to BCD format

    @param Dec - Value to be converted

    @retval Result of conversion

**/
UINT8 DecToBCD(
    IN UINT8 Dec
)
{
    UINT8 FirstDigit = Dec % 10;
    UINT8 SecondDigit = Dec / 10;

    return (SecondDigit << 4) + FirstDigit;
}

/**
    This function converts data from BCD to DEC format

    @param BCD - Value to be converted

    @retval Result of conversion

**/
UINT8 BCDToDec(IN UINT8 BCD)
{
    UINT8 FirstDigit = BCD & 0xf;
    UINT8 SecondDigit = BCD >> 4;;

    return SecondDigit * 10  + FirstDigit;
}

/**
    Read the RTC value at the given Index.

    @param Index - RTC Index

    @retval RTC Value read from the provided Index

    @note  Here is the control flow of this function:
              1. Read port 0x70 (RTC Index Register) to get bit 7.
                 Bit 7 is the NMI bit-it should not be changed.
              2. Set Index with the NMI bit setting.
              3. Output 0x70 with the Index.
              4. Read 0x71 for Data.
              5. Return the Data.
**/
UINT8 ReadRtcIndex(
    IN UINT8 Index
)
{
    // Read bit 7 (NMI setting).
    UINT8           NMI;
    volatile UINT8  Value;
    BOOLEAN         IntState = CPULib_GetInterruptState();

    CPULib_DisableInterrupt();

    Index &= ~RTC_NMI_MASK;

    // Enable Alternate access mode, let port 70h can be readable.
//    SbLibEnableAltAccessMode (TRUE);
    
    SwitchAlternateAccessMode(TRUE);         // Denverton AptioV Override - EIP#280221

    NMI = IoRead8(RTC_INDEX_REG) & RTC_NMI_MASK;

    // Disable Alternate access mode.
    
    SwitchAlternateAccessMode(FALSE);        // Denverton AptioV Override - EIP#280221
//    SbLibEnableAltAccessMode (FALSE);

    IoWrite8(RTC_INDEX_REG, Index | NMI);
    Value = IoRead8(RTC_DATA_REG); // Read register.

    if (IntState) CPULib_EnableInterrupt();

    if ((Index <= RTC_YEAR_REG) || (Index == ACPI_CENTURY_CMOS))
        Value = BCDToDec(Value);

    return (UINT8)Value;
}

/**
    Write the RTC value at the given Index.

    @param Index - RTC Index
    @param Value - Value to be written

    @retval VOID

    @note  Here is the control flow of this function:
              1. Read port 0x70 (RTC Index Register) to get bit 7.
                 Bit 7 is the NMI bit-it should not be changed.
              2. Set Index with the NMI bit setting.
              3. Output 0x70 with the Index.
              4. Write the data to 0x71.
**/
VOID WriteRtcIndex(
    IN UINT8 Index,
    IN UINT8 Value
)
{
    // Read bit 7 (NMI setting).
    UINT8   NMI;
    BOOLEAN IntState = CPULib_GetInterruptState();

    if ((Index <= RTC_YEAR_REG) || (Index == ACPI_CENTURY_CMOS))
        Value = DecToBCD(Value);

    CPULib_DisableInterrupt();

    Index &= ~RTC_NMI_MASK;

    // Enable Alternate access mode, let port 70h can be readable.
//    SbLibEnableAltAccessMode (TRUE);

    NMI = IoRead8(RTC_INDEX_REG) & RTC_NMI_MASK;

    // Disable Alternate access mode.
//    SbLibEnableAltAccessMode (FALSE);

    IoWrite8(RTC_INDEX_REG, Index | NMI);
    IoWrite8(RTC_DATA_REG, Value); // Write Register.

    if (IntState) CPULib_EnableInterrupt();
}

/**
    This function initializes RTC

    @param VOID

    @retval VOID

**/
VOID InitRtc(VOID)
{
    WriteRtcIndex(RTC_REG_B_INDEX, 0x82);
    WriteRtcIndex(RTC_REG_A_INDEX, 0x26);
    ReadRtcIndex(RTC_REG_C_INDEX);
    ReadRtcIndex(RTC_REG_D_INDEX);
    WriteRtcIndex(RTC_REG_B_INDEX, 0x02);
}

/**
    Enables Disables RTC Date and Time update cicles.

    @param Enable TRUE or FALSE to Enable\Disable RTC Update.

    @retval VOID

**/
VOID SetUpdate(
    IN BOOLEAN Enable
)
{
    RTC_REG_B       RegB;
    UINT8           Set = (Enable) ? 0 : 1;

    RegB.REG_B = ReadRtcIndex(RTC_REG_B_INDEX);
    if (RegB.Set != Set) {
        RegB.Set = Set;
        WriteRtcIndex(RTC_REG_B_INDEX, RegB.REG_B);
    }
}

/**
    Check if RTC Date and Time update in progress and waits till
    it's over.

    @param VOID

    @retval VOID

**/
VOID CheckUpdateCmplete (VOID)
{
    volatile RTC_REG_A  RegA;
    UINTN               TimeOut = 0;

    RegA.REG_A = ReadRtcIndex(RTC_REG_A_INDEX);
    while (RegA.UpdInProgr) {
        RegA.REG_A = ReadRtcIndex(RTC_REG_A_INDEX);
        TimeOut++;
        if (TimeOut >= 0x0fffff) {
            gTimeOut = TRUE;
            return;
        }
    }

    gTimeOut = FALSE;
}

/**
    Returns an index that represents the day of the week of the date passed in

    @param Time - Pointer to EFI_TIME structure

    @retval Returns the index to the day of the week.
            0 = Sunday, 1 = Monday ... 6 = Saturday

**/
UINT8 GetDayOfTheWeek (
    IN EFI_TIME         *Time
)
{
    UINT16      a;
    UINT16      m;
    UINT16      d;
    UINT16      y;

    a = (14 - Time->Month) / 12;
    y = Time->Year - a;
    m = Time->Month + 12 * a - 2;
    d = (Time->Day + y + y / 4 - y / 100 + y / 400 + (31 * m) / 12) % 7;

    return (UINT8)d;
}

/**
    This routine verifies if time and data if needed, before
    setting the RTC

    @param Time - Time to verify with

    @retval TRUE - if valid time and date

    @note  Here is the control flow of this function:
              1. Decrease month and date to change to 0-base
              2. Validate Year, Month and Day. If invalid, return FALSE.
              3. Validate Hour, Minute, and Second. If invalid, return FALSE.
              4. Return True.

**/
BOOLEAN VerifyTime(
    IN EFI_TIME *Time
)
{
    //Always check these to satisfy EFI compliancy test even for setting wake-up time.
    UINT8 Month = Time->Month - 1;
    UINT8 Day   = Time->Day - 1;

    if (Time->Year < MINIMUM_YEAR || Time->Year > MAXIMUM_YEAR) return FALSE;
    if (Month > 11) return FALSE;             //0 based month
    if (Month != 1 || (Time->Year & 3)) { //not leap year or not February.
        if (Day > DaysInMonth[Month]) return FALSE; //All values already adjusted for 0 based.
    } else {
        if (Day > 28) return FALSE; //February
    }

    if (Time->Hour > 23) return FALSE;
    if (Time->Minute > 59) return FALSE;
    if (Time->Second > 59) return FALSE;

    //Check these to satisfy EFI compliancy test.
    if (Time->Nanosecond > 999999999) return FALSE; //999,999,999
    if (Time->TimeZone < -1440) return FALSE;
    if (Time->TimeZone > 1440 && Time->TimeZone != \
                        EFI_UNSPECIFIED_TIMEZONE) return 0;

    return TRUE;
}

/**
    Check if RTC Mode and Format have appropriate values and sets
    them if necessary

    @param Set - if true, force RTC to 24 hour mode and binary format

    @retval EFI_SUCCESS RTC mode and format have appropriate values
    @retval EFI_DEVICE_ERROR RTC mode and/or format are invalid

**/
EFI_STATUS CheckRtc(
    IN BOOLEAN Set
)
{
    RTC_REG_B   RegB;

    //Check RTC Conditions and stuff
    RegB.REG_B = ReadRtcIndex(RTC_REG_B_INDEX);

    if(RegB.Mode == 0 || RegB.Format == 1) {
        if(Set) {
            RegB.Mode = 1;                  // 0 - 12 hour mode              1 - 24 hour mode
            RegB.Format = 0;                // 0 - BCD Format                1 - Binary Format
            WriteRtcIndex(RTC_REG_B_INDEX,RegB.REG_B);
        } else {
            return EFI_DEVICE_ERROR;
        }
    }
    return EFI_SUCCESS;
}

/**
    Return the current date and time

    @param Time - Current time filled in EFI_TIME structure
    @param Capabilities - Time capabilities (OPTIONAL)

    @retval EFI_SUCCESS

    @note  Here is the control flow of this function:
              1. Read the original time format 12/24 hours and BCD/binary.
              2. Set the format to 24 hrs and binary.
              3. Read the 2 digit year.
              4. Add either 1900 or 2000, so the year is between 1998 - 2097.
              5. Read the month, day, hour, minute, second.
              6. Set the nanosecond to 0.
              7. Set the time to zone to unspecified.
              8. Set daylight savings value to 0.
              9. Restore the original time format.
              10. Set Capabilities with 1 sec Resolution, 0 Accuracy (Unknown), and False SetsToZero.
              11. Return EFI_SUCCESS.

**/
EFI_STATUS AmiChipsetGetTime (
    OUT EFI_TIME                *Time,
    OUT EFI_TIME_CAPABILITIES   *Capabilities OPTIONAL
)
{
    EFI_STATUS    Status;
    UINT8         Year;
    BOOLEAN       IntState, SmiState;
    UINTN         TimeVarSize = sizeof(TIME_VARIABLE);
    TIME_VARIABLE TimeVar;
//-------------------------
    if (Time == NULL) return EFI_INVALID_PARAMETER;
    //Check RTC Conditions (24h Mode and BCD is ON)
    Status=CheckRtc(FALSE);
    if(EFI_ERROR(Status)) return Status;
    //Get SMI State and disable it
    SmiState=SbLib_GetSmiState();
    SbLib_SmiDisable();
    //Get INTERRUPT State and disable it
    IntState = CPULib_GetInterruptState();
    CPULib_DisableInterrupt();
    // Wait till RTC is safe to read,
    CheckUpdateCmplete();
    if(gTimeOut) {
        InitRtc();
    }

    // After control comes back, we will have 488 u's to read data.
    Year            = ReadRtcIndex(RTC_YEAR_REG);
    Time->Month     = ReadRtcIndex(RTC_MONTH_REG);
    Time->Day       = ReadRtcIndex(RTC_DAY_OF_MONTH_REG);
    Time->Hour      = ReadRtcIndex(RTC_HOURS_REG);
    Time->Minute    = ReadRtcIndex(RTC_MINUTES_REG);
    Time->Second    = ReadRtcIndex(RTC_SECONDS_REG);

    //Restore SMIs and INTERRUPT State
    if(IntState) CPULib_EnableInterrupt();
    if(SmiState) SbLib_SmiEnable();

    //This Register is not affected by UIP bit so read it very last.
    // If RTC Year only 1 digit, EFI spec says years rang is 1998 - 2097
    Time->Year = ReadRtcIndex(ACPI_CENTURY_CMOS) * 100 + Year;
    Time->Nanosecond= 0;

    Status = pRS->GetVariable(
                        L"EfiTime",
                        &gEfiTimeVariableGuid,
                        NULL,
                        &TimeVarSize,
                        &TimeVar
                            );
    if (EFI_ERROR(Status))
    {
        Time->TimeZone  = EFI_UNSPECIFIED_TIMEZONE;
        Time->Daylight  = 0;
    }
    else
    {
        Time->TimeZone  = TimeVar.TimeZone;
        Time->Daylight  = TimeVar.Daylight;
    }

    gTimeZone = Time->TimeZone;
    gDaylight = Time->Daylight;

    if (Capabilities != NULL) {
        Capabilities->Resolution = 1;
        Capabilities->Accuracy = 0;
        Capabilities->SetsToZero = 0;
    }

    return  EFI_SUCCESS;
}

/**
    Sets the RTC time

    @param Time - Time to set

    @retval EFI_SUCCESS Time is Set
    @retval EFI_INVALID_PARAMETER Time to Set is not valid.

    @note  Here is the control flow of this function:
              1. Read the original time format 12/24 hours and BCD/binary.
              2. Set the format to 24 hrs and binary.
              3. Verify the time to set. If it is an invalid time,
                 restore the time format and return EFI_INVALID_PARAMETER.
              4. Change the 4 digit year to a 2 digit year.
              5. Stop the RTC time.
              6. Store time and data on the RTC.
              7. Read the month, day, hour, minute, second.
              8. Start the RTC time.
              9. Restore the original time format.
              10. Return EFI_SUCCESS.

**/
EFI_STATUS AmiChipsetSetTime (
    IN EFI_TIME     *Time
)
{
    EFI_STATUS    Status = EFI_SUCCESS;
    UINTN         TimeVarSize = sizeof(TIME_VARIABLE);
    TIME_VARIABLE TimeVar;

    //Check RTC Conditions and stuff
    CheckRtc(TRUE);

    if (Time == NULL) return EFI_INVALID_PARAMETER;
    if (!VerifyTime(Time)) return EFI_INVALID_PARAMETER;

    SetUpdate(FALSE);
    WriteRtcIndex(ACPI_CENTURY_CMOS, Time->Year / 100);
    WriteRtcIndex(RTC_YEAR_REG, Time->Year % 100);
    WriteRtcIndex(RTC_MONTH_REG,Time->Month);
    WriteRtcIndex(RTC_DAY_OF_MONTH_REG,Time->Day);
    // calculate the day of the week. Add 1, because Algorithm is 0 based and RTC is 1 based
    WriteRtcIndex(RTC_DAY_OF_WEEK_REG, GetDayOfTheWeek(Time) + 1);
    WriteRtcIndex(RTC_HOURS_REG,Time->Hour);
    WriteRtcIndex(RTC_MINUTES_REG,Time->Minute);
    WriteRtcIndex(RTC_SECONDS_REG,Time->Second);
    SetUpdate(TRUE);

    Status = pRS->GetVariable(
                        L"EfiTime",
                        &gEfiTimeVariableGuid,
                        NULL,
                        &TimeVarSize,
                        &TimeVar
                        );

    if (EFI_ERROR(Status) || (TimeVar.TimeZone != Time->TimeZone) || (TimeVar.Daylight != Time->Daylight))
    {
        TimeVar.TimeZone  = Time->TimeZone;
        TimeVar.Daylight  = Time->Daylight;
        Status = pRS->SetVariable(
                        L"EfiTime",
                        &gEfiTimeVariableGuid,
                        EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                        sizeof(TIME_VARIABLE),
                        &TimeVar
                        );
        if (!EFI_ERROR(Status)) {
          gTimeZone = Time->TimeZone;
          gDaylight = Time->Daylight;
        }

    }
    return  Status;
}

/**
    Read the wake time. Read the status if it is enabled or
    if the system has woken up.   

    @param Enabled Flag indicating the validity of wakeup time
    @param Pending Check if wake up time has expired.
    @param Time Current wake up time setting

    @retval EFI_STATUS

    @note  Here is the control flow of this function:
              1. Read the original time format 12/24 hours and BCD/binary.
              2. Set the format to 24 hrs and binary.
              3. Read the status if the wake up time is enabled or if it has expired.
              4. Set the wakeup time.
              5. Restore the original time format.
              6. Return EFI_SUCCESS.

**/
EFI_STATUS AmiChipsetGetWakeupTime (
    OUT BOOLEAN     *Enabled,
    OUT BOOLEAN     *Pending,
    OUT EFI_TIME    *Time
)
{
    RTC_REG_B               RegB;
    RTC_REG_C               RegC;
    BOOLEAN                 IntState, SmiState;
    EFI_STATUS              Status;
    UINT8                   Year;
    EFI_TIME                RtcTime;    
    UINTN                   DataSize;
#if ACPI_ALARM_DAY_CMOS
    RTC_DATE_ALARM_REG      RegDateAlarm;
    UINT8                   Day;
#endif
#if ACPI_ALARM_MONTH_CMOS
    RTC_MONTH_ALARM_REG     RegMonthAlarm;
    UINT8                   Month;
#endif


    if (!Enabled || !Pending || !Time)
        return EFI_INVALID_PARAMETER;

    //Check RTC Conditions (24h Mode and BCD is ON)
    Status = CheckRtc(FALSE);
    if(EFI_ERROR(Status))
        return Status;

    //Get SMI State and disable it
    SmiState = SbLib_GetSmiState();
    SbLib_SmiDisable();

    //Get INTERRUPT State and disable it
    IntState = CPULib_GetInterruptState();
    CPULib_DisableInterrupt();

    // Wait till RTC is safe to read,
    CheckUpdateCmplete();
    if(gTimeOut)
        InitRtc();

    Time->Hour   = ReadRtcIndex(RTC_HOURS_ALARM_REG);
    Time->Minute = ReadRtcIndex(RTC_MINUTES_ALARM_REG);
    Time->Second = ReadRtcIndex(RTC_SECONDS_ALARM_REG);

    //Restore SMIs and INTERRUPT State
    if(IntState)
        CPULib_EnableInterrupt();

    if(SmiState)
        SbLib_SmiEnable();

    Time->Month = ReadRtcIndex(RTC_MONTH_REG);
    Time->Day = ReadRtcIndex(RTC_DAY_OF_MONTH_REG);
#if ACPI_ALARM_MONTH_CMOS
    RegMonthAlarm.REG_MONTH_ALARM = ReadRtcIndex(ACPI_ALARM_MONTH_CMOS);
    Month = BCDToDec(RegMonthAlarm.MonthAlarm);
    if ((Month < 13) && (Month > 0)) Time->Month = Month;
#endif
#if ACPI_ALARM_DAY_CMOS
    RegDateAlarm.REG_DATE_ALARM = ReadRtcIndex(ACPI_ALARM_DAY_CMOS);
    Day = BCDToDec(RegDateAlarm.DateAlarm);
    if ((Day < 32) && (Day > 0)) Time->Day = Day;
#endif

    RegB.REG_B = ReadRtcIndex(RTC_REG_B_INDEX);
    RegC.REG_C = ReadRtcIndex(RTC_REG_C_INDEX);

    *Enabled = RegB.AlarmInt;
    *Pending = RegC.AlarmFlag;

    Year = ReadRtcIndex(RTC_YEAR_REG);
    Time->Year = ReadRtcIndex(ACPI_CENTURY_CMOS) * 100 + Year;
    Time->TimeZone = gTimeZone;
    Time->Daylight = gDaylight;
    Time->Nanosecond = 0;

    //
    // Get the alarm info from variable
    //    
    DataSize = sizeof (EFI_TIME);
    Status = pRS->GetVariable (
                   L"RTCALARM",
                   &gEfiTimeVariableGuid,
                   NULL,
                   &DataSize,
                   &RtcTime
                   );
    if (!EFI_ERROR (Status)) {
      Time->Year     = RtcTime.Year;
      Time->TimeZone = RtcTime.TimeZone;
      Time->Daylight = RtcTime.Daylight;
    }
    
    return EFI_SUCCESS;

}


/**
    Enable/disable and set wakeup time

    @param Enable Flag indicating whether to enable/disable the time
    @param Time Time to set

    @retval EFI_STATUS
            EFI_SUCCESS           Time is Set and/or Enabled/Disabled.
            EFI_INVALID_PARAMETER Invalid time or enabling with a NULL Time.

    @note  Here is the control flow of this function:
              1. Read the original time format 12/24 hours and BCD/binary.
              2. If Time is not NULL,
                  a. Verify the wakeup time to set. If it is an invalid time,
                     restore the time format and return EFI_INVALID_PARAMETER.
                  b. Set the wakeup time.
              3. If Time is NULL and Enable is true, restore original time 
                 format and return EFI_INVALID_PARAMETER.
              4. Enable/Disable wakeup.
              5. Restore the original time format.
              6. Return EFI_SUCCESS.

**/
EFI_STATUS AmiChipsetSetWakeupTime (
    IN BOOLEAN      Enable,
    IN EFI_TIME     *Time OPTIONAL
)
{
    RTC_REG_B               RegB;
    UINT8                   Day = 0;
    EFI_STATUS              Status;
    EFI_TIME                RtcTime = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
#if ACPI_ALARM_DAY_CMOS
    RTC_DATE_ALARM_REG      RegDateAlarm;
#endif
#if ACPI_ALARM_MONTH_CMOS
    RTC_MONTH_ALARM_REG     RegMonthAlarm;
#endif


    //Check RTC Conditions (24h Mode and BCD is ON)
    Status = CheckRtc(FALSE);
    if(EFI_ERROR(Status))
        return Status;

    if (Time != NULL) {
        //Some Work around for the Day Alarm
        //it is uses 6 bits 0..5 to program day of month.
        //but 0 is valid also. it means every day.
#if ACPI_ALARM_EVERYDAY_SUPPORT
        Day = Time->Day;
        if (Day == 0) Time->Day = 1;
#endif
        if (!VerifyTime(Time)) return EFI_INVALID_PARAMETER;
#if ACPI_ALARM_EVERYDAY_SUPPORT
        if (Day != Time->Day) Time->Day = 0;
#endif
    } else {
      if (Enable) return EFI_INVALID_PARAMETER;
    }

    RegB.REG_B = ReadRtcIndex(RTC_REG_B_INDEX);

    SetUpdate(FALSE);

    if(Time != NULL) {
        WriteRtcIndex(RTC_HOURS_ALARM_REG,Time->Hour);
        WriteRtcIndex(RTC_MINUTES_ALARM_REG,Time->Minute);
        WriteRtcIndex(RTC_SECONDS_ALARM_REG,Time->Second);
#if ACPI_ALARM_DAY_CMOS
        // Day == 0 means don't care
        RegDateAlarm.DateAlarm = DecToBCD(Time->Day);
        WriteRtcIndex(ACPI_ALARM_DAY_CMOS, RegDateAlarm.REG_DATE_ALARM);
#endif
#if ACPI_ALARM_MONTH_CMOS
        // Month == 0 means don't care
        RegMonthAlarm.MonthAlarm = DecToBCD(Time->Month);
        WriteRtcIndex(ACPI_ALARM_MONTH_CMOS, RegMonthAlarm.REG_MONTH_ALARM);
#endif
    }
    //Set Enable/Disable
    RegB.AlarmInt = Enable;
    WriteRtcIndex(RTC_REG_B_INDEX,RegB.REG_B);

    SetUpdate(TRUE);
    
    //
    // Set the Y/M/D info to variable as it has no corresponding HW registers.
    //
    if (Enable) {  
      RtcTime.Year     = Time->Year;
      RtcTime.TimeZone = Time->TimeZone;
      RtcTime.Daylight = Time->Daylight;
      Status =  pRS->SetVariable (
                       L"RTCALARM",
                       &gEfiTimeVariableGuid,
                       EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                       sizeof (RtcTime),
                       &RtcTime
                       );
    }

    return EFI_SUCCESS;
}

/**
    This function is the entry point for this DXE Driver. This function
    make the runtime services in SMM.

    @param ImageHandle Image handle
    @param SystemTable pointer to the UEFI System Table

    @retval Return Status based on errors generated by GetTime function.

**/
EFI_STATUS
SbRuntimeServiceLibConstructor (
  IN EFI_HANDLE			ImageHandle,
  IN EFI_SYSTEM_TABLE	*SystemTable
  )
{
	EFI_STATUS      Status = EFI_SUCCESS;

// Denverton AptioV override Start - EIP#263420	
//    EFI_TIME        Time;
// Denverton AptioV override End - EIP#263420

    InitAmiLib(ImageHandle, SystemTable);

// Denverton AptioV override Start - EIP#263420
/*
    //MakeSure Mode, Format and REG_A is OK
    CheckRtc(TRUE);

    Status = AmiChipsetGetTime(&Time,NULL);

    if( EFI_ERROR(Status) || (!VerifyTime(&Time)))
    {
        EFI_TIME nt;

        #if defined(GENERIC_BAD_DATE_TIME_ERR_CODE_SUPPORT) && (GENERIC_BAD_DATE_TIME_ERR_CODE_SUPPORT != 0)
        ERROR_CODE(GENERIC_BAD_DATE_TIME_ERROR,  EFI_ERROR_MINOR);
        #endif

        TRACE((TRACE_ALWAYS, "\n\nTime: %d/%d/%d  %d:%d:%d\n",
            Time.Month,
            Time.Day,
            Time.Year,
            Time.Hour,
            Time.Minute,
            Time.Second
        ));

        TRACE((TRACE_ALWAYS, "Nanosecond: %d TimeZone: %d\n\n\n",
            Time.Nanosecond,
            Time.TimeZone
        ));

        //if Time is invalid the battery probably has been removed
        //Let's setup RTC_REG_A just in case...
        WriteRtcIndex(RTC_REG_A_INDEX, 0x26);

        //Check to see what part of EFI_TIME was wrong.
        //reset unrelated to RTC fields.
        Time.TimeZone = EFI_UNSPECIFIED_TIMEZONE;
        Time.Daylight = 0;
        Time.Nanosecond = 0;

        nt = Time;

        nt.Hour   = 0;
        nt.Minute = 0;
        nt.Second = 0;

        if(VerifyTime(&nt)) {
            //if we here that means Time was wrong
            Time.Hour   = 0;
            Time.Minute = 0;
            Time.Second = 0;
        } else {
            //if we here that means Date was wrong
            Time.Month = DEFAULT_MONTH;
            Time.Day   = DEFAULT_DAY;
            Time.Year  = DEFAULT_YEAR;
        }
        //Here is the situation when both Time and Date is Incorrect.
        if(!VerifyTime(&Time)) {
            Time.Hour   = 0;
            Time.Minute = 0;
            Time.Second = 0;
            Time.Month = DEFAULT_MONTH;
            Time.Day   = DEFAULT_DAY;
            Time.Year  = DEFAULT_YEAR;
        }

        TRACE((TRACE_ALWAYS, "Reseting Invalid Date and Time to: %d/%d/%d  %d:%d:%d\n\n",
            Time.Month,
            Time.Day,
            Time.Year,
            Time.Hour,
            Time.Minute,
            Time.Second
         ));
        AmiChipsetSetTime(&Time);
    }
*/
// Denverton AptioV override End - EIP#263420	
    return Status;
}

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
