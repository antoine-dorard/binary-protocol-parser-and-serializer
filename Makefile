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