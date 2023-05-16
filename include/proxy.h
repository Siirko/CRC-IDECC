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

#define MAX_CLIENTS 10

/**
 * @brief Function used in thread that handle client connection
 * @param arg client_t
 */
void *handle_client(void *arg);
/**
 * @brief Start server connection
 * @return int sockserver
 */
int start_server_connection(char *addr, char *port);
/**
 * @brief Entry point of the server
 * @param addr address of the proxy
 * @param port port of the proxy
 * @param addr_proxy address of the server to connect
 * @param port_proxy port of the server to connect
 */
void start_proxy(char *addr, char *port, char *server_addr, char *server_port);