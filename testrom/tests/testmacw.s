/**************
 Initialization
 **************/
.global _testmacw
_testmacw:
 sts.l  pr, @-r15
 mov.l  _pfail, r13 !fail address
 bra    _testgo
 nop
_pfail: .long _fail
_testgo:

/************************
 MAC.W @Rm+, @Rn+ : basic
 ************************/
_macw:
 mov    #0, r0
 ldc    r0, sr !S=0
 mov    #0x02, r0 
 clrmac
 mov.l  _pmacwdata1, r1
 mov.l  _pmacwdata2, r2
 mac.w  @r2+, @r1+
 ldc    r0, sr !S=1, no effect to MAC operation
 sts    mach, r3
 sts    macl, r4
 mov.l  _pmacwdata3, r5
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
 mac.w  @r2+, @r1+
 clrmac !only check clear timing
!----
 mac.w  @r2+, @r1+
 mac.w  @r2+, @r1+
 mac.w  @r2+, @r1+
 mac.w  @r2+, @r1+
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
 bra    _macwend
 nop
 .align 4
_macwdata1:
 .word 0x1234 !4660
 .word 0xfffd !dummy (-3)
 .word 0x0002
 .word 0x0003
 .word 0x0004
 .word 0x0005
_macwdata2:
 .word 0xabcd !-21555
 .word 0x0002 !dummy (2)
 .word 0x0006
 .word 0x0007
 .word 0x0008
 .word 0x0009
_macwdata3:
 .long 0xFFFFFFFF !-100446300
 .long 0xFA034FA4
 .long 0x00000000 !(2x6)+(3x7)+(4x8)+(5x9)=12+21+28+45=110
 .long 0x0000006e
.align 4
_pmacwdata1: .long _macwdata1
_pmacwdata2: .long _macwdata2
_pmacwdata3: .long _macwdata3
_macwend:

/***********************************
 MAC.W @Rm+, @Rn+ : value dependency
 ***********************************/
_macw_value:
 mov.l _pmacwsbit, r1
 mov.l _pmacwinih, r2
 mov.l _pmacwinil, r3
 mov.l _pmacwval1, r4
 mov.l _pmacwval2, r5
 mov.l _pmacwmach, r6
 mov.l _pmacwmacl, r7
 mov.l _pmacwcount, r8

_mac_value1:
 mov.l @r1+, r0
 ldc   r0, sr
 mov.l @r2+, r0
 lds   r0, mach
 mov.l @r3+, r0
 lds   r0, macl
 mac.w @r5+, @r4+
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
 bra    _macw_value_end:
 nop

 .align 4
!-----------------------------
! S=0 0000 x 0000

! S=0 0001 x 7fff
! S=0 7fff x 0001
! S=0 ffff x 7fff
! S=0 7fff x ffff

! S=0 0001 x 8000
! S=0 8000 x 0001
! S=0 ffff x 8000
! S=0 8000 x ffff

! S=0 7fff x 7fff
! S=0 8000 x 8000
! S=1 7fff x 7fff
! S=1 8000 x 8000

! S=0 7fff x 8000
! S=0 8000 x 7fff
! S=1 7fff x 8000
! S=1 8000 x 7fff

! S=0 0001 x 0001 + 00000000:7fffffff
! S=0 0001 x ffff + ffffffff:80000000

! S=1 0001 x 0001 + 00000000:7fffffff
! S=1 0001 x ffff + ffffffff:80000000

! S=1 0001 x 0001 + 00000006:7fffffff
! S=1 0001 x ffff + 0000000a:80000000

_macwsbit: ! R1
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

_macwinih: ! R2
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

 .long 0x00000000
 .long 0xffffffff

 .long 0x00000000
 .long 0xffffffff

 .long 0x00000006
 .long 0x0000000a

_macwinil: ! R3
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

 .long 0x7fffffff
 .long 0x80000000

 .long 0x7fffffff
 .long 0x80000000

 .long 0x7fffffff
 .long 0x80000000

_macwval1: ! R4
 .word 0x0000

 .word 0x0001
 .word 0x7fff
 .word 0xffff
 .word 0x7fff

 .word 0x0001
 .word 0x8000
 .word 0xffff
 .word 0x8000

 .word 0x7fff
 .word 0x8000
 .word 0x7fff
 .word 0x8000

 .word 0x7fff
 .word 0x8000
 .word 0x7fff
 .word 0x8000

 .word 0x0001
 .word 0x0001

 .word 0x0001
 .word 0x0001

 .word 0x0001
 .word 0x0001

_macwval2: ! R5
 .word 0x0000

 .word 0x7fff
 .word 0x0001
 .word 0x7fff
 .word 0xffff

 .word 0x8000
 .word 0x0001
 .word 0x8000
 .word 0xffff

 .word 0x7fff
 .word 0x8000
 .word 0x7fff
 .word 0x8000

 .word 0x8000
 .word 0x7fff
 .word 0x8000
 .word 0x7fff

 .word 0x0001
 .word 0xffff

 .word 0x0001
 .word 0xffff

 .word 0x0001
 .word 0xffff

_macwmach: ! R6
 .long 0x00000000

 .long 0x00000000
 .long 0x00000000
 .long 0xffffffff
 .long 0xffffffff

 .long 0xffffffff
 .long 0xffffffff
 .long 0x00000000
 .long 0x00000000

 .long 0x00000000
 .long 0x00000000
 .long 0x00000000
 .long 0x00000000

 .long 0xffffffff
 .long 0xffffffff
 .long 0x00000000
 .long 0x00000000

 .long 0x00000000
 .long 0xffffffff

 .long 0x00000001
 .long 0xffffffff

 .long 0x00000007
 .long 0x0000000b

_macwmacl: ! R7
 .long 0x00000000
 
 .long 0x00007fff
 .long 0x00007fff
 .long 0xffff8001
 .long 0xffff8001

 .long 0xffff8000
 .long 0xffff8000
 .long 0x00008000
 .long 0x00008000

 .long 0x3fff0001
 .long 0x40000000
 .long 0x3fff0001
 .long 0x40000000

 .long 0xc0008000
 .long 0xc0008000
 .long 0xc0008000
 .long 0xc0008000

 .long 0x80000000
 .long 0x7fffffff

 .long 0x7fffffff
 .long 0x80000000

 .long 0x7fffffff
 .long 0x80000000

!-----------------------------
.align 4
_pmacwsbit: .long _macwsbit
_pmacwinih: .long _macwinih
_pmacwinil: .long _macwinil
_pmacwval1: .long _macwval1
_pmacwval2: .long _macwval2
_pmacwmach: .long _macwmach
_pmacwmacl: .long _macwmacl
_pmacwcount: .long (_macwinih - _macwsbit)/4
_macw_value_end:

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
_ppass_value: .long 0x00000061

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
