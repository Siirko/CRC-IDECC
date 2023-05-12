#include "../include/client.h"
#include "../include/bitman.h"
#include "../include/utilities.h"

void *handle_receiving(void *arg)
{
    int sockfd = *(int *)arg;
    uint16_t packet = 0;
    for (;;)
    {
        int nbytes;
        CHK(nbytes = recv(sockfd, &packet, sizeof(uint16_t), 0));
        if (nbytes == 0)
        {
            CHK(printf("Server disconnected\n"));
            exit(EXIT_SUCCESS);
        }
        if (ERROR_CODE == packet)
        {
            CHK(printf("Server couldn't fix packet, retry\n"));
            continue;
        }
        CHK(printf("Received: %c\n", packet >> 8));
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

    pthread_t handler_id;
    CHK(pthread_create(&handler_id, NULL, handle_receiving, (void *)&sockfd));
    for (;;)
    {
        char buf[1] = {0};
        CHK(scanf(" %c", buf));
        uint8_t const message[1] = {buf[0]};
        uint16_t packet = concat(1, message, crc_8_slow(message, 1));
        CHK(send(sockfd, &packet, sizeof(packet), 0));
    }
}