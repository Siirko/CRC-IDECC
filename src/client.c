#include "../include/client.h"
#include "../include/utilities.h"
void *handle_receiving(void *arg)
{
    int sockfd = *(int *)arg;
    char buf[BUFLEN] = {0};
    for (;;)
    {
        int nbytes;
        CHK(nbytes = recv(sockfd, buf, BUFLEN, 0));
        if (nbytes == 0)
        {
            CHK(printf("Server disconnected\n"));
            // do something to unblock the main thread
            break;
        }
        buf[nbytes] = '\0';
        CHK(printf("%s", buf));
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
        char buf[BUFLEN] = {0};
        CHK(read(STDIN_FILENO, buf, BUFLEN));
        CHK(send(sockfd, buf, strlen(buf), 0));
        // clear from stdout the message sent
        CHK(printf("\033[1A\033[2K"));
    }
}