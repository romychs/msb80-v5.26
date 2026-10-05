; ===================================================
; Microsoft Basic-80 v5.26,  for CP/M OS,  for Triumph
; Adler Alphatronic PC.
;
; BASIC string module
; Disassembled by Romychs @ 2026-09-21
; ===================================================

; --------------------------------------------------
; Compare two strings
;  Inp: DE -> str1
;       FAC -> str2
;  Out: A = 0 - equal
;       A = 0xff - str1 > str2
;       A = 1 - str1 < str2
;
; --------------------------------------------------
MB_STRCMP:
    CALL  MB_FRESTR
    LD    A, (HL)
    INC   HL
    LD    C, (HL)
    INC   HL
    LD    B, (HL)
    POP   DE
    PUSH  BC
    PUSH  AF
    CALL  MB_FRETMP
    POP   AF
    LD    D, A
    LD    E, (HL)
    INC   HL
    LD    C, (HL)
    INC   HL
    LD    B, (HL)
    POP   HL
MB_CSLOOP:
    LD    A, E
    OR    D
    RET   Z
    LD    A, D
    SUB   0x1
    RET   C
    XOR   A
    CP    E
    INC   A
    RET   NC
    DEC   D
    DEC   E
    LD    A, (BC)
    INC   BC
    CP    (HL)
    INC   HL
    JR    Z, MB_CSLOOP
    CCF
    JP    MB_SIGNS

; --------------------------------------------------
; OCT$ function
; --------------------------------------------------
MB_OCT_:
    CALL  MB_FOUTO
    JR    MB_CNV_CMN

; --------------------------------------------------
; HEX$ function
; --------------------------------------------------
MB_HEX_:
    CALL  MB_FOUTH
    JR    MB_CNV_CMN

; --------------------------------------------------
; STR$ function
; --------------------------------------------------
MB_STR_:
    CALL  MB_FOUT
MB_CNV_CMN:
    CALL  MB_STRLIT
    CALL  MB_FREFAC
    LD    BC, MB_FINBCK
    PUSH  BC

; --------------------------------------------------
; Copy string to new temporary string
;  Inp: HL -> source string descriptor
;  Out: DE -> DSCTMP
; --------------------------------------------------
MB_STRCPY:
    LD    A, (HL)
    INC   HL
    PUSH  HL
    CALL  MB_GETSPA
    POP   HL
    LD    C, (HL)
    INC   HL
    LD    B, (HL)
    CALL  MB_STRAD2
    PUSH  HL
    LD    L, A
    CALL  MB_MOVSTR
    POP   DE
    RET

; --------------------------------------------------
; Allocate temporary string of 1 DB
; --------------------------------------------------
MB_STRIN1:
    LD    A, 1

; --------------------------------------------------
; Allocate temporary string
;  Inp: A = stringsize
;  Out: DE -> string,  HL -> descriptor
; --------------------------------------------------
MB_STRINI:
    CALL  MB_GETSPA

; --------------------------------------------------
MB_STRAD2:
    LD    HL, MB_DSCTMP
    PUSH  HL
    LD    (HL), A
    INC   HL
    LD    (HL), E
    INC   HL
    LD    (HL), D
    POP   HL
    RET

; --------------------------------------------------
MB_STRLIT:
    DEC   HL
MB_STRLTI:
    LD    B, '"'
MB_STRLT3:
    LD    D, B
MB_STRLT2:
    PUSH  HL
    LD    C, -1
MB_STRGET:
    INC   HL
    LD    A, (HL)
    INC   C
    OR    A
    JR    Z, MB_STRFIN
    CP    D
    JR    Z, MB_STRFIN
    CP    B
    JR    NZ, MB_STRGET
MB_STRFIN:
    CP    '"'
    CALL  Z, MB_CHRGTR
    PUSH  HL
    LD    A, B
    CP    ','
    JR    NZ, MB_NTTRLS
    INC   C
MB_LPTRLS:
    DEC   C
    JR    Z, MB_NTTRLS
    DEC   HL
    LD    A, (HL)
    CP    ' '
    JR    Z, MB_LPTRLS
MB_NTTRLS:
    POP   HL
    EX    (SP), HL
    INC   HL
    EX    DE, HL
    LD    A, C
    CALL  MB_STRAD2

; --------------------------------------------------
; Push temporary descriptor to temporary desciptor
; heap
; --------------------------------------------------
MB_PUTNEW:
    LD    DE, MB_DSCTMP
    DB    LD_A_n

; --------------------------------------------------
; Put temp string result descriptor to DAC
; --------------------------------------------------
MB_PUTTMP:
    PUSH  DE
    LD    HL, (MB_TEMPPT)
    LD    (MB_DACC+2), HL
    LD    A, TSTR
    LD    (MB_VALTYP), A
    CALL  MB_VMOVE
    LD    DE, MB_FRETOP
    RST   DCOMPR
    LD    (MB_TEMPPT), HL
    POP   HL
    LD    A, (HL)
    RET   NZ
    LD    DE, 0x10
    JP    MB_ERROR

; -------------------------------------------------
; Skip first character,  and out message to
; interpreter output
; -------------------------------------------------
MB_STROUI:
    INC   HL
; --------------------------------------------------
; Out message to interpreter output
; --------------------------------------------------
MB_STROUT:
    CALL  MB_STRLIT
; --------------------------------------------------
; Print string pointed by FAC
; --------------------------------------------------
MB_STRPRT:
    CALL  MB_FREFAC
    CALL  MB_GETSTR
    INC   D
MB_STRPR2:
    DEC   D
    RET   Z
    LD    A, (BC)
    RST   OUTDO
    CP    ASCII_CR
    CALL  Z, MB_CRFIN
    INC   BC
    JR    MB_STRPR2

; ==================================================
; String garbage collection - GETSPA,  GARBAG
; ==================================================

; --------------------------------------------------
; Get space for character string (may force GC)
;  Inp: A - number of symbols
;  Out: DE -> string buffer
; --------------------------------------------------
MB_GETSPA:
    OR    A
    DB    LD_C_n
MB_TRYGI2:
    POP   AF
    PUSH  AF
    LD    HL, (MB_STREND)
    EX    DE, HL
    LD    HL, (MB_FRETOP)
    CPL
    LD    C, A
    LD    B, 0xff
    ADD   HL, BC
    INC   HL
    RST   DCOMPR
    JR    C, MB_GARBAG
    LD    (MB_FRETOP), HL
    INC   HL
    EX    DE, HL
MB_PPSWRT:
    POP   AF
    RET

; --------------------------------------------------
MB_GARBAG:
    POP   AF
    LD    DE, ERRSO
    JP    Z, MB_ERROR
    CP    A
    PUSH  AF
    LD    BC, MB_TRYGI2
    PUSH  BC

; --------------------------------------------------
; Do a garbage collection
; --------------------------------------------------
MB_GARBA2:
    LD    HL, (MB_MEMSIZ)
MB_FNDVAR:
    LD    (MB_FRETOP), HL
    LD    HL, 0x0
    PUSH  HL
    LD    HL, (MB_STREND)
    PUSH  HL
    LD    HL, MB_TEMPST
MB_TVAR:
    LD    DE, (MB_TEMPPT)
    RST   DCOMPR
    LD    BC, MB_TVAR
    JP    NZ, MB_DVAR2
    LD    HL, MB_PRMPRV
    LD    (MB_TEMP9), HL
    LD    HL, (MB_ARYTAB)
    LD    (MB_ARYTA2), HL
    LD    HL, (MB_VARTAB)
MB_SVAR:
    LD    DE, (MB_ARYTA2)
    RST   DCOMPR
    JR    Z, MB_ARYVAR
    LD    A, (HL)
    INC   HL
    INC   HL
    INC   HL
    PUSH  AF
    CALL  MB_IADAHL
    POP   AF
    CP    0x3
    JR    NZ, MB_SKPVAR
    CALL  MB_DVARS
    XOR   A
MB_SKPVAR:
    LD    E, A
    LD    D, 0x0
    ADD   HL, DE
    JR    MB_SVAR
MB_ARYVAR:
    LD    HL, (MB_TEMP9)
    LD    E, (HL)
    INC   HL
    LD    D, (HL)
    LD    A, D
    OR    E
    LD    HL, (MB_ARYTAB)
    JR    Z, MB_ARYVA4
    EX    DE, HL
    LD    (MB_TEMP9), HL
    INC   HL
    INC   HL
    LD    E, (HL)
    INC   HL
    LD    D, (HL)
    INC   HL
    EX    DE, HL
    ADD   HL, DE
    LD    (MB_ARYTA2), HL
    EX    DE, HL
    JR    MB_SVAR
MB_ARYVA2:
    POP   BC
MB_ARYVA4:
    LD    DE, (MB_STREND)
    RST   DCOMPR
    JP    Z, MB_GRBPAS
    LD    A, (HL)
    INC   HL
    PUSH  AF
    INC   HL
    INC   HL
    CALL  MB_IADAHL
    LD    C, (HL)
    INC   HL
    LD    B, (HL)
    INC   HL
    POP   AF
    PUSH  HL
    ADD   HL, BC
    CP    0x3
    JR    NZ, MB_ARYVA2
    LD    (MB_CPMFIL), HL
    POP   HL
    LD    C, (HL)
    LD    B, 0x0
    ADD   HL, BC
    ADD   HL, BC
    INC   HL
.ARYSTR:
    EX    DE, HL
    LD    HL, (MB_CPMFIL)
    EX    DE, HL
    RST   DCOMPR
    JR    Z, MB_ARYVA4
    LD    BC, .ARYSTR
MB_DVAR2:
    PUSH  BC
; --------------------------------------------------
MB_DVARS:
    XOR   A
    OR    (HL)
    INC   HL
    LD    E, (HL)
    INC   HL
    LD    D, (HL)
    INC   HL
    RET   Z
    LD    B, H
    LD    C, L
    LD    HL, (MB_FRETOP)
    RST   DCOMPR
    LD    H, B
    LD    L, C
    RET   C
    POP   HL
    EX    (SP), HL
    RST   DCOMPR
    EX    (SP), HL
    PUSH  HL
    LD    H, B
    LD    L, C
    RET   NC
    POP   BC
    POP   AF
    POP   AF
    PUSH  HL
    PUSH  DE
    PUSH  BC
    RET

; One complete pass thru str vars
; --------------------------------------------------
MB_GRBPAS:
    POP   DE
    POP   HL
    LD    A, H
    OR    L
    RET   Z
    DEC   HL
    LD    B, (HL)
    DEC   HL
    LD    C, (HL)
    PUSH  HL
    DEC   HL
    LD    L, (HL)
    LD    H, 0x0
    ADD   HL, BC
    LD    D, B
    LD    E, C
    DEC   HL
    LD    B, H
    LD    C, L
    LD    HL, (MB_FRETOP)
    CALL  MB_BLTUC
    POP   HL
    LD    (HL), C
    INC   HL
    LD    (HL), B
    LD    H, B
    LD    L, C
    DEC   HL
    JP    MB_FNDVAR

; --------------------------------------------------
; String concat
; FAC + HL->str
; --------------------------------------------------
MB_CAT:
    PUSH  BC
    PUSH  HL
    LD    HL, (MB_DACC+2)
    EX    (SP), HL
    CALL  MB_EVAL
    EX    (SP), HL
    CALL  MB_CHKSTR
    LD    A, (HL)
    PUSH  HL
    LD    HL, (MB_DACC+2)
    PUSH  HL
    ADD   A, (HL)
    LD    DE, 0xf
    JP    C, MB_ERROR
    CALL  MB_STRINI
    POP   DE
    CALL  MB_FRETMP
    EX    (SP), HL
    CALL  MB_FRETM2
    PUSH  HL
    LD    HL, (MB_DSCTMP+1)
    EX    DE, HL
    CALL  MB_MOVINS
    CALL  MB_MOVINS
    LD    HL, MB_TSTOP
    EX    (SP), HL
    PUSH  HL
    JP    MB_PUTNEW

; --------------------------------------------------
; Copy string (descriptor on stack)
;  Inp: string descriptor on stack
;       DE -> destination string
; --------------------------------------------------
MB_MOVINS:
    POP   HL
    EX    (SP), HL
    LD    A, (HL)
    INC   HL
    LD    C, (HL)
    INC   HL
    LD    B, (HL)
    LD    L, A

; --------------------------------------------------
MB_MOVSTR:
    INC   L
MB_MOVLP:
    DEC   L
    RET   Z
    LD    A, (BC)
    LD    (DE), A
    INC   BC
    INC   DE
    JR    MB_MOVLP

; --------------------------------------------------
; Free temporary string with type check
;  Inp: VALTYP - Type (if not string type,  an error occurs)
;       DE -> string descriptor
;  Out: HL -> string descriptor
; --------------------------------------------------
MB_FRESTR:
    CALL  MB_CHKSTR
MB_FREFAC:
    LD    HL, (MB_DACC+2)
MB_FRETM2:
    EX    DE, HL
;Freeup temporary
MB_FRETMP:
    CALL  MB_FRETMS
    EX    DE, HL
    RET   NZ
    PUSH  DE
    LD    D, B
    LD    E, C
    DEC   DE
    LD    C, (HL)
    LD    HL, (MB_FRETOP)
    RST   DCOMPR
    JR    NZ, MB_NOTLST
    LD    B, A
    ADD   HL, BC
    LD    (MB_FRETOP), HL
MB_NOTLST:
    POP   HL
    RET

; --------------------------------------------------
; Free descriptor if temporary and on top of heap
;  Inp: DE -> descriptor
; --------------------------------------------------
MB_FRETMS:
    LD    HL, (MB_TEMPPT)
    DEC   HL
    LD    B, (HL)
    DEC   HL
    LD    C, (HL)
    DEC   HL
    RST   DCOMPR
    RET   NZ
    LD    (MB_TEMPPT), HL
    RET

; --------------------------------------------------
; LEN($) Function
; --------------------------------------------------
MB_LEN:
    LD    BC, MB_SNGFLT
    PUSH  BC
; --------------------------------------------------
; Free temporary string and get size
; --------------------------------------------------
MB_LEN1:
    CALL  MB_FRESTR
    XOR   A
    LD    D, A
    LD    A, (HL)
    OR    A
    RET

; --------------------------------------------------
; ASC Function
; --------------------------------------------------
MB_ASC:
    LD    BC, MB_SNGFLT
    PUSH  BC

; --------------------------------------------------
; Free temporary string and get first character
; --------------------------------------------------
MB_ASC2:
    CALL  MB_LEN1
    JP    Z, MB_FCERR
    INC   HL
    LD    E, (HL)
    INC   HL
    LD    D, (HL)
    LD    A, (DE)
    RET

; --------------------------------------------------
; CHR$ function
; --------------------------------------------------
MB_CHR_:
    CALL  MB_STRIN1
    CALL  MB_CONINT

; --------------------------------------------------
; Set first character of temporary string and put
; it on heap
; --------------------------------------------------
MB_SETSTR:
    LD    HL, (MB_DSCTMP+1)     ;MB_DSCTMP+INCSTR
    LD    (HL), E

; --------------------------------------------------
MB_FINBCK:
    POP   BC
    JP    MB_PUTNEW

; --------------------------------------------------
; STRING$ function
; --------------------------------------------------
MB_STRNG_:
    RST   CHRGTR
    RST   SYNCHR
    DB    '('
    CALL  MB_GETBYT
    PUSH  DE
    RST   SYNCHR
    DB    ','
    CALL  MB_FRMEVL
    RST   SYNCHR
    DB    ')'
    EX    (SP), HL
    PUSH  HL
    RST   GETYPTR
    JR    Z, MB_STRSTR
    CALL  MB_CONINT
    JR    MB_CALSPA
MB_STRSTR:
    CALL  MB_ASC2
MB_CALSPA:
    POP   DE
    CALL  MB_SPACE2

; --------------------------------------------------
; SPACE$ function
; --------------------------------------------------
MB_SPACE_:
    CALL  MB_CONINT
    LD    A, ASCII_SP

; --------------------------------------------------
MB_SPACE2:
    PUSH  AF
    LD    A, E
    CALL  MB_STRINI
    LD    B, A
    POP   AF
    INC   B
    DEC   B
    JR    Z, MB_FINBCK
    LD    HL, (MB_DSCTMP+1)
MB_SPLP_:
    LD    (HL), A
    INC   HL
    DJNZ  MB_SPLP_
    JR    MB_FINBCK

; --------------------------------------------------
; LEFT$ function
; --------------------------------------------------
MB_LEFT_:
    CALL  MB_PREAM
    XOR   A
MB_LEFT3:
    EX    (SP), HL
    LD    C, A
    DB    LD_A_n    ;LD A, n

; --------------------------------------------------
MB_LEFTUS:
    PUSH  HL
MB_LEFT2:
    PUSH  HL
    LD    A, (HL)
    CP    B
    JR    C, MB_ALLSTR
    LD    A, B
    DB    LD_DE_nn  ;LD DE, nn

; --------------------------------------------------
MB_ALLSTR:
    LD    C, 0x0
    PUSH  BC
    CALL  MB_GETSPA
    POP   BC
    POP   HL
    PUSH  HL
    INC   HL
    LD    B, (HL)
    INC   HL
    LD    H, (HL)
    LD    L, B
    LD    B, 0x0
    ADD   HL, BC
    LD    B, H
    LD    C, L
    CALL  MB_STRAD2
    LD    L, A
    CALL  MB_MOVSTR
    POP   DE
    CALL  MB_FRETMP
    JP    MB_PUTNEW

; --------------------------------------------------
; RIGHT$ function
; --------------------------------------------------
MB_RIGHT_:
    CALL  MB_PREAM
    POP   DE
    PUSH  DE
    LD    A, (DE)
    SUB   B
    JR    MB_LEFT3

; --------------------------------------------------
; MID$ function
; --------------------------------------------------
MB_MID_:
    EX    DE, HL
    LD    A, (HL)
    CALL  MB_PREAM2
    INC   B
    DEC   B
    JP    Z, MB_FCERR
    PUSH  BC
    CALL  MB_MIDRST
    POP   AF
    EX    (SP), HL
    LD    BC, MB_LEFT2
    PUSH  BC
    DEC   A
    CP    (HL)
    LD    B, 0x0
    RET   NC
    LD    C, A
    LD    A, (HL)
    SUB   C
    CP    E
    LD    B, A
    RET   C
    LD    B, E
    RET

; --------------------------------------------------
; VAL function
; --------------------------------------------------
MB_VAL:
    CALL  MB_LEN1
    JP    Z, MB_SNGFLT
    LD    E, A
    INC   HL
    LD    A, (HL)
    INC   HL
    LD    H, (HL)
    LD    L, A
    PUSH  HL
    ADD   HL, DE
    LD    B, (HL)
    LD    (HL), D
    EX    (SP), HL
    PUSH  BC
    DEC   HL
    RST   CHRGTR
    CALL  MB_FIN
    POP   BC
    POP   HL
    LD    (HL), B
    RET

; --------------------------------------------------
; Test and set left$ and right$ fn parameters
; --------------------------------------------------
MB_PREAM:
    EX    DE, HL
    RST   SYNCHR
    DB    ')'

; --------------------------------------------------
; Test and set mid$ fn parameters
; --------------------------------------------------
MB_PREAM2:
    POP   BC
    POP   DE
    PUSH  BC
    LD    B, E
    RET

; --------------------------------------------------
; INSTR function
; --------------------------------------------------
MB_INSTR:
    RST   CHRGTR
    CALL  MB_FRMPRN
    RST   GETYPTR
    LD    A, 1
    PUSH  AF
    JR    Z, MB_WUZSTR     ;Was a string
    POP   AF
    CALL  MB_CONINT
    OR    A
    JP    Z, MB_FCERR
    PUSH  AF
    RST   SYNCHR
    DB    ','
    CALL  MB_FRMEVL
    CALL  MB_CHKSTR
MB_WUZSTR:
    RST   SYNCHR
    DB    ','
    PUSH  HL
    LD    HL, (MB_DACC+2)
    EX    (SP), HL
    CALL  MB_FRMEVL
    RST   SYNCHR
    DB    ')'
    PUSH  HL
    CALL  MB_FRESTR
    EX    DE, HL
    POP   BC
    POP   HL
    POP   AF
    PUSH  BC
    LD    BC, MB_POPHLRT
    PUSH  BC
    LD    BC, MB_SNGFLT
    PUSH  BC
    PUSH  AF
    PUSH  DE
    CALL  MB_FRETM2
    POP   DE
    POP   AF
    LD    B, A
    DEC   A
    LD    C, A
    CP    (HL)
    LD    A, 0x0
    RET   NC
    LD    A, (DE)
    OR    A
    LD    A, B
    RET   Z
    LD    A, (HL)
    INC   HL
    LD    B, (HL)
    INC   HL
    LD    H, (HL)
    LD    L, B
    LD    B, 0x0
    ADD   HL, BC
    SUB   C
    LD    B, A
    PUSH  BC
    PUSH  DE
    EX    (SP), HL
    LD    C, (HL)
    INC   HL
    LD    E, (HL)
    INC   HL
    LD    D, (HL)
    POP   HL
MB_MID_CHK1:
    PUSH  HL
    PUSH  DE
    PUSH  BC
MB_MID_CHK3:
    LD    A, (DE)
    CP    (HL)
    JR    NZ, MB_OHWELL
    INC   DE
    DEC   C
    JR    Z, MB_GOTSTR
    INC   HL
    DJNZ  MB_MID_CHK3
    POP   DE
    POP   DE
    POP   BC
MB_RETZR:
    POP   DE
    XOR   A
    RET
MB_GOTSTR:
    POP   HL
    POP   DE
    POP   DE
    POP   BC
    LD    A, B
    SUB   H
    ADD   A, C
    INC   A
    RET
MB_OHWELL:
    POP   BC
    POP   DE
    POP   HL
    INC   HL
    DJNZ  MB_MID_CHK1
    JR    MB_RETZR

; --------------------------------------------------
; MID$ statement
; --------------------------------------------------
MB_MIDFN:
    RST   SYNCHR
    DB    '('
    CALL  MB_PTRGET
    CALL  MB_CHKSTR
    PUSH  HL
    PUSH  DE
    EX    DE, HL
    INC   HL
    LD    E, (HL)
    INC   HL
    LD    D, (HL)
    LD    HL, (MB_STREND)
    RST   DCOMPR
    JR    C, MB_NCPMID
    LD    HL, (MB_TXTTAB)  ;=0x733F
    RST   DCOMPR
    JR    NC, MB_NCPMID
    POP   HL
    PUSH  HL
    CALL  MB_STRCPY
    POP   HL
    PUSH  HL
    CALL  MB_VMOVE
MB_NCPMID:
    POP   HL
    EX    (SP), HL
    RST   SYNCHR
    DB    ','
    CALL  MB_GETBYT
    OR    A
    JP    Z, MB_FCERR
    PUSH  AF
    LD    A, (HL)
    CALL  MB_MIDRST
    PUSH  DE
    CALL  MB_FRMEQL
    PUSH  HL
    CALL  MB_FRESTR
    EX    DE, HL
    POP   HL
    POP   BC
    POP   AF
    LD    B, A
    EX    (SP), HL
    PUSH  HL
    LD    HL, MB_POPHLRT
    EX    (SP), HL
    LD    A, C
    OR    A
    RET   Z
    LD    A, (HL)
    SUB   B
    JP    C, MB_FCERR
    INC   A
    CP    C
    JR    C, MB_BIGLEN
    LD    A, C

; --------------------------------------------------
MB_BIGLEN:
    LD    C, B
    DEC   C
    LD    B, 0x0
    PUSH  DE
    INC   HL
    LD    E, (HL)
    INC   HL
    LD    H, (HL)
    LD    L, E
    ADD   HL, BC
    LD    B, A
    POP   DE
    EX    DE, HL
    LD    C, (HL)
    INC   HL
    LD    A, (HL)
    INC   HL
    LD    H, (HL)
    LD    L, A
    EX    DE, HL
    LD    A, C
    OR    A
    RET   Z

; --------------------------------------------------
MB_MID_LP:
    LD    A, (DE)
    LD    (HL), A
    INC   DE
    INC   HL
    DEC   C
    RET   Z
    DJNZ  MB_MID_LP
    RET

; --------------------------------------------------
MB_MIDRST:
    LD    E, 0xff
    CP    0x29
    JR    Z, MB_MID2
    RST   SYNCHR
    INC   L
    CALL  MB_GETBYT
MB_MID2:
    RST   SYNCHR
    DB    ')'
    RET

; --------------------------------------------------
MB_FRE:
    RST   GETYPTR
    JP    NZ, MB_CLCDIF
    CALL  MB_FREFAC
    CALL  MB_GARBA2

MB_CLCDIF:
    LD    DE, (MB_STREND)
    LD    HL, (MB_FRETOP)
    JP    MB_GIVDBL
