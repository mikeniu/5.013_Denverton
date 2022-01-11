/*-----------------------------------------------------------------------------
-------------------------------------------------------------------------------

 Intel(R) Confidential
 Intel Goldmont Processor Power Management BIOS Reference Code
 Copyright (c) 2006 - 2014 Intel Corporation
 Filename:	APIST.ASL
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
	efficiency ratings) based upon power source changes, END user
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


DefinitionBlock (
  "APIST.aml",
  "SSDT",
  2,
  "PmRef",
  "ApIst",
  0x3000
  )
{
  External(\_PR.CPU0._PSS, MethodObj)
  External(\_PR.CPU0._PCT, MethodObj)
  External(\_PR.CPU0._PPC, IntObj)
  External(\_PR.CPU0._PSD, MethodObj)
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
  External (CFGD)
  External (PDC0)
  External (TCNT)

  Scope(\_PR.CPU1)
  {
    Method(_PPC,0)
    {
      Return(\_PR.CPU0._PPC)  // Return P0 _PPC value.
    }

    Method(_PCT,0)
    {
      Return(\_PR.CPU0._PCT)  // Return P0 _PCT.
    }

    Method(_PSS,0)
    {
      //Return the same table as CPU0 for CMP cases.
      Return(\_PR.CPU0._PSS)
    }

    // The _PSD object provides information to the OSPM related
    // to P-State coordination between processors in a multi-processor
    // configurations.
    //
    Method(_PSD,0)
    {
      Return(Package(){    // HW_ALL
              Package(){
                         5,              // # entries.
                         0,              // Revision.
                         1,              // Domain #.
                         0xFE,           // Coord Type- HW_ALL.
                         1               // # processors.
              }
            })
    }

  }

  Scope(\_PR.CPU2)
  {
    Method(_PPC,0)
    {
      Return(\_PR.CPU0._PPC)  // Return CPU0 _PPC value.
    }

    Method(_PCT,0)
    {
      Return(\_PR.CPU0._PCT)  // Return CPU0 _PCT value.
    }

    Method(_PSS,0)
    {
      Return(\_PR.CPU0._PSS)  // Return CPU0 _PSS.
    }

    // The _PSD object provides information to the OSPM related
    // to P-State coordination between processors in a multi-processor
    // configurations.
    //
    Method(_PSD,0)
    {
      Return(Package(){    // HW_ALL
              Package(){
                         5,              // # entries.
                         0,              // Revision.
                         2,              // Domain #.
                         0xFE,           // Coord Type- HW_ALL.
                         1               // # processors.
              }
            })
    }

  }


  Scope(\_PR.CPU3)
  {
    Method(_PPC,0)
    {
      Return(\_PR.CPU0._PPC)  // Return CPU0 _PPC value.
    }

    Method(_PCT,0)
    {
      Return(\_PR.CPU0._PCT)  // Return CPU0 _PCT value.
    }

    Method(_PSS,0)
    {
      Return(\_PR.CPU0._PSS)  // Return CPU0 _PSS.
    }

    // The _PSD object provides information to the OSPM related
    // to P-State coordination between processors in a multi-processor
    // configurations.
    //
    Method(_PSD,0)
    {
      Return(Package(){    // HW_ALL
              Package(){
                         5,              // # entries.
                         0,              // Revision.
                         3,              // Domain #.
                         0xFE,           // Coord Type- HW_ALL.
                         1               // # processors.
              }
            })
    }

  }

  Scope(\_PR.CPU4)
  {
    Method(_PPC,0)
    {
      Return(\_PR.CPU0._PPC)	// Return P0 _PPC value.
    }

    Method(_PCT,0)
    {
      Return(\_PR.CPU0._PCT)	// Return P0 _PCT.
    }

    Method(_PSS,0)
    {
      //Return the same table as CPU0 for CMP cases.
      Return(\_PR.CPU0._PSS)
    }

    // The _PSD object provides information to the OSPM related
    // to P-State coordination between processors in a multi-processor
    // configurations.
    //
    Method(_PSD,0)
    {
      Return(Package(){    // HW_ALL
              Package(){
                         5,              // # entries.
                         0,              // Revision.
                         4,              // Domain #.
                         0xFE,           // Coord Type- HW_ALL.
                         1               // # processors.
              }
            })
    }

  }

  Scope(\_PR.CPU5)
  {
    Method(_PPC,0)
    {
      Return(\_PR.CPU0._PPC)	// Return P0 _PPC value.
    }

    Method(_PCT,0)
    {
      Return(\_PR.CPU0._PCT)	// Return P0 _PCT.
    }

    Method(_PSS,0)
    {
      //Return the same table as CPU0 for CMP cases.
      Return(\_PR.CPU0._PSS)
    }

    // The _PSD object provides information to the OSPM related
    // to P-State coordination between processors in a multi-processor
    // configurations.
    //
    Method(_PSD,0)
    {
      Return(Package(){    // HW_ALL
              Package(){
                         5,              // # entries.
                         0,              // Revision.
                         5,              // Domain #.
                         0xFE,           // Coord Type- HW_ALL.
                         1               // # processors.
              }
            })
    }

  }

  Scope(\_PR.CPU6)
  {
    Method(_PPC,0)
    {
      Return(\_PR.CPU0._PPC)	// Return P0 _PPC value.
    }

    Method(_PCT,0)
    {
      Return(\_PR.CPU0._PCT)	// Return P0 _PCT.
    }

    Method(_PSS,0)
    {
      //Return the same table as CPU0 for CMP cases.
      Return(\_PR.CPU0._PSS)
    }

    // The _PSD object provides information to the OSPM related
    // to P-State coordination between processors in a multi-processor
    // configurations.
    //
    Method(_PSD,0)
    {
      Return(Package(){    // HW_ALL
              Package(){
                         5,              // # entries.
                         0,              // Revision.
                         6,              // Domain #.
                         0xFE,           // Coord Type- HW_ALL.
                         1               // # processors.
              }
            })
    }

  }
  Scope(\_PR.CPU7)
  {
    Method(_PPC,0)
    {
      Return(\_PR.CPU0._PPC)	// Return P0 _PPC value.
    }

    Method(_PCT,0)
    {
      Return(\_PR.CPU0._PCT)	// Return P0 _PCT.
    }

    Method(_PSS,0)
    {
      //Return the same table as CPU0 for CMP cases.
      Return(\_PR.CPU0._PSS)
    }

    // The _PSD object provides information to the OSPM related
    // to P-State coordination between processors in a multi-processor
    // configurations.
    //
    Method(_PSD,0)
    {
      Return(Package(){    // HW_ALL
              Package(){
                         5,              // # entries.
                         0,              // Revision.
                         7,              // Domain #.
                         0xFE,           // Coord Type- HW_ALL.
                         1               // # processors.
              }
            })
    }
  }

  Scope(\_PR.CPU8)
  {
    Method(_PPC,0)
    {
      Return(\_PR.CPU0._PPC)	// Return P0 _PPC value.
    }

    Method(_PCT,0)
    {
      Return(\_PR.CPU0._PCT)	// Return P0 _PCT.
    }

    Method(_PSS,0)
    {
      //Return the same table as CPU0 for CMP cases.
      Return(\_PR.CPU0._PSS)
    }

    // The _PSD object provides information to the OSPM related
    // to P-State coordination between processors in a multi-processor
    // configurations.
    //
    Method(_PSD,0)
    {
      Return(Package(){    // HW_ALL
              Package(){
                         5,              // # entries.
                         0,              // Revision.
                         8,              // Domain #.
                         0xFE,           // Coord Type- HW_ALL.
                         1               // # processors.
              }
            })
    }
  }

  Scope(\_PR.CPU9)
  {
    Method(_PPC,0)
    {
      Return(\_PR.CPU0._PPC)	// Return P0 _PPC value.
    }

    Method(_PCT,0)
    {
      Return(\_PR.CPU0._PCT)	// Return P0 _PCT.
    }

    Method(_PSS,0)
    {
      //Return the same table as CPU0 for CMP cases.
      Return(\_PR.CPU0._PSS)
    }

    // The _PSD object provides information to the OSPM related
    // to P-State coordination between processors in a multi-processor
    // configurations.
    //
    Method(_PSD,0)
    {
      Return(Package(){    // HW_ALL
              Package(){
                         5,              // # entries.
                         0,              // Revision.
                         9,              // Domain #.
                         0xFE,           // Coord Type- HW_ALL.
                         1               // # processors.
              }
            })
    }
  }

  Scope(\_PR.CPUA)
  {
    Method(_PPC,0)
    {
      Return(\_PR.CPU0._PPC)	// Return P0 _PPC value.
    }

    Method(_PCT,0)
    {
      Return(\_PR.CPU0._PCT)	// Return P0 _PCT.
    }

    Method(_PSS,0)
    {
      //Return the same table as CPU0 for CMP cases.
      Return(\_PR.CPU0._PSS)
    }

    // The _PSD object provides information to the OSPM related
    // to P-State coordination between processors in a multi-processor
    // configurations.
    //
    Method(_PSD,0)
    {
      Return(Package(){    // HW_ALL
              Package(){
                         5,              // # entries.
                         0,              // Revision.
                         10,              // Domain #.
                         0xFE,           // Coord Type- HW_ALL.
                         1               // # processors.
              }
            })
    }
  }

  Scope(\_PR.CPUB)
  {
    Method(_PPC,0)
    {
      Return(\_PR.CPU0._PPC)	// Return P0 _PPC value.
    }

    Method(_PCT,0)
    {
      Return(\_PR.CPU0._PCT)	// Return P0 _PCT.
    }

    Method(_PSS,0)
    {
      //Return the same table as CPU0 for CMP cases.
      Return(\_PR.CPU0._PSS)
    }

    // The _PSD object provides information to the OSPM related
    // to P-State coordination between processors in a multi-processor
    // configurations.
    //
    Method(_PSD,0)
    {
      Return(Package(){    // HW_ALL
              Package(){
                         5,              // # entries.
                         0,              // Revision.
                         11,              // Domain #.
                         0xFE,           // Coord Type- HW_ALL.
                         1               // # processors.
              }
            })
    }
  }

  Scope(\_PR.CPUC)
  {
    Method(_PPC,0)
    {
      Return(\_PR.CPU0._PPC)	// Return P0 _PPC value.
    }

    Method(_PCT,0)
    {
      Return(\_PR.CPU0._PCT)	// Return P0 _PCT.
    }

    Method(_PSS,0)
    {
      //Return the same table as CPU0 for CMP cases.
      Return(\_PR.CPU0._PSS)
    }

    // The _PSD object provides information to the OSPM related
    // to P-State coordination between processors in a multi-processor
    // configurations.
    //
    Method(_PSD,0)
    {
      Return(Package(){    // HW_ALL
              Package(){
                         5,              // # entries.
                         0,              // Revision.
                         12,              // Domain #.
                         0xFE,           // Coord Type- HW_ALL.
                         1               // # processors.
              }
            })
    }
  }

  Scope(\_PR.CPUD)
  {
    Method(_PPC,0)
    {
      Return(\_PR.CPU0._PPC)	// Return P0 _PPC value.
    }

    Method(_PCT,0)
    {
      Return(\_PR.CPU0._PCT)	// Return P0 _PCT.
    }

    Method(_PSS,0)
    {
      //Return the same table as CPU0 for CMP cases.
      Return(\_PR.CPU0._PSS)
    }

    // The _PSD object provides information to the OSPM related
    // to P-State coordination between processors in a multi-processor
    // configurations.
    //
    Method(_PSD,0)
    {
      Return(Package(){    // HW_ALL
              Package(){
                         5,              // # entries.
                         0,              // Revision.
                         13,              // Domain #.
                         0xFE,           // Coord Type- HW_ALL.
                         1               // # processors.
              }
            })
    }
  }

  Scope(\_PR.CPUE)
  {
    Method(_PPC,0)
    {
      Return(\_PR.CPU0._PPC)	// Return P0 _PPC value.
    }

    Method(_PCT,0)
    {
      Return(\_PR.CPU0._PCT)	// Return P0 _PCT.
    }

    Method(_PSS,0)
    {
      //Return the same table as CPU0 for CMP cases.
      Return(\_PR.CPU0._PSS)
    }

    // The _PSD object provides information to the OSPM related
    // to P-State coordination between processors in a multi-processor
    // configurations.
    //
    Method(_PSD,0)
    {
      Return(Package(){    // HW_ALL
              Package(){
                         5,              // # entries.
                         0,              // Revision.
                         14,              // Domain #.
                         0xFE,           // Coord Type- HW_ALL.
                         1               // # processors.
              }
            })
    }
  }

  Scope(\_PR.CPUF)
  {
    Method(_PPC,0)
    {
      Return(\_PR.CPU0._PPC)	// Return P0 _PPC value.
    }

    Method(_PCT,0)
    {
      Return(\_PR.CPU0._PCT)	// Return P0 _PCT.
    }

    Method(_PSS,0)
    {
      //Return the same table as CPU0 for CMP cases.
      Return(\_PR.CPU0._PSS)
    }

    // The _PSD object provides information to the OSPM related
    // to P-State coordination between processors in a multi-processor
    // configurations.
    //
    Method(_PSD,0)
    {
      Return(Package(){    // HW_ALL
              Package(){
                         5,              // # entries.
                         0,              // Revision.
                         15,              // Domain #.
                         0xFE,           // Coord Type- HW_ALL.
                         1               // # processors.
              }
            })
    }
  }
} // End of Definition Block
