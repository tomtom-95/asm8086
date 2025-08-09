#include "../arena.c"
#include "../string.c"

void
test_string(void)
{
    Arena *arena = arena_alloc(MegaByte(1));

    u8 str[] = "Hello World!";

    String some_string = string_lit(" Hello World!     ");
    String other_string = string_lit(str);
    String stripped = string_strip(some_string);
    String copied = push_string_copy(arena, stripped);
}

int
main(void)
{
    test_string();
}
