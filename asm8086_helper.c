#include <stdlib.h>
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

