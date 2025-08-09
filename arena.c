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

// TODO: proper align on push and pop
Arena *
arena_alloc(u64 size) {
    Arena *arena = malloc(sizeof(Arena) + size);
    assert(arena);

    arena->base = sizeof(Arena);
    arena->pos  = 0;
    arena->size = size;

    return arena;
}

void
ctx_init(Ctx *ctx) {
    Arena **arena_ptr = ctx->arenas;
    for (u64 i = 0; i < ArrayCount(ctx->arenas); ++i, arena_ptr += 1) {
        *arena_ptr = arena_alloc(MegaByte(64));
    }
    ctx_local = ctx;
}

void
arena_dealloc(Arena *arena) {
    free(arena);
    arena = NULL;
}

void *
arena_push(Arena *arena, u64 size) {
    // TODO: this must become a proper handling
    assert(arena->pos + size <= arena->size);
    void *result = (u8 *)arena + arena->base + arena->pos;
    arena->pos += size;
    memset(result, 0, size);
    return result;
}

void
arena_pop(Arena *arena, u64 size) {
    assert(arena->pos >= size);
    arena->pos -= size;
}

void
arena_clear(Arena *arena) {
    arena->pos = 0;
}

Temp
temp_begin(Arena *arena) {
    return (Temp){ .arena = arena, .pos = arena->pos };
}

void
temp_end(Temp temp) {
    temp.arena->pos = temp.pos;
}

Temp
scratch_get(Arena **conflicts, u64 count) {
    Arena *result = 0;
    Arena **arena_ptr = ctx_local->arenas;
    for (u64 i = 0; i < ArrayCount(ctx_local->arenas); ++i, ++arena_ptr) {
        bool has_conflict = false;
        Arena **conflict_ptr = conflicts;
        for (u64 j = 0; j < count; ++j, ++conflict_ptr) {
            if (*arena_ptr == *conflict_ptr) {
                has_conflict = true;
                break;
            }
        }
        if (!has_conflict) {
            result = *arena_ptr;
            break;
        }
    }
    return temp_begin(result);
}

void
scratch_release(Temp temp) {
    temp_end(temp);
}

#endif // ARENA_C
