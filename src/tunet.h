#ifndef TUNET_H
#define TUNET_H

void tunet_init();
void tunet_cleanup();
void net_login(const char *username, const char *password);
void auth4_login(const char *username, const char *password);
void auth6_login(const char *username, const char *password);

#endif
