#include "../include/bitman.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

uint8_t crc_8_register[256];

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
        crc_8_register[i] = crc;
    }
}

uint8_t crc_8_fast(uint8_t const message[], int nBytes)
{
    uint8_t crc = 0;
    int byte;

    for (byte = 0; byte < nBytes; ++byte)
        crc = crc_8_register[crc ^ message[byte]];
    return crc;
}

uint8_t crc_8_slow(uint8_t const message[], int nBytes)
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

packet_t crc_8_encode(uint8_t const message[], int nBytes)
{
    packet_t packet = {0};
    uint8_t crc = crc_8_fast(message, nBytes);
    packet.size = nBytes;
    memcpy(packet.data, message, nBytes);
    packet.crc = crc;
    return packet;
}

void create_packet_error(packet_t *packet, int quantity)
{
    /*
    int indexes[packet->size]; // to avoid creating multiple errors in the same
                               // byte
    // create one error (one bit alternate) per byte
    for (int i = 0; i < quantity; i++)
    {
        int bit = rand() % 8;
        int byte = rand() % packet->size;
        while (indexes[byte] == 1)
            byte = rand() % packet->size;
        indexes[byte] = 1;
        packet->data[byte] = change_nth_bit(bit, packet->data[byte]);
    }
    */
    packet->data[packet->size - 1] =
        change_nth_bit(0, packet->data[packet->size - 1]);
}

void correct_packet_error(packet_t *packet)
{
    uint8_t crc_error = crc_8_fast(packet->data, packet->size);
    if (crc_error != packet->crc)
    {
        uint8_t crc_xored = crc_error ^ packet->crc;
        for (int i = 0; i < 256; i++)
        {
            int index = 3;
            if (crc_8_register[i] == crc_xored)
            {
                printf("i = %i\n", i);
                packet->data[index] ^= i;
                break;
            }
        }
    }
}

void create_error(uint16_t *packet, int quantity)
{
    // m = message (8bits) | crc (8bits)
    // only alternate message bits
    uint8_t mess = *packet >> 8;
    for (int i = 0; i < quantity; i++)
    {
        int bit = rand() % 8;
        mess = change_nth_bit(bit, mess);
    }
    *packet = concat(1, &mess, *packet & 0xFF);
}

void correct_error(uint16_t *packet)
{
    // m = message (8bits) | crc (8bits)
    uint8_t mess = *packet >> 8;
    uint8_t crc = *packet & 0xFF;
    uint8_t const message[1] = {mess};
    uint8_t crc_calc = crc_8_fast(message, 1);
    if (crc_calc != crc)
    {
        uint8_t xor = crc_calc ^ crc;
        for (int i = 0; i < 256; i++)
        {
            if (crc_8_register[i] == xor)
            {
                mess ^= i;
                break;
            }
        }
        *packet = concat(1, &mess, crc);
    }
}

bool crc_8_check(uint16_t packet)
{
    uint8_t const message[2] = {packet >> 8, packet & 0xFF};
    return crc_8_fast(message, 2) == 0;
}

bool crc_8_check_packet(packet_t packet)
{
    return crc_8_fast(packet.data, packet.size) == packet.crc;
}

int crc_8_hamming_distance(void)
{
    static int min = __INT_MAX__;
    if (min != __INT_MAX__)
        return min;
    for (int i = 1; i < 256; i++)
    {
        uint16_t res = i << 8 | crc_8_register[i];
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