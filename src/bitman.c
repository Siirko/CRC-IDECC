#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include "bitman.h"

uint16_t set_nth_bit(int n, uint16_t m)
{
    return m | (1 << n);
}

uint16_t get_nth_bit(int n, uint16_t m)
{
    return (m & (1 << n)) >> n;
}

uint16_t change_nth_bit(int n, uint16_t m)
{
    return m ^ (1 << n);
}

void print_word(int k, uint16_t m)
{
    printf("0b");
    for (int i = 1; i <= k; i++)
        printf("%i", get_nth_bit(sizeof(m) * 8 - i, m));
    printf("\n");
}