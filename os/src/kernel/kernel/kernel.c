#include <stdio.h>

#include <kernel/tty.h>





void kernel_main(void)
{
    terminal_initialize();
    terminal_writestring("The Kernel has been loaded successfully!\n");
    terminal_writestring("Welcome to SnowOS V0.01 By Vicente Rizzi    2021\n");
    terminal_writeint(-1);
}
