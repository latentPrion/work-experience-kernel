#include <asm.h>

BEGIN_GLOBAL_FUNCTION(reg_context_pop_into_cpu)
	/**	EXPLANATION:
	 * void reg_context_pop_into_cpu(reg_context_t *context);
	 **/
	/* At this point, ESP+0=retAddr, ESP+4=context. */
	movl	0x4(%esp), %esp
	popw	%es
	popw	%ds
	popw	%gs
	popw	%fs

	popal

	addl	$8, %esp
	iretl
END_FUNCTION(reg_context_pop_into_cpu)

BEGIN_GLOBAL_FUNCTION(reg_context_save_snapshot_and_switch)
    /* EXPLANATION
     *  void reg_context_save_snapshot_and_switch(
     *      void **old_threads_stack0_current_level,
     *      void *new_threads_stack0_current_level);
     *
     * At this point:
     * ESP+0 = RetEIP
     * ESP+4 = old_threads_stack0_current_level
     * ESP+8 = new_threads_stack0_current_level
     */
    pushfl
    /* Skip cs, eip, errorCode and vectorNo */
    subl $16, %esp
    pushal
    pushl %fs
    pushl %gs
    pushl %ds
    pushl %es

    /* EAX should be free for use right now. */

    /* return the current stack pointer to the caller. */
    movl 72(%esp), %eax
    movl %esp, (%eax)

    /* Now load the reg context of the new thread. */
    movl 76(%esp), %eax
    movl %eax, %esp

    popl %es
    popl %ds
    popl %gs
    popl %fs
    popal
    addl $16, %esp
    popfl
    ret
END_FUNCTION(reg_context_save_snapshot_and_switch)
