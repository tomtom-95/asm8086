#ifndef PARSER_H
#define PARSER_H

#include "utils.c"
#include "tables.h"
#include "tokenizer.h"

/* Look up table to getting encoding of REG field starting from TokenKind (table 4.9 of Intel manual) */
u8 reg_field_lut[] = {
#define ENTRY(id, name, reg_encoding, ...) [Glue(TOK_, id)] = reg_encoding,
    TABLE_REGISTER_GENERAL
#undef ENTRY
};

/* Look up table to getting encoding of W field starting from TokenKind (table 4.9 of Intel manual) */
u8 w_field_lut[] = {
#define ENTRY(id, name, reg_encoding, w_encoding) [Glue(TOK_, id)] = w_encoding,
    TABLE_REGISTER_GENERAL
#undef ENTRY
};

typedef struct EffectiveAddress EffectiveAddress;
struct EffectiveAddress {
    TokenKind register_base;
    TokenKind register_index;

    TokenKind displacement;
    u64 displacement_value;

    TokenKind direct_address;
    s64 direct_address_value;
};

typedef enum OperandKind
{
    OPERAND_NULL,
    OPERAND_EFFECTIVE_ADDRESS,
    OPERAND_GENERAL_REGISTER_8_BIT,
    OPERAND_GENERAL_REGISTER_16_BIT,
    OPERAND_SEGMENT_REGISTER,
    OPERAND_IMMEDIATE_8_BIT,
    OPERAND_IMMEDIATE_16_BIT,
    OPERAND_COUNT
}
OperandKind;

typedef struct Operand Operand;
struct Operand {
    OperandKind operand_kind;
    TokenKind register_general;
    TokenKind register_segment;
    EffectiveAddress effective_address;
    u16 data;
    s16 immediate;
};

typedef struct InstructionData InstructionData;
struct InstructionData {
    TokenKind mnemonic;
    Operand src; 
    Operand dst;
};

internal s32 is_base(TokenKind r);
internal s32 is_index(TokenKind r);

internal void             parse(TokenList *token_list, u64 *idx);
internal void             parse_line_(TokenList *token_list, u64 *idx);
internal void             parse_line(TokenList *token_list, u64 *idx);
internal void             parse_label(TokenList *token_list, u64 *idx);
internal void             parse_instruction(TokenList *token_list, u64 *idx);
internal void             parse_instruction_tail(TokenList *token_list, u64 *idx);
internal TokenKind        parse_mnemonic_(TokenList *token_list, u64 *idx);
internal Operand          parse_operand_(TokenList *token_list, u64 *idx);
internal void             parse_opr_prefix(TokenList *token_list, u64 *idx);
internal void             parse_prefix(TokenList *token_list, u64 *idx);
internal TokenKind        parse_mnemonic(TokenList *token_list, u64 *idx);
internal Operand          parse_operand(TokenList *token_list, u64 *idx);
internal TokenKind        parse_register_general(TokenList *token_list, u64 *idx);
s64                       parse_direct_address(TokenList *token_list, u64 *idx);

internal EffectiveAddress parse_eaddr__(TokenList *token_list, u64 *idx);
internal EffectiveAddress parse_eaddr_(TokenList *token_list, u64 *idx);
internal EffectiveAddress parse_eaddr(TokenList *token_list, u64 *idx);

internal TokenKind        parse_register_segment(TokenList *token_list, u64 *idx);
TokenKind                 parse_register_base(TokenList *token_list, u64 *idx);
TokenKind                 parse_register_index(TokenList *token_list, u64 *idx);
internal s16              parse_imm(TokenList *token_list, u64 *idx);
internal TokenKind        parse_opr_math(TokenList *token_list, u64 *idx);
internal void             parse_opr_size(TokenList *token_list, u64 *idx);
internal void             parse_seg_ovr(TokenList *token_list, u64 *idx);
internal Token            parse_terminal(TokenList *token_list, TokenKind token_kind, u64 *idx);

u64                       parse_signed_num(TokenList *token_list, u64 *idx);

InstructionData instruction_data = {0};

#endif // PARSER_H
