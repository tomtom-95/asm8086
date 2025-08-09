#include <stdlib.h>
#include <inttypes.h>
#include <errno.h>

#include "asm8086_lexer.h"
#include "asm8086.h"

int lexer(struct LexTok      **tok_spp,
          char               **pp_buf,
          struct LexTokLabel **lbl_spp,
          struct LexTokNum   **num_spp)
{
    for (;;)
    {
        if (**pp_buf == '\0')
        {
            break;
        }
        else if (**pp_buf == ';')
        {
            while (*((*pp_buf)++) != '\n');
            continue;
        }
        else
        {
            while(**pp_buf == ' ' || **pp_buf == '\t' || **pp_buf == '\n')
                ++(*pp_buf);
        }

        if (lexer_line(tok_spp, pp_buf, lbl_spp, num_spp))
        {
            printf("lexer_line failed\n");
            return 1; 
        }
    }

    return 0;
}

int lexer_line(struct LexTok      **tok_spp,
               char               **pp_buf,
               struct LexTokLabel **lbl_spp,
               struct LexTokNum   **num_spp)
{
    for (;;)
    {
        while (**pp_buf == ' ' || **pp_buf == '\t')
            ++(*pp_buf);

        if (**pp_buf == '\n')
        {
            (**tok_spp).id0 = TOK0_EOL;
            (**tok_spp).id1 = TOK1_EOL;
            ++(*tok_spp);
            ++(*pp_buf);

            break;
        }
        else if (isalpha(**pp_buf))
        {
            char *start = *pp_buf;
            char *end = *pp_buf;
            while (isalpha(*end))
                ++(end);

            long str_len = end - start;
            assert(str_len <= LEN_MAX_LABEL);

            char str[LEN_MAX_LABEL+ 1];
            strlcpy(str, *pp_buf, str_len + 1);

            *pp_buf += str_len;

            int matched_flag = 0;
            for (size_t idx = 0; idx < ArrayCount(names_lex_tok1); ++idx)
            {
                if (strcmp(names_lex_tok1[idx], str) == 0)
                {
                    (**tok_spp).id0 = id_lex_tok0[idx];
                    (**tok_spp).id1 = id_lex_tok1[idx];
                    ++(*tok_spp);
                    matched_flag = 1;

                    break;
                }
            }
            if (matched_flag == 0)
            {
                (**tok_spp).id0 = TOK0_LABEL;
                (**tok_spp).id1 = TOK1_LABEL;
                (**tok_spp).p_label = (*lbl_spp)++; 
                strcpy((**tok_spp).p_label -> name, str);
                ++(*tok_spp);
            }
        }
        else if (isdigit(**pp_buf))
        {
            lexer_num(pp_buf, tok_spp, num_spp, "+");
        }
        else
        {
            switch (**pp_buf)
            {
            case '+':
            {
                (**tok_spp).id0 = TOK0_OPR_MATH;
                (**tok_spp).id1 = TOK1_PLUS;
                ++(*tok_spp);
                ++(*pp_buf);
                
                while (**pp_buf == ' ' || **pp_buf == '\t')
                    ++(*pp_buf);
                
                if (isdigit(**pp_buf))
                    lexer_num(pp_buf, tok_spp, num_spp, "+");

                break;
            }
            case '-':
            {
                (**tok_spp).id0 = TOK0_OPR_MATH;
                (**tok_spp).id1 = TOK1_MINUS;
                ++(*tok_spp);
                ++(*pp_buf);

                while (**pp_buf == ' ' || **pp_buf == '\t')
                    ++(*pp_buf);
                
                if (isdigit(**pp_buf))
                {
                    lexer_num(pp_buf, tok_spp, num_spp, "-");
                }
                else
                {
                    printf("Error: minus sign where it should not be\n");
                    return 1;
                }

                break;
            }
            case ':':
            {
                (**tok_spp).id0 = TOK0_COLON;
                (**tok_spp).id1 = TOK1_COLON;
                ++(*tok_spp);
                ++(*pp_buf);
                break;
            }
            case ',':
            {
                (**tok_spp).id0 = TOK0_COMMA;
                (**tok_spp).id1 = TOK1_COMMA;
                ++(*tok_spp);
                ++(*pp_buf);
                break;
            }
            case '[':
            {
                (**tok_spp).id0 = TOK0_LSQUARE_BR;
                (**tok_spp).id1 = TOK1_LSQUARE_BR;
                ++(*tok_spp);
                ++(*pp_buf);
                break;
            }
            case ']':
            {
                (**tok_spp).id0 = TOK0_RSQUARE_BR;
                (**tok_spp).id1 = TOK1_RSQUARE_BR;
                ++(*tok_spp);
                ++(*pp_buf);
                break;
            }
            case ';':
            {
                while (*((*pp_buf)++) != '\n');

                (**tok_spp).id0 = TOK0_EOL;
                (**tok_spp).id1 = TOK1_EOL;
                ++(*tok_spp);
                ++(*pp_buf);
                break;
            }
            default:
            {
                printf("cannot match input with any token\n");
                return 1;
            }
            } // switch (**pp_buf)
        }
    } // for(;;)

    return 0;
}

int lexer_num(char             **pp_buf,
              struct LexTok    **pp_tok,
              struct LexTokNum **num_spp,
              const char         sign[2])
{
    char *start = *pp_buf;
    char *end = *pp_buf;
    while (isdigit(*end))
        ++(end);

    long num_str_len = end - start;
    assert(num_str_len <= LEN_MAX_NUM_STR);

    char num_str[LEN_MAX_NUM_STR + 1];
    strlcpy(num_str, sign, num_str_len + 2);
    strlcat(num_str, *pp_buf, num_str_len + 2);
    long num = strtol(num_str, NULL, 10);

    assert(errno != EINVAL);
    assert(errno != ERANGE);

    num = num & (LEN_SEGMENT - 1);

    (**pp_tok).id0 = TOK0_NUM;
    if (num == 0)
        (**pp_tok).id1 = TOK1_NUM_ZERO;
    else
        (**pp_tok).id1 = TOK1_NUM;
    (**pp_tok).p_num = (*num_spp)++;
    (**pp_tok).p_num -> value = num;

    ++(*pp_tok);
    *pp_buf += num_str_len;

    return 0;
}
