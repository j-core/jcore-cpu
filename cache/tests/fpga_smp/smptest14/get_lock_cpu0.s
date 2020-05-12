	.file	"get_lock_cpu0_stub.c" 
        /* modify manually get_lock_cpu0_stub.s -> get_lock_cpu0.s */
	.text
	.text
	.align 1
	.global	get_lock_cpu0
	.type	get_lock_cpu0, @function
get_lock_cpu0:
	mov.l	r14,@-r15
	add	#-8,r15
	mov	r15,r14
	mov	r14,r1
	add	#-56,r1
	mov.l	.L4,r2
	mov.l	r2,@(60,r1)
	mov	r14,r1
	add	#-56,r1
	mov.l	.L5,r2
	mov.l	r2,@(56,r1)
.L1001:
	mov	r14,r1
	add	#-56,r1
	mov.l	@(60,r1),r1
	mov	r1,r0
	mov	#51,r1
	mov	#52,r2
/*	opcode before 2015-09-24 */
/*	CAS	r2,r1,r0 opcode bit 0x3nm1 => 0x3211 => .word 12817 */
/*	opcode after 2015-09-24 */
/*	CAS	r2,r1,r0 opcode bit 0x2nm3 => 0x2213 => .word 8723 */
 	.word	8723
/*	end ofCAS r2,r1,r0 */
/* DEL	tas.b	@r1   load.b from 0x14010020 */
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
	mov	#0,r1  /* return(0) */
	mov	r1,r0  /* return(0) */
	add	#8,r14  /* stack pointer */
	mov	r14,r15
	mov.l	@r15+,r14
	rts	
	nop
.L6:
	.align 2
.L4:
	.long	286326816  /* (test14_adrs11) 286326816 (dec) = 0x11110020 */
                           /* (test14) 335609888 (dec) = 0x14010020 */
.L5:
	.long	286281724  /* (test14_adrs11) 286281724 (dec) = 0x11104ffc */
	                   /* (test14) 335564796 (dec) = 0x14004ffc */
	.size	get_lock_cpu0, .-get_lock_cpu0
	.ident	"GCC: (Sourcery G++ Lite 2011.03-36 for SEI by OZH) 4.5.2"
