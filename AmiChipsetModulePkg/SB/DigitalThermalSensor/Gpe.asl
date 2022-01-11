//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

OperationRegion(PMIO, SystemIo, 0x188C, 1) // GPE0a_STS
Field (PMIO, ByteAcc, NoLock, Preserve)
{
  RESV, 1,  // Reserved
  HPST, 1,  // Hot Plug Status
  SGPS, 1,  // Software GPE status
}

Scope(\_GPE) {
  // Software GPE caused the event.
  Method(_L62) {
    // Clear the SWGPE Status Bit.
    Store(1,SGPS)
    // Handle the DTS Thermal Event.
    Notify(\_TZ.TZ01,0x80)
    
    // Handle Whea Events
    #ifdef WHEA_SUPPORT_FLAG
      Store(0xBB, \_SB.IO80)
      Notify (\_SB.WERR, 0x80)        // Notify a WHEA error event (actually for all CE's)
    #endif
  }
}

Scope (\_TZ) {
ThermalZone (TZ01) {
    // Return the temperature at which the OS must perform a Critcal
    // Shutdown.
    Method (_CRT) {
      Return (Add(2732, Multiply(\CRTT, 10)))  
    }

    // Return the DTS Temperature to the OS.
    Method (_TMP) {
      // Return the Max ( BSP temp, AP temp)
      If(LGreaterEqual(DTS1, DTS2))
      {
          Return(Add(2732, Multiply(DTS1, 10)))
      }
      Return(Add(2732, Multiply(DTS2, 10)))
    } // End Method(_TMP)
  } // End of ThermalZone (TZ01)
} // End Scope (_TZ)


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
