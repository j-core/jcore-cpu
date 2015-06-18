/**************
 Initialization
 **************/
.global _testdiv
_testdiv:
 sts.l  pr, @-r15
 mov.l  _pfail, r13 !fail address
 bra    _testgo
 nop
_pfail: .long _fail

_testgo:
 mov.l  _ptestvalue, r9
/******************************************
 Unsigned R1(32bit) / R0(16bit) -> R1(16bit)
 ******************************************/
 mov.l @r9+, r1
 mov.l @r9+, r0
 div0u
 div1 r0, r1
 div1 r0, r1
 div1 r0, r1
 div1 r0, r1
 div1 r0, r1
 div1 r0, r1
 div1 r0, r1
 div1 r0, r1
 div1 r0, r1
 div1 r0, r1
 div1 r0, r1
 div1 r0, r1
 div1 r0, r1
 div1 r0, r1
 div1 r0, r1
 div1 r0, r1
 rotcl r1
 mov.l @r9+, r2
 cmp/eq r2, r1
 bt     .+6
 jmp    @r13
 nop
/**********************************************
 Unsigned R1:R2(64bit) / R0(32bit) -> R2(32bit)
 **********************************************/
 mov.l @r9+, r1
 mov.l @r9+, r2
 mov.l @r9+, r0
 div0u
 rotcl r2
 div1  r0,r1
 rotcl r2
 div1  r0,r1
 rotcl r2
 div1  r0,r1
 rotcl r2
 div1  r0,r1
 rotcl r2
 div1  r0,r1
 rotcl r2
 div1  r0,r1
 rotcl r2
 div1  r0,r1
 rotcl r2
 div1  r0,r1

 rotcl r2
 div1  r0,r1
 rotcl r2
 div1  r0,r1
 rotcl r2
 div1  r0,r1
 rotcl r2
 div1  r0,r1
 rotcl r2
 div1  r0,r1
 rotcl r2
 div1  r0,r1
 rotcl r2
 div1  r0,r1
 rotcl r2
 div1  r0,r1

 rotcl r2
 div1  r0,r1
 rotcl r2
 div1  r0,r1
 rotcl r2
 div1  r0,r1
 rotcl r2
 div1  r0,r1
 rotcl r2
 div1  r0,r1
 rotcl r2
 div1  r0,r1
 rotcl r2
 div1  r0,r1
 rotcl r2
 div1  r0,r1

 rotcl r2
 div1  r0,r1
 rotcl r2
 div1  r0,r1
 rotcl r2
 div1  r0,r1
 rotcl r2
 div1  r0,r1
 rotcl r2
 div1  r0,r1
 rotcl r2
 div1  r0,r1
 rotcl r2
 div1  r0,r1
 rotcl r2
 div1  r0,r1

 rotcl r2
 mov.l @r9+, r3
 cmp/eq r3, r2
 bt     .+6
 jmp    @r13
 nop
/*****************************************
 Signed R1(16bit) / R0(16bit) -> R1(16bit)
 *****************************************/
 mov.l @r9+, r1
 mov.l @r9+, r0
 div0s r0, r1
 div1  r0, r1
 div1  r0, r1
 div1  r0, r1
 div1  r0, r1
 div1  r0, r1
 div1  r0, r1
 div1  r0, r1
 div1  r0, r1
 div1  r0, r1
 div1  r0, r1
 div1  r0, r1
 div1  r0, r1
 div1  r0, r1
 div1  r0, r1
 div1  r0, r1
 div1  r0, r1
 mov.l @r9+, r2
 cmp/eq r2, r1
 bt     .+6
 jmp    @r13
 nop
/*****************************************
 Signed R2(32bit) / R0(32bit) -> R2(32bit)
 *****************************************/
 mov.l @r9+, r1
 mov.l @r9+, r2
 mov.l @r9+, r0
 div0s r0, r1
 rotcl r2
 div1  r0, r1
 rotcl r2
 div1  r0, r1
 rotcl r2
 div1  r0, r1
 rotcl r2
 div1  r0, r1
 rotcl r2
 div1  r0, r1
 rotcl r2
 div1  r0, r1
 rotcl r2
 div1  r0, r1
 rotcl r2
 div1  r0, r1
 
 rotcl r2
 div1  r0, r1
 rotcl r2
 div1  r0, r1
 rotcl r2
 div1  r0, r1
 rotcl r2
 div1  r0, r1
 rotcl r2
 div1  r0, r1
 rotcl r2
 div1  r0, r1
 rotcl r2
 div1  r0, r1
 rotcl r2
 div1  r0, r1

 rotcl r2
 div1  r0, r1
 rotcl r2
 div1  r0, r1
 rotcl r2
 div1  r0, r1
 rotcl r2
 div1  r0, r1
 rotcl r2
 div1  r0, r1
 rotcl r2
 div1  r0, r1
 rotcl r2
 div1  r0, r1
 rotcl r2
 div1  r0, r1

 rotcl r2
 div1  r0, r1
 rotcl r2
 div1  r0, r1
 rotcl r2
 div1  r0, r1
 rotcl r2
 div1  r0, r1
 rotcl r2
 div1  r0, r1
 rotcl r2
 div1  r0, r1
 rotcl r2
 div1  r0, r1
 rotcl r2
 div1  r0, r1

 rotcl r2
 mov.l @r9+, r3
 cmp/eq r3, r2
 bt     .+6
 jmp    @r13
 nop
!----
 bra    _testfinish
 nop
!----
 .align 4
_ptestvalue: .long _testvalue
_testvalue :
!----32 by 16 unsigned
 .long 0x71c638e4
 .long 0xaaaa0000
 .long 0xaaacaaaa
!----64 by 32 unsigned
 .long 0x0b00ea4e
 .long 0x242d2080
 .long 0x9abcdef0
 .long 0x12345678
!----16 by 16 signed
 .long 0xfffffeff !=ffffff00-1
 .long 0x00100000
 .long 0x000ffff7
!----32 by 32 signed
 .long 0xffffffff
 .long 0xdb97530f
 .long 0xfffffffe
 .long 0x12345678

_testfinish:

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
_ppass_value: .long 0x00000051

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
