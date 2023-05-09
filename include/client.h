#pragma once
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

typedef struct client client_t;
typedef struct client
{
    char *addr;
    int fd;
    pthread_t id;
    client_t *arr;
    int *arr_len;
    int *serverfd;
    pthread_mutex_t *server_mutex;
} client_t;

void *handle_receiving(void *arg);
void start_client(char *addr_proxy, char *port_proxy);