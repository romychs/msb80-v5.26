; ===================================================
; Microsoft Basic-80 v5.26, for CP/M OS, for Triumph
; Adler Alphatronic PC.
;
; Operator and function dispatch tables
; Disassembled by Romychs @ 2026-09-16
; ===================================================

; ---------------------------------------------------
; Statement dispatch table
; ---------------------------------------------------
MB_STMDSP:
    DW          MB_ENDS
    DW          MB_FOR
    DW          MB_NEXT
    DW          MB_DATAS
    DW          MB_INPUT
    DW          MB_DIM
    DW          MB_READ
    DW          MB_LET
    DW          MB_GOTO
    DW          MB_RUN
    DW          MB_IFS
    DW          MB_RESTORE
    DW          MB_GOSUB
    DW          MB_RETURN
    DW          MB_REM_ELSE
; 0x90..0x9F ---------------------------------------
    DW          MB_STOP
    DW          MB_PRINT
    DW          MB_CLEAR
    DW          MB_LIST
    DW          MB_NEWS
    DW          MB_ONGOTO
    DW          MB_FNWAIT
    DW          MB_DEF
    DW          MB_POKE
    DW          MB_CONT
    DW          MB_CSAVE
    DW          MB_CLOAD
    DW          MB_FNOUT
    DW          MB_LPRINT
    DW          MB_LLIST
    DW          MB_KEY
; 0xA0..0xAF ---------------------------------------
    DW          MB_WIDTH
    DW          MB_REM_ELSE
    DW          MB_TON
    DW          MB_TOFF
    DW          MB_SWAP
    DW          MB_ERASE
    DW          MB_EDIT
    DW          MB_ERRORS
    DW          MB_RESUME
    DW          MB_DELETE
    DW          MB_AUTO
    DW          MB_RESEQ                             ; RENUM
    DW          MB_DEFSTR
    DW          MB_DEFINT
    DW          MB_DEFSNG
    DW          MB_DEFDBL
; 0xB0..0xBF ---------------------------------------
    DW          MB_LINE
    DW          MB_WHILE
    DW          MB_WEND
    DW          MB_CALLS
    DW          MB_BEEP
    DW          MB_MON
    DW          0                                    ; none
    DW          MB_WRITE
    DW          MB_DATAS                             ; ex MB_COMMON
    DW          MB_CHAIN
    DW          MB_OPTION
    DW          MB_RANDOM
    DW          MB_DSKOFN
    DW          MB_OPEN
    DW          MB_FIELD
    DW          MB_GET
; 0xC0..0xCF ---------------------------------------
    DW          MB_PUT
    DW          MB_SET
    DW          MB_CLOSE
    DW          MB_LOAD
    DW          MB_MERGE
    DW          MB_FILES
    DW          MB_NAME
    DW          MB_KILL
    DW          MB_LSET
    DW          MB_RSET
    DW          MB_SAVE
    DW          MB_LFILES
    DW          MB_PSET
    DW          MB_PRESET
    DW          MB_COLOR
    DW          MB_LOCATE
; 0xD0..0xDF ---------------------------------------
    DW          MB_CLS
    DW          MB_CONSOLE

; ---------------------------------------------------
; Function dispatch table
; ---------------------------------------------------
; 0x81..0x8F
MB_FUNDSP:
    DW          MB_LEFT_
    DW          MB_RIGHT_
    DW          MB_MID_
    DW          MB_SGN
    DW          MB_INT
    DW          MB_ABS
    DW          MB_SQR
    DW          MB_RND
    DW          MB_SIN
    DW          MB_LOG
    DW          MB_EXP
    DW          MB_COS
    DW          MB_TAN
    DW          MB_ATN
    DW          MB_FRE
; 0x90..0x9F
    DW          MB_FNINP
    DW          MB_POS
    DW          MB_LEN
    DW          MB_STR_
    DW          MB_VAL
    DW          MB_ASC
    DW          MB_CHR_
    DW          MB_PEEK
    DW          MB_SPACE_
    DW          MB_OCT_
    DW          MB_HEX_
    DW          MB_LPOS
    DW          MB_FRCINT
    DW          MB_FRCSNG
    DW          MB_FRCDBL
    DW          MB_FIXER
; 0xA0..0xA2
    DW          MB_CVI
    DW          MB_CVS
    DW          MB_CVD

; --------------------------------------------------
; Disk functions
; --------------------------------------------------
; 0xA3..0xA7
    DW          MB_DSKF
    DW          MB_EOF
    DW          MB_LOC
    DW          MB_LOF
    DW          MB_FPOS
; --------------------------------------------------
; Conversion functions
; --------------------------------------------------
; 0xA8..0xAA
    DW          MB_MKI_
    DW          MB_MKS_
    DW          MB_MKD_


; ---------------------------------------------------
; Alphabetic operator and function dispatch table
; ---------------------------------------------------
MB_ALPTAB:
    DW          MB_ATAB
    DW          MB_BTAB
    DW          MB_CTAB
    DW          MB_DTAB
    DW          MB_ETAB
    DW          MB_FTAB
    DW          MB_GTAB
    DW          MB_HTAB
    DW          MB_ITAB
    DW          MB_JTAB
    DW          MB_KTAB
    DW          MB_LTAB
    DW          MB_MTAB
    DW          MB_NTAB
    DW          MB_OTAB
    DW          MB_PTAB
    DW          MB_QTAB
    DW          MB_RTAB
    DW          MB_STAB
    DW          MB_TTAB
    DW          MB_UTAB
    DW          MB_VTAB
    DW          MB_WTAB
    DW          MB_XTAB
    DW          MB_YTAB
    DW          MB_ZTAB

; ---------------------------------------------------
; Alphabetic character by character
; operator and function table
; ---------------------------------------------------

; ---------------------------------------------------
MB_ATAB:
    DB    "UT",     'O' + 0x80, _AUTO
    DB    "N",      'D' + 0x80, _AND
    DB    "B",      'S' + 0x80, _ABS - 0x80
    DB    "T",      'N' + 0x80, _ATN - 0x80
    DB    "S",      'C' + 0x80, _ASC - 0x80
    DB    "TTR",    '$' + 0x80, _ATTR_
    DB    0
; ---------------------------------------------------
MB_BTAB:
    DB    "EE",     'P' + 0x80, _BEEP
    DB    0
; ---------------------------------------------------
MB_CTAB:
    DB    "OLO",    'R' + 0x80, _COLOR
    DB    "ONSOL",  'E' + 0x80, _CONSOLE
    DB    "LOS",    'E' + 0x80, _CLOSE
    DB    "ON",     'T' + 0x80, _CONT
    DB    "LEA",    'R' + 0x80, _CLEAR
    DB    "LOA",    'D' + 0x80, _CLOAD
    DB    "SAV",    'E' + 0x80, _CSAVE
    DB    "SRLI",   'N' + 0x80, _CSRLIN
    DB    "IN",     'T' + 0x80, _CINT - 0x80
    DB    "SN",     'G' + 0x80, _CSNG - 0x80
    DB    "DB",     'L' + 0x80, _CDBL - 0x80
    DB    "V",      'I' + 0x80, _CVI - 0x80
    DB    "V",      'S' + 0x80, _CVS - 0x80
    DB    "V",      'D' + 0x80, _CVD - 0x80
    DB    "O",      'S' + 0x80, _COS - 0x80
    DB    "HR",     '$' + 0x80, _CHR_ - 0x80
    DB    "AL",     'L' + 0x80, _CALL
    DB    "OMMO",   'N' + 0x80, _COMMON
    DB    "HAI",    'N' + 0x80, _CHAIN
    DB    "L",      'S' + 0x80, _CLS
    DB    0
; ---------------------------------------------------
MB_DTAB:
    DB    "ELET",   'E' + 0x80, _DELETE
    DB    "AT",     'A' + 0x80, _DATA
    DB    "I",      'M' + 0x80, _DIM
    DB    "EFST",   'R' + 0x80, _DEFSTR
    DB    "EFIN",   'T' + 0x80, _DEFINT
    DB    "EFSN",   'G' + 0x80, _DEFSNG
    DB    "EFDB",   'L' + 0x80, _DEFDBL
    DB    "SKO",    '$' + 0x80, _DSKO_
    DB    "E",      'F' + 0x80, _DEF
    DB    "SKI",    '$' + 0x80, _DSKI_
    DB    "SK",     'F' + 0x80, _DSKF - 0x80
    DB    0
; ---------------------------------------------------
MB_ETAB:
    DB    "LS",     'E' + 0x80, _ELSE
    DB    "N",      'D' + 0x80, _END
    DB    "RAS",    'E' + 0x80, _ERASE
    DB    "DI",     'T' + 0x80, _EDIT
    DB    "RRO",    'R' + 0x80, _ERROR
    DB    "R",      'L' + 0x80, _ERL
    DB    "R",      'R' + 0x80, _ERR
    DB    "X",      'P' + 0x80, _EXP - 0x80
    DB    "O",      'F' + 0x80, _EOF - 0x80
    DB    "Q",      'V' + 0x80, _EQV
    DB    0
; ---------------------------------------------------
MB_FTAB:
    DB    "O",      'R' + 0x80, _FOR
    DB    "IEL",    'D' + 0x80, _FIELD
    DB    "ILE",    'S' + 0x80, _FILES
    DB              'N' + 0x80, _FN
    DB    "R",      'E' + 0x80, _FRE - 0x80
    DB    "I",      'X' + 0x80, _FIX - 0x80
    DB    "PO",     'S' + 0x80, _FPOS - 0x80
    DB    0
; ---------------------------------------------------
MB_GTAB:
    DB    "OT",     'O' + 0x80, _GOTO
    DB    "O T",    'O' + 0x80, _GOTO
    DB    "OSU",    'B' + 0x80, _GOSUB
    DB    "E",      'T' + 0x80, _GET
    DB    0
; ---------------------------------------------------
MB_HTAB:
    DB    "EX",     '$' + 0x80, _HEX_ - 0x80
    DB    0
; ---------------------------------------------------
MB_ITAB:
    DB    "NPU",    'T' + 0x80, _INPUT
    DB              'F' + 0x80, _IF
    DB    "NST",    'R' + 0x80, _INSTR
    DB    "N",      'T' + 0x80, _INT - 0x80
    DB    "N",      'P' + 0x80, _INP - 0x80
    DB    "M",      'P' + 0x80, _IMP
    DB    "NKEY",   '$' + 0x80, _INKEY
    DB    0
; ---------------------------------------------------
MB_JTAB:
    DB    0
; ---------------------------------------------------
MB_KTAB:
    DB    "IL",     'L' + 0x80, _KILL
    DB    "E",      'Y' + 0x80, _KEY
    DB    0
; ---------------------------------------------------
MB_LTAB:
    DB    "OCAT",   'E' + 0x80, _LOCATE
    DB    "PRIN",   'T' + 0x80, _LPRINT
    DB    "LIS",    'T' + 0x80, _LLIST
    DB    "PO",     'S' + 0x80, _LPOS - 0x80
    DB    "E",      'T' + 0x80, _LET
    DB    "IN",     'E' + 0x80, _LINE
    DB    "OA",     'D' + 0x80, _LOAD
    DB    "SE",     'T' + 0x80, _LSET
    DB    "IS",     'T' + 0x80, _LIST
    DB    "FILE",   'S' + 0x80, _LFILES
    DB    "O",      'G' + 0x80, _LOG - 0x80
    DB    "O",      'C' + 0x80, _LOC - 0x80
    DB    "E",      'N' + 0x80, _LEN - 0x80
    DB    "EFT",    '$' + 0x80, _LEFT_ - 0x80
    DB    "O",      'F' + 0x80, _LOF - 0x80
    DB    0
; ---------------------------------------------------
MB_MTAB:
    DB    "ERG",    'E' + 0x80, _MERGE
    DB    "O",      'D' + 0x80, _MOD
    DB    "KI",     '$' + 0x80, _MKI_ - 0x80
    DB    "KS",     '$' + 0x80, _MKS_ - 0x80
    DB    "KD",     '$' + 0x80, _MKD_ - 0x80
    DB    "ID",     '$' + 0x80, _MID_ - 0x80
    DB    "O",      'N' + 0x80, _MON
    DB    0
; ---------------------------------------------------
MB_NTAB:
    DB    "EX",     'T' + 0x80, _NEXT
    DB    "AM",     'E' + 0x80, _NAME
    DB    "E",      'W' + 0x80, _NEW                 ; alias for SCRATCH
    DB    "O",      'T' + 0x80, _NOT
    DB    0
; ---------------------------------------------------
MB_OTAB:
    DB    "PE",     'N' + 0x80, _OPEN
    DB    "U",      'T' + 0x80, _OUT
    DB              'N' + 0x80, _ON
    DB              'R' + 0x80, _OR
    DB    "CT",     '$' + 0x80, _OCT_ - 0x80
    DB    "PTIO",   'N' + 0x80, _OPTION
    DB    0
; ---------------------------------------------------
MB_PTAB:
    DB    "RIN",    'T' + 0x80, _PRINT
    DB    "U",      'T' + 0x80, _PUT
    DB    "OK",     'E' + 0x80, _POKE
    DB    "O",      'S' + 0x80, _POS - 0x80
    DB    "EE",     'K' + 0x80, _PEEK - 0x80
    DB    "SE",     'T' + 0x80, _PSET
    DB    "RESE",   'T' + 0x80, _PRESET
    DB    "OIN",    'T' + 0x80, _POINT
    DB    0
; ---------------------------------------------------
MB_QTAB:
    DB    0
; ---------------------------------------------------
MB_RTAB:
    DB    "ETUR",   'N' + 0x80, _RETURN
    DB    "EA",     'D' + 0x80, _READ
    DB    "U",      'N' + 0x80, _RUN
    DB    "ESTOR",  'E' + 0x80, _RESTORE
    DB    "E",      'M' + 0x80, _REM
    DB    "ESUM",   'E' + 0x80, _RESUME
    DB    "SE",     'T' + 0x80, _RSET
    DB    "IGHT",   '$' + 0x80, _RIGHT_ - 0x80
    DB    "N",      'D' + 0x80, _RND - 0x80
    DB    "ENU",    'M' + 0x80, _RENUM
    DB    "ANDOMIZ",'E' + 0x80, _RANDOMIZE
    DB    0
; ---------------------------------------------------
MB_STAB:
    DB    "TO",     'P' + 0x80, _STOP
    DB    "WA",     'P' + 0x80, _SWAP
    DB    "E",      'T' + 0x80, _SET
    DB    "AV",     'E' + 0x80, _SAVE
    DB    "PC",     '(' + 0x80, _SPC
    DB    "TE",     'P' + 0x80, _STEP
    DB    "G",      'N' + 0x80, _SGN - 0x80
    DB    "Q",      'R' + 0x80, _SQR - 0x80
    DB    "I",      'N' + 0x80, _SIN - 0x80
    DB    "TR",     '$' + 0x80, _STR_ - 0x80
    DB    "TRING",  '$' + 0x80, _STRING_
    DB    "PACE",   '$' + 0x80, _SPACE_
    DB    0
; ---------------------------------------------------
MB_TTAB:
    DB    "HE",     'N' + 0x80, _THEN
    DB    "RO",     'N' + 0x80, _TRON
    DB    "ROF",    'F' + 0x80, _TROFF
    DB    "AB",     '(' + 0x80, _TAB
    DB              'O' + 0x80, _TO
    DB    "A",      'N' + 0x80, _TAN - 0x80
    DB    0
; ---------------------------------------------------
MB_UTAB:
    DB    "SIN",    'G' + 0x80, _USING
    DB    "S",      'R' + 0x80, _USR
    DB    0
; ---------------------------------------------------
MB_VTAB:
    DB    "A",      'L' + 0x80, _VAL - 0x80
    DB    "ARPT",   'R' + 0x80, _VARPTR
    DB    0
; ---------------------------------------------------
MB_WTAB:
    DB    "IDT",    'H' + 0x80, _WIDTH
    DB    "AI",     'T' + 0x80, _WAIT
    DB    "HIL",    'E' + 0x80, _WHILE
    DB    "EN",     'D' + 0x80, _WEND
    DB    "RIT",    'E' + 0x80, _WRITE
    DB    0
; ---------------------------------------------------
MB_XTAB:
    DB    "O",      'R' + 0x80, _XOR
    DB    0
; ---------------------------------------------------
MB_YTAB:
    DB    0
; ---------------------------------------------------
MB_ZTAB:
    DB    0
; ---------------------------------------------------

; ---------------------------------------------------
; Special operators table
; ---------------------------------------------------
MB_SPCTAB
    DB    '+' + 0x80
    DB    PLUSTK
    DB    '-' + 0x80
    DB    MINUTK
    DB    '*' + 0x80
    DB    MULTK
    DB    '/' + 0x80
    DB    DIVTK
    DB    '^' + 0x80
    DB    EXPTK
    DB    ASCII_BKSL + 0x80
    DB    IDIVTK
    DB    ASCII_SQ + 0x80
    DB    SNGQTK
    DB    '>' + 0x80
    DB    GREATK
    DB    '=' + 0x80
    DB    EQULTK
    DB    '<' + 0x80
    DB    LESSTK
    DB    0

; ---------------------------------------------------
; Math precedences table
; ---------------------------------------------------
MB_OPTAB:
    DB    121                                       ; +
    DB    121                                       ; -
    DB    124                                       ; *
    DB    124                                       ; /
    DB    127                                       ; ^
    DB    80                                        ; OR
    DB    70                                        ; AND
    DB    60                                        ; XOR
    DB    50                                        ; EQV
    DB    40                                        ; IMP
    DB    122                                       ; MOD
    DB    123                                       ; IDIV

; ---------------------------------------------------
MB_FRCTBL:
    DW    MB_FRCDBL
    DW    0
    DW    MB_FRCINT
    DW    MB_CHKSTR
    DW    MB_FRCSNG

; ---------------------------------------------------
; Double real (8b) Operations displacement table
; ---------------------------------------------------
MB_DBLDSP:
    DW    MB_DADD
    DW    MB_DSUB
    DW    MB_DMULT
    DW    MB_DDIV
    DW    MB_DEXP
    DW    MB_DCOMP

; ---------------------------------------------------
; Single real (4b) Operations displacement table
; ---------------------------------------------------
MB_SNGDSP:
    DW    MB_FADD
    DW    MB_FSUB
    DW    MB_FMUL
    DW    MB_FDIV
    DW    MB_FEXP
    DW    MB_FCOMP

; ---------------------------------------------------
; INTEGER Operations displacement table
; ---------------------------------------------------
MB_INTDSP:
    DW    MB_IADD
    DW    MB_ISUB
    DW    MB_IMUL
    DW    MB_INTDIV
    DW    MB_IEXP
    DW    MB_ICOMP
