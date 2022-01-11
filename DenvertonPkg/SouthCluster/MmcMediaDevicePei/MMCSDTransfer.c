/*++
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
--*/
/*++

Copyright (c)  1999 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

--*/

/*++
Module Name:

  MMCSDTransfer.c

Abstract:

  MMC/SD transfer specific functions

--*/

#include <MediaDeviceDriverPei.h>
#include <SaCommonDefinitions.h>

#define MmioAddress( BaseAddr, Register ) \
  ( (UINTN)BaseAddr + \
    (UINTN)(Register) \
  )

#include "PchAccess.h"
typedef struct
{
  UINT8  manufacturer;
  UINT16* String;
  UINT32 rx_cmd_data_dly_1;
  UINT32 rx_cmd_data_dly_2;
  UINT32 tx_cmd_dly;
  UINT32 tx_data_dly_1;
  UINT32 tx_data_dly_2;
}DllValue;

DllValue dllValues []=
{
    {
        CID_MANUFACTUER_ID_DEFAULT,
        L"Default",
        0x00181e27,
        0x00010014,
        0x00000508,
        0x00000c10,
        0x1c2a2a2a,
    },
    {
        CID_MANUFACTUER_ID_SAMSUNG,
        L"Samsung",
        0x00202727,
        0x0001001e,
        0x00000507,
        0x00000c11,
        0x1c2a2a2a,
    },
    {
        CID_MANUFACTUER_ID_SANDISK,
        L"Sandisk",
        0x001a1e27,
        0x0001001c,
        0x00000508,
        0x00000a0f,
        0x1c282828,
    },
    {
        CID_MANUFACTUER_ID_TOSHIBA,
        L"Toshiba",
#ifdef CAR_LAKE_SUPPORT
        0x0014193b,
        0x0001001e,
        0x00000505,
        0x00000a0f,
        0x1c2a2727,
#else //CAR_LAKE_SUPPORT        
        0x00181e27,
        0x00010011,
        0x00000508,
        0x00000b10,
        0x1c2a2a2a,
#endif //CAR_LAKE_SUPPORT
    },
    {0},
};


//
// UINT32
//

#define Mmio32Ptr( BaseAddr, Register ) \
  ( (volatile UINT32 *)MmioAddress( BaseAddr, Register ) )

#define Mmio32( BaseAddr, Register ) \
  *Mmio32Ptr( BaseAddr, Register )

#define DEC_REG 1

EFI_STATUS
MmcSetExtCsd8 (
  IN  PEI_CARD_DATA          *PeiCardData,
  IN  UINT8                  Index,
  IN  UINT8                  Value
  );


EFI_STATUS
CheckCardStatus (
  IN  UINT32    Status
  )
/*++

  Routine Description:
    Check card status, print the debug info and check the error

  Arguments:
    Status - Status got from card status register

  Returns:
    EFI_DEVICE_ERROR
    EFI_SUCCESS
 --*/
{
  CARD_STATUS    *CardStatus;
  CardStatus = (CARD_STATUS*)(&Status);

  if (CardStatus->ADDRESS_OUT_OF_RANGE) {
    DEBUG ((EFI_D_ERROR, "CardStatus: ADDRESS_OUT_OF_RANGE\n"));
  }

  if (CardStatus->ADDRESS_MISALIGN) {
    DEBUG ((EFI_D_ERROR, "CardStatus: ADDRESS_MISALIGN\n"));
  }

  if (CardStatus->BLOCK_LEN_ERROR) {
    DEBUG ((EFI_D_ERROR, "CardStatus: BLOCK_LEN_ERROR\n"));
  }

  if (CardStatus->ERASE_SEQ_ERROR) {
    DEBUG ((EFI_D_ERROR, "CardStatus: ERASE_SEQ_ERROR\n"));
  }

  if (CardStatus->ERASE_PARAM) {
    DEBUG ((EFI_D_ERROR, "CardStatus: ERASE_PARAM\n"));
  }

  if (CardStatus->WP_VIOLATION) {
    DEBUG ((EFI_D_ERROR, "CardStatus: WP_VIOLATION\n"));
  }

  if (CardStatus->CARD_IS_LOCKED) {
    DEBUG ((EFI_D_ERROR, "CardStatus: CARD_IS_LOCKED\n"));
  }

  if (CardStatus->LOCK_UNLOCK_FAILED) {
    DEBUG ((EFI_D_ERROR, "CardStatus: LOCK_UNLOCK_FAILED\n"));
  }

  if (CardStatus->COM_CRC_ERROR) {
    DEBUG ((EFI_D_ERROR, "CardStatus: COM_CRC_ERROR\n"));
  }

  if (CardStatus->ILLEGAL_COMMAND) {
    DEBUG ((EFI_D_ERROR, "CardStatus: ILLEGAL_COMMAND\n"));
  }

  if (CardStatus->CARD_ECC_FAILED) {
    DEBUG ((EFI_D_ERROR, "CardStatus: CARD_ECC_FAILED\n"));
  }

  if (CardStatus->CC_ERROR) {
    DEBUG ((EFI_D_ERROR, "CardStatus: CC_ERROR\n"));
  }

  if (CardStatus->ERROR) {
    DEBUG ((EFI_D_ERROR, "CardStatus: ERROR\n"));
  }

  if (CardStatus->UNDERRUN) {
    DEBUG ((EFI_D_ERROR, "CardStatus: UNDERRUN\n"));
  }

  if (CardStatus->OVERRUN) {
    DEBUG ((EFI_D_ERROR, "CardStatus: OVERRUN\n"));
  }

  if (CardStatus->CID_CSD_OVERWRITE) {
    DEBUG ((EFI_D_ERROR, "CardStatus: CID_CSD_OVERWRITE\n"));
  }

  if (CardStatus->WP_ERASE_SKIP) {
    DEBUG ((EFI_D_ERROR, "CardStatus: WP_ERASE_SKIP\n"));
  }

  if (CardStatus->ERASE_RESET) {
    DEBUG ((EFI_D_ERROR, "CardStatus: ERASE_RESET\n"));
  }

  if (CardStatus->SWITCH_ERROR) {
    DEBUG ((EFI_D_ERROR, "CardStatus: SWITCH_ERROR\n"));
  }

  if ((Status & 0xFCFFA080) != 0) {
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}


EFI_STATUS
SendCommand (
  IN   PEI_SD_CONTROLLER_PPI      *This,
  IN   UINT16                     CommandIndex,
  IN   UINT32                     Argument,
  IN   TRANSFER_TYPE              DataType,
  IN   UINT8                      *Buffer, OPTIONAL
  IN   UINT32                     BufferSize,
  IN   RESPONSE_TYPE              ResponseType,
  IN   UINT32                     TimeOut,
  OUT  UINT32                     *ResponseData
  )
/*++

  Routine Description:
    Send command by using Host IO protocol

  Arguments:
    This           - Pointer to EFI_SD_HOST_IO_PROTOCOL
    CommandIndex   - The command index to set the command index field of command register
    Argument       - Command argument to set the argument field of command register
    DataType       - TRANSFER_TYPE, indicates no data, data in or data out
    Buffer         - Contains the data read from / write to the device
    BufferSize     - The size of the buffer
    ResponseType   - RESPONSE_TYPE
    TimeOut        - Time out value in 1 ms unit
    ResponseData   - Depending on the ResponseType, such as CSD or card status

  Returns:
    EFI_INVALID_PARAMETER
    EFI_UNSUPPORTED
    EFI_DEVICE_ERROR
    EFI_SUCCESS
 --*/
{

  EFI_STATUS    Status;

  Status = This->SendCommand (
                   This,
                   CommandIndex,
                   Argument,
                   DataType,
                   Buffer,
                   BufferSize,
                   ResponseType,
                   TimeOut,
                   ResponseData
                   );
  if (!EFI_ERROR (Status)) {
    if (ResponseType == ResponseR1 || ResponseType == ResponseR1b) {
      ASSERT(ResponseData != NULL);
      if(ResponseData == NULL){
	    DEBUG ((EFI_D_ERROR, "\nERROR!! SendCommand() - ResponseData pointer is null!\n"));
	    return EFI_DEVICE_ERROR;
      }
      Status = CheckCardStatus (*ResponseData);
    }
  } else {
    This->ResetSdHost (This, Reset_DAT_CMD);
  }

  return Status;
}


EFI_STATUS
CalculateCardParameter (
  IN  PEI_CARD_DATA    *PeiCardData
  )
/*++

  Routine Description:
    Get card interested information for CSD rergister

  Arguments:
    CardData  - Pointer to CARD_DATA

  Returns:
    EFI_INVALID_PARAMETER
    EFI_UNSUPPORTED
    EFI_SUCCESS
 --*/
{
  EFI_STATUS     Status;
  UINT32         Frequency;
  UINT32         Multiple;
  UINT32         CSize;
  CSD_SDV2       *CsdSDV2;

  Status = EFI_SUCCESS;

  switch (PeiCardData->CSDRegister.TRAN_SPEED & 0x7) {
    case 0:
      Frequency = 100 * 1000;
      break;

    case 1:
      Frequency = 1 * 1000 * 1000;
      break;

    case 2:
      Frequency = 10 * 1000 * 1000;
      break;

    case 3:
      Frequency = 100 * 1000 * 1000;
      break;

    default:
      DEBUG((EFI_D_ERROR, "CalculateCardParameter: Invalid CSD TRAN_SPEED Frequency: 0x%x\n", PeiCardData->CSDRegister.TRAN_SPEED & 0x7));
      Status = EFI_INVALID_PARAMETER;
      goto Exit;
  }

  switch ((PeiCardData->CSDRegister.TRAN_SPEED >> 3) & 0xF) {
    case 1:
      Multiple = 10;
      break;

    case 2:
      Multiple = 12;
      break;

    case 3:
      Multiple = 13;
      break;

    case 4:
      Multiple = 15;
      break;

    case 5:
      Multiple = 20;
      break;

    case 6:
      if (PeiCardData->CardType == MMCCard) {
        Multiple = 26;
      } else {
        Multiple = 25;
      }
      break;

    case 7:
      Multiple = 30;
      break;

    case 8:
      Multiple = 35;
      break;

    case 9:
      Multiple = 40;
      break;

    case 10:
      Multiple = 45;
      break;

    case 11:
      if (PeiCardData->CardType == MMCCard) {
        Multiple = 52;
      } else {
        Multiple = 50;
      }
      break;

    case 12:
      Multiple = 55;
      break;

    case 13:
      Multiple = 60;
      break;

    case 14:
      Multiple = 70;
      break;

    case 15:
      Multiple = 80;
      break;

    default:
      DEBUG((EFI_D_ERROR, "CalculateCardParameter: Invalid CSD TRAN_SPEED Multiple: 0x%x\n", PeiCardData->CSDRegister.TRAN_SPEED >> 3));
      Status = EFI_INVALID_PARAMETER;
      goto Exit;
  }

  Frequency = Frequency * Multiple / 10;
  PeiCardData->MaxFrequency = Frequency;

  if ((PeiCardData->ExtCSDRegister.CARD_TYPE & BIT2) ||
      (PeiCardData->ExtCSDRegister.CARD_TYPE & BIT3)) {
    PeiCardData->BlockLen = 512;
  } else {
    PeiCardData->BlockLen = 1 << PeiCardData->CSDRegister.READ_BL_LEN;
  }

  if (PeiCardData->CardType == SDMemoryCard2High) {
    ASSERT(PeiCardData->CSDRegister.CSD_STRUCTURE == 1);
    CsdSDV2 = (CSD_SDV2*)&PeiCardData->CSDRegister;
    //
    // The SD Spec 2.0 says (CSize + 1) * 512K is the total size, so block numbber is (CSize + 1) * 1K
    // the K here means 1024 not 1000
    //
    PeiCardData->BlockNumber = DivU64x32 (MultU64x32 (CsdSDV2->C_SIZE + 1, 512 * 1024) , PeiCardData->BlockLen);
  } else {
    //
    // For MMC card > 2G, the block number will be recaculate later
    //
    CSize = PeiCardData->CSDRegister.C_SIZELow2 | (PeiCardData->CSDRegister.C_SIZEHigh10 << 2);
    PeiCardData->BlockNumber = MultU64x32 (LShiftU64 (1, PeiCardData->CSDRegister.C_SIZE_MULT + 2), CSize + 1);
  }

//
//For >= 2G card, BlockLen may be 1024, but the transfer size is still 512 bytes
//
  if (PeiCardData->BlockLen > 512) {
    PeiCardData->BlockNumber = DivU64x32 (MultU64x32 (PeiCardData->BlockNumber, PeiCardData->BlockLen), 512);
    PeiCardData->BlockLen    = 512;
  }

  DEBUG ((
    EFI_D_ERROR,
    "CalculateCardParameter: Card Size: 0x%lx\n", MultU64x32 (PeiCardData->BlockNumber, PeiCardData->BlockLen)
     ));

Exit:
  return Status;
}

EFI_STATUS
MMCCardBusWidthTest (
  IN  PEI_CARD_DATA         *PeiCardData,
  IN  UINT32                Width
  )
/*++

  Routine Description:
    Test the bus width setting for MMC card
    It is used only for verification purpose

  Notes:
    1. Prior to executing the bus testing procedure (CMD19, CMD14), it is
       recommended to set up the clock frequency used for data transfer. This
       way the bus test gives a true result, which might not be the case if
       the bus testing procedure is performed with lower clock frequency than
       the data transfer frequency.
    2. The following commands: bus testing (CMD19, CMD14), lock-unlock (CMD42)
       and set block-length (CMD16) are not allowed once the Device is
       configured to operate in dual data rate mode and shall not be executed
       but regarded as illegal commands.

  Arguments:
    CardData  - Pointer to CARD_DATA
    Width     - 1, 4, 8 bits
  Returns:
    EFI_INVALID_PARAMETER
    EFI_UNSUPPORTED
    EFI_SUCCESS
 --*/
{
  EFI_STATUS                 Status;
  PEI_SD_CONTROLLER_PPI      *SdControllerPpi;
  UINT64                     Data;
  UINT64                     Value;

  ASSERT(PeiCardData != NULL);
  if(PeiCardData == NULL){
    DEBUG ((EFI_D_ERROR, "\nERROR!!! Attempting to use a NULL pointer.\n"));
    return EFI_INVALID_PARAMETER;
  }
  SdControllerPpi = PeiCardData->SdControllerPpi;

  Value = 0;

  switch (Width) {
    case 1:
      Data = 0x80;
      break;

    case 4:
      Data = 0x5A;
      break;

    case 8:
      Data = 0xAA55;
      break;

    default:
      Status = EFI_INVALID_PARAMETER;
      goto Exit;
  }

  CopyMem (PeiCardData->AlignedBuffer, &Data, Width);
  Status  = SendCommand (
              SdControllerPpi,
              BUSTEST_W,
              0,
              OutData,
              PeiCardData->AlignedBuffer,
              Width,
              ResponseR1,
              TIMEOUT_COMMAND,
              (UINT32*)&(PeiCardData->CardStatus)
              );
  if (EFI_ERROR (Status)) {
    DEBUG((EFI_D_ERROR, "MMCCardBusWidthTest: BUSTEST_W -> CardStatus = 0x%x\n", *(UINT32*)&(PeiCardData->CardStatus)));
    goto Exit;
  }

  MicroSecondDelay (10);
  Data = 0;

  Status  = SendCommand (
              SdControllerPpi,
              BUSTEST_R,
              0,
              InData,
              PeiCardData->AlignedBuffer,
              Width,
              ResponseR1,
              TIMEOUT_COMMAND,
              (UINT32*)&(PeiCardData->CardStatus)
              );

  if (EFI_ERROR (Status)) {
    goto Exit;
  }
  CopyMem (&Data, PeiCardData->AlignedBuffer, Width);

  switch (Width) {
    case 1:
      Value = (~(Data ^ 0x80)) & 0xC0;
      break;
    case 4:
      Value = (~(Data ^ 0x5A)) & 0xFF;
      break;
    case 8:
      Value = (~(Data ^ 0xAA55)) & 0xFFFF;
      break;
  }

  if (Value == 0) {
    Status = EFI_SUCCESS;
  } else {
    Status = EFI_UNSUPPORTED;
  }

Exit:
  return Status;
}


EFI_STATUS
SendCmd0 (
  IN  PEI_CARD_DATA *PeiCardData
  )
{
  return SendCommand (
           PeiCardData->SdControllerPpi,
           GO_IDLE_STATE,
           0,
           NoData,
           NULL,
           0,
           ResponseNo,
           TIMEOUT_COMMAND,
           NULL
           );

}

EFI_STATUS
SendCmd1 (
  IN  PEI_CARD_DATA  *PeiCardData,
  IN  UINT32         OcrValue
  )
{
  return SendCommand (
           PeiCardData->SdControllerPpi,
           SEND_OP_COND,
           OcrValue,
           NoData,
           NULL,
           0,
           ResponseR3,
           TIMEOUT_COMMAND,
           (UINT32*)&(PeiCardData->OCRRegister)
           );
}

EFI_STATUS
SendCmd2 (
  IN  PEI_CARD_DATA *PeiCardData
  )
{
  return SendCommand (
           PeiCardData->SdControllerPpi,
           ALL_SEND_CID,
           0,
           NoData,
           NULL,
           0,
           ResponseR2,
           TIMEOUT_COMMAND,
           (UINT32*)&(PeiCardData->CIDRegister)
           );

}

EFI_STATUS
SendCmd3 (
  IN  PEI_CARD_DATA *PeiCardData
  )
{
  return SendCommand (
           PeiCardData->SdControllerPpi,
           SET_RELATIVE_ADDR,
           (PeiCardData->Address << 16),
           NoData,
           NULL,
           0,
           ResponseR1,
           TIMEOUT_COMMAND,
           (UINT32*)&(PeiCardData->CardStatus)
           );

}

EFI_STATUS
SendCmd6 (
  IN  PEI_CARD_DATA     *PeiCardData,
  IN  SWITCH_ARGUMENT   Argument
  )
{
  return  SendCommand (
            PeiCardData->SdControllerPpi,
            SWITCH,
            *(UINT32*)&Argument,
            NoData,
            NULL,
            0,
            ResponseR1b,
            TIMEOUT_COMMAND,
            (UINT32*)&(PeiCardData->CardStatus)
            );


}


EFI_STATUS
SendCmd7 (
  IN  PEI_CARD_DATA *PeiCardData,
  IN  UINT32        Address
  )
{
  return  SendCommand (
            PeiCardData->SdControllerPpi,
            SELECT_DESELECT_CARD,
            (Address << 16),
            NoData,
            NULL,
            0,
            ResponseR1,
            TIMEOUT_COMMAND,
            (UINT32*)&(PeiCardData->CardStatus)
            );

}

EFI_STATUS
SendCmd8 (
  IN  PEI_CARD_DATA *PeiCardData
  )
{
  return SendCommand (
           PeiCardData->SdControllerPpi,
           SEND_EXT_CSD,
           0,
           InData,
           PeiCardData->AlignedBuffer,
           sizeof (EXT_CSD),
           ResponseR1,
           TIMEOUT_DATA,
           (UINT32*)&(PeiCardData->CardStatus)
           );

}

EFI_STATUS
SendCmd9 (
  IN  PEI_CARD_DATA *PeiCardData
  )
{
  return SendCommand (
           PeiCardData->SdControllerPpi,
           SEND_CSD,
           (PeiCardData->Address << 16),
           NoData,
           NULL,
           0,
           ResponseR2,
           TIMEOUT_COMMAND,
           (UINT32*)&(PeiCardData->CSDRegister)
           );
}


EFI_STATUS
SendCmd13 (
  IN  PEI_CARD_DATA *PeiCardData
  )
{
  return  SendCommand (
            PeiCardData->SdControllerPpi,
            SEND_STATUS,
            (PeiCardData->Address << 16),
            NoData,
            NULL,
            0,
            ResponseR1,
            TIMEOUT_COMMAND,
            (UINT32*)&(PeiCardData->CardStatus)
            );


}

EFI_STATUS
MmcDecodeOCR (
  OCR  *OCRReg
  )
{
#if defined (DEC_REG) && (DEC_REG == 1)
  DEBUG((EFI_D_INFO," OCR = 0x%08X\n", *((UINT32 *)OCRReg)));
  DEBUG((EFI_D_INFO, " CARD_NOT_BUSY      = 0x%X\n",OCRReg->Busy));
  DEBUG((EFI_D_INFO, " ACCESS_MODE        = 0x%X\n",OCRReg->AccessMode));
  DEBUG((EFI_D_INFO, " VDD_270_360        = 0x%X\n",OCRReg->V270_V360));
  DEBUG((EFI_D_INFO, " VDD_200_260        = 0x%X\n",OCRReg->V200_V260));
  DEBUG((EFI_D_INFO, " VDD_170_195        = 0x%X\n",OCRReg->V170_V195));
#endif
  return 0;
}



EFI_STATUS
MmcDecodeCID (
  CID * CIDReg
  )
{
#if defined (DEC_REG) && (DEC_REG == 1)
  UINT32 i = 0;
  DEBUG((EFI_D_INFO," CID = 0x%032X\n",CIDReg));
  DEBUG((EFI_D_INFO," MANUFACTURER_ID     = 0x%X\n",CIDReg->MID));
  DEBUG((EFI_D_INFO," CARD_OR_BGA         = 0x%X\n",(CIDReg->OID & 0xFF00)>>6));
  DEBUG((EFI_D_INFO," OEM_APPLICATION_ID  = 0x%X\n",(CIDReg->OID>>8)&0xFF));
  DEBUG((EFI_D_INFO," PRODUCT_NAME        = "));
  for (i=0; i < 6; i++) {
    DEBUG((EFI_D_INFO, "%c",CIDReg->PNM[i]));
  }
  DEBUG((EFI_D_INFO, "\n"));

  DEBUG((EFI_D_INFO," PRODUCT_REVISION    = 0x%X\n",CIDReg->PRV));
  DEBUG((EFI_D_INFO," PRODUCT_SERIAL_NUM  = 0x%X\n",CIDReg->PSN));
  DEBUG((EFI_D_INFO," MANUFACTURE_DATE    = 0x%X\n",CIDReg->MDT));
#endif
  return 0;
}

EFI_STATUS
MmcDecodeCSD (
  CSD * CSDReg
  )
{
#if defined (DEC_REG) && (DEC_REG == 1)

  DEBUG((EFI_D_INFO, "\n==========DECODE MMC CSD REGISTER==================\n"));
  DEBUG((EFI_D_INFO, "csd_struct        : [0x%0x] \n",CSDReg->CSD_STRUCTURE));
  DEBUG((EFI_D_INFO, "specs_ver         : [0x%0x] \n",CSDReg->SPEC_VERS));
  DEBUG((EFI_D_INFO, "reserve2          : [0x%0x] \n",CSDReg->Reserved2));
  DEBUG((EFI_D_INFO, "taac              : [0x%0x] \n",CSDReg->TAAC));
  DEBUG((EFI_D_INFO, "nsac              : [0x%0x] \n",CSDReg->NSAC));
  DEBUG((EFI_D_INFO, "tran_speed        : [0x%0x] \n",CSDReg->TRAN_SPEED));
  DEBUG((EFI_D_INFO, "ccc               : [0x%0x] \n",CSDReg->CCC));
  DEBUG((EFI_D_INFO, "read_bl_len       : [0x%0x] \n",CSDReg->READ_BL_LEN));
  DEBUG((EFI_D_INFO, "read_partial      : [0x%0x] \n",CSDReg->READ_BL_PARTIAL));
  DEBUG((EFI_D_INFO, "write_misalign    : [0x%0x] \n",CSDReg->WRITE_BLK_MISALIGN));
  DEBUG((EFI_D_INFO, "read_misalign     : [0x%0x] \n",CSDReg->READ_BLK_MISALIGN));
  DEBUG((EFI_D_INFO, "dsr_imp           : [0x%0x] \n",CSDReg->DSR_IMP));
  DEBUG((EFI_D_INFO, "reserve1          : [0x%0x] \n",CSDReg->Reserved1));
  DEBUG((EFI_D_INFO, "c_size            : [0x%0x] \n",CSDReg->C_SIZELow2 | CSDReg->C_SIZEHigh10<<2));
  DEBUG((EFI_D_INFO, "vdd_r_curr_min    : [0x%0x] \n",CSDReg->VDD_R_CURR_MIN));
  DEBUG((EFI_D_INFO, "vdd_r_curr_max    : [0x%0x] \n",CSDReg->VDD_R_CURR_MAX));
  DEBUG((EFI_D_INFO, "vdd_w_curr_min    : [0x%0x] \n",CSDReg->VDD_W_CURR_MIN));
  DEBUG((EFI_D_INFO, "vdd_w_curr_max    : [0x%0x] \n",CSDReg->VDD_W_CURR_MAX));
  DEBUG((EFI_D_INFO, "c_size_mult       : [0x%0x] \n",CSDReg->C_SIZE_MULT));
  DEBUG((EFI_D_INFO, "erase_grp_size    : [0x%0x] \n",CSDReg->ERASE_GRP_SIZE));
  DEBUG((EFI_D_INFO, "erase_grp_mult    : [0x%0x] \n",CSDReg->ERASE_GRP_MULT));
  DEBUG((EFI_D_INFO, "wp_grp_size       : [0x%0x] \n",CSDReg->WP_GRP_SIZE));
  DEBUG((EFI_D_INFO, "wp_grp_enable     : [0x%0x] \n",CSDReg->WP_GRP_ENABLE));
  DEBUG((EFI_D_INFO, "default_ecc       : [0x%0x] \n",CSDReg->DEFAULT_ECC));
  DEBUG((EFI_D_INFO, "r2w_factor        : [0x%0x] \n",CSDReg->R2W_FACTOR));
  DEBUG((EFI_D_INFO, "write_bl_len      : [0x%0x] \n",CSDReg->WRITE_BL_LEN));
  DEBUG((EFI_D_INFO, "write_partial     : [0x%0x] \n",CSDReg->WRITE_BL_PARTIAL));
  DEBUG((EFI_D_INFO, "reserve0          : [0x%0x] \n",CSDReg->Reserved0));
  DEBUG((EFI_D_INFO, "content_prot_app  : [0x%0x] \n",CSDReg->CONTENT_PROT_APP));
  DEBUG((EFI_D_INFO, "file_format_grp   : [0x%0x] \n",CSDReg->FILE_FORMAT_GRP));
  DEBUG((EFI_D_INFO, "copy              : [0x%0x] \n",CSDReg->COPY));
  DEBUG((EFI_D_INFO, "perm_write_protect: [0x%0x] \n",CSDReg->PERM_WRITE_PROTECT));
  DEBUG((EFI_D_INFO, "tmp_write_prot    : [0x%0x] \n",CSDReg->TMP_WRITE_PROTECT));
  DEBUG((EFI_D_INFO, "file_format       : [0x%0x] \n",CSDReg->FILE_FORMAT));
  DEBUG((EFI_D_INFO, "ecc               : [0x%0x] \n",CSDReg->ECC));
  DEBUG((EFI_D_INFO, "==================================================\n"));
#endif

  return 0;
}

EFI_STATUS
MmcDecodeExtCSD (
  EXT_CSD  *ExtCSDReg
  )
{
#if defined (DEC_REG) && (DEC_REG == 1)

  DEBUG((EFI_D_INFO, "\n==========DECODE MMC EXT CSD REGISTER==================\n"));

  DEBUG((EFI_D_INFO," SUPPORTED_CMD_SETS        = 0x%X\n",ExtCSDReg->CMD_SET));
  DEBUG((EFI_D_INFO," HPI_FEATURES              = 0x%X\n",ExtCSDReg->HPI_FEATURES));
  DEBUG((EFI_D_INFO," BKOPS_SUPPORT             = 0x%X\n",ExtCSDReg->BKOPS_SUPPORT));
  DEBUG((EFI_D_INFO," BKOPS_STATUS              = 0x%X\n",ExtCSDReg->BKOPS_STATUS));
  DEBUG((EFI_D_INFO," CORRECTLY_PRG_SECTORS_NUM = 0x%X%X%X%X\n", \
                                                 ExtCSDReg->CORRECTLY_PRG_SECTORS_NUM[3], \
                                                 ExtCSDReg->CORRECTLY_PRG_SECTORS_NUM[2], \
                                                 ExtCSDReg->CORRECTLY_PRG_SECTORS_NUM[1], \
                                                 ExtCSDReg->CORRECTLY_PRG_SECTORS_NUM[0]));
  DEBUG((EFI_D_INFO," INI_TIMEOUT_AP            = 0x%X\n",ExtCSDReg->INI_TIMEOUT_AP));
  DEBUG((EFI_D_INFO," PWR_CL_DDR_52_195         = 0x%X\n",ExtCSDReg->PWR_CL_DDR_52_195));
  DEBUG((EFI_D_INFO," PWR_CL_DDR_52_360         = 0x%X\n",ExtCSDReg->PWR_CL_DDR_52_360));
  DEBUG((EFI_D_INFO," MIN_PRF_DDR_W_8_52        = 0x%X\n",ExtCSDReg->MIN_PERF_DDR_W_8_52));
  DEBUG((EFI_D_INFO," MIN_PRF_DDR_R_8_52        = 0x%X\n",ExtCSDReg->MIN_PERF_DDR_R_8_52));
  DEBUG((EFI_D_INFO," TRIM_MULT                 = 0x%X\n",ExtCSDReg->TRIM_MULT));
  DEBUG((EFI_D_INFO," SEC_FEATURE_SUPP          = 0x%X\n",ExtCSDReg->SEC_FEATURE_SUPPORT));
  DEBUG((EFI_D_INFO," SEC_ERASE_MULT            = 0x%X\n",ExtCSDReg->SEC_ERASE_MULT));
  DEBUG((EFI_D_INFO," SEC_TRIM_MULT             = 0x%X\n",ExtCSDReg->SEC_TRIM_MULT));
  DEBUG((EFI_D_INFO," BOOT_INFO                 = 0x%X\n",ExtCSDReg->BOOT_INFO));
  DEBUG((EFI_D_INFO," BOOT_PART_SIZE            = 0x%X\n",ExtCSDReg->BOOT_SIZE_MULTI));
  DEBUG((EFI_D_INFO," ACCESS_SIZE               = 0x%X\n",ExtCSDReg->ACC_SIZE));
  DEBUG((EFI_D_INFO," HI_CAP_ER_GRP_SIZE        = 0x%X\n",ExtCSDReg->HC_ERASE_GRP_SIZE));
  DEBUG((EFI_D_INFO," HI_CAP_ER_TIMEOUT         = 0x%X\n",ExtCSDReg->ERASE_TIMEOUT_MULT));
  DEBUG((EFI_D_INFO," REL_WR_SECTOR_CNT         = 0x%X\n",ExtCSDReg->REL_WR_SEC_C));
  DEBUG((EFI_D_INFO," HI_CAP_WP_GRP_SIZE        = 0x%X\n",ExtCSDReg->HC_WP_GRP_SIZE));
  DEBUG((EFI_D_INFO," SLEEP_CURRENT_VCC         = 0x%X\n",ExtCSDReg->S_C_VCC));
  DEBUG((EFI_D_INFO," SLEEP_CURRENT_VCCQ        = 0x%X\n",ExtCSDReg->S_C_VCCQ));
  DEBUG((EFI_D_INFO," SLP_AWK_TIMEOUT           = 0x%X\n",ExtCSDReg->S_A_TIMEOUT));
  DEBUG((EFI_D_INFO," SECTOR_COUNT              = 0x%X\n",*(UINT32*)((UINT8 *)&ExtCSDReg->SEC_COUNT)));
  DEBUG((EFI_D_INFO," MIN_PERF_W_8_52           = 0x%X\n",ExtCSDReg->MIN_PERF_W_8_52));
  DEBUG((EFI_D_INFO," MIN_PERF_R_8_52           = 0x%X\n",ExtCSDReg->MIN_PERF_R_8_52));
  DEBUG((EFI_D_INFO," MIN_PERF_W_8_26_4_52      = 0x%X\n",ExtCSDReg->MIN_PERF_W_8_26_4_52));
  DEBUG((EFI_D_INFO," MIN_PERF_W_8_26_4_52      = 0x%X\n",ExtCSDReg->MIN_PERF_W_8_26_4_52));
  DEBUG((EFI_D_INFO," MIN_PERF_W_4_26           = 0x%X\n",ExtCSDReg->MIN_PERF_W_4_26));
  DEBUG((EFI_D_INFO," MIN_PERF_R_4_26           = 0x%X\n",ExtCSDReg->MIN_PERF_R_4_26));
  DEBUG((EFI_D_INFO," PWR_CLASS_26_360          = 0x%X\n",ExtCSDReg->PWR_CL_26_360));
  DEBUG((EFI_D_INFO," PWR_CLASS_52_360          = 0x%X\n",ExtCSDReg->PWR_CL_52_360));
  DEBUG((EFI_D_INFO," PWR_CLASS_26_195          = 0x%X\n",ExtCSDReg->PWR_CL_26_195));
  DEBUG((EFI_D_INFO," PWR_CLASS_52_195          = 0x%X\n",ExtCSDReg->PWR_CL_52_195));
  DEBUG((EFI_D_INFO," PARTITION_SWITCH_TIME     = 0x%X\n",ExtCSDReg->PARTITION_SWITCH_TIME));
  DEBUG((EFI_D_INFO," OUT_OF_INTERRUPT_TIME     = 0x%X\n",ExtCSDReg->OUT_OF_INTERRUPT_TIME));
  DEBUG((EFI_D_INFO," CARD_TYPE                 = 0x%X\n",ExtCSDReg->CARD_TYPE));
  DEBUG((EFI_D_INFO," CSD_STRUCTURE             = 0x%X\n",ExtCSDReg->CSD_STRUCTURE));
  DEBUG((EFI_D_INFO," EXT_CSD_REV               = 0x%X\n",ExtCSDReg->EXT_CSD_REV));
  DEBUG((EFI_D_INFO," CMD_SET                   = 0x%X\n",ExtCSDReg->CMD_SET));
  DEBUG((EFI_D_INFO," CMD_SET_REV               = 0x%X\n",ExtCSDReg->CMD_SET_REV));
  DEBUG((EFI_D_INFO," PWR_CLASS                 = 0x%X\n",ExtCSDReg->POWER_CLASS));
  DEBUG((EFI_D_INFO," HI_SPEED_TIMING           = 0x%X\n",ExtCSDReg->HS_TIMING));
  DEBUG((EFI_D_INFO," BUS_WIDTH_MODE            = 0x%X\n",ExtCSDReg->BUS_WIDTH));
  DEBUG((EFI_D_INFO," ERASED_MEM_CONTENT        = 0x%X\n",ExtCSDReg->ERASED_MEM_CONT));
  DEBUG((EFI_D_INFO," PARTITION_CONFIG          = 0x%X\n",ExtCSDReg->PARTITION_CONFIG));
  DEBUG((EFI_D_INFO," BOOT_CONFIG_PROT          = 0x%X\n",ExtCSDReg->BOOT_CONFIG_PROT));
  DEBUG((EFI_D_INFO," BOOT_BUS_WIDTH            = 0x%X\n",ExtCSDReg->BOOT_BUS_WIDTH));
  DEBUG((EFI_D_INFO," HI_DEN_ER_GRP_DEF         = 0x%X\n",ExtCSDReg->ERASE_GROUP_DEF));
  DEBUG((EFI_D_INFO," BOOT_WP                   = 0x%X\n",ExtCSDReg->BOOT_WP));
  DEBUG((EFI_D_INFO," USER_WP                   = 0x%X\n",ExtCSDReg->USER_WP));
  DEBUG((EFI_D_INFO," FW_CONFIG                 = 0x%X\n",ExtCSDReg->FW_CONFIG));
  DEBUG((EFI_D_INFO," RPMB_SIZE_MULT            = 0x%X\n",ExtCSDReg->RPMB_SIZE_MULT));
  DEBUG((EFI_D_INFO," RST_N_FUNCTION            = 0x%X\n",ExtCSDReg->RST_n_FUNCTION));
  DEBUG((EFI_D_INFO," PARTITIONING_SUPP         = 0x%X\n",ExtCSDReg->PARTITIONING_SUPPORT));
  DEBUG((EFI_D_INFO," MAX_ENH_SIZE_MULT         = 0x%02X%02X%02X\n",ExtCSDReg->MAX_ENH_SIZE_MULT[2],ExtCSDReg->MAX_ENH_SIZE_MULT[1],ExtCSDReg->MAX_ENH_SIZE_MULT[0]));
  DEBUG((EFI_D_INFO," PART_ATTRIBUTE            = 0x%X\n",ExtCSDReg->PARTITIONS_ATTRIBUTES));
  DEBUG((EFI_D_INFO," PART_SETTING_COMP         = 0x%X\n",ExtCSDReg->PARTITION_SETTING_COMPLETED));
  DEBUG((EFI_D_INFO," GP_SIZE_MULT              = 0x%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X\n",ExtCSDReg->GP_SIZE_MULT_4[2],ExtCSDReg->GP_SIZE_MULT_4[1],ExtCSDReg->GP_SIZE_MULT_4[0],
                                                  ExtCSDReg->GP_SIZE_MULT_3[2],ExtCSDReg->GP_SIZE_MULT_3[1],ExtCSDReg->GP_SIZE_MULT_3[0],
                                                  ExtCSDReg->GP_SIZE_MULT_2[2],ExtCSDReg->GP_SIZE_MULT_2[1],ExtCSDReg->GP_SIZE_MULT_2[0],
                                                  ExtCSDReg->GP_SIZE_MULT_1[2],ExtCSDReg->GP_SIZE_MULT_1[1],ExtCSDReg->GP_SIZE_MULT_1[0]));
  DEBUG((EFI_D_INFO," ENH_SIZE_MULT             = 0x%02X%02X%02X\n",ExtCSDReg->ENH_SIZE_MULT[2],ExtCSDReg->ENH_SIZE_MULT[1],ExtCSDReg->ENH_SIZE_MULT[0]));
  DEBUG((EFI_D_INFO," ENH_START_ADDR            = 0x%02X%02X%02X%02X\n",ExtCSDReg->ENH_START_ADDR[3],ExtCSDReg->ENH_START_ADDR[2],ExtCSDReg->ENH_START_ADDR[1],ExtCSDReg->ENH_START_ADDR[0]));
  DEBUG((EFI_D_INFO," SEC_BAD_BLK_MGMNT         = 0x%X\n",ExtCSDReg->SEC_BAD_BLOCK_MGMNT));
  DEBUG((EFI_D_INFO, "==================================================\n"));
#endif

  return 0;
}


EFI_STATUS
MmcGetOCR (
  IN   PEI_CARD_DATA              *PeiCardData
  )
{
  EFI_STATUS                 Status;
  UINT32                     TimeOut=1000;
  BOOLEAN                    TryAgain = TRUE;

Try_Again:
  //
  // Go to Idle
  //
  CHK_FUNC_CALL(SendCmd0(PeiCardData), "CMD0", TRUE);

  // Delay to make sure card is in idle
  MicroSecondDelay (100);

  //
  // Get OCR register to check voltage support, first time the OCR is 0
  //
  CHK_FUNC_CALL(SendCmd1(PeiCardData, 0x0), "CMD1", TRUE);

  Status = EFI_SUCCESS;

  do {
    PeiCardData->OCRRegister.AccessMode = 0x02; // sector mode;
    CHK_FUNC_CALL(SendCmd1(PeiCardData, *(UINT32*)&(PeiCardData->OCRRegister)), "CMD1", TRUE);

    MicroSecondDelay (1000);

    TimeOut--;
    if (TimeOut == 0) {
      DEBUG((EFI_D_ERROR, "Card is always in busy state\n"));
      Status = EFI_TIMEOUT;
      goto Exit;
    }

  } while (PeiCardData->OCRRegister.Busy != 1);

  MmcDecodeOCR(&PeiCardData->OCRRegister);

Exit:
  //
  // Try one more time once CMD1 timeout
  //
  if ((EFI_ERROR (Status)) &&(TryAgain == TRUE)) {
    TryAgain = FALSE;
    goto Try_Again;
  }

  return Status;
}


EFI_STATUS
MmcGetCID (
  IN   PEI_CARD_DATA              *PeiCardData
  )
{

  CHK_FUNC_CALL(SendCmd2(PeiCardData), NULL, TRUE);

  MmcDecodeCID(&PeiCardData->CIDRegister);
  return EFI_SUCCESS;
}

EFI_STATUS
MmcGetCSD (
  IN   PEI_CARD_DATA              *PeiCardData
  )
{
  //
  // Get CSD Register
  //
  CHK_FUNC_CALL(SendCmd9(PeiCardData), NULL, TRUE);
  MmcDecodeCSD(&PeiCardData->CSDRegister);
  return EFI_SUCCESS;
}

EFI_STATUS
MmcGetExtCSD (
  IN   PEI_CARD_DATA              *PeiCardData
  )
{
//  UINT8    PartitionNo;

  //
  // Get Ext CSD Register
  //
  CHK_FUNC_CALL(SendCmd8(PeiCardData), NULL, TRUE);

  CopyMem (&(PeiCardData->ExtCSDRegister), PeiCardData->AlignedBuffer, sizeof (EXT_CSD));

//  PartitionNo = 0x2; // set boot partition 2 for other FVs.
//  MmcSetPartition(PeiCardData, PartitionNo);

//  PeiCardData->ExtCSDRegister.PARTITION_CONFIG &= 0xf8;
//  PeiCardData->ExtCSDRegister.PARTITION_CONFIG |= PartitionNo;

#if 0
//hard code some ext_csd for simulation purpose.
  PeiCardData->ExtCSDRegister.CARD_TYPE = 0x017; // Hardcode DDR52 for LR2 card
  *(UINT32*)((UINT8 *)&(PeiCardData->ExtCSDRegister.SEC_COUNT)) = 0x3A3E000;
  PeiCardData->ExtCSDRegister.PARTITIONING_SUPPORT = 0x3;
  PeiCardData->ExtCSDRegister.PARTITION_SETTING_COMPLETED = 0x0;
  PeiCardData->ExtCSDRegister.HPI_FEATURES = 0x1;
  PeiCardData->ExtCSDRegister.BKOPS_SUPPORT = 0x1;
  PeiCardData->ExtCSDRegister.INI_TIMEOUT_AP = 0x1E;
  PeiCardData->ExtCSDRegister.PWR_CL_DDR_52_195 = 0x66;
  PeiCardData->ExtCSDRegister.PWR_CL_DDR_52_360 = 0x33;
  PeiCardData->ExtCSDRegister.MIN_PERF_DDR_W_8_52 = 0xA;
  PeiCardData->ExtCSDRegister.MIN_PERF_DDR_R_8_52 = 0x50;
  PeiCardData->ExtCSDRegister.TRIM_MULT = 0x1;
  PeiCardData->ExtCSDRegister.SEC_FEATURE_SUPPORT = 0x55;
  PeiCardData->ExtCSDRegister.SEC_ERASE_MULT = 0x3;
  PeiCardData->ExtCSDRegister.SEC_TRIM_MULT = 0x14;
  PeiCardData->ExtCSDRegister.BOOT_INFO = 0x7;
  PeiCardData->ExtCSDRegister.BOOT_SIZE_MULTI = 0x20;
  PeiCardData->ExtCSDRegister.ACC_SIZE = 0x8;
  PeiCardData->ExtCSDRegister.HC_ERASE_GRP_SIZE = 0x8;
  PeiCardData->ExtCSDRegister.ERASE_TIMEOUT_MULT = 0x2;
  PeiCardData->ExtCSDRegister.REL_WR_SEC_C =0x10;
  PeiCardData->ExtCSDRegister.HC_WP_GRP_SIZE= 0x2;
  PeiCardData->ExtCSDRegister.S_C_VCC = 0x8;
  PeiCardData->ExtCSDRegister.S_C_VCCQ = 0x9;
  PeiCardData->ExtCSDRegister.S_A_TIMEOUT = 0x11;
  PeiCardData->ExtCSDRegister.MIN_PERF_W_8_52 = 0x14;
  PeiCardData->ExtCSDRegister.MIN_PERF_R_8_52 = 0x64;
  PeiCardData->ExtCSDRegister.MIN_PERF_W_8_26_4_52 = 0xF;
  PeiCardData->ExtCSDRegister.MIN_PERF_W_8_26_4_52 = 0xF;
  PeiCardData->ExtCSDRegister.MIN_PERF_W_4_26 = 0xF;
  PeiCardData->ExtCSDRegister.MIN_PERF_R_4_26 = 0x1E;
  PeiCardData->ExtCSDRegister.PWR_CL_26_360 = 0x11;
  PeiCardData->ExtCSDRegister.PWR_CL_52_360 = 0x11;
  PeiCardData->ExtCSDRegister.PWR_CL_26_195 = 0x66;
  PeiCardData->ExtCSDRegister.PWR_CL_52_195 = 0x66;
  PeiCardData->ExtCSDRegister.PARTITION_SWITCH_TIME = 0x1;
  PeiCardData->ExtCSDRegister.OUT_OF_INTERRUPT_TIME = 0xA;
  PeiCardData->ExtCSDRegister.CARD_TYPE = 0x017;
  PeiCardData->ExtCSDRegister.CSD_STRUCTURE = 0x2;
  PeiCardData->ExtCSDRegister.EXT_CSD_REV = 0x6;
  PeiCardData->ExtCSDRegister.BOOT_BUS_WIDTH = 0x6;
  PeiCardData->ExtCSDRegister.BOOT_WP = 0x10;
  PeiCardData->ExtCSDRegister.RPMB_SIZE_MULT = 0x4;
  PeiCardData->ExtCSDRegister.RST_n_FUNCTION = 0x1;
  PeiCardData->ExtCSDRegister.MAX_ENH_SIZE_MULT[2] = 0x00;
  PeiCardData->ExtCSDRegister.MAX_ENH_SIZE_MULT[1] = 0x07;
  PeiCardData->ExtCSDRegister.MAX_ENH_SIZE_MULT[0] = 0x48;

#endif
//daisywang end

  MmcDecodeExtCSD(&PeiCardData->ExtCSDRegister);

  return EFI_SUCCESS;
}


EFI_STATUS
GetCardType (
  IN   PEI_CARD_DATA              *PeiCardData
  )
/*++

  Routine Description:
    This function can detect these card types
    1. MMC card

  Arguments:
    CardData    - Pointer to CARD_DATA

  Returns:
    EFI_INVALID_PARAMETER
    EFI_UNSUPPORTED
    EFI_SUCCESS
 --*/

{
  EFI_STATUS                 Status = 0;
  PeiCardData->CardType = MMCCard;
  return Status;
}



EFI_STATUS
MMCCardSetBusWidth (
  IN  PEI_CARD_DATA          *PeiCardData,
  IN  UINT8                  BusWidth
  )
/*++

  Routine Description:
    This function set the bus and device width for MMC card

  Arguments:
    CardData    - Pointer to CARD_DATA
    BusWidth    - 1, 4, 8 bits
  Returns:
    EFI_INVALID_PARAMETER
    EFI_UNSUPPORTED
    EFI_SUCCESS
 --*/
{
  EFI_STATUS                 Status;
  PEI_SD_CONTROLLER_PPI      *SdControllerPpi;
  SWITCH_ARGUMENT            SwitchArgument;
  UINT8                      Value;

  SdControllerPpi = PeiCardData->SdControllerPpi;
  Value = 0;
  switch (BusWidth) {
    case 28:    //20 in 28 indicates DDR in 8 bit bus
      Value = 6;
      break;
    case 24:    //20 in 24 indicates DDR in 4 bit bus
      Value = 5;
      break;
    case 8:
      Value = 2;
      break;

    case 4:
      Value = 1;
      break;

    case 1:
      Value = 0;
      break;

    default:
      ASSERT(0);
  }

  if(Value == 5 || Value == 6 ) {
    ZeroMem(&SwitchArgument, sizeof (SWITCH_ARGUMENT));
    SwitchArgument.CmdSet = 0;
    SwitchArgument.Value  = 0x1;
    SwitchArgument.Index  = (UINT32)((UINTN)
                            (&(PeiCardData->ExtCSDRegister.HS_TIMING)) - (UINTN)(&(PeiCardData->ExtCSDRegister)));
    SwitchArgument.Access = WriteByte_Mode;
    Status  = SendCmd6(PeiCardData, SwitchArgument);
    if (!EFI_ERROR (Status)) {
      Status  = SendCmd13(PeiCardData);
    } else {
      DEBUG((EFI_D_ERROR, "SWITCH Fail in HS Timing setting\n"));
    }
  }

  ZeroMem(&SwitchArgument, sizeof (SWITCH_ARGUMENT));
  SwitchArgument.CmdSet = 0;
  SwitchArgument.Value  = Value;
  SwitchArgument.Index  = (UINT32)((UINTN)
                                   (&(PeiCardData->ExtCSDRegister.BUS_WIDTH)) - (UINTN)(&(PeiCardData->ExtCSDRegister)));
  SwitchArgument.Access = WriteByte_Mode;
  Status  = SendCmd6(PeiCardData, SwitchArgument);
  if (!EFI_ERROR (Status)) {
    Status  = SendCmd13(PeiCardData);
    if (EFI_ERROR (Status)) {
      DEBUG((EFI_D_ERROR, "MMCCardSetBusWidth: SWITCH %d bits Fail\n", BusWidth));
      goto Exit;
    } else {
      if ((BusWidth == 24) || (BusWidth == 28)) {
        Status = SdControllerPpi->SetBusWidth (SdControllerPpi, BusWidth - 20);
      } else {
        Status = SdControllerPpi->SetBusWidth (SdControllerPpi, BusWidth);
      }
      if (EFI_ERROR (Status)) {
        goto Exit;
      }
      MicroSecondDelay (10); //synced to byt-cr. bxt power on used (5 * 1000)
    }
  }
  if ((BusWidth == 24) || (BusWidth == 28)) {
    goto Exit;
  } else {
    Status = MMCCardBusWidthTest (PeiCardData, BusWidth);
  }
  if (EFI_ERROR (Status)) {
    DEBUG((EFI_D_ERROR, "MMCCardBusWidthTest %d bit Fail\n", BusWidth));
    goto Exit;
  }

  PeiCardData->CurrentBusWidth = BusWidth;

Exit:
  return Status;
}

EFI_STATUS
MMCSDCardInit (
  IN  PEI_CARD_DATA              *PeiCardData
  )
/*++

  Routine Description:
    MMC/SD card init function

  Arguments:
    CardData    - Pointer to CARD_DATA

  Returns:
    EFI_INVALID_PARAMETER
    EFI_SUCCESS
    EFI_UNSUPPORTED
    EFI_BAD_BUFFER_SIZE
 --*/
{
  EFI_STATUS                 Status;
  PEI_SD_CONTROLLER_PPI      *SdControllerPpi;
  SWITCH_ARGUMENT            SwitchArgument;
  UINT32                     Data;
  UINT8                      PowerValue;
  UINT8                      DoubleSpeed;
  UINTN       Offset;
  DllValue *pDll;


  Status = 0;
  PowerValue = 0;
  DoubleSpeed = 0;

  ASSERT(PeiCardData != NULL);
  if(PeiCardData == NULL){
    DEBUG ((EFI_D_ERROR, "\nERROR!!! Attempting to use a NULL pointer.\n"));
    return EFI_INVALID_PARAMETER;
  }
  SdControllerPpi                  = PeiCardData->SdControllerPpi;
  PeiCardData->CurrentBusWidth = 1;

  CHK_FUNC_CALL(GetCardType(PeiCardData), "GetCardType", TRUE);

  ASSERT (PeiCardData->CardType != UnknownCard);

  //
  //MMC, SD card need host auto stop command support
  //
  SdControllerPpi->EnableAutoStopCmd (SdControllerPpi, TRUE);

  SdControllerPpi->SetupDevice (SdControllerPpi);

  CHK_FUNC_CALL(MmcGetOCR(PeiCardData), "MmcGetOCR", TRUE);

  CHK_FUNC_CALL(MmcGetCID(PeiCardData), "MmcGetCID", TRUE);

  pDll = dllValues;
  while(pDll->manufacturer != 0x0)
  {
    if(pDll->manufacturer == PeiCardData->CIDRegister.MID)break;
    pDll++;
  }

  if(pDll->manufacturer == 0)
  {
    DEBUG ((EFI_D_INFO,"EMMC: Unknown MID\n"));
    pDll = dllValues;
  }
  DEBUG ((EFI_D_INFO,"EMMC: Setting DLL for: %s\n", pDll->String));

  MmioWrite32(mSdBaseAddress + R_SCC_MEM_RX_CMD_DATA_DLL_CNTL1, pDll->rx_cmd_data_dly_1);
  MmioWrite32(mSdBaseAddress + R_SCC_MEM_RX_CMD_DATA_DLL_CNTL2, pDll->rx_cmd_data_dly_2);
  MmioWrite32(mSdBaseAddress + R_SCC_MEM_TX_CMD_DLL_CNTL      , pDll->tx_cmd_dly);
  MmioWrite32(mSdBaseAddress + R_SCC_MEM_TX_DATA_DLL_CNTL1    , pDll->tx_data_dly_1);
  MmioWrite32(mSdBaseAddress + R_SCC_MEM_TX_DATA_DLL_CNTL2    , pDll->tx_data_dly_2);

#ifdef CAR_LAKE_SUPPORT
  MmioWrite32(mSdBaseAddress + R_SCC_MEM_RX_STROBE_DLL_CNTL    , 0x808);
#endif //CAR_LAKE_SUPPORT




  //
  //SET_RELATIVE_ADDR
  //
  if (PeiCardData->CardType == MMCCard) {
    PeiCardData->Address = 1;

    //
    // Set RCA Register
    //
    Status  = SendCmd3(PeiCardData);

    if (EFI_ERROR (Status)) {
      DEBUG((EFI_D_ERROR, "MMCSDCardInit: SET_RELATIVE_ADDR Fail -> Status = 0x%x\n", Status));
      goto Exit;
    }
  }

  //
  // Get CSD Register
  //
  CHK_FUNC_CALL(MmcGetCSD(PeiCardData), "MmcGetCSD", TRUE);

  CHK_FUNC_CALL(CalculateCardParameter (PeiCardData), "CalculateCardParameter", TRUE);



  //
  //Put the card into tran state
  //
  CHK_FUNC_CALL(SendCmd7(PeiCardData, PeiCardData->Address), "CMD7", TRUE);

  CHK_FUNC_CALL(SendCmd13(PeiCardData), "CMD13", TRUE);

  if (PeiCardData->CardType == MMCCard) {
    //
    //Only V4.0 and above supports more than 1 bits and high speed
    //
    if (PeiCardData->CSDRegister.SPEC_VERS >= 4) {
      //
      //After every power up, when host uses a device in which partition(s) are configured, it must set the
      //ERASE_GROUP_DEF bit to high before issuing read, write, erase and write protect commands, because
      //this bit is reset after power up. Otherwise, these may not work correctly and it may leave the stored data
      //in an unknown state.
      //
      Offset = OFFSET_OF (EXT_CSD, ERASE_GROUP_DEF);
      Status = MmcSetExtCsd8 (PeiCardData, (UINT8)Offset,(UINT8)0x1);
      CHK_FUNC_CALL(MmcGetExtCSD(PeiCardData), "MmcGetExtCSD", TRUE);


      //
      // Recaculate the block number for >2G MMC card
      //
      Data  = (PeiCardData->ExtCSDRegister.SEC_COUNT[0]) |
              (PeiCardData->ExtCSDRegister.SEC_COUNT[1] << 8) |
              (PeiCardData->ExtCSDRegister.SEC_COUNT[2] << 16) |
              (PeiCardData->ExtCSDRegister.SEC_COUNT[3] << 24);

      if (Data != 0) {
        PeiCardData->BlockNumber = Data;
      }
      DEBUG((DEBUG_INFO, "CardData->BlockNumbe  %d\n", Data));
      // Check current chipset capability and the plugged-in card
      // whether supports HighSpeed
      //
//daisywang start
      DoubleSpeed = 0;
      DEBUG((EFI_D_ERROR, "CardData->ExtCSDRegister.CARD_TYPE -> %d\n", (UINTN)PeiCardData->ExtCSDRegister.CARD_TYPE));
      if ((PeiCardData->ExtCSDRegister.CARD_TYPE & BIT2)||
          (PeiCardData->ExtCSDRegister.CARD_TYPE & BIT3)) {
        DEBUG((DEBUG_INFO, "Card support DDR\n"));
        DoubleSpeed = 20;   //Add 20 for double speed, decoded in MMCCardSetBusWidth()
      }
//daisywang end



      if (SdControllerPpi->HostCapability.HighSpeedSupport) {
        //
        //Change card timing to high speed interface timing
        //
        ZeroMem(&SwitchArgument, sizeof (SWITCH_ARGUMENT));
        SwitchArgument.CmdSet = 0;
        SwitchArgument.Value  = 1;
        SwitchArgument.Index  = (UINT32)((UINTN)(&(PeiCardData->ExtCSDRegister.HS_TIMING))
                                         - (UINTN)(&(PeiCardData->ExtCSDRegister)));
        SwitchArgument.Access = WriteByte_Mode;
        Status  = SendCmd6(PeiCardData, SwitchArgument);

        if (EFI_ERROR (Status)) {
          DEBUG((EFI_D_ERROR, "SWITCH frequency -> %r\n", Status));
        }

        MicroSecondDelay (5);
        if (!EFI_ERROR (Status)) {
          Status  = SendCmd13(PeiCardData);
          if (!EFI_ERROR (Status)) {
            //
            // Change host clock to support high speed and enable chispet to
            // support speed
            //
//daisywang start
            if (DoubleSpeed != 0) {
              DEBUG((EFI_D_ERROR, "Set to DDR50 mode \n", Status));
              Status = SdControllerPpi->SetHostDdrMode(SdControllerPpi, TRUE);
            } else {
              DEBUG((EFI_D_ERROR, "Set to HS mode \n", Status));
              SdControllerPpi->SetHostSpeedMode (SdControllerPpi, 1);
            }
//daisywang end
            if ((PeiCardData->ExtCSDRegister.CARD_TYPE & BIT1) != 0) {
              Status = SdControllerPpi->SetClockFrequency (SdControllerPpi, FREQUENCY_MMC_PP_HIGH);
            } else if ((PeiCardData->ExtCSDRegister.CARD_TYPE & BIT0) != 0) {
              Status = SdControllerPpi->SetClockFrequency (SdControllerPpi, FREQUENCY_MMC_PP);
            } else {
              Status = EFI_UNSUPPORTED;
            }
            if (EFI_ERROR (Status)) {
              DEBUG((EFI_D_ERROR, "MMCSDCardInit: SetClockFrequency Fail -> Status = 0x%x\n", Status));
              goto Exit;
            }
            //
            // It seems no need to stall after changing bus freqeuncy.
            // It is said that the freqeuncy can be changed at any time. Just appends 8 clocks after command.
            // But SetClock alreay has delay.
            //
          }
        }
        // Card bus width test only works for single data rate mode !!!
      }

    } else {
      DEBUG((EFI_D_ERROR, "MMCSDCardInit: MMC Card version %d only supportes 1 bits at lower transfer speed\n",PeiCardData->CSDRegister.SPEC_VERS));
    }
    //
      // Prefer wide bus width for performance
      //
      //
      // Set to BusWidth bits mode, only version 4.0 or above support more than 1 bits
      //
      if (SdControllerPpi->HostCapability.BusWidth8 == TRUE) {
        Status = MMCCardSetBusWidth (PeiCardData, DoubleSpeed + 8);
        if (EFI_ERROR (Status)) {
          //
          // CE-ATA may support 8 bits and 4 bits, but has no software method for detection
          //
          Status = MMCCardSetBusWidth (PeiCardData, DoubleSpeed + 4);
          if (EFI_ERROR (Status)) {
            goto Exit;
          }
        }
      } else if (SdControllerPpi->HostCapability.BusWidth4 == TRUE) {
        Status = MMCCardSetBusWidth (PeiCardData, DoubleSpeed + 4);
        if (EFI_ERROR (Status)) {
          goto Exit;
        }
      }

      PowerValue = 0;

      if (PeiCardData->CurrentBusWidth == 8) {
        if ((PeiCardData->ExtCSDRegister.CARD_TYPE & BIT1) != 0) {
          PowerValue = PeiCardData->ExtCSDRegister.PWR_CL_52_360;
          PowerValue = PowerValue >> 4;
        } else if ((PeiCardData->ExtCSDRegister.CARD_TYPE & BIT0) != 0) {
          PowerValue = PeiCardData->ExtCSDRegister.PWR_CL_26_360;
          PowerValue = PowerValue >> 4;
        }
      } else if (PeiCardData->CurrentBusWidth == 4) {
        if ((PeiCardData->ExtCSDRegister.CARD_TYPE & BIT1) != 0) {
          PowerValue = PeiCardData->ExtCSDRegister.PWR_CL_52_360;
          PowerValue = PowerValue & 0xF;
        } else if ((PeiCardData->ExtCSDRegister.CARD_TYPE & BIT0) != 0) {
          PowerValue = PeiCardData->ExtCSDRegister.PWR_CL_26_360;
          PowerValue = PowerValue & 0xF;
        }
      }

      if (PowerValue != 0) {
        //
        //Update Power Class
        //
        ZeroMem(&SwitchArgument, sizeof (SWITCH_ARGUMENT));
        SwitchArgument.CmdSet = 0;
        SwitchArgument.Value  = PowerValue;
        SwitchArgument.Index  = (UINT32)((UINTN)(&(PeiCardData->ExtCSDRegister.POWER_CLASS))
                                         - (UINTN)(&(PeiCardData->ExtCSDRegister)));
        SwitchArgument.Access = WriteByte_Mode;
        Status  = SendCmd6(PeiCardData, SwitchArgument);
        if (!EFI_ERROR (Status)) {
          Status  = SendCmd13(PeiCardData);
          if (EFI_ERROR (Status)) {
            DEBUG((EFI_D_ERROR, "MMCSDCardInit: SWITCH Power Class Fail -> Status = 0x%x\n", Status));
          }
          MicroSecondDelay (10);
        }
      }

  }

//
// Set Block Length, to improve compatibility in case of some cards
//
  if (!((PeiCardData->ExtCSDRegister.CARD_TYPE & BIT2) ||
        (PeiCardData->ExtCSDRegister.CARD_TYPE & BIT3))) {
      Status  = SendCommand (
                            SdControllerPpi,
                            SET_BLOCKLEN,
                            512,
                            NoData,
                            NULL,
                            0,
                            ResponseR1,
                            TIMEOUT_COMMAND,
                            (UINT32*)&(PeiCardData->CardStatus)
                            );
      if (EFI_ERROR (Status)) {
          DEBUG((EFI_D_ERROR, "MMCSDCardInit: SET_BLOCKLEN Fail -> Status = 0x%x\n", Status));
          goto Exit;
      }
  }
  SdControllerPpi->SetBlockLength (SdControllerPpi, 512);

Exit:
  return Status;
}

EFI_STATUS
MmcSelect (
  IN  PEI_CARD_DATA          *PeiCardData,
  IN  BOOLEAN                Select
  )
{
  PEI_SD_CONTROLLER_PPI      *SdControllerPpi;
  SdControllerPpi = PeiCardData->SdControllerPpi;

  return SendCommand (
           SdControllerPpi,
           SELECT_DESELECT_CARD,
           Select ? (PeiCardData->Address << 16) : ~(PeiCardData->Address << 16),
           NoData,
           NULL,
           0,
           ResponseR1,
           TIMEOUT_COMMAND,
           (UINT32*)&(PeiCardData->CardStatus)
           );
}


EFI_STATUS
MmcSendSwitch (
  IN PEI_CARD_DATA           *PeiCardData,
  IN SWITCH_ARGUMENT         *SwitchArgument
  )
{
  EFI_STATUS                 Status;
  PEI_SD_CONTROLLER_PPI      * SdControllerPpi;

  SdControllerPpi = PeiCardData->SdControllerPpi;

  Status  = SendCmd6(PeiCardData, *SwitchArgument);

  if (!EFI_ERROR (Status)) {
    Status  = SendCmd13(PeiCardData);

    if (EFI_ERROR (Status)) {
      DEBUG((EFI_D_ERROR, "MmcSendSwitch: SWITCH FAILURE\n"));
    } else {
      MicroSecondDelay (5);
    }
  }

  return Status;
}


EFI_STATUS
MmcUpdateCardStatus (
  IN  PEI_CARD_DATA          *PeiCardData
  )
{
  return SendCmd13(PeiCardData);
}


EFI_STATUS
MmcMoveToTranState (
  IN  PEI_CARD_DATA          *PeiCardData
  )
{
  EFI_STATUS Status;

  Status = EFI_SUCCESS;

  if (PeiCardData->CardStatus.CURRENT_STATE != Tran_STATE) {
    //
    // Put the card into tran state
    //
    Status = MmcSelect (PeiCardData, TRUE);
    DEBUG((EFI_D_INFO, "MmcMoveToTranState: CMD7 -> %r\n", Status));
    Status = MmcUpdateCardStatus (PeiCardData);
  }

//  if ((PLATFORM_ID != VALUE_PUREVP)) {
    if (PeiCardData->CardStatus.CURRENT_STATE != Tran_STATE) {
      DEBUG((EFI_D_ERROR, "MmcMoveToTranState: Unable to put card into tran state\n"));
      return EFI_DEVICE_ERROR;
    }
//  }
  return Status;
}


EFI_STATUS
MmcReadExtCsd (
  IN  PEI_CARD_DATA          *PeiCardData
  )
{
  EFI_STATUS Status;

  Status = MmcMoveToTranState (PeiCardData);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status  = SendCmd8(PeiCardData);
  DEBUG ((EFI_D_INFO, "MmcReadExtCsd: SEND_EXT_CSD -> %r\n", Status));
  if (EFI_ERROR (Status)) {
    return EFI_DEVICE_ERROR;
  }

  CopyMem (&(PeiCardData->ExtCSDRegister), PeiCardData->AlignedBuffer, sizeof (EXT_CSD));

  return Status;
}


EFI_STATUS
MmcSetExtCsd8 (
  IN  PEI_CARD_DATA          *PeiCardData,
  IN  UINT8                  Index,
  IN  UINT8                  Value
  )
{
  EFI_STATUS                 Status;
  SWITCH_ARGUMENT            SwitchArgument;

  Status = MmcMoveToTranState (PeiCardData);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  ZeroMem(&SwitchArgument, sizeof (SWITCH_ARGUMENT));
  SwitchArgument.CmdSet = 0;
  SwitchArgument.Value  = (UINT8) Value;
  SwitchArgument.Index  = (UINT8) Index;
  SwitchArgument.Access = WriteByte_Mode; // SetBits_Mode;
  return MmcSendSwitch (PeiCardData, &SwitchArgument);
}


EFI_STATUS
MmcSetExtCsd24 (
  IN  PEI_CARD_DATA          *PeiCardData,
  IN  UINT8                  Index,
  IN  UINT32                 Value
  )
{
  EFI_STATUS                 Status;
  UINTN                      Loop;

  ASSERT ((Value & 0xff000000ULL) == 0);

  for (Loop = 0; Loop < 3; Loop++) {
    Status = MmcSetExtCsd8 (PeiCardData, Index + (UINT8)Loop, Value & 0xff);
    if (EFI_ERROR (Status)) {
      return Status;
    }
    Value = Value >> 8;
  }

  return Status;
}


UINT32
MmcGetExtCsd8 (
  IN  PEI_CARD_DATA                    *PeiCardData,
  IN  UINTN                            Offset
  )
{
  ASSERT (Offset < sizeof (PeiCardData->ExtCSDRegister));
  return((UINT8*)&PeiCardData->ExtCSDRegister)[Offset];
}


UINT32
MmcGetExtCsd32 (
  IN PEI_CARD_DATA                    *PeiCardData,
  IN UINTN                            Offset
  )
{
  return *(UINT32*) (((UINT8*)&PeiCardData->ExtCSDRegister) + Offset);
}


UINT32
MmcGetExtCsd24 (
  IN PEI_CARD_DATA                    *PeiCardData,
  IN UINTN                            Offset
  )
{
  return MmcGetExtCsd32 (PeiCardData, Offset) & 0xffffff;
}


UINTN
MmcGetCurrentPartitionNum (
  IN  PEI_CARD_DATA              *PeiCardData
  )
{
  return MmcGetExtCsd8 (
           PeiCardData,
           OFFSET_OF (EXT_CSD, PARTITION_CONFIG)
         ) & 0x7;
}


EFI_STATUS
MmcSelectPartitionNum (
  IN  PEI_CARD_DATA          *PeiCardData,
  IN  UINT8                  Partition
  )
{
  EFI_STATUS  Status;
  UINTN       Offset;
  UINT8       *ExtByte;
  UINTN       CurrentPartition;

  if (Partition > 7) {
    return EFI_INVALID_PARAMETER;
  }

  CurrentPartition = MmcGetCurrentPartitionNum (PeiCardData);
  if (Partition == CurrentPartition) {
    return EFI_SUCCESS;
  }

  DEBUG ((EFI_D_INFO,
          "MmcSelectPartitionNum: Switch partition: %d => %d\n",
          CurrentPartition,
          Partition
         ));

  Offset = OFFSET_OF (EXT_CSD, PARTITION_CONFIG);
  Status = MmcSetExtCsd8 (PeiCardData, (UINT8)Offset, Partition);

#if 1
  Status = MmcReadExtCsd (PeiCardData);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  CurrentPartition = MmcGetCurrentPartitionNum (PeiCardData);
  if (Partition != CurrentPartition) {
    DEBUG ((EFI_D_INFO, "MmcSelectPartitionNum: Switch partition failed!\n"));
    return EFI_DEVICE_ERROR;
  }

  ExtByte = NULL;
#else
  if (!EFI_ERROR (Status)) {
    ExtByte = ((UINT8*)&CardData->ExtCSDRegister) + Offset;
    *ExtByte = (UINT8) ((*ExtByte & 0xF7) | Partition);
  }
#endif

  return Status;
}

EFI_STATUS
MmcSelectPartition (
  IN  PEI_MMC_PARTITION_DATA     *Partition
  )
{
  return MmcSelectPartitionNum (
           Partition->PeiCardData,
           (UINT8)PEI_CARD_DATA_PARTITION_NUM (Partition)
           );
}

// daisywang start
// set partition
// value = 0: user partition; 1: boot partition 1; 2:boot partition 2
EFI_STATUS
MmcSetPartition (
  IN  PEI_CARD_DATA          *PeiCardData,
  IN  UINT8                  Value
  )
{
  EFI_STATUS    Status;
  UINTN         Offset;
  UINT32        Data;

  Offset = OFFSET_OF (EXT_CSD, PARTITION_CONFIG);
  Data = MmcGetExtCsd8 (PeiCardData,Offset);
  Data &= 0xf8;
  Data |= Value;

  Status = MmcSetExtCsd8 (PeiCardData, (UINT8)Offset, (UINT8)Data);
  return Status;
}
// daisywang end

