#include <stdlib.h>

#include "list.h"

number_list *new_number_list(int length)
{
    number_list *new_array = (number_list *)malloc(sizeof(number_list));

    if (length == 0)
        new_array->data = NULL;
    else
        new_array->data = malloc(length * sizeof(number));
    new_array->length = length;

    return new_array;
}

void free_number_list(number_list *array)
{
    if (array->length)
        free(array->data);
    free(array);
}

void add_to_number_list(number_list *array, number value)
{
    int length = array->length;
    number *data = array->data;

    if (data)
        array->data = realloc(data, (length + 1) * sizeof(number));
    else
        array->data = malloc(sizeof(number));
    array->data[length] = value;
    array->length = length + 1;
}

void number_list_set(number_list *array, int index, number value)
{
    array->data[index] = value;
}

number number_list_get(number_list *array, int index)
{
    return array->data[index];
}

void change_number_list_length(number_list *array, int length)
{
    number *data = array->data;

    array->data = realloc(data, length * sizeof(number));
    array->length = length;
}
