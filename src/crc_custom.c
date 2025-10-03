#include "crc_custom.h"
#include <stdint.h>
#include <stdio.h>
#include <assert.h>

#define GENERATOR_POLY 0x1021 // x^16 + x^12 + x^5 + 1

/*
Extract a subset of bits from a byte buffer.
params:
    buffer: pointer to the data buffer
    n: size of the data buffer in bytes
    start: starting bit index (0-based)
    end: ending bit index (exclusive)
    shift_counter: pointer to an integer to count the number of bits processed

What happens if the requested subset is larger than 16 bits? Maybe we should simply return an int? but what if the subset is even larger than an int? How about long? Until when could this go?
*/
uint16_t bit_subset(const uint8_t* buffer, size_t n, size_t start, size_t end, int* shift_counter)
{
    assert(end - start <= 16); // TODO Handle this case
    assert(end > start); // The smaller subset possible is one bit, i.e. `end = start+1`

    size_t start_byte = (size_t) start / 8;
    size_t end_byte = (size_t) end / 8;

    size_t start_bit = start % 8;
    size_t end_bit = end % 8;

    uint16_t subset = 0;

    size_t byte = start_byte;
    while (byte <= end_byte){

        size_t bit = 7;
        size_t bit_bound = 0;
        if (byte == start_byte && byte == end_byte) {
            bit = 7 - start_bit;
            bit_bound = 7 - end_bit + 1;
        }
        else if (byte == start_byte) bit = 7 - start_bit;
        else if (byte == end_byte) bit_bound = 7 - end_bit + 1;

        uint8_t current_byte = 0x0;

        if (byte >= n) {
            current_byte = 0x0;
        } else {
            current_byte = buffer[byte];
        }

        while (bit >= bit_bound){
            uint16_t next_bit = (current_byte >> bit) & 1u;
            subset = (subset << 1) | next_bit; 
            (*shift_counter)++;

            if (bit > 0) bit--;
            else break;
        }


        byte++;
    }
    
    return subset;
}

/*
*/
uint16_t accumulate(const uint8_t* buffer, size_t n, int* shift_counter)
{
    uint16_t acc = 0;

    for (size_t byte = 0; byte < n; byte++){
        uint8_t current_byte = buffer[byte];

        for (int bit = 7; bit >= 0; bit--){
            uint16_t next_bit = (current_byte >> bit) & 1u;
            acc = (acc << 1) | next_bit; 
            (*shift_counter)++;
            
            if ((acc & 0x8000) == 0x8000){ // shift register has shifted to have the most significant bit to 1
                // perform the operation one more time since the 1 is 
                if (bit == 0) {
                    bit = 7;
                    byte++;
                    if (byte >= n) {
                        current_byte = 0x0;
                    } else {
                        current_byte = buffer[byte];
                    }
                } else {
                    bit--;
                }
                uint16_t next_bit = (current_byte >> bit) & 1u;
                acc = (acc << 1) | next_bit; 
                (*shift_counter)++;
                return acc;
            } 
        }
    }
    return acc;
}


uint16_t compute_remainder(const uint8_t* data, size_t length, uint16_t gen, int pad_crc)
{
    const int BITS_LENGTH = pad_crc ? length * 8 + 16 : length * 8; // length of the data in bits

    uint16_t remainder = 0;
    uint16_t shift_register = 0;
    int shift_counter = 0;

    //iter 1
    
    shift_register = accumulate(data, length, &shift_counter);
    remainder = shift_register ^ gen;


    //uint16_t previous_remainder = remainder;
    //iter 1 -> n;
    //while (remainder >= previous_remainder) {
    printf("remainder iter1: %04X\n", remainder);
    printf("gen condition: %04X\n", (gen | 0x10000));
    int _loop_counter = 0;

    _loop_counter++; // to take into account the first iteration above

    // >= 16 is the order of the generator polynomial
    const int MAX_SHIFT = BITS_LENGTH - (shift_counter - 16); // max bits that can be shifted in the shift register (excluding the 2 bits already shifted in the first iteration)
    printf("MAX_SHIFT: %d\n", MAX_SHIFT);
    while (MAX_SHIFT - (shift_counter - 16) >= 16) {
    //while (1) {
        _loop_counter++;
        shift_register = remainder;
       // previous_remainder = remainder;

        // shift remainder
        int remainder_shift_counter = 0;
        
        while ((shift_register & 0x8000) != 0x8000){
            //printf("Shifting remainder left: %04X\n", shift_register);
            shift_register = shift_register << 1;
            remainder_shift_counter++;
        }
        shift_register = shift_register << 1; // remove the extra bit
        remainder_shift_counter++;

        // add the remaining bits from the buffer to fill in the 16 bits
        shift_register = shift_register | bit_subset(data, length, shift_counter, shift_counter + remainder_shift_counter, &shift_counter);

        // perform the division step
        remainder = shift_register ^ gen;


        printf("%d. Current remainder: %04X\n", _loop_counter, remainder); 
        printf("%d. %04X\n", _loop_counter, (gen | 0x10000));

        //printf("%d. Current shift_register: %04X\n", _loop_counter, shift_register);
        printf("\n");
        if (remainder == 0) break; // early exit if remainder is zero
    }

    return remainder;
}

uint16_t bs_crc16_ccitt(const uint8_t* data, size_t length)
{
    return compute_remainder(data, length, GENERATOR_POLY, 1);
}

int bs_verify_crc(const uint8_t* frame_buffer, size_t frame_len)
{
    //uint16_t gen = 0x1021;
    assert(frame_buffer != NULL);
    assert(frame_len >= 2);

    uint16_t remainder = compute_remainder(frame_buffer, frame_len, GENERATOR_POLY, 0);
    printf("Calculated remainder: %04X\n", remainder);

    return remainder == 0;
}