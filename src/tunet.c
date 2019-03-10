#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <curl/curl.h>
#include <cJSON.h>
#include <sds.h>
#include <tidy.h>
#include <tidybuffio.h>

#include "tunet.h"
#include "../lib/portal.h"
#include "../lib/parser.h"

#define NET 0
#define AUTH4 4
#define AUTH6 6

static const char *NET_URL = "https://net.tsinghua.edu.cn/do_login.php";
static const char *AUTH4_URL = "https://auth4.tsinghua.edu.cn/cgi-bin/srun_portal";
static const char *AUTH6_URL = "https://auth6.tsinghua.edu.cn/cgi-bin/srun_portal";
static const char *AUTH4_CHALLENGE_URL = "https://auth4.tsinghua.edu.cn/cgi-bin/get_challenge";
static const char *AUTH6_CHALLENGE_URL = "https://auth6.tsinghua.edu.cn/cgi-bin/get_challenge";

static const char *USEREG_LOGIN_URL = "https://usereg.tsinghua.edu.cn/do.php";
static const char *USEREG_SESSIONS_URL = "https://usereg.tsinghua.edu.cn/online_user_ipv4.php";
static const char *NET_USER_INFO_URL = "https://net.tsinghua.edu.cn/rad_user_info.php";
static const char *USEREG_USER_DETAIL_URL = "https://usereg.tsinghua.edu.cn/user_detail_list.php";

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

static size_t result_callback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t length = size * nmemb;
    sds *message = (sds *)userp;
    *message = sdscat(*message, contents);
    return length;
}

static size_t write_to_tidy_callback(char *in, size_t size, size_t nmemb, TidyBuffer *out)
{
    size_t r = size * nmemb;
    tidyBufAppend(out, in, r);
    return r;
}

static sds get_challenge(CURL *curl, const char *username, char stack)
{
    sds composed_url = sdscatprintf(sdsempty(),
                                    "%s?username=%s&float_stack=1&ip&callback=callback",
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

static res check_response(sds message)
{
    res response = UNKNOWN_ERR;

    if (strstr(message, "E2553") != NULL ||
        strstr(message, "E2531") != NULL ||
        strstr(message, "E5992") != NULL)
        response = WRONG_CREDENTIAL;

    else if (strstr(message, "E3004") != NULL ||
             strstr(message, "E2616") != NULL)
        response = OUT_OF_BALANCE;

    else if (strstr(message, "E2532") != NULL)
        response = TOO_SHORT_INTERVAL;

    else if (strstr(message, "E2533") != NULL)
        response = TOO_MANY_ATTEMPTS;

    else if (strstr(message, "E2620") != NULL)
        response = ALREADY_ONLINE;

    else if (strstr(message, "E2833") != NULL)
        response = INVALID_IP;

    return response;
}

static res auth_login(const char *username, const char *password, char stack)
{
    CURL *curl = curl_easy_init();
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3L);
#ifdef __ANDROID__
    curl_easy_setopt(curl, CURLOPT_CAINFO, CA_BUNDLE_PATH);
#endif

    sds challenge = get_challenge(curl, username, stack);
    if (challenge == NULL)
        return EMPTY_CHALLENGE;

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
                            "info=%s&action=login&float_stack=1&ac_id=1&n=200&type=1&username=%s&password=%s&chksum=%s",
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

    res response = UNKNOWN_ERR;
    if (success != CURLE_OK || strncmp(message, "login_ok", 8))
    {
        fprintf(stderr, "Error: auth%d login\n\tMessage: %s\n", stack, message);

        if (success == CURLE_OK)
            response = check_response(message);
    }
    else
    {
        printf("Success: auth%d login\n", stack);
        response = SUCCESS;
    }

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

    return response;
}

TUNET_DLLEXPORT res net_login(const char *username, const char *password)
{
    CURL *curl = curl_easy_init();
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3L);
#ifdef __ANDROID__
    curl_easy_setopt(curl, CURLOPT_CAINFO, CA_BUNDLE_PATH);
#endif

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

    res response = UNKNOWN_ERR;
    if (success != CURLE_OK || strncmp(message, "Login is successful", 19))
    {
        fprintf(stderr, "Error: net login\n\tMessage: %s\n", message);

        if (success == CURLE_OK)
        {
            if (!strncmp(message, "IP has been online", 18))
                response = ALREADY_ONLINE;
            else
                response = check_response(message);
        }
    }
    else
    {
        printf("Success: net login\n");
        response = SUCCESS;
    }

    sdsfree(composed_url);
    sdsfree(password_md5);
    sdsfree(message);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    return response;
}

static res logout(char stack)
{
    CURL *curl = curl_easy_init();
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3L);
#ifdef __ANDROID__
    curl_easy_setopt(curl, CURLOPT_CAINFO, CA_BUNDLE_PATH);
#endif

    sds composed_url = sdscatprintf(sdsempty(),
                                    "%s?action=logout",
                                    stack == NET ? NET_URL : stack == AUTH4 ? AUTH4_URL : AUTH6_URL);

    sds message;
    curl_easy_setopt(curl, CURLOPT_URL, composed_url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, auth_login_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&message);
    CURLcode success = curl_easy_perform(curl);

    res response = UNKNOWN_ERR;
    if (stack == NET)
    {
        if (success != CURLE_OK || strncmp(message, "Logout is successful", 20))
        {
            fprintf(stderr, "Error: net logout\n\tMessage: %s\n", message);

            if (success == CURLE_OK)
                response = check_response(message);
        }
        else
        {
            printf("Success: net logout\n");
            response = SUCCESS;
        }
    }
    else
    {
        if (success != CURLE_OK || strncmp(message, "logout_ok", 9))
        {
            fprintf(stderr, "Error: auth%d logout\n\tMessage: %s\n", stack, message);

            if (success == CURLE_OK)
                response = check_response(message);
        }
        else
        {
            printf("Success: auth%d logout\n", stack);
            response = SUCCESS;
        }
    }

    sdsfree(composed_url);
    sdsfree(message);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    return response;
}

TUNET_DLLEXPORT res usereg_login(CURL *curl, const char *username, const char *password)
{
    sds password_md5 = md5(password);
    sds data = sdscatprintf(sdsempty(),
                            "action=login&user_login_name=%s&user_password=%s",
                            username, password_md5);
    sds message = sdsempty();

    curl_easy_setopt(curl, CURLOPT_URL, USEREG_LOGIN_URL);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&message);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, result_callback);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, sdslen(data));
    CURLcode success = curl_easy_perform(curl);

    res response = UNKNOWN_ERR;
    if (success != CURLE_OK)
    {
        fprintf(stderr, "Error: usereg login\n");
    }
    if (strncmp(message, "ok", 2))
    {
        response = WRONG_CREDENTIAL;
    }
    else
    {
        response = SUCCESS;
    }

    sdsfree(data);
    sdsfree(password_md5);
    sdsfree(message);

    return response;
}

TUNET_DLLEXPORT char *get_sessions(CURL *curl)
{
    TidyDoc tdoc = tidyCreate();
    TidyBuffer docbuf = {0};
    tidyOptSetBool(tdoc, TidyShowWarnings, no);
    tidyOptSetBool(tdoc, TidyQuiet, yes);
    tidyBufInit(&docbuf);

    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&docbuf);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_to_tidy_callback);
    curl_easy_setopt(curl, CURLOPT_URL, USEREG_SESSIONS_URL);
    CURLcode success = curl_easy_perform(curl);

    if (success != CURLE_OK)
    {
        fprintf(stderr, "Error: usereg get sessions\n");
        return NULL;
    }

    cJSON *results = cJSON_CreateArray();
    tidyParseBuffer(tdoc, &docbuf);
    parse_sessions_page(tdoc, tidyGetRoot(tdoc), results);
    sessions_count = 0;

    char *string = cJSON_PrintUnformatted(results);

    cJSON_Delete(results);
    tidyBufFree(&docbuf);
    tidyRelease(tdoc);

    return string;
}

TUNET_DLLEXPORT void drop_session(CURL *curl, const char *id)
{
    sds data = sdscatprintf(sdsempty(), "action=drops&user_ip=%s,", id);
    sds message = sdsempty();

    curl_easy_setopt(curl, CURLOPT_URL, USEREG_SESSIONS_URL);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&message);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, result_callback);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, sdslen(data));
    CURLcode success = curl_easy_perform(curl);

    sdsfree(message);
    sdsfree(data);
}

TUNET_DLLEXPORT float get_usage()
{
    CURL *curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3L);
#ifdef __ANDROID__
    curl_easy_setopt(curl, CURLOPT_CAINFO, CA_BUNDLE_PATH);
#endif

    sds message = sdsempty();
    curl_easy_setopt(curl, CURLOPT_URL, NET_USER_INFO_URL);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&message);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, result_callback);
    CURLcode success = curl_easy_perform(curl);

    int begin = 0;
    int end = 0;
    int i;
    int count = 0;
    int len = sdslen(message);
    for (i = 0; i < len; i++)
    {
        if (message[i] == ',')
            count++;
        if (count == 6)
            begin = i + 1;
        if (count == 7)
        {
            end = i;
            break;
        }
    }

    sdsrange(message, begin, end);
    float usage = strtod(message, NULL);
    printf("%s\n", message);

    sdsfree(message);
    curl_easy_cleanup(curl);

    return usage;
}

TUNET_DLLEXPORT float get_usage_detail(CURL *curl, const char *start_time, const char *end_time)
{
    TidyDoc tdoc = tidyCreate();
    TidyBuffer docbuf = {0};
    tidyOptSetBool(tdoc, TidyShowWarnings, no);
    tidyOptSetBool(tdoc, TidyQuiet, yes);
    tidyBufInit(&docbuf);

    sds composed_url = sdscatprintf(sdsempty(),
                                    "%s?action=query&start_time=%s&end_time=%s&offset=10000",
                                    USEREG_USER_DETAIL_URL,
                                    start_time,
                                    end_time);

    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&docbuf);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_to_tidy_callback);
    curl_easy_setopt(curl, CURLOPT_URL, composed_url);
    CURLcode success = curl_easy_perform(curl);

    if (success != CURLE_OK)
    {
        fprintf(stderr, "Error: usereg get user detail\n");
    }

    tidyParseBuffer(tdoc, &docbuf);
    float sum = 0;
    parse_user_detail_page(tdoc, tidyGetRoot(tdoc), &sum);

    tidyBufFree(&docbuf);
    tidyRelease(tdoc);
    sdsfree(composed_url);

    return sum;
}

TUNET_DLLEXPORT res auth4_login(const char *username, const char *password)
{
    return auth_login(username, password, AUTH4);
}

TUNET_DLLEXPORT res auth6_login(const char *username, const char *password)
{
    return auth_login(username, password, AUTH6);
}

TUNET_DLLEXPORT res net_logout()
{
    return logout(NET);
}

TUNET_DLLEXPORT res auth4_logout()
{
    return logout(AUTH4);
}

TUNET_DLLEXPORT res auth6_logout()
{
    return logout(AUTH6);
}

TUNET_DLLEXPORT char *usereg_get_sessions(const char *username, const char *password)
{
    CURL *curl = curl_easy_init();
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3L);
    curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "");
#ifdef __ANDROID__
    curl_easy_setopt(curl, CURLOPT_CAINFO, CA_BUNDLE_PATH);
#endif

    usereg_login(curl, username, password);
    char *results = get_sessions(curl);

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    return results;
}

TUNET_DLLEXPORT void usereg_drop_session(const char *username, const char *password, const char *session_id)
{
    CURL *curl = curl_easy_init();
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3L);
    curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "");
#ifdef __ANDROID__
    curl_easy_setopt(curl, CURLOPT_CAINFO, CA_BUNDLE_PATH);
#endif

    usereg_login(curl, username, password);
    drop_session(curl, session_id);

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
}

TUNET_DLLEXPORT float usereg_get_usage()
{
    return get_usage();
}

TUNET_DLLEXPORT float usereg_get_usage_detail(const char *username, const char *password,
                                              const char *start_time, const char *end_time)
{
    CURL *curl = curl_easy_init();
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3L);
    curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "");
#ifdef __ANDROID__
    curl_easy_setopt(curl, CURLOPT_CAINFO, CA_BUNDLE_PATH);
#endif

    usereg_login(curl, username, password);
    float sum = get_usage_detail(curl, start_time, end_time);

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    return sum;
}

TUNET_DLLEXPORT void tunet_init()
{
    curl_global_init(CURL_GLOBAL_ALL);
}

TUNET_DLLEXPORT void tunet_cleanup()
{
    curl_global_cleanup();
}
