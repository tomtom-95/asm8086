#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "arena.c"
#include "string.c"
#include "tables.h"
#include "asm8086.h"
#include "tokenizer.c"

int main(void)
{
    // TODO: base layer for reading file and do stuff with the file OS independently
    Arena *arena = arena_alloc(MegaByte(1));

    FILE *file_input = fopen("./resources/test_push.asm", "r");
    assert(file_input != NULL);

    fseek(file_input, 0L, SEEK_END);
    u64 file_len = (u64)ftell(file_input);
    rewind(file_input);

    String input = { .str = arena_push(arena, file_len), .len = file_len };
    assert(fread(input.str, 1, file_len, file_input) == file_len);
    fclose(file_input);

    TokenList token_list = token_list_init(arena);

    tokenize(&token_list, input);

    token_list_print(&token_list);

    return 0;
}
