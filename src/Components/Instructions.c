#include "Instructions.h"


uint8_t ADC(_2A03CPU* cpu) {
    fetch(cpu, &opcode_table[cpu->opcode]);
    uint16_t sum = (uint16_t)cpu->Accumulator + (uint16_t)cpu->fetched + (uint16_t)get_flag(cpu, FLAG_C);
    set_flag(cpu, FLAG_C, sum > 0x00FF);
    set_flag(cpu, FLAG_Z, (sum & 0x00FF) == 0);
    set_flag(cpu, FLAG_N, sum & 0x0080);
    set_flag(cpu, FLAG_V, (~((uint16_t)cpu->Accumulator ^ (uint16_t)cpu->fetched) &
                            ((uint16_t)cpu->Accumulator ^ sum)) & 0x0080);
    cpu->Accumulator = sum & 0x00FF;
    return 1;
}

uint8_t SBC(_2A03CPU* cpu) {
    fetch(cpu, &opcode_table[cpu->opcode]);
    uint16_t value = ((uint16_t)cpu->fetched) ^ 0x00FF;   /* invert for subtraction-as-addition */
    uint16_t sum = (uint16_t)cpu->Accumulator + value + (uint16_t)get_flag(cpu, FLAG_C);
    set_flag(cpu, FLAG_C, sum & 0xFF00);
    set_flag(cpu, FLAG_Z, (sum & 0x00FF) == 0);
    set_flag(cpu, FLAG_N, sum & 0x0080);
    set_flag(cpu, FLAG_V, (sum ^ (uint16_t)cpu->Accumulator) & (sum ^ value) & 0x0080);
    cpu->Accumulator = sum & 0x00FF;
    return 1;
}

uint8_t AND(_2A03CPU* cpu) {
    fetch(cpu, &opcode_table[cpu->opcode]);
    cpu->Accumulator &= cpu->fetched;
    set_flag(cpu, FLAG_Z, cpu->Accumulator == 0);
    set_flag(cpu, FLAG_N, cpu->Accumulator & 0x80);
    return 1;
}

uint8_t ORA(_2A03CPU* cpu) {
    fetch(cpu, &opcode_table[cpu->opcode]);
    cpu->Accumulator |= cpu->fetched;
    set_flag(cpu, FLAG_Z, cpu->Accumulator == 0);
    set_flag(cpu, FLAG_N, cpu->Accumulator & 0x80);
    return 1;
}

uint8_t EOR(_2A03CPU* cpu) {
    fetch(cpu, &opcode_table[cpu->opcode]);
    cpu->Accumulator ^= cpu->fetched;
    set_flag(cpu, FLAG_Z, cpu->Accumulator == 0);
    set_flag(cpu, FLAG_N, cpu->Accumulator & 0x80);
    return 1;
}

uint8_t BIT(_2A03CPU* cpu) {
    fetch(cpu, &opcode_table[cpu->opcode]);
    uint8_t temp = cpu->Accumulator & cpu->fetched;
    set_flag(cpu, FLAG_Z, temp == 0);
    set_flag(cpu, FLAG_N, cpu->fetched & (1 << 7));
    set_flag(cpu, FLAG_V, cpu->fetched & (1 << 6));
    return 0;
}

uint8_t CMP(_2A03CPU* cpu) {
    fetch(cpu, &opcode_table[cpu->opcode]);
    uint16_t temp = (uint16_t)cpu->Accumulator - (uint16_t)cpu->fetched;
    set_flag(cpu, FLAG_C, cpu->Accumulator >= cpu->fetched);
    set_flag(cpu, FLAG_Z, (temp & 0x00FF) == 0);
    set_flag(cpu, FLAG_N, temp & 0x0080);
    return 1;
}

uint8_t CPX(_2A03CPU* cpu) {
    fetch(cpu, &opcode_table[cpu->opcode]);
    uint16_t temp = (uint16_t)cpu->X - (uint16_t)cpu->fetched;
    set_flag(cpu, FLAG_C, cpu->X >= cpu->fetched);
    set_flag(cpu, FLAG_Z, (temp & 0x00FF) == 0);
    set_flag(cpu, FLAG_N, temp & 0x0080);
    return 0;
}

uint8_t CPY(_2A03CPU* cpu) {
    fetch(cpu, &opcode_table[cpu->opcode]);
    uint16_t temp = (uint16_t)cpu->Y - (uint16_t)cpu->fetched;
    set_flag(cpu, FLAG_C, cpu->Y >= cpu->fetched);
    set_flag(cpu, FLAG_Z, (temp & 0x00FF) == 0);
    set_flag(cpu, FLAG_N, temp & 0x0080);
    return 0;
}

uint8_t ASL(_2A03CPU* cpu) {
    const Instruction* instr = &opcode_table[cpu->opcode];
    fetch(cpu, instr);
    uint16_t temp = (uint16_t)cpu->fetched << 1;
    set_flag(cpu, FLAG_C, temp & 0xFF00);
    set_flag(cpu, FLAG_Z, (temp & 0x00FF) == 0);
    set_flag(cpu, FLAG_N, temp & 0x0080);
    write_back(cpu, instr, temp & 0x00FF);
    return 0;
}

uint8_t LSR(_2A03CPU* cpu) {
    const Instruction* instr = &opcode_table[cpu->opcode];
    fetch(cpu, instr);
    set_flag(cpu, FLAG_C, cpu->fetched & 0x0001);
    uint8_t temp = cpu->fetched >> 1;
    set_flag(cpu, FLAG_Z, temp == 0);
    set_flag(cpu, FLAG_N, temp & 0x80);
    write_back(cpu, instr, temp);
    return 0;
}

uint8_t ROL(_2A03CPU* cpu) {
    const Instruction* instr = &opcode_table[cpu->opcode];
    fetch(cpu, instr);
    uint16_t temp = ((uint16_t)cpu->fetched << 1) | get_flag(cpu, FLAG_C);
    set_flag(cpu, FLAG_C, temp & 0xFF00);
    set_flag(cpu, FLAG_Z, (temp & 0x00FF) == 0);
    set_flag(cpu, FLAG_N, temp & 0x0080);
    write_back(cpu, instr, temp & 0x00FF);
    return 0;
}

uint8_t ROR(_2A03CPU* cpu) {
    const Instruction* instr = &opcode_table[cpu->opcode];
    fetch(cpu, instr);
    uint16_t temp = ((uint16_t)get_flag(cpu, FLAG_C) << 7) | (cpu->fetched >> 1);
    set_flag(cpu, FLAG_C, cpu->fetched & 0x01);
    set_flag(cpu, FLAG_Z, (temp & 0x00FF) == 0);
    set_flag(cpu, FLAG_N, temp & 0x0080);
    write_back(cpu, instr, temp & 0x00FF);
    return 0;
}

uint8_t INC(_2A03CPU* cpu) {
    const Instruction* instr = &opcode_table[cpu->opcode];
    fetch(cpu, instr);
    uint8_t temp = cpu->fetched + 1;
    set_flag(cpu, FLAG_Z, temp == 0);
    set_flag(cpu, FLAG_N, temp & 0x80);
    write_back(cpu, instr, temp);
    return 0;
}

uint8_t DEC(_2A03CPU* cpu) {
    const Instruction* instr = &opcode_table[cpu->opcode];
    fetch(cpu, instr);
    uint8_t temp = cpu->fetched - 1;
    set_flag(cpu, FLAG_Z, temp == 0);
    set_flag(cpu, FLAG_N, temp & 0x80);
    write_back(cpu, instr, temp);
    return 0;
}

uint8_t INX(_2A03CPU* cpu) { cpu->X++; set_flag(cpu, FLAG_Z, cpu->X == 0); set_flag(cpu, FLAG_N, cpu->X & 0x80); return 0; }
uint8_t INY(_2A03CPU* cpu) { cpu->Y++; set_flag(cpu, FLAG_Z, cpu->Y == 0); set_flag(cpu, FLAG_N, cpu->Y & 0x80); return 0; }
uint8_t DEX(_2A03CPU* cpu) { cpu->X--; set_flag(cpu, FLAG_Z, cpu->X == 0); set_flag(cpu, FLAG_N, cpu->X & 0x80); return 0; }
uint8_t DEY(_2A03CPU* cpu) { cpu->Y--; set_flag(cpu, FLAG_Z, cpu->Y == 0); set_flag(cpu, FLAG_N, cpu->Y & 0x80); return 0; }

uint8_t LDA(_2A03CPU* cpu) {
    fetch(cpu, &opcode_table[cpu->opcode]);
    cpu->Accumulator = cpu->fetched;
    set_flag(cpu, FLAG_Z, cpu->Accumulator == 0);
    set_flag(cpu, FLAG_N, cpu->Accumulator & 0x80);
    return 1;
}

uint8_t LDX(_2A03CPU* cpu) {
    fetch(cpu, &opcode_table[cpu->opcode]);
    cpu->X = cpu->fetched;
    set_flag(cpu, FLAG_Z, cpu->X == 0);
    set_flag(cpu, FLAG_N, cpu->X & 0x80);
    return 1;
}

uint8_t LDY(_2A03CPU* cpu) {
    fetch(cpu, &opcode_table[cpu->opcode]);
    cpu->Y = cpu->fetched;
    set_flag(cpu, FLAG_Z, cpu->Y == 0);
    set_flag(cpu, FLAG_N, cpu->Y & 0x80);
    return 1;
}
uint8_t STA(_2A03CPU* cpu) { cpu_write(cpu, cpu->addr_abs, cpu->Accumulator); return 0; }
uint8_t STX(_2A03CPU* cpu) { cpu_write(cpu, cpu->addr_abs, cpu->X); return 0; }
uint8_t STY(_2A03CPU* cpu) { cpu_write(cpu, cpu->addr_abs, cpu->Y); return 0; }

uint8_t TAX(_2A03CPU* cpu) { cpu->X = cpu->Accumulator; set_flag(cpu, FLAG_Z, cpu->X == 0); set_flag(cpu, FLAG_N, cpu->X & 0x80); return 0; }
uint8_t TAY(_2A03CPU* cpu) { cpu->Y = cpu->Accumulator; set_flag(cpu, FLAG_Z, cpu->Y == 0); set_flag(cpu, FLAG_N, cpu->Y & 0x80); return 0; }
uint8_t TXA(_2A03CPU* cpu) { cpu->Accumulator = cpu->X; set_flag(cpu, FLAG_Z, cpu->Accumulator == 0); set_flag(cpu, FLAG_N, cpu->Accumulator & 0x80); return 0; }
uint8_t TYA(_2A03CPU* cpu) { cpu->Accumulator = cpu->Y; set_flag(cpu, FLAG_Z, cpu->Accumulator == 0); set_flag(cpu, FLAG_N, cpu->Accumulator & 0x80); return 0; }
uint8_t TSX(_2A03CPU* cpu) { cpu->X = cpu->SP; set_flag(cpu, FLAG_Z, cpu->X == 0); set_flag(cpu, FLAG_N, cpu->X & 0x80); return 0; }
uint8_t TXS(_2A03CPU* cpu) { cpu->SP = cpu->X; return 0; }

uint8_t PHA(_2A03CPU* cpu) { stack_push(cpu, cpu->Accumulator); return 0; }

uint8_t PLA(_2A03CPU* cpu) {
    cpu->Accumulator = stack_pop(cpu);
    set_flag(cpu, FLAG_Z, cpu->Accumulator == 0);
    set_flag(cpu, FLAG_N, cpu->Accumulator & 0x80);
    return 0;
}
uint8_t PHP(_2A03CPU* cpu) {
    stack_push(cpu, cpu->P | FLAG_B | FLAG_U);
    set_flag(cpu, FLAG_B, false);
    set_flag(cpu, FLAG_U, false);
    return 0;
}

uint8_t PLP(_2A03CPU* cpu) {
    cpu->P = stack_pop(cpu);
    set_flag(cpu, FLAG_U, true);
    return 0;
}

uint8_t branch(_2A03CPU* cpu) {
    cpu->cycles_remaining++;
    cpu->addr_abs = cpu->PC + cpu->addr_rel;
    if ((cpu->addr_abs & 0xFF00) != (cpu->PC & 0xFF00))
        cpu->cycles_remaining++;
    cpu->PC = cpu->addr_abs;
    return 0;
}

uint8_t BCC(_2A03CPU* cpu) { if (!get_flag(cpu, FLAG_C)) return branch(cpu); return 0; }
uint8_t BCS(_2A03CPU* cpu) { if (get_flag(cpu, FLAG_C))  return branch(cpu); return 0; }
uint8_t BEQ(_2A03CPU* cpu) { if (get_flag(cpu, FLAG_Z))  return branch(cpu); return 0; }
uint8_t BNE(_2A03CPU* cpu) { if (!get_flag(cpu, FLAG_Z)) return branch(cpu); return 0; }
uint8_t BMI(_2A03CPU* cpu) { if (get_flag(cpu, FLAG_N))  return branch(cpu); return 0; }
uint8_t BPL(_2A03CPU* cpu) { if (!get_flag(cpu, FLAG_N)) return branch(cpu); return 0; }
uint8_t BVC(_2A03CPU* cpu) { if (!get_flag(cpu, FLAG_V)) return branch(cpu); return 0; }
uint8_t BVS(_2A03CPU* cpu) { if (get_flag(cpu, FLAG_V))  return branch(cpu); return 0; }

uint8_t JMP(_2A03CPU* cpu) { cpu->PC = cpu->addr_abs; return 0; }

uint8_t JSR(_2A03CPU* cpu) {
    cpu->PC--;
    stack_push(cpu, (cpu->PC >> 8) & 0x00FF);
    stack_push(cpu, cpu->PC & 0x00FF);
    cpu->PC = cpu->addr_abs;
    return 0;
}

uint8_t RTS(_2A03CPU* cpu) {
    uint16_t lo = stack_pop(cpu);
    uint16_t hi = stack_pop(cpu);
    cpu->PC = (hi << 8) | lo;
    cpu->PC++;
    return 0;
}

uint8_t BRK(_2A03CPU* cpu) {
    cpu->PC++;
    set_flag(cpu, FLAG_I, true);
    stack_push(cpu, (cpu->PC >> 8) & 0x00FF);
    stack_push(cpu, cpu->PC & 0x00FF);
    stack_push(cpu, cpu->P | FLAG_B | FLAG_U);
    set_flag(cpu, FLAG_B, false);
    cpu->PC = (uint16_t)cpu_read(cpu, 0xFFFF) << 8 | cpu_read(cpu, 0xFFFE);
    return 0;
}

uint8_t RTI(_2A03CPU* cpu) {
    cpu->P = stack_pop(cpu);
    set_flag(cpu, FLAG_B, false);
    set_flag(cpu, FLAG_U, true);
    uint16_t lo = stack_pop(cpu);
    uint16_t hi = stack_pop(cpu);
    cpu->PC = (hi << 8) | lo;
    return 0;
}

uint8_t CLC(_2A03CPU* cpu) { set_flag(cpu, FLAG_C, false); return 0; }
uint8_t CLD(_2A03CPU* cpu) { set_flag(cpu, FLAG_D, false); return 0; }
uint8_t CLI(_2A03CPU* cpu) { set_flag(cpu, FLAG_I, false); return 0; }
uint8_t CLV(_2A03CPU* cpu) { set_flag(cpu, FLAG_V, false); return 0; }
uint8_t SEC(_2A03CPU* cpu) { set_flag(cpu, FLAG_C, true); return 0; }
uint8_t SED(_2A03CPU* cpu) { set_flag(cpu, FLAG_D, true); return 0; }
uint8_t SEI(_2A03CPU* cpu) { set_flag(cpu, FLAG_I, true); return 0; }

uint8_t NOP(_2A03CPU* cpu) { (void)cpu; return 0; }
uint8_t XXX(_2A03CPU* cpu) { (void)cpu; return 0; }


Instruction opcode_table[256] = {
    /*0x00*/ {"BRK",BRK,IMP,7},{"ORA",ORA,IZX,6},{"???",XXX,IMP,2},{"???",XXX,IMP,8},
    /*0x04*/ {"???",NOP,ZP0,3},{"ORA",ORA,ZP0,3},{"ASL",ASL,ZP0,5},{"???",XXX,IMP,5},
    /*0x08*/ {"PHP",PHP,IMP,3},{"ORA",ORA,IMM,2},{"ASL",ASL,IMP,2},{"???",XXX,IMP,2},
    /*0x0C*/ {"???",NOP,ABS,4},{"ORA",ORA,ABS,4},{"ASL",ASL,ABS,6},{"???",XXX,IMP,6},
    /*0x10*/ {"BPL",BPL,REL,2},{"ORA",ORA,IZY,5},{"???",XXX,IMP,2},{"???",XXX,IMP,8},
    /*0x14*/ {"???",NOP,ZPX,4},{"ORA",ORA,ZPX,4},{"ASL",ASL,ZPX,6},{"???",XXX,IMP,6},
    /*0x18*/ {"CLC",CLC,IMP,2},{"ORA",ORA,ABY,4},{"???",NOP,IMP,2},{"???",XXX,IMP,7},
    /*0x1C*/ {"???",NOP,ABX,4},{"ORA",ORA,ABX,4},{"ASL",ASL,ABX,7},{"???",XXX,IMP,7},
    /*0x20*/ {"JSR",JSR,ABS,6},{"AND",AND,IZX,6},{"???",XXX,IMP,2},{"???",XXX,IMP,8},
    /*0x24*/ {"BIT",BIT,ZP0,3},{"AND",AND,ZP0,3},{"ROL",ROL,ZP0,5},{"???",XXX,IMP,5},
    /*0x28*/ {"PLP",PLP,IMP,4},{"AND",AND,IMM,2},{"ROL",ROL,IMP,2},{"???",XXX,IMP,2},
    /*0x2C*/ {"BIT",BIT,ABS,4},{"AND",AND,ABS,4},{"ROL",ROL,ABS,6},{"???",XXX,IMP,6},
    /*0x30*/ {"BMI",BMI,REL,2},{"AND",AND,IZY,5},{"???",XXX,IMP,2},{"???",XXX,IMP,8},
    /*0x34*/ {"???",NOP,ZPX,4},{"AND",AND,ZPX,4},{"ROL",ROL,ZPX,6},{"???",XXX,IMP,6},
    /*0x38*/ {"SEC",SEC,IMP,2},{"AND",AND,ABY,4},{"???",NOP,IMP,2},{"???",XXX,IMP,7},
    /*0x3C*/ {"???",NOP,ABX,4},{"AND",AND,ABX,4},{"ROL",ROL,ABX,7},{"???",XXX,IMP,7},
    /*0x40*/ {"RTI",RTI,IMP,6},{"EOR",EOR,IZX,6},{"???",XXX,IMP,2},{"???",XXX,IMP,8},
    /*0x44*/ {"???",NOP,ZP0,3},{"EOR",EOR,ZP0,3},{"LSR",LSR,ZP0,5},{"???",XXX,IMP,5},
    /*0x48*/ {"PHA",PHA,IMP,3},{"EOR",EOR,IMM,2},{"LSR",LSR,IMP,2},{"???",XXX,IMP,2},
    /*0x4C*/ {"JMP",JMP,ABS,3},{"EOR",EOR,ABS,4},{"LSR",LSR,ABS,6},{"???",XXX,IMP,6},
    /*0x50*/ {"BVC",BVC,REL,2},{"EOR",EOR,IZY,5},{"???",XXX,IMP,2},{"???",XXX,IMP,8},
    /*0x54*/ {"???",NOP,ZPX,4},{"EOR",EOR,ZPX,4},{"LSR",LSR,ZPX,6},{"???",XXX,IMP,6},
    /*0x58*/ {"CLI",CLI,IMP,2},{"EOR",EOR,ABY,4},{"???",NOP,IMP,2},{"???",XXX,IMP,7},
    /*0x5C*/ {"???",NOP,ABX,4},{"EOR",EOR,ABX,4},{"LSR",LSR,ABX,7},{"???",XXX,IMP,7},
    /*0x60*/ {"RTS",RTS,IMP,6},{"ADC",ADC,IZX,6},{"???",XXX,IMP,2},{"???",XXX,IMP,8},
    /*0x64*/ {"???",NOP,ZP0,3},{"ADC",ADC,ZP0,3},{"ROR",ROR,ZP0,5},{"???",XXX,IMP,5},
    /*0x68*/ {"PLA",PLA,IMP,4},{"ADC",ADC,IMM,2},{"ROR",ROR,IMP,2},{"???",XXX,IMP,2},
    /*0x6C*/ {"JMP",JMP,IND,5},{"ADC",ADC,ABS,4},{"ROR",ROR,ABS,6},{"???",XXX,IMP,6},
    /*0x70*/ {"BVS",BVS,REL,2},{"ADC",ADC,IZY,5},{"???",XXX,IMP,2},{"???",XXX,IMP,8},
    /*0x74*/ {"???",NOP,ZPX,4},{"ADC",ADC,ZPX,4},{"ROR",ROR,ZPX,6},{"???",XXX,IMP,6},
    /*0x78*/ {"SEI",SEI,IMP,2},{"ADC",ADC,ABY,4},{"???",NOP,IMP,2},{"???",XXX,IMP,7},
    /*0x7C*/ {"???",NOP,ABX,4},{"ADC",ADC,ABX,4},{"ROR",ROR,ABX,7},{"???",XXX,IMP,7},
    /*0x80*/ {"???",NOP,IMM,2},{"STA",STA,IZX,6},{"???",NOP,IMM,2},{"???",XXX,IMP,6},
    /*0x84*/ {"STY",STY,ZP0,3},{"STA",STA,ZP0,3},{"STX",STX,ZP0,3},{"???",XXX,IMP,3},
    /*0x88*/ {"DEY",DEY,IMP,2},{"???",NOP,IMM,2},{"TXA",TXA,IMP,2},{"???",XXX,IMP,2},
    /*0x8C*/ {"STY",STY,ABS,4},{"STA",STA,ABS,4},{"STX",STX,ABS,4},{"???",XXX,IMP,4},
    /*0x90*/ {"BCC",BCC,REL,2},{"STA",STA,IZY,6},{"???",XXX,IMP,2},{"???",XXX,IMP,6},
    /*0x94*/ {"STY",STY,ZPX,4},{"STA",STA,ZPX,4},{"STX",STX,ZPY,4},{"???",XXX,IMP,4},
    /*0x98*/ {"TYA",TYA,IMP,2},{"STA",STA,ABY,5},{"TXS",TXS,IMP,2},{"???",XXX,IMP,5},
    /*0x9C*/ {"???",XXX,IMP,5},{"STA",STA,ABX,5},{"???",XXX,IMP,5},{"???",XXX,IMP,5},
    /*0xA0*/ {"LDY",LDY,IMM,2},{"LDA",LDA,IZX,6},{"LDX",LDX,IMM,2},{"???",XXX,IMP,6},
    /*0xA4*/ {"LDY",LDY,ZP0,3},{"LDA",LDA,ZP0,3},{"LDX",LDX,ZP0,3},{"???",XXX,IMP,3},
    /*0xA8*/ {"TAY",TAY,IMP,2},{"LDA",LDA,IMM,2},{"TAX",TAX,IMP,2},{"???",XXX,IMP,2},
    /*0xAC*/ {"LDY",LDY,ABS,4},{"LDA",LDA,ABS,4},{"LDX",LDX,ABS,4},{"???",XXX,IMP,4},
    /*0xB0*/ {"BCS",BCS,REL,2},{"LDA",LDA,IZY,5},{"???",XXX,IMP,2},{"???",XXX,IMP,5},
    /*0xB4*/ {"LDY",LDY,ZPX,4},{"LDA",LDA,ZPX,4},{"LDX",LDX,ZPY,4},{"???",XXX,IMP,4},
    /*0xB8*/ {"CLV",CLV,IMP,2},{"LDA",LDA,ABY,4},{"TSX",TSX,IMP,2},{"???",XXX,IMP,4},
    /*0xBC*/ {"LDY",LDY,ABX,4},{"LDA",LDA,ABX,4},{"LDX",LDX,ABY,4},{"???",XXX,IMP,4},
    /*0xC0*/ {"CPY",CPY,IMM,2},{"CMP",CMP,IZX,6},{"???",NOP,IMM,2},{"???",XXX,IMP,8},
    /*0xC4*/ {"CPY",CPY,ZP0,3},{"CMP",CMP,ZP0,3},{"DEC",DEC,ZP0,5},{"???",XXX,IMP,5},
    /*0xC8*/ {"INY",INY,IMP,2},{"CMP",CMP,IMM,2},{"DEX",DEX,IMP,2},{"???",XXX,IMP,2},
    /*0xCC*/ {"CPY",CPY,ABS,4},{"CMP",CMP,ABS,4},{"DEC",DEC,ABS,6},{"???",XXX,IMP,6},
    /*0xD0*/ {"BNE",BNE,REL,2},{"CMP",CMP,IZY,5},{"???",XXX,IMP,2},{"???",XXX,IMP,8},
    /*0xD4*/ {"???",NOP,ZPX,4},{"CMP",CMP,ZPX,4},{"DEC",DEC,ZPX,6},{"???",XXX,IMP,6},
    /*0xD8*/ {"CLD",CLD,IMP,2},{"CMP",CMP,ABY,4},{"???",NOP,IMP,2},{"???",XXX,IMP,7},
    /*0xDC*/ {"???",NOP,ABX,4},{"CMP",CMP,ABX,4},{"DEC",DEC,ABX,7},{"???",XXX,IMP,7},
    /*0xE0*/ {"CPX",CPX,IMM,2},{"SBC",SBC,IZX,6},{"???",NOP,IMM,2},{"???",XXX,IMP,8},
    /*0xE4*/ {"CPX",CPX,ZP0,3},{"SBC",SBC,ZP0,3},{"INC",INC,ZP0,5},{"???",XXX,IMP,5},
    /*0xE8*/ {"INX",INX,IMP,2},{"SBC",SBC,IMM,2},{"NOP",NOP,IMP,2},{"???",XXX,IMP,2},
    /*0xEC*/ {"CPX",CPX,ABS,4},{"SBC",SBC,ABS,4},{"INC",INC,ABS,6},{"???",XXX,IMP,6},
    /*0xF0*/ {"BEQ",BEQ,REL,2},{"SBC",SBC,IZY,5},{"???",XXX,IMP,2},{"???",XXX,IMP,8},
    /*0xF4*/ {"???",NOP,ZPX,4},{"SBC",SBC,ZPX,4},{"INC",INC,ZPX,6},{"???",XXX,IMP,6},
    /*0xF8*/ {"SED",SED,IMP,2},{"SBC",SBC,ABY,4},{"???",NOP,IMP,2},{"???",XXX,IMP,7},
    /*0xFC*/ {"???",NOP,ABX,4},{"SBC",SBC,ABX,4},{"INC",INC,ABX,7},{"???",XXX,IMP,7},
};