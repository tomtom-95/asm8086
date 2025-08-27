#ifndef ARENA_H
#define ARENA_H

#include "utils.c"

#define AlignOf(T) __alignof__(T)

// #if COMPILER_MSVC
// #define AlignOf(T) __alignof(T)
// #elif COMPILER_CLANG
// #define AlignOf(T) __alignof(T)
// #elif COMPILER_GCC
// #define AlignOf(T) __alignof__(T)
// #elif GNUC
// #define AlignOf(T) alignof(T)
// #else
// #error AlignOf not defined for this compiler.
// #endif

typedef struct Arena Arena;
struct Arena {
    Arena *prev;
    u64    pos;
    u64    size;
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

internal Arena *arena_alloc(u64 size);
internal void   arena_release(Arena *arena);

internal void   ctx_init(Ctx *ctx);

internal u64    arena_pos(Arena *arena);
internal void  *arena_push(Arena **arena, u64 size, u64 align);
internal void   arena_clear(Arena **arena);

internal Temp   temp_begin(Arena *arena);
internal Temp   scratch_get(Arena **conflicts, u64 count);

internal void   temp_end(Temp temp);
internal void   scratch_release(Temp temp);

#define MemoryZero(s,z)       memset((s),0,(z))

//- rjf: push helper macros
#define push_array_no_zero_aligned(a, T, c, align) (T *)arena_push((a), sizeof(T)*(c), (align))
#define push_array_aligned(a, T, c, align) (T *)MemoryZero(push_array_no_zero_aligned(a, T, c, align), sizeof(T)*(c))
#define push_array_no_zero(a, T, c) push_array_no_zero_aligned(a, T, c, Max(8, AlignOf(T)))
#define push_array(a, T, c) push_array_aligned(a, T, c, Max(8, AlignOf(T)))

#endif // ARENA_H
