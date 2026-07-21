#ifndef UART_H
#define UART_H

#include <stddef.h>
#include <stdint.h>

void uart_init(void);
void uart_putc(uint8_t c);
void uart_write(const uint8_t *data, size_t len);

// Non-blocking read: returns the next byte (0-255), or -1 if the RX FIFO is empty.
int16_t uart_try_getc(void);

// Blocking read: waits for and returns the next byte from the controller.
uint8_t uart_getc(void);

#endif
