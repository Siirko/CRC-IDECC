#pragma once
#include <arpa/inet.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <netdb.h>
#include <pthread.h>
#include <signal.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

typedef struct client client_t;
typedef struct client
{
    int fd;
    pthread_t id;
    int serverfd;
} client_t;

/**
 * @brief Function used in thread that handle receiving packets
 * @param arg client_t
 */
void *handle_receiving(void *arg);
/**
 * @brief Entry point of the client
 * @param addr_proxy address of the proxy
 * @param port_proxy port of the proxy
 */
void start_client(char *addr_proxy, char *port_proxy);