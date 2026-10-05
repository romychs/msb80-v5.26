; ==================================================
; Microsoft Basic-80 v5.26, for CP/M OS, for Triumph
; Adler Alphatronic PC.
;
; BASIC error messages and their codes
; Disassembled by Romychs @ 2026-09-16
; ==================================================

; --------------------------------------------------
MB_ERRTAB:
    db    0
MB_ERROR_MSGS:
    DB    "NEXT without FOR", 0                     ; 1
    DB    "Syntax error", 0                         ; 2
    DB    "RETURN without GOSUB", 0                 ; 3
    DB    "Out of DATA", 0                          ; 4
    DB    "Illegal function call", 0                ; 5
MSG_OVR_ERR:
    DB    "Overflow", 0                             ; 6
    DB    "Out of memory", 0                        ; 7
    DB    "Undefined line number", 0                ; 8
    DB    "Subscript out of range", 0               ; 9
    DB    "Duplicate Definition", 0                 ; 10
MSG_DBZ_ERR:
    DB    "Division by zero", 0                     ; 11
    DB    "Illegal direct",0                        ; 12
MSG_TYPE_MM:
    DB    "Type mismatch", 0                        ; 13
    DB    "Out of string space", 0                  ; 14
    DB    "String too long", 0                      ; 15
    DB    "String formula too complex", 0           ; 16

    DB    "Can't continue", 0                       ; 17
    DB    "Undefined user function", 0              ; 18
    DB    "No RESUME", 0                            ; 19
    DB    "RESUME without error", 0                 ; 20
    DB    "Unprintable error", 0                    ; 21
    DB    "Missing operand", 0                      ; 22
    DB    "Line buffer overflow", 0                 ; 23
    DB    "Device I/O error", 0                     ; 24
    db    '?', 0                                    ; 25
    db    '?', 0                                    ; 26
    DB    "FOR Without NEXT", 0                     ; 27 - 0x1B
    db    '?', 0                                    ; 28
    db    '?', 0                                    ; 29
    DB    "WHILE without WEND", 0                   ; 30
    DB    "WEND without WHILE", 0                   ; 31

MSG_FOW_ERR:
    DB    "FIELD overflow", 0                       ; 50
    DB    "Internal error", 0                       ; 51
    DB    "Bad file number", 0                      ; 52
    DB    "File not found", 0                       ; 53
    DB    "File already open", 0                    ; 54
    DB    "Disk not mounted", 0                     ; 55
    DB    "Disk I/O error", 0                       ; 56
    DB    "File already exists", 0                  ; 57
    db    '?', 0                                    ; 58
    DB    "Disk already mounted", 0                 ; 59
    DB    "Disk full",0                             ; 60
    DB    "Input past end", 0                       ; 61
    DB    "Bad file name", 0                        ; 62
    DB    "Direct statement in file", 0             ; 63
    DB    "Bad allocation table", 0                 ; 64
    DB    "Bad drive number", 0                     ; 65
    DB    "Bad track/sector", 0                     ; 66
    DB    "Deleted record", 0                       ; 67
MSG_RAD_ERR:
    DB    "Rename across disks", 0                  ; 68
    DB    "Sequential after PUT", 0                 ; 69
    DB    "Sequential I/O only", 0                  ; 70
    DB    "File not OPEN", 0                        ; 71
    DB    "File write protected", 0                 ; 72
    DB    "Disk offline", 0                         ; 73

; --------------------------------------------------
; Error codes
; --------------------------------------------------
ERRNF       EQU 0x01
ERRSN	    EQU 0x02                                ; 2
ERRRG	    EQU 0x03                                ; 3
ERROD	    EQU 0x04                                ; 4
ERRFC	    EQU 0x05                                ; 5
ERROV	    EQU 0x06                                ; 6
ERROM       EQU 0x07
ERRULN 	    EQU 0x08                                ; 8
ERRBS	    EQU 0x09                                ; 9
ERRDD	    EQU 0x0a                                ; 10
ERRDV0	    EQU 0x0b                                ; 11
ERRID	    EQU 0x0c                                ; 12
ERRTM	    EQU 0x0d                                ; 13
ERRSO       EQU 0x0e                                ; Out of space
ERRLS	    EQU 0x0f                                ; 15
ERRST       EQU 0x10                                ; String formula too complex
ERRCN	    EQU 0x11                                ; 17
ERRUF       EQU 0x12                                ; Undefined user function
ERRNR	    EQU 0x13                                ; 19
ERRRE	    EQU 0x14                                ; 20
ERRUE       EQU 0x15                                ; Unprintable error
ERRMO       EQU 0x16                                ; Missing operand
ERRLBO	    EQU 0x17                                ; 23
ERRDIO      EQU 0x18                                ; Device I/O error
; skipped and 19, 1a
ERRFN	    EQU 0x1b                               ; 2
; skipped 1c and 1d
ERRWH	    EQU 0x1e                                ; 29
ERRWE	    EQU 0x1f                                ; 30

NONDSK	    EQU 0x1f                                ; Last non disk error

; Disk errors starts from here
DSKERR	    EQU 0x31                                ; 49
;
ERRFOV	    EQU 0x32                                ; 50
ERRIER	    EQU 0x33                                ; 51
ERRBFN	    EQU 0x34                                ; 52
ERRFNF	    EQU 0x35                                ; 53
ERRFAO	    EQU 0x36                                ; 54
ERRDNM	    EQU 0x37                                ; 55
ERRIOE	    EQU 0x38                                ; 56
ERRFAE	    EQU 0x39                                ; 57
; 0x3a - 58
ERRDAM      EQU 0x3b                                ; 59 Disk already mounted
ERRDFL	    EQU 0x3c                                ; 60 Disk full
ERRRPE	    EQU 0x3d                                ; 61 Inp past end
ERRBFL      EQU 0x3e                                ; 62 Bad file name
ERRDSF      EQU 0x3f                                ; 63 Direct statement in file
ERRBAT      EQU 0x40                                ; 64 Bad alloc table
ERRBDN      EQU 0x41                                ; 65
ERRBTS	    EQU 0x42                                ; 66 Bad trk/sec
ERRDLR      EQU 0x43                                ; 67 Deleted record

ERRRAD      EQU 0x44                                ; 68 ren across disks
ERRSAP      EQU 0x45                                ; 69 Seq aft PUT
ERRSIO      EQU 0x46                                ; 70 Seq I/O only
ERRFNO      EQU 0x47                                ; 71 File not open
ERRDOF      EQU 0x48                                ; 73 Disk offline
ERRFWP      EQU 0x49                                ; 72 File protected

; --------------------------------------------------
LSTERR	    EQU ERRFWP + 1