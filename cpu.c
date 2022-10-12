#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "disassembler.c"
#include "8080emu.h"

static void unimplementedInstruction(State8080* state) {
    // pc will have advanced one, so undo that
    printf ("Error: Unimplemented instruction\n");
    exit(1);
}

static int parity(int x, int size) {
    int i;
    int p = 0;
    x = (x & ((1<<size)-1));
    for (i=0; i<size; i++)
    {
        if (x & 0x1) p++;
        x = x >> 1;
    }
    return (0 == (p & 0x1));
}

static uint8_t machineIN(State8080* state, uint8_t port) {
    uint8_t a;
    switch(port)
    {
        case 1:
            a = state->port.read1;
            break;
        case 2:
            a = state->port.read2;
            break;
        case 3:
        {
            uint16_t v = (state->port.shift1<<8) | state->port.shift0;
            a = ((v >> (8 - state->port.write2)) & 0xff);
            break;
        }
        default:
            unimplementedInstruction(state);
            break;
    }
    return a;
}

static void machineOUT(State8080* state, uint8_t port) {
    switch(port)
    {
        case 2:
            state->port.write2 = state->a & 0x7;
            break;
        case 4:
            state->port.shift0 = state->port.shift1;
            state->port.shift1 = state->a;
            break;
    }
}


int emulate8080(State8080* state) {
    unsigned char *opcode = &state->memory[state->pc];

    Disassemble8080Op(opcode, state->pc);

    state->pc += 1;

    switch(*opcode) {
        case 0x00: // NOP
            break;
        case 0x80:      //ADD B    
        {    
            // do the math with higher precision so we can capture the    
            // carry out    
            uint16_t answer = (uint16_t) state->a + (uint16_t) state->b;    

            // Zero flag: if the result is zero,    
            // set the flag to zero    
            // else clear the flag    
            if ((answer & 0xff) == 0)    
                state->cc.z = 1;    
            else    
                state->cc.z = 0;    

            // Sign flag: if bit 7 is set,    
            // set the sign flag    
            // else clear the sign flag    
            if (answer & 0x80)    
                state->cc.s = 1;    
            else    
                state->cc.s = 0;    

            // Carry flag    
            if (answer > 0xff)    
                state->cc.cy = 1;    
            else    
                state->cc.cy = 0;    

            // Parity is handled by a subroutine    
            state->cc.p = parity( answer & 0xff, 8);    

            state->a = answer & 0xff;    
        }    

    //The code for ADD can be condensed like this
      case 0x81:      //ADD C
      {
            uint16_t answer = (uint16_t) state->a + (uint16_t) state->c;
            state->cc.z = ((answer & 0xff) == 0);
            state->cc.s = ((answer & 0x80) != 0);
            state->cc.cy = (answer > 0xff);
            state->cc.p = parity(answer&0xff, 8);
            state->a = answer & 0xff;
        }
      default:
          printf("%x\n", *opcode);
          unimplementedInstruction(state);
          break;
    }
    return 1;
}

//RUN CPU
static void push(State8080 *state, uint8_t msbReg, uint8_t lsbReg) {
	state->memory[state->sp - 1] = msbReg;
	state->memory[state->sp - 2] = lsbReg;

	state->sp -= 2;
}

void generateInterrupt(State8080 *state, int interrupt_num) {
	//Push PC
	push(state, ((state->pc & 0xff00) >> 8), (state->pc & 0xff));

	//Set PC to low memory vector
	//RST interrupt_num
	state->pc = 8 * interrupt_num;
	state->int_enable = 0;
}
