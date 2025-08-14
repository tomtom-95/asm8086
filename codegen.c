#include "codegen.h"

// let's start from the fact that InstructionData has been filled
// before starting the codegen phase I can look at what I have and make decision
// for example:
// mov si, bx
// for this instruction what do I have in InstructionData?
// I can look at the two operands, realize that are both OPERAND_GENERAL_REGISTER_16_BIT and make decision

typedef enum {
  FORM_RM_TOFROM_REG,
  FORM_MOFFS_TO_ACC,  // AL/AX <- moffs16
  FORM_RM_TO_ACC,
  FORM_ACC_TO_MEM,
  FORM_IMM_TO_RM,     // r/m <- imm (GRP1/GRP2/etc. with /digit)
  FORM_RM_TO_SR,      // segreg <-> r/m
  FORM_SR_TO_RM,
  FORM_REL,           // Jcc, JMP, CALL near/short (PC-rel)
  FORM_FAR_IMM,       // far JMP/CALL with imm16:16
} IrForm;

typedef struct InstructionIR InstructionIR;
struct InstructionIR {
    IrForm ir_form;
    u8     mod;
    u8     reg;
    u8     w;
    u8     d;
};

bool
is_operand_general_reg(Operand operand)
{
    return(operand.operand_kind == OPERAND_GENERAL_REGISTER_8_BIT
           ||
           operand.operand_kind == OPERAND_GENERAL_REGISTER_16_BIT);
}

bool
is_operand_reg_or_mem(Operand operand)
{
    return is_operand_general_reg(operand) || (operand.operand_kind == OPERAND_EFFECTIVE_ADDRESS);
}

bool
is_operand_immediate(Operand operand)
{
    return(operand.operand_kind == OPERAND_IMMEDIATE_8_BIT
           ||
           operand.operand_kind == OPERAND_IMMEDIATE_16_BIT);
}

bool
is_operand_effective_address(Operand operand)
{
    return (operand.operand_kind == OPERAND_EFFECTIVE_ADDRESS);
}

bool
is_operand_accumulator(Operand operand)
{
    if (operand.operand_kind == OPERAND_GENERAL_REGISTER_8_BIT || operand.operand_kind == OPERAND_GENERAL_REGISTER_16_BIT)
    {
        return (operand.register_general == TOK_AH || operand.register_general == TOK_AL || operand.register_general == TOK_AX);
    }
    else
    {
        return false;
    }
}

bool
is_operand_segment_register(Operand operand)
{
    return (operand.operand_kind == OPERAND_SEGMENT_REGISTER);
}

InstructionIR
fill_intermediate_representation(InstructionData inst_data)
{
    InstructionIR ir = {0};

    Operand operand_src = inst_data.src;
    Operand operand_dst = inst_data.dst;

    bool src_is_reg = is_operand_general_reg(operand_src);
    bool dst_is_reg = is_operand_general_reg(operand_dst);

    bool src_is_rm = is_operand_reg_or_mem(operand_src);
    bool dst_is_rm = is_operand_reg_or_mem(operand_dst);

    bool src_is_immediate = is_operand_immediate(operand_src);

    bool src_is_acc = is_operand_accumulator(operand_src);
    bool dst_is_acc = is_operand_accumulator(operand_dst);

    bool src_is_mem = is_operand_effective_address(operand_src);
    bool dst_is_mem = is_operand_effective_address(operand_dst);

    bool src_is_seg_reg = is_operand_segment_register(operand_src);
    bool dst_is_seg_reg = is_operand_segment_register(operand_dst);


    if (is_16_bit_reg_involved(operand_src, operand_dst) ||
        is_16_bit_operand_immediate_involved(operand_src, operand_dst))
    {
        ir.w = 1;
    }
    else
    {
        ir.w = 0;
    }

    if (src_is_reg)
    {
        ir.d = 0;
    }
    else
    {
        ir.d = 1;
    }

    if (src_is_rm && dst_is_rm)
    {
        ir.ir_form = FORM_RM_TOFROM_REG;
    }
    else if (src_is_immediate && is_operand_reg_or_mem(operand_dst))
    {
        ir.ir_form = FORM_IMM_TO_RM;
    }
    else if (src_is_mem && dst_is_acc)
    {
        ir.ir_form = FORM_RM_TO_ACC;
    }
    else if (src_is_acc && dst_is_mem)
    {
        ir.ir_form = FORM_ACC_TO_MEM;
    }
    else if (src_is_rm && dst_is_seg_reg)
    {
        ir.ir_form = FORM_RM_TO_SR;
    }
    else if (src_is_seg_reg && dst_is_rm)
    {
        ir.ir_form = FORM_SR_TO_RM;
    }
}

bool
is_valid_reg_reg_operation(Operand src, Operand dst)
{
    bool src_8_bit = (src.operand_kind == OPERAND_GENERAL_REGISTER_8_BIT);
    bool dst_8_bit = (dst.operand_kind == OPERAND_GENERAL_REGISTER_8_BIT);

    bool src_16_bit = (src.operand_kind == OPERAND_GENERAL_REGISTER_16_BIT);
    bool dst_16_bit = (dst.operand_kind == OPERAND_GENERAL_REGISTER_16_BIT);

    return (src_8_bit && dst_8_bit) || (src_16_bit && dst_16_bit);
}

bool
is_16_bit_reg_involved(Operand src, Operand dst)
{
    bool src_16_bit = (src.operand_kind == OPERAND_GENERAL_REGISTER_16_BIT);
    bool dst_16_bit = (dst.operand_kind == OPERAND_GENERAL_REGISTER_16_BIT);

    return (src_16_bit || dst_16_bit);
}

bool
is_16_bit_operand_immediate_involved(Operand src, Operand dst)
{
    bool src_immediate_16_bit = (src.operand_kind == OPERAND_IMMEDIATE_16_BIT);
    bool dst_immediate_16_bit = (dst.operand_kind == OPERAND_IMMEDIATE_16_BIT);

    return (src_immediate_16_bit || dst_immediate_16_bit);
}

static u64
encode_rm(TokenKind base, TokenKind index)
{
    if (base == TOK_NULL) { base = index; index = TOK_NULL; }

    if (is_base(base) && is_index(index)) {
        return ((base==TOK_BP) << 1) | (index==TOK_DI);
    }
    if (index == TOK_NULL) {
        switch (base) {
            case TOK_SI: return 4;
            case TOK_DI: return 5;
            case TOK_BP: return 6;
            case TOK_BX: return 7;
            default: break;
        }
    }
    return 0xFF;
}

static u64
encode_mod(EffectiveAddress eaddr)
{
    TokenKind base = eaddr.register_base;
    TokenKind index = eaddr.register_index;
    u64 displacement = eaddr.displacement;
    u64 direct_addr = eaddr.direct_address;

    if (direct_addr)
    {
        return 0;
    }

    if (base == TOK_BP && !displacement)
    {
        return 1;
    }

    if (!displacement)
    {
        return 0;
    }

    if (displacement < 256)
    {
        return 1;
    }

    return 2; 
}

u64
encode_sr(Operand operand)
{
    switch (operand.register_segment)
    {
        case TOK_ES: return 0;
        case TOK_CS: return 1;
        case TOK_SS: return 2;
        case TOK_DS: return 3;
    }
}

InstructionEncoding
codegen(void)
{
    InstDescription inst_description;
    // TODO: fill the other option for instruction description

    InstructionEncoding enc = {0};

    for (u64 i = 0; i < ArrayCount(inst_table); ++i)
    {
        bool mnemonic_found = (inst_table[i].mnemonic == instruction_data.mnemonic);
        bool description_found = (inst_table[i].instruction_description == inst_description);
        if (mnemonic_found && description_found)
        {
            Instruction instruction = inst_table[i]; 
            for (u64 j = 0; instruction.fields[j].id != INST_END; ++j)
            {
                InstField field = instruction.fields[j];
                enc.encoding = enc.encoding << instruction.fields[j].size;

                if (instruction.fields[j].id == INST_OPCODE)
                {
                    enc.encoding = enc.encoding | field.value;
                }
                else if (instruction.fields[j].id == INST_D)
                {
                    // TODO: get the value of D bit based on what I have in InstructionData
                    // d = 0: Instruction source is in the reg field
                    // d = 1: Instruction destination is in the reg field
                    // Let's take the mov instruction: mov si, bx -> both dst and src are register -> this means I have the freedom to choose
                    // d = 0 or d = 1
                    // If d == 0 -> Encoding will be: 10001001 | 11 011 110 -> x89DE : this is the encoding nasm choose
                    // If d == 1 -> Encoding will be: 10001011 | 11 110 011 -> x8BF3
                    // which encoding is chosen by nasm?
                    // If both src and dst are register -> assume d = 0 and the reg field will contain the encoding of the source
                    // If one operand is a register and the other is an address -> if the register is dst -> d = 1 else d = 0

                    // NOTE: if I take the move instruction this is the only case in which I think I would need to update the d bit to 1
                    //       in all the other cases 0 is fine
                    //       pretty sure this will break in the future with other instructions
                    if (!src_is_general_reg && dst_is_general_reg)
                    {
                        enc.encoding = enc.encoding | 1;
                    }
                }
                else if (instruction.fields[j].id == INST_W)
                {
                    if (is_16_bit_reg_involved(instruction_data.src, instruction_data.dst))
                    {
                        enc.encoding = enc.encoding | 1; 
                    }
                    if (is_16_bit_operand_immediate_involved(instruction_data.src, instruction_data.dst))
                    {
                        enc.encoding = enc.encoding | 1; 
                    }
                }
                else if (instruction.fields[j].id == INST_MOD)
                {
                    u64 mod = 0;
                    if (is_operand_effective_address(instruction_data.src))
                    {
                        mod = encode_mod(instruction_data.src.effective_address);
                    }
                    else if (is_operand_effective_address(instruction_data.dst))
                    {
                        mod = encode_mod(instruction_data.dst.effective_address);
                    }
                    else
                    {
                        mod = 3;
                    }

                    enc.encoding = enc.encoding | mod;
                }
                else if (instruction.fields[j].id == INST_REG)
                {
                    u64 reg = 0;
                    if (is_operand_general_reg(instruction_data.src))
                    {
                        reg = reg_field_lut[instruction_data.src.register_general];
                    }
                    else if (is_operand_general_reg(instruction_data.dst))
                    {
                        reg = reg_field_lut[instruction_data.dst.register_general];
                    }

                    enc.encoding = enc.encoding | reg;
                }
                else if (instruction.fields[j].id == INST_RM)
                {
                    u64 rm;
                    if (is_operand_effective_address(instruction_data.src))
                    {
                        rm = encode_rm(instruction_data.src.effective_address.register_base,
                            instruction_data.src.effective_address.register_index);
                    }
                    else if (is_operand_effective_address(instruction_data.dst))
                    {
                        rm = encode_rm(instruction_data.dst.effective_address.register_base,
                            instruction_data.dst.effective_address.register_index);
                    }
                    else
                    {
                        rm = reg_field_lut[instruction_data.dst.register_general];
                    }

                    enc.encoding = enc.encoding | rm;
                }
                else if (instruction.fields[j].id == INST_DISP)
                {
                    Operand operand_eaddr;
                    if (is_operand_effective_address(instruction_data.src))
                    {
                        operand_eaddr = instruction_data.src;
                    }
                    else
                    {
                        operand_eaddr = instruction_data.dst;
                    }

                    if (operand_eaddr.effective_address.register_base == TOK_BP)
                    {
                        // need to have a displacement even if 0
                        if (operand_eaddr.effective_address.displacement == TOK_NULL)
                        {
                            enc.encoding = enc.encoding << 8;
                            enc.len += 8;
                        }
                        else
                        {
                            if (instruction_data.src.effective_address.displacement_value < 256)
                            {
                                enc.encoding = enc.encoding << 8;
                                enc.len += 8;
                            }
                            else
                            {
                                enc.encoding = enc.encoding << 16;
                                enc.len += 16;
                            }
                            enc.encoding = enc.encoding | operand_eaddr.effective_address.displacement_value;
                        }
                    }
                    else
                    {
                        if (operand_eaddr.effective_address.displacement == TOK_NUM)
                        {
                            if (operand_eaddr.effective_address.displacement_value < 256)
                            {
                                enc.encoding = enc.encoding << 8;
                                enc.len += 8;
                            }
                            else
                            {
                                enc.encoding = enc.encoding << 16;
                                enc.len += 16;
                            }
                            enc.encoding = enc.encoding | operand_eaddr.effective_address.displacement_value;
                        }
                    }
                }
                else if (instruction.fields[j].id == INST_IMM)
                {
                    Operand operand_imm;
                    if (is_operand_immediate(instruction_data.src))
                    {
                        operand_imm = instruction_data.src;
                    }
                    else
                    {
                        operand_imm = instruction_data.dst;
                    }

                    if (is_16_bit_operand_immediate_involved(instruction_data.src, instruction_data.dst) ||
                        is_16_bit_reg_involved(instruction_data.src, instruction_data.dst))
                    {
                        enc.len += 16;

                        u16 b0 = ((u16)operand_imm.immediate) & 0xFF;
                        u16 b1 = (((u16)operand_imm.immediate) >> 8) & 0xFF;

                        enc.encoding = enc.encoding << 8;
                        enc.encoding = enc.encoding | b0;
                        enc.encoding = enc.encoding << 8;
                        enc.encoding = enc.encoding | b1;
                        // the immediate value I read is a 16 bit already
                    }
                    else
                    {
                        enc.len += 8;
                        enc.encoding = enc.encoding << 8;
                        enc.encoding = enc.encoding | (u8)operand_imm.immediate;
                    }
                }
                else if (instruction.fields[j].id == INST_SR)
                {
                    u64 sr = 0;
                    if (src_is_seg_reg)
                    {
                        sr = encode_sr(instruction_data.src);
                    }
                    else
                    {
                        sr = encode_sr(instruction_data.dst);
                    }

                    enc.encoding = enc.encoding | sr;
                }

                enc.len += instruction.fields[j].size;
            }

            return enc;
        }
    }

    printf("Codegen failed\n");
    return (InstructionEncoding){0};
}
