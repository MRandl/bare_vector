.section ".text"
.global _start  // Execution starts here

_start:
    // Set stack to start below our code
    ldr     sp, =_start
    // Jump to our main() routine in C (make sure it doesn't return)
2:  bl      main
    // In case it does return, halt
3:  wfe
    b       3b
