	.file	"cpu1_int_proto.c"
	.text
	.text
	.align 1
	.global	main
	.type	main, @function
main:
	mov.l	r14,@-r15
	add	#-4,r15
	mov	r15,r14
	mov	r14,r1
	add	#-60,r1
	mov.l	.L2,r2
	mov.l	r2,@(60,r1)
	mov	r14,r1
	add	#-60,r1
	mov.l	@(60,r1),r1
	mov.l	@r1,r1
	mov	r1,r2
	add	#1,r2
	mov	r14,r1
	add	#-60,r1
	mov.l	@(60,r1),r1
	mov.l	r2,@r1
	mov	#0,r1
	mov	r1,r0
	add	#4,r14
	mov	r14,r15
	mov.l	@r15+,r14
	rts	
	nop
.L3:
	.align 2
.L2:
	.long	33044
	.size	main, .-main
	.ident	"GCC: (Sourcery G++ Lite 2011.03-36 for SEI by OZH) 4.5.2"
