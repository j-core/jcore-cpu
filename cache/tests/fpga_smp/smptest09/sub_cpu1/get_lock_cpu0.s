	.file	"get_lock_cpu0_stub.c"
	/* modify manually get_lock_cpu0_stub.s -> get_lock_cpu0.s */
	.text
	.text
	.align 1
	.global	_get_lock_cpu0
	.type	_get_lock_cpu0, @function
_get_lock_cpu0:
	add	#-8,r15
	mov	r15,r1
	add	#-56,r1
	mov.l	.L4,r2
	mov.l	r2,@(60,r1)
	mov	r15,r1
	add	#-56,r1
	mov.l	.L5,r2
	mov.l	r2,@(56,r1)
.L1001:
	mov	r15,r1
	add	#-56,r1
	mov.l	@(60,r1),r1
	tas.b	@r1  /* address 0x8100 */
	bf	.L1001
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
.L1002:
	mov	#0,r1
	mov	r1,r0
	add	#8,r15
	rts	
	nop
.L6:
	.align 2
.L4:
	.long	33024  /* 33024 (dec) = 0x8100 */
.L5:
	.long	335564796
	.size	_get_lock_cpu0, .-_get_lock_cpu0
	.ident	"GCC: (GNU) 4.6.0"
