#pragma once
#include <stdbool.h>
#include <stdint.h>

#define POLYNOME 0b10111001 //   x^8 + x^7 + x^5 + x^4 + x^3 + 0
#define LEN (8 * sizeof(uint8_t))
#define TOPBIT (1 << (LEN - 1))
#define CEIL(x, y) (((x) + (y)-1)) / (y)

#define ERROR_CODE 0x44
extern uint8_t crc8_register[256];

typedef struct
{
    int size; // 1 <= size <= 8
    uint8_t data[8];
    uint8_t crc;
} packet_t;

uint16_t set_nth_bit(int n, uint16_t m);
uint16_t get_nth_bit(int n, uint16_t m);
uint16_t change_nth_bit(int n, uint16_t m);
void print_word(int k, uint16_t m);

void noisify(uint16_t *packet, int quantity);
void noisifiy_packet(packet_t *packet, int quantity);
void denoisify(uint16_t *packet);
void denoisifiy_packet(packet_t *packet);

int crc8_hamming_distance(void);
void crc8_init_register(void);
uint8_t crc8_fast(uint8_t const message[], int nBytes);
uint8_t crc8_slow(uint8_t const message[], int nBytes);
uint16_t crc8_encode(uint8_t message, uint8_t crc);
packet_t crc8_encode_packet(uint8_t const message[], int nBytes);
bool crc8_verify(uint16_t message);
bool crc8_verify_packet(packet_t packet);
