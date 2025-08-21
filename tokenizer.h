#ifndef TOKENIZER_H
#define TOKENIZER_H

#include "utils.c"
#include "string.c"
#include "tables.h"

typedef enum TokenKind
{
    TOK_NULL,
#define ENTRY(id, ...) Glue(TOK_, id),
    TABLE_MNEMONIC
    TABLE_REGISTER_GENERAL
    TABLE_REGISTER_SEGMENT
    TABLE_OPERAND_PREFIX_SIZE
    TABLE_PREFIX
    TABLE_OPERATORS
    TABLE_ONE_CHARACTER_TOKEN
#undef ENTRY
    TOK_COUNT,
}
TokenKind;


typedef struct Token Token;
struct Token {
    TokenKind token_kind;
    String    token_view;
    u64       num;
};

typedef struct TokenList TokenList;
struct TokenList {
    Arena *arena;
    Token *token;
    u64    cnt;
};

String token_string[] = {
    string_lit("null"),
#define ENTRY(...) ENTRY_IMPL(__VA_ARGS__, 0)
#define ENTRY_IMPL(id, name, ...) string_lit(Stringify(name)),
    TABLE_MNEMONIC
    TABLE_REGISTER_GENERAL
    TABLE_REGISTER_SEGMENT
    TABLE_OPERAND_PREFIX_SIZE
    TABLE_PREFIX
    TABLE_OPERATORS
    TABLE_ONE_CHARACTER_TOKEN
#undef ENTRY_IMPL
#undef ENTRY
    string_lit("count")
};

// Map character to TokenKind enum
// e.g. character '[' -> TOK_LSQUAREBR
static const TokenKind lut_one_character_token[] = {
#define ENTRY(id, ch) [(ch)] = (Glue(TOK_, id)),
    TABLE_ONE_CHARACTER_TOKEN
#undef ENTRY
};

bool      char_is_space(u8 c);
TokenList token_list_init(Arena *arena);
void      tokenize(TokenList *token_list, String input);
u64       peek_token(TokenList *token_list, String input, u64 idx);
void      token_list_add(TokenList *token_list, TokenKind token_kind, String string, u64 num);

void      token_list_print(TokenList *token_list);

#endif // TOKENIZER_H
