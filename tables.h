#ifndef TABLES_H
#define TABLES_H

#define TABLE_MNEMONIC  \
    ENTRY(MOV,   mov)   \
    ENTRY(PUSH,  push)  \
    ENTRY(POP,   pop)   \
    ENTRY(XCHG,  xchg)  \
    ENTRY(IN,    in)    \
    ENTRY(OUT,   out)   \
    ENTRY(XLAT,  xlat)  \
    ENTRY(LEA,   lea)   \
    ENTRY(LDS,   lds)   \
    ENTRY(LES,   les)   \
    ENTRY(LAHF,  lahf)  \
    ENTRY(SAHF,  sahf)  \
    ENTRY(PUSHF, pushf) \
    ENTRY(POPF,  popf)  \
    ENTRY(ADD,   add)   \
    ENTRY(ADC,   adc)   \
    ENTRY(INC,   inc)   \
    ENTRY(AAA,   aaa)   \
    ENTRY(DAA,   daa)   \
    ENTRY(SUB,   sub)   \
    ENTRY(SBB,   sbb)   \
    ENTRY(DEC,   dec)   \
    ENTRY(NEG,   neg)   \
    ENTRY(CMP,   cmp)   \
    ENTRY(AAS,   aas)   \
    ENTRY(DAS,   das)   \
    ENTRY(MUL,   mul)   \
    ENTRY(IMUL,  imul)  \
    ENTRY(AAM,   aam)   \
    ENTRY(DIV,   div)   \
    ENTRY(IDIV,  idiv)  \
    ENTRY(AAD,   aad)   \
    ENTRY(CBW,   cbw)   \
    ENTRY(CWD,   cwd)   \
    ENTRY(NOT,   not)   \
    ENTRY(SHL,   shl)   \
    ENTRY(SAL,   sal)   \
    ENTRY(SHR,   shr)   \
    ENTRY(SAR,   sar)   \
    ENTRY(ROL,   rol)   \
    ENTRY(ROR,   ror)   \
    ENTRY(RCL,   rcl)   \
    ENTRY(RCR,   rcr)   \
    ENTRY(JMP,   jmp)   \

// Chapter 4, Table 4.9: REG Field Encoding
// NOTE: the same table applies in MOD=11 case when encoding R/M field (see table 4.10)
#define TABLE_REGISTER_GENERAL               \
    ENTRY(AL, al, 0, 0, OP_REG8  | OP_ACC8 ) \
    ENTRY(AX, ax, 0, 1, OP_REG16 | OP_ACC16) \
    ENTRY(CL, cl, 1, 0, OP_REG8            ) \
    ENTRY(CX, cx, 1, 1, OP_REG16           ) \
    ENTRY(DL, dl, 2, 0, OP_REG8            ) \
    ENTRY(DX, dx, 2, 1, OP_REG16           ) \
    ENTRY(BL, bl, 3, 0, OP_REG8            ) \
    ENTRY(BX, bx, 3, 1, OP_REG16           ) \
    ENTRY(AH, ah, 4, 0, OP_REG8            ) \
    ENTRY(SP, sp, 4, 1, OP_REG16           ) \
    ENTRY(CH, ch, 5, 0, OP_REG8            ) \
    ENTRY(BP, bp, 5, 1, OP_REG16           ) \
    ENTRY(DH, dh, 6, 0, OP_REG8            ) \
    ENTRY(SI, si, 6, 1, OP_REG16           ) \
    ENTRY(BH, bh, 7, 0, OP_REG8            ) \
    ENTRY(DI, di, 7, 1, OP_REG16           )

#define TABLE_REGISTER_SEGMENT \
    ENTRY(ES, es, 1)           \
    ENTRY(CS, cs, 2)           \
    ENTRY(SS, ss, 3)           \
    ENTRY(DS, ds, 4)

#define TABLE_OPERAND_PREFIX_SIZE \
    ENTRY(BYTE,  byte, 1)         \
    ENTRY(WORD,  word, 2)         \

#define TABLE_PREFIX              \
    ENTRY(LOCK,  lock,  11110000) \
    ENTRY(REP,   rep,   11110011) \
    ENTRY(REPE,  repe,  11110011) \
    ENTRY(REPZ,  repz,  11110011) \
    ENTRY(REPNE, repne, 11110010) \
    ENTRY(REPNZ, repnz, 11110010)

#define TABLE_ONE_CHARACTER_TOKEN \
    ENTRY(LSQUARE_BR, '[' )       \
    ENTRY(RSQUARE_BR, ']' )       \
    ENTRY(PLUS,       '+' )       \
    ENTRY(MINUS,      '-' )       \
    ENTRY(COLON,      ':' )       \
    ENTRY(COMMA,      ',' )       \
    ENTRY(ZERO,       '0' )       \
    ENTRY(EOL,        '\n')

#define TABLE_OPERATORS  \
    ENTRY(BITS,  bits)   \
    ENTRY(LABEL, label)  \
    ENTRY(NUM,   num)    \

#define TABLE_EADDR          \
    ENTRY      (BX, SI, 000) \
    ENTRY      (BX, DI, 001) \
    ENTRY      (BP, SI, 010) \
    ENTRY      (BP, DI, 011) \
    ENTRYINDEX (    SI, 100) \
    ENTRYINDEX (    DI, 101) \
    ENTRYBASE  (BP,     110) \
    ENTRYBASE  (BX,     111)

#define OPCODE(bits)   ENTRY(OPCODE,   bits, sizeof(#bits) - 1)
#define LBL            ENTRY(LBL,      0,    0                )
#define MOD            ENTRY(MOD,      0,    2                )
#define REG            ENTRY(REG,      0,    3                )
#define SR             ENTRY(SR,       0,    2                )
#define RM             ENTRY(RM,       0,    3                )
#define DISP           ENTRY(DISP,     0,    0                )
#define DATA8          ENTRY(DATA8,    0,    8                )
#define IMM8           ENTRY(IMM8,     0,    8                )
#define IMM16          ENTRY(IMM16,    0,    16               )
#define ADDR           ENTRY(ADDR,     0,    0                )
#define D              ENTRY(D,        0,    1                )
#define W              ENTRY(W,        0,    1                )
#define S              ENTRY(S,        0,    1                )
#define END            ENTRY(END,      0,    0                )

#define ImplD(bit)     ENTRY(ImplD,   bit,  1)
#define ImplW(bit)     ENTRY(ImplW,   bit,  1)
#define ImplS(bit)     ENTRY(ImplS,   bit,  1)
#define ImplV(bit)     ENTRY(ImplV,   bit,  1)
#define ImplMod(bits)  ENTRY(ImplMod, bits, 2)

#define INST_FIELD_LIST \
    OPCODE(0)  \
    MOD        \
    REG        \
    SR         \
    RM         \
    DISP       \
    DATA8      \
    IMM8       \
    IMM16      \
    ADDR       \
    D          \
    W          \
    S          \
    LBL        \
    END        \
    ImplD(0)   \
    ImplW(0)   \
    ImplMod(0) \
    ImplS(0)   \
    ImplV(0)   \

#define TABLE_INSTRUCTION \
    INST(MOV,   REG8,        REG8,        { OPCODE(100010),                     ImplD(0),           ImplW(0), ImplMod(11),                    REG,                  RM,              END }) \
    INST(MOV,   REG16,       REG16,       { OPCODE(100010),                     ImplD(0),           ImplW(1), ImplMod(11),                    REG,                  RM,              END }) \
    INST(MOV,   ACC8,        DADDR,       { OPCODE(1010000),                                        ImplW(0),                           ADDR,                                        END }) \
    INST(MOV,   ACC16,       DADDR,       { OPCODE(1010000),                                        ImplW(1),                           ADDR,                                        END }) \
    INST(MOV,   DADDR,       ACC8,        { OPCODE(1010001),                                        ImplW(0),                           ADDR,                                        END }) \
    INST(MOV,   DADDR,       ACC16,       { OPCODE(1010001),                                        ImplW(1),                           ADDR,                                        END }) \
    INST(MOV,   REG8,        MEM,         { OPCODE(100010),                     ImplD(1),           ImplW(0), MOD,                            REG,                  RM, DISP,        END }) \
    INST(MOV,   REG16,       MEM,         { OPCODE(100010),                     ImplD(1),           ImplW(1), MOD,                            REG,                  RM, DISP,        END }) \
    INST(MOV,   MEM,         REG8,        { OPCODE(100010),                     ImplD(0),           ImplW(0), MOD,                            REG,                  RM, DISP,        END }) \
    INST(MOV,   MEM,         REG16,       { OPCODE(100010),                     ImplD(0),           ImplW(1), MOD,                            REG,                  RM, DISP,        END }) \
    INST(MOV,   MEM,         IMMEDIATE8,  { OPCODE(1100011),                                        ImplW(0), MOD,         OPCODE(000),                             RM, DISP, IMM8,  END }) \
    INST(MOV,   MEM,         IMMEDIATE16, { OPCODE(1100011),                                        ImplW(1), MOD,         OPCODE(000),                             RM, DISP, IMM16, END }) \
    INST(MOV,   REG8,        IMMEDIATE,   { OPCODE(1011),                                           ImplW(0),                                 REG,                            IMM8,  END }) \
    INST(MOV,   REG16,       IMMEDIATE,   { OPCODE(1011),                                           ImplW(1),                                 REG,                            IMM16, END }) \
    INST(MOV,   SEGREG,      REG16,       { OPCODE(10001110),                                                 ImplMod(11), OPCODE(0),              SR,              RM, DISP,        END }) \
    INST(MOV,   SEGREG,      MEM,         { OPCODE(10001110),                                                 MOD,         OPCODE(0),              SR,              RM, DISP,        END }) \
    INST(MOV,   REG16,       SEGREG,      { OPCODE(10001100),                                                 ImplMod(11), OPCODE(0),              SR,              RM, DISP,        END }) \
    INST(MOV,   MEM,         SEGREG,      { OPCODE(10001100),                                                 MOD,         OPCODE(0),              SR,              RM, DISP,        END }) \
    INST(PUSH,  MEM,         NONE,        { OPCODE(11111111),                                                 MOD,         OPCODE(110),                             RM, DISP,        END }) \
    INST(PUSH,  REG16,       NONE,        { OPCODE(01010),                                                                                    REG,                                   END }) \
    INST(PUSH,  SEGREG,      NONE,        { OPCODE(000),                                                                                           SR, OPCODE(110),                  END }) \
    INST(POP,   MEM,         NONE,        { OPCODE(10001111),                                                 MOD,         OPCODE(000),                             RM, DISP,        END }) \
    INST(POP,   REG16,       NONE,        { OPCODE(01011),                                                                                    REG,                                   END }) \
    INST(POP,   SEGREG,      NONE,        { OPCODE(000),                                                                                           SR, OPCODE(111),                  END }) \
    INST(XCHG,  ACC16,       REG16,       { OPCODE(10010),                                                                                    REG,                                   END }) \
    INST(XCHG,  REG8,        REG8,        { OPCODE(1000011),                                        ImplW(0), MOD,                            REG,                  RM, DISP,        END }) \
    INST(XCHG,  REG16,       REG16,       { OPCODE(1000011),                                        ImplW(1), MOD,                            REG,                  RM, DISP,        END }) \
    INST(XCHG,  MEM,         REG8,        { OPCODE(1000011),                                        ImplW(0), MOD,                            REG,                  RM, DISP,        END }) \
    INST(XCHG,  MEM,         REG16,       { OPCODE(1000011),                                        ImplW(1), MOD,                            REG,                  RM, DISP,        END }) \
    INST(XCHG,  REG8,        MEM,         { OPCODE(1000011),                                        ImplW(0), MOD,                            REG,                  RM, DISP,        END }) \
    INST(XCHG,  REG16,       MEM,         { OPCODE(1000011),                                        ImplW(1), MOD,                            REG,                  RM, DISP,        END }) \
    INST(IN,    ACC8,        IMMEDIATE,   { OPCODE(1110010),                                        ImplW(0),                                                                 DATA8, END }) \
    INST(IN,    ACC16,       IMMEDIATE,   { OPCODE(1110010),                                        ImplW(1),                                                                 DATA8, END }) \
    INST(IN,    ACC8,        REG16,       { OPCODE(1110110),                                        ImplW(0),                                                                        END }) \
    INST(IN,    ACC16,       REG16,       { OPCODE(1110110),                                        ImplW(1),                                                                        END }) \
    INST(OUT,   IMMEDIATE,   ACC8,        { OPCODE(1110011),                                        ImplW(0),                                                                 DATA8, END }) \
    INST(OUT,   IMMEDIATE,   ACC16,       { OPCODE(1110011),                                        ImplW(1),                                                                 DATA8, END }) \
    INST(OUT,   REG16,       ACC8,        { OPCODE(1110111),                                        ImplW(0),                                                                        END }) \
    INST(OUT,   REG16,       ACC16,       { OPCODE(1110111),                                        ImplW(1),                                                                        END }) \
    INST(XLAT,  NONE,        NONE,        { OPCODE(11010111),                                                                                                                        END }) \
    INST(LEA,   REG16,       MEM,         { OPCODE(10001101),                                                 MOD,                            REG,                  RM, DISP,        END }) \
    INST(LDS,   REG16,       MEM,         { OPCODE(11000101),                                                 MOD,                            REG,                  RM, DISP,        END }) \
    INST(LES,   REG16,       MEM,         { OPCODE(11000100),                                                 MOD,                            REG,                  RM, DISP,        END }) \
    INST(LAHF,  NONE,        NONE,        { OPCODE(10011111),                                                                                                                        END }) \
    INST(SAHF,  NONE,        NONE,        { OPCODE(10011110),                                                                                                                        END }) \
    INST(PUSHF, NONE,        NONE,        { OPCODE(10011100),                                                                                                                        END }) \
    INST(POPF,  NONE,        NONE,        { OPCODE(10011101),                                                                                                                        END }) \
    INST(ADD,   REG8,        REG8,        { OPCODE(000000),                     ImplD(0),           ImplW(0), MOD,                            REG,                  RM, DISP,        END }) \
    INST(ADD,   REG16,       REG16,       { OPCODE(000000),                     ImplD(0),           ImplW(1), MOD,                            REG,                  RM, DISP,        END }) \
    INST(ADD,   ACC8,        IMMEDIATE8,  { OPCODE(0000010),                                        ImplW(0),                                                                 IMM8,  END }) \
    INST(ADD,   ACC16,       IMMEDIATE16, { OPCODE(0000010),                                        ImplW(1),                                                                 IMM16, END }) \
    INST(ADD,   REG8,        MEM,         { OPCODE(000000),                     ImplD(1),           ImplW(0), MOD,                            REG,                  RM, DISP,        END }) \
    INST(ADD,   REG16,       MEM,         { OPCODE(000000),                     ImplD(1),           ImplW(1), MOD,                            REG,                  RM, DISP,        END }) \
    INST(ADD,   MEM,         REG8,        { OPCODE(000000),                     ImplD(0),           ImplW(0), MOD,                            REG,                  RM, DISP,        END }) \
    INST(ADD,   MEM,         REG16,       { OPCODE(000000),                     ImplD(0),           ImplW(1), MOD,                            REG,                  RM, DISP,        END }) \
    INST(ADD,   REG8,        IMMEDIATE8,  { OPCODE(100000),           ImplS(0),                     ImplW(0), MOD,         OPCODE(000),                             RM, DISP, IMM8,  END }) \
    INST(ADD,   REG16,       IMMEDIATE,   { OPCODE(100000),           ImplS(0),                     ImplW(1), MOD,         OPCODE(000),                             RM, DISP, IMM16, END }) \
    INST(ADC,   REG8,        REG8,        { OPCODE(000100),                     ImplD(0),           ImplW(0), MOD,                            REG,                  RM, DISP,        END }) \
    INST(ADC,   REG16,       REG16,       { OPCODE(000100),                     ImplD(0),           ImplW(1), MOD,                            REG,                  RM, DISP,        END }) \
    INST(ADC,   ACC8,        IMMEDIATE8,  { OPCODE(0001010),                                        ImplW(0),                                                                 IMM8,  END }) \
    INST(ADC,   ACC16,       IMMEDIATE16, { OPCODE(0001010),                                        ImplW(1),                                                                 IMM16, END }) \
    INST(ADC,   REG8,        MEM,         { OPCODE(000100),                     ImplD(1),           ImplW(0), MOD,                            REG,                  RM, DISP,        END }) \
    INST(ADC,   REG16,       MEM,         { OPCODE(000100),                     ImplD(1),           ImplW(1), MOD,                            REG,                  RM, DISP,        END }) \
    INST(ADC,   MEM,         REG8,        { OPCODE(000100),                     ImplD(0),           ImplW(0), MOD,                            REG,                  RM, DISP,        END }) \
    INST(ADC,   MEM,         REG16,       { OPCODE(000100),                     ImplD(0),           ImplW(1), MOD,                            REG,                  RM, DISP,        END }) \
    INST(ADC,   REG8,        IMMEDIATE8,  { OPCODE(100000),           ImplS(0),                     ImplW(0), MOD,         OPCODE(010),                             RM, DISP, IMM8,  END }) \
    INST(ADC,   REG16,       IMMEDIATE16, { OPCODE(100000),           ImplS(0),                     ImplW(1), MOD,         OPCODE(010),                             RM, DISP, IMM16, END }) \
    INST(ADC,   REG16,       IMMEDIATE8,  { OPCODE(100000),           ImplS(1),                     ImplW(1), MOD,         OPCODE(010),                             RM, DISP, IMM8,  END }) \
    INST(INC,   MEM,         NONE,        { OPCODE(1111111),                                        W,        MOD,         OPCODE(000),                             RM, DISP,        END }) \
    INST(INC,   REG8,        NONE,        { OPCODE(1111111),                                        ImplW(0), ImplMod(11), OPCODE(000),                             RM,              END }) \
    INST(INC,   REG16,       NONE,        { OPCODE(01000),                                                                                    REG,                                   END }) \
    INST(AAA,   NONE,        NONE,        { OPCODE(00110111),                                                                                                                        END }) \
    INST(DAA,   NONE,        NONE,        { OPCODE(00100111),                                                                                                                        END }) \
    INST(SUB,   REG8,        REG8,        { OPCODE(001010),                     ImplD(0),           ImplW(0), MOD,                            REG,                  RM, DISP,        END }) \
    INST(SUB,   REG16,       REG16,       { OPCODE(001010),                     ImplD(0),           ImplW(1), MOD,                            REG,                  RM, DISP,        END }) \
    INST(SUB,   ACC8,        IMMEDIATE8,  { OPCODE(0010110),                                        ImplW(0),                                                                 IMM8,  END }) \
    INST(SUB,   ACC16,       IMMEDIATE16, { OPCODE(0010110),                                        ImplW(1),                                                                 IMM16, END }) \
    INST(SUB,   REG8,        MEM,         { OPCODE(001010),                     ImplD(1),           ImplW(0), MOD,                            REG,                  RM, DISP,        END }) \
    INST(SUB,   REG16,       MEM,         { OPCODE(001010),                     ImplD(1),           ImplW(1), MOD,                            REG,                  RM, DISP,        END }) \
    INST(SUB,   MEM,         REG8,        { OPCODE(001010),                     ImplD(0),           ImplW(0), MOD,                            REG,                  RM, DISP,        END }) \
    INST(SUB,   MEM,         REG16,       { OPCODE(001010),                     ImplD(0),           ImplW(1), MOD,                            REG,                  RM, DISP,        END }) \
    INST(SUB,   REG8,        IMMEDIATE8,  { OPCODE(100000),           ImplS(0),                     ImplW(0), MOD,         OPCODE(101),                             RM, DISP, IMM8,  END }) \
    INST(SUB,   REG16,       IMMEDIATE16, { OPCODE(100000),           ImplS(0),                     ImplW(1), MOD,         OPCODE(101),                             RM, DISP, IMM16, END }) \
    INST(SUB,   REG16,       IMMEDIATE8,  { OPCODE(100000),           ImplS(1),                     ImplW(1), MOD,         OPCODE(101),                             RM, DISP, IMM8,  END }) \
    INST(SBB,   REG8,        REG8,        { OPCODE(000110),                     ImplD(0),           ImplW(0), MOD,                            REG,                  RM, DISP,        END }) \
    INST(SBB,   REG16,       REG16,       { OPCODE(000110),                     ImplD(0),           ImplW(1), MOD,                            REG,                  RM, DISP,        END }) \
    INST(SBB,   ACC8,        IMMEDIATE8,  { OPCODE(0001110),                                        ImplW(0),                                                                 IMM8,  END }) \
    INST(SBB,   ACC16,       IMMEDIATE16, { OPCODE(0001110),                                        ImplW(1),                                                                 IMM16, END }) \
    INST(SBB,   REG8,        MEM,         { OPCODE(000110),                     ImplD(1),           ImplW(0), MOD,                            REG,                  RM, DISP,        END }) \
    INST(SBB,   REG16,       MEM,         { OPCODE(000110),                     ImplD(1),           ImplW(1), MOD,                            REG,                  RM, DISP,        END }) \
    INST(SBB,   MEM,         REG8,        { OPCODE(000110),                     ImplD(0),           ImplW(0), MOD,                            REG,                  RM, DISP,        END }) \
    INST(SBB,   MEM,         REG16,       { OPCODE(000110),                     ImplD(0),           ImplW(1), MOD,                            REG,                  RM, DISP,        END }) \
    INST(SBB,   REG8,        IMMEDIATE8,  { OPCODE(100000),           ImplS(0),                     ImplW(0), MOD,         OPCODE(011),                             RM, DISP, IMM8,  END }) \
    INST(SBB,   REG16,       IMMEDIATE16, { OPCODE(100000),           ImplS(0),                     ImplW(1), MOD,         OPCODE(011),                             RM, DISP, IMM16, END }) \
    INST(SBB,   REG16,       IMMEDIATE8,  { OPCODE(100000),           ImplS(1),                     ImplW(1), MOD,         OPCODE(011),                             RM, DISP, IMM8,  END }) \
    INST(DEC,   REG8,        NONE,        { OPCODE(1111111),                                        ImplW(0), MOD,         OPCODE(001),                             RM,              END }) \
    INST(DEC,   MEM,         NONE,        { OPCODE(1111111),                                        W,        MOD,         OPCODE(001),                             RM, DISP,        END }) \
    INST(DEC,   REG16,       NONE,        { OPCODE(01001),                                                                                    REG,                                   END }) \
    INST(NEG,   REG8,        NONE,        { OPCODE(1111011),                                        ImplW(0), MOD,         OPCODE(011),                             RM,              END }) \
    INST(NEG,   REG16,       NONE,        { OPCODE(1111011),                                        ImplW(1), MOD,         OPCODE(011),                             RM,              END }) \
    INST(NEG,   MEM,         NONE,        { OPCODE(1111011),                                        W,        MOD,         OPCODE(011),                             RM, DISP,        END }) \
    INST(CMP,   REG8,        REG8,        { OPCODE(001110),                     ImplD(0),           ImplW(0), ImplMod(11),                    REG,                  RM,              END }) \
    INST(CMP,   REG16,       REG16,       { OPCODE(001110),                     ImplD(0),           ImplW(1), ImplMod(11),                    REG,                  RM,              END }) \
    INST(CMP,   ACC8,        IMMEDIATE,   { OPCODE(0011110),                                        ImplW(0),                                                                 IMM8,  END }) \
    INST(CMP,   ACC16,       IMMEDIATE,   { OPCODE(0011110),                                        ImplW(1),                                                                 IMM16, END }) \
    INST(CMP,   REG8,        MEM,         { OPCODE(001110),                     ImplD(1),           ImplW(0), MOD,                            REG,                  RM, DISP,        END }) \
    INST(CMP,   REG16,       MEM,         { OPCODE(001110),                     ImplD(1),           ImplW(1), MOD,                            REG,                  RM, DISP,        END }) \
    INST(CMP,   MEM,         REG8,        { OPCODE(001110),                     ImplD(0),           ImplW(0), MOD,                            REG,                  RM, DISP,        END }) \
    INST(CMP,   MEM,         REG16,       { OPCODE(001110),                     ImplD(0),           ImplW(1), MOD,                            REG,                  RM, DISP,        END }) \
    INST(CMP,   MEM,         IMMEDIATE8,  { OPCODE(100000),           S,                            ImplW(0), MOD,         OPCODE(111),                             RM, DISP, IMM8,  END }) \
    INST(CMP,   MEM,         IMMEDIATE16, { OPCODE(100000),           ImplS(0),                     ImplW(1), MOD,         OPCODE(111),                             RM, DISP, IMM16, END }) \
    INST(CMP,   REG8,        IMMEDIATE8,  { OPCODE(100000),           ImplS(0),                     ImplW(0), MOD,         OPCODE(111),                             RM,       IMM8,  END }) \
    INST(CMP,   REG16,       IMMEDIATE8,  { OPCODE(100000),           ImplS(1),                     ImplW(1), MOD,         OPCODE(111),                             RM,       IMM8,  END }) \
    INST(CMP,   REG16,       IMMEDIATE16, { OPCODE(100000),           ImplS(0),                     ImplW(1), MOD,         OPCODE(111),                             RM,       IMM16, END }) \
    INST(AAS,   NONE,        NONE,        { OPCODE(00111111),                                                                                                                        END }) \
    INST(DAS,   NONE,        NONE,        { OPCODE(00101111),                                                                                                                        END }) \
    INST(MUL,   REG8,        NONE,        { OPCODE(1111011),                                ImplW(0), ImplMod(11), OPCODE(100),                             RM,                      END }) \
    INST(MUL,   REG16,       NONE,        { OPCODE(1111011),                                ImplW(1), ImplMod(11), OPCODE(100),                             RM,                      END }) \
    INST(MUL,   MEM,         NONE,        { OPCODE(1111011),                                W,        MOD,         OPCODE(100),                             RM, DISP,                END }) \
    INST(IMUL,  REG8,        NONE,        { OPCODE(1111011),                                ImplW(0), ImplMod(11), OPCODE(101),                             RM,                      END }) \
    INST(IMUL,  REG16,       NONE,        { OPCODE(1111011),                                ImplW(1), ImplMod(11), OPCODE(101),                             RM,                      END }) \
    INST(IMUL,  MEM,         NONE,        { OPCODE(1111011),                                W,        MOD,         OPCODE(101),                             RM, DISP,                END }) \
    INST(AAM,   NONE,        NONE,        { OPCODE(1101010000001010),                                                                                                                END }) \
    INST(AAM,   MEM,         NONE,        { OPCODE(1101010000001010),                                                                                           DISP,                END }) \
    INST(DIV,   REG8,        NONE,        { OPCODE(1111011),                                ImplW(0), ImplMod(11), OPCODE(110),                             RM,                      END }) \
    INST(DIV,   REG16,       NONE,        { OPCODE(1111011),                                ImplW(1), ImplMod(11), OPCODE(110),                             RM,                      END }) \
    INST(DIV,   MEM,         NONE,        { OPCODE(1111011),                                W,        MOD,         OPCODE(110),                             RM, DISP,                END }) \
    INST(IDIV,  REG8,        NONE,        { OPCODE(1111011),                                ImplW(0), ImplMod(11), OPCODE(111),                             RM,                      END }) \
    INST(IDIV,  REG16,       NONE,        { OPCODE(1111011),                                ImplW(1), ImplMod(11), OPCODE(111),                             RM,                      END }) \
    INST(IDIV,  MEM,         NONE,        { OPCODE(1111011),                                W,        MOD,         OPCODE(111),                             RM, DISP,                END }) \
    INST(AAD,   NONE,        NONE,        { OPCODE(1101010100001010),                                                                                                                END }) \
    INST(AAD,   MEM,         NONE,        { OPCODE(1101010100001010),                                                                                           DISP,                END }) \
    INST(CBW,   NONE,        NONE,        { OPCODE(10011000),                                                                                                                        END }) \
    INST(CWD,   NONE,        NONE,        { OPCODE(10011001),                                                                                                                        END }) \
    INST(NOT,   REG8,        NONE,        { OPCODE(1111011),                                ImplW(0), ImplMod(11), OPCODE(010),                             RM,                      END }) \
    INST(NOT,   REG16,       NONE,        { OPCODE(1111011),                                ImplW(1), ImplMod(11), OPCODE(010),                             RM,                      END }) \
    INST(NOT,   MEM,         NONE,        { OPCODE(1111011),                                W,        MOD,         OPCODE(010),                             RM, DISP,                END }) \
    INST(SHL,   REG8,        IMMEDIATE8,  { OPCODE(110100),                       ImplV(0), ImplW(0), ImplMod(11), OPCODE(100),                             RM,                      END }) \
    INST(SHL,   REG16,       IMMEDIATE8,  { OPCODE(110100),                       ImplV(0), ImplW(1), ImplMod(11), OPCODE(100),                             RM,                      END }) \
    INST(SHL,   REG8,        REG8,        { OPCODE(110100),                       ImplV(1), ImplW(0), ImplMod(11), OPCODE(100),                             RM,                      END }) \
    INST(SHL,   REG16,       REG8,        { OPCODE(110100),                       ImplV(1), ImplW(1), ImplMod(11), OPCODE(100),                             RM,                      END }) \
    INST(SHL,   MEM,         IMMEDIATE8,  { OPCODE(110100),                       ImplV(0), W,        MOD,         OPCODE(100),                             RM, DISP,                END }) \
    INST(SHL,   MEM,         REG8,        { OPCODE(110100),                       ImplV(1), W,        MOD,         OPCODE(100),                             RM, DISP,                END }) \
    INST(SAL,   REG8,        IMMEDIATE8,  { OPCODE(110100),                       ImplV(0), ImplW(0), ImplMod(11), OPCODE(100),                             RM,                      END }) \
    INST(SAL,   REG16,       IMMEDIATE8,  { OPCODE(110100),                       ImplV(0), ImplW(1), ImplMod(11), OPCODE(100),                             RM,                      END }) \
    INST(SAL,   REG8,        REG8,        { OPCODE(110100),                       ImplV(1), ImplW(0), ImplMod(11), OPCODE(100),                             RM,                      END }) \
    INST(SAL,   REG16,       REG8,        { OPCODE(110100),                       ImplV(1), ImplW(1), ImplMod(11), OPCODE(100),                             RM,                      END }) \
    INST(SAL,   MEM,         IMMEDIATE8,  { OPCODE(110100),                       ImplV(0), W,        MOD,         OPCODE(100),                             RM, DISP,                END }) \
    INST(SAL,   MEM,         REG8,        { OPCODE(110100),                       ImplV(1), W,        MOD,         OPCODE(100),                             RM, DISP,                END }) \
    INST(SHR,   REG8,        IMMEDIATE8,  { OPCODE(110100),                       ImplV(0), ImplW(0), ImplMod(11), OPCODE(101),                             RM,                      END }) \
    INST(SHR,   REG16,       IMMEDIATE8,  { OPCODE(110100),                       ImplV(0), ImplW(1), ImplMod(11), OPCODE(101),                             RM,                      END }) \
    INST(SHR,   REG8,        REG8,        { OPCODE(110100),                       ImplV(1), ImplW(0), ImplMod(11), OPCODE(101),                             RM,                      END }) \
    INST(SHR,   REG16,       REG8,        { OPCODE(110100),                       ImplV(1), ImplW(1), ImplMod(11), OPCODE(101),                             RM,                      END }) \
    INST(SHR,   MEM,         IMMEDIATE8,  { OPCODE(110100),                       ImplV(0), W,        MOD,         OPCODE(101),                             RM, DISP,                END }) \
    INST(SHR,   MEM,         REG8,        { OPCODE(110100),                       ImplV(1), W,        MOD,         OPCODE(101),                             RM, DISP,                END }) \
    INST(SAR,   REG8,        IMMEDIATE8,  { OPCODE(110100),                       ImplV(0), ImplW(0), ImplMod(11), OPCODE(111),                             RM,                      END }) \
    INST(SAR,   REG16,       IMMEDIATE8,  { OPCODE(110100),                       ImplV(0), ImplW(1), ImplMod(11), OPCODE(111),                             RM,                      END }) \
    INST(SAR,   REG8,        REG8,        { OPCODE(110100),                       ImplV(1), ImplW(0), ImplMod(11), OPCODE(111),                             RM,                      END }) \
    INST(SAR,   REG16,       REG8,        { OPCODE(110100),                       ImplV(1), ImplW(1), ImplMod(11), OPCODE(111),                             RM,                      END }) \
    INST(SAR,   MEM,         IMMEDIATE8,  { OPCODE(110100),                       ImplV(0), W,        MOD,         OPCODE(111),                             RM, DISP,                END }) \
    INST(SAR,   MEM,         REG8,        { OPCODE(110100),                       ImplV(1), W,        MOD,         OPCODE(111),                             RM, DISP,                END }) \
    INST(ROL,   REG8,        IMMEDIATE8,  { OPCODE(110100),                       ImplV(0), ImplW(0), ImplMod(11), OPCODE(000),                             RM,                      END }) \
    INST(ROL,   REG16,       IMMEDIATE8,  { OPCODE(110100),                       ImplV(0), ImplW(1), ImplMod(11), OPCODE(000),                             RM,                      END }) \
    INST(ROL,   REG8,        REG8,        { OPCODE(110100),                       ImplV(1), ImplW(0), ImplMod(11), OPCODE(000),                             RM,                      END }) \
    INST(ROL,   REG16,       REG8,        { OPCODE(110100),                       ImplV(1), ImplW(1), ImplMod(11), OPCODE(000),                             RM,                      END }) \
    INST(ROL,   MEM,         IMMEDIATE8,  { OPCODE(110100),                       ImplV(0), W,        MOD,         OPCODE(000),                             RM, DISP,                END }) \
    INST(ROL,   MEM,         REG8,        { OPCODE(110100),                       ImplV(1), W,        MOD,         OPCODE(000),                             RM, DISP,                END }) \
    INST(ROR,   REG8,        IMMEDIATE8,  { OPCODE(110100),                       ImplV(0), ImplW(0), ImplMod(11), OPCODE(001),                             RM,                      END }) \
    INST(ROR,   REG16,       IMMEDIATE8,  { OPCODE(110100),                       ImplV(0), ImplW(1), ImplMod(11), OPCODE(001),                             RM,                      END }) \
    INST(ROR,   REG8,        REG8,        { OPCODE(110100),                       ImplV(1), ImplW(0), ImplMod(11), OPCODE(001),                             RM,                      END }) \
    INST(ROR,   REG16,       REG8,        { OPCODE(110100),                       ImplV(1), ImplW(1), ImplMod(11), OPCODE(001),                             RM,                      END }) \
    INST(ROR,   MEM,         IMMEDIATE8,  { OPCODE(110100),                       ImplV(0), W,        MOD,         OPCODE(001),                             RM, DISP,                END }) \
    INST(ROR,   MEM,         REG8,        { OPCODE(110100),                       ImplV(1), W,        MOD,         OPCODE(001),                             RM, DISP,                END }) \
    INST(RCL,   REG8,        IMMEDIATE8,  { OPCODE(110100),                       ImplV(0), ImplW(0), ImplMod(11), OPCODE(010),                             RM,                      END }) \
    INST(RCL,   REG16,       IMMEDIATE8,  { OPCODE(110100),                       ImplV(0), ImplW(1), ImplMod(11), OPCODE(010),                             RM,                      END }) \
    INST(RCL,   REG8,        REG8,        { OPCODE(110100),                       ImplV(1), ImplW(0), ImplMod(11), OPCODE(010),                             RM,                      END }) \
    INST(RCL,   REG16,       REG8,        { OPCODE(110100),                       ImplV(1), ImplW(1), ImplMod(11), OPCODE(010),                             RM,                      END }) \
    INST(RCL,   MEM,         IMMEDIATE8,  { OPCODE(110100),                       ImplV(0), W,        MOD,         OPCODE(010),                             RM, DISP,                END }) \
    INST(RCL,   MEM,         REG8,        { OPCODE(110100),                       ImplV(1), W,        MOD,         OPCODE(010),                             RM, DISP,                END }) \
    INST(RCR,   REG8,        IMMEDIATE8,  { OPCODE(110100),                       ImplV(0), ImplW(0), ImplMod(11), OPCODE(011),                             RM,                      END }) \
    INST(RCR,   REG16,       IMMEDIATE8,  { OPCODE(110100),                       ImplV(0), ImplW(1), ImplMod(11), OPCODE(011),                             RM,                      END }) \
    INST(RCR,   REG8,        REG8,        { OPCODE(110100),                       ImplV(1), ImplW(0), ImplMod(11), OPCODE(011),                             RM,                      END }) \
    INST(RCR,   REG16,       REG8,        { OPCODE(110100),                       ImplV(1), ImplW(1), ImplMod(11), OPCODE(011),                             RM,                      END }) \
    INST(RCR,   MEM,         IMMEDIATE8,  { OPCODE(110100),                       ImplV(0), W,        MOD,         OPCODE(011),                             RM, DISP,                END }) \
    INST(RCR,   MEM,         REG8,        { OPCODE(110100),                       ImplV(1), W,        MOD,         OPCODE(011),                             RM, DISP,                END }) \
    INST(JMP,   LABEL,       NONE,        { OPCODE(11101001), LBL, END }) \
    // TODO: shl instruction: check for immediate is not enough, to be valid the number must be exactly 1, check for REG8 is not enough, to be valid the register must be cl

#endif // TABLES_H
