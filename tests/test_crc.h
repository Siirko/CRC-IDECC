#pragma once
#include "../include/bitman.h"
#include "tps_unit_test.h"
#include <stdbool.h>
#include <stdio.h>
#include <time.h>

void test_crc_8()
{
    uint8_t const message[] = "a";
    uint8_t crc = crc_8_slow(message, 1);
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
    uint8_t crc = crc_8_slow(message, 9);
    uint8_t crc_table = crc_8_fast(message, 9);
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
    uint16_t to_send = concat(1, message, crc_8_slow(message, 1));
    tps_assert(crc_8_check(to_send));
    print_word(16, to_send);
    create_error(&to_send, crc_8_hamming_distance());
    print_word(16, to_send);
    tps_assert(!crc_8_check(to_send));
}

void test_crc_8_correct()
{
    srand(time(NULL));
    crc_8_init_table();
    uint8_t const message[] = "a";
    uint16_t to_send = concat(1, message, crc_8_slow(message, 1));
    tps_assert(crc_8_check(to_send));
    printf("mess: \t\t");
    print_word(8, to_send);
    create_error(&to_send, crc_8_hamming_distance() - 1);
    printf("mess_error: \t");
    print_word(8, to_send);
    tps_assert(!crc_8_check(to_send));
    correct_error(&to_send);
    printf("mess_nerror: \t");
    print_word(8, to_send);
    tps_assert(to_send >> 8 == 'a');
    printf("message: %c\n", to_send >> 8);
}

void test_crc_8_encoding()
{
    uint8_t const message[] = "1234567";
    packet_t packet = crc_8_encode(message, 7);
    printf("message: %s\n", packet.data);
    printf("crc: 0x%x\n", packet.crc);
    tps_assert(packet.crc == 0x9F);
    tps_assert(crc_8_check_packet(packet));
    tps_assert(packet.data[0] == '1');
    tps_assert(packet.data[1] == '2');
    tps_assert(packet.data[2] == '3');
    tps_assert(packet.data[3] == '4');
    tps_assert(packet.data[4] == '5');
    tps_assert(packet.data[5] == '6');
    tps_assert(packet.data[6] == '7');
    tps_assert(packet.size == 7);
}

void test_crc_8_packet_error()
{
    srand(time(NULL));
    int len = 4;
    uint8_t const message[] = "1234";
    packet_t packet = crc_8_encode(message, len);
    tps_assert(crc_8_check_packet(packet));
    tps_assert(packet.crc == 0x1A);
    tps_assert(packet.data[0] == '1');
    tps_assert(packet.data[1] == '2');
    tps_assert(packet.data[2] == '3');
    tps_assert(packet.data[3] == '4');
    tps_assert(packet.size == len);
    packet_t packet_error = packet;
    create_packet_error(&packet_error, 1);
    for (int i = 0; i < len; i++)
    {
        printf("%d.\n", i);
        print_word(16, packet.data[i]);
        print_word(16, packet_error.data[i]);
        printf("-----------------\n");
    }
    tps_assert(!crc_8_check_packet(packet_error));
    packet_t corrected = packet_error;
    correct_packet_error(&corrected);
    printf("packet: %s\n", packet.data);
    printf("packet_error: %s\n", packet_error.data);
    printf("packet_corrected: %s\n", corrected.data);
}
void unit_test_crc()
{
    TEST(test_crc_8_hamming);
    TEST(test_crc_8);
    TEST(test_crc_8_table);
    TEST(test_crc_8_check);
    TEST(test_crc_8_correct);
    TEST(test_crc_8_encoding);
    TEST(test_crc_8_packet_error);
}