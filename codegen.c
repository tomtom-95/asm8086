#include "codegen.h"

// bool
// is_operand_general_reg(Operand operand)
// {
//     return(operand.operand_kind == OPERAND_GENERAL_REGISTER_8_BIT
//            ||
//            operand.operand_kind == OPERAND_GENERAL_REGISTER_16_BIT);
// }
// 
// bool
// is_operand_reg_or_mem(Operand operand)
// {
//     return is_operand_general_reg(operand) || (operand.operand_kind == OPERAND_EFFECTIVE_ADDRESS);
// }
// 
// bool
// is_operand_immediate(Operand operand)
// {
//     return(operand.operand_kind == OPERAND_IMMEDIATE_8_BIT
//            ||
//            operand.operand_kind == OPERAND_IMMEDIATE_16_BIT);
// }
// 
// bool
// is_operand_effective_address(Operand operand)
// {
//     return (operand.operand_kind == OPERAND_EFFECTIVE_ADDRESS);
// }
// 
// bool
// is_operand_accumulator(Operand operand)
// {
//     if (operand.operand_kind == OPERAND_GENERAL_REGISTER_8_BIT || operand.operand_kind == OPERAND_GENERAL_REGISTER_16_BIT)
//     {
//         return (operand.register_general == TOK_AH || operand.register_general == TOK_AL || operand.register_general == TOK_AX);
//     }
//     else
//     {
//         return false;
//     }
// }
// 
// bool
// is_operand_segment_register(Operand operand)
// {
//     return (operand.operand_kind == OPERAND_SEGMENT_REGISTER);
// }

// InstructionIR
// fill_intermediate_representation(InstructionData inst_data)
// {
//     InstructionIR ir = {0};
// 
//     Operand operand_src = inst_data.src;
//     Operand operand_dst = inst_data.dst;
// 
//     bool src_is_reg = is_operand_general_reg(operand_src);
//     bool dst_is_reg = is_operand_general_reg(operand_dst);
// 
//     bool src_is_rm = is_operand_reg_or_mem(operand_src);
//     bool dst_is_rm = is_operand_reg_or_mem(operand_dst);
// 
//     bool src_is_immediate = is_operand_immediate(operand_src);
// 
//     bool src_is_acc = is_operand_accumulator(operand_src);
//     bool dst_is_acc = is_operand_accumulator(operand_dst);
// 
//     bool src_is_mem = is_operand_effective_address(operand_src);
//     bool dst_is_mem = is_operand_effective_address(operand_dst);
// 
//     bool src_is_seg_reg = is_operand_segment_register(operand_src);
//     bool dst_is_seg_reg = is_operand_segment_register(operand_dst);
// 
// 
//     if (is_16_bit_reg_involved(operand_src, operand_dst) ||
//         is_16_bit_operand_immediate_involved(operand_src, operand_dst))
//     {
//         ir.w = 1;
//     }
//     else
//     {
//         ir.w = 0;
//     }
// 
//     if (src_is_reg)
//     {
//         ir.d = 0;
//     }
//     else
//     {
//         ir.d = 1;
//     }
// 
//     if (src_is_rm && dst_is_rm)
//     {
//         ir.ir_form = FORM_RM_TOFROM_REG;
//     }
//     else if (src_is_immediate && is_operand_reg_or_mem(operand_dst))
//     {
//         ir.ir_form = FORM_IMM_TO_RM;
//     }
//     else if (src_is_mem && dst_is_acc)
//     {
//         ir.ir_form = FORM_RM_TO_ACC;
//     }
//     else if (src_is_acc && dst_is_mem)
//     {
//         ir.ir_form = FORM_ACC_TO_MEM;
//     }
//     else if (src_is_rm && dst_is_seg_reg)
//     {
//         ir.ir_form = FORM_RM_TO_SR;
//     }
//     else if (src_is_seg_reg && dst_is_rm)
//     {
//         ir.ir_form = FORM_SR_TO_RM;
//     }
// }

// bool
// is_valid_reg_reg_operation(Operand src, Operand dst)
// {
//     bool src_8_bit = (src.operand_kind == OPERAND_GENERAL_REGISTER_8_BIT);
//     bool dst_8_bit = (dst.operand_kind == OPERAND_GENERAL_REGISTER_8_BIT);
// 
//     bool src_16_bit = (src.operand_kind == OPERAND_GENERAL_REGISTER_16_BIT);
//     bool dst_16_bit = (dst.operand_kind == OPERAND_GENERAL_REGISTER_16_BIT);
// 
//     return (src_8_bit && dst_8_bit) || (src_16_bit && dst_16_bit);
// }
// 
// bool
// is_16_bit_reg_involved(Operand src, Operand dst)
// {
//     bool src_16_bit = (src.operand_kind == OPERAND_GENERAL_REGISTER_16_BIT);
//     bool dst_16_bit = (dst.operand_kind == OPERAND_GENERAL_REGISTER_16_BIT);
// 
//     return (src_16_bit || dst_16_bit);
// }
// 
// bool
// is_16_bit_operand_immediate_involved(Operand src, Operand dst)
// {
//     bool src_immediate_16_bit = (src.operand_kind == OPERAND_IMMEDIATE_16_BIT);
//     bool dst_immediate_16_bit = (dst.operand_kind == OPERAND_IMMEDIATE_16_BIT);
// 
//     return (src_immediate_16_bit || dst_immediate_16_bit);
// }

u8
encode_sr(Operand operand)
{
    switch (operand.register_segment)
    {
        case TOK_ES: return 0;
        case TOK_CS: return 1;
        case TOK_SS: return 2;
        case TOK_DS: return 3;
        default: assert(0);
    }
}

InstEncoding
codegen(void)
{
    InstEncoding enc = {0};

    for (u64 i = 0; i < ArrayCount(inst_table_keys); ++i)
    {
        bool mnemonic_found    = (inst_table_keys[i].mnemonic == instruction_data.mnemonic);
        bool operand_dst_found = (inst_table_keys[i].dst & instruction_data.dst.operand_kind);

        if (mnemonic_found)
        {
            printf("here");
        }

        bool operand_src_found = false;
        if (instruction_data.src.operand_kind == OP_NONE)
        {
            operand_src_found = (inst_table_keys[i].src == OP_NONE);
        }
        else
        {
            operand_src_found = (inst_table_keys[i].src & instruction_data.src.operand_kind);
        }

        if (mnemonic_found && operand_dst_found && operand_src_found)
        {
            InstTableEntry inst = inst_table[i];
            for (u64 j = 0; inst.inst_fields[j].id != INST_END; ++j)
            {
                InstField field = inst.inst_fields[j];
                if (field.id == INST_OPCODE || field.id == INST_ImplD ||
                    field.id == INST_ImplW  || field.id == INST_ImplMod)
                {
                    enc.encoding <<=  field.bitlen;
                    enc.encoding |= field.value;
                    enc.bitlen += field.bitlen;
                }
                else if (field.id == INST_REG)
                {
                    enc.encoding <<= field.bitlen;

                    Operand reg_operand = {0};
                    if (instruction_data.src.operand_kind & (OP_REG8 | OP_REG16))
                    {
                        reg_operand = instruction_data.src;
                    }
                    else
                    {
                        reg_operand = instruction_data.dst;
                    }

                    enc.encoding |= reg_field_lut[reg_operand.register_general];
                    enc.bitlen += field.bitlen;
                }
                else if (field.id == INST_MOD)
                {
                    enc.encoding <<= field.bitlen;

                    Operand mem_operand = {0};
                    if (instruction_data.src.operand_kind & (OP_MEM | OP_DADDR))
                    {
                        mem_operand = instruction_data.src;
                    }
                    else
                    {
                        mem_operand = instruction_data.dst;
                    }

                    enc.encoding |= mem_operand.effective_address.mod;
                    enc.bitlen += field.bitlen;
                }
                else if (field.id == INST_RM)
                {
                    enc.encoding <<= field.bitlen;

                    Operand mem_operand = {0};
                    Operand reg_operand = {0};

                    if (instruction_data.src.operand_kind & (OP_MEM | OP_DADDR))
                    {
                        mem_operand = instruction_data.src;
                    }
                    else if (instruction_data.dst.operand_kind & (OP_MEM | OP_DADDR))
                    {
                        mem_operand = instruction_data.dst;
                    }
                    else if (instruction_data.dst.operand_kind & (OP_REG8 | OP_REG16))
                    {
                        reg_operand = instruction_data.dst;
                    }
                    else
                    {
                        assert(0);
                    }

                    if (mem_operand.operand_kind)
                    {
                        enc.encoding |= mem_operand.effective_address.rm;
                    }
                    else
                    {
                        enc.encoding |= reg_field_lut[reg_operand.register_general]; 
                    }

                    enc.bitlen += field.bitlen;
                }
                else if (field.id == INST_DISP)
                {
                    Operand mem_operand = {0};

                    if (instruction_data.src.operand_kind & (OP_MEM | OP_DADDR))
                    {
                        mem_operand = instruction_data.src;
                    }
                    else if (instruction_data.dst.operand_kind & (OP_MEM | OP_DADDR))
                    {
                        mem_operand = instruction_data.dst;
                    }

                    // TODO: I kinda hate this, maybe I prefer a flag that tells me
                    //       if I actually need to encode a displacement (even if it
                    //       is one byte with 0)
                    if (mem_operand.effective_address.mod)
                    {
                        s16 displacement = mem_operand.effective_address.displacement;

                        if (-128 <= displacement && displacement < 128)
                        {
                            enc.encoding <<= 8;
                            enc.encoding |= (u8)((s8)displacement);
                            enc.bitlen += 8;
                        }
                        else
                        {
                            enc.encoding <<= 8;
                            enc.encoding |= (u8)(((u16)displacement) & 0xff);
                            enc.encoding <<= 8;
                            enc.encoding |= (u8)((((u16)displacement) >> 8) & 0xff);
                            enc.bitlen += 16;
                        }
                    }
                    else
                    {
                        if (mem_operand.effective_address.direct_address) // TODO: should I handle the case [0]? there should be a flag that says need_direct_address 
                        {
                            s16 daddr = mem_operand.effective_address.direct_address;

                            enc.encoding <<= 8;
                            enc.encoding |= (u8)(((u16)daddr) & 0xff);
                            enc.encoding <<= 8;
                            enc.encoding |= (u8)((((u16)daddr) >> 8) & 0xff);
                            enc.bitlen += 16;
                        }
                    }
                }
                else if (field.id == INST_ADDR)
                {
                    Operand daddr_operand = {0};
                    if (instruction_data.src.operand_kind & OP_DADDR)
                    {
                        daddr_operand = instruction_data.src;
                    }
                    else
                    {
                        daddr_operand = instruction_data.dst;
                    }

                    s16 daddr = daddr_operand.effective_address.direct_address;

                    enc.encoding <<= 8;
                    enc.encoding |= (u8)(((u16)daddr) & 0xff);
                    enc.encoding <<= 8;
                    enc.encoding |= (u8)((((u16)daddr) >> 8) & 0xff);
                    enc.bitlen += 16;
                }
                else if (field.id == INST_IMM8)
                {
                    Operand imm_operand = {0};
                    if (instruction_data.src.operand_kind & OP_IMMEDIATE)
                    {
                        imm_operand = instruction_data.src;
                    }
                    else
                    {
                        imm_operand = instruction_data.dst;
                    }

                    s16 imm = imm_operand.immediate;

                    enc.encoding <<= 8;
                    enc.encoding |= (u8)((s8)imm);
                    enc.bitlen += field.bitlen;
                }
                else if (field.id == INST_IMM16)
                {
                    Operand imm_operand = {0};
                    if (instruction_data.src.operand_kind & OP_IMMEDIATE)
                    {
                        imm_operand = instruction_data.src;
                    }
                    else
                    {
                        imm_operand = instruction_data.dst;
                    }

                    s16 imm = imm_operand.immediate;

                    enc.encoding <<= 8;
                    enc.encoding |= (u8)(((u16)imm) & 0xff);
                    enc.encoding <<= 8;
                    enc.encoding |= (u8)((((u16)imm) >> 8) & 0xff);
                    enc.bitlen += field.bitlen;
                }
                else if (field.id == INST_SR)
                {
                    Operand sr_operand = {0};
                    if (instruction_data.src.operand_kind & OP_SEGREG)
                    {
                        sr_operand = instruction_data.src;
                    }
                    else
                    {
                        sr_operand = instruction_data.dst;
                    }

                    u8 sr = encode_sr(sr_operand);
                    enc.encoding <<= field.bitlen;
                    enc.encoding |= sr;
                    enc.bitlen += field.bitlen;
                }
            } // for (u64 j = 0; inst.inst_fields[j].id != INST_END; ++j)

            // Calculate the length in bit of the instruction just encoded
            // s8 msb_pos = get_msb_pos(enc.encoding);
            // assert(msb_pos != -1);
            // enc.bitlen = (u8)(msb_pos + 1);

            return enc;
        } // if (mnemonic_found && operand_dst_found && operand_src_found)
    }

    printf("Codegen failed\n");
    return (InstEncoding){0};
}
