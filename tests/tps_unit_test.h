/**
* Small librairy to perform unit tests to C programs
* Author: Quentin Bramas <bramas@unistra.fr>
*
* usage:
* - create functions for each feature you
*   want to test, and call them in a main()
*   using the macro TEST(test_function)
* - in a function you can use tps_assert(cond)
*   to check the condition.
* - you can change the value of the global var
*   __remaining_alloc in order to make allocations
*   fail when you want.
*   - if __remaining_alloc is 0, all the remaining allocations
*     will fail (malloc, realloc and calloc).
*   - if __remaining_alloc is < 0 (default),
*     all the remaining allocations will work as expected.
*   - otherwise the next __remaining_alloc will work
*     as expected and then remaining allocations will fail
* - global variable __remaining_alloc should be declared
* Example:

#include "tps_unit_test.h"

int __remaining_alloc;

void test1()
{
  __remaining_alloc = 0;
  int * a = malloc(sizeof(int));
  tps_assert(a == NULL);
}
void test2()
{
  __remaining_alloc = 1;
  int * a = malloc(sizeof(int));
  int * b = malloc(sizeof(int));
  tps_assert(a != NULL);
  tps_assert(b == NULL);
}

int main(void)
{
  TEST(test1);
  TEST(test2);
}

*/

#include <stdio.h>
#include <stdlib.h>

#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_WHITE "\x1b[97m"

#define ANSI_BOLD "\x1b[1m"
#define ANSI_UNDERLINE "\x1b[4m"
#define ANSI_RESET "\x1b[0m"

#ifndef TPS_UNIT_TEST_H
#define TPS_UNIT_TEST_H

extern int __remaining_alloc;

#define calloc(a, b)                                                           \
    (__remaining_alloc ? __remaining_alloc--, calloc(a, b) : NULL)
#define malloc(a) (__remaining_alloc ? __remaining_alloc--, malloc(a) : NULL)
#define realloc(a, b)                                                          \
    (__remaining_alloc ? __remaining_alloc--, realloc(a, b) : NULL)

char *__current_test_name;

#define tps_test_error(msg) printf("Error %s [ FAILED ]\n", msg)

#define tps_assert(expr)                                                       \
    do                                                                         \
    {                                                                          \
        if (!(expr))                                                           \
        {                                                                      \
            puts(__current_test_name);                                         \
            tps_test_error(ANSI_COLOR_MAGENTA                                  \
                           "\nassertion failed: " #expr ANSI_RESET);           \
            exit(1);                                                           \
            return;                                                            \
        }                                                                      \
    } while (0)

#define TEST(func)                                                             \
    do                                                                         \
    {                                                                          \
        __current_test_name = #func;                                           \
        func();                                                                \
        fprintf(stdout, ANSI_BOLD ANSI_COLOR_YELLOW #func "\n");               \
        puts(ANSI_COLOR_GREEN                                                  \
             "Done\n----------------------------" ANSI_RESET);                 \
    } while (0)

#endif