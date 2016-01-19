#ifndef _MIPS_EMU_H_
#define _MIPS_EMU_H_

#include "il.h"

typedef enum
{
    REG_ZR = 0,
    REG_AT = 1,
    REG_V0 = 2,
    REG_V1 = 3,
    REG_A0 = 4,
    REG_A1 = 5,
    REG_A2 = 6,
    REG_A3 = 7,
    REG_T0 = 8,
    REG_T1 = 9,
    REG_T2 = 10,
    REG_T3 = 11,
    REG_T4 = 12,
    REG_T5 = 13,
    REG_T6 = 14,
    REG_T7 = 15,
    REG_S0 = 16,
    REG_S1 = 17,
    REG_S2 = 18,
    REG_S3 = 19,
    REG_S4 = 20,
    REG_S5 = 21,
    REG_S6 = 22,
    REG_S7 = 23,
    REG_T8 = 24,
    REG_T9 = 25,
    REG_K0 = 26,
    REG_K1 = 27,
    REG_GP = 28,
    REG_SP = 29,
    REG_FP = 30,
    REG_RA = 31,
    REG_SPECIAL_PC = 32,
    REG_SPECIAL_HI = 33,
    REG_SPECIAL_LO = 34
} MIPS_REGISTERS;

typedef enum
{
    MIPS_OP_ADD = 0x00,
    MIPS_OP_ADDI = 0x08,
    MIPS_OP_ADDIU = 0x09,
    MIPS_OP_ADDU = 0x00,
    MIPS_OP_AND = 0x00,
    MIPS_OP_ANDI = 0x0C,
    MIPS_OP_BEQ = 0x04,
    MIPS_OP_BNE = 0x05,
    MIPS_OP_BLEZ = 6,
    MIPS_OP_BGTZ = 7,
    MIPS_OP_DIV = 0x00,
    MIPS_OP_DIVU = 0x00,
    MIPS_OP_J = 0x02,
    MIPS_OP_JAL = 0x03,
    MIPS_OP_JALR = 0x00,
    MIPS_OP_JR = 0x00,
    MIPS_OP_LLO = 24,
    MIPS_OP_LHI = 25,
    MIPS_OP_LB = 32,
    MIPS_OP_LH = 33,
    MIPS_OP_LBU = 0x24,
    MIPS_OP_LHU = 0x25,
    MIPS_OP_LUI = 0x0F,
    MIPS_OP_LW = 0x23,
    MIPS_OP_MFHI = 0x00,
    MIPS_OP_MTHI = 0x00,
    MIPS_OP_MFLO = 0x00,
    MIPS_OP_MTLO = 0x00,
    MIPS_OP_MULT = 0x00,
    MIPS_OP_MULTU = 0x00,
    MIPS_OP_NOR = 0x00,
    MIPS_OP_XOR = 0x00,
    MIPS_OP_XORI = 14,
    MIPS_OP_OR = 0x00,
    MIPS_OP_ORI = 0x0D,
    MIPS_OP_SB = 0x28,
    MIPS_OP_SH = 0x29,
    MIPS_OP_SLT = 0x00,
    MIPS_OP_SLTI = 0x0A,
    MIPS_OP_SLTIU = 0x0B,
    MIPS_OP_SLTU = 0x00,
    MIPS_OP_SLL = 0x00,
    MIPS_OP_SLLV = 0x00,
    MIPS_OP_SRL = 0x00,
    MIPS_OP_SRLV = 0x00,
    MIPS_OP_SRA = 0x00,
    MIPS_OP_SRAV = 0x00,
    MIPS_OP_SUB = 0x00,
    MIPS_OP_SUBU = 0x00,
    MIPS_OP_SW = 0x2B,
    MIPS_OP_TRAP = 0x1A,
    MIPS_OP_SYSCALL = 0x00
} MIPS_OPCODES;

typedef enum
{
    MIPS_FUNCT_ADD = 0x20,
    MIPS_FUNCT_ADDU = 0x21,
    MIPS_FUNCT_AND = 0x24,
    MIPS_FUNCT_DIV = 0x1A,
    MIPS_FUNCT_DIVU = 0x1B,
    MIPS_FUNCT_JALR = 0x9,
    MIPS_FUNCT_JR = 0x08,
    MIPS_FUNCT_MFHI = 0x10,
    MIPS_FUNCT_MTHI = 17,
    MIPS_FUNCT_MFLO = 0x12,
    MIPS_FUNCT_MTLO = 19,
    MIPS_FUNCT_MULT = 0x18,
    MIPS_FUNCT_MULTU = 0x19,
    MIPS_FUNCT_NOR = 0x27,
    MIPS_FUNCT_XOR = 0x26,
    MIPS_FUNCT_OR = 0x25,
    MIPS_FUNCT_SLT = 0x2A,
    MIPS_FUNCT_SLTU = 0x2B,
    MIPS_FUNCT_SLL = 0x00,
    MIPS_FUNCT_SLLV = 0x04,
    MIPS_FUNCT_SRL = 0x02,
    MIPS_FUNCT_SRLV = 0x06,
    MIPS_FUNCT_SRA = 0x03,
    MIPS_FUNCT_SRAV = 0x07,
    MIPS_FUNCT_SUB = 0x22,
    MIPS_FUNCT_SUBU = 0x23,
    MIPS_FUNCT_SYSCALL = 12
} MIPS_FUNCTS;

typedef struct
{
    unsigned int opcode : 6;
    unsigned int rs : 5;
    unsigned int rt : 5;
    unsigned int rd : 5;
    unsigned int shamt : 5;
    unsigned int funct : 6;
} MIPS_INSTRUCTION_R;

typedef struct
{
    unsigned int opcode : 6;
    unsigned int rs : 5;
    unsigned int rt : 5;
    unsigned int imm : 16;
} MIPS_INSTRUCTION_I;

typedef struct
{
    unsigned int opcode : 6;
    unsigned int address : 26;
} MIPS_INSTRUCTION_J;

typedef struct
{
    union
    {
        MIPS_INSTRUCTION_J j_inst;
        MIPS_INSTRUCTION_I i_inst;
        MIPS_INSTRUCTION_R r_inst;
    } instructionData;
} MIPS_INSTRUCTION;

int
MIPS_TranslateInstruction(MIPS_INSTRUCTION *src,
                          IL_OperationInfo *dst);


#endif
