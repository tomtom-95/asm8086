#ifndef ASM8086_H
#define ASM8086_H

#include <stdint.h>
#include <stdbool.h>

#include "asm8086_tables.h"

#define K(num) (1024 * num)
#define M(num) (1024 * K(num))

#define ArrayCount(array) (sizeof(array) / sizeof(array[0]))

#define LEN_MAX_FILENAME      64
#define LEN_MAX_LABEL         32
#define LEN_MAX_NUM_STR       8
#define LEN_MAX_INST_STACK    10
#define LEN_MAX_INST_BYTES    6 
#define LEN_SEGMENT           K(64)

typedef u_int8_t  u8;
typedef u_int16_t u16;
typedef u_int32_t u32;
typedef u_int64_t u64;

enum IdLexTok0
{
    TOK0_NONE,
#define ENTRY(id0, id1, name) TOK0_##id0,
#define ENTRY_ALT(...)
    TABLE_ALL
#undef ENTRY
#undef ENTRY_ALT
    TOK0_COUNT,
};

enum IdLexTok1
{
    TOK1_NONE,
#define ENTRY(id0, id1, name) TOK1_##id1,
#define ENTRY_ALT ENTRY
    TABLE_ALL
#undef ENTRY
#undef ENTRY_ALT
    TOK1_COUNT,
};

enum IdLexTok0 id_lex_tok0[] = {
    TOK0_NONE,
#define ENTRY(id0, id1, name) TOK0_##id0,
#define ENTRY_ALT ENTRY
    TABLE_ALL
#undef ENTRY
#undef ENTRY_ALT
    TOK0_COUNT,
};

enum IdLexTok1 id_lex_tok1[] = {
    TOK1_NONE,
#define ENTRY(id0, id1, name) TOK1_##id1,
#define ENTRY_ALT ENTRY
    TABLE_ALL
#undef ENTRY
#undef ENTRY_ALT
    TOK1_COUNT,
};

const char *names_lex_tok1[] = {
    "none",
#define ENTRY(id0, id1, name) #name,
#define ENTRY_ALT ENTRY
    TABLE_ALL
#undef ENTRY
#undef ENTRY_ALT
    "count"
};

enum IdLexTok1 id_reg_gen[] = {
#define ENTRY(id, enc, ...) TOK1_##id,
    TABLE_REG_GEN_ENCODING
#undef ENTRY
};

const u8 enc_reg_gen[] = {
#define ENTRY(id, enc, ...) 0b##enc,
    TABLE_REG_GEN_ENCODING
#undef ENTRY
};

const u8 wid_reg_gen[] = {
#define ENTRY(id, enc, wid) 0b##wid,
    TABLE_REG_GEN_ENCODING
#undef ENTRY
};

enum IdLexTok1 id_reg_seg[] = {
#define ENTRY(id, enc, ...) TOK1_##id,
    TABLE_REG_SEG_ENCODING
#undef ENTRY
};

const u8 enc_reg_seg[] = {
#define ENTRY(id, enc, ...) 0b##enc,
    TABLE_REG_SEG_ENCODING
#undef ENTRY
};

struct LexTokLabel
{
    char name[32];
};

struct LexTokNum
{
    long value;
};

struct LexTok
{
    enum IdLexTok0 id0;
    enum IdLexTok1 id1;
    union
    {
        struct LexTokLabel *p_label;
        struct LexTokNum   *p_num;
    };
};

enum IdParserNode
{
    NODE_NONE,
#define ENTRY(id, name) NODE_##id,
    TABLE_NODE_NON_TERMINAL
#undef ENTRY
    NODE_TERMINAL,
    NODE_COUNT,
};

enum IdParserNode id_parser_node[] = {
    NODE_NONE,
#define ENTRY(id, name) NODE_##id,
    TABLE_NODE_NON_TERMINAL
#undef ENTRY
    NODE_TERMINAL,
    NODE_COUNT,
};

const char *names_parser_node[] = {
    "none",
#define ENTRY(id, name) #name,
    TABLE_NODE_NON_TERMINAL
#undef ENTRY
    "terminal",
    "count",
};

struct ParserNode
{
    enum IdParserNode  id;
    struct LexTok     *p_tok;
};

struct EAddr
{
    enum IdLexTok1 reg_base;
    enum IdLexTok1 reg_idx;
};

struct EAddr eaddr[] = {
#define ENTRY(base, idx, ...) {TOK1_##base, TOK1_##idx},
    TABLE_EADDR
#undef ENTRY
};

const u8 enc_rm_eaddr[] = {
#define ENTRY(base, idx, rm) 0b##rm,
    TABLE_EADDR
#undef ENTRY
};

enum InstFieldId
{
#define ENTRY(id, ...) INST_##id,
    OPCODE(1, ...)
    OPCODE(2, ...)
    ImplD(...)
    ImplReg(...)
    TABLE_INST_FIELDS
#undef ENTRY
};

struct InstField
{
    enum InstFieldId id;
    u8 value;
    u8 size;
};

struct Instruction
{
    enum IdLexTok1   id_mnemonic;
    struct InstField fields[15];
};

struct InstructionData
{
    struct ParserNode *p_eaddr__;
    struct ParserNode *p_daddr;
    struct ParserNode *p_imm;
    struct LexTok     *p_comma;
    enum IdLexTok1     mnemonic;
    enum IdLexTok1     reg_seg;
    enum IdLexTok1     reg_base;
    enum IdLexTok1     reg_idx;
    enum IdLexTok1     seg_ovr;
    u8   d;
    u8   w;
    u8   mod;
    u8   rm;
    u8   sr;
    long disp; 
    long daddr;
    long imm;
    u8   operand_cnt;
};

static struct InstructionData inst_data = {0};

struct InstEnc
{
    u64 enc;
    u8  len;
};

struct Instruction inst_table[] = {
#define ENTRY(id, value, len) {INST_##id, 0b##value, len}
    {TOK1_MOV, {OPCODE(1,100010), D, W, MOD, REG, RM, DISP, END}}, 
    {TOK1_MOV, {OPCODE(1,1100011), ImplD(1), W, MOD, OPCODE(2,000), RM, DISP, IMM, END}}, 
    {TOK1_MOV, {OPCODE(1,1011), ImplD(1), W, REG, IMM, END}},
    {TOK1_MOV, {OPCODE(1,1010000), ImplD(1), ImplReg(000), W, ADDR, END}},
    {TOK1_MOV, {OPCODE(1,1010001), ImplD(0), ImplReg(000), W, ADDR, END}},
    {TOK1_MOV, {OPCODE(1,10001110), ImplD(1), MOD, OPCODE(2,0), SR, RM, DISP, END}},
    {TOK1_MOV, {OPCODE(1,10001100), ImplD(0), MOD, OPCODE(2,0), SR, RM, DISP, END}},
    {TOK1_NONE, {0}},
#undef ENTRY
};

#endif // ASM8086_H
