#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <curl/curl.h>
#include "../lib/cJSON.h"
#include "../lib/sds.h"

#include "tunet.h"
#include "../lib/portal.h"

#define NET 0
#define AUTH4 4
#define AUTH6 6

static const char *NET_URL = "https://net.tsinghua.edu.cn/do_login.php";
static const char *AUTH4_URL = "https://auth4.tsinghua.edu.cn/cgi-bin/srun_portal";
static const char *AUTH6_URL = "https://auth6.tsinghua.edu.cn/cgi-bin/srun_portal";
static const char *AUTH4_CHALLENGE_URL = "https://auth4.tsinghua.edu.cn/cgi-bin/get_challenge";
static const char *AUTH6_CHALLENGE_URL = "https://auth6.tsinghua.edu.cn/cgi-bin/get_challenge";

static size_t challenge_callback(void *contents, size_t size, size_t nmemb, void *userp)
{
    sds *challenge = (sds *)userp;
    sds callback = sdsnew(contents);
    int length = sdslen(callback);
    sdsrange(callback, 9, length - 2);

    cJSON *json = cJSON_Parse(callback);
    if (json == NULL)
    {
        fprintf(stderr, "Error: parse challenge JSON\n");
        challenge = NULL;
    }

    cJSON *challenge_pair = cJSON_GetObjectItemCaseSensitive(json, "challenge");
    if (cJSON_IsString(challenge_pair) && (challenge_pair->valuestring != NULL))
        *challenge = sdsnew(challenge_pair->valuestring);

    cJSON_Delete(json);
    sdsfree(callback);

    return length;
}

static size_t auth_login_callback(void *contents, size_t size, size_t nmemb, void *userp)
{
    sds *message = (sds *)userp;
    *message = sdsnew(contents);
    int length = sdslen(*message);

    return length;
}

static sds get_challenge(CURL *curl, const char *username, char stack)
{
    sds composed_url = sdscatprintf(sdsempty(),
                                    "%s?username=%s&double_stack=1&ip&callback=callback",
                                    stack == AUTH4 ? AUTH4_CHALLENGE_URL : AUTH6_CHALLENGE_URL,
                                    username);
    sds challenge;

    curl_easy_setopt(curl, CURLOPT_URL, composed_url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, challenge_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&challenge);
    CURLcode success = curl_easy_perform(curl);

    if (success != CURLE_OK)
    {
        fprintf(stderr, "Error: get challenge\n");
        challenge = NULL;
    }
    sdsfree(composed_url);

    return challenge;
}

static void auth_login(const char *username, const char *password, char stack)
{
    CURL *curl = curl_easy_init();
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3L);

    sds challenge = get_challenge(curl, username, stack);
    if (challenge == NULL)
        return;

    sds info = sdscatprintf(sdsempty(),
                            "{\"username\":\"%s\",\"password\":\"%s\",\"ip\":\"\",\"acid\":\"1\",\"enc_ver\":\"srun_bx1\"}",
                            username,
                            password);
    sds encoded_info = x_encode(info, challenge);
    sds full_encoded_info = sdscatprintf(sdsempty(), "{SRBX1}%s", encoded_info);
    char *url_encoded_info = curl_easy_escape(curl, full_encoded_info, sdslen(full_encoded_info));

    sds password_md5 = md5_hmac(challenge, password);
    sds full_password_md5 = sdscatprintf(sdsempty(), "{MD5}%s", password_md5);
    char *url_encoded_password_md5 = curl_easy_escape(curl, full_password_md5, sdslen(full_password_md5));
    sds combined = sdscatprintf(sdsempty(), "%s%s%s%s%s1%s%s200%s1%s{SRBX1}%s",
                                challenge, username, challenge, password_md5, challenge,
                                challenge, challenge, challenge, challenge, encoded_info);
    sds chksum = sha1(combined);

    sds data = sdscatprintf(sdsempty(),
                            "info=%s&action=login&double_stack=1&ac_id=1&n=200&type=1&username=%s&password=%s&chksum=%s",
                            url_encoded_info,
                            username,
                            url_encoded_password_md5,
                            chksum);

    sds message;
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, auth_login_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&message);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, sdslen(data));
    curl_easy_setopt(curl, CURLOPT_URL, stack == AUTH4 ? AUTH4_URL : AUTH6_URL);
    CURLcode success = curl_easy_perform(curl);

    if (success != CURLE_OK || strncmp(message, "login_ok", 8))
        fprintf(stderr, "Error: auth%d login\n\tMessage: %s\n", stack, message);
    else
        printf("Success: auth%d login\n", stack);

    sdsfree(full_encoded_info);
    sdsfree(full_password_md5);
    sdsfree(message);
    sdsfree(password_md5);
    sdsfree(info);
    sdsfree(challenge);
    sdsfree(encoded_info);
    sdsfree(combined);
    sdsfree(chksum);
    sdsfree(data);
    curl_free(url_encoded_info);
    curl_free(url_encoded_password_md5);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
}

TUNET_DLLEXPORT void net_login(const char *username, const char *password)
{
    CURL *curl = curl_easy_init();
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    sds password_md5 = md5(password);
    sds composed_url = sdscatprintf(sdsempty(),
                                    "%s?action=login&username=%s&password={MD5_HEX}%s&ac_id=1",
                                    NET_URL,
                                    username,
                                    password_md5);

    sds message;
    curl_easy_setopt(curl, CURLOPT_URL, composed_url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, auth_login_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&message);
    CURLcode success = curl_easy_perform(curl);

    if (success != CURLE_OK || strncmp(message, "Login is successful", 19))
        fprintf(stderr, "Error: net login\n\tMessage: %s\n", message);
    else
        printf("Success: net login\n");

    sdsfree(composed_url);
    sdsfree(password_md5);
    sdsfree(message);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
}

static void logout(char stack)
{
    CURL *curl = curl_easy_init();
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    sds composed_url = sdscatprintf(sdsempty(),
                                    "%s?action=logout",
                                    stack == NET ? NET_URL : stack == AUTH4 ? AUTH4_URL : AUTH6_URL);

    sds message;
    curl_easy_setopt(curl, CURLOPT_URL, composed_url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, auth_login_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&message);
    CURLcode success = curl_easy_perform(curl);

    if (stack == NET)
    {
        if (success != CURLE_OK || strncmp(message, "Logout is successful", 20))
            fprintf(stderr, "Error: net logout\n\tMessage: %s\n", message);
        else
            printf("Success: net logout\n");
    }
    else
    {
        if (success != CURLE_OK || strncmp(message, "logout_ok", 9))
            fprintf(stderr, "Error: auth%d logout\n\tMessage: %s\n", stack, message);
        else
            printf("Success: auth%d logout\n", stack);
    }

    sdsfree(composed_url);
    sdsfree(message);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
}

TUNET_DLLEXPORT void auth4_login(const char *username, const char *password)
{
    auth_login(username, password, AUTH4);
}

TUNET_DLLEXPORT void auth6_login(const char *username, const char *password)
{
    auth_login(username, password, AUTH6);
}

TUNET_DLLEXPORT void net_logout()
{
    logout(NET);
}

TUNET_DLLEXPORT void auth4_logout()
{
    logout(AUTH4);
}

TUNET_DLLEXPORT void auth6_logout()
{
    logout(AUTH6);
}

TUNET_DLLEXPORT void tunet_init()
{
    curl_global_init(CURL_GLOBAL_ALL);
}

TUNET_DLLEXPORT void tunet_cleanup()
{
    curl_global_cleanup();
}
