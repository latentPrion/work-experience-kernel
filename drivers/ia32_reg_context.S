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
    /* At this point:
     * ESP+0 = RetEIP
     * ESP+4 = old thread's reg context
     * ESP+8 = new thread's reg context
     */
    pushfl
    /* Skip cs, eip, errorCode and vectorNo */
    subl $16, %esp
    pushal
    pushw %fs
    pushw %gs
    pushw %ds
    pushw %es

    /* Now load the reg context of the new thread.
     * Eax should be free right now.
     */
    movl 68(%esp), %eax
    movl %eax, %esp
    popw %es
    popw %ds
    popw %gs
    popw %fs
    popal
    addl $16, %esp
    popfl
    ret
END_FUNCTION(reg_context_save_snapshot_and_switch)
