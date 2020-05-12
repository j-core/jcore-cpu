	.file	"cpu1_main.c"
	.text
	.text
	.align 1
	.global	main
	.type	main, @function
main:
	mov.l	r14,@-r15
	add	#-16,r15
	mov	r15,r14
	mov	r14,r1
	add	#-48,r1
	mov	#0,r2
	mov.l	r2,@(60,r1)
	mov	r14,r1
	add	#-48,r1
	mov.l	.L5,r2
	mov.l	r2,@(56,r1)
	mov	r14,r1
	add	#-48,r1
	mov.l	@(56,r1),r1
	mov	#11,r2
	mov.l	r2,@r1
	mov	r14,r1
	add	#-48,r1
	mov.l	.L6,r2
	mov.l	r2,@(52,r1)
	mov	r14,r1
	add	#-48,r1
	mov.l	@(52,r1),r1
	mov.w	.L7,r2
	mov.l	r2,@r1
	mov	r14,r1
	add	#-48,r1
	mov.l	.L8,r2
	mov.l	r2,@(48,r1)
	mov	r14,r1
	add	#-48,r1
	mov.l	@(48,r1),r1
	mov	#0,r2
	mov.l	r2,@r1
	bra	.L2
	nop
	.align 1
.L3:
	mov	r14,r1
	add	#-48,r1
	mov.l	@(48,r1),r1
	mov.l	@r1,r3
	mov	r14,r1
	add	#-48,r1
	mov	r14,r2
	add	#-48,r2
	mov.l	@(60,r2),r2
	add	r3,r2
	mov.l	r2,@(60,r1)
	mov	r14,r1
	add	#-48,r1
	mov.l	@(48,r1),r1
	mov.l	@r1,r1
	mov	r1,r2
	add	#1,r2
	mov	r14,r1
	add	#-48,r1
	mov.l	@(48,r1),r1
	mov.l	r2,@r1
.L2:
	mov	r14,r1
	add	#-48,r1
	mov.l	@(48,r1),r1
	mov.l	@r1,r2
	mov.l	.L9,r1
	cmp/gt	r1,r2
	bf	.L3
	mov	r14,r1
	add	#-48,r1
	mov.l	.L10,r2
	mov.l	r2,@(56,r1)
	mov	r14,r1
	add	#-48,r1
	mov.l	@(56,r1),r1
	mov	r14,r2
	add	#-48,r2
	mov.l	@(60,r2),r2
	mov.l	r2,@r1
	mov	r14,r1
	add	#-48,r1
	mov.l	.L11,r2
	mov.l	r2,@(56,r1)
	mov	r14,r1
	add	#-48,r1
	mov.l	@(56,r1),r1
	mov	#1,r2
	mov.l	r2,@r1
.L4:
	bra	.L4
	nop
	.align 1
.L7:
	.short	24576
.L12:
	.align 2
.L5:
	.long	33068
.L6:
	.long	-1412627960
.L8:
	.long	33052
.L9:
	.long	199999
.L10:
	.long	33036
.L11:
	.long	33040
	.size	main, .-main
	.ident	"GCC: (Sourcery G++ Lite 2011.03-36 for SEI by OZH) 4.5.2"
