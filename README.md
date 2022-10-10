# GNU8080
Intel 8080 Disassembler

# HEXDUMP

hexdump -v invaders.h

# COMPILE

gcc disassembler.c -o disassemble

# RUN

./disassemble invaders.h

#SAVE OUTPUT TO FILE

./disassemble invaders.h > dis.txt

#COMBINE FOUR ROM FILES INTO ONE
cat invaders.h invaders.g invaders.f invaders.e > invaders.rom
