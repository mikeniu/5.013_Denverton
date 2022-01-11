/*++
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
--*/
/*++

  Copyright (c) 2001-2016 Intel Corporation.  All rights reserved.

  This software and associated documentation (if any) is furnished under
  a license and may only be used or copied in accordance with the terms
  of the license.  Except as permitted by such license, no part of this
  software or documentation may be reproduced, stored in a retrieval system,
  or transmitted in any form or by any means without the express written
  consent of Intel Corporation.

  Module Name:

    SpsNm.asl

  Abstract:

    This file defines ASL code for Node Manager (NM) support.

--*/

#define POWER_METER 1 // Can temporary disable Power Meter if needed
#define PROCESSOR_AGGREGATOR 1 // Can temporary disable Processor Aggregator if needed

#if 0
#define DbgTrace(a) Store(a, Debug)
#else
#define DbgTrace(a)
#endif

#define Notify80Socket0()  Notify(\_PR.CPU0, 0x80) \
                           Notify(\_PR.CPU1, 0x80) \
                           Notify(\_PR.CPU2, 0x80) \
                           Notify(\_PR.CPU3, 0x80) \
                           Notify(\_PR.CPU4, 0x80) \
                           Notify(\_PR.CPU5, 0x80) \
                           Notify(\_PR.CPU6, 0x80) \
                           Notify(\_PR.CPU7, 0x80) \
                           Notify(\_PR.CPU8, 0x80) \
                           Notify(\_PR.CPU9, 0x80) \
                           Notify(\_PR.CPUA, 0x80) \
                           Notify(\_PR.CPUB, 0x80) \
                           Notify(\_PR.CPUC, 0x80) \
                           Notify(\_PR.CPUD, 0x80) \
                           Notify(\_PR.CPUE, 0x80) \
                           Notify(\_PR.CPUF, 0x80)


#define Notify82Socket0()  Notify(\_PR.CPU0, 0x82) \
                           Notify(\_PR.CPU1, 0x82) \
                           Notify(\_PR.CPU2, 0x82) \
                           Notify(\_PR.CPU3, 0x82) \
                           Notify(\_PR.CPU4, 0x82) \
                           Notify(\_PR.CPU5, 0x82) \
                           Notify(\_PR.CPU6, 0x82) \
                           Notify(\_PR.CPU7, 0x82) \
                           Notify(\_PR.CPU8, 0x82) \
                           Notify(\_PR.CPU9, 0x82) \
                           Notify(\_PR.CPUA, 0x82) \
                           Notify(\_PR.CPUB, 0x82) \
                           Notify(\_PR.CPUC, 0x82) \
                           Notify(\_PR.CPUD, 0x82) \
                           Notify(\_PR.CPUE, 0x82) \
                           Notify(\_PR.CPUF, 0x82)

#define Notify80Socket1()
#define Notify82Socket1()
#define Notify80Socket2()
#define Notify82Socket2()
#define Notify80Socket3()
#define Notify82Socket3()
#define Notify80Socket4()
#define Notify82Socket4()
#define Notify80Socket5()
#define Notify82Socket5()
#define Notify80Socket6()
#define Notify82Socket6()
#define Notify80Socket7()
#define Notify82Socket7()

DefinitionBlock("SpsNm.aml", "SSDT", 2, "INTEL", "SpsNm   ", 2)
{
  External(\HWEN, FieldUnitObj)       // HWPM enabled as provided in BIOS_ACPI_PARAM::HWPMEnable
  External(\PETE, IntObj)
  External(\PSLM, PkgObj)
  External(\TSLM, PkgObj)

  External(\_SB.PRAD, DeviceObj)

  External(\_PR.CPU0, DeviceObj)
  External(\_PR.CPU1, DeviceObj)
  External(\_PR.CPU2, DeviceObj)
  External(\_PR.CPU3, DeviceObj)
  External(\_PR.CPU4, DeviceObj)
  External(\_PR.CPU5, DeviceObj)
  External(\_PR.CPU6, DeviceObj)
  External(\_PR.CPU7, DeviceObj)
  External(\_PR.CPU8, DeviceObj)
  External(\_PR.CPU9, DeviceObj)
  External(\_PR.CPUA, DeviceObj)
  External(\_PR.CPUB, DeviceObj)
  External(\_PR.CPUC, DeviceObj)
  External(\_PR.CPUD, DeviceObj)
  External(\_PR.CPUE, DeviceObj)
  External(\_PR.CPUF, DeviceObj)


  External(\_SB.PCI0, DeviceObj)
  External(\_SB.PCI0.HECI, DeviceObj)
  External(\_SB.PCI0.HEC2, DeviceObj)

  Scope(\)
  {
#if PROCESSOR_AGGREGATOR
    Name(PURA, 0)
    Name(PURB, 0)
    Name(PURE, 0)
#endif
    Name(THNU, 0x800)
  }

  Scope(\_SB.PCI0.HECI)
  {
    //
    // HECI-1 HFS register
    // It holds ME Firmware Status #1
    //
    OperationRegion(MFS1, PCI_Config, 0x40, 4)
    Field(MFS1, DWordAcc, NoLock, Preserve)
    {
      MEFS, 32
    }
    //
    // Return HECI-1 device status according to HECI visibility on PCI bus.
    //
    Method(_STA, 0)
    {
      DbgTrace("H1: _STA") 
      DbgTrace(MEFS)
      If (LEqual(MEFS, 0xFFFFFFFF)) // if HECI-1 hidded on disabled
      {
        Return(0x00)
      }
      Return(0x0F)
    }
  }

  Scope(\_SB.PCI0.HEC2)
  {
    //
    // H2ST is HECI-2 device status and is checked by OSPM before _INI
    // is called. Initially it is defined as not present and it is
    // updated by SPS ACPI hooks according to true ME state.
    // If ME firmware is not operational, or NM is not enabled
    // HECI-2 is reported present but disabled. If ME is operational
    // HECI-2 is present and functioning, but hidden from UI.
    //
    Method(_STA, 0)
    {
      Name(H2ST, 0x04)
      DbgTrace("H2: _STA") 
      DbgTrace(H2ST)
      Return(H2ST)
    }

    //
    // Define the region for HECI-2 registers mapped to HECI-2 MBAR.
    //
    //OperationRegion(HCSR, PCIBARTarget, 0x10, 0x10)
    // The PCIBARTarget is not implemented in several OSes.
    // The address below is just stub to be updated in UpdateAslCode().
    //
    OperationRegion(H2CS, SystemMemory, 0x8000000080000000, 0x10)
    Field(H2CS, DWordAcc, NoLock, Preserve)
    {
      CBWW,32, // write window - R_HECI_HOST_CB_WW
      // Bits of R_HECI_HOST_H_CSR (Host Control Status Register)
      HIE, 1,
      HIS, 1,
      HIG, 1,
      HRD, 1,
      HRS, 1,
         , 3,
      HRP, 8, // host read pointer
      HWP, 8, // host write pointer
      HBD, 8, // host buffer depth
      CBRW,32, // read window - R_HECI_ME_CB_RW
      // Bits of R_HECI_ME_CSR_HA (ME Control Status Register - Host Access)
      MIE, 1,
      MIS, 1,
      MIG, 1,
      MRD, 1,
      MRS, 1,
         , 3,
      MRP, 8, // ME read pointer
      MWP, 8, // ME write pointer
      MBD, 8  // ME buffer depth
    }

    //
    // Init method enables HECI-2 on OS boot
    //
    Method(_INI, 0, Serialized)
    {
      DbgTrace("H2: Init")
      HWAK()
    }

    //
    // Wake-up handler method - called from platform _WAK() method
    //
    Method(HWAK, 0, Serialized)
    {
      DbgTrace("H2: Wake-up")
      If (And(_STA, 0x08)) // If HECI-2 functional
      {
        Store(1, HRD) // set host ready
        Store(1, HIE) // set interrupt enable
        Store(1, HIG) // kick ME - I'm ready now
      }
    }

    //
    // Prepare-to-sleep handler method - called from platform _PTS() method
    //
    Method(HPTS, 0, Serialized)
    {
      DbgTrace("H2: Prepare-to-sleep")
      If (And(_STA, 0x08)) // If HECI-2 functional
      {
        Store(0, HIE) // clear interrupts enable
        Store(0, HRD) //clear host ready
        Store(1, HIG) // kick ME - I'm not ready any more
      }
    }

    //
    // Send one dword message via HECI-2
    //
    // Arg0 - The body of the message to send.
    //
    Method(SEND, 1, Serialized)
    {
      DbgTrace("H2: Send:")
      DbgTrace(Arg0)
      If (LAnd(HRD, MRD))  // If host and ME side ready
      {
        Store(0x80040011, CBWW)
        Store(Arg0, CBWW)
        Store(1, HIG)
        Store(1, HIE)
      }
    }

    //
    // Trigger HECI-2 reset
    //
    Method(RSET, 0, Serialized)
    {
      DbgTrace("H2: Reseting HECI")
      Store(1, HIS)
      Store(1, HRS)
      Store(1, HIG)
      Store(1, HIE)
    }

    //
    // HECI-2 queue read method. It retireves and dispatches ME requests.
    //
    Method(H2RD, 0, Serialized)
    {
      If (LNot(And(_STA, 0x08))) // If HECI-2 not functional just return
      {
        Return(0xFF)
      }
      If (MRS) // If reset request from ME - respond with host reset
      {
        DbgTrace("H2: HECI reset requested")
        RSET()
      }
      Else
      {
        If (MRD) // If ME is ready
        {
          If (LNot(HRD)) // ME signals ready, make this side ready too
          {
            DbgTrace("H2: HECI ready")
            Store(0, HRS)
            Store(1, HRD)
            Store(1, HIG)                    // kick ME - I'm ready now
          }
          If (LNotEqual(MWP, MRP))           // If buffer not empty
          {
            Store(CBRW, Local0)              // Read message header
            Store(CBRW, Local1)              // Read message DW1
            DbgTrace("H2: Got message:")
            DbgTrace(Local0)
            DbgTrace(Local1)
            Store(1, HIG)                    // message consumed kick ME - queue is not full

            And(Local0, 0x01FF0000, Local6)  // extract msg length and store in Local6
            And(Local1, 0x000000FF, Local7)  // extract msg command and store in Local7

            If (LEqual(Local7, 0x00))        // if performance change request
            {
              If (LEqual(Local6, 0x00040000)) // If it is 4 bytes long legacy message
              {
                DbgTrace("H2: Handle Performance Change legacy msg")
                //
                // Copy P-state and T-state to Local2 and Local3 to allow handling it
                // as if it was the new message format.
                //
                And(Local1, 0x00FF0000, Local2)
                Or(Local2,  ShiftLeft(And(Local1, 0x00FF0000), 8), Local2)
                Or(Local2, ShiftRight(And(Local1, 0x00FF0000), 8), Local2)
                Or(Local2, ShiftRight(And(Local1, 0x00FF0000),16), Local2)
                Store(Local2, Local3)
                And(Local1, 0xFF000000, Local4)
                Or(Local4, ShiftRight(And(Local1, 0xFF000000), 8), Local4)
                Or(Local4, ShiftRight(And(Local1, 0xFF000000),16), Local4)
                Or(Local4, ShiftRight(And(Local1, 0xFF000000),24), Local4)
                Store(Local4, Local5)
              }
              ElseIf (LEqual(Local6, 0x00140000)) // if new 20 bytes long message
              {
                DbgTrace("H2: Handle Performance Change")
                Store(CBRW, Local2)
                Store(CBRW, Local3)
                Store(CBRW, Local4)
                Store(CBRW, Local5)
              }
              Else
              {
                DbgTrace("H2: Unknown message, reseting HECI")
                RSET()
                Return (0xFF)
              }
              If (\HWEN)
              {
                DbgTrace("H2: HWPM enabled, sending NACK")
                //
                // If Hardware P-states are enabled send NACK for Performance Change message.
                //
                SEND(Or(Local1, 1)) // send NACK NM_CMD_MAXPTSTATE_HWPM_NACK
              }
              Else
              {
                If (And(\PETE, 0x80))
                {
                  //
                  // If P-states supported update current P-state limit
                  //
                  DbgTrace("H2: Notify 80 to processors")
                  And(           Local2    , 0xFF, Index(\PSLM, 0))
                  And(ShiftRight(Local2, 8), 0xFF, Index(\PSLM, 1))
                  And(ShiftRight(Local2,16), 0xFF, Index(\PSLM, 2))
                  And(ShiftRight(Local2,24), 0xFF, Index(\PSLM, 3))
                  And(           Local3    , 0xFF, Index(\PSLM, 4))
                  And(ShiftRight(Local3, 8), 0xFF, Index(\PSLM, 5))
                  And(ShiftRight(Local3,16), 0xFF, Index(\PSLM, 6))
                  And(ShiftRight(Local3,24), 0xFF, Index(\PSLM, 7))
                  //
                  // Notify processor objects to recalculate limits
                  //
                  Notify80Socket0()
                  Notify80Socket1()
                  Notify80Socket2()
                  Notify80Socket3()
                  Notify80Socket4()
                  Notify80Socket5()
                  Notify80Socket6()
                  Notify80Socket7()
                }
                If (And(\PETE, 0x40))
                {
                  //
                  // If T-states supported update current T-state limit
                  //
                  DbgTrace("H2: Notify 82 to processors")
                  And(           Local4    , 0xFF, Index(\TSLM, 0))
                  And(ShiftRight(Local4, 8), 0xFF, Index(\TSLM, 1))
                  And(ShiftRight(Local4,16), 0xFF, Index(\TSLM, 2))
                  And(ShiftRight(Local4,24), 0xFF, Index(\TSLM, 3))
                  And(           Local5    , 0xFF, Index(\TSLM, 4))
                  And(ShiftRight(Local5, 8), 0xFF, Index(\TSLM, 5))
                  And(ShiftRight(Local5,16), 0xFF, Index(\TSLM, 6))
                  And(ShiftRight(Local5,24), 0xFF, Index(\TSLM, 7))
                  //
                  // Notify processor objects to recalculate limit
                  //
                  Notify82Socket0()
                  Notify82Socket1()
                  Notify82Socket2()
                  Notify82Socket3()
                  Notify82Socket4()
                  Notify82Socket5()
                  Notify82Socket6()
                  Notify82Socket7()
                }
                //
                // Send ACK for P/T-state message
                //
                SEND(Or(Local1, \PETE))
              } // If (HWPE) Else
            } // If cmd is HECI_CMD_MAXPTSTATE
#if PROCESSOR_AGGREGATOR
            ElseIf (LAnd(LEqual(Local7, 0x03), LEqual(Local6, 0x00040000)))
            {
              DbgTrace("H2: Handle Processor Utilisation req")
              If (\PURE)
              {
                Store(Local1, \PURA)
                Notify(\_SB.PRAD, 0x80)
              }
              else
              {
                SEND(Local1)
              }
            }
#endif
#if POWER_METER
            ElseIf (LAnd(LEqual(Local7, 0x04), LEqual(Local6, 0x00040000)))
            {
              DbgTrace("H2: Handle Measurement req")
              Store(Multiply(And(ShiftRight(Local1, 0x10), 0xFFFF), 1000), PWR)
              SEND(Or(Local1, 0x80))
            }
            ElseIf (LAnd(LEqual(Local7, 0x05), LEqual(Local6, 0x00040000)))
            {
              DbgTrace("H2: Handle Get Averaging Interval req")
              Store(Multiply(And(ShiftRight(Local1, 0x10), 0xFFFF), 1000), PMAI)
              SEND(Or(Local1, 0x80))
              Notify(\_SB.PMT1, 0x84)
            }
            ElseIf (LAnd(LEqual(Local7, 0x06), LEqual(Local6, 0x00040000)))
            {
              DbgTrace("H2: Handle Get HW Limit req")
              Store(Multiply(And(ShiftRight(Local1, 0x10), 0xFFFF), 1000), HWL)
              SEND(Or(Local1, 0x80))
              Notify(\_SB.PMT1, 0x82)
            }
            ElseIf (LAnd(LEqual(Local7, 0x0C), LEqual(Local6, 0x00040000)))
            {
              DbgTrace("H2: Handle Power State Change req")
              //
              // ACK: Rsvd:Active:SeqNo:Command
              //
              SEND(Or(ShiftLeft(PMAV, 0x10), And(Or(Local1, 0x80), 0xFFFF)))
              If (LEqual(1, PMAV)) // If Power Meter is active
              {
                // capabilities possible change
                If (LEqual(And(ShiftRight(Local1, 0x10), 0xFF), 1))
                {
                  \_SB.PCI0.HEC2.SSFW(0, 0x08, 0, 0)
                }
                Store(Divide(HWL, 1000), Local2)
                \_SB.PCI0.HEC2.SSFW(0, 0x07, Local2, 0) // Set HWL
                Store(Divide(PMAI, 1000), Local2)
                \_SB.PCI0.HEC2.SSFW(0, 0x0A, Local2, 0) // Set PAI
                Store(Divide(PTPU, 1000), Local2)
                Store(Divide(PTPL, 1000), Local3)
                \_SB.PCI0.HEC2.SSFW(1, 0x0B, Local2, Local3) // Set PAI
              }
            }
            // if Get Current Capabilities ACK, or Report Capabilities REQ
            ElseIf (LAnd(Or(LEqual(Local7, 0x88),
                             LEqual(Local7, 0x0D)), LEqual(Local6, 0x00200000)))
            {
              DbgTrace("H2: Handle Get Current Capabilities ack or Report Capabilities req")
              Store(And(ShiftRight(Local1, 0x18), 0xFF), Index(PMCP, 0)) // supported capabilities
              Store(And(ShiftRight(Local1, 0x88), 0x01), Index(PMCP, 2)) // measurement type
              If (LEqual(And(ShiftRight(Local1, 0x10), 0x01), 0x01))    // Is hw limit configurable
              {
                Store(0xFFFFFFFF, Index(PMCP, 8))
              }
              Else
              {
                Store(0x00000000, Index(PMCP, 8))
              }
              Store(CBRW, Index(PMCP, 3))
              Store(CBRW, Index(PMCP, 4))
              Store(CBRW, Index(PMCP, 5))
              Store(CBRW, Index(PMCP, 6))
              Store(CBRW, Index(PMCP, 7))
              Store(CBRW, Index(PMCP, 9))
              Store(CBRW, Index(PMCP, 10))
              If (LEqual(Local7, 0x0D))
              {
                SEND(And(Or(Local1, 0x80), 0xFFFF))
              }
              Else
              {
                Store(1, HIG)
              }
              Store(1, PMAV)
              Notify(\_SB.PMT1, 0x80)
            }
            ElseIf (LAnd(LEqual(Local7, 0x09), LEqual(Local6, 0x00040000)))
            {
              DbgTrace("H2: Handle Power Trip Points exceeded req")
              SEND(Or(Local1, 0x80))
              Notify(\_SB.PMT1, 0x81)
            }
            ElseIf (LAnd(LEqual(Local7, 0x87), LEqual(Local6, 0x00040000)))
            {
              DbgTrace("H2: Handle Set Hardware Limit ack")
              Store(And(ShiftRight(Local1, 0x08), 0xFF), HLST)
            }
            ElseIf (LAnd(LEqual(Local7, 0x8A), LEqual(Local6, 0x00040000)))
            {
              DbgTrace("H2: Handle Power Averaging Interval ack")
              Store(And(ShiftRight(Local1, 0x08), 0xFF), AIST)
            }
            ElseIf (LAnd(LEqual(Local7, 0x8B), LEqual(Local6, 0x00040000)))
            {
              DbgTrace("H2: Handle Power Trip Points ack")
              Store(And(ShiftRight(Local1, 0x08), 0xFF), TPST)
            }
#endif // POWER_METER
            Else
            {
              DbgTrace("H2: Unknown message, reseting HECI")
              RSET()
              Return (0xFF)
            }
            Return(Local7) // The command recently handled is returned by H2RD
          } // If (buffer not empty)
        } // If (MRD)
      } // if (MRS )
      Return(0xFF) // There is no command to return
    } // Method(H2RD,...)

    //
    // SCI handler method - called from _L24() SCI handler
    //
    Method(HSCI, 0, Serialized)
    {
      DbgTrace("H2: SCI handler for HECI-2")
      If (HIS) // If Host Interrupt Status is Set (when ME_IG is set to 1)
      {
        Store(1, HIS)    // clear interrupt status before reading queue
        Store(8, Local0) // set timeout counter
        While (LNotEqual(H2RD(), 0xFF))
        {
          Decrement(Local0)
          If (LNot(Local0))
          {
            //
            // Too many requests from ME, reset queue to slow it down.
            //
            DbgTrace("H2: Got 8 messages in a row, reseting HECI-2")
            RSET()
            Break
          }
        }
      } // If (HIS)
      
      //
      // If ME reset occurred during H2RD() HECI may be isolated.
      // We need to make sure HECI Interrupt Enable (HIE) is set
      // before leaving HECI SCI.
      //
      Store(300, Local0) // 3 seconds timeout
      While (LAnd(LNot(HIE), Local0))
      {
        Decrement(Local0)
        Sleep(10)
        Store(1, HIE)
      }
    } // Method(HSCI,...)
    
#if POWER_METER
    //
    // Send request to ME and wait for answer.
    // It sends to ME message frame of 1 or 2 DWORDs,
    // Message header plus depending on input parameter.
    // Arg0  Message size: 0 - 1 DWORD (_SHL, _PAI), 1 - 2 DWORDS (_PTP).
    // Arg1  Command.
    // Arg2  Setting param. Depending on command, it represents
    //         Hardware limit [W], or averaging interval [s], or upper trip point [W]
    // Arg3  Setting param in 2 DWORDs message. Lower trip point [sec].
    //
    Method(SSFW, 4, Serialized)
    {
      If (LAnd(HRD, MRD))  // If host and ME side ready
      {
        Store(HIE, Local1) // Save HECI-2 SCI interrupt enable state
        Store(0, HIE)      // Disable HECI-2 SCI interrupt
        If (LEqual(Arg0, 0))
        {
          Store(0x80040011, CBWW)
          Store(Or(ShiftLeft(Arg2, 0x10), And(Arg1, 0x00FF)), CBWW)
        }
        ElseIf (LEqual(Arg0, 1))
        {
          Store(0x80080011, CBWW)
          Store(And (Arg1, 0xFF), CBWW)
          Store(Or(ShiftLeft(Arg3, 0x10), And(Arg2, 0xFFFF)), CBWW)
        }
        Store(1, HIG)
        Store(0, Local0)  // Timeout counter
        While (LNotEqual(Local0, 10))
        {
          If (LEqual(1, HIS))
          {
            Store(1, HIS) // Clear interrupt status
            If (LEqual(H2RD(), Or(Arg1, 0x80)))
            {
              Break
            }
          }
          Add(Local0, 1, Local0)
          Sleep(100)
        }
        Store(Local1, HIE)  // Restore HECI-2 SCI interrupt enable state
      }
    }
#endif // POWER_METER
  } // Scope(\_SB.PCI0.HEC2)

#if POWER_METER
  Scope (\_SB)
  {
    Name(PWR, 0xFFFFFFFF)  // Power reading
    Name(PMAI, 0xFFFFFFFF) // Power meter averaging interval
    Name(HWL, 0xFFFFFFFF)  // HW limit
    Name(PTPU, 0)          // Upper Trip Point
    Name(PTPL, 0)          // Lower Trip Point
    Name(PMAV, 0)          // Is Power Meter inferface active
    Name(HLST, 0)          // Set HW Limit Status, return code
    Name(AIST, 0)          // Set Averaging Interval Status, return code
    Name(TPST, 0)          // Set Trip Points Status, return code

    // Power Meter capabilities
    Name(PMCP, Package()
    {
      0x00000000, // Supported capabilities, (from LSB) -> measurement, trip points, HW enforced limit, notifications when HW limit is enforced, 3 x reserved, report only when discharging
      0x00000000, // Measurement unit, mW only
      0x00000000, // Measurement input power
      0x00000000, // Accuracy of PMT measurements, 95000 = 95%
      0xFFFFFFFF, // Sampling time of PMT in ms
      0x00000000, // Min averaging interval in ms
      0x00000000, // Max averaging interval in ms
      0xFFFFFFFF, // Hysteresis margin (mW)
      0x00000000, // Is HW limit configurable, 0x00000000 = read only, 0xFFFFFFFF = writable
      0x00000000, // Min configurable HW limit (mW)
      0x00000000, // Max configurable HW limit (mW)
      "Intel(R) Node Manager",// PMT model number, empty string if not supported
      "",         // PMT serial number,  empty string if not supported
      "Meter measures total domain" // OEM Info
    })

    Device(PMT1) //Power Meter # 1
    {
      Name(_HID, "ACPI000D")
      Name(_UID,  0x00)      // Unique ID required if many power meters

      //
      // Device status handler method - called by OSPM before _INI.
      // Power Meter status follows HECI-2 status but is not hidden from UI.
      //
      Method(_STA, 0, NotSerialized)
      {
        Return(Or(\_SB.PCI0.HEC2._STA, 0x04))
      }

      //
      // Power Meter Capabilities
      //
      Method(_PMC, 0, NotSerialized)
      {
        If (LEqual(PMAV, 0))
        {
          \_SB.PCI0.HEC2.SSFW(0, 0x08, 0, 0)
        }
        Return(PMCP)
      }

      //
      // Power Meter Devices
      //
      Method(_PMD, 0, NotSerialized)
      {
        Return(Package(){\_SB})
      }

      //
      // Power Meter Measurement
      //
      Method(_PMM, 0, NotSerialized)
      {
        Return(PWR)
      }

      //
      // Power Meter Averaging Interval
      //
      Method(_PAI, 1, NotSerialized)
      {
        Store(Divide(Arg0, 1000), Local0)
        Store(0x00000002, AIST)
        \_SB.PCI0.HEC2.SSFW(0, 0x0A, Local0, 0)
        If (LEqual(And (AIST, 0xFF), 0x00))
        {
          Store(Arg0, PMAI)
          Notify(\_SB.PMT1, 0x84)
        }
        Return(AIST)
      }

      //
      // Get Averaging Interval
      //
      Method(_GAI, 0, NotSerialized)
      {
        Return(PMAI)
      }

      //
      // Power Trip Points
      //
      Method(_PTP, 2, NotSerialized)
      {
        Store(Divide(Arg0, 1000), Local0)
        //
        // Round lower trip point to higher value
        //
        Store(Add(Arg1, 500), Local1)
        Store(Divide(Arg1, 1000), Local1)
        Store(0x00000002, TPST)
        \_SB.PCI0.HEC2.SSFW(1, 0x0B, Local0, Local1)
        If (LEqual(And(TPST, 0xFF), 0))
        {
          Store(Arg0, PTPU)
          Store(Arg1, PTPL)
        }
        Return (TPST)
      }

      //
      //Get Hardware Limit
      //
      Method(_GHL, 0, NotSerialized)
      {
        Return(HWL)
      }

      //
      // Set Hardware Limit
      //
      Method(_SHL, 1, NotSerialized)
      {
        Store(Divide(Arg0, 1000), Local0)
        Store(0x00000002, HLST)
        \_SB.PCI0.HEC2.SSFW (0, 0x07, Local0, 0)
        If (LEqual(And(HLST, 0xFF), 0x00))
        {
          Store(Arg0, HWL)
          Notify(\_SB.PMT1, 0x83)
        }
        Return (HLST)
      }
    } // Device(PMT1)
  } // Scope(SB)
#endif // POWER_METER

#if PROCESSOR_AGGREGATOR
  Scope(\_SB)
  {
    //
    // Processor Aggregator Device
    //
    Device(PRAD)
    {
      Name(_HID, "ACPI000C")

      Method(_STA, 0, NotSerialized)
      {
        Return(0x0F)
      }

      //
      // Processor Utilization Request method returns the
      // number of processors that platform wants to idle.
      //
      Method(_PUR, 0, Serialized)
      {
        Store("PRAD: Entering _PUR", Debug)
        Name(IDL, Package() {1, 0})
        And(Or(\PURA, 0x80), 0xFFFF, \PURB)
        And(ShiftRight(\PURA, 16), 0xffff, Index(IDL, 1))
        Return(IDL)
      }

      //
      // OSPM Status Indication method
      //
      Method(_OST, 3, Serialized)
      {
        Store("PRAD: Entering _OST", Debug)
        //
        // If _OST is called after _PUR send ACK to ME.
        // Arg2 conveys number of threads currently idled.
        // Calculate number of active threads and send it to ME.
        //
        If (LEqual(And(\PURB, 0xff), 0x83))
        {
          Subtract(\THNU, ToInteger(Arg2), Local0)
          Or(\PURB, ShiftLeft(Local0, 16), \PURB)
          \_SB.PCI0.HEC2.SEND(\PURB) // Send ACK for core idling
        }
      } // _OST
    } // Devide PRAD
  } // Scope SB
#endif

  Scope(\_GPE)
  {
    Method(_L6C, 0x0, Serialized)
    {
      \_SB.PCI0.HEC2.HSCI()
    }
  }
}// Definition Block
