/**************
 Initialization
 **************/
.global _testmulconf
_testmulconf:
 sts.l  pr, @-r15
 mov.l  _pfail, r13 !fail address
 bra    _testgo
 nop
_pfail: .long _fail
_testgo:

/**********************************
 Conflict Combinations
 ----------------------------------
     before : LDS.L(H) 1  done
              LDS.L(L) 2  done
              STS.L(H) 3  done
              STS.L(L) 4  done
              MAC.W    5
              MAC.L    6
              DMULS.L  7
              DMULU.L  8
              MUL.L    9
              MULS.W   A
              MULU.W   B
 ---------------before=123456789AB
     after  : STS      vvvvv
              STS.L    vvvvv
              CLRMAC   vvvv
              LDS      vvvv
              LDS.L    vvvv
              MAC.W    vvvv
              MAC.L    vvvv
              DMULS.L  vvvv
              DMULU.L  vvvv
              MUL.L    vvvv
              MULS.W   vvvv
              MULU.W   vvvv
 **********************************/

!---------------------
!===[5]===============
!---------------------
 mov.l  _pram20, r4
 mov.l  _pram21, r5
 mov    #3, r0
 mov.w  r0, @r4
 add    #2, r4
 mov    #6, r0
 mov.w  r0, @r4
 add    #-2, r4
 clrmac
 mov    #0xff, r0
 lds    r0, mach   ! MAC=FFFFFFFF 00000000
 mac.w  @r4+, @r4+ ! <--------
 sts.l  mach, @-r5 ! <--------
 mov.l  @r5, r0
 cmp/eq #0xff,  r0
 bt     .+6
 jmp    @r13
 nop
 sts    macl, r0
 cmp/eq #18, r0
 bt     .+6
 jmp    @r13
 nop
!----
 mov.l  _pram20, r4
 mov.l  _pram21, r5
 mov    #4, r0
 mov.w  r0, @r4
 add    #2, r4
 mov    #8, r0
 mov.w  r0, @r4
 add    #-2, r4
 clrmac
 mov    #0xff, r0
 lds    r0, mach   ! MAC=FFFFFFFF 00000000
 mac.w  @r4+, @r4+ ! <--------
 sts.l  macl, @-r5 ! <--------
 mov.l  @r5, r0
 cmp/eq #32,  r0
 bt     .+6
 jmp    @r13
 nop
 sts    mach, r0
 cmp/eq #0xff, r0
 bt     .+6
 jmp    @r13
 nop
!---------------------
 mov.l  _pram20, r4
 mov    #4, r0
 mov.w  r0, @r4
 add    #2, r4
 mov    #6, r0
 mov.w  r0, @r4
 add    #-2, r4
 clrmac
 mac.w  @r4+, @r4+ ! <--------
 sts    mach, r0   ! <--------
 cmp/eq #0,  r0
 bt     .+6
 jmp    @r13
 nop
 sts    macl, r0
 cmp/eq #24, r0
 bt     .+6
 jmp    @r13
 nop
!----
 mov.l  _pram20, r4
 mov    #7, r0
 mov.w  r0, @r4
 add    #2, r4
 mov    #8, r0
 mov.w  r0, @r4
 add    #-2, r4
 clrmac
 mac.w  @r4+, @r4+ ! <--------
 sts    macl, r0   ! <--------
 cmp/eq #56,  r0
 bt     .+6
 jmp    @r13
 nop
 sts    mach, r0
 cmp/eq #0, r0
 bt     .+6
 jmp    @r13
 nop
!---------------------
 bra    skip3
 nop
 .align 4
_pram20:   .long _ram0+128
_pram21:   .long _ram0+128+0x10
skip3:
!---------------------
!===[4]===============
!---------------------
 mov    #0x55,  r0
 mov.l  _pram10, r4
 add    #4,  r4
 mov    #8,  r1
 mov    #9,  r2
 lds    r0,   mach
 lds    r0,   macl
 sts.l  macl, @-r4 ! <-------- 
 mulu.w r2, r1     ! <--------
 sts    mach, r0
 cmp/eq #0x55,  r0
 bt     .+6
 jmp    @r13
 nop
 sts    macl, r0
 cmp/eq #72,  r0
 bt     .+6
 jmp    @r13
 nop
!---------------------
 mov    #0xaa,  r0
 mov.l  _pram10, r4
 add    #4,  r4
 mov    #7,  r1
 mov    #-8,  r2
 lds    r0,   mach
 lds    r0,   macl
 sts.l  macl, @-r4 ! <-------- 
 muls.w r2, r1     ! <--------
 sts    mach, r0
 cmp/eq #0xaa,  r0
 bt     .+6
 jmp    @r13
 nop
 sts    macl, r0
 cmp/eq #-56,  r0
 bt     .+6
 jmp    @r13
 nop
!---------------------
 mov    #0x55,  r0
 mov.l  _pram10, r4
 add    #4,  r4
 mov    #6,  r1
 mov    #7,  r2
 lds    r0,   mach
 lds    r0,   macl
 sts.l  macl, @-r4 ! <-------- 
 mul.l  r2, r1     ! <--------
 sts    mach, r0
 cmp/eq #0x55,  r0
 bt     .+6
 jmp    @r13
 nop
 sts    macl, r0
 cmp/eq #42,  r0
 bt     .+6
 jmp    @r13
 nop
!---------------------
 mov    #0xaa,  r0
 mov.l  _pram10, r4
 mov    #5,  r1
 mov    #6,  r2
 lds    r0,   mach
 lds    r0,   macl
 sts.l  macl, @-r4 ! <-------- 
 dmulu.l r2, r1    ! <--------
 sts    mach, r0
 cmp/eq #0x00,  r0
 bt     .+6
 jmp    @r13
 nop
 sts    macl, r0
 cmp/eq #30,  r0
 bt     .+6
 jmp    @r13
 nop
!---------------------
 mov    #0xaa,  r0
 mov.l  _pram10, r4
 mov    #5,  r1
 mov    #-6,  r2
 lds    r0,   mach
 lds    r0,   macl
 sts.l  macl, @-r4 ! <-------- 
 dmuls.l r2, r1    ! <--------
 sts    mach, r0
 cmp/eq #0xff,  r0
 bt     .+6
 jmp    @r13
 nop
 sts    macl, r0
 cmp/eq #-30,  r0
 bt     .+6
 jmp    @r13
 nop
!---------------------
 mov    #0xff,  r1
 mov.l  _pram10, r4
 add    #4, r4
 mov.l  r1, @r4
 lds    r1, mach
 mov    #0, r0     ! S=0
 ldc    r0, sr
 lds    r1, macl   ! MAC=ffffffff ffffffff
 sts.l  macl, @-r4 ! <--------
 mac.l  @r4+, @r4+ ! <--------
 sts    mach, r0   ! MAC=00000000 00000000
 cmp/eq #0x00,  r0
 bt     .+6
 jmp    @r13
 nop
 sts    macl, r0
 cmp/eq #0x00,  r0
 bt     .+6
 jmp    @r13
 nop
!---------------------
 mov    #0xff,  r1
 mov.l  _pram10, r4
 add    #4,  r4
 lds    r1, mach
 mov    #0, r0     ! S=0
 ldc    r0, sr
 lds    r1, macl   ! MAC=ffffffff ffffffff
 sts.l  macl, @-r4 ! <--------
 mac.w  @r4+, @r4+ ! <--------
 sts    mach, r0   ! MAC=00000000 00000000
 cmp/eq #0x00,  r0
 bt     .+6
 jmp    @r13
 nop
 sts    macl, r0
 cmp/eq #0x00,  r0
 bt     .+6
 jmp    @r13
 nop
!---------------------
 mov    #0xaa,   r0
 mov.l  _pram10, r4
 add    #4,   r4
 lds    r0,   macl
 mov    r4,   r5
 mov    #0x55, r1
 mov.l  r1,   @r5
 sts.l  macl, @-r4 ! <--------
 lds.l  @r5+, macl ! <--------
 sts    macl,  r0
 cmp/eq #0x55, r0
 bt     .+6
 jmp    @r13
 nop
 mov.l  @r4+,  r0
 cmp/eq #0xaa, r0
 bt     .+6
 jmp    @r13
 nop
 mov.l  @r4+,  r0
 cmp/eq #0x55, r0
 bt     .+6
 jmp    @r13
 nop
!---------------------
 mov    #0xaa,   r0
 mov    #0x55,   r1
 mov.l  _pram10, r4
 add    #4,   r4
 lds    r0,   macl
 sts.l  macl, @-r4 ! <--------
 lds    r1,   macl ! <--------
 sts    macl, r0
 cmp/eq #0x55, r0
 bt     .+6
 jmp    @r13
 nop
 mov.l  @r4+,  r0
 cmp/eq #0xaa, r0
 bt     .+6
 jmp    @r13
 nop
!---------------------
 mov    #0x55,   r0
 mov.l  _pram10, r4
 add    #4,   r4
 lds    r0,   macl
 sts.l  macl, @-r4 ! <--------
 clrmac            ! <--------
 sts    macl,  r0
 cmp/eq #0x00, r0
 bt     .+6
 jmp    @r13
 nop
 mov.l  @r4+,  r0
 cmp/eq #0x55, r0
 bt     .+6
 jmp    @r13
 nop
!---------------------
 mov    #0xaa,   r0
 mov.l  _pram10, r4
 add    #8,   r4
 lds    r0,   macl
 sts.l  macl, @-r4 ! <--------
 sts.l  macl, @-r4 ! <--------
 sts    macl, r1
 cmp/eq r1, r0
 bt     .+6
 jmp    @r13
 nop
 mov.l  @r4+, r2
 cmp/eq r2, r0
 bt     .+6
 jmp    @r13
 nop
 mov.l  @r4+, r3
 cmp/eq r3, r0
 bt     .+6
 jmp    @r13
 nop
!---------------------
 mov    #0xaa,   r0
 mov.l  _pram10, r4
 add    #4,   r4
 lds    r0,   macl
 sts.l  macl, @-r4 ! <--------
 sts    macl, r1   ! <--------
 cmp/eq r1, r0
 bt     .+6
 jmp    @r13
 nop
 mov.l  @r4,  r2
 cmp/eq r2, r0
 bt     .+6
 jmp    @r13
 nop
!---------------------
!===[3]===============
!---------------------
 mov    #0x55,  r0
 mov.l  _pram10, r4
 add    #4,  r4
 mov    #8,  r1
 mov    #9,  r2
 lds    r0,   mach
 sts.l  mach, @-r4 ! <-------- 
 mulu.w r2, r1     ! <--------
 sts    mach, r0
 cmp/eq #0x55,  r0
 bt     .+6
 jmp    @r13
 nop
 sts    macl, r0
 cmp/eq #72,  r0
 bt     .+6
 jmp    @r13
 nop
!---------------------
 mov    #0xaa,  r0
 mov.l  _pram10, r4
 add    #4,  r4
 mov    #7,  r1
 mov    #-8,  r2
 lds    r0,   mach
 sts.l  mach, @-r4 ! <-------- 
 muls.w r2, r1     ! <--------
 sts    mach, r0
 cmp/eq #0xaa,  r0
 bt     .+6
 jmp    @r13
 nop
 sts    macl, r0
 cmp/eq #-56,  r0
 bt     .+6
 jmp    @r13
 nop
!---------------------
 mov    #0x55,  r0
 mov.l  _pram10, r4
 add    #4,  r4
 mov    #6,  r1
 mov    #7,  r2
 lds    r0,   mach
 sts.l  mach, @-r4 ! <-------- 
 mul.l  r2, r1     ! <--------
 sts    mach, r0
 cmp/eq #0x55,  r0
 bt     .+6
 jmp    @r13
 nop
 sts    macl, r0
 cmp/eq #42,  r0
 bt     .+6
 jmp    @r13
 nop
!---------------------
 mov    #0xaa,  r0
 mov.l  _pram10, r4
 mov    #5,  r1
 mov    #6,  r2
 lds    r0,   mach
 sts.l  mach, @-r4 ! <-------- 
 dmulu.l r2, r1    ! <--------
 sts    mach, r0
 cmp/eq #0x00,  r0
 bt     .+6
 jmp    @r13
 nop
 sts    macl, r0
 cmp/eq #30,  r0
 bt     .+6
 jmp    @r13
 nop
!---------------------
 mov    #0xaa,  r0
 mov.l  _pram10, r4
 mov    #5,  r1
 mov    #-6,  r2
 lds    r0,   mach
 sts.l  mach, @-r4 ! <-------- 
 dmuls.l r2, r1    ! <--------
 sts    mach, r0
 cmp/eq #0xff,  r0
 bt     .+6
 jmp    @r13
 nop
 sts    macl, r0
 cmp/eq #-30,  r0
 bt     .+6
 jmp    @r13
 nop
!---------------------
 mov    #0xff,  r1
 mov.l  _pram10, r4
 add    #4, r4
 mov.l  r1, @r4
 lds    r1, mach
 mov    #0, r0     ! S=0
 ldc    r0, sr
 lds    r0, macl   ! MAC=ffffffff 00000000
 sts.l  mach, @-r4 ! <--------
 mac.l  @r4+, @r4+ ! <--------
 sts    mach, r0   ! MAC=ffffffff 00000001
 cmp/eq #0xff,  r0
 bt     .+6
 jmp    @r13
 nop
 sts    macl, r0
 cmp/eq #0x01,  r0
 bt     .+6
 jmp    @r13
 nop
!---------------------
 mov    #0xff,  r1
 mov.l  _pram10, r4
 add    #4,  r4
 lds    r1, mach
 mov    #0, r0     ! S=0
 ldc    r0, sr
 lds    r0, macl   ! MAC=ffffffff 00000000
 sts.l  mach, @-r4 ! <--------
 mac.w  @r4+, @r4+ ! <--------
 sts    mach, r0   ! MAC=ffffffff 00000001
 cmp/eq #0xff,  r0
 bt     .+6
 jmp    @r13
 nop
 sts    macl, r0
 cmp/eq #0x01,  r0
 bt     .+6
 jmp    @r13
 nop
!---------------------
 mov    #0xaa,   r0
 mov.l  _pram10, r4
 add    #4,   r4
 lds    r0,   mach
 mov    r4,   r5
 mov    #0x55, r1
 mov.l  r1,   @r5
 sts.l  mach, @-r4 ! <--------
 lds.l  @r5+, mach ! <--------
 sts    mach,  r0
 cmp/eq #0x55, r0
 bt     .+6
 jmp    @r13
 nop
 mov.l  @r4+,  r0
 cmp/eq #0xaa, r0
 bt     .+6
 jmp    @r13
 nop
 mov.l  @r4+,  r0
 cmp/eq #0x55, r0
 bt     .+6
 jmp    @r13
 nop
!---------------------
 mov    #0xaa,   r0
 mov    #0x55,   r1
 mov.l  _pram10, r4
 add    #4,   r4
 lds    r0,   mach
 sts.l  mach, @-r4 ! <--------
 lds    r1,   mach ! <--------
 sts    mach, r0
 cmp/eq #0x55, r0
 bt     .+6
 jmp    @r13
 nop
 mov.l  @r4+,  r0
 cmp/eq #0xaa, r0
 bt     .+6
 jmp    @r13
 nop
!---------------------
 mov    #0x55,   r0
 mov.l  _pram10, r4
 add    #4,   r4
 lds    r0,   mach
 sts.l  mach, @-r4 ! <--------
 clrmac            ! <--------
 sts    mach,  r0
 cmp/eq #0x00, r0
 bt     .+6
 jmp    @r13
 nop
 mov.l  @r4+,  r0
 cmp/eq #0x55, r0
 bt     .+6
 jmp    @r13
 nop
!---------------------
 mov    #0xaa,   r0
 mov.l  _pram10, r4
 add    #8,   r4
 lds    r0,   mach
 sts.l  mach, @-r4 ! <--------
 sts.l  mach, @-r4 ! <--------
 sts    mach, r1
 cmp/eq r1, r0
 bt     .+6
 jmp    @r13
 nop
 mov.l  @r4+, r2
 cmp/eq r2, r0
 bt     .+6
 jmp    @r13
 nop
 mov.l  @r4+, r3
 cmp/eq r3, r0
 bt     .+6
 jmp    @r13
 nop
!---------------------
 mov    #0xaa,   r0
 mov.l  _pram10, r4
 add    #4,   r4
 lds    r0,   mach
 sts.l  mach, @-r4 ! <--------
 sts    mach, r1   ! <--------
 cmp/eq r1, r0
 bt     .+6
 jmp    @r13
 nop
 mov.l  @r4,  r2
 cmp/eq r2, r0
 bt     .+6
 jmp    @r13
 nop
!---------------------
!---------------------
 bra    skip2
 nop
 .align 4
_pram10:   .long _ram0+128
_pram11:   .long _ram0+128+0x10
skip2:
!---------------------
!===[2]===============
!---------------------
 mov    #0x55,  r0
 mov.l  _pram00, r4
 mov.l  r0,  @r4
 mov    #8,  r1
 mov    #9,  r2
 lds    r0,   mach
 lds.l  @r4+, macl ! <-------- 
 mulu.w r2, r1     ! <--------
 sts    mach, r0
 cmp/eq #0x55,  r0
 bt     .+6
 jmp    @r13
 nop
 sts    macl, r0
 cmp/eq #72,  r0
 bt     .+6
 jmp    @r13
 nop
!---------------------
 mov    #0xaa,  r0
 mov.l  _pram00, r4
 mov.l  r0,  @r4
 mov    #7,  r1
 mov    #-8,  r2
 lds    r0,   mach
 lds.l  @r4+, macl ! <-------- 
 muls.w r2, r1     ! <--------
 sts    mach, r0
 cmp/eq #0xaa,  r0
 bt     .+6
 jmp    @r13
 nop
 sts    macl, r0
 cmp/eq #-56,  r0
 bt     .+6
 jmp    @r13
 nop
!---------------------
 mov    #0x55,  r0
 mov.l  _pram00, r4
 mov.l  r0,  @r4
 mov    #6,  r1
 mov    #7,  r2
 lds    r0,   mach
 lds.l  @r4+, macl ! <-------- 
 mul.l  r2, r1     ! <--------
 sts    mach, r0
 cmp/eq #0x55,  r0
 bt     .+6
 jmp    @r13
 nop
 sts    macl, r0
 cmp/eq #42,  r0
 bt     .+6
 jmp    @r13
 nop
!---------------------
 mov    #0xaa,  r0
 mov.l  _pram00, r4
 mov.l  r0,  @r4
 mov    #5,  r1
 mov    #6,  r2
 lds    r0,   mach
 lds.l  @r4+, macl ! <-------- 
 dmulu.l r2, r1    ! <--------
 sts    mach, r0
 cmp/eq #0,  r0
 bt     .+6
 jmp    @r13
 nop
 sts    macl, r0
 cmp/eq #30,  r0
 bt     .+6
 jmp    @r13
 nop
!---------------------
 mov    #0xaa,  r0
 mov.l  _pram00, r4
 mov.l  r0,  @r4
 mov    #5,  r1
 mov    #-6,  r2
 lds    r0,   mach
 lds.l  @r4+, macl ! <-------- 
 dmuls.l r2, r1    ! <--------
 sts    mach, r0
 cmp/eq #0xff,  r0
 bt     .+6
 jmp    @r13
 nop
 sts    macl, r0
 cmp/eq #-30,  r0
 bt     .+6
 jmp    @r13
 nop
!---------------------
 mov    #0,  r0
 mov    #4,  r1
 mov    #5,  r2
 mov.l  _pram00, r4
 mov.l  r0,  @r4
 add    #4,  r4
 mov.l  r1,  @r4
 add    #4,  r4
 mov.l  r2,  @r4
 mov.l  _pram00, r4
 lds    r0, mach   ! R0=0
 ldc    r0, sr     ! S=0
 lds.l  @r4+, macl ! <--------
 mac.l  @r4+, @r4+ ! <--------
 sts    mach, r0
 cmp/eq #0,  r0
 bt     .+6
 jmp    @r13
 nop
 sts    macl, r0
 cmp/eq #20,  r0
 bt     .+6
 jmp    @r13
 nop
!---------------------
 mov    #0,  r0
 mov    #4,  r1
 mov    #5,  r2
 mov.l  _pram00, r4
 mov.l  r0,  @r4
 add    #4,  r4
 mov.w  r1,  @r4
 add    #2,  r4
 mov.w  r2,  @r4
 mov.l  _pram00, r4
 lds    r0, mach   ! R0=0
 ldc    r0, sr     ! S=0
 lds.l  @r4+, macl ! <--------
 mac.w  @r4+, @r4+ ! <--------
 sts    mach, r0
 cmp/eq #0,  r0
 bt     .+6
 jmp    @r13
 nop
 sts    macl, r0
 cmp/eq #20,  r0
 bt     .+6
 jmp    @r13
 nop
!---------------------
 mov.l  _pram00, r4
 mov    #0xaa,   r0
 mov.l  r0,  @r4
 mov    r4,  r5
 add    #4,  r5
 mov    #0x55,   r0
 mov.l  r0,  @r5
 lds.l  @r4+, macl ! <--------
 lds.l  @r5+, macl ! <--------
 sts    macl,  r0
 cmp/eq #0x55, r0
 bt     .+6
 jmp    @r13
 nop
!---------------------
 mov.l  _pram00, r4
 mov    #0x55,   r0
 mov.l  r0,  @r4
 mov    #0xaa,   r1
 lds.l  @r4+, macl ! <--------
 lds    r1,   macl ! <--------
 sts    macl, r2
 cmp/eq r2, r1
 bt     .+6
 jmp    @r13
 nop
!---------------------
 mov.l  _pram00, r4
 mov    #0x55,   r0
 mov.l  r0,  @r4
 mov    #0x00,   r1
 lds.l  @r4+, macl
 clrmac          ! <--------  
 sts    macl, r2 ! <--------
 cmp/eq r2, r1
 bt     .+6
 jmp    @r13
 nop
!---------------------
 mov.l  _pram00, r4
 mov    #0x55, r0
 mov.l  r0,  @r4
 mov.l  _pram01, r5
 lds.l  @r4+, macl ! <--------
 sts.l  macl, @-r5 ! <--------
 mov.l  @r5+, r0
 cmp/eq #0x55, r0
 bt     .+6
 jmp    @r13
 nop
!---------------------
 mov.l  _pram00, r4
 mov    #0xaa, r0
 mov.l  r0,  @r4
 lds.l  @r4+, macl ! <--------
 sts    macl, r0   ! <--------
 cmp/eq #0xaa, r0
 bt     .+6
 jmp    @r13
 nop
!---------------------
!===[1]===============
!---------------------
 mov    #0x55,  r0
 mov.l  _pram00, r4
 mov.l  r0,  @r4
 mov    #8,  r1
 mov    #9,  r2
 lds.l  @r4+, mach ! <-------- 
 mulu.w r2, r1     ! <--------
 sts    mach, r0
 cmp/eq #0x55,  r0
 bt     .+6
 jmp    @r13
 nop
 sts    macl, r0
 cmp/eq #72,  r0
 bt     .+6
 jmp    @r13
 nop
!---------------------
 mov    #0xaa,  r0
 mov.l  _pram00, r4
 mov.l  r0,  @r4
 mov    #7,  r1
 mov    #-8,  r2
 lds.l  @r4+, mach ! <-------- 
 muls.w r2, r1     ! <--------
 sts    mach, r0
 cmp/eq #0xaa,  r0
 bt     .+6
 jmp    @r13
 nop
 sts    macl, r0
 cmp/eq #-56,  r0
 bt     .+6
 jmp    @r13
 nop
!---------------------
 mov    #0x55,  r0
 mov.l  _pram00, r4
 mov.l  r0,  @r4
 mov    #6,  r1
 mov    #7,  r2
 lds.l  @r4+, mach ! <-------- 
 mul.l  r2, r1     ! <--------
 sts    mach, r0
 cmp/eq #0x55,  r0
 bt     .+6
 jmp    @r13
 nop
 sts    macl, r0
 cmp/eq #42,  r0
 bt     .+6
 jmp    @r13
 nop
!---------------------
 mov    #0xaa,  r0
 mov.l  _pram00, r4
 mov.l  r0,  @r4
 mov    #5,  r1
 mov    #6,  r2
 lds.l  @r4+, mach ! <-------- 
 dmulu.l r2, r1    ! <--------
 sts    mach, r0
 cmp/eq #0,  r0
 bt     .+6
 jmp    @r13
 nop
 sts    macl, r0
 cmp/eq #30,  r0
 bt     .+6
 jmp    @r13
 nop
!---------------------
 mov    #0xaa,  r0
 mov.l  _pram00, r4
 mov.l  r0,  @r4
 mov    #5,  r1
 mov    #-6,  r2
 lds.l  @r4+, mach ! <-------- 
 dmuls.l r2, r1    ! <--------
 sts    mach, r0
 cmp/eq #0xff,  r0
 bt     .+6
 jmp    @r13
 nop
 sts    macl, r0
 cmp/eq #-30,  r0
 bt     .+6
 jmp    @r13
 nop
!---------------------
 mov    #0,  r0
 mov    #4,  r1
 mov    #5,  r2
 mov.l  _pram00, r4
 mov.l  r0,  @r4
 add    #4,  r4
 mov.l  r1,  @r4
 add    #4,  r4
 mov.l  r2,  @r4
 mov.l  _pram00, r4
 lds    r0, macl   ! R0=0
 ldc    r0, sr     ! S=0
 lds.l  @r4+, mach ! <--------
 mac.l  @r4+, @r4+ ! <--------
 sts    mach, r0
 cmp/eq #0,  r0
 bt     .+6
 jmp    @r13
 nop
 sts    macl, r0
 cmp/eq #20,  r0
 bt     .+6
 jmp    @r13
 nop
!---------------------
 mov    #0,  r0
 mov    #4,  r1
 mov    #5,  r2
 mov.l  _pram00, r4
 mov.l  r0,  @r4
 add    #4,  r4
 mov.w  r1,  @r4
 add    #2,  r4
 mov.w  r2,  @r4
 mov.l  _pram00, r4
 lds    r0, macl   ! R0=0
 ldc    r0, sr     ! S=0
 lds.l  @r4+, mach ! <--------
 mac.w  @r4+, @r4+ ! <--------
 sts    mach, r0
 cmp/eq #0,  r0
 bt     .+6
 jmp    @r13
 nop
 sts    macl, r0
 cmp/eq #20,  r0
 bt     .+6
 jmp    @r13
 nop
!---------------------
 mov.l  _pram00, r4
 mov    #0xaa,   r0
 mov.l  r0,  @r4
 mov    r4,  r5
 add    #4,  r5
 mov    #0x55,   r0
 mov.l  r0,  @r5
 lds.l  @r4+, mach ! <--------
 lds.l  @r5+, mach ! <--------
 sts    mach,  r0
 cmp/eq #0x55, r0
 bt     .+6
 jmp    @r13
 nop
!---------------------
 mov.l  _pram00, r4
 mov    #0x55,   r0
 mov.l  r0,  @r4
 mov    #0xaa,   r1
 lds.l  @r4+, mach ! <--------
 lds    r1,   mach ! <--------
 sts    mach, r2
 cmp/eq r2, r1
 bt     .+6
 jmp    @r13
 nop
!---------------------
 mov.l  _pram00, r4
 mov    #0x55,   r0
 mov.l  r0,  @r4
 mov    #0x00,   r1
 lds.l  @r4+, mach
 clrmac          ! <--------  
 sts    mach, r2 ! <--------
 cmp/eq r2, r1
 bt     .+6
 jmp    @r13
 nop
!---------------------
 mov.l  _pram00, r4
 mov    #0x55, r0
 mov.l  r0,  @r4
 mov.l  _pram01, r5
 lds.l  @r4+, mach ! <--------
 sts.l  mach, @-r5 ! <--------
 mov.l  @r5+, r0
 cmp/eq #0x55, r0
 bt     .+6
 jmp    @r13
 nop
!---------------------
 mov.l  _pram00, r4
 mov    #0xaa, r0
 mov.l  r0,  @r4
 lds.l  @r4+, mach ! <--------
 sts    mach, r0   ! <--------
 cmp/eq #0xaa, r0
 bt     .+6
 jmp    @r13
 nop
!---------------------
!-----------------------------
 bra    _testfinish
 nop
!-----------------------------
 .align 4
_pram00:   .long _ram0+128
_pram01:   .long _ram0+128+0x10
!-----------------------------
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
_ppass_value: .long 0x00000047

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
