#include "utils.c"
#include "tokenizer.h"
#include "parser.h"

internal s32
is_base(TokenKind r)
{
    return r==TOK_BX || r==TOK_BP;
}

internal s32
is_index(TokenKind r)
{
    return r==TOK_SI || r==TOK_DI;
}

static u64
eaddr_encode_rm(TokenKind base, TokenKind index)
{
    if (base == TOK_NULL) { base = index; index = TOK_NULL; }

    if (is_base(base) && is_index(index)) {
        return (((u64)(base==TOK_BP)) << 1) | ((u64)(index==TOK_DI));
    }
    if (index == TOK_NULL) {
        switch (base) {
            case TOK_SI:   return 4;
            case TOK_DI:   return 5;
            case TOK_BP:   return 6;
            case TOK_BX:   return 7;
            case TOK_NULL: return 6;
            default: break;
        }
    }
    assert(0);
}

static u64
eaddr_encode_mod(EffectiveAddress eaddr)
{
    // TODO: this must be fixed to actually know at codegen time if I want to
    //       emit a displacement byte or not even when displ is 0 (see case [bp])
    //       the easiest thing is to add a bool to eaddr -> has_displacement
    //       the flag is set here (or at the caller site) and it is checked at codegen time
    TokenKind base         = eaddr.register_base;
    TokenKind index        = eaddr.register_index;
    s16       displacement = eaddr.displacement;
    s16       direct_addr  = eaddr.direct_address;

    bool is_displacement_8_bit = (-128 <= displacement && displacement < 128);

    if (base == TOK_BP && index == TOK_NULL && is_displacement_8_bit)
    {
        return 1;
    }
    else if (direct_addr || !displacement)
    {
        return 0;
    }
    else if (is_displacement_8_bit)
    {
        return 1;
    }
    else
    {
        return 2; 
    }
}

internal bool
parse(TokenList *token_list, u64 *idx)
{
    if (parse_line_(token_list, idx))
    {
        printf("Line parsed\n");
        return true;
    }
    else
    {
        printf("Failed to parse line\n");
        return false;
    }
}

internal bool
parse_line_(TokenList *token_list, u64 *idx)
{
    while (parse_terminal(token_list, idx, TOK_EOL))
    {
        printf("Skipping empty line\n");
    }

    u64 cursor = *idx;

    instruction_data = (InstructionData){0};
    if (parse_line(token_list, idx) && parse_terminal(token_list, idx, TOK_EOL))
    {
        printf("Parsing line_ succeded\n");
        return true;
    }

    *idx = cursor;
    printf("Failed to parse line_\n");

    return false;
}

internal bool
parse_line(TokenList *token_list, u64 *idx)
{
    u64 cursor = *idx;

    if (parse_terminal(token_list, idx, TOK_BITS) && parse_terminal(token_list, idx, TOK_NUM))
    {
        return true;
    }

    *idx = cursor;

    if (parse_label(token_list, idx))
    {
        return true;
    }

    InstructionData instruction = {0};
    if (parse_instruction(token_list, idx, &instruction))
    {
        instruction_data = instruction;
        return true;
    }

    printf("Failed to parse line\n");

    return false;
}

internal bool
parse_label(TokenList *token_list, u64 *idx)
{
    // TODO: implement this, for now it returns as if it failed
    u64 cursor = *idx;
    return false;

    u64 p = *idx;

    if (token_list->token[p].token_kind == TOK_LABEL)
    {
        ++p;
        if (token_list->token[p].token_kind == TOK_COLON)
        {
            /*
            TODO:
                Take the label and store it in a map must 
                store position in the obj file that will be created
                I can do it if I know how many bytes each instruction
                is while I am assembling the line
                This must be the case: for each line I must be able to get
                how many bytes it takes, the only case for this not to be the
                case is the "jmp label" instruction kind. Based on how far the
                label is the jmp instruction could be encoded in 
            */
            *idx = p + 1;
            return false;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}

internal bool
parse_instruction(TokenList *token_list, u64 *idx, InstructionData *instruction)
{
    TokenKind mnemonic = TOK_NULL;
    if (parse_mnemonic_(token_list, idx, &mnemonic))
    {
        instruction->mnemonic = mnemonic;
        parse_instruction_tail(token_list, idx, instruction);
        return true;
    }

    return false;
}

internal bool
parse_instruction_tail(TokenList *token_list, u64 *idx, InstructionData *instruction)
{
    u64 cursor = *idx;

    Operand_ operand_src = {0};
    Operand_ operand_dst = {0};

    if (parse_operand_(token_list, idx, &operand_dst) &&
        parse_terminal(token_list, idx, TOK_COMMA)    &&
        parse_operand_(token_list, idx, &operand_src))
    {
        instruction->dst = operand_dst;
        instruction->src = operand_src;
        return true;
    }

    *idx = cursor;

    if (parse_operand_(token_list,  idx, &operand_dst))
    {
        instruction->dst = operand_dst;
        return true;
    }

    if (parse_terminal(token_list, idx, TOK_LABEL))
    {
        return true;
    }

    return false;
}

internal bool
parse_mnemonic_(TokenList *token_list, u64 *idx, TokenKind *mnemonic)
{
    u64 cursor = *idx;

    while (parse_prefix(token_list, idx));

    if (parse_mnemonic(token_list, idx, mnemonic))
    {
        return true;
    }

    *idx = cursor;

    return false;
}

internal bool
parse_operand_(TokenList *token_list, u64 *idx, Operand_ *operand_)
{
    u64 cursor = *idx;

    PrefixOperand prefix_operand = {0};
    while (parse_opr_prefix(token_list, idx, &prefix_operand));
    operand_->prefix_operand = prefix_operand;

    Operand operand = {0};
    if (parse_operand(token_list, idx, &operand))
    {
        operand_->operand = operand;
        return true;
    }

    *idx = cursor;

    return false;
}

internal bool
parse_opr_prefix(TokenList *token_list, u64 *idx, PrefixOperand *prefix_operand)
{
    u64 cursor = *idx;

    PrefixSize prefix_size = PREFIX_SIZE_NONE;
    if (parse_opr_size(token_list, idx, &prefix_size))
    {
        prefix_operand->prefix_size |= prefix_size;
        return true;
    }

    PrefixSegOvr prefix_seg_ovr = PREFIX_SEGOVR_NONE;
    if (parse_seg_ovr(token_list, idx, &prefix_seg_ovr))
    {
        prefix_operand->prefix_seg_ovr |= prefix_seg_ovr;
        return true;
    }

    return false;
}

internal bool
parse_prefix(TokenList *token_list, u64 *idx)
{
    // TODO
    printf("parse_prefix to be implemented\n");
    return false;
}

internal bool
parse_mnemonic(TokenList *token_list, u64 *idx, TokenKind *mnemonic)
{
    TokenKind token_kind = token_list->token[*idx].token_kind;

    // TODO: every time a new mnemonic is add this range must be changed (ugly)
    if (TOK_MOV <= token_kind && token_kind <= TOK_INC)
    {
        *mnemonic = token_kind;
        ++(*idx);
        return true;
    }

    *mnemonic = TOK_NULL;
    return false;
}

internal bool
parse_operand(TokenList *token_list, u64 *idx, Operand *operand)
{
    u64 cursor = *idx;

    TokenKind register_general = TOK_NULL;
    if (parse_register_general(token_list, idx, &register_general))
    {
        operand->register_general = register_general;
        operand->operand_kind = op_kind_from_reg_lut[register_general];

        return true;
    }

    EffectiveAddress eaddr = {0};
    if (parse_eaddr__(token_list, idx, &eaddr))
    {
        operand->effective_address = eaddr;

        if (eaddr.register_base || eaddr.register_index)
        {
            operand->operand_kind = OP_MEM;
        }
        else
        {
            operand->operand_kind = OP_MEM | OP_DADDR;
        }

        return true;
    }

    TokenKind register_segment = TOK_NULL;
    if (parse_register_segment(token_list, idx, &register_segment))
    {
        operand->register_segment = register_segment;
        operand->operand_kind = OP_SEGREG;

        return true;
    }

    s16 imm = 0;
    if (parse_imm(token_list, idx, &imm))
    {
        operand->immediate = imm;
        if (-128 <= imm && imm < 128)
        {
            operand->operand_kind = (OP_IMMEDIATE | OP_IMMEDIATE8);
        }
        else
        {
            operand->operand_kind = (OP_IMMEDIATE | OP_IMMEDIATE16);
        }

        return true;
    }

    return false;
}

internal bool
parse_register_general(TokenList *token_list, u64 *idx, TokenKind *reg_gen)
{
    TokenKind token_kind = token_list->token[*idx].token_kind;

    // NOTE: the assumption here is that TABLE_REGISTER_GENERAL will not be modified. OK but it is a bit mhe ...
    if (TOK_AL <= token_kind && token_kind <= TOK_DI) 
    {
        *reg_gen = token_kind;
        ++(*idx);

        return true;
    }
    else
    {
        return false;
    }
}

internal bool
parse_eaddr__(TokenList *token_list, u64 *idx, EffectiveAddress *eaddr)
{
    u64 cursor = *idx;

    if (parse_terminal(token_list, idx, TOK_LSQUARE_BR) &&
        parse_eaddr_(token_list, idx, eaddr) &&
        parse_terminal(token_list, idx, TOK_RSQUARE_BR))
    {
        eaddr->mod = eaddr_encode_mod(*eaddr);
        eaddr->rm  = eaddr_encode_rm(eaddr->register_base, eaddr->register_index);
        return true;
    }

    *idx = cursor;

    return false;
}

internal bool
parse_eaddr_(TokenList *token_list, u64 *idx, EffectiveAddress *eaddr)
{
    u64 cursor = *idx;

    parse_terminal(token_list, idx, TOK_PLUS);

    if (parse_eaddr(token_list, idx, eaddr))
    {
        return true;
    }

    *idx = cursor;

    return false;
}

internal bool
parse_eaddr(TokenList *token_list, u64 *idx, EffectiveAddress *eaddr)
{
    u64 cursor = *idx;

    if (parse_register_base(token_list, idx, &eaddr->register_base) &&
        parse_terminal(token_list, idx, TOK_PLUS) &&
        parse_register_index(token_list, idx, &eaddr->register_index))
    {
        parse_signed_num(token_list, idx, &eaddr->displacement);
        return true;
    }

    *idx = cursor;

    if (parse_register_base(token_list, idx, &eaddr->register_base))
    {
        parse_signed_num(token_list, idx, &eaddr->displacement);
        return true;
    }

    *idx = cursor;

    if (parse_register_index(token_list, idx, &eaddr->register_index))
    {
        parse_signed_num(token_list, idx, &eaddr->displacement);
        return true;
    }

    if (parse_direct_address(token_list, idx, &eaddr->direct_address))
    {
        return true;
    }

    *idx = cursor;

    return false;
}

internal bool
parse_register_base(TokenList *token_list, u64 *idx, TokenKind *reg_base)
{
    TokenKind token_kind = token_list->token[*idx].token_kind;
    if (token_kind == TOK_BX || token_kind == TOK_BP)
    {
        ++(*idx);
        *reg_base = token_kind;
        return true;
    }
    else
    {
        return false;
    }
}

internal bool
parse_register_index(TokenList *token_list, u64 *idx, TokenKind *reg_idx)
{
    TokenKind token_kind = token_list->token[*idx].token_kind;
    if (token_kind == TOK_SI || token_kind == TOK_DI)
    {
        ++(*idx);
        *reg_idx = token_kind;
        return true;
    }
    else
    {
        return false;
    }
}

internal bool
parse_signed_num(TokenList *token_list, u64 *idx, s16 *signed_num)
{
    u64 cursor = *idx;

    TokenKind opr_math;

    if (parse_opr_math(token_list, idx, &opr_math) &&
        parse_terminal(token_list, idx, TOK_NUM))
    {
        s16 num = (s16)token_list->token[(*idx) - 1].num;

        if (opr_math == TOK_PLUS)
        {
            *signed_num = num;
        }
        else
        {
            *signed_num = -num;
        }

        return true;
    }

    *idx = cursor;

    return false;
}

internal bool
parse_direct_address(TokenList *token_list, u64 *idx, s16 *direct_addr)
{
    u64 cursor = *idx;

    TokenKind opr_math = TOK_NULL;
    parse_opr_math(token_list, idx, &opr_math);

    if (parse_terminal(token_list, idx, TOK_NUM))
    {
        s16 num = (s16)token_list->token[(*idx) - 1].num;

        if (opr_math == TOK_NULL || opr_math == TOK_PLUS)
        {
            *direct_addr = num;
        }
        else
        {
            assert(opr_math == TOK_MINUS);
            *direct_addr = num;
        }

        return true;
    }

    *idx = cursor;

    return false;
}

internal bool
parse_register_segment(TokenList *token_list, u64 *idx, TokenKind *reg_seg)
{
    TokenKind token_kind = token_list->token[*idx].token_kind;

    // NOTE: the assumption here is that TABLE_REGISTER_SEGMENT will not be modified. OK but it is a bit mhe ...
    if (TOK_ES <= token_kind && token_kind <= TOK_DS) 
    {
        ++(*idx);
        *reg_seg = token_kind;
        return true;
    }

    return false;
}

internal bool
parse_imm(TokenList *token_list, u64 *idx, s16 *imm)
{
    u64 cursor = *idx;

    TokenKind opr_math = TOK_NULL;

    parse_opr_math(token_list, idx, &opr_math);

    TokenKind token_kind = token_list->token[*idx].token_kind;
    if (token_kind == TOK_NUM || token_kind == TOK_ZERO)
    {
        // TODO: I should check that the number I am parsing (with sign included)
        //       can actually fit in 16 bit 2's complement
        s16 num = (s16)token_list->token[(*idx)++].num;

        if (opr_math == TOK_NULL || opr_math == TOK_PLUS)
        {
            *imm = num;
        }
        else
        {
            assert(opr_math == TOK_MINUS);
            *imm = -num;
        }
        return true;
    }

    *idx = cursor;

    return false;
}

internal bool
parse_opr_math(TokenList *token_list, u64 *idx, TokenKind *opr_math)
{
    TokenKind token_kind = token_list->token[*idx].token_kind;
    if (token_kind == TOK_PLUS || token_kind == TOK_MINUS)
    {
        ++(*idx);
        *opr_math = token_kind;
        return true;
    }
    else
    {
        return false;
    }
}

internal bool
parse_opr_size(TokenList *token_list, u64 *idx, PrefixSize *prefix_size)
{
    TokenKind token_kind = token_list->token[*idx].token_kind;

    if (token_kind == TOK_BYTE)
    {
        ++(*idx);
        *prefix_size = PREFIX_SIZE_BYTE;
        return true;
    }
    else if (token_kind == TOK_WORD)
    {
        ++(*idx);
        *prefix_size = PREFIX_SIZE_WORD;
        return true;
    }
    else
    {
        return false;
    }
}

internal bool
parse_seg_ovr(TokenList *token_list, u64 *idx, PrefixSegOvr *prefix_seg_ovr)
{
    u64 cursor = *idx;

    TokenKind reg_seg;
    if (parse_register_segment(token_list, idx, &reg_seg) &&
        parse_terminal(token_list, idx, TOK_COLON))
    {
        *prefix_seg_ovr = seg_ovr_lut[reg_seg];
        return true;
    }
    else
    {
        *idx = cursor;

        return false;
    }
}

bool
parse_terminal(TokenList *token_list, u64 *idx, TokenKind token_kind)
{
    if (token_list->token[*idx].token_kind == token_kind)
    {
        ++(*idx);
        return true;
    }
    else
    {
        return false;
    }
}
