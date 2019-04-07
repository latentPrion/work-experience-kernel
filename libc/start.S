#ifndef __ASSEMBLER__
    #define __ASSEMBLER__
#endif

#include <asm.h>
#include <core.h>

.section .multiboot,"a"

.text

/* This point below, "_start" is where the CPU begins executing our OS kernel.
 *
 * Unfortunately there are some things we can only do in Assembly language,
 * that we cannot do in higher level source code languages.
 *
 * So we quickly do a few things here in assembly and then jump into our
 * high level source code language, known as "The C Programming Language".
 *
 * The line below which says "call main" is the line where we actually get
 * ourselves out of assembly and into our higher level source language (C).
 */

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
