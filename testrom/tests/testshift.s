/**************
 Initialization
 **************/
.global _testshift
_testshift:
 sts.l  pr, @-r15
 mov.l  _pfail, r13 !fail address
 bra    _testgo
 nop
_pfail: .long _fail
_testgo:

/************************
 SHLL Rn
 ************************/
 mov.l  _ptestvalue, r1
!----
 mov.l  @r1+, r2 ! initial SR
 ldc    r2, sr
 mov.l  @r1+, r3 ! initial value
 shll   r3
 mov.l  @r1+, r4 ! result SR
 mov.l  @r1+, r5 ! result value
 stc    sr, r6
 cmp/eq r3, r5
 bt     .+6
 jmp    @r13
 nop
 cmp/eq r4, r6
 bt     .+6
 jmp    @r13
 nop
!----
 mov.l  @r1+, r2 ! initial SR
 ldc    r2, sr
 mov.l  @r1+, r3 ! initial value
 shal   r3
 mov.l  @r1+, r4 ! result SR
 mov.l  @r1+, r5 ! result value
 stc    sr, r6
 cmp/eq r3, r5
 bt     .+6
 jmp    @r13
 nop
 cmp/eq r4, r6
 bt     .+6
 jmp    @r13
 nop
!----
 mov.l  @r1+, r2 ! initial SR
 ldc    r2, sr
 mov.l  @r1+, r3 ! initial value
 shlr   r3
 mov.l  @r1+, r4 ! result SR
 mov.l  @r1+, r5 ! result value
 stc    sr, r6
 cmp/eq r3, r5
 bt     .+6
 jmp    @r13
 nop
 cmp/eq r4, r6
 bt     .+6
 jmp    @r13
 nop
!----
 mov.l  @r1+, r2 ! initial SR
 ldc    r2, sr
 mov.l  @r1+, r3 ! initial value
 shar   r3
 mov.l  @r1+, r4 ! result SR
 mov.l  @r1+, r5 ! result value
 stc    sr, r6
 cmp/eq r3, r5
 bt     .+6
 jmp    @r13
 nop
 cmp/eq r4, r6
 bt     .+6
 jmp    @r13
 nop
!----
 mov.l  @r1+, r2 ! initial SR
 ldc    r2, sr
 mov.l  @r1+, r3 ! initial value
 rotl   r3
 mov.l  @r1+, r4 ! result SR
 mov.l  @r1+, r5 ! result value
 stc    sr, r6
 cmp/eq r3, r5
 bt     .+6
 jmp    @r13
 nop
 cmp/eq r4, r6
 bt     .+6
 jmp    @r13
 nop
!----
 mov.l  @r1+, r2 ! initial SR
 ldc    r2, sr
 mov.l  @r1+, r3 ! initial value
 rotcl  r3
 mov.l  @r1+, r4 ! result SR
 mov.l  @r1+, r5 ! result value
 stc    sr, r6
 cmp/eq r3, r5
 bt     .+6
 jmp    @r13
 nop
 cmp/eq r4, r6
 bt     .+6
 jmp    @r13
 nop
!----
 mov.l  @r1+, r2 ! initial SR
 ldc    r2, sr
 mov.l  @r1+, r3 ! initial value
 rotr   r3
 mov.l  @r1+, r4 ! result SR
 mov.l  @r1+, r5 ! result value
 stc    sr, r6
 cmp/eq r3, r5
 bt     .+6
 jmp    @r13
 nop
 cmp/eq r4, r6
 bt     .+6
 jmp    @r13
 nop
!----
 mov.l  @r1+, r2 ! initial SR
 ldc    r2, sr
 mov.l  @r1+, r3 ! initial value
 rotcr  r3
 mov.l  @r1+, r4 ! result SR
 mov.l  @r1+, r5 ! result value
 stc    sr, r6
 cmp/eq r3, r5
 bt     .+6
 jmp    @r13
 nop
 cmp/eq r4, r6
 bt     .+6
 jmp    @r13
 nop
!----
 mov.l  @r1+, r2 ! initial SR
 ldc    r2, sr
 mov.l  @r1+, r3 ! initial value
 shll2  r3
 mov.l  @r1+, r4 ! result SR
 mov.l  @r1+, r5 ! result value
 stc    sr, r6
 cmp/eq r3, r5
 bt     .+6
 jmp    @r13
 nop
 cmp/eq r4, r6
 bt     .+6
 jmp    @r13
 nop
!----
 mov.l  @r1+, r2 ! initial SR
 ldc    r2, sr
 mov.l  @r1+, r3 ! initial value
 shll8  r3
 mov.l  @r1+, r4 ! result SR
 mov.l  @r1+, r5 ! result value
 stc    sr, r6
 cmp/eq r3, r5
 bt     .+6
 jmp    @r13
 nop
 cmp/eq r4, r6
 bt     .+6
 jmp    @r13
 nop
!----
 mov.l  @r1+, r2 ! initial SR
 ldc    r2, sr
 mov.l  @r1+, r3 ! initial value
 shll16 r3
 mov.l  @r1+, r4 ! result SR
 mov.l  @r1+, r5 ! result value
 stc    sr, r6
 cmp/eq r3, r5
 bt     .+6
 jmp    @r13
 nop
 cmp/eq r4, r6
 bt     .+6
 jmp    @r13
 nop
!----
 mov.l  @r1+, r2 ! initial SR
 ldc    r2, sr
 mov.l  @r1+, r3 ! initial value
 shlr2  r3
 mov.l  @r1+, r4 ! result SR
 mov.l  @r1+, r5 ! result value
 stc    sr, r6
 cmp/eq r3, r5
 bt     .+6
 jmp    @r13
 nop
 cmp/eq r4, r6
 bt     .+6
 jmp    @r13
 nop
!----
 mov.l  @r1+, r2 ! initial SR
 ldc    r2, sr
 mov.l  @r1+, r3 ! initial value
 shlr8  r3
 mov.l  @r1+, r4 ! result SR
 mov.l  @r1+, r5 ! result value
 stc    sr, r6
 cmp/eq r3, r5
 bt     .+6
 jmp    @r13
 nop
 cmp/eq r4, r6
 bt     .+6
 jmp    @r13
 nop
!----
 mov.l  @r1+, r2 ! initial SR
 ldc    r2, sr
 mov.l  @r1+, r3 ! initial value
 shlr16 r3
 mov.l  @r1+, r4 ! result SR
 mov.l  @r1+, r5 ! result value
 stc    sr, r6
 cmp/eq r3, r5
 bt     .+6
 jmp    @r13
 nop
 cmp/eq r4, r6
 bt     .+6
 jmp    @r13
 nop

 mov.l _ppass_value, r3
 mov.l _ppass_addr, r4
 mov.l r3, @r4

! Unlike other shifter instructions, SHAD and SHLD do not read or
! write the T bit. Instead of storing the initial and expected SR
! values in the test case, the following tests initialize SR to 0
! and compare against 0, using R0=0.        
 mov   #0, r0
!---- SHLD
.rept 4
 mov.l @r1+, r2 ! initial value A
 mov.l @r1+, r3 ! initial value B
 ldc   r0, sr
 shld  r3, r2
 stc   sr, r0
 mov.l @r1+, r5 ! result value
 cmp/eq r2, r5
 bt     .+6
 jmp    @r13
 nop
 cmp/eq #0, r0
 bt     .+6
 jmp    @r13
 nop
.endr
!----- SHAD
.rept 6
 mov.l @r1+, r2 ! initial value A
 mov.l @r1+, r3 ! initial value B
 ldc   r0, sr
 shad  r3, r2
 stc   sr, r0
 mov.l @r1+, r5 ! result value
 cmp/eq r2, r5
 bt     .+6
 jmp    @r13
 nop
 cmp/eq #0, r0
 bt     .+6
 jmp    @r13
 nop
.endr
        
!----
 bra    _pass
 nop
!----
 .align 4
_ptestvalue: .long _testvalue
_testvalue :
!----SHLL
 .long 0x00000000
 .long 0xaaaaaaab ! 1010....1011
 .long 0x00000001
 .long 0x55555556 ! 0101....0110
!----SHAL
 .long 0x00000001
 .long 0x55555557 ! 0101....0111
 .long 0x00000000
 .long 0xaaaaaaae ! 1010....1110
!----SHLR
 .long 0x00000001
 .long 0xeaaaaaaa ! 1110....1010
 .long 0x00000000
 .long 0x75555555 ! 0111....0101
!----SHAR
 .long 0x00000001
 .long 0xaaaaaaaa ! 1010....1010
 .long 0x00000000
 .long 0xd5555555 ! 1101....0101
!----ROTL
 .long 0x00000000
 .long 0xaaaaaaab ! 1010....1011
 .long 0x00000001
 .long 0x55555557 ! 0101....0111
!----ROTCL
 .long 0x00000000
 .long 0xaaaaaaab ! 1010....1011
 .long 0x00000001
 .long 0x55555556 ! 0101....0110
!----ROTR
 .long 0x00000000
 .long 0xd5555555 ! 1101....0101
 .long 0x00000001
 .long 0xeaaaaaaa ! 1110....1010
!----ROTCR
 .long 0x00000000
 .long 0xd5555555 ! 1101....0101
 .long 0x00000001
 .long 0x6aaaaaaa ! 0110....1010
!----SHLL2
 .long 0x00000001
 .long 0x12345678
 .long 0x00000001
 .long 0x48d159e0
!----SHLL8
 .long 0x00000001
 .long 0x12345678
 .long 0x00000001
 .long 0x34567800
!----SHLL16
 .long 0x00000001
 .long 0x12345678
 .long 0x00000001
 .long 0x56780000
!----SHLR2
 .long 0x00000000
 .long 0x12345678
 .long 0x00000000
 .long 0x048d159e
!----SHLR8
 .long 0x00000000
 .long 0x12345678
 .long 0x00000000
 .long 0x00123456
!----SHLR16
 .long 0x00000000
 .long 0x12345678
 .long 0x00000000
 .long 0x00001234
!----SHLD
 .long 0xa55a5aa5 ! A
 .long 3          ! B
 .long 0x2ad2d528 ! Y

 .long 0xa55a5aa5
 .long -21
 .long 0x0000052a

 .long 0xa55a5aa5
 .long -32
 .long 0
        
 .long 0x255a5aa5
 .long -32
 .long 0
!----SHAD
 .long 0x0aa5a55a ! A
 .long 3          ! B
 .long 0x552d2ad0 ! Y
        
 .long 0x0aa5a55a
 .long 4
 .long 0xaa5a55a0
        
 .long 0x5aa5a55a
 .long -3
 .long 0x0b54b4ab
        
 .long 0xa55a5aa5
 .long -10
 .long 0xffe95696

 .long 0xa55a5aa5
 .long -32
 .long -1
        
 .long 0x255a5aa5
 .long -32
 .long 0

/**************
 Congratulations
 **************/
_pass:
 lds.l  @r15+, pr
 mov.l _ppass2_value, r0
 mov.l _ppass_addr, r1
 mov.l r0, @r1
 rts
 nop
.align 4
_ppass_addr: .long 0xABCD0000
_ppass_value: .long 0x00000032
_ppass2_value: .long 0x00000033

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
