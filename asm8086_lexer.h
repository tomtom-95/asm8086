#ifndef ASM8086_LEXER_H
#define ASM8086_LEXER_H

int lexer(struct LexTok      **tok_spp,
          char               **p_asm_buf,
          struct LexTokLabel **lbl_spp,
          struct LexTokNum   **num_spp);

int lexer_line(struct LexTok      **tok_spp,
               char               **pp_buf,
               struct LexTokLabel **lbl_spp,
               struct LexTokNum   **num_spp);

int lexer_num(char **pp_buf,
              struct LexTok **pp_tok,
              struct LexTokNum **num_spp,
              const char sign[2]);

void skip_whitespaces(char **pp_buf);
void skip_comment(char **pp_buf);

#endif // ASM8086_LEXER_H
