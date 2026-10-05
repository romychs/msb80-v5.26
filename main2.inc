; ==================================================
; Microsoft Basic-80 v5.26, for CP/M OS, for Triumph
; Adler Alphatronic PC.
;
; Main code implementation 2
; Disassembled by Romychs @ 2026-09-28
; ==================================================

; --------------------------------------------------
; Block transfer routine,  a check is made to make
; sure a reasonable amount of space remains between
; the top of the stack and the highest location
; transferred into
; Inp:
;   DE -> low address to be transferred
;   BC -> high address to be transferred
;   HL -> destination of high address
; Out:
;   HL=DE=low BC=location low was moved into
; --------------------------------------------------
MB_BLTU:
    CALL  MB_REASON
; --------------------------------------------------
MB_BLTUC:
    PUSH  BC
    EX    (SP), HL
    POP   BC
MB_BLTLOP:
    RST   DCOMPR
    LD    A, (HL)
    LD    (BC), A
    RET   Z
    DEC   BC
    DEC   HL
    JR    MB_BLTLOP

; --------------------------------------------------
; Make shure enough space on stack anailable
;  Inp: C - number of words necessary
; --------------------------------------------------
MB_GETSTK:
    PUSH  HL
    LD    HL, (MB_MEMSIZ)
    LD    B, 0
    ADD   HL, BC
    ADD   HL, BC
    LD    A, CONS1                                  ; 256-(2*NUMLEV)
    SUB   L
    LD    L, A
    LD    A, 0xff
    SBC   A, H
    LD    H, A
    JR    C, MB_OMERR
    ADD   HL, SP
    POP   HL
    RET   C

; --------------------------------------------------
; Out of memory error
; --------------------------------------------------
MB_OMERR:
    CALL  MB_LINKER
    LD    HL, (MB_TOPMEM)                           ;=0x73A2
    DEC   HL
    DEC   HL
    LD    (MB_SAVSTK), HL
MB_OMERRR:
    LD    DE, ERROM
    JP    MB_ERROR

; --------------------------------------------------
; Check destination to make sure the stack won't
; be overrun
; --------------------------------------------------
MB_REASON:
    CALL  MB_REALLY
    RET   NC                                        ; Enough?
    LD    A, (MB_CHNFLG)
    OR    A
    JR    NZ, MB_OMERR
    PUSH  BC
    PUSH  DE
    PUSH  HL
    CALL  MB_GARBA2
    POP   HL
    POP   DE
    POP   BC
    CALL  MB_REALLY                                 ; Check again after GC
    RET   NC
    JR    MB_OMERR                                  ; OOM Error if still not enough

; --------------------------------------------------
; Enough space between string and stack?
;  Out: CF Set - Not enough
; --------------------------------------------------
MB_REALLY:
    PUSH  DE
    EX    DE, HL
    LD    HL, (MB_FRETOP)
    RST   DCOMPR
    EX    DE, HL
    POP   DE
    RET

; --------------------------------------------------
; NEW Command
; --------------------------------------------------
MB_NEWS:
    RET   NZ

; --------------------------------------------------
; Clear BASIC program
; --------------------------------------------------
MB_SCRTCH:
    LD    HL, (MB_TXTTAB)  ;=0x733F
    CALL  MB_TOFF
    LD    (MB_AUTFLG), A
    LD    (MB_PTRFLG), A
    LD    (HL), A
    INC   HL
    LD    (HL), A
    INC   HL
    LD    (MB_VARTAB), HL

; --------------------------------------------------
; Initialize interpreter,   basic pointer at start
; of program
; --------------------------------------------------
MB_RUNC:
    LD    HL, (MB_TXTTAB)  ;=0x733F
    DEC   HL

; --------------------------------------------------
; Initialize interpreter,
; Init variable and array space
; --------------------------------------------------
MB_CLEARC1:
    LD    (MB_TEMP), HL
    LD    A, (MB_MRGFLG)
    OR    A
    JR    NZ, MB_LEVDTB
    XOR   A
    LD    (MB_OPTFLAG_1), A
    LD    (MB_OPTVAL), A
    ; Initialize default VALTYPE table
    LD    B, 26
    LD    HL, MB_DEFTBL
MB_LOPDFT:
    LD    (HL), TDBL
    INC   HL
    DJNZ  MB_LOPDFT
MB_LEVDTB:
    CALL  MB_INI_RNDX
    XOR   A
    LD    (MB_ONEFLG), A
    LD    L, A
    LD    H, A
    LD    (MB_ONELIN), HL
    LD    (MB_OLDTXT), HL
    LD    HL, (MB_MEMSIZ)
    LD    A, (MB_CHNFLG)
    OR    A
    JR    NZ, MB_GODFRE                             ; FRETOP IS GOOD,  LEAVE IT ALONE
    LD    (MB_FRETOP), HL

MB_GODFRE:
    XOR   A
    CALL  MB_RESTORE
    LD    HL, (MB_VARTAB)
    LD    (MB_ARYTAB), HL
    LD    (MB_STREND), HL
    LD    A, (MB_MRGFLG)
    OR    A
    CALL  Z, MB_CLSALL
    LD    A, (MB_NLONLY)
    AND   1
    JR    NZ, MB_STKINI
    LD    (MB_NLONLY), A

; --------------------------------------------------
MB_STKINI:
    POP   BC
    LD    HL, (MB_TOPMEM)  ;=0x73A2
    DEC   HL
    DEC   HL
    LD    (MB_SAVSTK), HL
    INC   HL
    INC   HL

; --------------------------------------------------
MB_STKERR:
    LD    SP, HL
    LD    HL, MB_TEMPST
    LD    (MB_TEMPPT), HL
    CALL  MB_CLROVC
    CALL  MB_FINLPT
    CALL  MB_FINPRT
    XOR   A
    LD    H, A
    LD    L, A
    LD    (MB_PRMLEN), HL
    LD    (MB_NOFUNS), A
    LD    (MB_PRMLN2), HL
    LD    (MB_FUNACT), HL
    LD    (MB_PRMSTK), HL
    LD    (MB_SUBFLG), A
    PUSH  HL
    PUSH  BC
MB_GET_TEMP:
    LD    HL, (MB_TEMP)
    RET

; --------------------------------------------------
; RESTORE <lineno> Statement
; Re read DATA from specified line by next READ stmt
; --------------------------------------------------
MB_RESTORE:
    EX    DE, HL
    LD    HL, (MB_TXTTAB)                           ;=0x733F
    JR    Z, MB_BGNRST                              ; Restore to begin of program
    EX    DE, HL
    CALL  MB_LINGET                                 ; Get line number
    PUSH  HL
    CALL  MB_FNDLIN                                 ; find line
    LD    H, B                                      ; set HL = linenumber
    LD    L, C
    POP   DE
    JP    NC, MB_USERR
MB_BGNRST:
    DEC   HL
MB_RESFIN:
    LD    (MB_DATPTR), HL
    EX    DE, HL
    RET

; --------------------------------------------------
; STOP Statement
; --------------------------------------------------
MB_STOP:
    RET   NZ

MB_STOPRG:
    INC   A
    JR    MB_CONSTP

; --------------------------------------------------
; END Statement
; --------------------------------------------------
MB_ENDS:
    RET   NZ
    XOR   A
    LD    (MB_ONEFLG), A
    PUSH  AF
    CALL  Z, MB_CLSALL
    POP   AF
MB_CONSTP:
    LD    (MB_SAVTXT), HL
    LD    HL, MB_TEMPST
    LD    (MB_TEMPPT), HL
    DB    LD_HL_nn
MB_STPEND:
    OR    0xff
    POP   BC
MB_ENDCON:
    LD    HL, (MB_CURLIN)                           ;=0xFFFE
    PUSH  HL
    PUSH  AF
    LD    A, L
    AND   H
    INC   A
    JR    Z, MB_DIRIS
    LD    (MB_OLDLIN), HL
    LD    HL, (MB_SAVTXT)
    LD    (MB_OLDTXT), HL
MB_DIRIS:
    CALL  MB_FINLPT
    CALL  MB_CRDONZ
    POP   AF
    LD    HL, MB_BRKTXT                             ;= "Break"
    JP    NZ, MB_ERRFIN
    JP    MB_STPRDY
    LD    A, CTRL_O

; --------------------------------------------------
MB_NTCTCT:
    PUSH  AF
    LD    A, '^'
    RST   OUTDO
    POP   AF
    ADD   A, 0x40
    RST   OUTDO
    JP    MB_CRDO

; --------------------------------------------------
; CONT Statement
; --------------------------------------------------
MB_CONT:
    LD    HL, (MB_OLDTXT)
    LD    A, H
    OR    L
    LD    DE, 0x11
    JP    Z, MB_ERROR
    LD    DE, (MB_OLDLIN)
    LD    (MB_CURLIN), DE                           ;=0xFFFE
    RET

; --------------------------------------------------
; TRON Statement
; --------------------------------------------------
MB_TON:
    DB    LD_A_n

; --------------------------------------------------
; TROFF Statement
; --------------------------------------------------
MB_TOFF:
    XOR   A
    LD    (MB_TRCFLG), A
    RET

; --------------------------------------------------
; SWAP Statement
; SWAP <var1>,  <var2>
; --------------------------------------------------
MB_SWAP:
    CALL  MB_PTRGET
    PUSH  DE
    PUSH  HL
    LD    HL, MB_SWPTMP
    CALL  MB_VMOVE
    LD    HL, (MB_ARYTAB)
    EX    (SP), HL
    RST   GETYPTR
    PUSH  AF
    RST   SYNCHR
    DB    ','
    CALL  MB_PTRGET
    POP   AF
    LD    B, A
    RST   GETYPTR
    CP    B
    JP    NZ, MB_TMERR
    EX    (SP), HL
    EX    DE, HL
    PUSH  HL
    LD    HL, (MB_ARYTAB)
    RST   DCOMPR
    JR    NZ, MB_FCERR1
    POP   DE
    POP   HL
    EX    (SP), HL
    PUSH  DE
    CALL  MB_VMOVE
    POP   HL
    LD    DE, MB_SWPTMP
    CALL  MB_VMOVE
    POP   HL
    RET
MB_FCERR1:
    JP    MB_FCERR

; --------------------------------------------------
; ERASE array_var
; --------------------------------------------------
MB_ERASE:
    LD    A, 1
    LD    (MB_SUBFLG), A
    CALL  MB_PTRGET
    JR    NZ, MB_FCERR1
    PUSH  HL
    LD    (MB_SUBFLG), A
    LD    H, B
    LD    L, C
    DEC   BC
    DEC   BC
    DEC   BC
MB_LPBKNM:
    LD    A, (BC)
    DEC   BC
    OR    A
    JP    M, MB_LPBKNM
    DEC   BC
    DEC   BC
    ADD   HL, DE
    EX    DE, HL
    LD    HL, (MB_STREND)
MB_ERSLOP:
    RST   DCOMPR
    LD    A, (DE)
    LD    (BC), A
    INC   DE
    INC   BC
    JR    NZ, MB_ERSLOP
    DEC   BC
    LD    H, B
    LD    L, C
    LD    (MB_STREND), HL
    POP   HL
    LD    A, (HL)
    CP    ','
    RET   NZ
    RST   CHRGTR
    JR    MB_ERASE
MB_POPAHT:
    POP   AF
    POP   HL
    RET

; --------------------------------------------------
; Current symbol is uppercase letter?
;  Inp: HL -> symbol
;  Out: CF set if letter
; --------------------------------------------------
MB_ISLET:
    LD    A, (HL)

; --------------------------------------------------
; Current symbol is uppercase letter?
;  Inp: A = symbol
; --------------------------------------------------
MB_ISLET2:
    CP    'A'
    RET   C
    CP    'Z' + 1
    CCF
    RET
MB_CLEARC:
    JP    MB_CLEARC1

; --------------------------------------------------
; CLEAR Statement
; --------------------------------------------------
MB_CLEAR:
    JR    Z, MB_CLEARC
    CP    ','
    JR    Z, MB_CSKPCM
    CALL  MB_INTIDX2
    DEC   HL
    RST   CHRGTR
    JR    Z, MB_CLEARC
MB_CSKPCM:
    RST   SYNCHR
    DB    ','
    JR    Z, MB_CLEARC
    LD    DE, (MB_TOPMEM)                           ;=0x73A2
    CP    ','
    JR    Z, MB_CLEARS
    CALL  MB_INTID2
MB_CLEARS:
    DEC   HL
    RST   CHRGTR
    PUSH  DE
    JR    Z, MB_CDFSTK
    RST   SYNCHR
    DB    ','
    JR    Z, MB_CDFSTK
    CALL  MB_INTID2
    DEC   HL
    RST   CHRGTR
    JP    NZ, MB_SNERR

; --------------------------------------------------
MB_CLEART:
    EX    (SP), HL
    PUSH  HL
    LD    HL, (2*NUMLEV)+20                         ; CHECK STACK SIZE IS REASONABLE
    RST   DCOMPR
    JR    NC, MB_OMERR1
    POP   HL
    CALL  MB_SUBDE
    JR    C, MB_OMERR1
    PUSH  HL
    LD    HL, (MB_VARTAB)
    LD    BC, 20                                    ; LEAVE BREATHING ROOM
    ADD   HL, BC
    RST   DCOMPR
    JR    NC, MB_OMERR1
    EX    DE, HL
    LD    (HL), 0
    DEC   HL
    LD    (MB_MEMSIZ), HL
    POP   HL
    LD    (MB_TOPMEM), HL                           ; =0x73A2
    POP   HL
    JP    MB_CLEARC

; --------------------------------------------------
MB_INTID2:
    CALL  MB_PGETADR
    LD    A, D
    OR    E
    JP    Z, MB_FCERR
    RET
MB_OMERR1:
    JP    MB_OMERR
MB_CDFSTK:
    PUSH  HL
    LD    HL, (MB_TOPMEM)                            ; =0x73A2
    EX    DE, HL
    LD    HL, (MB_MEMSIZ)
    INC   HL
    LD    A, E
    SUB   L
    LD    E, A
    LD    A, D
    SBC   A, H
    LD    D, A
    POP   HL
    JR    MB_CLEART
MB_SUBDE:
    LD    A, L
    SUB   E
    LD    E, A
    LD    A, H
    SBC   A, D
    LD    D, A
    RET
MB_NEXT:
    PUSH  AF
    DB    OR_n
MB_NEXTS:
    XOR   A
    LD    (MB_NXTFLG), A
    POP   AF
    LD    DE, 0
MB_NEXTC:
    LD    (MB_NXTTXT), HL
    CALL  NZ, MB_PTRGET
    LD    (MB_TEMP), HL
    CALL  MB_FNDFOR
    JP    NZ, MB_NFERR
    LD    SP, HL
    PUSH  DE
    LD    E, (HL)
    INC   HL
    LD    D, (HL)
    INC   HL
    PUSH  HL
    LD    HL, (MB_NXTTXT)                           ; Pointer to text,  start this next
    RST   DCOMPR
    JP    NZ, MB_NFERR                              ; Not match,  error
    POP   HL
    POP   DE
    PUSH  DE
    LD    A, (HL)
    PUSH  AF
    INC   HL
    PUSH  DE
    LD    A, (HL)
    INC   HL
    OR    A
    JP    M, MB_CMP_INT
    DEC   A
    JR    NZ, MB_NXTGNG
    LD    BC, 8
    ADD   HL, BC
MB_NXTGNG:
    ADD   A, TSGN
    LD    (MB_VALTYP), A
    CALL  MB_VMOVFM
    EX    DE, HL
    EX    (SP), HL
    PUSH  HL
    LD    A, (MB_NXTFLG)
    OR    A
    JR    NZ, MB_INXTDO
    LD    HL, MB_FVALSV
    CALL  MB_VMOVFM
    XOR   A
MB_INXTDO:
    PUSH  AF
    RST   GETYPTR
    JR    NC, MB_CMP_DBL
    POP   AF
    JP    Z, MB_NXT_FLOATSTEP
    CALL  MB_LDSGL
    CALL  MB_FADD
    ; float step add + compare
MB_NXT_FLOATSTEP:
    POP   HL
    CALL  MB_MOVSM
    POP   HL
    CALL  MB_MOVRM
    PUSH  HL
    CALL  MB_FCOMP
    JR    MB_CMP_CLEANUP
MB_CMP_INT:
    LD    BC, 12
    ADD   HL, BC
    LD    C, (HL)
    INC   HL
    LD    B, (HL)
    INC   HL
    EX    (SP), HL
    LD    E, (HL)
    INC   HL
    LD    D, (HL)
    PUSH  HL
    LD    L, C
    LD    H, B
    LD    A, (MB_NXTFLG)
    OR    A
    JR    NZ, MB_NXT_INCCHK
    ; value is float type
MB_NXT_VALFLOAT:
    LD    HL, (MB_FVALSV)
    JR    MB_NXT_STORECMP
    ; increment check (signed)
MB_NXT_INCCHK:
    CALL  MB_IADD
    LD    A, (MB_VALTYP)
    CP    TINT
    JP    NZ, MB_OVERR
    ; store comparison result
MB_NXT_STORECMP:
    EX    DE, HL
    POP   HL
    LD    (HL), D
    DEC   HL
    LD    (HL), E
    POP   HL
    PUSH  DE
    LD    E, (HL)
    INC   HL
    LD    D, (HL)
    INC   HL
    EX    (SP), HL
    CALL  MB_ICOMP
MB_CMP_CLEANUP:
    POP   HL
    POP   BC
    SUB   B
    CALL  MB_MOVRM
    JR    Z, MB_CMP_NXTSTT
    EX    DE, HL
    LD    (MB_CURLIN), HL
    LD    L, C
    LD    H, B
    JP    MB_NXTCON
MB_CMP_DBL:
    POP   AF
    CALL  NZ, MB_DRADD
    POP   HL
    CALL  MB_MOVMF
    POP   HL
    CALL  MB_MOVE
    PUSH  DE
    CALL  MB_XDCOMP
    JR    MB_CMP_CLEANUP
MB_CMP_NXTSTT:
    LD    SP, HL
    LD    (MB_SAVSTK), HL
    EX    DE, HL
    LD    HL, (MB_TEMP)
    LD    A, (HL)
    CP    ','
    JP    NZ, MB_NEWSTT
    LD    (MB_NXTFLG), A
    RST   CHRGTR
    CALL  MB_NEXTC
MB_ISFLIO:
    PUSH  HL
    LD    HL, (MB_PTRFIL)
    LD    A, H
    OR    L
;MB_CTLTBL_BASE:
    POP   HL
    RET

; --------------------------------------------------
; 9-entry control char table
; --------------------------------------------------
MB_CTLTBL_BASE:
    DB    ASCII_BELL
    DW    MB_CRTOUT_C
    DB    ASCII_LF
    DW    AL_EDT_LF_ADVROW
    DB    KEY_HOME
    DW    AL_EDT_VT_GOTOHOME
    DB    CTRL_L
    DW    AL_EDT_FF_HDLR
    DB    ASCII_CR
    DW    AL_EDT_CR_HOME
    DB    KEY_RT
    DW    AL_EDT_CTRLC_HDLR
    DB    KEY_LT
    DW    AL_EDT_CTRLD_HDLR
    DB    KEY_UP
    DW    AL_EDT_CTRLE_HDLR
    DB    KEY_DN
    DW    AL_EDT_CTLRF_HDLR

; --------------------------------------------------
; Wrap-pos check; reposition or delete char
; --------------------------------------------------
AL_EDT_CTRLC_HDLR:
    CALL  AL_EDT_WRAPPOS_CHK
    JR    NZ, AL_EDT_CURMOVE
    JR    AL_EDT_CTRLC_DELCH

; --------------------------------------------------
; move to EOL, reposition cursor
; --------------------------------------------------
AL_EDT_CTRLD_HDLR:
    CALL  AL_EDT_EOL_MOVE
    JR    AL_EDT_CURMOVE

; --------------------------------------------------
; advance row, reposition cursor
; --------------------------------------------------
AL_EDT_CTLRF_HDLR:
    CALL  AL_EDT_ROWADV_CHK
    JR    AL_EDT_CURMOVE

; --------------------------------------------------
; row adjust, reposition cursor
; --------------------------------------------------
AL_EDT_CTRLE_HDLR:
    CALL  AL_EDT_ROWADJ
    JR    AL_EDT_CURMOVE

; --------------------------------------------------
; home column (col 1)
; --------------------------------------------------
AL_EDT_CR_HOME:
    LD    HL, (MB_CURPOS)    ;= 1
    LD    H, 1
    JP    AL_EDT_SETCURS

; --------------------------------------------------
; advance row, set cursor
; --------------------------------------------------
AL_EDT_LF_ADVROW:
    LD    HL, (MB_CURPOS)    ;= 1
    CALL  AL_EDT_ROWST_SET_USED
    CALL  AL_EDT_ROWADV_CHK
    JP    NZ, AL_EDT_SETCURS

; --------------------------------------------------
; delete-char path (flag-gated)
; --------------------------------------------------
AL_EDT_CTRLC_DELCH:
    LD    (MB_CURPOS), HL    ;= 1
    LD    A, (MB_CONSOLE_ROWFLG)
    AND   A
    JR    NZ, AL_EDT_CTRLC_ALT
    CALL  AL_EDT_DELCHAR

; --------------------------------------------------
; Reposition cursor to CURPOS
; --------------------------------------------------
AL_EDT_CURMOVE:
    PUSH  AF
    LD    HL, (MB_CURPOS)    ;= 1
    CALL  MB_CUR_MOVE
    POP   AF
    RET
    ; wrap check + fill spaces to EOL
AL_EDT_FF_HDLR:
    CALL  AL_EDT_MAXROW
    CP    L
    JR    NC, AL_EDT_FF_ROWCLAMP
    INC   A
    LD    L, A
    CALL  AL_EDT_MAXROW_FIX
    ; space output loop
AL_EDT_FF_SPACELP:
    SUB   L
    INC   A
    LD    B, A
    DEC   L
    CALL  NZ, AL_EDT_ROWST_SET_USED
    INC   L
    PUSH  HL
    ; per-space iteration
AL_EDT_FF_SPACE_ITER:
    PUSH  HL
    CALL  AL_EDT_HOME_COL
    POP   HL
    INC   L
    DJNZ  AL_EDT_FF_SPACE_ITER
    POP   HL
    JR    AL_EDT_HOME_POS
    ; clamp row to available rows
AL_EDT_FF_ROWCLAMP:
    PUSH  AF
    LD    A, (MB_SCRN_ROWLIM)     ;=1
    CP    L
    JR    C, AL_EDT_FF_NEWROW
    JR    Z, AL_EDT_FF_NEWROW
    LD    A, 1
    ; set clamped/new row, continue
AL_EDT_FF_NEWROW:
    LD    L, A
    POP   AF
    JR    AL_EDT_FF_SPACELP
    ; alternate path when flag set (artifact call)
AL_EDT_CTRLC_ALT:
    CALL  AL_EDT_ROWST_SET_USED

; --------------------------------------------------
; Goto home position
; --------------------------------------------------
AL_EDT_VT_GOTOHOME:
    LD    A, (MB_SCRN_ROWLIM)     ;=1
    LD    L, A
    ; set row/col, store cursor
AL_EDT_HOME_POS:
    LD    H, 1
    JP    AL_EDT_SETCURS

; --------------------------------------------------
MB_INLIN:
    CALL  MB_ISFLIO
    JP    NZ, MB_INLIN_FROMFILE
    LD    A, (MB_AUTFLG)
    AND   A
    JR    NZ, MB_INLIN_INIT
    LD    L, 0
    JR    MB_INLIN_INIT
MB_GET_NF:
    LD    A, '?'
    RST   OUTDO
    LD    A, ASCII_SP
    RST   OUTDO

; --------------------------------------------------
; In-line input: initialize buffer/position
; --------------------------------------------------
MB_INLIN_INIT:
    XOR   A
    LD    (MB_NULCNT), A
MB_SINLIN:
    LD    HL, (MB_CURPOS)                           ;= 1
    DEC   L
    CALL  NZ, AL_EDT_ROWST_SET_USED
    INC   L
    LD    (MB_EDT_COLROW), HL
    ; main key loop
MB_INLIN_KEYLOOP:
    CALL  MB_DIRECT_GETKEY
    PUSH  AF
    LD    HL, MB_CNTOFL
    INC   (HL)
    DEC   (HL)
    JR    NZ, MB_INLIN_CHARPROC
    LD    HL, MB_INLIN_SPECIAL_TBL      ;=0x7
    LD    BC, 0xf
    CPIR
    JR    NZ, MB_INLIN_CHARPROC
    XOR   A
    LD    (MB_EDT_OVRFLG), A
    ; process character
MB_INLIN_CHARPROC:
    POP   AF
    LD    HL, MB_CTLTBL_JUMP
    ; first-character check
MB_INLIN_FIRSTCHK:
    LD    C, ASCII_CR
    CALL  MB_INLIN_MODECHK
    PUSH  AF
    CALL  NZ, MB_INLIN_ECHO
    POP   AF
    JR    NC, MB_INLIN_KEYLOOP
    LD    HL, MB_BUFMIN    ;=0x2C
    RET   Z
    CCF
    ; exit to editor
MB_INLIN_EXIT:
    RET

; --------------------------------------------------
; Echo printable character (shift if insert mode)
; --------------------------------------------------
MB_INLIN_ECHO:
    CP    0x20
    JR    C, MB_INLIN_ECHO_SHIFTPREP
    PUSH  AF
    CALL  AL_EDT_CLRCELL_SAFE
    POP   AF
MB_INLIN_ECHO_SHIFTPREP:
    PUSH  AF
    LD    A, (MB_CNTOFL)
    AND   A
    JR    Z, MB_INLIN_ECHO_XFORMED
    POP   AF
    AND   A
    JP    M, MB_INLIN_ECHO_OUTDO
    ADD   A, 0x40
    PUSH  AF
MB_INLIN_ECHO_XFORMED:
    POP   AF
MB_INLIN_ECHO_OUTDO:
    RST   OUTDO
    RET

; --------------------------------------------------
; Mode/first-char check
; --------------------------------------------------
MB_INLIN_MODECHK:
    PUSH  AF
    LD    A, (MB_CNTOFL)
    AND   A
    JR    Z, MB_INLIN_NOSPECIAL
    POP   AF
    AND   A
    LD    C, 0
    DEC   C
    RET
MB_INLIN_NOSPECIAL:
    POP   AF

; --------------------------------------------------
; Control char table scan (dispatch by key code)
; --------------------------------------------------
MB_CTLTBL_SCAN:
    INC   HL
    INC   HL
    AND   A
    DEC   C
    RET   M
    CP    (HL)
    INC   HL
    JR    NZ, MB_CTLTBL_SCAN
    LD    C, (HL)
    INC   HL
    LD    B, (HL)
    LD    HL, (MB_CURPOS)    ;= 1
    CALL  MB_CTLTBL_JUMP
    XOR   A
    RET

; --------------------------------------------------
; Table jump target
; --------------------------------------------------
MB_CTLTBL_JUMP:
    PUSH  BC
    RET

; --------------------------------------------------
    DB    ASCII_BS
    DW    AL_EDT_BS_HDLR
    DB    KEY_INS
    DW    AL_EDT_INS_HDLR
    DB    ASCII_LF
    DW    AL_EDT_LF_HDLR
    DB    ASCII_ESC
    DW    MB_INLIN_EXIT
    DB    CTRL_B
    DW    AL_EDT_CTRLB_HDLR
    DB    CTRL_F
    DW    AL_EDT_CTRLF_HDLR
    DB    CTRL_N
    DW    AL_EDT_CTRLN_HDLR
    DB    CTRL_E
    DW    AL_EDT_CTRLE2_HDLR
    DB    CTRL_C
    DW    AL_EDT_CTRLC_RESET
    DB    ASCII_CR
    DW    MB_INLIN_CR_HDLR
    DB    CTRL_U
    DW    AL_EDT_CTRLU_HDLR
    DB    ASCII_TAB
    DW    AL_EDT_TAB_HDLR
    DB    ASCII_DEL
    DW    AL_EDT_DEL_HDLR

; --------------------------------------------------
; Special-key set for CPIR detection
; --------------------------------------------------
MB_INLIN_SPECIAL_TBL:
    DB    0x7
    DB    0x0B, 0x0C, 0x1C, 0x1D, 0x1E, 0x1F, 0x02, 0x03
    DB    0x05, 0x06, 0x08, 0x0D, 0x0E, 0x15, 0x3A, 0xCB
    DB    0x0B, 0xB7, 0xCA, 0x10, 0x55, 0xAF, 0x77, 0x21
    DB    0x41, 0x09, 0xC9

; --------------------------------------------------
MB_SCNSEM:
    PUSH  AF
    LD    A, 0
    LD    (MB_TEMPA), A
    POP   AF
    CP    ';'
    RET   NZ
    LD    (MB_TEMPA), A
    JP    MB_CHRGTR
MB_INLIN_CR_HDLR:
    CALL  AL_EDT_FIND_EOL
    LD    A, (MB_AUTFLG)
    AND   A
    JR    Z, MB_INLIN_CR_INIT
    LD    H, 1
MB_INLIN_CR_INIT:
    LD    DE, MB_BUF
    LD    B, 0xfe
    DEC   L
MB_INLIN_CR_COLLP:
    INC   L
MB_INLIN_CR_CHARLP:
    PUSH  DE
    PUSH  BC
    CALL  AL_VRAM_READCHAR
    POP   BC
    POP   DE
    CP    0x80
    JR    NC, MB_INLIN_CR_STORE
    CP    0x20
    JR    NC, MB_INLIN_CR_WRCH
    ADD   A, 0x40
MB_INLIN_CR_STORE:
    DEC   B
    JR    Z, MB_INLIN_CR_TRIMSPC
    PUSH  AF
    LD    A, 0xdf
    LD    (DE), A
    INC   DE
    POP   AF
MB_INLIN_CR_WRCH:
    LD    (DE), A
    INC   DE
    DEC   B
    JR    Z, MB_INLIN_CR_TRIMSPC
    INC   H
    LD    A, (MON.CLUMSZ)      ;= 80
    CP    H
    JR    NC, MB_INLIN_CR_CHARLP
    PUSH  DE
    CALL  AL_VRAM_GETCOLST
    POP   DE
    LD    H, 1
    JR    Z, MB_INLIN_CR_COLLP
MB_INLIN_CR_TRIMSPC:
    DEC   DE
    LD    A, (DE)     ;=0x2C
    CP    ASCII_SP
    JR    Z, MB_INLIN_CR_TRIMSPC
    INC   DE
    XOR   A
    LD    (DE), A
    LD    A, ASCII_CR
    AND   A
MB_INLIN_CR_FINISH:
    PUSH  AF
    CALL  AL_EDT_ROWST_SET_USED
    CALL  AL_EDT_SETCURS
    LD    A, ASCII_LF
    RST   OUTDO
    XOR   A
    LD    (MB_EDT_OVRFLG), A
    POP   AF
    SCF
    POP   HL
    RET
AL_EDT_LF_HDLR:
    LD    A, (MB_EDT_OVRFLG)
    OR    A
    JR    NZ, AL_EDT_LF_CLREOL_MODE
    LD    A, ASCII_LF
    PUSH  AF
    RST   OUTDO
    LD    HL, (MB_CURPOS)    ;= 1
    LD    H, 1
    CALL  AL_EDT_SETCURS
    POP   AF
    OR    A
    RET
AL_EDT_LF_CLREOL_MODE:
    PUSH  HL
    LD    A, (MON.CLUMSZ)      ;= 80
    SUB   H
    INC   A
    LD    B, A
AL_EDT_LF_CLRRESTLP:
    PUSH  BC
    CALL  AL_EDT_CLRCELL_SAFE
    POP   BC
    DJNZ  AL_EDT_LF_CLRRESTLP
    POP   HL
    CALL  AL_EDT_ROWST_SET_USED
    JP    MB_INLIN_CR_HDLR
AL_EDT_TAB_HDLR:
    LD    A, H
    DEC   A
    AND   0xf8
    ADD   A, 0x8
    INC   A
    LD    B, A
    LD    A, (MON.CLUMSZ)      ;= 80
    CP    B
    JR    NC, AL_EDT_TAB_OVRCHK
    LD    B, A
AL_EDT_TAB_OVRCHK:
    LD    A, (MB_EDT_OVRFLG)
    OR    A
    LD    A, B
    JR    NZ, AL_EDT_TAB_REMAIN
    CP    H
    JR    Z, AL_EDT_TAB_EXACT
    SUB   H
AL_EDT_TAB_SPACELP:
    PUSH  AF
    LD    A, ASCII_SP
    RST   OUTDO
    POP   AF
    DEC   A
    JR    NZ, AL_EDT_TAB_SPACELP
AL_EDT_TAB_EXACT:
    AND   A
    RET
AL_EDT_TAB_REMAIN:
    SUB   H
    RET   Z
AL_EDT_TAB_CLEARLP:
    PUSH  AF
    CALL  AL_EDT_CLRCELL_SAFE
    LD    A, ASCII_SP
    RST   OUTDO
    POP   AF
    DEC   A
    JR    NZ, AL_EDT_TAB_CLEARLP
    RET
AL_EDT_CTRLC_INC:
    INC   L
AL_EDT_CTRLC_RESET:
    CALL  AL_VRAM_GETCOLST
    JR    Z, AL_EDT_CTRLC_INC
    XOR   A
    LD    (MB_BUF), A
    LD    H, 1
    JP    MB_INLIN_CR_FINISH

; --------------------------------------------------
AL_EDT_INS_HDLR:
    LD    HL, MB_EDT_OVRFLG
    LD    A, (HL)
    XOR   0xff
    LD    (HL), A
    RET

; --------------------------------------------------
; Clear cell with safety check (CTV off/on)
; --------------------------------------------------
AL_EDT_CLRCELL_SAFE:
    PUSH  AF
    CALL  MON.CTVOFF
    POP   AF
    CALL  AL_EDT_CLRFROMCURS
    JP    MON.CTVON

; --------------------------------------------------
; Clear from cursor to screen end
; --------------------------------------------------
AL_EDT_CLRFROMCURS:
    PUSH  AF
    LD    A, (MB_EDT_OVRFLG)
    AND   A
    JR    NZ, AL_EDT_CLR_PREP
    ; nothing to clear
AL_EDT_CLR_NONE:
    POP   AF
    RET
    ; prepare clear range
AL_EDT_CLR_PREP:
    LD    A, (MB_CNTOFL)
    AND   A
    JR    NZ, AL_EDT_CLR_NONE
    POP   AF
    LD    HL, (MB_CURPOS)    ;= 1
    LD    A, (MON.CPATR)       ;= 7
    LD    B, A
    LD    C, 0x20
    ; per-row clear loop
AL_EDT_CLR_ROWLP:
    PUSH  HL
    ; per-column clear loop
AL_EDT_CLR_COLLP:
    PUSH  BC
    CALL  AL_VRAM_GETCH_ATTR
    POP   DE
    PUSH  BC
    LD    B, D
    LD    C, E
    CALL  AL_VRAM_PUTCH_ATTR
    POP   BC
    INC   H
    LD    A, (MON.CLUMSZ)      ;= 80
    CP    H
    JR    NC, AL_EDT_CLR_COLLP
    POP   HL
    CALL  AL_VRAM_GETCOLST
    JR    Z, AL_EDT_CLR_NEXTROW
    LD    A, C
    CP    0x20
    JR    NZ, AL_EDT_CLR_WRAPEDGE
    LD    A, (MON.CLUMSZ)      ;= 80
    CP    H
    JP    NZ, AL_EDT_CURMOVE

    ; wrap edge handling
AL_EDT_CLR_WRAPEDGE:
    PUSH  AF
    PUSH  BC
    PUSH  HL
    CALL  AL_EDT_ROWST_CLR
    CALL  AL_EDT_WRAPSHIFT
    POP   HL
    POP   BC
    JR    NC, AL_EDT_CLR_ROWEND_CHK
    DEC   L
    ; row end check
AL_EDT_CLR_ROWEND_CHK:
    POP   AF
    JR    Z, AL_EDT_CLR_REPOS
    ; advance to next row
AL_EDT_CLR_NEXTROW:
    INC   L
    LD    H, 1
    JR    AL_EDT_CLR_ROWLP
    ; reposition cursor
AL_EDT_CLR_REPOS:
    CALL  AL_EDT_MAXROW
    LD    L, A
    CALL  AL_EDT_ROWST_SET_USED
    CALL  AL_EDT_MAXROW_FIX
    LD    L, A
    CALL  AL_EDT_ROWST_SET_USED
    JP    AL_EDT_CURMOVE

; --------------------------------------------------
; Delete character at cursor (shift line left)
; --------------------------------------------------
AL_EDT_DELCHAR:
    PUSH  HL
    LD    HL, MB_EDT_COLROW
    DEC   (HL)
    POP   HL
    CALL  AL_EDT_ROWCHK
    CP    L
    JP    Z, AL_EDT_HOME_COL
    PUSH  HL
    LD    H, A
    PUSH  HL
    CALL  AL_VRAM_GETCOLST
    PUSH  DE
    LD    L, H
    CALL  AL_VRAM_GETCOLST
    POP   HL
    OR    A
    SBC   HL, DE
    LD    C, L
    LD    B, H
    LD    A, L
    PUSH  AF
    LD    H, D
    LD    L, E
    INC   HL
    LDIR
    POP   AF
    CALL  AL_EDT_CELLVADR
    POP   HL
    LD    L, H
    LD    H, 1
    PUSH  HL
    CALL  MB_CALC_VADR
    EX    (SP), HL
    INC   L
    CALL  MB_CALC_VADR
    POP   DE
    PUSH  BC
    PUSH  DE
    PUSH  HL
    CALL  MON.CTVOFF
    POP   HL
    POP   DE
    POP   BC
    PUSH  BC
    PUSH  DE
    PUSH  HL
    LDIR
    ; add attr offset
    LD    BC, 0x800
    POP   HL
    ADD   HL, BC
    EX    DE, HL
    POP   HL
    ADD   HL, BC
    EX    DE, HL
    POP   BC
    LDIR
    CALL  MON.CTVON
    POP   HL
    JP    AL_EDT_HOME_COL

; --------------------------------------------------
; Row boundary check for delete
; --------------------------------------------------
AL_EDT_ROWCHK:
    LD    A, (MB_SCRN_ROWS2)                        ;= 24
    LD    B, A
    INC   B
    CP    L
    LD    A, B
    RET   C
    LD    A, (MB_SCRN_ROWLIM)                       ;=1
    RET

; --------------------------------------------------
; Compute VRAM cell offset from (row,col)
; --------------------------------------------------
AL_EDT_CELLVADR:
    PUSH  HL
    LD    B, 0x5
    LD    C, A
    XOR   A
    ; row × line-length loop
AL_EDT_CELLOFS_LP:
    ADD   A, C
    DJNZ  AL_EDT_CELLOFS_LP
    LD    L, A
    LD    H, 0
    ADD   HL, HL
    ADD   HL, HL
    ADD   HL, HL
    LD    A, (MON.CLUMSZ)                           ;= 80
    CP    40
    JR    Z, AL_EDT_CELLOFS_DBL
    ADD   HL, HL
    ; double for 80-col mode
AL_EDT_CELLOFS_DBL:
    LD    C, L
    LD    B, H
    POP   HL
    RET

; --------------------------------------------------
; DEL key handler
; --------------------------------------------------
AL_EDT_DEL_HDLR:
    LD    A, KEY_RT
    RST   OUTDO
    LD    HL, (MB_CURPOS)


; --------------------------------------------------
;  Backspace key handler
; --------------------------------------------------
AL_EDT_BS_HDLR:
    CALL  MON.CTVOFF                                
    DEC   H                                         ; cursor left 1 column
    JR    NZ, AL_BS_SAVECURS                        ; jump if not left bound
    INC   H                                         ; restore column
    PUSH  HL
    DEC   L                                         ; previous row
    JR    Z, AL_BS_KEEPPOS                          ; jump if not top bound
    LD    A, (MON.CLUMSZ)                           ;= 80
    LD    H, A                                      ; set cursor columt to rightmost
    CALL  AL_VRAM_GETCOLST                          ;
    JR    NZ, AL_BS_KEEPPOS
    EX    (SP), HL
    ; keep position registers
AL_BS_KEEPPOS:
    POP   HL
    ; save cursor position
AL_BS_SAVECURS:
    LD    (MB_CURPOS), HL                           ;= 1
    ; scan column leftward
AL_BS_COLSCANLP:
    LD    A, (MON.CLUMSZ)                           ;= 40/80
    CP    H
    JR    Z, AL_BS_TRAILSPC
    INC   H
    ; shift characters right (LDIR)
AL_BS_SHIFTRIGHT:
    CALL  AL_VRAM_GETCH_ATTR
    DEC   H
    CALL  AL_VRAM_PUTCH_ATTR
    INC   H
    INC   H
    LD    A, (MON.CLUMSZ)                           ;= 40/80
    INC   A
    CP    H
    JR    NZ, AL_BS_SHIFTRIGHT
    DEC   H
    ; write trailing space
AL_BS_TRAILSPC:
    LD    C, ASCII_SP
    LD    A, (MON.CPATR)                            ;= 7
    LD    B, A
    CALL  AL_VRAM_PUTCH_ATTR
    CALL  AL_VRAM_GETCOLST
    JR    NZ, AL_BS_REPOS
    PUSH  HL
    INC   L
    LD    H, 1
    CALL  AL_VRAM_GETCH_ATTR
    EX    (SP), HL
    CALL  AL_VRAM_PUTCH_ATTR
    POP   HL
    JR    AL_BS_COLSCANLP

    ; reposition cursor (CTVON + CUR_MOVE)
AL_BS_REPOS:
    CALL  MON.CTVON
    JP    AL_EDT_CURMOVE

; --------------------------------------------------
; (Ctrl-U): move to EOL, clear tail
; --------------------------------------------------
AL_EDT_CTRLU_HDLR:
    CALL  AL_EDT_FIND_EOL
    LD    (MB_CURPOS), HL                           ;= 1
    JR    AL_EDT_CLRTOEOL

; --------------------------------------------------
; (Ctrl-E): clear from cursor to EOL
; --------------------------------------------------
AL_EDT_CTRLE2_HDLR:
    PUSH  HL
    POP   HL
    ; clear-to-EOL with row advance
AL_EDT_CLRTOEOL:
    CALL  AL_VRAM_GETCOLST
    PUSH  AF
    PUSH  HL
    CALL  AL_EDT_CLREOL
    POP   HL
    POP   AF
    JR    NZ, AL_EDT_REPOSRET
    LD    H, 1
    INC   L
    JR    AL_EDT_CLRTOEOL

; --------------------------------------------------
; Reposition and return
; --------------------------------------------------
AL_EDT_REPOSRET:
    CALL  AL_EDT_CURMOVE
    XOR   A
    RET

; --------------------------------------------------
; (Ctrl-N): move to next non-blank char
; --------------------------------------------------
AL_EDT_CTRLN_HDLR:
    LD    HL, (MB_CURPOS)                           ;= 1
    DEC   L
AL_EDT_NXTCHAR_LP:
    INC   L
    CALL  AL_VRAM_GETCOLST
    JR    Z, AL_EDT_NXTCHAR_LP
    LD    A, (MON.CLUMSZ)                           ;= 40/80
    LD    H, A
    INC   H
AL_EDT_NXTCHAR_CHARLP:
    DEC   H
    JR    Z, AL_EDT_NXTCHAR_WRAP
    CALL  AL_VRAM_READCHAR
    CP    0x20
    JR    Z, AL_EDT_NXTCHAR_CHARLP
AL_EDT_NXTCHAR_WRAP:
    CALL  AL_EDT_WRAPPOS_CHK
    JR    AL_EDT_REPOSRET

; --------------------------------------------------
; (Ctrl-F): forward search for non-alpha
; --------------------------------------------------
AL_EDT_CTRLF_HDLR:
    CALL  AL_EDT_SRCH_FWD_CHK
    JR    Z, AL_EDT_REPOSRET
    JR    C, AL_EDT_CTRLF_HDLR
    ; Forward search loop
.SRCH_FWD_LP:
    CALL  AL_EDT_SRCH_FWD_CHK
    JR    Z, AL_EDT_REPOSRET
    JR    NC, .SRCH_FWD_LP
    JR    AL_EDT_REPOSRET

; --------------------------------------------------
; (Ctrl-B): backward search for non-alpha
; --------------------------------------------------
AL_EDT_CTRLB_HDLR:
    CALL  AL_EDT_SRCH_BWD_CHK
    JR    Z, AL_EDT_REPOSRET
    JR    NC, AL_EDT_CTRLB_HDLR
    ; Backward search: wrap check, done
.SRCH_BWD_DON:
    CALL  AL_EDT_SRCH_BWD_CHK
    JR    Z, AL_EDT_REPOSRET
    JR    C, .SRCH_BWD_DON
    CALL  AL_EDT_WRAPPOS_CHK
    JR    AL_EDT_REPOSRET

; --------------------------------------------------
; Forward search: distance-to-EOL + char check
; --------------------------------------------------
AL_EDT_SRCH_FWD_CHK:
    LD    HL, (MB_CURPOS)                           ;= 1
    PUSH  HL
    CALL  AL_EDT_WRAPPOS_CHK
    POP   HL
    CALL  AL_EDT_MAXROW
    LD    E, A
    LD    A, (MON.CLUMSZ)                           ;= 80
    LD    D, A
    RST   DCOMPR
    RET   Z
    CALL  AL_EDT_MAXROW_FIX
    LD    E, A
    JR    AL_EDT_ISALPHA

; --------------------------------------------------
; Backward search: distance-to-BOL + row check
; --------------------------------------------------
AL_EDT_SRCH_BWD_CHK:
    LD    HL, (MB_CURPOS)                           ;= 1
    PUSH  HL
    CALL  AL_EDT_EOL_MOVE
    POP   HL
    LD    DE, 0x0101
    RST   DCOMPR
    RET   Z
    LD    A, (MB_SCRN_ROWLIM)                       ;=1
    LD    E, A
    LD    D, 1
    RST   DCOMPR
    RET   Z
    CALL  AL_EDT_MAXROW
    INC   A
    LD    E, A
    LD    D, 1
    ; is alphanumeric?
AL_EDT_ISALPHA:
    LD    HL, (MB_CURPOS)                           ;= 1
    RST   DCOMPR
    RET   Z
    LD    DE, AL_EDT_ISALPHA_YES
    PUSH  DE
    CALL  AL_VRAM_READCHAR
    CP    '0'
    CCF
    RET   NC
    CP    '9' + 1
    RET   C
    CP    'A'
    CCF
    RET   NC
    CP    'Z' + 1
    RET   C
    CP    'a'
    CCF
    RET   NC
    CP    'z' + 1
AL_EDT_ISALPHA_YES:
    LD    A, 0
    INC   A
    RET

; --------------------------------------------------
; Find end of line from cursor
; --------------------------------------------------
AL_EDT_FIND_EOL:
    DEC   L
    JR    Z, AL_EDT_EOL_POS
    CALL  AL_VRAM_GETCOLST
    JR    Z, AL_EDT_FIND_EOL
    ; position found, compare with line length
AL_EDT_EOL_POS:
    INC   L
    LD    A, (MB_EDT_COLROW)
    CP    L
    LD    H, 1
    RET   NZ
    LD    HL, (MB_EDT_COLROW)
    RET

; --------------------------------------------------
MB_ERREDT:
    LD    (MB_ERRFLG), A
    LD    HL, (MB_ERRLIN)
    LD    A, H
    AND   L
    INC   A
    EX    DE, HL
    RET   Z
    JR    MB_EDIT_SHOWLINE
    LD    HL, MB_BUFMIN    ;=0x2C
    RET   Z
    SCF
    PUSH  AF
    INC   HL
    JP    MB_EDENT

; --------------------------------------------------
MB_EDIT:
    CALL  MB_LINSPC
    JP    NZ, MB_FCERR
    ; find and print line
MB_EDIT_SHOWLINE:
    POP   HL
    LD    (MB_DOT), DE
    CALL  MB_FNDLIN
    JP    NC, MB_USERR
    LD    H, B
    LD    L, C
    INC   HL
    INC   HL
    LD    E, (HL)
    INC   HL
    LD    D, (HL)
    INC   HL
    PUSH  HL
    EX    DE, HL
    CALL  MB_LINPRT
    POP   HL
    LD    A, (HL)
    CP    0x9
    JR    Z, MB_EDIT_LOADBUF
    LD    A, ASCII_SP
    RST   OUTDO
    ; load line into buffer, print
MB_EDIT_LOADBUF:
    CALL  MB_BUFLIN
    LD    HL, MB_BUF
    CALL  MB_LISPRT
    LD    HL, (MB_CURPOS)    ;= 1
    LD    A, 1
    CALL  AL_EDT_ROWST_STORE
    LD    HL, (MB_CURPOS)    ;= 1
    LD    H, 1
    CALL  AL_EDT_FIND_EOL
    CALL  AL_EDT_SETCURS
    JP    MB_MAIN

; --------------------------------------------------
; Read line from file into input buffer
; --------------------------------------------------
MB_INLIN_FROMFILE:
    LD    B, 0xff
    LD    HL, MB_BUF
MB_FILEIN_CHRLP:
    CALL  MB_INDSKC
    JR    C, MB_FILEIN_BUFFULL
    LD    (HL), A
    CP    ASCII_CR
    JR    Z, MB_FILEIN_END
    CP    ASCII_TAB
    JR    Z, MB_FILEIN_SKIPN
    CP    ASCII_SP
    JR    C, MB_FILEIN_CHRLP
MB_FILEIN_SKIPN:
    INC   HL
    DJNZ  MB_FILEIN_CHRLP
MB_FILEIN_END:
    XOR   A
    LD    (HL), A
    LD    HL, MB_BUFMIN    ;=0x2C
    RET

; --------------------------------------------------
; File input: buffer full → run file program
; --------------------------------------------------
MB_FILEIN_BUFFULL:
    INC   B
    JR    NZ, MB_FILEIN_END
    LD    A, (MB_NLONLY)
    AND   0x80
    LD    (MB_NLONLY), A
    CALL  MB_PRGFIN_1
    LD    A, (MB_FILNAM)
    AND   A
    JP    Z, MB_STPRDY
    CALL  MB_RUNC
    JP    MB_NEWSTT

; --------------------------------------------------
; Marks editor row L as used/non-empty
; --------------------------------------------------
AL_EDT_ROWST_SET_USED:
    DB    LD_A_n                                   ; Load A with non zero value  and skip next XOR A instruction

; --------------------------------------------------
; Clears row L's state
; --------------------------------------------------
AL_EDT_ROWST_CLR:
    XOR   A

; --------------------------------------------------
; Stores A into MB_COLSTATE_TBL[L]
; --------------------------------------------------
AL_EDT_ROWST_STORE:
    PUSH  AF
    CALL  AL_VRAM_GETCOLST
    POP   AF
    LD    (DE), A
    RET

; --------------------------------------------------
; Get column state byte from 70d7 table
; --------------------------------------------------
AL_VRAM_GETCOLST:
    PUSH  HL
    LD    DE, MB_COLSTATE_TBL
    LD    H, 0
    ADD   HL, DE
    LD    A, (HL)
    EX    DE, HL
    POP   HL
    AND   A
    RET

; --------------------------------------------------
; Read character from VRAM at (HL)
; --------------------------------------------------
AL_VRAM_GETCHR:
    PUSH  HL
    CALL  MB_CALC_VADR
    CALL  MB_WVSYNC
    LD    C, (HL)
    POP   HL
    RET

; --------------------------------------------------
; Read char via GETCHR, return in A
; --------------------------------------------------
AL_VRAM_READCHAR:
    CALL  AL_VRAM_GETCHR
    LD    A, C
    RET
    PUSH  HL
    CALL  MB_CALC_VADR
    CALL  MB_WVSYNC
    LD    (HL), C
    POP   HL
    RET
    PUSH  HL
    CALL  MB_CALC_VADR

; --------------------------------------------------
; Read attribute from attribute plane (+8 offset)
; --------------------------------------------------
AL_VRAM_GETATTR:
    LD    A, 0x8
    ADD   A, H
    LD    H, A
    LD    B, (HL)
    POP   HL
    RET
    PUSH  HL
    CALL  MB_CALC_VADR

; --------------------------------------------------
; Write attribute to attribute plane
; --------------------------------------------------
AL_VRAM_SETATTR:
    LD    A, 0x8
    ADD   A, H
    LD    H, A
    LD    (HL), B
    POP   HL
    RET

; --------------------------------------------------
; Read char then attribute (combined)
; --------------------------------------------------
AL_VRAM_GETCH_ATTR:
    PUSH  HL
    CALL  MB_CALC_VADR
    LD    C, (HL)
    JR    AL_VRAM_GETATTR

; --------------------------------------------------
; Write char then attribute (combined)
; --------------------------------------------------
AL_VRAM_PUTCH_ATTR:
    PUSH  HL
    CALL  MB_CALC_VADR
    LD    (HL), C
    JR    AL_VRAM_SETATTR

; --------------------------------------------------
; Calc VRAM address
;  Inp: HL - col,  row
;  Put: HL - offset
; --------------------------------------------------
MB_CALC_VADR:
    PUSH  DE
    CALL  MB_CALC_VOFS
    LD    DE, VRAMAD
    ADD   HL, DE
    POP   DE
    RET

; --------------------------------------------------
; Calc VRAM offset
;  Inp: HL - col,  row
;  Put: HL - offset
; --------------------------------------------------
MB_CALC_VOFS:
    DEC   L
    DEC   H
    LD    A, L
    LD    E, H
    LD    H, 0
    LD    D, H
    ADD   A, A
    ADD   A, A
    ADD   A, L
    LD    L, A
    ADD   HL, HL
    ADD   HL, HL
    ADD   HL, HL
    LD    A, (MON.CLUMSZ)      ;= 80
    CP    40
    JR    Z, MB_CVROFS_40
    ADD   HL, HL
MB_CVROFS_40:
    ADD   HL, DE
    RET
    ; Store cursor position (CURPOS = HL)
AL_EDT_SETCURS:
    PUSH  DE
    PUSH  HL
    LD    (MB_CURPOS), HL    ;= 1

; --------------------------------------------------
; ESC 'Y' row col
; Direct cursor move
;  Inp: H - col. L - row
; --------------------------------------------------
MB_CUR_MOVE:
    LD    DE, 0x1f1f                                ; D=E=31
    ADD   HL, DE                                    ; convert coordinates to digit
    PUSH  HL
    LD    C, ASCII_ESC
    CALL  MON.CRTOUT
    LD    C, 'Y'
    CALL  MON.CRTOUT
    POP   HL
    PUSH  HL
    LD    C, L
    CALL  MON.CRTOUT
    POP   HL
    LD    C, H
    CALL  MON.CRTOUT
    POP   HL
    POP   DE
    RET

; --------------------------------------------------
; Get max row (menu bar aware)
; --------------------------------------------------
AL_EDT_MAXROW:
    PUSH  HL
    LD    A, (MB_SCRN_ROWS2)                        ;= 24
    LD    L, A
    LD    A, (AL_SCRN_ROWS)                         ;= 24
    CP    L
    JR    NZ, AL_EDT_ROWMAX_RET
    ; menu bar row adjustment
AL_EDT_MENU_ADJ:
    LD    A, (AL_MENU_FLAG)                         ;=1
    AND   A
    JR    Z, AL_EDT_ROWMAX_RET
    DEC   L
    ; return in A
AL_EDT_ROWMAX_RET:
    LD    A, L
    POP   HL
    RET

; --------------------------------------------------
;  Get max row (fixed, with row-shift up)
; --------------------------------------------------
AL_EDT_MAXROW_FIX:
    PUSH  HL
    LD    A, (AL_SCRN_ROWS)      ;= 24
    LD    L, A
    JR    AL_EDT_MENU_ADJ
    LD    H, A
    CP    L
    JP    Z, AL_EDT_HOME_COL
    RET   NC
    PUSH  HL
    PUSH  HL
    PUSH  AF
    LD    L, H
    CALL  AL_VRAM_GETCOLST
    PUSH  DE
    LD    A, (MB_SCRN_ROWLIM)     ;=1
    DEC   A
    LD    L, A
    CALL  AL_VRAM_GETCOLST
    INC   L
    PUSH  DE
    CALL  AL_VRAM_GETCOLST
    POP   BC
    POP   HL
    OR    A
    DEC   SP
    INC   D
    PUSH  HL
    PUSH  DE
    PUSH  BC
    POP   DE
    POP   HL
    POP   BC
    INC   BC
    LDIR
    LD    A, 1
    LD    (DE), A
    POP   AF
    POP   HL
    PUSH  HL
    LD    H, 1
    LD    L, A
    CALL  MB_CALC_VOFS
    PUSH  HL
    LD    A, (MB_SCRN_ROWLIM)     ;=1
    LD    L, A
    LD    H, 1
    CALL  MB_CALC_VOFS
    POP   DE
    OR    A
    EX    DE, HL
    DEC   SP
    INC   D
    EX    (SP), HL
    LD    A, (MB_SCRN_ROWLIM)     ;=1
    LD    L, A
    INC   L
    LD    H, 1
    PUSH  HL
    CALL  MB_CALC_VADR
    EX    (SP), HL
    DEC   L
    CALL  MB_CALC_VADR
    POP   DE
    EX    DE, HL
    POP   BC
    PUSH  BC
    PUSH  DE
    PUSH  HL
    LD    A, B
    OR    C
    JR    Z, AL_EDT_SHIFTEXT
    LDIR
    ; shift text rows up (LDIR)
AL_EDT_SHIFTEXT:
    POP   HL
    LD    BC, 0x8
    ADD   HL, BC
    EX    DE, HL
    POP   HL
    ADD   HL, BC
    EX    DE, HL
    POP   BC
    LD    A, B
    OR    C
    JR    Z, AL_EDT_SHIFTATTR
    LDIR
    ; shift attribute plane up
AL_EDT_SHIFTATTR:
    POP   HL
    DEC   L

; --------------------------------------------------
; Set column to 1 (home)
; --------------------------------------------------
AL_EDT_HOME_COL:
    LD    H, 1

; --------------------------------------------------
; Clear from cursor to end of line
; --------------------------------------------------
AL_EDT_CLREOL:
    CALL  AL_EDT_ROWST_SET_USED
    PUSH  BC
    LD    C, H
    CALL  MB_CALC_VADR
    CALL  MON.CTVOFF

    ; write space + attribute per cell
AL_EDT_CLR_CELL:
    LD    (HL), ASCII_SP
    PUSH  HL
    LD    A, 0x8
    ADD   A, H
    LD    H, A
    LD    A, (MON.CPATR)       ;= 7
    LD    (HL), A
    POP   HL
    INC   HL
    INC   C
    LD    A, (MON.CLUMSZ)      ;= 80
    CP    C
    JR    NC, AL_EDT_CLR_CELL
    CALL  MON.CTVON
    POP   BC
    RET

; --------------------------------------------------
; Shift row up if at bottom
; --------------------------------------------------
AL_EDT_WRAPSHIFT:
    CALL  AL_EDT_MAXROW
    CP    L
    JR    NC, AL_EDT_SHIFTLINE
    CALL  AL_EDT_MAXROW_FIX
    ; same-row line shift (DELCHAR)
AL_EDT_SHIFTLINE:
    CP    L
    JR    NZ, AL_EDT_SHIFTLEFT
    PUSH  HL
    CALL  AL_EDT_DELCHAR
    POP   HL
    DEC   L
    CALL  AL_EDT_SETCURS
    SCF
    RET
    ; shift text left by N columns
AL_EDT_SHIFTLEFT:
    INC   L
    CP    L
    JR    Z, AL_EDT_WRAP_HOME
    PUSH  HL
    LD    H, A
    PUSH  HL
    CALL  AL_VRAM_GETCOLST
    PUSH  DE
    LD    L, H
    CALL  AL_VRAM_GETCOLST
    EX    DE, HL
    POP   DE
    PUSH  HL
    OR    A
    SBC   HL, DE
    LD    C, L
    LD    B, H
    LD    A, L
    POP   DE
    PUSH  AF
    LD    L, E
    LD    H, D
    DEC   HL
    LDDR
    POP   AF
    CALL  AL_EDT_CELLVADR
    POP   HL
    LD    L, H
    LD    A, (MON.CLUMSZ)      ;= 80
    LD    H, A
    PUSH  HL
    CALL  MB_CALC_VADR
    EX    (SP), HL
    DEC   L
    CALL  MB_CALC_VADR
    POP   DE
    PUSH  BC
    PUSH  DE
    PUSH  HL
    CALL  MON.CTVOFF
    LDDR
    CALL  MON.CTVON
    LD    BC, 0x800
    POP   HL
    ADD   HL, BC
    EX    DE, HL
    POP   HL
    ADD   HL, BC
    EX    DE, HL
    POP   BC
    CALL  MON.CTVOFF
    LDDR
    CALL  MON.CTVON
    POP   HL
    ; home column, return
AL_EDT_WRAP_HOME:
    CALL  AL_EDT_HOME_COL
    OR    A
    RET

; --------------------------------------------------
; Move cursor to end of line
; --------------------------------------------------
AL_EDT_EOL_MOVE:
    CALL  AL_EDT_ATCOL1
    RET   NZ
    LD    A, (MON.CLUMSZ)      ;= 80
    LD    H, A

; --------------------------------------------------
; Row boundary adjustment (clamp)
; --------------------------------------------------
AL_EDT_ROWADJ:
    LD    A, (MB_SCRN_ROWLIM)     ;=1
    CP    L
    RET   Z
    CALL  AL_EDT_MAXROW
    INC   A
    CP    L
    RET   Z
    DEC   L
    RET   Z
    JR    AL_EDT_STORECURS

; --------------------------------------------------
; Check if at column 1
; --------------------------------------------------
AL_EDT_ATCOL1:
    DEC   H
    RET   Z
    JR    AL_EDT_STORECURS

; --------------------------------------------------
; Wrap position check (col+row bounds)
; --------------------------------------------------
AL_EDT_WRAPPOS_CHK:
    CALL  AL_EDT_ATCOLMAX
    RET   NZ
    LD    H, 1
    CALL  AL_EDT_MAXROW
    CP    L
    RET   Z
    CALL  AL_EDT_MAXROW_FIX
    CP    L
    RET   Z
    INC   L
    JR    AL_EDT_STORECURS

; --------------------------------------------------
; Check/advance at max column
; --------------------------------------------------
AL_EDT_ATCOLMAX:
    LD    A, (MON.CLUMSZ)      ;= 80
    CP    H
    RET   Z
    INC   H
    JR    AL_EDT_STORECURS

; --------------------------------------------------
; Row advance check
; --------------------------------------------------
AL_EDT_ROWADV_CHK:
    CALL  AL_EDT_MAXROW
    CP    L
    RET   Z
    CALL  AL_EDT_MAXROW_FIX
    CP    L
    RET   Z
    INC   L

; --------------------------------------------------
; Store cursor position and return
; --------------------------------------------------
AL_EDT_STORECURS:
    LD    (MB_CURPOS), HL    ;= 1
    RET

; ==================================================
; BASIC Interpreter pointer get routines/WHG/PGA
; ==================================================

; --------------------------------------------------
; Dimension and variable searching
; --------------------------------------------------
MB_DIMCON:
    DEC   HL
    RST   CHRGTR
    RET   Z
    RST   SYNCHR
    DB    ','

; --------------------------------------------------
; Sets DIMFLAG and fall back to into variable search
; routine.
; --------------------------------------------------
MB_DIM:
    LD    BC, MB_DIMCON
    PUSH  BC
    DB    OR_n

; --------------------------------------------------
; Read variable name at current text position.
; Out: DE -> to its value
;      HL -> to netx char after variable name
;      VALTYP - variable type
; --------------------------------------------------
MB_PTRGET:
    XOR   A
    LD    (MB_DIMFLG), A
    LD    C, (HL)
MB_PTRGT2:
    CALL  MB_ISLET
    JP    C, MB_SNERR
    XOR   A
    LD    B, A
    LD    (MB_NAMCNT), A
    INC   HL
    LD    A, (HL)
    CP    '.'
    JR    C, MB_NOSEC
    JR    Z, MB_ISSEC
    CP    '9'+1
    JR    NC, MB_PTRGT3
    CP    '0'
    JR    NC, MB_ISSEC
MB_PTRGT3:
    CALL  MB_ISLET2
    JR    C, MB_NOSEC
MB_ISSEC:
    LD    B, A
    PUSH  BC
    LD    B, 0xff
    LD    DE, MB_NAMCNT
MB_VMORCH:
    OR    0x80
    INC   B
    LD    (DE), A
    INC   DE
    INC   HL
    LD    A, (HL)
    CP    '9'+1
    JR    NC, MB_VMORC1
    CP    '0'
    JR    NC, MB_VMORCH
MB_VMORC1:
    CALL  MB_ISLET2
    JR    NC, MB_VMORCH
    CP    '.'
    JR    Z, MB_VMORCH
    LD    A, B
    CP    39  ;NAMLEN-1
    JP    NC, MB_SNERR
    POP   BC
    LD    (MB_NAMCNT), A
    LD    A, (HL)
MB_NOSEC:
    CP    '%'+1
    JR    NC, MB_TABTYP
    LD    DE, MB_HAVTYP
    PUSH  DE
    LD    D, 2
    CP    '%'
    RET   Z
    INC   D
    CP    '$'
    RET   Z
    INC   D
    CP    '!'
    RET   Z
    LD    D, 0x8
    CP    '#'
    RET   Z
    POP   AF
MB_TABTYP:
    LD    A, C
    AND   0x7f
    LD    E, A
    LD    D, 0
    PUSH  HL
    LD    HL, MB_TEMPST_1  ;MB_DEFTBL-'A'
    ADD   HL, DE
    LD    D, (HL)
    POP   HL
    DEC   HL
MB_HAVTYP:
    LD    A, D
    LD    (MB_VALTYP), A
    RST   CHRGTR
    LD    A, (MB_SUBFLG)
    DEC   A
    JP    Z, MB_ERSFIN
    JP    P, MB_NOARYS
    LD    A, (HL)
    SUB   '('
    JP    Z, MB_ISARY
    SUB   51  ;'['-')'+1
    JP    Z, MB_ISARY
MB_NOARYS:
    XOR   A
    LD    (MB_SUBFLG), A
    PUSH  HL
    LD    A, (MB_NOFUNS)
    OR    A
    LD    (MB_PRMFLG), A
    JR    Z, MB_SNFUNS
    LD    HL, (MB_PRMLEN)
    LD    DE, MB_PARM1
    ADD   HL, DE
    LD    (MB_ARYTA2), HL
    EX    DE, HL
    JR    MB_LOPFND

MB_LOPTOP:
    LD    A, (DE)
    LD    L, A
    INC   DE
    LD    A, (DE)
    INC   DE
    CP    C
    JR    NZ, MB_NOTIT1
    LD    A, (MB_VALTYP)
    CP    L
    JR    NZ, MB_NOTIT1
    LD    A, (DE)
    CP    B
    JP    Z, MB_FINPTR
MB_NOTIT1:
    INC   DE
MB_NFINPT:
    LD    A, (DE)
MB_SNOMAT:
    LD    H, 0
    ADD   A, L
    INC   A
    LD    L, A
    ADD   HL, DE
MB_LOPFND:
    EX    DE, HL
    LD    A, (MB_ARYTA2)
    CP    E
    JP    NZ, MB_LOPTOP
    LD    A, (MB_ARYTA2+1)
    CP    D
    JR    NZ, MB_LOPTOP
    LD    A, (MB_PRMFLG)
    OR    A
    JR    Z, MB_SMKVAR
    XOR   A
    LD    (MB_PRMFLG), A
MB_SNFUNS:
    LD    HL, (MB_ARYTAB)
    LD    (MB_ARYTA2), HL
    LD    HL, (MB_VARTAB)
    JR    MB_LOPFND

; --------------------------------------------------
MB_NVRFIL:
    CALL  MB_PTRGET
MB_FILRET:
    RET

    ; Exit for VARPTR
MB_VARNOT:
    LD    D, A
    LD    E, A
    POP   BC
    EX    (SP), HL
    RET

; --------------------------------------------------
MB_SMKVAR:
    POP   HL
    EX    (SP), HL
    PUSH  DE
    LD    DE, MB_FILRET
    RST   DCOMPR
    JR    Z, MB_VARNOT
    LD    DE, MB_RETVAR
    RST   DCOMPR
    POP   DE
    JR    Z, MB_FINZER
    EX    (SP), HL
    PUSH  HL
    PUSH  BC
    LD    A, (MB_VALTYP)
    LD    B, A
    LD    A, (MB_NAMCNT)
    ADD   A, B
    INC   A
    LD    C, A
    PUSH  BC
    LD    B, 0
    INC   BC
    INC   BC
    INC   BC
    LD    HL, (MB_STREND)
    PUSH  HL
    ADD   HL, BC
    POP   BC
    PUSH  HL
    CALL  MB_BLTU
    POP   HL
    LD    (MB_STREND), HL
    LD    H, B
    LD    L, C
    LD    (MB_ARYTAB), HL
.ZEROER:
    DEC   HL
    LD    (HL), 0
    RST   DCOMPR
    JR    NZ, .ZEROER
    POP   DE
    LD    (HL), D
    INC   HL
    POP   DE
    LD    (HL), E
    INC   HL
    LD    (HL), D
    CALL  MB_NPUTSB
    EX    DE, HL
    INC   DE
    POP   HL
    RET

; --------------------------------------------------
MB_FINPTR:
    INC   DE
    LD    A, (MB_NAMCNT)
    LD    H, A
    LD    A, (DE)
    CP    H
    JP    NZ, MB_NFINPT
    OR    A
    JR    NZ, MB_NTFPRT
    INC   DE
    POP   HL
    RET

; --------------------------------------------------
MB_NTFPRT:
    EX    DE, HL
    CALL  MB_MATSUB
    EX    DE, HL
    JP    NZ, MB_SNOMAT
    POP   HL
    RET

; --------------------------------------------------
; Make all types zero and skip return
; --------------------------------------------------
MB_FINZER:
    LD    (MB_DACC), A
    LD    H, A
    LD    L, A
    LD    (MB_DACC+2), HL
    RST   GETYPTR
    JR    NZ, MB_POPHR2
    LD    HL, MB_OKTXT-1
    LD    (MB_DACC+2), HL
MB_POPHR2:
    POP   HL
    RET

; --------------------------------------------------
MB_ISARY:
    PUSH  HL
    LD    HL, (MB_DIMFLG)
    EX    (SP), HL
    LD    D, A
MB_INDLOP:
    PUSH  DE
    PUSH  BC
    LD    DE, MB_NAMCNT
    LD    A, (DE)
    OR    A
    JR    Z, MB_SHTNAM
    EX    DE, HL
    ADD   A, 0x2
    RRA
    LD    C, A
    CALL  MB_GETSTK
    LD    A, C
MB_LPPSNM:
    LD    C, (HL)
    INC   HL
    LD    B, (HL)
    INC   HL
    PUSH  BC
    DEC   A
    JR    NZ, MB_LPPSNM
    PUSH  HL
    LD    A, (MB_NAMCNT)
    PUSH  AF
    EX    DE, HL
    CALL  MB_INTIDX
    POP   AF
    LD    (MB_NAMTMP), HL
    POP   HL
    ADD   A, 0x2
    RRA
MB_LPLNAM:
    POP   BC
    DEC   HL
    LD    (HL), B
    DEC   HL
    LD    (HL), C
    DEC   A
    JR    NZ, MB_LPLNAM
    LD    HL, (MB_NAMTMP)
    JR    MB_LNGNAM
MB_SHTNAM:
    CALL  MB_INTIDX
    XOR   A
    LD    (MB_NAMCNT), A
MB_LNGNAM:
    LD    A, (MB_OPTVAL)
    OR    A
    JR    Z, MB_OPTB0
    LD    A, D
    OR    E
    JP    Z, MB_BSERR
    DEC   DE
MB_OPTB0:
    POP   BC
    POP   AF
    EX    DE, HL
    EX    (SP), HL
    PUSH  HL
    EX    DE, HL
    INC   A
    LD    D, A
    LD    A, (HL)
    CP    ','
    JP    Z, MB_INDLOP
    CP    ')'
    JR    Z, MB_DOCHRT
    CP    ']'
    JP    NZ, MB_SNERR
MB_DOCHRT:
    RST   CHRGTR
    LD    (MB_TEMP2), HL
    POP   HL
    LD    (MB_DIMFLG), HL
    LD    E, 0
    PUSH  DE
    DB    LD_DE_nn

; --------------------------------------------------
MB_ERSFIN:
    PUSH  HL
    PUSH  AF
    LD    HL, (MB_ARYTAB)
    DB    LD_A_n
MB_LOPFDA:
    ADD   HL, DE
    LD    DE, (MB_STREND)
    RST   DCOMPR
    JR    Z, MB_NOTFDD
    LD    E, (HL)
    INC   HL
    LD    A, (HL)
    INC   HL
    CP    C
    JR    NZ, MB_NMARY1
    LD    A, (MB_VALTYP)
    CP    E
    JR    NZ, MB_NMARY1
    LD    A, (HL)
    CP    B
    JR    Z, MB_CMPNAM
MB_NMARY1:
    INC   HL
MB_BNAMSZ:
    LD    E, (HL)
    INC   E
    LD    D, 0
    ADD   HL, DE
MB_CNOMAT:
    LD    E, (HL)
    INC   HL
    LD    D, (HL)
    INC   HL
    JR    NZ, MB_LOPFDA
    LD    A, (MB_DIMFLG)
    OR    A
    JP    NZ, MB_DDERR
    POP   AF
    LD    B, H
    LD    C, L
    JP    Z, MB_POPHLRT
    SUB   (HL)
    JP    Z, MB_GETDEF

; --------------------------------------------------
MB_BSERR:
    LD    DE, ERRBS                                 ; Subscript out of range
    JP    MB_ERROR

; --------------------------------------------------
MB_CMPNAM:
    INC   HL
    LD    A, (MB_NAMCNT)
    CP    (HL)
    JR    NZ, MB_BNAMSZ
    INC   HL
    OR    A
    JR    Z, MB_CNOMAT
    DEC   HL
    CALL  MB_MATSUB
    JR    MB_CNOMAT

; --------------------------------------------------
MB_NOTFDD:
    LD    A, (MB_VALTYP)
    LD    (HL), A
    INC   HL
    LD    E, A
    LD    D, 0
    POP   AF
    JP    Z, MB_PTRRNZ
    LD    (HL), C
    INC   HL
    LD    (HL), B
    CALL  MB_NPUTSB
    INC   HL
    LD    C, A
    CALL  MB_GETSTK
    INC   HL
    INC   HL
    LD    (MB_TEMP3), HL
    LD    (HL), C
    INC   HL
    LD    A, (MB_DIMFLG)
    RLA
    LD    A, C
MB_LOPPTA:
    JR    C, MB_POPDIM
    PUSH  AF
    LD    A, (MB_OPTVAL)
    XOR   0xb
    LD    C, A
    LD    B, 0
    POP   AF
    JR    NC, MB_NOTDIM
MB_POPDIM:
    POP   BC
    INC   BC
MB_NOTDIM:
    LD    (HL), C
    PUSH  AF
    INC   HL
    LD    (HL), B
    INC   HL
    CALL  MB_UMULT
    POP   AF
    DEC   A
    JR    NZ, MB_LOPPTA
    PUSH  AF
    LD    B, D
    LD    C, E
    EX    DE, HL
    ADD   HL, DE
    JP    C, MB_OMERR
    CALL  MB_REASON
    LD    (MB_STREND), HL
MB_ZERITA:
    DEC   HL
    LD    (HL), 0
    RST   DCOMPR
    JR    NZ, MB_ZERITA
    INC   BC
    LD    D, A
    LD    HL, (MB_TEMP3)
    LD    E, (HL)
    EX    DE, HL
    ADD   HL, HL
    ADD   HL, BC
    EX    DE, HL
    DEC   HL
    DEC   HL
    LD    (HL), E
    INC   HL
    LD    (HL), D
    INC   HL
    POP   AF
    JR    C, MB_FINNOW
MB_GETDEF:
    LD    B, A
    LD    C, A
    LD    A, (HL)
    INC   HL
    DB    LD_D_n
MB_INLPNM:
    POP   HL
    LD    E, (HL)
    INC   HL
    LD    D, (HL)
    INC   HL
    EX    (SP), HL
    PUSH  AF
    RST   DCOMPR
    JP    NC, MB_BSERR
    CALL  MB_UMULT
    ADD   HL, DE
    POP   AF
    DEC   A
    LD    B, H
    LD    C, L
    JR    NZ, MB_INLPNM
    LD    A, (MB_VALTYP)
    LD    B, H
    LD    C, L
    ADD   HL, HL
    SUB   0x4
    JR    C, MB_SMLVAL
    ADD   HL, HL
    JR    Z, MB_DONMUL
    ADD   HL, HL
MB_SMLVAL:
    OR    A
    JP    PO, MB_DONMUL
    ADD   HL, BC
MB_DONMUL:
    POP   BC
    ADD   HL, BC
    EX    DE, HL
MB_FINNOW:
    LD    HL, (MB_TEMP2)
    RET
MB_PTRRNZ:
    SCF
    SBC   A, A
    POP   HL
    RET


; ==================================================
; Long variable names subroutines
; ==================================================

; --------------------------------------------------
; Add A to HL
; --------------------------------------------------
MB_IADAHL:
    LD    A, (HL)
    INC   HL
MB_ADDAHL:
    PUSH  BC
    LD    B, 0
    LD    C, A
    ADD   HL, BC
    POP   BC
    RET
MB_NPUTSB:
    PUSH  BC
    PUSH  DE
    PUSH  AF
    LD    DE, MB_NAMCNT
    LD    A, (DE)
    LD    B, A
    INC   B
MB_SLPLNG:
    LD    A, (DE)
    INC   DE
    INC   HL
    LD    (HL), A
    DEC   B
    JR    NZ, MB_SLPLNG
    POP   AF
    POP   DE
    POP   BC
    RET

; --------------------------------------------------
; Tries to perform a match
; --------------------------------------------------
MB_MATSUB:
    PUSH  DE
    PUSH  BC
    LD    DE, MB_NAMBUF
    LD    B, A
    INC   HL
    INC   B
MB_SLPMAT:
    DEC   B
    JR    Z, MB_ISMAT2
    LD    A, (DE)
    INC   DE
    CP    (HL)
    INC   HL
    JR    Z, MB_SLPMAT
    LD    A, B
    DEC   A
    CALL  NZ, MB_ADDAHL
    XOR   A
    DEC   A
MB_ISMAT2:
    POP   BC
    POP   DE
    RET
; ==================================================
; End of BASIC Interpreter pointer get routines
; ==================================================


; ==================================================
; BASIC Interpreter PRINT USING Driver
; ==================================================

; --------------------------------------------------
; PRINT USING statement
; --------------------------------------------------
MB_PRINUS:
    CALL  MB_FRMCHK
    CALL  MB_CHKSTR
    RST   SYNCHR
    DB    ';'
    EX    DE, HL
    LD    HL, (MB_DACC+2)
    JR    MB_INIUS

; --------------------------------------------------
MB_REUSST:
    LD    A, (MB_USFLG)
    OR    A
    JR    Z, MB_FCERR3
    POP   DE
    EX    DE, HL
MB_INIUS:
    PUSH  HL
    XOR   A
    LD    (MB_USFLG), A
    INC   A
    PUSH  AF
    PUSH  DE
    LD    B, (HL)
    INC   B
    DEC   B
MB_FCERR3:
    JP    Z, MB_FCERR
    INC   HL
    LD    A, (HL)
    INC   HL
    LD    H, (HL)
    LD    L, A
    JR    MB_PRCCHR
MB_BGSTRF:
    LD    E, B
    PUSH  HL
    LD    C, 0x2
MB_LPSTRF:
    LD    A, (HL)
    INC   HL
    CP    ASCII_BKSL
    JP    Z, MB_ISSTRF
    CP    ASCII_SP
    JR    NZ, MB_NOSTRF
    INC   C
    DJNZ  MB_LPSTRF
MB_NOSTRF:
    POP   HL
    LD    B, E
    LD    A, ASCII_BKSL
MB_NEWUCH:
    CALL  MB_PLSPRT
    RST   OUTDO
MB_PRCCHR:
    XOR   A
    LD    E, A
    LD    D, A
MB_PLSFIN:
    CALL  MB_PLSPRT
    LD    D, A
    LD    A, (HL)
    INC   HL
    CP    '!'
    JP    Z, MB_SMSTRF
    CP    '#'
    JR    Z, MB_NUMNUM
    CP    '&'
    JP    Z, MB_VARSTR
    DEC   B
    JP    Z, MB_REUSIN
    CP    '+'
    LD    A, 0x8     ;plus flag
    JR    Z, MB_PLSFIN
    DEC   HL
    LD    A, (HL)
    INC   HL
    CP    '.'
    JR    Z, MB_DOTNUM
    CP    '_'
    JP    Z, MB_LITCHR
    CP    ASCII_BKSL
    JR    Z, MB_BGSTRF
    CP    (HL)
    JR    NZ, MB_NEWUCH
    CP    '$'
    JR    Z, MB_DOLRNM
    CP    '*'
    JR    NZ, MB_NEWUCH
    INC   HL
    LD    A, B
    CP    0x2
    JR    C, MB_NOTSPC
    LD    A, (HL)
    CP    '$'
MB_NOTSPC:
    LD    A, ASCII_SP                               ; Set asterisk bit
    JR    NZ, MB_SPCNUM
    DEC   B
    INC   E
    DB    CP_n
MB_DOLRNM:
    XOR   A
    ADD   A, 0x10                                   ; Set floating $ flag
    INC   HL
MB_SPCNUM:
    INC   E
    ADD   A, D
    LD    D, A
MB_NUMNUM:
    INC   E
    LD    C, 0
    DEC   B
    JR    Z, MB_ENDNUS
    LD    A, (HL)
    INC   HL
    CP    '.'
    JR    Z, MB_AFTDOT
    CP    '#'
    JR    Z, MB_NUMNUM
    CP    ','
    JR    NZ, MB_FINNUM
    LD    A, D
    OR    0x40
    LD    D, A
    JR    MB_NUMNUM
MB_DOTNUM:
    LD    A, (HL)
    CP    '#'
    LD    A, '.'
    JP    NZ, MB_NEWUCH
    LD    C, 1
    INC   HL
MB_AFTDOT:
    INC   C
    DEC   B
    JR    Z, MB_ENDNUS
    LD    A, (HL)
    INC   HL
    CP    '#'
    JR    Z, MB_AFTDOT
MB_FINNUM:
    PUSH  DE
    LD    DE, MB_NOTSCI
    PUSH  DE
    LD    D, H
    LD    E, L
    CP    '^'
    RET   NZ
    CP    (HL)
    RET   NZ
    INC   HL
    CP    (HL)
    RET   NZ
    INC   HL
    CP    (HL)
    RET   NZ
    INC   HL
    LD    A, B
    SUB   0x4
    RET   C
    POP   DE
    POP   DE
    LD    B, A
    INC   D
    INC   HL
    DB    JP_Z_nn
; --------------------------------------------------
MB_NOTSCI:
    EX    DE, HL
    POP   DE
MB_ENDNUS:
    LD    A, D
    DEC   HL
    INC   E
    AND   0x8
    JR    NZ, MB_ENDNUM
    DEC   E
    LD    A, B
    OR    A
    JR    Z, MB_ENDNUM
    LD    A, (HL)
    SUB   '-'
    JR    Z, MB_SGNTRL                              ; trailing sign
    CP    '+' - '-'                                 ; trailing plus?
    JR    NZ, MB_ENDNUM
    LD    A, 0x8
MB_SGNTRL:
    ADD   A, 0x4                                    ; turn on trailing sign flag
    ADD   A, D
    LD    D, A
    DEC   B
MB_ENDNUM:
    POP   HL
    POP   AF
    JR    Z, MB_FLDFIN
    PUSH  BC
    PUSH  DE
    CALL  MB_FRMEVL
    POP   DE
    POP   BC
    PUSH  BC
    PUSH  HL
    LD    B, E
    LD    A, B
    ADD   A, C
    CP    25
    JP    NC, MB_FCERR
    LD    A, D
    OR    0x80
    CALL  MB_PUFOUT
    CALL  MB_STROUT
MB_FNSTRF:
    POP   HL
    DEC   HL
    RST   CHRGTR
    SCF
    JR    Z, MB_CRDNUS
    LD    (MB_USFLG), A
    CP    ';'
    JR    Z, MB_SEMUSN
    RST   SYNCHR
    DB    ','
    DB    LD_B_n
MB_SEMUSN:
    RST   CHRGTR
MB_CRDNUS:
    POP   BC
    EX    DE, HL
    POP   HL
    PUSH  HL
    PUSH  AF
    PUSH  DE
    LD    A, (HL)
    SUB   B
    INC   HL
    LD    D, 0
    LD    E, A
    LD    A, (HL)
    INC   HL
    LD    H, (HL)
    LD    L, A
    ADD   HL, DE
CHKUSI:
    LD    A, B
    OR    A
    JP    NZ, MB_PRCCHR
    JR    MB_FINUSI

; --------------------------------------------------
MB_REUSIN:
    CALL  MB_PLSPRT
    RST   OUTDO
MB_FINUSI:
    POP   HL
    POP   AF
    JP    NZ, MB_REUSST
MB_FLDFIN:
    CALL  C, MB_CRDO
    EX    (SP), HL
    CALL  MB_FRETM2
    POP   HL
    JP    MB_FINPRT

; --------------------------------------------------
MB_LITCHR:
    CALL  MB_PLSPRT
    DEC   B
    LD    A, (HL)
    INC   HL
    RST   OUTDO
    JR    CHKUSI

; --------------------------------------------------
MB_VARSTR:
    LD    C, 0
    JR    MB_ISSTR1
MB_SMSTRF:
    LD    C, 1
    DB    LD_A_n
MB_ISSTRF:
    POP   AF
MB_ISSTR1:
    DEC   B
    CALL  MB_PLSPRT
    POP   HL
    POP   AF
    JR    Z, MB_FLDFIN
    PUSH  BC
    CALL  MB_FRMEVL
    CALL  MB_CHKSTR
    POP   BC
    PUSH  BC
    PUSH  HL
    LD    HL, (MB_DACC+2)
    LD    B, C
    LD    C, 0
    LD    A, B
    PUSH  AF
    LD    A, B
    OR    A
    CALL  NZ, MB_LEFTUS
    CALL  MB_STRPRT
    LD    HL, (MB_DACC+2)
    POP   AF
    OR    A
    JP    Z, MB_FNSTRF
    SUB   (HL)
    LD    B, A
    LD    A, ASCII_SP
    INC   B
MB_UPRTSP:
    DEC   B
    JP    Z, MB_FNSTRF
    RST   OUTDO
    JR    MB_UPRTSP

; --------------------------------------------------
MB_PLSPRT:
    PUSH  AF
    LD    A, D
    OR    A
    LD    A, '+'
    CALL  NZ, OUTDO
    POP   AF
    RET

; ==================================================
; End of PRINT USING Driver
; ==================================================


; ==================================================
; BASIC Interpreter I/O routines
; ==================================================

; --------------------------------------------------
; Print char
;  Inp: A - char to print
; --------------------------------------------------
MB_OUTDO:
    CALL  MB_ISFLIO
    JP    NZ, MB_ISCONO
    LD    A, (MB_PRTFLG)
    OR    A
    JP    Z, MB_TTYCHR
    POP   AF

; --------------------------------------------------
MB_OUTDO1:
    PUSH  AF
    CP    ASCII_BS
    JR    NZ, MB_NTBKS2
    LD    A, (MB_LPTPOS)
    SUB   1
    JR    C, MB_SKPBS
    LD    (MB_LPTPOS), A
MB_SKPBS:
    POP   AF
    JR    MB_OUTEXIT
MB_NTBKS2:
    CP    ASCII_TAB
    JR    NZ, MB_NOTABL
MB_MORSPL:
    LD    A, ASCII_SP
    CALL  MB_OUTDO1
    LD    A, (MB_LPTPOS)
    AND   0x7
    JR    NZ, MB_MORSPL
    POP   AF
    RET
MB_NOTABL:
    SUB   ASCII_CR
    JR    Z, MB_ZERLP1                              ; do nothing
    JR    C, MB_LPTCH1                              ; just print char
    CP    0x13                                      ; <0x20 pintable?
    JR    C, MB_LPTCH1
    LD    A, (MB_LPTSIZ)                            ; = 255
    INC   A
    LD    A, (MB_LPTPOS)
    JR    Z, MB_ZERLPT
    PUSH  HL
    LD    HL, MB_LPTSIZ                             ; = 255
    CP    (HL)                                      ; = 255
    POP   HL
    CALL  Z, MB_PRINTW
MB_ZERLPT:
    INC   A
MB_ZERLP1:
    LD    (MB_LPTPOS), A
MB_LPTCH1:
    POP   AF

; --------------------------------------------------
MB_OUTEXIT:
    JP    MB_LPT_OUTCHK

; --------------------------------------------------
; End printer output
; --------------------------------------------------
MB_FINLPT:
    XOR   A
    LD    (MB_PRTFLG), A
    LD    A, (MB_LPTPOS)
    OR    A
    RET   Z

; --------------------------------------------------
MB_PRINTW:
    LD    A, ASCII_CR
    CALL  MB_OUTEXIT
    LD    A, ASCII_LF
    CALL  MB_OUTEXIT
    XOR   A
    LD    (MB_LPTPOS), A
    RET

; --------------------------------------------------
MB_TTYCHR:
    POP   AF
    PUSH  AF
    PUSH  HL
    LD    HL, MB_CNTOFL
    DEC   (HL)
    INC   (HL)
    POP   HL
    JR    NZ, MB_TRYOUT
    CP    ASCII_BS
    JR    NZ, MB_NTBKS1
    LD    A, (MB_TTYPOS)
    OR    A
    JR    Z, MB_MORSPR
    DEC   A
    LD    (MB_TTYPOS), A
    LD    A, ASCII_BS
    JR    MB_TRYOUT
MB_NTBKS1:
    CP    ASCII_TAB
    JR    NZ, MB_TRYOUT
MB_MORSP:
    LD    A, ASCII_SP
    RST   OUTDO
    LD    A, (MB_TTYPOS)
    AND   0x7       ;at tabstop?
    JR    NZ, MB_MORSP
MB_MORSPR:
    POP   AF
    RET

; --------------------------------------------------
MB_TRYOUT:
    CALL  MB_COUT
    LD    A, (MB_COUT_FL)                           ; = 1
    DEC   A
    LD    (MB_TTYPOS), A
    POP   AF
    RET
    CALL  MB_ISFLIO
    JR    Z, MB_TRYOUT2
    CALL  MB_INDSKC
    RET   NC
    PUSH  BC
    PUSH  DE
    PUSH  HL
    LD    A, (MB_NLONLY)
    AND   0x80
    LD    (MB_NLONLY), A
    CALL  MB_PRGFIN_1
    POP   HL
    POP   DE
    POP   BC
    LD    A, (MB_CHNFLG)
    OR    A
    JP    NZ, MB_CHNRET
    LD    A, (MB_FILNAM)
    OR    A
    JR    Z, MB_TRYOUT1
    CALL  MB_RUNC
    JP    MB_NEWSTT
MB_TRYOUT1:
    PUSH  HL
    PUSH  BC
    PUSH  DE
    LD    HL, MB_OKTXT     ;= "Ok\r\n"
    CALL  MB_STROUT
    POP   DE
    POP   BC
    LD    A, ASCII_CR
    POP   HL
    RET

; --------------------------------------------------
MB_TRYOUT2:
    CALL  MB_DIRECT_GETKEY
    RET

; --------------------------------------------------
; Only print a carriage return when not at left margin
; --------------------------------------------------
MB_CRDONZ:
    LD    A, (MB_TTYPOS)
    OR    A
    RET   Z
    JR    MB_CRDO
    LD    (HL), 0
    CALL  MB_ISFLIO
    LD    HL, MB_BUFMIN    ;=0x2C
    JR    NZ, MB_CRFIN

; --------------------------------------------------
; Put out a carriage return
;  Out: A=0,  Zero CC set,  Carry reset.
; --------------------------------------------------
MB_CRDO:
    LD    A, ASCII_CR
    RST   OUTDO
    LD    A, ASCII_LF
    RST   OUTDO

; --------------------------------------------------
MB_CRFIN:
    CALL  MB_ISFLIO
    JR    Z, MB_CRCONT
    XOR   A
    RET

; --------------------------------------------------
MB_CRCONT:
    LD    A, (MB_PRTFLG)
    OR    A
    JR    Z, MB_ISSCRN
    XOR   A
    LD    (MB_LPTPOS), A
    RET

; --------------------------------------------------
MB_ISSCRN:
    XOR   A
    LD    (MB_TTYPOS), A
    XOR   A
    RET

; --------------------------------------------------
MB_ISCNTC:
    CALL  MB_CTRLC_CHK
    RET   Z

; --------------------------------------------------
MB_CNTCCN:
    CALL  MB_TRYOUT2
    CP    0x13
    CALL  Z, MB_TRYOUT2
    LD    (MB_NULCNT), A
    CP    0x3
    CALL  Z, MB_NTCTCT
    JP    MB_STOP

; --------------------------------------------------
; INKEY$ function
; --------------------------------------------------
MB_INKEY:
    RST   CHRGTR
    PUSH  HL
    CALL  MB_CHARCG
    JR    NZ, MB_BUFCIN
    CALL  MB_CTRLC_CHK
    JR    Z, MB_NULRT                               ; no key, return empty string
    CALL  MB_TRYOUT2
MB_BUFCIN:
    PUSH  AF
    CALL  MB_STRIN1
    POP   AF
    LD    E, A
    CALL  MB_SETSTR
MB_NULRT:
    LD    HL, MB_OKTXT-1
    LD    (MB_DACC+2), HL
    LD    A, TSTR
    LD    (MB_VALTYP), A
    POP   HL
    RET

; --------------------------------------------------
MB_CHARCG:
    LD    A, (MB_NULCNT)
    OR    A
    RET   Z
    PUSH  AF
    XOR   A
    LD    (MB_NULCNT), A
    POP   AF
    RET

; --------------------------------------------------
; Out char to interpreter output,  LF expanded
; --------------------------------------------------
MB_OUTCH1:
    RST   OUTDO
    CP    ASCII_LF
    RET   NZ
    LD    A, ASCII_CR
    RST   OUTDO
    CALL  MB_CRFIN
    LD    A, ASCII_LF
    RET

; ==================================================
; End of BASIC Interpreter I/O routines
; ==================================================

; --------------------------------------------------
; Coordinate mode check (@ absolute / - relative)
; --------------------------------------------------
MB_COORD_MODECHK:
    LD    A, (HL)
    CP    '@'                                       ; 0x40 | 64 | 'A'-1
    CALL  Z, MB_CHRGTR
    LD    BC, 0
    LD    D, B
    LD    E, C
    CP    MINUTK
    JR    Z, MB_COORD_ADDX

; --------------------------------------------------
MB_GETPAR_XY:
    LD    A, (HL)
    CP    _STEP                                     ; relative coordinates?
    PUSH  AF
    CALL  Z, MB_CHRGTR
    RST   SYNCHR
    DB    '('
    CALL  MB_GETIN2                                 ; Get X
    PUSH  DE
    RST   SYNCHR
    DB    ','
    CALL  MB_GETIN2                                 ; Get Y
    RST   SYNCHR
    DB    ')'
    POP   BC
    POP   AF
    ; add to X cursor
MB_COORD_ADDX:
    PUSH  HL
    LD    HL, (MB_GFX_CURX)
    JR    Z, MB_COORD_XADD
    LD    HL, 0
    ; X addition done
MB_COORD_XADD:
    ADD   HL, BC
    LD    (MB_GFX_CURX), HL
    LD    (MB_LINE_X1), HL
    LD    B, H
    LD    C, L
    LD    HL, (MB_GFX_CURY)
    JR    Z, MB_COORD_YADD
    LD    HL, 0
    ; Y addition done
MB_COORD_YADD:
    ADD   HL, DE
    LD    (MB_GFX_CURY), HL
    LD    (MB_LINE_Y1), HL
    EX    DE, HL
    POP   HL
    RET

; --------------------------------------------------
; PRESET Statement
; PRESET (X, Y) [, <color>]
; --------------------------------------------------
MB_PRESET:
    LD    A, 0xff
    JR    MB_PSET_PREP

; --------------------------------------------------
; PSET Statement
; PSET (X, Y) [, <color>]
; --------------------------------------------------
MB_PSET:
    LD    A, (MB_COLOR_FGBG)                        ; =0x7
    ; push color, get coords
MB_PSET_PREP:
    PUSH  AF
    CALL  MB_GETPAR_XY
    POP   AF
    CALL  MB_PSET_PARSECOLOR
    PUSH  HL
    CALL  MB_RANGE_CHKXY
    JR    NC, MB_PSET_RET
    CALL  MB_PXPOS_CALC
    CALL  MB_WRITE_PX
    ; return path
MB_PSET_RET:
    POP   HL
    RET

; --------------------------------------------------
; Get color of point
; POINT(X, Y) function
; --------------------------------------------------
MB_POINT:
    RST   CHRGTR
    PUSH  HL
    CALL  MB_GFX_CURPX_GET
    POP   DE
    PUSH  HL
    PUSH  AF
    LD    HL, (MB_LINE_Y1)
    PUSH  HL
    LD    HL, (MB_LINE_X1)
    PUSH  HL
    LD    HL, (MB_GFX_CURY)
    PUSH  HL
    LD    HL, (MB_GFX_CURX)
    PUSH  HL
    EX    DE, HL
    CALL  MB_GETPAR_XY
    PUSH  HL
    CALL  MB_RANGE_CHKXY
    LD    HL, 0xffff
    JR    NC, MB_POINT_CALC
    CALL  MB_PXPOS_CALC
    CALL  MB_READ_PX
    LD    L, A
    LD    H, 0
    ; compute result, restore cursor
MB_POINT_CALC:
    CALL  MB_MAKINT
    POP   DE
    POP   HL
    LD    (MB_GFX_CURX), HL
    POP   HL
    LD    (MB_GFX_CURY), HL
    POP   HL
    LD    (MB_LINE_X1), HL
    POP   HL
    LD    (MB_LINE_Y1), HL
    POP   AF
    POP   HL
    PUSH  DE
    CALL  MB_GFX_CURPX_SET
    POP   HL
    RET

; --------------------------------------------------
; Get default color
; --------------------------------------------------
MB_LINE_GETCLR:
    LD    A, (MB_COLOR_FGBG)     ;=0x7

; --------------------------------------------------
; Parse optional [,color] arg
; --------------------------------------------------
MB_PSET_PARSECOLOR:
    PUSH  BC
    PUSH  DE
    LD    E, A
    DEC   HL
    RST   CHRGTR
    JR    Z, MB_PSET_VALIDCLR
    RST   SYNCHR
    DB    ','
    CP    ','
    JR    Z, MB_PSET_VALIDCLR
    CALL  MB_GETBYT
    ; validate color, check error
MB_PSET_VALIDCLR:
    LD    A, E
    PUSH  HL
    CALL  MB_GFX_CLRCHK
    JP    C, MB_FCERR
    POP   HL
    POP   DE
    POP   BC
    JP    MB_CHRGT2

; --------------------------------------------------
; LINE: compute DX = X2-X1 with sign
; --------------------------------------------------
MB_LINE_CALC_DX:
    LD    HL, (MB_LINE_X1)
    LD    A, L
    SUB   C
    LD    L, A
    LD    A, H
    SBC   A, B
    LD    H, A
    ; return with sign in C
MB_LINE_SGNRET:
    RET   NC

; --------------------------------------------------
; Negate HL (0 - HL), set C
; --------------------------------------------------
MB_LINE_NEGATE:
    XOR   A
    SUB   L
    LD    L, A
    SBC   A, H
    SUB   L
    LD    H, A
    SCF
    RET

; --------------------------------------------------
; LINE: compute DY = Y2-Y1 with sign
; --------------------------------------------------
MB_LINE_DY_SGN:
    LD    HL, (MB_LINE_Y1)
    LD    A, L
    SUB   E
    LD    L, A
    LD    A, H
    SBC   A, D
    LD    H, A
    JR    MB_LINE_SGNRET

; --------------------------------------------------
; LINE: swap Y1/Y2 endpoints
; --------------------------------------------------
MB_LINE_EXCHG_Y:
    PUSH  HL
    LD    HL, (MB_LINE_Y1)
    EX    DE, HL
    LD    (MB_LINE_Y1), HL
    POP   HL
    RET

; --------------------------------------------------
; LINE: swap Y entry point
; --------------------------------------------------
MB_LINE_SWAPY_ENTRY:
    CALL  MB_LINE_EXCHG_Y

; --------------------------------------------------
; LINE: swap X1/X2 endpoints
; --------------------------------------------------
MB_LINE_SWAPPX:
    PUSH  HL
    PUSH  BC
    LD    HL, (MB_LINE_X1)
    EX    (SP), HL
    LD    (MB_LINE_X1), HL
    POP   BC
    POP   HL
    RET

; --------------------------------------------------
; Draw Line,  rectangle,  filled block
; LINE [(X1, Y1)] - (X2, Y2) [, [<color>] [, B[F]]]
;  Mode 40x24 -> X 0..79,   Y 0..71
;  Mode 80x24 -> X 0..159,  Y 0..71
; --------------------------------------------------
MB_LINEST:
    CALL  MB_COORD_MODECHK
    PUSH  BC
    PUSH  DE
    RST   SYNCHR
    DB    MINUTK
    CALL  MB_GETPAR_XY
    CALL  MB_LINE_GETCLR
    POP   DE
    POP   BC
    JR    Z, MB_LINE_SETENDPTS
    RST   SYNCHR
    INC   L
    RST   SYNCHR
    LD    B, D
    JP    Z, MB_LINE_DRAWRECT
    RST   SYNCHR
    LD    B, (HL)
    PUSH  HL
    CALL  MB_RANGE_CHKXY
    CALL  MB_LINE_SWAPY_ENTRY
    CALL  MB_RANGE_CHKXY
    CALL  MB_LINE_DY_SGN
    CALL  C, MB_LINE_EXCHG_Y
    INC   HL
    PUSH  HL
    CALL  MB_LINE_CALC_DX
    CALL  C, MB_LINE_SWAPPX
    INC   HL
    PUSH  HL
    CALL  MB_PXPOS_CALC
    POP   DE
    POP   BC
    ; LINE (no B): horizontal segment step loop
MB_LINE_SEGLOOP:
    PUSH  DE
    PUSH  BC
    CALL  MB_GFX_CURPX_GET
    PUSH  AF
    PUSH  HL
    EX    DE, HL
    CALL  MB_GFX_HLINE
    POP   HL
    POP   AF
    CALL  MB_GFX_CURPX_SET
    CALL  MB_PXSTEP_RIGHT
    POP   BC
    POP   DE
    DEC   BC
    LD    A, B
    OR    C
    JR    NZ, MB_LINE_SEGLOOP
    POP   HL
    RET

; --------------------------------------------------
; LINE: Init Bresenham, set endpoint = cursor
; --------------------------------------------------
MB_LINE_SETENDPTS:
    PUSH  BC
    PUSH  DE
    PUSH  HL
    CALL  MB_LINE_PREPBRES
    LD    HL, (MB_GFX_CURX)
    LD    (MB_LINE_X1), HL
    LD    HL, (MB_GFX_CURY)
    LD    (MB_LINE_Y1), HL
    POP   HL
    POP   DE
    POP   BC
    RET

; --------------------------------------------------
; LINE with B: draw 4 rectangle sides
; --------------------------------------------------
MB_LINE_DRAWRECT:
    PUSH  HL
    LD    HL, (MB_LINE_Y1)
    PUSH  HL
    PUSH  DE
    EX    DE, HL
    CALL  MB_LINE_SETENDPTS
    POP   HL
    LD    (MB_LINE_Y1), HL
    EX    DE, HL
    CALL  MB_LINE_SETENDPTS
    POP   HL
    LD    (MB_LINE_Y1), HL
    LD    HL, (MB_LINE_X1)
    PUSH  BC
    LD    B, H
    LD    C, L
    CALL  MB_LINE_SETENDPTS
    POP   HL
    LD    (MB_LINE_X1), HL
    LD    B, H
    LD    C, L
    CALL  MB_LINE_SETENDPTS
    POP   HL
    RET

; --------------------------------------------------
; LINE: prepare Bresenham step functions/pointers
; --------------------------------------------------
MB_LINE_PREPBRES:
    CALL  MB_RANGE_CHKXY
    CALL  MB_LINE_SWAPY_ENTRY
    CALL  MB_RANGE_CHKXY
    CALL  MB_LINE_DY_SGN
    CALL  C, MB_LINE_SWAPY_ENTRY
    PUSH  DE
    PUSH  HL
    CALL  MB_LINE_CALC_DX
    EX    DE, HL
    LD    HL, MB_PXSTEP_FWD
    JR    NC, MB_LINE_SETPTRS
    LD    HL, MB_PXSTEP_BWD
    ; store step pointers (case 1)
MB_LINE_SETPTRS:
    EX    (SP), HL
    RST   DCOMPR
    JR    NC, MB_LINE_SETPTRS2
    LD    (MB_LINE_STEPPTR1), HL
    POP   HL
    LD    (MB_LINE_CALLTGT1), HL
    LD    HL, MB_PXSTEP_RIGHT
    LD    (MB_LINE_CALLTGT2), HL
    EX    DE, HL
    JR    MB_LINE_INITLOOP
    ; store step pointers (case 2)
MB_LINE_SETPTRS2:
    EX    (SP), HL
    LD    (MB_LINE_CALLTGT2), HL
    LD    HL, MB_PXSTEP_RIGHT
    LD    (MB_LINE_CALLTGT1), HL
    EX    DE, HL
    LD    (MB_LINE_STEPPTR1), HL
    POP   HL
    ; init loop vars, divide by 2
MB_LINE_INITLOOP:
    POP   DE
    PUSH  HL
    CALL  MB_LINE_NEGATE
    LD    (MB_LINE_STEPPTR2), HL
    CALL  MB_PXPOS_CALC
    POP   DE
    PUSH  DE
    CALL  MB_LINE_DIV2
    POP   BC
    INC   BC
    JR    MB_LINE_BRESLOOP
    ; iteration done check
MB_LINE_DONECHK:
    POP   HL
    LD    A, B
    OR    C
    RET   Z

    ; call step function (gadget 700f)
MB_LINE_STEP1:
    CALL  MB_LINE_CALLGADGET1

    ; main Bresenham loop
MB_LINE_BRESLOOP:
    CALL  MB_WRITE_PX
    DEC   BC
    PUSH  HL
    LD    HL, (MB_LINE_STEPPTR1)
    ADD   HL, DE
    EX    DE, HL
    LD    HL, (MB_LINE_STEPPTR2)
    ADD   HL, DE
    JR    NC, MB_LINE_DONECHK
    EX    DE, HL
    POP   HL
    LD    A, B
    OR    C
    RET   Z
    CALL  MB_LINE_CALLGADGET2
    JR    MB_LINE_STEP1

; --------------------------------------------------
; Divide DE by 2 (line length halving)
; --------------------------------------------------
MB_LINE_DIV2:
    LD    A, D
    OR    A
    RRA
    LD    D, A
    LD    A, E
    RRA
    LD    E, A
    RET


; ===================================================
; 5.0 Features -WHILE/WEND, CALL, CHAIN, WRITE
; ===================================================

; --------------------------------------------------
; WHILE Statement
; --------------------------------------------------
MB_WHILE:
    LD    (MB_ENDFOR), HL                            ; Keep WHILE pointer
    CALL  MB_WNDSCN                                  ; Search for matching WEND
    RST   CHRGTR                                     ; points at character after WEND
    EX    DE, HL
    CALL  MB_FNDWND
    INC   SP
    INC   SP
    JR    NZ, MB_WNOTOL
    ADD   HL, BC
    LD    SP, HL
    LD    (MB_SAVSTK), HL
MB_WNOTOL:
    LD    HL, (MB_CURLIN)                            ; =0xFFFE
    PUSH  HL
    LD    HL, (MB_ENDFOR)
    PUSH  HL
    PUSH  DE
    JR    MB_FNWEND
MB_WEND:
    JP    NZ, MB_SNERR
    EX    DE, HL
    CALL  MB_FNDWND
    JR    NZ, MB_WEERR
    LD    SP, HL
    LD    (MB_SAVSTK), HL
    LD    DE, (MB_CURLIN)                            ; =0xFFFE
    LD    (MB_NXTLIN), DE
    INC   HL
    INC   HL
    LD    E, (HL)
    INC   HL
    LD    D, (HL)
    INC   HL
    LD    A, (HL)
    INC   HL
    LD    H, (HL)
    LD    L, A
    LD    (MB_CURLIN), HL  ;=0xFFFE
    EX    DE, HL
MB_FNWEND:
    CALL  MB_FRMEVL
    PUSH  HL
    CALL  MB_VSIGN
    POP   HL
    JR    Z, MB_FLSWHL
    LD    BC, _WHILE
    LD    B, C
    PUSH  BC
    INC   SP
    JP    MB_NEWSTT
MB_FLSWHL:
    LD    HL, (MB_NXTLIN)
    LD    (MB_CURLIN), HL  ;=0xFFFE
    POP   HL
    POP   BC
    POP   BC
    JP    MB_NEWSTT

; --------------------------------------------------
MB_FNDWND:
    LD    HL, 0x4
    ADD   HL, SP
MB_FNDWN2:
    LD    A, (HL)
    INC   HL
    LD    BC, _FOR
    CP    C
    JR    NZ, MB_FNDWN3
    LD    BC, WHILSIZ
    ADD   HL, BC
    JR    MB_FNDWN2
MB_FNDWN3:
    LD    BC, _WHILE
    CP    C
    RET   NZ
    PUSH  HL
    LD    A, (HL)
    INC   HL
    LD    H, (HL)
    LD    L, A
    RST   DCOMPR
    POP   HL
    LD    BC, 0x6
    RET   Z
    ADD   HL, BC
    JR    MB_FNDWN2
MB_WEERR:
    LD    DE, ERRWE                                  ; While wo end
    JP    MB_ERROR

; --------------------------------------------------
; Statement To call an assembly language subroutine
; CALL <variable name>[(<argument list>)]
; --------------------------------------------------
MB_CALLS:
    LD    A, 0x80                                    ; Flag PTRGET not to allow arrays
    LD    (MB_SUBFLG), A
    CALL  MB_PTRGET                                  ; get address parameter
    PUSH  HL
    EX    DE, HL
    RST   GETYPTR
    CALL  MB_VMOVFM
    CALL  MB_FRQINT                                  ; address to integer
    LD    (MB_TEMPA), HL
    LD    C, MAXPRM
    CALL  MB_GETSTK                                  ; get 64b stack space
    POP   DE
    LD    HL, -64                                    ; -2*MAXPRM
    ADD   HL, SP
    LD    SP, HL
    EX    DE, HL
    LD    C, MAXPRM
    DEC   HL
    RST   CHRGTR
    LD    (MB_TEMP), HL
    JR    Z, MB_CALLST
    RST   SYNCHR
    DB    '('
MB_GETPAR:
    PUSH  BC
    PUSH  DE
    PUSH  HL
    LD    HL, (MB_ARYTAB)
    EX    (SP), HL
    CALL  MB_PTRGET
    EX    (SP), HL
    PUSH  DE
    EX    DE, HL
    LD    HL, (MB_ARYTAB)
    RST   DCOMPR
    JP    NZ, MB_FCERR                               ; llegal function call
    POP   DE
    POP   HL
    EX    (SP), HL
    LD    (HL), E
    INC   HL
    LD    (HL), D
    INC   HL
    EX    (SP), HL
    POP   DE
    POP   BC
    LD    A, (HL)
    CP    ','
    JR    NZ, MB_ENDPAR
    DEC   C
    RST   CHRGTR
    JR    MB_GETPAR
MB_ENDPAR:
    RST   SYNCHR
    DB    ')'
    LD    (MB_TEMP), HL
    LD    A, MAXPRM+1
    SUB   C
    POP   HL
    DEC   A
    JR    Z, MB_CALLST
    POP   DE
    DEC   A
    JR    Z, MB_CALLST
    POP   BC
    DEC   A
    JR    Z, MB_CALLST
    PUSH  BC
    PUSH  HL
    LD    HL, 2                                      ; Point to rest of parm list
    ADD   HL, SP
    LD    B, H
    LD    C, L
    POP   HL
MB_CALLST:
    PUSH  HL
    LD    HL, MB_CALLRT
    EX    (SP), HL
    PUSH  HL
    LD    HL, (MB_TEMPA)
    EX    (SP), HL
    RET

; --------------------------------------------------
MB_CALLRT:
    LD    HL, (MB_SAVSTK)
    LD    SP, HL
    LD    HL, (MB_TEMP)
    JP    MB_NEWSTT

; --------------------------------------------------
; CHAIN Statement
; CHAIN [MERGE] <file name> [, [<line number>]
;       [, ALL] [, DELETE <range>]]
; --------------------------------------------------
MB_CHAIN:
    XOR   A
    LD    (MB_MRGFLG), A
    LD    (MB_MDLFLG), A
    LD    A, (MB_OPTFLAG_1)
    LD    (MB_OPTFLAG_2), A
    LD    A, (MB_OPTVAL)
    LD    (MB_OPTVAL_2), A
    LD    A, (HL)
    LD    DE, _MERGE
    CP    E
    JR    NZ, MB_NTCHNM
    LD    (MB_MRGFLG), A
    INC   HL
MB_NTCHNM:
    DEC   HL                                         ; Rescan filename
    RST   CHRGTR
    CALL  MB_NAMFIL
    LD    A, D
    LD    (MB_CHAIN_FP), A
    PUSH  HL
    LD    HL, 0
    LD    (MB_CHNLIN), HL
    POP   HL
    DEC   HL
    RST   CHRGTR
    JR    Z, MB_NTCHAL
    RST   SYNCHR
    DB    ','
    CP    ','
    JR    Z, MB_NTLINF
    CALL  MB_FRMEVL
    PUSH  HL
    CALL  MB_FRQINT
    LD    (MB_CHNLIN), HL
    POP   HL
    DEC   HL
    RST   CHRGTR
    JR    Z, MB_NTCHAL
MB_NTLINF:
    RST   SYNCHR
    DB    ','
    LD    DE, _DELETE                                ; chk for DELETE option
    CP    E
    JR    Z, MB_CHMWDL
    RST   SYNCHR
    DB    'A'
    RST   SYNCHR
    DB    'L'
    RST   SYNCHR
    DB    'L'
    JP    Z, MB_DNCMDA
    RST   SYNCHR
    DB    ','
    CP    E
    JP    NZ, MB_SNERR
    OR    A
MB_CHMWDL:
    PUSH  AF  ;save ALL flag
    LD    (MB_MDLFLG), A
    RST   CHRGTR
    CALL  MB_SCNLIN
    PUSH  BC
    CALL  MB_DEPTR
    POP   BC
    POP   DE
    PUSH  BC
    LD    H, B
    LD    L, C
    LD    (MB_CMSPTR), HL
    CALL  MB_FNDLIN
    JR    NC, MB_FCERRG1
    LD    D, H
    LD    E, L
    LD    (MB_CMEPTR), HL
    POP   HL
    RST   DCOMPR                                     ; Make sure the start comes before the end
MB_FCERRG1:
    JP    NC, MB_FCERR                               ; if so,  Illegal function call error
    POP   AF
    JP    NZ, MB_DNCMDA
MB_NTCHAL:
    LD    HL, (MB_CURLIN)                            ; =0xFFFE
    PUSH  HL
    LD    HL, (MB_TXTTAB)                            ; =0x733F
    DEC   HL
MB_CLPSC1:
    INC   HL                                         ; Look at first char of next line
    LD    A, (HL)
    INC   HL
    OR    (HL)
    JP    Z, MB_CLPFIN                               ; Jump if program end
    INC   HL
    LD    E, (HL)
    INC   HL
    LD    D, (HL)
    LD    (MB_CURLIN), DE                            ; =0xFFFE
MB_CSTSCN:
    RST   CHRGTR

MB_AFTCBRA:
    OR    A
    JR    Z, MB_CLPSC1
    CP    ':'
    JR    Z, MB_CSTSCN
    LD    DE, _COMMON
    CP    E
    JR    Z, MB_DOCOMM
    RST   CHRGTR
    CALL  MB_DATAS
    DEC   HL
    JR    MB_CSTSCN
MB_DOCOMM:
    RST   CHRGTR
    JR    Z, MB_AFTCBRA
MB_NXTCOM:
    PUSH  HL
    LD    A, 1
    LD    (MB_SUBFLG), A
    CALL  MB_NVRFIL
    JR    Z, MB_FNDAAY
    LD    A, B
    OR    0x80
    LD    B, A
    XOR   A
    CALL  MB_ERSFIN
    LD    A, 0
    LD    (MB_SUBFLG), A
    JR    NZ, MB_NTFN2T
    LD    A, (HL)
    CP    '('
    JR    NZ, MB_SCNSMP
    POP   AF
    JR    MB_COMADY
MB_NTFN2T:
    LD    A, (HL)
    CP    '('
    POP   DE
    JR    Z, MB_FCERRR
    PUSH  DE

    ; Rescan variable name for start
MB_SCNSMP:
    POP   HL
    CALL  MB_NVRFIL
    LD    A, D                                       ; If var not found,  DE=0
    OR    E
    JR    NZ, MB_COMFNS
    LD    A, B
    OR    0x80
    LD    B, A
    LD    DE, MB_COMPT2
    PUSH  DE
    LD    DE, MB_FILRET
    PUSH  DE
    LD    A, (MB_VALTYP)
    LD    D, A
    JP    MB_NOARYS
MB_COMPT2:
    LD    A, D
    OR    E
    JR    Z, MB_FCERRR
MB_COMFNS:
    PUSH  HL
    LD    B, D
    LD    C, E
    LD    HL, MB_BCKUCM
    PUSH  HL

; --------------------------------------------------
MB_CBAKBL:
    DEC   BC                                         ; Point at first char of rest
MB_LPBKNC:
    LD    A, (BC)
    DEC   BC
    OR    A
    JP    M, MB_LPBKNC
    LD    A, (BC)
    OR    0x80
    LD    (BC), A
    RET

    ; Array found,  clear SUBFLG
MB_FNDAAY:
    LD    (MB_SUBFLG), A
    LD    A, (HL)
    CP    '('
    JR    NZ, MB_SCNSMP
    EX    (SP), HL
    DEC   BC
    DEC   BC
    CALL  MB_CBAKBL

; --------------------------------------------------
MB_BCKUCM:
    POP   HL
MB_FCERRR:
    DEC   HL
    RST   CHRGTR
    JP    Z, MB_AFTCBRA
    CP    '('
    JR    NZ, MB_CHKCST

; --------------------------------------------------
MB_COMADY:
    PUSH  HL                                        ; Fetch char after parent
    RST   CHRGTR
    CP    ')'
    JR    Z, .CBRA
    POP   HL
    CALL  MB_EVAL
    RST   GETYPTR
    JP    Z, MB_FCERR
    JR    MB_CKCLBR

.CBRA:
    POP   DE
MB_CKCLBR:
    RST   SYNCHR                                    ; Right paren should follow
    DB    ')'
    JP    Z, MB_AFTCBRA
MB_CHKCST:
    RST   SYNCHR
    DB    ','
    JP    MB_NXTCOM                                 ; Get next COMMON variable

; Step 3: Squeeze
MB_CLPFIN:
    POP   HL
    LD    (MB_CURLIN), HL                           ; =0xFFFE
    LD    DE, (MB_ARYTAB)                           ; End of simple var squeeze
    LD    HL, (MB_VARTAB)                           ; Start of simple var squeeze
MB_CLPSLP:
    RST   DCOMPR                                    ; Are we done?
    JR    Z, MB_DNCMDS                              ; Yes done,  with simps
    PUSH  HL
    LD    C, (HL)                                   ; Get VALTYP
    INC   HL
    INC   HL
    LD    A, (HL)                                   ; Get COMMON bit
    OR    A                                         ; Set minus if COMMON
    PUSH  AF
    AND   0x7f                                      ; Clear COMMON bit
    LD    (HL), A                                   ; Set it back
    INC   HL
    CALL  MB_IADAHL                                 ; Skip over rest of var name
    LD    B, 0
    ADD   HL, BC
    POP   AF
    POP   BC
    JP    M, MB_CLPSLP
    PUSH  BC
    CALL  MB_VARDLS
    LD    HL, (MB_ARYTAB)
    ADD   HL, DE
    LD    (MB_ARYTAB), HL
    EX    DE, HL
    POP   HL
    JR    MB_CLPSLP

; --------------------------------------------------
MB_VARDLS:
    EX    DE, HL                                     ; Point to where var ends
    LD    HL, (MB_STREND)
MB_DLSVLP:
    RST   DCOMPR
    LD    A, (DE)
    LD    (BC), A
    INC   DE
    INC   BC
    JR    NZ, MB_DLSVLP
    LD    A, C
    SUB   L
    LD    E, A
    LD    A, B
    SBC   A, H
    LD    D, A
    DEC   DE
    DEC   BC
    LD    H, B
    LD    L, C
    LD    (MB_STREND), HL
    RET

; --------------------------------------------------
MB_DNCMDS:
    LD    DE, (MB_STREND)                            ; Limit of array search
MB_CLPAKP:
    RST   DCOMPR                                     ; Done?
    JR    Z, MB_DNCMDA
    PUSH  HL
    INC   HL
    INC   HL
    LD    A, (HL)
    OR    A
    PUSH  AF
    AND   0x7f
    LD    (HL), A
    INC   HL
    CALL  MB_IADAHL
    LD    C, (HL)
    INC   HL
    LD    B, (HL)
    INC   HL
    ADD   HL, BC
    POP   AF
    POP   BC
    JP    M, MB_CLPAKP
    PUSH  BC
    CALL  MB_VARDLS
    EX    DE, HL
    POP   HL
    JR    MB_CLPAKP

; --------------------------------------------------
; Step 4 - Copy literals into string space
; --------------------------------------------------
MB_DNCMDA:
    LD    HL, (MB_VARTAB)
MB_CSVAR:
    LD    DE, (MB_ARYTAB)
    RST   DCOMPR
    JR    Z, MB_CAYVAR
    CALL  MB_SKPNX_STR
    JR    NZ, MB_CSKPVA
    CALL  MB_CDVARS
    XOR   A
MB_CSKPVA:
    LD    E, A
    LD    D, 0
    ADD   HL, DE
    JR    MB_CSVAR
MB_CAYVA2:
    POP   BC
MB_CAYVAR:
    LD    DE, (MB_STREND)
    RST   DCOMPR
    JR    Z, MB_DNCCLS
    CALL  MB_SKPNX_STR
    PUSH  AF
    LD    C, (HL)
    INC   HL
    LD    B, (HL)
    INC   HL
    POP   AF
    PUSH  HL
    ADD   HL, BC
    CP    TSTR
    JR    NZ, MB_CAYVA2
    LD    (MB_TEMP3), HL
    POP   HL
    LD    C, (HL)
    LD    B, 0
    ADD   HL, BC
    ADD   HL, BC
    INC   HL
MB_ARYSTR:
    LD    DE, (MB_TEMP3)
    RST   DCOMPR
    JR    Z, MB_CAYVAR
    LD    BC, MB_ARYSTR
    PUSH  BC

; --------------------------------------------------
MB_CDVARS:
    LD    A, (HL)
    INC   HL
    LD    E, (HL)
    INC   HL
    LD    D, (HL)
    INC   HL
    OR    A
    RET   Z
    PUSH  HL
    LD    HL, (MB_VARTAB)
    RST   DCOMPR
    POP   HL
    RET   C
    PUSH  HL
    LD    HL, (MB_TXTTAB)  ;=0x733F
    RST   DCOMPR
    POP   HL
    RET   NC
    PUSH  HL
    DEC   HL
    DEC   HL
    DEC   HL
    PUSH  HL
    CALL  MB_STRCPY
    POP   HL
    LD    B, 3
    CALL  MB_MOVE_N
    POP   HL
    RET

; Step 5 - Move stuff up into string space!
MB_DNCCLS:
    CALL  MB_GARBA2
    LD    HL, (MB_STREND)
    LD    B, H
    LD    C, L
    LD    DE, (MB_VARTAB)
    LD    HL, (MB_ARYTAB)
    OR    A
    SBC   HL, DE
    LD    (MB_TEMP9), HL
    LD    HL, (MB_FRETOP)
    LD    (MB_SAVFRE), HL
    CALL  MB_BLTUC
    LD    H, B
    LD    L, C
    DEC   HL
    LD    (MB_FRETOP), HL
    LD    A, (MB_MDLFLG)
    OR    A
    JR    Z, MB_NTMDLT
    LD    HL, (MB_CMSPTR)
    LD    B, H
    LD    C, L
    LD    HL, (MB_CMEPTR)
    CALL  MB_DEL
    LD    (MB_ARYTAB), HL
    LD    (MB_STREND), HL
    CALL  MB_LINKER

; Step 6 - load new program
MB_NTMDLT:
    LD    A, 1       ;Set CHAIN flag
    LD    (MB_CHNFLG), A
    LD    A, (MB_CHAIN_FP)
    LD    D, A
    LD    A, (MB_MRGFLG)
    SUB   1
    LD    HL, (MB_TXTTAB)  ;=0x733F
    DEC   HL
    SBC   A, A
    JP    MB_CHNENT

; Step 7 - Move stuff back down
MB_CHNRET:
    LD    A, (MB_OPTVAL_2)
    LD    (MB_OPTVAL), A
    LD    A, (MB_OPTFLAG_2)
    LD    (MB_OPTFLAG_1), A

    ; Clear CHAIN,  MERGE flags
    XOR   A
    LD    (MB_CHNFLG), A
    LD    (MB_MRGFLG), A
    LD    HL, (MB_VARTAB)
    LD    B, H
    LD    C, L
    LD    HL, (MB_TEMP9)
    ADD   HL, BC
    LD    (MB_ARYTAB), HL
    LD    HL, (MB_FRETOP)
    INC   HL
    EX    DE, HL
    LD    HL, (MB_SAVFRE)
    LD    (MB_FRETOP), HL
MB_MVBKVR:
    RST   DCOMPR
    LD    A, (DE)
    LD    (BC), A
    INC   DE
    INC   BC
    JR    NZ, MB_MVBKVR
    DEC   BC
    LD    H, B
    LD    L, C
    LD    (MB_STREND), HL
    XOR   A
    CALL  MB_RESTORE
    LD    DE, (MB_CHNLIN)
    LD    HL, (MB_TXTTAB)  ;=0x733F
    DEC   HL
    LD    A, D
    OR    E
    JP    Z, MB_NEWSTT
    CALL  MB_FNDLIN
    JP    NC, MB_USERR
    DEC   BC
    LD    H, B
    LD    L, C
    JP    MB_NEWSTT

; --------------------------------------------------
MB_SKPNX_STR:
    LD    A, (HL)
    INC   HL
    INC   HL
    INC   HL
    PUSH  AF
    CALL  MB_IADAHL
    POP   AF
    CP    TSTR
    RET
MB_COMMON:
    JP    MB_DATAS  ;ToDo: check usage

; --------------------------------------------------
; WRITE [<list of expressions>]
; WRITE# <file number>, <list of expressions>
; --------------------------------------------------
MB_WRITE:
    LD    C, MD.SQO  ;Sequential output
    CALL  MB_FILGET
    DEC   HL
    RST   CHRGTR
    JR    Z, MB_WRTFIN
MB_WRTMLP:
    CALL  MB_FRMEVL
    PUSH  HL
    RST   GETYPTR
    JR    Z, MB_WRTSTR
    CALL  MB_FOUT
    CALL  MB_STRLIT
    LD    HL, (MB_DACC+2)
    INC   HL
    LD    E, (HL)
    INC   HL
    LD    D, (HL)
    LD    A, (DE)
    CP    ASCII_SP
    JR    NZ, MB_WRTNEG
    INC   DE
    LD    (HL), D
    DEC   HL
    LD    (HL), E
    DEC   HL
    DEC   (HL)
MB_WRTNEG:
    CALL  MB_STRPRT
MB_NXTWRV:
    POP   HL
    DEC   HL
    RST   CHRGTR
    JR    Z, MB_WRTFIN
    CP    ';'
    JR    Z, MB_WASEMI
    RST   SYNCHR
    DB    ','
    DEC   HL
MB_WASEMI:
    RST   CHRGTR
    LD    A, ','
    RST   OUTDO
    JR    MB_WRTMLP
MB_WRTSTR:
    LD    A, '"'
    RST   OUTDO
    CALL  MB_STRPRT
    LD    A, '"'
    RST   OUTDO
    JR    MB_NXTWRV
MB_WRTFIN:
    CALL  MB_CRDO
    JP    MB_FINPRT
; ==================================================
; End of 5.0 Features WHILE/WEND, CALL, CHAIN, WRITE
; ==================================================

; --------------------------------------------------
; Allocate a block for current file (scan DAC)
; --------------------------------------------------
MB_ALLOC_BLK:
    LD    HL, (MB_HDR_PTR)
    LD    B, (HL)
    INC   HL
    INC   HL
    LD    A, (HL)
    OR    A
    JP    Z, MB_DFLERR
    INC   HL
    EX    DE, HL
    LD    A, B
    ADD   A, A
    ADD   A, A
    LD    C, A
    LD    B, A
MB_ALLOC_SCANENTRY:
    LD    H, 0
    LD    L, B
    ADD   HL, DE
    LD    A, (HL)
    INC   A
    JR    Z, MB_ALLOC_FOUND
MB_ALLOC_NEXTENT:
    LD    A, C
    OR    A
    JR    Z, MB_ALLOC_LIMIT
    DEC   C
    LD    H, 0
    LD    L, C
    ADD   HL, DE
    LD    A, (HL)
    INC   A
    JR    Z, MB_ALLOC_MARKUSED
MB_ALLOC_LIMIT:
    LD    A, B
    CP    0x9f
    JR    NC, MB_ALLOC_NEXTENT
    INC   B
    JR    MB_ALLOC_SCANENTRY
MB_ALLOC_FOUND:
    LD    C, B
MB_ALLOC_MARKUSED:
    LD    (HL), 0xc0
    EX    DE, HL
    DEC   HL
    DEC   (HL)
    INC   HL
    EX    DE, HL
    LD    HL, (MB_PTRFIL)
    INC   HL
    LD    A, (HL)
    OR    A
    INC   A
    LD    (HL), C
    JR    Z, MB_ALLOC_FINISH
    DEC   A
    LD    (HL), A
    PUSH  BC
    CALL  MB_LOC_COUNTBLKS
    POP   BC
    LD    (HL), C
MB_ALLOC_FINISH:
    LD    B, 0x8
    DB    JP_C_nn

; --------------------------------------------------
; Truncate file at current position
; --------------------------------------------------
MB_TRUNC_MARK:
    LD    B, 1
    LD    HL, (MB_HDR_PTR)
    DEC   HL
    DEC   HL
    LD    (HL), 0xff
    INC   HL
    INC   HL
    INC   HL
    LD    A, (HL)
    OR    A
    JR    NZ, MB_TRUNC_CHKERR
    LD    A, 0x10
    LD    (HL), A
MB_TRUNC_CHKERR:
    SUB   B
    JP    C, MB_FILEOP_ERRSAVE
    LD    (HL), A
    RET

; --------------------------------------------------
; Free all blocks of a file
; --------------------------------------------------
MB_FREE_ALLBLKS:
    LD    HL, (MB_PTRFIL)
    INC   HL
    LD    A, (HL)
    CALL  MB_DAC_GETADDR
    LD    B, 0
MB_FREE_SCANLOT:
    LD    L, A
    LD    H, 0
    ADD   HL, DE
    LD    A, (HL)
    INC   B
    LD    (HL), 0xff
    CP    0xc0
    JR    C, MB_FREE_SCANLOT
    DEC   DE
    LD    A, (DE)
    ADD   A, B
    LD    (DE), A
    DEC   DE
    LD    (DE), A
    EX    DE, HL
    DEC   HL
    DEC   HL
    DEC   HL
    LD    (HL), 0xff
    EX    DE, HL
    RET

; --------------------------------------------------
; Verify all blocks of a file (read + checksum)
; --------------------------------------------------
MB_VERIFY_FILE:
    LD    E, 0
    LD    D, 0xe
MB_VERIFY_CHECKBLK:
    CALL  MB_SECTOR_STATUS
    JR    NZ, MB_VERIFY_NEXTBLK
    INC   E
MB_VERIFY_NEXTBLK:
    INC   D
    LD    A, D
    SUB   0x11
    JR    NZ, MB_VERIFY_CHECKBLK
    OR    E
    JP    Z, MB_BATERR
    LD    A, 0x3
    SUB   E
    CALL  NZ, MB_BATERR_PRINT
    LD    HL, (MB_HDR_PTR)
    LD    (HL), 0x12
    DEC   HL
    DEC   HL
    LD    (HL), 0
    INC   HL
    INC   HL
    INC   HL
    INC   HL
    PUSH  HL
    INC   HL
    LD    B, 0xa0
    XOR   A
MB_VERIFY_SUMCHK:
    LD    C, (HL)
    INC   HL
    INC   C
    JR    NZ, MB_VERIFY_SUMDJNZ
    INC   A
MB_VERIFY_SUMDJNZ:
    DJNZ  MB_VERIFY_SUMCHK
    PUSH  AF
    LD    BC, MB_COUT_NLCHK
    LD    HL, (MB_PTRFIL1)
    LD    A, 1
    OR    A
    CALL  MB_SECT_EXEC
    DEC   A
    JR    C, MB_VERIFY_FIXMODE
    LD    A, (HL)
MB_VERIFY_FIXMODE:
    LD    HL, (MB_HDR_PTR)
    DEC   HL
    AND   0x70
    LD    (HL), A
    POP   AF
    POP   HL
    LD    (HL), A
    RET

; --------------------------------------------------
MS_COA_BAD:
    DB    " copies of allocation bad on drive ", 0

; --------------------------------------------------
; Prepare for write (drive check, verify)
; --------------------------------------------------
MB_PREP_WRITE:
    LD    C, A
    CALL  MB_CURDRV_SET
    RET   Z
    DEC   HL
    LD    (HL), 0x40
    DEC   HL
    LD    A, (HL)
    OR    A
    JR    Z, MB_CURFIL_PREP
    CALL  MB_SECOPER_VERIFY
    JP    C, MB_BATERR

; --------------------------------------------------
MB_CURFIL_PREP:
    LD    HL, (MB_HDR_PTR)
    INC   HL
    LD    (HL), 0
    DEC   HL
    LD    (HL), 0xff
    DEC   HL
    LD    (HL), 0
    DEC   HL
    LD    (HL), 0
    RET

; --------------------------------------------------
; File open by index: prepare (verify if needed)
; --------------------------------------------------
MB_FILOPEN_PREP:
    PUSH  HL
    PUSH  DE
    PUSH  BC
    LD    B, A
    LD    A, (MB_CURDRV)
    PUSH  AF
    LD    A, B
    OR    A
    JP    M, MB_FILOPEN_IDXDONE
    LD    A, (MB_CURDRV)
    CALL  MB_SCAN_IDX
    CALL  NZ, MB_PREP_WRITE
MB_FILOPEN_IDXDONE:
    POP   AF
    OR    A
    CALL  P, MB_CURDRV_SET
    JR    MB_FILEOP_COMMONRET

; --------------------------------------------------
; Scan file index table by drive number
; --------------------------------------------------
MB_SCAN_IDX:
    LD    C, A
    LD    A, (MB_MAXFIL)
    LD    B, A
MB_SCAN_SLOTSCAN:
    LD    A, B
    CALL  MB_FILIDX
    JR    Z, MB_SCAN_NEXTSLOT
    INC   HL
    INC   HL
    INC   HL
    INC   HL
    LD    A, (HL)
    CP    C
    RET   Z
MB_SCAN_NEXTSLOT:
    DEC   B
    JP    P, MB_SCAN_SLOTSCAN
    LD    A, C
    RET

; --------------------------------------------------
; Optional file verify (if flag set)
; --------------------------------------------------
MB_VERIFY_OPT:
    PUSH  HL
    PUSH  DE
    PUSH  BC
    OR    A
    CALL  P, MB_VERIFY_FILE

; --------------------------------------------------
; File op common return (JP ISCONO_RET)
; --------------------------------------------------
MB_FILEOP_COMMONRET:
    JP    MB_FILEOP_ISCONO_RET

; --------------------------------------------------
; Find record block number for position
; --------------------------------------------------
MB_SCAN_FINDRECB:
    CALL  MB_DIV_8
    LD    HL, (MB_PTRFIL)
    INC   HL
    LD    A, (HL)
MB_SCAN_BLKSCAN:
    CALL  MB_DAC_GETADDR
    LD    L, A
    LD    H, 0
    ADD   HL, DE
    DEC   B
    LD    E, A
    JR    Z, MB_SCAN_VALCMP
    LD    A, (HL)
    CP    0xc0
    JR    C, MB_SCAN_BLKSCAN
    LD    A, E
    SCF
    RET

; --------------------------------------------------
MB_SCAN_VALCMP:
    LD    A, (HL)
    SUB   0xc0
    CCF
    JR    NC, MB_SCAN_RETVAL
    CP    C
MB_SCAN_RETVAL:
    LD    A, E
    RET

; --------------------------------------------------
; Divide DE by 8 (C = quotient, B = remainder)
; --------------------------------------------------
MB_DIV_8:
    LD    B, 0
    LD    C, 0x8
MB_DIV8_ITER:
    LD    A, E
    INC   B
    SUB   C
    LD    E, A
    LD    A, D
    SBC   A, 0
    LD    D, A
    JR    NC, MB_DIV8_ITER
    LD    A, E
    ADD   A, C
    JR    NZ, MB_DIV8_ADJREM
    LD    A, C
    DEC   B
MB_DIV8_ADJREM:
    LD    C, A
    DEC   C
    RET

; --------------------------------------------------
MB_FILEOP_LOC:
    INC   HL
    LD    A, (HL)
    INC   HL
    LD    B, (HL)
    CALL  MB_DAC_GETADDR
    LD    C, 0
    JR    MB_LOC_SLOTCHK

; --------------------------------------------------
MB_LOC_SLOTSCAN:
    LD    L, A
    LD    H, 0
    ADD   HL, DE
    LD    A, (HL)
    INC   C
    JP    Z, MB_BATERR

MB_LOC_SLOTCHK:
    CP    B
    JR    NZ, MB_LOC_SLOTSCAN
    RET

; --------------------------------------------------
; Get DAC block address (CURFIL+3 → DE)
; --------------------------------------------------
MB_DAC_GETADDR:
    LD    HL, (MB_HDR_PTR)
    INC   HL
    INC   HL
    INC   HL
    EX    DE, HL
    RET

; --------------------------------------------------
MB_FILEOP_EOF:
    INC   HL
    INC   HL
    LD    E, (HL)
    INC   HL
    LD    B, (HL)
    CALL  MB_DAC_GETADDR
    LD    H, 0
    ADD   HL, DE
    LD    A, (HL)
    SUB   0xc0
    CCF
    RET   NC
    CP    B
    RET   NZ
    SCF
    RET

; --------------------------------------------------
; Count blocks up to position (FINDNEXT)
; --------------------------------------------------
MB_LOC_COUNTBLKS:
    LD    HL, (MB_PTRFIL)
MB_FILEOP_LOF:
    INC   HL
    LD    A, (HL)
    CALL  MB_DAC_GETADDR
    LD    C, 0
    ; block scan loop
.BLKLP:
    LD    L, A
    LD    B, A
    LD    H, 0
    ADD   HL, DE
    INC   C
    JP    Z, MB_BATERR
    LD    A, (HL)
    CP    0xc0
    JR    C, .BLKLP
    CP    0xc9
    JP    NC, MB_BATERR
    DEC   C
    SUB   0xc0
    RET

; --------------------------------------------------
; Return DE = C*8
; --------------------------------------------------
MB_CMUL8:
    PUSH  BC
    LD    L, C
    LD    H, 0
    ADD   HL, HL
    ADD   HL, HL
    ADD   HL, HL
    EX    DE, HL
    POP   BC
    RET

; --------------------------------------------------
MB_FILOPN:
    XOR   A
    LD    (MB_CHAIN_FP), A
    LD    D, 1
    ; read directory sector (16 entries)
MB_FILOPN_READSECT:
    CALL  MB_DIR_READSECT
    LD    E, 0x10
    ; per-entry scan loop
MB_FILOPN_ENTLP:
    LD    A, (HL)
    INC   A
    JR    Z, MB_FILOPN_ISFOUND
    DEC   A
    JR    NZ, MB_FILOPN_CMPNAME
    LD    (MB_DIR_ENTPTR), HL
    EX    DE, HL
    LD    (MB_CHAIN_FP), HL
    EX    DE, HL
    ; compare 9-byte filename
MB_FILOPN_CMPNAME:
    LD    BC, MB_FN_FIRST
    PUSH  DE
    PUSH  HL
    LD    E, 9
    ; name character compare loop
MB_FILOPN_CHRLP:
    LD    A, (BC)
    CP    (HL)
    JR    NZ, MB_FILOPN_NEXTENT
    INC   HL
    INC   BC
    DEC   E
    JR    NZ, MB_FILOPN_CHRLP
    POP   DE
    POP   HL
    LD    (MB_CHAIN_FP), HL
    EX    DE, HL
    LD    (MB_DIR_ENTPTR), HL
    RET

    ; next entry / next drive
MB_FILOPN_NEXTENT:
    POP   HL
    LD    DE, 0x10
    ADD   HL, DE
    POP   DE
    DEC   E
    JR    NZ, MB_FILOPN_ENTLP
    INC   D
    LD    A, D
    CP    ASCII_CR
    JR    NZ, MB_FILOPN_READSECT
    ; entry found, check drive
MB_FILOPN_ISFOUND:
    LD    A, (MB_CHAIN_FP)
    OR    A
    JR    Z, MB_FILOPN_MISSING
    LD    HL, (MB_DIR_ENTPTR)
    LD    A, (MB_DIR_DRVST)
    CP    D
    JR    Z, MB_FILOPN_DRVRET
    ADD   A, 0x80
    LD    (MB_DIR_DRVST), A
    ; drive match return
MB_FILOPN_DRVRET:
    OR    A
    RET

    ; not found (set missing marker)
MB_FILOPN_MISSING:
    LD    A, E
    ADD   A, 0x80
    LD    (MB_CHAIN_FP), A
    RET

; --------------------------------------------------
MB_NAMFIL:
    CALL  MB_FRMEVL

MB_NAMFIL1:
    PUSH  HL
    CALL  MB_FRESTR
    LD    A, (HL)
    OR    A
    JR    Z, MB_ERNMF
    INC   HL
    LD    E, (HL)
    INC   HL
    LD    H, (HL)
    LD    L, E
    LD    E, A
    PUSH  HL
    LD    D, A
    CALL  MB_GETNXA
    JR    Z, MB_DNAMF
    LD    B, A
    CALL  MB_GETNXA
    JR    Z, MB_DNAMF
    CP    ':'
    LD    A, B
    JR    Z, MB_SKDRVSEP
MB_DNAMF:
    LD    A, '1'
    POP   HL
    PUSH  HL
    LD    E, D
    SCF
MB_SKDRVSEP:
    EX    (SP), HL
    POP   HL
    PUSH  AF
    LD    BC, MB_FN_FIRST
    LD    D, 9
    INC   E
MB_DONXTFL:
    DEC   E
    JR    Z, MB_FIRMNSP
    LD    A, (HL)
    CP    ASCII_SP
    JR    C, MB_ERNMF
    CP    '.'
    JR    Z, MB_HDL_FNEXT
    LD    (BC), A
    INC   BC
    INC   HL
    DEC   D
    JR    NZ, MB_DONXTFL
MB_EVFN_END:
    POP   AF
    PUSH  AF
    SUB   '1'
    JP    C, MB_BDNERR
    CP    8
    JP    NC, MB_BDNERR
    LD    D, A
    LD    A, (MB_FN_FIRST)
    INC   A
    JR    Z, MB_ERNMF
    POP   AF
    POP   HL
    RET

; --------------------------------------------------
MB_ERNMF:
    JP    MB_BFLERR

; --------------------------------------------------
MB_EVEXT_OK:
    INC   HL
    JR    MB_DONXTFL

; --------------------------------------------------
MB_HDL_FNEXT:
    LD    A, D
    CP    ASCII_TAB
    JP    Z, MB_ERNMF
    CP    3
    JP    C, MB_ERNMF
    JR    Z, MB_EVEXT_OK
    LD    A, ASCII_SP
    LD    (BC), A
    INC   BC
    DEC   D
    JR    MB_HDL_FNEXT

; --------------------------------------------------
MB_FIRMNSP:
    LD    A, ASCII_SP
    LD    (BC), A
    INC   BC
    DEC   D
    JR    NZ, MB_FIRMNSP
    JR    MB_EVFN_END

; --------------------------------------------------
MB_GETNXA:
    LD    A, (HL)
    INC   HL
    DEC   E
    RET

; ===================================================
; Common routines for Disk BASIC
; ===================================================

; --------------------------------------------------
; Get i/o channel control block (DAC)
;  Inp: (DAC) = i/o channel number
; --------------------------------------------------
MB_FILFRM:
    CALL  MB_CONINT                                  ; Get file number into A

; --------------------------------------------------
; Get i/o channel control block
;  Inp: A = i/o channel number
; --------------------------------------------------
MB_FILIDX:
    LD    L, A                                       ; Get file number into E
    LD    A, (MB_MAXFIL)
    CP    L
    JP    C, MB_BFNERR
    LD    H, 0
    ADD   HL, HL
    EX    DE, HL
    LD    HL, (MB_FILTAB)                            ; Pointer to a file descritor table
    ADD   HL, DE
    LD    A, (HL)
    INC   HL
    LD    H, (HL)
    LD    L, A
    LD    A, (MB_NLONLY)
    INC   A
    RET   Z
    LD    A, (HL)
    OR    A
    RET   Z
    PUSH  HL
    LD    DE, 0x4
    ADD   HL, DE
    LD    A, (HL)
    CALL  MB_CURDRV_SET
    POP   HL
    LD    A, (HL)
    OR    A
    RET

; --------------------------------------------------
; Drive number range check (1..8)
; --------------------------------------------------
MB_DRV_RANGECHECK:
    SUB   1
    JP    C, MB_BDNERR

; --------------------------------------------------
; Sets current drive
; --------------------------------------------------
MB_CURDRV_SET:
    LD    H, 0
    LD    L, A
    LD    A, (MB_DRIVENO)
    OR    A
    JP    Z, MB_BDNERR                              ; Bad drive number
    DEC   A
    CP    L
    JP    C, MB_BDNERR
    LD    A, L
    LD    (MB_CURDRV), A
    ADD   HL, HL                                    ; no*2
    EX    DE, HL
    LD    HL, (MB_NULBUF)
    ADD   HL, DE
    LD    A, (HL)
    INC   HL
    LD    H, (HL)
    LD    L, A
    LD    A, (HL)
    LD    (MB_HDR_PTR), HL
    INC   A
    RET

; --------------------------------------------------
; Get file info by number
; --------------------------------------------------
MB_FILSCN:
    DEC   HL
    RST   CHRGTR
    CP    '#'
    CALL  Z, MB_CHRGTR
    CALL  MB_GETBYT
    EX    (SP), HL
    PUSH  HL

; --------------------------------------------------
; Redirect interpreter input/output to file
; --------------------------------------------------
MB_REDIR_IO:
    CALL  MB_FILIDX
    JP    Z, MB_ERRFNO
    LD    (MB_PTRFIL), HL
    EX    DE, HL
    LD    HL, (MB_HDR_PTR)
    LD    A, (HL)
    INC   A
    EX    DE, HL
    LD    A, (HL)
    RET   NZ
    LD    (HL), 0
    JP    MB_IERERR

; --------------------------------------------------
; NAME Command
; NAME old-file AS new-file
; --------------------------------------------------
MB_NAME:
    CALL  MB_NAMFIL                                  ; Get old file
    PUSH  DE
    PUSH  HL
    CALL  MB_FNSWAP
    POP   HL
    RST   SYNCHR
    DB    'A'
    RST   SYNCHR
    DB    'S'
    CALL  MB_NAMFIL                                  ; Get new file
    EX    (SP), HL
    LD    A, H
    JR    C, MB_FRENAM
    CP    D
    JP    NZ, MB_RADERR
MB_FRENAM:
    CALL  MB_CURDRV_SET
    CALL  MB_FILOPN
    JP    Z, MB_FAEERR
    CALL  MB_FNSWAP
    CALL  MB_FILOPN
    JP    NZ, MB_FNFERR
    CALL  MB_SWAP_FN2
    LD    A, (MB_DIR_DRVST)
    LD    D, A
    CALL  MB_SECTOR_WRITE
    POP   HL
    RET

; --------------------------------------------------
MB_FNSWAP:
    LD    HL, MB_FN_FIRST
MB_SWAP_FN2:
    LD    DE, MB_FN_SECOND
MB_SWAP_9B:
    LD    B, 9

; --------------------------------------------------
; Exchange two bufferers
;  Inp: B - bytes count
;       HL -> buffer1
;       DE -> buffer2
; --------------------------------------------------
MB_SWAPB:
    LD    C, (HL)
    LD    A, (DE)
    LD    (HL), A
    LD    A, C
    LD    (DE), A
    INC   DE
    INC   HL
    DJNZ  MB_SWAPB
    RET

; --------------------------------------------------
; OPEN Statement
; OPEN "FILE.DAT" FOR INPUT AS #1
; --------------------------------------------------
MB_OPEN:
    LD    BC, MB_FINPRT
    PUSH  BC
    CALL  MB_NAMFIL
    LD    A, (HL)
    CP    _FOR
    LD    E, ERROD   ;Out of DATA
    JR    NZ, MB_FO_ASN
    RST   CHRGTR
    CP    _INPUT
    LD    E, 1
    JR    Z, MB_FO_INP
    CP    'O'
    JR    Z, MB_FO_CHK_OUT

    ; Check for "APPEND"
    RST   SYNCHR
    DB    'A'
    RST   SYNCHR
    DB    'P'
    RST   SYNCHR
    DB    'P'
    RST   SYNCHR
    DB    'E'
    RST   SYNCHR
    DB    'N'
    RST   SYNCHR
    DB    'D'
    LD    E, 0x8
    JR    MB_FO_ASN

    ; Check for "OUTPUT"
MB_FO_CHK_OUT:
    RST   CHRGTR
    RST   SYNCHR
    DB    'U'
    RST   SYNCHR
    DB    'T'
    RST   SYNCHR
    DB    'P'
    RST   SYNCHR
    DB    'U'
    RST   SYNCHR
    DB    'T'
    LD    E, 2
    JR    MB_FO_ASN
MB_FO_INP:
    RST   CHRGTR

    ; Check for "AS"
MB_FO_ASN:
    RST   SYNCHR
    DB    'A'
    RST   SYNCHR
    DB    'S'
    PUSH  DE
    LD    A, (HL)
    CP    '#'
    CALL  Z, MB_CHRGTR
    CALL  MB_GETBYT
    OR    A
    JP    Z, MB_BFNERR
    DB    LD_E_n

; --------------------------------------------------
; Open i/o channel
;  Inp: A = i/o channel number,
;       E = filemode
; --------------------------------------------------
MB_OPEN_CH:
    PUSH  DE
    DEC   HL
    LD    E, A
    RST   CHRGTR
    JP    NZ, MB_SNERR
    EX    (SP), HL
    LD    A, E
    PUSH  AF
    PUSH  HL
    CALL  MB_FILIDX
    JP    NZ, MB_FAOERR
    POP   DE
    PUSH  DE
    PUSH  HL
    LD    A, D
    CALL  MB_CURDRV_SET
    POP   HL
    POP   DE
    JR    NZ, MB_OPEN_INITDAC
    LD    A, D
    CALL  MB_VERIFY_OPT
MB_OPEN_INITDAC:
    LD    (HL), E
    INC   HL
    LD    (HL), 0xff
    INC   HL
    INC   HL
    INC   HL
    LD    (HL), D
    POP   AF
    CALL  MB_REDIR_IO
    LD    A, (HL)
    LD    (HL), 0
    LD    (MB_FILNAM), A
    PUSH  AF
    CALL  MB_FILOPN
    JR    Z, MB_OPEN_ATTACH
    POP   AF
    LD    B, A
    AND   0x86
    JP    Z, MB_FNFERR
    LD    A, B
    SCF
    CALL  MB_OPEN_DUPOPEN_CHK
MB_OPEN_CREATE:
    CALL  MB_OPEN_CREATEENT
    JP    MB_OPEN_ZEROPOS
MB_OPEN_ATTACH:
    PUSH  HL
    LD    DE, 0x9
    ADD   HL, DE
    LD    A, (HL)
    INC   HL
    LD    E, (HL)
    LD    HL, (MB_PTRFIL)
    INC   HL
    LD    (HL), E
    INC   HL
    INC   HL
    INC   HL
    INC   HL
    INC   HL
    INC   HL
    AND   0xf1
    LD    (HL), A
    POP   HL
    POP   AF
    CP    0x8
    JR    Z, MB_OPEN_ATEND
    LD    B, A
    AND   0x5
    JR    NZ, MB_OPEN_ZEROPOS
    PUSH  BC
    PUSH  HL
    CALL  MB_OPEN_DUPOPEN_CHK
    CALL  MB_OPEN_DUPCHK
    JP    C, MB_FAOERR
    CALL  MB_FREE_ALLBLKS
    LD    HL, (MB_PTRFIL)
    INC   HL
    LD    (HL), 0xff
    POP   HL
    POP   AF
    AND   0x82
    JR    NZ, MB_OPEN_CREATE
    LD    (HL), A
    LD    A, (MB_DIR_DRVST)
    LD    D, A
    CALL  MB_SECTOR_WRITE
    POP   HL
    RET
MB_OPEN_ATEND:
    CALL  MB_LOC_COUNTBLKS
    LD    HL, (MB_PTRFIL)
    INC   HL
    INC   HL
    LD    (HL), B
    INC   HL
    OR    A
    JR    Z, MB_OPEN_NEWFLG
    DEC   A
    LD    (HL), A
    JR    MB_OPEN_PREPBUF
MB_OPEN_NEWFLG:
    LD    HL, (MB_PTRFIL)
    LD    (HL), 0x2
MB_OPEN_ZEROPOS:
    LD    HL, (MB_PTRFIL)
    INC   HL
    LD    A, (HL)
    INC   HL
    LD    (HL), A
    INC   HL
    LD    (HL), 0
MB_OPEN_PREPBUF:
    INC   HL
    INC   HL
    XOR   A
    LD    (HL), A
    INC   HL
    LD    (HL), A
    INC   HL
    LD    A, (HL)
    OR    0x2
    LD    (HL), A
    CALL  MB_PBUF_CLR
    DEC   HL
    LD    (HL), A
    LD    HL, (MB_PTRFIL)
    LD    A, (MB_FILNAM)
    LD    (HL), A
    POP   HL
    RET

; --------------------------------------------------
; Double-open check (bit 4 conflict)
; --------------------------------------------------
MB_OPEN_DUPOPEN_CHK:
    PUSH  DE
    PUSH  HL
    LD    HL, (MB_HDR_PTR)
    DEC   HL
    PUSH  AF
    LD    A, (HL)
    AND   0x10
    JP    NZ, MB_DORERR
    POP   AF
    POP   DE
    JR    C, MB_OPEN_DBL_OK
    PUSH  AF
    LD    HL, 0x9
    ADD   HL, DE
    LD    A, (HL)
    AND   0x10
    JP    NZ, MB_DORERR
    POP   AF
MB_OPEN_DBL_OK:
    EX    DE, HL
    POP   DE
    RET

; --------------------------------------------------
; Close I/O channel
;  Inp: A = channel number
; --------------------------------------------------
MB_CLSFIL:
    PUSH  HL
    OR    A
    JR    NZ, MB_NTFL0
    LD    A, (MB_NLONLY)
    AND   1
    JP    NZ, MB_PBUF_RET
MB_NTFL0:
    CALL  MB_FILIDX
    JR    Z, MB_NTOPNC
    LD    (MB_PTRFIL), HL
    PUSH  HL
    CALL  MB_7THB_4BI
    JR    Z, MB_CLSFL2
    LD    HL, (MB_PTRFIL)
    LD    DE, 6
    ADD   HL, DE
    LD    A, (HL)
    OR    A
    JR    Z, MB_CLSFL4
    CALL  MB_REC_TERMREC
MB_CLSFL2:
    CALL  MB_PBUF_CLR
    POP   HL
MB_NTOPNC:
    PUSH  HL
    LD    DE, 7
    ADD   HL, DE
    LD    (HL), A
    LD    H, A
    LD    L, A
    LD    (MB_PTRFIL), HL
    POP   HL
    ADD   A, (HL)
    LD    (HL), 0
    JR    Z, MB_CLSFL3
    INC   HL
    INC   HL
    INC   HL
    INC   HL
    LD    A, (HL)
    CALL  MB_FILOPEN_PREP
    LD    HL, (MB_HDR_PTR)
    INC   HL
    LD    A, (HL)
    OR    A
MB_CLSFL3:
    POP   HL
    RET   Z
    JP    MB_FILEOP_ERRSAVE
MB_CLSFL4:
    LD    HL, (MB_HDR_PTR)
    INC   HL
    INC   (HL)
    EX    DE, HL
    LD    HL, (MB_PTRFIL)
    LD    BC, 0x2
    ADD   HL, BC
    LD    L, (HL)
    LD    H, 0
    INC   DE
    INC   DE
    ADD   HL, DE
    DEC   (HL)
    LD    A, (HL)
    CP    0xc0
    JR    NZ, MB_CLSFL2
    LD    HL, (MB_PTRFIL)
    INC   HL
    LD    A, (HL)
MB_CLSFL5:
    LD    H, 0
    LD    L, A
    ADD   HL, DE
    LD    C, A
    LD    A, (HL)
    CP    0xc0
    JR    NC, MB_CLSFL2
    LD    B, A
    LD    L, A
    LD    H, 0
    ADD   HL, DE
    LD    A, (HL)
    CP    0xc0
    LD    A, B
    JR    C, MB_CLSFL5
    EX    DE, HL
    LD    B, 0
    ADD   HL, BC
    LD    (HL), 0xc8
    EX    DE, HL
    LD    (HL), 0xff
    LD    HL, (MB_HDR_PTR)
    INC   HL
    INC   HL
    INC   (HL)
    JP    MB_CLSFL2

; --------------------------------------------------
; Delete file from disk
; KILL <filename>
; --------------------------------------------------
MB_KILL:
    CALL  MB_NAMFIL
    LD    E, 0x10
    PUSH  AF
    XOR   A
    CALL  MB_OPEN_CH
    XOR   A
    CALL  MB_CLSFIL
    POP   AF
    JP    MB_FILOPEN_PREP

; --------------------------------------------------
; RUN statement (with filespecification)
; --------------------------------------------------
MB_LRUN:
    SCF
    DB    LD_DE_nn

; --------------------------------------------------
; LOAD statement
; --------------------------------------------------
MB_LOAD:
    DB    OR_n

; --------------------------------------------------
; MERGE statement
; MERGE "[drv:]filename"
; --------------------------------------------------
MB_MERGE:
    XOR   A
    PUSH  AF
    CALL  MB_NAMFIL
    POP   AF
    PUSH  AF
    JR    Z, MB_MRGENT
    LD    A, (HL)
    SUB   ','
    OR    A
    JR    NZ, MB_MRGENT
    RST   CHRGTR
    RST   SYNCHR
    DB    'R'
    POP   AF
    SCF

; --------------------------------------------------
MB_CHNENT:
    PUSH  AF
MB_MRGENT:
    PUSH  AF
    XOR   A
    LD    E, 1
    CALL  MB_OPEN_CH
    LD    HL, (MB_PTRFIL)
    LD    BC, 0x7
    ADD   HL, BC
    POP   AF
    SBC   A, A
    AND   0x80
    OR    1
    LD    (MB_NLONLY), A
    POP   AF
    PUSH  AF
    SBC   A, A
    LD    (MB_FILNAM), A
    LD    A, (HL)
    OR    A
    JP    M, MB_BINLOD
    POP   AF
    CALL  NZ, MB_SCRTCH
    XOR   A
    CALL  MB_REDIR_IO
    JP    MB_MAIN

; --------------------------------------------------
; SAVE statement
; SAVE "[drv]:file" [,A][,P]
; --------------------------------------------------
MB_SAVE:
    CALL  MB_NAMFIL
    DEC   HL
    RST   CHRGTR
    LD    E, 0x80
    SCF
    JR    Z, MB_SAV_CHK
    RST   SYNCHR
    DB    ','
    RST   SYNCHR
    DB    'A'
    OR    A
    LD    E, 2                                       ; ASCII Mode
MB_SAV_CHK:
    PUSH  AF
    XOR   A
    CALL  MB_OPEN_CH
    POP   AF
    JR    C, MB_BSAV_1
    DEC   HL
    RST   CHRGTR
    JP    MB_LIST
MB_BSAV_1:
    PUSH  HL
    CALL  MB_SCCPTR
    LD    HL, (MB_TXTTAB)                            ; =0x733F
    PUSH  HL
MB_BSAV_2:
    CALL  MB_FILEOP_GETPOS
    CALL  MB_POS_SECTOROF
    INC   C
    POP   DE
    LD    HL, (MB_VARTAB)
    LD    A, L
    SUB   E
    LD    L, A
    LD    A, H
    SBC   A, D
    INC   L
    DEC   L
    JR    Z, MB_SAVE_WRTSEC
    JP    P, MB_SAVE_ODDADJ
    INC   A
MB_SAVE_ODDADJ:
    INC   A
MB_SAVE_WRTSEC:
    CP    0x8
    JR    C, MB_SAVE_SECTWRT
    LD    A, 0x8
MB_SAVE_SECTWRT:
    OR    A
    SCF
    EX    DE, HL
    CALL  NZ, MB_SECOP
    OR    A
    PUSH  AF
    LD    B, A
    LD    C, 0
    ADD   HL, BC
    EX    DE, HL
    LD    HL, (MB_VARTAB)
    RST   DCOMPR
    JR    Z, MB_SAVE_ENDFILE
    JR    C, MB_SAVE_ENDFILE
    POP   AF
    PUSH  DE
    CALL  MB_ALLOC_BLK
    LD    HL, (MB_PTRFIL)
    INC   HL
    INC   HL
    LD    (HL), C
    JR    MB_BSAV_2
MB_SAVE_ENDFILE:
    LD    HL, (MB_PTRFIL)
    INC   HL
    INC   HL
    LD    E, (HL)
    LD    D, 0
    CALL  MB_DAC_GETADDR
    ADD   HL, DE
    POP   AF
    ADD   A, (HL)
    LD    (HL), A
    CALL  MB_TRUNC_MARK
    POP   HL
    XOR   A
    JP    MB_CLSFIL
MB_BINLOD:
    POP   AF
    JP    Z, MB_SIOERR
    CALL  MB_SCRTCH
    CALL  MB_CLSALL
    XOR   A
    CALL  MB_FILIDX
    LD    (HL), 0x80
    LD    (MB_PTRFIL), HL
    CALL  MB_LOC_COUNTBLKS
    LD    B, A
    PUSH  BC
    LD    HL, (MB_TXTTAB)  ;=0x733F
    PUSH  HL
MB_BINLD_SECTLP:
    CALL  MB_FILEOP_GETPOS
    CALL  MB_DAC_GETADDR
    LD    L, B
    LD    H, 0
    ADD   HL, DE
    LD    A, (HL)
    CP    0xc0
    JR    NC, MB_BINLD_EXEC
    POP   HL
    PUSH  HL
    PUSH  AF
    LD    DE, 0x800
    ADD   HL, DE
    JR    C, MB_BINLD_MEMFAIL
    CALL  MB_BINLD_SPACECHK
    POP   AF
    LD    HL, (MB_PTRFIL)
    INC   HL
    INC   HL
    LD    (HL), A
    CALL  MB_POS_SECTOROF
    INC   C
    POP   HL
    LD    A, 0x8
    OR    A
    CALL  MB_SECOP
    LD    DE, 0x800
    ADD   HL, DE
    PUSH  HL
    JR    MB_BINLD_SECTLP
MB_BINLD_EXEC:
    POP   HL
    POP   BC
    LD    A, B
    PUSH  AF
    LD    D, A
    LD    E, 0
    PUSH  HL
    INC   D
    ADD   HL, DE
    JR    C, MB_BINLD_MEMFAIL
    CALL  MB_BINLD_SPACECHK
    CALL  MB_FILEOP_GETPOS
    CALL  MB_POS_SECTOROF
    INC   C
    POP   HL
    POP   AF
    OR    A
    CALL  NZ, MB_SECOP
    CALL  MB_LINKER
    INC   HL
    LD    (MB_VARTAB), HL
    CALL  MB_RUNC
    XOR   A
    LD    (MB_NLONLY), A
    CALL  MB_CLSFIL
    LD    A, (MB_CHNFLG)
    OR    A
    JP    NZ, MB_CHNRET
    LD    A, (MB_FILNAM)
    OR    A
    JP    NZ, MB_NEWSTT
    JP    MB_READY


; --------------------------------------------------
; BINLOAD: free memory space check
; --------------------------------------------------
MB_BINLD_SPACECHK:
    EX    DE, HL
    LD    HL, (MB_FRETOP)
    EX    DE, HL
    RST   DCOMPR
    RET   C
MB_BINLD_MEMFAIL:
    CALL  MB_SCRTCH
    XOR   A
    LD    (MB_NLONLY), A
    JP    MB_OMERR
    PUSH  HL
    PUSH  DE
    LD    HL, (MB_PTRFIL)
    LD    DE, 0x4
    ADD   HL, DE
    LD    A, (HL)
    POP   DE
    POP   HL
    RET

; --------------------------------------------------
; Check if position already in use by another file
; --------------------------------------------------
MB_OPEN_DUPCHK:
    LD    HL, (MB_PTRFIL)
    INC   HL
    LD    B, (HL)
    INC   HL
    INC   HL
    INC   HL
    LD    C, (HL)
    PUSH  HL
    LD    HL, (MB_FILTAB)
    LD    A, (MB_MAXFIL)
    INC   A
    PUSH  AF
MB_DUPCHK_SCANTAB:
    POP   AF
MB_DUPCHK_NXTENT:
    DEC   A
    JP    M, MB_DUPCHK_NONE
    PUSH  AF
    LD    E, (HL)
    INC   HL
    LD    D, (HL)
    INC   HL
    LD    A, (DE)
    OR    A
    JR    Z, MB_DUPCHK_SCANTAB
    INC   DE
    LD    A, (DE)
    CP    B
    JR    NZ, MB_DUPCHK_SCANTAB
    INC   DE
    INC   DE
    INC   DE
    LD    A, (DE)
    CP    C
    JR    NZ, MB_DUPCHK_SCANTAB
    POP   AF
    EX    (SP), HL
    PUSH  AF
    RST   DCOMPR
    JR    NZ, MB_DUPCHK_ISDUP
    POP   AF
    EX    (SP), HL
    JR    MB_DUPCHK_NXTENT
MB_DUPCHK_ISDUP:
    POP   AF
    POP   HL
    SCF
    RET
MB_DUPCHK_NONE:
    POP   HL
    OR    A
    RET

; --------------------------------------------------
; Create new directory entry on disk
; --------------------------------------------------
MB_OPEN_CREATEENT:
    PUSH  AF
    LD    A, (MB_CHAIN_FP)
    OR    A
    JP    P, MB_DIRCREAT_WRITENT
    CP    0x81
    LD    A, D
    LD    (MB_DIR_DRVST), A
    JP    M, MB_DFLERR
    JR    NZ, MB_DIRCREAT_NEWSECT
    CP    0xc
    JR    Z, MB_DIRCREAT_SETCNT
    PUSH  HL
    LD    HL, (MB_PTRFIL1)
    LD    A, (HL)
    PUSH  AF
    LD    (HL), 0xff
    INC   D
    CALL  MB_SECTOR_WRITE
    POP   AF
    LD    (HL), A
    JR    MB_DIRCREAT_POP
MB_DIRCREAT_NEWSECT:
    PUSH  HL
    LD    BC, 0x10
    ADD   HL, BC
    LD    (HL), 0xff
MB_DIRCREAT_POP:
    POP   HL
MB_DIRCREAT_SETCNT:
    LD    A, E
    LD    (MB_CHAIN_FP), A
MB_DIRCREAT_WRITENT:
    LD    A, (MB_DIR_DRVST)
    ADD   A, 0x80
    LD    D, A
    PUSH  HL
    CALL  C, MB_DIR_READSECT
    CALL  MB_ALLOC_BLK
    LD    DE, MB_FN_FIRST
    POP   HL
    CALL  MB_SWAP_9B
    POP   AF
    AND   0x80
    EX    DE, HL
    LD    HL, (MB_HDR_PTR)
    DEC   HL
    OR    (HL)
    LD    (DE), A
    LD    HL, (MB_PTRFIL)
    INC   HL
    PUSH  HL
    INC   HL
    INC   HL
    INC   HL
    INC   HL
    INC   HL
    INC   HL
    LD    (HL), A
    POP   HL
    LD    A, (HL)
    INC   DE
    LD    (DE), A
    LD    A, (MB_DIR_DRVST)
    AND   0x7f
    LD    D, A
    JP    MB_SECTOR_WRITE

; --------------------------------------------------
MB_RSET:
    DB    OR_n

; --------------------------------------------------
MB_LSET:
    SCF
    PUSH  AF
    CALL  MB_PTRGET
    CALL  MB_CHKSTR
    PUSH  DE
    RST   SYNCHR
    DB    EQULTK
    CALL  MB_FRMEVL
    POP   BC
    EX    (SP), HL
    PUSH  HL
    PUSH  BC
    CALL  MB_FRESTR
    LD    B, (HL)
    EX    (SP), HL
    LD    A, (HL)
    LD    C, A
    PUSH  BC
    PUSH  HL
    PUSH  AF
    INC   HL
    LD    E, (HL)
    INC   HL
    LD    D, (HL)
    OR    A
    JP    Z, MB_LSET_ZEROLEN
    LD    HL, (MB_TXTTAB)  ;=0x733F
    RST   DCOMPR
    JR    NC, MB_LSET_CALCLEN
    LD    HL, (MB_VARTAB)
    RST   DCOMPR
    JR    C, MB_LSET_CALCLEN
    LD    E, C
    LD    D, 0
    LD    HL, (MB_STREND)
    ADD   HL, DE
    EX    DE, HL
    LD    HL, (MB_FRETOP)
    RST   DCOMPR
    JP    C, MB_LSET_RESIZE
    ; get/allocate string space
MB_LSET_GETSPACE:
    POP   AF
    LD    A, C
    CALL  MB_GETSPA
    POP   HL
    POP   BC
    EX    (SP), HL
    PUSH  DE
    PUSH  BC
    CALL  MB_FRESTR
    POP   BC
    POP   DE
    EX    (SP), HL
    PUSH  BC
    PUSH  HL
    INC   HL
    LD    (HL), E
    INC   HL
    LD    (HL), D
    PUSH  AF
    ; calculate new length
MB_LSET_CALCLEN:
    POP   AF
    POP   HL
    INC   HL
    LD    E, (HL)
    INC   HL
    LD    D, (HL)
    POP   BC
    POP   HL
    PUSH  DE
    INC   HL
    LD    E, (HL)
    INC   HL
    LD    D, (HL)
    EX    DE, HL
    POP   DE
    LD    A, C
    SUB   B
    JR    NC, MB_LSET_ADJLEN
    LD    B, C
    ; adjust length fields
MB_LSET_ADJLEN:
    LD    A, C
    SUB   B
    LD    C, A
    POP   AF
    CALL  NC, MB_LSET_FILLSPC
    INC   B
    ; copy old string data
MB_LSET_COPYLP:
    DEC   B
    JR    Z, MB_LSET_PADEND
    LD    A, (HL)
    LD    (DE), A
    INC   HL
    INC   DE
    JR    MB_LSET_COPYLP
    ; zero-length result
MB_LSET_ZEROLEN:
    POP   BC
    POP   BC
    POP   BC
    POP   BC
    POP   BC
    ; pad end of string
MB_LSET_PADEND:
    CALL  C, MB_LSET_FILLSPC
    POP   HL
    RET

; --------------------------------------------------
; LSET/RSET: fill with spaces
; --------------------------------------------------
MB_LSET_FILLSPC:
    LD    A, 0x20
    INC   C
MB_LSET_FILLLP:
    DEC   C
    RET   Z
    LD    (DE), A
    INC   DE
    JR    MB_LSET_FILLLP
MB_LSET_RESIZE:
    POP   AF
    POP   HL
    POP   BC
    EX    (SP), HL
    EX    DE, HL
    JR    NZ, MB_LSET_RETRY
    PUSH  BC
    LD    A, B
    CALL  MB_STRINI
    CALL  MB_PUTNEW
    POP   BC
MB_LSET_RETRY:
    EX    (SP), HL
    PUSH  BC
    PUSH  HL
    PUSH  AF
    JP    MB_LSET_GETSPACE

; --------------------------------------------------
MB_FIELD:
    DEC   HL
    RST   CHRGTR
    CP    '#'
    CALL  Z, MB_CHRGTR
    CALL  MB_GETBYT
    PUSH  HL
    CALL  MB_FILIDX
    CALL  MB_ADDHL9
    POP   DE
    XOR   A
    LD    B, A
MB_FIELD_PARSELP:
    LD    C, A
    PUSH  HL
    ADD   HL, BC
    EX    DE, HL
    LD    A, (HL)
    CP    ','
    JP    NZ, MB_IOPREP_RET
    PUSH  DE
    PUSH  BC
    CALL  MB_GTBYTC
    PUSH  AF
    RST   SYNCHR
    DB    'A'
    RST   SYNCHR
    DB    'S'
    CALL  MB_PTRGET
    CALL  MB_CHKSTR
    POP   AF
    POP   BC
    EX    (SP), HL
    INC   B
    ADD   A, C
    JR    C, MB_FIELD_SETDESC
    DEC   B
MB_FIELD_SETDESC:
    JP    NZ, MB_FOVERR
    EX    DE, HL
    SUB   C
    LD    (HL), A
    ADD   A, C
    INC   HL
    LD    (HL), E
    INC   HL
    LD    (HL), D
    POP   DE
    POP   HL
    JR    MB_FIELD_PARSELP

; --------------------------------------------------
MB_SET:
    LD    A, (HL)
    CALL  MB_FILSCN_1
    PUSH  AF
    PUSH  DE
    CALL  MB_SET_ATTRPARSE
    EX    (SP), HL
    LD    C, A
    LD    A, (HL)
    AND   0xaf
    ADD   A, C
    LD    (HL), A
    LD    D, A
    POP   HL
    POP   AF
    JR    C, MB_SET_FLUSH
    RET   Z
    PUSH  HL
    PUSH  DE
    LD    HL, (MB_PTRFIL1)
    LD    BC, MB_COUT_NLCHK
    LD    A, 1
    OR    A
    CALL  MB_SECT_EXEC
    JP    C, MB_IOEERR
    POP   DE
    LD    (HL), D
    SCF
    CALL  MB_SECT_EXEC
    JP    C, MB_IOEERR
    POP   HL
    POP   AF
    PUSH  HL
    CALL  MB_CURDRV_SET
    POP   HL
    RET

; --------------------------------------------------
MB_SET_FLUSH:
    PUSH  HL
    LD    A, (MB_DIR_DRVST)
    LD    D, A
    CALL  MB_SECTOR_WRITE
    POP   HL
    EX    (SP), HL
    CALL  MB_HRSLF
    POP   HL
    RET

; --------------------------------------------------
; SET: parse attribute value (A = attr, C = has-attr)
; --------------------------------------------------
MB_SET_ATTRPARSE:
    RST   SYNCHR
    DB    ','
    CALL  MB_FRMEVL
    PUSH  HL
    CALL  MB_FRESTR
    LD    A, (HL)
    OR    A
    JP    Z, MB_PBUF_RET
    INC   HL
    LD    E, (HL)
    INC   HL
    LD    D, (HL)
    LD    B, A
    LD    C, 0
MB_SET_ATTRLP:
    LD    HL, MB_SET_ATTRACCUM
    PUSH  HL
    LD    A, (DE)
    INC   DE
    CP    'P'
    LD    L, 0x10
    RET   Z
    CP    'R'
    LD    L, 0x40
    RET   Z
    LD    L, 0
    RET
MB_SET_ATTRACCUM:
    LD    A, C
    OR    L
    LD    C, A
    DJNZ  MB_SET_ATTRLP
    POP   HL
    RET

; --------------------------------------------------
MB_ATTR_:
    RST   CHRGTR
    RST   SYNCHR
    DB    '('
    CALL  MB_FILSCN_1
    JR    NC, MB_ATTR_GETFD
    EX    (SP), HL
    CALL  MB_HRSLF
    POP   HL
    JR    MB_ATTR_MAKESPEC
MB_ATTR_GETFD:
    JR    Z, MB_ATTR_MAKESPEC
    POP   AF
    PUSH  HL
    PUSH  DE
    CALL  MB_CURDRV_SET
    POP   DE
    POP   HL
MB_ATTR_MAKESPEC:
    RST   SYNCHR
    DB    ')'
    PUSH  HL
    LD    A, (DE)
    PUSH  AF
    LD    A, 0x2
    CALL  MB_STRINI
    EX    DE, HL
    POP   AF
    LD    B, 0x2
    LD    DE, MB_ATTR_MASK1
    LD    C, A
MB_ATTR_PAIRLP:
    LD    A, (DE)
    INC   DE
    AND   C
    LD    A, (DE)
    INC   DE
    JR    NZ, MB_ATTR_OUTCH
    LD    A, 0x20
MB_ATTR_OUTCH:
    LD    (HL), A
    INC   HL
    DJNZ  MB_ATTR_PAIRLP
    JP    MB_PUTNEW

; --------------------------------------------------
MB_ATTR_MASK1:
    DB    0b01000000                                ; mask byte 0x40 ('R' = readable)
MB_ATTR_CHR1:
    DB    'R'
MB_ATTR_MASK2:
    DB    0b00010000                                ; mask byte 0x10 ('P' = printable)
MB_ATTR_CHR2:
    DB    'P'

; --------------------------------------------------
MB_FILSCN_1:
    CP    '#'
    JR    Z, MB_CHRGTR_1
    CALL  MB_FRMEVL
    RST   GETYPTR
    JR    Z, MB_FILSCN_USECUR
    LD    A, (MB_CURDRV)
    POP   DE
    PUSH  AF
    PUSH  DE
    CALL  MB_CONINT
    PUSH  HL
    PUSH  AF
    CALL  MB_DRV_RANGECHECK
    DEC   HL
    JR    NZ, MB_FILSCN_NOOPEN
    POP   AF
    DEC   A
    CALL  MB_VERIFY_OPT
    LD    A, (HL)
    PUSH  HL
    CALL  MB_CURFIL_PREP
    POP   HL
    LD    (HL), A
    PUSH  AF
    ; no file open error path
MB_FILSCN_NOOPEN:
    POP   AF
    LD    A, 1
    ; return (C = found)
MB_FILSCN_RET:
    EX    DE, HL
    POP   HL
    OR    A
    RET

; --------------------------------------------------
MB_CHRGTR_1:
    RST   CHRGTR
    CALL  MB_GETBYT
    PUSH  HL
    LD    HL, (MB_PTRFIL)
    PUSH  HL
    CALL  MB_FILIDX
    JP    Z, MB_ERRFNO
    LD    DE, 0x7
    ADD   HL, DE
    EX    (SP), HL
    CALL  MB_HRSLF
    POP   HL
    XOR   A
    JR    MB_FILSCN_RET

; --------------------------------------------------
; FILSCN: use current position
; --------------------------------------------------
MB_FILSCN_USECUR:
    EX    DE, HL
    LD    HL, (MB_PTRFIL)
    EX    (SP), HL
    PUSH  HL
    EX    DE, HL
    CALL  MB_NAMFIL1
    PUSH  HL
    LD    A, D
    CALL  MB_CURDRV_SET
    CALL  MB_FILOPN
    JP    NZ, MB_FNFERR
    LD    DE, 0x9
    ADD   HL, DE
    EX    DE, HL
    POP   HL
    SCF
    RET

; --------------------------------------------------
MB_MKI_:
    LD    A, TINT
    DB    LD_BC_nn

; --------------------------------------------------
MB_MKS_:
    LD    A, TSGN
    DB    LD_BC_nn

; --------------------------------------------------
MB_MKD_:
    LD    A, TDBL
    PUSH  AF
    CALL  MB_DOCNVF
    POP   AF
    CALL  MB_STRINI
    LD    HL, (MB_DSCTMP+1)
    CALL  MB_MOVMF
    JP    MB_FINBCK

; --------------------------------------------------
MB_CVI:
    LD    A, 1
    DB    LD_BC_nn

; --------------------------------------------------
MB_CVS:
    LD    A, 0x3
    DB    LD_BC_nn

; --------------------------------------------------
MB_CVD:
    LD    A, 0x7
    PUSH  AF
    CALL  MB_FRESTR
    POP   AF
    CP    (HL)
    JP    NC, MB_FCERR
    INC   A
    INC   HL
    LD    C, (HL)
    INC   HL
    LD    H, (HL)
    LD    L, C
    LD    (MB_VALTYP), A
    JP    MB_VMOVFM

; --------------------------------------------------
MB_GET_IOCHN:
    JR    NZ, MB_EVA_CH
    PUSH  HL

MB_CHCK_NXT:
    PUSH  BC
    PUSH  AF
    LD    DE, MB_CHN_SCAN_RET
    PUSH  DE
    PUSH  BC
    OR    A
    RET

; --------------------------------------------------
MB_CHN_SCAN_RET:
    POP   AF
    POP   BC
    DEC   A
    JP    P, MB_CHCK_NXT
    POP   HL
    RET

; --------------------------------------------------
MB_EVA_CH_END:
    POP   BC
    POP   HL
    LD    A, (HL)
    CP    ','
    RET   NZ
    RST   CHRGTR

MB_EVA_CH:
    PUSH  BC
    LD    A, (HL)
    CP    '#'
    CALL  Z, MB_CHRGTR
    CALL  MB_GETBYT
    EX    (SP), HL
    PUSH  HL
    LD    DE, MB_EVA_CH_END
    PUSH  DE
    SCF
    JP    (HL)

; --------------------------------------------------
; CLOSE statement
; CLOSE [[#]file_no, [..]]
; --------------------------------------------------
MB_CLOSE:
    LD    BC, MB_CLSFIL
    LD    A, (MB_MAXFIL)
    JR    MB_GET_IOCHN

; --------------------------------------------------
; Close ALL disks
; --------------------------------------------------
MB_CLSALL:
    LD    A, (MB_NLONLY)
    OR    A
    RET   M
    LD    BC, MB_CLSFIL
    XOR   A
    LD    A, (MB_MAXFIL)
    JR    MB_GET_IOCHN

; --------------------------------------------------
MB_INITSA:
    XOR   A
    LD    B, A
    ; clear file descriptor table
MB_INITSA_CLEARTAB:
    LD    A, B
    CALL  MB_FILIDX
    LD    (HL), 0
    LD    A, (MB_MAXFIL)
    INC   B
    SUB   B
    JR    NC, MB_INITSA_CLEARTAB
    LD    B, 0
    ; clear DAC block
MB_INITSA_CLRDAC:
    LD    A, B
    CALL  MB_CURDRV_SET
    DEC   HL
    LD    (HL), 0
    INC   HL
    LD    (HL), 0xff
    INC   HL
    LD    (HL), 0
    INC   HL
    LD    (HL), 0
    INC   B
    LD    A, (MB_DRIVENO)
    SUB   B
    JR    NZ, MB_INITSA_CLRDAC
    LD    (MB_NLONLY), A
    CALL  AL_FDC_HWRST
    CALL  MB_SCRTCH
    LD    HL, (MB_TXTTAB)  ;=0x733F
    DEC   HL
    LD    (HL), 0
    JP    MB_READYR

; --------------------------------------------------
; LFILES [drv] print list of files to printer
; --------------------------------------------------
MB_LFILES:
    LD    A, 1
    LD    (MB_PRTFLG), A

; --------------------------------------------------
; FILES [drv] print list of files to screen
; --------------------------------------------------
MB_FILES:
    LD    E, 1
    JR    Z, MB_FILSOUT
    CALL  MB_GETBYT
    DEC   HL
    RST   CHRGTR
    JP    NZ, MB_SNERR

; --------------------------------------------------
MB_FILSOUT:
    LD    A, E
    PUSH  HL
    DEC   E
    PUSH  DE
    CALL  MB_DRV_RANGECHECK
    POP   DE
    LD    A, E
    JR    NZ, MB_FILSOUT_INITDRV
    CALL  MB_VERIFY_OPT
    PUSH  HL
    CALL  MB_CURFIL_PREP
    POP   HL
MB_FILSOUT_INITDRV:
    LD    D, 1
MB_FILSOUT_DIRLP:
    CALL  MB_DIR_READSECT
    PUSH  DE
    LD    B, 0x10
MB_FILSOUT_ENTLP:
    LD    A, (HL)
    INC   A
    JP    Z, MB_FILSOUT_DONE
    DEC   A
    JR    Z, MB_FILSOUT_PRINTNAME
    CALL  MB_FILSOUT_NAMESTR
    CALL  MB_FILSOUT_PRINTEOF
    CALL  MB_FILSOUT_ALIGNSPC
    JR    MB_FILSOUT_PRINTSIZE

; --------------------------------------------------
; Format filename string
; --------------------------------------------------
MB_FILSOUT_NAMESTR:
    LD    D, 0x9
    ; name character loop
MB_FILSOUT_NAMLP:
    LD    A, (HL)
    INC   HL
    RST   OUTDO
    DEC   D
    RET   Z
    LD    A, D
    CP    0x3
    JR    NZ, MB_FILSOUT_NAMLP
    PUSH  HL
    INC   HL
    INC   HL
    INC   HL
    LD    A, (HL)
    AND   0x80
    JR    Z, MB_FILSOUT_TYPOCHR
    ADD   A, 0xe
    ; type character (extension)
MB_FILSOUT_TYPOCHR:
    ADD   A, 0x20
    AND   0x7f
    RST   OUTDO
    POP   HL
    JR    MB_FILSOUT_NAMLP

; --------------------------------------------------
; FILSOUT: print EOF marker
; --------------------------------------------------
MB_FILSOUT_PRINTEOF:
    PUSH  HL
    PUSH  BC
    LD    A, ASCII_SP
    RST   OUTDO
    CALL  MB_FILEOP_LOF
    LD    L, C
    LD    H, 0
    INC   L
    CALL  MB_LINPRT
    POP   BC
    POP   HL
    RET

; --------------------------------------------------
; Align to column with
; --------------------------------------------------
MB_FILSOUT_ALIGNSPC:
    CALL  MB_CURCOL_GET
    AND   0xf
    RET   Z
    LD    A, ASCII_SP
    RST   OUTDO
    JR    MB_FILSOUT_ALIGNSPC

    ; print name field
MB_FILSOUT_PRINTNAME:
    LD    DE, 0x9
    ADD   HL, DE

    ; print size field
MB_FILSOUT_PRINTSIZE:
    LD    DE, 0x7
    ADD   HL, DE
    CALL  MB_FILSOUT_WRAPCHK
    DJNZ  MB_FILSOUT_ENTLP
    POP   DE
    INC   D
    LD    A, D
    CP    ASCII_CR
    JP    C, MB_FILSOUT_DIRLP
    PUSH  DE

    ; all entries done
MB_FILSOUT_DONE:
    POP   DE
    CALL  MB_CRDO
    POP   HL
    JP    MB_FINPRT

; --------------------------------------------------
; Line wrap check (max 4 names per line)
; --------------------------------------------------
MB_FILSOUT_WRAPCHK:
    CALL  MB_MAXCOL_GET
    LD    C, A
    CALL  MB_CURCOL_GET
    ADD   A, 0xf
    CP    C
    JP    NC, MB_CRDO
    RET

; --------------------------------------------------
; Get current output column
; --------------------------------------------------
MB_CURCOL_GET:
    LD    A, (MB_PRTFLG)
    OR    A
    LD    A, (MB_TTYPOS)
    RET   Z
    LD    A, (MB_LPTPOS)
    RET

; --------------------------------------------------
; Get max output column
; --------------------------------------------------
MB_MAXCOL_GET:
    LD    A, (MB_PRTFLG)
    OR    A
    LD    A, (MB_LINLEN)   ;= 40
    RET   Z
    LD    A, (MB_LPTSIZ)   ;= 255
    RET

; --------------------------------------------------
; PUT statement
; PUT [#]nr,  var
; --------------------------------------------------
MB_PUT:
    DB    OR_n

; --------------------------------------------------
; GET statement
; GET [#]nr,  var
; --------------------------------------------------
MB_GET:
    XOR   A
    PUSH  AF
    CALL  MB_FILSCN
    INC   HL
    INC   HL
    LD    E, (HL)
    INC   HL
    LD    C, (HL)
    POP   DE
    POP   AF
    PUSH  AF
    PUSH  DE
    JR    Z, MB_GET_RECNUM
    INC   HL
    INC   HL
    INC   HL
    INC   HL
    LD    A, (HL)
    AND   0x10
    JP    NZ, MB_DORERR
    LD    HL, (MB_PTRFIL)
    LD    A, (HL)
    CP    0x2
    JR    Z, MB_GET_RECNUM
    CP    0x4
    JP    NZ, MB_DORERR
MB_GET_RECNUM:
    POP   HL
    DEC   HL
    RST   CHRGTR
    EX    (SP), HL
    PUSH  HL
    JP    Z, MB_FILEOP_CURPOS
    POP   HL
    EX    (SP), HL
    RST   SYNCHR
    DB    ','
    DEC   HL
    CALL  MB_INTIDX
    DEC   HL
    RST   CHRGTR
    JP    NZ, MB_SNERR
    EX    (SP), HL
    PUSH  HL
    CALL  MB_SCAN_FINDRECB
    JR    NC, MB_FILEOP_STOREPOS
    POP   AF
    JP    Z, MB_RPEERR
    PUSH  AF
    LD    A, B
    OR    A
    JR    Z, MB_PUT_COMMITPOS
    LD    HL, (MB_HDR_PTR)
    INC   HL
    INC   HL
    DEC   A
    CP    (HL)
    JP    NC, MB_DFLERR
MB_FILEOP_ALLOCN:
    PUSH  BC
    CALL  MB_ALLOC_BLK
    POP   BC
    DJNZ  MB_FILEOP_ALLOCN
    PUSH  BC
    CALL  MB_LOC_COUNTBLKS
    LD    E, B
    POP   BC
MB_PUT_COMMITPOS:
    LD    A, C
    ADD   A, 0xc0
    LD    (HL), A
    PUSH  BC
    CALL  MB_TRUNC_MARK
    POP   BC
MB_FILEOP_STOREPOS:
    CALL  MB_7THB_4BI
    JR    Z, MB_FILEOP_TRANSFER
    POP   AF
    PUSH  AF
    LD    A, (HL)
    OR    0x4
    LD    (HL), A
    LD    B, E
    POP   AF
    PUSH  AF
    PUSH  BC
    CALL  Z, MB_REC_TERMREC
    POP   BC
    LD    E, B
MB_FILEOP_TRANSFER:
    LD    HL, (MB_PTRFIL)
    INC   HL
    INC   HL
    LD    (HL), E
    INC   HL
    LD    (HL), C
    POP   AF
    PUSH  AF
    CALL  Z, MB_REC_READREC
    POP   AF
    CALL  NZ, MB_REC_DIRTYFLUSH
    POP   HL
    JP    MB_FINPRT
MB_FILEOP_CURPOS:
    CALL  MB_FILEOP_GETPOS
    LD    E, B
    JR    MB_FILEOP_STOREPOS

; --------------------------------------------------
MB_ISCONO:
    POP   AF
    PUSH  HL
    PUSH  DE
    PUSH  BC
    PUSH  AF
    LD    HL, (MB_PTRFIL)
    LD    DE, 0x7
    ADD   HL, DE
    LD    A, (HL)
    AND   0x4
    JP    NZ, MB_SAPERR
    LD    A, (HL)
    AND   0x10
    JP    NZ, MB_DORERR
    LD    A, (HL)
    AND   0x2
    DEC   HL
    DEC   HL
    PUSH  HL
    CALL  NZ, MB_REC_GETDISPATCH
    POP   HL
    INC   HL
    LD    E, (HL)
    INC   HL
    LD    A, (HL)
    OR    0x8
    LD    (HL), A
    INC   HL
    LD    A, E
    POP   AF
    PUSH  HL
    PUSH  AF
    SUB   0xd
    JR    Z, MB_PUT_COMMITPOS2
    INC   (HL)
    CP    0x13
    SBC   A, A
    ADD   A, (HL)
MB_PUT_COMMITPOS2:
    LD    (HL), A
    INC   HL
    LD    D, 0
    EX    DE, HL
    ADD   HL, DE
    POP   AF
    LD    (HL), A
    POP   HL
    PUSH  AF
    DEC   HL
    DEC   HL
    INC   (HL)
    LD    A, (HL)
    OR    A
    CALL  Z, MB_REC_FLUSHDO
    POP   AF
MB_FILEOP_ISCONO_RET:
    POP   BC
    POP   DE
    POP   HL
    RET
MB_INDSKC:
    PUSH  BC
    PUSH  HL
    PUSH  DE
MB_INDSKC_SCAN:
    LD    HL, (MB_PTRFIL)
    LD    DE, 0x7
    ADD   HL, DE
    LD    A, (HL)
    AND   0x4
    JP    NZ, MB_SAPERR
    LD    A, (HL)
    AND   0x2
    DEC   HL
    PUSH  HL
    SUB   1
    CCF
    CALL  C, MB_REC_GETDISPATCH
    POP   HL
    JR    C, MB_INDSKC_FIXFLG
    DEC   HL
    LD    A, (HL)
    INC   HL
    LD    E, (HL)
    INC   (HL)
    CP    (HL)
    INC   HL
    JR    NZ, MB_INDSKC_GETPOS
    LD    A, (HL)
    OR    0x2
    LD    (HL), A
MB_INDSKC_GETPOS:
    INC   HL
    INC   HL
    LD    D, 0
    ADD   HL, DE
    LD    A, (HL)
MB_INDSKC_FIXFLG:
    PUSH  AF
    LD    HL, (MB_PTRFIL)
    LD    BC, 0x7
    ADD   HL, BC
    LD    C, (HL)
    LD    A, C
    AND   0x7f
    LD    (HL), A
    CP    C
    JR    Z, MB_INDSKC_DONE
    POP   AF
    PUSH  AF
    JR    C, MB_INDSKC_DONE
    CP    0xa
    JR    Z, MB_INDSKC_RETRY
MB_INDSKC_DONE:
    POP   AF
    POP   DE
    POP   HL
    POP   BC
    RET
MB_INDSKC_RETRY:
    POP   AF
    JR    MB_INDSKC_SCAN

; --------------------------------------------------
; INPUT$(nbytes,  [#]fileno) function
; --------------------------------------------------
MB_FIXINP:
    RST   CHRGTR
    RST   SYNCHR
    DB    '$'
    RST   SYNCHR
    DB    '('
    PUSH  HL
    LD    HL, (MB_PTRFIL)
    PUSH  HL
    LD    HL, 0
    LD    (MB_PTRFIL), HL
    POP   HL
    EX    (SP), HL
    CALL  MB_GETBYT
    PUSH  DE
    LD    A, (HL)
    CP    ','
    JR    NZ, MB_REDTTY                              ; read from user terminal

    ; input from file
    RST   CHRGTR
    CALL  MB_FILSCN
    CP    1
    JP    Z, MB_FINPT
    CP    0x4
    JP    NZ, MB_RPEERR
MB_FINPT:
    POP   HL
    XOR   A
    LD    A, (HL)

; --------------------------------------------------
; Read from TTY
; --------------------------------------------------
MB_REDTTY:
    PUSH  AF
    RST   SYNCHR
    DB    ')'
    POP   AF
    EX    (SP), HL
    PUSH  AF
    LD    A, L
    OR    A
    JP    Z, MB_FCERR
    PUSH  HL
    CALL  MB_STRINI
    EX    DE, HL
    POP   BC

; --------------------------------------------------
MB_FIXLOP:
    POP   AF
    PUSH  AF
    JR    Z, MB_DSKCHR                              ; read disk file
    CALL  MB_TRYOUT2                                ; get char if one ready
    CP    CTRL_C
    JR    Z, MB_INTCTC

; --------------------------------------------------
MB_PUTCHR:
    LD    (HL), A                                   ; put character to string buffer
    INC   HL
    DEC   C
    JR    NZ, MB_FIXLOP
    POP   AF
    POP   BC
    POP   HL
    CALL  MB_HRSLF
    PUSH  BC
    JP    MB_PUTNEW

; --------------------------------------------------
MB_INTCTC:
    POP   AF
    LD    HL, (MB_CURLIN)  ;=0xFFFE
    LD    (MB_ERRLIN), HL
    POP   HL
    JP    MB_READYR
MB_DSKCHR:
    CALL  MB_INDSKC
    JP    C, MB_RPEERR
    JR    MB_PUTCHR
MB_HRSLF:
    PUSH  HL
    PUSH  DE
    PUSH  BC
    PUSH  AF
    LD    (MB_PTRFIL), HL
    LD    A, H
    OR    L
    JR    Z, MB_HRSLF_DORET
    LD    DE, 0x4
    ADD   HL, DE
    LD    A, (HL)
    OR    A
    CALL  P, MB_CURDRV_SET
MB_HRSLF_DORET:
    POP   AF
    POP   BC
    POP   DE
    POP   HL
    RET

; --------------------------------------------------
; Read disk sector
; DSKI$(disk, track, sector) Function
; --------------------------------------------------
MB_DSKIFN:
    RST   CHRGTR
    RST   SYNCHR
    DB    '('
    CALL  MB_GET_DTSS
    PUSH  AF
    RST   SYNCHR
    DB    ')'
    POP   AF
    PUSH  HL
    LD    L, A
    LD    A, (MB_CURDRV)
    PUSH  AF
    LD    A, L
    CALL  MB_DRV_RANGECHECK
    LD    A, 0xff
    PUSH  BC
    CALL  MB_STRINI
    EX    DE, HL
    POP   BC
    PUSH  HL
    LD    HL, (MB_PTRFIL1)
    LD    A, 1
    OR    A
    CALL  MB_SECOP
    POP   DE
    LD    HL, (MB_PTRFIL1)
    EX    DE, HL
    LD    C, 255
    CALL  MB_MDEHLC
    POP   AF
    CALL  MB_CURDRV_SET
    JP    MB_PUTNEW

; --------------------------------------------------
; Write disk sector
; DSKO$(disk, track, sector,  str) Function
; --------------------------------------------------
MB_DSKOFN:
    CALL  MB_GET_DTSS
    PUSH  AF
    DEC   HL
    RST   CHRGTR
    JP    NZ, MB_SNERR
    EX    (SP), HL
    PUSH  HL
    LD    HL, (MB_PTRFIL1)
    POP   AF
    PUSH  HL
    CALL  MB_DRV_RANGECHECK
    POP   HL
    SCF
    LD    A, 1
    CALL  MB_SECOP
    POP   HL
    RET

; --------------------------------------------------
; Move C bytes (DE) -> (HL),  HL do not changed
; --------------------------------------------------
MB_MDEHLC:
    PUSH  HL
MB_MDEHLC_NXT:
    LD    A, (DE)
    LD    (HL), A
    INC   HL
    INC   DE
    DEC   C
    JR    NZ, MB_MDEHLC_NXT
    POP   HL
    RET

; --------------------------------------------------
; Get disk, track, sector [,str] parameters
; --------------------------------------------------
MB_GET_DTSS:
    CALL  MB_GETBYT       ;disk
    PUSH  AF
    RST   SYNCHR
    DB    ','
    CALL  MB_GETBYT       ;track
    CP    TINT
    JP    NC, MB_BTSERR
    LD    (MB_TMP_TRK), A
    RST   SYNCHR
    DB    ','
    CALL  MB_GETBYT       ;sector
    CP    '('
    JP    NC, MB_BTSERR
    ADD   A, A
    LD    B, A
    LD    A, (MB_TMP_TRK)
    ADD   A, B
    PUSH  AF
    RST   SYNCHR
    DB    ','
    CALL  MB_GETBYT
    DEC   A
    CP    16
    JP    NC, MB_BTSERR
    INC   A
    POP   BC
    LD    C, A
    POP   AF
    RET

; --------------------------------------------------
MB_PBUF_CLR:
    CALL  MB_GBUFPTR      ;Get ptr to 9th byte of PTRFIL
    PUSH  HL
    LD    B, 0
    CALL  MB_FILL0

MB_PBUF_RET:
    POP   HL
    RET

; --------------------------------------------------
; Find record in sector buffer (scan)
; --------------------------------------------------
MB_RECBUF_FIND:
    LD    B, 0xff
    LD    HL, (MB_PTRFIL)
    EX    DE, HL
    LD    HL, 264
    ADD   HL, DE
MB_RECBUF_SCANLP:
    LD    A, (HL)     ;=0x2F9A
    OR    A
    JR    Z, MB_RECBUF_SCANBACK
    CP    0x1a
    LD    A, B
    RET   Z
    OR    A
    INC   A
    INC   HL
    RET   NZ
    SCF
    RET
MB_RECBUF_SCANBACK:
    LD    A, B
    SUB   1
    LD    B, A
    DEC   HL
    JR    NC, MB_RECBUF_SCANLP
    INC   HL
    XOR   A
    RET

; --------------------------------------------------
; Fill by zeroes
; Inp: HL -> memory to fill
;      B  - counter
; --------------------------------------------------
MB_FILL0:
    XOR   A
MB_FILL0_NXT:
    LD    (HL), A
    INC   HL
    DJNZ  MB_FILL0_NXT
    RET

    ; Return 4-th bit of PTRFIL+7
MB_7THB_4BI:
    PUSH  DE
    LD    HL, (MB_PTRFIL)
    LD    DE, 7
    ADD   HL, DE
    LD    A, (HL)
    AND   0x8
    POP   DE
    RET

; --------------------------------------------------
; Check current file state before op
; --------------------------------------------------
MB_FILEOP_CHKCUR:
    LD    HL, (MB_HDR_PTR)
    INC   HL
    LD    A, (HL)
    OR    A
    RET   Z
MB_FILEOP_ERRSAVE:
    PUSH  BC
    PUSH  DE
    PUSH  HL
    PUSH  AF
    CALL  MB_SECOPER_VERIFY
    POP   AF
    POP   HL
    POP   DE
    POP   BC
    RET

; --------------------------------------------------
; Verify sector read (checksum, retry)
; --------------------------------------------------
MB_SECOPER_VERIFY:
    LD    BC, 0xe
MB_SECOPER_RDLOOP:
    PUSH  BC
    CALL  MB_DAC_GETADDR
    EX    DE, HL
    LD    B, 0x25
    XOR   A
    LD    (MB_SECT_ERRCNT), A
    LD    A, 1
    SCF
    CALL  MB_IOPREP_START
    POP   BC
    JR    NC, MB_SECOPER_NXTTRY
    INC   B
MB_SECOPER_NXTTRY:
    INC   C
    LD    A, C
    CP    0x11
    JR    C, MB_SECOPER_RDLOOP
    DEC   HL
    DEC   HL
    XOR   A
    LD    (HL), A
    OR    B
    RET   Z
    CP    0x3
    CCF
    JR    NC, MB_BATERR_PRINT
    DEC   HL
    DEC   HL
    DEC   HL
    LD    (HL), 0
    RET

; --------------------------------------------------
; Print battery/checksum error message
; --------------------------------------------------
MB_BATERR_PRINT:
    ADD   A, 0x30
    LD    HL, (MB_PTRFIL)
    PUSH  HL
    LD    HL, 0
    LD    (MB_PTRFIL), HL
    RST   OUTDO
    LD    HL, MS_COA_BAD   ;= " copies of allocation"
    CALL  MB_STROUT
    LD    A, (MB_CURDRV)
    ADD   A, 0x31
    RST   OUTDO
    CALL  MB_CRDO
    POP   HL
    LD    (MB_PTRFIL), HL
    RET

; --------------------------------------------------
MB_SECTOR_STATUS:
    PUSH  DE
    LD    B, 0x25
    LD    C, D
    CALL  MB_DAC_GETADDR
    EX    DE, HL
    LD    A, E
    OR    A
    JR    Z, MB_SECTOR_ADJFLG
    LD    A, 0xff
MB_SECTOR_ADJFLG:
    INC   A
    CALL  MB_SECTOR_READ
    SBC   A, A
    POP   DE
    RET

; --------------------------------------------------
; Read one sector to buffer
; --------------------------------------------------
MB_SECTOR_READ:
    LD    A, 1
    LD    HL, (MB_PTRFIL1)
    CALL  MB_SECT_EXEC
    RET   C
    CALL  MB_DAC_GETADDR
    LD    HL, (MB_PTRFIL1)
    LD    B, 160
    JP    MB_SWAPB

; --------------------------------------------------
; Read directory sector (with retry)
; --------------------------------------------------
MB_DIR_READSECT:
    LD    A, 1
    OR    A
MB_DIR_READPREP:
    PUSH  DE
    LD    HL, (MB_PTRFIL1)
    PUSH  HL
    LD    C, D
    LD    B, 0x25
    CALL  MB_IOPREP_START
    JP    C, MB_IOEERR
    POP   HL
    POP   DE
    RET

; --------------------------------------------------
; Write one sector from buffer
; --------------------------------------------------
MB_SECTOR_WRITE:
    XOR   A
    LD    (MB_SECT_ERRCNT), A
    LD    A, 1
    SCF
    JR    MB_DIR_READPREP

; --------------------------------------------------
; Flush dirty record (test + flush)
; --------------------------------------------------
MB_REC_DIRTYFLUSH:
    CALL  MB_REC_DIRTY_TEST
    JP    MB_REC_WRITFLUSH

; --------------------------------------------------
; Perform the actual flush write
; --------------------------------------------------
MB_REC_FLUSHDO:
    CALL  MB_REC_WRITFLUSH

; --------------------------------------------------
; End current record in buffer
; --------------------------------------------------
MB_REC_ENDRECORD:
    CALL  MB_REC_DIRTY_TEST
    JR    NC, MB_REC_POSITION
    CALL  MB_PBUF_CLR
    LD    (HL), 0x1a
    DEC   HL
    DEC   HL
    LD    A, (HL)
    OR    0x8
    LD    (HL), A
    XOR   A
    DEC   HL
    LD    (HL), A
    DEC   HL
    LD    (HL), 0
    RET

; --------------------------------------------------
; Read one record from file
; --------------------------------------------------
MB_REC_READREC:
    CALL  MB_REC_READPRE2
    JP    C, MB_RPEERR
    SCF

; --------------------------------------------------
; Position to given record number
; --------------------------------------------------
MB_REC_POSITION:
    PUSH  AF
    CALL  MB_IOPREP_WRITE
    LD    HL, (MB_PTRFIL)
    CALL  MB_FILEOP_EOF
    LD    C, 0xff
    JR    NC, MB_RECIODS_ZEROPOS
    CALL  MB_RECBUF_FIND
    LD    B, A
    JR    C, MB_RECIODS_SETFLG
    OR    A
    JR    NZ, MB_RECIODS_SETFLG
    LD    B, 0
    SCF
MB_RECIODS_SETFLG:
    SBC   A, A
    LD    C, A
    POP   AF
    JR    C, MB_RECIODS_SETMODE
    LD    A, C
    INC   A
    JR    Z, MB_RECIODS_SETMODE
    LD    (HL), 0x1a
MB_RECIODS_SETMODE:
    LD    HL, (MB_PTRFIL)
    LD    A, (HL)
    CP    0x8
    JR    NZ, MB_RECIODS_CLEARFLG
    LD    (HL), 0x2
    LD    A, B
    OR    A
    PUSH  AF
    JR    NZ, MB_RECIODS_WRITPOS
    CALL  MB_REC_ENDRECORD
MB_RECIODS_ZEROPOS:
    POP   AF
    LD    B, 0
MB_RECIODS_CLEARFLG:
    PUSH  AF
    XOR   A
MB_RECIODS_WRITPOS:
    LD    HL, (MB_PTRFIL)
    LD    DE, 0x7
    ADD   HL, DE
    PUSH  AF
    LD    A, (HL)
    AND   0xfd
    LD    (HL), A
    POP   AF
    DEC   HL
    LD    (HL), A
    DEC   HL
    LD    (HL), B
    POP   AF
    RET   NC
    INC   C
    JR    Z, MB_RECIODS_RETFLG
    INC   B
    DEC   B
    RET   Z
MB_RECIODS_RETFLG:
    OR    A
    RET

; --------------------------------------------------
; Record GET dispatch (read + commit)
; --------------------------------------------------
MB_REC_GETDISPATCH:
    PUSH  AF
    CALL  MB_7THB_4BI
    CALL  NZ, MB_REC_TERMREC
    POP   AF
    PUSH  AF
    CALL  MB_REC_SEEKPOS
    JR    C, MB_REC_RDCOMMIT
    POP   AF
    PUSH  AF
    SCF
    CALL  MB_REC_POSITION
    JR    C, MB_REC_RDCOMMIT
    POP   AF
    INC   HL
    INC   HL
    INC   HL
    INC   HL
    LD    A, (HL)
    OR    A
    RET

    ; commit after read
MB_REC_RDCOMMIT:
    POP   AF
    RET   C
    INC   HL
    INC   HL
    INC   HL
    LD    (HL), 0
    INC   HL
    LD    (HL), 0
    INC   HL
    LD    A, (HL)
    AND   0xfd
    LD    (HL), A
    SCF
    RET

; --------------------------------------------------
; Terminate record in buffer (NUL/0x1A)
; --------------------------------------------------
MB_REC_TERMREC:
    LD    HL, (MB_PTRFIL)
    CALL  MB_FILEOP_EOF
    JR    NC, MB_REC_WRITFLUSH
    CALL  MB_RECBUF_FIND
    JR    C, MB_REC_WRITFLUSH
    LD    (HL), 0x1a

; --------------------------------------------------
; Write and flush current record
; --------------------------------------------------
MB_REC_WRITFLUSH:
    CALL  MB_FILEOP_GETPOS
    LD    A, (HL)
    AND   0xf7
    LD    (HL), A
    CALL  MB_POS_SECTOROF
    CALL  MB_GBUFPTR
    LD    A, 1
    SCF
    JP    MB_SECOP
MB_GBUFPTR:
    LD    HL, (MB_PTRFIL)
MB_ADDHL9:
    LD    DE, 9
    ADD   HL, DE
    RET

; Dirty-flag test routine
MB_REC_DIRTY_TEST:
    DB    OR_n

; --------------------------------------------------
; Prepare second read buffer area
; --------------------------------------------------
MB_REC_READPRE2:
    SCF

; --------------------------------------------------
; Seek to record position (allocate if needed)
; --------------------------------------------------
MB_REC_SEEKPOS:
    LD    HL, (MB_PTRFIL)
    PUSH  HL
    PUSH  AF
    CALL  MB_FILEOP_EOF
    JR    NC, MB_REC_SEEKPREP
    POP   AF
    JP    C, MB_PBUF_RET
    PUSH  AF
    ; prepare seek params
MB_REC_SEEKPREP:
    POP   HL
    EX    (SP), HL
    INC   HL
    INC   HL
    LD    B, (HL)
    INC   HL
    LD    C, (HL)
    INC   C
    LD    A, C
    CP    0x9
    CCF
    JR    NC, MB_REC_ALLOCBLK
    PUSH  HL
    CALL  MB_DAC_GETADDR
    LD    L, B
    XOR   A
    LD    H, A
    ADD   HL, DE
    LD    C, 1
    LD    B, (HL)
    POP   HL
    LD    A, 0xbf
    CP    B
    JR    NC, MB_REC_COMMIT
    POP   AF
    PUSH  AF
    CCF
    JR    NC, MB_REC_COMMIT
    PUSH  HL
    CALL  MB_ALLOC_BLK
    POP   HL
    LD    B, C
    LD    C, 1
    ; allocate new block at end
MB_REC_ALLOCBLK:
    POP   AF
    PUSH  AF
    CCF
    JR    NC, MB_REC_COMMIT
    PUSH  BC
    PUSH  HL
    CALL  MB_DAC_GETADDR
    LD    L, B
    LD    H, 0
    ADD   HL, DE
    LD    A, 0xc0
    ADD   A, C
    LD    E, A
    LD    A, (HL)
    CP    E
    JR    NC, MB_REC_FOUND
    CP    0xc0
    CCF
    JR    NC, MB_REC_FOUND
    INC   (HL)
    CALL  MB_TRUNC_MARK
    SCF
    ; position found
MB_REC_FOUND:
    POP   HL
    POP   BC
    ; commit new position
MB_REC_COMMIT:
    EX    (SP), HL
    POP   HL
    LD    (HL), C
    DEC   HL
    LD    (HL), B
    RET

; --------------------------------------------------
; Prepare write operation (mode check)
; --------------------------------------------------
MB_IOPREP_WRITE:
    CALL  MB_FILEOP_GETPOS
    LD    A, (HL)
    AND   0xf3
    LD    (HL), A
    CALL  MB_POS_SECTOROF
    INC   HL
    INC   HL
    LD    A, 1
MB_SECOP:
    CALL  MB_SECOPER
    RET   NC
    JP    MB_IOEERR

; --------------------------------------------------
; Get current position (record/sector)
; --------------------------------------------------
MB_FILEOP_GETPOS:
    LD    HL, (MB_PTRFIL)
    INC   HL
    INC   HL
    LD    B, (HL)
    INC   HL
    LD    C, (HL)
    INC   HL
    INC   HL
    INC   HL
    INC   HL
    RET

; --------------------------------------------------
; Compute sector number from position
; --------------------------------------------------
MB_POS_SECTOROF:
    LD    A, C
    PUSH  AF
    LD    A, B
    LD    B, 0xff
    ; division loop
MB_POS_LOOP:
    SUB   0x4
    INC   B
    JR    NC, MB_POS_LOOP
    ADD   A, 0x4
    LD    C, A
    RRCA
    RRCA
    LD    A, B
    RLA
    LD    B, A
    LD    A, C
    AND   1
    ADD   A, A
    ADD   A, A
    ADD   A, A
    LD    C, A
    POP   AF
    ADD   A, C
    LD    C, A
    RET

; --------------------------------------------------
; Execute sector operation (read/write via FDC)
; --------------------------------------------------
MB_SECT_EXEC:
    EX    DE, HL
    PUSH  AF
    LD    HL, (MB_HDR_PTR)
    INC   (HL)
    JR    Z, MB_SECT_UNSEL
    LD    A, B
    OR    A
    RRA
    LD    (HL), A
    INC   (HL)
    ; deselect drive on error
MB_SECT_UNSEL:
    DEC   (HL)
    POP   AF
    PUSH  DE
    PUSH  AF
    CALL  AL_FDC_RWSECT
    JR    C, MB_SECT_FAIL
    POP   AF
    POP   HL
    OR    A
    RET
    ; failure path
MB_SECT_FAIL:
    POP   AF
    POP   HL
    SCF
    RET

; --------------------------------------------------
MB_SECOPER:
    PUSH  AF
    XOR   A
    LD    (MB_SECT_ERRCNT), A
    PUSH  HL
    LD    HL, (MB_PTRFIL)
    LD    A, H
    OR    L
    JR    Z, MB_SECOPER_DACSEL
    LD    A, (HL)
    OR    A
    JR    Z, MB_SECOPER_DACSEL
    LD    DE, 0x7
    ADD   HL, DE
    JR    MB_SECOPER_APPCHK
MB_SECOPER_DACSEL:
    LD    HL, (MB_HDR_PTR)
    DEC   HL
MB_SECOPER_APPCHK:
    LD    A, (HL)
    CPL
    AND   0x40
    LD    (MB_SECT_WRFLG), A
    POP   HL
MB_SECOPER_WRCHK:
    POP   AF
    LD    E, A
    JR    NC, MB_SECOPER_FLGPREP
    LD    A, (MB_SECT_WRFLG)
    OR    A
    JR    Z, MB_SECOPER_FLGPREP
    SCF
MB_SECOPER_FLGPREP:
    LD    A, E
    PUSH  AF
    JR    C, MB_SECOPER_EXEC
    JR    NZ, MB_SECOPER_EXEC
    SCF
MB_SECOPER_EXEC:
    CALL  MB_IOPREP_DOIO
    JR    C, MB_SECOPER_STATCHK
    POP   AF
    CCF
    RET   NC
    CCF
    RET   NZ
    PUSH  AF
    CALL  MB_IOPREP_DOIO
    JP    NC, MB_IOPREP_RET
MB_SECOPER_RETRYCNT:
    LD    A, (MB_SECT_ERRCNT)
    INC   A
    LD    (MB_SECT_ERRCNT), A
    CP    0x4
    JR    C, MB_SECOPER_WRCHK
    POP   AF
MB_SECOPER_FAIL:
    SCF
    RET
MB_SECOPER_STATCHK:
    POP   AF
    JR    NZ, MB_SECOPER_FAIL
    PUSH  AF
    JR    MB_SECOPER_RETRYCNT

; --------------------------------------------------
; Start I/O operation (select, verify)
; --------------------------------------------------
MB_IOPREP_START:
    PUSH  AF
    PUSH  HL
    LD    HL, (MB_HDR_PTR)
    DEC   HL
    JR    MB_SECOPER_APPCHK

; --------------------------------------------------
; Do the actual I/O (call FDC, check status)
; --------------------------------------------------
MB_IOPREP_DOIO:
    PUSH  AF
    PUSH  HL
    LD    HL, (MB_HDR_PTR)
    LD    A, (HL)
    INC   A
    JR    Z, MB_IOPREP_CALLFDC
    LD    D, 0x12
    LD    A, D
    ADD   A, A
    INC   A
    SUB   B
    JR    Z, MB_IOPREP_CALLFDC
    LD    E, A
    LD    A, D
    SUB   (HL)
    XOR   E
    CALL  M, MB_FILEOP_CHKCUR

MB_IOPREP_CALLFDC:
    POP   HL
    POP   AF
    CALL  MB_SECT_EXEC
    PUSH  AF
    LD    A, B
    CP    0x25
    JP    NZ, MB_IOPREP_RET
    LD    A, C
    CP    0xe
    PUSH  HL
    CALL  C, MB_FILEOP_CHKCUR
    POP   HL
MB_IOPREP_RET:
    POP   AF
    RET

; --------------------------------------------------
; DSKF(<disk>)
; Return free clusters. Disk will be mounted
; (with MOUNT)
; --------------------------------------------------
MB_DSKF:
    CALL  MB_CONINT
    LD    B, A
    LD    A, (MB_CURDRV)
    PUSH  AF
    LD    A, B
    DEC   B
    CALL  MB_DRV_RANGECHECK
    LD    A, B
    JR    NZ, MB_DSKF_TONUM
    CALL  MB_VERIFY_OPT
    PUSH  HL
    CALL  MB_CURFIL_PREP
    POP   HL
MB_DSKF_TONUM:
    INC   HL
    INC   HL
    LD    L, (HL)
    LD    H, 0
    POP   AF
    PUSH  HL
    CALL  MB_CURDRV_SET
    POP   HL
    JP    MB_UI2SNG

; --------------------------------------------------
; LOC(fnum) function
; Get position of last operation in file
; --------------------------------------------------
MB_LOC:
    LD    A, (MB_CURDRV)
    PUSH  AF
    CALL  MB_FILFRM
    JP    Z, MB_ERRFNO
    PUSH  HL
    CALL  MB_FILEOP_LOC
    ; load position into HL
MB_LOC_POSLOAD:
    CALL  MB_CMUL8
    POP   HL
    INC   HL
    INC   HL
    INC   HL
    LD    L, (HL)
    ; return (A = position)
MB_LOC_RET:
    LD    H, 0
    ADD   HL, DE
    POP   AF
    PUSH  HL
    CALL  MB_CURDRV_SET
    POP   HL
    JP    MB_MAKINT

; --------------------------------------------------
; LOF(fnum) function
; Get Length of file
; --------------------------------------------------
MB_LOF:
    LD    A, (MB_CURDRV)
    PUSH  AF
    CALL  MB_FILFRM
    JP    Z, MB_ERRFNO
    CALL  MB_FILEOP_LOF
    PUSH  AF
    CALL  MB_CMUL8
    POP   AF
    LD    L, A
    JR    MB_LOC_RET

; --------------------------------------------------
; EOF(fnum) function
; Return true if end of file reached
; --------------------------------------------------
MB_EOF:
    LD    A, (MB_CURDRV)
    PUSH  AF
    CALL  MB_FILFRM
    JP    Z, MB_ERRFNO
    PUSH  HL
    CALL  MB_FILEOP_EOF
    POP   HL
    JR    NZ, MB_EOF_RET
    LD    DE, 0x5
    ADD   HL, DE
    LD    A, (HL)
    INC   HL
    SUB   (HL)
    OR    A
    JR    NZ, MB_EOF_RET
    SCF
    ; return (A = 0/1)
MB_EOF_RET:
    SBC   A, A
    LD    B, A
    POP   AF
    CALL  MB_CURDRV_SET
    LD    A, B
    JP    MB_CONIA

; --------------------------------------------------
; FPOS(fnum) function
; Return next position in file to read/write
; --------------------------------------------------
MB_FPOS:
    LD    A, (MB_CURDRV)
    PUSH  AF
    CALL  MB_FILFRM
    PUSH  HL
    INC   HL
    INC   HL
    LD    C, (HL)
    JP    MB_LOC_POSLOAD

; --------------------------------------------------
; Direct statement
; --------------------------------------------------
MB_HDIOP:
    CALL  MB_ISFLIO
    JP    Z, MB_GONE
    XOR   A
    CALL  MB_CLSFIL
    JP    MB_DSFERR

; --------------------------------------------------
; Redirect interpreter input if i/o channel specified
; --------------------------------------------------
MB_FILINP:
    LD    C, 1

; --------------------------------------------------
; Redirect interpreter input/output if i/o channel
; specified
; --------------------------------------------------
MB_FILGET:
    CP    '#'
    RET   NZ
    PUSH  BC
    CALL  MB_GTBYTC
    RST   SYNCHR
    DB    ','
    LD    A, E
    PUSH  HL
    CALL  MB_REDIR_IO
    LD    A, (HL)
    POP   HL
    POP   BC
    CP    C
    JR    Z, MB_FG_END
    CP    0x4
    JR    Z, MB_FG_END
    CP    0x8
    JR    NZ, MB_FG_ERR
    LD    A, C
    CP    0x2
MB_FG_ERR:
    JR    Z, MB_FG_END
    LD    A, C
    CP    1
    JP    Z, MB_RPEERR
    JP    MB_DORERR
MB_FG_END:
    LD    A, (HL)
    RET

; --------------------------------------------------
; Close I/O channel 0 and load HL from (TEMP)
; --------------------------------------------------
MB_PRGFIN_1:
    LD    BC, MB_GET_TEMP
    PUSH  BC
    XOR   A
    JP    MB_CLSFIL

; --------------------------------------------------
; Setup to input sequential item
; --------------------------------------------------
MB_FILIND:
    RST   GETYPTR
    LD    BC, MB_DOASIG
    LD    DE, CON1
    JR    NZ, MB_INPDOR    ;if numeric,  go to file read
    LD    E, D
    JR    MB_INPDOR

; --------------------------------------------------
; Line input and read code for item fetching
; from sequential input file
; --------------------------------------------------
MB_DLINE:
    LD    BC, MB_FINPRT
    PUSH  BC
    CALL  MB_FILINP
    CALL  MB_PTRGET
    CALL  MB_CHKSTR
    PUSH  DE
    LD    BC, MB_LETCON
    XOR   A
    LD    D, A
    LD    E, A
MB_INPDOR:
    PUSH  AF
    PUSH  BC
    PUSH  HL
MB_NOTNWT:
    CALL  MB_INDSKC
    JP    C, MB_RPEERR
    CP    ASCII_SP
    JR    NZ, MB_NOTSPC1
    INC   D
    DEC   D
    JR    NZ, MB_NOTNWT
MB_NOTSPC1:
    CP    '"'
    JR    NZ, MB_NOTQTE
    LD    A, E
    CP    ','
    LD    A, '"'
    JR    NZ, MB_NOTQTE
    LD    D, A
    LD    E, A
    CALL  MB_INDSKC
    JR    C, MB_QUITSI
MB_NOTQTE:
    LD    HL, MB_BUF
    LD    B, 0xff
MB_LOPCRS:
    LD    C, A
    LD    A, D
    CP    '"'
    LD    A, C
    JR    Z, MB_NOTQTL
    CP    ASCII_CR
    PUSH  HL
    JR    Z, MB_ICASLF
    POP   HL
    CP    ASCII_LF
    JR    NZ, MB_NOTQTL
MB_SKPLF:
    LD    C, A
    LD    A, E
    CP    ','
    LD    A, C
    CALL  NZ, MB_STRCHR
    CALL  MB_INDSKC
    JR    C, MB_QUITSI
    CP    ASCII_LF
    JR    Z, MB_SKPLF
    CP    ASCII_CR
    JR    NZ, MB_NOTQTL
    LD    A, E
    CP    ASCII_SP
    JR    Z, MB_LPCRGT
    CP    ','
    LD    A, ASCII_CR
    JR    Z, MB_LPCRGT
MB_NOTQTL:
    OR    A
    JR    Z, MB_LPCRGT
    CP    D
    JR    Z, MB_QUITSI
    CP    E
    JR    Z, MB_QUITSI
    CALL  MB_STRCHR
MB_LPCRGT:
    CALL  MB_INDSKC
    JR    NC, MB_LOPCRS
MB_QUITSI:
    PUSH  HL
    CP    '"'
    JR    Z, MB_MORSPC
    CP    ASCII_SP
    JR    NZ, MB_NOSKCR
MB_MORSPC:
    CALL  MB_INDSKC
    JR    C, MB_NOSKCR
    CP    ASCII_SP
    JR    Z, MB_MORSPC
    CP    ','
    JR    Z, MB_NOSKCR
    CP    ASCII_CR
    JR    NZ, MB_BAKUPT
MB_ICASLF:
    CALL  MB_INDSKC
    JR    C, MB_NOSKCR
    CP    ASCII_LF
    JR    Z, MB_NOSKCR
MB_BAKUPT:
    LD    HL, (MB_PTRFIL)
    LD    BC, NMLOFS
    ADD   HL, BC
    DEC   (HL)
    INC   HL
    LD    A, (HL)
    AND   0xfd
    LD    (HL), A
MB_NOSKCR:
    POP   HL
MB_DLINE_END:
    LD    (HL), 0
    LD    HL, MB_BUFMIN    ;=0x2C
    LD    A, E
    SUB   ASCII_SP
    JR    Z, MB_NUMIMK
    LD    B, 0
    CALL  MB_STRLT3
    POP   HL
    RET

; --------------------------------------------------
MB_NUMIMK:
    RST   GETYPTR
    PUSH  AF
    RST   CHRGTR
    POP   AF
    PUSH  AF
    CALL  C, MB_FIN
    POP   AF
    CALL  NC, MB_FIN
    POP   HL
    RET

; --------------------------------------------------
MB_STRCHR:
    OR    A
    RET   Z
    LD    (HL), A
    INC   HL
    DEC   B
    RET   NZ
    POP   AF
    JP    MB_DLINE_END

; --------------------------------------------------
MB_BATERR:
    LD    E, ERRBAT                                  ; Bad alloc table
    DB    LD_BC_nn
MB_BDNERR:
    LD    E, ERRBDN                                  ; Bad drive number
    DB    LD_BC_nn
MB_BFLERR:
    LD    E, ERRBFL                                  ; Bad file name
    DB    LD_BC_nn
MB_BTSERR:
    LD    E, ERRBTS                                  ; Bad trk/sec
    DB    LD_BC_nn
MB_DAMERR:
    LD    E, ERRDAM
    DB    LD_BC_nn
MB_DNMERR:
    LD    E, ERRDNM
    DB    LD_BC_nn
MB_DLRERR:
    LD    E, ERRDLR
    DB    LD_BC_nn
MB_FAEERR:
    LD    E, ERRFAE
    DB    LD_BC_nn
MB_FAOERR:
    LD    E, ERRFAO
    DB    LD_BC_nn
MB_DSFERR:
    LD    E, ERRDSF
    DB    LD_BC_nn
MB_FNFERR:
    LD    E, ERRFNF
    DB    LD_BC_nn
MB_ERRFNO:
    LD    E, ERRFNO
    DB    LD_BC_nn
MB_FOVERR:
    LD    E, ERRFOV
    DB    LD_BC_nn
MB_DFLERR:
    LD    E, ERRDFL
    DB    LD_BC_nn
MB_DORERR:
    LD    E, ERRDOF
    DB    LD_BC_nn
MB_BFNERR:
    LD    E, ERRBFN
    DB    LD_BC_nn
MB_IERERR:
    LD    E, ERRIER
    DB    LD_BC_nn
MB_IOEERR:
    LD    E, ERRIOE
    DB    LD_BC_nn
MB_FWPERR:
    LD    E, ERRFWP
    DB    LD_BC_nn
MB_RADERR:
    LD    E, ERRRAD
    DB    LD_BC_nn
MB_RPEERR:
    LD    E, ERRRPE
    DB    LD_BC_nn
MB_SAPERR:
    LD    E, ERRSAP
    DB    LD_BC_nn
MB_SIOERR:
    LD    E, ERRSIO
    XOR   A
    LD    (MB_NLONLY), A
    PUSH  DE
    CALL  MB_CLSFIL
    XOR   A
    CALL  MB_FILOPEN_PREP
    LD    A, 1
    CALL  MB_FILOPEN_PREP
    LD    A, 0x80
    OUT   (AL_FDD_F0), A
    POP   DE
    XOR   A
    JP    MB_ERROR

; --------------------------------------------------
MB_DRIVENO:
    DB   0                                          ; Number of disk drives available on the system (1..8)
MB_MAXFIL:
    DB   0                                          ; Maximum number of files (file-number limit, ≤16)
MB_FILTAB:
    DW   0                                          ; Pointer to the per-file part of the file descriptor table
MB_NULBUF:
    DW   0                                          ; Base pointer of the whole file descriptor table (MB_FIL_DSC_TAB)
MB_PTRFIL1:
    DW   0                                          ; Pointer into the first file's (file #0, system scratch) header block, set once at startup
MB_CURDRV:
    DB   0                                          ; Current drive number
MB_HDR_PTR:
    DW   0                                          ; Pointer to the current drive's DAC header block.
                                                    ; The block holds the drive status/flag bytes and the DAC address.
MB_PTRFIL:
    DW   0                                          ; Pointer to the currently selected file's descriptor block.

MB_DIR_ENTPTR:
    DW   0                                          ; Directory entry pointer (DW)
MB_CHAIN_FP:
    DB   0
MB_DIR_DRVST:
    DB   0                                          ; Directory drive state
MB_FILNAM:
    DB   0
MB_FN_FIRST:
    .9  DB 0
MB_FN_SECOND:
    .9  DB 0
MB_TMP_TRK:
    DB   0
    DB   0
MB_NLONLY:
    DB   0
    DB   0, 0, 0
MB_SECT_ERRCNT:
    DB   0                                          ; Sector operation error/retry counter
MB_SECT_WRFLG:
    DB   0                                          ; Sector operation write flag (from mode bit 6)
    DB   0, 0, 0, 0

; --------------------------------------------------
; FDC main operation dispatcher (read/write/seek/reset)
; --------------------------------------------------
AL_FDC_OP:
    LD    A, 0xa
    PUSH  AF
    ; Deselect drive, wait delay
AL_FDC_DESELECT:
    IN    A, (AL_FDD_F0)
    RLCA
    LD    A, 0xc0
    OUT   (AL_FDD_F0), A
    JR    NC, AL_FDC_MODECHK
    LD    HL, 0
    ; Wait fixed delay after deselect
AL_FDC_WAITDELAY:
    DEC   HL
    LD    A, L
    OR    H
    JR    NZ, AL_FDC_WAITDELAY
    ; Check operation mode (read/write/seek)
AL_FDC_MODECHK:
    LD    A, 0x4
    CALL  AL_FDC_BUILDCMD
    CALL  AL_FDC_READDATA
    LD    C, A
    AND   0x20
    JR    NZ, AL_FDC_RWSEEK_CHK
    CALL  MB_CTRLC_CHK
    JR    Z, AL_FDC_MODECHK
    CALL  MB_TRYOUT2
    CP    0x3
    JR    NZ, AL_FDC_MODECHK
    JP    MB_IOEERR
    ; Read/write/seek dispatch check
AL_FDC_RWSEEK_CHK:
    LD    A, C
    AND   0x40
    JR    Z, AL_FDC_SEEKCHK
    LD    A, (AL_FDC_CMD)
    CP    0x45
    JP    Z, AL_FDC_SETOK
    ; Seek operation check
AL_FDC_SEEKCHK:
    LD    A, (MB_CURDRV)
    LD    C, A
    LD    B, 0
    LD    HL, AL_FDC_TRKCACH      ;=0xFF
    ADD   HL, BC
    LD    A, (HL)       ;=0xFF
    INC   A
    CALL  Z, AL_FDC_DOSEEK
    LD    A, (AL_FDC_TRK)
    CP    (HL)   ;=0xFF
    CALL  NZ, AL_FDC_SEEKDO
    LD    A, 0x44
    OUT   (AL_DMAC_68), A
    LD    HL, (AL_FDC_BUFADDR)
    LD    A, L
    OUT   (AL_DMAC_64), A
    LD    A, H
    OUT   (AL_DMAC_64), A
    LD    A, 0xff
    OUT   (AL_DMAC_65), A
    LD    A, (AL_FDC_CMD)
    LD    C, A
    LD    H, 0x40
    CP    0x46
    JR    Z, AL_FDC_RWSEL
    LD    H, 0x80
    ; Select drive for read/write
AL_FDC_RWSEL:
    LD    A, (AL_FDC_RW_FLG)
    OR    H
    DEC   A
    OUT   (AL_DMAC_65), A
    LD    A, C
    CALL  AL_FDC_BUILDCMD
    LD    HL, AL_FDC_TRK
    LD    B, 0x7
    ; Send track/sector/side params to FDC
AL_FDC_SENDPARAMS:
    LD    A, (HL)
    CALL  AL_FDC_WRITEDATA
    INC   HL
    DJNZ  AL_FDC_SENDPARAMS
    ; Wait for FDC sync (not busy)
AL_FDC_WAITSYNC:
    IN    A, (AL_FDD_F0)
    AND   0x40
    JR    NZ, AL_FDC_WAITSYNC
    LD    HL, AL_FDC_STATBUF0
    LD    B, 0x7
    ; Read status register from FDC
AL_FDC_STATREAD:
    CALL  AL_FDC_READDATA
    LD    (HL), A
    INC   HL
    DJNZ  AL_FDC_STATREAD
    LD    A, (AL_FDC_STATBUF0)
    AND   0xc0
    JR    Z, AL_FDC_DESELRET
    ; Retry check (decrement counter)
AL_FDC_RETRYCHK:
    POP   AF
    DEC   A
    PUSH  AF
    JR    Z, AL_FDC_SETOK
    AND   0x3
    CALL  Z, AL_FDC_HWRST
    JP    AL_FDC_DESELECT
    ; Set OK flag, return success
AL_FDC_SETOK:
    LD    A, 1
    ; Deselect and return
AL_FDC_DESELRET:
    LD    (AL_FDC_RESFLG), A
    LD    A, 0x80
    OUT   (AL_FDD_F0), A
    POP   AF
    LD    A, (AL_FDC_RESFLG)
    RET

; --------------------------------------------------
; Perform seek operation
; --------------------------------------------------
AL_FDC_DOSEEK:
    PUSH  HL
    LD    A, 0x7
    CALL  AL_FDC_BUILDCMD
    CALL  AL_FDC_STATWAIT
    POP   HL
    RET   Z
    ; retry on failure
AL_FDC_SEEK_RETRY:
    POP   HL
    JR    AL_FDC_RETRYCHK

; --------------------------------------------------
; Send seek command, wait
; --------------------------------------------------
AL_FDC_SEEKDO:
    PUSH  HL
    LD    A, 0xf
    CALL  AL_FDC_BUILDCMD
    LD    A, (AL_FDC_TRK)
    CALL  AL_FDC_WRITEDATA
    CALL  AL_FDC_STATWAIT
    POP   HL
    JR    NZ, AL_FDC_SEEK_RETRY
    LD    A, (AL_FDC_TRK)
    LD    (HL), A
    LD    HL, 4000

; --------------------------------------------------
MB_DELAY4K:
    DEC   HL
    LD    A, H
    OR    L
    JR    NZ, MB_DELAY4K
    RET

; --------------------------------------------------
; Wait for FDC status ready
; --------------------------------------------------
AL_FDC_STATWAIT:
    IN    A, (AL_FDD_F0)
    AND   0x40
    JR    NZ, AL_FDC_STATWAIT
    ; Accumulate status bytes
AL_FDC_STATACCUM:
    LD    A, 0x8
    CALL  AL_FDC_WRITEDATA
    CALL  AL_FDC_READDATA
    CP    0x80
    JR    Z, AL_FDC_STATRET
    LD    C, A
    CALL  AL_FDC_READDATA
    LD    A, C
    ADD   A, 0x40
    JR    C, AL_FDC_STATACCUM
    AND   0x80
    JR    Z, AL_FDC_STATSTORE
    LD    A, C
    ; Store status in buffer
AL_FDC_STATSTORE:
    LD    (AL_FDC_STATBUF0), A
    JR    AL_FDC_STATACCUM
    ; Return from status wait
AL_FDC_STATRET:
    LD    A, (AL_FDC_STATBUF0)
    OR    A
    RET

; --------------------------------------------------
; Read data from FDC (DMA)
; --------------------------------------------------
AL_FDC_READDATA:
    IN    A, (FDCST)
    ADD   A, 0x40
    JR    NC, AL_FDC_READDATA
    IN    A, (FDCDT)
    RET

; --------------------------------------------------
; Build command byte (opcode + params)
; --------------------------------------------------
AL_FDC_BUILDCMD:
    CALL  AL_FDC_WRITEDATA
    CP    0x7
    JR    Z, AL_FDC_DRVADD
    LD    A, (AL_FDC_SIDE)
    RRCA
    ; Add drive number to command
AL_FDC_DRVADD:
    LD    A, (MB_CURDRV)
    JR    NC, AL_FDC_WRITEDATA
    OR    0x4

; --------------------------------------------------
; Write data to FDC (DMA)
; --------------------------------------------------
AL_FDC_WRITEDATA:
    CALL  AL_FDC_WAITNOTBUSY
    OUT   (FDCDT), A
    RET

; --------------------------------------------------
; Wait until FDC not busy
; --------------------------------------------------
AL_FDC_WAITNOTBUSY:
    PUSH  AF
    ; Busy poll loop on status port
.BUSYPOLL:
    IN    A, (FDCST)
    RLCA
    JR    NC, .BUSYPOLL
    POP   AF
    RET

; --------------------------------------------------
; Hardware reset FDC
; --------------------------------------------------
AL_FDC_HWRST:
    XOR   A
    OUT   (AL_FDD_F0), A
    LD    HL, 0xc8
    CALL  AL_FDC_WAITLOOP
    LD    A, 0xc0
    OUT   (AL_FDD_F0), A
    LD    A, 0x3
    CALL  AL_FDC_WRITEDATA
    LD    A, 0xcf
    CALL  AL_FDC_WRITEDATA
    LD    A, 0x1c
    CALL  AL_FDC_WRITEDATA
    LD    HL, 0xffff
    LD    (AL_FDC_TRKCACH), HL    ;=0xFF
    RET

; --------------------------------------------------
;  Generic wait loop (delay)
; --------------------------------------------------
AL_FDC_WAITLOOP:
    DEC   HL
    EX    (SP), HL
    EX    (SP), HL
    LD    A, L
    OR    H
    JP    NZ, AL_FDC_WAITLOOP
    RET

; --------------------------------------------------
AL_FDC_TRK:
    DB    0                                         ; FDC parameter: track number
AL_FDC_SIDE:
    DB    0                                         ; FDC parameter: side
AL_FDC_SCT:
    DB    0                                         ; FDC parameter: sector number
    DB    1
    DB    0x10
    DB    0x36
    DB    0xFF
AL_FDC_TRKCACH:
    DW    0xFFFF                                    ; FDC track cache
AL_FDC_CMD:
    DB    0                                         ; FDC command byte
AL_FDC_BUFADDR:
    DW    0                                         ; FDC DMA buffer address
AL_FDC_RW_FLG:
    DB    0                                         ; FDC read/write flag
AL_FDC_RESFLG:
    DB    0                                         ; FDC result/ready flag
AL_FDC_STATBUF0:
    DB    0                                         ; FDC status buffer byte 0
AL_FDC_STATBUF1:
    DB    0                                         ; FDC status buffer byte 1
    .6 DB 0
MB_SCRN_GEO_POS:
    DW    0x1801                                    ; Clamped cursor position (col,row) after LOCATE/CONSOLE
MB_SCRN_ROWLIM:
    DB    1                                         ; Available text row count (set by CONSOLE; reset to 1 by WIDTH)
MB_SCRN_ROWS2:
    DB    24
    DB    0
AL_MENU_FLAG:
    DB    1
MB_COLOR_FGBG:
    DW    0x0007                                    ; Current color pair: FG in low byte, BG in high byte (default 7)
MB_COLOR_ATTR:
    DB    0                                         ; Current COLOR attribute value (default 0)
MB_GFX_CURCLR:
    DB    0                                         ; Current graphics color (set by DEFCLR/PSET)
MB_LINE_STEPPTR1:
    DW    0                                         ; Bresenham step pointer 1
MB_LINE_STEPPTR2:
    DW    0                                         ; Bresenham step pointer 2
MB_LINE_CALLGADGET1:
    DB    0xC3                                      ; CALL nnnn for step fn 1
MB_LINE_CALLTGT1:
    DW    0                                         ; Step fn 1 target address
MB_LINE_CALLGADGET2:
    DB    0xC3                                      ; CALL nnnn for step fn 2
MB_LINE_CALLTGT2:
    DW    0                                         ; Step fn 2 target address
AL_BUF_POS:
    DW    0                                         ; Holding the current read position in the menu bar string

; --------------------------------------------------
MENU_BAR1:
    DB    "GOTO "
    .11   DB 0
    DB    "COLOR "
    .10   DB 0
    DB    "PRINT "
    .10   DB 0
    DB    "LIST "
    .11   DB 0
    DB    "EDIT "
    .11   DB 0
    DB    "RUN\r"
    .12   DB 0

; --------------------------------------------------
MENU_BAR2:
    DB    "GOSUB "
    .10   DB 0
    DB    "LOCATE "
    .9    DB 0
    DB    "INPUT "
    .10   DB 0
    DB    "KEY "
    .12   DB 0
    DB    "AUTO "
    .11   DB 0
    DB    "CONT\r"
    .11   DB 0

; --------------------------------------------------
; Column state table (per-column bit flags for editor)
; --------------------------------------------------
MB_COLSTATE_TBL:
    .25   DB 1
MB_GFX_CURROW:
    DW    0                                         ; Graphics cursor row (accessed via HL)
MB_GFX_CURCOL:
    DB    0                                         ; Graphics cursor column (accessed via A)
    DB    0
MB_EDT_OVRFLG:
    DB    0                                         ; Editor overwrite/insert toggle flag
MB_EDT_COLROW:
    DB    0                                         ; Working cursor position word (col,row) for editor ops
    DB    0

; Keyboard state
; bit - 0
; bit - 1 INS/DEL
; bit - 2 Shift
; bit - 3 Tab
; bit - 4 Enter
; bit - 5 Graph
AL_KEYSTATE:
    DB    0

MB_CS_FNAME:
    DB    0
; Cassette area: reserved bytes (unused)
    .5    DB   0
MB_CAS_FNAMBUF:
    .6 DB 0                                         ; Cassette filename buffer (6-char name)
MB_CL_FLG2:
    DB    0
MB_CL_FLG1:
    DB    0
MB_LINE_X1:
    DW    0                                         ; LINE endpoint X1
MB_LINE_Y1:
    DW    0                                         ; LINE endpoint Y1
MB_GFX_CURX:
    DW    0                                         ; Graphics cursor X
MB_GFX_CURY:
    DW    0                                         ; Graphics cursor Y
MB_CONSOLE_ROWFLG:
    DB    0                                         ; CONSOLE row parameter flag (0 or 1, gates editor path
MB_DIRECT_KEYST:
    DB    0                                         ; Direct mode: last key stored

MB_CNTOFL:
    DB    0

; File descriptor table
MB_FIL_DSC_TAB:
    .4 DB 0

; --------------------------------------------------
; Print copyright/branding message at startup
; --------------------------------------------------
MB_INIT_PRINTCOPYR:
    LD    HL, MB_AUTTXT                             ; "Owned by Microsoft"
    CALL  MB_STROUT

; --------------------------------------------------
MB_INIT:
    LD    HL, MB_TSTACK
    LD    SP, HL
    LD    (MB_TOPMEM), HL                           ;=0x73A2
    LD    SP, HL
    LD    HL, MB_RST_INP                            ;= ':'
    LD    (HL), ':'                                 ;= ':'
    CALL  MB_STKINI
    LD    (MB_TTYPOS), A
    LD    (MB_SAVSTK), HL
    LD    HL, 0xffff
    LD    (MB_CURLIN), HL                           ;=0xFFFE
    CALL  AL_MENUBAR_DRAW
    CALL  MB_CRDO
    XOR   A
    LD    (MB_ENDBUF), A
    LD    (MB_CHNFLG), A
    LD    (MB_MRGFLG), A
    LD    (MB_ERRFLG), A
    LD    HL, 0
    LD    (MB_LPTPOS), HL
    LD    HL, MB_TEMPST
    LD    (MB_TEMPPT), HL
    LD    HL, MB_PRMSTK
    LD    (MB_PRMPRV), HL
    LD    HL, 0xe000                                ; First address of MONITOR
    DEC   HL
    DEC   HL
    LD    (MB_MEMSIZ), HL
    DEC   HL
    PUSH  HL
    LD    A, 2
    LD    (MB_DRIVENO), A

; --------------------------------------------------
MB_GFN_AGN:
    LD    HL, MSG_HOW_MF                            ; "How many files(0-15)"
    CALL  MB_STROUT                                 ; ??
    CALL  MB_GET_NF
    JR    C, MB_GFN_AGN
    RST   CHRGTR
    OR    A
    LD    A, (MB_DRIVENO)
    JR    Z, MB_FN_OK
    CALL  MB_CNSGET
    LD    A, D
    OR    A
    JR    NZ, MB_GFN_AGN
    LD    A, E
    CP    16
    JR    NC, MB_GFN_AGN

; --------------------------------------------------
MB_FN_OK:
    LD    (MB_MAXFIL), A
    LD    HL, MB_FIL_DSC_TAB
    PUSH  HL
    LD    (MB_NULBUF), HL
    LD    A, (MB_DRIVENO)
    LD    B, A
    ADD   A, A
    LD    E, A
    LD    D, 0
    ADD   HL, DE
    LD    (MB_FILTAB), HL
    LD    A, (MB_MAXFIL)
    INC   A
    ADD   A, A
    LD    E, A
    LD    D, 0
    ADD   HL, DE
    POP   DE
    EX    DE, HL
    INC   DE
    INC   DE
    INC   DE
    INC   DE
    INC   DE
    INC   DE
    INC   DE
    INC   DE
    LD    (HL), E
    INC   HL
    LD    (HL), D
    INC   HL
    LD    A, B
    LD    BC, 166
    DEC   A
    JR    Z, MB_INIT_DACLAST

    ; Initialize DAC block (zero fill)
MB_INIT_FILLDAC:
    EX    DE, HL
    ADD   HL, BC
    EX    DE, HL
    LD    (HL), E
    INC   HL
    LD    (HL), D
    INC   HL
    DEC   A
    JR    NZ, MB_INIT_FILLDAC

    ; DAC initialization: last entry marker
MB_INIT_DACLAST:
    EX    DE, HL
    ADD   HL, BC
    EX    DE, HL
    LD    HL, (MB_FILTAB)
    LD    (HL), E
    INC   HL
    LD    (HL), D
    INC   HL
    LD    A, (MB_MAXFIL)
    LD    BC, 0x0109                                   ; 265
    OR    A
    JR    Z, MB_LOPFLB
.MOVNXT:
    EX    DE, HL
    ADD   HL, BC
    EX    DE, HL
    LD    (HL), E
    INC   HL
    LD    (HL), D
    INC   HL
    DEC   A
    JR    NZ, .MOVNXT

; --------------------------------------------------
MB_LOPFLB:
    EX    DE, HL
    ADD   HL, BC
    INC   HL
    PUSH  HL
    DEC   A
    LD    (MB_NLONLY), A
    LD    HL, (MB_FILTAB)
    LD    E, (HL)
    INC   HL
    LD    D, (HL)
    LD    HL, 0x9
    ADD   HL, DE
    LD    (MB_PTRFIL1), HL
    POP   HL
    INC   HL
    LD    (MB_TXTTAB), HL  ;=0x733F
    LD    (MB_SAVSTK), HL
    POP   DE
    LD    A, E
    SUB   L
    LD    L, A
    LD    A, D
    SBC   A, H
    LD    H, A
    JP    C, MB_OMERRR
    LD    B, 0x3

; --------------------------------------------------
MB_SHFLF3:
    OR    A
    LD    A, H
    RRA
    LD    H, A
    LD    A, L
    RRA
    LD    L, A
    DJNZ  MB_SHFLF3
    LD    A, H
    CP    0x2
    JR    C, MB_SMLSTK
    LD    HL, 512

; --------------------------------------------------
MB_SMLSTK:
    LD    A, E
    SUB   L
    LD    L, A
    LD    A, D
    SBC   A, H
    LD    H, A
    JP    C, MB_OMERRR
    LD    (MB_MEMSIZ), HL
    EX    DE, HL
    LD    (MB_TOPMEM), HL                           ;=0x73A2
    LD    (MB_FRETOP), HL
    LD    SP, HL
    LD    (MB_SAVSTK), HL
    LD    HL, (MB_TXTTAB)                           ;=0x733F
    EX    DE, HL
    CALL  MB_REASON
    OR    A
    SBC   HL, DE
    DEC   HL
    DEC   HL
    PUSH  HL
    LD    HL, MB_HEDING                             ;= "BASIC-80 Rev. 5.26\r\n"
    CALL  MB_STROUT
    POP   HL
    CALL  MB_LINPRT
    LD    HL, MB_WORDS                              ;= " Bytes free"
    CALL  MB_STROUT
    LD    HL, MB_STROUT
    LD    (MB_INIADR1), HL                          ;=0x711B
    CALL  MB_CRDO
    LD    HL, MB_READYR
    LD    (READYR_ADDR), HL
    JP    MB_INITSA

; --------------------------------------------------
MSG_HOW_MF:
    DB    "How many files(0-15)", 0

; --------------------------------------------------
MB_AUTTXT:
    DB    "\r\n\nOwned by Microsoft\r\n", 0
    DB    "Memory size", 0

; --------------------------------------------------
MB_WORDS:
    DB    " Bytes free", 0

; --------------------------------------------------
MB_HEDING:
    DB    "BASIC-80 Rev. 5.26\r\nCopyright 1977-1983 (C) "
    DB    "by Microsoft\r\nCreated: 27-Oct-83\r\n", 0
