#pragma once

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>

#define BUFLEN 4096

#define TCHK(op)                                                               \
    do                                                                         \
    {                                                                          \
        if ((errno = (op)) > 0)                                                \
            raler(1, #op);                                                     \
    } while (0)

#define CHK(op)                                                                \
    do                                                                         \
    {                                                                          \
        if ((op) < 0)                                                          \
            raler(1, #op);                                                     \
    } while (0)

#define CHKNUL(op)                                                             \
    do                                                                         \
    {                                                                          \
        if ((op) == NULL)                                                      \
            raler(1, #op);                                                     \
    } while (0)

static noreturn void raler(int syserr, const char *msg, ...)
{
    va_list ap;

    va_start(ap, msg);
    vfprintf(stderr, msg, ap);
    fprintf(stderr, "\n");
    va_end(ap);

    if (syserr == 1)
        perror("");

    exit(EXIT_FAILURE);
}
