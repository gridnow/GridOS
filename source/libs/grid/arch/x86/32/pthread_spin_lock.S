#include "def.h"

	.globl	pthread_spin_lock
	.type	pthread_spin_lock,@function
	.align	16
pthread_spin_lock:
	mov	4(%esp), %eax
1:	LOCK
	decl	0(%eax)
	jne	2f
	xor	%eax, %eax
	ret

	.align	16
2:	rep
	nop
	cmpl	$0, 0(%eax)
	jg	1b
	jmp	2b
	.size	pthread_spin_lock,.-pthread_spin_lock
