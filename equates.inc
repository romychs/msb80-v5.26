; ===================================================
; Microsoft Basic-80 v5.26, for CP/M OS, for Triumph
; Adler Alphatronic PC.
;
; Constants
; Disassembled by Romychs @ 2026-09-16
; ===================================================

; --------------------------------------------------
; ASCII codes
; --------------------------------------------------
ASCII_NUL	EQU 0x00                                ; 0  - NUL (null)
ASCII_EOT	EQU 0x04                                ; 4  - EOT (End of transmission)
ASCII_BELL	EQU 0x07                                ; 7  - Beep sound
ASCII_BS	EQU 0x08                                ; 8  - Back Space
ASCII_TAB	EQU 0x09                                ; 9  - HT - Horizontal tab
ASCII_LF	EQU 0x0A                                ; 10 - LF (Line feed)
ASCII_VTAB	EQU 0x0B                                ; 11 - Vertical TAB
ASCII_FF    EQU 0x0C                                ; 12 - Form Feed
ASCII_CR	EQU 0x0D                                ; 13 - CR (Carriage return)
ASCII_ESC	EQU 0x1B                                ; 27 - ESC (Escape)
ASCII_SP    EQU 0x20                                ; 32 - Space
ASCII_CUR   EQU 0x24                                ; $
ASCII_SQ    EQU 0x27                                ; '
ASCII_BKSL  EQU 0x5C                                ; \
ASCII_DEL	EQU 0x7F                                ; 127 - DEL (Delete)

CTRL_B      EQU 0x02                                ; Move the cursor back one word
CTRL_C      EQU 0x03                                ; Break
CTRL_E      EQU 0x05                                ; Delete to end of string
CTRL_F      EQU 0x06                                ; Go to the beginning of the next word
CTRL_N      EQU 0x0E                                ; Go to to end of line
CTRL_O	    EQU 0x0F                                ;
CTRL_L	    EQU 0x0C                                ; Clear screen
CTRL_U      EQU 0x15                                ; Delete line

KEY_HOME    EQU 0x0B
KEY_INS     EQU 0x12

KEY_RT      EQU 0x1C                                ; Move cursor Right
KEY_LT      EQU 0x1D                                ; Move cursor Left
KEY_UP      EQU 0x1E                                ; Move cursor Up
KEY_DN      EQU 0x1F                                ; Move cursor Down

KEY_F1      EQU 0x81                                ; Functional key 1
KEY_F2      EQU 0x82                                ; Functional key 2
KEY_F3      EQU 0x83                                ; Functional key 3
KEY_F4      EQU 0x84                                ; Functional key 4
KEY_F5      EQU 0x85                                ; Functional key 5
KEY_F6      EQU 0x86                                ; Functional key 6
KEY_F7      EQU 0x87                                ; Functional key 7
KEY_F8      EQU 0x88                                ; Functional key 8
KEY_F9      EQU 0x89                                ; Functional key 9
KEY_F10     EQU 0x8A                                ; Functional key 10
KEY_F11     EQU 0x8B                                ; Functional key 11
KEY_F12     EQU 0x87                                ; Functional key 12




_SPACE_	    EQU 0x18                                ; 24
;_CDBL	    EQU 0x1e                                ; 30


; --------------------------------------------------
; Base tokens
; --------------------------------------------------
OCTCON	    EQU 0x0B		                        ; 11 embeded Octal constant
HEXCON	    EQU 0x0C			                    ; 12 embeded Hex constant
PTRCON	    EQU 0x0D  	                            ; 13 line pointer constant
LINCON	    EQU 0x0E		                        ; 14 line number


IN2CON	    EQU 0x0F		                        ; 15 Single byte integer (two byte with token) 11..255
CONCN2      EQU 0x10                                ; 16 Internal token, returned second type constant is scaned

ONECON	    EQU 0x11		                        ; 17 First of 10 special tokens for Int  (0x11 - 1, 0x12 - 2 ... 0x1B - 10)
INTCON	    EQU 0x1C 		                        ; 28 2-byte Int

; ToDo: check usage
SNGCON	    EQU 0x1D		                        ; 29 4-byte single precision real EEDDDDDD
CONCON	    EQU 0x1E		                        ; 30 Internal token, returned by CHARGET after constant scanned

; ToDo: check usage
DBLCON	    EQU 0x1F		                        ; 31 8-byte double precision real EEDDDDDDDDDDDDDD


; --------------------------------------------------
; Statement tokens
; --------------------------------------------------
_END	    EQU 0x81                                ; 129
_FOR	    EQU 0x82                                ; 130
_NEXT	    EQU 0x83                                ; 131
_DATA	    EQU 0x84                                ; 132
_INPUT	    EQU 0x85                                ; 133
_DIM	    EQU 0x86                                ; 134
_READ	    EQU 0x87                                ; 135
_LET	    EQU 0x88                                ; 136
_GOTO	    EQU 0x89                                ; 137
_RUN	    EQU 0x8a                                ; 138
_IF	        EQU 0x8b                                ; 139
_RESTORE	EQU 0x8c                                ; 140
_GOSUB	    EQU 0x8d                                ; 141
_RETURN	    EQU 0x8e                                ; 142
_REM	    EQU 0x8f                                ; 143
; --------------------------------------------------
_STOP	    EQU 0x90                                ; 144
_PRINT	    EQU 0x91                                ; 145
_CLEAR	    EQU 0x92                                ; 146
_LIST	    EQU 0x93                                ; 147
_NEW        EQU 0x94                                ; 148
_ON	        EQU 0x95                                ; 149
_WAIT	    EQU 0x96                                ; 150
_DEF	    EQU 0x97                                ; 151
_POKE	    EQU 0x98                                ; 152
_CONT	    EQU 0x99                                ; 153
_CSAVE	    EQU 0x9a                                ; 154
_CLOAD	    EQU 0x9b                                ; 155
_OUT	    EQU 0x9c                                ; 156
_LPRINT	    EQU 0x9d                                ; 157
_LLIST	    EQU 0x9e                                ; 158
_KEY	    EQU 0x9f                                ; 159
; --------------------------------------------------
_WIDTH	    EQU 0xa0                                ; 160
_ELSE	    EQU 0xa1                                ; 161
_TRON	    EQU 0xa2                                ; 162
_TROFF	    EQU 0xa3                                ; 163
_SWAP	    EQU 0xa4                                ; 164
_ERASE	    EQU 0xa5                                ; 165
_EDIT	    EQU 0xa6                                ; 166
_ERROR	    EQU 0xa7                                ; 167
_RESUME	    EQU 0xa8                                ; 168
_DELETE	    EQU 0xa9                                ; 169
_AUTO	    EQU 0xaa                                ; 170
_RENUM	    EQU 0xab                                ; 171
_DEFSTR	    EQU 0xac                                ; 172
_DEFINT	    EQU 0xad                                ; 173
_DEFSNG	    EQU 0xae                                ; 174
_DEFDBL	    EQU 0xaf                                ; 175
; --------------------------------------------------
_LINE	    EQU 0xb0                                ; 176
_WHILE	    EQU 0xb1                                ; 177
_WEND	    EQU 0xb2                                ; 178
_CALL	    EQU 0xb3                                ; 179
_BEEP	    EQU 0xb4                                ; 180
_MON	    EQU 0xb5                                ; 181
; ? b6
_WRITE	    EQU 0xb7                                ; 183
_COMMON	    EQU 0xb8                                ; 184
_CHAIN	    EQU 0xb9                                ; 185
_OPTION	    EQU 0xba                                ; 186
_RANDOMIZE	EQU 0xbb                                ; 187
_DSKO_	    EQU 0xbc                                ; 188
_OPEN	    EQU 0xbd                                ; 189
_FIELD	    EQU 0xbe                                ; 190
_GET	    EQU 0xbf                                ; 191
; --------------------------------------------------
_PUT	    EQU 0xc0                                ; 192
_SET	    EQU 0xc1                                ; 193
_CLOSE	    EQU 0xc2                                ; 194
_LOAD	    EQU 0xc3                                ; 195
_MERGE	    EQU 0xc4                                ; 196
_FILES	    EQU 0xc5                                ; 197
_NAME	    EQU 0xc6                                ; 198
_KILL	    EQU 0xc7                                ; 199
_LSET	    EQU 0xc8                                ; 200
_RSET	    EQU 0xc9                                ; 201
_SAVE	    EQU 0xca                                ; 202
_LFILES	    EQU 0xcb                                ; 203
_PSET	    EQU 0xcc                                ; 204
_PRESET	    EQU 0xcd                                ; 205
_COLOR	    EQU 0xce                                ; 206
_LOCATE	    EQU 0xcf                                ; 207
; 0xd0..0xd1 ---------------------------------------
_CLS	    EQU 0xd0                                ; 208
_CONSOLE	EQU 0xd1                                ; 209

NUMCMD      EQU _CONSOLE - _END + 1

; ---------------------------------------------------
; Function tokens
; ---------------------------------------------------
;d2?
_TO	        EQU 0xd3                                 ; 211
_THEN	    EQU 0xd4                                 ; 212
_TAB	    EQU 0xd5                                 ; 213
_STEP	    EQU 0xd6                                 ; 214
_USR	    EQU 0xd7                                 ; 215
_FN	        EQU 0xd8                                 ; 216
_SPC	    EQU 0xd9                                 ; 217
_NOT        EQU 0xda                                 ; 218
_ERL	    EQU 0xdb                                 ; 219
_ERR	    EQU 0xdc                                 ; 220
_STRING_	EQU 0xdd                                 ; 221
_USING	    EQU 0xde                                 ; 222
_INSTR	    EQU 0xdf                                 ; 223
; --------------------------------------------------
; ? E0
_VARPTR	    EQU 0xe1                                 ; 225
_CSRLIN	    EQU 0xe2                                 ; 226
_ATTR_	    EQU 0xe3                                 ; 227
_DSKI_	    EQU 0xe4                                 ; 228
_POINT	    EQU 0xe5                                 ; 229
_INKEY	    EQU 0xe6                                 ; 230

LESSTK	    EQU 0xf0
PLUSTK	    EQU 0xf1
MINUTK	    EQU 0xf2
MULTK	    EQU 0xf3
DIVTK	    EQU 0xf4
EXPTK	    EQU 0xf5

_AND	    EQU 0xf6                                 ; 246
_OR	        EQU 0xf7                                 ; 247
_XOR	    EQU 0xf8                                 ; 248
_EQV	    EQU 0xf9                                 ; 249
_IMP	    EQU 0xfa                                 ; 250
_MOD	    EQU 0xfb                                 ; 251
IDIVTK	    EQU 0xfc

LSTOPK	    EQU IDIVTK + 1 - PLUSTK                 ; 0x0C 12

; ---------------------------------------------------
; Extended function tokens
; ---------------------------------------------------
ONEFUN      EQU 0x81
_LEFT_	    EQU 0x81                                 ; 129
_RIGHT_	    EQU 0x82                                 ; 130
_MID_	    EQU 0x83                                 ; 131
_SGN	    EQU 0x84                                 ; 132
_INT	    EQU 0x85                                 ; 133
_ABS	    EQU 0x86                                 ; 134
_SQR	    EQU 0x87                                 ; 135
_RND	    EQU 0x88                                 ; 136

_SIN	    EQU 0x89                                 ; 137
_LOG	    EQU 0x8a                                 ; 138
_EXP	    EQU 0x8b                                 ; 139
_COS	    EQU 0x8c                                 ; 140
_TAN	    EQU 0x8d                                 ; 141
_ATN	    EQU 0x8e                                 ; 142
_FRE	    EQU 0x8f                                 ; 143

NUMGFN      EQU 2 * _MID_ - 2 * ONEFUN + 1           ;
BOTCON      EQU (_SQR - ONEFUN) * 2


; ---------------------------------------------------
_INP	    EQU 0x90                                 ; 144
_POS	    EQU 0x91                                 ; 145
_LEN	    EQU 0x92                                 ; 146
_STR_	    EQU 0x93                                 ; 147
_VAL	    EQU 0x94                                 ; 148
_ASC	    EQU 0x95                                 ; 149
_CHR_	    EQU 0x96                                 ; 150
_PEEK	    EQU 0x97                                 ; 151
; ?? 0x98 - 0x80 = 0x18
_OCT_	    EQU 0x99                                 ; 153
_HEX_	    EQU 0x9a                                 ; 154
_LPOS	    EQU 0x9b                                 ; 155
_CINT	    EQU 0x9c                                 ; 156
_CSNG	    EQU 0x9d                                 ; 157
_CDBL       EQU 0x9e
_FIX	    EQU 0x9f                                 ; 159

; ---------------------------------------------------
; Conversion functions
; ---------------------------------------------------
_CVI     	EQU 0xA0                                 ; 160
_CVS     	EQU 0xA1                                 ; 161
_CVD     	EQU 0xA2                                 ; 162

; ---------------------------------------------------
; Disk functions
; ---------------------------------------------------

_DSKF     	EQU 0xA3                                 ; 163
_EOF     	EQU 0xA4                                 ; 164
_LOC     	EQU 0xA5                                 ; 165
_LOF     	EQU 0xA6                                 ; 166
_FPOS     	EQU 0xA7                                 ; 167


; ---------------------------------------------------
; Conversion functions
; ---------------------------------------------------
_MKI_     	EQU 0xA8                                 ; 168
_MKS_     	EQU 0xA9                                 ; 169
_MKD_     	EQU 0xAA                                 ; 170


; --------------------------------------------------
; Operation tokens
; --------------------------------------------------
TABTK	    EQU 0xdb

SNGQTK	    EQU 0xe0
USINTK	    EQU 0xe4
GREATK	    EQU 0xee
EQULTK	    EQU 0xef


; --------------------------------------------------
; Types
; --------------------------------------------------
TINT	    EQU 0x02                                ; 2  2-byte Integer
TSTR	    EQU 0x03                                ; 3  <B_len>+<W-prt> String
TSGN	    EQU 0x04                                ; 4  4-byte single precision real
TDBL	    EQU 0x08                                ; 8  8-byte single precision real

; --------------------------------------------------
; File access mode
; --------------------------------------------------
MD.SQI	    EQU 1                                   ; seq inp
MD.SQO	    EQU 2                                   ; seq out
MD.RND	    EQU 3                                   ; random

; --------------------------------------------------
; Constants to skip next Ops
; --------------------------------------------------
LD_A_n	    EQU 0x3E                                ; skip next 1-byte op
LD_B_n      EQU 0x06                                ; skip next 1-byte op
LD_C_n	    EQU 0x0E                                ; skip next 1-byte op
LD_D_n      EQU 0x16                                ; skip next 1-byte op
LD_E_n      EQU 0x1E
LD_BC_nn	EQU 0x01                                ; skip next 2-byte op
LD_DE_nn	EQU 0x11                                ; skip next 2-byte op
LD_HL_nn    EQU 0x21
JP_NZ_nn    EQU 0xC2                                ;
JP_Z_nn     EQU 0xCA                                ; skip next 2-byte op
JP_C_nn     EQU 0xDA
OR_n        EQU 0xF6                                ; skip next byte op, set flags
CP_n        EQU 0xFE                                ; skip next byte op, set flags
CALL_nn     EQU 0xCD

; --------------------------------------------------
; Other constants
; --------------------------------------------------
CONS1       EQU 0x88                                ; 136
CON1	    EQU 0x2c20
NMLOFS	    EQU 6
NMREL	    EQU 3
MAXPRM	    EQU 32                                  ; 0x20
READYR_ADDR EQU 0x0002

WHILSIZ	    EQU 0x1A                                ; Memory size to store "WHILE" loop data
FORSIZ	    EQU 0x18                                ; Memory size to store "FOR" loop data

DATA_FLG	EQU 0x02
REM_FLG	    EQU 0x04
NMLO.C	    EQU 8
CLMWID	    EQU 14                                  ; Comma columns
LINLN	    EQU 40                                  ; Terminal line length
PRMSIZ      EQU 100                                 ; Definition block size
NUMLEV      EQU 60                                  ; Number of stack levels reserved

VRAMAD      EQU 0xF000                              ; Alphatronics VRAM address
