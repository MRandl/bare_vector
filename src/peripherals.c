#include "peripherals.h"

void act_init() {
    mem_barrier();
    GPFSEL4 = (GPFSEL4 & ~(0x7u << 21)) | (0x1u << 21);
}

void irq_init() {
    mem_barrier(); // switching from GPIO to the interrupt controller
    IRQ_ENABLE1 = SYSTIMER_MASK; // enable core IRQ line
}

void set_act(void) {
    mem_barrier();
    GPCLR1 = 1u << (ACT_LED_PIN - 32); // clear = LED on (active-low)
}

void clear_act(void) {
    mem_barrier();
    GPSET1 = 1u << (ACT_LED_PIN - 32);
}

void wait_until(uint32_t target) {
    mem_barrier();
    SYSTIMER_CLOCK_TARGET = target;
    while (!(SYSTIMER_CLOCK_INTERRUPT_STATUS & SYSTIMER_MASK)) {
        __asm__ volatile("wfi");
    }
    SYSTIMER_CLOCK_INTERRUPT_STATUS = SYSTIMER_MASK; // write 1 to interr. bit == acknowledge and clear
}
