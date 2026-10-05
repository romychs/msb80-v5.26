; ===================================================
; Microsoft Basic-80 v5.26, for CP/M OS, for Triumph
; Adler Alphatronic PC.
;
; Main code implementation 1
; Disassembled by Romychs @ 2026-09-28
; ===================================================

MB_FNDFOR:
    LD    HL, 0x4
    ADD   HL, SP
MB_LOOPER:
    LD    A, (HL)
    INC   HL
    CP    _WHILE
    JR    NZ, MB_LOPFOR
    LD    BC, 0x6
    ADD   HL, BC
    JR    MB_LOOPER
MB_LOPFOR:
    CP    _FOR
    RET   NZ
    LD    C, (HL)
    INC   HL
    LD    B, (HL)
    INC   HL
    PUSH  HL
    LD    H, B
    LD    L, C
    LD    A, D
    OR    E
    EX    DE, HL
    JR    Z, MB_LOOP_E
    EX    DE, HL
    RST   DCOMPR                                    ;Ex: MB_DCOMPR
MB_LOOP_E:
    LD    BC, FORSIZ
    POP   HL
    RET   Z
    ADD   HL, BC
    JR    MB_LOOPER
MB_READYR:
    LD    BC, MB_STPRDY
    JP    MB_ERESET
MB_PRGEND:
    LD    HL, (MB_CURLIN)                           ;=0xFFFE
    LD    A, H
    AND   L
    INC   A
    JR    Z, MB_PRGEND_NORUN
    LD    A, (MB_ONEFLG)
    OR    A
    LD    E, ERRNR
    JR    NZ, MB_ERROR
MB_PRGEND_NORUN:
    JP    MB_ENDCON
    JR    MB_ERROR

; --------------------------------------------------
MB_ERR_INPP:
    LD    HL, (MB_DATLIN)
    LD    (MB_CURLIN), HL  ;=0xFFFE
MB_SNERR:
    LD    E, ERRSN
    DB    LD_BC_nn
MB_DV0ERR:
    LD    E, ERRDV0
    DB    LD_BC_nn
MB_NFERR:
    LD    E, ERRNF
    DB    LD_BC_nn
MB_DDERR:
    LD    E, ERRDD
    DB    LD_BC_nn
MB_UFERR:
    LD    E, ERRUF   ;Undefined user function
    DB    LD_BC_nn
MB_REERR:
    LD    E, ERRRE
    DB    LD_BC_nn
MB_OVERR:
    LD    E, ERROV
    DB    LD_BC_nn
MB_MOERR:
    LD    E, ERRMO
    DB    LD_BC_nn
MB_TMERR:
    LD    E, ERRTM

; --------------------------------------------------
; Basic error
;  Inp: E - error number
; --------------------------------------------------
MB_ERROR:
    XOR   A
    LD    (MB_NLONLY), A
    LD    HL, (MB_CURLIN)                           ;=0xFFFE
    LD    (MB_ERRLIN), HL
    XOR   A
    LD    (MB_MRGFLG), A
    LD    (MB_CHNFLG), A
    LD    A, H
    AND   L
    INC   A
    JR    Z, MB_ERRESM
    LD    (MB_DOT), HL
MB_ERRESM:
    LD    BC, MB_ERRMOR
MB_ERESET:
    LD    HL, (MB_SAVSTK)
    JP    MB_STKERR
MB_ERRMOR:
    POP   BC
    LD    A, E
    LD    C, E
    LD    (MB_ERRFLG), A
    LD    HL, (MB_SAVTXT)
    LD    (MB_ERRTXT), HL
    EX    DE, HL
    LD    HL, (MB_ERRLIN)
    LD    A, H
    AND   L
    INC   A
    JR    Z, MB_NTMDCN
    LD    (MB_OLDLIN), HL
    EX    DE, HL
    LD    (MB_OLDTXT), HL
MB_NTMDCN:
    LD    HL, (MB_ONELIN)
    LD    A, H
    OR    L
    EX    DE, HL
    LD    HL, MB_ONEFLG
    JR    Z, MB_NOTRAP
    AND   (HL)
    JR    NZ, MB_NOTRAP
    DEC   (HL)
    EX    DE, HL
    JP    MB_GONE4

; --------------------------------------------------
MB_NOTRAP:
    XOR   A
    LD    (HL), A
    LD    E, C
    CALL  MB_CRDONZ
    LD    HL, MB_ERRTAB
    LD    A, E
    CP    LSTERR
    JR    NC, MB_UPERR
    CP    DSKERR + 1
    JR    NC, MB_NTDER2
    CP    NONDSK + 1
    JR    C, MB_NTDERR
MB_UPERR:
    LD    A, ERRUE + DSKERR - NONDSK
MB_NTDER2:
    SUB   DSKERR - NONDSK                           ; FIX OFFSET INTO TABLE OF MES...
    LD    E, A
MB_NTDERR:
    CALL  MB_REM_ELSE
    INC   HL
    DEC   E
    JR    NZ, MB_NTDERR
    PUSH  HL
    LD    HL, (MB_ERRLIN)
    EX    (SP), HL
MB_ERRFIN:
    LD    A, (HL)                                   ;= "NEXT without FOR"
    CP    '?'
    JR    NZ, MB_ERRFN1
    POP   HL
    LD    HL, MB_ERRTAB
    JR    MB_UPERR

; --------------------------------------------------
MB_ERRFN1:
    CALL  MB_STROUT
    POP   HL
    LD    A, H
    AND   L
    INC   A
    CALL  NZ, MB_INPRT
    LD    A, 0xff
    RST   OUTDO
    DB    '>'
MB_STPRDY:
    POP   BC
MB_READY:
    CALL  MB_FINLPT
    CALL  MB_PRGFIN_1
    CALL  MB_CRDONZ
    LD    HL, MB_OKTXT                              ;= "Ok\"
    DB    CALL_nn
MB_INIADR1:
    DW    MB_INIT
    LD    A, (MB_ERRFLG)
    SUB   ERRSN
    CALL  Z, MB_ERREDT
MB_MAIN:
    LD    HL, 0xffff
    LD    (MB_CURLIN), HL                           ; Interpreter in direct mode
    LD    HL, MB_ENDPRG                             ;= ':'
    LD    (MB_SAVTXT), HL
    LD    A, (MB_AUTFLG)
    OR    A
    JR    Z, MB_NTAUTO
    LD    HL, (MB_AUTLIN)
    PUSH  HL
    CALL  MB_LINPRT
    POP   DE
    PUSH  DE
    CALL  MB_FNDLIN
    LD    A, '*'
    JR    C, MB_AUTELN
    LD    A, ASCII_SP

MB_AUTELN:
    RST   OUTDO
    LD    (MB_AUTFLG), A

MB_NTAUTO:
    CALL  MB_INLIN
    JR    NC, MB_AUTGOD
    XOR   A
    LD    (MB_AUTFLG), A
    JP    MB_MAIN

MB_AUTGOD:
    RST   CHRGTR
    INC   A
    DEC   A
    JR    Z, MB_MAIN
    PUSH  AF
    CALL  MB_LINGET
    JR    NC, MB_LINEN_OK
    CALL  MB_ISFLIO
    JP    Z, MB_SNERR

MB_LINEN_OK:
    CALL  MB_SKIPSP
    LD    A, (MB_AUTFLG)
    OR    A
    JR    Z, MB_NOAUTON
    CP    '*'
    JR    NZ, MB_NOAUTON
    CP    (HL)
    CALL  Z, MB_INCHLRT

MB_NOAUTON:
    LD    A, D
    OR    E
    JP    Z, MB_EDENT
    LD    A, (HL)
    CP    ASCII_SP
    CALL  Z, MB_INCHLRT

; --------------------------------------------------
MB_EDENT:
    PUSH  DE
    CALL  MB_CRUNCH
    POP   DE
    POP   AF
    LD    (MB_SAVTXT), HL
    JR    C, MB_SAINCOFF
    XOR   A
    LD    (MB_AUTFLG), A
    JP    MB_HDIOP                                  ; Handle direct input op

; --------------------------------------------------
MB_SAINCOFF:
    PUSH  DE
    PUSH  BC
    RST   CHRGTR
    OR    A
    PUSH  AF
    LD    (MB_DOT), DE
    LD    HL, (MB_AUTINC)
    ADD   HL, DE
    JR    C, MB_LNISOVR
    PUSH  DE
    LD    DE, 65530
    RST   DCOMPR
    POP   DE
    JR    NC, MB_LNISOVR
    LD    (MB_AUTLIN), HL
    JR    MB_ALNCONT

; --------------------------------------------------
MB_LNISOVR:
    XOR   A
    LD    (MB_AUTFLG), A

MB_ALNCONT:
    CALL  MB_FNDLIN
    JR    C, MB_FONLN
    POP   AF
    PUSH  AF
    JP    NZ, MB_NFNEL
    LD    A, (MB_AUTFLG)
    OR    A
    JP    Z, MB_USERR
    PUSH  BC
    JP    MB_FINI

; -------------------------------------------------
MB_NFNEL:
    OR    A
    JR    MB_DEL_LIN_CMT

; -------------------------------------------------
MB_FONLN:
    POP   AF
    PUSH  AF
    JP    NZ, MB_FNELR
    LD    A, (MB_AUTFLG)
    OR    A
    JP    Z, MB_FNELR
    PUSH  BC
    JP    MB_FINI

; --------------------------------------------------
; Found + no auto + empty line, flag line removeable
; --------------------------------------------------
MB_FNELR:
    SCF
MB_DEL_LIN_CMT:
    PUSH  BC
    PUSH  AF
    PUSH  HL
    CALL  MB_DEPTR
    POP   HL
    POP   AF
    POP   BC
    PUSH  BC
    CALL  C, MB_DEL
    POP   DE
    POP   AF
    PUSH  DE
    JR    Z, MB_FINI
    POP   DE
    LD    A, (MB_CHNFLG)
    OR    A
    JR    NZ, MB_LEVFRE
    LD    HL, (MB_MEMSIZ)
    LD    (MB_FRETOP), HL
MB_LEVFRE:
    LD    HL, 0
    LD    (MB_ONELIN), HL
    LD    HL, (MB_VARTAB)
    EX    (SP), HL
    POP   BC
    PUSH  HL
    ADD   HL, BC
    PUSH  HL
    CALL  MB_BLTU
    POP   HL
    LD    (MB_VARTAB), HL
    EX    DE, HL
    LD    (HL), H
    POP   BC
    POP   DE
    PUSH  HL
    INC   HL
    INC   HL
    LD    (HL), E
    INC   HL
    LD    (HL), D
    INC   HL
    LD    DE, MB_KBUF
    DEC   BC
    DEC   BC
    DEC   BC
    DEC   BC
MB_KBUF_MOVE_NXT:
    LD    A, (DE)
    LD    (HL), A
    INC   HL
    INC   DE
    DEC   BC
    LD    A, C
    OR    B
    JR    NZ, MB_KBUF_MOVE_NXT

; --------------------------------------------------
MB_FINI:
    POP   DE
    CALL  MB_CHEAD
    LD    HL, (MB_PTRFIL)
    LD    (MB_TEMP2), HL
    CALL  MB_RUNC
    LD    HL, (MB_TEMP2)
    LD    (MB_PTRFIL), HL
    JP    MB_MAIN

; --------------------------------------------------
; Setup BASIC line links
; --------------------------------------------------
MB_LINKER:
    LD    HL, (MB_TXTTAB)                           ;=0x733F
    EX    DE, HL
MB_CHEAD:
    LD    H, D
    LD    L, E
    LD    A, (HL)
    INC   HL
    OR    (HL)
    RET   Z
    INC   HL
    INC   HL
MB_CZLOOP:
    INC   HL
    LD    A, (HL)
MB_CZLOO2:
    OR    A
    JR    Z, MB_CZLIN
    CP    ASCII_SP
    JR    NC, MB_CZLOOP
    CP    OCTCON
    JR    C, MB_CZLOOP
    CALL  MB_CHRGT2
    RST   CHRGTR
    JR    MB_CZLOO2
; --------------------------------------------------
MB_CZLIN:
    INC   HL
    EX    DE, HL
    LD    (HL), E
    INC   HL
    LD    (HL), D
    JR    MB_CHEAD
; --------------------------------------------------
MB_SCNLIN:
    LD    DE, 0
    PUSH  DE
    JR    Z, MB_ALLLST
    POP   DE
    CALL  MB_LINSPC
    PUSH  DE
    JR    Z, MB_SNGLIN
    RST   SYNCHR                                    ; MB_SYNCHR
    DB    MINUTK
MB_ALLLST:
    LD    DE, 65530
    CALL  NZ, MB_LINSPC
    JP    NZ, MB_SNERR
MB_SNGLIN:
    EX    DE, HL
    POP   DE
MB_FNDLN1:
    EX    (SP), HL
    PUSH  HL

; --------------------------------------------------
; Search Line Number
; --------------------------------------------------
MB_FNDLIN:
    LD    HL, (MB_TXTTAB)                           ;=0x733F
MB_FL_LOOP:
    LD    B, H
    LD    C, L
    LD    A, (HL)
    INC   HL
    OR    (HL)
    DEC   HL
    RET   Z
    INC   HL
    INC   HL
    LD    A, (HL)
    INC   HL
    LD    H, (HL)
    LD    L, A
    RST   DCOMPR
    LD    H, B
    LD    L, C
    LD    A, (HL)
    INC   HL
    LD    H,(HL)
    LD    L, A
    CCF
    RET   Z
    CCF
    RET   NC
    JR    MB_FL_LOOP

; --------------------------------------------------
; Crunch BASIC code
; --------------------------------------------------
MB_CRUNCH:
    XOR   A
    LD    (MB_DORES), A
    LD    (MB_OPRTYP), A
    LD    BC, KBFLEN - 3
    LD    DE, MB_KBUF
MB_K_LOOP:
    LD    A, (HL)
    OR    A
    JR    NZ, MB_CR_EMP

MB_CRDONE:
    LD    HL, KBFLEN + 2
    LD    A, L
    SUB   C
    LD    C, A
    LD    A, H
    SBC   A, B
    LD    B, A
    LD    HL, MB_RST_INP                            ;= ':'
    XOR   A
    LD    (DE), A
    INC   DE
    LD    (DE), A
    INC   DE
    LD    (DE), A
    RET

; --------------------------------------------------
MB_CR_EMP:
    CP    '"'
    JP    Z, MB_STRNG
    CP    ASCII_SP
    JR    Z, MB_STUFFH
    LD    A, (MB_OPRTYP)
    OR    A
    LD    A, (HL)
    JR    Z, MB_CHK_Q

; Advances past the current source character, writes
; it to the crunch buffer while checking for overflow
; and if that character was a colon or REM statement
; marker, it updates the operator type and resume flag
; so line numbers get re-evaluate
; --------------------------------------------------
MB_STUFFH:
    INC   HL
    PUSH  AF
    CALL  MB_KRNSAV
    POP   AF
    SUB   ':'
    JR    Z, MB_COLIS
    CP    _DATA - ':'
    JR    NZ, MB_NODATT
    LD    A, 1
MB_COLIS:
    LD    (MB_OPRTYP), A
    LD    (MB_DORES), A
MB_NODATT:
    SUB   _REM - ':'
    JR    NZ, MB_K_LOOP
    PUSH  AF
MB_STR1:
    LD    A, (HL)
    OR    A
    EX    (SP), HL
    LD    A, H
    POP   HL
    JR    Z, MB_CRDONE
    CP    (HL)
    JR    Z, MB_STUFFH

; --------------------------------------------------
MB_STRNG:
    PUSH  AF
    LD    A, (HL)

; --------------------------------------------------
MB_STRNG2:
    INC   HL
    CALL  MB_KRNSAV
    JR    MB_STR1

; --------------------------------------------------
MB_CHK_Q:
    INC   HL
    OR    A
    JP    M, MB_K_LOOP
    DEC   HL
    CP    '?'
    LD    A, _PRINT
    PUSH  DE
    PUSH  BC
    JP    Z, MB_NOTFN2
    LD    DE, MB_SPCTAB                             ;=0xAB
    CALL  MB_MAKUPL
    CALL  MB_ISLET2
    JP    C, MB_TSTNUM
    PUSH  HL
    LD    DE, MB_ST_GO                              ;= "GO "
    CALL  MB_SUB_ADR
    JR    NZ, MB_NOTGOS
    RST   CHRGTR
    LD    DE, MB_ST_TO                              ;= "TO"
    CALL  MB_SUB_ADR
    LD    A, _GOTO
    JP    Z, MB_DO_GTOSUB
    LD    DE, MB_ST_UB                              ;= "UB"
    CALL  MB_SUB_ADR
    JR    NZ, MB_NOTGOS
    LD    A, _GOSUB
MB_DO_GTOSUB:
    POP   BC
    JP    MB_NOTFN2

; --------------------------------------------------
MB_SUB_ADR:
    LD    A, (DE)
    OR    A
    RET   Z
    LD    C, A
    CALL  MB_MAKUPL
    CP    C
    RET   NZ
    INC   HL
    INC   DE
    JR    MB_SUB_ADR

; --------------------------------------------------
MB_ST_GO:
    DB    "GO ", 0
MB_ST_TO:
    DB    "TO", 0
MB_ST_UB:
    DB    "UB", 0

; --------------------------------------------------
MB_NOTGOS:
    POP   HL
    CALL  MB_MAKUPL
    PUSH  HL
    LD    HL, MB_ALPTAB                             ;=0x165
    SUB   'A'
    ADD   A, A
    LD    C, A
    LD    B, 0
    ADD   HL, BC
    LD    E, (HL)                                   ;=0xC
    INC   HL
    LD    D, (HL)                                   ;= "HR", A4h
    POP   HL
    INC   HL
MB_TRYAGA:
    PUSH  HL
MB_LOPPSI:
    CALL  MB_MAKUPL
    LD    C, A
    LD    A, (DE)
    AND   0x7f
    JP    Z, MB_NOTRES
    INC   HL
    CP    C
    JR    NZ, MB_LOPSKP
    LD    A, (DE)
    INC   DE
    OR    A
    JP    P, MB_LOPPSI
    LD    A, C
    CP    '('
    JR    Z, MB_ISRESW
    LD    A, (DE)
    CP    _FN
    JR    Z, MB_ISRESW
    CP    _USR
    JR    Z, MB_ISRESW
    CALL  MB_MAKUPL
    CP    '.'
    JR    Z, MB_ISVARS
    CALL  MB_TSTANM
MB_ISVARS:
    LD    A, 0
    JP    NC, MB_NOTRES
MB_ISRESW:
    POP   AF
    LD    A, (DE)
    OR    A
    JP    M, MB_NOTFNT
    POP   BC
    POP   DE
    OR    0x80
    PUSH  AF
    LD    A, 0xff
    CALL  MB_KRNSAV
    XOR   A
    LD    (MB_DORES), A
    POP   AF
    CALL  MB_KRNSAV
    JP    MB_K_LOOP
MB_LOPSKP:
    POP   HL
MB_LOPSK2:
    LD    A, (DE)
    INC   DE
    OR    A
    JP    P, MB_LOPSK2
    INC   DE
    JP    MB_TRYAGA

; --------------------------------------------------
MB_NOTFNT:
    DEC   HL
MB_NOTFN2:
    PUSH  AF
    LD    DE, MB_TOK_LN                             ;=0x8C
    LD    C, A
MB_NX_TK:
    LD    A, (DE)                                   ;=0x8C
    OR    A
    JR    Z, MB_TK_END
    INC   DE
    CP    C
    JR    NZ, MB_NX_TK
    JR    MB_NOTRS2

; --------------------------------------------------
; Tokens with line number as operand
; --------------------------------------------------
MB_TOK_LN:
    DB    _RESTORE
    DB    _AUTO
    DB    _RENUM
    DB    _DELETE
    DB    _EDIT
    DB    _RESUME
    DB    _ERL
    DB    _ELSE
    DB    _RUN
    DB    _LIST
    DB    _LLIST
    DB    _GOTO
    DB    _THEN
    DB    _GOSUB
    DB    0

; --------------------------------------------------
MB_TK_END:
    XOR   A
    DB    JP_NZ_nn
MB_NOTRS2:
    LD    A, 1
MB_NOTRS6:
    LD    (MB_DORES), A
    POP   AF
    POP   BC
    POP   DE
    CP    _ELSE
    PUSH  AF
    CALL  Z, MB_KRNSVC
    POP   AF
    CP    _WHILE
    JR    NZ, MB_NOWLE
    CALL  MB_KRNSAV
    LD    A, PLUSTK
MB_NOWLE:
    CP    SNGQTK
    JP    NZ, MB_NTSNGT
    PUSH  AF
    CALL  MB_KRNSVC
    LD    A, _REM
    CALL  MB_KRNSAV
    POP   AF
    PUSH  HL
    LD    HL, 0
    EX    (SP), HL
    JP    MB_STRNG2
; --------------------------------------------------
MB_TSTNUM:
    LD    A, (HL)
    CP    '.'
    JR    Z, MB_NUMTRY
    CP    '9' + 1
    JP    NC, MB_SRCSPC
    CP    '0'
    JP    C, MB_SRCSPC
; --------------------------------------------------
MB_NUMTRY:
    LD    A, (MB_DORES)
    OR    A
    LD    A, (HL)
    POP   BC
    POP   DE
    JP    M, MB_STUFFH
    JR    Z, MB_FLTGET
    CP    '.'
    JP    Z, MB_STUFFH
    LD    A, LINCON
    CALL  MB_KRNSAV
    PUSH  DE
    CALL  MB_LINGET
    CALL  MB_SKIPSP
; --------------------------------------------------
MB_SAVINT:
    EX    (SP), HL
    EX    DE, HL
MB_SAVI:
    LD    A, L
    CALL  MB_KRNSAV
    LD    A, H
; --------------------------------------------------
; Pop H and stuff away character
; --------------------------------------------------
MB_POPSTF:
    POP   HL
    CALL  MB_KRNSAV
    JP    MB_K_LOOP
; --------------------------------------------------
; Get floating number
; --------------------------------------------------
MB_FLTGET:
    PUSH  DE
    PUSH  BC
    LD    A, (HL)
    CALL  MB_FIN
    CALL  MB_SKIPSP
    POP   BC
    POP   DE
    PUSH  HL
    LD    A, (MB_VALTYP)
    CP    TINT
    JR    NZ, MB_NTINTG
    LD    HL, (MB_DACC+2)
    LD    A, H
    OR    A
    LD    A, TINT
    JR    NZ, MB_NTINTG
    LD    A, L
    LD    H, L
    LD    L, IN2CON
    CP    10                                        ; One byte const?
    JR    NC, MB_SAVI
    ADD   A, ONECON                                 ; make one byte constant
    JR    MB_POPSTF

; --------------------------------------------------
MB_NTINTG:
    PUSH  AF
    RRCA
    ADD   A, INTCON - 1
    CALL  MB_KRNSAV
    LD    HL, MB_DACC
    LD    A, (MB_VALTYP)
    CP    0x2
    JR    NZ, MB_NTDBL
    LD    HL, MB_DACC + 2
MB_NTDBL:
    POP   AF
MB_MOVCON:
    PUSH  AF
    LD    A, (HL)
    CALL  MB_KRNSAV
    POP   AF
    INC   HL
    DEC   A
    JR    NZ, MB_MOVCON
    POP   HL
    JP    MB_K_LOOP
; --------------------------------------------------
MB_SRCSPC:
    LD    DE, MB_SPCTAB - 1
MB_SRCSP2:
    INC   DE
    LD    A, (DE)
    AND   0x7f
    JP    Z, MB_NOTRS5
    INC   DE
    CP    (HL)
    LD    A, (DE)
    JR    NZ, MB_SRCSP2
    JP    MB_NOTRS1
MB_NTSNGT:
    CP    '&'
    JP    NZ, MB_STUFFH
    PUSH  HL
    RST   CHRGTR
    POP   HL
    CALL  MB_MAKUPS
    CP    'H'
    LD    A, OCTCON
    JR    NZ, MB_WUZOCT
    LD    A, HEXCON
MB_WUZOCT:
    CALL  MB_KRNSAV
    PUSH  DE
    PUSH  BC
    CALL  MB_OCTCNS
    POP   BC
    JP    MB_SAVINT
; --------------------------------------------------
MB_KRNSVC:
    LD    A, ':'
; --------------------------------------------------
MB_KRNSAV:
    LD    (DE), A
    INC   DE
    DEC   BC
    LD    A, C
    OR    B
    RET   NZ
    LD    E, ERRLBO
    JP    MB_ERROR
; --------------------------------------------------
MB_NOTRES:
    POP   HL
    DEC   HL
    DEC   A
    LD    (MB_DORES), A
    POP   BC
    POP   DE
    CALL  MB_MAKUPL
MB_KRNVAR:
    CALL  MB_KRNSAV
    INC   HL
    CALL  MB_MAKUPL
    CALL  MB_ISLET2
    JR    NC, MB_KRNVAR
    CP    '9' + 1
    JR    NC, MB_JKLOOP
    CP    '0'
    JR    NC, MB_KRNVAR
    CP    '.'
    JR    Z, MB_KRNVAR
MB_JKLOOP:
    JP    MB_K_LOOP
; --------------------------------------------------
MB_NOTRS5:
    LD    A, (HL)
    CP    ASCII_SP
    JR    NC, MB_NOTRS1
    CP    ASCII_TAB
    JR    Z, MB_NOTRS1
    CP    ASCII_LF
    JR    Z, MB_NOTRS1
    LD    A, ASCII_SP
MB_NOTRS1:
    PUSH  AF
    LD    A, (MB_DORES)
    INC   A
    JR    Z, MB_JNTRS6
    DEC   A
MB_JNTRS6:
    JP    MB_NOTRS6
; --------------------------------------------------
; Skip space chars
; --------------------------------------------------
MB_SKIPSP:
    DEC   HL
    LD    A, (HL)
    CP    ASCII_SP
    JR    Z, MB_SKIPSP
    CP    ASCII_TAB
    JR    Z, MB_SKIPSP
    CP    ASCII_LF
    JR    Z, MB_SKIPSP
    INC   HL
    RET
; --------------------------------------------------
; FOR Statement
; --------------------------------------------------
MB_FOR:
    LD    A, 100     ;variable search flag = loop variable
    LD    (MB_SUBFLG), A
    ; Init loop variable
    CALL  MB_PTRGET
    RST   SYNCHR
    DB    EQULTK
    PUSH  DE
    LD    (MB_TEMP), DE
    LD    A, (MB_VALTYP)
    PUSH  AF
    CALL  MB_FRMEVL
    POP   AF
    PUSH  HL
    CALL  MB_DOCNVF
    LD    HL, MB_FVALSV
    CALL  MB_MOVMF
    POP   HL
    POP   DE
    POP   BC
    PUSH  HL
    CALL  MB_DATAS
    LD    (MB_ENDFOR), HL
    LD    HL, 0x2    ;skip 1st word
    ADD   HL, SP
MB_LPFORM:
    CALL  MB_LOOPER       ;Search FOR|WHILE loop on stack
    JR    NZ, MB_NOTOL
    ADD   HL, BC
    PUSH  DE
    DEC   HL
    LD    D, (HL)
    DEC   HL
    LD    E, (HL)
    INC   HL
    INC   HL
    PUSH  HL
    LD    HL, (MB_ENDFOR)
    RST   DCOMPR
    POP   HL
    POP   DE
    JR    NZ, MB_LPFORM
    POP   DE
    LD    SP, HL
    LD    (MB_SAVSTK), HL
    DB    LD_C_n
MB_NOTOL:
    POP   DE
    EX    DE, HL
    LD    C, 12
    CALL  MB_GETSTK                                 ; check 12 words available on stack
    PUSH  HL
    LD    HL, (MB_ENDFOR)
    EX    (SP), HL
    PUSH  HL
    LD    HL, (MB_CURLIN)
    EX    (SP), HL
    RST   SYNCHR
    DB    _TO
    RST   GETYPTR
    JP    Z, MB_TMERR
    PUSH  AF
    CALL  MB_FRMEVL
    POP   AF
    PUSH  HL
    JR    NC, MB_SNGFOR_1
    JP    P, MB_SNGFOR
    CALL  MB_FRCINT
    EX    (SP), HL
    LD    DE, 1
    LD    A, (HL)
    CP    _STEP
    CALL  Z, MB_GETINT
    PUSH  DE
    PUSH  HL
    EX    DE, HL
    CALL  MB_ISIGN
    JR    MB_STPSGN
MB_SNGFOR_1:
    CALL  MB_FRCDBL
    POP   DE
    LD    HL, -8
    ADD   HL, SP
    LD    SP, HL
    PUSH  DE
    CALL  MB_MOVMF
    POP   HL
    LD    A, (HL)
    CP    _STEP
    LD    DE, C_ONE
    LD    A, 1
    JR    NZ, MB_FOR_STORE
    RST   CHRGTR
    CALL  MB_FRMEVL
    PUSH  HL
    CALL  MB_FRCDBL
    RST   SIGN
    LD    DE, MB_DACC
    POP   HL
MB_FOR_STORE:
    LD    B, H
    LD    C, L
    LD    HL, -8
    ADD   HL, SP
    LD    SP, HL
    PUSH  AF
    PUSH  BC
    CALL  MB_VMOVE
    POP   HL
    POP   AF
    JR    MB_FOR_TYPC
MB_SNGFOR:
    CALL  MB_FRCSNG
    CALL  MB_MOVRF
    POP   HL
    PUSH  BC
    PUSH  DE
    LD    BC, 0x1041                                ; single 1.0
    LD    DE, 0x0000
    LD    A, (HL)
    CP    _STEP
    LD    A, 1
    JR    NZ, MB_ONEON
    CALL  MB_FRMCHK
    PUSH  HL
    CALL  MB_FRCSNG
    CALL  MB_MOVRF
    RST   SIGN
MB_STPSGN:
    POP   HL
MB_ONEON:
    PUSH  DE
    PUSH  BC
    PUSH  BC
    PUSH  BC
    PUSH  BC
    PUSH  BC
MB_FOR_TYPC:
    OR    A
    JR    NZ, .ONEON_2
    LD    A, 2
.ONEON_2:
    LD    C, A
    RST   GETYPTR
    LD    B, A
    PUSH  BC
    DEC   HL
    RST   CHRGTR
    JP    NZ, MB_SNERR
    CALL  MB_NXTSCN
    RST   CHRGTR
    PUSH  HL
    PUSH  HL
    LD    HL, (MB_NXTLIN)
    LD    (MB_CURLIN), HL  ;=0xFFFE
    LD    HL, (MB_TEMP)
    EX    (SP), HL
    LD    B, _FOR
    PUSH  BC
    INC   SP
    PUSH  AF
    PUSH  AF
    JP    MB_NEXTS
; --------------------------------------------------
MB_NXTCON:
    LD    B, _FOR
    PUSH  BC
    INC   SP
; --------------------------------------------------
; Ecxecute a text (new statement)
;  Inp: HL -> Text to be executed
; --------------------------------------------------
MB_NEWSTT:
    CALL  MB_CTRLC_CHK
    CALL  NZ, MB_CNTCCN
    LD    (MB_SAVTXT), HL
    LD    (MB_SAVSTK), SP
    LD    A, (HL)
    CP    ':'
    JR    Z, MB_GONE
    OR    A
    JP    NZ, MB_SNERR
    INC   HL
MB_GONE4:
    LD    A, (HL)
    INC   HL
    OR    (HL)  ;End of BASIC program?
    JP    Z, MB_PRGEND
    INC   HL
    LD    E, (HL)
    INC   HL
    LD    D, (HL)
    EX    DE, HL
    LD    (MB_CURLIN), HL  ;=0xFFFE
    LD    A, (MB_TRCFLG)
    OR    A
    JR    Z, MB_NOTTRC
    PUSH  DE
    LD    A, '['
    RST   OUTDO
    CALL  MB_LINPRT
    LD    A, ']'
    RST   OUTDO
    POP   DE
MB_NOTTRC:
    EX    DE, HL
MB_GONE:
    RST   CHRGTR
    LD    DE, MB_NEWSTT
    PUSH  DE
    RET   Z
MB_GONE2:
    SUB   _END
    JP    C, MB_LET
    CP    NUMCMD
    JP    NC, MB_ISMID
    RLCA
    LD    C, A
    LD    B, 0
    EX    DE, HL
    LD    HL, MB_STMDSP      ;=0x4632
    ADD   HL, BC
    LD    C, (HL)
    INC   HL
    LD    B, (HL)
    PUSH  BC
    EX    DE, HL
; --------------------------------------------------
; Get next BASIC char
; --------------------------------------------------
MB_CHRGTR:
    INC   HL
MB_CHRGT2:
    LD    A, (HL)
    CP    ':'
    RET   NC
; --------------------------------------------------
MB_CHRCON:
    CP    ASCII_SP
    JR    Z, MB_CHRGTR
    JR    NC, MB_NOTLFT
    OR    A
    RET   Z
    CP    OCTCON
    JR    C, MB_NOTCON
    CP    CONCON
    JR    NZ, MB_NTRSCC
    LD    A, (MB_CONSAV)
    OR    A
    RET
; --------------------------------------------------
MB_NTRSCC:
    CP    CONCN2
    JR    Z, MB_NTRSC2
    PUSH  AF
    INC   HL
    LD    (MB_CONSAV), A
    SUB   INTCON
    JR    NC, MB_MAKTKN
    SUB   ONECON - INTCON                           ; 0xf5
    JR    NC, MB_ONEI
    CP    IN2CON - ONECON
    JR    NZ, MB_FRCINC
    LD    A, (HL)
    INC   HL
MB_ONEI:
    LD    (MB_CONTXT), HL
    LD    H, 0
MB_ONEI2:
    LD    L, A
    LD    (MB_CONLO), HL
    LD    A, 0x2
    LD    (MB_CONTYP), A
    LD    HL, MB_NUMCON
    POP   AF
    OR    A
    RET
; --------------------------------------------------
MB_FRCINC:
    LD    A, (HL)
    INC   HL
    INC   HL
    LD    (MB_CONTXT), HL
    DEC   HL
    LD    H, (HL)
    JR    MB_ONEI2
; --------------------------------------------------
MB_RETOKN:
    CALL  MB_CONFAC
MB_NTRSC2:
    LD    HL, (MB_CONTXT)
    JR    MB_CHRGT2
; --------------------------------------------------
MB_MAKTKN:
    INC   A
    RLCA
    LD    (MB_CONTYP), A
    PUSH  DE
    PUSH  BC
    LD    DE, MB_CONLO
    EX    DE, HL
    LD    B, A
    CALL  MB_MOVE_N
    EX    DE, HL
    POP   BC
    POP   DE
    LD    (MB_CONTXT), HL
    POP   AF
    LD    HL, MB_NUMCON    ;=0x1E
    OR    A
    RET
; --------------------------------------------------
MB_NOTCON:
    CP    ASCII_TAB
    JP    NC, MB_CHRGTR
MB_NOTLFT:
    CP    '0'
    CCF
    INC   A
    DEC   A
    RET
; --------------------------------------------------
MB_NUMCON:
    DB    CONCON
    DB    CONCN2
; --------------------------------------------------
; Get numeric constant in DAC
; --------------------------------------------------
MB_CONFAC:
    LD    A, (MB_CONSAV)
    CP    LINCON+1
    JR    NC, MB_NTLINE
    CP    PTRCON
    JR    C, MB_NTLINE
    LD    HL, (MB_CONLO)
    JR    NZ, MB_FLTLIN
    INC   HL
    INC   HL
    INC   HL
    LD    E, (HL)
    INC   HL
    LD    D, (HL)
    EX    DE, HL
MB_FLTLIN:
    JP    MB_UI2SNG
; --------------------------------------------------
MB_NTLINE:
    LD    A, (MB_CONTYP)
    LD    (MB_VALTYP), A
    CP    0x2
    JR    NZ, MB_CONFDB
    LD    HL, (MB_CONLO)
    LD    (MB_DACC+2), HL
MB_CONFDB:
    LD    HL, MB_CONLO
    JP    MB_VMOVFM
; --------------------------------------------------
; DEFSTR statement
; --------------------------------------------------
MB_DEFSTR:
    LD    E, TSTR
    DB    LD_BC_nn
; --------------------------------------------------
; DEFINT statement
; --------------------------------------------------
MB_DEFINT:
    LD    E, TINT
    DB    LD_BC_nn
; --------------------------------------------------
; DEFSNG statement
; --------------------------------------------------
MB_DEFSNG:
    LD    E, TSGN
    DB    LD_BC_nn
; --------------------------------------------------
; DEFDBL statement
; --------------------------------------------------
MB_DEFDBL:
    LD    E, TDBL
; --------------------------------------------------
MB_DEFCON:
    CALL  MB_ISLET
    LD    BC, MB_SNERR
    PUSH  BC
    RET   C
    SUB   'A'
    LD    C, A
    LD    B, A
    RST   CHRGTR
    CP    MINUTK
    JR    NZ, MB_NOTRNG
    RST   CHRGTR
    CALL  MB_ISLET
    RET   C
    SUB   'A'
    LD    B, A
    RST   CHRGTR
MB_NOTRNG:
    LD    A, B
    SUB   C
    RET   C
    INC   A
    EX    (SP), HL
    LD    HL, MB_DEFTBL
    LD    B, 0
    ADD   HL, BC
MB_LPDCHG:
    LD    (HL), E
    INC   HL
    DEC   A
    JR    NZ, MB_LPDCHG
    POP   HL
    LD    A, (HL)
    CP    ','
    RET   NZ
    RST   CHRGTR
    JR    MB_DEFCON

; --------------------------------------------------
; Read formula from current pos and convert
; to positive integer
;  Out: DE - integer 0..32767
; --------------------------------------------------
MB_INTIDX:
    RST   CHRGTR
MB_INTIDX2:
    CALL  MB_GETIN2
    RET   P

; --------------------------------------------------
MB_FCERR:
    LD    E, ERRFC
    JP    MB_ERROR

; --------------------------------------------------
MB_LINSPC:
    LD    A, (HL)
    CP    '.'
    LD    DE, (MB_DOT)
    JP    Z, MB_CHRGTR
MB_LINGET:
    DEC   HL
; --------------------------------------------------
MB_LINGT2:
    RST   CHRGTR
    CP    LINCON
    JR    Z, MB_LINGT3
    CP    PTRCON
MB_LINGT3:
    LD    DE, (MB_CONLO)
    JP    Z, MB_CHRGTR
    XOR   A
    LD    (MB_CONSAV), A
    LD    DE, 0
    DEC   HL

; --------------------------------------------------
MB_MORLIN:
    RST   CHRGTR
    RET   NC
    PUSH  HL
    PUSH  AF
    LD    HL, 6552
    RST   DCOMPR
    JR    C, MB_POPHSR
    LD    H, D
    LD    L, E
    ADD   HL, DE
    ADD   HL, HL
    ADD   HL, DE
    ADD   HL, HL
    POP   AF
    SUB   '0'
    LD    E, A
    LD    D, 0
    ADD   HL, DE
    EX    DE, HL
    POP   HL
    JR    MB_MORLIN
; --------------------------------------------------
MB_POPHSR:
    POP   AF
    POP   HL
    RET
; --------------------------------------------------
MB_RUN:
    JP    Z, MB_RUNC
    CP    LINCON
    JR    Z, MB_CONRUN
    CP    PTRCON
    JP    NZ, MB_LRUN
; --------------------------------------------------
MB_CONRUN:
    CALL  MB_CLEARC1
    LD    BC, MB_NEWSTT
    JR    MB_RUN_GO

; --------------------------------------------------
; GOSUB statement
; --------------------------------------------------
MB_GOSUB:
    LD    C, 3
    CALL  MB_GETSTK
    CALL  MB_LINGET
    POP   BC
    PUSH  HL
    PUSH  HL
    LD    HL, (MB_CURLIN)  ;=0xFFFE
    EX    (SP), HL
    LD    A, _GOSUB
    PUSH  AF
    INC   SP
    PUSH  BC
    JR    MB_GOTO2

; --------------------------------------------------
MB_RUN_GO:
    PUSH  BC

; --------------------------------------------------
MB_GOTO:
    CALL  MB_LINGET
; --------------------------------------------------
MB_GOTO2:
    LD    A, (MB_CONSAV)
    CP    PTRCON
    EX    DE, HL
    RET   Z
    CP    LINCON
    JP    NZ, MB_SNERR
    EX    DE, HL
    PUSH  HL
    LD    HL, (MB_CONTXT)
    EX    (SP), HL
    CALL  MB_REM_ELSE
    INC   HL
    PUSH  HL
    LD    HL, (MB_CURLIN)  ;=0xFFFE
    RST   DCOMPR
    POP   HL
    CALL  C, MB_FL_LOOP
    CALL  NC, MB_FNDLIN
    JR    NC, MB_USERR
    DEC   BC
    LD    A, PTRCON
    LD    (MB_PTRFLG), A
    POP   HL
    CALL  MB_CONCH2
    LD    H, B
    LD    L, C
    RET

; ----------------------------------------------------
; Undefined line number error
; ----------------------------------------------------
MB_USERR:
    LD    E, ERRULN
    JP    MB_ERROR

; ----------------------------------------------------
; Return statement
; ----------------------------------------------------
MB_RETURN:
    RET   NZ
    LD    D, 255
    CALL  MB_FNDFOR
    CP    _GOSUB
    JR    Z, MB_STOP_GS
    DEC   HL
MB_STOP_GS:
    LD    SP, HL
    LD    (MB_SAVSTK), HL
    LD    E, ERRRG
    JP    NZ, MB_ERROR
    POP   HL
    LD    (MB_CURLIN), HL  ;=0xFFFE
    LD    HL, MB_NEWSTT
    EX    (SP), HL
    DB    LD_A_n
    POP   HL

; ----------------------------------------------------
; DATA Statement
; and skip to end of statement
; ----------------------------------------------------
MB_DATAS:
    DB    LD_BC_nn
    DB    3Ah       ;LD A, (nn)

; ----------------------------------------------------
; REM/ELSE statement handler
; Also skip to end of BASIC line
; ----------------------------------------------------
MB_REM_ELSE:
    LD    C, 0
    LD    B, 0
MB_EXCHQT:
    LD    A, C
    LD    C, B
    LD    B, A
MB_REMER:
    DEC   HL
MB_REMER1:
    RST   CHRGTR
    OR    A
    RET   Z
    CP    B
    RET   Z
    INC   HL
    CP    '"'
    JR    Z, MB_EXCHQT
    INC   A
    JR    Z, MB_REMER1
    SUB   _IF+1
    JR    NZ, MB_REMER
    CP    B
    ADC   A, D
    LD    D, A
    JR    MB_REMER

; --------------------------------------------------
; LET Entry point with VLTYP-3 in A
; --------------------------------------------------
MB_LETCON:
    POP   AF
    ADD   A, 0x3
    JR    MB_LETCN2

; --------------------------------------------------
; LET Statement
; --------------------------------------------------
MB_LET:
    CALL  MB_PTRGET
    RST   SYNCHR
    DB    EQULTK
    LD    (MB_TEMP), DE
    PUSH  DE
    LD    A, (MB_VALTYP)
    PUSH  AF
    CALL  MB_FRMEVL
    POP   AF
MB_LETCN2:
    EX    (SP), HL
MB_INPCOM:
    LD    B, A
    LD    A, (MB_VALTYP)
    CP    B
    LD    A, B
    JR    Z, MB_LETCN5
    CALL  MB_DOCNVF
MB_LETCN4:
    LD    A, (MB_VALTYP)
MB_LETCN5:
    LD    DE, MB_DACC
    CP    0x2
    JR    NZ, MB_LETCN6
    LD    DE, MB_DACC+2
MB_LETCN6:
    PUSH  HL
    CP    0x3
    JR    NZ, MB_COPNUM
    LD    HL, (MB_DACC+2)
    PUSH  HL
    INC   HL
    LD    E, (HL)
    INC   HL
    LD    D, (HL)
    LD    HL, (MB_TXTTAB)  ;=0x733F
    RST   DCOMPR
    JR    NC, MB_INBUFC
    LD    HL, (MB_STREND)
    RST   DCOMPR
    POP   DE
    JR    NC, MB_DNTCPY
    LD    HL, MB_DSCTMP
    RST   DCOMPR
    JR    NC, MB_DNTCPY
    DB    LD_A_n    ;LD A,  n
MB_INBUFC:
    POP   DE
    CALL  MB_FRETMS
    EX    DE, HL
    CALL  MB_STRCPY
MB_DNTCPY:
    CALL  MB_FRETMS
    EX    (SP), HL
MB_COPNUM:
    CALL  MB_VMOVE
    POP   DE
    POP   HL
    RET

; --------------------------------------------------
; ON..GOTO
; --------------------------------------------------
MB_ONGOTO:
    CP    _ERROR
    JR    NZ, MB_NTOERR
    RST   CHRGTR
    RST   SYNCHR
    DB    _GOTO
    CALL  MB_LINGET
    LD    A, D
    OR    E
    JR    Z, MB_RESTRP
    CALL  MB_FNDLN1
    LD    D, B
    LD    E, C
    POP   HL
    JP    NC, MB_USERR
; --------------------------------------------------
MB_RESTRP:
    LD    (MB_ONELIN), DE
    RET   C
    LD    A, (MB_ONEFLG)
    OR    A
    LD    A, E
    RET   Z
    LD    A, (MB_ERRFLG)
    LD    E, A
    JP    MB_ERRESM

; --------------------------------------------------
; ON statement (not ON ERROR)
; --------------------------------------------------
MB_NTOERR:
    CALL  MB_GETBYT
    LD    A, (HL)
    LD    B, A
    CP    _GOSUB
    JR    Z, MB_ISGOSU
    RST   SYNCHR
    DB    _GOTO
    DEC   HL
MB_ISGOSU:
    LD    C, E
MB_LOOPON:
    DEC   C
    LD    A, B
    JP    Z, MB_GONE2
    CALL  MB_LINGT2
    CP    ','
    RET   NZ
    JR    MB_LOOPON

; --------------------------------------------------
; RESUME Statement
; --------------------------------------------------
MB_RESUME:
    LD    A, (MB_ONEFLG)
    OR    A
    JR    NZ, MB_RESUME1
    LD    (MB_ONELIN), A
    LD    (MB_ONELIN+1), A
    JP    MB_REERR

; --------------------------------------------------
MB_RESUME1:
    INC   A
    LD    (MB_ERRFLG), A
    LD    A, (HL)
    CP    _NEXT
    JR    Z, MB_RESNXT
    CALL  MB_LINGET
    RET   NZ
    LD    A, D
    OR    E
    JR    Z, MB_RESUME_E
    CALL  MB_GOTO2
    XOR   A
    LD    (MB_ONEFLG), A
    RET
; --------------------------------------------------
MB_RESNXT:
    RST   CHRGTR
    RET   NZ
    JR    MB_RESTXT
; --------------------------------------------------
MB_RESUME_E:
    XOR   A
    LD    (MB_ONEFLG), A
    INC   A
MB_RESTXT:
    LD    HL, (MB_ERRTXT)
    EX    DE, HL
    LD    HL, (MB_ERRLIN)
    LD    (MB_CURLIN), HL  ;=0xFFFE
    EX    DE, HL
    RET   NZ
    LD    A, (HL)
    OR    A
    JR    NZ, MB_NOTBGL
    INC   HL
    INC   HL
    INC   HL
    INC   HL
MB_NOTBGL:
    INC   HL
    XOR   A
    LD    (MB_ONEFLG), A
    JP    MB_DATAS

; --------------------------------------------------
; ERROR Statement
; --------------------------------------------------
MB_ERRORS:
    CALL  MB_GETBYT
    RET   NZ
    OR    A
    JP    Z, MB_FCERR
    JP    MB_ERROR

; --------------------------------------------------
; AUTO Statement
; --------------------------------------------------
MB_AUTO:
    LD    DE, 10
    PUSH  DE
    JR    Z, MB_SNGAUT
    CALL  MB_LINSPC
    EX    DE, HL
    EX    (SP), HL
    JR    Z, MB_SNGAU1
    EX    DE, HL
    RST   SYNCHR
    DB    ','
    LD    DE, (MB_AUTINC)
    JR    Z, MB_SNGAUT
    CALL  MB_LINGET
    JP    NZ, MB_SNERR

; --------------------------------------------------
MB_SNGAUT:
    EX    DE, HL
MB_SNGAU1:
    LD    A, H
    OR    L
    JP    Z, MB_FCERR
    LD    (MB_AUTINC), HL
    LD    (MB_AUTFLG), A
    POP   HL
    LD    (MB_AUTLIN), HL
    POP   BC
    JP    MB_MAIN

; --------------------------------------------------
; IF Statement
; --------------------------------------------------
MB_IFS:
    CALL  MB_FRMEVL
    LD    A, (HL)
    CP    ','
    CALL  Z, MB_CHRGTR
    CP    _GOTO
    JR    Z, MB_OKGOTO
    RST   SYNCHR
    DB    _THEN
    DEC   HL
MB_OKGOTO:
    PUSH  HL
    CALL  MB_VSIGN
    POP   HL
    JR    Z, MB_FALSIF
MB_DOCOND:
    RST   CHRGTR
    RET   Z
    CP    LINCON
    JP    Z, MB_GOTO
    CP    PTRCON
    JP    NZ, MB_GONE2
    LD    HL, (MB_CONLO)
    RET

; --------------------------------------------------
MB_FALSIF:
    LD    D, 1
MB_SKPMRF:
    CALL  MB_DATAS
    OR    A
    RET   Z
    RST   CHRGTR
    CP    _ELSE
    JR    NZ, MB_SKPMRF
    DEC   D
    JR    NZ, MB_SKPMRF
    JR    MB_DOCOND

; --------------------------------------------------
; LPRINT Statement
; --------------------------------------------------
MB_LPRINT:
    LD    A, 1
    LD    (MB_PRTFLG), A
    JP    MB_NEWCHAR

; --------------------------------------------------
; PRINT Statement
; --------------------------------------------------
MB_PRINT:
    LD    C, 2
    CALL  MB_FILGET
MB_NEWCHAR:
    DEC   HL
    RST   CHRGTR
    CALL  Z, MB_CRDO
MB_PRINTC:
    JP    Z, MB_FINPRT
    CP    _USING
    JP    Z, MB_PRINUS
    CP    _TAB
    JP    Z, MB_TABER
    CP    _SPC
    JP    Z, MB_TABER
    PUSH  HL
    CP    ','
    JR    Z, MB_COMPRT
    CP    ';'
    JP    Z, MB_NOTABR
    POP   BC
    CALL  MB_FRMEVL
    PUSH  HL
    RST   GETYPTR
    JR    Z, MB_STRDON
    CALL  MB_FOUT
    CALL  MB_STRLIT
    LD    (HL), ASCII_SP
    LD    HL, (MB_DACC+2)
    INC   (HL)
MB_STRDON:
    CALL  MB_ISFLIO
    JR    NZ, MB_LINCH2
    LD    HL, (MB_DACC+2)
    LD    A, (MB_PRTFLG)
    OR    A
    JR    Z, MB_ISTTY
    LD    A, (MB_LPTSIZ)   ;= 255
    LD    B, A
    INC   A
    JR    Z, MB_LINCH2
    LD    A, (MB_LPTPOS)
    OR    A
    JR    Z, MB_LINCH2
    ADD   A, (HL)
    CCF
    JR    NC, MB_LINCHK
    DEC   A
    CP    B
    JR    MB_LINCHK

; --------------------------------------------------
MB_ISTTY:
    LD    A, (MB_LINLEN)   ;= 40
    LD    B, A
    INC   A
    JR    Z, MB_LINCH2
    LD    A, (MB_TTYPOS)
    OR    A
    JR    Z, MB_LINCH2
    ADD   A, (HL)
    CCF
    JR    NC, MB_LINCHK
    DEC   A
    CP    B
MB_LINCHK:
    JR    C, MB_LINCH2
    CALL  Z, MB_CRFIN
    CALL  NZ, MB_CRDO
MB_LINCH2:
    CALL  MB_STRPRT
    POP   HL
    JP    MB_NEWCHAR

; --------------------------------------------------
MB_COMPRT:
    LD    BC, NMLO.C
    LD    HL, (MB_PTRFIL)
    ADD   HL, BC
    CALL  MB_ISFLIO
    LD    A, (HL)
    JR    NZ, MB_MORCOM
    LD    A, (MB_PRTFLG)
    OR    A
    JR    Z, MB_ISCTTY
    LD    A, (MB_NLPPOS)   ;= 238
    LD    B, A
    INC   A
    LD    A, (MB_LPTPOS)
    JR    Z, MB_MORCOM
    CP    B
    JP    MB_CHKCOM

; --------------------------------------------------
MB_ISCTTY:
    LD    A, (MB_CLMLST)   ;=0xE
    LD    B, A
    LD    A, (MB_TTYPOS)
    CP    0xff
    JR    Z, MB_MORCOM
    CP    B
MB_CHKCOM:
    CALL  NC, MB_CRDO
    JP    NC, MB_NOTABR
MB_MORCOM:
    SUB   CLMWID
    JR    NC, MB_MORCOM
    CPL
    JR    MB_ASPA2

; --------------------------------------------------
; TAB or SPC function
; --------------------------------------------------
MB_TABER:
    PUSH  AF
    RST   CHRGTR
    CALL  MB_GETIN2
    POP   AF
    PUSH  AF
    CP    _SPC
    JR    Z, MB_SPCNDC
    DEC   DE
MB_SPCNDC:
    LD    A, D
    OR    A
    JP    P, MB_TBNONG
    LD    DE, 0
MB_TBNONG:
    PUSH  HL
    CALL  MB_ISFLIO
    JR    NZ, MB_LNOMOD
    LD    A, (MB_PRTFLG)
    OR    A
    LD    A, (MB_LPTSIZ)   ;= 255
    JR    NZ, MB_LPTMDF
    LD    A, (MB_LINLEN)   ;= 40
MB_LPTMDF:
    LD    L, A
    INC   A
    JR    Z, MB_LNOMOD
    LD    H, 0
    CALL  MB_IMOD
    EX    DE, HL
MB_LNOMOD:
    POP   HL
    RST   SYNCHR
    DB    ')'
    DEC   HL
    POP   AF
    SUB   _SPC
    PUSH  HL
    JR    Z, MB_DOSIZT
    LD    BC, NMLO.C
    LD    HL, (MB_PTRFIL)
    ADD   HL, BC
    CALL  MB_ISFLIO
    LD    A, (HL)
    JR    NZ, MB_DOSIZT
    LD    A, (MB_PRTFLG)
    OR    A
    JP    Z, MB_TTYIST
    LD    A, (MB_LPTPOS)
    JR    MB_DOSIZT

; --------------------------------------------------
MB_TTYIST:
    LD    A, (MB_TTYPOS)
MB_DOSIZT:
    CPL
    ADD   A, E
    JR    C, MB_ASPA2
    INC   A
    JR    Z, MB_NOTABR
    CALL  MB_CRDO
    LD    A, E
    DEC   A
    JP    M, MB_NOTABR
MB_ASPA2:
    INC   A
    LD    B, A
    LD    A, ASCII_SP

; --------------------------------------------------
MB_REPOUT:
    RST   OUTDO
    DJNZ  MB_REPOUT
MB_NOTABR:
    POP   HL
    RST   CHRGTR
    JP    MB_PRINTC

; --------------------------------------------------
MB_FINPRT:
    XOR   A
    LD    (MB_PRTFLG), A
    PUSH  HL
    LD    H, A
    LD    L, A
    LD    (MB_PTRFIL), HL
    POP   HL
    RET

; --------------------------------------------------
; LINE statement
; --------------------------------------------------
MB_LINE:
    CP    _INPUT
    JP    NZ, MB_LINEST
    RST   SYNCHR
    DB    _INPUT
    CP    '#'
    JP    Z, MB_DLINE
    CALL  MB_SCNSEM
    CALL  MB_QTINP
    CALL  MB_PTRGET
    CALL  MB_CHKSTR
    PUSH  DE
    PUSH  HL
    CALL  MB_SINLIN
    POP   DE
    POP   BC
    JP    C, MB_STPEND
    PUSH  BC
    PUSH  DE
    LD    B, 0
    CALL  MB_STRLT3
    POP   HL
    LD    A, 0x3
    JP    MB_LETCN2

; --------------------------------------------------
MB_TRYAGN:
    DB    "?Redo from start\r\n", 0

; --------------------------------------------------
MB_SCNSTR:
    INC   HL
    LD    A, (HL)
    OR    A
    JP    Z, MB_SNERR
    CP    '"'
    JR    NZ, MB_SCNSTR
    JP    MB_SCNCON1

; --------------------------------------------------
MB_INPBAK:
    POP   HL
    POP   HL
    JR    MB_RDOIN2

; --------------------------------------------------
; Error on BASIC Input operation
; --------------------------------------------------
MB_TRMNOK:
    LD    A, (MB_USFLG)
    OR    A
    JP    NZ, MB_ERR_INPP
MB_RDOIN2:
    POP   BC
    LD    HL, MB_TRYAGN    ;= "?Redo from start\r\n"
    CALL  MB_STROUT
    LD    HL, (MB_SAVTXT)
    RET

; --------------------------------------------------
MB_FILSTI:
    CALL  MB_FILINP
    PUSH  HL
    LD    HL, MB_BUFMIN    ;=0x2C
    JP    MB_INPCN3

; --------------------------------------------------
; INPUT Statement
; --------------------------------------------------
MB_INPUT:
    CP    '#'
    JR    Z, MB_FILSTI
    CALL  MB_SCNSEM
    LD    BC, MB_NOTQTI
    PUSH  BC

; --------------------------------------------------
MB_QTINP:
    CP    '"'
    LD    A, 0     ;ToDo: check
    LD    A, 0xff
    LD    (MB_TEMPA+1), A
    RET   NZ
    CALL  MB_STRLTI
MB_QTINP1:
    LD    A, (HL)
    CP    ','
    JR    NZ, MB_NTICMA
    XOR   A
    LD    (MB_TEMPA+1), A
    RST   CHRGTR
    JR    MB_INPCMA
MB_NTICMA:
    RST   SYNCHR
    DB    ';'
MB_INPCMA:
    PUSH  HL
    CALL  MB_STRPRT
    POP   HL
    RET

; --------------------------------------------------
MB_NOTQTI:
    PUSH  HL
    LD    A, (MB_TEMPA+1)
    OR    A
    JR    Z, MB_SUPPRS
    LD    A, '?'
    RST   OUTDO
    DB    '>'
    JR    NZ, MB_QTINP1
MB_SUPPRS:
    CALL  MB_SINLIN
    POP   BC
    JP    C, MB_STPEND
    PUSH  BC
    XOR   A
    LD    (MB_USFLG), A
    LD    (HL), ','
    EX    DE, HL
    POP   HL
    PUSH  HL
    PUSH  DE
    PUSH  DE
    DEC   HL
MB_VARLOP:
    LD    A, 0x80
    LD    (MB_SUBFLG), A
    RST   CHRGTR
    CALL  MB_NVRFIL
    LD    A, (HL)
    DEC   HL
    CP    '['
    JR    Z, MB_IS_ARR
    CP    '('
    JR    NZ, MB_ENDSCN
MB_IS_ARR:
    INC   HL
    LD    B, 0
MB_SCNCON:
    INC   B
MB_SCNCON1:
    RST   CHRGTR
    JP    Z, MB_SNERR
    CP    '"'
    JP    Z, MB_SCNSTR
    CP    '('
    JR    Z, MB_SCNCON
    CP    '['
    JR    Z, MB_SCNCON1
    CP    ']'
    JR    Z, MB_ARR_CLOS
    CP    ')'
    JR    NZ, MB_SCNCON1
MB_ARR_CLOS:
    DJNZ  MB_SCNCON1
MB_ENDSCN:
    RST   CHRGTR
    JR    Z, MB_OKVLST
    CP    ','
    JP    NZ, MB_SNERR
MB_OKVLST:
    EX    (SP), HL
    LD    A, (HL)
    CP    ','
    JP    NZ, MB_INPBAK
    LD    A, 1
    LD    (MB_OVCSTR), A
    LD    (MB_OVCSTR_1), A
    CALL  SCNVAL
    LD    A, (MB_OVCSTR_1)
    DEC   A
    LD    (MB_OVCSTR_1), A
    LD    A, (MB_OVCSTR)
    DEC   A
    JP    NZ, MB_INPBAK
    PUSH  HL
    RST   GETYPTR
    CALL  Z, MB_FREFAC
    POP   HL
    DEC   HL
    RST   CHRGTR
    EX    (SP), HL
    LD    A, (HL)
    CP    ','
    JP    Z, MB_VARLOP
    POP   HL
    DEC   HL
    RST   CHRGTR
    OR    A
    POP   HL
    JP    NZ, MB_RDOIN2
MB_INPCN3:
    LD    (HL), ','   ;=0x2C
    JR    MB_INPCON

; --------------------------------------------------
; READ statement
; --------------------------------------------------
MB_READ:
    PUSH  HL
    LD    HL, (MB_DATPTR)
    DB    OR_n      ;OR n
MB_INPCON:
    XOR   A
    LD    (MB_USFLG), A
    EX    (SP), HL
    DB    LD_BC_nn
MB_LOPDT2:
    RST   SYNCHR
    DB    ','
    CALL  MB_PTRGET
    EX    (SP), HL
    PUSH  DE
    LD    A, (HL)     ;=0x2C
    CP    ','
    JR    Z, MB_DATBK
    LD    A, (MB_USFLG)
    OR    A
    JP    NZ, MB_DATLOP
MB_DATBK:
    DB    OR_n      ;OR n
SCNVAL:
    XOR   A
    LD    (MB_INPPAS), A
    CALL  MB_ISFLIO
    JP    NZ, MB_FILIND
    RST   GETYPTR
    PUSH  AF
    JR    NZ, MB_NUMINS
    RST   CHRGTR
    LD    D, A
    LD    B, A
    CP    '"'
    JR    Z, MB_NOWGET
    LD    A, (MB_USFLG)
    OR    A
    LD    D, A
    JR    Z, MB_NCOLST
    LD    D, ':'
MB_NCOLST:
    LD    B, ','
    DEC   HL
MB_NOWGET:
    CALL  MB_STRLT2
MB_DOASIG:
    POP   AF
    ADD   A, 0x3
    LD    C, A
    LD    A, (MB_INPPAS)
    OR    A
    RET   Z
    LD    A, C
    EX    DE, HL
    LD    HL, MB_STRDN2
    EX    (SP), HL
    PUSH  DE
    JP    MB_INPCOM
MB_NUMINS:
    RST   CHRGTR
    LD    BC, MB_DOASIG
    PUSH  BC
    JP    MB_FIN
MB_STRDN2:
    DEC   HL
    RST   CHRGTR
    JR    Z, MB_TRMOK
    CP    ','
    JP    NZ, MB_TRMNOK
MB_TRMOK:
    EX    (SP), HL
    DEC   HL
    RST   CHRGTR
    JP    NZ, MB_LOPDT2
    POP   DE
    LD    A, (MB_USFLG)
    OR    A
    EX    DE, HL
    JP    NZ, MB_RESFIN
    PUSH  DE
    POP   HL
    JP    MB_FINPRT
MB_DATLOP:
    CALL  MB_DATAS
    OR    A
    JR    NZ, MB_NOWLIN
    INC   HL
    LD    A, (HL)
    INC   HL
    OR    (HL)
    LD    E, ERROD
    JP    Z, MB_ERROR
    INC   HL
    LD    E, (HL)
    INC   HL
    LD    D, (HL)
    LD    (MB_DATLIN), DE
MB_NOWLIN:
    RST   CHRGTR
    CP    _DATA
    JR    NZ, MB_DATLOP
    JP    MB_DATBK

; --------------------------------------------------
; evaluate = expression
;  Inp: HL - basic pointer
; --------------------------------------------------
MB_FRMEQL:
    RST   SYNCHR
    DB    EQULTK
    DB    LD_BC_nn
MB_FRMPRN:
    RST   SYNCHR
    DB    '('

; --------------------------------------------------
; FRMEVL (Expression Evaluator)
;  Inp: HL - BASIC pointer
; --------------------------------------------------
MB_FRMEVL:
    DEC   HL

; --------------------------------------------------
; Skip character and evaluate expression
; --------------------------------------------------
MB_FRMCHK:
    LD    D, 0
MB_LPOPER:
    PUSH  DE
    LD    C, 1
    CALL  MB_GETSTK
    CALL  MB_EVAL
    XOR   A
    LD    (MB_FLGOVC), A   ;ToDO: check
MB_TSTOP:
    LD    (MB_TEMP2), HL   ;ToDo: check
MB_RETAOP:
    LD    HL, (MB_TEMP2)
    POP   BC
    LD    A, (HL)
    LD    (MB_TEMP3), HL
    CP    GREATK
    RET   C
    CP    LESSTK+1
    JR    C, MB_DORELS
    SUB   LESSTK+1
    LD    E, A
    JR    NZ, MB_NTPLUS
    LD    A, (MB_VALTYP)
    CP    TSTR
    LD    A, E
    JP    Z, MB_CAT
MB_NTPLUS:
    CP    LSTOPK
    RET   NC
    LD    HL, MB_OPTAB     ;= 121
    LD    D, 0
    ADD   HL, DE
    LD    A, B
    LD    D, (HL)      ;= 121
    CP    D
    RET   NC
    PUSH  BC
    LD    BC, MB_RETAOP
    PUSH  BC
    LD    A, D
    CP    0x51
    JR    C, MB_EXPSTK
    AND   0xfe
    CP    0x7a
    JR    Z, MB_EXPSTK
MB_NUMREL:
    LD    HL, MB_DACC+2
    LD    A, (MB_VALTYP)
    SUB   0x3
    JP    Z, MB_TMERR
    OR    A
    LD    HL, (MB_DACC+2)
    PUSH  HL
    JP    M, MB_VPUSHD
    LD    HL, (MB_DACC)
    PUSH  HL
    JP    PO, MB_VPUSHD
    LD    HL, (MB_DACC+6)
    PUSH  HL
    LD    HL, (MB_DACC+4)
    PUSH  HL
MB_VPUSHD:
    ADD   A, 0x3
    LD    C, E
    LD    B, A
    PUSH  BC
    LD    BC, MB_APPLOP
MB_FINTMP:
    PUSH  BC
    LD    HL, (MB_TEMP3)
    JP    MB_LPOPER

; --------------------------------------------------
MB_DORELS:
    LD    D, 0
MB_LOPREL:
    SUB   GREATK
    JR    C, MB_FINREL
    CP    NMREL
    JR    NC, MB_FINREL
    CP    1
    RLA
    XOR   D
    CP    D
    LD    D, A
    JP    C, MB_SNERR
    LD    (MB_TEMP3), HL
    RST   CHRGTR
    JR    MB_LOPREL

; --------------------------------------------------
MB_EXPSTK:
    PUSH  DE
    CALL  MB_FRCINT
    POP   DE
    PUSH  HL
    LD    BC, MB_DANDOR
    JR    MB_FINTMP

; --------------------------------------------------
MB_FINREL:
    LD    A, B
    CP    100
    RET   NC
    PUSH  BC
    PUSH  DE
    LD    DE, 0x6405       ;0+256*100+OPCNT
    LD    HL, MB_DOCMP
    PUSH  HL
    RST   GETYPTR
    JP    NZ, MB_NUMREL
    LD    HL, (MB_DACC+2)
    PUSH  HL
    LD    BC, MB_STRCMP
    JR    MB_FINTMP

; --------------------------------------------------
MB_APPLOP:
    POP   BC
    LD    A, C
    LD    (MB_OPRTYP), A
    LD    A, (MB_VALTYP)
    CP    B
    JR    NZ, MB_VALNSM
    CP    TINT
    JR    Z, MB_INTDPC
    CP    TSGN
    JP    Z, MB_SNGDPC
    JR    NC, MB_DBLDPC
MB_VALNSM:
    LD    D, A
    LD    A, B
    CP    TDBL
    JR    Z, MB_STKDBL
    LD    A, D
    CP    TDBL
    JR    Z, MB_FACDBL
    LD    A, B
    CP    TSGN
    JR    Z, MB_STKSNG
    LD    A, D
    CP    TSTR
    JP    Z, MB_TMERR
    JR    NC, MB_FACSNG

; --------------------------------------------------
MB_INTDPC:
    LD    HL, MB_INTDSP    ;=0x3CA0
    LD    B, 0
    ADD   HL, BC
    ADD   HL, BC
    LD    C, (HL)
    INC   HL
    LD    B, (HL)
    POP   DE
    LD    HL, (MB_DACC+2)
    PUSH  BC
    RET

; --------------------------------------------------
MB_STKDBL:
    CALL  MB_FRCDBL
MB_DBLDPC:
    CALL  MB_VMOVAF
    POP   HL
    LD    (MB_DACC+4), HL
    POP   HL
    LD    (MB_DACC+6), HL

; --------------------------------------------------
MB_SNGDBL:
    POP   BC
    POP   DE
    CALL  MB_MOVFR

; --------------------------------------------------
MB_SETDBL:
    CALL  MB_FRCDBL
    LD    HL, MB_DBLDSP    ;=0x3186
MB_DODSP:
    LD    A, (MB_OPRTYP)
    RLCA
    ADD   A, L
    LD    L, A
    ADC   A, H
    SUB   L
    LD    H, A
    LD    A, (HL)
    INC   HL
    LD    H, (HL)
    LD    L, A
    JP    (HL)

; --------------------------------------------------
MB_FACDBL:
    LD    A, B
    PUSH  AF
    CALL  MB_VMOVAF
    POP   AF
    LD    (MB_VALTYP), A
    CP    0x4
    JR    Z, MB_SNGDBL
    POP   HL
    LD    (MB_DACC+2), HL
    JR    MB_SETDBL

; --------------------------------------------------
MB_STKSNG:
    CALL  MB_FRCSNG
MB_SNGDPC:
    POP   BC
    POP   DE
MB_SNGDO:
    LD    HL, MB_SNGDSP    ;=0x3DFC
    JR    MB_DODSP

; --------------------------------------------------
MB_FACSNG:
    POP   HL
    CALL  MB_PUSHF
    CALL  MB_CONSIH
    CALL  MB_MOVRF
    POP   HL
    LD    (MB_DACC), HL
    POP   HL
    LD    (MB_DACC+2), HL
    JR    MB_SNGDO

; --------------------------------------------------
MB_INTDIV:
    PUSH  HL
    EX    DE, HL
    CALL  MB_CONSIH
    POP   HL
    CALL  MB_PUSHF
    CALL  MB_CONSIH
    JP    MB_FDIV1

; --------------------------------------------------
; Evaluate variable, constant, function call
; --------------------------------------------------
MB_EVAL:
    RST   CHRGTR
    JP    Z, MB_MOERR
    JP    C, MB_FIN
    CALL  MB_ISLET2
    JP    NC, MB_ISVAR
    CP    DBLCON + 1                                ; It is an embeded constant?
    JP    C, MB_RETOKN
    INC   A
    JP    Z, MB_ISFUN                               ; It is a function call?
    DEC   A
    CP    PLUSTK                                    ; ignore plus
    JR    Z, MB_EVAL
    CP    MINUTK                                    ; negation?
    JP    Z, MB_DOMIN
    CP    '"'                                       ; String const?
    JP    Z, MB_STRLTI
    CP    _NOT                                      ; if NOT operator?
    JP    Z, MB_NOTER
    CP    '&'                                       ; is octal const?
    JP    Z, MB_OCTCNS
    CP    _ERR
    JR    NZ, MB_ERL_GE
    RST   CHRGTR
    LD    A, (MB_ERRFLG)
    PUSH  HL
    CALL  MB_SNGFLT
    POP   HL
    RET

    ; ERL$ and after
MB_ERL_GE:
    CP    _ERL
    JR    NZ, MB_VARPTR_GE
    RST   CHRGTR
    PUSH  HL
    LD    HL, (MB_ERRLIN)
    CALL  MB_UI2SNG
    POP   HL
    RET

    ; VARPTR and after
MB_VARPTR_GE:
    CP    _VARPTR
    JR    NZ, MB_NTVARP
    RST   CHRGTR
    RST   SYNCHR
    DB    '('
    CP    '#'
    JR    NZ, MB_NVRFIL1
    CALL  MB_GTBYTC
    PUSH  HL
    CALL  MB_FILIDX
    EX    DE, HL
    POP   HL
    JP    MB_VARRET
MB_NVRFIL1:
    CALL  MB_NVRFIL
MB_VARRET:
    RST   SYNCHR
    DB    ')'
    PUSH  HL
    EX    DE, HL
    LD    A, H
    OR    L
    JP    Z, MB_FCERR
    CALL  MB_MAKINT
    POP   HL
    RET
MB_NTVARP:
    CP    _USR
    JP    Z, MB_USRFN
    CP    _INSTR
    JP    Z, MB_INSTR
    CP    _POINT
    JP    Z, MB_POINT
    CP    _INKEY
    JP    Z, MB_INKEY
    CP    _STRING_
    JP    Z, MB_STRNG_
    CP    _INPUT
    JP    Z, MB_FIXINP
    CP    _CSRLIN
    JP    Z, MB_CSRLIN
    CP    _ATTR_
    JP    Z, MB_ATTR_
    CP    _DSKI_
    JP    Z, MB_DSKIFN
    CP    _FN
    JP    Z, MB_FNDOER
MB_PARCHK:
    CALL  MB_FRMPRN
    RST   SYNCHR
    DB    ')'
    RET
; --------------------------------------------------
MB_DOMIN:
    LD    D, 125                                    ; precedence below ^ (greater that -)
    CALL  MB_LPOPER
    LD    HL, (MB_TEMP2)
    PUSH  HL
    CALL  MB_VNEG
MB_LABBCK:
    POP   HL
    RET

; --------------------------------------------------
; Get variable value
; --------------------------------------------------
MB_ISVAR:
    CALL  MB_PTRGET

; --------------------------------------------------
MB_RETVAR:
    PUSH  HL
    EX    DE, HL
    LD    (MB_DACC + 2), HL
    RST   GETYPTR
    CALL  NZ, MB_VMOVFM
    POP   HL
    RET

; --------------------------------------------------
; UpperCase char
;  Inp: HL -> char
;  Out: A - char
; --------------------------------------------------
MB_MAKUPL:
    LD    A, (HL)

; --------------------------------------------------
; UpperCase char
;  Inp: A - char
;  Out: A - char
; --------------------------------------------------
MB_MAKUPS:
    CP    'A' + 0x20
    RET   C
    CP    'Z' + 0x21
    RET   NC
    AND   0x5f
    RET

; --------------------------------------------------
MB_CNSGET:
    CP    '&'
    JP    NZ, MB_LINGET
MB_OCTCNS:
    LD    DE, 0
    RST   CHRGTR
    CALL  MB_MAKUPS
    CP    'O'
    JR    Z, MB_LOPOCT
    CP    'H'
    JR    NZ, MB_LOPOC2
    LD    B, 5
MB_LOPHEX:
    INC   HL
    LD    A, (HL)
    CALL  MB_MAKUPS
    CALL  MB_ISLET2
    EX    DE, HL
    JR    NC, MB_ALPTST
    CP    '9' + 1
    JR    NC, MB_OCTFIN
    SUB   '0'
    JR    C, MB_OCTFIN
    JR    MB_NXTHEX
MB_ALPTST:
    CP    'F' + 1
    JR    NC, MB_OCTFIN
    SUB   '7'
MB_NXTHEX:
    ADD   HL, HL
    ADD   HL, HL
    ADD   HL, HL
    ADD   HL, HL
    OR    L
    LD    L, A
    EX    DE, HL
    DJNZ  MB_LOPHEX

MB_OVERR1A:
    LD    A, (MB_OVCSTR_1)
    OR    A
    JP    Z, MB_OVERR
    PUSH  HL
    LD    HL, MSG_OVR_ERR                           ;= "Overflow"
    CALL  MB_STRPRN
    CALL  MB_CRDO
    POP   HL
MB_INC_IACC:
    LD    A, (MB_FLGOVC)
    INC   A
    LD    (MB_FLGOVC), A
    RET
MB_SNERRR:
    LD    A, (MB_OVCSTR_1)
    OR    A
    JP    NZ, MB_INC_IACC
    JP    MB_SNERR
MB_LOPOC2:
    DEC   HL
MB_LOPOCT:
    RST   CHRGTR
    EX    DE, HL
    JR    NC, MB_OCTFIN
    CP    '8'
    JP    NC, MB_SNERRR
    LD    BC, MB_OVERR1A
    PUSH  BC
    ADD   HL, HL
    RET   C
    ADD   HL, HL
    RET   C
    ADD   HL, HL
    RET   C
    POP   BC
    LD    B, 0
    SUB   '0'
    LD    C, A
    ADD   HL, BC
    EX    DE, HL
    JR    MB_LOPOCT
MB_OCTFIN:
    CALL  MB_MAKINT
    EX    DE, HL
    RET

; --------------------------------------------------
MB_ISFUN:
    INC   HL
    LD    A, (HL)
    SUB   _LEFT_
    CP    _RND - _LEFT_
    JR    NZ, MB_NTMRND
    PUSH  HL
    RST   CHRGTR
    CP    '('
    POP   HL
    JP    NZ, MB_ISFUN_NB
    LD    A, _RND - ONEFUN
MB_NTMRND:
    LD    B, 0
    RLCA
    LD    C, A
    PUSH  BC
    RST   CHRGTR
    LD    A, C
    CP    NUMGFN                                    ; It is a past LASTNUM?
    JR    NC, MB_OKNORM
    CALL  MB_FRMPRN
    RST   SYNCHR
    DB    ','
    CALL  MB_CHKSTR
    EX    DE, HL
    LD    HL, (MB_DACC + 2)
    EX    (SP), HL
    PUSH  HL
    EX    DE, HL
    CALL  MB_GETBYT
    EX    DE, HL
    EX    (SP), HL
    JR    MB_FINGO
MB_OKNORM:
    CALL  MB_PARCHK
    EX    (SP), HL
    LD    A, L
    CP    BOTCON                                    ; 0xc
    JR    C, MB_NOTFRF
    CP    ONECON+10
    JR    NC, MB_NOTFRF
    RST   GETYPTR
    PUSH  HL
    CALL  C, MB_FRCDBL
    POP   HL
MB_NOTFRF:
    LD    DE, MB_LABBCK
    PUSH  DE
    LD    A, 1
    LD    (MB_FLGOVC), A
MB_FINGO:
    LD    BC, MB_FUNDSP                             ;=0x2FE0
; --------------------------------------------------
MB_DISPAT:
    ADD   HL, BC
    LD    C, (HL)
    INC   HL
    LD    H, (HL)
    LD    L, C
    JP    (HL)
; --------------------------------------------------
MB_MINPLS:
    DEC   D
    CP    MINUTK
    RET   Z
    CP    '-'
    RET   Z
    INC   D
    CP    '+'
    RET   Z
    CP    PLUSTK
    RET   Z
    DEC   HL
    RET

; --------------------------------------------------
; MORE FORMULA EVALUATION - LOGICAL,  RELATIONAL OPS
; --------------------------------------------------
MB_DOCMP:
    INC   A
    ADC   A, A
    POP   BC
    AND   B
    ADD   A, 0xff
    SBC   A, A
    CALL  MB_CONIA
    JR    MB_RETAPG

; --------------------------------------------------
MB_NOTER:
    LD    D, 90                                     ; NOT precedence
    CALL  MB_LPOPER
    CALL  MB_FRCINT
    LD    A, L
    CPL
    LD    L, A
    LD    A, H
    CPL
    LD    H, A
    LD    (MB_DACC + 2), HL
    POP   BC

; Return to refetch
MB_RETAPG:
    JP    MB_RETAOP

; --------------------------------------------------
MB_GETYPR:
    JR    NC, MB_NCASE
    SUB   0x3
    OR    A
    SCF
    RET

; --------------------------------------------------
MB_NCASE:
    SUB   0x3
    OR    A
    RET

; --------------------------------------------------
; Appleies "AND" and "OR" operators
; shoud be used to implement alll logical ops
; --------------------------------------------------
MB_DANDOR:
    LD    A, B
    PUSH  AF
    CALL  MB_FRCINT
    POP   AF
    POP   DE
    CP    122
    JP    Z, MB_IMOD
    CP    123
    JP    Z, MB_IDIV
    LD    BC, MB_GIVINT
    PUSH  BC
    CP    70
    JR    NZ, MB_NOTOR
    LD    A, E
    OR    L
    LD    L, A
    LD    A, H
    OR    D
    RET

; --------------------------------------------------
MB_NOTOR:
    CP    80
    JR    NZ, MB_NOTAND
    LD    A, E
    AND   L
    LD    L, A
    LD    A, H
    AND   D
    RET

; --------------------------------------------------
MB_NOTAND:
    CP    60
    JR    NZ, MB_NOTXOR
    LD    A, E
    XOR   L
    LD    L, A
    LD    A, H
    XOR   D
    RET

; --------------------------------------------------
MB_NOTXOR:
    CP    50
    JR    NZ, MB_NOTEQV
    LD    A, E
    XOR   L
    CPL
    LD    L, A
    LD    A, H
    XOR   D
    CPL
    RET

; --------------------------------------------------
MB_NOTEQV:
    LD    A, L
    CPL
    AND   E
    CPL
    LD    L, A
    LD    A, H
    CPL
    AND   D
    CPL
    RET

; --------------------------------------------------
; Substract HL - DE and place result to FAC
; --------------------------------------------------
MB_GIVDBL:
    OR    A
    SBC   HL, DE
    JP    MB_UI2SNG

; --------------------------------------------------
MB_LPOS:
    LD    A, (MB_LPTPOS)
    JR    MB_SNGFLI

; --------------------------------------------------
MB_POS:
    LD    A, (MB_TTYPOS)

; --------------------------------------------------
MB_SNGFLI:
    INC   A

; --------------------------------------------------
MB_SNGFLT:
    LD    L, A
    XOR   A

; --------------------------------------------------
MB_GIVINT:
    LD    H, A
    JP    MB_MAKINT

; --------------------------------------------------
MB_USRFN:
    CALL  MB_SCNUSR
    PUSH  DE
    CALL  MB_PARCHK
    EX    (SP), HL
    LD    E, (HL)
    INC   HL
    LD    D, (HL)
    LD    HL, MB_POPHLRT
    PUSH  HL
    PUSH  DE
    LD    A, (MB_VALTYP)
    PUSH  AF
    CP    TSTR
    CALL  Z, MB_FREFAC
    POP   AF
    EX    DE, HL
    LD    HL, MB_DACC
    RET

; --------------------------------------------------
MB_SCNUSR:
    RST   CHRGTR
    LD    BC, 0
    CP    ONECON + 10
    JR    NC, MB_NOARGU
    CP    ONECON
    JR    C, MB_NOARGU
    RST   CHRGTR
    LD    A, (MB_CONLO)
    OR    A
    RLA
    LD    C, A

; --------------------------------------------------
MB_NOARGU:
    EX    DE, HL
    LD    HL, MB_USRTAB                             ;=0x13F1
    ADD   HL, BC
    EX    DE, HL
    RET

; --------------------------------------------------
; DEFUSR Statement
; --------------------------------------------------
MB_DEFUSR:
    CALL  MB_SCNUSR
    PUSH  DE
    RST   SYNCHR
    DB    EQULTK
    CALL  MB_PGETADR
    EX    (SP), HL
    LD    (HL), E
    INC   HL
    LD    (HL), D
    POP   HL
    RET

; --------------------------------------------------
; DEF Statement
; --------------------------------------------------
MB_DEF:
    CP    _USR
    JR    Z, MB_DEFUSR
    CALL  MB_GETFNM
    CALL  MB_ERRDIR
    EX    DE, HL
    LD    (HL), E
    INC   HL
    LD    (HL), D
    EX    DE, HL
    LD    A, (HL)
    CP    '('
    JP    NZ, MB_DATAS
    RST   CHRGTR

; --------------------------------------------------
; DATA Statement
; --------------------------------------------------
MB_DATA:
    CALL  MB_PTRGET
    LD    A, (HL)
    CP    ')'
    JP    Z, MB_DATAS
    RST   SYNCHR
    DB    ','
    JR    MB_DATA

; --------------------------------------------------
; FN Function
; --------------------------------------------------
MB_FNDOER:
    CALL  MB_GETFNM
    LD    A, (MB_VALTYP)
    OR    A
    PUSH  AF
    LD    (MB_TEMP2), HL
    EX    DE, HL
    LD    A, (HL)
    INC   HL
    LD    H, (HL)
    LD    L, A
    LD    A, H
    OR    L
    JP    Z, MB_UFERR
    LD    A, (HL)
    CP    '('
    JP    NZ, MB_FINVLS
    RST   CHRGTR
    LD    (MB_TEMP3), HL
    EX    DE, HL
    LD    HL, (MB_TEMP2)
    RST   SYNCHR
    DB    '('
    XOR   A
    PUSH  AF
    PUSH  HL
    EX    DE, HL
MB_ASGMOR:
    LD    A, 128
    LD    (MB_SUBFLG), A
    CALL  MB_PTRGET
    EX    DE, HL
    EX    (SP), HL
    LD    A, (MB_VALTYP)
    PUSH  AF
    PUSH  DE
    CALL  MB_FRMEVL
    LD    (MB_TEMP2), HL
    POP   HL
    LD    (MB_TEMP3), HL
    POP   AF
    CALL  MB_DOCNVF
    LD    C, 4                                      ; make room for value (8b)
    CALL  MB_GETSTK
    LD    HL, -8                                    ; store 8 places
    ADD   HL, SP
    LD    SP, HL
    CALL  MB_MOVMF
    LD    A, (MB_VALTYP)
    PUSH  AF
    LD    HL, (MB_TEMP2)
    LD    A, (HL)
    CP    ')'
    JR    Z, MB_POPASG
    RST   SYNCHR
    DB    ','
    PUSH  HL
    LD    HL, (MB_TEMP3)
    RST   SYNCHR
    DB    ','
    JR    MB_ASGMOR

; --------------------------------------------------
MB_POPAS2:
    POP   AF
    LD    (MB_PRMLN2), A

; --------------------------------------------------
MB_POPASG:
    POP   AF
    OR    A
    JR    Z, MB_FINASG
    LD    (MB_VALTYP), A
    LD    HL, 0
    ADD   HL, SP
    CALL  MB_VMOVFM
    LD    HL, 8                                     ; restore space
    ADD   HL, SP
    LD    SP, HL
    POP   DE
    LD    L, 3

MB_LPSIZL:
    INC   L
    DEC   DE
    LD    A, (DE)
    OR    A
    JP    M, MB_LPSIZL
    DEC   DE
    DEC   DE
    DEC   DE
    LD    A, (MB_VALTYP)
    ADD   A, L
    LD    B, A
    LD    A, (MB_PRMLN2)
    LD    C, A
    ADD   A, B
    CP    PRMSIZ
    JP    NC, MB_FCERR
    PUSH  AF
    LD    A, L
    LD    B, 0
    LD    HL, MB_PARM2
    ADD   HL, BC
    LD    C, A
    CALL  MB_BCTRAN
    LD    BC, MB_POPAS2
    PUSH  BC
    PUSH  BC
    JP    MB_LETCN4

; --------------------------------------------------
MB_FINASG:
    LD    HL, (MB_TEMP2)
    RST   CHRGTR
    PUSH  HL
    LD    HL, (MB_TEMP3)
    RST   SYNCHR
    DB    ')'
    DB    LD_A_n
MB_FINVLS:
    PUSH  DE
    LD    (MB_TEMP3), HL
    LD    A, (MB_PRMLEN)
    ADD   A, 0x4
    PUSH  AF
    RRCA
    LD    C, A
    CALL  MB_GETSTK
    POP   AF
    LD    C, A
    CPL
    INC   A
    LD    L, A
    LD    H, 0xff
    ADD   HL, SP
    LD    SP, HL
    PUSH  HL
    LD    DE, MB_PRMSTK
    CALL  MB_BCTRAN
    POP   HL
    LD    (MB_PRMSTK), HL
    LD    HL, (MB_PRMLN2)
    LD    (MB_PRMLEN), HL
    LD    B, H
    LD    C, L
    LD    HL, MB_PARM1
    LD    DE, MB_PARM2
    CALL  MB_BCTRAN
    LD    H, A
    LD    L, A
    LD    (MB_PRMLN2), HL
    LD    HL, (MB_FUNACT)
    INC   HL
    LD    (MB_FUNACT), HL
    LD    A, H
    OR    L
    LD    (MB_NOFUNS), A
    LD    HL, (MB_TEMP3)
    CALL  MB_FRMEQL
    DEC   HL
    RST   CHRGTR
    JP    NZ, MB_SNERR
    RST   GETYPTR
    JR    NZ, MB_NOCPRS
    LD    DE, MB_DSCTMP
    LD    HL, (MB_DACC+2)
    RST   DCOMPR
    JR    C, MB_NOCPRS
    CALL  MB_STRCPY
    CALL  MB_PUTTMP

; --------------------------------------------------
MB_NOCPRS:
    LD    HL, (MB_PRMSTK)
    LD    D, H
    LD    E, L
    INC   HL
    INC   HL
    LD    C, (HL)
    INC   HL
    LD    B, (HL)
    INC   BC
    INC   BC
    INC   BC
    INC   BC
    LD    HL, MB_PRMSTK
    CALL  MB_BCTRAN
    EX    DE, HL
    LD    SP, HL
    LD    HL, (MB_FUNACT)
    DEC   HL
    LD    (MB_FUNACT), HL
    LD    A, H
    OR    L
    LD    (MB_NOFUNS), A
    POP   HL
    POP   AF
; --------------------------------------------------
MB_DOCNVF:
    PUSH  HL
    AND   0x7
    LD    HL, MB_FRCTBL    ;=0x3B6A
    LD    C, A
    LD    B, 0
    ADD   HL, BC
    CALL  MB_DISPAT
    POP   HL
    RET

; --------------------------------------------------
MB_BCTRAL:
    LD    A, (DE)
    LD    (HL), A
    INC   HL
    INC   DE
    DEC   BC
; --------------------------------------------------
MB_BCTRAN:
    LD    A, B
    OR    C
    JR    NZ, MB_BCTRAL
    RET

; --------------------------------------------------
MB_ERRDIR:
    PUSH  HL
    LD    HL, (MB_CURLIN)  ;=0xFFFE
    INC   HL
    LD    A, H
    OR    L
    POP   HL
    RET   NZ
    LD    E, 0xc
    JP    MB_ERROR

; --------------------------------------------------
; Get pointer to function name
; --------------------------------------------------
MB_GETFNM:
    RST   SYNCHR
    RET   C
    LD    A, 0x80
    LD    (MB_SUBFLG), A
    OR    (HL)
    LD    C, A
    JP    MB_PTRGT2

; --------------------------------------------------
MB_ISMID:
    CP    0xff-_END
    JP    NZ, MB_SNERR
    INC   HL
    LD    A, (HL)
    INC   HL
    CP    _MID_
    JP    Z, MB_MIDFN
    JP    MB_SNERR
; --------------------------------------------------
MB_FNINP:
    CALL  MB_FRQINT
    LD    B, H
    LD    C, L
    IN    A, (C)
    JP    MB_SNGFLT
; --------------------------------------------------
MB_SETIO:
    CALL  MB_PGETADR
    PUSH  DE
    RST   SYNCHR
    DB    ','
    CALL  MB_GETBYT
    POP   BC
    RET
MB_FNOUT:
    CALL  MB_SETIO
    OUT   (C), A
    RET
MB_FNWAIT:
    CALL  MB_SETIO
    PUSH  BC
    PUSH  AF
    LD    E, 0
    DEC   HL
    RST   CHRGTR
    JR    Z, MB_NOTTHR
    RST   SYNCHR
    DB    ','
    CALL  MB_GETBYT
MB_NOTTHR:
    POP   AF
    LD    D, A
    POP   BC
MB_STAINP:
    IN    A, (C)
    XOR   E
    AND   D
    JR    Z, MB_STAINP
    RET

; --------------------------------------------------
; Evaluate integer operand
; --------------------------------------------------
MB_GETINT:
    RST   CHRGTR
; --------------------------------------------------
MB_GETIN2:
    CALL  MB_FRMEVL

; --------------------------------------------------
; Convert FAC to Int in DE
; --------------------------------------------------
MB_INTFR2:
    PUSH  HL
    CALL  MB_FRCINT
    EX    DE, HL
    POP   HL
    LD    A, D
    OR    A
    RET

; --------------------------------------------------
MB_GTBYTC:
    RST   CHRGTR
; --------------------------------------------------
MB_GETBYT:
    CALL  MB_FRMEVL

; --------------------------------------------------
; Get integer parameter into A
; --------------------------------------------------
MB_CONINT:
    CALL  MB_INTFR2
    JP    NZ, MB_FCERR
    DEC   HL
    RST   CHRGTR
    LD    A, E
    RET

; --------------------------------------------------
MB_LLIST:
    LD    A, 1
    LD    (MB_PRTFLG), A

; --------------------------------------------------
MB_LIST:
    POP   BC
    CALL  MB_SCNLIN
    PUSH  BC
MB_LIST4:
    LD    HL, 0xffff
    LD    (MB_CURLIN), HL  ;=0xFFFE
    POP   HL
MB_LIST_LOOP:
    POP   DE
    LD    C, (HL)
    INC   HL
    LD    B, (HL)
    INC   HL
    LD    A, B
    OR    C
    JP    Z, MB_READY
    CALL  MB_ISFLIO
    CALL  Z, MB_ISCNTC
    PUSH  BC
    LD    C, (HL)
    INC   HL
    LD    B, (HL)
    INC   HL
    PUSH  BC
    EX    (SP), HL
    EX    DE, HL
    RST   DCOMPR
    POP   BC
    JP    C, MB_STPRDY
    EX    (SP), HL
    PUSH  HL
    PUSH  BC
    EX    DE, HL
    LD    (MB_DOT), HL
    CALL  MB_LINPRT
    POP   HL
    LD    A, (HL)
    CP    ASCII_TAB
    JR    Z, MB_NOSPAL
    LD    A, ASCII_SP
    RST   OUTDO
MB_NOSPAL:
    CALL  MB_BUFLIN
    LD    HL, MB_BUF
    CALL  MB_LISPRT
    CALL  MB_CRDO
    JR    MB_LIST4

; --------------------------------------------------
MB_LISPRT:
    LD    A, (HL)
    OR    A
    RET   Z
    CALL  MB_OUTCH1
    INC   HL
    JR    MB_LISPRT
; --------------------------------------------------
MB_BUFLIN:
    LD    BC, MB_BUF
    LD    D, 0xff
    XOR   A
    LD    (MB_OPRTYP), A
    XOR   A
    LD    (MB_TEMPA), A
    JR    MB_PLOOP2
; --------------------------------------------------
MB_PLOOP:
    INC   BC
    INC   HL
    DEC   D
    RET   Z

; --------------------------------------------------
MB_PLOOP2:
    LD    A, (HL)
    OR    A
    LD    (BC), A
    RET   Z
    CP    OCTCON
    JR    C, MB_NTEMBL
    CP    ASCII_SP
    LD    E, A
    JR    C, MB_PRTVAR1
    CP    '"'
    JR    NZ, MB_PLSTR1
    LD    A, (MB_OPRTYP)
    XOR   1
    LD    (MB_OPRTYP), A
    LD    A, '"'
MB_PLSTR1:
    CP    ':'
    JR    NZ, MB_NTEMBL
    LD    A, (MB_OPRTYP)
    RRA
    JR    C, MB_PLSTR2
    RLA
    AND   11111101b
    LD    (MB_OPRTYP), A
MB_PLSTR2:
    LD    A, ':'
MB_NTEMBL:
    OR    A
    JP    M, MB_PLOOPR
    LD    E, A
    CP    '.'
    JR    Z, MB_PRTVAR1
    CALL  MB_TSTANM
    JR    NC, MB_PRTVAR1
    XOR   A
    JR    MB_PLOOPH
; --------------------------------------------------
MB_PRTVAR1:
    LD    A, (MB_TEMPA)
    OR    A
    JR    Z, MB_PLOOPG
    INC   A
    JR    NZ, MB_PLOOPG
    LD    A, ASCII_SP
    LD    (BC), A
    INC   BC
    DEC   D
    RET   Z
MB_PLOOPG:
    LD    A, 1
MB_PLOOPH:
    LD    (MB_TEMPA), A
    LD    A, E
    CP    OCTCON
    JR    C, MB_PLOOPZ
    CP    ASCII_SP
    JP    C, MB_NUMLIN

MB_PLOOPZ:
    LD    (BC), A  ;=0x2C
    JP    MB_PLOOP

; --------------------------------------------------
MB_PLOOPR:
    LD    A, (MB_OPRTYP)
    RRA
    JR    C, MB_PRTVAR
    RRA
    RRA
    JR    NC, MB_PLOOP_KEY
    LD    A, (HL)
    CP    SNGQTK
    PUSH  HL
    PUSH  BC
    LD    HL, MB_PRTVAR_SETUP
    PUSH  HL
    RET   NZ
    DEC   BC
    LD    A, (BC)
    CP    'M'
    RET   NZ
    DEC   BC
    LD    A, (BC)     ;=0x2C
    CP    'E'
    RET   NZ
    DEC   BC
    LD    A, (BC)
    CP    'R'
    RET   NZ
    DEC   BC
    LD    A, (BC)
    CP    ':'
    RET   NZ
    POP   AF
    POP   AF
    POP   HL
    INC   D
    INC   D
    INC   D
    INC   D
    JR    MB_PLOOPR1
MB_PRTVAR_SETUP:
    POP   BC
    POP   HL
    LD    A, (HL)
MB_PRTVAR:
    JP    MB_PLOOP

; --------------------------------------------------
; Set data statement flag
; --------------------------------------------------
MB_SET_DSF:
    LD    A, (MB_OPRTYP)
    OR    DATA_FLG
MB_OPRFLG_SET::
    LD    (MB_OPRTYP), A
    XOR   A
    RET

; --------------------------------------------------
; Set REM statement flag
; --------------------------------------------------
MB_SET_REMF:
    LD    A, (MB_OPRTYP)
    OR    REM_FLG
    JR    MB_OPRFLG_SET:
MB_PLOOP_KEY:
    RLA
    JR    C, MB_PRTVAR
    LD    A, (HL)
    CP    _DATA
    CALL  Z, MB_SET_DSF
    CP    _REM
    CALL  Z, MB_SET_REMF
MB_PLOOPR1:
    LD    A, (HL)
    INC   A
    LD    A, (HL)
    JR    NZ, MB_ELS_TK
    INC   HL
    LD    A, (HL)
    AND   0x7f
MB_ELS_TK:
    INC   HL
    CP    _ELSE
    JR    NZ, MB_WHL_TK
    DEC   BC
    INC   D
MB_WHL_TK:
    CP    _WHILE
    JR    NZ, MB_PLOOP_PUT
    LD    A, (HL)
    INC   HL
    CP    PLUSTK
    LD    A, _WHILE
    JR    Z, MB_PLOOP_PUT
    DEC   HL
MB_PLOOP_PUT:
    PUSH  HL
    PUSH  BC
    PUSH  DE
    LD    HL, MB_ATAB-1
    LD    B, A
    LD    C, 'A' - 1
MB_TAB_ROW_NXT:
    INC   C
MB_TAB_CHR_NXT:
    INC   HL
    LD    D, H
    LD    E, L
MB_TAB_SCAN:
    LD    A, (HL)
    OR    A
    JR    Z, MB_TAB_ROW_NXT
    INC   HL
    JP    P, MB_TAB_SCAN
    LD    A, (HL)
    CP    B
    JR    NZ, MB_TAB_CHR_NXT
    EX    DE, HL
    CP    _USR
    JR    Z, MB_NOISPA
    CP    _FN
MB_NOISPA:
    LD    A, C
    POP   DE
    POP   BC
    LD    E, A
    JR    NZ, MB_NTFNEX
    LD    A, (MB_TEMPA)
    OR    A
    LD    A, 0
    LD    (MB_TEMPA), A
    JR    MB_MORLNZ
MB_NTFNEX:
    CP    'Z' + 1
    JR    NZ, MB_NTSPCH
    XOR   A
    LD    (MB_TEMPA), A
    JR    MB_MORPUR
MB_NTSPCH:
    LD    A, (MB_TEMPA)
    OR    A
    LD    A, 0xff
    LD    (MB_TEMPA), A
MB_MORLNZ:
    JR    Z, MB_MORLN0
    LD    A, ASCII_SP
    LD    (BC), A
    INC   BC
    DEC   D
    JP    Z, MB_PPSWRT
MB_MORLN0:
    LD    A, E
    JR    MB_MORLN1
MB_MORPUR:
    LD    A, (HL)                                   ;= "UT", CFh
    INC   HL
    LD    E, A
MB_MORLN1:
    AND   0x7f
    LD    (BC), A
    INC   BC
    DEC   D
    JP    Z, MB_PPSWRT
    OR    E
    JP    P, MB_MORPUR
    CP    '(' + 0x80                                ;'(' + 0x80 SPC( OR TAB( ?
    JR    NZ, MB_NTSPCT
    XOR   A
    LD    (MB_TEMPA), A
MB_NTSPCT:
    POP   HL
    JP    MB_PLOOP2
MB_TSTANM:
    CALL  MB_ISLET2
    RET   NC
    CP    '0'
    RET   C
    CP    '9' + 1
    CCF
    RET

; --------------------------------------------------
; Decode numeric tokens
; --------------------------------------------------
MB_NUMLIN:
    DEC   HL
    RST   CHRGTR
    PUSH  DE
    PUSH  BC
    PUSH  AF
    CALL  MB_CONFAC
    POP   AF
    LD    BC, MB_CONLIN
    PUSH  BC
    CP    OCTCON
    JP    Z, MB_FOUTO
    CP    HEXCON
    JP    Z, MB_FOUTH
    LD    HL, (MB_CONLO)
    JP    MB_FOUT
MB_CONLIN:
    POP   BC
    POP   DE
    LD    A, (MB_CONSAV)
    LD    E, 'O'
    CP    OCTCON
    JR    Z, MB_SAVBAS
    CP    HEXCON
    LD    E, 'H'
    JR    NZ, MB_NUMSLN
MB_SAVBAS:
    LD    A, '&'
    LD    (BC), A
    INC   BC
    DEC   D
    RET   Z
    LD    A, E
    LD    (BC), A
    INC   BC
    DEC   D
    RET   Z
MB_NUMSLN:
    LD    A, (MB_CONTYP)
    CP    TSGN
    LD    E, 0
    JR    C, MB_TYPSET
    LD    E, '!'
    JR    Z, MB_TYPSET
    LD    E, '#'
MB_TYPSET:
    LD    A, (HL)
    CP    ASCII_SP
    CALL  Z, MB_INCHLRT
MB_NUMSL2:
    LD    A, (HL)
    INC   HL
    OR    A
    JR    Z, MB_NUMDN
    LD    (BC), A
    INC   BC
    DEC   D
    RET   Z
    LD    A, (MB_CONTYP)
    CP    TSGN
    JR    C, MB_NUMSL2
    DEC   BC
    LD    A, (BC)
    INC   BC
    JR    NZ, MB_DBLSCN
    CP    '.'
    JR    Z, MB_ZERE
MB_DBLSCN:
    CP    'D'
    JR    Z, MB_ZERE
    CP    'E'
    JR    NZ, MB_NUMSL2
MB_ZERE:
    LD    E, 0
    JR    MB_NUMSL2
MB_NUMDN:
    LD    A, E
    OR    A
    JR    Z, MB_NOD
    LD    (BC), A
    INC   BC
    DEC   D
    RET   Z
MB_NOD:
    LD    HL, (MB_CONTXT)
    JP    MB_PLOOP2

; --------------------------------------------------
; DELETE statement
; --------------------------------------------------
MB_DELETE:
    CALL  MB_SCNLIN
    PUSH  BC
    CALL  MB_DEPTR
    POP   BC
    POP   DE
    PUSH  BC
    PUSH  BC
    CALL  MB_FNDLIN
    JR    NC, MB_FCERRG
    LD    D, H
    LD    E, L
    EX    (SP), HL
    PUSH  HL
    RST   DCOMPR
MB_FCERRG:
    JP    NC, MB_FCERR
    LD    HL, MB_OKTXT                              ;= "Ok"
    CALL  MB_STROUT
    POP   BC
    LD    HL, MB_FINI
    EX    (SP), HL

; --------------------------------------------------
; Remove line(s)
;  Inp: HL = start of BASIC text that follows
;       BC = start of deleted BASIC text
; --------------------------------------------------
MB_DEL:
    EX    DE, HL
    LD    HL, (MB_VARTAB)
MB_MLOOP:
    LD    A, (DE)
    LD    (BC), A
    INC   BC
    INC   DE
    RST   DCOMPR
    JR    NZ, MB_MLOOP
    LD    H, B
    LD    L, C
    LD    (MB_VARTAB), HL
    LD    (MB_ARYTAB), HL
    LD    (MB_STREND), HL
    RET

; --------------------------------------------------
MB_PEEK:
    CALL  MB_FRQINT
    LD    A, (HL)
    JP    MB_SNGFLT

; --------------------------------------------------
MB_POKE:
    CALL  MB_PGETADR
    PUSH  DE
    RST   SYNCHR
    DB    ','
    CALL  MB_GETBYT
    POP   DE
    LD    (DE), A
    RET

; --------------------------------------------------
MB_PGETADR:
    CALL  MB_FRMEVL
    PUSH  HL
    CALL  MB_FRQINT
    EX    DE, HL
    POP   HL
    RET

; --------------------------------------------------
; Evaluate expression and force result to 16-bit integer
;  Inp: HL - BASIC text (expression)
;  Out: integer value in MB_DACC+2, HL = text after expression
;  Range-checked: anything outside -32768..+32767 raises "Overflow"
; --------------------------------------------------
MB_FRQINT:
    LD    BC, MB_FRCINT                             ; address of the integer-forcing routine
    PUSH  BC                                        ; push it as a marker / stack-checkpoint for error unwinding (never used as a return vector here)
    RST   GETYPTR                                   ; fetch type of current value: M set = integer, Z set = real, NZ+P = string
    RET   M                                         ; already an integer - done, value stays in MB_DACC+2
    RST   SIGN                                      ; test sign byte of MB_DACC (Z=0 for reals here)
    RET   M                                         ; negative integer-pending value - accept as is
    CALL  MB_FRCSNG                                 ; otherwise force to single real: integer -> real, string -> VAL() conversion
    LD    BC, 0x3245                                ; BC/DE = low/high halves of constant -32768 (exp 0x45, mantissa 0x32 80 76)
    LD    DE, 0x8076
    CALL  MB_FCOMP                                  ; compare MB_DACC with -32768
    RET   C                                         ; below the lower limit: return (C set = "too small" flag for callers such as FOR loops)
    LD    BC, 0x6545                                ; BC/DE = constant +32767 (exp 0x45, mantissa 0x45 60 53)
    LD    DE, 0x6053
    CALL  MB_FCOMP                                  ; compare MB_DACC with +32767
    JP    NC, MB_OVERR                              ; at or above the upper limit: "Overflow" error
    LD    BC, 0x65c5                                ; BC/DE = constant +32768 (exp 0x45, mantissa 0xC5 60 53)
    LD    DE, 0x6053
    JP    MB_FADD                                   ; add the 32768 bias and finish: tail call returns to MB_FRQINT's caller via the return address left by CALL MB_FRCSNG

; --------------------------------------------------
; Renumber
; --------------------------------------------------
MB_RESEQ:
    LD    BC, 10
    PUSH  BC
    LD    D, B
    LD    E, B
    JR    Z, MB_RESNN
    CP    ','
    JR    Z, MB_COLLNO
    PUSH  DE
    CALL  MB_LINSPC
    LD    B, D
    LD    C, E
    POP   DE
    JR    Z, MB_RESNN
MB_COLLNO:
    RST   SYNCHR
    DB    ','
    CALL  MB_LINSPC
    JR    Z, MB_RESNN
    POP   AF
    RST   SYNCHR
    DB    ','
    PUSH  DE
    CALL  MB_LINGET
    JP    NZ, MB_SNERR
    LD    A, D
    OR    E
    JP    Z, MB_FCERR
    EX    DE, HL
    EX    (SP), HL
    EX    DE, HL
MB_RESNN:
    PUSH  BC
    CALL  MB_FNDLIN
    POP   DE
    PUSH  DE
    PUSH  BC
    CALL  MB_FNDLIN
    LD    H, B
    LD    L, C
    POP   DE
    RST   DCOMPR
    EX    DE, HL
    JP    C, MB_FCERR
    POP   DE
    POP   BC
    POP   AF
    PUSH  HL
    PUSH  DE
    JR    MB_NXTRSL
MB_NXTRSC:
    ADD   HL, BC
    JP    C, MB_FCERR
    EX    DE, HL
    PUSH  HL
    LD    HL, 65529                                 ; test for current line number too big
    RST   DCOMPR
    POP   HL
    JP    C, MB_FCERR
MB_NXTRSL:
    PUSH  DE
    LD    E, (HL)
    INC   HL
    LD    D, (HL)
    LD    A, D
    OR    E
    EX    DE, HL
    POP   DE
    JR    Z, MB_LIN_EMPTY
    LD    A, (HL)
    INC   HL
    OR    (HL)
    DEC   HL
    EX    DE, HL
    JR    NZ, MB_NXTRSC
MB_LIN_EMPTY:
    PUSH  BC
    CALL  MB_SCCLIN
    POP   BC
    POP   DE
    POP   HL
MB_LIN_PTR_UPD:
    PUSH  DE
    LD    E, (HL)
    INC   HL
    LD    D, (HL)
    LD    A, D
    OR    E
    JR    Z, MB_SCCALL
    EX    DE, HL
    EX    (SP), HL
    EX    DE, HL
    INC   HL
    LD    (HL), E
    INC   HL
    LD    (HL), D
    EX    DE, HL
    ADD   HL, BC
    EX    DE, HL
    POP   HL
    JR    MB_LIN_PTR_UPD

; --------------------------------------------------
MB_SCCALL:
    LD    BC, MB_STPRDY
    PUSH  BC
    DB    CP_n

; --------------------------------------------------
MB_SCCLIN:
    DB    OR_n

; --------------------------------------------------
MB_SCCPTR:
    XOR   A
    LD    (MB_PTRFLG), A
    LD    HL, (MB_TXTTAB)  ;=0x733F
    DEC   HL

; --------------------------------------------------
MB_SCNPLN:
    INC   HL
    LD    A, (HL)
    INC   HL
    OR    (HL)
    RET   Z
    INC   HL
    LD    E, (HL)
    INC   HL
    LD    D, (HL)
MB_SCNEXT:
    RST   CHRGTR
MB_SCNEX2:
    OR    A
    JR    Z, MB_SCNPLN
    LD    C, A
    LD    A, (MB_PTRFLG)
    OR    A
    LD    A, C
    JR    Z, MB_SCNPT2
    CP    _ERROR
    JR    NZ, MB_NTERRG
    RST   CHRGTR
    CP    _GOTO
    JR    NZ, MB_SCNEX2
    RST   CHRGTR
    CP    LINCON
    JR    NZ, MB_SCNEX2
    PUSH  DE
    CALL  MB_LINGT3
    LD    A, D
    OR    E
    JR    NZ, MB_CHGPTR
    JR    MB_SCNEX3

; --------------------------------------------------
MB_NTERRG:
    CP    LINCON
    JR    NZ, MB_SCNEXT
    PUSH  DE
    CALL  MB_LINGT3

; --------------------------------------------------
MB_CHGPTR:
    PUSH  HL
    CALL  MB_FNDLIN
    DEC   BC
    LD    A, PTRCON
    JR    C, MB_MAKPTR
    CALL  MB_CRDONZ
    LD    HL, MB_LINM                               ;= "Undefined line "
    PUSH  DE
    CALL  MB_STROUT
    POP   HL
    CALL  MB_LINPRT
    POP   BC
    POP   HL
    PUSH  HL
    PUSH  BC
    CALL  MB_INPRT
MB_SCNPOP:
    POP   HL
MB_SCNEX3:
    POP   DE
    DEC   HL
MB_SCNEXT0:
    JR    MB_SCNEXT

; --------------------------------------------------
MB_LINM:
    DB    "Undefined line ", 0

; --------------------------------------------------
MB_SCNPT2:
    CP    PTRCON
    JR    NZ, MB_SCNEXT0
    PUSH  DE
    CALL  MB_LINGT3
    PUSH  HL
    EX    DE, HL
    INC   HL
    INC   HL
    INC   HL
    LD    C, (HL)
    INC   HL
    LD    B, (HL)
    LD    A, LINCON

; --------------------------------------------------
MB_MAKPTR:
    LD    HL, MB_SCNPOP
    PUSH  HL
    LD    HL, (MB_CONTXT)
MB_CONCH2:
    PUSH  HL
    DEC   HL
    LD    (HL), B
    DEC   HL
    LD    (HL), C
    DEC   HL
    LD    (HL), A
    POP   HL
    RET

; --------------------------------------------------
; Convert to linepointers to linenumbers if needed
; --------------------------------------------------
MB_DEPTR:
    LD    A, (MB_PTRFLG)
    OR    A
    RET   Z
    JP    MB_SCCPTR

; ===================================================
; ANSI - THE ROUTINES TO HANDLE ANSI FEATURES
; ===================================================
MB_OPTION:
    RST   SYNCHR
    DB    'B'
    RST   SYNCHR
    DB    'A'
    RST   SYNCHR
    DB    'S'
    RST   SYNCHR
    DB    'E'
    LD    A, (HL)
    SUB   '0'
    JP    C, MB_SNERR
    CP    2
    JP    NC, MB_SNERR
    LD    B, A
    LD    A, (MB_OPTVAL)
    CP    B
    JR    Z, MB_OPTION_ENDCHK
    LD    A, (MB_OPTFLAG_1)
    OR    A
    JP    NZ, MB_DDERR
    PUSH  HL
    LD    HL, (MB_ARYTAB)
    EX    DE, HL
    LD    HL, (MB_STREND)
    RST   DCOMPR
    JP    NZ, MB_DDERR
    POP   HL
    LD    A, B
    LD    (MB_OPTVAL), A
    INC   A
    LD    (MB_OPTFLAG_1), A
MB_OPTION_ENDCHK:
    RST   CHRGTR
    RET

; --------------------------------------------------
MB_STRPRN:
    LD    A, (HL)
    OR    A
    RET   Z
    CALL  MB_CALTTY
    INC   HL
    JP    MB_STRPRN

; --------------------------------------------------
MB_CALTTY:
    PUSH  AF
    JP    MB_TTYCHR

; --------------------------------------------------
MB_RANDOM:
    JR    Z, MB_INPRG
    CALL  MB_FRMEVL
    PUSH  HL
    CALL  MB_FRCINT
    JR    MB_STRNDS

; --------------------------------------------------
MB_INPRG:
    PUSH  HL
MB_INPRAG:
    LD    HL, MB_RANMES                             ;= "Random number seed "
    CALL  MB_STROUT
    CALL  MB_GET_NF
    POP   DE
    JP    C, MB_STPEND
    PUSH  DE
    INC   HL
    LD    A, (HL)
    CALL  MB_FIN
    LD    A, (HL)
    OR    A
    JR    NZ, MB_INPRAG
    CALL  MB_FRCINT
MB_STRNDS:
    CALL  MB_RNDMN2
    POP   HL
    RET

; --------------------------------------------------
MB_RANMES:
    DB    "Random number seed (-32768 to 32767)", 0

; --------------------------------------------------
; Scan ahead to find NEXT that matches a FOR
; --------------------------------------------------
MB_WNDSCN:
    LD    C, ERRWH
    JR    MB_SCNCNT
MB_NXTSCN:
    LD    C, ERRFN
MB_SCNCNT:
    LD    B, 0
    EX    DE, HL
    LD    HL, (MB_CURLIN)                           ;=0xFFFE
    LD    (MB_NXTLIN), HL
    EX    DE, HL
MB_FORINC:
    INC   B
MB_FNLOP:
    DEC   HL
MB_SCANCNT_LOOP:
    RST   CHRGTR
    JR    Z, MB_FORTRM
    CP    '"'
    JR    NZ, MB_FOR_ET
MB_FORSCN1:
    RST   CHRGTR
    OR    A
    JR    Z, MB_FORTRM
    CP    '"'
    JR    NZ, MB_FORSCN1
MB_FOR_ET:
    CP    _ELSE
    JR    Z, MB_FOR_RE
    CP    _THEN
    JR    NZ, MB_SCANCNT_LOOP
MB_FORTRM:
    OR    A
    JR    NZ, MB_FOR_RE
    INC   HL
    LD    A, (HL)
    INC   HL
    OR    (HL)
    LD    E, C
    JP    Z, MB_ERROR
    INC   HL
    LD    E, (HL)
    INC   HL
    LD    D, (HL)
    LD    (MB_NXTLIN), DE
MB_FOR_RE:
    RST   CHRGTR
    CP    _REM
    JR    NZ, MB_FOR_DT
    PUSH  BC
    CALL  MB_REM_ELSE
    POP   BC
    JR    MB_FORTRM
MB_FOR_DT:
    CP    _DATA
    JR    NZ, MB_FOR_WHWE
    PUSH  BC
    CALL  MB_DATAS
    POP   BC
    JR    MB_FORTRM
MB_FOR_WHWE:
    LD    A, C
    CP    ONECON + 10
    LD    A, (HL)
    JR    Z, MB_FOR_FRNX
    CP    _WHILE
    JR    Z, MB_FORINC
    CP    _WEND
    JR    NZ, MB_FNLOP
    DJNZ  MB_FNLOP
    RET
MB_FOR_FRNX:
    CP    _FOR
    JR    Z, MB_FORINC
    CP    _NEXT
    JR    NZ, MB_FNLOP
MB_FOR_DNXT:
    DEC   B
    RET   Z
    RST   CHRGTR
    JR    Z, MB_FORTRM
    EX    DE, HL
    LD    HL, (MB_CURLIN)                           ;=0xFFFE
    PUSH  HL
    LD    HL, (MB_NXTLIN)
    LD    (MB_CURLIN), HL                           ;=0xFFFE
    EX    DE, HL
    PUSH  BC
    CALL  MB_PTRGET
    POP   BC
    DEC   HL
    RST   CHRGTR
    LD    DE, MB_FORTRM
    JR    Z, MB_FOR_END
    RST   SYNCHR
    DB    ','
    DEC   HL
    LD    DE, MB_FOR_DNXT
MB_FOR_END:
    EX    (SP), HL
    LD    (MB_CURLIN), HL                           ;=0xFFFE
    POP   HL
    PUSH  DE
    RET
; --------------------------------------------------
MB_FINOVC:
    PUSH  AF
    LD    A, (MB_FLGOVC)
    LD    (MB_OVCSTR), A
    POP   AF
; --------------------------------------------------
MB_CLROVC:
    PUSH  AF
    XOR   A
    LD    (MB_FLGOVC), A
    POP   AF
    RET

; ==================================================
; Alphatronic specific subroutines
; ==================================================

; --------------------------------------------------
; Direct-mode key entry
; Drain pending-key flag/buffer, else wait for
; keypress (monitor/keyboard matrix);
; handles Shift menu pick; called from
; MB_INLIN_KEYLOOP and INKEY$
; --------------------------------------------------
MB_DIRECT_GETKEY:
    LD    A, (AL_VAR1)
    PUSH  AF
    XOR   A
    LD    (AL_VAR1), A
    POP   AF
    OR    A
    RET   NZ
    CALL  AL_KEYBUF_NEXT
    RET   NZ
    PUSH  HL
    PUSH  DE
    PUSH  BC

    ; Wait for key pressed
MB_DFN_WKEY:
    LD    A, (AL_MENU_FLAG)                         ;=1
    AND   A
    JR    Z, MB_DFN_NOMNU
    IN    A, (AL_KBD_XA)
    AND   0x4                                       ; bit 3 Shift key state
    LD    HL, AL_KEYSTATE
    CP    (HL)
    CALL  NZ, AL_MENUBAR_DRAW
MB_DFN_NOMNU:
    CALL  MON.CRTST
    AND   A
    JR    Z, MB_DFN_WKEY
    ; Direct mode: read key from monitor
MB_DIRECT_KEYRD:
    CALL  MON.CRTIN                                 ; Read key
    LD    HL, MB_DIRECT_KEYST
    INC   (HL)
    DEC   (HL)
    LD    (HL), 0
    JR    NZ, MB_NO_STMT
    CP    _INSTR
    JR    NZ, MB_DIRECT_MENU_PICK
    LD    (MB_DIRECT_KEYST), A
    JR    MB_NO_STMT
    ; Direct mode: menu bar item pick
MB_DIRECT_MENU_PICK:
    CP    _END
    JR    C, MB_NO_STMT
    CP    _GOSUB
    JR    NC, MB_NO_STMT
    SUB   _END
    RLCA
    RLCA
    RLCA
    RLCA
    LD    C, A
    LD    B, 0
    LD    HL, MENU_BAR1                             ;= "GOTO "
    ADD   HL, BC
    LD    A, (HL)                                   ;= "GOTO "
    OR    A
    JR    Z, MB_DFN_WKEY
    INC   HL
    LD    (AL_BUF_POS), HL
MB_NO_STMT:
    POP   BC
    POP   DE
    POP   HL
    RET

    ; Direct mode: key read with wrap handling
MB_DIRECT_KEYRD_WRAP:
    PUSH  HL
    PUSH  DE
    PUSH  BC
    JR    MB_DIRECT_KEYRD
MB_CTRLC_CHK:
    LD    A, (AL_VAR1)
    AND   A
    RET   NZ
    CALL  AL_KEYBUF_NEXT
    JR    NZ, MB_CTRLC_FLAGSET
    PUSH  HL
    PUSH  DE
    PUSH  BC
    CALL  MON.CRTST
    POP   BC
    POP   DE
    POP   HL
    AND   A
    RET   Z
    CALL  MB_DIRECT_KEYRD_WRAP

    ; Ctrl-C flag set path
MB_CTRLC_FLAGSET:
    LD    (AL_VAR1), A
    AND   A
    RET

; --------------------------------------------------
; Reads one character from pending-key buffer,
; advances pointer, A=0 on end
; --------------------------------------------------
AL_KEYBUF_NEXT:
    LD    A, (AL_BUF_POS)
    OR    A
    RET   Z
    PUSH  HL
    LD    HL, (AL_BUF_POS)
    LD    A, (HL)
    INC   HL
    LD    (AL_BUF_POS), HL
    POP   HL
    OR    A
    RET   NZ
    LD    (AL_BUF_POS), A
    RET

; --------------------------------------------------
MB_COUT:
    PUSH  HL
    PUSH  DE
    PUSH  BC
    PUSH  AF
    LD    C, A
    LD    HL, MB_CNTOFL
    XOR   A
    CP    (HL)
    LD    (HL), A
    LD    A, C
    JR    Z, MB_COUT_DUPCHK
    AND   A
    JP    M, MB_COUT_OUTCHR
    SUB   0x40
    LD    C, A
    JR    MB_COUT_OUTCHR

    ; duplicate character check
MB_COUT_DUPCHK:
    CP    _INSTR
    JR    NZ, MB_COUT_CHARCLASS
    LD    (HL), A
    CALL  MON.CRTOUT
    JR    MB_COUT_RET

    ; character class dispatch
MB_COUT_CHARCLASS:
    CP    ASCII_ESC
    JR    Z, MB_COUT_OUTRAW
    CP    ASCII_SP
    JR    C, MB_COUT_CTLPARSE
    CP    0x7f
    JR    Z, MB_COUT_CTLPARSE
    LD    C, A
    ; output character
MB_COUT_OUTCHR:
    LD    A, (MB_LINLEN)                            ;= 40
    LD    HL, (MB_CURPOS)                           ;= 1
    ; newline check (also checksum target)
MB_COUT_NLCHK:
    CP    H
    CALL  Z, AL_EDT_ROWST_CLR
    CALL  MON.CRTOUT
    CALL  AL_EDT_CTRLC_HDLR
    ; return path
MB_COUT_RET:
    POP   AF
    POP   BC
    POP   DE
    POP   HL
    RET

    ; Control character table scan
MB_COUT_CTLPARSE:
    LD    HL, MB_CTLTBL_BASE - 2
    LD    C, 9
    CALL  MB_CTLTBL_SCAN
    JR    MB_COUT_RET

    ; raw output pass-through
MB_COUT_OUTRAW:
    CALL  MB_CRTOUT_C
    JR    MB_COUT_RET

; --------------------------------------------------
; Output char in C to CRT (shared with editor)
; --------------------------------------------------
MB_CRTOUT_C:
    LD    C, A
    JP    MON.CRTOUT

; --------------------------------------------------
MB_LPT_OUTCHK:
    PUSH  HL
    PUSH  DE
    PUSH  BC
    PUSH  AF
    LD    C, A
    CALL  MON.LPTOUT
    JR    NC, MB_LPT_OUTRET
    XOR   A
    LD    (MB_LPTPOS), A
    LD    E, ERRDIO
    JP    MB_ERROR
    ; LPT output return path
MB_LPT_OUTRET:
    POP   AF
    POP   BC
    POP   DE
    POP   HL
    RET

; --------------------------------------------------
; Menu bar update/refresh
; --------------------------------------------------
AL_MENUBAR_UPD:
    LD    A, (AL_MENU_FLAG)      ;=1
    AND   A
    JR    NZ, AL_MENUBAR_DRAW
    CALL  MB_23TH_OFS
    LD    A, (MB_LINLEN)   ;= 40
    LD    B, A
    PUSH  BC
    PUSH  HL
AL_MENUBAR_CLRTEXT:
    LD    (HL), ASCII_SP
    INC   HL
    DJNZ  AL_MENUBAR_CLRTEXT
    POP   HL
    POP   BC
    LD    A, 0x8
    ADD   A, H
    LD    H, A
    LD    A, (MON.CPATR)       ;= 7
AL_MENUBAR_CLRATTR:
    LD    (HL), A
    INC   HL
    DJNZ  AL_MENUBAR_CLRATTR
    RET

; --------------------------------------------------
; Draws function-key menu bar 1 or 2 (if Shift key)
; no-op when menu flag clear;
; --------------------------------------------------
AL_MENUBAR_DRAW:
    LD    A, (AL_MENU_FLAG)                         ;=1
    AND   A
    RET   Z
    IN    A, (AL_KBD_XA)
    AND   0x4
    LD    (AL_KEYSTATE), A
    LD    HL, MENU_BAR1                             ;= "GOTO "
    JR    Z, MB_NOSHFT
    LD    HL, MENU_BAR2                             ;= "GOSUB "
MB_NOSHFT:
    PUSH  HL
    CALL  MON.CTVOFF
    CALL  MB_23TH_OFS
    CALL  AL_MENU_ITEMEND
    LD    C, 0x6
AL_MENU_OUTITEM:
    LD    B, D
AL_MENU_CHRCPY:
    EX    (SP), HL
    LD    A, (HL)                                   ;= "GOSUB "
    INC   HL
    EX    (SP), HL
    CP    ASCII_SP
    JR    NC, AL_MENU_HILITE
    LD    A, ASCII_SP
AL_MENU_HILITE:
    LD    (HL), A                                   ;= "GOSUB "
    PUSH  HL
    LD    A, 0x8
    ADD   A, H
    LD    H, A
    LD    (HL), 0x87
    POP   HL
    INC   HL
    DJNZ  AL_MENU_CHRCPY
    CALL  AL_MENU_ITEMEND
    EX    (SP), HL
    LD    A, 0x10
    SUB   D
    PUSH  DE
    LD    D, 0
    LD    E, A
    ADD   HL, DE
    POP   DE
    EX    (SP), HL
    DEC   C
    JR    NZ, AL_MENU_OUTITEM
AL_MENU_SPACEGAP:
    LD    (HL), ASCII_SP
    PUSH  HL
    LD    A, 0x8
    ADD   A, H
    LD    H, A
    LD    (HL), 0x7
    POP   HL
    INC   HL
    DEC   E
    JR    NZ, AL_MENU_SPACEGAP
    CALL  MON.CTVON
    POP   HL
    RET

; --------------------------------------------------
; Menu item end marker check
; --------------------------------------------------
AL_MENU_ITEMEND:
    LD    (HL), ASCII_SP
    PUSH  HL
    LD    A, 8
    ADD   A, H
    LD    H, A
    LD    (HL), 0x7
    POP   HL
    INC   HL
    RET

; --------------------------------------------------
; Get offset of 23th screen line
;  Out: HL - offset
;       DE - ?
; --------------------------------------------------
MB_23TH_OFS:
    LD    A, (MB_LINLEN)   ;= 40
    CP    40
    LD    DE, 0x504
    LD    HL, 0xf398       ;line 23 offset in 40x24 mode
    RET   Z
    LD    DE, 0xc08
    LD    HL, 0xf730       ;line 23 offset in 80x24 mode
    RET
    DB    CALL_nn
MB_LPTST_CALL:
    DW    MON.LPTST
    AND   A
    RET

; --------------------------------------------------
;  Cassette: stop motor, unstack registers
; --------------------------------------------------
AL_CASS_STOPMOTOR:
    PUSH  BC
    PUSH  DE
    PUSH  HL
    PUSH  AF
    LD    B, 0x37
    CALL  AL_CASS_MOTOR_OFF
    CALL  AL_BEEP_ON
AL_CASS_UNSTACK:
    POP   AF
AL_CASS_RET:
    POP   HL
    POP   DE
    POP   BC
    RET

; --------------------------------------------------
; Cassette: read one bit from data port
; --------------------------------------------------
AL_CASS_BITREAD:
    PUSH  BC
    PUSH  DE
    PUSH  HL
AL_CASS_WAITSYNC:
    CALL  AL_CASS_READBIT
    JR    C, AL_CASS_TMOUTACT
    IN    A, (AL_CASS_DATA)
    LD    B, A
    AND   0x38
    JR    NZ, AL_CASS_HDRCHK
    LD    A, B
    AND   0x2
    JR    Z, AL_CASS_WAITSYNC
    IN    A, (AL_CASS_ADDR)
    JP    AL_CASS_RET
AL_CASS_TMOUTACT:
    LD    A, (MB_CL_FLG1)
    OR    A
    CALL  Z, MB_SCRTCH
    CALL  AL_CASS_MOTOR_ON
AL_CASS_ERRSTOP:
    LD    B, 0x37
    CALL  AL_CASS_MOTOR_ON
    OUT   (AL_PORT_33), A
    EI
    JP    MB_STOPRG
AL_CASS_HDRCHK:
    CALL  AL_CASS_STOPMOTOR
    LD    A, (MB_CL_FLG2)
    INC   A
    JP    NZ, AL_CASS_RET
    JR    AL_CASS_WAITSYNC

; --------------------------------------------------
; Cassette: start motor, send command
; --------------------------------------------------
AL_CASS_MOTOR_ON:
    PUSH  BC
    PUSH  DE
    PUSH  HL
    PUSH  AF
AL_CASS_SENDCMD:
    LD    A, 0x92
    OUT   (AL_CASS_DATA), A
    OUT   (AL_CASS_DATA), A
    LD    A, 0x40
    OUT   (AL_CASS_DATA), A
    LD    A, 0xfa
    OUT   (AL_CASS_DATA), A
    LD    A, 0x37
    OUT   (AL_CASS_DATA), A
    LD    A, (MON.SFF0)
    LD    B, A
    ; Turn cassete motor ON
    LD    A, 0x4
    OR    B
    LD    (MON.SFF0), A
    OUT   (MON.SPORT), A
    CALL  AL_BEEP_OFF
    JR    AL_CASS_UNSTACK

; --------------------------------------------------
; Cassette: idle preparation
; --------------------------------------------------
AL_CASS_IDLEPREP:
    PUSH  BC
    PUSH  DE
    PUSH  HL
    PUSH  AF
    CALL  AL_BEEP_ON
    CALL  AL_CASS_DELAY_SET
    CALL  AL_CASS_DELAY_SET
    LD    B, 0x33
    CALL  AL_CASS_MOTOR_OFF
    CALL  AL_CASS_DELAY_SET
    JR    AL_CASS_UNSTACK

; --------------------------------------------------
; Cassette: set bit delay constant
; --------------------------------------------------
AL_CASS_DELAY_SET:
    LD    BC, 0x8000


; --------------------------------------------------
; Cassette: wait for bit transition
; --------------------------------------------------
AL_CASS_BITWAIT:
    CALL  AL_CASS_READBIT
    RET   C
    EX    (SP), HL
    EX    (SP), HL
    DEC   BC
    LD    A, B
    OR    C
    JR    NZ, AL_CASS_BITWAIT
    RET
    CALL  AL_CASS_BITWRITE                               ; ToDo: check usage

; --------------------------------------------------
; Cassette: write one bit to data port
; --------------------------------------------------
AL_CASS_BITWRITE:
    PUSH  BC
    PUSH  DE
    PUSH  HL
    PUSH  AF
AL_CASS_WAITRDY:
    IN    A, (AL_CASS_DATA)
    RRCA
    JR    C, AL_CASS_OUTBIT
    CALL  AL_CASS_READBIT
    JR    NC, AL_CASS_WAITRDY
    CALL  MB_CS_FINL
    JR    AL_CASS_ERRSTOP
AL_CASS_OUTBIT:
    POP   AF
    OUT   (AL_CASS_ADDR), A
    JP    AL_CASS_RET

; --------------------------------------------------
;
; --------------------------------------------------
MB_CS_FINL:
    PUSH  BC
    PUSH  DE
    PUSH  HL
    PUSH  AF
    LD    BC, 1000
    CALL  AL_CASS_BITWAIT
    JR    AL_CASS_SENDCMD

; --------------------------------------------------
; Cassette: stop motor
; --------------------------------------------------
AL_CASS_MOTOR_OFF:
    LD    A, (MON.SFF0)
    LD    C, A

    ; Casette motor OFF
    LD    A, 0xfb
    AND   C
    LD    (MON.SFF0), A
    OUT   (MON.SPORT), A
    CALL  AL_CASS_CMDOUT

; --------------------------------------------------
; Cassette: command output routine
; --------------------------------------------------
AL_CASS_CMDOUT:
    LD    A, 0x92
    OUT   (AL_CASS_DATA), A
    OUT   (AL_CASS_DATA), A
    LD    A, 0x40
    OUT   (AL_CASS_DATA), A
    LD    A, 0xce
    OUT   (AL_CASS_DATA), A
    LD    A, B
    OUT   (AL_CASS_DATA), A
    RET

; --------------------------------------------------
; Cassette: restart (bare RET)
; --------------------------------------------------
AL_CASS_RESTART:
    RET

; --------------------------------------------------
; Turn beeper OFF
; --------------------------------------------------
AL_BEEP_OFF:
    LD    A, (MON.SFF0)
    AND   0xf7
    JR    AL_SET_SPORT

; --------------------------------------------------
; Turn beeper ON
; --------------------------------------------------
AL_BEEP_ON:
    LD    A, (MON.SFF0)
    OR    0x8

    ; Out A to system port 0x10 (SPORT)
AL_SET_SPORT:
    LD    (MON.SFF0), A
    OUT   (MON.SPORT), A
    RET

; --------------------------------------------------
; Cassette: read bit low-level
; --------------------------------------------------
AL_CASS_READBIT:
    IN    A, (AL_KBD_XA)
    ADD   A, A
    RET

; --------------------------------------------------
; FDC: read/write sector wrapper
; (sets params, calls AL_FDC_OP)
; --------------------------------------------------
AL_FDC_RWSECT:
    PUSH  BC
    PUSH  DE
    PUSH  HL
    LD    (AL_FDC_RW_FLG), A
    LD    A, 0x45
    JR    C, AL_FDC_SETPARAMS
    INC   A
    ; set track/sector/side parameters
AL_FDC_SETPARAMS:
    LD    (AL_FDC_CMD), A
    LD    (AL_FDC_BUFADDR), DE
    LD    A, B
    OR    A
    RRA
    LD    (AL_FDC_TRK), A
    LD    A, 0
    RLA
    LD    (AL_FDC_SIDE), A
    LD    A, C
    LD    (AL_FDC_SCT), A
    CALL  AL_FDC_OP
    POP   HL
    POP   DE
    POP   BC
    OR    A
    RET   Z
    SCF
    RET

; --------------------------------------------------
; Get current graphics cursor position (A=col, HL=row)
; --------------------------------------------------
MB_GFX_CURPX_GET:
    LD    A, (MB_GFX_CURCOL)
    LD    HL, (MB_GFX_CURROW)
    RET

; --------------------------------------------------
; Set current graphics cursor position
; --------------------------------------------------
MB_GFX_CURPX_SET:
    LD    (MB_GFX_CURCOL), A
    LD    (MB_GFX_CURROW), HL
    RET

; --------------------------------------------------
; Check X,Y in screen range (sets C if out)
; --------------------------------------------------
MB_RANGE_CHKXY:
    PUSH  HL
    PUSH  BC
    LD    A, B
    OR    D
    JP    M, MB_FCERR
    LD    B, 1
    EX    DE, HL
    LD    DE, 72
    LD    A, (AL_MENU_FLAG)                         ;=1
    OR    A
    JR    Z, MB_YCORD_MOF
    LD    DE, 69

MB_YCORD_MOF:
    RST   DCOMPR
    JR    C, MB_RANGE_XCHK
    EX    DE, HL
    DEC   HL
    LD    B, 0
    ; Range check: X comparison
MB_RANGE_XCHK:
    EX    (SP), HL
    LD    DE, 160
    LD    A, (MB_LINLEN)                            ;= 40
    CP    80
    JR    Z, MB_RANGE_XLIMIT
    LD    DE, 80
    ; Range check: X limit test
MB_RANGE_XLIMIT:
    RST   DCOMPR
    JR    C, MB_RANGE_OKRET
    EX    DE, HL
    DEC   HL
    LD    B, 0
    ; Range check: OK return
MB_RANGE_OKRET:
    LD    A, B
    RRCA
    LD    B, H
    LD    C, L
    POP   DE
    POP   HL
    RET

; ==================================================
; Pixel position read/write
; ==================================================

; --------------------------------------------------
; Convert (X,Y) to VRAM pixel address + bit mask
; --------------------------------------------------
MB_PXPOS_CALC:
    PUSH  BC
    PUSH  DE
    LD    A, 0x2
    CALL  MB_MOD_CALC
    LD    H, C
    LD    D, A
    LD    C, E
    LD    A, 0x3
    CALL  MB_MOD_CALC
    LD    L, C
    RLCA
    ADD   A, D
    LD    B, A
    LD    A, 1
    JR    Z, MB_PXPOS_SETVRAM

    ; Pixel position: bit loop
MB_PXPOS_BITLOOP:
    RLCA
    DJNZ  MB_PXPOS_BITLOOP

    ; Pixel position: set VRAM pointer
MB_PXPOS_SETVRAM:
    LD    (MB_GFX_CURCOL), A
    CALL  MB_ROWVADR
    LD    (MB_GFX_CURROW), HL
    POP   DE
    POP   BC
    RET

; --------------------------------------------------
; Graphics color check (default vs specified)
; --------------------------------------------------
MB_GFX_CLRCHK:
    CP    0xff
    JR    Z, MB_GFX_DEFCLR_SET
    CP    0x8
    CCF
    RET   C
MB_GFX_DEFCLR_SET:
    LD    (MB_GFX_CURCLR), A
    RET

; --------------------------------------------------
; Read pixel value at current position
; --------------------------------------------------
MB_READ_PX:
    PUSH  DE
    CALL  MB_GFX_CELLTEST
    JR    NC, MB_READPX_RET
    LD    DE, 0x800
    ADD   HL, DE
    LD    D, A
    LD    A, (MB_GFX_CURCOL)
    AND   D
    LD    A, (HL)
    JR    NZ, MB_READPX_COLOR
    RRCA
    RRCA
    RRCA
MB_READPX_COLOR:
    AND   0x7
MB_READPX_RET:
    POP   DE
    RET

; --------------------------------------------------
; Write pixel at current position (BRES/STEP target)
; --------------------------------------------------
MB_WRITE_PX:
    PUSH  DE
    CALL  MB_GFX_CELLTEST
    LD    D, A
    LD    A, (MB_GFX_CURCLR)
    LD    E, A
    LD    A, (MB_GFX_CURCOL)
    INC   E
    JR    NZ, MB_WRITEPX_SETBIT
    CPL
    AND   D
    JR    MB_WRITEPX_CHARPREP
MB_WRITEPX_SETBIT:
    OR    D
MB_WRITEPX_CHARPREP:
    CP    ASCII_SP
    JR    C, MB_WRITEPX_STORE
    CPL
    SUB   0x40
MB_WRITEPX_STORE:
    LD    DE, 0x800
    CALL  MB_WVSYNC
    LD    (HL), A
    ADD   HL, DE
    LD    A, (HL)
    AND   0xf8
    LD    D, A
    LD    A, (MB_GFX_CURCLR)
    INC   A
    JR    Z, MB_WRITEPX_RET
    DEC   A
    OR    D
    CALL  MB_WVSYNC
    LD    (HL), A
MB_WRITEPX_RET:
    POP   DE
    RET

; --------------------------------------------------
; Wait for CRT controller ready to update VRAM
; without flicker
; --------------------------------------------------
MB_WVSYNC:
    PUSH  AF
    IN    A, (MON.SPORT)
    RLCA
    JR    NC, MB_WVSYN2
    LD    A, 6
MB_WVSYNC_DLY:
    DEC   A
    JR    NZ, MB_WVSYNC_DLY
MB_WVSYN2:
    IN    A, (MON.SPORT)
    RLCA
    JR    NC, MB_WVSYN2
    POP   AF
    RET

; --------------------------------------------------
; Test if screen cell is blank
; --------------------------------------------------
MB_GFX_CELLTEST:
    LD    HL, (MB_GFX_CURROW)
    CALL  MB_WVSYNC
    LD    A, (HL)
    CP    0x20
    JR    C, MB_GFX_CELLNOTBLANK
    CP    0x80
    JR    C, MB_GFX_CELLBLANK
    CP    0xa0
    JR    NC, MB_GFX_CELLBLANK
    CPL
    SUB   0x40
    JR    MB_GFX_CELLNOTBLANK
MB_GFX_CELLBLANK:
    XOR   A
    RET
MB_GFX_CELLNOTBLANK:
    SCF
    RET
    PUSH  DE
    LD    A, (MB_GFX_CURCOL)
    CP    0x3
    JR    C, MB_GFX_ROWUP
MB_PXPOS_SHIFTR:
    RRCA
    RRCA
    LD    (MB_GFX_CURCOL), A
    POP   DE
    RET
MB_GFX_ROWUP:
    LD    HL, (MB_GFX_CURROW)
    LD    A, (MB_LINLEN)   ;= 40
    CPL
    INC   A
    LD    E, A
    LD    D, 0xff
    ADD   HL, DE
    LD    (MB_GFX_CURROW), HL
    LD    A, (MB_GFX_CURCOL)
    RRCA
    RRCA
    JR    MB_PXPOS_SHIFTR

; --------------------------------------------------
; Step right: advance to next cell (BRES step fn)
; --------------------------------------------------
MB_PXSTEP_RIGHT:
    PUSH  DE
    LD    A, (MB_GFX_CURCOL)
    CP    0x10
    JR    NC, MB_GFX_ROWDOWN
MB_PXSTEP_SHIFTL:
    RLCA
    RLCA
    LD    (MB_GFX_CURCOL), A
    POP   DE
    RET
MB_GFX_ROWDOWN:
    LD    HL, (MB_GFX_CURROW)
    LD    A, (MB_LINLEN)                            ;= 40
    LD    E, A
    LD    D, 0
    ADD   HL, DE
    LD    (MB_GFX_CURROW), HL
    LD    A, (MB_GFX_CURCOL)
    RLCA
    RLCA
    JR    MB_PXSTEP_SHIFTL

; --------------------------------------------------
; Step forward (Bresenham step function, +X)
; --------------------------------------------------
MB_PXSTEP_FWD:
    LD    A, (MB_GFX_CURCOL)
    LD    L, A
    AND   0x15
    LD    A, L
    JR    Z, MB_PXSTEP_NEXTCELL
    RLCA
MB_PXSTEP_STOREBIT:
    LD    (MB_GFX_CURCOL), A
    RET
MB_PXSTEP_NEXTCELL:
    LD    HL, (MB_GFX_CURROW)
    INC   HL
    LD    (MB_GFX_CURROW), HL
    RRCA
    JR    MB_PXSTEP_STOREBIT
MB_PXSTEP_BWD:
    LD    A, (MB_GFX_CURCOL)
    LD    L, A
    AND   0x15
    LD    A, L
    JR    NZ, MB_PXSTEP_PREVCELL
    RRCA
MB_PXSTEP_BWDBIT:
    LD    (MB_GFX_CURCOL), A
    RET
MB_PXSTEP_PREVCELL:
    LD    HL, (MB_GFX_CURROW)
    DEC   HL
    LD    (MB_GFX_CURROW), HL
    RLCA
    JR    MB_PXSTEP_BWDBIT

; --------------------------------------------------
; Draw horizontal line segment
; --------------------------------------------------
MB_GFX_HLINE:
    LD    B, L
MB_HLINE_LP:
    CALL  MB_WRITE_PX
    CALL  MB_PXSTEP_FWD
    DJNZ  MB_HLINE_LP
    RET

; --------------------------------------------------
; Compute VRAM row base address from row number
; --------------------------------------------------
MB_ROWVADR:
    PUSH  HL
    LD    A, L
    CALL  MB_MUL40_A
    LD    A, (MB_LINLEN)                            ;= 40
    CP    '('
    JR    Z, MB_ROWVADR_CALC
    ADD   HL, HL
MB_ROWVADR_CALC:
    EX    DE, HL
    POP   HL
    LD    L, H
    LD    H, 0
    ADD   HL, DE
    LD    DE, 0xf000
    ADD   HL, DE
    RET

; --------------------------------------------------
; Multiply A by 40 (row width in 40-col mode)
; --------------------------------------------------
MB_MUL40_A:
    LD    H, 0
    LD    L, A
    ADD   HL, HL
    ADD   HL, HL
    ADD   HL, HL
    LD    C, L
    LD    B, H
    ADD   HL, HL
    ADD   HL, HL
    ADD   HL, BC
    RET

; --------------------------------------------------
; Modulo calculation helper
; --------------------------------------------------
MB_MOD_CALC:
    LD    B, A
    LD    A, C
    LD    C, 0xff
MB_MODULO_LP:
    INC   C
    SUB   B
    JR    NC, MB_MODULO_LP
    ADD   A, B
    RET

; --------------------------------------------------
; Evaluate byte expression (A)
; --------------------------------------------------
MB_EVAL_BYTE:
    PUSH  DE
    PUSH  BC
    CALL  MB_GETBYT
    POP   BC
    POP   DE
    RET

; --------------------------------------------------
; Output character in A
; --------------------------------------------------
MB_OUTCHR_A:
    PUSH  HL
    PUSH  BC
    LD    C, A
    CALL  MON.CRTOUT
    POP   BC
    POP   HL
    RET

; --------------------------------------------------
; COLOR statement: parse FG[,BG][,ATTR] args
; --------------------------------------------------
MB_COLOR_PARSEARGS:
    CP    ','
    JR    Z, MB_COLOR_PARSEBG
    CALL  MB_EVAL_BYTE
    CALL  MB_COLOR_VALCHK
    LD    (MB_COLOR_FGBG), A                        ;=0x7
    RET   Z
MB_COLOR_PARSEBG:
    RST   SYNCHR
    DB    ','
    CP    ','
    JR    Z, MB_COLOR_PARSEATTR
    CALL  MB_EVAL_BYTE
    CALL  MB_COLOR_VALCHK
    LD    (MB_COLOR_FGBG+1), A
    RET   Z
MB_COLOR_PARSEATTR:
    RST   SYNCHR
    DB    ','
    CALL  MB_EVAL_BYTE
    CP    0x4
    JP    NC, MB_FCERR
    LD    (MB_COLOR_ATTR), A
    RET

; --------------------------------------------------
; COLOR: value range check
; --------------------------------------------------
MB_COLOR_VALCHK:
    PUSH  AF
    CP    0x8
    JR    NC, MB_COLOR_FCERR
    POP   AF
    RET

; --------------------------------------------------
; COLOR: same-color check (skip if unchanged)
; --------------------------------------------------
MB_COLOR_SAMECHK:
    PUSH  HL
    LD    HL, (MB_COLOR_FGBG)
    LD    A, L
    CP    H
    POP   HL
    RET   NZ
    LD    HL, (MB_COLOR_SAVED)
    LD    (MB_COLOR_FGBG), HL
MB_COLOR_FCERR:
    JP    MB_FCERR

; --------------------------------------------------
; COLOR: send VT100 escape sequence
; --------------------------------------------------
MB_COLOR_SENDVT:
    LD    A, ASCII_ESC
    CALL  MB_OUTCHR_A
    LD    A, C
    JP    MB_OUTCHR_A

; --------------------------------------------------
; COLOR: print digit of color value
; --------------------------------------------------
MB_COLOR_PRINTDIG:
    LD    A, (HL)
    OR    0x30
    JP    MB_OUTCHR_A

; --------------------------------------------------
; KEY and KEY LIST command
; --------------------------------------------------
MB_KEY:
    CP    _LIST
    JR    NZ, MB_KEY_SET                            ; Set FN Key

    ; Else display FN Keys
    RST   CHRGTR
    PUSH  HL
    LD    HL, MENU_BAR1                             ; MB_FNKSTR

    ;for 6 keys
    LD    C, 6
MB_KEY_DNXT:
    CALL  MB_KEYL_OUT                               ; call 2 time for display plain and shifted keys
    CALL  MB_KEYL_OUT
    CALL  MB_CRDO
    DEC   C
    JR    NZ, MB_KEY_DNXT
    POP   HL
    RET

; --------------------------------------------------
; Output KEY Label with spaces
;  Inp: HL -> Key label string
;  Out: HL -> after label string
; --------------------------------------------------
MB_KEYL_OUT:
    LD    B, 16
    CALL  MB_OUTC_B
    LD    B, 3
MB_KEYL_ONX:
    RST   OUTDO
    DJNZ  MB_KEYL_ONX
    RET

; --------------------------------------------------
; Output string to console
;  Inp: HL -> string
;       B - length
;  Out: A = 0x20 (Space)
; --------------------------------------------------
MB_OUTC_B:
    LD    A, (HL)
    CP    0x7f
    JR    Z, MB_KEYL_SP
    CP    ASCII_SP
    JR    NC, MB_KEY_ALPF
MB_KEYL_SP:
    LD    A, ASCII_SP
MB_KEY_ALPF:
    RST   OUTDO
    INC   HL
    DJNZ  MB_OUTC_B
    LD    A, ASCII_SP
    RET

; --------------------------------------------------
MB_KEY_SET:
    CALL  MB_GETBYT                                  ; Get FN key number
    DEC   A
    CP    12                                         ; Check for FN key number <=12
    JP    NC, MB_FCERR
    EX    DE, HL

    ; Calc address of FN key string (base + 16*n)
    LD    L, A
    LD    H, 0
    ADD   HL, HL
    ADD   HL, HL
    ADD   HL, HL
    ADD   HL, HL
    LD    BC, MENU_BAR1                              ;= "GOTO "
    ADD   HL, BC
    PUSH  HL
    EX    DE, HL
    RST   SYNCHR

    ; Get new FN Key value
    DB    ','
    CALL  MB_FRMEVL
    PUSH  HL
    CALL  MB_FRESTR
    LD    B, (HL)
    INC   HL
    LD    E, (HL)
    INC   HL
    LD    D, (HL)
    POP   HL
    EX    (SP), HL
    LD    C, 15
    LD    A, B
    AND   A
    JR    Z, MB_KSET_ZFIL
MB_KSET_STOR:
    LD    A, (DE)
    AND   A
    JP    Z, MB_FCERR                               ; Illegal function call
    LD    (HL), A
    INC   DE
    INC   HL
    DEC   C
    JR    Z, MB_KSET_END
    DJNZ  MB_KSET_STOR
MB_KSET_ZFIL:
    LD    (HL), B
    INC   HL
    DEC   C
    JR    NZ, MB_KSET_ZFIL
MB_KSET_END:
    LD    (HL), C
    CALL  AL_MENUBAR_DRAW
    POP   HL
    RET

; --------------------------------------------------
; COLOR statement
; COLOR foreColor,  backColor,  attribute
; --------------------------------------------------
MB_COLOR:
    JP    Z, MB_MOERR                                ; Missing operand
    LD    C, 0x55

; --------------------------------------------------
; COLOR: full parse entry
; --------------------------------------------------
MB_COLOR_PARSE:
    PUSH  BC
    PUSH  HL
    LD    HL, (MB_COLOR_FGBG)
    LD    (MB_COLOR_SAVED), HL
    POP   HL
    CALL  MB_COLOR_PARSEARGS
    CALL  MB_COLOR_SAMECHK
    POP   BC

; --------------------------------------------------
; COLOR: apply VT sequence to screen
; --------------------------------------------------
MB_COLOR_APPLYVT:
    PUSH  HL
    PUSH  BC
    LD    HL, MB_COLOR_ATTR
    LD    C, 0x56
    CALL  MB_COLOR_SENDVT
    CALL  MB_COLOR_PRINTDIG
    POP   BC
    CALL  MB_COLOR_SENDVT
    LD    HL, MB_COLOR_FGBG
    CALL  MB_COLOR_PRINTDIG
    INC   HL
    CALL  MB_COLOR_PRINTDIG
    POP   HL
    RET

; --------------------------------------------------
; Clear screen and home cursor
; CLS foreColor,  backColor,  attribute
; --------------------------------------------------
MB_CLS:
    LD    C, 84
    JR    Z, MB_CLS_DEFCOLOR
    CALL  MB_COLOR_PARSE
    JR    MB_CLS_CLEAREXEC
MB_CLS_DEFCOLOR:
    PUSH  HL
    LD    HL, 7
    LD    (MB_COLOR_FGBG), HL
    XOR   A
    LD    (MB_COLOR_ATTR), A
    POP   HL
    CALL  MB_COLOR_APPLYVT
MB_CLS_CLEAREXEC:
    PUSH  HL
    CALL  AL_EDT_VT_GOTOHOME
    CALL  AL_MENUBAR_DRAW
    POP   HL
    RET

; --------------------------------------------------
; Change text screen width to 40/80 columns
; WIDTH <expr>
; Or set Printer line width
; WIDTH LPRINT <expr>
; --------------------------------------------------
MB_WIDTH:
    CP    _LPRINT
    JR    Z, MB_WIDTH_LPRINT
    CALL  MB_GETBYT
    CP    80
    JR    Z, MB_SET_WIDTH
    CP    40
    JP    NZ, MB_FCERR
MB_SET_WIDTH:
    LD    (MB_LINLEN), A                            ;= 40
    PUSH  AF
    DEC   HL
    RST   CHRGTR
    JP    NZ, MB_SNERR
    LD    A, (AL_SCRN_ROWS)                         ;= 24
    LD    (MB_SCRN_ROWS2), A
    LD    A, 1
    LD    (MB_SCRN_ROWLIM), A
    POP   AF
    PUSH  HL
    CP    40
    LD    B, 0
    JR    Z, MB_WIDTH_SETMODE
    INC   B
MB_WIDTH_SETMODE:
    LD    E, A
MB_WIDTH_SENDVT:
    SUB   14
    JR    NC, MB_WIDTH_SENDVT
    ADD   A, 28
    CPL
    INC   A
    ADD   A, E
    LD    (MB_CLMLST), A                            ;=0xE
    LD    A, 0x30
    ADD   A, B
    PUSH  AF
    LD    C, 0x53
    CALL  MB_COLOR_SENDVT
    POP   AF
    CALL  MB_OUTCHR_A
    LD    C, 0x54
    CALL  MB_COLOR_APPLYVT
    LD    A, 0xc
    CALL  MB_COUT
    CALL  AL_MENUBAR_DRAW
    POP   HL
    RET

; --------------------------------------------------
MB_WIDTH_LPRINT:
    INC   HL
    CALL  MB_GETBYT
    OR    A
    JP    Z, MB_COLOR_FCERR
    LD    (MB_LPTSIZ), A                            ;= 255
    RET

; --------------------------------------------------
; Cursor positioning
; LOCATE [<col>] [, <row>]
; --------------------------------------------------
MB_LOCATE:
    CALL  MB_GETBYT                                 ; get column
    OR    A
    JP    M, MB_FCERR
    LD    A, (MB_LINLEN)                            ; check column value < screen width
    INC   E
    CP    E
    JP    C, MB_FCERR
    PUSH  DE
    RST   SYNCHR
    DB    ','
    CALL  MB_GETBYT                                 ; Get row
    OR    A
    JP    M, MB_FCERR
    LD    A, (AL_MENU_FLAG)                         ;=1
    OR    A
    LD    A, (MON.LINESZ)                           ;= 24
    JR    Z, MB_LOC_NOMNU
    DEC   A
MB_LOC_NOMNU:
    INC   E
    CP    E
    JP    C, MB_FCERR
    ; put target coords to HL
    EX    (SP), HL
    LD    H, L
    LD    L, E

; --------------------------------------------------
; LOCATE: execute cursor positioning
; --------------------------------------------------
MB_LOCATE_EXEC:
    PUSH  HL
    CALL  AL_EDT_SETCURS
    POP   AF
    LD    (MB_TTYPOS), A
    POP   HL
    DEC   HL
    RST   CHRGTR
    RET   Z
    RST   SYNCHR
    DB    ','
    CALL  MB_GETBYT
    ADD   A, 0xff
    LD    A, 0x12
    ADC   A, 0
    LD    C, A
    JP    MB_COLOR_SENDVT

; --------------------------------------------------
MB_CONSOLE:
    LD    DE, (MB_SCRN_GEO_POS)                     ;=0x1801
    CP    0x2c
    JR    Z, MB_CONSOLE_PARSEMENU
    CALL  MB_EVAL_BYTE
    PUSH  AF
    INC   A
    LD    E, A
    LD    A, (AL_SCRN_ROWS)                         ;= 24
    CP    E
    JP    C, MB_FCERR
    POP   AF
    LD    A, D
    JR    NZ, MB_CONSOLE_PARSEMENU
    PUSH  AF
    JR    MB_CONSOLE_CHKROW

    ; parse optional menu-bar argument
MB_CONSOLE_PARSEMENU:
    RST   SYNCHR
    DB    ','
    CP    ','
    JR    Z, MB_CONSOLE_PARMENU
    CALL  MB_EVAL_BYTE
    PUSH  AF
    ADD   A, E
    ; row parameter check
MB_CONSOLE_CHKROW:
    CP    E
    JP    Z, MB_FCERR
    JP    C, MB_FCERR
    DEC   A
    LD    D, A
    LD    A, (AL_SCRN_ROWS)                         ;= 24
    CP    D
    JP    C, MB_FCERR
    POP   AF
    JR    Z, MB_CONSOLE_EXEC
    ; menu bar flag handling
MB_CONSOLE_PARMENU:
    RST   SYNCHR
    DB    ','
    CP    ','
    JR    Z, MB_CONSOLE_PARROW
    CALL  MB_EVAL_BYTE
    PUSH  AF
    CP    0x2
    JP    NC, MB_FCERR
    LD    (AL_MENU_FLAG), A                         ;=1
    POP   AF
    JR    Z, MB_CONSOLE_EXEC
    ; row parameter handling
MB_CONSOLE_PARROW:
    RST   SYNCHR
    DB    ','
    CALL  MB_EVAL_BYTE
    PUSH  AF
    CP    0x2
    JP    NC, MB_FCERR
    SUB   1
    LD    (MB_CONSOLE_ROWFLG), A
    POP   AF
    JP    NZ, MB_SNERR
    ; main execute body
MB_CONSOLE_EXEC:
    PUSH  HL
    CALL  MB_CONSOLE_GEOMCALC
    LD    A, (MB_CURPOS)                            ;= 1
    LD    B, A
    LD    A, (MB_SCRN_ROWS2)                        ;= 24
    CP    B
    JR    NC, MB_CONSOLE_FINISH
    LD    HL, (MB_CURPOS)                           ;= 1
    LD    L, A
    LD    BC, MB_CONSOLE_FINISH
    PUSH  BC
    CALL  MB_LOCATE_EXEC
    ; update menu bar, return
MB_CONSOLE_FINISH:
    CALL  AL_MENUBAR_UPD
    POP   HL
    RET

; --------------------------------------------------
; Compute screen geometry (rows/cols)
; --------------------------------------------------
MB_CONSOLE_GEOMCALC:
    PUSH  BC
    PUSH  HL
    LD    A, (AL_MENU_FLAG)
    AND   A
    LD    B, 0
    JR    Z, MB_CONS_GEO_CLAMPROW
    INC   B
    ; clamp row
MB_CONS_GEO_CLAMPROW:
    LD    A, (AL_SCRN_ROWS)                         ;= 24
    SUB   B
    EX    DE, HL
    CP    L
    JR    NC, MB_CONS_GEO_CLAMPCOL
    LD    L, A
    ; clamp column
MB_CONS_GEO_CLAMPCOL:
    ADD   A, B
    CP    H
    JR    NC, MB_CONS_GEO_SAVE
    LD    H, A
    CP    H
    ; save clamped position
MB_CONS_GEO_SAVE:
    LD    (MB_SCRN_GEO_POS), HL                     ;=0x1801
    JR    NZ, MB_CONS_GEO_SAVEROW
    SUB   B
    LD    H, A
    ; save available row count
MB_CONS_GEO_SAVEROW:
    LD    (MB_SCRN_ROWLIM), HL                      ;=1
    POP   HL
    POP   BC
    RET

; --------------------------------------------------
MB_CSRLIN:
    RST   CHRGTR
    PUSH  HL
    LD    A, (MB_CURPOS)                            ;= 1
    LD    L, A
    XOR   A
    LD    H, A
    CALL  MB_MAKINT
    POP   HL
    RET

; --------------------------------------------------
; BEEP ON|OFF Statement
; --------------------------------------------------
MB_BEEP:
    JR    NZ, MB_BP_CKON
    LD    A, 0x7
    JP    MB_OUTCHR_A
MB_BP_CKON:
    CP    _ON
    JR    NZ, MB_BP_CKOF
    PUSH  HL
    CALL  MON.BEEP1
    JR    MB_BP_END
MB_BP_CKOF:
    RST   SYNCHR
    DB    'O'
    RST   SYNCHR
    DB    'F'
    RST   SYNCHR
    DB    'F'
    PUSH  HL
    CALL  MON.BEEP0
MB_BP_END:
    POP   HL
    RST   CHRGTR
    JP    NZ, MB_SNERR
    RET

; --------------------------------------------------
; MON Statement, enter to monitor
; --------------------------------------------------
MB_MON:
    RET   NZ
    PUSH  HL
    CALL  MON.MONTOP
    CALL  AL_MENUBAR_DRAW
    POP   HL
    RET

; --------------------------------------------------
; CSAVE  save program to cassette
; --------------------------------------------------
MB_CSAVE:
    CALL  MB_CS_GETFN
    PUSH  HL
    CALL  MB_CS_SVHDR
    LD    HL, (MB_TXTTAB)                           ;=0x733F
    CALL  MB_CS_SVBODY
    LD    BC, MB_LIST_LOOP
MB_CS_DL:
    DEC   BC
    LD    A, B
    OR    C
    JP    NZ, MB_CS_DL
    CALL  MB_CS_FINL
    POP   HL
    RET

; --------------------------------------------------
; CLOAD  save program to cassette
; --------------------------------------------------
MB_CLOAD:
    SUB   _PRINT
    JR    Z, MB_CL_PRNT
    XOR   A
    DB    LD_BC_nn
MB_CL_PRNT:
    CPL
    INC   HL
    CP    1
    PUSH  AF
    LD    A, 0xff
    LD    (MB_CL_FLG1), A
    CALL  MB_CL_GETFN
    ; search tape for program header
MB_CLOAD_SEARCH:
    LD    A, 0xff
    LD    (MB_CL_FLG2), A
    PUSH  DE
    CALL  AL_CASS_STOPMOTOR
    POP   DE
    CALL  MB_CLOAD_FINDHDR
    LD    HL, MB_CAS_FNAMBUF
    CALL  MB_CLOAD_CMPNAME
    JP    NZ, MB_CLOAD_SKIPMSG
    LD    HL, MSG_FOUND                             ;= "Found:"
    CALL  MB_CLOAD_PRINTMSG
    POP   AF
    LD    (MB_DACC + 2), A
    CALL  C, MB_SCRTCH
    LD    A, (MB_DACC + 2)
    CP    1
    LD    (MB_CL_FLG1), A
    PUSH  AF
    CALL  MB_DEPTR
    POP   AF
    LD    HL, (MB_TXTTAB)                           ;=0x733F
    CALL  MB_CLOAD_READPROGRAM
    JR    NZ, MB_CLOAD_VERIFYLEN
    LD    (MB_VARTAB), HL
    ; print OK message
MB_CLOAD_OKMSG:
    LD    HL, MB_OKTXT                              ;= "Ok"
    CALL  MB_STROUT
    CALL  AL_CASS_MOTOR_ON
    LD    HL, (MB_TXTTAB)                           ;=0x733F
    PUSH  HL
    XOR   A
    LD    (MB_CL_FLG1), A
    JP    MB_FINI
    ; verify line length
MB_CLOAD_VERIFYLEN:
    INC   HL
    EX    DE, HL
    LD    HL, (MB_VARTAB)
    RST   DCOMPR
    JP    C, MB_CLOAD_OKMSG
    LD    HL, MSG_BAD                               ;= "Bad"
    CALL  MB_STROUT
    JP    MB_STPRDY
    ; skip non-matching program
MB_CLOAD_SKIPMSG:
    LD    HL, MSG_SKIP                              ;= "Skip:"
    CALL  MB_CLOAD_PRINTMSG
    ; sync wait initialization
MB_CLOAD_SYNC_INIT:
    LD    B, 0xa
    ; sync byte wait loop (10×0xD3)
MB_CLOAD_SYNC_LP:
    CALL  AL_CASS_BITREAD
    OR    A
    JR    NZ, MB_CLOAD_SYNC_INIT
    DJNZ  MB_CLOAD_SYNC_LP
    JP    MB_CLOAD_SEARCH

; --------------------------------------------------
MSG_BAD:
    DB    "Bad\r\n", 0

; --------------------------------------------------
MB_CL_GETFN:
    XOR   A
    LD    (MB_CS_FNAME), A
    DEC   HL
    RST   CHRGTR
    RET   Z

; --------------------------------------------------
; Get filename for CSAVE
; --------------------------------------------------
MB_CS_GETFN:
    CALL  MB_FRMEVL
    PUSH  HL
    CALL  MB_ASC2
    DEC   HL
    DEC   HL
    LD    B, (HL)
    LD    C, 6                                      ; FileName len up to 6 chars
    LD    HL, MB_CS_FNAME
MB_CSFN_CPY:
    LD    A, (DE)
    LD    (HL), A
    INC   HL
    INC   DE
    DEC   C
    JR    Z, MB_CSFN_END
    DJNZ  MB_CSFN_CPY
    LD    B, C
MB_CSFN_FIN:
    LD    (HL), 0
    INC   HL
    DJNZ  MB_CSFN_FIN
MB_CSFN_END:
    POP   HL
    RET

; --------------------------------------------------
; CLOAD: find program header on tape
; --------------------------------------------------
MB_CLOAD_FINDHDR:
    LD    B, 10
    ; single sync byte wait
MB_CLOAD_SYNCWAIT:
    CALL  AL_CASS_BITREAD
    SUB   0xd3                                      ; end of sync header mark?
    JR    NZ, MB_CLOAD_FINDHDR
    DJNZ  MB_CLOAD_SYNCWAIT
    XOR   A
    LD    (MB_CL_FLG2), A
    LD    HL, MB_CAS_FNAMBUF
    LD    B, 6
    ; read 6-char filename from tape
MB_CLOAD_READFNAM:
    CALL  AL_CASS_BITREAD
    LD    (HL), A
    INC   HL
    DJNZ  MB_CLOAD_READFNAM
    RET

; --------------------------------------------------
; Compare tape filename with requested
; --------------------------------------------------
MB_CLOAD_CMPNAME:
    LD    BC, MB_CS_FNAME
    LD    E, 6
    LD    A, (BC)
    OR    A
    RET   Z
    ; name comparison loop
MB_CLOAD_NAMECMP_LP:
    LD    A, (BC)
    CP    (HL)
    INC   HL
    INC   BC
    RET   NZ
    DEC   E
    JR    NZ, MB_CLOAD_NAMECMP_LP
    RET

; --------------------------------------------------
MSG_FOUND:
    DB    "Found:", 0

MSG_SKIP:
    DB    "Skip:", 0

; --------------------------------------------------
; Print status message
; --------------------------------------------------
MB_CLOAD_PRINTMSG:
    PUSH  DE
    PUSH  AF
    CALL  MB_STROUT
    LD    HL, MB_CAS_FNAMBUF
    LD    B, 6
    ; message character loop
MB_CLOAD_MSGLP:
    LD    A, (HL)
    INC   HL
    RST   OUTDO
    DJNZ  MB_CLOAD_MSGLP
    CALL  MB_CRDO
    POP   AF
    POP   DE
    RET
    CALL  MB_CS_GETFILENAME
    PUSH  HL
    PUSH  AF
    LD    A, 0xff
    LD    (MB_CL_FLG1), A
    LD    (MB_CL_FLG2), A
    CALL  AL_CASS_STOPMOTOR

    ; Orphaned code (after RET): end-of-tape wait
MB_CLOAD_ORPH_ENDWAIT:
    LD    B, 6
    ; Orphaned code: end-of-tape loop
MB_CLOAD_ORPH_ENDLP:
    CALL  AL_CASS_BITREAD
    CP    0x9c
    JP    NZ, MB_CLOAD_ORPH_ENDWAIT
    DJNZ  MB_CLOAD_ORPH_ENDLP
    XOR   A
    LD    (MB_CL_FLG2), A
    POP   AF
    POP   HL
    RET
    CALL  MB_CS_GETFILENAME
    PUSH  HL
    PUSH  AF
    CALL  AL_CASS_IDLEPREP
    LD    B, 6
    LD    A, 0x9c
    ; Orphaned code: CSAVE tail write
MB_CSAVE_ORPH_TAILWR:
    CALL  AL_CASS_BITWRITE
    DJNZ  MB_CSAVE_ORPH_TAILWR
    POP   AF
    POP   HL
    RET

; --------------------------------------------------
; Cassette save/load: get filename string
; --------------------------------------------------
MB_CS_GETFILENAME:
    PUSH  DE
    RST   CHRGTR
    CALL  MB_GETIN2
    INC   DE
    LD    A, E
    OR    D
    JP    NZ, MB_FCERR
    POP   DE
    RST   SYNCHR
    DB    ','
    RET

; --------------------------------------------------
MB_CS_SVHDR:
    CALL  AL_CASS_IDLEPREP
    LD    A, 0xd3
    LD    B, 0xa
MB_CSHDR_SYNCWRITE:
    CALL  AL_CASS_BITWRITE
    DJNZ  MB_CSHDR_SYNCWRITE
    LD    B, 0x6
    LD    HL, MB_CS_FNAME
MB_CSHDR_WRITEFNAM:
    LD    A, (HL)
    INC   HL
    CALL  AL_CASS_BITWRITE
    DJNZ  MB_CSHDR_WRITEFNAM
    LD    BC, 40000
MB_CSHDR_GAPDELAY:
    DEC   BC
    LD    A, B
    OR    C
    JP    NZ, MB_CSHDR_GAPDELAY
    RET

; --------------------------------------------------
MB_CS_SVBODY:
    PUSH  HL
    CALL  MB_DEPTR
    POP   HL
    EX    DE, HL
    LD    HL, (MB_VARTAB)
MB_CSBODY_LNLOOP:
    LD    A, (DE)
    INC   DE
    CALL  AL_CASS_BITWRITE
    RST   DCOMPR
    JR    NZ, MB_CSBODY_LNLOOP
    LD    L, 0x9
MB_CSBODY_TAILWR:
    CALL  AL_CASS_BITWRITE
    DEC   L
    JR    NZ, MB_CSBODY_TAILWR
    RET

; --------------------------------------------------
; Read program lines from tape
; --------------------------------------------------
MB_CLOAD_READPROGRAM:
    LD    A, 0xff
    LD    (MB_CL_FLG2), A
    SBC   A, A
    CPL
    LD    D, A
    ; line setup (length prefix)
MB_CLOAD_LNSETUP:
    LD    B, 10
    ; character read loop per line
MB_CLOAD_CHRLP:
    CALL  AL_CASS_BITREAD
    LD    E, A
    CALL  MB_REASON
    LD    A, E
    SUB   (HL)
    AND   D
    RET   NZ
    LD    (HL), E
    LD    A, (HL)
    OR    A
    INC   HL
    JR    NZ, MB_CLOAD_LNSETUP
    CALL  AL_CASS_RESTART
    DJNZ  MB_CLOAD_CHRLP
    LD    BC, -6
    ADD   HL, BC
    XOR   A
    RET
