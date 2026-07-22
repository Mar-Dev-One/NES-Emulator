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
    /* Only the pushed byte gets B and U forced to 1 -- the live P
       register is never touched by PHP on real hardware. */
    stack_push(cpu, cpu->P | FLAG_B | FLAG_U);
    return 0;
}

uint8_t PLP(_2A03CPU* cpu) {
    cpu->P = stack_pop(cpu);
    set_flag(cpu, FLAG_B, false);
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

/* NOP doesn't touch any registers, but several of its illegal-opcode
   forms (e.g. $1C absolute,X) still perform the memory read their
   addressing mode implies, and that read still costs an extra cycle
   if it crosses a page boundary -- same as LDA/ORA/etc. Returning 1
   here (instead of 0) lets that page-cross bit from the addressing
   mode survive the `extra_from_addr & extra_from_op` combine in
   cpu_clock(). It's a no-op for addressing modes that never report a
   page-cross (ZP0/ZPX/ABS/IMP/IMM), since 0 & 1 is still 0 there. */
uint8_t NOP(_2A03CPU* cpu) { (void)cpu; return 1; }
uint8_t XXX(_2A03CPU* cpu) { (void)cpu; return 0; }

/*
===================== Illegal / undocumented opcodes ====================== */

uint8_t SLO(_2A03CPU* cpu) {
    const Instruction* instr = &opcode_table[cpu->opcode];
    fetch(cpu, instr);
    uint16_t temp = (uint16_t)cpu->fetched << 1;
    set_flag(cpu, FLAG_C, temp & 0xFF00);
    write_back(cpu, instr, temp & 0x00FF);
    cpu->Accumulator |= (temp & 0x00FF);
    set_flag(cpu, FLAG_Z, cpu->Accumulator == 0);
    set_flag(cpu, FLAG_N, cpu->Accumulator & 0x80);
    return 0;
}

uint8_t RLA(_2A03CPU* cpu) {
    const Instruction* instr = &opcode_table[cpu->opcode];
    fetch(cpu, instr);
    uint16_t temp = ((uint16_t)cpu->fetched << 1) | get_flag(cpu, FLAG_C);
    set_flag(cpu, FLAG_C, temp & 0xFF00);
    write_back(cpu, instr, temp & 0x00FF);
    cpu->Accumulator &= (temp & 0x00FF);
    set_flag(cpu, FLAG_Z, cpu->Accumulator == 0);
    set_flag(cpu, FLAG_N, cpu->Accumulator & 0x80);
    return 0;
}

uint8_t SRE(_2A03CPU* cpu) {
    const Instruction* instr = &opcode_table[cpu->opcode];
    fetch(cpu, instr);
    set_flag(cpu, FLAG_C, cpu->fetched & 0x01);
    uint8_t temp = cpu->fetched >> 1;
    write_back(cpu, instr, temp);
    cpu->Accumulator ^= temp;
    set_flag(cpu, FLAG_Z, cpu->Accumulator == 0);
    set_flag(cpu, FLAG_N, cpu->Accumulator & 0x80);
    return 0;
}

uint8_t RRA(_2A03CPU* cpu) {
    const Instruction* instr = &opcode_table[cpu->opcode];
    fetch(cpu, instr);
    bool new_carry = cpu->fetched & 0x01;
    uint8_t temp = ((uint8_t)get_flag(cpu, FLAG_C) << 7) | (cpu->fetched >> 1);
    write_back(cpu, instr, temp);

    uint16_t sum = (uint16_t)cpu->Accumulator + (uint16_t)temp + (uint16_t)new_carry;
    set_flag(cpu, FLAG_C, sum > 0x00FF);
    set_flag(cpu, FLAG_Z, (sum & 0x00FF) == 0);
    set_flag(cpu, FLAG_N, sum & 0x0080);
    set_flag(cpu, FLAG_V, (~((uint16_t)cpu->Accumulator ^ (uint16_t)temp) &
                            ((uint16_t)cpu->Accumulator ^ sum)) & 0x0080);
    cpu->Accumulator = sum & 0x00FF;
    return 0;
}

uint8_t DCP(_2A03CPU* cpu) {
    const Instruction* instr = &opcode_table[cpu->opcode];
    fetch(cpu, instr);
    uint8_t temp = cpu->fetched - 1;
    write_back(cpu, instr, temp);

    set_flag(cpu, FLAG_C, cpu->Accumulator >= temp);
    set_flag(cpu, FLAG_Z, cpu->Accumulator == temp);
    set_flag(cpu, FLAG_N, (cpu->Accumulator - temp) & 0x80);
    return 0;
}

uint8_t ISC(_2A03CPU* cpu) {
    const Instruction* instr = &opcode_table[cpu->opcode];
    fetch(cpu, instr);
    uint8_t temp = cpu->fetched + 1;
    write_back(cpu, instr, temp);

    uint16_t value = ((uint16_t)temp) ^ 0x00FF;
    uint16_t sum = (uint16_t)cpu->Accumulator + value + (uint16_t)get_flag(cpu, FLAG_C);
    set_flag(cpu, FLAG_C, sum & 0xFF00);
    set_flag(cpu, FLAG_Z, (sum & 0x00FF) == 0);
    set_flag(cpu, FLAG_N, sum & 0x0080);
    set_flag(cpu, FLAG_V, (sum ^ (uint16_t)cpu->Accumulator) & (sum ^ value) & 0x0080);
    cpu->Accumulator = sum & 0x00FF;
    return 0;
}

uint8_t LAX(_2A03CPU* cpu) {
    fetch(cpu, &opcode_table[cpu->opcode]);
    cpu->Accumulator = cpu->fetched;
    cpu->X = cpu->fetched;
    set_flag(cpu, FLAG_Z, cpu->Accumulator == 0);
    set_flag(cpu, FLAG_N, cpu->Accumulator & 0x80);
    return 1;   /* honors the page-cross penalty on its indexed forms */
}

uint8_t SAX(_2A03CPU* cpu) {
    cpu_write(cpu, cpu->addr_abs, cpu->Accumulator & cpu->X);
    return 0;
}

uint8_t ANC(_2A03CPU* cpu) {
    fetch(cpu, &opcode_table[cpu->opcode]);
    cpu->Accumulator &= cpu->fetched;
    set_flag(cpu, FLAG_Z, cpu->Accumulator == 0);
    set_flag(cpu, FLAG_N, cpu->Accumulator & 0x80);
    set_flag(cpu, FLAG_C, cpu->Accumulator & 0x80);   /* N copied into C */
    return 0;
}

uint8_t ALR(_2A03CPU* cpu) {
    fetch(cpu, &opcode_table[cpu->opcode]);
    cpu->Accumulator &= cpu->fetched;
    set_flag(cpu, FLAG_C, cpu->Accumulator & 0x01);
    cpu->Accumulator >>= 1;
    set_flag(cpu, FLAG_Z, cpu->Accumulator == 0);
    set_flag(cpu, FLAG_N, cpu->Accumulator & 0x80);   /* always clear post-shift, kept for symmetry */
    return 0;
}

uint8_t ARR(_2A03CPU* cpu) {
    fetch(cpu, &opcode_table[cpu->opcode]);
    cpu->Accumulator &= cpu->fetched;
    cpu->Accumulator = ((uint8_t)get_flag(cpu, FLAG_C) << 7) | (cpu->Accumulator >> 1);
    set_flag(cpu, FLAG_Z, cpu->Accumulator == 0);
    set_flag(cpu, FLAG_N, cpu->Accumulator & 0x80);
    set_flag(cpu, FLAG_C, cpu->Accumulator & 0x40);
    set_flag(cpu, FLAG_V, ((cpu->Accumulator >> 6) ^ (cpu->Accumulator >> 5)) & 0x01);
    return 0;
}

uint8_t AXS(_2A03CPU* cpu) {
    fetch(cpu, &opcode_table[cpu->opcode]);
    uint8_t and_result = cpu->Accumulator & cpu->X;
    uint16_t temp = (uint16_t)and_result - (uint16_t)cpu->fetched;
    set_flag(cpu, FLAG_C, and_result >= cpu->fetched);
    cpu->X = temp & 0x00FF;
    set_flag(cpu, FLAG_Z, cpu->X == 0);
    set_flag(cpu, FLAG_N, cpu->X & 0x80);
    return 0;
}

uint8_t JAM(_2A03CPU* cpu) {
    cpu->jammed = true;
    return 0;
}

/* --- Unstable opcodes: documented common-case approximation only --- */

uint8_t ANE(_2A03CPU* cpu) {
    fetch(cpu, &opcode_table[cpu->opcode]);
    /* Real behavior depends on analog bus capacitance decay and varies
       by chip; 0xEE is the constant most commonly cited as matching
       typical NES 2A03 units. */
    cpu->Accumulator = (cpu->Accumulator | 0xEE) & cpu->X & cpu->fetched;
    set_flag(cpu, FLAG_Z, cpu->Accumulator == 0);
    set_flag(cpu, FLAG_N, cpu->Accumulator & 0x80);
    return 0;
}

uint8_t LAX_IMM(_2A03CPU* cpu) {
    fetch(cpu, &opcode_table[cpu->opcode]);
    cpu->Accumulator = (cpu->Accumulator | 0xEE) & cpu->fetched;
    cpu->X = cpu->Accumulator;
    set_flag(cpu, FLAG_Z, cpu->Accumulator == 0);
    set_flag(cpu, FLAG_N, cpu->Accumulator & 0x80);
    return 0;
}

uint8_t SHA(_2A03CPU* cpu) {
    uint8_t hi = (uint8_t)(cpu->addr_abs >> 8) + 1;
    cpu_write(cpu, cpu->addr_abs, cpu->Accumulator & cpu->X & hi);
    return 0;
}

uint8_t SHX(_2A03CPU* cpu) {
    uint8_t hi = (uint8_t)(cpu->addr_abs >> 8) + 1;
    cpu_write(cpu, cpu->addr_abs, cpu->X & hi);
    return 0;
}

uint8_t SHY(_2A03CPU* cpu) {
    uint8_t hi = (uint8_t)(cpu->addr_abs >> 8) + 1;
    cpu_write(cpu, cpu->addr_abs, cpu->Y & hi);
    return 0;
}

uint8_t TAS(_2A03CPU* cpu) {
    cpu->SP = cpu->Accumulator & cpu->X;
    uint8_t hi = (uint8_t)(cpu->addr_abs >> 8) + 1;
    cpu_write(cpu, cpu->addr_abs, cpu->SP & hi);
    return 0;
}

uint8_t LAS(_2A03CPU* cpu) {
    fetch(cpu, &opcode_table[cpu->opcode]);
    cpu->SP &= cpu->fetched;
    cpu->Accumulator = cpu->SP;
    cpu->X = cpu->SP;
    set_flag(cpu, FLAG_Z, cpu->Accumulator == 0);
    set_flag(cpu, FLAG_N, cpu->Accumulator & 0x80);
    return 1;
}


Instruction opcode_table[256] = {
    /*0x00*/ {"BRK",BRK,IMP,7},{"ORA",ORA,IZX,6},{"JAM",JAM,IMP,2},{"SLO",SLO,IZX,8},
    /*0x04*/ {"???",NOP,ZP0,3},{"ORA",ORA,ZP0,3},{"ASL",ASL,ZP0,5},{"SLO",SLO,ZP0,5},
    /*0x08*/ {"PHP",PHP,IMP,3},{"ORA",ORA,IMM,2},{"ASL",ASL,IMP,2},{"ANC",ANC,IMM,2},
    /*0x0C*/ {"???",NOP,ABS,4},{"ORA",ORA,ABS,4},{"ASL",ASL,ABS,6},{"SLO",SLO,ABS,6},
    /*0x10*/ {"BPL",BPL,REL,2},{"ORA",ORA,IZY,5},{"JAM",JAM,IMP,2},{"SLO",SLO,IZY,8},
    /*0x14*/ {"???",NOP,ZPX,4},{"ORA",ORA,ZPX,4},{"ASL",ASL,ZPX,6},{"SLO",SLO,ZPX,6},
    /*0x18*/ {"CLC",CLC,IMP,2},{"ORA",ORA,ABY,4},{"???",NOP,IMP,2},{"SLO",SLO,ABY,7},
    /*0x1C*/ {"???",NOP,ABX,4},{"ORA",ORA,ABX,4},{"ASL",ASL,ABX,7},{"SLO",SLO,ABX,7},
    /*0x20*/ {"JSR",JSR,ABS,6},{"AND",AND,IZX,6},{"JAM",JAM,IMP,2},{"RLA",RLA,IZX,8},
    /*0x24*/ {"BIT",BIT,ZP0,3},{"AND",AND,ZP0,3},{"ROL",ROL,ZP0,5},{"RLA",RLA,ZP0,5},
    /*0x28*/ {"PLP",PLP,IMP,4},{"AND",AND,IMM,2},{"ROL",ROL,IMP,2},{"ANC",ANC,IMM,2},
    /*0x2C*/ {"BIT",BIT,ABS,4},{"AND",AND,ABS,4},{"ROL",ROL,ABS,6},{"RLA",RLA,ABS,6},
    /*0x30*/ {"BMI",BMI,REL,2},{"AND",AND,IZY,5},{"JAM",JAM,IMP,2},{"RLA",RLA,IZY,8},
    /*0x34*/ {"???",NOP,ZPX,4},{"AND",AND,ZPX,4},{"ROL",ROL,ZPX,6},{"RLA",RLA,ZPX,6},
    /*0x38*/ {"SEC",SEC,IMP,2},{"AND",AND,ABY,4},{"???",NOP,IMP,2},{"RLA",RLA,ABY,7},
    /*0x3C*/ {"???",NOP,ABX,4},{"AND",AND,ABX,4},{"ROL",ROL,ABX,7},{"RLA",RLA,ABX,7},
    /*0x40*/ {"RTI",RTI,IMP,6},{"EOR",EOR,IZX,6},{"JAM",JAM,IMP,2},{"SRE",SRE,IZX,8},
    /*0x44*/ {"???",NOP,ZP0,3},{"EOR",EOR,ZP0,3},{"LSR",LSR,ZP0,5},{"SRE",SRE,ZP0,5},
    /*0x48*/ {"PHA",PHA,IMP,3},{"EOR",EOR,IMM,2},{"LSR",LSR,IMP,2},{"ALR",ALR,IMM,2},
    /*0x4C*/ {"JMP",JMP,ABS,3},{"EOR",EOR,ABS,4},{"LSR",LSR,ABS,6},{"SRE",SRE,ABS,6},
    /*0x50*/ {"BVC",BVC,REL,2},{"EOR",EOR,IZY,5},{"JAM",JAM,IMP,2},{"SRE",SRE,IZY,8},
    /*0x54*/ {"???",NOP,ZPX,4},{"EOR",EOR,ZPX,4},{"LSR",LSR,ZPX,6},{"SRE",SRE,ZPX,6},
    /*0x58*/ {"CLI",CLI,IMP,2},{"EOR",EOR,ABY,4},{"???",NOP,IMP,2},{"SRE",SRE,ABY,7},
    /*0x5C*/ {"???",NOP,ABX,4},{"EOR",EOR,ABX,4},{"LSR",LSR,ABX,7},{"SRE",SRE,ABX,7},
    /*0x60*/ {"RTS",RTS,IMP,6},{"ADC",ADC,IZX,6},{"JAM",JAM,IMP,2},{"RRA",RRA,IZX,8},
    /*0x64*/ {"???",NOP,ZP0,3},{"ADC",ADC,ZP0,3},{"ROR",ROR,ZP0,5},{"RRA",RRA,ZP0,5},
    /*0x68*/ {"PLA",PLA,IMP,4},{"ADC",ADC,IMM,2},{"ROR",ROR,IMP,2},{"ARR",ARR,IMM,2},
    /*0x6C*/ {"JMP",JMP,IND,5},{"ADC",ADC,ABS,4},{"ROR",ROR,ABS,6},{"RRA",RRA,ABS,6},
    /*0x70*/ {"BVS",BVS,REL,2},{"ADC",ADC,IZY,5},{"JAM",JAM,IMP,2},{"RRA",RRA,IZY,8},
    /*0x74*/ {"???",NOP,ZPX,4},{"ADC",ADC,ZPX,4},{"ROR",ROR,ZPX,6},{"RRA",RRA,ZPX,6},
    /*0x78*/ {"SEI",SEI,IMP,2},{"ADC",ADC,ABY,4},{"???",NOP,IMP,2},{"RRA",RRA,ABY,7},
    /*0x7C*/ {"???",NOP,ABX,4},{"ADC",ADC,ABX,4},{"ROR",ROR,ABX,7},{"RRA",RRA,ABX,7},
    /*0x80*/ {"???",NOP,IMM,2},{"STA",STA,IZX,6},{"???",NOP,IMM,2},{"SAX",SAX,IZX,6},
    /*0x84*/ {"STY",STY,ZP0,3},{"STA",STA,ZP0,3},{"STX",STX,ZP0,3},{"SAX",SAX,ZP0,3},
    /*0x88*/ {"DEY",DEY,IMP,2},{"???",NOP,IMM,2},{"TXA",TXA,IMP,2},{"ANE",ANE,IMM,2},
    /*0x8C*/ {"STY",STY,ABS,4},{"STA",STA,ABS,4},{"STX",STX,ABS,4},{"SAX",SAX,ABS,4},
    /*0x90*/ {"BCC",BCC,REL,2},{"STA",STA,IZY,6},{"JAM",JAM,IMP,2},{"SHA",SHA,IZY,6},
    /*0x94*/ {"STY",STY,ZPX,4},{"STA",STA,ZPX,4},{"STX",STX,ZPY,4},{"SAX",SAX,ZPY,4},
    /*0x98*/ {"TYA",TYA,IMP,2},{"STA",STA,ABY,5},{"TXS",TXS,IMP,2},{"TAS",TAS,ABY,5},
    /*0x9C*/ {"SHY",SHY,ABX,5},{"STA",STA,ABX,5},{"SHX",SHX,ABY,5},{"SHA",SHA,ABY,5},
    /*0xA0*/ {"LDY",LDY,IMM,2},{"LDA",LDA,IZX,6},{"LDX",LDX,IMM,2},{"LAX",LAX,IZX,6},
    /*0xA4*/ {"LDY",LDY,ZP0,3},{"LDA",LDA,ZP0,3},{"LDX",LDX,ZP0,3},{"LAX",LAX,ZP0,3},
    /*0xA8*/ {"TAY",TAY,IMP,2},{"LDA",LDA,IMM,2},{"TAX",TAX,IMP,2},{"LAX",LAX_IMM,IMM,2},
    /*0xAC*/ {"LDY",LDY,ABS,4},{"LDA",LDA,ABS,4},{"LDX",LDX,ABS,4},{"LAX",LAX,ABS,4},
    /*0xB0*/ {"BCS",BCS,REL,2},{"LDA",LDA,IZY,5},{"JAM",JAM,IMP,2},{"LAX",LAX,IZY,5},
    /*0xB4*/ {"LDY",LDY,ZPX,4},{"LDA",LDA,ZPX,4},{"LDX",LDX,ZPY,4},{"LAX",LAX,ZPY,4},
    /*0xB8*/ {"CLV",CLV,IMP,2},{"LDA",LDA,ABY,4},{"TSX",TSX,IMP,2},{"LAS",LAS,ABY,4},
    /*0xBC*/ {"LDY",LDY,ABX,4},{"LDA",LDA,ABX,4},{"LDX",LDX,ABY,4},{"LAX",LAX,ABY,4},
    /*0xC0*/ {"CPY",CPY,IMM,2},{"CMP",CMP,IZX,6},{"???",NOP,IMM,2},{"DCP",DCP,IZX,8},
    /*0xC4*/ {"CPY",CPY,ZP0,3},{"CMP",CMP,ZP0,3},{"DEC",DEC,ZP0,5},{"DCP",DCP,ZP0,5},
    /*0xC8*/ {"INY",INY,IMP,2},{"CMP",CMP,IMM,2},{"DEX",DEX,IMP,2},{"AXS",AXS,IMM,2},
    /*0xCC*/ {"CPY",CPY,ABS,4},{"CMP",CMP,ABS,4},{"DEC",DEC,ABS,6},{"DCP",DCP,ABS,6},
    /*0xD0*/ {"BNE",BNE,REL,2},{"CMP",CMP,IZY,5},{"JAM",JAM,IMP,2},{"DCP",DCP,IZY,8},
    /*0xD4*/ {"???",NOP,ZPX,4},{"CMP",CMP,ZPX,4},{"DEC",DEC,ZPX,6},{"DCP",DCP,ZPX,6},
    /*0xD8*/ {"CLD",CLD,IMP,2},{"CMP",CMP,ABY,4},{"???",NOP,IMP,2},{"DCP",DCP,ABY,7},
    /*0xDC*/ {"???",NOP,ABX,4},{"CMP",CMP,ABX,4},{"DEC",DEC,ABX,7},{"DCP",DCP,ABX,7},
    /*0xE0*/ {"CPX",CPX,IMM,2},{"SBC",SBC,IZX,6},{"???",NOP,IMM,2},{"ISC",ISC,IZX,8},
    /*0xE4*/ {"CPX",CPX,ZP0,3},{"SBC",SBC,ZP0,3},{"INC",INC,ZP0,5},{"ISC",ISC,ZP0,5},
    /*0xE8*/ {"INX",INX,IMP,2},{"SBC",SBC,IMM,2},{"NOP",NOP,IMP,2},{"SBC",SBC,IMM,2},
    /*0xEC*/ {"CPX",CPX,ABS,4},{"SBC",SBC,ABS,4},{"INC",INC,ABS,6},{"ISC",ISC,ABS,6},
    /*0xF0*/ {"BEQ",BEQ,REL,2},{"SBC",SBC,IZY,5},{"JAM",JAM,IMP,2},{"ISC",ISC,IZY,8},
    /*0xF4*/ {"???",NOP,ZPX,4},{"SBC",SBC,ZPX,4},{"INC",INC,ZPX,6},{"ISC",ISC,ZPX,6},
    /*0xF8*/ {"SED",SED,IMP,2},{"SBC",SBC,ABY,4},{"???",NOP,IMP,2},{"ISC",ISC,ABY,7},
    /*0xFC*/ {"???",NOP,ABX,4},{"SBC",SBC,ABX,4},{"INC",INC,ABX,7},{"ISC",ISC,ABX,7},
};