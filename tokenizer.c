#ifndef TOKENIZER_C
#define TOKENIZER_C

#include <stdbool.h>
#include "string.c"
#include "asm8086.h"
#include "tokenizer.h"

bool
char_is_space(u8 c)
{
    return(c == ' ' || c == '\t' || c == '\r' || c == '\f' || c == '\v');
}

TokenList
token_list_init(Arena *arena)
{
    Token *token = push_array(&arena, Token, 1);
    TokenList token_list = { .arena = arena, .cnt = 1, .token = token };

    return token_list;
}

void
token_list_add(TokenList *token_list, TokenKind token_kind, String string, u64 num)
{
    Token *token = push_array(&token_list->arena, Token, 1);

    token->token_kind = token_kind;
    token->token_view = string;
    token->num = num;
    ++token_list->cnt;
}

void
token_list_print(TokenList *token_list)
{
    for (u64 idx = 0; idx < token_list->cnt; ++idx)
    {
        string_print(token_string[(token_list->token + idx)->token_kind]);
    }
}

u64
peek_token(TokenList *token_list, String input, u64 idx)
{
    TokenKind token_kind = TOK_NULL;
    String string_view = {0};
    u64 num = 0;

    u64 start = idx;
    u64 end   = idx;

    u8 ch = *(input.str + start);

    if (ch == '[' || ch == ']' || ch == '+' || ch == '-' ||
        ch == ':' || ch == ',' || ch == '0' || ch == '\n')
    {
        token_kind = lut_one_character_token[ch];
        string_view = (String){ .str = input.str + start, .len = 1 };
        ++end;
    }
    else if (char_is_alpha(ch))
    {
        while (char_is_alpha(*(input.str + end)))
            ++end;

        string_view = (String){ .str = input.str + start, .len = end - start };

        for (u64 i = 0; i < ArrayCount(token_string); ++i) {
            if (string_cmp(string_view, token_string[i])) {
                token_kind = (TokenKind)i;
                break;
            }
        }

        if (token_kind == TOK_NULL)
        {
            token_kind = TOK_LABEL;
        }
    }
    else if (char_is_digit(ch, 10))
    {
        while (ch = *(input.str + end), 0 <= ch && ch < 128 && char_is_digit(*(input.str + end), 10))
            ++end;

        string_view = (String){ .str = input.str + start, .len = end - start };
        num = u64_from_string(string_view, 10);
        token_kind = TOK_NUM;
    }

    token_list_add(token_list, token_kind, string_view, num);

    return end;
}

void
tokenize(TokenList *token_list, String input)
{
    u64 idx = 0;
    while (idx < input.len)
    {
        while (char_is_space(*(input.str + idx)))
        {
            ++idx;
        }

        idx = peek_token(token_list, input, idx);
    }

    if ((token_list->token + token_list->cnt)->token_kind != TOK_EOL)
    {
        // If the last line of the file does not have a '\n' add a
        // TOK_EOL anyway to make the parser happy later
        token_list_add(token_list, TOK_EOL, (String){0}, 0);
    }
}

#endif // TOKENIZER_C
