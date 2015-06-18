/**************
 Initialization
 **************/
.global _testmulu
_testmulu:
 sts.l  pr, @-r15
 mov.l  _pfail, r13 !fail address
 bra    _testgo
 nop
_pfail: .long _fail
_testgo:

/************************
 MULU.L Rm, Rn
 ************************/
 mov.l  _ptestvalue1, r1
 mov.l  _ptestvalue2, r2
 mov    #0xff, r0
 lds    r0, mach

 mulu.w r2, r1
 mulu.w r1, r2
 mulu.w r2, r1
 mulu.w r1, r2 !You should check mult contention,here. 

_testloop:
 mov.l  @r1+, r3
 mov.l  @r1+, r4
 mov.l  @r1+, r5
 mov.l  @r1+, r6

 mulu.w r4, r3

 sts    mach, r3 !You should check mult contention,here.
 sts    macl, r4

!----
 cmp/eq r5, r3
 bt     .+6
 jmp    @r13
 nop
!----
 cmp/eq r6, r4
 bt     .+6
 jmp    @r13
 nop
!----
 cmp/eq r2, r1
 bf     _testloop
 bra    _testfinish
 nop
!----
 .align 4
_ptestvalue1: .long _testvalue1
_ptestvalue2: .long _testvalue2

 .align 4
_testvalue1:
 .long  0xffff0002 !Rn
 .long  0xffff0003 !Rm
 .long  0xffffffff !MACH
 .long  0x00000006 !MACL

 .long  0xffff1234
 .long  0x00009abc
 .long  0xffffffff
 .long  0x0b00a630

 .long  0xffff0001
 .long  0x0000ffff
 .long  0xffffffff
 .long  0x0000ffff

 .long  0x0000ffff
 .long  0xffff0001
 .long  0xffffffff
 .long  0x0000ffff

 .long  0xffff7fff
 .long  0x00008000
 .long  0xffffffff
 .long  0x3fff8000

 .long  0x00008000
 .long  0xffff7fff
 .long  0xffffffff
 .long  0x3fff8000

 .long  0x1234ffff
 .long  0x5678ffff
 .long  0xffffffff
 .long  0xfffe0001

 .long  0x9abc7fff
 .long  0x9abc7fff
 .long  0xffffffff
 .long  0x3fff0001

 .long  0x12348000
 .long  0x12348000
 .long  0xffffffff
 .long  0x40000000
_testvalue2:

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
_ppass_value: .long 0x00000042

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
