#ifndef ASM8086_H
#define ASM8086_H

#include <stdint.h>
#include <stdbool.h>
#include <limits.h>

#include "utils.c"
#include "tables.h"

#define LEN_MAX_FILENAME      64
#define LEN_MAX_LABEL         32
#define CNT_MAX_LABEL         100 // Maximum number of labels in .asm I am trying to compile
#define LEN_MAX_NUM_STR       8
#define LEN_MAX_INST_STACK    10
#define LEN_MAX_INST_BYTES    6 
#define LEN_SEGMENT           K(64)

typedef enum TokenKind TokenKind;
enum TokenKind
{
    TOK_NULL,
#define ENTRY(id, ...) Glue(TOK_, id),
    TABLE_MNEMONIC
    TABLE_REGISTER_GENERAL
    TABLE_REGISTER_SEGMENT
    TABLE_PREFIX
    TABLE_OPERATORS
    TABLE_ONE_CHARACTER_TOKEN
#undef ENTRY
    TOK_COUNT,
};

typedef struct Token Token;
struct Token {
    TokenKind token_kind;
    String token_view;
    u64 num;
};

typedef struct TokenList TokenList;
struct TokenList {
    Arena *arena;
    Token *token;
    u64 cnt;
};

String token_string[] = {
    string_lit("null"),
#define ENTRY(id, name, ...) string_lit(Stringify(name)),
    TABLE_MNEMONIC
    TABLE_REGISTER_GENERAL
    TABLE_REGISTER_SEGMENT
    TABLE_PREFIX
    TABLE_OPERATORS
    TABLE_ONE_CHARACTER_TOKEN
#undef ENTRY
    string_lit("count")
};

static const TokenKind kLUT[UCHAR_MAX + 1] = {
#define ENTRY(id, ch) [(ch)] = (Glue(TOK_, id)),
    TABLE_ONE_CHARACTER_TOKEN
#undef ENTRY
};

// enum Mnemonic
// {
// #define ENTRY(id, ...) id,
//     TABLE_MNEMONIC
// #undef ENTRY
// };
// 
// enum Register
// {
// #define ENTRY(id, ...) id,
//     TABLE_REGISTER_FULL
// #undef ENTRY
// };
// 
// enum Prefix
// {
// #define ENTRY(id, ...) id,
//     TABLE_PREFIX
// #undef ENTRY
// };
// 
// char *names_registers[] = {
// #define ENTRY(id, name, ...) Stringify(name)
//     TABLE_REGISTER_FULL
// #undef ENTRY
// };
// 
// char *names_prefix[] = {
// #define ENTRY(id, name, ...) Stringify(name)
//     TABLE_PREFIX
// #undef ENTRY
// };

// u8 reg_field_registers[] = {
// #define ENTRY(id, name, reg_field, ...) 0b##reg_field,
//     TABLE_REGISTER_FULL
// #undef ENTRY
// };
// 
// u8 w_field_registers[] = {
// #define ENTRY(id, name, reg_field, w_field) 0b##w_field,
//     TABLE_REGISTER_FULL
// #undef ENTRY
// };
// 
// char *names_tokens[] = {
// #define ENTRY(id, name, ...) Stringify(name),
//     TABLE_MNEMONIC
//     TABLE_REGISTER_FULL
//     TABLE_PREFIX
//     TABLE_OPERATORS
// #undef ENTRY
// },


// enum IdLexTok1 id_reg_seg[] = {
// #define ENTRY(id, enc, ...) TOK1_##id,
//     TABLE_REG_SEG_ENCODING
// #undef ENTRY
// };
// 
// const u8 enc_reg_seg[] = {
// #define ENTRY(id, enc, ...) 0b##enc,
//     TABLE_REG_SEG_ENCODING
// #undef ENTRY
// };

// struct LexTokLabel
// {
//     char name[32];
// };
// 
// struct LexTokNum
// {
//     long value;
// };
// 
// struct LexTok
// {
//     enum IdLexTok0 id0;
//     enum IdLexTok1 id1;
//     union
//     {
//         struct LexTokLabel *p_label;
//         struct LexTokNum   *p_num;
//     };
// };
// 
// enum IdParserNode
// {
//     NODE_NONE,
// #define ENTRY(id, name) NODE_##id,
//     TABLE_NODE_NON_TERMINAL
// #undef ENTRY
//     NODE_TERMINAL,
//     NODE_COUNT,
// };
// 
// enum IdParserNode id_parser_node[] = {
//     NODE_NONE,
// #define ENTRY(id, name) NODE_##id,
//     TABLE_NODE_NON_TERMINAL
// #undef ENTRY
//     NODE_TERMINAL,
//     NODE_COUNT,
// };
// 
// const char *names_parser_node[] = {
//     "none",
// #define ENTRY(id, name) #name,
//     TABLE_NODE_NON_TERMINAL
// #undef ENTRY
//     "terminal",
//     "count",
// };
// 
// struct ParserNode
// {
//     enum IdParserNode  id;
//     struct LexTok     *p_tok;
// };
// 
// struct EAddr
// {
//     enum IdLexTok1 reg_base;
//     enum IdLexTok1 reg_idx;
// };
// 
// struct EAddr eaddr[] = {
// #define ENTRY(base, idx, ...) {TOK1_##base, TOK1_##idx},
//     TABLE_EADDR
// #undef ENTRY
// };
// 
// const u8 enc_rm_eaddr[] = {
// #define ENTRY(base, idx, rm) 0b##rm,
//     TABLE_EADDR
// #undef ENTRY
// };
// 
// enum InstFieldId
// {
// #define ENTRY(id, ...) INST_##id,
//     OPCODE(1, ...)
//     OPCODE(2, ...)
//     ImplRegD(...)
//     ImplRmD(...)
//     ImplReg(...)
//     TABLE_INST_FIELDS
// #undef ENTRY
// };
// 
// struct InstField
// {
//     enum InstFieldId id;
//     u8 value;
//     u8 size;
// };
// 
// struct Instruction
// {
//     enum IdLexTok1   id_mnemonic;
//     struct InstField fields[15];
// };
// 
// struct InstructionData
// {
//     struct ParserNode *p_eaddr__;
//     struct ParserNode *p_daddr;
//     struct ParserNode *p_imm;
//     struct LexTok     *p_comma;
//     struct LexTok     *p_reg_gen_dst;
//     struct LexTok     *p_reg_gen_src;
//     enum IdLexTok1     mnemonic;
//     enum IdLexTok1     reg_seg;
//     enum IdLexTok1     reg_base;
//     enum IdLexTok1     reg_idx;
//     enum IdLexTok1     seg_ovr;
//     u8                 regd;
//     u8                 rmd;
//     u8                 w;
//     u8                 mod;
//     u8                 rm;
//     u8                 sr;
//     long               disp; 
//     long               daddr;
//     long               imm;
//     u8                 operand_cnt;
// };
// 
// /*
//     In the case of mov si, bx
//     instruction_data.mnemonic = MOV
//     instruction_data.
// */
// 
// static struct InstructionData inst_data = {0};
// 
// struct InstEnc
// {
//     u64 enc;
//     u8  len;
// };
// 
// struct Instruction inst_table[] = {
// #define INST(mnemonic, ...) { TOK1_##mnemonic, __VA_ARGS__ },
// #define ENTRY(id, value, len) { INST_##id, 0b##value, len }
//     TABLE_INST
// #undef ENTRY
// #undef INST
// };

#endif // ASM8086_H
