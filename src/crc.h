#include <stdint.h>
#include <stddef.h>

// Calculate CRC-16-XMODEM over data buffer
uint16_t crc16_xmodem(const uint8_t* data, size_t length);

// Verify CRC of a complete frame
// Returns: 0 if CRC valid, -1 if invalid
int crc16_xmodem_verify(const uint8_t* frame_buffer, size_t frame_len);