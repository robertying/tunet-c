#include <stdio.h>
#include <stdlib.h>

#include "lib/list.h"

int main(int argc, char const *argv[])
{
    number_list *array = new_number_list(0);
    printf("number_list at %p with length %d\n", array->data, array->length);
    if (array->data != 0)
        abort();
    if (array->length != 0)
        abort();

    add_to_number_list(array, 10000);
    printf("number_list with length %d has element %d at index %d\n", array->length, number_list_get(array, 0), 0);
    if (array->length != 1)
        abort();
    if (number_list_get(array, 0) != 10000)
        abort();
    free_number_list(array);

    array = new_number_list(5);
    printf("number_list at %p with length %d\n", array->data, array->length);
    if (array->length != 5)
        abort();

    number_list_set(array, 4, 20000);
    printf("number_list has element %d at index %d\n", number_list_get(array, 4), 4);
    if (number_list_get(array, 4) != 20000)
        abort();

    free_number_list(array);
    printf("List test passed\n");

    return 0;
}
