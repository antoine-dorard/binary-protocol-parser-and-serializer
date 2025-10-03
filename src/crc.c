#include "crc.h"

#define GENERATOR_POLY 0x1021u

/* compute CRC-16/XMODEM for `data` of length `len` bytes */
uint16_t crc16_xmodem(const uint8_t *data, size_t len)
{
    uint16_t crc = 0x0000; // XMODEM initial value
    for (size_t i = 0; i < len; ++i) {
        crc ^= (uint16_t)data[i] << 8;     // bring next byte into MSB of 16-bit reg
        for (int b = 0; b < 8; ++b) {
            if (crc & 0x8000u) crc = (crc << 1) ^ GENERATOR_POLY;
            else               crc <<= 1;
        }
    }
    return crc & 0xFFFFu;
}

/* verifies that frame_buffer (len bytes) contains data+CRC (CRC is last two bytes, high-byte first)
   returns 1 if valid (remainder 0), 0 otherwise */
int crc16_xmodem_verify(const uint8_t *frame_buffer, size_t len)
{
    // compute CRC across the entire frame (including the 2 CRC bytes)
    uint16_t crc = crc16_xmodem(frame_buffer, len);
    return (crc == 0x0000u) ? 1 : 0;
}