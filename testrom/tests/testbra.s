/**************
 Initialization
 **************/
.global _testbra
_testbra:
 sts.l  pr, @-r15
 mov.l  _pfail, r13 !fail address
 bra    _testgo
 nop
.align 4
_pfail: .long _fail
_testgo:

/***********************
 BRA and load contention
 ***********************/
 mov    #0x8b, r0 
 mov.l  _pram0, r1
 mov.l  r0, @r1

 nop
 bra _bracont
 mov.l  @r1, r2

_bracont:
 mov    #-4, r1
 and    r1, r2

 mov    r2, r0
 cmp/eq #0x88, r0
 bt     .+6
 jmp    @r13
 nop

/************************
 RTS and write contention
 ************************/
 mov.l  _prts_target, r1
 lds    r1, pr
 mov    #0xab, r0

 mov.l  r0, @-r15
 mov.l  r0, @-r15
 mov.l  r0, @-r15
 mov.l  r0, @-r15
 mov.l  r0, @-r15
 mov.l  r0, @-r15
 mov.l  r0, @-r15
 mov.l  r0, @-r15
 sts.l  pr, @-r15

 lds    r0, pr

 lds.l  @r15+, pr
 mov.l  @r15+, r1
 mov.l  @r15+, r2
 mov.l  @r15+, r3
 mov.l  @r15+, r4
 mov.l  @r15+, r5
 mov.l  @r15+, r6
 mov.l  @r15+, r7
 rts
 mov.l  @r15+, r8

.align 4
_prts_target: .long _rts_target

_rts_target:
 mov    #0x12, r8
 mov    r8, r0
 cmp/eq #0x12, r0
 bt     .+6
 jmp    @r13
 nop

/********
 BRA disp
 BSR disp
 ********/
 mov    #126, r0

 bra    _bratarget
 add    #1, r0

_brareturn:
 cmp/eq #0xaa, r0
 bt     .+6
 jmp    @r13
 nop

 mov    #123, r0
 bsr    _bsrtarget
 add    #2, r0

 cmp/eq #0x55, r0
 bt     .+6
 jmp    @r13
 nop

 bra    _endbrabsr
 nop

_bratarget:
 cmp/eq #127, r0
 bt     .+6
 jmp    @r13
 nop
 bra    _brareturn
 mov    #0xaa, r0

_bsrtarget:
 cmp/eq #125, r0
 bt     .+6
 jmp    @r13
 nop
 rts
 mov    #0x55, r0

_endbrabsr:

/*******
 JMP @Rn
 JSR @Rn
 *******/
 mov.l  _pjmptarget, r1
 mov.l  _pjsrtarget, r3
 mov    #126, r0

 jmp    @r1
 add    #1, r0

_jmpreturn:
 cmp/eq #0xaa, r0
 bt     .+6
 jmp    @r13
 nop

 mov    #123, r0
 jsr    @r3
 add    #2, r0

 cmp/eq #0x55, r0
 bt     .+6
 jmp    @r13
 nop

 bra    _endjmpjsr
 nop

_jmptarget:
 cmp/eq #127, r0
 bt     .+6
 jmp    @r13
 nop
 bra    _jmpreturn
 mov    #0xaa, r0

_jsrtarget:
 cmp/eq #125, r0
 bt     .+6
 jmp    @r13
 nop
 rts
 mov    #0x55, r0

.align 4
_pjmptarget: .long _jmptarget
_pjsrtarget: .long _jsrtarget

_endjmpjsr:

/*********
 BT/S disp
 BF/S disp
 *********/
 mov.l  _pram0, r1
 mov    #0xa0, r0
 mov.b  r0, @(0, r1)
 add    #1, r0
 mov.b  r0, @(1, r1)
 add    #1, r0
 mov.b  r0, @(2, r1)
 add    #1, r0
 mov.b  r0, @(3, r1)

 clrt
 bt/s   _btsfail
 mov.b  @(0, r1), r0
 cmp/eq #0xa0, r0
 bf     _btsfail

 clrt
 bf/s   _bts1
 mov.b  @(1, r1), r0
 bra    _btsfail
 nop
_bts1:
 cmp/eq #0xa1, r0
 bf     _btsfail

 sett
 bf/s   _btsfail
 mov.b  @(2, r1), r0
 cmp/eq #0xa2, r0
 bf     _btsfail

 sett
 bt/s   _bts2
 mov.b  @(3, r1), r0
 bra    _btsfail
 nop
_bts2:
 cmp/eq #0xa3, r0
 bt     _btspass
 
_btsfail:
 jmp    @r13
 nop
_btspass:

/*********************
 Branch Subroutine Far
 *********************/
 mov    #(target_bsrf - origin_bsrf), r0
 mov    #0xab, r1
 mov.l  _pram0, r2
 bsrf   r0
 mov.l  r1, @r2
origin_bsrf:
 nop
 nop
 nop
 mov.l  @r2, r0
 cmp/eq #0xac, r0
 bt     .+6
 jmp    @r13
 nop
 bra    _bsrfend
 nop
target_bsrf:
 mov.l  @r2, r1
 add    #1,  r1
 rts
 mov.l  r1,  @r2
_bsrfend:

/**********
 Branch Far
 **********/
 mov    #(target_braf - origin_braf), r0
 mov    #0xab, r1
 mov.l  _pram0, r2
 braf   r0
 mov.l  r1, @r2
origin_braf:
 nop
 nop
 nop
 nop
 nop
 nop
 jmp    @r13
 nop
target_braf:
 mov.l  @r2, r0
 cmp/eq #0xab, r0
 bt     .+6
 jmp    @r13
 nop

/******************************
 Subroutine : Generic Operation
 ******************************/
_subroutine:
 bsr    _subtest
 mov    #0x12, r0
 cmp/eq #0x12, r0
 bt     .+6
 jmp    @r13
 nop
 bra    _subroutineend
 nop
!----
_subtest:
 mov.l  r0, @-r15
 sts.l  pr, @-r15
 bsr    _subtest2
 mov    #0xab, r0
 lds.l  @r15+, pr
 rts
 mov.l  @r15+, r0
!----
_subtest2:
 mov.l  r0, @-r15
 sts.l  pr, @-r15
 mov    #0x88, r0
 lds.l  @r15+, pr
 rts
 mov.l  @r15+, r0
!----
_subroutineend:

/*****************
 Compare and BT/BF
 *****************/
_cmpbtbf:
 mov.l  _p12345678, r0
 mov.l  _p89abcdef, r1
 cmp/eq r1, r0
 bt     .+4
 bf     .+6
 jmp    @r13
 nop
!----
 mov.l  _p12345678, r1
 cmp/eq r1, r0
 bf     .+4
 bt     .+6
 jmp    @r13
 nop

/**************
 Full CMP check
 **************/
_compare:
 mov #0xab, r0
 cmp/eq #0xab, r0 !T=1
 bf     _cmpfail
 cmp/eq #0xac, r0 !T=0
 bt     _cmpfail
!----
 mov.l _p5a5a5a5a, r4
 mov.l _p5a5a5a5a, r5
 cmp/eq r5, r4 !T=1
 bf     _cmpfail
 cmp/eq r4, r5 !T=1
 bf     _cmpfail
 cmp/hs r5, r4 !T=1
 bf     _cmpfail
 cmp/hs r4, r5 !T=1
 bf     _cmpfail
 cmp/ge r5, r4 !T=1
 bf     _cmpfail
 cmp/ge r4, r5 !T=1
 bf     _cmpfail
 cmp/hi r5, r4 !T=0
 bt     _cmpfail
 cmp/hi r4, r5 !T=0
 bt     _cmpfail
 cmp/gt r5, r4 !T=0
 bt     _cmpfail
 cmp/gt r4, r5 !T=0
 bt     _cmpfail
!----
 mov.l _p5a5a5a5a, r4
 mov.l _p4a5a5a5a, r5
 cmp/eq r5, r4 !T=0
 bt     _cmpfail
 cmp/eq r4, r5 !T=0
 bt     _cmpfail
 cmp/hs r5, r4 !T=1
 bf     _cmpfail
 cmp/hs r4, r5 !T=0
 bt     _cmpfail
 cmp/ge r5, r4 !T=1
 bf     _cmpfail
 cmp/ge r4, r5 !T=0
 bt     _cmpfail
 cmp/hi r5, r4 !T=1
 bf     _cmpfail
 cmp/hi r4, r5 !T=0
 bt     _cmpfail
 cmp/gt r5, r4 !T=1
 bf     _cmpfail
 cmp/gt r4, r5 !T=0
 bt     _cmpfail
!----
 mov.l _p5a5a5a5a, r4
 mov.l _paa5a5a5a, r5
 cmp/eq r5, r4 !T=0
 bt     _cmpfail
 cmp/eq r4, r5 !T=0
 bt     _cmpfail
 cmp/hs r5, r4 !T=0
 bt     _cmpfail
 cmp/hs r4, r5 !T=1
 bf     _cmpfail
 cmp/ge r5, r4 !T=1
 bf     _cmpfail
 cmp/ge r4, r5 !T=0
 bt     _cmpfail
 cmp/hi r5, r4 !T=0
 bt     _cmpfail
 cmp/hi r4, r5 !T=1
 bf     _cmpfail
 cmp/gt r5, r4 !T=1
 bf     _cmpfail
 cmp/gt r4, r5 !T=0
 bt     _cmpfail
!----
 mov.l _p89abcdef, r4
 mov.l _paa5a5a5a, r5
 cmp/eq r5, r4 !T=0
 bt     _cmpfail
 cmp/eq r4, r5 !T=0
 bt     _cmpfail
 cmp/hs r5, r4 !T=0
 bt     _cmpfail
 cmp/hs r4, r5 !T=1
 bf     _cmpfail
 cmp/ge r5, r4 !T=0
 bt     _cmpfail
 cmp/ge r4, r5 !T=1
 bf     _cmpfail
 cmp/hi r5, r4 !T=0
 bt     _cmpfail
 cmp/hi r4, r5 !T=1
 bf     _cmpfail
 cmp/gt r5, r4 !T=0
 bt     _cmpfail
 cmp/gt r4, r5 !T=1
 bf     _cmpfail
!----
 sett
 mov.l   _p12345678, r4
 mov.l   _p12abcdef, r5
 cmp/str r5, r4 !T=1
 bf      _cmpfail
 cmp/str r4, r5 !T=1
 bf      _cmpfail
 clrt
 mov.l   _p12345678, r4
 mov.l   _p12abcdef, r5
 cmp/str r5, r4 !T=1
 bf      _cmpfail
 cmp/str r4, r5 !T=1
 bf      _cmpfail
 clrt
 mov.l   _p12345678, r4
 mov.l   _pab34cdef, r5
 cmp/str r5, r4 !T=1
 bf      _cmpfail
 cmp/str r4, r5 !T=1
 bf      _cmpfail
 sett
 mov.l   _p12345678, r4
 mov.l   _pabcd56ef, r5
 cmp/str r5, r4 !T=1
 bf      _cmpfail
 cmp/str r4, r5 !T=1
 bf      _cmpfail
 mov.l   _p12345678, r4
 mov.l   _pabcdef78, r5
 cmp/str r5, r4 !T=1
 bf      _cmpfail
 cmp/str r4, r5 !T=1
 bf      _cmpfail
 clrt
 mov.l   _p12345678, r4
 mov.l   _pabcdef01, r5
 cmp/str r5, r4 !T=0
 bt      _cmpfail
 cmp/str r4, r5 !T=0
 bt      _cmpfail
 sett
 mov.l   _p12345678, r4
 mov.l   _pabcdef01, r5
 cmp/str r5, r4 !T=0
 bt      _cmpfail
 cmp/str r4, r5 !T=0
 bt      _cmpfail
 sett
 mov.l   _pffffffff, r4
 mov.l   _p00000001, r5
 cmp/str r5, r4 !T=0
 bt      _cmpfail
 cmp/str r4, r5 !T=0
 bt      _cmpfail
!----
 mov    #0, r4
 mov    #1, r5
 mov    #-1, r6
 cmp/pz r4 !T=1
 bf     _cmpfail
 cmp/pz r5 !T=1
 bf     _cmpfail
 cmp/pz r6 !T=0
 bt     _cmpfail
 cmp/pl r4 !T=0
 bt     _cmpfail
 cmp/pl r5 !T=1
 bf     _cmpfail
 cmp/pl r6 !T=0
 bt     _cmpfail
!----
 bra   _cmpgood
 nop
_cmpfail:
 jmp   @r13
 nop
_cmpgood:

/**************
 Constant Table
 **************/
 bra    _constantend
 nop
.align 4
_p12345678: .long 0x12345678
_p89abcdef: .long 0x89abcdef
_p5a5a5a5a: .long 0x5a5a5a5a
_paa5a5a5a: .long 0xaa5a5a5a
_p4a5a5a5a: .long 0x4a5a5a5a
_p12abcdef: .long 0x12abcdef
_pab34cdef: .long 0xab34cdef
_pabcd56ef: .long 0xabcd56ef
_pabcdef78: .long 0xabcdef78
_pabcdef01: .long 0xabcdef01
_p00000001: .long 0x00000001
_pffffffff: .long 0xffffffff
_pram0    : .long _ram0+128
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
_ppass_value: .long 0x00000012

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
