#ifndef ASM8086_H
#define ASM8086_H

#include <stdint.h>
#include <stdbool.h>

#include "utils.c"
#include "tables.h"

#define LEN_MAX_FILENAME      64
#define LEN_MAX_LABEL         32
#define LEN_MAX_NUM_STR       8
#define LEN_MAX_INST_STACK    10
#define LEN_MAX_INST_BYTES    6 
#define LEN_SEGMENT           K(64)

enum TokenKind
{
    TOK_NULL,
#define ENTRY(id, ...) Glue(TOK_, id),
    TABLE_MNEMONIC
    TABLE_REGISTER_GENERAL
    TABLE_REGISTER_SEGMENT
    TABLE_PREFIX
    TABLE_OPERATORS
#undef ENTRY
    TOK_COUNT
};

const char *token_string[] = {
    "null",
#define ENTRY(id, name, ...) Stringify(name),
    TABLE_MNEMONIC
    TABLE_REGISTER_GENERAL
    TABLE_REGISTER_SEGMENT
    TABLE_PREFIX
    TABLE_OPERATORS
#undef ENTRY
    "count"
};

#endif // ASM8086_H
