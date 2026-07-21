#ifndef UART_H
#define UART_H

#include <stddef.h>
#include <stdint.h>

void uart_init(void);
void uart_putc(uint8_t c);
void uart_write(const uint8_t *data, size_t len);
int16_t uart_try_getc(void);
uint8_t uart_getc(void);

#endif
