#ifndef ARENA_H
#define ARENA_H

#include "utils.c"

typedef struct Arena Arena;
struct Arena {
    u64 base;
    u64 pos;
    u64 size;
};

typedef struct Ctx Ctx;
struct Ctx {
    Arena *arenas[2];
};

typedef struct Temp Temp;
struct Temp {
    Arena *arena;
    u64 pos;
};

Ctx *ctx_local;

Arena *arena_alloc(u64 size);
void arena_dealloc(Arena *arena);

void ctx_init(Ctx *ctx);

void *arena_push(Arena *arena, u64 size);
void arena_pop(Arena *arena, u64 size);
void arena_clear(Arena *arena);

Temp temp_begin(Arena *arena);
Temp scratch_get(Arena **conflicts, u64 count);

void temp_end(Temp temp);
void scratch_release(Temp temp);

#endif // ARENA_H
