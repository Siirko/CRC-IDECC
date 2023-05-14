#include "../include/server.h"
#include "../include/bitman.h"
#include "../include/utilities.h"
void start_server(char *addr, char *port)
{
    struct sockaddr_storage proxy_addr = {0};
    socklen_t proxy_addr_len = sizeof(proxy_addr);
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
    // wait a connection
    int proxyfd;
    CHK(printf("Waiting proxy connection...\n"));
    CHK(proxyfd =
            accept(sockfd, (struct sockaddr *)&proxy_addr, &proxy_addr_len));
    CHK(printf("Proxy connected\n"));
    for (;;)
    {
        // receive a message
        uint16_t packet = 0;
        int nbytes = 0;
        CHK(nbytes = recv(proxyfd, &packet, sizeof(uint16_t), 0));
        if (nbytes == 0)
        {
            CHK(fprintf(stderr, "Proxy disconnected\n"));
            goto end;
        }
        CHK(printf("Received: %c\n", packet >> 8));
        if (crc8_verify(packet) == 0)
            CHK(printf("Packet is correct\n"));
        else
        {
            CHK(printf("Packet is corrupted\n"));
            denoisify(&packet);
            if (crc8_verify(packet) == 0)
                CHK(printf("Packet fixed\n"));
            else
            {
                CHK(printf("Couldn't fix packet\n"));
                packet = ERROR_CODE;
            }
        }
        // send the message back
        CHK(send(proxyfd, &packet, sizeof(uint16_t), 0));
    }

end:
    CHK(close(proxyfd));
    CHK(close(sockfd));
    freeaddrinfo(res);
}