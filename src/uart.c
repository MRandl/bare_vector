#include "uart.h"
#include "peripherals.h"

#define UART0_FR_TXFF (1u << 5) // transmit FIFO full
#define UART0_FR_RXFE (1u << 4) // receive FIFO empty

#define UART_BAUD_RATE 115200UL

// Fixed-point (x64) baud rate divisor, rounded to the nearest 1/64th:
// divisor = UART_CLOCK_HZ / (16 * UART_BAUD_RATE), so divisor * 64 = 4 * UART_CLOCK_HZ / UART_BAUD_RATE.
#define UART_BAUD_DIVISOR_X64 ((4 * UART_CLOCK_HZ + UART_BAUD_RATE / 2) / UART_BAUD_RATE)

static void delay_cycles(volatile uint32_t count) {
    while (count--) {
        __asm__ volatile("nop");
    }
}

void uart_init(void) {
    mem_barrier();
    UART0_CR = 0; // disable UART while it's configured
    mem_barrier(); // switching from the UART peripheral to GPIO

    // GPIO14/15 = TXD0/RXD0, alternate function 0 (ALT0 = 0b100), 3 bits per pin
    GPFSEL1 = (GPFSEL1 & ~((0x7u << 12) | (0x7u << 15))) | (0x4u << 12) | (0x4u << 15);

    // Disable pull up/down on pins 14 and 15 (BCM2835 ARM peripherals manual 6.2),
    // so an idle/floating line can't be misread as noise on TX/RX. GPPUD and
    // GPPUDCLK0 don't take effect on write: they feed an internal control
    // signal that needs a set number of clock cycles to propagate through the
    // pad logic before the next step is allowed to happen, so the datasheet
    // mandates busy-waiting ~150 cycles between each write, not just after.
    GPPUD = 0;
    delay_cycles(150);
    GPPUDCLK0 = (1u << 14) | (1u << 15);
    delay_cycles(150);
    GPPUD = 0;
    GPPUDCLK0 = 0;

    mem_barrier(); // switching from GPIO back to the UART peripheral
    UART0_ICR = 0x7FFu; // clear pending interrupts

    UART0_IBRD = UART_BAUD_DIVISOR_X64 / 64;
    UART0_FBRD = UART_BAUD_DIVISOR_X64 % 64;

    UART0_LCRH = (0x3u << 5); // 8 data bits, no parity, one stop bit

    UART0_CR = (1u << 0) | (1u << 8) | (1u << 9); // UART enable, TX enable, RX enable
    mem_barrier(); // ensure the UART is fully configured before another peripheral is touched
}

void uart_putc(uint8_t c) {
    while (UART0_FR & UART0_FR_TXFF) {
        __asm__ volatile("nop");
    }
    UART0_DR = c;
}

void uart_write(const uint8_t *data, size_t len) {
    for (size_t i = 0; i < len; i++) {
        uart_putc(data[i]);
    }
}

int16_t uart_try_getc(void) {
    if (UART0_FR & UART0_FR_RXFE) {
        return -1;
    }
    return (int16_t)(UART0_DR & 0xFFu);
}

uint8_t uart_getc(void) {
    while (UART0_FR & UART0_FR_RXFE) {
        __asm__ volatile("nop");
    }
    return (uint8_t)(UART0_DR & 0xFFu);
}
