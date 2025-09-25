# binary-protocol-parser-and-serializer
## Description
The purpose of this project is to familiarise myself with low-level C programming concepts essential for embedded systems development. This ByteStream Protocol implementation serves as a practical introduction to memory management, pointer arithmetic, bit manipulation, endianness handling, and binary protocol design—core skills needed for embedded firmware development.
Rather than learning C in isolation, this project applies fundamental concepts through a real-world scenario: building a robust communication protocol similar to those used in automotive, aerospace, and IoT systems. The implementation emphasizes embedded best practices including static memory allocation, defensive programming, comprehensive error handling, and professional development workflows.

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

## Contributions
This project is closed for contributions. However, I welcome feedback and suggestions via GitHub issues.

## License
This project is under the [MIT License](https://github.com/antoine-dorard/binary-protocol-parser-and-serializer/blob/main/LICENSE).