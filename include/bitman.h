#pragma once
#include <stdbool.h>
#include <stdint.h>

#define POLYNOME 0b10111001 //   x^8 + x^7 + x^5 + x^4 + x^3 + 0
#define LEN (8 * sizeof(uint8_t))
#define TOPBIT (1 << (LEN - 1))

extern uint8_t crc_8_lookup[256];

uint16_t set_nth_bit(int n, uint16_t m);
uint16_t get_nth_bit(int n, uint16_t m);
uint16_t change_nth_bit(int n, uint16_t m);
void print_word(int k, uint16_t m);

void crc_8_init_table(void);
uint8_t crc_8_tablelookup(uint8_t const message[], int nBytes);

int crc_8_hamming_distance(void);
uint8_t crc_8(uint8_t const message[], int nBytes);
bool crc_8_check(uint16_t message);
uint16_t concat(int size, uint8_t const message[size], uint8_t crc);