#include <stdlib.h>
#include <stdio.h>
#include "lib/portal.h"

int main(int argc, char const *argv[])
{
    char *username = "yingr16";
    char *password = "password";
    sds payload = sdscatprintf(sdsempty(), "{\"username\":\"%s\",\"password\":\"%s\",\"ip\":\"\",\"acid\":\"1\",\"enc_ver\":\"srun_bx1\"}", username, password);
    sds key = sdsnew("16edd2b7e700eb65fa78240e347305aae6c9e4255e1863f9d6fdc4df72a1b5da");
    sds encoded = x_encode(payload, key);
    sds correct = sdsnew("LrvAVQFkpzBHl6vFIlTEK8LP2SwJ7CDt0Uqm/rYMHEaN0J5bwNaUH1Hbu4siLQNP+yizTpj8W9zhlzuDzymcUOxzsGX7lfbNk+Sofg4c216ODkZ8hwUm3L==");

    printf("Correct encoded: %s\nEncoded payload: %s\n", correct, encoded);
    if (sdscmp(encoded, correct))
        abort();

    sdsfree(payload);
    sdsfree(correct);
    sdsfree(key);
    sdsfree(encoded);

    printf("Encode test passed\n");
    return 0;
}
