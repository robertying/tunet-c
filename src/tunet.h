#ifndef TUNET_H
#define TUNET_H

#ifdef TUNET_STATIC
#define TUNET_DLLEXPORT
#else
#ifdef _WIN32
#ifdef TUNET_EXPORTS
#define TUNET_DLLEXPORT __declspec(dllexport)
#else
#define TUNET_DLLEXPORT __declspec(dllimport)
#endif
#elif __GNUC__ >= 4
#define TUNET_DLLEXPORT __attribute__((visibility("default")))
#else
#define TUNET_DLLEXPORT
#endif
#endif

#ifdef __cplusplus
extern "C"
{
#endif

    typedef enum
    {
        SUCCESS = 1,
        UNKNOWN_ERR,
        EMPTY_CHALLENGE,
        WRONG_CREDENTIAL,    // E2553 E2531 E5992
        OUT_OF_BALANCE,      // E3004 E2616
        TOO_SHORT_INTERVAL,  // E2532
        TOO_MANY_ATTEMPTS,   // E2533
        ALREADY_ONLINE,      // E2620
        INVALID_IP,          // E2833
        POSSIBLY_WRONG_ACID, // sign_error
    } res;

    static const char *res_string[] = {
        "NULL",
        "SUCCESS",
        "UNKNOWN_ERR",
        "EMPTY_CHALLENGE",
        "WRONG_CREDENTIAL",
        "OUT_OF_BALANCE",
        "TOO_SHORT_INTERVAL",
        "TOO_MANY_ATTEMPTS",
        "ALREADY_ONLINE",
        "INVALID_IP",
        "POSSIBLY_WRONG_ACID"};

#ifdef __ANDROID__
    extern char *CA_BUNDLE_PATH;
#endif
    extern int sessions_count;

    TUNET_DLLEXPORT void tunet_init();
    TUNET_DLLEXPORT void tunet_cleanup();
    TUNET_DLLEXPORT res net_login(const char *username, const char *password);
    TUNET_DLLEXPORT res auth4_login(const char *username, const char *password);
    TUNET_DLLEXPORT res auth6_login(const char *username, const char *password);
    TUNET_DLLEXPORT res net_logout();
    TUNET_DLLEXPORT res auth4_logout();
    TUNET_DLLEXPORT res auth6_logout();
    TUNET_DLLEXPORT res usereg_login(const char *username, const char *password);
    TUNET_DLLEXPORT char *usereg_get_sessions(const char *username, const char *password);
    TUNET_DLLEXPORT void usereg_drop_session(const char *username, const char *password, const char *session_id);
    TUNET_DLLEXPORT float usereg_get_usage();
    TUNET_DLLEXPORT float usereg_get_balance();
    TUNET_DLLEXPORT float usereg_get_usage_detail(const char *username, const char *password,
                                                  const char *start_time, const char *end_time);

#ifdef __cplusplus
}
#endif

#endif
