#include <stdint.h>
#include <stdnoreturn.h>

#include "peripherals.h"
#include "uart.h"

#define BLINK_HALF_PERIOD_USEC 500000

static void echo_uart(void) {
    // uint16_t read = uart_try_getc();
    // while (read >= 0) {
    //     uart_putc((uint8_t) read);
    //     read = uart_try_getc();
    // }
}

__attribute__((used))
noreturn void kernel_main(void) {
    act_init();
    irq_init();
    uart_init();

    uint32_t next_wake = SYSTIMER_CLOCK_VALUE;
    while (1) {
        set_act();
        next_wake += BLINK_HALF_PERIOD_USEC;
        wait_until(next_wake);

        clear_act();
        next_wake += BLINK_HALF_PERIOD_USEC;
        wait_until(next_wake);

        echo_uart();
    }
}

__attribute__((naked, section(".text.boot")))
noreturn void _start(void) {
    __asm__ volatile(
        "ldr sp, =_start \n"      // sp = location of code downwards
        "ldr r0, =__bss_start \n" // wipe bss region
        "ldr r1, =__bss_end \n"
        "1: \n"
        "cmp r0, r1 \n"
        "bge 2f \n"
        "mov r2, #0 \n"
        "strb r2, [r0], #1 \n"
        "b 1b \n"
        "2: \n"
        "b kernel_main \n"        // call kernel
    );
}
