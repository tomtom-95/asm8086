#include "codegen.h"

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



InstructionEncoding
codegen(void)
{
    bool src_is_general_reg = is_operand_general_reg(instruction_data.src);
    bool dst_is_general_reg = is_operand_general_reg(instruction_data.dst);

    bool src_is_reg_or_mem = is_operand_reg_or_mem(instruction_data.src);
    bool dst_is_reg_or_mem = is_operand_reg_or_mem(instruction_data.dst);

    if (src_is_general_reg && dst_is_general_reg)
    {
        if (!is_valid_reg_reg_operation(instruction_data.src, instruction_data.dst))
        {
            printf("Error: register operands have different sizes\n");
            return (InstructionEncoding){0};
        }
    }

    // TODO: more validation here, what to expect when one of the operand is an immediate etc ...

    InstDescription inst_description;
    if (src_is_reg_or_mem && dst_is_reg_or_mem)
    {
        inst_description = REGMEM_TOFROM_REG;
    }

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
                    if (is_operand_effective_address(instruction_data.src))
                    {
                        if (instruction_data.src.effective_address.displacement == TOK_NUM)
                        {
                            if (instruction_data.src.effective_address.displacement_value < 256)
                            {
                                enc.encoding = enc.encoding << 8;
                            }
                            else
                            {
                                enc.encoding = enc.encoding << 16;
                            }
                            enc.encoding = enc.encoding | instruction_data.src.effective_address.displacement_value;
                        }
                    }
                    else if (is_operand_effective_address(instruction_data.dst))
                    {
                        if (instruction_data.dst.effective_address.displacement == TOK_NUM)
                        {
                            if (instruction_data.dst.effective_address.displacement_value < 256)
                            {
                                enc.encoding = enc.encoding << 8;
                            }
                            else
                            {
                                enc.encoding = enc.encoding << 16;
                            }
                            enc.encoding = enc.encoding | instruction_data.dst.effective_address.displacement_value;
                        }
                    }
                }
                // TODO: keep defining this stuff
                else if (instruction.fields[j].id == INST_IMM)
                {
                    if (is_operand_immediate(instruction_data.src))
                    {
                        // TODO: get if it is a 8 or 16 bit immediate
                    }
                    if (is_operand_immediate(instruction_data.dst))
                    {
                        // TODO: get if it is a 8 or 16 bit immediate
                    }
                }
                else if (instruction.fields[j].id == INST_ImplRmD)
                {

                }
                else if (instruction.fields[j].id == INST_ImplRegD)
                {

                }
                else if (instruction.fields[j].id == INST_ImplReg)
                {

                }
                else if (instruction.fields[j].id == INST_SR)
                {

                }

                enc.len += instruction.fields[j].size;
            }

            return enc;
        }
    }

    printf("Codegen failed\n");
    return (InstructionEncoding){0};
}
