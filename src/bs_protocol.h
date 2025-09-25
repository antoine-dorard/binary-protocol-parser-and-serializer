#include <stdint.h>
#include <stddef.h>

typedef struct {
    uint8_t  sync[2];        // Sync pattern
    uint8_t  flags;          // Control flags
    uint16_t length;         // Total frame length
    uint8_t  sequence;       // Sequence number
    uint8_t  type;           // Message type
    uint8_t* payload;        // Pointer to payload data
    size_t   payload_len;    // Payload length
    uint16_t crc;            // CRC-16 checksum
} bs_frame_t;

typedef struct {
    uint8_t* buffer;         // Underlying storage
    size_t   size;           // Total buffer size
    size_t   head;           // Write position
    size_t   tail;           // Read position
    size_t   count;          // Bytes currently in buffer
} bs_ringbuf_t;

// Error codes
#define BS_SUCCESS           0
#define BS_ERR_INVALID_ARG  -1
#define BS_ERR_BUFFER_FULL  -2
#define BS_ERR_NO_SYNC      -3
#define BS_ERR_BAD_CRC      -4
#define BS_ERR_BAD_LENGTH   -5
#define BS_ERR_NO_MEMORY    -6