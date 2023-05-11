#include "../include/args_parsing.h"
#include "../include/bitman.h"
#include "../include/client.h"
#include "../include/proxy.h"
#include "../include/server.h"
#include "../include/utilities.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

arguments_t init_args(int argc, char *argv[])
{
    arguments_t arguments = {0};
    argp_parse(&argp, argc, argv, 0, 0, &arguments);
    if (arguments.mode == SERVER)
    {
        if (arguments.server_address == NULL || arguments.server_port == NULL)
        {
            fprintf(stderr, "Server address and port must be specified\n");
            exit(EXIT_FAILURE);
        }
    }
    else if (arguments.mode == PROXY)
    {
        if (arguments.proxy_address == NULL || arguments.proxy_port == NULL ||
            arguments.server_address == NULL || arguments.server_port == NULL)
        {
            fprintf(stderr,
                    "Proxy and Server address, port and must be specified\n");
            exit(EXIT_FAILURE);
        }
    }
    else if (arguments.mode == CLIENT)
    {
        if (arguments.proxy_address == NULL || arguments.proxy_port == NULL)
        {
            fprintf(stderr, "Server address and port must be specified\n");
            exit(EXIT_FAILURE);
        }
    }
    return arguments;
}

int main(int argc, char *argv[])
{
    arguments_t arguments = init_args(argc, argv);
    crc_8_init_table();
    switch (arguments.mode)
    {
    case SERVER:
        start_server(arguments.server_address, arguments.server_port);
        break;
    case PROXY:
        start_proxy(arguments.proxy_address, arguments.proxy_port,
                    arguments.server_address, arguments.server_port);
        break;
    case CLIENT:
        start_client(arguments.proxy_address, arguments.proxy_port);
        break;
    default:
        break;
    }
    return 0;
}
