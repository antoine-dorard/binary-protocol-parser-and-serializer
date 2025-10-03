# binary-protocol-parser-and-serializer
## Description
The purpose of this project is to familiarise myself with low-level C programming concepts essential for embedded systems development. This ByteStream Protocol implementation serves as a practical introduction to memory management, pointer arithmetic, bit manipulation, endianness handling, and binary protocol design—core skills needed for embedded firmware development.
Rather than learning C in isolation, this project applies fundamental concepts through a real-world scenario: building a robust communication protocol similar to those used in automotive, aerospace, and IoT systems. The implementation emphasizes embedded best practices including static memory allocation, defensive programming, comprehensive error handling, and professional development workflows.

## Implemented features
- [x] CRC
- [ ] Parser
- [ ] Ring Buffer

## Setup

Install the repo:
```bash
git clone --recursive https://github.com/antoine-dorard/binary-protocol-parser-and-serializer.git
```

Get the dev tools:
```bash
sudo update
sudo apt install build-essentials cppcheck
```

## Testing
This project uses [Unity](https://github.com/ThrowTheSwitch/Unity) for unit testing.

```
make test
```

## Debugging
Use `make debug` to build the test files with the debug compiler flags.
Run gdb to debug, or the integrated C/C++ Runner extension in vscode.

At the moment, the current file to debug is manually specified in `launch.json`. If you want to debug a specific file, change the `program` property of the `C/C++ Runner: Debug Session` configuration.

> [!NOTE]
> If `make debug` or any other step breaks, try to `make clean` before trying again.

## Contributions
This project is closed for contributions. However, I welcome feedback and suggestions via GitHub issues.

## License
This project is under the [MIT License](https://github.com/antoine-dorard/binary-protocol-parser-and-serializer/blob/main/LICENSE).