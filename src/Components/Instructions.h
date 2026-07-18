#pragma once

#include <stdint.h>

#include "2A03.h"

/*
===================== Instruction engine ======================
Addressing modes resolve where an instruction's operand lives
(cpu->addr_abs for memory, or the accumulator for IMP) and may
request one extra cycle (e.g. a page boundary was crossed).
Operations act on that operand and may also request one extra
cycle; the two are combined with & (an extra cycle is only spent
when both the addressing mode and the operation agree it matters,
e.g. a page-crossing penalty only applies to instructions that
actually read the crossed-into byte). 
*/

typedef struct Instruction {
    const char* name;
    uint8_t (*operate)(_2A03CPU*);
    uint8_t (*addrmode)(_2A03CPU*);
    uint8_t cycles;
} Instruction;

extern Instruction opcode_table[256];

uint8_t ADC(_2A03CPU* cpu);
uint8_t SBC(_2A03CPU* cpu);
uint8_t AND(_2A03CPU* cpu);
uint8_t ORA(_2A03CPU* cpu);
uint8_t EOR(_2A03CPU* cpu);
uint8_t BIT(_2A03CPU* cpu);
uint8_t CMP(_2A03CPU* cpu);
uint8_t CPX(_2A03CPU* cpu);
uint8_t CPY(_2A03CPU* cpu);
uint8_t ASL(_2A03CPU* cpu);
uint8_t LSR(_2A03CPU* cpu);
uint8_t ROL(_2A03CPU* cpu);
uint8_t ROR(_2A03CPU* cpu);
uint8_t INC(_2A03CPU* cpu);
uint8_t DEC(_2A03CPU* cpu);
uint8_t INX(_2A03CPU* cpu);
uint8_t INY(_2A03CPU* cpu);
uint8_t DEX(_2A03CPU* cpu);
uint8_t DEY(_2A03CPU* cpu);

uint8_t LDA(_2A03CPU* cpu);
uint8_t LDX(_2A03CPU* cpu);
uint8_t LDY(_2A03CPU* cpu);

uint8_t STA(_2A03CPU* cpu);
uint8_t STX(_2A03CPU* cpu);
uint8_t STY(_2A03CPU* cpu);

uint8_t TAX(_2A03CPU* cpu);
uint8_t TAY(_2A03CPU* cpu);
uint8_t TXA(_2A03CPU* cpu);
uint8_t TYA(_2A03CPU* cpu);
uint8_t TSX(_2A03CPU* cpu);
uint8_t TXS(_2A03CPU* cpu);

uint8_t PHA(_2A03CPU* cpu);
uint8_t PLA(_2A03CPU* cpu);
uint8_t PHP(_2A03CPU* cpu);
uint8_t PLP(_2A03CPU* cpu);

uint8_t branch(_2A03CPU* cpu);
uint8_t BCC(_2A03CPU* cpu);
uint8_t BCS(_2A03CPU* cpu);
uint8_t BEQ(_2A03CPU* cpu);
uint8_t BNE(_2A03CPU* cpu);
uint8_t BMI(_2A03CPU* cpu);
uint8_t BPL(_2A03CPU* cpu);
uint8_t BVC(_2A03CPU* cpu);
uint8_t BVS(_2A03CPU* cpu);

uint8_t JMP(_2A03CPU* cpu);
uint8_t JSR(_2A03CPU* cpu);
uint8_t RTS(_2A03CPU* cpu);
uint8_t BRK(_2A03CPU* cpu);
uint8_t RTI(_2A03CPU* cpu);

uint8_t CLC(_2A03CPU* cpu);
uint8_t CLD(_2A03CPU* cpu);
uint8_t CLI(_2A03CPU* cpu);
uint8_t CLV(_2A03CPU* cpu);
uint8_t SEC(_2A03CPU* cpu);
uint8_t SED(_2A03CPU* cpu);
uint8_t SEI(_2A03CPU* cpu);

uint8_t NOP(_2A03CPU* cpu);
/* Illegal/undocumented opcode. The 2A03 has real (if quirky) behavior
   for these that some games rely on; until that's implemented, treat
   them as a NOP so execution doesn't stall on an unmapped opcode. */
uint8_t XXX(_2A03CPU* cpu);

