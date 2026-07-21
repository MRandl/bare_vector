#ifndef PERIPHERALS_H
#define PERIPHERALS_H

#include <stdint.h>

// The BCM2835 does not guarantee that accesses to different peripherals
// complete in program order so a barrier is required whenever code switches
// from touching one peripheral block to another
static inline void mem_barrier(void) {
    __asm__ volatile("mcr p15, 0, %0, c7, c10, 4" :: "r"(0) : "memory");
}

#define PERIPHERAL_BASE 0x20000000UL

#define ACT_LED_PIN 47 // GPIO pin driving the ACT LED on this board, active-low

// gpio
#define GPIO_BASE (PERIPHERAL_BASE + 0x200000UL)
#define GPFSEL1   (*(volatile uint32_t *)(GPIO_BASE + 0x04))
#define GPFSEL4   (*(volatile uint32_t *)(GPIO_BASE + 0x10))
#define GPSET1    (*(volatile uint32_t *)(GPIO_BASE + 0x20))
#define GPCLR1    (*(volatile uint32_t *)(GPIO_BASE + 0x2C))
#define GPPUD     (*(volatile uint32_t *)(GPIO_BASE + 0x94))
#define GPPUDCLK0 (*(volatile uint32_t *)(GPIO_BASE + 0x98))

// clocks
#define SYSTIMER_BASE (PERIPHERAL_BASE + 0x3000UL)
#define SYSTIMER_CLOCK_INTERRUPT_STATUS (*(volatile uint32_t *)(SYSTIMER_BASE + 0x00))
#define SYSTIMER_CLOCK_VALUE            (*(volatile uint32_t *)(SYSTIMER_BASE + 0x04))
#define SYSTIMER_CLOCK_TARGET           (*(volatile uint32_t *)(SYSTIMER_BASE + 0x10))

// interrupts
#define IRQ_BASE    (PERIPHERAL_BASE + 0xB000UL)
#define IRQ_ENABLE1 (*(volatile uint32_t *)(IRQ_BASE + 0x210)) // write-1-to-enable, per source
#define SYSTIMER_MASK (1u << 1) // system timer 1 match bit, shared by CS and IRQ_ENABLE1/PENDING1


void irq_init(void);
void act_init(void);

void set_act(void);
void clear_act(void);

void wait_until(uint32_t);

// UART
#define UART_CLOCK_HZ 48000000UL

#define UART0_BASE (PERIPHERAL_BASE + 0x201000UL)
#define UART0_DR   (*(volatile uint32_t *)(UART0_BASE + 0x00))
#define UART0_FR   (*(volatile uint32_t *)(UART0_BASE + 0x18))
#define UART0_IBRD (*(volatile uint32_t *)(UART0_BASE + 0x24))
#define UART0_FBRD (*(volatile uint32_t *)(UART0_BASE + 0x28))
#define UART0_LCRH (*(volatile uint32_t *)(UART0_BASE + 0x2C))
#define UART0_CR   (*(volatile uint32_t *)(UART0_BASE + 0x30))
#define UART0_ICR  (*(volatile uint32_t *)(UART0_BASE + 0x44))

#endif
