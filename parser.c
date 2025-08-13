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

internal bool
is_register_8_bit(TokenKind r)
{
    bool is_low = (r == TOK_AL || r == TOK_CL || r == TOK_DL || r == TOK_BL);
    bool is_high = (r == TOK_AH || r == TOK_CH || r == TOK_DH || r == TOK_BH);

    return is_low || is_high;
}

internal void
parse(TokenList *token_list, u64 *idx)
{
    instruction_data = (InstructionData){0};

    parse_line_(token_list, idx);
    if (idx)
    {
        printf("Line parsed\n");
        // Parsing of the line was successfull
        // I can emit machine code if the instruction does not involve a label
    }
    else
    {
        printf("Failed to parse line\n");
        return;
    }
}

internal void
parse_line_(TokenList *token_list, u64 *idx)
{
    u64 p = *idx;

    parse_line(token_list, &p);
    parse_terminal(token_list, TOK_EOL, &p);

    if (p)
    {
        *idx = p;
        return;
    }

    printf("Failed to parse line_\n");
    *idx = 0;
}

internal void
parse_line(TokenList *token_list, u64 *idx)
{
    u64 p = *idx;

    // This must go somewhere in instruction data
    parse_terminal(token_list, TOK_BITS, &p);
    parse_terminal(token_list, TOK_NUM, &p);

    if (p)
    {
        *idx = p;
        return;
    }

    p = *idx;

    parse_label(token_list, &p);

    if (p)
    {
        *idx = p;
        return;
    }

    p = *idx;

    parse_instruction(token_list, &p);

    if (p)
    {
        *idx = p;
        return;
    }

    *idx = 0;
}

internal void
parse_label(TokenList *token_list, u64 *idx)
{
    // TODO: implement this, for now it returns as if it failed
    *idx = 0;
    return;

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
            return;
        }
        else
        {
            return;
        }
    }
    else
    {
        return;
    }
}

internal void
parse_instruction(TokenList *token_list, u64 *idx)
{
    instruction_data = (InstructionData){0};

    u64 p = *idx;

    instruction_data.mnemonic = parse_mnemonic_(token_list, &p);

    *idx = p;

    parse_instruction_tail(token_list, &p);

    if (p)
    {
        *idx = p;
    }
}

internal void
parse_instruction_tail(TokenList *token_list, u64 *idx)
{
    Operand operand_src = {0};
    Operand operand_dst = {0};

    u64 p = *idx;

    operand_dst = parse_operand_(token_list, &p);
    parse_terminal(token_list, TOK_COMMA, &p);
    operand_src = parse_operand_(token_list, &p);

    if (p)
    {
        instruction_data.src = operand_src;
        instruction_data.dst = operand_dst;
        *idx = p;
        return;
    }

    p = *idx;

    operand_src = parse_operand_(token_list,  &p);

    if (p)
    {
        instruction_data.src = operand_src;
        *idx = p;
        return;
    }

    p = *idx;

    parse_terminal(token_list, TOK_LABEL, &p);

    if (p)
    {
        *idx = p;
        return;
    }

    *idx = 0;
}

internal TokenKind
parse_mnemonic_(TokenList *token_list, u64 *idx)
{
    u64 p = *idx;

    while (p)
    {
        *idx = p;
        // TODO: must a struct to hold the list of prefixes
        parse_prefix(token_list, &p);
    }

    p = *idx;
    TokenKind mnemonic = parse_mnemonic(token_list, &p);

    if (p != 0)
    {
        *idx = p;
        return mnemonic;
    }

    *idx = 0;
    return TOK_NULL;
}

internal Operand
parse_operand_(TokenList *token_list, u64 *idx)
{
    Operand res = {0};

    u64 p = *idx;

    while (p)
    {
        *idx = p;
        // TODO: must store in a list the prefixes that has been parsed
        parse_opr_prefix(token_list, &p);
    }

    p = *idx;
    res = parse_operand(token_list, &p);

    if (p != 0)
    {
        *idx = p;
        return res;
    }

    *idx = 0;
    return res;
}

internal void
parse_opr_prefix(TokenList *token_list, u64 *idx)
{
    u64 p = *idx;

    parse_opr_size(token_list, &p);

    if (p)
    {
        *idx = p;
        return;
    }

    p = *idx;

    parse_seg_ovr(token_list, &p);

    if (p)
    {
        *idx = p;
        return;
    }

    *idx = 0;
}

internal void
parse_prefix(TokenList *token_list, u64 *idx)
{
    // TODO
    printf("parse_prefix to be implemented\n");

    *idx = 0;

    return;
}

internal TokenKind
parse_mnemonic(TokenList *token_list, u64 *idx)
{
    TokenKind token_kind = token_list->token[*idx].token_kind;

    if (TOK_MOV <= token_kind && token_kind <= TOK_POP)
    {
        ++(*idx);
        return token_kind;
    }

    *idx = 0;
}

internal Operand
parse_operand(TokenList *token_list, u64 *idx)
{
    u64 p = *idx;

    Operand operand = {0};

    TokenKind register_general = parse_register_general(token_list, &p);
    if (p)
    {
        if (is_register_8_bit(register_general))
        {
            operand.operand_kind = OPERAND_GENERAL_REGISTER_8_BIT;
        }
        else
        {
            operand.operand_kind = OPERAND_GENERAL_REGISTER_16_BIT;
        }

        operand.register_general = register_general;
        *idx = p;

        return operand;
    }

    p = *idx;

    EffectiveAddress effective_address = parse_eaddr__(token_list, &p);
    if (p)
    {
        operand.operand_kind = OPERAND_EFFECTIVE_ADDRESS;
        operand.effective_address = effective_address;
        *idx = p;

        return operand;
    }
    
    p = *idx;

    TokenKind register_segment = parse_register_segment(token_list, &p);
    if (p)
    {
        operand.operand_kind = OPERAND_SEGMENT_REGISTER;
        operand.register_segment = register_segment;
        *idx = p;

        return operand;
    }

    p = *idx;

    s16 immediate = parse_imm(token_list, &p);
    if (p)
    {
        if (immediate < 256)
        {
            operand.operand_kind = OPERAND_IMMEDIATE_8_BIT;
        }
        else
        {
            operand.operand_kind = OPERAND_IMMEDIATE_16_BIT;
        }

        operand.immediate = immediate;
        *idx = p;

        return operand;
    }

    operand = (Operand){0};
    *idx = 0;

    return operand;
}

internal TokenKind
parse_register_general(TokenList *token_list, u64 *idx)
{
    TokenKind token_kind = token_list->token[*idx].token_kind;

    // NOTE: the assumption here is that TABLE_REGISTER_GENERAL will not be modified. OK but it is a bit mhe ...
    if (TOK_AL <= token_kind && token_kind <= TOK_DI) 
    {
        ++(*idx);
    }
    else
    {
        *idx = 0;
    }

    return token_kind;
}

internal EffectiveAddress
parse_eaddr__(TokenList *token_list, u64 *idx)
{
    u64 p = *idx;

    parse_terminal(token_list, TOK_LSQUARE_BR, &p);
    EffectiveAddress effective_address = parse_eaddr_(token_list, &p);
    parse_terminal(token_list, TOK_RSQUARE_BR, &p);

    if (p)
    {
        *idx = p;
        return effective_address;
    }

    effective_address = (EffectiveAddress){0};
    *idx = 0;

    return effective_address;
}

internal EffectiveAddress
parse_eaddr_(TokenList *token_list, u64 *idx)
{
    u64 p = *idx;

    parse_terminal(token_list, TOK_PLUS, &p);

    if (p)
    {
        *idx = p;
    }

    p = *idx;

    EffectiveAddress effective_address = parse_eaddr(token_list, &p);

    if (p)
    {
        *idx = p;
        return effective_address;
    }

    effective_address = (EffectiveAddress){0};
    *idx = 0;

    return effective_address;
}

internal EffectiveAddress
parse_eaddr(TokenList *token_list, u64 *idx)
{
    EffectiveAddress eaddr = {0};
    u64 p = *idx;

    eaddr.register_base = parse_register_base(token_list, &p);
    parse_terminal(token_list, TOK_PLUS, &p);
    eaddr.register_index = parse_register_index(token_list, &p);

    if (p)
    {
        *idx = p;

        eaddr.displacement_value = parse_signed_num(token_list, &p);

        if (p)
        {
            eaddr.displacement = TOK_NUM;
            *idx = p;
        }

        return eaddr;
    }

    eaddr = (EffectiveAddress){0};
    p = *idx;

    eaddr.register_base = parse_register_base(token_list, &p);

    if (p)
    {
        *idx = p;

        eaddr.displacement_value = parse_signed_num(token_list, &p);

        if (p)
        {
            eaddr.displacement = TOK_NUM;
        }

        return eaddr;
    }

    eaddr = (EffectiveAddress){0};
    p = *idx;

    eaddr.register_index = parse_register_index(token_list, &p);

    if (p)
    {
        *idx = p;

        eaddr.displacement_value = parse_signed_num(token_list, &p);

        if (p)
        {
            eaddr.displacement = TOK_NUM;
        }

        return eaddr;
    }

    eaddr = (EffectiveAddress){0};
    p = *idx;

    eaddr.direct_address_value = parse_direct_address(token_list, &p);

    if (p)
    {
        *idx = p;

        eaddr.direct_address = TOK_NUM;

        return eaddr;
    }

    eaddr = (EffectiveAddress){0};
    *idx = 0;

    return eaddr;
}

TokenKind
parse_register_base(TokenList *token_list, u64 *idx)
{
    TokenKind token_kind = token_list->token[*idx].token_kind;
    if (token_kind == TOK_BX || token_kind == TOK_BP)
    {
        ++(*idx);
    }

    return token_kind;
}

TokenKind
parse_register_index(TokenList *token_list, u64 *idx)
{
    TokenKind token_kind = token_list->token[*idx].token_kind;
    if (token_kind == TOK_SI || token_kind == TOK_DI)
    {
        ++(*idx);
    }

    return token_kind;
}

u64
parse_signed_num(TokenList *token_list, u64 *idx)
{
    u64 p = *idx;

    parse_opr_math(token_list, &p);
    Token token = parse_terminal(token_list, TOK_NUM, &p);

    if (p)
    {
        *idx = p;
        return token.num;
    }

    *idx = 0;
    return 0;
}

internal TokenKind
parse_register_segment(TokenList *token_list, u64 *idx)
{
    TokenKind token_kind = token_list->token[*idx].token_kind;

    // NOTE: the assumption here is that TABLE_REGISTER_SEGMENT will not be modified. OK but it is a bit mhe ...
    if (TOK_ES <= token_kind && token_kind <= TOK_DS) 
    {
        ++(*idx);
        return token_kind;
    }

    *idx = 0;

    return token_kind;
}

internal s16
parse_imm(TokenList *token_list, u64 *idx)
{
    u64 p = *idx;
    s32 num;

    TokenKind operator_math = parse_opr_math(token_list, &p);
    if (p)
    {
        *idx = p;
    }

    TokenKind token_kind = token_list->token[*idx].token_kind;
    if (token_kind == TOK_NUM || token_kind == TOK_ZERO)
    {
        num = token_list->token[*idx].num;

        ++(*idx);
        if (operator_math == TOK_NULL || operator_math == TOK_PLUS)
        {
            return num;
        }
        else
        {
            assert(operator_math == TOK_MINUS);
            return -num;
        }
    }

    *idx = 0;

    return 0;
}

internal TokenKind
parse_opr_math(TokenList *token_list, u64 *idx)
{
    TokenKind token_kind = token_list->token[*idx].token_kind;
    if (token_kind == TOK_PLUS || token_kind == TOK_MINUS)
    {
        ++(*idx);
        return token_kind;
    }

    *idx = 0;
    return TOK_NULL;
}

internal void
parse_opr_size(TokenList *token_list, u64 *idx)
{
    TokenKind token_kind = token_list->token[*idx].token_kind;
    if (token_kind == TOK_BYTE || token_kind == TOK_WORD)
    {
        ++(*idx);
    }

    *idx = 0;
}

internal void
parse_seg_ovr(TokenList *token_list, u64 *idx)
{
    u64 p = *idx;

    parse_register_segment(token_list, &p);
    parse_terminal(token_list, TOK_COLON, &p);

    if (p)
    {
        *idx = p;
        return;
    }

    *idx = 0;
}

s64
parse_direct_address(TokenList *token_list, u64 *idx)
{
    u64 p = *idx;

    TokenKind tok_opr_math = parse_opr_math(token_list, &p);

    if (p)
    {
        *idx = p;
    }

    p = *idx;

    Token tok_num = parse_terminal(token_list, TOK_NUM, &p);

    if (p)
    {
        *idx = p;
        if (tok_opr_math == TOK_NULL || tok_opr_math == TOK_PLUS)
        {
            return tok_num.num;
        }
        else
        {
            return -((s64)tok_num.num);
        }
    }

    *idx = 0;
}

Token
parse_terminal(TokenList *token_list, TokenKind token_kind, u64 *idx)
{
    Token token = token_list->token[*idx];
    if (token.token_kind == token_kind)
    {
        ++(*idx);
    }
    else
    {
        *idx = 0;
    }

    token;
}
