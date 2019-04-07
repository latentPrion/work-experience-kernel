#ifndef __ASSEMBLER__
    #define __ASSEMBLER__
#endif

#include <asm.h>
#include <core.h>

.section .multiboot,"a"

.text

BEGIN_GLOBAL_FUNCTION(_start)
    cli

    /* Set up stack */
    movl $stack, %esp
    addl $WEK_STACK_NBYTES, %esp

    /* Push multiboot info */
    pushl %ebx
    pushl %eax

    /* Zero out .bss */
    call __wek_libc_pre_main
    call main
    call __wek_libc_post_main

1:
    cli
    hlt
    jmp 1b
END_FUNCTION(_start)

/*
BEGIN_GLOBAL_FUNCTION(__libc_csu_init)
    ret
END_FUNCTION(__libc_csu_init)

BEGIN_GLOBAL_FUNCTION(__libc_csu_fini)
    ret
END_FUNCTION(__libc_csu_fini)

BEGIN_GLOBAL_FUNCTION(__libc_start_main)
    ret
END_FUNCTION(__libc_start_main)
*/
