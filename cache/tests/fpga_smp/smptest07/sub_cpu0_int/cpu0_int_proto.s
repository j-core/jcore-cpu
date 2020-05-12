	.file	"cpu0_int_proto.c"
	.text
	.text
	.align 1
	.global	main
	.type	main, @function
main:
	mov.l	r14,@-r15
	add	#-44,r15
	mov	r15,r14
	mov	r14,r1
	mov.l	.L6,r2
	mov.l	r2,@r1
	mov.l	.L7,r2
	mov.l	r2,@(4,r1)
	mov.l	.L8,r2
	mov.l	r2,@(8,r1)
	mov.l	.L9,r2
	mov.l	r2,@(12,r1)
	mov.l	.L10,r2
	mov.l	r2,@(16,r1)
	mov.l	.L11,r2
	mov.l	r2,@(20,r1)
	add	#24,r1
	mov	#0,r2
	mov.b	r2,@r1
	mov	r14,r1
	add	#-20,r1
	mov.l	.L12,r2
	mov.l	r2,@(48,r1)
	mov	r14,r1
	add	#-20,r1
	mov.l	@(48,r1),r1
	mov.l	@r1,r2
	mov	r14,r1
	add	#-20,r1
	mov.l	r2,@(60,r1)
	mov	r14,r1
	add	#-20,r1
	mov	r14,r2
	mov.l	r2,@(56,r1)
	bra	.L2
	nop
	.align 1
.L3:
	mov	r14,r1
	add	#-20,r1
	mov	r14,r2
	add	#-20,r2
	mov.l	@(60,r2),r2
	add	#1,r2
	mov.l	r2,@(60,r1)
.L2:
	mov	r14,r1
	add	#-20,r1
	mov.l	@(60,r1),r1
	mov.b	@r1,r1
	exts.b	r1,r1
	tst	r1,r1
	bf	.L3
	mov	r14,r1
	add	#-20,r1
	mov	#0,r2
	mov.l	r2,@(52,r1)
	bra	.L4
	nop
	.align 1
.L5:
	mov	r14,r1
	add	#-20,r1
	mov.l	@(56,r1),r1
	mov.b	@r1,r1
	exts.b	r1,r2
	mov	r14,r1
	add	#-20,r1
	mov.l	@(60,r1),r1
	mov.b	r2,@r1
	mov	r14,r1
	add	#-20,r1
	mov	r14,r2
	add	#-20,r2
	mov.l	@(60,r2),r2
	add	#1,r2
	mov.l	r2,@(60,r1)
	mov	r14,r1
	add	#-20,r1
	mov	r14,r2
	add	#-20,r2
	mov.l	@(56,r2),r2
	add	#1,r2
	mov.l	r2,@(56,r1)
	mov	r14,r1
	add	#-20,r1
	mov	r14,r2
	add	#-20,r2
	mov.l	@(52,r2),r2
	add	#1,r2
	mov.l	r2,@(52,r1)
.L4:
	mov	r14,r1
	add	#-20,r1
	mov.l	@(52,r1),r2
	mov	#23,r1
	cmp/gt	r1,r2
	bf	.L5
	mov	#0,r1
	mov	r1,r0
	add	#44,r14
	mov	r14,r15
	mov.l	@r15+,r14
	rts	
	nop
.L13:
	.align 2
.L6:
	.long	1868784501
.L7:
	.long	1914726768
.L8:
	.long	1763715170
.L9:
	.long	2032157520
.L10:
	.long	1429217385
.L11:
	.long	1853106442
.L12:
	.long	33028
	.size	main, .-main
	.ident	"GCC: (Sourcery G++ Lite 2011.03-36 for SEI by OZH) 4.5.2"
