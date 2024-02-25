# Default compiler
CC := gcc
# Default flags
CFLAGS := -Wall -Wextra -O2

# Check if cross compiler is provided
ifdef CROSS_COMPILE
	CC := $(CROSS_COMPILE)gcc
endif

# Source file
SRC := writer.c
# Output binary
OUT := writer

all: $(OUT)

$(OUT): $(SRC)
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(OUT)

