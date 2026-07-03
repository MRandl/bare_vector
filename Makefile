CC := clang
OBJCOPY := llvm-objcopy

TARGET := armv6-none-eabi
CFLAGS := --target=$(TARGET) -mcpu=arm1176jzf-s -ffreestanding -O2 -Wall -Wextra
LDFLAGS := --target=$(TARGET) -mcpu=arm1176jzf-s -fuse-ld=lld -T linker.ld -nostdlib

OBJS := booter.o main.o

all: kernel.img

kernel.img: kernel.elf
	$(OBJCOPY) -O binary $< $@

kernel.elf: $(OBJS) linker.ld
	$(CC) $(LDFLAGS) -o $@ $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

%.o: %.S
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS) kernel.elf kernel.img

.PHONY: all clean
