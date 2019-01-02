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

    TUNET_DLLEXPORT void tunet_init();
    TUNET_DLLEXPORT void tunet_cleanup();
    TUNET_DLLEXPORT void net_login(const char *username, const char *password);
    TUNET_DLLEXPORT void auth4_login(const char *username, const char *password);
    TUNET_DLLEXPORT void auth6_login(const char *username, const char *password);
    TUNET_DLLEXPORT void net_logout();
    TUNET_DLLEXPORT void auth4_logout();
    TUNET_DLLEXPORT void auth6_logout();

#ifdef __cplusplus
}
#endif

#endif
