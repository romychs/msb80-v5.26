; ==================================================
; Microsoft Basic-80 v5.26, for CP/M OS, for Triumph
; Adler Alphatronic PC.
;
; IO Ports
; Disassembled by Romychs @ 2026-09-28
; ==================================================


AL_PORT_00      EQU 0x00
; --------------------------------------------------
; In:
;  bit 0 - FDC ready
;  bit 1 - Videocard accessible
;  bit 2..4 - codepage (000 -International)
;  bit 5 - LPT ON
;  bit 6 - is NTSC
;  bit 7 - is VSYNC
;
; Out:
;  bit 0 - 0 40x24, 1 - 80x24 vmode
;  bit 1 - screeen off/on
;  bit 2 - 0 - centronics, 1 - v.24 interface
;  bit 3 - cassete motor off/on
;  bit 4 - beeper off/on
;  bit 5 - always 0
;  bit 6 - ROM-pack off/on
;  bit 7 - 0 - ROM, 1 - RAM 64K
; --------------------------------------------------
MON_SPORT:      EQU 0x10

; Keyboard key status:
; Inp: bit 0 - Key "0" .. bit 7 Key "7"
; out: Monitor ROM, CRT controller and printer
MON_SPORT2      EQU 0x20
MON_KBPORT      EQU MON_SPORT2

; Keyboard key status:
; bit 1 - 8
; bit 2 - 9
; bit 3 - .
; bit 4 - -
; bit 5 - =
; bit 6 - +
AL_KBD_X1:      EQU 0x21
AL_KBD_X2:      EQU 0x22
AL_KBD_X3:      EQU 0x23
AL_KBD_X4:      EQU 0x24
AL_KBD_X5:      EQU 0x25
AL_KBD_X6:      EQU 0x26
AL_KBS_X7:      EQU 0x27
AL_KBD_X8:      EQU 0x28
AL_KBD_X9:      EQU 0x29

; Keyboard key status:
; bit 0
; bit 1 - INS/DEL
; bit 2 - Shift
; bit 3 - Tab
; bit 4 - Enter
; bit 5 - Graph
; bit 6 - Clear home
; bit 7 - ESC
AL_KBD_XA       EQU 0x2A
AL_KBD_XB       EQU 0x2B

; Centronix data output
AL_CENT_DO      EQU 0x30

AL_PORT_33      EQU 0x33

; Usart DI/DO port for Cassete
AL_CASS_ADDR   EQU 0x40

; Usart DI/DO port for Cassete control
AL_CASS_DATA   EQU 0x41

; CRTC - HD46505 / HD6845SP Address
AL_CRTC_ADDR    EQU 0x50
CRTPT0          EQU AL_CRTC_ADDR

; CRTC - HD46505 / HD6845SP Data
AL_CRTC_DATA    EQU 0x51
; 8257 DMAC
AL_DMAC_64      EQU 0x64

; 8257 DMAC
AL_DMAC_65      EQU 0x65
; 8257 DMAC
AL_DMAC_68      EQU 0x68

; Programable interrupt controller port 0
PIC70           EQU 0x70
; Programable interrupt controller port 1
PIC71           EQU 0x71

; Floppy fisk controller ports
AL_FDD_F0       EQU 0xF0
FDCDT           EQU 0xF8
FDCST           EQU 0xF9
