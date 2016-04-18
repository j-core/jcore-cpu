/**************
 Initialization
 **************/
.global _testmov
_testmov:
 sts.l  pr, @-r15
 mov.l  _pfail, r13 !fail address
 bra    _testgo
 nop
_pfail: .long _fail
_testgo:

/******************
 LDC Rm, SR/GBR/VBR
 STC SR/GBR/VBR, Rn
 ******************/
_ldcstc:
 mov     #0xff, r0
 mov.l   _p01234567, r1
 mov.l   _p89abcdef, r2
 ldc     r0,  sr
 mov.l   _p000003f3, r0
 stc     sr,  r3
 ldc     r1,  gbr
 stc     gbr, r4
 ldc     r2,  vbr
 stc     vbr, r5
!-----
 cmp/eq r3, r0
 bt     .+6
 jmp    @r13
 nop
!-----
 cmp/eq r4, r1
 bt     .+6
 jmp    @r13
 nop
!-----
 cmp/eq r5, r2
 bt     .+6
 jmp    @r13
 nop

/********************
 CLRMAC
 LDS Rm, MACH/MACL/PR
 STS MACH/MACL/PR, Rn
 ********************/
_ldssts:
 clrmac
 mov.l  _p01234567, r0
 lds    r0, mach
 mov.l  _p89abcdef, r1
 lds    r1, macl
 mov.l  _p55aa55aa, r2
 lds    r2, pr
 sts    mach, r3
 sts    macl, r4
 sts    pr, r5
!-----
 cmp/eq r3, r0
 bt     .+6
 jmp    @r13
 nop
!----
 cmp/eq r4, r1
 bt     .+6
 jmp    @r13
 nop
!----
 cmp/eq r5, r2
 bt     .+6
 jmp    @r13
 nop

/****************
 LDS.L @Rm+, MACH
 STS.L MACH, @-Rn
 ****************/
_ldslstsl:
 clrmac
 mov.l  _pram0, r1
 mov.l  _pram0_16, r2
 mov.l  _p01234567, r0
 mov.l  r0, @r1
!----
 lds.l  @r1+, mach
 sts.l  mach, @-r2
!----
 sts    mach, r3
 cmp/eq r3, r0
 bt     .+6
 jmp    @r13
 nop
!----
 mov.l  @r2, r3
 cmp/eq r3, r0
 bt     .+6
 jmp    @r13
 nop
!----
 mov.l  _pram0_4, r4
 cmp/eq r4, r1
 bt     .+6
 jmp    @r13
 nop
!----
 mov.l  _pram0_12, r5 !_pram0+16-4
 cmp/eq r5, r2
 bt     .+6
 jmp    @r13
 nop

/****************
 LDS.L @Rm+, MACL
 STS.L MACL, @-Rn
 ****************/
 clrmac
 mov.l  _pram0, r1
 mov.l  _pram0_16, r2
 mov.l  _p89abcdef, r0
 mov.l  r0, @r1
!----
 lds.l  @r1+, macl
 sts.l  macl, @-r2
!----
 sts    macl, r3
 cmp/eq r3, r0
 bt     .+6
 jmp    @r13
 nop
!----
 mov.l  @r2, r3
 cmp/eq r3, r0
 bt     .+6
 jmp    @r13
 nop
!----
 mov.l  _pram0_4, r4
 cmp/eq r4, r1
 bt     .+6
 jmp    @r13
 nop
!----
 mov.l  _pram0_12, r5 !_pram0+16-4
 cmp/eq r5, r2
 bt     .+6
 jmp    @r13
 nop

/**************
 LDS.L @Rm+, PR
 STS.L PR, @-Rn
 **************/
 mov.l  _pram0, r1
 mov.l  _pram0_16, r2
 mov.l  _p11223344, r0
 mov.l  r0, @r1
!----
 lds.l  @r1+, pr
 sts.l  pr, @-r2
!----
 sts    pr, r3
 cmp/eq r3, r0
 bt     .+6
 jmp    @r13
 nop
!----
 mov.l  @r2, r3
 cmp/eq r3, r0
 bt     .+6
 jmp    @r13
 nop
!----
 mov.l  _pram0_4, r4
 cmp/eq r4, r1
 bt     .+6
 jmp    @r13
 nop
!----
 mov.l  _pram0_12, r5 !_pram0+16-4
 cmp/eq r5, r2
 bt     .+6
 jmp    @r13
 nop

/**************
 LDC.L @Rm+, SR
 STC.L SR, @-Rn
 **************/
_ldclstcl:
 mov    #0, r0
 ldc    r0, sr
 mov.l  _pram0, r1
 mov.l  _pram0_16, r2
 mov    #0xff, r0
 mov.l  r0, @r1
 mov.l  _p000003f3, r0
!----
 ldc.l  @r1+, sr
 stc.l  sr, @-r2
!----
 stc    sr, r3
 cmp/eq r3, r0
 bt     .+6
 jmp    @r13
 nop
!----
 mov.l  @r2, r3
 cmp/eq r3, r0
 bt     .+6
 jmp    @r13
 nop
!----
 mov.l  _pram0_4, r4
 cmp/eq r4, r1
 bt     .+6
 jmp    @r13
 nop
!----
 mov.l  _pram0_12, r5 !_pram0+16-4
 cmp/eq r5, r2
 bt     .+6
 jmp    @r13
 nop

/***************
 LDC.L @Rm+,GBR
 STC.L GBR, @-Rn
 ***************/
 mov    #0, r0
 ldc    r0, gbr
 mov.l  _pram0, r1
 mov.l  _pram0_16, r2
 mov.l  _p11223344, r0
 mov.l  r0, @r1
!----
 ldc.l  @r1+, gbr
 stc.l  gbr,  @-r2
!----
 stc    gbr, r3
 cmp/eq r3, r0
 bt     .+6
 jmp    @r13
 nop
!----
 mov.l  @r2, r3
 cmp/eq r3, r0
 bt     .+6
 jmp    @r13
 nop
!----
 mov.l  _pram0_4, r4
 cmp/eq r4, r1
 bt     .+6
 jmp    @r13
 nop
!----
 mov.l  _pram0_12, r5 !_pram0+16-4
 cmp/eq r5, r2
 bt     .+6
 jmp    @r13
 nop

/***************
 LDC.L @Rm+,VBR
 STC.L VBR, @-Rn
 ***************/
 mov    #0, r0
 ldc    r0, vbr
 mov.l  _pram0, r1
 mov.l  _pram0_16, r2
 mov.l  _p89abcdef, r0
 mov.l  r0, @r1
!----
 ldc.l  @r1+, vbr
 stc.l  vbr,  @-r2
!----
 stc    vbr, r3
 cmp/eq r3, r0
 bt     .+6
 jmp    @r13
 nop
!----
 mov.l  @r2, r3
 cmp/eq r3, r0
 bt     .+6
 jmp    @r13
 nop
!----
 mov.l  _pram0_4, r4
 cmp/eq r4, r1
 bt     .+6
 jmp    @r13
 nop
!----
 mov.l  _pram0_12, r5 !_pram0+16-4
 cmp/eq r5, r2
 bt     .+6
 jmp    @r13
 nop

/**************
 MOV.L Rm, @-Rn
 **************/
_movlramr:
 mov.l  _pram0, r1
 mov.l  _paabbccdd, r0
!----
 mov    r1, r2
 add    #4, r2
 mov.l  r0, @-r2
!----
 cmp/eq r1, r2
 bt     .+6
 jmp    @r13
 nop
!----
 mov.l  @r1, r3
 cmp/eq r3, r0
 bt     .+6
 jmp    @r13
 nop

/**************
 MOV.W Rm, @-Rn
 **************/
_movwramr:
 mov.l  _pram0, r1
 mov.w  _paabb, r0
!----
 mov    r1, r2
 add    #2, r2
 mov.w  r0, @-r2
!----
 cmp/eq r1, r2
 bt     .+6
 jmp    @r13
 nop
!----
 mov.w  @r1, r3
 cmp/eq r3, r0
 bt     .+6
 jmp    @r13
 nop

/**************
 MOV.B Rm, @-Rn
 **************/
_movbramr:
 mov.l  _pram0, r1
 mov    #0xaa, r0
!----
 mov    r1, r2
 add    #1, r2
 mov.b  r0, @-r2
!----
 cmp/eq r1, r2
 bt     .+6
 jmp    @r13
 nop
!----
 mov.b  @r1, r3
 cmp/eq r3, r0
 bt     .+6
 jmp    @r13
 nop

/**************
 MOV.L @Rm+, Rn
 **************/
_movlarpr:
 mov.l  _pram0, r1
 mov.l  _paabbccdd, r0
 mov.l  r0, @r1
!----
 mov     r1, r2
 mov.l   @r2+, r3
!----
 add    #4, r1
 cmp/eq r1, r2
 bt     .+6
 jmp    @r13
 nop
!----
 cmp/eq r3, r0
 bt     .+6
 jmp    @r13
 nop

/**************
 MOV.W @Rm+, Rn
 **************/
_movwarpr:
 mov.l  _pram0, r1
 mov.w  _paabb, r0
 mov.w  r0, @r1
!----
 mov     r1, r2
 mov.w   @r2+, r3
!----
 add    #2, r1
 cmp/eq r1, r2
 bt     .+6
 jmp    @r13
 nop
!----
 cmp/eq r3, r0
 bt     .+6
 jmp    @r13
 nop

/**************
 MOV.B @Rm+, Rn
 **************/
_movbarpr:
 mov.l  _pram0, r1
 mov    #0xaa, r0
 mov.b  r0, @r1
!----
 mov     r1, r2
 mov.b   @r2+, r3
!----
 add    #1, r1
 cmp/eq r1, r2
 bt     .+6
 jmp    @r13
 nop
!----
 cmp/eq r3, r0
 bt     .+6
 jmp    @r13
 nop

/**************
 MOV.L @Rm+, Rm
 **************/
_movlarpr2:
 mov.l  _pram0, r1
 mov.l  _paabbccdd, r0
 mov.l  r0, @r1
!----
 mov.l   @r1+, r1
!----
 cmp/eq r1, r0
 bt     .+6
 jmp    @r13
 nop

/**************
 MOV.W @Rm+, Rm
 **************/
_movwarpr2:
 mov.l  _pram0, r1
 mov.w  _paabb, r0
 mov.w  r0, @r1
!----
 mov.w   @r1+, r1
!----
 cmp/eq r1, r0
 bt     .+6
 jmp    @r13
 nop

/**************
 MOV.B @Rm+, Rm
 **************/
_movbarpr2:
 mov.l  _pram0, r1
 mov    #0xaa, r0
 mov.b  r0, @r1
!----
 mov.b   @r1+, r1
!----
 cmp/eq r1, r0
 bt     .+6
 jmp    @r13
 nop

/*****************
 MOV.L/W/B @Rm, Rn
 MOV.L/W/B Rm, @Rn
 *****************/
_movlwb:
 mov.l  _pram0, r2
 mov.l  _p11223344, r1
 mov.l  r1, @r2
!----
 mov.l  _p11223344, r8
 mov.l  _p00001122, r9
 mov.l  _p00000011, r10
!----
 mov.l  @r2, r0
 cmp/eq r0, r8
 bt     .+6
 jmp    @r13
 nop
!----
 mov.w  @r2, r0
 cmp/eq r0, r9
 bt     .+6
 jmp    @r13
 nop
!----
 mov.b  @r2, r0
 cmp/eq r0, r10
 bt     .+6
 jmp    @r13
 nop
!----
 mov.l  _pram0, r0
 mov    #0xaa, r1
 mov.b  r1, @r0
 add    #1, r0
 mov    #0xbb, r1
 mov.b  r1, @r0
 add    #1, r0
 mov.l  _pccdd, r1
 mov.w  r1, @r0
!----
 mov.l  _paabbccdd, r8
 mov.l  _pffffaabb, r9
 mov.l  _pffffffaa, r10
!----
 mov.l  _pram0, r0
 mov.l  @r0, r2
 cmp/eq r2, r8
 bt     .+6
 jmp    @r13
 nop
!----
 mov.w  @r0, r3
 cmp/eq r3, r9
 bt     .+6
 jmp    @r13
 nop
!----
 mov.b  @r0, r4
 cmp/eq r4, r10
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

        
/*****************
 CAS Rm, Rn, @R0
 *****************/
_cas_r:
 mov.l  _pram0_cas, r0
 mov.l  _p11223344_cas, r1
 mov.l  _p00001122_cas, r2
 mov.l  _p55aa55aa_cas, r3
 mov.l  r1, @r0

 mov.l  _pram0_4_cas, r4
 mov.l  _paabbccdd_cas, r5
 mov.l  r5, @r4
!----
 mov    #0, r10
 mov    #1, r11
 mov    #2, r12
 mov    r1, r8
 mov    r2, r9
/* cas.l r8, r9, @r0 */
 .word 0x02983

! cas.l had a bug where a subsequent instruction was skipped when the write back happened
! Do some movs to check later        
 mov #10, r10
 mov #11, r11
 mov #12, r12

!---- check CAS succeeded
 bt     .+6
 jmp    @r13
 nop
!---- check r8 unchanged
 cmp/eq r8, r1
 bt     .+6
 jmp    @r13
 nop
!---- check r9 was set to old @R0
 cmp/eq r9, r1
 bt     .+6
 jmp    @r13
 nop
!---- check that @R0 was written
 mov.l  @r0, r4
 cmp/eq r4, r2
 bt     .+6
 jmp    @r13
 nop
!---- check mov instructions after cas set r10, r11, and r12
 mov    #10, r7
 cmp/eq r7, r10
 bt     .+6
 jmp    @r13
 nop        
 mov    #11, r7   
 cmp/eq r7, r11
 bt     .+6
 jmp    @r13
 nop        
 mov    #12, r7   
 cmp/eq r7, r12
 bt     .+6
 jmp    @r13
 nop
!----

 mov    #0, r10
 mov    #1, r11
 mov    #2, r12
 mov.l  r1, @r0
 mov    r3, r8
 mov    r2, r9
/* cas.l r8, r9, @r0 */
 .word 0x02983

! cas.l had a bug where a subsequent instruction was skipped when the write back happened
! Do some movs to check later        
 mov #10, r10
 mov #11, r11
 mov #12, r12

!---- check CAS failed
 bf     .+6
 jmp    @r13
 nop
!---- check r8 unchanged
 cmp/eq r8, r3
 bt     .+6
 jmp    @r13
 nop
!---- check r9 was set to old @R0
 cmp/eq r9, r1
 bt     .+6
 jmp    @r13
 nop
!---- check that @R0 unchanged
 mov.l  @r0, r4
 cmp/eq r4, r1
 bt     .+6
 jmp    @r13
 nop
!---- check mov instructions after cas set r10, r11, and r12
 mov    #10, r7
 cmp/eq r7, r10
 bt     .+6
 jmp    @r13
 nop        
 mov    #11, r7   
 cmp/eq r7, r11
 bt     .+6
 jmp    @r13
 nop        
 mov    #12, r7   
 cmp/eq r7, r12
 bt     .+6
 jmp    @r13
 nop
!----

/**************
 Constant Table - Second table because cas tests were pushing previous
	constant table past pcrel limit
 **************/
 bra    _constantend_cas
 nop
.align 4
_pram0_cas    : .long _ram0+128
_pram0_4_cas  : .long _ram0+128+4
_p55aa55aa_cas: .long 0x55aa55aa
_p11223344_cas: .long 0x11223344
_p00001122_cas: .long 0x00001122
_paabbccdd_cas: .long 0xaabbccdd
.align 2
_constantend_cas:

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
_ppass_value: .long 0x00000021

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
