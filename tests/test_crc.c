#include "unity.h"
#include "crc.h"
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
    // char text[] = "Hi!";
    // int length = sizeof(text);

    uint8_t data[3] = { 0x48, 0x69, 0x21 }; // Hi!
 
    //uint8_t* data = (uint8_t*)text;

    uint16_t expected_crc = 0x31FD;
    uint16_t calculated_crc = bs_crc16_ccitt(data, sizeof(data));
    printf("Calculated CRC: #%04X\n", calculated_crc);
    TEST_ASSERT(calculated_crc == expected_crc);
}

// not needed when using generate_test_runner.rb
int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_bit_subset);
    RUN_TEST(test_crc);
    return UNITY_END();
}