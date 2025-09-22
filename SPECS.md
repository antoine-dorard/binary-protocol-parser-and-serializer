# Project Specification: ByteStream Protocol Parser

## CS-EMB-101: Introduction to Low-Level C Programming
### Project Duration: 4 Weeks | Expected Effort: 32-48 hours

---

## Executive Summary

You will implement a **binary protocol parser and serializer** in pure C that handles structured message frames commonly used in embedded systems communication. This project introduces fundamental C concepts through a practical application that mirrors real-world embedded firmware development. You'll build a library capable of encoding/decoding messages with headers, payloads, and error detection (CRC), while establishing professional development practices that will serve you throughout your embedded systems journey.

The ByteStream protocol you'll implement is similar to protocols used in automotive CAN networks, satellite communications, and IoT devices—domains where every byte matters and reliability is critical.

---

## Learning Objectives

Upon successful completion of this project, you will:

1. **Master C memory management**: Understand stack vs. heap allocation, manual memory management, and buffer handling without memory leaks
2. **Develop pointer proficiency**: Navigate through memory using pointer arithmetic, understand array-pointer duality, and manipulate data at the byte level
3. **Implement bit-level operations**: Pack/unpack data efficiently using bitfields, masks, and shifts
4. **Handle endianness**: Convert between network and host byte order, understanding how multi-byte values are stored
5. **Build robust error handling**: Implement defensive programming techniques crucial for embedded systems
6. **Establish professional practices**: Set up a complete development environment with version control, automated testing, and continuous integration

---

## Project Overview

### The ByteStream Protocol

You will implement a parser for the following binary message format:

```
+--------+--------+--------+--------+--------+--------+...+--------+--------+
| SYNC   | SYNC   | FLAGS  | LENGTH | SEQ    | TYPE   |   PAYLOAD  | CRC16  |
| (0xAA) | (0x55) | (1B)   | (2B)   | (1B)   | (1B)   | (0-1024B)  | (2B)   |
+--------+--------+--------+--------+--------+--------+...+--------+--------+
```

**Field Descriptions:**

- **SYNC** (2 bytes): Synchronization pattern `0xAA55` marking frame start
- **FLAGS** (1 byte): Bit flags for frame properties
  - Bit 7: ACK_REQUIRED (1 = acknowledgment needed)
  - Bit 6: FRAGMENTED (1 = part of larger message)
  - Bit 5: ENCRYPTED (1 = payload encrypted - you'll just mark, not implement)
  - Bit 4: PRIORITY (1 = high priority)
  - Bits 3-0: VERSION (protocol version, currently 0x1)
- **LENGTH** (2 bytes): Total frame length including header and CRC (big-endian)
- **SEQ** (1 byte): Sequence number (0-255, wraps around)
- **TYPE** (1 byte): Message type identifier
- **PAYLOAD** (variable): Actual data (0-1024 bytes)
- **CRC16** (2 bytes): CRC-16-CCITT checksum over entire frame except CRC field itself (big-endian)

### What is Endianness?

Endianness refers to the order in which bytes are stored in memory for multi-byte values. In **big-endian** (network byte order), the most significant byte comes first—like writing numbers normally: 1234. In **little-endian** (common on x86/ARM), the least significant byte comes first—like 4321. When transmitting data between systems, we typically use big-endian to ensure consistency. Your parser must handle this conversion correctly.

---

## Detailed Requirements

### Core Components

#### 1. Frame Parser Module (`parser.h` / `parser.c`)

Implement the following functions:

```c
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
```

#### 2. Frame Builder Module (`builder.h` / `builder.c`)

```c
// Initialize a frame with given parameters
// Returns: 0 on success, negative error code on failure
int bs_init_frame(bs_frame_t* frame, uint8_t type, uint8_t seq,
                  uint8_t flags, const uint8_t* payload, size_t payload_len);

// Serialize frame to byte buffer
// Returns: number of bytes written, negative on error
ssize_t bs_serialize_frame(const bs_frame_t* frame, uint8_t* buffer,
                           size_t buffer_max);

// Build complete frame with automatic CRC calculation
// Returns: number of bytes written, negative on error
ssize_t bs_build_frame(uint8_t type, uint8_t seq, uint8_t flags,
                       const uint8_t* payload, size_t payload_len,
                       uint8_t* buffer, size_t buffer_max);
```

#### 3. CRC Module (`crc.h` / `crc.c`)

```c
// Calculate CRC-16-CCITT over data buffer
uint16_t bs_crc16_ccitt(const uint8_t* data, size_t length);

// Verify CRC of a complete frame
// Returns: 0 if CRC valid, -1 if invalid
int bs_verify_crc(const uint8_t* frame_buffer, size_t frame_len);
```

#### 4. Ring Buffer Module (`ringbuf.h` / `ringbuf.c`)

A **ring buffer** (or circular buffer) is a fixed-size buffer that wraps around when it reaches the end, treating the buffer as if it were connected end-to-end in a circle. This is crucial in embedded systems for handling continuous data streams without dynamic memory allocation. When the write pointer reaches the end, it wraps back to the beginning, overwriting old data if the buffer is full.

```c
// Initialize a ring buffer
void bs_ringbuf_init(bs_ringbuf_t* rb, uint8_t* buffer, size_t size);

// Write data to ring buffer
// Returns: number of bytes written (may be less than requested if full)
size_t bs_ringbuf_write(bs_ringbuf_t* rb, const uint8_t* data, size_t len);

// Read data from ring buffer
// Returns: number of bytes read
size_t bs_ringbuf_read(bs_ringbuf_t* rb, uint8_t* data, size_t len);

// Get number of bytes available to read
size_t bs_ringbuf_available(const bs_ringbuf_t* rb);

// Get free space in buffer
size_t bs_ringbuf_free(const bs_ringbuf_t* rb);
```

### Data Structure Definitions

```c
// In bs_protocol.h
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
```

---

## Implementation Milestones

### Week 1: Foundation and Environment Setup

**Goals:**
- Set up complete development environment
- Implement CRC calculation
- Create basic project structure

**Deliverables:**
1. GitHub repository with proper `.gitignore` for C projects
2. `Makefile` that compiles with strict warnings: `-Wall -Wextra -Werror -pedantic`
3. Working CRC-16-CCITT implementation with lookup table
4. Unit tests for CRC using a simple test framework
5. README with build instructions

**Key Concepts to Master:**
- Understanding hexadecimal notation and byte manipulation
- Using `uint8_t`, `uint16_t` from `<stdint.h>` for portable code
- Bitwise operations: AND (`&`), OR (`|`), XOR (`^`), shifts (`<<`, `>>`)
- Static vs. dynamic linking

### Week 2: Parser Implementation

**Goals:**
- Implement frame validation and parsing
- Handle endianness conversion correctly
- Practice pointer arithmetic

**Deliverables:**
1. Complete parser module with all specified functions
2. Endianness conversion utilities using `htons()`/`ntohs()` or manual implementation
3. Comprehensive error handling with meaningful return codes
4. Tests with valid and corrupted frames

**Key Concepts to Master:**
- Pointer arithmetic: navigating through buffers
- Type casting between `uint8_t*` and structured data
- Memory alignment considerations
- Defensive programming: boundary checks

### Week 3: Builder and Ring Buffer

**Goals:**
- Implement frame construction
- Create thread-safe ring buffer
- Understand circular buffer mathematics

**Deliverables:**
1. Complete builder module with serialization
2. Ring buffer implementation with wrap-around logic
3. Stream processing demo: continuously parse frames from ring buffer
4. Memory leak detection using Valgrind or AddressSanitizer

**What is Valgrind?**
Valgrind is a memory debugging tool that detects memory leaks, use of uninitialized memory, and other memory-related errors. It instruments your program at runtime to track every memory allocation and access. Running your program under Valgrind (`valgrind --leak-check=full ./your_program`) helps ensure your C code properly manages memory—critical for embedded systems where memory is limited.

**Key Concepts to Master:**
- Dynamic memory allocation with `malloc()`/`free()`
- Modulo arithmetic for circular indexing
- Memory ownership and lifetime management
- Buffer overflow prevention

### Week 4: Integration and Professional Practices

**Goals:**
- Set up continuous integration
- Create comprehensive test suite
- Document everything professionally

**Deliverables:**
1. GitHub Actions CI pipeline that:
   - Builds project on multiple compiler versions
   - Runs entire test suite
   - Checks code formatting (using `clang-format`)
   - Generates code coverage report
2. Stress tests: parse 10,000+ frames, verify no memory leaks
3. Performance benchmarks: measure parsing throughput
4. Complete API documentation using Doxygen comments
5. Design document explaining architecture decisions

**Key Concepts to Master:**
- Writing portable C code
- Creating reusable libraries
- Performance profiling basics
- Documentation as code

---

## Testing Requirements

### Unit Test Framework

Implement a minimal test framework (or use an existing one like Unity or MinUnit):

```c
// test_parser.c example
void test_parse_valid_frame(void) {
    uint8_t buffer[] = {
        0xAA, 0x55,                    // Sync
        0x01,                          // Flags (version 1)
        0x00, 0x0C,                    // Length (12 bytes)
        0x42,                          // Sequence
        0x10,                          // Type
        0x48, 0x65, 0x6C, 0x6C, 0x6F, // Payload "Hello"
        0xXX, 0xXX                     // CRC (calculate)
    };

    bs_frame_t frame;
    int result = bs_parse_frame(buffer, sizeof(buffer), &frame);

    ASSERT_EQ(result, BS_SUCCESS);
    ASSERT_EQ(frame.sequence, 0x42);
    ASSERT_EQ(frame.type, 0x10);
    ASSERT_EQ(frame.payload_len, 5);
    ASSERT_MEM_EQ(frame.payload, "Hello", 5);
}
```

### Test Coverage Requirements

Your test suite must include:

1. **CRC Tests** (minimum 10 tests)
   - Known test vectors from CRC-16-CCITT specification
   - Empty data, single byte, maximum size data
   - Verify detection of single-bit errors

2. **Parser Tests** (minimum 15 tests)
   - Valid frames of various sizes
   - Corrupted sync patterns
   - Invalid lengths (too short, too long, mismatched)
   - Bad CRC values
   - Truncated frames
   - All flag combinations

3. **Builder Tests** (minimum 10 tests)
   - Frames with 0-byte to maximum payloads
   - Verify CRC calculation
   - Buffer overflow prevention
   - Round-trip tests (build → parse → verify)

4. **Ring Buffer Tests** (minimum 12 tests)
   - Write/read basic operations
   - Wrap-around behavior
   - Full buffer handling
   - Partial reads/writes
   - Interleaved operations
   - Empty/full state detection

5. **Integration Tests** (minimum 5 tests)
   - Stream of multiple frames
   - Fragmented frame reception
   - Recovery from corrupted data
   - Performance benchmarks
   - Memory leak detection

---

## Performance Specifications

Your implementation must achieve:

- **Parsing throughput**: ≥ 10 MB/s on modern x86_64 CPU
- **Memory usage**: Maximum 2KB static allocation for ring buffer
- **Dynamic allocation**: Only for payload storage in parsed frames
- **CRC calculation**: Use lookup table for O(n) complexity
- **Zero memory leaks**: Verified by Valgrind memcheck

---

## Development Environment Setup

### Required Tools

1. **Compiler**: GCC 9+ or Clang 10+
   ```bash
   # Ubuntu/Debian
   sudo apt-get install build-essential

   # macOS
   brew install gcc
   ```

2. **Debugging Tools**:
   ```bash
   # Ubuntu/Debian
   sudo apt-get install gdb valgrind

   # macOS
   brew install gdb   # Note: requires code signing
   ```

3. **Static Analysis**:
   ```bash
   sudo apt-get install clang-tools cppcheck
   ```

4. **Documentation**:
   ```bash
   sudo apt-get install doxygen graphviz
   ```

### Makefile Template

```makefile
CC = gcc
CFLAGS = -Wall -Wextra -Werror -pedantic -std=c11 -O2
CFLAGS_DEBUG = -g -O0 -fsanitize=address -fsanitize=undefined
LDFLAGS =

SRC = $(wildcard src/*.c)
OBJ = $(SRC:.c=.o)
TEST_SRC = $(wildcard tests/*.c)
TEST_OBJ = $(TEST_SRC:.c=.o)

TARGET = libbytestream.a
TEST_TARGET = test_runner

.PHONY: all clean test debug

all: $(TARGET)

$(TARGET): $(OBJ)
	ar rcs $@ $^

debug: CFLAGS = $(CFLAGS_DEBUG)
debug: $(TARGET)

test: $(TEST_TARGET)
	./$(TEST_TARGET)

$(TEST_TARGET): $(TEST_OBJ) $(TARGET)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $

clean:
	rm -f $(OBJ) $(TEST_OBJ) $(TARGET) $(TEST_TARGET)

check:
	cppcheck --enable=all --suppress=missingIncludeSystem src/

format:
	clang-format -i src/*.c src/*.h tests/*.c

docs:
	doxygen Doxyfile
```

### GitHub Actions CI Configuration

Create `.github/workflows/ci.yml`:

```yaml
name: CI

on: [push, pull_request]

jobs:
  build:
    runs-on: ubuntu-latest

    steps:
    - uses: actions/checkout@v2

    - name: Install dependencies
      run: |
        sudo apt-get update
        sudo apt-get install -y build-essential valgrind cppcheck

    - name: Build
      run: make

    - name: Run tests
      run: make test

    - name: Run Valgrind
      run: valgrind --leak-check=full --error-exitcode=1 ./test_runner

    - name: Static analysis
      run: make check

    - name: Check formatting
      run: |
        clang-format --version
        find . -name "*.c" -o -name "*.h" | xargs clang-format --dry-run --Werror
```

---

## Submission Requirements

Your final submission must include:

1. **GitHub Repository** containing:
   - All source code organized in `src/`, `include/`, `tests/` directories
   - `Makefile` with targets: `all`, `clean`, `test`, `debug`, `docs`
   - `.gitignore` appropriately configured for C projects
   - CI configuration passing all checks

2. **Documentation**:
   - `README.md` with build instructions, usage examples, and architecture overview
   - `DESIGN.md` explaining key decisions and tradeoffs (2-3 pages)
   - API documentation generated by Doxygen
   - Test coverage report

3. **Demo Program** (`demo/main.c`):
   - Simulate a communication channel
   - Send various message types through the protocol
   - Demonstrate error recovery
   - Show performance metrics

4. **Learning Reflection** (`LEARNING.md`):
   - Document 3 major challenges and solutions
   - List 5 key C concepts you mastered
   - Identify areas for improvement
   - Time log showing hours spent per week

---

## Grading Rubric

| Component | Weight | Criteria |
|-----------|--------|----------|
| **Functionality** | 40% | All modules work correctly, handle edge cases, pass tests |
| **Code Quality** | 20% | Clean, readable, well-commented, follows C best practices |
| **Testing** | 20% | Comprehensive test coverage, CI integration, no memory leaks |
| **Documentation** | 10% | Clear README, design document, API docs |
| **Professional Practices** | 10% | Git history, code organization, build system |

---

## Additional Challenges (Optional)

For those who finish early or want extra practice:

1. **Performance Optimization**: Achieve 50 MB/s parsing throughput using SIMD instructions
2. **Multi-threading**: Make ring buffer thread-safe using mutexes
3. **Compression**: Add simple RLE compression for payloads
4. **State Machine**: Implement frame parser as formal finite state machine
5. **Fuzzing**: Create fuzzer to find edge cases in your parser

---

## Resources and References

### Essential Reading
- "The C Programming Language" (K&R) - Chapters 5-6 on pointers and structures
- "Computer Systems: A Programmer's Perspective" - Chapter 2 on data representation
- [Beej's Guide to C Programming](https://beej.us/guide/bgc/) - Excellent online resource

### CRC Implementation
- [CRC-16-CCITT Specification](https://crccalc.com/) - Online calculator for verification
- Polynomial: 0x1021, Initial value: 0xFFFF

### Best Practices
- [CERT C Coding Standard](https://wiki.sei.cmu.edu/confluence/display/c) - Security guidelines
- [Linux Kernel Coding Style](https://www.kernel.org/doc/html/latest/process/coding-style.html) - Industry standard

### Tools Documentation
- [Valgrind Quick Start](https://valgrind.org/docs/manual/quick-start.html)
- [GDB Tutorial](https://www.cs.cmu.edu/~gilpin/tutorial/)
- [Makefile Tutorial](https://makefiletutorial.com/)

---

## Academic Integrity

This project is designed to be challenging but achievable with 8-12 hours per week of focused effort. You're encouraged to:
- Research concepts and implementations online
- Discuss approaches with peers
- Use AI assistants for learning concepts

However, your submitted code must be your own work. Document any significant resources that influenced your implementation.

---

## Getting Started Checklist

- [ ] Fork the starter repository (if provided) or create new repo
- [ ] Set up development environment with all required tools
- [ ] Read through entire specification and create task breakdown
- [ ] Implement CRC module first (it's independent and testable)
- [ ] Set up CI pipeline early (catch issues immediately)
- [ ] Commit frequently with descriptive messages
- [ ] Test continuously - don't wait until the end
- [ ] Document as you go - don't leave it for last

---

## Final Notes

This project simulates real embedded development challenges while remaining achievable on a desktop environment. The skills you develop here—memory management, bit manipulation, protocol handling, and professional practices—form the foundation for embedded systems programming.

Remember: In embedded systems, every byte counts, every cycle matters, and errors can have real consequences. Build habits of careful design, thorough testing, and clear documentation from day one.

**Good luck, and welcome to low-level programming!**

---

*Specification Version: 1.0.0*
*Last Updated: [Current Date]*
*Estimated Completion Time: 32-48 hours over 4 weeks*
