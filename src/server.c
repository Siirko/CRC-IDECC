#include "../include/server.h"
#include "../include/bitman.h"
#include "../include/utilities.h"

int init_server(char *addr, char *port)
{
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
    CHK(listen(sockfd, 1));
    freeaddrinfo(res);
    return sockfd;
}

void start_server(char *addr, char *port)
{
    srand(time(NULL));
    int sockfd = init_server(addr, port);
    // wait a connection
    struct sockaddr_storage proxy_addr = {0};
    socklen_t proxy_addr_len = sizeof(proxy_addr);
    int proxyfd;
    CHK(printf("Waiting proxy connection...\n"));
    CHK(proxyfd =
            accept(sockfd, (struct sockaddr *)&proxy_addr, &proxy_addr_len));
    CHK(printf("Proxy connected\n"));
    for (;;)
    {
        // receive a message
        uint16_t packet = 0;
        ssize_t nbytes = 0;
        CHK(nbytes = recv(proxyfd, &packet, sizeof(uint16_t), 0));
        if (nbytes == 0)
        {
            CHK(fprintf(stderr, "Proxy disconnected\n"));
            goto end;
        }
        CHK(printf("Received: %c\n", packet >> 8));
        if (crc8_verify(packet) == 0)
            CHK(printf("Packet is correct\n\n"));
        else
        {
            CHK(printf("Packet is corrupted\nTrying to fix packet ...\n"));
            denoisify(&packet);
            // rand() % 100 < 90 => 90% de chance de corriger le paquet
            // crc8_verify(packet) == 0 && (rand() % 100 < 90)
            if (crc8_verify(packet) == 0)
                CHK(printf("Packet fixed\n\n"));
            else
            {
                CHK(printf("Couldn't fix packet\n\n"));
                packet = ERROR_CODE;
            }
        }
        // send the message back
        CHK(send(proxyfd, &packet, sizeof(uint16_t), 0));
    }

end:
    CHK(close(proxyfd));
    CHK(close(sockfd));
}