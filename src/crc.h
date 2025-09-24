#include <stdint.h>
#include <stddef.h>


// Calculate CRC-16-CCITT over data buffer
uint16_t bs_crc16_ccitt(const uint8_t* data, size_t length);

// Verify CRC of a complete frame
// Returns: 0 if CRC valid, -1 if invalid
int bs_verify_crc(const uint8_t* frame_buffer, size_t frame_len);