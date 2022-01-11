/*-----------------------------------------------------------------------------
-------------------------------------------------------------------------------

 Intel(R) Confidential

 Intel Goldmont Processor Power Management BIOS Reference Code

 Copyright (c) 2006-2014 Intel Corporation

 Filename:	Tstates.ASL

 Revision:	Refer to Readme

 Date:		Refer to Readme

--------------------------------------------------------------------------------
-------------------------------------------------------------------------------

 This Processor Power Management BIOS Source Code is furnished under license
 and may only be used or copied in accordance with the terms of the license.
 The information in this document is furnished for informational use only, is
 subject to change without notice, and should not be construed as a commitment
 by Intel Corporation. Intel Corporation assumes no responsibility or liability
 for any errors or inaccuracies that may appear in this document or any
 software that may be provided in association with this document.

 Except as permitted by such license, no part of this document may be
 reproduced, stored in a retrieval system, or transmitted in any form or by
 any means without the express written consent of Intel Corporation.

 WARNING: You are authorized and licensed to install and use this BIOS code
 ONLY on an IST PC. This utility may damage any system that does not
 meet these requirements.

 	An IST PC is a computer which
 	(1) Is capable of seamlessly and automatically transitioning among
 	multiple performance states (potentially operating at different
 	efficiency ratings) based upon power source changes, end user
 	preference, processor performance demand, and thermal conditions; and
 	(2) Includes an Intel Pentium II processors, Intel Pentium III
	processor, Mobile Intel Pentium III Processor-M, Mobile Intel Pentium 4
	Processor-M, Intel Pentium M Processor, or any other future Intel
	processors that incorporates the capability to transition between
	different performance states by altering some, or any combination of,
	the following processor attributes: core voltage, core frequency, bus
	frequency, number of processor cores available, or any other attribute
	that changes the efficiency (instructions/unit time-power) at which the
	processor operates.

-------------------------------------------------------------------------------
-------------------------------------------------------------------------------

NOTES:
	(1) <TODO> - IF the trap range and port definitions do not match those
	specified by this reference code, this file must be modified IAW the
	individual implmentation.

--------------------------------------------------------------------------------
------------------------------------------------------------------------------*/
/*++
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/                                                                      

   Name(_TPC, 0)	// All T-States are available

   //
   // T-State Control/Status interface
   //
   Method(_PTC, 0)
   {
     Return(Package() {
            ResourceTemplate(){Register(FFixedHW, 0, 0, 0)},
            ResourceTemplate(){Register(FFixedHW, 0, 0, 0)}
     })
   }

   Method(_TSS, 0)
   {

     If(And(CFGD,0x0001)){

       // Update "Power" fields of TSSI/TSSM with the LFM
       // power data IF _PSS is available
       //
       IF (LAnd(LNot(TSSF),CondRefOf(_PSS)))
       {
         Store(_PSS, Local0)
         Store(SizeOf(Local0), Local1)	// _PSS size
         Decrement(Local1)		// Index of LFM
         Store(DerefOf(Index(DerefOf(Index(Local0,Local1)),1)), Local2)	// LFM Power

         Store(0, Local3)
         While(LLess(Local3, SizeOf(TSSM)))
         {
           Store(Divide(Multiply(Local2, Subtract(8, Local3)), 8),
             Local4)		// Power for this TSSM entry
           Store(Local4,Index(DerefOf(Index(TSSM,Local3)),1))
           Increment(Local3)
         }
         Store(Ones, TSSF)		// TSSI/TSSM are updated
       }
       Return(TSSM)
     }
    Return(Package(){100, 1000, 0, 0x00, 0}) 
  }

/*
   //
   // T-State Dependency Optional use the same as PSD but the bit locations are diff
   //
   Method(_TSD, 0)
   {
     If(LAnd(And(CFGD,0x0800000),LNot(And(PDC0,4))))
     {
       Return(Package(){	// SW_ANY
					Package(){
						5,	          // # entries.
						0,	          // Revision.
						0,	          // Domain #.
						0xFD,	        // Coord Type- SW_ANY
						4	            // # processors.
					}
				})
			}
			If(LAnd(And(CFGD,0x1000000),LNot(And(PDC0,4))))
			{
				Return(Package(){	// SW_ANY
					Package(){
						5,	          // # entries.
						0,	          // Revision.
						0,	          // Domain #.
						0xFD,	        // Coord Type- SW_ANY
						2	            // # processors.
					}
				})
			}
			Return(Package(){		// SW_ALL
				Package(){
					5,		          // # entries.
					0,		          // Revision.
					0,		          // Domain #.
					0xFC,		        // Coord Type- SW_ALL
					1		            // # processors.
				}
			})
   }
*/


