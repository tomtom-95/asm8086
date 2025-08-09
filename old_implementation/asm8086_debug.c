#include "asm8086.h"
#include "asm8086_debug.h"

void print_tok_stack(struct LexTok *tok_stack, struct LexTok *tok_sp)
{
    struct LexTok *p_tok = tok_stack;
    while (p_tok != tok_sp)
    {
        printf("{%s", names_lex_tok1[p_tok -> id1]);
        if (p_tok -> id0 == TOK0_NUM)
            printf(" %ld", p_tok -> p_num -> value);
        printf("}\n");

        ++p_tok;
    }
}

void print_node_stack(struct ParserNode *node_stack, struct ParserNode *node_sp)
{
    struct ParserNode *p_node = node_stack;
    while (p_node != node_sp)
    {
        printf("{%s", names_parser_node[p_node -> id]);
        if (p_node -> id == NODE_TERMINAL)
            printf(" %s", names_lex_tok1[p_node -> p_tok -> id1]);
        printf("}\n");

        ++p_node;
    }
}
