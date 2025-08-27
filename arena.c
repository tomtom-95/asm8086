#ifndef ARENA_C
#define ARENA_C

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <inttypes.h>
#include <string.h>
#include <stdbool.h>

#include "utils.c"

#include "arena.h"

internal Arena *
arena_alloc(u64 size)
{
    u64 header_size = sizeof(Arena);
    Arena *arena = malloc(header_size + size);
    assert(arena);

    arena->prev = NULL;
    arena->pos  = header_size;
    arena->size = header_size + size;

    return arena;
}

internal void
ctx_init(Ctx *ctx)
{
    Arena **arena_ptr = ctx->arenas;
    for (u64 i = 0; i < ArrayCount(ctx->arenas); ++i, arena_ptr += 1)
    {
        *arena_ptr = arena_alloc(MegaByte(64));
    }
    ctx_local = ctx;
}

internal void
arena_release(Arena *arena)
{
    Arena *curr = arena;
    while (curr)
    {
        Arena *prev = curr->prev;
        free(curr);
        curr = prev;
    }
}

internal void
arena_clear(Arena **arena)
{
    Arena *curr = *arena;
    while (curr->prev)
    {
        Arena *prev = curr->prev;
        free(curr);
        curr = prev;
    }
    curr->pos = sizeof(Arena);
    *arena    = curr;
}


internal u64
arena_pos(Arena *arena)
{
    return arena->pos;
}

internal u64
align_forward(u64 pos, u64 align)
{
    u64 modulo = pos & (align - 1);
    if (modulo)
    {
        pos += align - modulo;
    }

    return pos;
}

internal void *
arena_push(Arena **arena, u64 size, u64 align)
{
    for (;;)
    {
        u64 start = align_forward((*arena)->pos, align);

        if (start + size <= (*arena)->size)
        {
            u8 *result = (u8 *)(*arena) + start;
            (*arena)->pos = start + size;
            return result;
        }
        else
        {
            Arena *next = arena_alloc(2 * (*arena)->size);
            next->prev  = *arena;
            *arena      = next;
        }
    }
}

internal Temp
temp_begin(Arena *arena)
{
    return (Temp){ .arena = arena, .pos = arena->pos };
}

internal void
temp_end(Temp temp)
{
    temp.arena->pos = temp.pos;
}

internal Temp
scratch_get(Arena **conflicts, u64 count)
{
    Arena *result = 0;
    Arena **arena_ptr = ctx_local->arenas;
    for (u64 i = 0; i < ArrayCount(ctx_local->arenas); ++i, ++arena_ptr)
    {
        bool has_conflict = false;
        Arena **conflict_ptr = conflicts;
        for (u64 j = 0; j < count; ++j, ++conflict_ptr)
        {
            if (*arena_ptr == *conflict_ptr)
            {
                has_conflict = true;
                break;
            }
        }
        if (!has_conflict)
        {
            result = *arena_ptr;
            break;
        }
    }
    return temp_begin(result);
}

internal void
scratch_release(Temp temp)
{
    temp_end(temp);
}

#endif // ARENA_C
