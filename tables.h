#ifndef TABLES_H
#define TABLES_H

#define TABLE_MNEMONIC  \
    ENTRY(MOV,   mov)   \
    ENTRY(PUSH,  push)  \
    ENTRY(POP,   pop)

// Chapter 4, Table 4.9: REG Field Encoding
// NOTE: the same table applies in MOD=11 case when encoding R/M field (see table 4.10)
#define TABLE_REGISTER_GENERAL \
    ENTRY(AL, al, 0, 0)        \
    ENTRY(AX, ax, 0, 1)        \
    ENTRY(CL, cl, 1, 0)        \
    ENTRY(CX, cx, 1, 1)        \
    ENTRY(DL, dl, 2, 0)        \
    ENTRY(DX, dx, 2, 1)        \
    ENTRY(BL, bl, 3, 0)        \
    ENTRY(BX, bx, 3, 1)        \
    ENTRY(AH, ah, 4, 0)        \
    ENTRY(SP, sp, 4, 1)        \
    ENTRY(CH, ch, 5, 0)        \
    ENTRY(BP, bp, 5, 1)        \
    ENTRY(DH, dh, 6, 0)        \
    ENTRY(SI, si, 6, 1)        \
    ENTRY(BH, bh, 7, 0)        \
    ENTRY(DI, di, 7, 1)

#define TABLE_REGISTER_SEGMENT \
    ENTRY(ES, es, 0, 1)        \
    ENTRY(CS, cs, 1, 1)        \
    ENTRY(SS, ss, 2, 1)        \
    ENTRY(DS, ds, 3, 1)

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
    ENTRY(BYTE,  byte)   \
    ENTRY(WORD,  word)   \
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


#define TABLE_NODE_NON_TERMINAL     \
    ENTRY(EADDR__,     eaddr__)     \
    ENTRY(EADDR_,      eaddr_)      \
    ENTRY(EADDR,       eaddr)       \
    ENTRY(REG_BASE,    reg_base)    \
    ENTRY(REG_IDX,     reg_idx)     \
    ENTRY(SIGNED_NUM,  signed_num)  \
    ENTRY(SIGNED_ZERO, signed_zero) \
    ENTRY(DIRECT_ADDR, direct_addr) \
    ENTRY(LINE_ ,      line_)       \
    ENTRY(LINE,        line)        \
    ENTRY(LBL,         lbl)         \
    ENTRY(INST,        inst)        \
    ENTRY(MNEMONIC_,   mnemonic_)   \
    ENTRY(MNEMONIC,    mnemonic)    \
    ENTRY(OPERAND_,    operand_)    \
    ENTRY(OPERAND,     operand)     \
    ENTRY(REG_GEN,     reg_gen)     \
    ENTRY(REG_SEG,     reg_seg)     \
    ENTRY(PREFIX,      prefix)      \
    ENTRY(OPR_PREFIX,  opr_prefix)  \
    ENTRY(OPR_MATH,    opr_math)    \
    ENTRY(OPR_SIZE,    opr_size)    \
    ENTRY(SEG_OVR,     seg_ovr)     \
    ENTRY(IMM,         imm)

#define OPCODE(bits)  ENTRY(OPCODE,   bits, sizeof(#bits) - 1)
#define MOD           ENTRY(MOD,  0,   2)
#define REG           ENTRY(REG,  0,   3)
#define SR            ENTRY(SR,   0,   2)
#define RM            ENTRY(RM,   0,   3)
#define DISP          ENTRY(DISP, 0,   0)
#define IMM           ENTRY(IMM,  0,   0)
#define ADDR          ENTRY(ADDR, 0,   0)
#define D             ENTRY(D,    0,   1)
#define W             ENTRY(W,    0,   1)
#define END           ENTRY(END,  0,   0)

#define TABLE_INST_FIELDS \
    MOD  \
    REG  \
    SR   \
    RM   \
    DISP \
    IMM  \
    ADDR \
    D    \
    W    \
    END 

#define TABLE_INSTRUCTION \
    INST(MOV, REGMEM_TOFROM_REG, { OPCODE(100010), D, W, MOD, REG, RM, DISP, END })            \
    INST(MOV, IMM_TO_REGMEM,     { OPCODE(1100011), W, MOD, OPCODE(000), RM, DISP, IMM, END }) \
    INST(MOV, IMM_TO_REG,        { OPCODE(1011), W, REG, IMM, END })                           \
    INST(MOV, MEM_TO_ACC,        { OPCODE(1010000), W, ADDR, END })                            \
    INST(MOV, ACC_TO_MEM,        { OPCODE(1010001), W, ADDR, END })                            \
    INST(MOV, REGMEM_TO_SEGREG,  { OPCODE(10001110), MOD, OPCODE(0), SR, RM, DISP, END })      \
    INST(MOV, SEGREG_TO_REGMEM,  { OPCODE(10001100), MOD, OPCODE(0), SR, RM, DISP, END })

#endif // TABLES_H
