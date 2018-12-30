#include <string.h>

#include <openssl/sha.h>
#include <openssl/md5.h>
#include <openssl/hmac.h>
#include "../utf8proc/utf8proc.h"

#include "list.h"
#include "utf.h"
#include "portal.h"

#define true 1
#define false 0

static number_list *s(sds a, bool b)
{
    int c = sdslen(a);
    number_list *v = new_number_list((c + 3) / 4);

    int i;
    for (i = 0; i < c; i += 4)
    {
        number d = (number)a[i] |
                   (number)a[i + 1] << 8 |
                   (number)a[i + 2] << 16 |
                   (number)a[i + 3] << 24;
        number_list_set(v, i >> 2, d);
    }

    if (b)
        add_to_number_list(v, c);

    return v;
}

static utf_string *l(number_list *a, bool b)
{
    int d = a->length;
    int c = (d - 1) << 2;

    if (b)
    {
        number m = number_list_get(a, d - 1);
        if (m < c - 3 || m > c)
            return NULL;
        c = m;
    }

    utf_string *e = (utf_string *)calloc(1, sizeof(utf_string));
    e->data = (utf_char *)malloc(d * 4 * sizeof(utf_char));
    e->length = d * 4;

    int i;
    for (i = 0; i < d; i++)
    {
        utf_char *data = e->data;
        data[4 * i] = from_char_code(number_list_get(a, i) & 0xff);
        data[4 * i + 1] = from_char_code(number_list_get(a, i) >> 8 & 0xff);
        data[4 * i + 2] = from_char_code(number_list_get(a, i) >> 16 & 0xff);
        data[4 * i + 3] = from_char_code(number_list_get(a, i) >> 24 & 0xff);
    }

    return e;
}

static sds base64_encode(utf_string *s)
{
    char *n = "LVoJPiCN2R8G90yg+hmFHuacZ1OWMnrsSTXkYpUq/3dlbfKwv6xztjI7DeBE45QA";
    int o, i, h;
    int a = s->length;
    utf_char *t = s->data;
    sds u = sdsempty();

    for (o = 0; o < a; o += 3)
    {
        h = char_code_at(t[o]) << 16 |
            (o + 1 < a ? char_code_at(t[o + 1]) << 8 : 0) |
            (o + 2 < a ? char_code_at(t[o + 2]) : 0);
        for (i = 0; i < 4; i++)
        {
            if (o * 8 + i * 6 > a * 8)
                u = sdscat(u, "=");
            else
            {
                const char c[2] = {n[h >> 6 * (3 - i) & 63], 0};
                u = sdscat(u, (const char *)&c);
            }
        }
    }

    return u;
}

static utf_string *encode(sds str, sds key)
{
    if (!strcmp(str, ""))
    {
        return NULL;
    }

    number_list *v = s(str, 1);
    number_list *k = s(key, 0);
    if (k->length < 4)
    {
        change_number_list_length(k, 4);
    }

    int n = v->length - 1;
    number z = number_list_get(v, n);
    number y = number_list_get(v, 0);
    int q = 6 + 52 / (n + 1);
    uint32_t d = 0;

    while (q-- > 0)
    {
        d += 0x9E3779B9;
        uint32_t e = (d >> 2) & 3;
        int p;
        for (p = 0; p <= n; p++)
        {
            y = number_list_get(v, p == n ? 0 : p + 1);
            uint32_t m = (z >> 5) ^ (y << 2);
            m += (y >> 3) ^ (z << 4) ^ (d ^ y);
            m += number_list_get(k, (p & 3) ^ (int)e) ^ z;
            number_list_set(v, p, number_list_get(v, p) + m);
            z = number_list_get(v, p);
        }
    }

    utf_string *result = l(v, false);
    free_number_list(k);
    free_number_list(v);

    return result;
}

sds x_encode(const sds str, const sds key)
{
    return base64_encode(encode(str, key));
}

sds sha1(const char *str)
{
    unsigned char md[SHA_DIGEST_LENGTH];
    SHA1((unsigned char *)str, strlen(str), md);

    sds s = sdsempty();
    int i;
    for (i = 0; i < SHA_DIGEST_LENGTH; i++)
    {
        s = sdscatprintf(s, "%02x", md[i]);
    }

    return s;
}

sds md5(const char *str)
{
    unsigned char md[MD5_DIGEST_LENGTH];
    MD5((unsigned char *)str, strlen(str), md);

    sds s = sdsempty();
    int i;
    for (i = 0; i < MD5_DIGEST_LENGTH; i++)
    {
        s = sdscatprintf(s, "%02x", md[i]);
    }

    return s;
}

sds md5_hmac(const char *key, const char *data)
{
    unsigned int md_len;
    unsigned char md[EVP_MAX_MD_SIZE];

    HMAC(EVP_md5(), key, strlen(key),
         (unsigned char *)data, strlen(data),
         md, &md_len);
    md[md_len] = 0;

    sds s = sdsempty();
    int i;
    for (i = 0; i < md_len; i++)
    {
        s = sdscatprintf(s, "%02x", md[i]);
    }

    return s;
}
