; ===================================================
; Microsoft Basic-80 v5.26, for CP/M OS, for Triumph
; Adler Alphatronic PC.
;
; Main source file
; Disassembled by Romychs @ 2026-09-16
; ===================================================

    DEVICE NOSLOT64K
    SLDOPT COMMENT WPMEM,  ASSERTION,  LOGPOINT

    INCLUDE "equates.inc"
    INCLUDE "ioports.inc"
    INCLUDE "monitor.inc"

    ;LABELSLIST "mb.label"
    OUTPUT  basic.bin

    ORG     0x0000

; ---------------------------------------------------
; Computer startup entry and common BASIC routines
; ---------------------------------------------------

    INCLUDE "startup.inc"

; ---------------------------------------------------
; Operator, statement, function dispatcher tables
; ---------------------------------------------------

    INCLUDE "disptab.inc"

; ---------------------------------------------------
; Errors
; ---------------------------------------------------

    INCLUDE "errors.inc"

; ---------------------------------------------------
; Initialized variables
; ---------------------------------------------------

    INCLUDE "variables.inc"

; ---------------------------------------------------
; Main code implimentation
; ---------------------------------------------------

    INCLUDE "main1.inc"

; ---------------------------------------------------
; String subroutines
; ---------------------------------------------------

    INCLUDE "string.inc"

; ---------------------------------------------------
; Math subroutines
; ---------------------------------------------------

    INCLUDE "math.inc"

; ---------------------------------------------------
; Main code implimentation, continue
; ---------------------------------------------------

    INCLUDE "main2.inc"

; ---------------------------------------------------
; Stack
; ---------------------------------------------------

MB_TSTACK   EQU $ + 70

; ===================================================
