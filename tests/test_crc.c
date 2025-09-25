#include "unity.h"
#include "crc.h"
#include "unity_internals.h"

void setUp(void) {
    // set stuff up here
}

void tearDown(void) {
    // clean stuff up here
}

void test_crc(){
}

// not needed when using generate_test_runner.rb
int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_crc);
    return UNITY_END();
}