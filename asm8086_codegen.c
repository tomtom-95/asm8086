#include <stdlib.h>
#include <stdio.h>

#include "asm8086.h"
#include "asm8086_codegen.h"

int set_p_comma(struct LexTok *p_tok_start)
{
    for (struct LexTok *p_tok = p_tok_start; p_tok -> id0 != TOK0_EOL; ++p_tok)
    {
        if (p_tok -> id0 == TOK0_COMMA)
        {
            inst_data.p_comma = p_tok;
            return 0;
        }
    }

    return 1;
}

int set_d(struct LexTok *p_tok_start)
{
    struct LexTok *p_tok_reg_gen_dst = NULL;
    struct LexTok *p_tok_reg_gen_src = NULL;
    for (struct LexTok *p_tok = p_tok_start; p_tok -> id0 != TOK0_EOL; ++p_tok)
    {
        if (p_tok -> id0 == TOK0_REG_GEN)
        {
            if (inst_data.p_comma - p_tok > 0)
                p_tok_reg_gen_dst = p_tok;
            else
                p_tok_reg_gen_src = p_tok;
        }
    }

    if (p_tok_reg_gen_dst != NULL && p_tok_reg_gen_src != NULL)
        inst_data.d = 0;
    else if (p_tok_reg_gen_dst != NULL && p_tok_reg_gen_src == NULL)
        inst_data.d = 1;
    else if (p_tok_reg_gen_dst == NULL && p_tok_reg_gen_src != NULL)
        inst_data.d = 0;
    else if (p_tok_reg_gen_dst == NULL && p_tok_reg_gen_src == NULL)
    {
        for (struct LexTok *p_tok = p_tok_start; p_tok -> id0 != TOK0_EOL; ++p_tok)
        {
            if (p_tok -> id0 == TOK0_LSQUARE_BR)
            {
                if (inst_data.p_comma - p_tok > 0)
                    inst_data.d = 0;
                else
                    inst_data.d = 1;
            }
        }
    }

    return 0;
}

int set_mod(void)
{
    if (inst_data.p_eaddr__ == NULL)
    {
        inst_data.mod = 0b11;
        return 0;
    }

    if (inst_data.p_daddr != NULL || inst_data.disp == 0)
    {
        inst_data.mod = 0b00;
        return 0;
    }

    long msb_disp = inst_data.disp >> 8;
    if (msb_disp == 0x00 || msb_disp == 0xff)
        inst_data.mod = 0b01;
    else
        inst_data.mod = 0b10;

    return 0;
}

int set_rm(struct LexTok *p_tok_start)
{
    if (inst_data.p_eaddr__ != NULL && inst_data.p_daddr != NULL)
    {
        inst_data.rm = 0b110;
        return 0;
    }

    for (size_t i = 0; i < ArrayCount(eaddr); ++i)
    {
        if (inst_data.reg_base == eaddr[i].reg_base && inst_data.reg_idx == eaddr[i].reg_idx)
        {
            inst_data.rm = enc_rm_eaddr[i];
            return 0;
        }
    }

    struct LexTok *p_tok_reg_gen = get_reg_gen(p_tok_start, INST_RM);
    if (p_tok_reg_gen == NULL)
        return 1;

    for (size_t i = 0; i < ArrayCount(id_reg_gen); ++i)
    {
        if (p_tok_reg_gen -> id1 == id_reg_gen[i])
        {
            inst_data.rm = enc_reg_gen[i];
            return 0;
        }
    }

    return 1;
}

struct LexTok *get_reg_gen(struct LexTok    *p_tok_start,
                           enum InstFieldId  field_id)
{
    struct LexTok *p_tok_reg_gen = NULL;

    if (inst_data.operand_cnt == 2)
    {
        if ((inst_data.d == 0 && field_id == INST_REG) ||
            (inst_data.d == 1 && field_id == INST_RM))
        {
            for (struct LexTok *p_tok = inst_data.p_comma; p_tok -> id0 != TOK0_EOL; ++p_tok)
            {
                if (p_tok -> id0 == TOK0_REG_GEN)
                {
                    p_tok_reg_gen = p_tok;
                    break;
                }
            }
        }
        else if ((inst_data.d == 1 && field_id == INST_REG) ||
                 (inst_data.d == 0 && field_id == INST_RM))
        {
            for (struct LexTok *p_tok = p_tok_start; p_tok != inst_data.p_comma; ++p_tok)
            {
                if (p_tok -> id0 == TOK0_REG_GEN)
                {
                    p_tok_reg_gen = p_tok;
                    break;
                }
            }
        }
    }
    else if (inst_data.operand_cnt == 1)
    {
        printf("TODO: one operand case\n");
    }

    return p_tok_reg_gen;
}

int codegen(struct ParserNode *p_node_start,
            struct LexTok     *p_tok_start,
            struct InstEnc    *encoding)
{
    u64 *inst_enc_stack = malloc(LEN_MAX_INST_STACK * sizeof(*inst_enc_stack));
    u64 *inst_enc_sp    = inst_enc_stack;

    if (inst_data.operand_cnt == 2)
        assert(set_p_comma(p_tok_start) == 0);
        set_d(p_tok_start);

    set_mod();
    if (set_rm(p_tok_start) != 0)
        return 1;

    size_t inst_idx = 0;
    while (inst_data.mnemonic != inst_table[inst_idx].id_mnemonic)
        ++inst_idx;

    while (inst_data.mnemonic == inst_table[inst_idx].id_mnemonic)
    {
        u64 inst_enc = 0;
        if (instgen(inst_table[inst_idx++], p_node_start, p_tok_start, &inst_enc) != 0)
            continue;

        *inst_enc_sp++ = inst_enc;
    }

    if (inst_enc_sp == inst_enc_stack)
        return free(inst_enc_stack), 1;

    for (u64 *p_enc = inst_enc_stack; p_enc != inst_enc_sp; ++p_enc)
        printf("%llx\n", *p_enc);

    encoding -> enc = *inst_enc_stack;
    encoding -> len = LEN_MAX_INST_BYTES;
    for (u64 *p_enc = inst_enc_stack; p_enc != inst_enc_sp; ++p_enc)
    {
        u8 inst_len = 1;
        while ((*p_enc >> (inst_len * 8)) != 0)
            ++inst_len;
        
        if (inst_len < encoding -> len)
        {
            encoding -> enc = *p_enc;
            encoding -> len = inst_len;
        }
    }

    printf("chosen encoding = %llx\n", encoding -> enc);

    free(inst_enc_stack);

    return 0;
}

int instgen(struct Instruction   inst,
            struct ParserNode   *p_node_start,
            struct LexTok       *p_tok_start,
            u64                 *p_inst_enc)
{
    for (size_t idx = 0; inst.fields[idx].id != INST_END; ++idx)
    {
        u8 size  = inst.fields[idx].size;
        u8 value = inst.fields[idx].value; 

        switch (inst.fields[idx].id)
        {
        case INST_OPCODE1:
        case INST_OPCODE2:
        {
            *p_inst_enc = (*p_inst_enc << size) | value; 
            break;
        }
        case INST_D:
        {
            *p_inst_enc = (*p_inst_enc << size) | inst_data.d;
            break;
        }
        case INST_ImplD:
        {
            if (inst_data.d != value)
                return 1;

            break;
        }
        case INST_W:
        {
            *p_inst_enc = (*p_inst_enc << size) | inst_data.w;
            break;
        }
        case INST_MOD:
        {
            *p_inst_enc = (*p_inst_enc << size) | inst_data.mod;
            break;
        }
        case INST_REG:
        {
            struct LexTok *p_tok_reg_gen = get_reg_gen(p_tok_start, INST_REG);
            if (p_tok_reg_gen == NULL)
                return 1;

            for (size_t i = 0; i < ArrayCount(id_reg_gen); ++i)
            {
                if (p_tok_reg_gen -> id1 == id_reg_gen[i])
                {
                    *p_inst_enc = (*p_inst_enc << size) | enc_reg_gen[i];
                    break;
                }
            }

            break;
        }
        case INST_ImplReg:
        {
            struct LexTok *p_tok_reg_gen = get_reg_gen(p_tok_start, INST_ImplReg);
            if (p_tok_reg_gen == NULL)
                return 1;
            
            for (size_t i = 0; i < ArrayCount(id_reg_gen); ++i)
            {
                if (p_tok_reg_gen -> id1 == id_reg_gen[i])
                {
                    if (enc_reg_gen[i] != value)
                        return 1;
                }
            }

            break;
        }
        case INST_RM:
        {
            *p_inst_enc = (*p_inst_enc << size) | inst_data.rm;
            break;
        }
        case INST_SR:
        {
            if (inst_data.reg_seg == TOK1_NONE)
                return 1;

            for (size_t i = 0; i < ArrayCount(id_reg_seg); ++i)
            {
                if (inst_data.reg_seg == id_reg_seg[i])
                {
                    *p_inst_enc = (*p_inst_enc << size) | enc_reg_seg[i];
                    break;
                }
            }

            break;
        }
        case INST_DISP:
        {
            switch (inst_data.mod)
            {
            case 0b00:
            {
                u64 daddr_little_endian = ((u64)inst_data.daddr) >> 8 | ((inst_data.daddr << 8) & 0xff00);
                *p_inst_enc = (*p_inst_enc << 16) | daddr_little_endian;
                break;
            }
            case 0b01:
            {
                *p_inst_enc = (*p_inst_enc << 8) | (inst_data.disp & 0xff);
                break;
            }
            case 0b10:
            {
                u64 disp_little_endian = ((u64)inst_data.disp) >> 8 | ((inst_data.disp << 8) & 0xff00);
                *p_inst_enc = (*p_inst_enc << 16) | disp_little_endian;
                break;
            }
            }

            break;
        }
        case INST_IMM:
        {
            if (inst_data.p_imm == NULL)
                return 1;

            if (inst_data.w == 0)
            {
                if (inst_data.imm >> 8 == 0x00 || inst_data.imm >> 8 == 0xff)
                    *p_inst_enc = (*p_inst_enc << 8) | (inst_data.imm & 0xff);
                else
                    return 1;
            }
            else
            {
                u64 imm_little_endian = (u64)inst_data.imm >> 8 | ((inst_data.imm << 8) & 0xff00);
                *p_inst_enc = (*p_inst_enc << 16) | imm_little_endian;
            }

            break;
        }
        case INST_ADDR:
        {
            if (inst_data.p_eaddr__ == NULL)
                return 1;

            u64 eaddr_little_endian = (((u64)inst_data.daddr) >> 8) | ((inst_data.daddr << 8) & 0xff00);
            *p_inst_enc = (*p_inst_enc << 16) | eaddr_little_endian;
            break;
        }
        case INST_END:
        {
            break;
        }
        }
    }

    return 0;
}
