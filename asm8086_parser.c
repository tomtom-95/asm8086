#include <stddef.h>

#include "asm8086_parser.h"
#include "asm8086.h"

void parser_add_terminal(struct ParserNode **pp_node,
                         struct LexTok     **pp_tok)
{
    (**pp_node).id = NODE_TERMINAL;
    (**pp_node).p_tok = *pp_tok;
    ++(*pp_node);
    ++(*pp_tok);
}

void parser_backtrack(struct ParserNode **pp_node,
                      struct ParserNode  *p_reset_node,
                      struct LexTok     **pp_tok,
                      struct LexTok      *p_reset_tok)
{
    *pp_node = p_reset_node;
    *pp_tok = p_reset_tok;
}

int parser_line_(struct ParserNode **pp_node, struct LexTok **pp_tok)
{
    struct ParserNode *p_reset_node = *pp_node;
    struct LexTok     *p_reset_tok = *pp_tok;

    (*((*pp_node)++)).id = NODE_LINE_;

    if (parser_line(pp_node, pp_tok) == 0)
    {
        if ((**pp_tok).id1 == TOK1_EOL)
        {
            parser_add_terminal(pp_node, pp_tok);
            return 0;
        }
    }

    parser_backtrack(pp_node, p_reset_node, pp_tok, p_reset_tok);
    return 1;
}

int parser_line(struct ParserNode **pp_node, struct LexTok **pp_tok)
{
    struct ParserNode *p_reset_node = *pp_node;
    struct LexTok     *p_reset_tok = *pp_tok;

    (*((*pp_node)++)).id = NODE_LINE;

    if ((**pp_tok).id1 == TOK1_BITS)
    {
        parser_add_terminal(pp_node, pp_tok);
        if ((**pp_tok).id1 == TOK1_NUM)
        {
            parser_add_terminal(pp_node, pp_tok);
            return 0;
        }
    }

    parser_backtrack(pp_node, p_reset_node + 1, pp_tok, p_reset_tok);

    if (parser_lbl(pp_node, pp_tok) == 0)
        return 0;

    parser_backtrack(pp_node, p_reset_node + 1, pp_tok, p_reset_tok);

    if (parser_inst(pp_node, pp_tok) == 0)
        return 0;

    parser_backtrack(pp_node, p_reset_node, pp_tok, p_reset_tok);
    return 1;
}

int parser_lbl(struct ParserNode **pp_node, struct LexTok **pp_tok)
{
    struct ParserNode *p_reset_node = *pp_node;
    struct LexTok     *p_reset_tok = *pp_tok;

    (*((*pp_node)++)).id = NODE_LBL;

    if ((**pp_tok).id1 == TOK1_LABEL)
    {
        parser_add_terminal(pp_node, pp_tok);
        if ((**pp_tok).id1 == TOK1_COLON)
        {
            parser_add_terminal(pp_node, pp_tok);
            return 0;            
        }
    }

    parser_backtrack(pp_node, p_reset_node, pp_tok, p_reset_tok);
    return 1;
}

int parser_inst(struct ParserNode **pp_node, struct LexTok **pp_tok)
{
    struct ParserNode *p_reset_node = *pp_node;
    struct LexTok *p_reset_tok = *pp_tok;

    (*((*pp_node)++)).id = NODE_INST;

    if (parser_mnemonic_(pp_node, pp_tok) == 0)
    {
        if (parser_operand_(pp_node, pp_tok) == 0)
        {
            if ((**pp_tok).id1 == TOK1_COMMA)
            {
                parser_add_terminal(pp_node, pp_tok);
                if (parser_operand_(pp_node, pp_tok) == 0)
                {
                    inst_data.operand_cnt = 2;
                    return 0;
                }
            }
        }
    }

    parser_backtrack(pp_node, p_reset_node + 1, pp_tok, p_reset_tok);

    if (parser_mnemonic_(pp_node, pp_tok) == 0)
    {
        if (parser_operand_(pp_node, pp_tok) == 0)
        {
            inst_data.operand_cnt = 1;
            return 0;
        }
    }

    parser_backtrack(pp_node, p_reset_node + 1, pp_tok, p_reset_tok);

    if (parser_mnemonic_(pp_node, pp_tok) == 0)
        return 0;

    parser_backtrack(pp_node, p_reset_node, pp_tok, p_reset_tok);
    return 1;
}

int parser_mnemonic_(struct ParserNode **pp_node, struct LexTok **pp_tok)
{
    struct ParserNode *p_reset_node = *pp_node;
    struct LexTok *p_reset_tok = *pp_tok;

    (*((*pp_node)++)).id = NODE_MNEMONIC_;

    while (parser_prefix(pp_node, pp_tok) == 0);

    if (parser_mnemonic(pp_node, pp_tok) == 0)
        return 0;

    parser_backtrack(pp_node, p_reset_node, pp_tok, p_reset_tok);
    return 1;
}

int parser_operand_(struct ParserNode **pp_node, struct LexTok **pp_tok)
{
    struct ParserNode *p_reset_node = *pp_node;
    struct LexTok *p_reset_tok = *pp_tok;

    (*((*pp_node)++)).id = NODE_OPERAND_;

    while (parser_opr_prefix(pp_node, pp_tok) == 0);

    if (parser_operand(pp_node, pp_tok) == 0)
        return 0;

    parser_backtrack(pp_node, p_reset_node, pp_tok, p_reset_tok);
    return 1;
}

int parser_opr_prefix(struct ParserNode **pp_node, struct LexTok **pp_tok)
{
    struct ParserNode *p_reset_node = *pp_node;
    struct LexTok *p_reset_tok = *pp_tok;

    (*((*pp_node)++)).id = NODE_OPR_PREFIX;

    if (parser_opr_size(pp_node, pp_tok) == 0)
    {
        return 0;
    }
    else if (parser_seg_ovr(pp_node, pp_tok) == 0)
    {
        inst_data.seg_ovr = (*(*pp_tok - 2)).id1;
        return 0;
    }

    parser_backtrack(pp_node, p_reset_node, pp_tok, p_reset_tok);
    return 1;
}

int parser_opr_size(struct ParserNode **pp_node, struct LexTok **pp_tok)
{
    struct ParserNode *p_reset_node = *pp_node;
    struct LexTok *p_reset_tok = *pp_tok;

    (*((*pp_node)++)).id = NODE_OPR_SIZE;

    if ((**pp_tok).id0 == TOK0_OPR_SIZE)
    {
        if ((**pp_tok).id1 == TOK1_BYTE)
            inst_data.w = 0;
        else
            inst_data.w = 1;

        return 0;
    }

    parser_backtrack(pp_node, p_reset_node, pp_tok, p_reset_tok);
    return 1;
}

int parser_seg_ovr(struct ParserNode **pp_node, struct LexTok **pp_tok)
{
    struct ParserNode *p_reset_node = *pp_node;
    struct LexTok *p_reset_tok = *pp_tok;

    (*((*pp_node)++)).id = NODE_SEG_OVR;

    if (parser_reg_seg(pp_node, pp_tok) == 0)
    {
        if ((**pp_tok).id1 == TOK1_COLON)
        {
            parser_add_terminal(pp_node, pp_tok);
            return 0;
        }
    }

    parser_backtrack(pp_node, p_reset_node, pp_tok, p_reset_tok);
    return 1;
}

int parser_operand(struct ParserNode **pp_node, struct LexTok **pp_tok)
{
    struct ParserNode *p_reset_node = *pp_node;
    struct LexTok *p_reset_tok = *pp_tok;

    (*((*pp_node)++)).id = NODE_OPERAND;

    if (parser_reg_gen(pp_node, pp_tok) == 0)
    {
        bool reg_gen_is_dst = false;
        for (struct LexTok *p_tok = *pp_tok; p_tok -> id0 != TOK0_EOL; ++p_tok)
        {
            if (p_tok -> id0 == TOK0_COMMA)
            {
                inst_data.p_reg_gen_dst = *pp_tok - 1;
                reg_gen_is_dst = true;
                break;
            }
        }

        if (!reg_gen_is_dst)
            inst_data.p_reg_gen_src = *pp_tok - 1;

        return 0;
    }
    
    if (parser_eaddr__(pp_node, pp_tok) == 0)
        return 0;
    
    if (parser_reg_seg(pp_node, pp_tok) == 0)
    {
        inst_data.w       = 1;
        inst_data.reg_seg = (*(*pp_tok - 1)).id1;
        return 0;
    }
    if (parser_imm(pp_node, pp_tok) == 0)
    {
        return 0;
    }
    
    parser_backtrack(pp_node, p_reset_node, pp_tok, p_reset_tok);
    return 1;
}

int parser_reg_gen(struct ParserNode **pp_node, struct LexTok **pp_tok)
{
    struct ParserNode *p_reset_node = *pp_node;
    struct LexTok *p_reset_tok = *pp_tok;

    (*((*pp_node)++)).id = NODE_REG_GEN;

    if ((**pp_tok).id0 == TOK0_REG_GEN)
    {
        for (size_t i = 0; i < ArrayCount(id_reg_gen); ++i)
        {
            if ((**pp_tok).id1 == id_reg_gen[i])
                inst_data.w = wid_reg_gen[i];
        }

        parser_add_terminal(pp_node, pp_tok);
        return 0;
    }

    parser_backtrack(pp_node, p_reset_node, pp_tok, p_reset_tok);
    return 1;
}

int parser_reg_seg(struct ParserNode **pp_node, struct LexTok **pp_tok)
{
    struct ParserNode *p_reset_node = *pp_node;
    struct LexTok     *p_reset_tok  = *pp_tok;

    (*((*pp_node)++)).id = NODE_REG_SEG;

    if ((**pp_tok).id0 == TOK0_REG_SEG)
    {
        parser_add_terminal(pp_node, pp_tok);
        return 0;
    }

    parser_backtrack(pp_node, p_reset_node, pp_tok, p_reset_tok);
    return 1;
}

int parser_imm(struct ParserNode **pp_node, struct LexTok **pp_tok)
{
    struct ParserNode *p_reset_node = *pp_node;
    struct LexTok     *p_reset_tok = *pp_tok;

    inst_data.p_imm = *pp_node; 
    (*((*pp_node)++)).id = NODE_IMM;

    parser_opr_math(pp_node, pp_tok);
    if ((**pp_tok).id1 == TOK1_NUM || (**pp_tok).id1 == TOK1_NUM_ZERO)
    {
        inst_data.imm = (**pp_tok).p_num -> value;
        parser_add_terminal(pp_node, pp_tok);
        return 0;
    }

    inst_data.p_imm = NULL; 
    parser_backtrack(pp_node, p_reset_node, pp_tok, p_reset_tok);
    return 1;
}

int parser_opr_math(struct ParserNode **pp_node, struct LexTok **pp_tok)
{
    struct ParserNode *p_reset_node = *pp_node;
    struct LexTok     *p_reset_tok  = *pp_tok;

    (*((*pp_node)++)).id = NODE_OPR_MATH;

    switch((**pp_tok).id1)
    {
    case TOK1_PLUS:
    case TOK1_MINUS:
        parser_add_terminal(pp_node, pp_tok);
        return 0;
    default:
        parser_backtrack(pp_node, p_reset_node, pp_tok, p_reset_tok);
        return 1;
    }
}

// NOTE(I find the prefix always at the start of the instruction)
// I can use the node in the parser to retrieve info about
// the prefixes (just read nodes until you find a mnemonic node)
int parser_prefix(struct ParserNode **pp_node, struct LexTok **pp_tok)
{
    struct ParserNode *p_reset_node = *pp_node;
    struct LexTok     *p_reset_tok  = *pp_tok;

    (*((*pp_node)++)).id = NODE_PREFIX;

    if ((**pp_tok).id0 == TOK0_PREFIX)
    {
        parser_add_terminal(pp_node, pp_tok);
        return 0;
    }

    parser_backtrack(pp_node, p_reset_node, pp_tok, p_reset_tok);
    return 1;
}

int parser_mnemonic(struct ParserNode **pp_node, struct LexTok **pp_tok)
{
    struct ParserNode *p_reset_node = *pp_node;
    struct LexTok     *p_reset_tok  = *pp_tok;

    (*((*pp_node)++)).id = NODE_MNEMONIC;

    if ((**pp_tok).id0 == TOK0_MNEMONIC)
    {
        inst_data.mnemonic = (**pp_tok).id1;
        parser_add_terminal(pp_node, pp_tok);
        return 0;
    }

    parser_backtrack(pp_node, p_reset_node, pp_tok, p_reset_tok);
    return 1;
}

int parser_eaddr__(struct ParserNode **pp_node, struct LexTok **pp_tok)
{
    struct ParserNode *p_reset_node = *pp_node;
    struct LexTok *p_reset_tok      = *pp_tok;

    inst_data.p_eaddr__  = *pp_node;
    (*((*pp_node)++)).id = NODE_EADDR__;

    if ((**pp_tok).id1 == TOK1_LSQUARE_BR)
    {
        parser_add_terminal(pp_node, pp_tok);
        if (parser_eaddr_(pp_node, pp_tok) == 0)
        {
            if ((**pp_tok).id1 == TOK1_RSQUARE_BR)
            {
                parser_add_terminal(pp_node, pp_tok);
                return 0;
            }
        }
    }

    inst_data.p_eaddr__ = NULL;
    parser_backtrack(pp_node, p_reset_node, pp_tok, p_reset_tok);
    return 1;
}

int parser_eaddr_(struct ParserNode **pp_node, struct LexTok **pp_tok)
{
    struct ParserNode *p_reset_node = *pp_node;
    struct LexTok *p_reset_tok      = *pp_tok;

    (*((*pp_node)++)).id = NODE_EADDR_;

    if ((**pp_tok).id1 == TOK1_PLUS)
        parser_add_terminal(pp_node, pp_tok);

    if (parser_eaddr(pp_node, pp_tok) == 0)
        return 0;

    parser_backtrack(pp_node, p_reset_node, pp_tok, p_reset_tok);
    return 1;
}

int parser_eaddr(struct ParserNode **pp_node, struct LexTok **pp_tok)
{
    struct ParserNode *p_reset_node = *pp_node;
    struct LexTok *p_reset_tok      = *pp_tok;

    (*((*pp_node)++)).id = NODE_EADDR;

    if (parser_reg_base(pp_node, pp_tok) == 0)
    {
        if ((**pp_tok).id1 == TOK1_PLUS)
        {
            parser_add_terminal(pp_node, pp_tok);
            if (parser_reg_idx(pp_node, pp_tok) == 0)
            {
                if (parser_signed_num(pp_node, pp_tok) == 0)
                {
                    inst_data.disp = (*(*pp_tok - 1)).p_num -> value;
                    return 0;
                }
            }
        }
    }

    parser_backtrack(pp_node, p_reset_node + 1, pp_tok, p_reset_tok);

    if (parser_reg_base(pp_node, pp_tok) == 0)
    {
        if (parser_signed_num(pp_node, pp_tok) == 0)
        {
            inst_data.disp = (*(*pp_tok - 1)).p_num -> value;
            return 0;
        }

    }

    parser_backtrack(pp_node, p_reset_node + 1, pp_tok, p_reset_tok);

    if (parser_reg_idx(pp_node, pp_tok) == 0)
    {
        if (parser_signed_num(pp_node, pp_tok) == 0)
        {
            inst_data.disp = (*(*pp_tok - 1)).p_num -> value;
            return 0;
        }
    }

    parser_backtrack(pp_node, p_reset_node + 1, pp_tok, p_reset_tok);

    if (parser_reg_base(pp_node, pp_tok) == 0)
    {
        if ((**pp_tok).id1 == TOK1_PLUS)
        {
            parser_add_terminal(pp_node, pp_tok);
            if (parser_reg_idx(pp_node, pp_tok) == 0)
            {
                parser_signed_zero(pp_node, pp_tok);
                return 0;
            }
        }
    }

    parser_backtrack(pp_node, p_reset_node + 1, pp_tok, p_reset_tok);

    if ((**pp_tok).id1 == TOK1_BX)
    {
        inst_data.reg_base = TOK1_BX;
        parser_add_terminal(pp_node, pp_tok);
        parser_signed_zero(pp_node, pp_tok); 
        return 0;
    }

    if (parser_reg_idx(pp_node, pp_tok) == 0)
    {
        parser_signed_zero(pp_node, pp_tok); 
        return 0;
    }

    if (parser_direct_addr(pp_node, pp_tok) == 0)
        return 0;

    parser_backtrack(pp_node, p_reset_node, pp_tok, p_reset_tok);
    return 1;
}

int parser_direct_addr(struct ParserNode **pp_node, struct LexTok **pp_tok)
{
    struct ParserNode *p_reset_node = *pp_node;
    struct LexTok     *p_reset_tok = *pp_tok;

    inst_data.p_daddr = *pp_node;
    (*((*pp_node)++)).id = NODE_DIRECT_ADDR;

    parser_opr_math(pp_node, pp_tok);
    if ((**pp_tok).id1 == TOK1_NUM || (**pp_tok).id1 == TOK1_NUM_ZERO)
    {
        inst_data.daddr = (**pp_tok).p_num -> value;
        parser_add_terminal(pp_node, pp_tok);
        return 0;
    }

    inst_data.p_daddr = NULL;
    parser_backtrack(pp_node, p_reset_node, pp_tok, p_reset_tok);
    return 1;
}

int parser_signed_num(struct ParserNode **pp_node, struct LexTok **pp_tok)
{
    struct ParserNode *p_reset_node = *pp_node;
    struct LexTok *p_reset_tok = *pp_tok;

    (*((*pp_node)++)).id = NODE_SIGNED_NUM;

    if (parser_opr_math(pp_node, pp_tok) == 0)
    {
        if ((**pp_tok).id1 == TOK1_NUM) 
        {
            parser_add_terminal(pp_node, pp_tok);
            return 0;
        }
    }

    parser_backtrack(pp_node, p_reset_node, pp_tok, p_reset_tok);
    return 1;
}

int parser_signed_zero(struct ParserNode **pp_node, struct LexTok **pp_tok)
{
    struct ParserNode *p_reset_node = *pp_node;
    struct LexTok *p_reset_tok = *pp_tok;

    (*((*pp_node)++)).id = NODE_SIGNED_ZERO;

    if (parser_opr_math(pp_node, pp_tok) == 0)
    {
        if ((**pp_tok).id1 == TOK1_NUM_ZERO) 
        {
            parser_add_terminal(pp_node, pp_tok);
            return 0;
        }
    }

    parser_backtrack(pp_node, p_reset_node, pp_tok, p_reset_tok);
    return 1;
}

int parser_reg_base(struct ParserNode **pp_node, struct LexTok **pp_tok)
{
    struct ParserNode *p_reset_node = *pp_node;
    struct LexTok *p_reset_tok = *pp_tok;

    (*((*pp_node)++)).id = NODE_REG_BASE;

    if ((**pp_tok).id1 == TOK1_BX || (**pp_tok).id1 == TOK1_BP)
    {
        inst_data.reg_base = (**pp_tok).id1;
        parser_add_terminal(pp_node, pp_tok);
        return 0;
    }

    parser_backtrack(pp_node, p_reset_node, pp_tok, p_reset_tok);
    return 1;
}

int parser_reg_idx(struct ParserNode **pp_node, struct LexTok **pp_tok)
{
    struct ParserNode *p_reset_node = *pp_node;
    struct LexTok *p_reset_tok = *pp_tok;

    (*((*pp_node)++)).id = NODE_REG_IDX;

    if ((**pp_tok).id1 == TOK1_SI || (**pp_tok).id1 == TOK1_DI)
    {
        inst_data.reg_idx = (**pp_tok).id1;
        parser_add_terminal(pp_node, pp_tok);
        return 0;
    }

    parser_backtrack(pp_node, p_reset_node, pp_tok, p_reset_tok);
    return 1;
}
