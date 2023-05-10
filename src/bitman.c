#include "bitman.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

uint8_t crc_8(uint8_t const message[], int nBytes)
{
    uint8_t reste = 0;

    // divison modulo 2 byte par byte
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
    uint8_t mess = message >> 8;
    uint8_t crc_calc = crc_8(&mess, 1);
    return crc == crc_calc;
}

uint16_t concat(int size, uint8_t const message[size], uint8_t crc)
{
    uint16_t to_send = 0;
    memcpy(&to_send, message, size);
    to_send <<= 8;
    to_send |= crc;
    return to_send;
}