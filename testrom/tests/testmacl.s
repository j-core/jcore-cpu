/**************
 Initialization
 **************/
.global _testmacl
_testmacl:
 sts.l  pr, @-r15
 mov.l  _pfail, r13 !fail address
 bra    _testgo
 nop
_pfail: .long _fail
_testgo:

/************************
 MAC.L @Rm+, @Rn+ : basic
 ************************/
_macl:
 mov    #0, r0
 ldc    r0, sr !S=0
 mov    #0x02, r0 
 clrmac
 mov.l  _pmacldata1, r1
 mov.l  _pmacldata2, r2
 mac.l  @r2+, @r1+
 ldc    r0, sr !S=1, no effect to MAC operation
 sts    mach, r3
 sts    macl, r4
 mov.l  _pmacldata3, r5
 mov.l  @r5+, r6
 mov.l  @r5+, r7
!----
 cmp/eq r6, r3
 bt     .+6
 jmp    @r13
 nop
!----
 cmp/eq r7, r4
 bt     .+6
 jmp    @r13
 nop
!----
 mov    #0, r0
 ldc    r0, sr !S=0
 mac.l  @r2+, @r1+
 clrmac !only check clear timing
!----
 mac.l  @r2+, @r1+
 mac.l  @r2+, @r1+
 mac.l  @r2+, @r1+
 mac.l  @r2+, @r1+
 sts    macl, r4
 sts    mach, r3
 mov.l  @r5+, r6
 mov.l  @r5+, r7
!----
 cmp/eq r6, r3
 bt     .+6
 jmp    @r13
 nop
!----
 cmp/eq r7, r4
 bt     .+6
 jmp    @r13
 nop
!----
 bra    _maclend
 nop
 .align 4
_macldata1:
 .long 0x01234567 !19088743
 .long 0xfffffffd !dummy (-3)
 .long 0x00000002
 .long 0x00000003
 .long 0x00000004
 .long 0x00000005
_macldata2:
 .long 0x89abcdef !-1985229329
 .long 0x00000002 !dummy (2)
 .long 0x00000006
 .long 0x00000007
 .long 0x00000008
 .long 0x00000009
_macldata3:
 .long 0xff795e36 !-37895532457343447
 .long 0xc94e4629
 .long 0x00000000 !(2x6)+(3x7)+(4x8)+(5x9)=12+21+28+45=110
 .long 0x0000006e
.align 4
_pmacldata1: .long _macldata1
_pmacldata2: .long _macldata2
_pmacldata3: .long _macldata3
_maclend:

/***********************************
 MAC.L @Rm+, @Rn+ : value dependency
 ***********************************/
_macl_value:
 mov.l _pmaclsbit, r1
 mov.l _pmaclinih, r2
 mov.l _pmaclinil, r3
 mov.l _pmaclval1, r4
 mov.l _pmaclval2, r5
 mov.l _pmaclmach, r6
 mov.l _pmaclmacl, r7
 mov.l _pmaclcount, r8

_mac_value1:
 mov.l @r1+, r0
 ldc   r0, sr
 mov.l @r2+, r0
 lds   r0, mach
 mov.l @r3+, r0
 lds   r0, macl
 mac.l @r5+, @r4+
 sts   mach, r9
 sts   macl, r10
!----
 mov.l @r6+, r0
 cmp/eq r0, r9
 bt     .+6
 jmp    @r13
 nop
!----
 mov.l @r7+, r0
 cmp/eq r0, r10
 bt     .+6
 jmp    @r13
 nop
!----
 add   #-1, r8
 cmp/pl r8
 bt    _mac_value1
!----
 bra    _macl_value_end:
 nop

 .align 4
!-----------------------------
! S=0 00000000 x 00000000

! S=0 00000001 x 7fffffff
! S=0 7fffffff x 00000001
! S=0 ffffffff x 7fffffff
! S=0 7fffffff x ffffffff

! S=0 00000001 x 80000000
! S=0 80000000 x 00000001
! S=0 ffffffff x 80000000
! S=0 80000000 x ffffffff

! S=0 7fffffff x 7fffffff
! S=0 80000000 x 80000000
! S=1 7fffffff x 7fffffff
! S=1 80000000 x 80000000

! S=0 7fffffff x 80000000
! S=0 80000000 x 7fffffff
! S=1 7fffffff x 80000000
! S=1 80000000 x 7fffffff

! S=0 00000001 x 00000001 + 00007fff:ffffffff
! S=0 00000001 x ffffffff + ffff8000:00000000

! S=1 00000001 x 00000001 + 00007fff:ffffffff
! S=1 00000001 x ffffffff + ffff8000:00000000

! S=1 00000001 x 00000001 + 0007ffff:ffffffff
! S=1 00000001 x ffffffff + fff80000:00000000

_maclsbit: ! R1
 .long 0x00000000 ! S=0

 .long 0x00000000 ! S=0
 .long 0x00000000 ! S=0
 .long 0x00000000 ! S=0
 .long 0x00000000 ! S=0

 .long 0x00000000 ! S=0
 .long 0x00000000 ! S=0
 .long 0x00000000 ! S=0
 .long 0x00000000 ! S=0

 .long 0x00000000 ! S=0
 .long 0x00000000 ! S=0
 .long 0x00000002 ! S=1
 .long 0x00000002 ! S=1

 .long 0x00000000 ! S=0
 .long 0x00000000 ! S=0
 .long 0x00000002 ! S=1
 .long 0x00000002 ! S=1

 .long 0x00000000 ! S=0
 .long 0x00000000 ! S=0

 .long 0x00000002 ! S=1
 .long 0x00000002 ! S=1

 .long 0x00000002 ! S=1
 .long 0x00000002 ! S=1

_maclinih: ! R2
 .long 0x00000000

 .long 0x00000000
 .long 0x00000000
 .long 0x00000000
 .long 0x00000000

 .long 0x00000000
 .long 0x00000000
 .long 0x00000000
 .long 0x00000000

 .long 0x00000000
 .long 0x00000000
 .long 0x00000000
 .long 0x00000000

 .long 0x00000000
 .long 0x00000000
 .long 0x00000000
 .long 0x00000000

 .long 0x00007fff
 .long 0xffff8000

 .long 0x00007fff
 .long 0xffff8000

 .long 0x0007ffff
 .long 0xfff80000

_maclinil: ! R3
 .long 0x00000000

 .long 0x00000000
 .long 0x00000000
 .long 0x00000000
 .long 0x00000000

 .long 0x00000000
 .long 0x00000000
 .long 0x00000000
 .long 0x00000000

 .long 0x00000000
 .long 0x00000000
 .long 0x00000000
 .long 0x00000000

 .long 0x00000000
 .long 0x00000000
 .long 0x00000000
 .long 0x00000000

 .long 0xffffffff
 .long 0x00000000

 .long 0xffffffff
 .long 0x00000000

 .long 0xffffffff
 .long 0x00000000

_maclval1: ! R4
 .long 0x00000000

 .long 0x00000001
 .long 0x7fffffff
 .long 0xffffffff
 .long 0x7fffffff

 .long 0x00000001
 .long 0x80000000
 .long 0xffffffff
 .long 0x80000000

 .long 0x7fffffff
 .long 0x80000000
 .long 0x7fffffff
 .long 0x80000000

 .long 0x7fffffff
 .long 0x80000000
 .long 0x7fffffff
 .long 0x80000000

 .long 0x00000001
 .long 0x00000001

 .long 0x00000001
 .long 0x00000001

 .long 0x00000001
 .long 0x00000001

_maclval2: ! R5
 .long 0x00000000

 .long 0x7fffffff
 .long 0x00000001
 .long 0x7fffffff
 .long 0xffffffff

 .long 0x80000000
 .long 0x00000001
 .long 0x80000000
 .long 0xffffffff

 .long 0x7fffffff
 .long 0x80000000
 .long 0x7fffffff
 .long 0x80000000

 .long 0x80000000
 .long 0x7fffffff
 .long 0x80000000
 .long 0x7fffffff

 .long 0x00000001
 .long 0xffffffff

 .long 0x00000001
 .long 0xffffffff

 .long 0x00000001
 .long 0xffffffff

_maclmach: ! R6
 .long 0x00000000

 .long 0x00000000
 .long 0x00000000
 .long 0xffffffff
 .long 0xffffffff

 .long 0xffffffff
 .long 0xffffffff
 .long 0x00000000
 .long 0x00000000

 .long 0x3fffffff
 .long 0x40000000
 .long 0x00007fff
 .long 0x00007fff

 .long 0xc0000000
 .long 0xc0000000
 .long 0xffff8000
 .long 0xffff8000

 .long 0x00008000
 .long 0xffff7fff

 .long 0x00007fff
 .long 0xffff8000

 .long 0x00007fff
 .long 0xffff8000

_maclmacl: ! R7
 .long 0x00000000
 
 .long 0x7fffffff
 .long 0x7fffffff
 .long 0x80000001
 .long 0x80000001

 .long 0x80000000
 .long 0x80000000
 .long 0x80000000
 .long 0x80000000

 .long 0x00000001
 .long 0x00000000
 .long 0xffffffff
 .long 0xffffffff

 .long 0x80000000
 .long 0x80000000
 .long 0x00000000
 .long 0x00000000

 .long 0x00000000
 .long 0xffffffff

 .long 0xffffffff
 .long 0x00000000

 .long 0xffffffff
 .long 0x00000000

!-----------------------------
.align 4
_pmaclsbit: .long _maclsbit
_pmaclinih: .long _maclinih
_pmaclinil: .long _maclinil
_pmaclval1: .long _maclval1
_pmaclval2: .long _maclval2
_pmaclmach: .long _maclmach
_pmaclmacl: .long _maclmacl
_pmaclcount: .long (_maclinih - _maclsbit)/4
_macl_value_end:

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
_ppass_value: .long 0x00000062

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
