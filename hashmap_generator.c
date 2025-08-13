/*
    The idea is be able to generate an header file at compile time that contains the hash map representation
    of table 4.10 in the Intel manual
    The table will be used after the tokenization pass, it means I can use the tok enum as keys to the hashtable
    Example: [bx + si] -> after tokenization I got TOK_BX TOK_PLUS TOK_SI -> I can avoid considering the plus token
    and just taking TOK_BX, TOK_SI (TOK_PLUS is implicit)
    I want to obtain one key from this: I can have the map (TOK_BX, TOK_SI) -> TOK_BX + N * TOK_SI where N is the number
    of tokens, this will be the key that I use to query the hashmap -> hash_the_number(TOK_BX + N * TOK_SI) -> retrieve
    the rm and mod field
    This is all really cool and I want to deep dive into it at the appropriate time, but for now lets just do the simplest
    and dumbest thing, use the preprocessor to generate the table and linear search through it (slow and dumb but it works)
*/