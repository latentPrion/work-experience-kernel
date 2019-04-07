
#include <stdarg.h>
#include <stdint.h>
#include <libc/stdio.h>
#include <core.h>
#include <vga.h>
#include <multiboot.h>

/* EXPLANATION:
 * This is your stack. It should be large enough, but feel free to change its
 * size if you want (see include/core.h).
 *
 * The stack is where temporary variables are stored. Normally when writing a
 * program you don't have to think about the stack because it's allocated for
 * you automatically by your OS. But because you *ARE* the OS, you have to
 * reserve some space for your own stack.
 *
 * EXTRA:
 * We place this into the .data section so that memzeroing the .bss section
 * won't zero our stack.
 */
__attribute__((section(".data.stack"))) uint8_t stack[WEK_STACK_NBYTES];

int main(uint32_t mb_magic, multiboot_info_t *mb_info)
{
    int err;

    (void)mb_magic;
    (void)mb_info;

    /* Force multiboot header to be emitted.
     * Read core/multiboot.c for more info.
     */
    force_keep_multiboot_header();

    /* Initialize our printf function which will initialize our VGA driver and
     * enable us to print to the underlying VGA device. See libc/printf.c and
     * drivers/vga.c.
     */
    err = printf_init();
    if (err != 0) {
        return err;
    }

    // This is the introductory banner message. Proceed with caution!
    printf(
        "Hello and welcome to the Work Experience Kernel!\n"
        "\t[Multiboot magic %x, multiboot info @paddr %x].\n"
        "======================================================================"
        "==========\n"
        "This program is running on bare hardware with no OS beneath it.\n\n"
        "The machine is entirely at your mercy, and if you make any mistakes "
        "there is no OS to kill your program (because you *are* the OS) and "
        "no debugger to use to find out what went wrong!\n\n"
        "With great power comes great responsibility; use the force wisely "
        "young padawan!\n\n"
        "This is *your* OS, and you can do *anything* with it. Are there "
        "things that you don't like about OSes like Windows, Ubuntu, and "
        "MacOS? Well, why complain when you can write your own and fix those "
        "problems? Do you trust commercial OSes to be secure? No? Then why not "
        "write your own, and make it more secure?\n\n"
        "The digital world is your oyster!\n",
        mb_magic, mb_info);

    for(int i=0; i<3; i++) {
        printf("===========================================================\n");
    }

    /* You should do all of your work starting BELOW here!
     ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
     vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
     */

    return 0;
}

void panic(const char *fmt, ...)
{
    va_list args;

    /* EXPLANATION:
     * A very common pattern in modern OS kernels is to have a "panic" function.
     * This is what the kernel calls when it cannot think of any way to solve a
     * problem, and so it must plainly, abort execution and display a screen
     * that tries to summarize the reason why execution can no longer continue.
     *
     * Windows calls its panic a "Blue Screen of Death".
     * Linux calls its panic a "Kernel panic".
     * The Xbox signals its panic using the ring-shaped lights around the power
     * button.
     *
     * And so on. This panic function we've written here will simply try to
     * print a message to the screen (if printing is not malfunctioning or
     * unavailable) and then halt the CPU.
     */

    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);

    for (;;)
    {
        asm volatile(
            "cli\n\t"
            "hlt\n\t");
    }
}
