#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <inttypes.h>

#include "arena.c"
#include "string.c"
#include "tables.h"
#include "asm8086.h"
#include "tokenizer.c"
#include "parser.c"
// #include "codegen.c"

int main(void)
{
    // TODO: base layer for reading file and do stuff with the file OS independently
    Arena *arena = arena_alloc(MegaByte(1));

    FILE *file_input = fopen("./resources/test_one_instruction.asm", "r");
    assert(file_input != NULL);

    fseek(file_input, 0L, SEEK_END);
    u64 file_len = (u64)ftell(file_input);
    rewind(file_input);

    String input = { .str = arena_push(arena, file_len), .len = file_len };
    assert(fread(input.str, 1, file_len, file_input) == file_len);
    fclose(file_input);

    TokenList token_list = token_list_init(arena);

    u64 idx = 1;
    tokenize(&token_list, input);

    while (idx < token_list.cnt)
    {
        parse(&token_list, &idx);
        if (!idx)
        {
            return 1;
        }
    }
    
    #if 0
    // Write to the output file one byte at time: probably very stupid
    FILE *file_output = fopen("./resources/test_one_instruction.bin", "wb");
    if (!file_output)
    {
        printf("Error: cannot open file\n");
        return 1;
    }

    Arena *output_arena = arena_alloc(MegaByte(1));

    // Start from one, token 0 is a sentinel with dummy value
    u64 idx = 1;

    while (idx < token_list.cnt)
    {
        parse(&token_list, &idx);
        InstructionEncoding enc = codegen();

        u64 num_bytes = enc.len / 8;
        u8 *p = arena_push(output_arena, num_bytes);

        for (u64 i = 0; i < num_bytes; ++i)
        {
            u64 offset = num_bytes - i - 1;
            u8 b = (u8)((enc.encoding >> (8 * offset)) & 0xFF);
            p[i] = b;
        }

        printf("Encoding: 0x%" PRIx64 "\n", enc.encoding); 
    }

    u8 *base = (u8*)output_arena + output_arena->base;
    fwrite(base, output_arena->pos, 1, file_output);

    fclose(file_output);

    #endif

    return 0;
}
