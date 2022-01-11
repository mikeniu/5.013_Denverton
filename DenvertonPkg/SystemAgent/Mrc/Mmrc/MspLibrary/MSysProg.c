/** @file
  MSysProg.c 
   
  Copyright (c) 2005-2015 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

**/

#include "MSysProg.h"
#include "RegAccess.h"
#ifdef MSP_DXE_EXECUTION
  #include "MspPrintf.h"
#else
  #include "Printf.h"
#endif

#if defined(MSP_DXE_EXECUTION) || defined(SIM) || defined(JTAG)
  MSP_DATA *MyMspData;
#endif


/**
  It delays the execution in the MRC for 'DelayUs' microseconds
  The delay must never be between 0-9 microseconds 
  in order to do not create an overlap in the HPET and max value
  should be ('desired' / 0xF) < (2^32 - 1)

  @param[in]       DelayUs         Delay specified in microseconds.
  @retval          NONE
**/
VOID
McStall (
  IN    UINT32  DelayUs
  )
{
#if defined SIM || defined JTAG
  return;
#elif CAR
	DelayNano(DelayUs * 1000);
#else

  UINT32   Start;
  UINT32   Finish;
  UINT32   Now;
  BOOLEAN  Done;

  if (DelayUs > 0x11111110) {
    DelayUs = 0x11111110;
  } else if (DelayUs < 10) {
    DelayUs = 10;
  }
  DelayUs = (DelayUs * HPET_1US) / 1000;

  Done = FALSE;

  Start = Mmio32Read (ICH_HPET_BASE_ADDRESS + 0xF0);
  Finish = Start + DelayUs;

  do {
    Now = Mmio32Read (ICH_HPET_BASE_ADDRESS + 0xF0);
    if (Finish > Start) {
      if (Now >= Finish) {
        Done = TRUE;
      }
    } else {
      if ((Now < Start) && (Now >= Finish) ) {
        Done = TRUE;
      }
    }
  } while (!Done);

  return;
#endif
}

UINT16    mTimerLibLocalApicFrequencies[] = {
  100,
  133,
  200,
  167,
  83,
  400,
  267,
  333
};

UINT8     mTimerLibLocalApicDivisor[] = {
  0x02, 0x04, 0x08, 0x10,
  0x02, 0x04, 0x08, 0x10,
  0x20, 0x40, 0x80, 0x01,
  0x20, 0x40, 0x80, 0x01
};

UINTN
AsmReadApicBase()
{
#ifdef MSP_DXE_EXECUTION
  return 0xfee00900;
#else
  UINT32 Data32;
  Data32 = 0;
#ifndef SIM

  _asm {

    push    eax
    push    ebx
    push    ecx
    push    edx

    mov     ecx, 27
    rdmsr
    mov     Data32, eax

    pop     edx
    pop     ecx
    pop     ebx
    pop     eax
  }
#endif
  return Data32;
#endif
}

UINT32
AsmReadApicFreq()
{
#ifdef MSP_DXE_EXECUTION
  return 1;
#else
  UINT32  Data32;
  Data32 = 0;
#ifndef SIM
  _asm {

    push    eax
    push    ebx
    push    ecx
    push    edx

    mov     ecx, 205
    rdmsr
    mov     Data32, eax

    pop     edx
    pop     ecx
    pop     ebx
    pop     eax
  }
#endif
  return Data32;
#endif
}


UINT8
ReadApicFreqOffset()
{
  UINT32  ApicFreq;

  ApicFreq = (UINT32) AsmReadApicFreq ();
  ApicFreq = ((ApicFreq & 0x00070000) >> 16);

  return (UINT8) ApicFreq;
}

INT32
InternalX86GetTimerTick (
  UINTN                     ApicBase
)
{
  return Mmio32Read (ApicBase + 0x390);
}

/**
  It delays the execution in the MRC for 'DelayNs' nanoseconds
  There is a restriction this delay should be a multiple 50

  @param[in]       DelayNs         Delay specified in nanoseconds. 
                                   It should be a mutiple of 50
  @retval          NONE
**/
VOID
McStallNanoSecond (
  IN        UINT32    DelayNs
  )
{
#if 0
#if ( SIM )
  return;
#elif CAR
	DelayNano(DelayNs);
#else
  UINTN             ApicBase;
  UINT32            Buffer32;
  UINT32            Ticks;
  LOCAL_APIC_DCR    Dcr;
  UINT32            PsfClock;
  UINT32            Divisor;

  ApicBase = (AsmReadApicBase() & 0xFFFFF000);
  Dcr.Data32 =  Mmio32Read (ApicBase + 0x3e0);
  Divisor = Dcr.Bits.DivideValue1 | (Dcr.Bits.DivideValue2 << 2);
  Divisor = (Divisor + 1) & 0x7;
  Divisor = (1 << Divisor);

  PsfClock = mTimerLibLocalApicFrequencies[ReadApicFreqOffset()];
  Buffer32 = (PsfClock / Divisor);
  //
  // The target timer count is calculated here
  // RoundUp the delay, and convert to a multiple of '50'(x)
  // 1Seg / '20'(y) = 50ms / '1000000'(z) (due to adjust in mTimerLibLocalApicFrequencies) = 50nS
  // x * y * z = 1000000000
  //
  DelayNs = (DelayNs + 25) / 50;
  Ticks = ((Buffer32 * DelayNs) / 20);
  Mmio32Write (ApicBase + 0x380, Ticks);

  while (InternalX86GetTimerTick (ApicBase) != 0);
#endif
#else
  VOLATILE UINT32 Temp;

  for (Temp = 0;Temp < (DelayNs); Temp++) {
  }
#endif
}

MSP_STATUS 
MspDelay (
  IN        UINT8   Type,
  IN        UINT32  Delay
  )
{
#if !defined SIM  && !defined JTAG
  UINT32  TimeCounter;

  switch (Type) {
  case MILLI_DEL:
    for (TimeCounter = 0; TimeCounter < Delay; TimeCounter++) {
      McStall (1000);
    }
    break;
  case MICRO_DEL:
    McStall (Delay);
    break;  
  case NANO_DEL:
    McStallNanoSecond (Delay / 16);
    break;
  default:
    McStall (Delay);
    break;
  }
#endif  
  return MSP_SUCCESS;
}

/**
  Enables the High Precision Event Timer

  @param[in, out]  MrcData         Host structure for all data related to MMRC. 
  @param[in]       Channel         Channel under examination
  @retval          Status
**/
MSP_STATUS
McEnableHpet (
  IN  OUT   MSP_DATA   *MspData
  )
{
//  VOLATILE  UINT32  Data32;
//  UINT32 IchRcbaBaseAddr;

#if defined SIM || defined JTAG
  return MSP_SUCCESS;
#endif
  if (MspData->HpetHptcSave != 0) {
    return MSP_SUCCESS;
  }

#if 0 // wrong?
  IchRcbaBaseAddr = PCI_CFG_32B_READ_CF8CFC (
                      PCI_BUS_NUMBER_ICH,
                      PCI_DEVICE_NUMBER_ICH_LPC,
                      PCI_FUNCTION_NUMBER_ICH_LPC,
                      R_ICH_LPC_RCBA
                    );

  IchRcbaBaseAddr &= ~BIT0;
  //
  // Save current contents of HPET Control register (RCRB_HPTC) for restoration later
  //
  MspData->HpetHptcSave = Mmio32Read (IchRcbaBaseAddr + ICH_RCRB_HPTC);
  //
  // Enable ICH HPET hardware to support timers
  // Set R_HPTC[1:0] (HPTC_AS) = 00b to select ICH_HPET_BASE_ADDRESS decode region
  // Set R_HPTC[7]   (HPTC_AE) =  1b to enable address decode
  //
  Mmio32Write (IchRcbaBaseAddr + ICH_RCRB_HPTC, ((MspData->HpetHptcSave & ~ICH_RCRB_HPTC_AS) | ICH_RCRB_HPTC_AE) );
  Data32 = Mmio32Read (IchRcbaBaseAddr + ICH_RCRB_HPTC);
#endif

  MspData->HpetHptcSave = ICH_HPET_BASE_ADDRESS;
  //
  // Set HPET Timer enable to start counter spinning
  //
  Mmio32Or (ICH_HPET_BASE_ADDRESS + 0x10, 0x1);

  return MSP_SUCCESS;
}

/**
  Disables the High Precision Event Timer

  @param[in, out]  MrcData         Host structure for all data related to MMRC. 
  @param[in]       Channel         Channel under examination
  @retval          Status
**/
MSP_STATUS
McDisableHpet (
  IN  OUT   MSP_DATA   *MrcData
  )
{

  VOLATILE UINT32 Data32;
//  UINT32 IchRcbaBaseAddr;
#if defined SIM || defined JTAG
  return MMRC_SUCCESS;
#endif
#if 0
  IchRcbaBaseAddr = PCI_CFG_32B_READ_CF8CFC (
                      PCI_BUS_NUMBER_ICH,
                      PCI_DEVICE_NUMBER_ICH_LPC,
                      PCI_FUNCTION_NUMBER_ICH_LPC,
                      R_ICH_LPC_RCBA
                    );

  IchRcbaBaseAddr &= ~BIT0;

#endif

  // Clear HPET Timer enable to stop counter spinning
  Mmio32And (ICH_HPET_BASE_ADDRESS + 0x10, ~ (BIT1 | BIT0) );
  Data32 = Mmio32Read (ICH_HPET_BASE_ADDRESS + 0x10); // Read back to flush posted write

#if 0
  //
  // Restore HPET Configuration register
  //
  Mmio32Write (IchRcbaBaseAddr + ICH_RCRB_HPTC, MrcData->HpetHptcSave);
  Data32 = Mmio32Read (ICH_HPET_BASE_ADDRESS + 0x10); // Read back to flush posted write
#endif
  return MSP_SUCCESS;
}


/**
  Saves the MSP host structure to an MMX register for future 
  use. 

  @param[in, out]  MspDataAddress  Address of MSP Host 
        Structure.
  @retval          NONE
**/
VOID
SaveMspHostStructureAddress (
  IN    MSP_DATA   *MspDataAddress
  )
{
#if SIM || JTAG || (defined(MSP_DXE_EXECUTION) && (MSP_DXE_EXECUTION==1))
  MyMspData = MspDataAddress;
#else
#if __GNUC__
  asm (
    "movd %0,%%mm2;"
    :/* no output */
    : "c" (MspDataAddress)
    :/* no clobber */
    );
#else
  _asm {
    movd mm2, MspDataAddress;
  }
#endif
#endif
}

/**
  Returns the MSP host structure from an MMX register.

  @retval          MrcDataAddress  Address of MRC Host Structure.
**/
MSP_DATA *
GetMspHostStructureAddress (
  )
{
#if SIM || JTAG || (defined (MSP_DXE_EXECUTION) && (MSP_DXE_EXECUTION==1))
  return MyMspData;
#else
  MSP_DATA *MspData;
#if __GNUC__
  asm (
    "movd %%mm2,%0;"
    : "=c" (MspData)
    :/* no input */
    :/* no clobber */
    );
#else
  _asm {
    movd MspData, mm2;
  }
#endif
  return MspData;
#endif
}

UINTX
ByteEnableToUintX (
  IN       UINT8    Be, 
  IN       UINTX    Value, 
  IN       UINT8    *ValueLength
  ) 
{
  UINT8  CurrentByte;
  UINTX  FinalValue;
  UINT8  TestByte;
  UINT8  MaxBit;

#if USE_64_BIT_VARIABLES
  MaxBit = 64;
#else
  MaxBit = 32;
#endif
  *ValueLength = 0;
  CurrentByte = 0;
  FinalValue = 0;
  for (TestByte = 0; TestByte < MaxBit; TestByte += 8) {
    if ((Be & 1) == 1) {
      (*ValueLength)++;
      FinalValue += ((Value & 0xff) << TestByte);
      Value >>= 8;
    }
    Be >>= 1;
  }
  //
  // In case of no byte enables (i.e. wake command or something), set to 1 byte of data.
  //
  if (*ValueLength == 0)
  {
    *ValueLength = 1;
  }
  return FinalValue;
}



MSP_STATUS
CreatePFCTSel (
  IN  OUT   MSP_DATA        *MspData,
  IN        UINT8            Socket,
  IN        UINT8            Channel,
  IN        PFCT_VARIATIONS *PFCTSelect
  )
{
  UINT16  CurrentPlatform;
  UINT16  CurrentFrequency;
  UINT16  CurrentConfiguration;
  UINT16  CurrentType;

  CurrentPlatform       = MspData->CurrentPlatform[Channel];
  CurrentFrequency      = MspData->CurrentFrequency[Channel];
  CurrentConfiguration  = MspData->CurrentConfiguration[Channel];
  CurrentType           = MspData->CurrentTechnology[Channel];

  PFCTSelect[Pfct].Pfct     = 0;
  PFCTSelect[PfctT].Pfct    = (1  << CurrentType);
  PFCTSelect[PfctC].Pfct    = (1  << CurrentConfiguration);
  PFCTSelect[PfctCT].Pfct   = ((1 << CurrentConfiguration) << (NUM_TYPE)) | (1 << CurrentType);
  PFCTSelect[PfctF].Pfct    = (1  << CurrentFrequency);
  PFCTSelect[PfctFT].Pfct   = ((1 << CurrentFrequency) << (NUM_TYPE)) | (1 << CurrentType);
  PFCTSelect[PfctFC].Pfct   = ((1 << CurrentFrequency) << (NUM_CONF)) | (1 << CurrentConfiguration);
  PFCTSelect[PfctFCT].Pfct  = ((1 << CurrentFrequency) << (NUM_CONF + NUM_TYPE)) | ((1 << CurrentConfiguration) << (NUM_TYPE)) | ((1 << CurrentType));
  PFCTSelect[PfctP].Pfct    = (1  << CurrentPlatform);
  PFCTSelect[PfctPT].Pfct   = ((1 << CurrentPlatform) << (NUM_TYPE) ) | (1 << CurrentType);
  PFCTSelect[PfctPC].Pfct   = ((1 << CurrentPlatform) << (NUM_CONF) ) | (1 << CurrentConfiguration);
  PFCTSelect[PfctPCT].Pfct  = ((1 << CurrentPlatform) << (NUM_CONF + NUM_TYPE) ) | ( (1 << CurrentConfiguration) << (NUM_TYPE) ) | (1 << CurrentType);
  PFCTSelect[PfctPF].Pfct   = ((1 << CurrentPlatform) << (NUM_FREQ) ) | (1 << CurrentFrequency);
  PFCTSelect[PfctPFT].Pfct  = ((1 << CurrentPlatform) << (NUM_FREQ + NUM_TYPE) ) | ( (1 << CurrentFrequency) << (NUM_TYPE) ) | (1 << CurrentType);
  PFCTSelect[PfctPFC].Pfct  = ((1 << CurrentPlatform) << (NUM_FREQ + NUM_CONF) ) | ( (1 << CurrentFrequency) << (NUM_CONF) ) | (1 << CurrentConfiguration);
  PFCTSelect[PfctPFCT].Pfct = ((1 << CurrentPlatform) << (NUM_FREQ + NUM_CONF + NUM_TYPE) ) | ((1 << CurrentFrequency) << (NUM_CONF + NUM_TYPE)) | ((1 << CurrentConfiguration) << (NUM_TYPE)) | (1 << CurrentType);

  PFCTSelect[Pfct].LengthVariation     = 0;
  PFCTSelect[PfctT].LengthVariation    = (NUM_TYPE - 1) / 8 + 1;
  PFCTSelect[PfctC].LengthVariation    = (NUM_CONF - 1) / 8 + 1;
  PFCTSelect[PfctCT].LengthVariation   = (NUM_CONF + NUM_TYPE - 1) / 8 + 1;
  PFCTSelect[PfctF].LengthVariation    = (NUM_FREQ - 1) / 8 + 1;
  PFCTSelect[PfctFT].LengthVariation   = (NUM_FREQ + NUM_TYPE - 1) / 8 + 1;
  PFCTSelect[PfctFC].LengthVariation   = (NUM_FREQ + NUM_CONF - 1) / 8 + 1;
  PFCTSelect[PfctFCT].LengthVariation  = (NUM_FREQ + NUM_CONF + NUM_TYPE - 1) / 8 + 1;
  PFCTSelect[PfctP].LengthVariation    = (NUM_PLAT - 1) / 8 + 1;
  PFCTSelect[PfctPT].LengthVariation   = (NUM_PLAT + NUM_TYPE - 1) / 8 + 1;
  PFCTSelect[PfctPC].LengthVariation   = (NUM_PLAT + NUM_CONF - 1) / 8 + 1;
  PFCTSelect[PfctPCT].LengthVariation  = (NUM_PLAT + NUM_CONF + NUM_TYPE - 1) / 8 + 1;
  PFCTSelect[PfctPF].LengthVariation   = (NUM_PLAT + NUM_FREQ - 1) / 8 + 1;
  PFCTSelect[PfctPFT].LengthVariation  = (NUM_PLAT + NUM_FREQ + NUM_TYPE - 1) / 8 + 1;
  PFCTSelect[PfctPFC].LengthVariation  = (NUM_PLAT + NUM_FREQ + NUM_CONF - 1) / 8 + 1;
  PFCTSelect[PfctPFCT].LengthVariation = (NUM_PLAT + NUM_FREQ + NUM_CONF + NUM_TYPE - 1) / 8 + 1;

  return MSP_SUCCESS;
}


/**
  Decodes and executes the Capsule initialization. 
  The routine starts at a beginning index in the CapsuleInit 
  compressed data structure and continues until a ASSIGN_DONE 
  flag is detected. 
   
  @param[in, out]   MspData        Host structure for all data 
        related to MSP.
  @param[in]        Socket         Socket under test
  @param[in]        CapsuleStartIndex   Starting point within the 
                                   CapsuleData structure where
                                   the Init should begin.
  @param[in]        StringIndex     When provided, this is the index into the string
                                   table to show the register names.   
  @param[in]        Channel         Current Channel being examined.
  @retval           MSP_SUCCESS
**/
MSP_STATUS
MspExecuteTask (
  IN      MSP_DATA         *MspData,
  IN      UINT8             Socket,
  IN      UINT16            CapsuleStartIndex,
  IN      UINT16            StringIndex, 
  IN      UINT8             Channel
  )
{
  UINTX    PfctValue;                                 // Based on the current PFCT, holds the Value for the current assignment.
  UINT8    PfctStatus;                                // Flag to determine if any of the PFCT conditions were met on the current assign.
  UINT8    Action;                                    // Holds the Action of the current assign.
  UINT8    Counter;                                   // Counter for displaying results on the output when enabled.
  UINT8    *CurrentEncLoc;                            // Holds the location of the current assign. being decoded.
  UINT32   Delay;                                     // Holds the Delay for the assignment.  0 if not needed.
  UINT16   ConditionalExecution;                      // Flag specifying if the assignment should take place.
  UINT8    ChannelToWrite;                            // Specific channel the assignment is directed to, if any.
  UINT8    InstanceToWrite;                           // Specific instance the assignment is directed to, if any.
  UINT16   CurrentPtr;                                // Pointer to the encoded data stream.
  UINT16   RegisterOffset;                            // Register offset provided in the assignment.
  UINT8    BoxType;                                   // Boxtype provided in the assignment.
  UINT8    ByteEnable;                                // Byte enable provided in the assignment.
  UINTX    Mask;                                      // Mask that may be provided in the assignment.
  UINT8    MaskValueLength;                           // Length of the MASK in bytes, when the mask is provided.
  UINT8    Pfct;                                      // PFCT flag provided in the assignment.
  UINT8    PfctDynamic;                               // Dynamic field in the dPFCT provided in the assignment.
  UINT8    NumberConditionalValues;                   // Number of PFCT Condition-Values provided in the assignment.
  UINT8    NumberConditionalBytes;                    // Number of bytes within the condition portion of the PFCT assignment.
  UINT8    ConditionalFlag;                           // When decoding PFCT values, when the condition is met, this value is 1.
  UINT8    TestVal;                                   // Used to determine if the condition is true in the PFCT assignment.
  UINT8    NumberInstances;                           // Number of instances for a specific BoxType.
  UINT8    Instance;                                  // Instance being specified when looping on total Instances.
  UINT8    SkipIosf;                                  // Flag specifying if the IOSF assignment should be skipped.  (Broadcasts)
  UINTX    readValue;                                 // When doing a poll, this is the value read back.
  UINTX    Value;                                     // 32-bit value that gets written to for the assignment.
  UINT8    sLoop;                                     // When DETAILED_PHYINIT is enabled, this is used to print the strings.
  UINT8    StartingBit;                               // Final starting position that the assignment is accessing.
  UINT8    EndingBit;                                 // Final ending position that the assignment is accessing.
  UINT8    PfctValueSize;                             // The size in bytes of the value portion of the PFCT CVs.
  UINT32   BaseOffset;                                // Base offset for the instance of the boxtype.
  UINT8    Port;                                      // Port ID for the instance of the boxtype.
  UINT8    Bus;                                       // When doing PCI Access, PCI Bus.
  UINT8    Device;                                    // When doing PCI Access, PCI Device.
  UINT8    Func;                                      // When doing PCI Access, PCI Function.
  UINT8    i;
  UINTX    NewValue;                                  // Used for temporary storage in DynamicAssignmentGenerated call.
  UINT16   DynamicIndex;                              // Used in DynamicAssignmentGenerated (the switch statement index).
  UINT16   InitialStringIndex;
  UINT8    Strobe;                                    // Used to loop on strobes.
  UINT8    Rank;                                      // Used to loop on ranks.
  UINT8    ThisChannel;                               // Used for strobe/rank looping.
  UINT8    ThisInstance;                              // Used for strobe/rank looping.
  UINT8    MaxStrobe;                                 // Used to loop on strobes.
  UINT8    MaxRank;                                   // Used to loop on ranks.
  UINT8    ThisBoxType;                               // Used for strobe/rank looping.
  UINT8    MaxBit;                                    // Are we using 32 bit or 64 bit registers?
  UINT8    NumStrings;                                // Number of strings we need to output to the log.
  UINT8    Zero;                                      // Temporary location for resetting structures to 0.
  UINT8    k;                                         // Temporary counter.
  UINT8    MsgLevel;                                  // Temporary storage for backing up the message level.
  UINT8    MaxBoxName;                                // Maximum length of box string to print.
  BOOLEAN  ExecuteThisRoutineInParallel;
#if DUMP_REGISTER_NAMES
  UINT8    MaxRegisterName;                           // Maximum length of register string to print.
  UINT32   RegisterNumber;                            // Index to register number which will be printed.
#endif
  BOOLEAN  ChannelBasedOffsetDetected;
  UINT16   CapsuleStrIndex;
  PFCT_VARIATIONS                 PFCTVariations[MaxPfct];  // Holds all permutations of PFCTs
  REGISTER_LOCATION_ATTRIBUTES    *RegisterAttributes;      // Bit fields for the flags portion of the MEMORYINIT.
  REGISTER_LOCATION_ATTRIBUTES2   *RegisterAttributes2;     // Bit fields for the 2nd byte of the flags portion of the MEMORYINIT.
  REGISTER_ACCESS                 Register;                 // Field that specifies the location of the register including offset and bits.
  UINT32   BarOffset;
  
  if (!MspData->Lock) {
    MspDebugPrint ((MSP_DBG_MIN, "Warning: MspData data structure hasn't been locked yet\n"));
  }

#if USE_64_BIT_VARIABLES
  MaxBit = 64;
#else
  MaxBit = 32;
#endif
  InitialStringIndex  = StringIndex;
  ThisInstance        = 0;
  ThisChannel         = 0;
  MsgLevel            = MspData->DebugMsgLevel;
  for (Channel; Channel < MAX_CHANNELS; Channel++) {
    if (!MspData->Enabled[Channel]) {
      continue;
    }
    //
    // Initializing the Variables.
    //
    Value                          = 1;
    ChannelToWrite                 = 0;
    PfctValue                      = 0;
    StartingBit                    = 0;
    EndingBit                      = 0;
    sLoop                          = 0;
    Counter                        = 0;
    RegisterOffset                 = 0;
    PfctValueSize                  = 0;
    Delay                          = 0;
    CurrentPtr                     = 0;
    ConditionalExecution           = 1;
    Bus                            = 0;
    Device                         = 0;
    Func                           = 0;
    BaseOffset                     = 0;
    Port                           = 0;
    ThisChannel                    = 0;
    ThisInstance                   = 0;
    StringIndex                    = InitialStringIndex;
    
    //
    // If this task is marked as NO_PRINT, turn off all TRACE level messages. MSP_DBG_TRACE
    // is used by this routine to display all actions taken.
    //
    if (StringIndex != 0xFFFF) {
      if (MmrcStringCompare ( CapsuleStrings[StringIndex], "EnableChannels", 12) ||
          MmrcStringCompare (CapsuleStrings[StringIndex], "MmapForTrain", 11) ||
          MmrcStringCompare (CapsuleStrings[StringIndex], "DUnitAftTrain", 12)
          ) {
        if (MspData->DebugMsgLevel != 0) {
          MspData->DebugMsgLevel |= MSP_DBG_TRACE; 
        }
      }
    }
    if (StringIndex == MSP_NO_PRINT) {
      MspData->DebugMsgLevel &= ~MSP_DBG_TRACE;
    }
#if CAPSULESTRINGS
    if (StringIndex < MSP_NO_PRINT) {
      MspDebugPrint ((MSP_DBG_TRACE, " %s\n", CapsuleStrings[StringIndex]));
    }
    MspDebugPrint ((MSP_DBG_TRACE, "    Box"));
    if (MAX_BOXNAME_LENGTH < 3) {
      MaxBoxName = 0;
    } else {
      MaxBoxName = MAX_BOXNAME_LENGTH - 3;
    }
    for (i = 0; i < MaxBoxName; i++) {
      MspDebugPrint ((MSP_DBG_TRACE, " "));
    }
#endif
#if DUMP_REGISTER_NAMES
    MspDebugPrint ((MSP_DBG_TRACE, "Register Symbol"));
    if (MAX_REGISTER_NAME_LENGTH < 13) {
      MaxRegisterName = 0;
    } else {
      MaxRegisterName = MAX_REGISTER_NAME_LENGTH - 13;
    }
    for (i = 0; i < MaxRegisterName; i++) {
      MspDebugPrint ((MSP_DBG_TRACE, " "));
    }
#endif
    MspDebugPrint ((MSP_DBG_TRACE, "   Port Offset     Mask               Action    Delay      Value\n"));
    //
    // Given the current P, F, C, T provide all possible PFCT variations, this makes the comparison quicker
    // since this does not have to be derived for every assignment.  There are a total of 16 different PFCT
    // permutations.
    //
    CreatePFCTSel (MspData, Socket, Channel, PFCTVariations);
    //
    // Begin by assigning the Current encoded location to the start of the assignments.
    //
    CurrentEncLoc = (UINT8 *) &InitData[CapsuleStartIndex];
    //
    // The last assignment of the assignment list has the MACRO "ASSIGNDONE" which is the signature to stop decoding.
    //
    while (1) {
      ChannelBasedOffsetDetected  = TRUE;
      InstanceToWrite =  0;
      ChannelToWrite  =  Channel;
      ConditionalExecution = 1;
      Zero = 0;
      //
      // Reset to 0.
      //
      RegisterAttributes2 = (REGISTER_LOCATION_ATTRIBUTES2 *) &Zero;
      //
      // The register attributes for the FLAGS starts at the beginning location, and the default length of this field
      // is one byte.
      //
      RegisterAttributes = (REGISTER_LOCATION_ATTRIBUTES *) &CurrentEncLoc[0];
      CurrentPtr = 1;
      if (RegisterAttributes->Done == 1) {
        break;
      }
      if (RegisterAttributes->NeedSecondByte == 1) {
        RegisterAttributes2 = (REGISTER_LOCATION_ATTRIBUTES2 *) &CurrentEncLoc[CurrentPtr];
        CurrentPtr++;
      }
      //
      // Check if the assignment should be executed based on the execution flag.
      //
      if (RegisterAttributes->CondExec == 1) {
        ConditionalExecution = (UINT16) MspData->DynamicVars[Channel][*(UINT16 *) &CurrentEncLoc[CurrentPtr]];
        CurrentPtr += 2;
      }
      //
      // If the ChannelDependent or InstanceDependent flags are either set, then the flags will be one byte larger.
      // Also when not set, Channel/Instance to write are set to 0 but will not be used in the assignment.
      //
      if (RegisterAttributes2->ChannelDependent == 1) {
        ChannelToWrite  = CurrentEncLoc[CurrentPtr++];
      }
      if (RegisterAttributes2->InstanceDependent == 1) {
        InstanceToWrite = CurrentEncLoc[CurrentPtr++];
      }
      //
      // Following the flags is the register Offset.  This will be either a 8-bit or 16-bit value based on the
      // offset16 flag.
      //
      if (RegisterAttributes->Offset16 == 1) {
        RegisterOffset = *((UINT16 *) &CurrentEncLoc[CurrentPtr]);
        CurrentPtr +=2;
      } else {
        RegisterOffset = *((UINT8 *) &CurrentEncLoc[CurrentPtr]);
        CurrentPtr +=1;
      }
      //
      // Read either 2 bytes or 1 byte to get the boxtype and byte enables based on
      // the size of the boxtype number and the PFCT value to be written.
      //
      if (RegisterAttributes->UseBB2 == 1) {
        //
        // The Boxtype and ByteEnables are always provided in the next byte.  They are split into 4-bits each.
        //
        BoxType    = CurrentEncLoc[CurrentPtr];
        ByteEnable = CurrentEncLoc[CurrentPtr + 1];
        CurrentPtr += 2;
      } else {
        //
        // The Boxtype and ByteEnables are always provided in the next byte.  They are split into 4-bits each.
        //
        BoxType    = (CurrentEncLoc[CurrentPtr] >> 4) & 0x0f;
        ByteEnable = (CurrentEncLoc[CurrentPtr] >> 0) & 0x0f;
        CurrentPtr += 1;
      }
      //
      // If the MaskPresent bit in the flags is set, then the mask will be provided.  The length of the mask is based on the
      // byte enables.  Ex. If 2-bits in the BE (_0110) are set, then the mask will be 2-bytes long.
      //
      if (RegisterAttributes->MaskPresent == 1) {
        Mask = ByteEnableToUintX (ByteEnable, *((UINTX *) &CurrentEncLoc[CurrentPtr]), &MaskValueLength);
        CurrentPtr += MaskValueLength;
      } else {
#if USE_64_BIT_VARIABLES
        Mask = ByteEnableToUintX (ByteEnable, 0xffffffffffffffff, &MaskValueLength);
#else
        Mask = ByteEnableToUintX (ByteEnable, 0xffffffff, &MaskValueLength);
#endif
      }
      //
      // The Action/PFCT provides three distinct fields.  The action (get/set/poll), the PFCT, and the dynamic flag.
      // This is a 1-byte field and is split into [3-bits action, 1-bit dyn, 4-bits pfct].
      Action      = (CurrentEncLoc[CurrentPtr] >> 5) & 0x07;
      PfctDynamic = (CurrentEncLoc[CurrentPtr] >> 4) & 0x01;
      Pfct        = (CurrentEncLoc[CurrentPtr] >> 0) & 0x0f;
      CurrentPtr  += 1;
      //
      // If the action is SET_DELAY, then immediately following the ACTION will be a 4byte delay value.
      //
      if (Action == MMRC_SET_DELAY) {
        Delay = *(UINT32 *) &CurrentEncLoc[CurrentPtr];
        CurrentPtr += 4;
      }
#if 0
      //
      // This will ensure that every register is written during simulation
      // when using 'backdoor' method and should be only enabled to discard      
      // any suspicious behaviour in the registers or in the waveforms
      //
      // Note: This will add a delay to any SET action
      //
      if ((1 << MspData->CurrentPlatform[Channel]) == P_SIM) {
        if (Action == MMRC_SET) {
          Delay  = 10;
          Action = MMRC_SET_DELAY;
        }
      }
#endif
      // 
      // The PFCT status will provide the flag if any of the PFCT conditions where true, the assumption is no
      // condition is true, and will be changed to TAKE_ACTION if one is determined.
      //
      PfctStatus = SKIP_ACTION;
      // 
      // If the PFCT flag is set to 0, then the assignment is global/PFCT independent.  The assignment should then
      // provide one value.  If the dynamic bit is set, the value is an 8-bit index value, otherwise the value length
      // will be based on the byte-enables.
      //
      if (Pfct == PFCT_INDEPENDENT) {
        PfctStatus = TAKE_ACTION;
        if (PfctDynamic == 1) {
          PfctValue = *(UINT16 *) &CurrentEncLoc[CurrentPtr];
          CurrentPtr += 2;
        } else {
          PfctValue = ByteEnableToUintX (ByteEnable, *((UINTX *) &CurrentEncLoc[CurrentPtr]), &MaskValueLength);
          CurrentPtr += MaskValueLength;
        }
      } else {
        //
        // To reach this point, the PFCT Flag must have been non-zero so non-global.
        // The next byte is the total number of conditional-Value pairs..
        //
        NumberConditionalValues = CurrentEncLoc[CurrentPtr];
        CurrentPtr += 1;
        //
        // Loop through each Condition/Value pair looking for a match based on the current pfct.
        //
        for (Counter = 0; Counter < NumberConditionalValues; Counter++) {
          //
          // Determine the number of bytes are needed for the condition.  The system will
          // compare the PFCT condition byte-by-byte with the TRUE PFCT condition.  If all the
          // bits compare okay, then the condition is true.  ConditionalFlag is the byte-by-byte
          // comparison flag.
          //
          NumberConditionalBytes = PFCTVariations[Pfct].LengthVariation;
          ConditionalFlag = 1;
          while (NumberConditionalBytes > 0 && ConditionalFlag == 1) {
            TestVal = (UINT8) ((PFCTVariations[Pfct].Pfct >> ((NumberConditionalBytes - 1) * 8) ) & 0xff);
            if ((CurrentEncLoc[CurrentPtr+NumberConditionalBytes - 1]  & TestVal) != TestVal) {
               ConditionalFlag = 0;
            }
            NumberConditionalBytes--;
          }
          CurrentPtr += PFCTVariations[Pfct].LengthVariation;
          //
          // Read in the value which is also based on the byte-enables for its length, similar to the mask.
          //
          if (PfctDynamic == 1) {
            PfctValue = *(UINT16 *) &CurrentEncLoc[CurrentPtr];
            PfctValueSize = 2;
          } else {
            PfctValue = ByteEnableToUintX (ByteEnable, *((UINTX *) &CurrentEncLoc[CurrentPtr]), &MaskValueLength);
            PfctValueSize = MaskValueLength;
          }
          CurrentPtr += PfctValueSize;
          //
          // If the condition flag is still true, then move the currentPtr to the end of the remaining condition-values.
          // and set the status to take_action.
          if (ConditionalFlag == 1) {
            PfctStatus = TAKE_ACTION;
            CurrentPtr += (PFCTVariations[Pfct].LengthVariation + PfctValueSize) * (NumberConditionalValues - Counter - 1);
            break;
          }
        }  // for (Counter = 0; Counter < NumberConditionalValues; Counter++)
      } // if (Pfct == PFCT_INDEPENDENT) {
      //
      // Save the value which will be used as the index to the switch statement
      // in DynamicAssignmentGenerated.
      //
      DynamicIndex = (UINT16) PfctValue;
      if (RegisterAttributes2->StringPresent == 1) {
        NumStrings = CurrentEncLoc[CurrentPtr++];
      } else {
        NumStrings = 0;
      }
      //
      // Now that a value has been determined true, the value must be programmed based on the 
      // loopings to multiple registers.
      //
      if (PfctStatus == TAKE_ACTION  && ConditionalExecution == 1) {
        if (Action == MMRC_SUBTASK) {
#if CAPSULESTRINGS
          MspDebugPrint ((MSP_DBG_TRACE, "   +"));
          for (k = 0; k < MAX_BOXNAME_LENGTH; k++) {
            MspDebugPrint ((MSP_DBG_TRACE, " "));
          }
#endif
#if DUMP_REGISTER_NAMES
          for (k = 0; k < MAX_REGISTER_NAME_LENGTH + 2; k++) {
            MspDebugPrint ((MSP_DBG_TRACE, " "));
          }
#endif
#if USE_64_BIT_VARIABLES
          MspDebugPrint ((MSP_DBG_TRACE, "   NA   NA         NA                 "));
#else
          MspDebugPrint ((MSP_DBG_TRACE, "   NA   NA         NA         "));
#endif
          MspDebugPrint ((MSP_DBG_TRACE, "SUBTASK   NA         "));
          //
          // Search for index in capsuleStrings
          //
          CapsuleStrIndex = (UINT16) -1;
          for (k = 0; k < MAX_SUBTASKS; k++) {
            if (PfctValue == SubtaskOffsetIndex[k].FncOffset) {
              CapsuleStrIndex = SubtaskOffsetIndex[k].StrIndex;
              break;
            }
          }
          if (CapsuleStrIndex != -1) {
            MspDebugPrint ((MSP_DBG_TRACE, "%s", (CHAR8 *)CapsuleStrings[CapsuleStrIndex]));
          } else {
            MspDebugPrint ((MSP_DBG_TRACE, "ERROR"));
          }

#if CAPSULESTRINGS
          if (RegisterAttributes2->StringPresent == 1) {
            for (k = 0; k < NumStrings; k++) {
              MspDebugPrint ((MSP_DBG_TRACE, " %s",(CHAR8 *)OutputStrings[CurrentEncLoc[CurrentPtr + k]]));
              if ((k + 1) < NumStrings) {
                MspDebugPrint ((MSP_DBG_TRACE, ","));
              }
            }
          }
#endif // CAPSULESTRINGS
          MspDebugPrint ((MSP_DBG_TRACE, "\n"));
          ExecuteThisRoutineInParallel = MspData->ExecuteThisRoutineInParallel;
          MspData->ExecuteThisRoutineInParallel = FALSE;
          if (CapsuleStrIndex != -1) {
            MspExecuteTask (MspData, Socket, (UINT16) PfctValue, CapsuleStrIndex, Channel);
          } else {
            MspExecuteTask (MspData, Socket, (UINT16) PfctValue, 0, Channel);
          }
          MspData->ExecuteThisRoutineInParallel = ExecuteThisRoutineInParallel;
          //
          // Else, not a SUBTASK...so proceed to process normal action.
          //
        } else {
          if (RegisterAttributes->StrobeLoop == 1) {
#ifdef  MAX_STROBES
            MaxStrobe = MAX_STROBES;
#else 
            MaxStrobe = 1;
#endif
          } else {
            MaxStrobe = 1;
          }
          if (RegisterAttributes->RankLoop == 1) {
#ifdef  MAX_RANKS
            MaxRank = MAX_RANKS;
#else 
            MaxRank = 1;
#endif
          } else {
            MaxRank = 1;
          }
          //
          // If the register should be written only for a specific channel, and this call is
          // not the correct channel, just skip this assignment and go to the next assignment.
          //
          if (RegisterAttributes2->ChannelDependent == 0 || (RegisterAttributes2->ChannelDependent == 1 && Channel == ChannelToWrite)) {
            //
            // If the InstanceDependent flag is set, then the total number of instances that will be written is 1,
            // otherwise, the total number of instances/channel for the given boxtype will be written. The other case
            // is if we are looping on rank and/or strobe, then we will ignore the instance looping since strobe/rank
            // looping is a higher priority.
            //
            if (RegisterAttributes2->InstanceDependent == 0 && Action != MMRC_DELAY &&
                RegisterAttributes->StrobeLoop == 0 && RegisterAttributes->RankLoop == 0) {
                NumberInstances = MspInstancePortMap[BoxType].Instances;
                if (NumberInstances == 0) {
                  //
                  // If this boxtype is not channel based, then use the total number of instances
                  // instead of the instances per channel.
                  //
                  NumberInstances = MspInstancePortMap[BoxType].TotalInstances;
                  ChannelBasedOffsetDetected = FALSE;
                }
            } else {
              NumberInstances = 1;
            }
            //
            // The number of instances cannot be 0xff, if an 0xff is read this indicates 
            // that the current Box Type is not valid for the current blueprint, so we want
            // to skip any subsequent action
            //
            if (NumberInstances == 0xff) {
              NumberInstances = 0;
            }
            // 
            // Loop though all the instances determined earlier.
            //
            for (Instance = 0; Instance < NumberInstances; Instance++) {
              for (Rank = 0; Rank < MaxRank; Rank++) {
                for (Strobe = 0; Strobe < MaxStrobe; Strobe++) {
                  //
                  // If the register should be written only for a specific channel, and this call is
                  // not the correct channel, just skip this assignment and go to the next assignment.
                  //
                  if (RegisterAttributes2->InstanceDependent == 1) {
                    Instance = InstanceToWrite;
                  }
                  //
                  // If the broadcast feature within the IOSF is enabled, then only one write, the first, will occur as all the
                  // instances will be written to.
                  //
                  if (BROADCAST_SUP && Instance > 0) {
                    SkipIosf = 1;
                  } else {
                    SkipIosf = 0;
                  }
                  //
                  // Determine the actual offset/mask for the register to be accessed.  The offset currently is based off the
                  // offset for the assignment + the rank offset + bytelane offset. The mask and shift bit will also be used
                  // for PCI accesses although the data for the PCI access is in PciReg which is filled in below.
                  //
                  Register.Offset   = RegisterOffset;
                  Register.Mask     = Mask;
                  Register.ShiftBit = 0;
                  ThisBoxType = BoxType;
                  //
                  // If we're looping on strobe/rank we need to adjust the offset to the correct register location 
                  // for this strobe/rank.
                  //
                  if (RegisterAttributes->StrobeLoop == 0 && RegisterAttributes->RankLoop == 0) {
                    ThisChannel = Channel;
                    ThisInstance = Instance;
                  } else {
#ifndef MSP_DXE_EXECUTION
                    if (RegisterAttributes->StrobeLoop == 1) {
                      Register.Offset += MspStrobeToRegister[MspFloorPlan[Channel][Strobe].Strobelane][1];
                      ThisChannel = MspFloorPlan[Channel][Strobe].Channel;
                      ThisInstance = (UINT8)(Instance + MspStrobeToRegister[MspFloorPlan[Channel][Strobe].Strobelane][0]);
                    }
                    if (RegisterAttributes->RankLoop == 1) {
                      Register.Offset += MspRankToRegister[Rank][1];
                      ThisInstance += ((UINT8)(Instance + MspRankToRegister[Rank][0]));
                    }
#endif // MSP_DXE_EXECUTION
                    //
                    // Select the new boxtype since we may have increased the sub-box instance to
                    // a subsequent boxtype to reach the correct register for this rank/strobe. An
                    // example would be if strobe 16's registers were in the next PCI bus/dev/func
                    // and we needed to point to the next boxtype to get to that location.
                    //
                    for (i = 0; i < MAX_BOXES; i++) {
                      if (MspInstancePortMap[i].InstanceStartIndex <= (MspInstancePortMap[BoxType].InstanceStartIndex + ThisInstance)) {
                        ThisBoxType = i;
                      } else {
                        break;
                      }
                    }
                  }
                  //
                  // When a Boxtype that is not channel based we need to hardcode 'ThisChannel' = 0
                  //
                  if (!ChannelBasedOffsetDetected) {
                    ThisChannel = 0;
                  }
                  // 
                  // If the action was a GET/SET/SET_DELAY/POLL, then the register offset, mask, delay, and value will be printed.
                  //
                  if (Action != MMRC_DELAY) {
                    if (GetRegisterAccessInfo (BoxType, ThisChannel, ThisInstance, 
                        (UINT32 *)&readValue, &Port, &BaseOffset, &Bus, &Device, &Func, ModeRead) != NoError) {
                      continue;
                    }
                  }
                  //
                  // The printout of the assignment will begin with either a *,/, or +.
                  //  *-> the assignment was performed, but it completed the assignment of others as it had broadcast enabled.
                  //  /-> the assignment was performed.
                  //  +-> the assignment was skipeed as it was assigned with the broadcast.
                  //
                  if (Instance == 0 && RegisterAttributes2->InstanceDependent == 0 && BROADCAST_SUP) {
                    MspDebugPrint ((MSP_DBG_TRACE, "   *"));
                  } else {
                    MspDebugPrint ((MSP_DBG_TRACE, "   +"));
                  }
                  //
                  // Print out register information.
                  //
                  if (Action == MMRC_GET || Action == MMRC_SET || Action == MMRC_SET_DELAY || Action == MMRC_POLL) {
                    //
                    // Convert the PFCTValue to the dynamic value, if the flag is set.
                    //
                    if (PfctDynamic == 1 && Action != MMRC_GET) {
                      NewValue = MemRegRead (BoxType, ThisChannel, ThisInstance, Register);
                      DynamicAssignmentGenerated (MspData->DynamicVars, ThisChannel, DynamicIndex, &NewValue);
                      PfctValue = NewValue;
                    }
  #if CAPSULESTRINGS
                    MspDebugPrint ((MSP_DBG_TRACE, "%s ", MspUnitStrings[ThisBoxType]));
  #endif
  #if DUMP_REGISTER_NAMES
                    RegisterNumber = GetRegisterNumber (ThisBoxType, Register.Offset);
                    //
                    // If the register has a name, print it out. Else, just pad with spaces.
                    //
                    if (RegisterNumber == 0xFFFFFFFF) {
                      i = 0;
                    } else {
                      MspDebugPrint ((MSP_DBG_TRACE, "%s", 
                        Registers[GetRegisterNumber (ThisBoxType, Register.Offset)].RegInfo->RegisterName
                      ));
                      i = (UINT8) StringLength (Registers[RegisterNumber].RegInfo->RegisterName);
                    }
                    for (i; i <= MAX_REGISTER_NAME_LENGTH + 2; i++) {
                      MspDebugPrint ((MSP_DBG_TRACE, " "));
                    }
  #endif
                    BarOffset = 0;
                    if (MspInstancePortMap[ThisBoxType].AccessMethod == eSB) {
                      MspDebugPrint((MSP_DBG_TRACE, "   0x%02X", Port));
                    } else if (MspInstancePortMap[ThisBoxType].AccessMethod == eBAR) {
                      MspDebugPrint((MSP_DBG_TRACE, "    MEM"));
                      BarOffset = (UINT32) MspData->DynamicVars[Channel][MspInstancePortMap[BoxType].AccessBase];
                    } else if (MspInstancePortMap[ThisBoxType].AccessMethod == ePCI) {
                      MspDebugPrint((MSP_DBG_TRACE, "%02X/%02X/%01X", Bus, Device, Func));
                    }
#if USE_64_BIT_VARIABLES
                    MspDebugPrint((MSP_DBG_TRACE, " 0x%08X 0x%08X%08X ", 
                      BarOffset + Register.Offset + BaseOffset, (UINT32)(Register.Mask >> 32), (UINT32)Register.Mask
                    ));
#else
                    MspDebugPrint((MSP_DBG_TRACE, " 0x%08X 0x%08X%08X ", 
                      BarOffset + Register.Offset + BaseOffset, 0, (UINT32)Register.Mask
                    ));
#endif
                  }
                  if (Action == MMRC_GET) {
                    //
                    // Get the right shift bit to capture the variable being read.
                    //
                    for (i = 0; i < MaxBit; i++) {
                      if ((Register.Mask & (UINTX)(BIT0 << i)) != 0)
                      {
                        break;
                      }
                      Register.ShiftBit++;
                    }
                    MspData->DynamicVars[ChannelToWrite][PfctValue] = MemFieldRead (BoxType, ThisChannel, ThisInstance, Register);

                    if (!ChannelBasedOffsetDetected) {
                      //
                      // Even if we execute this task on a Non-Channel Based Boxtype (NCBB), input parameter Channel
                      // might be equal '1', so when we execute a GET command over a register on a 'NCBB',
                      // the read value also needs to be stored on ThisChannel index, in case we want to restore it later.
                      // The restore value (SET Command) will be taken from the ThisChannel index in the function 'DynamicAssignmentGenerated'
                      //
                      MspData->DynamicVars[ThisChannel][PfctValue] = MspData->DynamicVars[ChannelToWrite][PfctValue];
                    }
  #if USE_64_BIT_VARIABLES
                    MspDebugPrint ((MSP_DBG_TRACE, "GET       0x%08X 0x%08X%08X", 0, 
                      (UINT32)(MspData->DynamicVars[ChannelToWrite][PfctValue] >> 32),
                      (UINT32)(MspData->DynamicVars[ChannelToWrite][PfctValue])
                    ));
  #else
                    MspDebugPrint ((MSP_DBG_TRACE, "GET       0x%08X 0x%08X%08X", 0, 
                      0,
                      (UINT32)(MspData->DynamicVars[ChannelToWrite][PfctValue])
                    ));
  #endif
                  } else if (Action == MMRC_SET || Action == MMRC_SET_DELAY)  {
                    if (Action == MMRC_SET) {
                      MspDebugPrint ((MSP_DBG_TRACE, "SET       0x%08X ", 0));
                    }
                    if (Action == MMRC_SET_DELAY) {
                      MspDebugPrint ((MSP_DBG_TRACE, "SET_DELAY 0x%08X ", Delay));
                    }
  #if USE_64_BIT_VARIABLES
                    MspDebugPrint((MSP_DBG_TRACE, "0x%08X%08X", (UINT32)(PfctValue >> 32), (UINT32)(PfctValue)));
  #else
                    MspDebugPrint((MSP_DBG_TRACE, "0x%08X%08X", 0, (UINT32)(PfctValue)));
  #endif

                    MemRegWrite (BoxType, ThisChannel, ThisInstance, Register, PfctValue, ByteEnable);
                    //
                    // If the action has a delay, with the set, then delay between every assignment.
                    //
                    if (Action == MMRC_SET_DELAY) {
  #if SIM
                        MySimStall (Delay);
  #else

                        MspDelay (NANO_DEL, Delay);
  #endif
                    }
                  } else if (Action == MMRC_POLL) {
                    //
                    // If the action is a POLL, then print out the register/value to be polled for, and begin polling.
                    //
  #if USE_64_BIT_VARIABLES
                    MspDebugPrint((MSP_DBG_TRACE, "POLL      0x00000000 0x%08X%08X", 
                      (UINT32)(PfctValue >> 32), (UINT32)(PfctValue)
                    ));
  #else
                    MspDebugPrint((MSP_DBG_TRACE, "POLL      0x00000000 0x%08X%08X", 0, (UINT32)(PfctValue)));
  #endif

                    do {
                        readValue = MemRegRead (BoxType, ThisChannel, ThisInstance, Register);
  #if SIM
                        //
                        // Stall between polling passes in simulation so we don't slow down the
                        // simulator with a bunch of read requests.
                        //
                        MySimStall (50);
  #endif
                    } while (readValue != PfctValue);
                  } else if (Action == MMRC_DELAY) {
                    MspDebugPrint ((MSP_DBG_TRACE, "DELAY: %dns", PfctValue));
  #if SIM
                    MySimStall ((UINT32)PfctValue);
  #else
                    MspDelay (NANO_DEL, (UINT32) PfctValue);
  #endif
                  }
                  //
                  // Add the length of the current assignment to the current encoded location to the the location
                  //  of the next assignment.
                  //
                  if (RegisterAttributes2->StringPresent == 1) {
                    for (k = 0; k < NumStrings; k++) {
  #if CAPSULESTRINGS
                      MspDebugPrint ((MSP_DBG_TRACE, " %s", (CHAR8 *)OutputStrings[CurrentEncLoc[CurrentPtr + k]]));
                      if ((k + 1) < NumStrings) {
                        MspDebugPrint ((MSP_DBG_TRACE, ","));
                      }
  #endif // CAPSULESTRINGS
                    }
                  }
                  MspDebugPrint ((MSP_DBG_TRACE, "\n"));
                } // Strobe loop
              } // Rank loop
            } // Instance loop ...
          }
        }
      } // TAKE_ACTION
      CurrentEncLoc += (CurrentPtr + NumStrings);
    } // *CurrentEncLoc != ASSIGNDONE loop ...

    if (!MspData->ExecuteThisRoutineInParallel) {
      //
      // Stop the routine after first execution
      // if not supposed to be executed in parallel
      //
      break;
    }
  } // Channel loop ...

  MspData->DebugMsgLevel = MsgLevel;

  return MSP_SUCCESS;
}

MSP_STATUS
MspInit (
  IN      MSP_DATA         *MspData,
  IN      VOID             *InputStructure,
  IN      Callback          FunctionPtr
  )
{
  if (MspData->Lock) {
    MspDebugPrint ((MSP_DBG_MIN, "MspInit :: MspData structure is already locked\n"));
    return MSP_SUCCESS;
  }
  McEnableHpet (MspData);
  FunctionPtr (MspData, InputStructure);
  SaveMspHostStructureAddress (MspData);
  
  return MSP_SUCCESS;
}

VOID
FillUpMspDataStructure (
  IN  OUT     MSP_DATA      *MspData,
  IN          UINT8         Channel,
  IN          BOOLEAN       Enabled,
  IN          UINT16        CurrentPlatform,
  IN          UINT16        CurrentFrequency,
  IN          UINT16        CurrentTechnology,
  IN          UINT8         DebugMsgLevel,
  IN          BOOLEAN       ExecuteThisRoutineInParallel,
  IN          UINT8         CurrentBlueprint,
  IN          UINT32        EcBase
)
{
  MspData->Enabled[Channel]               = Enabled;
  MspData->CurrentPlatform[Channel]       = CurrentPlatform;
  MspData->CurrentFrequency[Channel]      = CurrentFrequency;
  MspData->CurrentTechnology[Channel]     = CurrentTechnology;
  MspData->DebugMsgLevel                  = DebugMsgLevel;
  MspData->ExecuteThisRoutineInParallel   = ExecuteThisRoutineInParallel;
  MspData->CurrentBlueprint               = CurrentBlueprint;
  MspData->EcBase                         = EcBase;
}

VOID
LockMspDataStructure ()
{
  UINT8     Channel;
  BOOLEAN   AnyChannelEnabled;
  MSP_DATA  *MspData;

  MspData = GetMspHostStructureAddress ();
  
  if (MspData->Lock) {
    MspDebugPrint ((MSP_DBG_MIN, "LockMspDataStructure :: MspData structure is already locked\n"));
  }

  AnyChannelEnabled = FALSE;
  MspData->Lock     = TRUE;
  
  MspDebugPrint ((MSP_DBG_MAX, "MspData->DebugMsgLevel                = 0x%02x\n", MspData->DebugMsgLevel));
  MspDebugPrint ((MSP_DBG_MAX, "MspData->ExecuteThisRoutineInParallel = 0x%02x\n", MspData->ExecuteThisRoutineInParallel));
  MspDebugPrint ((MSP_DBG_MAX, "MspData->CurrentBlueprint             = 0x%02x\n", MspData->CurrentBlueprint));
  MspDebugPrint ((MSP_DBG_MAX, "MspData->EcBase                       = 0x%08x\n", MspData->EcBase));
  
  for (Channel = 0; Channel < MAX_CHANNELS; Channel++) {
    if (MspData->Enabled[Channel]) {
      AnyChannelEnabled = TRUE;
    }
    MspDebugPrint ((MSP_DBG_MAX, "MspData->Enabled[%d]                   = 0x%02x\n", Channel, MspData->Enabled[Channel]));
    MspDebugPrint ((MSP_DBG_MAX, "MspData->CurrentPlatform[%d]           = 0x%02x\n", Channel, MspData->CurrentPlatform[Channel]));
    MspDebugPrint ((MSP_DBG_MAX, "MspData->CurrentFrequency[%d]          = 0x%02x\n", Channel, MspData->CurrentFrequency[Channel]));
    MspDebugPrint ((MSP_DBG_MAX, "MspData->CurrentTechnology[%d]         = 0x%02x\n", Channel, MspData->CurrentTechnology[Channel]));
  }
  
  if (!AnyChannelEnabled) {
    MspDebugPrint ((MSP_DBG_MIN, "No channels/ports were enabled. Were FillUpMspDataStructure and/or SetMspData called or set properly?\n"));
  }
}
