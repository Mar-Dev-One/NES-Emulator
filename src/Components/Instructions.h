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
/* Catch-all for any table slot that isn't wired up to something more
   specific below. Should not be reachable once opcode_table is fully
   populated -- kept as a safety net, not a real opcode's handler. */
uint8_t XXX(_2A03CPU* cpu);

/*
===================== Illegal / undocumented opcodes ======================
The 2A03 decodes all 256 possible opcode bytes; the 105 that aren't part
of the official 6502 instruction set still do *something* deterministic
(they're simple side effects of how the decode logic and ALU are wired),
and some commercial games and most test ROMs (nestest included) rely on
that behavior. The six combined read-modify-write ops (SLO/RLA/SRE/RRA/
DCP/ISC) and LAX/SAX/ANC/ALR/ARR/AXS/SBC($EB) are perfectly consistent
across real hardware. A handful of others (ANE/XAA, LAX #imm, SHA, SHX,
SHY, TAS, LAS) involve an unstable bus conflict inside the CPU whose
exact result varies between chip revisions/temperature/etc; these are
implemented using the commonly-documented approximation (see nesdev's
"CPU unofficial opcodes" page) rather than a guaranteed-accurate model,
since no fully deterministic behavior exists to model.
*/

/* SLO/RLA/SRE/RRA/DCP/ISC: RMW op combined with an ALU op, in one pass. */
uint8_t SLO(_2A03CPU* cpu);
uint8_t RLA(_2A03CPU* cpu);
uint8_t SRE(_2A03CPU* cpu);
uint8_t RRA(_2A03CPU* cpu);
uint8_t DCP(_2A03CPU* cpu);
uint8_t ISC(_2A03CPU* cpu);

uint8_t LAX(_2A03CPU* cpu);   /* LDA + LDX in one, from memory (stable). */
uint8_t SAX(_2A03CPU* cpu);   /* Store (A & X); no flags affected. */

uint8_t ANC(_2A03CPU* cpu);   /* AND #imm, then C = bit 7 of A (N copied to C). */
uint8_t ALR(_2A03CPU* cpu);   /* AND #imm, then LSR A. Aka ASR. */
uint8_t ARR(_2A03CPU* cpu);   /* AND #imm, then ROR A, with quirky C/V. */
uint8_t AXS(_2A03CPU* cpu);   /* X = (A & X) - #imm, C/N/Z set like CMP. Aka SBX. */

uint8_t JAM(_2A03CPU* cpu);   /* Locks up the CPU permanently (aka KIL/HLT). */

/* Unstable: documented common-case approximation, not guaranteed to
   match every physical chip. */
uint8_t ANE(_2A03CPU* cpu);      /* 0x8B, aka XAA. */
uint8_t LAX_IMM(_2A03CPU* cpu);  /* 0xAB, aka LXA/OAL -- LAX's immediate form. */
uint8_t SHA(_2A03CPU* cpu);      /* 0x93/0x9F, aka AHX/AXA. */
uint8_t SHX(_2A03CPU* cpu);      /* 0x9E. */
uint8_t SHY(_2A03CPU* cpu);      /* 0x9C. */
uint8_t TAS(_2A03CPU* cpu);      /* 0x9B, aka SHS/XAS. */
uint8_t LAS(_2A03CPU* cpu);      /* 0xBB, aka LAR. */