#pragma once
#include <stdbool.h>
#include <stdint.h>

#define POLYNOME 0b10111001 //   x^7 + x^5 + x^4 + x^3 + x^1 + x^0
#define LEN (8 * sizeof(uint8_t))
#define TOPBIT (1 << (LEN - 1))
#define CEIL(x, y) (((x) + (y)-1)) / (y)

#define ERROR_CODE 0xFFFF
extern uint8_t crc8_register[256];

typedef struct
{
    int size; // 1 <= size <= 8
    uint8_t data[8];
    uint8_t crc;
} packet_t;

/**
 * @brief Set the nth bit of m to 1
 */
uint16_t set_nth_bit(int n, uint16_t m);
/**
 * @brief Set the nth bit of m to 0
 */
uint16_t get_nth_bit(int n, uint16_t m);
/**
 * @brief Alter the nth bit of m
 */
uint16_t change_nth_bit(int n, uint16_t m);
/**
 * @brief Print the binary representation of m
 */
void print_word(int k, uint16_t m);

/**
 * @brief Put quantity errors in the packet (first 8bits)
 */
void noisify(uint16_t *packet, int quantity);
/**
 * @brief Put quantity errors in the packet
 */
void noisify_packet(packet_t *packet, int quantity);
/**
 * @brief try to correct the packet
 */
void denoisify(uint16_t *packet);
/**
 * @brief try to correct the packet
 */
void denoisify_packet(packet_t *packet);

/**
 * @brief Compute the hamming distance for the current polynome
 */
int crc8_hamming_distance(void);
/**
 * @brief Initiate crc8_register
 */
void crc8_init_register(void);
/**
 * @brief Compute the crc8 of a message using the register
 */
uint8_t crc8_fast(uint8_t const message[], int nBytes);
/**
 * @brief Compute the crc8 of a message doing division in F_2[X]
 */
uint8_t crc8_slow(uint8_t const message[], int nBytes);
/**
 * @brief Encode a messag and his crc8 in a packet
 * @note message | crc8(message)
 */
uint16_t crc8_encode(uint8_t message, uint8_t crc);
packet_t crc8_encode_packet(uint8_t const message[], int nBytes);
/**
 * @brief Verify the crc8 of a message
 * @return 0 if the crc8 is correct, other if not
 */
int crc8_verify(uint16_t message);
/**
 * @brief Verify the crc8 of an array of bytes
 * @return 0 if the crc8 is correct, other if not
 */
int crc8_verify_bytes(uint8_t const message[], int nBytes, uint8_t crc);
int crc8_verify_packet(packet_t packet);
