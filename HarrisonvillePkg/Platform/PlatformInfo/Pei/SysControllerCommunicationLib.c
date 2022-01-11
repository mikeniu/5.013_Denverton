/*++
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
--*/
/** @file
  System controller comunication library.

  Copyright (c) 2010-2015, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

**/

#include "SysControllerCommunicationLib.h"

#define STALL_TIME                 1000000
#define TRANSACTION_DELAY                 6000
#define STALL_PERIOD               10      // 10 microseconds



EFI_STATUS
I2CSysCtrlWrite(
  IN      UINT16 SmBusBarAddr,
  IN      UINT8  SlaveAddress,
  IN      UINT8  OpCode,
  IN      UINT8  Length,
  IN OUT  UINT8 *Buffer
 );

EFI_STATUS
I2CSysCtrlRead(
  IN      UINT16 SmBusBarAddr,
  IN      UINT8  SlaveAddress,
  IN      UINT8  Offset,
  IN      UINT8  Length,
  IN OUT  UINT8 *Buffer
 );

VOID
EnableI2C()
{
    //
    //Enable I2C
    //
    MmioWrite8( MmPciBase(0,
                     PCI_DEVICE_NUMBER_PCH_SMBUS,
                     PCI_FUNCTION_NUMBER_PCH_SMBUS) +
                     R_PCH_SMBUS_HOSTC,
                     (B_PCH_SMBUS_HOSTC_SPDWD | B_PCH_SMBUS_HOSTC_HST_EN | B_PCH_SMBUS_HOSTC_I2C_EN));

}

VOID
DisableI2C()
{
    //
    //Disable I2C Mode
    //
    MmioWrite8( MmPciBase(0,
                       PCI_DEVICE_NUMBER_PCH_SMBUS,
                       PCI_FUNCTION_NUMBER_PCH_SMBUS)+
                       R_PCH_SMBUS_HOSTC,
                       (B_PCH_SMBUS_HOSTC_SPDWD | B_PCH_SMBUS_HOSTC_HST_EN));

}

VOID
TimerStall (
  UINTN Usec
  )
/*++

Routine Description:

  This routine stalls the execution

Arguments:

  Microseconds to stall

Returns:

  None

--*/
{
  UINTN   Counter;
  UINTN   i;
  UINT32  Data32;
  UINT32  PrevData;

  Counter   = Usec * 3;
  PrevData  = IoRead32 (PCH_ACPI_BASE_ADDRESS + R_PCH_ACPI_PM1_TMR) & B_PCH_ACPI_PM1_TMR_VAL;
  for (i = 0; i < Counter;) {
    Data32 = IoRead32 (PCH_ACPI_BASE_ADDRESS + R_PCH_ACPI_PM1_TMR) & B_PCH_ACPI_PM1_TMR_VAL;
    if (Data32 < PrevData) { // Reset if there is a overlap
      PrevData = Data32;
      continue;
    }

    if (Data32 == PrevData) {
      IoWrite8(0x80, IoRead8(0x80)); // Soft stall
      i += 1;
    }
    else {
      i += (Data32 - PrevData);
    }

    PrevData = Data32;
  }
}

VOID
I2CSoftwareBusRelease(UINT16 SmBusBarAddr)
{
    UINT8 Data8;
    UINTN ReleaseTimeout = 90000;

    Data8 = IoRead8(SmBusBarAddr + R_PCH_SMBUS_SMBC);
    if (!(Data8 & B_PCH_SMBUS_SMBDATA_CUR_STS)
                    || !(Data8 & B_PCH_SMBUS_SMBCLK_CUR_STS))
    {
        DEBUG((DEBUG_INFO,"Releasing SmbusClock...\n"));
        while (ReleaseTimeout)
        {
            Data8 = IoRead8(SmBusBarAddr + R_PCH_SMBUS_SMBC);
            if (!(Data8 & B_PCH_SMBUS_SMBDATA_CUR_STS)
                            || !(Data8 & B_PCH_SMBUS_SMBCLK_CUR_STS))
            {
                //DEBUG ((EFI_D_INFO,"#"));
                Data8 &= ~B_PCH_SMBUS_SMBCLK_CTL;
                IoWrite8(SmBusBarAddr + R_PCH_SMBUS_SMBC,
                         Data8);

                TimerStall(10);
                Data8 |= B_PCH_SMBUS_SMBCLK_CTL;
                IoWrite8(SmBusBarAddr + R_PCH_SMBUS_SMBC,
                         Data8);
                TimerStall(10);
            }
            else
            {
                break;
            }
            ReleaseTimeout--;
        }
        if (!(Data8 & B_PCH_SMBUS_SMBDATA_CUR_STS)
                        || !(Data8 & B_PCH_SMBUS_SMBCLK_CUR_STS))
        {
            DEBUG((DEBUG_INFO,"Failing releasing the bus, try to restart the platform\n"));
            ASSERT(0);
        }
    }
}

EFI_STATUS
I2CSysCtrlRead(
  IN      UINT16 SmBusBarAddr,
  IN      UINT8  SlaveAddress,
  IN      UINT8  Offset,
  IN      UINT8  Length,
  IN OUT  UINT8 *Buffer
 )
{

#ifdef SYS_CTRL_COM_DISABLED
  DEBUG ((EFI_D_INFO,"ERROR: Syscontroll comunication disabled in compile time\n"));
  ASSERT(FALSE);
#else
  UINT8     Index;
  UINT8     Data8;
  UINTN     StallIndex;
  UINTN     StallTries;
  StallTries = STALL_TIME;

  //
  // Clear Host Data Registers
  //
  IoWrite8(SmBusBarAddr + R_PCH_SMBUS_HD0, 0);
  IoWrite8(SmBusBarAddr + R_PCH_SMBUS_HD1, 0);
  IoWrite8(SmBusBarAddr + R_PCH_SMBUS_HBD, 0);

  for (StallIndex = 0; StallIndex < StallTries; StallIndex++) {
    //
    //  Check for transaction errors
    //
    Data8 = IoRead8 (SmBusBarAddr + R_PCH_SMBUS_HSTS);
    if (Data8 & (B_PCH_SMBUS_DERR | B_PCH_SMBUS_BERR | B_PCH_SMBUS_INTR | B_PCH_SMBUS_HBSY)) {
      //
      // Clear Status Register
      //
      IoWrite8 (SmBusBarAddr + R_PCH_SMBUS_HSTS, B_PCH_SMBUS_HSTS_ALL);
      TimerStall (STALL_PERIOD);
     } else {
       break;
     }
   }

    //
    // Check if we timed out waiting to Status bits to clear
    //
    if (StallIndex == StallTries)
    {
        return EFI_TIMEOUT;
    }

  I2CSoftwareBusRelease(SmBusBarAddr);


  //gapalomi
  IoWrite8 (SmBusBarAddr + R_PCH_SMBUS_HCTL, (UINT8) (V_PCH_SMBUS_SMB_CMD_BLOCK));

  //
  // Check for SDA
  //

  EnableI2C();

  //
  // Set Auxiliary Control register
  //
  IoWrite8 (SmBusBarAddr + R_PCH_SMBUS_AUXC, 0);

  IoWrite8 (SmBusBarAddr + R_PCH_SMBUS_HCMD, 0x52);

  //
  // Reset the pointer of the internal buffer
  //
  IoRead8 (SmBusBarAddr + R_PCH_SMBUS_HCTL);

  //
  // Set SMBus slave address for the device to send/receive from
  // For I2c Read the value written into bit 0 of the Transmit Slave
  // SMB_Base + 04h[Bit 0 - Direction bit] should be a don’t care.
  //But do to LPT PCH still interpreting this bit, even for I2C reads,
  //we need to ensure it is set to 1=read. Otherwise, all I2C Reads
  //will be blocked, if target address is Ax.
  //
  IoWrite8 (SmBusBarAddr + R_PCH_SMBUS_TSA, (SlaveAddress | B_PCH_SMBUS_RW_SEL));

  //
  // Set Host Data 1 Register
  // Provide offset address to read since FRU is an EEPROM.
  //
  IoWrite8 (SmBusBarAddr + R_PCH_SMBUS_HD1, Offset);


  Data8 = IoRead8 (SmBusBarAddr + R_PCH_SMBUS_SCMD);
  Data8 |= B_PCH_SMBUS_SMBALERT_DIS;
  IoWrite8 (SmBusBarAddr + R_PCH_SMBUS_SCMD,Data8);
  //
  // Set Control Register (Initiate Operation, Interrupt disabled)
  //
  Data8 = IoRead8 (SmBusBarAddr + R_PCH_SMBUS_HCTL);
  IoWrite8 (SmBusBarAddr + R_PCH_SMBUS_HCTL, (UINT8) (Data8 |B_PCH_SMBUS_START));

  //
  // Read data from HD0 since this FRU is an I2C device instead of SMBUS.
  //
  for (Index = 0; Index < Length; Index++) {

    for (StallIndex = 0; StallIndex < StallTries; StallIndex++) {
      //
      //  Check for transaction errors
      //
      Data8 = IoRead8 (SmBusBarAddr + R_PCH_SMBUS_HSTS);
      if (Data8 & (B_PCH_SMBUS_DERR | B_PCH_SMBUS_BERR)) {

        DEBUG ((DEBUG_INFO,"ERROR reading FPGA, HSTS:0x%x\n",Data8));
        Data8 = IoRead8 (SmBusBarAddr + R_PCH_SMBUS_HCTL);
        Data8 |= B_PCH_SMBUS_KILL;
        IoWrite8 (SmBusBarAddr + R_PCH_SMBUS_HCTL,Data8);
        Data8 &= ~B_PCH_SMBUS_KILL;
        TimerStall (STALL_PERIOD);
        IoWrite8 (SmBusBarAddr + R_PCH_SMBUS_HCTL,Data8);

        DisableI2C();
        return EFI_DEVICE_ERROR;
      } else {
        //
        // Check for transaction completed
        //
        Data8 = IoRead8 (SmBusBarAddr + R_PCH_SMBUS_HSTS);
        if (Data8 & B_PCH_SMBUS_BYTE_DONE_STS) {
          //
          // Read one byte of Data from Host Block Data Register
          //
          *(Buffer)  = IoRead8 (SmBusBarAddr + R_PCH_SMBUS_HBD);
          break;
        } else {
            TimerStall (STALL_PERIOD);
        }
      }
    }

    //
    // Check this read is the n-1 byte or first byte is the last byte
    //
    if(Index == (Length - 1)) {
      //
      // Set the Last Byte Bit in the Host Control Register
      //
      Data8 = IoRead8(SmBusBarAddr + R_PCH_SMBUS_HCTL);
      Data8 |= B_PCH_SMBUS_LAST_BYTE;
      IoWrite8 (SmBusBarAddr + R_PCH_SMBUS_HCTL, Data8);
    }

    //
    // Clear the Byte Done Status bit in the Host Status Register
    //
    IoWrite8 (SmBusBarAddr + R_PCH_SMBUS_HSTS, B_PCH_SMBUS_BYTE_DONE_STS);
    Buffer++;

  }

  //
  // Clear Status Registers and exit
  //
  IoWrite8 (SmBusBarAddr + R_PCH_SMBUS_HSTS, B_PCH_SMBUS_HSTS_ALL);

  DisableI2C();
#endif
  return EFI_SUCCESS;
}

EFI_STATUS
SysCtrlReadDwordRetries(UINT32 Offset,
                        UINT32 *Data)
{
    EFI_STATUS Status;
    UINT8 Buffer[0x5] = { 0 };

    Status = EFI_SUCCESS;

    Buffer[0] = (UINT8)(Offset >> 0);
    Buffer[1] = (UINT8)(Offset >> 8);
    Buffer[2] = (UINT8)(Offset >> 16);
    Buffer[3] = (UINT8)(Offset >> 24);
    Buffer[4] = (UINT8) 4;

    TimerStall(TRANSACTION_DELAY);
    Status = I2CSysCtrlWrite(SMBUS_BASE_ADDRESS,
                             VF_SC_SMBUS_ADDRESS,
                             0x51,
                             0x5,
                             Buffer);
    if (EFI_ERROR(Status))
    {
        return Status;
    }
//    memset(Buffer,0,5);
    SetMem(Buffer,5,0);

    TimerStall(TRANSACTION_DELAY);
    Status = I2CSysCtrlRead(SMBUS_BASE_ADDRESS,
                            VF_SC_SMBUS_ADDRESS | 1,
                            0x52,
                            4,
                            Buffer);

    if (EFI_ERROR(Status))
    {
        return Status;
    }

    *Data = Buffer[0] | (Buffer[1] << 8) | (Buffer[2] << 16) | (Buffer[3] << 24);
    return Status;
}


EFI_STATUS
SysCtrlReadWordRetries(UINT32 Offset,
                        UINT8 *Data)
{
    EFI_STATUS Status;
    UINT8 Buffer[0x5] = { 0 };

    Status = EFI_SUCCESS;

    Buffer[0] = (UINT8)(Offset >> 0);
    Buffer[1] = (UINT8)(Offset >> 8);
    Buffer[2] = (UINT8)(Offset >> 16);
    Buffer[3] = (UINT8)(Offset >> 24);
    Buffer[4] = (UINT8) 4;

    TimerStall(TRANSACTION_DELAY);
    Status = I2CSysCtrlWrite(SMBUS_BASE_ADDRESS,
                             VF_SC_SMBUS_ADDRESS,
                             0x51,
                             0x5,
                             Buffer);
    if (EFI_ERROR(Status))
    {
        return Status;
    }
    SetMem(Buffer,0,5); // Denverton AptioV override 

    TimerStall(TRANSACTION_DELAY);
    Status = I2CSysCtrlRead(SMBUS_BASE_ADDRESS,
                            VF_SC_SMBUS_ADDRESS | 1,
                            0x52,
                            1,
                            Buffer);

    if (EFI_ERROR(Status))
    {
        return Status;
    }

    *Data = Buffer[1];
    return Status;
}


EFI_STATUS
SysCtrlReadWord(UINT32 Offset, UINT8 *Data)
{
  EFI_STATUS Status;
  UINT8 Retries = 100;
  do
  {

      Status = SysCtrlReadWordRetries(Offset, Data);
      Retries--;
      if (Retries == 0)
      {
          DEBUG((DEBUG_INFO,"Retries reading execed smbus. Restart the board\n",__LINE__));
          ASSERT(0);
          return EFI_TIMEOUT;
      }
  }
  while (EFI_ERROR(Status));
  return Status;
}

EFI_STATUS
SysCtrlReadDword(UINT32 Offset, UINT32 *Data)
{
  EFI_STATUS Status;
  UINT8 Retries = 100;
  do
  {

      Status = SysCtrlReadDwordRetries(Offset, Data);
      Retries--;
      if (Retries == 0)
      {
          DEBUG((DEBUG_INFO,"Retries reading execed smbus. Restart the board\n",__LINE__));
          ASSERT(0);
          return EFI_TIMEOUT;
      }
  }
  while (EFI_ERROR(Status));
  return Status;
}


EFI_STATUS
I2CSysCtrlWrite(
  IN      UINT16 SmBusBarAddr,
  IN      UINT8  SlaveAddress,
  IN      UINT8  OpCode,
  IN      UINT8  Length,
  IN OUT  UINT8 *Buffer
 )
{
#ifdef SYS_CTRL_COM_DISABLED
  DEBUG ((EFI_D_INFO,"ERROR: Syscontroll comunication disabled in compile time\n"));
  ASSERT(FALSE);
#else
  UINT8     Data8;
  UINT8     Index;
  UINTN     StallIndex;
  UINTN     StallTries;

  StallTries = STALL_TIME;

  //
  // Clear Host Data Registers
  //
  IoWrite8(SmBusBarAddr + R_PCH_SMBUS_HD0, 0);
  IoWrite8(SmBusBarAddr + R_PCH_SMBUS_HD1, 0);
  IoWrite8(SmBusBarAddr + R_PCH_SMBUS_HBD, 0);


  for (StallIndex = 0; StallIndex < StallTries; StallIndex++) {
    //
    //  Check for transaction errors
    //
    Data8 = IoRead8 (SmBusBarAddr + R_PCH_SMBUS_HSTS);

    if (Data8 & (B_PCH_SMBUS_DERR | B_PCH_SMBUS_BERR | B_PCH_SMBUS_INTR | B_PCH_SMBUS_HBSY)) {
      //
      // Clear Status Register
      //
      IoWrite8 (SmBusBarAddr + R_PCH_SMBUS_HSTS, B_PCH_SMBUS_HSTS_ALL);
      TimerStall (STALL_PERIOD);
     } else {

       break;
     }
   }

  I2CSoftwareBusRelease(SmBusBarAddr);
  //
  // Check if we timed out waiting to Status bits to clear
  //
  if(StallIndex == StallTries) {
   return EFI_TIMEOUT;
  }

  //
  // Set Auxiliary Control register
  //
  IoWrite8 (SmBusBarAddr + R_PCH_SMBUS_AUXC, 0);

  //
  // Reset the pointer of the internal buffer
  //
  IoRead8 (SmBusBarAddr + R_PCH_SMBUS_HCTL);

  //
  // Set SMBus slave address for the device to send/receive from
  // For I2c Read the value written into bit 0 of the Transmit Slave
  // SMB_Base + 04h[Bit 0 - Direction bit] should be a don’t care.
  //But do to LPT PCH still interpreting this bit, even for I2C reads,
  //we need to ensure it is set to 1=read. Otherwise, all I2C Reads
  //will be blocked, if target address is Ax.
  //
  IoWrite8 (SmBusBarAddr + R_PCH_SMBUS_TSA, (SlaveAddress & ~BIT0)); //Clear bit 0 to be a write address

  //
  // Set Host Data 0 Register
  // Data length
  //
  IoWrite8 (SmBusBarAddr + R_PCH_SMBUS_HD0, Length);


  EnableI2C();

  //
  // Set Control Register
  //
  IoWrite8 (SmBusBarAddr + R_PCH_SMBUS_HCTL, (UINT8) (V_PCH_SMBUS_SMB_CMD_BLOCK));

  //
  //Set the opcode for the Sys control
  // Write = 0x50
  // Read = 0x51 to write the offset to read and then 0x52 to read the data
  //
  IoWrite8 (SmBusBarAddr + R_PCH_SMBUS_HCMD, OpCode);

  //
  //Setd HBD with first data byte
  //
  IoWrite8 (SmBusBarAddr + R_PCH_SMBUS_HBD, *Buffer);


  //
  // Send the rest of the bytes
  //
  for (Index = 0; Index < Length; Index++) {

    //
    //Send the Start bit on first transaction
    //
    if(Index == 0)
    {
      Data8 = IoRead8 (SmBusBarAddr + R_PCH_SMBUS_HCTL);
      IoWrite8 (SmBusBarAddr + R_PCH_SMBUS_HCTL, (UINT8) (Data8 |B_PCH_SMBUS_START));
    }

    //
    //Wait for transaction to complete
    //
    StallIndex = 0;
    do{
      TimerStall (STALL_PERIOD);
      Data8 = IoRead8 (SmBusBarAddr + R_PCH_SMBUS_HSTS);
      StallIndex++;
    }while(!(Data8 & B_PCH_SMBUS_BYTE_DONE_STS) && (StallIndex < 10));

    //
    //Prepare data for next transaction
    //
    Buffer++;
    IoWrite8 (SmBusBarAddr + R_PCH_SMBUS_HBD, *Buffer);

    //
    //Send transaction
    //
    IoWrite8 (SmBusBarAddr + R_PCH_SMBUS_HSTS, B_PCH_SMBUS_BYTE_DONE_STS);

  }

  //
  // Clear Status Registers and exit
  //
  IoWrite8 (SmBusBarAddr + R_PCH_SMBUS_HSTS, B_PCH_SMBUS_HSTS_ALL);


  DisableI2C();
#endif
  return EFI_SUCCESS;
}


EFI_STATUS
SysCtrlWriteWord(UINT32 Offset, UINT16 Data)
{
  EFI_STATUS Status;
  UINT8 Retries;
  UINT8 Buffer[0x6] = {0};

  Status = EFI_SUCCESS;


  Buffer[0] = (UINT8)(Offset >> 0);
  Buffer[1] = (UINT8)(Offset >> 8);
  Buffer[2] = (UINT8)(Offset >> 16);
  Buffer[3] = (UINT8)(Offset >> 24);

  Buffer[4] = (UINT8)(Data >> 0);
  Buffer[5] = (UINT8)(Data >> 8);




  Retries = 100;
  do
  {
      TimerStall(TRANSACTION_DELAY);
      Status = I2CSysCtrlWrite(SMBUS_BASE_ADDRESS,VF_SC_SMBUS_ADDRESS,0x50,0x6,Buffer);
      Retries--;
    if(Retries == 0)
    {
        DEBUG ((DEBUG_INFO,"Retries reading execed smbus. Restart the board\n",__LINE__));
        ASSERT(0);
        return EFI_TIMEOUT;
    }
  }while(EFI_ERROR(Status));

  return Status;
}

///-----------------------
