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
    OP_LABEL        = (1u << 11),
    OP_POINTER      = (1u << 12)
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

// Note: A terrible name 
typedef struct Pointer Pointer;
struct Pointer {
    s16 selector; // A 16-bit selector for the target code segment
    s16 offset;   // A 16-bit or 32-bit offset for the instruction within the target segment
};

typedef struct Operand Operand;
struct Operand {
    OperandKind      operand_kind;
    TokenKind        register_general;
    TokenKind        register_segment;
    EffectiveAddress effective_address;
    s16              immediate;
    String           label;
    Pointer          pointer;
};

typedef struct Operand_ Operand_;
struct Operand_ {
    PrefixOperand prefix_operand;
    Operand       operand;
};

typedef struct InstructionData InstructionData;
struct InstructionData {
    TokenKind mnemonic;
    Operand_  dst;
    Operand_  src; 
};

typedef struct Label Label;
struct Label {
    String name;
    u64    pos;
};

typedef struct MapLabelEl MapLabelEl;
struct MapLabelEl {
    Label label;
    u64   next; 
};

typedef struct MapLabel MapLabel;
struct MapLabel {
    u64        *index_array;
    MapLabelEl *collision_array;
    u64         offset;
    u64         first_free;
    u64         bucket_count;
};

// Data structure for the patching of addresses
typedef struct AddrToPatch AddrToPatch;
struct AddrToPatch {
    String labelname;
    u64    inst_pointer;
    u8     bytelen;
};

typedef struct ListAddrToPatch ListAddrToPatch;
struct ListAddrToPatch {
    AddrToPatch *addr_to_patch;
    u64          cnt;
    u64          len;
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
internal u64  eaddr_encode_rm(TokenKind base, TokenKind index); 
internal u64  eaddr_encode_mod(EffectiveAddress eaddr);

internal s32  is_base(TokenKind r);
internal s32  is_index(TokenKind r);

internal bool parse(TokenList *token_list, u64 *idx);
internal bool parse_line_(TokenList *token_list, u64 *idx);
internal bool parse_line(TokenList *token_list, u64 *idx);
internal bool parse_label(TokenList *token_list, u64 *idx, Label *label);
internal bool parse_instruction(TokenList *token_list, u64 *idx, InstructionData *instruction);
internal bool parse_instruction_tail(TokenList *token_list, u64 *idx, InstructionData *instruction);
internal bool parse_mnemonic_(TokenList *token_list, u64 *idx, TokenKind *mnemonic);
internal bool parse_operand_(TokenList *token_list, u64 *idx, Operand_ *operand_);
internal bool parse_opr_prefix(TokenList *token_list, u64 *idx, PrefixOperand *prefix_operand);
internal bool parse_prefix(TokenList *token_list, u64 *idx);
internal bool parse_mnemonic(TokenList *token_list, u64 *idx, TokenKind *mnemonic);
internal bool parse_operand(TokenList *token_list, u64 *idx, Operand *operand);
internal bool parse_pointer(TokenList *token_list, u64 *idx, Pointer *pointer);
internal bool parse_register_general(TokenList *token_list, u64 *idx, TokenKind *reg_gen);

internal bool parse_eaddr__(TokenList *token_list, u64 *idx, EffectiveAddress *eaddr);
internal bool parse_eaddr_(TokenList *token_list, u64 *idx, EffectiveAddress *eaddr);
internal bool parse_eaddr(TokenList *token_list, u64 *idx, EffectiveAddress *eaddr);
internal bool parse_register_base(TokenList *token_list, u64 *idx, TokenKind *reg_base);
internal bool parse_register_index(TokenList *token_list, u64 *idx, TokenKind *reg_idx);
internal bool parse_signed_num(TokenList *token_list, u64 *idx, s16 *signed_num);
internal bool parse_direct_address(TokenList *token_list, u64 *idx, s16 *direct_address);

internal bool parse_register_segment(TokenList *token_list, u64 *idx, TokenKind *reg_seg);
internal bool parse_imm(TokenList *token_list, u64 *idx, s16 *imm);
internal bool parse_opr_math(TokenList *token_list, u64 *idx, TokenKind *opr_math);
internal bool parse_opr_size(TokenList *token_list, u64 *idx, PrefixSize *prefix_size);
internal bool parse_seg_ovr(TokenList *token_list, u64 *idx, PrefixSegOvr *prefix_seg_ovr);

internal bool parse_terminal(TokenList *token_list, u64 *idx, TokenKind token_kind);

internal u64      hash_string(String str);
internal MapLabel maplabel_init(Arena *arena, u64 bucket_count);
internal u64      maplabel_find(MapLabel *maplabel, String labelname);
internal void     maplabel_insert(MapLabel *maplabel, Label label);
internal void     maplabel_pop(MapLabel *maplabel, String labelname);


u64             g_instruction_pointer = 0;
InstructionData instruction_data = {0};
MapLabel        g_map_label = {0};
ListAddrToPatch list_addrtopatch = {0};

#endif // PARSER_H
