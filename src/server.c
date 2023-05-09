#include "../include/server.h"
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
        char buf[BUFSIZ] = {0};
        int nbytes = 0;
        CHK(nbytes = recv(proxyfd, buf, BUFSIZ, 0));
        if (nbytes == 0)
        {
            CHK(fprintf(stderr, "Proxy disconnected\n"));
            goto end;
        }
        CHK(printf("Received: %s\n", buf));
        // Fix the packet
        /* ................ */
        // send the message back
        CHK(send(proxyfd, buf, nbytes, 0));
    }

end:
    CHK(close(proxyfd));
    CHK(close(sockfd));
    freeaddrinfo(res);
}