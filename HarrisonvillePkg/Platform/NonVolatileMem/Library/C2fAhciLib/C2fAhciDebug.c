/******************************************************************************
Copy To Flash Sample Code

Copyright 2011-2017 Intel Corporation All Rights Reserved.

The source code, information and material ("Material") contained herein
is owned by Intel Corporation or its suppliers or licensors, and title
to such Material remains with Intel Corporation or its suppliers or licensors.
The Material contains proprietary information of Intel or its suppliers and
licensors. The Material is protected by worldwide copyright laws and treaty
provisions. No part of the Material may be used, copied, reproduced, modified,
published, uploaded, posted, transmitted, distributed or disclosed in any way
without Intel's prior express written permission. No license under any patent,
copyright or other intellectual property rights in the Material is granted to
or conferred upon you, either expressly, by implication, inducement, estoppel
or otherwise. Any license under such intellectual property rights must be
express and approved by Intel in writing.

Unless otherwise agreed by Intel in writing, you may not remove or alter this
notice or any other notice embedded in Materials by Intel or Intel’s suppliers
or licensors in any way.

******************************************************************************/
#include "C2fAhciDebug.h"
#include "C2fAhci.h"
#include <Library/DebugLib.h>

#define PEI_TRACE_DEBUG(param1, param2, ...) DEBUG ((EFI_D_ERROR, ##__VA_ARGS__))
#define PEI_TRACE(text) PEI_TRACE_DEBUG text
#define TRACE_ALWAYS 1
#define PeiServices 1

void printPCISpace()
{
#if 0
  UINT32 * pointer32;
  UINT32 i;
  PEI_TRACE((TRACE_ALWAYS, PeiServices, "--PCI SATA SPACE--\n"));
  for (i=0;i<0xFF;i+=4) {
    if(((i>=0x1c)&&(i<=0x23)) ||
       ((i>=0x24)&&(i<=0x9F)) ||
       ((i>=0xB4)&&(i<=0xBB)) ||
       ((i>=0xC0)&&(i<=0xDF)) ||
       ((i>=0xE4)&&(i<=0xE7))
       )
    {
      continue;
    }
    pointer32 = (UINT32 *)(IOH_SATA_MEM_BASE + i);
    PEI_TRACE((TRACE_ALWAYS, PeiServices, " ADDR[0x%08x] = 0x%08x\n",(UINT32)pointer32,*pointer32));
  }
  PEI_TRACE((TRACE_ALWAYS, PeiServices, "--PCI SATA SPACE FOR PORT1 (in use)--\n"));
  for (i=0x180;i<0x1FF;i+=4) {

    pointer32 = (UINT32 *)(IOH_SATA_MEM_BASE + i);
    PEI_TRACE((TRACE_ALWAYS, PeiServices, " ADDR[0x%08x] = 0x%08x\n",(UINT32)pointer32,*pointer32));
  }
#endif
}

void printDataBlock(UINT8* Address, UINT16 bytes, UINT8 * HeaderString){
  UINT16 i;

  PEI_TRACE((TRACE_ALWAYS, PeiServices, "%a\n",HeaderString));
  PEI_TRACE((TRACE_ALWAYS, PeiServices, "      0  1  2  3  4  5  6  7\n"));
  for(i=0;i<bytes;i++) {
    if(i%8 == 0) {
      PEI_TRACE((TRACE_ALWAYS, PeiServices, "%02d: ", i));
    }
    PEI_TRACE((TRACE_ALWAYS, PeiServices, " %02x",Address[i]));
    if(i%8 == 7) {
      PEI_TRACE((TRACE_ALWAYS, PeiServices, "\n"));
    }
  }
}


void printReceiveFis()
{
  struct ReceivedFIS * receivedFis = RECEIVED_FIS_AREA;

  PEI_TRACE((TRACE_ALWAYS, PeiServices, "--RECEIVED FIS--\n"));
  printDataBlock(receivedFis->DSFIS, 0x20, "  DSFIS--------------");
  PEI_TRACE((TRACE_ALWAYS, PeiServices, "\n"));
  printDataBlock(receivedFis->PSFIS, 0x20, "  PSFIS--------------");
  PEI_TRACE((TRACE_ALWAYS, PeiServices, "\n"));
  printDataBlock(receivedFis->RFIS, 0x18, "  RFIS--------------");
  PEI_TRACE((TRACE_ALWAYS, PeiServices, "\n"));
  printDataBlock(receivedFis->SDBFIS, 0x8, "  SDBFIS--------------");
  PEI_TRACE((TRACE_ALWAYS, PeiServices, "\n"));
  printDataBlock(receivedFis->UFIS, 0x8, "  UFIS--------------");
  PEI_TRACE((TRACE_ALWAYS, PeiServices, "\n"));
}

void printCmdTable()
{
  struct CommandTable * commandTable = COMMAND_LIST;
  struct CFIS * cfis = NULL;
  struct PRDT * prdt = NULL;
  struct ACMD * acmd = NULL;
  UINT32 i;
  UINT32 command;

  PEI_TRACE((TRACE_ALWAYS, PeiServices, "------STRUCTURE OF EACH COMMAND------\n"));

  for(command=0;command<COMMAND_SLOTS;command++) {
    cfis = &commandTable[command].cfis;
    prdt = &commandTable[command].prdt[0];
    acmd = &commandTable[command].acmd;

    PEI_TRACE((TRACE_ALWAYS, PeiServices, "--COMMAND TABLE[%d]:%x--\n",command,&commandTable[command]));

    PEI_TRACE((TRACE_ALWAYS, PeiServices, "CFIS:\n"));
    PEI_TRACE((TRACE_ALWAYS, PeiServices, " FISType = 0x%02x, ",(UINT8)cfis->FISType));
    PEI_TRACE((TRACE_ALWAYS, PeiServices, " MiscFlags = 0x%02x, ",(UINT8)cfis->MiscFlags));
    PEI_TRACE((TRACE_ALWAYS, PeiServices, " Command = 0x%02x, ",(UINT8)cfis->Command));
    PEI_TRACE((TRACE_ALWAYS, PeiServices, " Features = 0x%02x, ",(UINT8)cfis->Features));
    PEI_TRACE((TRACE_ALWAYS, PeiServices, " LBALow = 0x%02x, ",(UINT8)cfis->LBALow));
    PEI_TRACE((TRACE_ALWAYS, PeiServices, " LBAMid = 0x%02x\n",(UINT8)cfis->LBAMid));
    PEI_TRACE((TRACE_ALWAYS, PeiServices, " LBAHigh = 0x%02x, ",(UINT8)cfis->LBAHigh));
    PEI_TRACE((TRACE_ALWAYS, PeiServices, " Device = 0x%02x, ",(UINT8)cfis->Device));
    PEI_TRACE((TRACE_ALWAYS, PeiServices, " LBALowExp = 0x%02x, ",(UINT8)cfis->LBALowExp));
    PEI_TRACE((TRACE_ALWAYS, PeiServices, " LBAMidExp = 0x%02x, ",(UINT8)cfis->LBAMidExp));
    PEI_TRACE((TRACE_ALWAYS, PeiServices, " LBAHighExp = 0x%02x\n",(UINT8)cfis->LBAHighExp));
    PEI_TRACE((TRACE_ALWAYS, PeiServices, " FeaturesExp = 0x%02x, ",(UINT8)cfis->FeaturesExp));
    PEI_TRACE((TRACE_ALWAYS, PeiServices, " SectorCount = 0x%02x, ",(UINT8)cfis->SectorCount));
    PEI_TRACE((TRACE_ALWAYS, PeiServices, " SectorCountExp = 0x%02x, ",(UINT8)cfis->SectorCountExp));
    PEI_TRACE((TRACE_ALWAYS, PeiServices, " Control = 0x%02x\n",(UINT8)cfis->Control));
    printDataBlock(cfis->Padding, 48, "  ---Padding---");
    PEI_TRACE((TRACE_ALWAYS, PeiServices, "\nACMD:\n"));
    printDataBlock(acmd->pad, 0x10, "  ---acmd->pad---");
    PEI_TRACE((TRACE_ALWAYS, PeiServices, "\nPAD:\n"));
    printDataBlock(commandTable[command].pad, 0x30, "  ---commandTable[].pad---");
    PEI_TRACE((TRACE_ALWAYS, PeiServices, "\n"));
    for(i=0;i<8;i++) {
      prdt = &commandTable->prdt[i];
      PEI_TRACE((TRACE_ALWAYS, PeiServices, "PRDT[%d]:\n",i));
      PEI_TRACE((TRACE_ALWAYS, PeiServices, " DBA = 0x%08x, ",(UINT32)prdt->DBA));
      PEI_TRACE((TRACE_ALWAYS, PeiServices, " DBAU = 0x%08x, ",(UINT32)prdt->DBAU));
      PEI_TRACE((TRACE_ALWAYS, PeiServices, " DBC = 0x%08x\n",(UINT32)prdt->DBC));
    }
  }
}


void printCmdHeaderList()
{
  struct CommandHeader *commandHeader = COMMAND_HEADER_LIST;
  UINT32 i;

  PEI_TRACE((TRACE_ALWAYS, PeiServices, "--COMMAND HEADER LIST--\n"));
  for(i=0;i<COMMAND_SLOTS;i++) {
    PEI_TRACE((TRACE_ALWAYS, PeiServices, "--Command[%d]-Addr[0x%llx]\n",i,&commandHeader[i]));
    PEI_TRACE((TRACE_ALWAYS, PeiServices, "Misc = 0x%04x, ",(UINT16)commandHeader[i].Misc));
    PEI_TRACE((TRACE_ALWAYS, PeiServices, "PRDTL = 0x%04x, ",(UINT16)commandHeader[i].PRDTL));
    PEI_TRACE((TRACE_ALWAYS, PeiServices, "PRDBC = 0x%08x\n",(UINT32)commandHeader[i].PRDBC));
    PEI_TRACE((TRACE_ALWAYS, PeiServices, "CTBA = 0x%08x, ",(UINT32)commandHeader[i].CTBA));
    PEI_TRACE((TRACE_ALWAYS, PeiServices, "CTBAU = 0x%08x\n",(UINT32)commandHeader[i].CTBAU));
    printDataBlock((UINT8*)(commandHeader[i].pad), 16, "  ---commandHeader[].pad---");
    PEI_TRACE((TRACE_ALWAYS, PeiServices, "\n"));
  }
}

void printScratchArea()
{
#ifdef SERIAL_DBG_MSG
  UINT16 i;
  UINT16 *scratch = (UINT16 *)SCRATCH_PAD_AREA;
  PEI_TRACE((TRACE_ALWAYS, PeiServices, "--SCRATCH AREA--\n"));
  for(i=0;i<256;i+=1) {
    PEI_TRACE((TRACE_ALWAYS, PeiServices, "W[%d] = 0x%04x, ",i,scratch[i]));
    if ((i%6==0) && (i!=0)) {
      PEI_TRACE((TRACE_ALWAYS, PeiServices, "\n"));
    }
  }
  PEI_TRACE((TRACE_ALWAYS, PeiServices, "\n"));
#endif
}

void printAllVariables( UINT8 * string2print)
{
  PEI_TRACE((TRACE_ALWAYS, 1, "%s\n",string2print));
  //printPCISpace(PeiServices);
  printCmdHeaderList();
  printCmdTable();
  printReceiveFis();
  //printScratchArea(PeiServices);
}



