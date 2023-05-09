#pragma once
#include <argp.h>
#include <stdbool.h>
#include <stdlib.h>

enum MODE
{
    SERVER,
    PROXY,
    CLIENT
};

const char *argp_program_version = "1.0";

/* Program documentation. */
static char doc[] = "";

/* The options we understand. */
static struct argp_option options[] = {
    {"server", 's', 0, 0, "Server mode", 0},
    {"proxy", 'p', 0, 0, "Proxy mode", 0},
    {"client", 'c', 0, '0', "Client mode", 0},
    {"proxy-address", 'a', "ADDRESS", 0, "Address", 0},
    {"proxy-port", 't', "PORT", 0, "Port", 0},
    {"server-address", 'A', "SERVER_ADDRESS", 0, "Server address", 0},
    {"server-port", 'P', "SERVER_PORT", 0, "Server port", 0},
    {0}};

/* Used by main to communicate with parse_opt. */
typedef struct arguments
{
    // char *args[2];                /* arg1 & arg2 */
    enum MODE mode;
    char *proxy_address;
    char *proxy_port;
    char *server_address;
    char *server_port;
} arguments_t;

/* Parse a single option. */
static error_t parse_opt(int key, char *arg, struct argp_state *state)
{
    /* Get the input argument from argp_parse, which we
       know is a pointer to our arguments structure. */
    arguments_t *arguments = (arguments_t *)state->input;
    switch (key)
    {
    case ARGP_KEY_END:
        break;
    case 's':
        arguments->mode = SERVER;
        break;
    case 'p':
        arguments->mode = PROXY;
        break;
    case 'c':
        arguments->mode = CLIENT;
        break;
    case 'a':
        arguments->proxy_address = arg;
        break;
    case 't':
        arguments->proxy_port = arg;
        break;
    case 'A':
        arguments->server_address = arg;
        break;
    case 'P':
        arguments->server_port = arg;
        break;
    default:
        return ARGP_ERR_UNKNOWN;
    }

    return 0;
}

/* Our argp parser. */
static struct argp argp = {options, parse_opt, NULL, doc, NULL, NULL, NULL};
/** @} */