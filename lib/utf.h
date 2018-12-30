#ifndef UTF_H
#define UTF_H

#include <stdint.h>

struct utf_char
{
    unsigned char *data;
    int length;
};
typedef struct utf_char utf_char;

struct utf_string
{
    utf_char *data;
    int length;
};
typedef struct utf_string utf_string;

int char_code_at(utf_char a);
utf_char from_char_code(uint32_t a);

#endif
