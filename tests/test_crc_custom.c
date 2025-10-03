#include "unity.h"
#include "crc_custom.h"
#include "unity_internals.h"

#include <stdint.h>

void setUp(void) {
    // set stuff up here
}

void tearDown(void) {
    // clean stuff up here
}

void test_bit_subset(){
    uint8_t data[5] = { 0x48, 0x69, 0x21, 0x00, 0x00 }; // Hi!

    #define TEST_CASES 7

    size_t start[TEST_CASES] = { 18, 0, 0, 4, 4, 10, 11 };
    size_t end[TEST_CASES] = { 21, 8, 16, 10, 9, 11, 12 };

    uint16_t expected_subset[TEST_CASES] = { 0b100, 0b01001000, 0b0100100001101001, 0b100001, 0b10000, 0b1, 0b0 };

    int shift_counter = 0;

    for (size_t i = 0; i < TEST_CASES; i++){

        uint16_t calculated_subset = bit_subset(data, sizeof(data), start[i], end[i], &shift_counter);
        printf("Calculated subset: %02X\n", calculated_subset);
        printf("Expected subset:   %02X\n", expected_subset[i]);
        TEST_ASSERT(calculated_subset == expected_subset[i]);
    }
    return;
}

void test_crc(){
    // TEST 1 
    uint8_t data[3] = { 0x48, 0x69, 0x21 }; // Hi!
    uint16_t expected_crc = 0x31FD;
    uint16_t calculated_crc = bs_crc16_ccitt(data, sizeof(data));
    printf("Calculated CRC: #%04X\n", calculated_crc);
    TEST_ASSERT(calculated_crc == expected_crc);

    // TEST 2
    // uint8_t data2[11] = { 0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x2C, 0x20, 0x43, 0x52, 0x43, 0x21 }; // Hello, CRC!
    // uint16_t expected_crc2 = 0x1898;
    // uint16_t calculated_crc2 = bs_crc16_ccitt(data2, sizeof(data2));
    // printf("Calculated CRC: #%04X\n", calculated_crc2);
    // TEST_ASSERT(calculated_crc2 == expected_crc2);

    // TEST 3
    //uint8_t data3[6] = { 0x63, 0x6F, 0x75, 0x63, 0x6F, 0x75 }; // coucou
    // uint8_t data3[6] = { 0x75, 0x6F, 0x63, 0x75, 0x6F, 0x63 }; // uocuoc
    // uint16_t expected_crc3 = 0xE4F0;
    // uint16_t calculated_crc3 = bs_crc16_ccitt(data3, sizeof(data3));
    // printf("Calculated CRC: #%04X\n", calculated_crc3);
    // TEST_ASSERT(calculated_crc3 == expected_crc3);
}


void test_verify_crc_1(){
    uint8_t data_with_crc[5] = { 0x48, 0x69, 0x21, 0x31, 0xFD }; // Hi! + CRC

    int is_valid = bs_verify_crc(data_with_crc, sizeof(data_with_crc));
    TEST_ASSERT(is_valid == 1);

    uint8_t data_with_wrong_crc[5] = { 0x48, 0x69, 0x21, 0x00, 0x00 }; // Hi! + wrong CRC

    int is_valid_wrong = bs_verify_crc(data_with_wrong_crc, sizeof(data_with_wrong_crc));
    TEST_ASSERT(is_valid_wrong == 0);
}

void test_verify_crc_2(){

    uint8_t data[11] = { 0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x2C, 0x20, 0x43, 0x52, 0x43, 0x21 }; // Hello, CRC!
    uint16_t expected_crc = bs_crc16_ccitt(data, sizeof(data));

    uint8_t data_with_crc[13] = { 0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x2C, 0x20, 0x43, 0x52, 0x43, 0x21, (uint8_t)(expected_crc >> 8), (uint8_t)(expected_crc & 0xFF) }; // Hello, CRC! + CRC
    //uint8_t data_with_crc[13] = { 0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x2C, 0x20, 0x43, 0x52, 0x43, 0x21, 0x18, 0x98 }; // Hello, CRC! + CRC

    int is_valid = bs_verify_crc(data_with_crc, sizeof(data_with_crc));
    TEST_ASSERT(is_valid == 1);

    uint8_t data_with_wrong_crc[13] = { 0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x2C, 0x20, 0x43, 0x52, 0x43, 0x21, 0x00, 0x00 }; // Hello, CRC! + wrong CRC

    int is_valid_wrong = bs_verify_crc(data_with_wrong_crc, sizeof(data_with_wrong_crc));
    TEST_ASSERT(is_valid_wrong == 0);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_bit_subset);
    RUN_TEST(test_crc);
    // RUN_TEST(test_verify_crc_1);
    // RUN_TEST(test_verify_crc_2);
    return UNITY_END();
}