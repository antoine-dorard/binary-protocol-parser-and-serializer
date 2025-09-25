#include <stdint.h>
#include <sys/types.h>

#include "bs_protocol.h"

// Parse a raw byte buffer into a frame structure
// Returns: 0 on success, negative error code on failure
int bs_parse_frame(const uint8_t* buffer, size_t buffer_len, bs_frame_t* frame);

// Validate frame integrity (CRC, length, sync pattern)
// Returns: 0 if valid, negative error code if invalid
int bs_validate_frame(const uint8_t* buffer, size_t buffer_len);

// Extract just the payload from a raw frame
// Returns: number of bytes extracted, negative on error
ssize_t bs_extract_payload(const uint8_t* buffer, size_t buffer_len,
                           uint8_t* payload, size_t payload_max);
