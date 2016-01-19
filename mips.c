#include "mips.h"
#include "stdlib.h"
#include "llist.h"

MIPS_INSTRUCTION*
MIPS_GenerateBranch(MIPS_OPCODES ops,
		    MIPS_REGISTERS s,
		    MIPS_REGISTERS t,
		    int b_offset)
{
  MIPS_INSTRUCTION *inst = malloc(sizeof(MIPS_INSTRUCTION));
  inst->instructionData.i_inst.imm = (b_offset >> 2) & 0xFFFF;
  inst->instructionData.i_inst.rt = t;
  inst->instructionData.i_inst.rs = s;
  inst->instructionData.i_inst.opcode = ops;

  return inst;
}

LListEntry*
MIPS_PreprocessInstructionStream(MIPS_INSTRUCTION *src,
				 int entryCount)
{
  //Rearrange delay slots, translate 'likely' instructions into singular instructions that don't depend on the next instruction

  LListEntry *list = malloc(sizeof(LListEntry));
  list->prev = NULL;
  list->next = NULL;
  list->value = NULL;

  for(int i = 0; i < entryCount; i++)
    {

      switch(src[i].instructionData.j_inst.opcode)
	{
	case MIPS_OP_BEQL:
	  //Generate a BNE instruction to skip the delay slot
	  list->value = MIPS_GenerateBranch(MIPS_OP_BNE,
					    src[i].instructionData.i_inst.rs,
					    src[i].instructionData.i_inst.rt,
					    +4);
	  //Insert the delay slot instruction
	  list->next = malloc(sizeof(LListEntry));
	  list->next->prev = list;
	  list = list->next;
	  list->value = src[i + 1];

	  break;
	  //Swap delay slot instruction
	case MIPS_OP_BEQ:
	case MIPS_OP_BNE:
	case MIPS_OP_BLEZ:
	case MIPS_OP_BGTZ:]
	  if(i < entryCount - 1)
	    {
	      list->value = src[i + 1];
	      list->next = malloc(sizeof(LListEntry));
	      list->next->prev = list;
	      list = list->next;
	      list->value = src[i];
	    }
	  break;
	}

        list->next = malloc(sizeof (LListEntry));
	list->next->prev = list;
        list = list->next;
    }

}


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

	    operands[0].bitWidth = operands[1].bitWidth = operands[2].bitWidth = 32;

            operands[0].type = operands[1].type = operands[2].type = IL_Register;
            dst->operands = operands;
            dst->operandCount = 3;
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
                {
                    //break this down into a chain
                    //chain jump
                    dst->chainNext = malloc(sizeof(IL_OperationInfo));
                    dst->chainNext->operands = malloc(sizeof(IL_Operand));
                    dst->chainNext->operands[0].value = dst->operands[0].value;
                    dst->chainNext->operands[0].type = IL_Register;
		    dst->chainNext->operands[0].bitWidth = 32;
                    dst->chainNext->sign = IL_Unsigned;
                    dst->chainNext->instruction = IL_J;

                    //Link register
                    dst->sign = IL_Unsigned;
                    dst->instruction = IL_ADD;
                    dst->operands[0].value = REG_SPECIAL_PC;
                    dst->operands[1].value = 0;
                    dst->operands[1].type = IL_Immediate;
                    dst->operands[2].value = REG_RA;
                }
                break;
                case MIPS_FUNCT_JR:
                {
                    dst->sign = IL_Unsigned;
                    dst->instruction = IL_J;
                }
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
                {
                    dst->sign = IL_Unsigned;
                    dst->instruction = IL_ADD;
                    dst->operands[1].value = 0;
                    dst->operands[1].type = IL_Immediate;
                    dst->operands[2].value = REG_SPECIAL_HI;
                }
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
                {
                    dst->sign = IL_Unsigned;
                    dst->instruction = IL_ADD;
                    dst->operands[1].value = 0;
                    dst->operands[1].type = IL_Immediate;
                    dst->operands[2].value = REG_SPECIAL_LO;
                }
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
                    IL_Operand *chainOperands = malloc(sizeof(IL_Operand) * 2);
                    chainOperands[0].value = dst->operands[2].value;
                    chainOperands[0].type = dst->operands[2].type;
		    chainOperands[0].bitWidth = dst->operands[2].bitWidth;

                    chainOperands[1].value = dst->operands[2].value;
                    chainOperands[1].type = dst->operands[2].type;
		    chainOperands[1].bitWidth = dst->operands[2].bitWidth;

                    dst->chainNext->sign = IL_Unsigned;
                    dst->chainNext->instruction = IL_NOT;
                    dst->chainNext->operands = chainOperands;
                    dst->chainNext->operandCount = 2;
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
                    dst->sign = IL_Signed;
                    dst->instruction = IL_NLT;
                }
                break;
                case MIPS_FUNCT_SLTU:
                {
                    dst->sign = IL_Unsigned;
                    dst->instruction = IL_NLT;
                }
                break;
                case MIPS_FUNCT_SLL:
                {
                    dst->sign = IL_Unsigned;
                    dst->instruction = IL_SLL;
                    dst->operands[0].type = IL_Immediate;
                    dst->operands[0].value = src->instructionData.r_inst.shamt;
		    dst->operands[0].bitWidth = 5;
                }
                break;
                case MIPS_FUNCT_SLLV:
                {
                    dst->sign = IL_Unsigned;
                    dst->instruction = IL_SLL;
                }
                break;
                case MIPS_FUNCT_SRL:
                {
                    dst->sign = IL_Unsigned;
                    dst->instruction = IL_SRL;
                    dst->operands[0].type = IL_Immediate;
                    dst->operands[0].value = src->instructionData.r_inst.shamt;
		    dst->operands[0].bitWidth = 5;
                }
                break;
                case MIPS_FUNCT_SRLV:
                {
                    dst->sign = IL_Unsigned;
                    dst->instruction = IL_SRL;
                }
                break;
                case MIPS_FUNCT_SRA:
                {
                    dst->sign = IL_Signed;
                    dst->instruction = IL_SRL;
                    dst->operands[0].type = IL_Immediate;
                    dst->operands[0].value = src->instructionData.r_inst.shamt;
		    dst->operands[0].bitWidth = 5;
                }
                break;
                case MIPS_FUNCT_SRAV:
                {
                    dst->sign = IL_Signed;
                    dst->instruction = IL_SRL;
                }
                break;
                case MIPS_FUNCT_SUB:
                {
                    dst->sign = IL_Signed;
                    dst->instruction = IL_SUB;
                }
                break;
                case MIPS_FUNCT_SUBU:
                {
                    dst->sign = IL_Unsigned;
                    dst->instruction = IL_SUB;
                }
                break;
                case MIPS_FUNCT_SYSCALL:
                    break;
                }
        }
        break;

        case MIPS_OP_ADDI:
        {
            IL_Operand *operands = malloc(sizeof(IL_Operand) * 3);
            operands[0].value = src->instructionData.i_inst.rs;
            operands[1].value = src->instructionData.i_inst.imm;
            operands[2].value = src->instructionData.i_inst.rt;

            operands[0].type = operands[2].type = IL_Register;
	    operands[1].type = IL_Immediate;

	    operands[0].bitWidth = operands[2].bitWidth = 32;
	    operands[1].bitWidth = 16;

            dst->operands = operands;
            dst->operandCount = 3;
	    dst->sign = IL_Signed;
	    dst->instruction = IL_ADD;
            dst->chainNext = NULL;
        }
        break;
        case MIPS_OP_ADDIU:
        {
            IL_Operand *operands = malloc(sizeof(IL_Operand) * 3);
            operands[0].value = src->instructionData.i_inst.rs;
            operands[1].value = src->instructionData.i_inst.imm;
            operands[2].value = src->instructionData.i_inst.rt;

            operands[0].type = operands[2].type = IL_Register;
	    operands[1].type = IL_Immediate;

	    operands[0].bitWidth = operands[2].bitWidth = 32;
	    operands[1].bitWidth = 16;

            dst->operands = operands;
            dst->operandCount = 3;
	    dst->sign = IL_Unsigned;
	    dst->instruction = IL_ADD;
            dst->chainNext = NULL;
        }
            break;
        case MIPS_OP_ANDI:
        {
            IL_Operand *operands = malloc(sizeof(IL_Operand) * 3);
            operands[0].value = src->instructionData.i_inst.rs;
            operands[1].value = src->instructionData.i_inst.imm;
            operands[2].value = src->instructionData.i_inst.rt;

            operands[0].type = operands[2].type = IL_Register;
	    operands[1].type = IL_Immediate;

	    operands[0].bitWidth = operands[2].bitWidth = 32;
	    operands[1].bitWidth = 16;

            dst->operands = operands;
            dst->operandCount = 3;
	    dst->sign = IL_Unsigned;
	    dst->instruction = IL_AND;
            dst->chainNext = NULL;
        }
            break;
        case MIPS_OP_BEQ:
        {
            IL_Operand *operands = malloc(sizeof(IL_Operand) * 2);
            operands[0].value = src->instructionData.i_inst.rs;
            operands[1].value = src->instructionData.i_inst.rt;

            operands[0].type = IL_Register;
	    operands[1].type = IL_Register;

	    operands[0].bitWidth = 32;
	    operands[1].bitWidth = 32;

            dst->operands = operands;
            dst->operandCount = 2;
	    dst->sign = IL_Unsigned;
	    dst->instruction = IL_NEQ;

            dst->chainNext = malloc(sizeof(IL_OperationInfo));
	    dst->chainNext->operands = malloc(sizeof(IL_Operand));
	    dst->chainNext->operands[0].value = src->instructionData.i_inst.imm << 2;
	    dst->chainNext->operands[0].type = IL_Immediate;
	    dst->chainNext->operands[0].bitWidth = 18;
	    dst->chainNext->operandCount = 1;
	    dst->chainNext->sign = IL_Signed;
	    dst->chainNext->instruction = IL_B;
        }
            break;
        case MIPS_OP_BNE:
        {
            IL_Operand *operands = malloc(sizeof(IL_Operand) * 2);
            operands[0].value = src->instructionData.i_inst.rs;
            operands[1].value = src->instructionData.i_inst.rt;

            operands[0].type = IL_Register;
	    operands[1].type = IL_Register;

	    operands[0].bitWidth = 32;
	    operands[1].bitWidth = 32;

            dst->operands = operands;
            dst->operandCount = 2;
	    dst->sign = IL_Unsigned;
	    dst->instruction = IL_NNE;

            IL_OperationInfo *c0 = malloc(sizeof(IL_OperationInfo));
	    c0->operands = malloc(sizeof(IL_Operand));
	    c0->operands[0].value = src->instructionData.i_inst.imm << 2;
	    c0->operands[0].type = IL_Immediate;
	    c0->operands[0].bitWidth = 18;
	    c0->operandCount = 1;
	    c0->sign = IL_Signed;
	    c0->instruction = IL_B;

	    dst->chainNext = c0;
        }
            break;
        case MIPS_OP_BLEZ:
        {
            IL_Operand *operands = malloc(sizeof(IL_Operand) * 2);
            operands[0].value = src->instructionData.i_inst.rs;
            operands[1].value = 0;

            operands[0].type = IL_Register;
	    operands[1].type = IL_Immediate;

	    operands[0].bitWidth = 32;
	    operands[1].bitWidth = 32;

            dst->operands = operands;
            dst->operandCount = 2;
	    dst->sign = IL_Unsigned;
	    dst->instruction = IL_NLE;

            IL_OperationInfo *c0 = malloc(sizeof(IL_OperationInfo));
	    c0->operands = malloc(sizeof(IL_Operand));
	    c0->operands[0].value = src->instructionData.i_inst.imm << 2;
	    c0->operands[0].type = IL_Immediate;
	    c0->operands[0].bitWidth = 18;
	    c0->operandCount = 1;
	    c0->sign = IL_Signed;
	    c0->instruction = IL_B;

	    dst->chainNext = c0;
        }
            break;
        case MIPS_OP_BGTZ:
        {
            IL_Operand *operands = malloc(sizeof(IL_Operand) * 2);
            operands[0].value = 0;
            operands[1].value = src->instructionData.i_inst.rs;

            operands[0].type = IL_Immediate;
	    operands[1].type = IL_Register;

	    operands[0].bitWidth = 32;
	    operands[1].bitWidth = 32;

            dst->operands = operands;
            dst->operandCount = 2;
	    dst->sign = IL_Unsigned;
	    dst->instruction = IL_NLT;

            IL_OperationInfo *c0 = malloc(sizeof(IL_OperationInfo));
	    c0->operands = malloc(sizeof(IL_Operand));
	    c0->operands[0].value = src->instructionData.i_inst.imm << 2;
	    c0->operands[0].type = IL_Immediate;
	    c0->operands[0].bitWidth = 18;
	    c0->operandCount = 1;
	    c0->sign = IL_Signed;
	    c0->instruction = IL_B;

	    dst->chainNext = c0;
        }
            break;
        case MIPS_OP_J:
            break;
        case MIPS_OP_JAL:
            break;
        case MIPS_OP_LLO:
        {
            IL_Operand *operands = malloc(sizeof(IL_Operand) * 3);
            operands[0].value = 0;
            operands[1].value = src->instructionData.i_inst.imm;
            operands[2].value = src->instructionData.i_inst.rt;

            operands[2].type = IL_Register;
	    operands[0].type = operands[1].type = IL_Immediate;

	    operands[0].bitWidth = operands[1].bitWidth = 32;
	    operands[2].bitWidth = 32;

            dst->operands = operands;
            dst->operandCount = 3;
	    dst->sign = IL_Unsigned;
	    dst->instruction = IL_ADD;
            dst->chainNext = NULL;
        }
            break;
        case MIPS_OP_LHI:
        {
            IL_Operand *operands = malloc(sizeof(IL_Operand) * 3);
            operands[0].value = 0;
            operands[1].value = src->instructionData.i_inst.imm << 16;
            operands[2].value = src->instructionData.i_inst.rt;

            operands[2].type = IL_Register;
	    operands[0].type = operands[1].type = IL_Immediate;

	    operands[0].bitWidth = operands[1].bitWidth = 32;
	    operands[2].bitWidth = 32;

            dst->operands = operands;
            dst->operandCount = 3;
	    dst->sign = IL_Unsigned;
	    dst->instruction = IL_ADD;
            dst->chainNext = NULL;
        }
            break;
	  //Continue here
        case MIPS_OP_LB:
        {
            IL_Operand *operands = malloc(sizeof(IL_Operand) * 3);
            operands[0].value = src->instructionData.i_inst.rs;
            operands[1].value = src->instructionData.i_inst.imm;
            operands[2].value = src->instructionData.i_inst.rt;

            operands[0].type = operands[2].type = IL_Register;
	    operands[1].type = IL_Immediate;

            dst->operands = operands;
            dst->operandCount = 3;
	    dst->sign = IL_Signed;
	    dst->instruction = IL_ADD;
            dst->chainNext = NULL;
        }
            break;
        case MIPS_OP_LH:
        {
            IL_Operand *operands = malloc(sizeof(IL_Operand) * 3);
            operands[0].value = src->instructionData.i_inst.rs;
            operands[1].value = src->instructionData.i_inst.imm;
            operands[2].value = src->instructionData.i_inst.rt;

            operands[0].type = operands[2].type = IL_Register;
	    operands[1].type = IL_Immediate;

            dst->operands = operands;
            dst->operandCount = 3;
	    dst->sign = IL_Signed;
	    dst->instruction = IL_ADD;
            dst->chainNext = NULL;
        }
            break;
        case MIPS_OP_LBU:
        {
            IL_Operand *operands = malloc(sizeof(IL_Operand) * 3);
            operands[0].value = src->instructionData.i_inst.rs;
            operands[1].value = src->instructionData.i_inst.imm;
            operands[2].value = src->instructionData.i_inst.rt;

            operands[0].type = operands[2].type = IL_Register;
	    operands[1].type = IL_Immediate;

            dst->operands = operands;
            dst->operandCount = 3;
	    dst->sign = IL_Signed;
	    dst->instruction = IL_ADD;
            dst->chainNext = NULL;
        }
            break;
        case MIPS_OP_LHU:
        {
            IL_Operand *operands = malloc(sizeof(IL_Operand) * 3);
            operands[0].value = src->instructionData.i_inst.rs;
            operands[1].value = src->instructionData.i_inst.imm;
            operands[2].value = src->instructionData.i_inst.rt;

            operands[0].type = operands[2].type = IL_Register;
	    operands[1].type = IL_Immediate;

            dst->operands = operands;
            dst->operandCount = 3;
	    dst->sign = IL_Signed;
	    dst->instruction = IL_ADD;
            dst->chainNext = NULL;
        }
            break;
        case MIPS_OP_LUI:
        {
            IL_Operand *operands = malloc(sizeof(IL_Operand) * 3);
            operands[0].value = src->instructionData.i_inst.rs;
            operands[1].value = src->instructionData.i_inst.imm;
            operands[2].value = src->instructionData.i_inst.rt;

            operands[0].type = operands[2].type = IL_Register;
	    operands[1].type = IL_Immediate;

            dst->operands = operands;
            dst->operandCount = 3;
	    dst->sign = IL_Signed;
	    dst->instruction = IL_ADD;
            dst->chainNext = NULL;
        }
            break;
        case MIPS_OP_LW:
        {
            IL_Operand *operands = malloc(sizeof(IL_Operand) * 3);
            operands[0].value = src->instructionData.i_inst.rs;
            operands[1].value = src->instructionData.i_inst.imm;
            operands[2].value = src->instructionData.i_inst.rt;

            operands[0].type = operands[2].type = IL_Register;
	    operands[1].type = IL_Immediate;

            dst->operands = operands;
            dst->operandCount = 3;
	    dst->sign = IL_Signed;
	    dst->instruction = IL_ADD;
            dst->chainNext = NULL;
        }
            break;
        case MIPS_OP_XORI:
        {
            IL_Operand *operands = malloc(sizeof(IL_Operand) * 3);
            operands[0].value = src->instructionData.i_inst.rs;
            operands[1].value = src->instructionData.i_inst.imm;
            operands[2].value = src->instructionData.i_inst.rt;

            operands[0].type = operands[2].type = IL_Register;
	    operands[1].type = IL_Immediate;

            dst->operands = operands;
            dst->operandCount = 3;
	    dst->sign = IL_Unsigned;
	    dst->instruction = IL_XOR;
            dst->chainNext = NULL;
        }
            break;
        case MIPS_OP_ORI:
        {
            IL_Operand *operands = malloc(sizeof(IL_Operand) * 3);
            operands[0].value = src->instructionData.i_inst.rs;
            operands[1].value = src->instructionData.i_inst.imm;
            operands[2].value = src->instructionData.i_inst.rt;

            operands[0].type = operands[2].type = IL_Register;
	    operands[1].type = IL_Immediate;

            dst->operands = operands;
            dst->operandCount = 3;
	    dst->sign = IL_Unsigned;
	    dst->instruction = IL_OR;
            dst->chainNext = NULL;
        }
            break;
        case MIPS_OP_SB:
        {
            IL_Operand *operands = malloc(sizeof(IL_Operand) * 3);
            operands[0].value = src->instructionData.i_inst.rs;
            operands[1].value = src->instructionData.i_inst.imm;
            operands[2].value = src->instructionData.i_inst.rt;

            operands[0].type = operands[2].type = IL_Register;
	    operands[1].type = IL_Immediate;

            dst->operands = operands;
            dst->operandCount = 3;
	    dst->sign = IL_Signed;
	    dst->instruction = IL_ADD;
            dst->chainNext = NULL;
        }
            break;
        case MIPS_OP_SH:
        {
            IL_Operand *operands = malloc(sizeof(IL_Operand) * 3);
            operands[0].value = src->instructionData.i_inst.rs;
            operands[1].value = src->instructionData.i_inst.imm;
            operands[2].value = src->instructionData.i_inst.rt;

            operands[0].type = operands[2].type = IL_Register;
	    operands[1].type = IL_Immediate;

            dst->operands = operands;
            dst->operandCount = 3;
	    dst->sign = IL_Signed;
	    dst->instruction = IL_ADD;
            dst->chainNext = NULL;
        }
            break;
        case MIPS_OP_SLTI:
        {
            IL_Operand *operands = malloc(sizeof(IL_Operand) * 3);
            operands[0].value = src->instructionData.i_inst.rs;
            operands[1].value = src->instructionData.i_inst.imm;
            operands[2].value = src->instructionData.i_inst.rt;

            operands[0].type = operands[2].type = IL_Register;
	    operands[1].type = IL_Immediate;

            dst->operands = operands;
            dst->operandCount = 3;
	    dst->sign = IL_Signed;
	    dst->instruction = IL_ADD;
            dst->chainNext = NULL;
        }
            break;
        case MIPS_OP_SLTIU:
        {
            IL_Operand *operands = malloc(sizeof(IL_Operand) * 3);
            operands[0].value = src->instructionData.i_inst.rs;
            operands[1].value = src->instructionData.i_inst.imm;
            operands[2].value = src->instructionData.i_inst.rt;

            operands[0].type = operands[2].type = IL_Register;
	    operands[1].type = IL_Immediate;

            dst->operands = operands;
            dst->operandCount = 3;
	    dst->sign = IL_Signed;
	    dst->instruction = IL_ADD;
            dst->chainNext = NULL;
        }
            break;
        case MIPS_OP_SW:
        {
            IL_Operand *operands = malloc(sizeof(IL_Operand) * 3);
            operands[0].value = src->instructionData.i_inst.rs;
            operands[1].value = src->instructionData.i_inst.imm;
            operands[2].value = src->instructionData.i_inst.rt;

            operands[0].type = operands[2].type = IL_Register;
	    operands[1].type = IL_Immediate;

            dst->operands = operands;
            dst->operandCount = 3;
	    dst->sign = IL_Signed;
	    dst->instruction = IL_ADD;
            dst->chainNext = NULL;
        }
            break;
        case MIPS_OP_TRAP:
            break;
        }
    return 0;
}
