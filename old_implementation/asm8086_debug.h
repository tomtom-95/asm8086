#ifndef ASM8086_DEBUG_H
#define ASM8086_DEBUG_H

#include "asm8086.h"

void print_tok_stack(struct LexTok *tok_stack, struct LexTok *tok_sp);
void print_node_stack(struct ParserNode *node_stack, struct ParserNode *node_sp);

#endif  // ASM8086_DEBUG_H
