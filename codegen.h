#include "utils.c"
#include "tables.h"
#include "tokenizer.h"
#include "parser.h"

typedef enum InstDescription
{
    DESCRIPTION_NULL, 
#define INST(mnemonic, description, ...) description,
    TABLE_INSTRUCTION
#undef INST
    DESCRIPTION_COUNT
}
InstDescription;

typedef enum InstFieldId
{
    INST_NULL,
#define ENTRY(id, ...) Glue(INST_, id),
    OPCODE(...)
    TABLE_INST_FIELDS
#undef ENTRY
    INST_COUNT
}
InstFieldId;

typedef struct InstField InstField;
struct InstField
{
    enum InstFieldId id;
    u64 value;

    // number of bits the instruction field occupy in the encoding
    u8 size;
};

typedef struct Instruction Instruction;
struct Instruction
{
    TokenKind mnemonic;
    InstDescription instruction_description;
    InstField fields[15];
};

typedef struct InstructionEncoding InstructionEncoding;
struct InstructionEncoding
{
    u64 encoding;
    
    // len in bits, used while encoding the instruction
    u8 len;
};

struct Instruction inst_table[] = {
    {0, {0}, 0},
#define INST(mnemonic, description, ...) { Glue(TOK_, mnemonic), description, __VA_ARGS__ },
#define ENTRY(id, value, len) { Glue(INST_, id), Glue(0b, value), len }
    TABLE_INSTRUCTION
#undef ENTRY
#undef INST
};

/* Encoding of table 4.10 in Intel manual */
internal u64        encode_rm(TokenKind base, TokenKind index); 
internal u64        encode_mod(EffectiveAddress eaddr);

InstructionEncoding codegen(void);
