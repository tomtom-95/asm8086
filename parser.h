#ifndef PARSER_H
#define PARSER_H

#include "utils.c"
#include "tables.h"
#include "tokenizer.h"

typedef enum PrefixSegOvr
{
    PREFIX_SEGOVR_NONE = 0,
#define ENTRY(id, name, serial) Glue(PREFIX_SEGOVR_, id) = (1u << serial),
    TABLE_REGISTER_SEGMENT
#undef ENTRY
}
PrefixSegOvr;

typedef enum PrefixSize
{
    PREFIX_SIZE_NONE = 0,
#define ENTRY(id, name, serial) Glue(PREFIX_SIZE_, id) = (1u << serial),
    TABLE_OPERAND_PREFIX_SIZE
#undef ENTRY
}
PrefixSize;

typedef struct PrefixOperand PrefixOperand;
struct PrefixOperand {
    PrefixSegOvr prefix_seg_ovr;
    PrefixSize   prefix_size;
};

typedef enum OperandKind
{
    OP_NONE         = 0,
    OP_REG8         = (1u << 1),
    OP_REG16        = (1u << 2),
    OP_MEM          = (1u << 3),
    OP_ACC8         = (1u << 4),
    OP_ACC16        = (1u << 5),
    OP_IMMEDIATE    = (1u << 6),
    OP_IMMEDIATE8   = (1u << 7),
    OP_IMMEDIATE16  = (1u << 8),
    OP_DADDR        = (1u << 9),
    OP_SEGREG       = (1u << 10),
}
OperandKind;

typedef struct EffectiveAddress EffectiveAddress;
struct EffectiveAddress {
    TokenKind register_base;
    TokenKind register_index;
    s16       displacement;
    s16       direct_address;

    u64       mod;
    u64       rm;
};

typedef struct Operand Operand;
struct Operand {
    OperandKind      operand_kind;
    TokenKind        register_general;
    TokenKind        register_segment;
    EffectiveAddress effective_address;
    s16              immediate;
};

typedef struct Operand_ Operand_;
struct Operand_ {
    PrefixOperand prefix_operand;
    Operand       operand;
};

typedef struct InstructionData InstructionData;
struct InstructionData {
    TokenKind mnemonic;
    Operand_   dst;
    Operand_   src; 
};

/* Look up table for getting encoding of REG field starting from TokenKind (table 4.9 of Intel manual) */
u8 reg_field_lut[] = {
#define ENTRY(id, name, reg_encoding, ...) [Glue(TOK_, id)] = reg_encoding,
    TABLE_REGISTER_GENERAL
#undef ENTRY
};

/* Look up table for getting encoding of W field starting from TokenKind (table 4.9 of Intel manual) */
u8 w_field_lut[] = {
#define ENTRY(id, name, reg_encoding, w_encoding, ...) [Glue(TOK_, id)] = w_encoding,
    TABLE_REGISTER_GENERAL
#undef ENTRY
};

/* Look up table for getting OperandKind starting from TokenKind of a reg */
OperandKind op_kind_from_reg_lut[TOK_COUNT] = {
  [TOK_NULL] = OP_NONE,
#define ENTRY(id, name, reg_enc, w_enc, op_kind) [Glue(TOK_, id)] = op_kind,
  TABLE_REGISTER_GENERAL
#undef ENTRY
};

/* Look up table for getting Segment Override prefix from TokenKind of a segment register */
PrefixSegOvr seg_ovr_lut[TOK_COUNT] = {
#define ENTRY(id, name, serial) [Glue(TOK_, id)] = Glue(PREFIX_SEGOVR_, id),
    TABLE_REGISTER_SEGMENT
#undef ENTRY
};



/* Encoding of table 4.10 in Intel manual */
internal u64              eaddr_encode_rm(TokenKind base, TokenKind index); 
internal u64              eaddr_encode_mod(EffectiveAddress eaddr);

internal s32              is_base(TokenKind r);
internal s32              is_index(TokenKind r);

internal void             parse(TokenList *token_list, u64 *idx);
internal void             parse_line_(TokenList *token_list, u64 *idx);
internal void             parse_line(TokenList *token_list, u64 *idx);
internal void             parse_label(TokenList *token_list, u64 *idx);
internal void             parse_instruction(TokenList *token_list, u64 *idx);
internal void             parse_instruction_tail(TokenList *token_list, u64 *idx);
internal TokenKind        parse_mnemonic_(TokenList *token_list, u64 *idx);
internal Operand_         parse_operand_(TokenList *token_list, u64 *idx);
internal PrefixOperand    parse_opr_prefix(TokenList *token_list, u64 *idx);
internal void             parse_prefix(TokenList *token_list, u64 *idx);
internal TokenKind        parse_mnemonic(TokenList *token_list, u64 *idx);
internal Operand          parse_operand(TokenList *token_list, u64 *idx);
internal TokenKind        parse_register_general(TokenList *token_list, u64 *idx);
internal s16              parse_direct_address(TokenList *token_list, u64 *idx);

internal EffectiveAddress parse_eaddr__(TokenList *token_list, u64 *idx);
internal EffectiveAddress parse_eaddr_(TokenList *token_list, u64 *idx);
internal EffectiveAddress parse_eaddr(TokenList *token_list, u64 *idx);

internal TokenKind        parse_register_segment(TokenList *token_list, u64 *idx);
internal TokenKind        parse_register_base(TokenList *token_list, u64 *idx);
internal TokenKind        parse_register_index(TokenList *token_list, u64 *idx);
internal s16              parse_imm(TokenList *token_list, u64 *idx);
internal TokenKind        parse_opr_math(TokenList *token_list, u64 *idx);
internal PrefixSize       parse_opr_size(TokenList *token_list, u64 *idx);
internal PrefixSegOvr     parse_seg_ovr(TokenList *token_list, u64 *idx);
internal Token            parse_terminal(TokenList *token_list, TokenKind token_kind, u64 *idx);

internal s16              parse_signed_num(TokenList *token_list, u64 *idx);

InstructionData instruction_data = {0};

#endif // PARSER_H
