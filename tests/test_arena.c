#include "../arena.c"

void
test_arena(void)
{
    Arena *arena = arena_alloc(MegaByte(1));
    arena_release(arena);

    arena = arena_alloc(10);

    u32 *oneint = push_array_aligned(&arena, u32, 1, AlignOf(u32));
    *oneint = 0xffffffff;

    arena_clear(&arena);
    u64 pos = arena_pos(arena);

    u8 *sixbytes = push_array_aligned(&arena, u8, 6, AlignOf(u8));
    sixbytes[0] = 0xee;
    sixbytes[1] = 0xdd;
    sixbytes[2] = 0xcc;
    sixbytes[3] = 0xbb;
    sixbytes[4] = 0xaa;
    sixbytes[5] = 0x99;

    u8 *twobytes = push_array_aligned(&arena, u8, 2, AlignOf(u8));
    twobytes[0] = 0x88;
    twobytes[1] = 0x77;

    u8 *othertwobytes = push_array_aligned(&arena, u8, 2, AlignOf(u8));
    othertwobytes[0] = 0x66;
    othertwobytes[1] = 0x55;
}

int
main(void)
{
    test_arena();
}
