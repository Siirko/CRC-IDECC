#include "../include/proxy.h"
#include "../include/bitman.h"
#include "../include/client.h"
#include "../include/utilities.h"

volatile sig_atomic_t _sigint = 0;

void interrupt(int sig)
{
    switch (sig)
    {
    // Ctrl + C
    case SIGINT:
        _sigint = 1;
        break;
    default:
        break;
    }
}

void set_signal(int sig, void (*handler)(int))
{
    struct sigaction action;
    // Our handler
    action.sa_handler = handler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    // Register Ctrl + C handler
    sigaction(sig, &action, NULL);
}

void *handle_client(void *arg)
{
    client_t *client = (client_t *)arg;
    uint16_t packet = 0;
    for (;;)
    {
        int nbytes;
        CHK(nbytes = recv(client->fd, &packet, sizeof(uint16_t), 0));
        if (nbytes == 0)
        {
            CHK(fprintf(stderr, "Client %ld disconnected\n", client->id));
            break;
        }
        CHK(printf("Client %ld: %c\n", client->id, packet >> 8));
        // Provoque une erreur sur le contenu récupéré
        TCHK(pthread_mutex_lock(client->server_mutex));
        noisify(&packet, crc8_hamming_distance() - 1);
        TCHK(pthread_mutex_unlock(client->server_mutex));
        CHK(printf("Client %ld corrupted: %c\n", client->id, packet >> 8));
        // l'envoie au serveur
        TCHK(pthread_mutex_lock(client->server_mutex));
        CHK(send(*client->serverfd, &packet, sizeof(uint16_t), 0));
        CHK(nbytes = recv(*client->serverfd, &packet, sizeof(uint16_t), 0));
        if (nbytes == 0)
        {
            CHK(fprintf(stderr, "Server disconnected\n"));
            exit(EXIT_SUCCESS);
        }
        CHK(printf("Client %ld fixed: %c\n", client->id, packet >> 8));
        TCHK(pthread_mutex_unlock(client->server_mutex));
        // l'envoie au client
        CHK(send(client->fd, &packet, sizeof(uint16_t), 0));
    }
    return NULL;
}

int start_server_connection(char *addr, char *port)
{
    struct addrinfo *res = NULL;
    struct addrinfo hints = {0};
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    TCHK(getaddrinfo(addr, port, &hints, &res));
    CHKNUL(res);

    int sockfd;
    CHK(sockfd = socket(res->ai_family, hints.ai_socktype, 0));
    CHK(connect(sockfd, res->ai_addr, res->ai_addrlen));
    return sockfd;
}

void start_proxy(char *addr, char *port, char *server_addr, char *server_port)
{
    int serverfd = start_server_connection(server_addr, server_port);
    struct sockaddr_storage client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    struct addrinfo *res = NULL;
    struct addrinfo hints = {0};
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    TCHK(getaddrinfo(addr, port, &hints, &res));
    CHKNUL(res);
    CHK(printf("%s\n",
               inet_ntoa(((struct sockaddr_in *)res->ai_addr)->sin_addr)));
    int sockfd;
    CHK(sockfd = socket(res->ai_family, hints.ai_socktype, 0));
    // to restart server immediately after it is closed
    int optval = 1;
    CHK(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)));
    CHK(bind(sockfd, res->ai_addr, res->ai_addrlen));

    CHK(listen(sockfd, 10));
    // TODO:handle CTRL-C to exit cleanly
    set_signal(SIGINT, interrupt);
    int min = MIN_CLIENTS;
    client_t *clients = calloc(min, sizeof(client_t));
    pthread_mutex_t server_mutex = PTHREAD_MUTEX_INITIALIZER;

    for (int i = 0; _sigint == 0; ++i)
    {
        if (i >= min)
        {
            min *= 2;
            CHKNUL(clients = realloc(clients, min * sizeof(client_t)));
            CHK(printf("Max clients is now %d\n", min));
        }
        int clientfd;
        CHK(printf("Waiting for connection...\n"));
        CHK(clientfd = accept(sockfd, (struct sockaddr *)&client_addr,
                              &client_addr_len));
        CHK(printf("Connection from %s:%d\n",
                   inet_ntoa(((struct sockaddr_in *)&client_addr)->sin_addr),
                   ntohs(((struct sockaddr_in *)&client_addr)->sin_port)));
        clients[i] = (client_t){
            .fd = clientfd,
            .addr = inet_ntoa(((struct sockaddr_in *)&client_addr)->sin_addr),
            .id = i,
            .arr = clients,
            .arr_len = &min,
            .serverfd = &serverfd,
            .server_mutex = &server_mutex,
        };
        TCHK(pthread_create(&clients[i].id, NULL, handle_client, &clients[i]));
        TCHK(pthread_detach(clients[i].id));
    }

    freeaddrinfo(res);
    free(clients);
    CHK(close(serverfd));
    CHK(close(sockfd));
}