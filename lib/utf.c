#include <stdlib.h>
#include <string.h>
#include <utf8proc.h>

#include "utf.h"

int char_code_at(utf_char a)
{
    int r;
    utf8proc_iterate(a.data, a.length, &r);
    return r;
}

utf_char from_char_code(uint32_t a)
{
    char f[4];
    int len = utf8proc_encode_char(a, (unsigned char *)f);
    char *r = malloc(len + 1);
    strncpy(r, f, len);
    r[len] = 0;

    utf_char uchar;
    uchar.data = (unsigned char *)r;
    uchar.length = len;

    return uchar;
}
