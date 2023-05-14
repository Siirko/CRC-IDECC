#include "../include/bitman.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

uint8_t crc8_register[256];

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

void crc8_init_register(void)
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
        crc8_register[i] = crc;
    }
}

uint8_t crc8_fast(uint8_t const message[], int nBytes)
{
    uint8_t crc = 0;
    int byte;

    for (byte = 0; byte < nBytes; ++byte)
        crc = crc8_register[crc ^ message[byte]];
    return crc;
}

uint8_t crc8_slow(uint8_t const message[], int nBytes)
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

packet_t crc8_encode_packet(uint8_t const message[], int nBytes)
{
    packet_t packet = {0};
    uint8_t crc = crc8_fast(message, nBytes);
    packet.size = nBytes;
    memcpy(packet.data, message, nBytes);
    packet.crc = crc;
    return packet;
}

void noisify_packet(packet_t *packet, int quantity)
{
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
}

void denoisify_packet(packet_t *packet)
{
    uint8_t crc_error = crc8_fast(packet->data, packet->size);
    if (crc_error != packet->crc)
    {
        // uint8_t crc_xored = crc_error ^ packet->crc;
        uint8_t alternated_data[packet->size];
        for (int i = 0; i < packet->size; i++)
        {
            uint8_t byte = packet->data[i];
            for (int j = 0; j < 8; j++)
            {
                byte = change_nth_bit(j, byte);
                memcpy(alternated_data, packet->data, packet->size);
                alternated_data[i] = byte;
                if (crc8_verify_bytes(alternated_data, packet->size,
                                      packet->crc) == 0)
                {
                    packet->data[i] = byte;
                    return;
                }
                byte = change_nth_bit(j, byte);
            }
        }
    }
}

void noisify(uint16_t *packet, int quantity)
{
    // m = message (8bits) | crc (8bits)
    // only alternate message bits
    uint8_t mess = *packet >> 8;
    int indexes[8] = {0};
    for (int i = 0; i < quantity; i++)
    {
        int bit = rand() % 8;
        while (indexes[bit] == 1)
            bit = rand() % 8;
        indexes[bit] = 1;
        mess = change_nth_bit(bit, mess);
    }
    *packet = crc8_encode(mess, *packet);
}

void denoisify(uint16_t *packet)
{
    // packet = message (8bits) | crc (8bits)
    uint8_t mess = *packet >> 8;
    uint8_t crc = *packet;
    if (crc8_verify(*packet) != 0)
    {
        uint8_t crc_calc = crc8_fast(&mess, 1);
        uint8_t xor = crc_calc ^ crc;
        for (int i = 0; i < 256; i++)
        {
            if (crc8_register[i] == xor)
            {
                mess ^= i;
                break;
            }
        }
        *packet = crc8_encode(mess, crc);
    }
    else
    {
        printf("no error\n");
    }
}

int crc8_verify(uint16_t packet)
{
    uint8_t const message[2] = {packet >> 8, packet & 0xFF};
    return crc8_fast(message, 2);
}

int crc8_verify_bytes(uint8_t const message[], int nBytes, uint8_t crc)
{
    uint8_t message_crc[nBytes + 1];
    memcpy(message_crc, message, nBytes);
    message_crc[nBytes] = crc;
    return crc8_fast(message_crc, nBytes + 1);
}

int crc8_verify_packet(packet_t packet)
{
    uint8_t message[packet.size + 1];
    memcpy(message, packet.data, packet.size);
    message[packet.size] = packet.crc;
    return crc8_fast(message, packet.size + 1);
}

int crc8_hamming_distance(void)
{
    static int min = __INT_MAX__;
    if (min != __INT_MAX__)
        return min;
    for (int i = 1; i < 256; i++)
    {
        uint16_t res = crc8_encode(i, crc8_register[i]);
        int count = __builtin_popcount(res);
        if (count < min)
            min = count;
    }
    return min;
}

uint16_t crc8_encode(uint8_t message, uint8_t crc)
{
    return message << 8 | crc;
}