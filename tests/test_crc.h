#pragma once
#include "../include/bitman.h"
#include "tps_unit_test.h"
#include <stdbool.h>
#include <stdio.h>

void test_crc_8()
{
    uint8_t const message[] = "a";
    uint8_t crc = crc_8(message, 1);
    printf("message: %s\n", message);
    printf("crc: 0x%x\n", crc);
    tps_assert(crc == 0x43);
    uint16_t to_send = concat(1, message, crc);
    tps_assert(to_send == 0b110000101000011); // contient le char 'a' et le crc
    tps_assert(to_send >> 8 == 'a');
    tps_assert(crc_8_check(to_send));
}

void test_crc_8_table()
{
    crc_8_init_table();
    uint8_t const message[] = "123456789";
    uint8_t crc = crc_8(message, 9);
    uint8_t crc_table = crc_8_tablelookup(message, 9);
    printf("message: %s\n", message);
    printf("crc_table: 0x%x\n", crc_table);
    printf("crc: 0x%x\n", crc);
    tps_assert(crc_table == crc);
}

void test_crc_8_hamming()
{
    crc_8_init_table();
    int hamming = crc_8_hamming_distance();
    tps_assert(hamming == 4);
    printf("hamming: %d\n", hamming);
}

void test_crc_8_check()
{
    srand(1);
    crc_8_init_table();
    uint8_t const message[] = "a";
    uint16_t to_send = concat(1, message, crc_8(message, 1));
    tps_assert(crc_8_check(to_send));
    print_word(16, to_send);
    create_error(&to_send, crc_8_hamming_distance());
    print_word(16, to_send);
    tps_assert(!crc_8_check(to_send));
}

void unit_test_crc()
{
    TEST(test_crc_8_hamming);
    TEST(test_crc_8);
    TEST(test_crc_8_table);
    TEST(test_crc_8_check);
}