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

void test_crc(){
    // char text[] = "Hi!";
    // int length = sizeof(text);

    uint8_t data[5] = { 0x48, 0x69, 0x21, 0x00, 0x00 }; // Hi!
 
    //uint8_t* data = (uint8_t*)text;

    uint16_t expected_crc = 0x31FD;
    uint16_t calculated_crc = bs_crc16_ccitt(data, sizeof(data));
    printf("Calculated CRC: #%04X\n", calculated_crc);
    TEST_ASSERT(calculated_crc == expected_crc);
}

// not needed when using generate_test_runner.rb
int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_crc);
    return UNITY_END();
}