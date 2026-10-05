; ===================================================
; Microsoft Basic-80 v5.26, for CP/M OS, for Triumph
; Adler Alphatronic PC.
;
; Initialized Variables
; Disassembled by Romychs @ 2026-09-28
; ===================================================

; ---------------------------------------------------
; User function table
; ---------------------------------------------------
MB_USRTAB:
    DW    MB_FCERR
    DW    MB_FCERR
    DW    MB_FCERR
    DW    MB_FCERR
    DW    MB_FCERR
    DW    MB_FCERR
    DW    MB_FCERR
    DW    MB_FCERR
    DW    MB_FCERR
    DW    MB_FCERR

; ---------------------------------------------------
MB_NULCNT:
    DB    0
MB_ERRFLG:
    DB    0
    DB    0
MB_LPTPOS:
    DB    0
MB_PRTFLG:
    DB    0
MB_NLPPOS:
    DB    238
MB_LPTSIZ:
    DB    255
MB_LINLEN:
    DB    LINLN
MB_CLMLST:
    DB   0xE
    DB   0x00

; ---------------------------------------------------
; Stack pointer
; ---------------------------------------------------
MB_TOPMEM:
    DW    MB_TSTACK + 100
MB_CURLIN:
    DW   0xFFFE

; ---------------------------------------------------
; Start of BASIC program text
; ---------------------------------------------------
MB_TXTTAB:
    DW    0x733F

; Temp pointer to error message string (OVERR/DBZ handlers)
MB_ERRMSG_PTR:
    DW    0x488

AL_VAR1:
    DB    0

; Saved color value, restored on CONSOLE exit
MB_COLOR_SAVED:
    DW    0x7

AL_SCRN_ROWS:
    DB    24

; Current input cursor position (col,row), used as word
MB_CURPOS:
    DB    1

MB_COUT_FL:
    DB    1

MB_ENDPRG:
    DB    ':'
    DB    0
    DB    0
    DB    0
    DB    0

MB_NAMCNT:
    DB    0

; Name buffer
MB_NAMBUF:
    DB    0
    DB    0
    DB    0
    DB    0
    DB    0
    DB    0
    DB    0
    DB    0
    DB    0
    DB    0
    DB    0
    DB    0
    DB    0
    DB    0
    DB    0
    DB    0
    DB    0
    DB    0
    DB    0
    DB    0
    DB    0
    DB    0
    DB    0
    DB    0
    DB    0
    DB    0
    DB    0
    DB    0
    DB    0
    DB    0
    DB    0
    DB    0
    DB    0
    DB    0
    DB    0
    DB    0
    DB    0

MB_NAMBUF_END:
    DB    0

MB_NAMTMP:
    DW    0

MB_RST_INP:
    DB    ':'

; Buffer to store user input from keyboard
MB_KBUF:
    .318 DB    0

; Length of buffer above
KBFLEN  EQU $ - MB_KBUF


MB_BUFMIN:
    DB   0x2C
MB_BUF:
    .258 DB    0

MB_ENDBUF:
    DB    0
MB_DIMFLG:
    DB    0
MB_VALTYP:
    DB    0
MB_OPRTYP:
    DB    0
MB_DORES:
    DB    0
MB_CONTXT:
    DW    0
MB_CONSAV:
    DB    0
MB_CONTYP:
    DB    0
MB_CONLO:
    .4 DW 0

; ---------------------------------------------------
; Last address available for BASIC program
; ---------------------------------------------------
MB_MEMSIZ:
    DW    0

MB_TEMPPT:
    DW    0

MB_TEMPST:
    .15 DB    0

MB_TEMPST_1:
    .15 DB    0

MB_DSCTMP:
    .3  DB    0

MB_FRETOP:
    DW    0
MB_TEMP3:
    DW    0
MB_CPMFIL:
    DW    0
MB_ENDFOR:
    DW    0
MB_DATLIN:
    DW    0
MB_SUBFLG:
    DB    0
MB_USFLG:
    DB    0
MB_TEMP:
    DW    0
MB_PTRFLG:
    DB    0
MB_AUTFLG:
    DB    0
MB_AUTLIN:
    DW    0
MB_AUTINC:
    DW    0
MB_SAVTXT:
    DW    0
MB_SAVSTK:
    DW    0
MB_ERRLIN:
    DW    0
MB_DOT:
    DW    0
MB_ERRTXT:
    DW    0
MB_ONELIN:
    DW    0
MB_ONEFLG:
    DB    0
MB_TEMP2:
    DW    0
MB_OLDLIN:
    DW    0
MB_OLDTXT:
    DW    0

; Points to end of program text
MB_VARTAB:
    DW    0

; Start of variables
MB_ARYTAB:
    DW    0

; End of variables
MB_STREND:
    DW    0

MB_DATPTR:
    DW    0

MB_DEFTBL:
   .26  DB 0

MB_PRMSTK:
    DW    0
MB_PRMLEN:
    DW    0
MB_PARM1:
    .100 DB 0
MB_PRMPRV:
    DW    0
MB_PRMLN2:
    DW    0

MB_PARM2:
   .100 DB 0

MB_PRMFLG:
    DB    0
MB_ARYTA2:
    DW    0
MB_NOFUNS:
    DB    0
MB_TEMP9:
    DW    0
MB_FUNACT:
    DW    0
MB_INPPAS:
    DB    0
MB_NXTTXT:
    DW    0
MB_NXTFLG:
    DB    0
MB_FVALSV:
    .4 DW 0
MB_NXTLIN:
    DW    0
MB_OPTVAL:
    DB    0
MB_OPTFLAG_1:
    DB    0
MB_OPTVAL_2:
    DB   0
MB_OPTFLAG_2:
    DB   0
MB_PATCH:
   .30 DB 0
MB_TEMPA:
    DW   0
MB_SAVFRE:
    DW   0
MB_MRGFLG:
    DB   0
MB_MDLFLG:
    DB   0
MB_CMEPTR:
    DW   0
MB_CMSPTR:
    DW   0
MB_CHNFLG:
    DB   0
MB_CHNLIN:
    DW   0
MB_SWPTMP:
    .4 DW 0

MB_TRCFLG:
    DB   0

; --------------------------------------------------
; RAM temporary area for Math package routines
; --------------------------------------------------
MB_FLGOVC:
    DB    0
MB_OVCSTR:
    DB    0
MB_OVCSTR_1:
    DB    0, 0
MB_FBUFFR:
    .43 DB 0

MB_DECTMP:
    DW   0
MB_DECTM2:
    DW   0
MB_DECCNT:
    DW   0
MB_DACC:
    .16 DB 0
MB_HOLD8:
    .65 DB 0

MB_ARG:
    .16 DB 0

MB_RNDX:
    .8  DB 0

MB_INTXT:
    DB    " in ", 0
MB_OKTXT:
    DB    "Ok\r\n", 0
MB_BRKTXT:
    DB    "Break", 0
