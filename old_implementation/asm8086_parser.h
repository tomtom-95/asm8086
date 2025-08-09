#ifndef ASM8086_PARSER_H
#define ASM8086_PARSER_H

#include "asm8086.h"
#include "asm8086_lexer.h"
#include "asm8086_parser.h"

int parser_line_(struct ParserNode **pp_node, struct LexTok **pp_tok);
int parser_line(struct ParserNode **pp_node, struct LexTok **pp_tok);
int parser_lbl(struct ParserNode **pp_node, struct LexTok **pp_tok);
int parser_inst(struct ParserNode **pp_node, struct LexTok **pp_tok);
int parser_mnemonic_(struct ParserNode **pp_node, struct LexTok **pp_tok);
int parser_operand_(struct ParserNode **pp_node, struct LexTok **pp_tok);
int parser_opr_prefix(struct ParserNode **pp_node, struct LexTok **pp_tok);
int parser_opr_size(struct ParserNode **pp_node, struct LexTok **pp_tok);
int parser_seg_ovr(struct ParserNode **pp_node, struct LexTok **pp_tok);
int parser_operand(struct ParserNode **pp_node, struct LexTok **pp_tok);
int parser_reg_gen(struct ParserNode **pp_node, struct LexTok **pp_tok);
int parser_reg_seg(struct ParserNode **pp_node, struct LexTok **pp_tok);
int parser_imm(struct ParserNode **pp_node, struct LexTok **pp_tok);
int parser_opr_math(struct ParserNode **pp_node, struct LexTok **pp_tok);
int parser_prefix(struct ParserNode **pp_node, struct LexTok **pp_tok);
int parser_mnemonic(struct ParserNode **pp_node, struct LexTok **pp_tok);
int parser_eaddr__(struct ParserNode **pp_node, struct LexTok **pp_tok);
int parser_eaddr_(struct ParserNode **pp_node, struct LexTok **pp_tok);
int parser_eaddr(struct ParserNode **pp_node, struct LexTok **pp_tok);
int parser_direct_addr(struct ParserNode **pp_node, struct LexTok **pp_tok);
int parser_signed_num(struct ParserNode **pp_node, struct LexTok **pp_tok);
int parser_signed_zero(struct ParserNode **pp_node, struct LexTok **pp_tok);
int parser_reg_base(struct ParserNode **pp_node, struct LexTok **pp_tok);
int parser_reg_idx(struct ParserNode **pp_node, struct LexTok **pp_tok);

void parser_add_terminal(struct ParserNode **pp_node,
                         struct LexTok     **pp_tok);

void parser_backtrack(struct ParserNode **pp_node,
                      struct ParserNode  *p_reset_node,
                      struct LexTok     **pp_tok,
                      struct LexTok      *p_reset_tok);

void store_mod(void);
void store_rm(void);

#endif // ASM8086_PARSER_H
