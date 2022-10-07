# GNU8080
Intel 8080 Disassembler

HEXDUMP
hexdump -v invaders.h

COMPILE
gcc disassembler.c -o disassemble

RUN
./disassemble invaders.h

SAVE OUTPUT TO FILE
./disassemble invaders.h > dis.txt
