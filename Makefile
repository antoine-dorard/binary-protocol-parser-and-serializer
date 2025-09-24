ifeq ($(OSTYPE),cygwin)
	CLEANUP=rm -f
	MKDIR=mkdir -p
	TARGET_EXTENSION=out
else ifeq ($(OS),Windows_NT)
	CLEANUP=del /F /Q
	MKDIR=mkdir
	TARGET_EXTENSION=exe
else
	CLEANUP=rm -f
	MKDIR=mkdir -p
	TARGET_EXTENSION=out
endif

CC = gcc
CFLAGS = -Wall -Wextra -Werror -pedantic -std=c23 -O2 -I$(PATH_UNITY) -I$(PATH_SRC)
CFLAGS_DEBUG = -g -O0 -fsanitize=address -fsanitize=undefined
DEPEND_FLAGS=-MM -MG -MF

PATH_UNITY = Unity/src/
PATH_SRC = src/
PATH_TEST = tests/
PATH_BUILD = build/
PATH_DEPENDS = $(PATH_BUILD)depends/
PATH_OBJ = $(PATH_BUILD)objs/
PATH_RESULTS = $(PATH_BUILD)results/

SRC = $(wildcard $(PATH_SRC)*.c)
OBJ = $(SRC:.c=.o)
TEST_SRC = $(wildcard $(PATH_TEST)*.c)
TEST_OBJ = $(TEST_SRC:.c=.o)

BUILD_PATHS = $(PATH_BUILD) $(PATH_DEPENDS) $(PATH_OBJ) $(PATH_RESULTS)

LIB_TARGET = libbytestream.a

.PHONY: all clean test debug run

BUILDS = $(patsubst $(PATH_TEST)test_%.c,$(PATH_BUILD)test_%.$(TARGET_EXTENSION),$(TEST_SRC)) \
		$(patsubst $(PATH_SRC)%.c,$(PATH_BUILD)%.$(TARGET_EXTENSION),$(SRC))
		 
# Check if all c files have their corresponding test files
$(foreach file,$(SRC),$(if $(filter $(PATH_TEST)test_$(basename $(notdir $(file))).c,$(TEST_SRC)),,$(error No test file for $(file))))



all: $(BUILDS)

TEST_RESULTS = $(patsubst $(PATH_TEST)test_%.c,$(PATH_RESULTS)test_%.txt,$(TEST_SRC))
# build/results/test_crc.txt build/results/test_parser.txt


test: $(BUILD_PATHS) $(TEST_RESULTS)
	@echo $(BUILDS)
	@echo "\n-----------------------\nPASS:\n-----------------------"
	@echo `grep -s PASS $(PATH_RESULTS)*.txt`
	@echo "\n-----------------------\nIGNORES:\n-----------------------"
	@echo `grep -s IGNORE $(PATH_RESULTS)*.txt`
	@echo "\n-----------------------\n\nFAILURES:\n-----------------------"
	@echo `grep -s FAIL $(PATH_RESULTS)*.txt`
	@echo "\nDONE"


# Execute all executables
$(PATH_RESULTS)%.txt: $(PATH_BUILD)%.$(TARGET_EXTENSION)
	-./$< > $@ 2>&1 

# Create executables
$(PATH_BUILD)%.$(TARGET_EXTENSION): $(PATH_OBJ)%.o $(PATH_OBJ)unity.o #$(PATHD)%.d
	$(CC) $(CFLAGS) -o $@ $^
	
$(PATH_BUILD)%.$(TARGET_EXTENSION): $(PATH_OBJ)test_%.o $(PATH_OBJ)unity.o #$(PATHD)test_%.d
	$(CC) $(CFLAGS) -o $@ $^

# Create object files
$(PATH_OBJ)%.o:: $(PATH_SRC)%.c 
	$(CC) $(CFLAGS) -c -o $@ $<

$(PATH_OBJ)%.o:: $(PATH_TEST)%.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(PATH_OBJ)%.o:: $(PATH_UNITY)%.c $(PATH_UNITY)%.h
	$(CC) $(CFLAGS) -c -o $@ $<



# Dependencies
$(PATH_DEPENDS)%.d:: $(PATH_TEST)%.c
	$(CC) $(DEPEND_FLAGS) $@ $<



# Create build directories
$(PATH_BUILD):
	$(MKDIR) $(PATH_BUILD)

$(PATH_DEPENDS):
	$(MKDIR) $(PATH_DEPENDS)

$(PATH_OBJ):
	$(MKDIR) $(PATH_OBJ)

$(PATH_RESULTS):
	$(MKDIR) $(PATH_RESULTS)


clean:
	$(CLEANUP) $(PATH_OBJ)*.o
	$(CLEANUP) $(PATH_BUILD)*.$(TARGET_EXTENSION)
	$(CLEANUP) $(PATH_RESULTS)*.txt

# Prevent deletion of intermediate files
.PRECIOUS: $(PATH_BUILD)Test%.$(TARGET_EXTENSION)
#.PRECIOUS: $(PATH_DEPENDS)%.d
.PRECIOUS: $(PATH_OBJ)%.o
.PRECIOUS: $(PATH_RESULTS)%.txt

# build: # TODO 

debug: CFLAGS = $(CFLAGS_DEBUG)
debug: $(TARGET)

# clean:
# 	rm -f $(OBJ) $(TEST_OBJ) $(TARGET) $(TEST_TARGET)

check:
	cppcheck --enable=all --suppress=missingIncludeSystem src/

format:
	clang-format -i src/*.c src/*.h tests/*.c

docs:
	doxygen Doxyfile