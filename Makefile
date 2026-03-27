CC := emcc

SRC_DIR := src
INC_DIR := include
TARGET := index.wasm

CFLAGS := -I$(INC_DIR)

SRCS := $(wildcard $(SRC_DIR)/*.c)

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET) -s STANDALONE_WASM -Wl,--no-entry

clean:
	rm -f $(TARGET) 

.PHONY: $(TARGET) clean