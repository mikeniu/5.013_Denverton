#ifndef _MSYSPROGSTRUCTURES_H_
#define _MSYSPROGSTRUCTURES_H_

#ifdef MSP_DXE_EXECUTION
  #include "DxeProjectDefinitionsGenerated.h"
#else
  #include "DataTypes.h"
  #include "MmrcProjectDefinitionsGenerated.h"
#endif


#if USE_64_BIT_VARIABLES
#define UINTX UINT64
#else
#define UINTX UINT32
#endif

typedef UINT32            MSP_STATUS;
#define MSP_SUCCESS       0
#define MSP_FAILURE       0xFFFFFFFF

#ifndef VOLATILE
#define VOLATILE    volatile
#endif


#define MSP_NO_PRINT          0xFFFF

#define ICH_HPET_BASE_ADDRESS                   0xFED00000          // Program by McEnableHpet and restore by McDisableHPET
#define PCI_BUS_NUMBER_ICH                      0x00
//
// LPC Bridge Registers(D31:F0)
//
#define PCI_DEVICE_NUMBER_ICH_LPC               31
#define PCI_FUNCTION_NUMBER_ICH_LPC             0
#define R_ICH_LPC_RCBA                          0xF0
#define ICH_RCRB_HPTC                           0x3404            // High Performance Timer Configuration
#define ICH_RCRB_HPTC_AS                        (BIT1+BIT0)       // Address selection
#define ICH_RCRB_HPTC_AE                        (BIT7)            // Address enable

#define HPET_1US          14318
#define HPET_1MS          0x37EF

#define MILLI_DEL             0
#define MICRO_DEL             1
#define NANO_DEL              2
//
// Divide Configuration Register (DCR)
//
typedef union {
  struct {
    UINT32  DivideValue1:2;  ///< Low 2 bits of the divide value.
    UINT32  Reserved0:1;     ///< Always 0.
    UINT32  DivideValue2:1;  ///< Highest 1 bit of the divide value.
    UINT32  Reserved1:28;    ///< Reserved.
  } Bits;
  UINT32    Data32;
} LOCAL_APIC_DCR;

// 
// Support for Broadcasting during phyinit's when looping on modules.
//
#define BROADCAST_SUP         0
//
// Defines for register initialization table.
//
#define C_(c)      (c)
#define _I(i)      (i)
#define CI(c,i)    (c), (i)
#define RB(r,b)    (((r & 0x0f) << 4) + ((b & 0x0f) << 0))
#define BB(bt,be)  (((bt & 0x0f) << 4) + ((be & 0x0f) << 0))
#define BB2(bt,be) (bt & 0xff), (be & 0xff)
#define AP(ac,pf)  (((ac & 0x07) << 5) + ((pf & 0x1f) << 0))
#define MASK1(m)   CONV1(m)
#define MASK2(m)   CONV2(m)
#define MASK3(m)   CONV3(m)
#define MASK4(m)   CONV4(m)
#define MASK5(m)   CONV5(m)
#define MASK6(m)   CONV6(m)
#define MASK7(m)   CONV7(m)
#define MASK8(m)   CONV8(m)
#define VAL1(m)    CONV1(m)
#define VAL2(m)    CONV2(m)
#define VAL3(m)    CONV3(m)
#define VAL4(m)    CONV4(m)
#define VAL5(m)    CONV5(m)
#define VAL6(m)    CONV6(m)
#define VAL7(m)    CONV7(m)
#define VAL8(m)    CONV8(m)

//
// Bit definitions
//
#ifndef BIT0
#define BIT0                  0x00000001
#define BIT1                  0x00000002
#define BIT2                  0x00000004
#define BIT3                  0x00000008
#define BIT4                  0x00000010
#define BIT5                  0x00000020
#define BIT6                  0x00000040
#define BIT7                  0x00000080
#define BIT8                  0x00000100
#define BIT9                  0x00000200
#define BIT10                 0x00000400
#define BIT11                 0x00000800
#define BIT12                 0x00001000
#define BIT13                 0x00002000
#define BIT14                 0x00004000
#define BIT15                 0x00008000
#define BIT16                 0x00010000
#define BIT17                 0x00020000
#define BIT18                 0x00040000
#define BIT19                 0x00080000
#define BIT20                 0x00100000
#define BIT21                 0x00200000
#define BIT22                 0x00400000
#define BIT23                 0x00800000
#define BIT24                 0x01000000
#define BIT25                 0x02000000
#define BIT26                 0x04000000
#define BIT27                 0x08000000
#define BIT28                 0x10000000
#define BIT29                 0x20000000
#define BIT30                 0x40000000
#define BIT31                 0x80000000
#endif

#define SKIP_ACTION           0
#define TAKE_ACTION           1
#define PFCT_INDEPENDENT      0
// 
// Macros for converting Little-Endian to Big-Endian.
//
#define CONV1(x)              (x&0xff)
#define CONV2(x)              ((x>>0)&0xff), ((x>>8)&0xff)
#define CONV3(x)              ((x>>0)&0xff), ((x>>8)&0xff), ((x>>16)&0xff)
#define CONV4(x)              ((x>>0)&0xff), ((x>>8)&0xff), ((x>>16)&0xff), ((x>>24)& 0xff)
#define CONV5(x)              ((x>>0)& 0xff), ((x>>8)& 0xff), ((x>>16)& 0xff), ((x>>24)& 0xff), ((x>>32)& 0xff)
#define CONV6(x)              ((x>>0)& 0xff), ((x>>8)& 0xff), ((x>>16)& 0xff), ((x>>24)& 0xff), ((x>>32)& 0xff), ((x>>40)& 0xff)
#define CONV7(x)              ((x>>0)& 0xff), ((x>>8)& 0xff), ((x>>16)& 0xff), ((x>>24)& 0xff), ((x>>32)& 0xff), ((x>>40)& 0xff), ((x>>48)& 0xff)
#define CONV8(x)              ((x>>0)& 0xff), ((x>>8)& 0xff), ((x>>16)& 0xff), ((x>>24)& 0xff), ((x>>32)& 0xff), ((x>>40)& 0xff), ((x>>48)& 0xff), ((x>>56)& 0xff)

// 
// Macros for MASK/VAL which are used in the projectlibraries.c declarations.
//
#define MASK(x)               CONV4(x)
#define VAL(x)                CONV4(x)

#define ASSIGNDONE            (0xff)
#define MSP_NA                (0xFF)

typedef enum  {
  Pfct =  0,
  PfctT,
  PfctC,
  PfctCT,
  PfctF,
  PfctFT,
  PfctFC,
  PfctFCT,
  PfctP,
  PfctPT,
  PfctPC,
  PfctPCT,
  PfctPF,
  PfctPFT,
  PfctPFC,
  PfctPFCT,
  MaxPfct,
} ENUM_PCFT_TYPE;


//
// Byte Enables supported in the CAPSULEINIT Sequences.
//
typedef enum {
  _00000000,
  _00000001,
  _00000010,
  _00000011,
  _00000100,
  _00000101,
  _00000110,
  _00000111,
  _00001000,
  _00001001,
  _00001010,
  _00001011,
  _00001100,
  _00001101,
  _00001110,
  _00001111,
  _00010000,
  _00010001,
  _00010010,
  _00010011,
  _00010100,
  _00010101,
  _00010110,
  _00010111,
  _00011000,
  _00011001,
  _00011010,
  _00011011,
  _00011100,
  _00011101,
  _00011110,
  _00011111,
  _00100000,
  _00100001,
  _00100010,
  _00100011,
  _00100100,
  _00100101,
  _00100110,
  _00100111,
  _00101000,
  _00101001,
  _00101010,
  _00101011,
  _00101100,
  _00101101,
  _00101110,
  _00101111,
  _00110000,
  _00110001,
  _00110010,
  _00110011,
  _00110100,
  _00110101,
  _00110110,
  _00110111,
  _00111000,
  _00111001,
  _00111010,
  _00111011,
  _00111100,
  _00111101,
  _00111110,
  _00111111,
  _01000000,
  _01000001,
  _01000010,
  _01000011,
  _01000100,
  _01000101,
  _01000110,
  _01000111,
  _01001000,
  _01001001,
  _01001010,
  _01001011,
  _01001100,
  _01001101,
  _01001110,
  _01001111,
  _01010000,
  _01010001,
  _01010010,
  _01010011,
  _01010100,
  _01010101,
  _01010110,
  _01010111,
  _01011000,
  _01011001,
  _01011010,
  _01011011,
  _01011100,
  _01011101,
  _01011110,
  _01011111,
  _01100000,
  _01100001,
  _01100010,
  _01100011,
  _01100100,
  _01100101,
  _01100110,
  _01100111,
  _01101000,
  _01101001,
  _01101010,
  _01101011,
  _01101100,
  _01101101,
  _01101110,
  _01101111,
  _01110000,
  _01110001,
  _01110010,
  _01110011,
  _01110100,
  _01110101,
  _01110110,
  _01110111,
  _01111000,
  _01111001,
  _01111010,
  _01111011,
  _01111100,
  _01111101,
  _01111110,
  _01111111,
  _10000000,
  _10000001,
  _10000010,
  _10000011,
  _10000100,
  _10000101,
  _10000110,
  _10000111,
  _10001000,
  _10001001,
  _10001010,
  _10001011,
  _10001100,
  _10001101,
  _10001110,
  _10001111,
  _10010000,
  _10010001,
  _10010010,
  _10010011,
  _10010100,
  _10010101,
  _10010110,
  _10010111,
  _10011000,
  _10011001,
  _10011010,
  _10011011,
  _10011100,
  _10011101,
  _10011110,
  _10011111,
  _10100000,
  _10100001,
  _10100010,
  _10100011,
  _10100100,
  _10100101,
  _10100110,
  _10100111,
  _10101000,
  _10101001,
  _10101010,
  _10101011,
  _10101100,
  _10101101,
  _10101110,
  _10101111,
  _10110000,
  _10110001,
  _10110010,
  _10110011,
  _10110100,
  _10110101,
  _10110110,
  _10110111,
  _10111000,
  _10111001,
  _10111010,
  _10111011,
  _10111100,
  _10111101,
  _10111110,
  _10111111,
  _11000000,
  _11000001,
  _11000010,
  _11000011,
  _11000100,
  _11000101,
  _11000110,
  _11000111,
  _11001000,
  _11001001,
  _11001010,
  _11001011,
  _11001100,
  _11001101,
  _11001110,
  _11001111,
  _11010000,
  _11010001,
  _11010010,
  _11010011,
  _11010100,
  _11010101,
  _11010110,
  _11010111,
  _11011000,
  _11011001,
  _11011010,
  _11011011,
  _11011100,
  _11011101,
  _11011110,
  _11011111,
  _11100000,
  _11100001,
  _11100010,
  _11100011,
  _11100100,
  _11100101,
  _11100110,
  _11100111,
  _11101000,
  _11101001,
  _11101010,
  _11101011,
  _11101100,
  _11101101,
  _11101110,
  _11101111,
  _11110000,
  _11110001,
  _11110010,
  _11110011,
  _11110100,
  _11110101,
  _11110110,
  _11110111,
  _11111000,
  _11111001,
  _11111010,
  _11111011,
  _11111100,
  _11111101,
  _11111110,
  _11111111,
}  BYTE_ENABLES;

//
// PFCT Variations supported in the CAPSULEINIT Sequences.
//
typedef enum {
  _____ = 0,
  ____T,
  ___C_,
  ___CT, 
  __F__,
  __F_T,
  __FC_,
  __FCT,
  _P___,
  _P__T,
  _P_C_,
  _P_CT,
  _PF__,
  _PF_T,
  _PFC_,
  _PFCT,
  D____,
  D___T,
  D__C_,
  D__CT, 
  D_F__,
  D_F_T,
  D_FC_,
  D_FCT,
  DP___,
  DP__T,
  DP_C_,
  DP_CT,
  DPF__,
  DPF_T,
  DPFC_,
  DPFCT,
} DPFCT_TYPES;


/**
  Data structure to convert GSM_LT level to a blueprint.
**/
typedef struct {
  UINT16 PMask;
  UINT16 FMask;
  UINT16 CMask;
  UINT16 TMask;
  UINT8  IoLevel;
  UINT8  Blueprint;
} PFCT_AND_IO_LEVEL_TO_BLUEPRINT;

typedef enum {
  ChNone  = 0xFE,
  ChAll   = 0xFF,
  ChAllP  = 0xE0,
  MspNone = 0xFE,
  MspAll  = 0xFF,
  MspAllP = 0xE0
} CHANNEL_OR_PORT_SELECTION;

//
// Structures used to display registers.
//
typedef struct {
  char  *RegisterName;
  char  *RegisterDescription;
  UINT32 Offset;
  UINT8  NumFields;
  UINTX  Mask;
  UINTX  DefaultValue;
} RegisterInformation;

typedef struct {
  UINT8  StartBit;
  UINT8  EndBit;
  UINTX  DefaultFieldValue;
  char  *AccessType;
  char  *FieldName;
  char  *Description;
} RegisterField;

typedef struct {
  RegisterInformation *RegInfo;
  RegisterField       *RegFields;
} Register;

typedef struct {
  UINT32  Pfct;
  UINT8   LengthVariation;
} PFCT_VARIATIONS;

typedef struct {
  UINT32  Offset;           ///> Register Offset relative to the base address.
  UINTX   Mask;             ///> Starting bit within the register.
  UINT8   ShiftBit;         ///> Number of bits to shift to get to the register field in question
} REGISTER_ACCESS;

typedef struct {
  UINT8  TotalInstances;
  UINT8  Instances;
  UINT8  InstanceStartIndex;
  UINT8  ReadOperation;
  UINT8  WriteOperation;
  UINT8  AccessMethod;
  UINT16  AccessBase;
  UINT8  SimulationSupported;
  UINT32 StringIndex;
} INSTANCE_PORT_MAP;

typedef struct {
  UINT16 Port;
  UINT16 Offset;
} INSTANCE_PORT_OFFSET;

typedef struct {
  UINT16  StrIndex;
  UINT32  FncOffset;
} SUBTASK_INDEX_OFFSET;

/**
  Structure for the PHY init registers. This contains 
  information on how to find registers for a given bytelane, 
  module, rank, etc. Also indicates whether we should write a 
  register to all enabled channels or just to a specified 
  channel. 
**/
typedef struct {
  UINT8  RankLoop         :1;   ///> Loop on rank.
  UINT8  StrobeLoop       :1;   ///> Loop on strobe.
  UINT8  Offset16         :1;   ///> When set, the offset field is 2-bytes long, otherwise 1-byte long.
  UINT8  CondExec         :1;   ///> When set, Conditional Execution.
  UINT8  MaskPresent      :1;   ///> When set, mask is present in the Assignment.
  UINT8  NeedSecondByte   :1;   ///> When set, REGISTER_LOCATION_ATTRIBUTES2 is present.
  UINT8  UseBB2           :1;   ///> If the boxtype is above 15 or the value being written is greater than 32 bits 
                                ///> then we need to use the BB2 macro to allow 8 bits for boxtype and 8 for byte enables.
  UINT8  Done             :1;   ///> When set, this is the final entry for this task.
} REGISTER_LOCATION_ATTRIBUTES;

/**
  Optional Byte 2 of register attributes. 
  Only used if we need bits in this byte set to 1.
**/
typedef struct {
  UINT8  StringPresent    :1;   ///> There is a string that needs to be output associated with this function entry.
  UINT8  InstanceDependent:1;   ///> When set, only specific instance will be written to.
  UINT8  ChannelDependent :1;   ///> When set, only specific channel will be written.
  UINT8  Reserved         :5;   ///> Reserved
} REGISTER_LOCATION_ATTRIBUTES2;

typedef struct {
  BOOLEAN   ConvertMchBarToP2SBAR;
  BOOLEAN   ConvertAccessToAssembly;
} EMULATION_PARAMETERS;

typedef struct {
  VOID      *InputData;
  //
  // MspExecuteTask variables
  //
  UINT16    CurrentPlatform[MAX_CHANNELS];
  UINT16    CurrentFrequency[MAX_CHANNELS];
  UINT16    CurrentConfiguration[MAX_CHANNELS];
  UINT16    CurrentTechnology[MAX_CHANNELS];

  UINT8     DebugMsgLevel;
  UINT8     DebugMsgLevelRequested;
  BOOLEAN   ExecuteThisRoutineInParallel;
  BOOLEAN   Enabled[MAX_CHANNELS];
  BOOLEAN   ExecuteOnThisChannel[MAX_CHANNELS];
  UINT8     CurrentBlueprint;

  UINT32    EcBase;

  UINT32    HpetHptcSave;
  // 
  // GROWING LIST OF INPUT PARAMS
  //
  UINTX     DynamicVars[MAX_CHANNELS][DYNVAR_MAX];
  //
  // MSP Data Lock, after this MspData variables will 
  // not be writable anymore thru MspInit function
  //
  BOOLEAN   Lock;
  //
  // Overrides 
  //
  EMULATION_PARAMETERS  EmulationOverrides;
} MSP_DATA;

/** 
  The structure definition for the floorplan which is given a 
  channel and strobelane, to provide a physical channel and 
  strobelane. 
**/
typedef struct {
  UINT8 Channel;
  UINT8 Strobelane;
} FLOORPLAN;

#ifdef MSP_DXE_EXECUTION
typedef struct {
  UINT8   PostCode;
  UINT8   BootMode;
  MSP_STATUS  (*Function) (MSP_DATA *, UINT8, UINT16, UINT16, UINT8);
  UINT16  CapsuleStartIndex;
  UINT8   StringIndex;
  UINT8   Channel;
  UINT16  CondExec;
  UINT16  PList;
  UINT16  FList;
  UINT16  CList;
  UINT16  TList;
} TASK_DESCRIPTOR;
#endif

#endif // _MSYSPROGSTRUCTURES_H_
