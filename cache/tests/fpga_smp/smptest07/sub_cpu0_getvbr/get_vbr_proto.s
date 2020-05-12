	.file	"get_vbr_proto.c"
	.text
	.text
	.align 1
	.global	get_vbr
	.type	get_vbr, @function
get_vbr:
	mov.l	r14,@-r15
	mov	r15,r14
	mov	#13,r1
	mov	r1,r0
	mov	r14,r15
	mov.l	@r15+,r14
	rts	
	nop
	.size	get_vbr, .-get_vbr
	.ident	"GCC: (Sourcery G++ Lite 2011.03-36 for SEI by OZH) 4.5.2"
