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
#include "codegen.c"

int main(void)
{
    Arena *arena = arena_alloc(MegaByte(1));

    FILE *file_input = fopen("./resources/test_one_instruction.asm", "r");
    assert(file_input != NULL);

    fseek(file_input, 0L, SEEK_END);
    u64 file_len = (u64)ftell(file_input);
    rewind(file_input);

    String input = { .str = push_array(&arena, u8, file_len), .len = file_len };
    assert(fread(input.str, 1, file_len, file_input) == file_len);
    fclose(file_input);

    TokenList token_list = token_list_init(arena);

    tokenize(&token_list, input);

    FILE *file_output = fopen("./resources/test_one_instruction.bin", "wb");
    if (!file_output)
    {
        printf("Error: cannot open file\n");
        return 1;
    }

    Arena *codegen_arena = arena_alloc(MegaByte(10));
    g_map_label          = maplabel_init(codegen_arena, 1);
    list_addrtopatch     = list_addrtopatch_init(codegen_arena, 100);

    u64 encoding_start_pos = codegen_arena->pos;
    u8 *encoding_start_ptr = (u8 *)codegen_arena + encoding_start_pos;

    // TODO: now I want to divide the job in two pass:
    // in the first pass I can generate code for all the instruction except for jump instruction
    // I will always encode the jmp instruction as 3 bytes
    // In the second pass I will encode the jmp instruction by inserting the necessary addresses
    u64 idx = 1;
    while (idx < token_list.cnt)
    {
        instruction_data = (InstructionData){0}; 
        if (!parse(&token_list, &idx))
        {
            return 1;
        }

        InstEncoding enc = codegen();

        u64 num_bytes = enc.bitlen / 8;
        g_instruction_pointer += num_bytes;
        u8 *p = push_array_no_zero_aligned(&codegen_arena, u8, num_bytes, 1);

        for (u64 i = 0; i < num_bytes; ++i)
        {
            u64 offset = num_bytes - i - 1;
            u8 b = (u8)((enc.encoding >> (8 * offset)) & 0xFF);
            p[i] = b;
        }

        printf("Encoding: 0x%" PRIx64 "\n", enc.encoding); 
    }

    /*
        Patching of addresses for jump instruction that could not be resolved during the first pass of codegen
        Imagine this situation: 
            mov si, bx
            jmp mylabel
            mylabel:
                mov dh, al
        Right now I am emitting code as soon as a line is parsed (parse mov si, bx -> emit x89de)
        This means that when I arrive at jmp label I still do not know where mylabel is exactly
        What I do is to emit the opcode of the jmp instruction (1 byte) and record the fact that
        2 bytes must be patched with mylabel relative address (absolute addr of mylabel - absolute addr of jmp instruction)
    */
    for (u64 i = 0; i < list_addrtopatch.cnt; ++i)
    {
        AddrToPatch addr = list_addrtopatch.addr_to_patch[i];
        u64 map_idx = maplabel_find(&g_map_label, addr.labelname);

        u16 address_to_write = (u16)g_map_label.collision_array[map_idx].label.pos;
        u16 start_write = addr.inst_pointer;
        assert(address_to_write > start_write);

        u16 diff = address_to_write - (start_write + addr.bytelen);

        u8 *p = (u8 *)codegen_arena + encoding_start_pos + start_write;

        if (addr.bytelen == 1)
        {
            *p = diff & 0xff;
        }
        else if (addr.bytelen == 2)
        {
            *p = diff & 0xff;
            *(p + 1) = (diff >> 8) & 0xff;
        }
    }
    
    fwrite(encoding_start_ptr, codegen_arena->pos - encoding_start_pos, 1, file_output);
    fclose(file_output);

    return 0;
}
