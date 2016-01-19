#include "mips.h"
#include "stdlib.h"

int
MIPS_TranslateInstruction(MIPS_INSTRUCTION *src,
                          IL_OperationInfo *dst)
{
    switch(src->instructionData.j_inst.opcode)
        {

        //R type instructions
        case 0x00:
        {
            IL_Operand *operands = malloc(sizeof(IL_Operand) * 3);
            operands[0].value = src->instructionData.r_inst.rs;
            operands[1].value = src->instructionData.r_inst.rt;
            operands[2].value = src->instructionData.r_inst.rd;

            operands[0].type = operands[1].type = operands[2].type = IL_Register;
            dst->operands = operands;
            dst->operandCount = 3;
            dst->operandBitWidth = 32;
            dst->chainNext = NULL;

            switch(src->instructionData.r_inst.funct)
                {
                case MIPS_FUNCT_ADD:
                {
                    dst->sign = IL_Signed;
                    dst->instruction = IL_ADD;
                }
                break;
                case MIPS_FUNCT_ADDU:
                {
                    dst->sign = IL_Unsigned;
                    dst->instruction = IL_ADD;
                }
                break;
                case MIPS_FUNCT_AND:
                {
                    dst->sign = IL_Unsigned;
                    dst->instruction = IL_AND;
                }
                break;
		case MIPS_FUNCT_DIV:
		    {
		      dst->sign = IL_Signed;
		      dst->instruction = IL_DIV;
		    }
		  break;
		case MIPS_FUNCT_DIVU:
		    {
		      dst->sign = IL_Unsigned;
		      dst->instruction = IL_DIV;
		    }
		case MIPS_FUNCT_JALR:
		  break;
		case MIPS_FUNCT_JR:
		  break;
		case MIPS_FUNCT_MFHI:
		    {
		      dst->sign = IL_Unsigned;
		      dst->instruction = IL_ADD;
		      dst->operands[0].value = REG_SPECIAL_HI;
		      dst->operands[1].value = 0;
		      dst->operands[1].type = IL_Immediate;
		    }
		  break;
		case MIPS_FUNCT_MTHI:
		  break;
		case MIPS_FUNCT_MFLO:
		    {
		      dst->sign = IL_Unsigned;
		      dst->instruction = IL_ADD;
		      dst->operands[0].value = REG_SPECIAL_LO;
		      dst->operands[1].value = 0;
		      dst->operands[1].type = IL_Immediate;
		    }
		  break;
		case MIPS_FUNCT_MTLO:
		  break;
		case MIPS_FUNCT_MULT:
		    {
		      dst->sign = IL_Signed;
		      dst->instruction = IL_MUL;
		    }
		  break;
		case MIPS_FUNCT_MULTU:
		    {
		      dst->sign = IL_Unsigned;
		      dst->instruction = IL_MUL;
		    }
		  break;
		case MIPS_FUNCT_NOR:
		    {
		      //Break a NOR down into NOT-OR
		      dst->sign = IL_Unsigned;
		      dst->instruction = IL_OR;

		      dst->chainNext = malloc(sizeof(IL_OperationInfo));

		      //Create the NOT instruction
		      IL_Operand *chainOperands = malloc(sizeof(IL_Operand) * 3);
		      chainOperands[0].value = dst->operands[2].value;
		      chainOperands[0].type = dst->operands[2].type;

		      chainOperands[1].value = 0;
		      chainOperands[1].type = IL_Immediate;

		      chainOperands[2].value = dst->operands[2].value;
		      chainOperands[2].type = dst->operands[2].type;

		      dst->chainNext->sign = IL_Unsigned;
		      dst->chainNext->instruction = IL_NOT;
		      dst->chainNext->operands = chainOperands;
		      dst->chainNext->operandCount = 3;
		      dst->chainNext->operandBitWidth = dst->operandBitWidth;
		    }
		  break;
		case MIPS_FUNCT_XOR:
		    {
		      dst->sign = IL_Unsigned;
		      dst->instruction = IL_XOR;
		    }
		  break;
		case MIPS_FUNCT_OR:
		    {
		      dst->sign = IL_Unsigned;
		      dst->instruction = IL_OR;
		    }
		  break;
		case MIPS_FUNCT_SLT:
		    {

		    }
		  break;
		case MIPS_FUNCT_SLTU:
		  break;
		case MIPS_FUNCT_SLL:
		  break;
		case MIPS_FUNCT_SLLV:
		  break;
		case MIPS_FUNCT_SRL:
		  break;
		case MIPS_FUNCT_SRLV:
		  break;
		case MIPS_FUNCT_SRA:
		  break;
		case MIPS_FUNCT_SRAV:
		  break;
		case MIPS_FUNCT_SUB:
		    {
		      dst->sign = IL_Signed;
		      dst->instruction = IL_SUB;
		    }
		  break;
		case MIPS_FUNCT_SUBU:
		  break;
		case MIPS_FUNCT_SYSCALL:
		  break;
                }
        }
        break;

        case 0x02:
            break;
        case 0x03:
            break;
        case 0x04:
            break;
        case 0x05:
            break;
        case 0x08:
            break;
        case 0x09:
            break;
        case 0x0A:
            break;
        case 0x0B:
            break;
        case 0x0C:
            break;
        case 0x0D:
            break;
        case 0x0F:
            break;
        case 0x24:
            break;
        case 0x25:
            break;
        case 0x28:
            break;
        case 0x29:
            break;
        case 0x2B:
            break;
        }
  return 0;
}
