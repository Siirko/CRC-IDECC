#include "../include/bitman.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

uint8_t crc_8_lookup[256];

uint16_t set_nth_bit(int n, uint16_t m) { return m | (1 << n); }

uint16_t get_nth_bit(int n, uint16_t m) { return (m & (1 << n)) >> n; }

uint16_t change_nth_bit(int n, uint16_t m) { return m ^ (1 << n); }

void print_word(int k, uint16_t m)
{
    printf("0b");
    for (int i = 1; i <= k; i++)
        printf("%i", get_nth_bit(sizeof(m) * 8 - i, m));
    printf("\n");
}

void crc_8_init_table(void)
{
    uint8_t crc;
    uint8_t c;
    int i, j;

    for (i = 0; i < 256; i++)
    {
        crc = i;
        for (j = 0; j < 8; j++)
        {
            c = crc & TOPBIT;
            crc <<= 1;
            if (c)
                crc ^= POLYNOME;
        }
        crc_8_lookup[i] = crc;
    }
}

uint8_t crc_8_tablelookup(uint8_t const message[], int nBytes)
{
    uint8_t crc = 0;
    int byte;

    for (byte = 0; byte < nBytes; ++byte)
        crc = crc_8_lookup[crc ^ message[byte]];
    return crc;
}

uint8_t crc_8(uint8_t const message[], int nBytes)
{
    uint8_t reste = 0;

    // divison dans F_2[X]
    for (int byte = 0; byte < nBytes; ++byte)
    {
        reste ^= (message[byte] << (LEN - 8));

        for (uint8_t bit = 8; bit > 0; --bit)
        {
            if (reste & TOPBIT)
                reste = (reste << 1) ^ POLYNOME;
            else
                reste = (reste << 1);
        }
    }
    return reste;
}

bool crc_8_check(uint16_t message)
{
    uint8_t crc = message & 0xFF;
    uint8_t const message_8[1] = {message >> 8};
    return crc_8(message_8, 1) == crc;
}

int crc_8_hamming_distance(void)
{
    int min = __INT_MAX__;
    for (int i = 1; i < 256; i++)
    {
        uint16_t res = i << 8 | crc_8_lookup[i];
        int count = __builtin_popcount(res);
        if (count < min)
            min = count;
    }
    return min;
}

uint16_t concat(int size, uint8_t const message[size], uint8_t crc)
{
    uint16_t to_send = 0;
    memcpy(&to_send, message, size);
    to_send <<= 8;
    to_send |= crc;
    return to_send;
}