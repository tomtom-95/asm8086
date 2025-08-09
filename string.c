#ifndef STRING_C
#define STRING_C

// NOTE: Once created the content of the string is immutable
//       I can create new String view into the same data

#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include "utils.c"
#include "arena.c"

////////////////////////////////
//~ NOTE(allen): String <-> Integer Tables

read_only global u8 integer_symbols[16] = {
  '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F',
};

// NOTE(allen): Includes reverses for uppercase and lowercase hex.
read_only global u8 integer_symbol_reverse[128] = {
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
};

read_only global u8 base64[64] = {
  '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
  'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
  'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
  'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
  'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
  '_', '$',
};

read_only global u8 base64_reverse[128] = {
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0x3F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,
  0xFF,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F,0x30,0x31,0x32,
  0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,0xFF,0xFF,0xFF,0xFF,0x3E,
  0xFF,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,
  0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,0x20,0x21,0x22,0x23,0xFF,0xFF,0xFF,0xFF,0xFF,
};

typedef struct String String;
struct String {
    u8 *str;
    u64 len;
};

typedef struct StringNode StringNode;
struct StringNode {
    String str; 
    StringNode *next;
};

typedef struct StringList StringList;
struct StringList {
    StringNode *head;
    u64 len;
};

// #define string_lit(S) string((u8 *)(S), sizeof(S) - 1)
#define string_lit(S) (String){ .str = (u8 *)(S), .len = sizeof(S) - 1 }

String
string(u8 *str, u64 size)
{
    return (String){str, size};
}

String
push_string_copy(Arena *arena, String s)
{
    String str;
    str.len = s.len;
    str.str = arena_push(arena, str.len);
    memcpy(str.str, s.str, s.len);

    return(str);
}

String
push_string_cat(Arena *arena, String s1, String s2)
{
    u64 len = s1.len + s2.len;
    String str = { .str = arena_push(arena, len), .len = len };
    memcpy(str.str, s1.str, s1.len);
    memcpy(str.str + s1.len, s2.str, s2.len);

    return str;
}

u64
cstring_len(u8 *cstring)
{
    u8 *p = cstring;
    while (*p++);
    return (u64)(p - cstring);
}

void
cstring_copy(u8 *dst, u8 *src)
{
    while ((*dst++ = *src++));
}

u8 *
cstring_from_string(Arena *arena, String string)
{
    u8 *cstring = arena_push(arena, string.len + 1);
    memcpy(cstring, string.str, string.len);
    cstring[string.len] = '\0';

    return cstring;
}

String
string_strip(String str)
{
    u64 start = 0;
    u64 end   = str.len;

    while (start < str.len && isspace((unsigned char)str.str[start]))
        ++start;

    while (end > start && isspace((unsigned char)str.str[end - 1]))
        --end;

    return (String){ .str = str.str + start, .len = end - start};
}

bool
string_cmp(String s1, String s2)
{
    return s1.len == s2.len && (memcmp(s1.str, s2.str, s1.len) == 0); 
}

void
string_print(String string)
{
    printf("%.*s\n", (int)string.len, string.str); 
}

StringNode *
string_list_push(Arena *arena, StringList *list, String str)
{
    StringNode **node = &(list->head);

    while (*node)
        node = &((*node)->next);

    *node = arena_push(arena, sizeof(StringNode));
    (*node)->str = str;
    ++(list->len);

    return *node;
}

StringNode *
string_list_find(StringList *list, String str)
{
    StringNode *node = list->head;
    while (node) {
        if (string_cmp(node->str, str))
            return node;
        else
            node = node->next;
    }

    return NULL;
}

bool
char_is_upper(u8 c)
{
    return('A' <= c && c <= 'Z');
}

bool
char_is_lower(u8 c)
{
    return('a' <= c && c <= 'z');
}

bool
char_is_alpha(u8 c)
{
    return(char_is_upper(c) || char_is_lower(c));
}

bool
char_is_slash(u8 c)
{
    return(c == '/' || c == '\\');
}

bool
char_is_digit(u8 c, u32 base)
{
    bool result = 0;
    if (0 < base && base <= 16) {
        u8 val = integer_symbol_reverse[c];
        if (val < base) {
            result = 1;
        }
    }
    return(result);
}

u8
char_to_lower(u8 c)
{
    if (char_is_upper(c)) {
        c += ('a' - 'A');
    }
    return(c);
}

u8
char_to_upper(u8 c)
{
    if (char_is_lower(c)) {
        c += ('A' - 'a');
    }
    return(c);
}

u64
u64_from_string(String string, u32 radix)
{
    u64 x = 0;
    if (1 < radix && radix <= 16) {
        for (u64 i = 0; i < string.len; i += 1) {
            x *= radix;
            x += integer_symbol_reverse[string.str[i]&0x7F];
        }
    }
    return(x);
}

#endif // STRING_C
