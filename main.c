#include <stdint.h>
#include <stdnoreturn.h>

#define PERIPHERAL_BASE 0x20000000UL // BCM2835 physical peripheral base

#define GPIO_BASE 	(PERIPHERAL_BASE + 0x200000UL)
#define GPFSEL4 	(*(volatile uint32_t *)(GPIO_BASE + 0x10))
#define GPSET1  	(*(volatile uint32_t *)(GPIO_BASE + 0x20))
#define GPCLR1  	(*(volatile uint32_t *)(GPIO_BASE + 0x2C))

#define SYSTIMER_BASE (PERIPHERAL_BASE + 0x3000UL)
#define SYSTIMER_CLOCK_STATUS   (*(volatile uint32_t *)(SYSTIMER_BASE + 0x00)) // match status, write-1-to-clear
#define SYSTIMER_CLOCK  (*(volatile uint32_t *)(SYSTIMER_BASE + 0x04)) // free-running, increments once per microsecond
#define SYSTIMER_C1   (*(volatile uint32_t *)(SYSTIMER_BASE + 0x10)) // compare 1 (C0/C2 are claimed by the GPU firmware)

#define IRQ_BASE    (PERIPHERAL_BASE + 0xB000UL)
#define IRQ_ENABLE1 (*(volatile uint32_t *)(IRQ_BASE + 0x210)) // write-1-to-enable, per source

#define SYSTIMER_MASK (1u << 1) // system timer 1 match bit, shared by CS and IRQ_ENABLE1/PENDING1

#define ACT_LED_PIN 47 // GPIO47 drives the ACT LED on pi zero, active-low
#define BLINK_HALF_PERIOD_USEC 500000 // on-time == off-time == half the 1s blink period

static void wait_until(uint32_t target) {
    SYSTIMER_C1 = target;
    while (!(SYSTIMER_CLOCK_STATUS & SYSTIMER_MASK)) {
        __asm__ volatile("wfi");
    }
    SYSTIMER_CLOCK_STATUS = SYSTIMER_MASK;
}

noreturn int main() {
    // Pin 47 sits in GPFSEL4 (covers GPIO40-49), 3 bits per pin: bits [23:21]. 001 = output.
    GPFSEL4 = (GPFSEL4 & ~(0x7u << 21)) | (0x1u << 21);

    IRQ_ENABLE1 = SYSTIMER_MASK; // let system timer 1 matches assert the core's IRQ line

    uint32_t next_wake = SYSTIMER_CLOCK + BLINK_HALF_PERIOD_USEC;
    while (1) {
        GPCLR1 = 1u << (ACT_LED_PIN - 32); // clear = LED on (active-low)
        wait_until(next_wake);
        next_wake += BLINK_HALF_PERIOD_USEC;
        GPSET1 = 1u << (ACT_LED_PIN - 32); // set = LED off
        wait_until(next_wake);
        next_wake += BLINK_HALF_PERIOD_USEC;
    }
}
