CC := clang
OBJCOPY := llvm-objcopy

TARGET := armv6-none-eabi

SRC_DIR := src
INCLUDE_DIR := include
BUILD_DIR := target

CFLAGS := --target=$(TARGET) -mcpu=arm1176jzf-s -ffreestanding -O3 -Wall -Wextra -Wpedantic -Werror -std=c23 -flto=full -I$(INCLUDE_DIR)
LDFLAGS := --target=$(TARGET) -mcpu=arm1176jzf-s -fuse-ld=lld -T linker.ld -nostdlib -flto=full

SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))

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

# bear only records compiler invocations it observes, so a stale (already
# up-to-date) tree yields an empty compile_commands.json; force a full rebuild.
compile_commands.json: clean
	bear -- $(MAKE) all

.PHONY: all clean
