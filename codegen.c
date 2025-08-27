#include "codegen.h"

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
    if (instruction_data.mnemonic == TOK_NULL)
    {
        return (InstEncoding){0};
    }

    InstEncoding enc = {0};

    for (u64 i = 0; i < ArrayCount(inst_table_keys); ++i)
    {
        bool mnemonic_found    = (inst_table_keys[i].mnemonic == instruction_data.mnemonic);

        bool operand_dst_found = false;
        if (instruction_data.dst.operand.operand_kind == OP_NONE)
        {
            operand_dst_found = (inst_table_keys[i].dst == OP_NONE);
        }
        else
        {
            operand_dst_found = (inst_table_keys[i].dst & instruction_data.dst.operand.operand_kind);
        }

        bool operand_src_found = false;
        if (instruction_data.src.operand.operand_kind == OP_NONE)
        {
            operand_src_found = (inst_table_keys[i].src == OP_NONE);
        }
        else
        {
            operand_src_found = (inst_table_keys[i].src & instruction_data.src.operand.operand_kind);
        }

        if (mnemonic_found && operand_dst_found && operand_src_found)
        {
            InstTableEntry inst = inst_table[i];
            for (u64 j = 0; inst.inst_fields[j].id != INST_END; ++j)
            {
                InstField field = inst.inst_fields[j];
                if (field.id == INST_OPCODE  || field.id == INST_ImplD || field.id == INST_ImplW  ||
                    field.id == INST_ImplMod || field.id == INST_ImplS || field.id == INST_ImplV)
                {
                    enc.encoding <<=  field.bitlen;
                    enc.encoding |= field.value;
                    enc.bitlen += field.bitlen;
                }
                else if (field.id == INST_REG)
                {
                    enc.encoding <<= field.bitlen;

                    Operand_ reg_operand = {0};
                    if (instruction_data.src.operand.operand_kind & (OP_REG8 | OP_REG16))
                    {
                        reg_operand = instruction_data.src;
                    }
                    else
                    {
                        reg_operand = instruction_data.dst;
                    }

                    enc.encoding |= reg_field_lut[reg_operand.operand.register_general];
                    enc.bitlen += field.bitlen;
                }
                else if (field.id == INST_MOD)
                {
                    enc.encoding <<= field.bitlen;

                    u8 mod;
                    if (instruction_data.src.operand.operand_kind & (OP_MEM | OP_DADDR))
                    {
                        mod = (u8)instruction_data.src.operand.effective_address.mod;
                    }
                    else if (instruction_data.dst.operand.operand_kind & (OP_MEM | OP_DADDR))
                    {
                        mod = (u8)instruction_data.dst.operand.effective_address.mod;
                    }
                    else
                    {
                        mod = 3; 
                    }

                    enc.encoding |= mod;
                    enc.bitlen += field.bitlen;
                }
                else if (field.id == INST_RM)
                {
                    enc.encoding <<= field.bitlen;

                    Operand_ mem_operand = {0};
                    Operand_ reg_operand = {0};

                    if (instruction_data.src.operand.operand_kind & (OP_MEM | OP_DADDR))
                    {
                        mem_operand = instruction_data.src;
                    }
                    else if (instruction_data.dst.operand.operand_kind & (OP_MEM | OP_DADDR))
                    {
                        mem_operand = instruction_data.dst;
                    }
                    else if (instruction_data.dst.operand.operand_kind & (OP_REG8 | OP_REG16))
                    {
                        reg_operand = instruction_data.dst;
                    }
                    else
                    {
                        assert(0);
                    }

                    if (mem_operand.operand.operand_kind)
                    {
                        enc.encoding |= mem_operand.operand.effective_address.rm;
                    }
                    else
                    {
                        enc.encoding |= reg_field_lut[reg_operand.operand.register_general]; 
                    }

                    enc.bitlen += field.bitlen;
                }
                else if (field.id == INST_DISP)
                {
                    Operand_ mem_operand = {0};

                    if (instruction_data.src.operand.operand_kind & (OP_MEM | OP_DADDR))
                    {
                        mem_operand = instruction_data.src;
                    }
                    else if (instruction_data.dst.operand.operand_kind & (OP_MEM | OP_DADDR))
                    {
                        mem_operand = instruction_data.dst;
                    }

                    // TODO: I kinda hate this, maybe I prefer a flag that tells me
                    //       if I actually need to encode a displacement (even if it
                    //       is one byte with 0)
                    if (mem_operand.operand.effective_address.mod)
                    {
                        s16 displacement = mem_operand.operand.effective_address.displacement;

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
                        if (mem_operand.operand.effective_address.direct_address) // TODO: should I handle the case [0]? there should be a flag that says need_direct_address 
                        {
                            s16 daddr = mem_operand.operand.effective_address.direct_address;

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
                    Operand_ daddr_operand = {0};
                    if (instruction_data.src.operand.operand_kind & OP_DADDR)
                    {
                        daddr_operand = instruction_data.src;
                    }
                    else
                    {
                        daddr_operand = instruction_data.dst;
                    }

                    s16 daddr = daddr_operand.operand.effective_address.direct_address;

                    enc.encoding <<= 8;
                    enc.encoding |= (u8)(((u16)daddr) & 0xff);
                    enc.encoding <<= 8;
                    enc.encoding |= (u8)((((u16)daddr) >> 8) & 0xff);
                    enc.bitlen += 16;
                }
                else if (field.id == INST_IMM8)
                {
                    Operand_ imm_operand = {0};
                    if (instruction_data.src.operand.operand_kind & OP_IMMEDIATE)
                    {
                        imm_operand = instruction_data.src;
                    }
                    else
                    {
                        imm_operand = instruction_data.dst;
                    }

                    s16 imm = imm_operand.operand.immediate;

                    enc.encoding <<= 8;
                    enc.encoding |= (u8)((s8)imm);
                    enc.bitlen += field.bitlen;
                }
                else if (field.id == INST_IMM16)
                {
                    Operand_ imm_operand = {0};
                    if (instruction_data.src.operand.operand_kind & OP_IMMEDIATE)
                    {
                        imm_operand = instruction_data.src;
                    }
                    else
                    {
                        imm_operand = instruction_data.dst;
                    }

                    s16 imm = imm_operand.operand.immediate;

                    if (instruction_data.mnemonic == TOK_JMP)
                    {
                        // TODO: this is because I always encode the jump instruction in 3 bytes.
                        //       The code would break if this is not the case.
                        imm -= (g_instruction_pointer + 3);
                    }

                    enc.encoding <<= 8;
                    enc.encoding |= (u8)(((u16)imm) & 0xff);
                    enc.encoding <<= 8;
                    enc.encoding |= (u8)((((u16)imm) >> 8) & 0xff);
                    enc.bitlen += field.bitlen;
                }
                else if (field.id == INST_SR)
                {
                    Operand_ sr_operand = {0};
                    if (instruction_data.src.operand.operand_kind & OP_SEGREG)
                    {
                        sr_operand = instruction_data.src;
                    }
                    else
                    {
                        sr_operand = instruction_data.dst;
                    }

                    u8 sr = encode_sr(sr_operand.operand);
                    enc.encoding <<= field.bitlen;
                    enc.encoding |= sr;
                    enc.bitlen += field.bitlen;
                }
                else if (field.id == INST_DATA8)
                {
                    Operand_ data8_operand = {0};
                    if (instruction_data.src.operand.operand_kind & OP_IMMEDIATE)
                    {
                        data8_operand = instruction_data.src;
                    }
                    else
                    {
                        data8_operand = instruction_data.dst;
                    }

                    u8 data8 = (u8)data8_operand.operand.immediate;
                    enc.encoding <<= field.bitlen;
                    enc.encoding |= data8;
                    enc.bitlen += field.bitlen;
                }
                else if (field.id == INST_W)
                {
                    u8 w = 0;

                    PrefixSize prefix_src = instruction_data.src.prefix_operand.prefix_size;
                    PrefixSize prefix_dst = instruction_data.dst.prefix_operand.prefix_size;
                    if ((prefix_src & PREFIX_SIZE_BYTE) || (prefix_dst & PREFIX_SIZE_BYTE))
                    {
                        w = 0;
                    }
                    else if ((prefix_src & PREFIX_SIZE_WORD) || (prefix_dst & PREFIX_SIZE_WORD))
                    {
                        w = 1;
                    }

                    enc.encoding <<= field.bitlen;
                    enc.encoding |= w;
                    enc.bitlen += field.bitlen;
                }
                else if (field.id == INST_S)
                {
                    u8 s = 0;

                    PrefixSize prefix_src = instruction_data.src.prefix_operand.prefix_size;
                    PrefixSize prefix_dst = instruction_data.dst.prefix_operand.prefix_size;
                    if ((prefix_src & PREFIX_SIZE_BYTE) || (prefix_dst & PREFIX_SIZE_BYTE))
                    {
                        s = 0;
                    }
                    else if ((prefix_src & PREFIX_SIZE_WORD) || (prefix_dst & PREFIX_SIZE_WORD))
                    {
                        s = 1;
                    }

                    enc.encoding <<= field.bitlen;
                    enc.encoding |= s;
                    enc.bitlen += field.bitlen;
                }
                else if (field.id == INST_LBL)
                {
                    String labelname = instruction_data.dst.operand.label;

                    u64 idx = maplabel_find(&g_map_label, labelname);
                    if (idx == 0)
                    {
                        AddrToPatch addr = {
                            .inst_pointer = g_instruction_pointer + 1,
                            .labelname    = labelname,
                            .bytelen      = 2,
                        };

                        list_addrtopatch_add(&list_addrtopatch, addr);

                        enc.encoding <<= 16;
                        enc.bitlen += 16;
                    }
                    else
                    {
                        Label label = g_map_label.collision_array[idx].label;

                        s16 diff = (s16)label.pos - (s16)(g_instruction_pointer + 3);

                        enc.encoding <<= 8;
                        enc.encoding |= (u8)(((u16)diff) & 0xff);
                        enc.encoding <<= 8;
                        enc.encoding |= (u8)((((u16)diff) >> 8) & 0xff);
                        enc.bitlen += 16;
                    }
                }
                else if (field.id == INST_LBL8)
                {
                    String labelname = instruction_data.dst.operand.label;

                    u64 idx = maplabel_find(&g_map_label, labelname);
                    if (idx == 0)
                    {
                        AddrToPatch addr = {
                            .inst_pointer = g_instruction_pointer + 1,
                            .labelname    = labelname,
                            .bytelen      = 1
                        };

                        list_addrtopatch_add(&list_addrtopatch, addr);

                        enc.encoding <<= 8;
                        enc.bitlen += 8;
                    }
                    else
                    {
                        Label label = g_map_label.collision_array[idx].label;

                        s8 diff = (s16)label.pos - (s16)(g_instruction_pointer + 2);

                        enc.encoding <<= 8;
                        enc.encoding |= (u8)(((u16)diff) & 0xff);
                        enc.bitlen += 8;
                    }
                }
                else if (field.id == INST_IP)
                {
                    Pointer pointer = instruction_data.dst.operand.pointer;    
                    s16 offset      = pointer.offset;

                    enc.encoding <<= 8;
                    enc.encoding |= (u8)((u16)offset & 0xff);
                    enc.encoding <<= 8;
                    enc.encoding |= (u8)((((u16)offset) >> 8) & 0xff);
                    enc.bitlen += 16;
                }
                else if (field.id == INST_CODE_SEG)
                {
                    Pointer pointer = instruction_data.dst.operand.pointer;    
                    s16 selector    = pointer.selector;

                    enc.encoding <<= 8;
                    enc.encoding |= (u8)((u16)selector & 0xff);
                    enc.encoding <<= 8;
                    enc.encoding |= (u8)((((u16)selector) >> 8) & 0xff);
                    enc.bitlen += 16;
                }
            } // for (u64 j = 0; inst.inst_fields[j].id != INST_END; ++j)

            return enc;
        } // if (mnemonic_found && operand_dst_found && operand_src_found)
    }

    printf("Codegen failed\n");
    return (InstEncoding){0};
}
