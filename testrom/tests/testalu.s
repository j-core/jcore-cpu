/**************
 Initialization
 **************/
.global _testalu
_testalu:
 mov.l  _pfail, r13 !fail address
 bra    _testgo
 nop
.align 4
_pfail: .long _fail
_testgo:

/*************
 EXTU.B Rm, Rn
 EXTU.W Rm, Rn
 EXTS.B Rm, Rn
 EXTS.W Rm, Rn
 *************/
 mov.l  _p11223344, r0
 extu.b r0, r2
 extu.w r0, r4
 exts.b r0, r6
 exts.w r0, r8
 mov.l  _p00000044, r1
 mov.l  _p00003344, r3
 mov.l  _p00000044, r5
 mov.l  _p00003344, r7
 cmp/eq r1, r2
 bf     _extfail
 cmp/eq r3, r4
 bf     _extfail
 cmp/eq r5, r6
 bf     _extfail
 cmp/eq r7, r8
 bf     _extfail

 mov.l  _paabbccdd, r0
 extu.b r0, r2
 extu.w r0, r4
 exts.b r0, r6
 exts.w r0, r8
 mov.l  _p000000dd, r1
 mov.l  _p0000ccdd, r3
 mov.l  _pffffffdd, r5
 mov.l  _pffffccdd, r7
 cmp/eq r1, r2
 bf     _extfail
 cmp/eq r3, r4
 bf     _extfail
 cmp/eq r5, r6
 bf     _extfail
 cmp/eq r7, r8
 bf     _extfail

 bra    _extpass
 nop
_extfail:
 jmp    @r13
 nop
 .align 4
_p11223344 : .long 0x11223344
_paabbccdd : .long 0xaabbccdd
_p00000044 : .long 0x00000044
_p00003344 : .long 0x00003344
_p000000dd : .long 0x000000dd
_p0000ccdd : .long 0x0000ccdd
_pffffffdd : .long 0xffffffdd
_pffffccdd : .long 0xffffccdd
_extpass :

/***********
 NEGC Rm, Rn
 ***********/
 clrt             !negate 64bit value
 mov    #0x00, r4 !upper 32bit
 mov    #0x01, r2 !lower 32bit 
 negc   r2, r6
 bt     .+6
 jmp    @r13
 nop     
 negc   r4, r8
 bt     .+6
 jmp    @r13
 nop     
 mov    #0xff, r4
 mov    #0xff, r2
 cmp/eq r8, r4
 bt     .+6
 jmp    @r13
 nop     
 cmp/eq r6, r2
 bt     .+6
 jmp    @r13
 nop
 
 clrt
 mov    #0x00, r2
 negc   r2, r0
 bf     .+6
 jmp    @r13
 nop
 cmp/eq #0x00, r0
 bt     .+6
 jmp    @r13
 nop

/**********
 NEG Rm, Rn
 **********/
 mov    #127, r2
 neg    r2, r0
 cmp/eq #-127, r0
 bt     .+6
 jmp    @r13
 nop     

 mov    #-128, r2
 neg    r2, r0
 mov.l  _p00000080, r4
 cmp/eq r4, r0
 bt     .+6
 jmp    @r13
 nop

 bra    _negpass
 nop
 .align 4
_p00000080 : .long 0x00000080
_negpass :
 
/*************
 SWAP.B Rm, Rn
 SWAP.W Rm, Rn
 *************/
 mov.l  _p00112233, r2
 swap.b r2, r4
 swap.w r2, r6
 mov.l  _p00113322, r8
 mov.l  _p22330011, r10
 cmp/eq r8, r4
 bt     .+6
 jmp    @r13
 nop     
 cmp/eq r10, r6
 bt     .+6
 jmp    @r13
 nop     

/**********
 NOT Rm, Rn
 **********/
 mov    #0xaa, r2
 not    r2, r0
 cmp/eq #0x55, r0
 bt     .+6
 jmp    @r13
 nop     

/*********
 TAS.B @Rn
 *********/
 mov.l  _pram0, r1
 mov    #0x55, r0
 mov.b  r0, @r1
 tas.b  @r1
 bf     .+6
 jmp    @r13
 nop     
 mov.b  @r1, r0
 cmp/eq #0xd5, r0
 bt     .+6
 jmp    @r13
 nop     

 mov    #0x00, r0
 mov.b  r0, @r1
 tas.b  @r1
 bt     .+6
 jmp    @r13
 nop     
 mov.b  @r1, r0
 cmp/eq #0x80, r0
 bt     .+6
 jmp    @r13
 nop     

/*****
 DT Rn
 *****/
 mov    #0, r2
 mov    #10, r6
_loop_dt:
 add    r6, r2
 dt     r6
 bf     _loop_dt
 mov    r2, r0
 cmp/eq #55, r0
 bt     .+6
 jmp    @r13
 nop

/***********
 SUBV Rm, Rn
 ***********/
 mov    #0x7e, r0
 mov    #0x7f, r2
 subv   r2, r0
 bf     .+6
 jmp    @r13
 nop
 cmp/eq #0xff, r0
 bt     .+6
 jmp    @r13
 nop

 mov.l  _p80000000, r0
 mov    #0x01, r2
 subv   r2, r0
 bt     .+6
 jmp    @r13
 nop
 mov.l  _p7fffffff, r2
 cmp/eq r2, r0
 bt     .+6
 jmp    @r13
 nop

 mov.l  _p7fffffff, r0
 mov    #0xff, r2
 subv   r2, r0
 bt     .+6
 jmp    @r13
 nop
 mov.l  _p80000000, r2
 cmp/eq r2, r0
 bt     .+6
 jmp    @r13
 nop

/***********
 SUBC Rm, Rn
 ***********/
 clrt
 mov    #0x01, r0
 mov    #0x02, r1
 subc   r1, r0
 bt     .+6
 jmp    @r13
 nop
 cmp/eq #0xff, r0 
 bt     .+6
 jmp    @r13
 nop

 sett
 mov    #0x04, r0
 mov    #0x02, r1
 subc   r1, r0
 bf     .+6
 jmp    @r13
 nop
 cmp/eq #0x01, r0 
 bt     .+6
 jmp    @r13
 nop

/**********
 SUB Rm, Rn
 **********/
 mov    #86, r0
 mov    #127, r1
 sub    r1, r0
 cmp/eq #(86-127), r0
 bt     .+6
 jmp    @r13
 nop

/************
 ADD #imm, R0
 ************/
 mov    #0x12, r0
 add    #0x34, r0
 cmp/eq #0x46, r0
 bt     .+6
 jmp    @r13
 nop
 add    #1, r0
 cmp/eq #0x47, r0
 bt     .+6
 jmp    @r13
 nop

/***********
 ADDV Rm, Rn
 ***********/
 mov    #0xff, r0
 mov    #0x01, r2
 addv   r2, r0
 bf     .+6
 jmp    @r13
 nop
 cmp/eq #0x00, r0
 bt     .+6
 jmp    @r13
 nop

 mov.l  _p7fffffff, r0
 mov    #0x01, r2
 addv   r2, r0
 bt     .+6
 jmp    @r13
 nop
 mov.l  _p80000000, r2
 cmp/eq r2, r0
 bt     .+6
 jmp    @r13
 nop

 mov.l  _p80000000, r0
 mov    #0xff, r2
 addv   r2, r0
 bt     .+6
 jmp    @r13
 nop
 mov.l  _p7fffffff, r2
 cmp/eq r2, r0
 bt     .+6
 jmp    @r13
 nop

/***********
 ADDC Rm, Rn
 ***********/
 clrt
 mov    #0xff, r0
 mov    #0x01, r1
 addc   r1, r0
 bt     .+6
 jmp    @r13
 nop
 cmp/eq #0x00, r0 
 bt     .+6
 jmp    @r13
 nop

 sett
 mov    #0xfd, r0
 mov    #0x01, r1
 addc   r1, r0
 bf     .+6
 jmp    @r13
 nop
 cmp/eq #0xff, r0 
 bt     .+6
 jmp    @r13
 nop

/**********
 ADD Rm, Rn
 **********/
 mov    #89, r0
 mov    #-128, r1
 add    r1, r0
 cmp/eq #(89-128), r0
 bt     .+6
 jmp    @r13
 nop

/************
 XTRCT Rm, Rn
 ************/
 mov.l  _p00112233, r2
 mov.l  _p44556677, r4
 xtrct  r4, r2
 mov.l  _p66770011, r6
 cmp/eq r6, r2
 bt     .+6
 jmp    @r13
 nop
 
/**********
 XOR Rm, Rn
 **********/
 mov    #0xaa, r0
 mov    #0x55, r2
 xor    r2, r0
 tst    #0x00, r0
 bt     .+6
 jmp    @r13
 nop

 mov    #0xaa, r0 ! 1010
 mov    #0x77, r2 ! 0111
 xor    r2, r0
 cmp/eq #0xdd, r0 ! 1101
 bt     .+6
 jmp    @r13
 nop

/************
 XOR #imm, R0
 ************/

 mov    #0xaa, r0
 xor    #0x55, r0
 cmp/eq #0xff, r0
 bt     .+6
 jmp    @r13
 nop

 mov    #0xaa, r0 ! 1010
 xor    #0x77, r0 ! 0111
 cmp/eq #0xdd, r0 ! 1101
 bt     .+6
 jmp    @r13
 nop

/**********************
 XOR.B #imm, @(R0, GBR)
 **********************/
 mov.l  _pram0, r1
 ldc    r1, gbr

 mov    #0xaa, r0
 mov.b  r0, @(7, r1)
 mov    #7, r0
 xor.b  #0x55, @(r0, gbr)
 mov.b  @(7, r1), r0
 cmp/eq #0xff, r0
 bt     .+6
 jmp    @r13
 nop
!----
 mov    #0xaa, r0
 mov.b  r0, @(7, r1)
 mov    #7, r0
 xor.b  #0x77, @(r0, gbr)
 mov.b  @(7, r1), r0
 cmp/eq #0xdd, r0
 bt     .+6
 jmp    @r13
 nop

/**********
 TST Rm, Rn
 **********/
 mov    #0xaa, r2
 mov    #0x55, r4
 tst    r4, r2
 movt   r0
 cmp/eq #0x01, r0
 bt     .+6
 jmp    @r13
 nop

 mov    #0xaa, r2
 mov    #0x5d, r4
 tst    r4, r2
 movt   r0
 cmp/eq #0x00, r0
 bt     .+6
 jmp    @r13
 nop

/************
 TST #imm, R0
 ************/
 mov    #0xaa, r0
 tst    #0x55, r0
 movt   r0
 cmp/eq #0x01, r0
 bt     .+6
 jmp    @r13
 nop

 mov    #0xaa, r0
 tst    #0xd5, r0
 movt   r0
 cmp/eq #0x00, r0
 bt     .+6
 jmp    @r13
 nop

/**********************
 TST.B #imm, @(R0, GBR)
 **********************/
 mov.l  _pram0, r1
 ldc    r1, gbr

 mov    #0xaa, r0
 mov.b  r0, @(9, r1)

 mov    #9, r0
 clrt
 tst.b  #0x55, @(r0, gbr)
 bt     .+6
 jmp    @r13
 nop

 mov    #0xaa, r0
 mov.b  r0, @(11, r1)

 mov    #11, r0
 sett
 tst.b  #0xd5, @(r0, gbr)
 bf     .+6
 jmp    @r13
 nop
 
/**********
 AND Rm, Rn
 **********/
 mov    #0x00, r0
 mov    #0xff, r1
 and    r1, r0
 cmp/eq #0x00, r0
 bt     .+6
 jmp    @r13
 nop
!----
 mov    #0xaa, r0
 mov    #0x55, r1
 and    r1, r0
 cmp/eq #0x00, r0
 bt     .+6
 jmp    @r13
 nop
!----
 mov    #0x7e, r0 !01111110
 mov    #0xdb, r1 !11011011
 and    r1, r0
 cmp/eq #0x5a, r0 !01011010
 bt     .+6
 jmp    @r13
 nop

/************
 AND #imm, R0
 ************/
 mov    #0x00, r0
 and    #0xff, r0
 cmp/eq #0x00, r0
 bt     .+6
 jmp    @r13
 nop
!----
 mov    #0xaa, r0
 and    #0x55, r0
 cmp/eq #0x00, r0
 bt     .+6
 jmp    @r13
 nop
!----
 mov    #0x7e, r0 !01111110
 and    #0xdb, r0 !11011011
 cmp/eq #0x5a, r0 !01011010
 bt     .+6
 jmp    @r13
 nop

/**********************
 AND.B #imm, @(R0, GBR)
 **********************/
 mov.l  _pram0, r1
 ldc    r1, gbr

 mov    #0x00, r0
 mov.b  r0, @(7, r1)
 mov    #7, r0
 and.b  #0xff, @(r0, gbr)
 mov.b  @(7, r1), r0
 cmp/eq #0x00, r0
 bt     .+6
 jmp    @r13
 nop
!----
 mov    #0xaa, r0
 mov.b  r0, @(7, r1)
 mov    #7, r0
 and.b  #0x55, @(r0, gbr)
 mov.b  @(7, r1), r0
 cmp/eq #0x00, r0
 bt     .+6
 jmp    @r13
 nop
!----
 mov    #0x7e, r0         !01111110
 mov.b  r0, @(7, r1)
 mov    #7, r0
 and.b  #0xdb, @(r0, gbr) !11011011
 mov.b  @(7, r1), r0
 cmp/eq #0x5a, r0         !01011010
 bt     .+6
 jmp    @r13
 nop

/*********
 OR Rm, Rn
 *********/
 mov    #0x00, r0
 mov    #0xff, r1
 or     r1, r0
 cmp/eq #0xff, r0
 bt     .+6
 jmp    @r13
 nop
!----
 mov    #0xaa, r0
 mov    #0x55, r1
 or     r1, r0
 cmp/eq #0xff, r0
 bt     .+6
 jmp    @r13
 nop
!----
 mov    #0x55, r0 !01010101
 mov    #0x5a, r1 !01011010
 or     r1, r0
 cmp/eq #0x5f, r0 !01011111
 bt     .+6
 jmp    @r13
 nop

/***********
 OR #imm, R0
 ***********/
 mov    #0x00, r0
 or     #0xff, r0
 mov.w  _p00ff, r2
 cmp/eq r2, r0
 bt     .+6
 jmp    @r13
 nop
!----
 mov    #0xaa, r0
 or     #0x55, r0
 mov.w  _pffff, r2
 cmp/eq r2, r0
 bt     .+6
 jmp    @r13
 nop
!----
 mov    #0x55, r0 !01010101
 or     #0x5a, r0 !01011010
 cmp/eq #0x5f, r0 !01011111
 bt     .+6
 jmp    @r13
 nop

/*********************
 OR.B #imm, @(R0, GBR)
 *********************/
 mov.l  _pram0, r1
 ldc    r1, gbr

 mov    #0x00, r0
 mov.b  r0, @(7, r1)
 mov    #7, r0
 or.b  #0xff, @(r0, gbr)
 mov.b  @(7, r1), r0
 cmp/eq #0xff, r0
 bt     .+6
 jmp    @r13
 nop
!----
 mov    #0xaa, r0
 mov.b  r0, @(7, r1)
 mov    #7, r0
 or.b  #0x55, @(r0, gbr)
 mov.b  @(7, r1), r0
 cmp/eq #0xff, r0
 bt     .+6
 jmp    @r13
 nop
!----
 mov    #0x55, r0
 mov.b  r0, @(7, r1)
 mov    #7, r0
 or.b  #0x5a, @(r0, gbr)
 mov.b  @(7, r1), r0
 cmp/eq #0x5f, r0
 bt     .+6
 jmp    @r13
 nop

/********
 CLRT
 SETT
 MOVT Rn
 ********/
 sett
 movt   r0
 cmp/eq #0x01, r0
 bt     .+6
 jmp    @r13
 nop
 clrt
 movt   r0
 cmp/eq #0x00, r0
 bt     .+6
 jmp    @r13
 nop 

/**************
 Constant Table
 **************/
 bra    _constantend
 nop
.align 4
_pram0     : .long _ram0+128
_p7fffffff : .long 0x7fffffff
_p80000000 : .long 0x80000000
_p00112233 : .long 0x00112233
_p44556677 : .long 0x44556677
_p66770011 : .long 0x66770011
_p00113322 : .long 0x00113322
_p22330011 : .long 0x22330011
.align 2
_p00ff: .word 0x00ff
_pffff: .word 0xffff
.align 2
_constantend:

/**************
 Congratulations
 **************/
_pass:
 mov.l _ppass_value, r0
 mov.l _ppass_addr, r1
 mov.l r0, @r1
 rts
 nop
.align 4
_ppass_addr: .long 0xABCD0000
_ppass_value: .long 0x00000031

/**********
 You Failed
 **********/
_fail:
 mov.l _pfail_value, r0
 mov.l _pfail_value, r1
 bra _fail
 nop
.align 4
_pfail_value: .long 0x88888888

.end


