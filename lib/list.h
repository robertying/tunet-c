#include "stdint.h"

typedef uint32_t number;

struct number_list
{
    number *data;
    int length;
};
typedef struct number_list number_list;

number_list *new_number_list(int length);
void free_number_list(number_list *array);
void add_to_number_list(number_list *array, number value);
void number_list_set(number_list *array, int index, number value);
number number_list_get(number_list *array, int index);
void change_number_list_length(number_list *array, int length);
