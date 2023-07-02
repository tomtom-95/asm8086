#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#include "asm8086_helper.h"

void get_basename(const char *filename, char *basename)
{
    const char *extension = filename + strlen(filename);
    while (*extension != '.')
        extension--;

    size_t len = strlen(filename) - strlen(extension) + 1;

    strlcpy(basename, filename, len);
}

void write_inst_to_file(struct InstEnc inst, FILE* file)
{
    u8 bytes[LEN_MAX_INST_BYTES];

    for (size_t i = 0; i < inst.len; ++i)
        bytes[i] = (inst.enc >> (8 * (inst.len - i - 1))) & 0xff;

    fwrite(bytes, 1, inst.len, file);
}
