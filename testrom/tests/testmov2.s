/**************
 Initialization
 **************/
.global _testmov2
_testmov2:
 sts.l  pr, @-r15
 mov.l  _pfail, r13 !fail address
 bra    _testgo
 nop
_pfail: .long _fail
_testgo:

/********************
 MOVA @(disp, PC), R0
 ********************/
 mov     #0xaa, r1
 mova    _mova_target, r0
 jmp     @r0
 nop
 jmp     @r13
 nop
 jmp     @r13
 nop
 jmp     @r13
 nop
_mova_target:
 bra     _mova_target2
 mova    _mova_target3, r0
 jmp     @r13
 nop
_mova_target2:
 jmp     @r0       ! jmp _mova_target4
 nop
_mova_target3:
 mov     #0x55, r1 ! should be skipped
 jmp     @r13      ! should be skipped
_mova_target4:
 nop
 mov     r1, r0
 cmp/eq  #0xaa, r0
 bt      .+6
 jmp     @r13
 nop

/**********************
 MOV.L R0, @(disp, GBR)
 MOV.W R0, @(disp, GBR)
 MOV.B R0, @(disp, GBR)
 MOV.L @(disp, GBR), R0
 MOV.W @(disp, GBR), R0
 MOV.B @(disp, GBR), R0
 **********************/
 mov.l   _pram0, r1
 ldc     r1, gbr

 mov.l   _p11223344, r0
 mov.l   r0, @(4, gbr)
 mov.w   r0, @(10, gbr)
 mov.b   r0, @(13, gbr)

 mov     #0xff, r0
 cmp/eq  #0xff, r0
 bt      .+6
 jmp     @r13
 nop
 
 mov.l   @(4, r1), r0
 mov.l   _p11223344, r2
 cmp/eq  r2, r0
 bt      .+6
 jmp     @r13
 nop

 mov.w   @(10, r1), r0
 mov.l   _p00003344, r2
 cmp/eq  r2, r0
 bt      .+6
 jmp     @r13
 nop

 mov.b  @(13, r1), r0
 mov.l  _p00000044, r2
 cmp/eq  r2, r0
 bt      .+6
 jmp     @r13
 nop
 
 mov.l   @(4, gbr), r0
 mov.l   _p11223344, r2
 cmp/eq  r2, r0
 bt      .+6
 jmp     @r13
 nop

 mov.w   @(10, gbr), r0
 mov.l   _p00003344, r2
 cmp/eq  r2, r0
 bt      .+6
 jmp     @r13
 nop

 mov.b  @(13, gbr), r0
 mov.l  _p00000044, r2
 cmp/eq  r2, r0
 bt      .+6
 jmp     @r13
 nop

/*********************
 MOV.L Rm, @(disp, Rn)
 MOV.W R0, @(disp, Rn)
 MOV.B R0, @(disp, Rn)
 *********************/
 mov.l   _pram0,     r1
 mov.l   _p11223344, r2
 mov.l   _pram0_16,  r3

 mov.l   r2, @(16, r1)
 mov.l   @r3, r0
 cmp/eq  r2, r0
 bt      .+6
 jmp     @r13
 nop

 mov     #0xff, r0
 mov.l   r0, @(8, r1)
 mov.l   _p11223344, r0
 mov.w   r0, @(8, r1)
 mov     r1, r5
 add     #8, r5
 mov.b   @(0, r5), r0 
 cmp/eq  #0x33, r0
 bt      .+6
 jmp     @r13
 nop
 mov.b   @(1, r5), r0 
 cmp/eq  #0x44, r0
 bt      .+6
 jmp     @r13
 nop
 mov.w   @(2, r5), r0 
 cmp/eq  #0xff, r0
 bt      .+6
 jmp     @r13
 nop

 mov     #0xff, r0
 mov.l   r0, @(4, r1)
 mov.l   _p11223344, r0
 mov.b   r0, @(4, r1)
 mov     r1, r5
 add     #4, r5
 mov.b   @(0, r5), r0 
 cmp/eq  #0x44, r0
 bt      .+6
 jmp     @r13
 nop
 mov.b   @(1, r5), r0 
 cmp/eq  #0xff, r0
 bt      .+6
 jmp     @r13
 nop
 mov.w   @(2, r5), r0 
 cmp/eq  #0xff, r0
 bt      .+6
 jmp     @r13
 nop

/*********************
 MOV.L @(disp, Rm), Rn
 MOV.W @(disp, Rm), R0
 MOV.B @(disp, Rm), R0
 *********************/
 mov.l   _pram0_16,  r1
 mov.l   _p11223344, r2
 mov.l   r2, @r1
 mov.l   _pram0,     r3

 mov.l   @(16, r3), r4
 cmp/eq  r4, r2
 bt      .+6
 jmp     @r13
 nop

 mov.w   @(16, r3), r0
 mov.l   _p00001122, r4
 cmp/eq  r4, r0
 bt      .+6
 jmp     @r13
 nop

 add     #4, r3
 mov.b   @(16-4, r3), r0
 mov.l   _p00000011, r4
 cmp/eq  r4, r0
 bt      .+6
 jmp     @r13
 nop
 
/*******************
 MOV.B @(R0, Rm), Rn
 *******************/
 mov.l   _pram0,     r1
 mov.l   _p04050607, r0
 mov.l   _p11223344, r2
 mov.l   r0,  @(0, r1)
 mov.l   r2,  @(4, r1)
 
 mov.b   @(0, r1), r0
 mov.l   _pram0, r3 
 mov.b   @(r0, r3), r4
 mov.l   _p00000011, r0
 cmp/eq  r4, r0
 bt     .+6
 jmp    @r13
 nop

/*******************
 MOV.W @(R0, Rm), Rn
 *******************/
 mov.l   _pram0,     r1
 mov.l   _p04050607, r0
 mov.l   _p11223344, r2
 mov.l   r0,  @(0, r1)
 mov.l   r2,  @(4, r1)
 
 mov.b   @(0, r1), r0
 mov.l   _pram0, r3 
 mov.w   @(r0, r3), r4
 mov.l   _p00001122, r0
 cmp/eq  r4, r0
 bt     .+6
 jmp    @r13
 nop

/*******************
 MOV.L @(R0, Rm), Rn
 *******************/
 mov.l   _pram0,     r1
 mov.l   _p04050607, r0
 mov.l   _p11223344, r2
 mov.l   r0,  @(0, r1)
 mov.l   r2,  @(4, r1)
 
 mov.b   @(0, r1), r0
 mov.l   _pram0, r3 
 mov.l   @(r0, r3), r4
 mov.l   _p11223344, r0
 cmp/eq  r4, r0
 bt     .+6
 jmp    @r13
 nop
 
 mov.l   _pram0, r3 
 mov.b   @(0, r1), r0
 mov.l   @(r0, r3), r4
 mov.l   _p11223344, r0
 cmp/eq  r4, r0
 bt     .+6
 jmp    @r13
 nop

/*******************
 MOV.B Rm, @(R0, Rn)
 *******************/
 mov.l   _pram0,     r1
 mov.l   _pram0_16,  r3
 mov.l   _p00010203, r0
 mov.l   _p04050607, r2
 mov.l   r0,  @(0, r1)
 mov.l   r2,  @(4, r1)
 mov     #0xff, r0
 mov.l   r0,  @(4, r3)

 mov.l   _p01234567, r2
 mov.b   @(4, r1), r0   ! r0=4
 mov.b   r2, @(r0, r3)
 mov     r3, r5
 add     #4, r5
 mov.b   @(0, r5), r0
 cmp/eq  #0x67, r0
 bt     .+6
 jmp    @r13
 nop
 mov.b   @(1, r5), r0
 cmp/eq  #0xff, r0
 bt     .+6
 jmp    @r13
 nop
 mov.w   @(2, r5), r0
 cmp/eq  #0xff, r0
 bt     .+6
 jmp    @r13
 nop

/*******************
 MOV.W Rm, @(R0, Rn)
 *******************/
 mov.l   _pram0,     r1
 mov.l   _pram0_16,  r3
 mov.l   _p00010203, r0
 mov.l   _p04050607, r2
 mov.l   r0,  @(0, r1)
 mov.l   r2,  @(4, r1)
 mov     #0xff, r0
 mov.l   r0,  @(4, r3)

 mov.l   _p01234567, r2
 mov.b   @(4, r1), r0   ! r0=4
 mov.w   r2, @(r0, r3)
 mov     r3, r5
 add     #4, r5
 mov.b   @(0, r5), r0
 cmp/eq  #0x45, r0
 bt     .+6
 jmp    @r13
 nop
 mov.b   @(1, r5), r0
 cmp/eq  #0x67, r0
 bt     .+6
 jmp    @r13
 nop
 mov.w   @(2, r5), r0
 cmp/eq  #0xff, r0
 bt     .+6
 jmp    @r13
 nop

/***********************
 MOV.L Rm, @(R0, Rn)
 ***********************/
 mov.l   _pram0,     r1
 mov.l   _pram0_16,  r3
 mov.l   _p00010203, r0
 mov.l   _p04050607, r2
 mov.l   r0,  @(0, r1)
 mov.l   r2,  @(4, r1)
 mov     #0xff, r0
 mov.l   r0,  @(4, r3)

 mov.l   _p01234567, r2
 mov.b   @(4, r1), r0   ! r0=4
 mov.l   r2, @(r0, r3)
 mov     r3, r5
 add     #4, r5
 mov.l   @r5, r4
 cmp/eq  r2, r4
 bt     .+6
 jmp    @r13
 nop

 mov.b   @(4, r1), r0   ! r0=4
 mov.l   _p01234567, r2
 mov.l   r2, @(r0, r3)
 mov     r3, r5
 add     #4, r5
 mov.l   @r5, r4
 cmp/eq  r2, r4
 bt     .+6
 jmp    @r13
 nop

 mov.l   _p01234567, r2
 mov.b   @(4, r1), r0   ! r0=4
 mov.l   _pram0_16,  r3
 mov.l   r2, @(r0, r3)
 mov     r3, r5
 add     #4, r5
 mov.l   @r5, r4
 cmp/eq  r2, r4
 bt     .+6
 jmp    @r13
 nop

/**************
 Constant Table
 **************/
 bra    _constantend
 nop
.align 4
_pram0    : .long _ram0+128
_pram0_4  : .long _ram0+128+4
_pram0_12 : .long _ram0+128+16-4
_pram0_16 : .long _ram0+128+16
_p01234567: .long 0x01234567
_p89abcdef: .long 0x89abcdef
_p55aa55aa: .long 0x55aa55aa
_p11223344: .long 0x11223344
_p00001122: .long 0x00001122
_p00000011: .long 0x00000011
_p00003344: .long 0x00003344
_p00000044: .long 0x00000044
_paabbccdd: .long 0xaabbccdd
_pffffaabb: .long 0xffffaabb
_pffffffaa: .long 0xffffffaa
_pccdd    : .long 0xffffccdd
_p000003f3: .long 0x000003f3
_p00010203: .long 0x00010203
_p04050607: .long 0x04050607
_paabb    : .word 0xaabb
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
_ppass_value: .long 0x00000022

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
