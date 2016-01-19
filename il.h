#ifndef _IL_H_
#define _IL_H_

#include <stddef.h>
#include <stdint.h>

typedef enum
{
    IL_NOP,	//No operation
    IL_ADD,	//Addition
    IL_SUB,	//Subtraction
    IL_MUL,	//Multiplication
    IL_DIV,	//Division
    IL_OR,	//Bitwise OR
    IL_XOR,	//Bitwise XOR
    IL_AND,	//Bitwise AND
    IL_NOT,	//Bitwise NOT
    IL_SLL,	//Bitwise left shift
    IL_SRL,	//Bitwise right shift
    IL_B,	//Branch (relative to PC)
    IL_NLT,  //Execute next instruction if operand 0 is less than operand 1
    IL_NLE,  //Execute next instruction if operand 0 is less than or equal to operand 1
    IL_NEQ,  //Execute next instruction if the two operands are equal
    IL_J	//Jump (global)
} IL_Instruction;

typedef enum
{
    IL_Signed,
    IL_Unsigned
} IL_Sign;

typedef enum
{
    IL_Register,
    IL_MemoryAddress,
    IL_Immediate
} IL_OperandType;

typedef struct
{
    uint64_t value;
    IL_OperandType type;
} IL_Operand;

typedef struct IL_OperationInfo
{
    IL_Sign sign;
    IL_Operand *operands;
    uint32_t operandCount;
    IL_Instruction instruction;
    uint32_t operandBitWidth;

    struct IL_OperationInfo *chainNext;	//Allows for translation to create more than one IL instruction, these will be expanded into the final chain
} IL_OperationInfo;

#endif
