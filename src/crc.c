#include "crc.h"
#include <stdint.h>
#include <stdio.h>

/*
head is of length 2. It will contain the buffer data for the first iter and will have the remander of each previous iters for the remaining iterations

*/
uint16_t accumulate(const uint8_t* head, const uint8_t* buffer, size_t n, int* shift_counter)
{
    uint16_t acc = 0;
    *shift_counter = 0;

    for (size_t byte = 0; byte < n; byte++){
        const uint8_t current_byte = byte < 2 ? head[byte] : buffer[byte];

        for (int bit = 7; bit >= 0; bit--){
            uint16_t next_bit = (current_byte >> bit) & 1u;
            acc = (acc << 1) | next_bit; 
            shift_counter++;
            
            if ((acc & 0x8000) == 0x8000){ // shift register has shifted to have the most significant bit to 1
                // perform the operation one more time since the 1 is 
                acc = (acc << 1) | next_bit; 
                shift_counter++;
                return acc;
            } 
        }
    }
    return acc;
}



uint16_t bs_crc16_ccitt(const uint8_t* data, size_t length)
{
    uint16_t gen = 0x1021;
    //uint16_t pad = 0x0; // 16 bits padding for the least significant end of the message.

    uint16_t remainder = 0;
    uint16_t shift_register = 0;
    int shift_counter = 0;

    //iter 1
    
    shift_register = accumulate(data, data, length, &shift_counter);
    printf("shift_register iter1: %04X\n", shift_register);
    remainder = shift_register ^ gen;

    //iter 1 -> n;
    while (remainder >= shift_register) {

        // copy the data to a read-write buffer


        // shift the buffer to have the left-most bit of the most significant byte be a 1
        shift_register = accumulate((uint8_t*)&remainder, data, length, &shift_counter);

        remainder = shift_register ^ gen;

        printf("Current remainder: %04X\n", remainder);
        printf("Current shift_register: %04X\n", shift_register);
    }

    return remainder;
}
