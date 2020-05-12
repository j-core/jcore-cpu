/**************
 Initialization
 **************/
.global _testmov3
_testmov3:
 sts.l  pr, @-r15
 mov.l  _pfail, r13 !fail address
 bra    _testgo
 nop
_pfail: .long _fail
_testgo:

/********************
 LDS Rm, CPi_COM
 STS CPi_COM, Rn
 CLDS CPi_Rm, CPi_COM
 CSTS CPi_COM, CPi_Rn
 ********************/

 mov.l   _pram0, r1
 ldc     r1, gbr
! save pit_throttle
/* clds    cpi_r2, cpi_com (4m89) */
 .word   0x4289
/* sts     cpi_com, r0     (4nc8) */
 .word   0x40c8
 mov.l   r0, @(4, gbr)
! body
 mov.l   _p0006ffff, r2
/* lds     r2, cpi_com     (4m88) */
 .word   0x4288
/* csts    cpi_com, cpi_r2 (4nc9) */
 .word   0x42c9
 mov     #0, r0
/* lds     r0, cpi_com     (4m88) */
 .word   0x4088
/* clds    cpi_r2, cpi_com (4m89) */
 .word   0x4289
/* sts     cpi_com, r1     (4nc8) */
 .word   0x41c8
 cmp/eq  r1,r2
 bt     .+6
 jmp     @r13
 nop

/********************
 LDS Rm, CP0_COM
 STS CP0_COM, Rn
 CLDS CP0_Rm, CP0_COM
 CSTS CP0_COM, CP0_Rn
 ********************/

 mov.l   _p00070000, r2
/* lds     r2, cp0_com     (4m5a) */
 .word   0x425a
/* csts    cp0_com, cp0_r2 (fn0d) */
 .word   0xf20d
 mov     #0, r0
/* lds     r0, cp0_com     (4m5a) */
 .word   0x405a
/* clds    cp0_r2, cp0_com (fm1d) */
 .word   0xf21d
/* sts     cp0_com, r1     (0n5a) */
 .word   0x015a
 cmp/eq  r1,r2
#ifdef AAAAA
 bt     .+6
#else
 bt     .+6
#endif
 jmp     @r13
 nop

! restore pit_throttle
 mov.l   @(4, gbr), r0
 mov     r0, r3
/* lds     r3, cp0_com     (4m5a) */
 .word   0x435a
/* csts    cp0_com, cp0_r2 (fn0d) */
 .word   0xf20d

/**************
 Constant Table
 **************/
 bra    _constantend
 nop
.align 4
_pram0    : .long _ram0+128
_p0006ffff: .long 0x0006ffff 
_p00070000: .long 0x00070000 
.align 2
_constantend:

/**************
 Congratulations
 **************/
_pass:
 lds.l  @r15+, pr
 mov.l _ppass_value, r0
 mov.l _ppass_addr, r1
 mov.l r0, @r1
 rts
 nop
.align 4
_ppass_addr: .long 0xABCD0000
_ppass_value: .long 0x00000023

/**********
 You Failed
 **********/
_fail:
 mov.l _pfail_value, r0
 mov.l _pfail_value, r1
 bra   _fail
 nop
.align 4
_pfail_value: .long 0x88888888

.end
