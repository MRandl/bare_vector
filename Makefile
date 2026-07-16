CC := clang
OBJCOPY := llvm-objcopy

TARGET := armv6-none-eabi
CFLAGS := --target=$(TARGET) -mcpu=arm1176jzf-s -ffreestanding -O3 -Wall -Wextra -Wpedantic -Werror -std=c23 -flto=full
LDFLAGS := --target=$(TARGET) -mcpu=arm1176jzf-s -fuse-ld=lld -T linker.ld -nostdlib -flto=full

SRC_DIR := src
BUILD_DIR := target

OBJS := $(BUILD_DIR)/main.o

all: $(BUILD_DIR)/kernel.img

$(BUILD_DIR)/kernel.img: $(BUILD_DIR)/kernel.elf
	$(OBJCOPY) -O binary $< $@

$(BUILD_DIR)/kernel.elf: $(OBJS) linker.ld
	$(CC) $(LDFLAGS) -o $@ $(OBJS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD_DIR):
	mkdir -p $@

clean:
	rm -rf $(BUILD_DIR) compile_commands.json

.PHONY: all clean
