#include <stdint.h>
#include <stddef.h>

uint16_t bit_subset(const uint8_t* buffer, size_t n, size_t start, size_t end, int* shift_counter);

// Calculate CRC-16-CCITT over data buffer
uint16_t bs_crc16_ccitt(const uint8_t* data, size_t length);

// Verify CRC of a complete frame
// Returns: 0 if CRC valid, -1 if invalid
int bs_verify_crc(const uint8_t* frame_buffer, size_t frame_len);