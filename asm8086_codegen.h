#ifndef ASM8086_CODEGEN_H
#define ASM8086_CODEGEN_H

#include "asm8086.h"
#include "asm8086_lexer.h"

int codegen(struct ParserNode *p_node_start,
            struct LexTok     *p_tok_start,
            struct InstEnc    *encoding);

int instgen(struct Instruction  inst,
            struct ParserNode  *p_node_start,
            struct LexTok      *p_tok_start,
            u64                *p_inst_enc);

struct LexTok *get_reg_gen(struct LexTok    *p_tok_start,
                           enum InstFieldId  field_id);

struct LexTok *get_reg_gen_src(struct LexTok *p_tok_comma);
struct LexTok *get_reg_gen_dst(struct LexTok *p_tok_start);

int set_p_comma(struct LexTok *p_tok_start);
int set_d(struct LexTok *p_tok_start);
int set_rm(struct LexTok *p_tok_start);
int set_mod(void);

#endif // ASM8086_CODEGEN_H
