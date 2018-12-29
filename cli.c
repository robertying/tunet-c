#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>

#include "tunet.h"

void print_help()
{
    printf("\ntunet-c v0.1.0\n");
    printf("    C cli and library for Tsinghua University network login\n");
    printf("    2018 © Robert Ying\n\n");
    printf("Usage:\n");
    printf("\t-u\t Tsinghua username\n");
    printf("\t-p\t Tsinghua password\n");
    printf("\t\t    Caution: enter password in cli is not recommended\n");
    printf("\t\t    Please use config file instead\n");
    printf("\t-m\t Network choice: [net auth auth4 auth6]\n");
    printf("\t-c\t Credential config file path\n");
    printf("\t\t    Username should be in the first line and password in the second\n");
    printf("\t-h\t Usage help\n\n");
}

void read_config(const char *path, char **username, char **password)
{
    FILE *fp;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    fp = fopen(path, "r");
    if (fp == NULL)
    {
        fprintf(stderr, "Config file does not exist or is not accessible\n");
        exit(1);
    }

    if ((read = getline(&line, &len, fp)) != -1)
    {
        char *u = strdup(line);
        u[strlen(u) - 1] = 0;
        *username = u;
    }
    if ((read = getline(&line, &len, fp)) != -1)
    {
        char *p = strdup(line);
        p[strlen(p) - 1] = 0;
        *password = p;
    }

    if (!(*username && *password))
    {
        fprintf(stderr, "Error reading config file\n");
        exit(1);
    }

    fclose(fp);
}

int main(int argc, char **argv)
{
    char *username = NULL;
    char *password = NULL;
    char *mode = NULL;
    char *config = NULL;
    char show_help = 0;
    int c;
    opterr = 0;

    while ((c = getopt(argc, argv, "u:p:m:c:h")) != -1)
        switch (c)
        {
        case 'u':
            username = optarg;
            break;
        case 'p':
            password = optarg;
            break;
        case 'm':
            mode = optarg;
            break;
        case 'c':
            config = optarg;
            break;
        case 'h':
            show_help = 1;
            break;
        case '?':
            if (optopt == 'u')
                fprintf(stderr, "Please enter the username\n");
            else if (optopt == 'p')
                fprintf(stderr, "Please enter the password\n");
            else if (optopt == 'm')
                fprintf(stderr, "Please select one mode: [net auth auth4 auth6]\n");
            else if (optopt == 'c')
                fprintf(stderr, "Please specify a credential config path\n");
            else if (isprint(optopt))
                fprintf(stderr, "Unknown option `-%c'.\n", optopt);
            else
                fprintf(stderr,
                        "Unknown option character `\\x%x'.\n",
                        optopt);
            return 1;
        default:
            abort();
        }

    if (show_help)
    {
        print_help();
        exit(0);
    }

    if (!mode)
    {
        fprintf(stderr, "Please use -m to select the mode: [net auth auth4 auth6]\n");
        exit(1);
    }

    if (!(username && password) && !config)
    {
        fprintf(stderr, "Please use -u -p to enter the username and the password\nOr use -c to specify a credential config file\n");
        exit(1);
    }
    else if (config)
        printf("Config file will be used regardless of the input credential\n");

    if (config)
        read_config(config, &username, &password);

    if (!strcmp(mode, "net"))
        net_login(username, password);
    else if (!strcmp(mode, "auth4"))
        auth4_login(username, password);
    else if (!strcmp(mode, "auth6"))
        auth6_login(username, password);
    else if (!strcmp(mode, "auth"))
    {
        auth4_login(username, password);
        auth6_login(username, password);
    }
    else
    {
        fprintf(stderr, "Please select one mode: [net auth auth4 auth6]\n");
        exit(1);
    }

    return 0;
}
