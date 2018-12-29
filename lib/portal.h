#include "sds.h"

sds x_encode(const sds str, const sds key);
sds sha1(const char *str);
sds md5(const char *str);
sds md5_hmac(const char *key, const char *data);
