	.file	"get_lock_cpu0_stub.c"
	.text
	.text
	.align 1
	.global	_get_lock_cpu0_stub
	.type	_get_lock_cpu0_stub, @function
_get_lock_cpu0_stub:
	add	#-8,r15
	mov	r15,r1
	add	#-56,r1
	mov.l	.L4,r2
	mov.l	r2,@(60,r1)
	mov	r15,r1
	add	#-56,r1
	mov.l	.L5,r2
	mov.l	r2,@(56,r1)
	mov	r15,r1
	add	#-56,r1
	mov.l	@(60,r1),r1
	mov.b	@r1,r1
	exts.b	r1,r1
	mov	r1,r2
	add	#9,r2
	mov	r15,r1
	add	#-56,r1
	mov.l	@(56,r1),r1
	mov.l	r2,@r1
	bra	.L2
	nop
	.align 1
.L3:
	mov	r15,r1
	add	#-56,r1
	mov.l	@(56,r1),r1
	add	#44,r1
	mov.l	@r1,r2
	add	#1,r2
	mov.l	r2,@r1
.L2:
	mov	r15,r1
	add	#-56,r1
	mov.l	@(56,r1),r1
	mov.l	@r1,r1
	tst	r1,r1
	bt	.L3
	mov	#0,r1
	mov	r1,r0
	add	#8,r15
	rts	
	nop
.L6:
	.align 2
.L4:
	.long	335609888
.L5:
	.long	335564796
	.size	_get_lock_cpu0_stub, .-_get_lock_cpu0_stub
	.ident	"GCC: (GNU) 4.6.0"
