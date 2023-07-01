#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "asm8086.h"

#include "asm8086_helper.c"
#include "asm8086_lexer.c"
#include "asm8086_debug.c"
#include "asm8086_parser.c"
#include "asm8086_codegen.c"

int main(int argc, char **argv)
{
    FILE *asm_fp = fopen(argv[1], "r");
    assert(asm_fp != NULL);

    fseek(asm_fp, 0, SEEK_END);
    size_t asm_filelen = (size_t)(ftell(asm_fp) + 1);
    rewind(asm_fp);

    size_t asm_bufferlen = asm_filelen - 1;
    char *asm_buffer = malloc((asm_bufferlen + 1) * sizeof(*asm_buffer));
    assert(asm_buffer != NULL);

    fread(asm_buffer, 1, asm_bufferlen + 1, asm_fp);
    assert(ferror(asm_fp) == 0);
    assert(feof(asm_fp) != 0);
    assert(asm_buffer[asm_bufferlen - 1] == '\n');
    fclose(asm_fp);

    assert(strlen(argv[1]) < LEN_MAX_FILENAME - 1);
    char basename[LEN_MAX_FILENAME];
    get_basename(argv[1], basename);

    FILE *bin_fp = fopen(basename, "wb");
    assert(bin_fp != NULL);

    struct LexTok *tok_stack = malloc(K(1) * sizeof(*tok_stack));
    assert(tok_stack != NULL);
    struct LexTok *tok_sp = tok_stack;

    struct LexTokLabel *lbl_stack = malloc(K(1) * sizeof(*lbl_stack));
    assert(lbl_stack != NULL);
    struct LexTokLabel *lbl_sp = lbl_stack;

    struct LexTokNum *num_stack = malloc(K(1) * sizeof(*num_stack));
    assert(num_stack != NULL);
    struct LexTokNum *num_sp = num_stack;

    char *p_asm_buf = asm_buffer;
    if (lexer(&tok_sp, &p_asm_buf, &lbl_sp, &num_sp) != 0)
        return printf("lexer error\n"), 1;
    else
        printf("lexer success\n");

    struct ParserNode *node_stack = malloc(K(1) * sizeof(*node_stack));

    struct LexTok *p_tok       = tok_stack;
    struct ParserNode *node_sp = node_stack;
    while (p_tok != tok_sp)
    {
        memset(&inst_data, 0, sizeof(inst_data));

        struct LexTok *p_tok_start      = p_tok;
        struct ParserNode *p_node_start = node_sp;

        if (parser_line_(&node_sp, &p_tok) == 0)
            printf("parser success\n");
        else
            printf("parser error\n");

        print_node_stack(p_node_start, node_sp);

        struct InstEnc encoding = {0};
        if (codegen(p_node_start, p_tok_start, &encoding) == 0)
            fwrite(&(encoding.enc), encoding.len, 1, bin_fp);
        else
            printf("codegen error\n");
    }

    free(asm_buffer);
    free(tok_stack);
    free(lbl_stack);
    free(num_stack);
    free(node_stack);

    fclose(bin_fp);

    return 0;
}
