# Run 'make test' to execute the test program.
# Run 'make valgrind' to run the test program in Valgrind.
# Run 'make submit' to generate the ZIP file to upload to CPM.
# Run 'make clean' to remove compiled files.

BIN_DIR  := bin
BIN_NAME := test

CC					:= gcc
CFLAGS				:= -Wall -Wextra -std=c99 -g
WORDCOUNT_HEADER	:= wordcount.h
WORDCOUNT_SRC		:= wordcount.c
SRC					:= hashmap.c
DEPS				:= $(SRC) $(SRC:.c=.h)
TEST				:= test_sample.c
TARGET				:= $(BIN_DIR)/$(BIN_NAME)

# Checks if wordcount.h exists, if so defines COUNTING_WORDS, includes wordcount.c in compilation
# and updates the dependencies.
ifneq ($(wildcard $(WORDCOUNT_HEADER)),)
CFLAGS	+= -DCOUNTING_WORDS
SRC		+= $(WORDCOUNT_SRC)
DEPS	:= $(SRC) $(SRC:.c=.h)
endif

default: dirs test

.PHONY: compile
compile:
	$(CC) $(CFLAGS) $(SRC) $(TEST) -o $(TARGET)

.PHONY: test
test: compile
	./$(TARGET)

.PHONY: valgrind
valgrind:
	valgrind --tool=memcheck ./$(TARGET)

.PHONY: submit
submit: $(DEPS)
	zip $(SRC:.c=.zip) $(DEPS)

.PHONY: clean
clean:
	-rm -f *.o
	-rm -f $(TARGET) $(SRC:.c=.zip)

.PHONY: dirs
dirs:
	@mkdir -p $(BIN_DIR)

