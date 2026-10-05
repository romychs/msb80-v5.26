; ===================================================
; Microsoft Basic-80 v5.26,  for CP/M OS,  for Triumph
; Adler Alphatronic PC.
;
; Math subroutines for Single,  Double real,  Int operands
; Disassembled by Romychs @ 2026-09-21
; ===================================================

; ---------------------------------------------------
; Double substract
;  dacc = dacc - arg
; ---------------------------------------------------
MB_DSUB:
    LD    HL, MB_ARG
    LD    A, (HL)
    OR    A
    RET   Z
    XOR   0x80
    LD    (HL), A
    JR    MB_DO_DADD

; ---------------------------------------------------
; Double real addition
; dacc = dacc + (HL)
; ---------------------------------------------------
MB_DRADD:
    CALL  MB_MOVE

; ---------------------------------------------------
; Double real addition
; dacc = dacc + arg
; ---------------------------------------------------
MB_DADD:
    LD    HL, MB_ARG
    LD    A, (HL)
    OR    A
    RET   Z
MB_DO_DADD:
    AND   0x7f
    LD    B, A
    LD    DE, MB_DACC
    LD    A, (DE)
    OR    A
    JP    Z, MB_VMOVFA
    AND   0x7f
    SUB   B
    JR    NC, MB_DAC_RSHFT
    CPL
    INC   A
    PUSH  AF
    PUSH  HL
    LD    B, 0x8

; ---------------------------------------------------
; Exchange AGR and DAC
; ---------------------------------------------------
MB_DADD_NXB:
    LD    A, (DE)
    LD    C, (HL)
    LD    (HL), A
    LD    A, C
    LD    (DE), A
    INC   DE
    INC   HL
    DJNZ  MB_DADD_NXB
    POP   HL
    POP   AF
MB_DAC_RSHFT:
    CP    0x10
    RET   NC
    PUSH  AF
    XOR   A
    LD    (MB_DACC+8), A
    LD    (MB_ARG+8), A
    LD    HL, MB_ARG+1
    POP   AF
    CALL  MB_DAC_LSHFT
    LD    HL, MB_ARG
    LD    A, (MB_DACC)
    XOR   (HL)
    JP    M, MB_DADD_SUB
    LD    A, (MB_ARG+8)
    LD    (MB_DACC+8), A
    CALL  MB_DADD_BCD
    JP    NC, MB_DAC_ROUND
    EX    DE, HL
    LD    A, (HL)
    INC   (HL)
    XOR   (HL)
    JP    M, MB_OVERR2
    CALL  MB_DAC_RSHFT_BYTE1
    LD    A, (HL)
    OR    0x10
    LD    (HL), A
    JR    MB_DAC_ROUND
MB_DADD_SUB:
    CALL  MB_DSUB1

; ---------------------------------------------------
; Normalize daccum
; ---------------------------------------------------
MB_DECNRM:
    LD    HL, MB_DACC+1
    LD    BC, 2048
MB_NRM_NXT:
    LD    A, (HL)
    OR    A
    JR    NZ, MB_NRM_DO
    INC   HL
    DEC   C
    DEC   C
    DJNZ  MB_NRM_NXT
    JP    MB_ACC_ZERO
MB_NRM_DO:
    AND   0xf0
    JR    NZ, MB_NRM_SHIFT
    PUSH  HL
    CALL  MB_DAC_RSHFT_FULL
    POP   HL
    DEC   C
MB_NRM_SHIFT:
    LD    A, 0x8
    SUB   B
    JR    Z, MB_NRM_EXP
    PUSH  AF
    PUSH  BC
    LD    C, B
    LD    DE, MB_DACC+1
    LD    B, 0x0
    LDIR
    POP   BC
    POP   AF
    LD    B, A
    XOR   A
MB_NRM_CLR:
    LD    (DE), A
    INC   DE
    DJNZ  MB_NRM_CLR
MB_NRM_EXP:
    LD    A, C
    OR    A
    JR    Z, MB_DAC_ROUND
    LD    HL, MB_DACC
    LD    B, (HL)
    ADD   A, (HL)
    LD    (HL), A
    XOR   B
    LD    A, B
    JP    M, MB_OVERR4
    RET   Z

; --------------------------------------------------
; Round daccum and exit
; --------------------------------------------------
MB_DAC_ROUND:
    LD    HL, MB_DACC+8
    LD    B, 0x7

; ---------------------------------------------------
MB_ROUND:
    LD    A, (HL)
    CP    0x50
    RET   C
    DEC   HL
    XOR   A
    SCF

; ---------------------------------------------------
MB_ROUND_CARRY:
    ADC   A, (HL)
    DAA
    LD    (HL), A
    RET   NC
    DEC   HL
    DJNZ  MB_ROUND_CARRY
    LD    A, (HL)
    INC   (HL)
    XOR   (HL)
    LD    A, (HL)
    RLA
    CCF
    JP    M, MB_OVERR4
    INC   HL
    LD    (HL), 0x10
    RET

; ---------------------------------------------------
MB_DADD_BCD:
    LD    HL, MB_ARG+7
    LD    DE, MB_DACC+7
    LD    B, 0x7

; ---------------------------------------------------
MB_ADDB:
    XOR   A
MB_ADDB_NXT:
    LD    A, (DE)
    ADC   A, (HL)
    DAA
    LD    (DE), A
    DEC   DE
    DEC   HL
    DJNZ  MB_ADDB_NXT
    RET

; ---------------------------------------------------
MB_DSUB1:
    LD    HL, MB_ARG+8
    LD    A, (HL)
    CP    0x50
    JR    NZ, MB_DS1_PREP
    INC   (HL)
MB_DS1_PREP:
    LD    DE, MB_DACC+8
    LD    B, 8
    SCF
MB_DS1_LOOP:
    LD    A, 153
    ADC   A, 0x0
    SUB   (HL)
    LD    C, A
    LD    A, (DE)
    ADD   A, C
    DAA
    LD    (DE), A
    DEC   DE
    DEC   HL
    DJNZ  MB_DS1_LOOP
    RET   C
    EX    DE, HL
    LD    A, (HL)
    XOR   0x80
    LD    (HL), A
    LD    HL, MB_DACC+8
    LD    B, 0x8
    XOR   A
MB_DS1_NEG_DACC:
    LD    A, 154
    SBC   A, (HL)
    ADC   A, 0x0
    DAA
    CCF
    LD    (HL), A
    DEC   HL
    DJNZ  MB_DS1_NEG_DACC
    RET

; ---------------------------------------------------
MB_DAC_RSHFT_FULL:
    LD    HL, MB_DACC+8

; ---------------------------------------------------
MB_DAC_RSHFT_NIB:
    PUSH  BC
    LD    D, B
    LD    C, 0x4
MATH_RSNIB_NXT1:
    PUSH  HL
    OR    A
MATH_RSNIB_NXT2:
    LD    A, (HL)
    RLA
    LD    (HL), A
    DEC   HL
    DJNZ  MATH_RSNIB_NXT2
    LD    B, D
    POP   HL
    DEC   C
    JR    NZ, MATH_RSNIB_NXT1
    POP   BC
    RET

; ---------------------------------------------------
MB_DAC_LSHFT:
    OR    A
    RRA
    PUSH  AF
    OR    A
    JP    Z, MB_LSHFT_DONE
    PUSH  AF
    CPL
    INC   A
    LD    C, A
    LD    B, 0xff
    LD    DE, 0x7
    ADD   HL, DE
    LD    D, H
    LD    E, L
    ADD   HL, BC
    LD    A, 0x8
    ADD   A, C
    LD    C, A
    PUSH  BC
    LD    B, 0x0
    LDDR
    POP   BC
    POP   AF
    INC   HL
    INC   DE
    PUSH  DE
    LD    B, A
    XOR   A
MB_LSHFT_ZERO:
    LD    (HL), A
    INC   HL
    DJNZ  MB_LSHFT_ZERO
    POP   HL
    POP   AF
    RET   NC
    LD    A, C
MB_DAC_RSHFT_RRD:
    PUSH  BC
    PUSH  DE
    LD    D, A
    LD    C, 0x4
MB_RSHFT_RRD_NXT:
    LD    B, D
    PUSH  HL
    OR    A
MB_DAC_RSHFT_BYTE:
    LD    A, (HL)
    RRA
    LD    (HL), A
    INC   HL
    DJNZ  MB_DAC_RSHFT_BYTE
    POP   HL
    DEC   C
    JR    NZ, MB_RSHFT_RRD_NXT
    POP   DE
    POP   BC
    RET

; ---------------------------------------------------
MB_DAC_RSHFT_BYTE1:
    LD    HL, MB_DACC+1
MB_RSHFT_SETUP:
    LD    A, 0x8
    JR    MB_DAC_RSHFT_RRD
MB_LSHFT_DONE:
    POP   AF
    RET   NC
    JR    MB_RSHFT_SETUP

; --------------------------------------------------
MB_DMULT:
    RST   SIGN
    RET   Z
    LD    A, (MB_ARG)
    OR    A
    JP    Z, MB_ACC_ZERO
    LD    B, A
    LD    HL, MB_DACC
    XOR   (HL)
    AND   0x80
    LD    C, A
    LD    A, B
    AND   0x7f
    LD    B, A
    LD    A, (HL)
    AND   0x7f
    ADD   A, B
    LD    B, A
    LD    (HL), 0x0
    AND   0xc0
    RET   Z
    CP    0xc0
    JR    NZ, MB_MUL_EXP
    LD    A, C
    RLA
    JP    MB_OVERR4
MB_MUL_EXP:
    LD    A, B
    ADD   A, 0x40
    AND   0x7f
    RET   Z
    OR    C
    DEC   HL
    LD    (HL), A
    LD    DE, MB_HOLD8+63
    LD    BC, 8
    LD    HL, MB_DACC+7
    PUSH  DE
    LDDR
    INC   HL
    XOR   A
    LD    B, 0x8
MB_MUL_CLR:
    LD    (HL), A
    INC   HL
    DJNZ  MB_MUL_CLR
    POP   DE
    LD    BC, MB_MUL_DONE
    PUSH  BC

; --------------------------------------------------
MB_RNDCALC:
    CALL  MB_SUM_PRE8
    PUSH  HL
    LD    BC, 8
    EX    DE, HL
    LDDR
    EX    DE, HL
    LD    HL, MB_HOLD8+55
    LD    B, 8
    CALL  MB_ADDB
    POP   DE
    CALL  MB_SUM_PRE8
    LD    C, 7
    LD    DE, MB_ARG+7
MB_RNDC_SCAN:
    LD    A, (DE)
    OR    A
    JR    NZ, MB_RNDC_BYTE
    DEC   DE
    DEC   C
    JR    MB_RNDC_SCAN

; --------------------------------------------------
MB_RNDC_BYTE:
    LD    A, (DE)
    DEC   DE
    PUSH  DE
    LD    HL, MB_HOLD8+7
MB_RNDC_BIT:
    ADD   A, A
    JR    C, MB_RNDC_ACC
    JR    Z, MB_LSHFDAC
MB_RNDC_ADD:
    LD    DE, 8
    ADD   HL, DE
    JR    MB_RNDC_BIT

; --------------------------------------------------
MB_RNDC_ACC:
    PUSH  AF
    LD    B, 8
    LD    DE, MB_DACC+7
    PUSH  HL
    CALL  MB_ADDB
    POP   HL
    POP   AF
    JR    MB_RNDC_ADD
; --------------------------------------------------
MB_LSHFDAC:
    LD    B, 15
    LD    DE, MB_DACC+14
    LD    HL, MB_DACC+15
    CALL  MB_MVBACK
    LD    (HL), 0x0
    POP   DE
    DEC   C
    JR    NZ, MB_RNDC_BYTE
    RET

; --------------------------------------------------
MB_MUL_DONE:
    DEC   HL
    LD    A, (HL)
    INC   HL
    LD    (HL), A
    JP    MB_DECNRM

; ---------------------------------------------------
MB_SUM_PRE8:
    LD    HL, -8
    ADD   HL, DE
    LD    C, 3
MB_SU8_NXT3:
    LD    B, 8
    OR    A
MB_SU8_NXT8:
    LD    A, (DE)
    ADC   A, A
    DAA
    LD    (HL), A
    DEC   HL
    DEC   DE
    DJNZ  MB_SU8_NXT8
    DEC   C
    JR    NZ, MB_SU8_NXT3
    RET

; --------------------------------------------------
; Double division
;  dacc = dacc / arg
; --------------------------------------------------
MB_DDIV:
    LD    A, (MB_ARG)
    OR    A
    JP    Z, MB_DIV0ERR
    LD    B, A
    LD    HL, MB_DACC
    LD    A, (HL)
    OR    A
    JP    Z, MB_ACC_ZERO
    XOR   B
    AND   0x80
    LD    C, A
    LD    A, B
    AND   0x7f
    LD    B, A
    LD    A, (HL)
    AND   0x7f
    SUB   B
    LD    B, A
    RRA
    XOR   B
    AND   0x40
    LD    (HL), 0x0
    JR    Z, MB_DIV_EXP
    LD    A, B
    AND   0x80
    RET   NZ
MB_DDV_OVERR:
    LD    A, C
    RLA
    JP    MB_OVERR4

; --------------------------------------------------
MB_DIV_EXP:
    LD    A, B
    ADD   A, 0x41
    AND   0x7f
    LD    (HL), A
    JR    Z, MB_DDV_OVERR
    OR    C
    LD    (HL), 0x0
    DEC   HL
    LD    (HL), A
    LD    DE, MB_DACC+7
    LD    HL, MB_ARG+7
    LD    B, 0x7
MB_DIV_SCAN:
    LD    A, (HL)
    OR    A
    JR    NZ, MB_DIV_INIT
    DEC   DE
    DEC   HL
    DJNZ  MB_DIV_SCAN
MB_DIV_INIT:
    LD    (MB_DECTM2), HL
    EX    DE, HL
    LD    (MB_DECTMP), HL
    LD    A, B
    LD    (MB_DECCNT), A
    LD    HL, MB_HOLD8+56
MB_DIV_NIB:
    LD    B, 0xf
MB_DIV_STEP:
    PUSH  HL
    PUSH  BC
    LD    HL, (MB_DECTM2)
    EX    DE, HL
    LD    HL, (MB_DECTMP)
    LD    A, (MB_DECCNT)
    LD    C, 0xff
MB_DIV_CMP:
    SCF
    INC   C
    LD    B, A
    PUSH  HL
    PUSH  DE
MB_DIV_SUB:
    LD    A, 0x99
    ADC   A, 0x0
    EX    DE, HL
    SUB   (HL)
    EX    DE, HL
    ADD   A, (HL)
    DAA
    LD    (HL), A
    DEC   HL
    DEC   DE
    DJNZ  MB_DIV_SUB
    LD    A, (HL)
    CCF
    SBC   A, 0x0
    LD    (HL), A
    POP   DE
    POP   HL
    LD    A, (MB_DECCNT)
    JR    NC, MB_DIV_CMP
    LD    B, A
    EX    DE, HL
    CALL  MB_ADDB
    JR    NC, MB_DIV_QBIT
    EX    DE, HL
    INC   (HL)
MB_DIV_QBIT:
    LD    A, C
    POP   BC
    LD    C, A
    PUSH  BC
    LD    A, B
    OR    A
    RRA
    LD    B, A
    INC   B
    LD    E, B
    LD    D, 0x0
    LD    HL, MB_DECCNT+1
    ADD   HL, DE
    CALL  MB_DAC_RSHFT_NIB
    POP   BC
    POP   HL
    LD    A, B
    INC   C
    DEC   C
    JR    NZ, MB_DIV_STORE
    CP    0xf
    JR    Z, MB_DIV_CNT_DEC
    RRCA
    RLCA
    JR    NC, MB_DIV_STORE
    PUSH  BC
    PUSH  HL
    LD    HL, MB_DACC
    LD    B, 8
MB_DIV_ZERO:
    LD    A, (HL)
    OR    A
    JR    NZ, MB_DIV_QBIT2
    INC   HL
    DJNZ  MB_DIV_ZERO
    POP   HL
    POP   BC
    LD    A, B
    OR    A
    RRA
    INC   A
    LD    B, A
    XOR   A
MB_DIV_CLR:
    LD    (HL), A
    INC   HL
    DJNZ  MB_DIV_CLR
    JR    MB_DIV_DONE
MB_DIV_QBIT2:
    POP   HL
    POP   BC
    LD    A, B
    JR    MB_DIV_STORE
MB_DIV_CNT_DEC:
    LD    A, (MB_DECCNT+1)
    LD    E, A
    DEC   A
    LD    (MB_DECCNT+1), A
    XOR   E
    JP    P, MB_DIV_NIB
    JP    MB_ACC_ZERO

; ---------------------------------------------------
MB_DIV_STORE:
    RRA
    LD    A, C
    JR    C, MB_DIV_NIB4
    OR    (HL)
    LD    (HL), A
    INC   HL
    JR    MB_DIV_STEP_END
MB_DIV_NIB4:
    ADD   A, A
    ADD   A, A
    ADD   A, A
    ADD   A, A
    LD    (HL), A
MB_DIV_STEP_END:
    DEC   B
    JP    NZ, MB_DIV_STEP
MB_DIV_DONE:
    LD    HL, MB_DACC+8
    LD    DE, MB_HOLD8+63
    LD    B, 0x8
    CALL  MB_MVBACK
    JP    MB_MUL_DONE

; --------------------------------------------------
; COS Function
; --------------------------------------------------
MB_COS:
    LD    HL, C_015915
    CALL  MB_DMUL_HL      ;daccum = daccum * 0.15915494309190
    LD    A, (MB_DACC)
    AND   0x7f
    LD    (MB_DACC), A
    LD    HL, C_025
    CALL  MB_DSUB_HL
    CALL  MB_NEG
    JR    MB_COSIN_CMN

; --------------------------------------------------
; SIN Function
; --------------------------------------------------
MB_SIN:
    LD    HL, C_015915
    CALL  MB_DMUL_HL

; --------------------------------------------------
MB_COSIN_CMN:
    LD    A, (MB_DACC)
    OR    A
    CALL  M, MB_FLIP_SIGN
    CALL  MB_PUSH_DACC
    CALL  MB_INT
    CALL  MB_DAC_TO_ARG
    CALL  MB_POP_DACC
    CALL  MB_DSUB
    LD    A, (MB_DACC)
    CP    64
    JP    C, MB_CS_APROX
    LD    A, (MB_DACC+1)
    CP    37
    JP    C, MB_CS_APROX
    CP    117
    JP    NC, MB_CS_M1
    CALL  MB_DAC_TO_ARG
    LD    HL, C_05
    CALL  MB_SET_DAC
    CALL  MB_DSUB
    JP    MB_CS_APROX
MB_CS_M1:
    LD    HL, C_ONE
    CALL  MB_HL_TO_ARG
    CALL  MB_DSUB
MB_CS_APROX:
    LD    HL, MB_APROX_COSIN     ;=0x8
    JP    MB_POLY_APROX

; -------------------------------------------------
; TAN function
;  tan = sin / cos
; -------------------------------------------------
MB_TAN:
    CALL  MB_PUSH_DACC
    CALL  MB_COS
    CALL  MB_EX_DACSTK
    CALL  MB_SIN
    CALL  MB_POP_ARG
    LD    A, (MB_ARG)
    OR    A
    JP    NZ, MB_DDIV
    JP    MB_OVERR1

; --------------------------------------------------
; ATN function
; --------------------------------------------------
MB_ATN:
    LD    A, (MB_DACC)
    OR    A
    RET   Z
    CALL  M, MB_FLIP_SIGN
    CP    65
    JP    C, MB_ATN_HLP
    CALL  MB_DAC_TO_ARG
    LD    HL, C_ONE
    CALL  MB_SET_DAC
    CALL  MB_DDIV
    CALL  MB_ATN_HLP
    CALL  MB_DAC_TO_ARG
    LD    HL, C_15708
    CALL  MB_SET_DAC
    JP    MB_DSUB

; --------------------------------------------------
MB_ATN_HLP:
    LD    HL, C_02679
    CALL  MB_DCMP_HL                                ; Compare dacc with 0.26794919243112
    JP    M, MB_ATN_APROX
    CALL  MB_PUSH_DACC
    LD    HL, C_17320
    CALL  MB_DADD_HL                                ; dacc += 1.7320508075689
    CALL  MB_EX_DACSTK
    LD    HL, C_17320
    CALL  MB_DMUL_HL                                ; dacc *= 1.7320508075689
    LD    HL, C_ONE
    CALL  MB_DSUB_HL                                ; dacc -= 1.0
    CALL  MB_POP_ARG
    CALL  MB_DDIV                                   ; dacc /= arg
    CALL  MB_ATN_APROX
    LD    HL, C_05236
    JP    MB_DADD_HL                                ; dacc += 0.52359877559830

; --------------------------------------------------
MB_ATN_APROX:
    LD    HL, MB_APROX_ATN                          ; =0x8
    JP    MB_POLY_APROX                             ; polynomial approximation odd series

; --------------------------------------------------
; LOG function
; --------------------------------------------------
MB_LOG:
    RST   SIGN
    JP    M, MB_FCERR
    JP    Z, MB_FCERR
    LD    HL, MB_DACC
    LD    A, (HL)
    PUSH  AF
    LD    (HL), 'A'
    LD    HL, C_31623
    CALL  MB_DCMP_HL
    JP    M, MB_LOG_SERIES
    POP   AF
    INC   A
    PUSH  AF
    LD    HL, MB_DACC
    DEC   (HL)
MB_LOG_SERIES:
    POP   AF
    LD    (MB_TEMP3), A
    CALL  MB_PUSH_DACC
    LD    HL, C_ONE
    CALL  MB_DADD_HL
    CALL  MB_EX_DACSTK
    LD    HL, C_ONE
    CALL  MB_DSUB_HL
    CALL  MB_POP_ARG
    CALL  MB_DDIV
    CALL  MB_PUSH_DACC
    CALL  MB_DMUL_DACC
    CALL  MB_PUSH_DACC
    CALL  MB_PUSH_DACC
    LD    HL, MB_APROX_LOG2      ;=0x5
    CALL  MB_MUL_TAB
    CALL  MB_EX_DACSTK
    LD    HL, MB_APROX_LOG1      ;=0x4
    CALL  MB_MUL_TAB
    CALL  MB_POP_ARG
    CALL  MB_DDIV
    CALL  MB_POP_ARG
    CALL  MB_DMULT
    LD    HL, C_08685
    CALL  MB_DADD_HL
    CALL  MB_POP_ARG
    CALL  MB_DMULT
    CALL  MB_PUSH_DACC
    LD    A, (MB_TEMP3)
    SUB   0x41
    LD    L, A
    ADD   A, A
    SBC   A, A
    LD    H, A
    CALL  MB_CONSIH
    CALL  MB_CONDS
    CALL  MB_POP_ARG
    CALL  MB_DADD
    LD    HL, C_23025
    JP    MB_DMUL_HL

; --------------------------------------------------
; SQR function
; --------------------------------------------------
MB_SQR:
    RST   SIGN
    RET   Z
    JP    M, MB_FCERR
    CALL  MB_DAC_TO_ARG
    LD    A, (MB_DACC)
    OR    A
    RRA
    ADC   A, 0x20
    LD    (MB_ARG), A
    LD    A, (MB_DACC+1)
    OR    A
    RRCA
    OR    A
    RRCA
    AND   0x33
    ADD   A, 0x10
    LD    (MB_ARG+1), A
    LD    A, 0x7
MB_LOG_ITER:
    LD    (MB_TEMP3), A
    CALL  MB_PUSH_DACC
    CALL  MB_PUSH_ARG
    CALL  MB_DDIV
    CALL  MB_POP_ARG
    CALL  MB_DADD
    LD    HL, C_05
    CALL  MB_DMUL_HL
    CALL  MB_DAC_TO_ARG
    CALL  MB_POP_DACC
    LD    A, (MB_TEMP3)
    DEC   A
    JR    NZ, MB_LOG_ITER
    JP    MB_ARG_TO_DAC

; --------------------------------------------------
; EXP function
; --------------------------------------------------
MB_EXP:
    LD    HL, C_04343
    CALL  MB_DMUL_HL
    CALL  MB_PUSH_DACC
    CALL  MB_FRCINT
    LD    A, L
    RLA
    SBC   A, A
    CP    H
    JR    Z, MB_EXP_FRAC
    LD    A, H
    OR    A
    JP    P, MB_EXP_OVERR
    CALL  MB_VALDBL
    CALL  MB_POP_DACC
    LD    HL, C_ZERO
    JP    MB_SET_DAC

; --------------------------------------------------
MB_EXP_OVERR:
    CALL  MB_VALDBL
    CALL  MB_POP_DACC
    OR    A
    JP    MB_OVERR4

; --------------------------------------------------
MB_EXP_FRAC:
    LD    (MB_TEMP3), HL
    CALL  MB_FRCDBL
    CALL  MB_DAC_TO_ARG
    CALL  MB_POP_DACC
    CALL  MB_DSUB
    LD    HL, C_05
    CALL  MB_DCMP_HL
    PUSH  AF
    JR    Z, MB_EXP_POLY
    JR    C, MB_EXP_POLY
    LD    HL, C_05
    CALL  MB_DSUB_HL

; --------------------------------------------------
MB_EXP_POLY:
    CALL  MB_PUSH_DACC
    LD    HL, FCONST2      ;=0x3
    CALL  MB_POLY_APROX
    CALL  MB_EX_DACSTK
    LD    HL, FCONST1      ;=0x4
    CALL  MB_DMUL_HL1
    CALL  MB_POP_ARG
    CALL  MB_PUSH_ARG
    CALL  MB_PUSH_DACC
    CALL  MB_DSUB
    LD    HL, MB_HOLD8+56
    CALL  MB_GET_DAC
    CALL  MB_POP_ARG
    CALL  MB_POP_DACC
    CALL  MB_DADD
    LD    HL, MB_HOLD8+56
    CALL  MB_HL_TO_ARG
    CALL  MB_DDIV
    POP   AF
    JR    C, MB_EXP_SGNCHK
    JR    Z, MB_EXP_SGNCHK
    LD    HL, C_31623
    CALL  MB_DMUL_HL

; --------------------------------------------------
MB_EXP_SGNCHK:
    LD    A, (MB_TEMP3)
    LD    HL, MB_DACC
    LD    C, (HL)
    ADD   A, (HL)
    LD    (HL), A
    XOR   C
    RET   P
    OR    A
    JP    MB_OVERR4

; --------------------------------------------------
; RND Function
; --------------------------------------------------
MB_RND:
    RST   SIGN
    LD    HL, MB_RNDX
    JR    Z, MB_UC_RND
    CALL  M, MB_GET_DAC

; --------------------------------------------------
MB_RND1:
    LD    HL, MB_HOLD8+56
    LD    DE, MB_RNDX
    CALL  MB_MOV8B
    LD    HL, C_02132
    CALL  MB_HL_TO_ARG
    LD    HL, C_01439
    CALL  MB_SET_DAC
    LD    DE, MB_HOLD8+63
    CALL  MB_RNDCALC
    LD    DE, MB_DACC+8
    LD    HL, MB_RNDX+1
    LD    B, 7
    CALL  MB_MOVE_N
    LD    HL, MB_RNDX
    LD    (HL), 0x0
; --------------------------------------------------
MB_UC_RND:
    CALL  MB_SET_DAC
    LD    HL, MB_DACC
    LD    (HL), 0x40
    XOR   A
    LD    (MB_DACC+8), A
    JP    MB_DECNRM
; --------------------------------------------------
; Initialize RNDX table
; --------------------------------------------------
MB_INI_RNDX:
    LD    DE, MB_RNDCOP
    LD    HL, MB_RNDX
    JR    MB_MOV8B
; --------------------------------------------------
MB_ISFUN_NB:
    RST   CHRGTR
    PUSH  HL
    CALL  MB_VALDBL
    CALL  MB_RND1
    POP   HL
    RET

; --------------------------------------------------
; dacc = dacc + (HL)
; --------------------------------------------------
MB_DADD_HL:
    CALL  MB_HL_TO_ARG
    JP    MB_DADD

; --------------------------------------------------
; dacc = dacc - (HL)
; --------------------------------------------------
MB_DSUB_HL:
    CALL  MB_HL_TO_ARG
    JP    MB_DSUB

; ---------------------------------------------------
; dacc = dacc^2
; ---------------------------------------------------
MB_DMUL_DACC:
    LD    HL, MB_DACC

; ---------------------------------------------------
; dacc = dacc * (HL)
; ---------------------------------------------------
MB_DMUL_HL:
    CALL  MB_HL_TO_ARG
    JP    MB_DMULT

; --------------------------------------------------
; dacc = dacc / (HL)
; --------------------------------------------------
MB_DDIV_HL:
    CALL  MB_HL_TO_ARG
    JP    MB_DDIV

; --------------------------------------------------
; Compare dacc and (HL)
; HL -> operand
; --------------------------------------------------
MB_DCMP_HL:
    CALL  MB_HL_TO_ARG
    JP    MB_XDCOMP

; --------------------------------------------------
; Move DDAC to ARG
; --------------------------------------------------
MB_DAC_TO_ARG:
    LD    HL, MB_DACC

; --------------------------------------------------
; Move operand (HL) to ARG
; --------------------------------------------------
MB_HL_TO_ARG:
    LD    DE, MB_ARG

; --------------------------------------------------
; Move 8 bytes from (HL) to (DE)
; --------------------------------------------------
MB_MOV8_HLDE:
    EX    DE, HL
    CALL  MB_MOV8B
    EX    DE, HL
    RET

; --------------------------------------------------
; DDAC = ARG
; --------------------------------------------------
MB_ARG_TO_DAC:
    LD    HL, MB_ARG

; --------------------------------------------------
; daccum = (HL)
; --------------------------------------------------
MB_SET_DAC:
    LD    DE, MB_DACC
    JR    MB_MOV8_HLDE
    LD    DE, MB_ARG
    JR    MB_MOV8B

; ---------------------------------------------------
MB_RNDMN2:
    CALL  MB_CONSIH
    LD    HL, MB_RNDX

; --------------------------------------------------
; (HL) = DAC
; --------------------------------------------------
MB_GET_DAC:
    LD    DE, MB_DACC

; --------------------------------------------------
; Move 8 bytes (DE) -> (HL)
; --------------------------------------------------
MB_MOV8B:
    LD    B, 8
    JP    MB_MOVE_N

; --------------------------------------------------
; Exchange DAC with stack
; --------------------------------------------------
MB_EX_DACSTK:
    POP   HL
    LD    (MB_FBUFFR), HL
    CALL  MB_POP_ARG
    CALL  MB_PUSH_DACC
    CALL  MB_ARG_TO_DAC
    LD    HL, (MB_FBUFFR)
    JP    (HL)

; --------------------------------------------------
; Flip sign of DAC
; --------------------------------------------------
MB_FLIP_SIGN:
    CALL  MB_NEG
    LD    HL, MB_NEG
    EX    (SP), HL
    JP    (HL)

; --------------------------------------------------
; Polynomial approximation odd series
;  HL -> table len + table
; --------------------------------------------------
MB_POLY_APROX:
    LD    (MB_FBUFFR), HL
    CALL  MB_PUSH_DACC
    LD    HL, (MB_FBUFFR)
    CALL  MB_DMUL_HL1
    CALL  MB_POP_ARG
    JP    MB_DMULT

; --------------------------------------------------
MB_DMUL_HL1:
    LD    (MB_FBUFFR), HL
    CALL  MB_DMUL_DACC
    LD    HL, (MB_FBUFFR)

; --------------------------------------------------
MB_MUL_TAB:
    LD    A, (HL)
    PUSH  AF
    INC   HL
    PUSH  HL
    LD    HL, MB_FBUFFR
    CALL  MB_GET_DAC
    POP   HL
    CALL  MB_SET_DAC

MB_NXT_TVAL:
    POP   AF
    DEC   A
    RET   Z
    PUSH  AF
    PUSH  HL
    LD    HL, MB_FBUFFR
    CALL  MB_DMUL_HL
    POP   HL
    CALL  MB_HL_TO_ARG
    PUSH  HL
    CALL  MB_DADD
    POP   HL
    JR    MB_NXT_TVAL

; --------------------------------------------------
; Push DAC on stack
; --------------------------------------------------
MB_PUSH_ARG:
    LD    HL, MB_ARG+7
    JR    MB_PUSHDAC_LOOP

; --------------------------------------------------
MB_PUSH_DACC:
    LD    HL, MB_DACC+7
MB_PUSHDAC_LOOP:
    LD    A, 0x4
    POP   DE
MB_PUSH_WORD:
    LD    B, (HL)
    DEC   HL
    LD    C, (HL)
    DEC   HL
    PUSH  BC
    DEC   A
    JR    NZ, MB_PUSH_WORD
    EX    DE, HL
    JP    (HL)

; --------------------------------------------------
; Pop value from stack to ARG
; --------------------------------------------------
MB_POP_ARG:
    LD    HL, MB_ARG
    JR    MB_POPDAC_SETUP

; --------------------------------------------------
; Pop value from stack to DAC
; --------------------------------------------------
MB_POP_DACC:
    LD    HL, MB_DACC
MB_POPDAC_SETUP:
    LD    A, 0x4
    POP   DE
MB_POPDAC_NXT:
    POP   BC
    LD    (HL), C
    INC   HL
    LD    (HL), B
    INC   HL
    DEC   A
    JR    NZ, MB_POPDAC_NXT
    EX    DE, HL
    JP    (HL)

; --------------------------------------------------
; Float constants
; --------------------------------------------------
C_01439:
    ; 0.14389820420821
    DB    0x00, 0x14, 0x38, 0x98, 0x20, 0x42, 0x08, 0x21
C_02132:
    ; 0.21132486540519
    DB    0x00, 0x21, 0x13, 0x24, 0x86, 0x54, 0x05, 0x19
MB_RNDCOP:
    ; 0.40649651372358
    DB    0x00, 0x40, 0x64, 0x96, 0x51, 0x37, 0x23, 0x58
C_04343:
    ; 0.43429448190324
    DB    0x40, 0x43, 0x42, 0x94, 0x48, 0x19, 0x03, 0x24
C_05:
    ; 0.5
    DB    0x40, 0x50
C_ZERO:
    ; 0.0
    .8 DB 0x0
C_ONE:
    ; 1.0
    DB    0x41, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
C_025:
    ; 0.25
    DB    0x40, 0x25, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
C_31623:
    ; 3.1622776601684   foot to metric cvt
    DB    0x41, 0x31, 0x62, 0x27, 0x76, 0x60, 0x16, 0x84
C_08685:
    ; 0.86858896380650 = LOG10(E^2)
    DB    0x40, 0x86, 0x85, 0x88, 0x96, 0x38, 0x06, 0x50
C_23025:
    ; 2.3025850929940 = LN(10)
    DB    0x41, 0x23, 0x02, 0x58, 0x50, 0x92, 0x99, 0x40
C_15708:
    ; 1.5707963267949 = PI/2
    DB    0x41, 0x15, 0x70, 0x79, 0x63, 0x26, 0x79, 0x49
C_02679:
    ; 0.26794919243112 = tg(30) = tg(Pi/6)
    DB    0x40, 0x26, 0x79, 0x49, 0x19, 0x24, 0x31, 0x12
C_17320:
    ; 1.7320508075689 = √3
    DB    0x41, 0x17, 0x32, 0x05, 0x08, 0x07, 0x56, 0x89
C_05236:
    ; 0.52359877559830 = 30 grad in RAD
    DB    0x40, 0x52, 0x35, 0x98, 0x77, 0x55, 0x98, 0x30
C_015915:
    ; 0.15915494309190
    DB    0x40, 0x15, 0x91, 0x54, 0x94, 0x30, 0x91, 0x90

FCONST1:
    DB    4
    ; 1.0
    DB    0x41, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    ; 159.37415236031
    DB    0x43, 0x15, 0x93, 0x74, 0x15, 0x23, 0x60, 0x31
    ; 2709.3169408516
    DB    0x44, 0x27, 0x09, 0x31, 0x69, 0x40, 0x85, 0x16
    ; 4497.6335574058
    DB    0x44, 0x44, 0x97, 0x63, 0x35, 0x57, 0x40, 0x58

FCONST2:
    DB   3
    ; 18.312360159275
    DB    0x42, 0x18, 0x31, 0x23, 0x60, 0x15, 0x92, 0x75
    ; 831.40672129371
    DB    0x43, 0x83, 0x14, 0x06, 0x72, 0x12, 0x93, 0x71
    ; 5178.0919915162
    DB    0x44, 0x51, 0x78, 0x09, 0x19, 0x91, 0x51, 0x62

MB_APROX_LOG1:
    DB    4
    ; -0.71433382153226
    DB    0xC0, 0x71, 0x43, 0x33, 0x82, 0x15, 0x32, 0x26
    ; 6.2503651127908
    DB    0x41, 0x62, 0x50, 0x36, 0x51, 0x12, 0x79, 0x08
    ; -13.682370241503
    DB    0xC2, 0x13, 0x68, 0x23, 0x70, 0x24, 0x15, 0x03
    ; 8.5167319872389
    DB    0x41, 0x85, 0x16, 0x73, 0x19, 0x87, 0x23, 0x89

MB_APROX_LOG2:
    DB    5
    ; 1.0
    DB    0x41, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    ; -13.210478350156
    DB    0xC2, 0x13, 0x21, 0x04, 0x78, 0x35, 0x01, 0x56
    ; 47.925256043873
    DB    0x42, 0x47, 0x92, 0x52, 0x56, 0x04, 0x38, 0x73
    ; -64.906682740943
    DB    0xC2, 0x64, 0x90, 0x66, 0x82, 0x74, 0x09, 0x43
    ; 29.415750172323
    DB    0x42, 0x29, 0x41, 0x57, 0x50, 0x17, 0x23, 0x23

MB_APROX_COSIN:
    DB    8
    ; -0.69215692291809
    DB    0xC0, 0x69, 0x21, 0x56, 0x92, 0x29, 0x18, 0x09
    ; 3.8172886385771
    DB    0x41, 0x38, 0x17, 0x28, 0x86, 0x38, 0x57, 0x71
    ; -15.094499474801
    DB    0xC2, 0x15, 0x09, 0x44, 0x99, 0x47, 0x48, 0x01
    ; 42.058689667355
    DB    0x42, 0x42, 0x05, 0x86, 0x89, 0x66, 0x73, 0x55
    ; -76.705859683291
    DB    0xC2, 0x76, 0x70, 0x58, 0x59, 0x68, 0x32, 0x91
    ; 81.605249275513
    DB    0x42, 0x81, 0x60, 0x52, 0x49, 0x27, 0x55, 0x13
    ; -41.341702240398
    DB    0xC2, 0x41, 0x34, 0x17, 0x02, 0x24, 0x03, 0x98
    ; 6.2831853071796
    DB    0x41, 0x62, 0x83, 0x18, 0x53, 0x07, 0x17, 0x96

MB_APROX_ATN:
    DB    8
    ; -0.05208693904000
    DB    0xBF, 0x52, 0x08, 0x69, 0x39, 0x04, 0x00, 0x00
    ; 0.07530714913480
    DB    0x3F, 0x75, 0x30, 0x71, 0x49, 0x13, 0x48, 0x00
    ; -0.09081343224705
    DB    0xBF, 0x90, 0x81, 0x34, 0x32, 0x24, 0x70, 0x50
    ; 0.11110794184029
    DB    0x40, 0x11, 0x11, 0x07, 0x94, 0x18, 0x40, 0x29
    ; -0.14285708554884
    DB    0xC0, 0x14, 0x28, 0x57, 0x08, 0x55, 0x48, 0x84
    ; 0.19999999948967
    DB    0x40, 0x19, 0x99, 0x99, 0x99, 0x94, 0x89, 0x67
    ; -0.33333333333160
    DB    0xC0, 0x33, 0x33, 0x33, 0x33, 0x33, 0x31, 0x60
    ; 1.0
    DB    0x41, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00

; ==================================================
; SIGN, SGN, FLOAT, NEG and ABS
; ==================================================

; --------------------------------------------------
MB_SIGN:
    LD    A, (MB_DACC)
    OR    A
    RET   Z
MB_SIGN1:
    LD    A, (MB_DACC)
    JR    MB_ICOMPS

; --------------------------------------------------
MB_FCOMPS:
    CPL

; --------------------------------------------------
MB_ICOMPS:
    RLA

; --------------------------------------------------
MB_SIGNS:
    SBC   A, A
    RET   NZ
    INC   A
    RET

; --------------------------------------------------
; Set accumulator to 0.0
; --------------------------------------------------
MB_ACC_ZERO:
    XOR   A
    LD    (MB_DACC), A
    RET

; --------------------------------------------------
; Return absolute value
; --------------------------------------------------
MB_ABS:
    CALL  MB_VSIGN
    RET   P

; --------------------------------------------------
; Change sign
; --------------------------------------------------
MB_VNEG:
    RST   GETYPTR
    JP    M, MB_INEG
    JP    Z, MB_TMERR

; --------------------------------------------------
; Negate for single/double real
; --------------------------------------------------
MB_NEG:
    LD    HL, MB_DACC
    LD    A, (HL)
    OR    A
    RET   Z
    XOR   0x80
    LD    (HL), A
    RET

; --------------------------------------------------
; SGN function
; --------------------------------------------------
MB_SGN:
    CALL  MB_VSIGN
MB_CONIA:
    LD    L, A
    RLA
    SBC   A, A
    LD    H, A
    JP    MB_MAKINT

; --------------------------------------------------
; Get sign of accumulator
; --------------------------------------------------
MB_VSIGN:
    RST   GETYPTR
    JP    Z, MB_TMERR
    JP    P, MB_SIGN
    LD    HL, (MB_DACC+2)

; --------------------------------------------------
; Get sign of Int
;  Ret: A - sign
; --------------------------------------------------
MB_ISIGN:
    LD    A, H
    OR    L
    RET   Z
    LD    A, H
    JR    MB_ICOMPS

; --------------------------------------------------
; Push float accumulator to stack
; --------------------------------------------------
MB_PUSHF:
    EX    DE, HL
    LD    HL, (MB_DACC+2)
    EX    (SP), HL
    PUSH  HL
    LD    HL, (MB_DACC)
    EX    (SP), HL
    PUSH  HL
    EX    DE, HL
    RET

; --------------------------------------------------
; Single -> ACC
; --------------------------------------------------
    CALL  MB_MOVRM

; --------------------------------------------------
; Single in DE,  BC -> ACC
; --------------------------------------------------
MB_MOVFR:
    EX    DE, HL
    LD    (MB_DACC+2), HL
    LD    H, B
    LD    L, C
    LD    (MB_DACC), HL
    EX    DE, HL
    RET

; --------------------------------------------------
; Single from acc
;  Out: DE,  BC - single
; --------------------------------------------------
MB_MOVRF:
    LD    HL, (MB_DACC+2)
    EX    DE, HL
    LD    HL, (MB_DACC)
    LD    C, L
    LD    B, H
    RET

; --------------------------------------------------
; Load single
;  Inp: HL -> single
;  Out: DE,  BC - single
; --------------------------------------------------
MB_LDSGL:
    LD    C, (HL)
    INC   HL
    LD    B, (HL)
    INC   HL
    LD    E, (HL)
    INC   HL
    LD    D, (HL)
    INC   HL
    RET

; --------------------------------------------------
; Load single
;  Inp: HL -> single
;  Out: BC,  DE - single
; --------------------------------------------------
MB_MOVRM:
    LD    E, (HL)
    INC   HL

; --------------------------------------------------
; Get size and address of string
;  Inp: HL -> string descriptor
;  Out: D - size of string
;       BC -> string
; --------------------------------------------------
MB_GETSTR:
    LD    D, (HL)
    INC   HL
    LD    C, (HL)
    INC   HL
    LD    B, (HL)

; --------------------------------------------------
MB_INCHLRT:
    INC   HL
    RET

; --------------------------------------------------
; Move Single DAC -> (HL)
; --------------------------------------------------
MB_MOVSM:
    LD    DE, MB_DACC
    LD    B, 4
    JR    MB_MOVE_N

; --------------------------------------------------
MB_MOVE:
    LD    DE, MB_ARG

; --------------------------------------------------
MB_MOVVFM:
    EX    DE, HL

; --------------------------------------------------
; Copy variable content
;  Inp: HL -> src
;       DE -> dest
;       (VALTYP) - variable type
; --------------------------------------------------
MB_VMOVE:
    LD    A, (MB_VALTYP)
    LD    B, A

; --------------------------------------------------
; Move bytes
;  Inp: B - count
;       DE -> source
;       HL -> destination
; --------------------------------------------------
MB_MOVE_N:
    LD    A, (DE)
    LD    (HL), A
    INC   DE
    INC   HL
    DJNZ  MB_MOVE_N
    RET

; --------------------------------------------------
; Move bytes (decrement addresses)
;  Inp: B - counter
;       DE -> end of source
;       HL -> end of destination
; --------------------------------------------------
MB_MVBACK:
    LD    A, (DE)
    LD    (HL), A
    DEC   DE
    DEC   HL
    DJNZ  MB_MVBACK
    RET

; --------------------------------------------------
; DAC = ARG
; --------------------------------------------------
MB_VMOVFA:
    LD    HL, MB_ARG

; --------------------------------------------------
; DAC = HL
; --------------------------------------------------
MB_VMOVFM:
    LD    DE, MB_MOVVFM
    JR    MB_VMOV_DAC

; --------------------------------------------------
; ARG = DAC
; --------------------------------------------------
MB_VMOVAF:
    LD    HL, MB_ARG

; --------------------------------------------------
; HL = DAC
; --------------------------------------------------
MB_MOVMF:
    LD    DE, MB_VMOVE
MB_VMOV_DAC:
    PUSH  DE
    LD    DE, MB_DACC
    LD    A, (MB_VALTYP)
    CP    TINT
    RET   NZ
    LD    DE, MB_DACC+2
    RET

; --------------------------------------------------
; Single real compare
;  Inp: DE,  BC - first operand
;       accum - second operand
; --------------------------------------------------
MB_FCOMP:
    LD    A, C
    OR    A
    JP    Z, MB_SIGN
    LD    HL, MB_FCOMPS
    PUSH  HL
    RST   SIGN
    LD    A, C
    RET   Z
    LD    HL, MB_DACC
    XOR   (HL)
    LD    A, C
    RET   M
    CALL  MB_FCOMP2
    RRA
    XOR   C
    RET

; --------------------------------------------------
MB_FCOMP2:
    LD    A, C
    CP    (HL)
    RET   NZ
    INC   HL
    LD    A, B
    CP    (HL)
    RET   NZ
    INC   HL
    LD    A, E
    CP    (HL)
    RET   NZ
    INC   HL
    LD    A, D
    SUB   (HL)
    RET   NZ
    POP   HL
    POP   HL
    RET

; --------------------------------------------------
; Compare Integer
;  Inp: HL,  DE - integers
;  Out: ZF set if equal
; --------------------------------------------------
MB_ICOMP:
    LD    A, D
    XOR   H
    LD    A, H
    JP    M, MB_ICOMPS
    CP    D
    JR    NZ, MB_SIGNS1
    LD    A, L
    SUB   E
    RET   Z
MB_SIGNS1:
    JP    MB_SIGNS

; --------------------------------------------------
; Compare double real
; --------------------------------------------------
MB_XDCOMP:
    LD    DE, MB_ARG
    LD    A, (DE)
    OR    A
    JP    Z, MB_SIGN
    LD    HL, MB_FCOMPS
    PUSH  HL
    RST   SIGN
    LD    A, (DE)
    LD    C, A
    RET   Z
    LD    HL, MB_DACC
    XOR   (HL)
    LD    A, C
    RET   M
    LD    B, 8
MB_DCOMP1:
    LD    A, (DE)
    SUB   (HL)
    JR    NZ, MB_FCOMP_END
    INC   DE
    INC   HL
    DJNZ  MB_DCOMP1
    POP   BC
    RET
MB_FCOMP_END:
    RRA
    XOR   C
    RET

; --------------------------------------------------
; Compare double real
; --------------------------------------------------
MB_DCOMP:
    CALL  MB_XDCOMP
    JP    NZ, MB_FCOMPS
    RET

; --------------------------------------------------
; Convert accumulator to integer
; CINT function
; --------------------------------------------------
MB_FRCINT:
    RST   GETYPTR
    LD    HL, (MB_DACC+2)
    RET   M
    JP    Z, MB_TMERR
    CALL  MB_CVF2INT
    JP    C, MB_OVERR
    EX    DE, HL

; --------------------------------------------------
; Put HL to DACC
; --------------------------------------------------
MB_MAKINT:
    LD    (MB_DACC+2), HL
; --------------------------------------------------
MB_SETINT:
    LD    A, TINT
MB_CONISD:
    LD    (MB_VALTYP), A
    RET

; --------------------------------------------------
MB_CONSI_CHK:
    LD    BC, 12997
    LD    DE, 32886
    CALL  MB_FCOMP
    RET   NZ
    LD    HL, 0x8000
MB_CONSI_EXIT:
    POP   DE
    JR    MB_MAKINT

; --------------------------------------------------
; Convert DAC to single real
; CSNG function
; --------------------------------------------------
MB_FRCSNG:
    RST   GETYPTR
    RET   PO
    JP    M, MB_CONSI
    JP    Z, MB_TMERR
    CALL  MB_VALSNG
    CALL  MB_VALSNG1
    INC   HL
    LD    A, B
    OR    A
    RRA
    LD    B, A
    JP    MB_ROUND

; --------------------------------------------------
; Convert integer to single real
;  Inp: HL - integer
; --------------------------------------------------
MB_CONSI:
    LD    HL, (MB_DACC+2)

; --------------------------------------------------
; Convert to single precision real
; --------------------------------------------------
MB_CONSIH:
    LD    A, H
MB_CONSI2:
    OR    A
    PUSH  AF
    CALL  M, MB_INEGHL
    CALL  MB_VALSNG
    EX    DE, HL
    LD    HL, 0x0
    LD    (MB_DACC), HL
    LD    (MB_DACC+2), HL
    LD    A, D
    OR    E
    JP    Z, MB_PPSWRT
    LD    BC, 0x500
    LD    HL, MB_DACC+1
    PUSH  HL
    LD    HL, C_M10000     ;= -10000
MB_CONSI_DGT:
    LD    A, 0xff
    PUSH  DE
    LD    E, (HL)      ;= -10000
        ;= -1000
    INC   HL
    LD    D, (HL)
    INC   HL
    EX    (SP), HL
    PUSH  BC
MB_CONSI_SUB10K:
    LD    B, H
    LD    C, L
    ADD   HL, DE
    INC   A
    JR    C, MB_CONSI_SUB10K
    LD    H, B
    LD    L, C
    POP   BC
    POP   DE
    EX    DE, HL
    INC   C
    DEC   C
    JR    NZ, MB_CONSI_STOR
    OR    A
    JR    Z, MB_CONSI_NXT_DGT
    PUSH  AF
    LD    A, 0x40
    ADD   A, B
    LD    (MB_DACC), A
    POP   AF
MB_CONSI_STOR:
    INC   C
    EX    (SP), HL
    PUSH  AF
    LD    A, C
    RRA
    JR    NC, MB_CONSI_STOR_LO
    POP   AF
    ADD   A, A
    ADD   A, A
    ADD   A, A
    ADD   A, A
    LD    (HL), A
    JR    MATH_CONSI_RSTR
MB_CONSI_STOR_LO:
    POP   AF
    OR    (HL)
    LD    (HL), A
    INC   HL
MATH_CONSI_RSTR:
    EX    (SP), HL
MB_CONSI_NXT_DGT:
    LD    A, D
    OR    E
    JR    Z, MB_CONSI_SGN
    DJNZ  MB_CONSI_DGT
MB_CONSI_SGN:
    POP   HL
    POP   AF
    RET   P
    JP    MB_NEG
C_M10000:
    DW   -10000
C_M1000:
    DW   -1000
C_M100:
    DW   -100
C_M10:
    DW   -10
C_M1:
    DW   -1

; --------------------------------------------------
; Convert daccum to double real
; CDBL function
; --------------------------------------------------
MB_FRCDBL:
    RST   GETYPTR
    RET   NC
    JP    Z, MB_TMERR
    CALL  M, MB_CONSI

; --------------------------------------------------
MB_CONDS:
    LD    HL, 0x0
    LD    (MB_DACC+4), HL
    LD    (MB_DACC+6), HL
    LD    A, H
    LD    (MB_DACC+8), A

; --------------------------------------------------
MB_VALDBL:
    LD    A, 0x8
    JR    MB_VALCMN

; --------------------------------------------------
MB_VALSNG:
    LD    A, 0x4
MB_VALCMN:
    JP    MB_CONISD

; --------------------------------------------------
; Check if string
;  if not,  error - Type mismatch
; --------------------------------------------------
MB_CHKSTR:
    RST   GETYPTR
    RET   Z
    JP    MB_TMERR

; --------------------------------------------------
; Convert single/double real to int
; --------------------------------------------------
MB_CVF2INT:
    LD    HL, MB_CVF2INT_RET
    PUSH  HL
    LD    HL, MB_DACC
    LD    A, (HL)
    AND   0x7f
    CP    'F'
    RET   NC
    SUB   'A'
    JR    NC, MB_CVF2INT_DIG
    OR    A
    POP   DE
    LD    DE, 0x0
    RET
MB_CVF2INT_DIG:
    INC   A
    LD    B, A
    LD    DE, 0x0
    LD    C, D
    INC   HL
MB_CVF2INT_SHFT:
    LD    A, C
    INC   C
    RRA
    LD    A, (HL)
    JR    C, MB_CVF2INT_NXT_BYTE
    RRA
    RRA
    RRA
    RRA
    JR    MB_CVF2INT_SCALE
MB_CVF2INT_NXT_BYTE:
    INC   HL
MB_CVF2INT_SCALE:
    AND   0xf
    LD    (MB_DECTMP), HL
    LD    H, D
    LD    L, E
    ADD   HL, HL
    RET   C
    ADD   HL, HL
    RET   C
    ADD   HL, DE
    RET   C
    ADD   HL, HL
    RET   C
    LD    E, A
    LD    D, 0x0
    ADD   HL, DE
    RET   C
    EX    DE, HL
    LD    HL, (MB_DECTMP)
    DJNZ  MB_CVF2INT_SHFT
    LD    HL, 0x8000
    RST   DCOMPR
    LD    A, (MB_DACC)
    RET   C
    JR    Z, MB_CVF2INT_SGN
    POP   HL
    OR    A
    RET   P
    EX    DE, HL
    CALL  MB_INEGHL
    EX    DE, HL
    OR    A
    RET

; --------------------------------------------------
MB_CVF2INT_SGN:
    OR    A
    RET   P
    POP   HL
    RET

; --------------------------------------------------
MB_CVF2INT_RET:
    SCF
    RET
    DEC   BC
    RET

; --------------------------------------------------
; Round daccum
; FIX function
; --------------------------------------------------
MB_FIXER:
    RST   GETYPTR
    RET   M
    RST   SIGN
    JP    P, MB_INT
    CALL  MB_NEG
    CALL  MB_INT
    JP    MB_VNEG

; --------------------------------------------------
; INT Function
; --------------------------------------------------
MB_INT:
    RST   GETYPTR
    RET   M
    LD    HL, MB_DACC+8
    LD    C, 14
    JR    NC, MB_INT_RL
    JP    Z, MB_TMERR
    LD    HL, MB_DACC+4
    LD    C, 0x6
MB_INT_RL:
    LD    A, (MB_DACC)
    OR    A
    JP    M, MB_INT_MS
    AND   0x7f
    SUB   'A'
    JP    C, MB_ACC_ZERO
    INC   A
    SUB   C
    RET   NC
    CPL
    INC   A
    LD    B, A
MB_INT_NXB1:
    DEC   HL
    LD    A, (HL)
    AND   0xf0
    LD    (HL), A
    DEC   B
    RET   Z
    XOR   A
    LD    (HL), A
    DJNZ  MB_INT_NXB1
    RET

; --------------------------------------------------
MB_INT_MS:
    AND   0x7f
    SUB   'A'
    JR    NC, MB_INT_SETUP
    LD    HL, 0xffff
    JP    MB_MAKINT
MB_INT_SETUP:
    INC   A
    SUB   C
    RET   NC
    CPL
    INC   A
    LD    B, A
    LD    E, 0x0
MB_INT_NXB2:
    DEC   HL
    LD    A, (HL)
    LD    D, A
    AND   0xf0
    LD    (HL), A
    CP    D
    JR    Z, MB_INT_NIB_NXT
    INC   E
MB_INT_NIB_NXT:
    DEC   B
    JR    Z, MB_INT_NB2E
    XOR   A
    LD    (HL), A
    CP    D
    JR    Z, MB_INT_JCHK
    INC   E
MB_INT_JCHK:
    DJNZ  MB_INT_NXB2
MB_INT_NB2E:
    INC   E
    DEC   E
    RET   Z
    LD    A, C
    CP    0x6
    LD    BC, MB_POPSTF
    LD    DE, 0x0
    JP    Z, MB_FADD
    EX    DE, HL
    LD    (MB_ARG+6), HL
    LD    (MB_ARG+4), HL
    LD    (MB_ARG+2), HL
    LD    H, B
    LD    L, C
    LD    (MB_ARG), HL
    JP    MB_DADD

; --------------------------------------------------
; Unsigned int multiply
; --------------------------------------------------
MB_UMULT:
    PUSH  HL
    LD    HL, 0x0
    LD    A, B
    OR    C
    JR    Z, MB_UMUL_DONE
    LD    A, 0x10
MB_UMUL_SHIFT:
    ADD   HL, HL
    JP    C, MB_BSERR
    EX    DE, HL
    ADD   HL, HL
    EX    DE, HL
    JR    NC, MB_UMUL_ADD
    ADD   HL, BC
    JP    C, MB_BSERR
MB_UMUL_ADD:
    DEC   A
    JR    NZ, MB_UMUL_SHIFT
MB_UMUL_DONE:
    EX    DE, HL
    POP   HL
    RET

; --------------------------------------------------
; Substract Integer
; ISUB
; --------------------------------------------------
MB_ISUB:
    LD    A, H
    RLA
    SBC   A, A
    LD    B, A
    CALL  MB_INEGHL
    LD    A, C
    SBC   A, B
    JR    MB_IADD_CALC

; --------------------------------------------------
; Add Integer
; IADD
; --------------------------------------------------
MB_IADD:
    LD    A, H
    RLA
    SBC   A, A
MB_IADD_CALC:
    LD    B, A
    PUSH  HL
    LD    A, D
    RLA
    SBC   A, A
    ADD   HL, DE
    ADC   A, B
    RRCA
    XOR   H
    JP    P, MB_CONSI_EXIT
    PUSH  BC
    EX    DE, HL
    CALL  MB_CONSIH
    POP   AF
    POP   HL
    CALL  MB_PUSHF
    CALL  MB_CONSIH
    POP   BC
    POP   DE
    JP    MB_FADD

; --------------------------------------------------
; Multiply Integer
; IMUL
; --------------------------------------------------
MB_IMUL:
    LD    A, H
    OR    L
    JP    Z, MB_MAKINT
    PUSH  HL
    PUSH  DE
    CALL  MB_IMULDV
    PUSH  BC
    LD    B, H
    LD    C, L
    LD    HL, 0x0
    LD    A, 0x10
MB_IMUL_SHFT:
    ADD   HL, HL
    JR    C, MB_IMUL_OVFL
    EX    DE, HL
    ADD   HL, HL
    EX    DE, HL
    JR    NC, MB_IMUL_ADD
    ADD   HL, BC
    JR    C, MB_IMUL_OVFL
MB_IMUL_ADD:
    DEC   A
    JR    NZ, MB_IMUL_SHFT
    POP   BC
    POP   DE
MB_IMUL_SIGN:
    LD    A, H
    OR    A
    JP    M, MB_IMUL_NEG
    POP   DE
    LD    A, B
    JP    MB_INEGA
MB_IMUL_NEG:
    XOR   0x80
    OR    L
    JR    Z, MB_IMUL_ZERO
    EX    DE, HL
    JR    MB_IMUL_TOFLT
MB_IMUL_OVFL:
    POP   BC
    POP   HL
MB_IMUL_TOFLT:
    CALL  MB_CONSIH
    POP   HL
    CALL  MB_PUSHF
    CALL  MB_CONSIH
    POP   BC
    POP   DE
    JP    MB_FMUL
MB_IMUL_ZERO:
    LD    A, B
    OR    A
    POP   BC
    JP    M, MB_MAKINT
    PUSH  DE
    CALL  MB_CONSIH
    POP   DE
    JP    MB_NEG

; --------------------------------------------------
; Integer divide
; --------------------------------------------------
MB_IDIV:
    LD    A, H
    OR    L
    JP    Z, MB_DV0ERR
    CALL  MB_IMULDV
    PUSH  BC
    EX    DE, HL
    CALL  MB_INEGHL
    LD    B, H
    LD    C, L
    LD    HL, 0x0
    LD    A, 0x11
    PUSH  AF
    OR    A
    JR    MB_IDIV3
MB_IDIV1:
    PUSH  AF
    PUSH  HL
    ADD   HL, BC
    JR    NC, MB_IDIV2
    POP   AF
    SCF
    JR    MB_IDIV3
MB_IDIV2:
    POP   HL
MB_IDIV3:
    LD    A, E
    RLA
    LD    E, A
    LD    A, D
    RLA
    LD    D, A
    LD    A, L
    RLA
    LD    L, A
    LD    A, H
    RLA
    LD    H, A
    POP   AF
    DEC   A
    JR    NZ, MB_IDIV1
    EX    DE, HL
    POP   BC
    PUSH  DE
    JP    MB_IMUL_SIGN

; --------------------------------------------------
MB_IMULDV:
    LD    A, H
    XOR   D
    LD    B, A
    CALL  MB_INEGH
    EX    DE, HL

; --------------------------------------------------
MB_INEGH:
    LD    A, H

; --------------------------------------------------
MB_INEGA:
    OR    A
    JP    P, MB_MAKINT

; --------------------------------------------------
MB_INEGHL:
    XOR   A
    LD    C, A
    SUB   L
    LD    L, A
    LD    A, C
    SBC   A, H
    LD    H, A
    JP    MB_MAKINT

; --------------------------------------------------
MB_INEG:
    LD    HL, (MB_DACC+2)
    CALL  MB_INEGHL
    LD    A, H
    XOR   0x80
    OR    L
    RET   NZ

; --------------------------------------------------
; Convert unsigned integer to single real
; --------------------------------------------------
MB_UI2SNG:
    XOR   A
    JP    MB_CONSI2

; --------------------------------------------------
; Integer MOD
; --------------------------------------------------
MB_IMOD:
    PUSH  DE
    CALL  MB_IDIV
    XOR   A
    ADD   A, D
    RRA
    LD    H, A
    LD    A, E
    RRA
    LD    L, A
    CALL  MB_SETINT
    POP   AF
    JR    MB_INEGA
MB_OVERR0:
    LD    A, B
    XOR   (HL)
    RLA
    POP   HL
    JR    MB_OVERR4

; ---------------------------------------------------
MB_OVERR1:
    LD    A, (MB_DACC)
    RLA
    JR    MB_OVERR4
MB_OVERR2:
    LD    A, (MB_DACC)
    CPL
    RLA
    JR    MB_OVERR4
MB_DIV0ERR:
    LD    A, (MB_DACC)
    RLA
MB_OVERR3:
    LD    HL, MSG_DBZ_ERR  ;= "Division by zero"
    LD    (MB_ERRMSG_PTR), HL    ;=0x488

; ---------------------------------------------------
MB_OVERR4:
    PUSH  HL
    PUSH  BC
    PUSH  DE
    PUSH  AF
    PUSH  AF
    LD    HL, (MB_ONELIN)
    LD    A, H
    OR    L
    JR    NZ, MB_OVERR6
    LD    HL, MB_FLGOVC
    LD    A, (HL)
    OR    A
    JR    Z, MB_OVERR5
    DEC   A
    JR    NZ, MB_OVERR6
    INC   (HL)
MB_OVERR5:
    LD    HL, (MB_ERRMSG_PTR)    ;=0x488
    CALL  MB_STRPRN
    LD    (MB_TTYPOS), A
    LD    A, ASCII_CR
    CALL  MB_CALTTY
    LD    A, ASCII_LF
    CALL  MB_CALTTY
MB_OVERR6:
    LD    HL, MB_DACC+7
    LD    B, 0x7
    LD    A, 0x99
MB_OVERR7:
    LD    (HL), A
    DEC   HL
    DJNZ  MB_OVERR7
    POP   AF
    LD    A, 0xff
    JR    C, MB_OVERR8
    LD    A, 0x7f
MB_OVERR8:
    LD    (HL), A
    LD    HL, (MB_ONELIN)
    LD    A, H
    OR    L
    LD    HL, (MB_ERRMSG_PTR)    ;=0x488
    LD    DE, MSG_OVR_ERR  ;= "Overflow"
    EX    DE, HL
    LD    (MB_ERRMSG_PTR), HL    ;=0x488
    JR    Z, MB_OVERR9
    RST   DCOMPR
    JP    Z, MB_OVERR
    JP    MB_DV0ERR
MB_OVERR9:
    POP   AF
    POP   DE
    POP   BC
    POP   HL
    RET
    CALL  MB_MOVRM

; --------------------------------------------------
; Single real addition
; --------------------------------------------------
MB_FADD:
    CALL  MB_BCDE_TO_ARG
    CALL  MB_CONDS
    JP    MB_DADD

; --------------------------------------------------
; Single real subtract
; --------------------------------------------------
MB_FSUB:
    CALL  MB_NEG
    JR    MB_FADD

; --------------------------------------------------
; Single real muliply
;  Out: DAC * ARG
; --------------------------------------------------
MB_FMUL:
    CALL  MB_BCDE_TO_ARG
    CALL  MB_CONDS
    JP    MB_DMULT
MB_FDIV1:
    POP   BC
    POP   DE

; --------------------------------------------------
; Single real divide
;  Out: DAC / ARG
; --------------------------------------------------
MB_FDIV:
    LD    HL, (MB_DACC+2)
    EX    DE, HL
    LD    (MB_DACC+2), HL
    PUSH  BC
    LD    HL, (MB_DACC)
    EX    (SP), HL
    LD    (MB_DACC), HL
    POP   BC
    CALL  MB_BCDE_TO_ARG
    CALL  MB_CONDS
    JP    MB_DDIV

; ---------------------------------------------------
MB_BCDE_TO_ARG:
    EX    DE, HL
    LD    (MB_ARG+2), HL
    LD    H, B
    LD    L, C
    LD    (MB_ARG), HL
    LD    HL, 0x0
    LD    (MB_ARG+4), HL
    LD    (MB_ARG+6), HL
    RET

; --------------------------------------------------
; A-- and ret
; --------------------------------------------------
MB_DECART:
    DEC   A
    RET

; --------------------------------------------------
; HL-- and ret
; --------------------------------------------------
MB_DECHLRT:
    DEC   HL
    RET

; --------------------------------------------------
; Pop HL and ret
; --------------------------------------------------
MB_POPHLRT:
    POP   HL
    RET

; --------------------------------------------------
; Convert text to number
; --------------------------------------------------
MB_FIN:
    LD    BC, MB_FINOVC
    PUSH  BC
    LD    A, 0x1
    LD    (MB_FLGOVC), A
    EX    DE, HL
    LD    BC, 0xff
    LD    H, B
    LD    L, B
    CALL  MB_MAKINT
    EX    DE, HL
    LD    A, (HL)
    CP    '&'
    JP    Z, MB_OCTCNS
    CP    '-'
    PUSH  AF
    JR    Z, MB_FINC
    CP    '+'
    JR    Z, MB_FINC
    DEC   HL
MB_FINC:
    RST   CHRGTR
    JP    C, MB_FIN_MNT_NXT
    CP    '.'
    JP    Z, MB_FINDOT
    CP    'e'
    JR    Z, MB_FINEXP
    CP    'E'
MB_FINEXP:
    JR    NZ, MB_FIN_NEX
    PUSH  HL
    RST   CHRGTR
    CP    'l'
    JR    Z, MB_FIN_SFX_OK
    CP    'L'
    JR    Z, MB_FIN_SFX_OK
    CP    'q'
    JR    Z, MB_FIN_SFX_OK
    CP    'Q'
MB_FIN_SFX_OK:
    POP   HL
    JR    Z, MB_FIN_TSET
    RST   GETYPTR
    JR    NC, MB_FIN_DBL
    XOR   A
    JR    MB_FINEX1
MB_FIN_TSET:
    LD    A, (HL)
MB_FIN_NEX:
    CP    '%'
    JP    Z, MB_FIN_INT
    CP    '#'
    JP    Z, MB_FIN_DBF
    CP    '!'
    JP    Z, MB_FIN_SNF
    CP    'd'
    JR    Z, MB_FIN_DBL
    CP    'D'
    JR    NZ, MB_FIN_NORM
MB_FIN_DBL:
    OR    A
MB_FINEX1:
    CALL  MB_FINFRC
    RST   CHRGTR
    PUSH  DE
    LD    D, 0x0
    CALL  MB_MINPLS
    LD    C, D
    POP   DE
MB_FINEC:
    RST   CHRGTR
    JR    NC, MB_N_FINEDG
    LD    A, E
    CP    0xc
    JR    NC, MB_FIN_DGT_OVF
    RLCA
    RLCA
    ADD   A, E
    RLCA
    ADD   A, (HL)
    SUB   '0'
    LD    E, A
    JR    MB_FINEC
MB_FIN_DGT_OVF:
    LD    E, 0x80
    JR    MB_FINEC
MB_N_FINEDG:
    INC   C
    JR    NZ, MB_FIN_NORM
    XOR   A
    SUB   E
    LD    E, A
MB_FIN_NORM:
    RST   GETYPTR
    JP    M, MB_FIN_SIGN
    LD    A, (MB_DACC)
    OR    A
    JR    Z, MB_FIN_SIGN
    LD    A, D
    SUB   B
    ADD   A, E
    ADD   A, 0x40
    LD    (MB_DACC), A
    OR    A
    CALL  M, MB_FIN_OVERR
MB_FIN_SIGN:
    POP   AF
    PUSH  HL
    CALL  Z, MB_VNEG
    RST   GETYPTR
    JR    NC, MB_FIN_ROUND
    POP   HL
    RET   PE
    PUSH  HL
    LD    HL, MB_POPHLRT
    PUSH  HL
    CALL  MB_CONSI_CHK
    RET

; --------------------------------------------------
MB_FIN_ROUND:
    CALL  MB_DAC_ROUND
    POP   HL
    RET

; --------------------------------------------------
MB_FIN_OVERR:
    OR    A
    JP    MB_OVERR4

; --------------------------------------------------
MB_FINDOT:
    RST   GETYPTR
    INC   C
    JR    NZ, MB_FIN_NORM
    JR    NC, MATH_FIN_FRAC
    CALL  MB_FINFRC
    LD    A, (MB_DACC)
    OR    A
    JR    NZ, MATH_FIN_FRAC
    LD    D, A

; --------------------------------------------------
MATH_FIN_FRAC:
    JP    MB_FINC

; --------------------------------------------------
MB_FIN_INT:
    RST   CHRGTR
    POP   AF
    PUSH  HL
    LD    HL, MB_POPHLRT
    PUSH  HL
    LD    HL, MB_FRCINT
    PUSH  HL
    PUSH  AF
    JR    MB_FIN_NORM

; --------------------------------------------------
MB_FIN_DBF:
    OR    A
MB_FIN_SNF:
    CALL  MB_FINFRC
    RST   CHRGTR
    JR    MB_FIN_NORM

; --------------------------------------------------
MB_FINFRC:
    PUSH  HL
    PUSH  DE
    PUSH  BC
    PUSH  AF
    CALL  Z, MB_FRCSNG
    POP   AF
    CALL  NZ, MB_FRCDBL
    POP   BC
    POP   DE
    POP   HL
    RET
MB_FIN_MNT_NXT:
    SUB   '0'
    JP    NZ, MB_FIN_DGT_CHK
    OR    C
    JP    Z, MB_FIN_DGT_CHK
    AND   D
    JP    Z, MB_FINC
MB_FIN_DGT_CHK:
    INC   D
    LD    A, D
    CP    0x7
    JR    NZ, MB_FIN_ACCUM
    OR    A
    CALL  MB_FINFRC
MB_FIN_ACCUM:
    PUSH  DE
    LD    A, B
    ADD   A, C
    INC   A
    LD    B, A
    PUSH  BC
    PUSH  HL
    LD    A, (HL)
    SUB   0x30
    PUSH  AF
    RST   GETYPTR
    JP    P, MB_FIN_ZERO_CHK
    LD    HL, (MB_DACC+2)
    LD    DE, 3277
    RST   DCOMPR
    JR    NC, MB_FIN_CONSI
    LD    D, H
    LD    E, L
    ADD   HL, HL
    ADD   HL, HL
    ADD   HL, DE
    ADD   HL, HL
    POP   AF
    LD    C, A
    ADD   HL, BC
    LD    A, H
    OR    A
    JP    M, MB_FIN_OVF_SCA
    LD    (MB_DACC+2), HL
MB_FIN_ACC_DONE:
    POP   HL
    POP   BC
    POP   DE
    JP    MB_FINC
MB_FIN_OVF_SCA:
    LD    A, C
    PUSH  AF
MB_FIN_CONSI:
    CALL  MB_CONSI
MB_FIN_ZERO_CHK:
    POP   AF
    POP   HL
    POP   BC
    POP   DE
    JR    NZ, MB_FIN_FRAC_SCA
    LD    A, (MB_DACC)
    OR    A
    LD    A, 0x0
    JR    NZ, MB_FIN_FRAC_SCA
    LD    D, A
    JP    MB_FINC
MB_FIN_FRAC_SCA:
    PUSH  DE
    PUSH  BC
    PUSH  HL
    PUSH  AF
    LD    HL, MB_DACC
    LD    (HL), 0x1
    LD    A, D
    CP    0x10
    JR    C, MB_FIN_NIB_SHIFT
    POP   AF
    JR    MB_FIN_ACC_DONE
MB_FIN_NIB_SHIFT:
    INC   A
    OR    A
    RRA
    LD    B, 0x0
    LD    C, A
    ADD   HL, BC
    POP   AF
    LD    C, A
    LD    A, D
    RRA
    LD    A, C
    JR    NC, MB_FIN_STORE_NIB
    ADD   A, A
    ADD   A, A
    ADD   A, A
    ADD   A, A
MB_FIN_STORE_NIB:
    OR    (HL)
    LD    (HL), A
    JR    MB_FIN_ACC_DONE

; ===================================================
; FLOATING POINT OUTPUT ROUTINE
; ===================================================

; ---------------------------------------------------
MB_INPRT:
    PUSH  HL
    LD    HL, MB_INTXT     ;= " in "
    CALL  MB_STROUT
    POP   HL

; ---------------------------------------------------
; Print line Number to interpreter output
;  Inp: HL - integer
; ---------------------------------------------------
MB_LINPRT:
    LD    BC, MB_STROUI
    PUSH  BC
    CALL  MB_MAKINT                                  ; Make integer in DAC
    XOR   A
    LD    (MB_TEMP3), A
    LD    HL, MB_FBUFFR+1
    LD    (HL), ASCII_SP
    OR    (HL)
    JR    MB_FOUT_INT

; ---------------------------------------------------
; Float output,  unformatted. Convert DAC to text
; ---------------------------------------------------
MB_FOUT:
    XOR   A

; ---------------------------------------------------
; Convert DAC to text,   formatted
; Inp: B - numbers befole decimal dot
;      C - numbers after decimal dot
;      A - format bits
;       bit 7 - 0-free format (B, C ignored),  1-fixed format
;       bit 6 - 1-group digits by 3 with ',' sep
;       bit 5 - 1-fill leading spaces with '*'
;       bit 4 - 1-output '$' before
;       bit 3 - 1- output '+' for positive number
;       bit 2 - 1- output sign after number
;       bit 1 - unused
;       bit 0 - 1- output in 'E' notation
; ---------------------------------------------------
MB_PUFOUT:
    CALL  MB_FOUT_SNG_PREP
    AND   0x8
    JR    Z, MB_FOUT_SGN
    LD    (HL), '+'
MB_FOUT_SGN:
    EX    DE, HL
    CALL  MB_VSIGN
    EX    DE, HL
    JP    P, MB_FOUT_INT
    LD    (HL), '-'
    PUSH  BC
    PUSH  HL
    CALL  MB_VNEG
    POP   HL
    POP   BC
    OR    H

    ; Convert number to digits
MB_FOUT_INT:
    INC   HL
    LD    (HL), '0'
    LD    A, (MB_TEMP3)
    LD    D, A
    RLA
    LD    A, (MB_VALTYP)
    JP    C, MB_FOUTFX     ;fixed format output
    JP    Z, MB_FOUT_END   ;if number is zero
    CP    TSGN
    JP    NC, MB_FOUFRV    ;single or double
    LD    BC, 0x0
    CALL  MB_FOUTCI

; ---------------------------------------------------
; Supress zerro in buffer with space or '*'
; ---------------------------------------------------
MB_FOUTZS:
    LD    HL, MB_FBUFFR+1
    LD    B, (HL)
    LD    C, ASCII_SP
    LD    A, (MB_TEMP3)
    LD    E, A
    AND   0x20
    JR    Z, MB_FOTZS1
    LD    A, B
    CP    C
    LD    C, '*'
    JR    NZ, MB_FOTZS1
    LD    A, E
    AND   0x4                                        ; Sign is trailing?
    JR    NZ, MB_FOTZS1
    LD    B, C
MB_FOTZS1:
    LD    (HL), C
    RST   CHRGTR
    JR    Z, MB_FOTZS4
    CP    'E'
    JR    Z, MB_FOTZS4
    CP    'D'
    JR    Z, MB_FOTZS4
    CP    '0'
    JR    Z, MB_FOTZS1
    CP    ','
    JR    Z, MB_FOTZS1
    CP    '.'
    JR    NZ, MB_FOTZS2
MB_FOTZS4:
    DEC   HL
    LD    (HL), '0'
MB_FOTZS2:
    LD    A, E
    AND   0x10      ;Floating '$' sign?
    JR    Z, MB_FOTZS3
    DEC   HL
    LD    (HL), '$'
MB_FOTZS3:
    LD    A, E
    AND   0x4       ;Trailing sign?
    RET   NZ
    DEC   HL
    LD    (HL), B
    RET

; ---------------------------------------------------
; Real single/double value
; ---------------------------------------------------
MB_FOUFRV:
    PUSH  HL
    CALL  MB_VALSNG1
    LD    D, B
    LD    A, (MB_DACC)
    SUB   0x40
    LD    E, A
    JR    NC, MB_FOUFRV_NORMEXP
    CPL
    INC   A
    ; normalize exponent after division
MB_FOUFRV_NORMEXP:
    LD    C, A
    CALL  MB_LOC_NZD
    ADD   A, C
    SUB   B
    LD    A, E
    LD    BC, 0x300
    JR    NC, MB_FOUFRV_DIGADJ
    INC   A
    JR    MB_FOUFRV_DIGSET
    ; adjust mantissa digits
MB_FOUFRV_DIGADJ:
    INC   D
    LD    A, (MB_DACC)
    SUB   '?'
    JR    C, MB_FOUT_DIGCNT
    INC   D
    CP    D
    JR    NC, MB_FOUT_DIGCNT
    ; set digit count
MB_FOUFRV_DIGSET:
    INC   A
    LD    B, A
    LD    A, 0x2

MB_FOUT_DIGCNT:
    SUB   0x2
    POP   HL
    PUSH  AF
    CALL  MB_FOUTAN
    LD    (HL), '0'
    CALL  Z, MB_INCHLRT
    CALL  MB_FOUTCV
MB_FOUT_TRIM0:
    DEC   HL
    LD    A, (HL)
    CP    '0'
    JR    Z, MB_FOUT_TRIM0
    CP    '.'
    CALL  NZ, MB_INCHLRT
    POP   AF
    JR    Z, MB_FOUT_TERM

; ---------------------------------------------------
; Out exponent,  put 'E' or 'D' in buffer,  exp in A
; ---------------------------------------------------
MB_FOUT_EXP:
    LD    (HL), 'E'
    INC   HL
    LD    (HL), '+'
    JP    P, MB_FOUT_EXP_DGT
    LD    (HL), '-'
    CPL
    INC   A

    ;Calc two digit exponent
MB_FOUT_EXP_DGT:
    LD    B, '0' - 1
MB_FOUT_EXP_NIB:
    INC   B                                          ; increment digit
    SUB   10
    JR    NC, MB_FOUT_EXP_NIB
    ADD   A, '0' + 10                                ; add 10 and convert to ASCII character
    INC   HL
    LD    (HL), B
    INC   HL
    LD    (HL), A
MB_FOUT_END:
    INC   HL
MB_FOUT_TERM:
    LD    (HL), 0x0
    EX    DE, HL
    LD    HL, MB_FBUFFR+1
    RET

; ---------------------------------------------------
; Print a number in fixed format
; ---------------------------------------------------
MB_FOUTFX:
    INC   HL
    PUSH  BC
    CP    0x4
    LD    A, D
    JP    NC, MB_FOUFXV
    RRA
    JP    C, MB_FOUT_PREP2
    LD    BC, 0x603                                 ; decimal point to 6 and comma to 3
    CALL  MB_FOUTCC                                 ; check usage commas
    POP   DE
    LD    A, D
    SUB   0x5
    CALL  P, MB_FOTZER
    CALL  MB_FOUTCI
MB_FOUTTD:
    LD    A, E
    OR    A
    CALL  Z, MB_DECHLRT
    DEC   A
    CALL  P, MB_FOTZER

    ;Finish up fixed format number
MB_FOUTTS:
    PUSH  HL
    CALL  MB_FOUTZS
    POP   HL
    JR    Z, MB_FFXIX1                               ; check to have trailing sign
    LD    (HL), B                                    ; put sign to buffer
    INC   HL
MB_FFXIX1:
    LD    (HL), 0x0
    LD    HL, MB_FBUFFR                              ; pointer to begining
MB_FOUBE1:
    INC   HL
MB_FOUBE5:
    LD    A, (MB_TEMP2)
    SUB   L
    SUB   D
    RET   Z
    LD    A, (HL)
    CP    ASCII_SP
    JR    Z, MB_FOUBE1
    CP    '*'
    JR    Z, MB_FOUBE1
    DEC   HL
    PUSH  HL

    ; check to ignore leading '0' before '.'
MB_FOUBE2:
    PUSH  AF
    LD    BC, MB_FOUBE2
    PUSH  BC
    RST   CHRGTR
    CP    '-'
    RET   Z
MB_FOUBE2_PLUSCHK:
    CP    '+'
MB_FOUBE2_SIGNRET:
    RET   Z
    CP    ASCII_CUR
    RET   Z
MB_FOUBE2_ZEROSKIP:
    POP   BC
    CP    '0'
    JR    NZ, MB_FOUBE4
    INC   HL
MB_FOUBE2_NXTCH:
    RST   CHRGTR
    JR    NC, MB_FOUBE4
    DEC   HL
MB_FOUBE4_1:
    JR    MB_FOUBE3_1
MB_FOUBE3:
    DEC   HL
    LD    (HL), A
MB_FOUBE3_1:
    POP   AF
    JR    Z, MB_FOUBE3
    POP   BC
    JR    MB_FOUBE5
MB_FOUBE4:
    POP   AF
    JR    Z, MB_FOUBE4
    POP   HL
    LD    (HL), '%'
    RET

; ---------------------------------------------------
; Print single or double in fixed format
; ---------------------------------------------------
MB_FOUFXV:
    PUSH  HL
    RRA
    JP    C, MB_FFXFLV
    CALL  MB_VALSNG1
    LD    D, B
    LD    A, (MB_DACC)
    SUB   0x4f
    JR    C, MB_FFXSDC
    POP   HL
    POP   BC
    CALL  MB_FOUT
    LD    HL, MB_FBUFFR
    LD    (HL), '%'
    RET

; ---------------------------------------------------
; Actualy print single or double in fixed format
; ---------------------------------------------------
MB_FFXSDC:
    RST   SIGN
    CALL  NZ, MB_FOUT_EXP_DEC
    POP   HL
    POP   BC
    JP    M, MB_FOUT_EXP2
    PUSH  BC
    LD    E, A
    LD    A, B
    SUB   D
    SUB   E
    CALL  P, MB_FOTZER
MB_FFXSDC_DOTPLAC:
    CALL  MB_FOUTCD
    CALL  MB_FOUTCV
    OR    E
    CALL  NZ, MB_FOTZEC
    OR    E
    CALL  NZ, MB_FOUTED
    POP   DE
    JP    MB_FOUTTD
MB_FOUT_EXP2:
    LD    E, A
    LD    A, C
    OR    A
    CALL  NZ, MB_DECART
    ADD   A, E
    JP    M, MB_FOUT_SCAL
    XOR   A
MB_FOUT_SCAL:
    PUSH  BC
    PUSH  AF
    CALL  M, MB_FOUT_ROUND
    POP   BC
    LD    A, E
    SUB   B
    POP   BC
    LD    E, A
    ADD   A, D
    LD    A, B
    JP    M, MB_FOUT_NEGEXP
    SUB   D
    SUB   E
    CALL  P, MB_FOTZER
    PUSH  BC
    CALL  MB_FOUTCD
    JR    MB_FOUT_DGT_NXT
MB_FOUT_NEGEXP:
    CALL  MB_FOTZER
    LD    A, C
    CALL  MB_FOUTDP
    LD    C, A
    XOR   A
    SUB   D
    SUB   E
    CALL  MB_FOTZER
    PUSH  BC
    LD    B, A
    LD    C, A
MB_FOUT_DGT_NXT:
    CALL  MB_FOUTCV
    POP   BC
    OR    C
    JR    NZ, MB_FOUT_POS
    LD    HL, (MB_TEMP2)
MB_FOUT_POS:
    ADD   A, E
    DEC   A
    CALL  P, MB_FOTZER
    LD    D, B
    JP    MB_FOUTTS
MB_FOUT_PREP2:
    PUSH  HL
    PUSH  DE
    CALL  MB_CONSI
    POP   DE

; ---------------------------------------------------
; Pring single or double in fixed format,
; floating point notation
; ---------------------------------------------------
MB_FFXFLV:
    CALL  MB_VALSNG1
    LD    E, B
    RST   SIGN
    PUSH  AF
    CALL  NZ, MB_FOUT_EXP_DEC
    POP   AF
    POP   HL
    POP   BC
    PUSH  AF
    LD    A, C
    OR    A
    PUSH  AF
    CALL  NZ, MB_DECART
    ADD   A, B
    LD    C, A
    LD    A, D
    AND   0x4
    CP    0x1
    SBC   A, A
    LD    D, A
    ADD   A, C
    LD    C, A
    SUB   E
    PUSH  AF
    JP    P, MB_FOUT_DPOS
    CALL  MB_FOUT_ROUND
    JR    NZ, MB_FOUT_DPOS
    PUSH  HL
    CALL  MB_DAC_RSHFT_BYTE1
    LD    HL, MB_DACC
    INC   (HL)
    POP   HL
MB_FOUT_DPOS:
    POP   AF
    PUSH  BC
    PUSH  AF
    JP    M, MB_FOUT_DPOS_CALC
    XOR   A
MB_FOUT_DPOS_CALC:
    CPL
    INC   A
    ADD   A, B
    INC   A
    ADD   A, D
    LD    B, A
    LD    C, 0x0
    CALL  Z, MB_FOUTAN
    CALL  MB_FOUTCV
    POP   AF
    CALL  P, MB_FOTZNC
    CALL  MB_FOUTED
    POP   BC
    POP   AF
    JR    NZ, MB_FOUT_EXP_ADJ
    CALL  MB_DECHLRT
    LD    A, (HL)
    CP    '.'
    CALL  NZ, MB_INCHLRT
    LD    (MB_TEMP2), HL
MB_FOUT_EXP_ADJ:
    POP   AF
    LD    A, (MB_DACC)
    JR    Z, MB_FOUT_EXP_END
    ADD   A, E
    SUB   B
    SUB   D
MB_FOUT_EXP_END:
    PUSH  BC
    CALL  MB_FOUT_EXP
    EX    DE, HL
    POP   DE
    JP    MB_FOUTTS

; ---------------------------------------------------
; Put some zeroes in a buffer
;  In: A - number of zeroes
;      HL -> buffer
; ---------------------------------------------------
MB_FOTZER:
    OR    A
MB_FOTZR1:
    RET   Z
    DEC   A
    LD    (HL), '0'
    INC   HL
    JR    MB_FOTZR1

; ---------------------------------------------------
MB_FOTZNC:
    JR    NZ, MB_FOTZEC
MB_FOTZRC:
    RET   Z
    CALL  MB_FOUTED
; ---------------------------------------------------
MB_FOTZEC:
    LD    (HL), '0'
    INC   HL
    DEC   A
    JR    MB_FOTZRC

; ---------------------------------------------------
; Calc possible ',' count  or 0 if not using comma
; Out: C - comma count
; ---------------------------------------------------
MB_FOUTCD:
    LD    A, E
    ADD   A, D
    INC   A
    LD    B, A
    INC   A
MB_FOTCD1:
    SUB   3   ;A MOD 3
    JR    NC, MB_FOTCD1
    ADD   A, 5                                       ; Add back last 3 and 2 for scaling
    LD    C, A                                       ; Save comma count in C

; ---------------------------------------------------
; Check for comma usage
;  Out: A <> 0 - used,  = 0 - not used
; ---------------------------------------------------
MB_FOUTCC:
    LD    A, (MB_TEMP3)                              ; get format spec
    AND   0x40                                       ; check comma usage
    RET   NZ
    LD    C, A                                       ; reset to 0 if not use
    RET

; ---------------------------------------------------
; Put decimal comma and dot in correct places
; Inp: B - decimal point count
;      C - comma count
; ---------------------------------------------------
MB_FOUTAN:
    DEC   B
    JP    P, MB_FOUTE1
    LD    (MB_TEMP2), HL
    LD    (HL), '.'
MB_FOUTD1:
    INC   HL
    LD    (HL), '0'
    INC   B
    LD    C, B
    JR    NZ, MB_FOUTD1
    INC   HL
    RET

; ---------------------------------------------------
; Check time to place decimal point or comma
; and place if needed
; ---------------------------------------------------
MB_FOUTED:
    DEC   B
MB_FOUTE1:
    JR    NZ, MB_FOUED1                              ; no,  check for comma

; ---------------------------------------------------
; Put decimal point to buffer
; ---------------------------------------------------
MB_FOUTDP:
    LD    (HL), '.'
    LD    (MB_TEMP2), HL
    INC   HL
    LD    C, B
    RET

    ;Ckeck to place comma,  and place if needed
MB_FOUED1:
    DEC   C
    RET   NZ
    LD    (HL), ','
    INC   HL
    LD    C, 0x3
    RET

; ---------------------------------------------------
; Convert SGN or DBL to decimal digits
;  Inp: B - decimal point count
;       C - comma count
;       HL -> buffer to place first digit
;  Out: A = 0
; ---------------------------------------------------
MB_FOUTCV:
    PUSH  DE
    PUSH  HL
    PUSH  BC
    CALL  MB_VALSNG1
    LD    A, B
    POP   BC
    POP   HL
    LD    DE, MB_DACC+1
    SCF
MB_FOUCD1:
    PUSH  AF
    CALL  MB_FOUTED
    LD    A, (DE)
    JR    NC, MB_FOUCD2
    RRA
    RRA
    RRA
    RRA
    JR    MB_FOUCD3
MB_FOUCD2:
    INC   DE
MB_FOUCD3:
    AND   0xf
    ADD   A, 0x30
    LD    (HL), A
    INC   HL
    POP   AF
    DEC   A
    CCF
    JR    NZ, MB_FOUCD1
    JR    MB_FOUCI_END

; ---------------------------------------------------
; Convert an integer to decimal digits
; ---------------------------------------------------
MB_FOUTCI:
    PUSH  DE
    LD    DE, MB_FOITBL                              ; = 10000
    LD    A, 5                                       ; calc 5 digits (ex: 32767)
MB_FOUCI1:
    CALL  MB_FOUTED
    PUSH  BC
    PUSH  AF
    PUSH  HL
    EX    DE, HL
    LD    C, (HL)     ;= 10000
    INC   HL
    LD    B, (HL)
    PUSH  BC
    INC   HL
    EX    (SP), HL
    EX    DE, HL
    LD    HL, (MB_DACC+2)
    LD    B, '0' - 1
MB_FOUCI2:
    INC   B
    LD    A, L
    SUB   E
    LD    L, A
    LD    A, H
    SBC   A, D
    LD    H, A
    JR    NC, MB_FOUCI2
    ADD   HL, DE
    LD    (MB_DACC+2), HL
    POP   DE
    POP   HL
    LD    (HL), B
    INC   HL
    POP   AF
    POP   BC
    DEC   A
    JR    NZ, MB_FOUCI1
MB_FOUCI_END:
    CALL  MB_FOUTED
    LD    (HL), A
    POP   DE
    RET

; ---------------------------------------------------
; Integer power of ten table
; ---------------------------------------------------
MB_FOITBL:
    DW    10000
    DW    1000
    DW    100
    DW    10
    DW    1

; ==================================================
; Output routines for Octal and Hex numbers
; ==================================================

; --------------------------------------------------
; Convert integer to octal text
; --------------------------------------------------
MB_FOUTO:
    XOR   A
    LD    B, A
    JR    MB_FOUTXT

; --------------------------------------------------
; Convert integer to hexadecimal text
; --------------------------------------------------
MB_FOUTH:
    LD    B, 0x1
MB_FOUTXT:
    PUSH  BC
    CALL  MB_FRQINT
    POP   BC
    LD    DE, MB_FBUFFR
    PUSH  DE
    XOR   A
    LD    (DE), A
    DEC   B
    INC   B
    LD    C, 6                                       ; 6 digits for oct
    JR    Z, MB_OCTONE
    LD    C, 4                                       ; 4 digits for hex

    ; HL << 1
MB_OUTHLP:
    ADD   HL, HL
    ADC   A, A
MB_OUTOLP:
    ADD   HL, HL
    ADC   A, A
    ADD   HL, HL
    ADC   A, A

    ; do first oct digit
MB_OCTONE:
    ADD   HL, HL
    ADC   A, A
    JR    NZ, MB_MAKDIG                              ; make digit if not 0
    LD    A, C
    DEC   A
    JR    Z, MB_MAKDIG                               ; digit counter is 0 (last digit)?
    LD    A, (DE)
    OR    A                                          ; printed not 0 digit?
    JR    Z, MB_NOLEAD                               ; do not print lead 0
    XOR   A
MB_MAKDIG:
    ADD   A, '0'
    CP    '9'+1                                      ; It is 'A'-'F'?
    JR    C, MB_NOTHAL                               ; No,  dont add offset
    ADD   A, 'A'-'9'-1                               ; Add offset

MB_NOTHAL:
    LD    (DE), A
    INC   DE
    LD    (DE), A
MB_NOLEAD:
    XOR   A
    DEC   C   ;All done?
    JR    Z, MB_FINOHO     ;jump if done
    DEC   B
    INC   B
    JR    Z, MB_OUTOLP
    JR    MB_OUTHLP
MB_FINOHO:
    LD    (DE), A   ;store final '0'
    POP   HL
    RET

; ---------------------------------------------------
; Get value of real number
; ---------------------------------------------------
MB_VALSNG1:
    RST   GETYPTR
    LD    HL, MB_DACC+7
    LD    B, 14
    RET   NC
    LD    HL, MB_DACC+3
    LD    B, 6
    RET

; ---------------------------------------------------
MB_FOUT_SNG_PREP:
    LD    (MB_TEMP3), A
    PUSH  AF
    PUSH  BC
    PUSH  DE
    CALL  MB_FRCDBL
    LD    HL, C_ZERO
    LD    A, (MB_DACC)
    AND   A
    CALL  Z, MB_SET_DAC
    POP   DE
    POP   BC
    POP   AF
    LD    HL, MB_FBUFFR+1
    LD    (HL), ASCII_SP
    RET

; ---------------------------------------------------
MB_FOUT_ROUND:
    PUSH  HL
    PUSH  DE
    PUSH  BC
    PUSH  AF
    CPL
    INC   A
    LD    E, A
    LD    A, 0x1
    JP    Z, MB_FOUT_RND_END
    CALL  MB_VALSNG1
    PUSH  HL

MB_FOUT_RND_SHIFT:
    CALL  MB_DAC_RSHFT_BYTE1
    DEC   E
    JR    NZ, MB_FOUT_RND_SHIFT
    POP   HL
    INC   HL
    LD    A, B
    RRCA
    LD    B, A
    CALL  MB_ROUND
    CALL  MB_LOC_NZD
MB_FOUT_RND_END:
    POP   BC
    ADD   A, B
    POP   BC
    POP   DE
    POP   HL
    RET
; --------------------------------------------------
MB_FOUT_EXP_DEC:
    PUSH  BC
    PUSH  HL
    CALL  MB_VALSNG1
    LD    A, (MB_DACC)
    SUB   0x40
    SUB   B
    LD    (MB_DACC), A
    POP   HL
    POP   BC
    OR    A
    RET

; ---------------------------------------------------
; Locate the last non-zero digit of the mantissa
; stored in the DAC
; ---------------------------------------------------
MB_LOC_NZD:
    PUSH  BC
    CALL  MB_VALSNG1
MB_LNZ_NXT:
    LD    A, (HL)
    AND   0xf
    JR    NZ, MB_LNZ_FOUND
    DEC   B
    LD    A, (HL)
    OR    A
    JR    NZ, MB_LNZ_FOUND
    DEC   HL
    DJNZ  MB_LNZ_NXT
MB_LNZ_FOUND:
    LD    A, B
    POP   BC
    RET

; ---------------------------------------------------
; Single real to the power
; ---------------------------------------------------
MB_FEXP:
    CALL  MB_BCDE_TO_ARG
    CALL  MB_CONDS
    CALL  MB_PUSH_ARG
    CALL  MB_EX_DACSTK
    CALL  MB_POP_ARG

; ---------------------------------------------------
; Double real to the power
; ---------------------------------------------------
MB_DEXP:
    PUSH  HL
    LD    HL, MB_CLROVC
    EX    (SP), HL
    LD    A, 0x1
    LD    (MB_FLGOVC), A
    LD    A, (MB_ARG)
    OR    A
    JP    Z, MB_IEXP_ONE
    LD    H, A
    LD    A, (MB_DACC)
    OR    A
    JP    Z, MB_IEXP_NEGCHK
    CALL  MB_PUSH_DACC
    CALL  MB_DBL_AS_INT
    JR    C, MB_FPOW
    EX    DE, HL
    LD    (MB_CPMFIL), HL
    CALL  MB_VALDBL
    CALL  MB_POP_ARG
    CALL  MB_DBL_AS_INT
    CALL  MB_VALDBL
    LD    HL, (MB_CPMFIL)
    JP    NC, MB_IEXP_START
    LD    A, (MB_ARG)
    PUSH  AF
    PUSH  HL
    CALL  MB_ARG_TO_DAC
    LD    HL, MB_FBUFFR
    CALL  MB_GET_DAC
    LD    HL, C_ONE
    CALL  MB_SET_DAC
    POP   HL
    LD    A, H
    OR    A
    PUSH  AF
    JP    P, MB_IEXP_DAC
    XOR   A
    LD    C, A
    SUB   L
    LD    L, A
    LD    A, C
    SBC   A, H
    LD    H, A
MB_IEXP_DAC:
    PUSH  HL
    JP    MB_IEXP_NXT

; ---------------------------------------------------
MB_FPOW:
    CALL  MB_VALDBL
    CALL  MB_ARG_TO_DAC
    CALL  MB_EX_DACSTK
    CALL  MB_LOG
    CALL  MB_POP_ARG
    CALL  MB_DMULT
    JP    MB_EXP

; ---------------------------------------------------
; Integer to the power
; ---------------------------------------------------
MB_IEXP:
    LD    A, H
    OR    L
    JR    NZ, MB_IEXP_CHK
MB_IEXP_ONE:
    LD    HL, 0x1
    JP    MB_IEXP_TO_DAC
MB_IEXP_CHK:
    LD    A, D
    OR    E
    JR    NZ, MB_IEXP_START
MB_IEXP_NEGCHK:
    LD    A, H
    RLA
    JR    NC, MB_IEXP_ZERO
    CALL  MB_VALDBL
    OR    A
    JP    MB_OVERR3
MB_IEXP_ZERO:
    LD    HL, 0x0
MB_IEXP_TO_DAC:
    JP    MB_MAKINT
MB_IEXP_START:
    LD    (MB_CPMFIL), HL
    PUSH  HL
    LD    HL, MB_CLROVC
    EX    (SP), HL
    LD    A, 0x1
    LD    (MB_FLGOVC), A
    PUSH  DE
    LD    A, H
    OR    A
    PUSH  AF
    CALL  M, MB_INEGHL
    LD    B, H
    LD    C, L
    LD    HL, 0x1
MB_IEXP_SQ:
    OR    A
    LD    A, B
    RRA
    LD    B, A
    LD    A, C
    RRA
    LD    C, A
    JR    NC, MB_IEXP_MUL
    CALL  MB_IMUL_CRI
    JR    NZ, MB_IEXP_OVF
MB_IEXP_MUL:
    LD    A, B
    OR    C
    JR    Z, MB_IEXP_END
    PUSH  HL
    LD    H, D
    LD    L, E
    CALL  MB_IMUL_CRI
    EX    DE, HL
    POP   HL
    JR    Z, MB_IEXP_SQ
    PUSH  BC
    PUSH  HL
    LD    HL, MB_FBUFFR
    CALL  MB_GET_DAC
    POP   HL
    CALL  MB_CONSIH
    CALL  MB_CONDS
MB_IEXP_NXT:
    POP   BC
    LD    A, B
    OR    A
    RRA
    LD    B, A
    LD    A, C
    RRA
    LD    C, A
    JR    NC, MB_IEXP_ADV
    PUSH  BC
    LD    HL, MB_FBUFFR
    CALL  MB_DMUL_HL
    POP   BC
MB_IEXP_ADV:
    LD    A, B
    OR    C
    JR    Z, MB_IEXP_END
    PUSH  BC
    CALL  MB_PUSH_DACC
    LD    HL, MB_FBUFFR
    PUSH  HL
    CALL  MB_SET_DAC
    POP   HL
    PUSH  HL
    CALL  MB_DMUL_HL
    POP   HL
    CALL  MB_GET_DAC
    CALL  MB_POP_DACC
    JR    MB_IEXP_NXT

; ---------------------------------------------------
MB_IEXP_OVF:
    PUSH  BC
    PUSH  DE
    CALL  MB_FRCDBL
    CALL  MB_DAC_TO_ARG
    POP   HL
    CALL  MB_CONSIH
    CALL  MB_CONDS
    LD    HL, MB_FBUFFR
    CALL  MB_GET_DAC
    CALL  MB_ARG_TO_DAC
    POP   BC
    JR    MB_IEXP_ADV
MB_IEXP_END:
    POP   AF
    POP   BC
    RET   P
    LD    A, (MB_VALTYP)
    CP    0x2
    JR    NZ, MB_IEXP_SIGN2
    PUSH  BC
    CALL  MB_CONSIH
    CALL  MB_CONDS
    POP   BC
MB_IEXP_SIGN2:
    LD    A, (MB_DACC)
    OR    A
    JR    NZ, MB_IEXP_FRAC
    LD    HL, (MB_CPMFIL)
    OR    H
    RET   P
    LD    A, L
    RRCA
    AND   B
    RLA
    JP    MB_OVERR4
MB_IEXP_FRAC:
    CALL  MB_DAC_TO_ARG
    LD    HL, C_ONE
    CALL  MB_SET_DAC
    JP    MB_DDIV

; ---------------------------------------------------
; Int multiplication and check result for Int type
; ---------------------------------------------------
MB_IMUL_CRI:
    PUSH  BC
    PUSH  DE
    CALL  MB_IMUL
    LD    A, (MB_VALTYP)
    CP    0x2
    POP   DE
    POP   BC
    RET

; ---------------------------------------------------
; Check for Double can be interpreted as Int for
; faster Exp
; ---------------------------------------------------
MB_DBL_AS_INT:
    CALL  MB_ARG_TO_DAC
    CALL  MB_PUSH_ARG
    CALL  MB_INT
    CALL  MB_POP_ARG
    CALL  MB_XDCOMP
    SCF
    RET   NZ
    JP    MB_CVF2INT
; ===================================================
; END OF FLOAT LIB
; ===================================================
