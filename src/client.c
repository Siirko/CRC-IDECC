#include "../include/client.h"
#include "../include/bitman.h"
#include "../include/utilities.h"

typedef struct handler
{
    int sockfd;
    uint16_t *packet;
    pthread_barrier_t *tempo;
} handler_t;

void *handle_receiving(void *arg)
{
    handler_t *handler = (handler_t *)arg;
    int sockfd = handler->sockfd;
    pthread_barrier_t *tempo = handler->tempo;
    uint16_t packet_received = 0;
    for (;;)
    {
        CHK(write(STDIN_FILENO, "> ", 2));
        TCHK(pthread_barrier_wait(tempo));
        uint16_t tmp = *handler->packet;
        TCHK(pthread_barrier_wait(tempo));
        ssize_t nbytes;
        CHK(nbytes =
                recv(sockfd, &packet_received, sizeof(packet_received), 0));
        if (nbytes == 0)
        {
            CHK(printf("Server disconnected\n"));
            exit(EXIT_SUCCESS);
        }
        else if (ERROR_CODE == packet_received)
        {
        resend:
            CHK(printf("\rServer couldn't fix packet, resending ...\n"));
            CHK(fflush(stdout));
            CHK(send(sockfd, &tmp, sizeof(tmp), 0));
            if (recv(sockfd, &packet_received, sizeof(packet_received), 0) == 0)
            {
                CHK(printf("Server disconnected\n"));
                exit(EXIT_SUCCESS);
            }
            else if (ERROR_CODE == packet_received)
                goto resend;
        }
        CHK(printf("\rReceived: %c\n", packet_received >> 8));
        CHK(fflush(stdout));
    }
    return NULL;
}

void start_client(char *addr_proxy, char *port_proxy)
{
    struct addrinfo *res = NULL;
    struct addrinfo hints = {0};
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    TCHK(getaddrinfo(addr_proxy, port_proxy, &hints, &res));
    CHKNUL(res);

    int sockfd;
    CHK(sockfd = socket(res->ai_family, hints.ai_socktype, 0));
    CHK(connect(sockfd, res->ai_addr, res->ai_addrlen));
    freeaddrinfo(res);

    uint16_t packet = 0;
    pthread_t handler_id;
    pthread_barrier_t tempo = {0};
    TCHK(pthread_barrier_init(&tempo, NULL, 2));
    handler_t handler = {sockfd, &packet, &tempo};
    CHK(pthread_create(&handler_id, NULL, handle_receiving, &handler));
    CHK(printf("Connected to %s:%s\n", addr_proxy, port_proxy));
    CHK(printf("Type a character to send it to the server\n"));
    for (;;)
    {
        char buf[1] = {0};
        CHK(scanf(" %c", buf));
        uint8_t const message[1] = {buf[0]};
        packet = crc8_encode(message[0], crc8_fast(message, 1));
        CHK(send(sockfd, &packet, sizeof(packet), 0));
        // Barrière pour attendre que handler_receiving ait le temps de
        // faire une copie du paquet envoyé
        TCHK(pthread_barrier_wait(&tempo));
        TCHK(pthread_barrier_wait(&tempo));
    }
}