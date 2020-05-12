	.file	"cpu1_int_proto.c"
	.text
	.text
	.align 1
	.global	main
	.type	main, @function
main:
	mov.l	r14,@-r15
/* --	manual add start */
 	mov.l	r0,@-r15
 	mov.l	r1,@-r15
 	mov.l	r2,@-r15
/* --	manual add end */
	add	#-52,r15
	mov	r15,r14
	mov	r14,r1
	mov.l	.L8,r2
	mov.l	r2,@r1
	mov.l	.L9,r2
	mov.l	r2,@(4,r1)
	mov.l	.L10,r2
	mov.l	r2,@(8,r1)
	mov.l	.L11,r2
	mov.l	r2,@(12,r1)
	mov.l	.L12,r2
	mov.l	r2,@(16,r1)
	mov.l	.L13,r2
	mov.l	r2,@(20,r1)
	add	#24,r1
	mov	#0,r2
	mov.b	r2,@r1
	mov	r14,r1
	add	#-12,r1
	mov.l	.L14,r2
	mov.l	r2,@(48,r1)
	mov	r14,r1
	add	#-12,r1
	mov.l	@(48,r1),r1
	mov.l	@r1,r2
	mov	r14,r1
	add	#-12,r1
	mov.l	r2,@(60,r1)
	mov	r14,r1
	add	#-12,r1
	mov	r14,r2
	mov.l	r2,@(56,r1)
	bra	.L2
	nop
	.align 1
.L3:
	mov	r14,r1
	add	#-12,r1
	mov	r14,r2
	add	#-12,r2
	mov.l	@(60,r2),r2
	add	#1,r2
	mov.l	r2,@(60,r1)
.L2:
	mov	r14,r1
	add	#-12,r1
	mov.l	@(60,r1),r1
	mov.b	@r1,r1
	exts.b	r1,r1
	tst	r1,r1
	bf	.L3
	mov	r14,r1
	add	#-12,r1
	mov	#0,r2
	mov.l	r2,@(52,r1)
	bra	.L4
	nop
	.align 1
.L5:
	mov	r14,r1
	add	#-12,r1
	mov.l	@(56,r1),r1
	mov.b	@r1,r1
	exts.b	r1,r2
	mov	r14,r1
	add	#-12,r1
	mov.l	@(60,r1),r1
	mov.b	r2,@r1
	mov	r14,r1
	add	#-12,r1
	mov	r14,r2
	add	#-12,r2
	mov.l	@(60,r2),r2
	add	#1,r2
	mov.l	r2,@(60,r1)
	mov	r14,r1
	add	#-12,r1
	mov	r14,r2
	add	#-12,r2
	mov.l	@(56,r2),r2
	add	#1,r2
	mov.l	r2,@(56,r1)
	mov	r14,r1
	add	#-12,r1
	mov	r14,r2
	add	#-12,r2
	mov.l	@(52,r2),r2
	add	#1,r2
	mov.l	r2,@(52,r1)
.L4:
	mov	r14,r1
	add	#-12,r1
	mov.l	@(52,r1),r2
	mov	#23,r1
	cmp/gt	r1,r2
	bf	.L5
	mov	r14,r1
	add	#-12,r1
	mov.l	@(56,r1),r1
	mov	#0,r2
	mov.b	r2,@r1
	mov	r14,r1
	add	#-12,r1
	mov.l	.L15,r2
	mov.l	r2,@(44,r1)
	mov	r14,r1
	add	#-12,r1
	mov.l	.L16,r2
	mov.l	r2,@(40,r1)
	mov	r14,r1
	add	#-12,r1
	mov.l	@(44,r1),r1
	mov.l	@r1,r1
	tst	r1,r1
	bf	.L6
	mov	r14,r1
	add	#-12,r1
	mov.l	@(40,r1),r1
	mov.l	@r1,r2
	mov	r14,r1
	add	#-12,r1
	mov.l	@(44,r1),r1
	mov.l	r2,@r1
	bra	.L7
	nop
	.align 1
.L6:
	mov	r14,r1
	add	#-12,r1
	mov.l	@(44,r1),r1
	add	#4,r1
	mov	r14,r2
	add	#-12,r2
	mov.l	@(40,r2),r2
	mov.l	@r2,r2
	mov.l	r2,@r1
.L7:
	mov	r14,r1
	add	#-12,r1
	mov.l	.L17,r2
	mov.l	r2,@(48,r1)
	mov	r14,r1
	add	#-12,r1
	mov.l	@(48,r1),r1
	mov.l	@r1,r2
	mov.l	.L18,r1
	or	r1,r2
	mov	r14,r1
	add	#-12,r1
	mov.l	@(48,r1),r1
	mov.l	r2,@r1
	mov	#0,r1
	mov	r1,r0
	add	#52,r14
	mov	r14,r15
/* --	manual add start */
	mov.l	@r15+,r2
	mov.l	@r15+,r1
	mov.l	@r15+,r0
/* --	manual add end */
	mov.l	@r15+,r14
/* --	manual add start */
/* del	rts	<< del */
	rte	
/* --	manual add end */
	nop
.L19:
	.align 2
.L8:
	.long	1229998368
.L9:
	.long	1868784501
.L10:
	.long	1914710114
.L11:
	.long	2032157520
.L12:
	.long	1429282921
.L13:
	.long	1853106442
.L14:
	.long	33028
.L15:
	.long	33044
.L16:
	.long	33052
.L17:
	.long	-1412628288
.L18:
	.long	268435456
	.size	main, .-main
	.ident	"GCC: (Sourcery G++ Lite 2011.03-36 for SEI by OZH) 4.5.2"
