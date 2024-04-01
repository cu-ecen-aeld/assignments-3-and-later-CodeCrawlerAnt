# Default compiler
CC := gcc
# Default flags
CFLAGS := -Wall -Wextra -O2

# Check if cross compiler is provided
ifdef CROSS_COMPILE
	CC := $(CROSS_COMPILE)gcc
endif

# Source file
SRC := aesdsocket.c
# Output binary
OUT := aesdsocket

all: $(OUT)

$(OUT): $(SRC)
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(OUT)