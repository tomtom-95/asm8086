#include "utils.c"
#include "tables.h"
#include "tokenizer.h"
#include "parser.h"

typedef enum InstFieldId
{
    INST_NULL,
#define ENTRY(name, bits, size) Glue(INST_, name),
    INST_FIELD_LIST
#undef ENTRY
    INST_COUNT
}
InstFieldId;

typedef struct InstField InstField;
struct InstField
{
    InstFieldId id;
    u64         value;
    u8          bitlen;
};

typedef struct InstEncoding InstEncoding;
struct InstEncoding
{
    u64 encoding;
    u8  bitlen;
};

typedef struct InstTableKey InstTableKey;
struct InstTableKey
{
    TokenKind   mnemonic;
    OperandKind dst;
    OperandKind src;
};

InstTableKey instruction_table_key[] = {
#define INST(mnemonic, dst, src, ...) { Glue(TOK_, mnemonic), Glue(OP_, dst), Glue(OP_, src) },
    TABLE_INSTRUCTION_V2
#undef INST
};

typedef struct InstTableEntry InstTableEntry;
struct InstTableEntry
{
    InstField inst_fields[16];
};

InstTableEntry instruction_table[] = {
#define INST(mnemonic, dst, src, ...) __VA_ARGS__,
#define ENTRY(id, value, len) { Glue(INST_, id), Glue(0b, value), len }
    TABLE_INSTRUCTION_V2
#undef ENTRY
#undef INST
};

/* Encoding of table 4.10 in Intel manual */
internal u64        encode_rm(TokenKind base, TokenKind index); 
internal u64        encode_mod(EffectiveAddress eaddr);

InstEncoding        codegen(void);
