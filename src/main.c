#include <stdint.h>
#include <stdnoreturn.h>

#define PERIPHERAL_BASE 0x20000000UL // BCM2835 physical peripheral base

#define GPIO_BASE 	(PERIPHERAL_BASE + 0x200000UL)
#define GPFSEL4 	(*(volatile uint32_t *)(GPIO_BASE + 0x10))
#define GPSET1  	(*(volatile uint32_t *)(GPIO_BASE + 0x20))
#define GPCLR1  	(*(volatile uint32_t *)(GPIO_BASE + 0x2C))

#define SYSTIMER_BASE (PERIPHERAL_BASE + 0x3000UL)
#define SYSTIMER_CLOCK_INTERRUPT_STATUS   (*(volatile uint32_t *)(SYSTIMER_BASE + 0x00))
#define SYSTIMER_CLOCK_VALUE    (*(volatile uint32_t *)(SYSTIMER_BASE + 0x04))
#define SYSTIMER_CLOCK_TARGET   (*(volatile uint32_t *)(SYSTIMER_BASE + 0x10))

#define IRQ_BASE    (PERIPHERAL_BASE + 0xB000UL)
#define IRQ_ENABLE1 (*(volatile uint32_t *)(IRQ_BASE + 0x210)) // write-1-to-enable, per source

#define SYSTIMER_MASK (1u << 1) // system timer 1 match bit, shared by CS and IRQ_ENABLE1/PENDING1

#define ACT_LED_PIN 47 // GPIO47 drives the ACT LED on pi zero, active-low
#define BLINK_HALF_PERIOD_USEC 500000

static void wait_until(uint32_t target) {
    SYSTIMER_CLOCK_TARGET = target;
    while (!(SYSTIMER_CLOCK_INTERRUPT_STATUS & SYSTIMER_MASK)) {
        __asm__ volatile("wfi");
    }
    SYSTIMER_CLOCK_INTERRUPT_STATUS = SYSTIMER_MASK; // write 1 to interr. bit == acknowledge and clear
}

__attribute__((used))
noreturn void kernel_main(void) {
    // Pin 47 sits in GPFSEL4 (covers GPIO40-49), 3 bits per pin: bits [23:21]
    // setting these to 001 to declare the pin as output.
    GPFSEL4 = (GPFSEL4 & ~(0x7u << 21)) | (0x1u << 21);

    IRQ_ENABLE1 = SYSTIMER_MASK; // enable core IRQ line

    uint32_t next_wake = SYSTIMER_CLOCK_VALUE + BLINK_HALF_PERIOD_USEC;
    while (1) {
        GPCLR1 = 1u << (ACT_LED_PIN - 32); // clear = LED on (active-low)
        wait_until(next_wake);
        next_wake += BLINK_HALF_PERIOD_USEC;
        GPSET1 = 1u << (ACT_LED_PIN - 32); // set = LED off
        wait_until(next_wake);
        next_wake += BLINK_HALF_PERIOD_USEC;
    }
}

__attribute__((naked, section(".text.boot")))
noreturn void _start(void) {
    __asm__ volatile(
        "ldr sp, =_start \n"
        "b kernel_main \n"
    );
}
