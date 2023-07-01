#ifndef TABLES_H
#define TABLES_H

#define TABLE_ALL \
    ENTRY     (BITS,       BITS,       bits)       \
    ENTRY     (MNEMONIC,   MOV,        mov)        \
    ENTRY_ALT (MNEMONIC,   PUSH,       push)       \
    ENTRY_ALT (MNEMONIC,   POP,        pop)        \
    ENTRY     (REG_GEN,    AX,         ax)         \
    ENTRY_ALT (REG_GEN,    AH,         ah)         \
    ENTRY_ALT (REG_GEN,    AL,         al)         \
    ENTRY_ALT (REG_GEN,    BX,         bx)         \
    ENTRY_ALT (REG_GEN,    BH,         bh)         \
    ENTRY_ALT (REG_GEN,    BL,         bl)         \
    ENTRY_ALT (REG_GEN,    CX,         cx)         \
    ENTRY_ALT (REG_GEN,    CH,         ch)         \
    ENTRY_ALT (REG_GEN,    CL,         cl)         \
    ENTRY_ALT (REG_GEN,    DX,         dx)         \
    ENTRY_ALT (REG_GEN,    DH,         dh)         \
    ENTRY_ALT (REG_GEN,    DL,         dl)         \
    ENTRY_ALT (REG_GEN,    SP,         sp)         \
    ENTRY_ALT (REG_GEN,    BP,         bp)         \
    ENTRY_ALT (REG_GEN,    SI,         si)         \
    ENTRY_ALT (REG_GEN,    DI,         di)         \
    ENTRY     (REG_SEG,    ES,         es)         \
    ENTRY_ALT (REG_SEG,    CS,         cs)         \
    ENTRY_ALT (REG_SEG,    SS,         ss)         \
    ENTRY_ALT (REG_SEG,    DS,         ds)         \
    ENTRY     (PREFIX,     LOCK,       lock)       \
    ENTRY_ALT (PREFIX,     REP,        rep)        \
    ENTRY_ALT (PREFIX,     REPE,       repe)       \
    ENTRY_ALT (PREFIX,     REPZ,       repz)       \
    ENTRY_ALT (PREFIX,     REPNE,      repne)      \
    ENTRY_ALT (PREFIX,     REPNZ,      repnz)      \
    ENTRY     (OPR_MATH,   PLUS,       plus)       \
    ENTRY_ALT (OPR_MATH,   MINUS,      minus)      \
    ENTRY     (OPR_SIZE,   BYTE,       byte)       \
    ENTRY_ALT (OPR_SIZE,   WORD,       word)       \
    ENTRY     (LABEL,      LABEL,      label)      \
    ENTRY     (NUM,        NUM,        num)        \
    ENTRY_ALT (NUM,        NUM_ZERO,   num_zero)   \
    ENTRY     (COLON,      COLON,      colon)      \
    ENTRY     (COMMA,      COMMA,      comma)      \
    ENTRY     (LSQUARE_BR, LSQUARE_BR, lsquare_br) \
    ENTRY     (RSQUARE_BR, RSQUARE_BR, rsquare_br) \
    ENTRY     (EOL,        EOL,        eol)

#define TABLE_PREFIX \
    ENTRY (LOCK,  11110000) \
    ENTRY (REP,   11110011) \
    ENTRY (REPE,  11110011) \
    ENTRY (REPZ,  11110011) \
    ENTRY (REPNE, 11110010) \
    ENTRY (REPNZ, 11110010)

#define TABLE_REG_GEN_ENCODING \
    ENTRY (AX, 000, 1) \
    ENTRY (AH, 000, 0) \
    ENTRY (AL, 100, 0) \
    ENTRY (BX, 011, 1) \
    ENTRY (BH, 011, 0) \
    ENTRY (BL, 111, 0) \
    ENTRY (CX, 001, 1) \
    ENTRY (CH, 101, 0) \
    ENTRY (CL, 001, 0) \
    ENTRY (DX, 010, 1) \
    ENTRY (DH, 110, 0) \
    ENTRY (DL, 010, 0) \
    ENTRY (SP, 100, 1) \
    ENTRY (BP, 101, 1) \
    ENTRY (SI, 110, 1) \
    ENTRY (DI, 111, 1)

#define TABLE_REG_SEG_ENCODING \
    ENTRY (ES, 00, 1) \
    ENTRY (CS, 01, 1) \
    ENTRY (SS, 10, 1) \
    ENTRY (DS, 11, 1)

#define TABLE_NODE_NON_TERMINAL \
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

#define TABLE_EADDR \
    ENTRY(BX,   SI,   000) \
    ENTRY(BX,   DI,   001) \
    ENTRY(BP,   SI,   010) \
    ENTRY(BP,   DI,   011) \
    ENTRY(NONE, SI,   100) \
    ENTRY(NONE, DI,   101) \
    ENTRY(BP,   NONE, 110) \
    ENTRY(BX,   NONE, 111)

#define OPCODE(num, bits) ENTRY(OPCODE##num, bits, sizeof(#bits) - 1)
#define ImplRegD(bit)     ENTRY(ImplRegD, bit, 0)
#define ImplRmD(bit)      ENTRY(ImplRmD, bit, 0)
#define ImplReg(bits)     ENTRY(ImplReg, bits, 0)

#define MOD               ENTRY(MOD,  0,   2)
#define REG               ENTRY(REG,  0,   3)
#define SR                ENTRY(SR,   0,   2)
#define RM                ENTRY(RM,   0,   3)
#define DISP              ENTRY(DISP, 0,   0)
#define IMM               ENTRY(IMM,  0,   0)
#define ADDR              ENTRY(ADDR, 0,   0)
#define D                 ENTRY(D,    0,   1)
#define W                 ENTRY(W,    0,   1)
#define END               ENTRY(END,  0,   0)

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

#define TABLE_INST \
    INST(MOV, {OPCODE(1,100010), D, W, MOD, REG, RM, DISP, END})                          \
    INST(MOV, {OPCODE(1,1100011), ImplRmD(1), W, MOD, OPCODE(2,000), RM, DISP, IMM, END}) \
    INST(MOV, {OPCODE(1,1011), ImplRegD(1), W, REG, IMM, END})                            \
    INST(MOV, {OPCODE(1,1010000), ImplRegD(1), ImplReg(000), W, ADDR, END})               \
    INST(MOV, {OPCODE(1,1010001), ImplRegD(0), ImplReg(000), W, ADDR, END})               \
    INST(MOV, {OPCODE(1,10001110), ImplRmD(0), MOD, OPCODE(2,0), SR, RM, DISP, END})      \
    INST(MOV, {OPCODE(1,10001100), ImplRmD(1), MOD, OPCODE(2,0), SR, RM, DISP, END})      \
    INST(NONE, {0})

#endif // TABLES_H
