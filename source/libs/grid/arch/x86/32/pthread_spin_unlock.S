	.globl	pthread_spin_unlock
	.type	pthread_spin_unlock,@function
	.align	16
pthread_spin_unlock:
	movl	4(%esp), %eax
	movl	$1, (%eax)
	xorl	%eax, %eax
	ret
	.size	pthread_spin_unlock,.-pthread_spin_unlock

	/* The implementation of pthread_spin_init is identical.  */
	.globl	pthread_spin_init
pthread_spin_init = pthread_spin_unlock
