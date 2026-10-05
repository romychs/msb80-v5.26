# Microsoft BASIC-80 v5.26 for Alphatronic computer

Source codes of Microsoft Basic-80 v5.26, for Triumph Adler Alphatronic PC (Z80 CPU, 64k RAM).
This is the latest version of MS BASIC-80 I could find on the Internet. All the other, newer 
ones turned out to be fakes and differed little—or not at all, aside from the version number—from 
version 5.21.

On the Internet, this version is often referred to as being intended for CP/M; however, it uses only the built-in MONITOR program.
I think the application was loading from the tape (or diskette) at address 0x0000.

I tried to preserve the original label names exactly as they appear in the source code published by Microsoft.


## Compile

To compile sources, use [sjasmplus Z80 assembler](https://github.com/z00m128/sjasmplus).
