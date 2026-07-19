#include "2A03.h"
#include "Instructions.h"

#include <string.h>

bool init_cpu(_2A03CPU* cpu)
{
    cpu->Accumulator = 0;
    cpu->X = 0;
    cpu->Y = 0;
    cpu->P = 0;
    cpu->PC = 0;
    cpu->SP = 0;

    cpu->cycles_remaining = 0;
    cpu->opcode = 0;
    cpu->addr_abs = 0;
    cpu->addr_rel = 0;
    cpu->fetched = 0;

    cpu->bus = malloc(sizeof(memory_bus));
    if (!cpu->bus)
        return false;

    memset(cpu->bus->ram, 0, 0x0800);

    reset_cpu(cpu);

    return true;
}

bool reset_cpu(_2A03CPU* cpu)
{
    cpu->SP -= 3;

    set_flag(cpu, FLAG_U, true);
    set_flag(cpu, FLAG_I, true);

    uint8_t lo = cpu_read(cpu, 0xFFFC);
    uint8_t hi = cpu_read(cpu, 0xFFFD);
    cpu->PC = (uint16_t)(hi << 8) | lo;

    return true;
    
}

void set_flag(_2A03CPU* cpu, CPUFlag flag, bool value)
{
    if (value) {
        cpu->P |= flag;
    }else {
        cpu->P &= ~flag;
    }
}

bool get_flag(_2A03CPU* cpu, CPUFlag flag)
{
    return ((cpu->P & flag) != 0);
}

uint8_t cpu_read(_2A03CPU* cpu, uint16_t addr) {
    return bus_read(cpu->bus, addr);
}
void cpu_write(_2A03CPU* cpu, uint16_t addr, uint8_t value) {
    bus_write(cpu->bus, addr, value);
}

bool cpu_clock(_2A03CPU* cpu)
{
    if (cpu->cycles_remaining == 0) {
        cpu->opcode = cpu_read(cpu, cpu->PC);
        cpu->PC++;

        const Instruction* instr = &opcode_table[cpu->opcode];
        cpu->cycles_remaining = instr->cycles;

        uint8_t extra_from_addr = instr->addrmode(cpu);
        uint8_t extra_from_op = instr->operate(cpu);
        cpu->cycles_remaining += (extra_from_addr & extra_from_op);
    }

    cpu->cycles_remaining--;
    return true;
}

uint8_t fetch(_2A03CPU* cpu, const Instruction* instr) {
    if (instr->addrmode != IMP)
        cpu->fetched = cpu_read(cpu, cpu->addr_abs);
    return cpu->fetched;
}

void write_back(_2A03CPU* cpu, const Instruction* instr, uint8_t value) {
    if (instr->addrmode == IMP)
        cpu->Accumulator = value;
    else
        cpu_write(cpu, cpu->addr_abs, value);
}

void stack_push(_2A03CPU* cpu, uint8_t value) {
    cpu_write(cpu, 0x0100 + cpu->SP, value);
    cpu->SP--;
}

uint8_t stack_pop(_2A03CPU* cpu) {
    cpu->SP++;
    return cpu_read(cpu, 0x0100 + cpu->SP);
}

uint8_t IMP(_2A03CPU* cpu) { cpu->fetched = cpu->Accumulator; return 0; }
uint8_t IMM(_2A03CPU* cpu) { cpu->addr_abs = cpu->PC; cpu->PC++; return 0; }

uint8_t ZP0(_2A03CPU* cpu) {
    cpu->addr_abs = cpu_read(cpu, cpu->PC) & 0x00FF;
    cpu->PC++;
    return 0;
}

uint8_t ZPX(_2A03CPU* cpu) {
    cpu->addr_abs = (cpu_read(cpu, cpu->PC) + cpu->X) & 0x00FF;
    cpu->PC++;
    return 0;
}

uint8_t ZPY(_2A03CPU* cpu) {
    cpu->addr_abs = (cpu_read(cpu, cpu->PC) + cpu->Y) & 0x00FF;
    cpu->PC++;
    return 0;
}

uint8_t REL(_2A03CPU* cpu) {
    cpu->addr_rel = cpu_read(cpu, cpu->PC);
    cpu->PC++;
    if (cpu->addr_rel & 0x80)
        cpu->addr_rel |= 0xFF00;   /* sign-extend to 16 bits */
    return 0;
}

uint8_t ABS(_2A03CPU* cpu) {
    uint16_t lo = cpu_read(cpu, cpu->PC); cpu->PC++;
    uint16_t hi = cpu_read(cpu, cpu->PC); cpu->PC++;
    cpu->addr_abs = (hi << 8) | lo;
    return 0;
}

uint8_t ABX(_2A03CPU* cpu) {
    uint16_t lo = cpu_read(cpu, cpu->PC); cpu->PC++;
    uint16_t hi = cpu_read(cpu, cpu->PC); cpu->PC++;
    uint16_t base = (hi << 8) | lo;
    cpu->addr_abs = base + cpu->X;
    return ((cpu->addr_abs & 0xFF00) != (base & 0xFF00)) ? 1 : 0;
}

uint8_t ABY(_2A03CPU* cpu) {
    uint16_t lo = cpu_read(cpu, cpu->PC); cpu->PC++;
    uint16_t hi = cpu_read(cpu, cpu->PC); cpu->PC++;
    uint16_t base = (hi << 8) | lo;
    cpu->addr_abs = base + cpu->Y;
    return ((cpu->addr_abs & 0xFF00) != (base & 0xFF00)) ? 1 : 0;
}

uint8_t IND(_2A03CPU* cpu) {
    uint16_t ptr_lo = cpu_read(cpu, cpu->PC); cpu->PC++;
    uint16_t ptr_hi = cpu_read(cpu, cpu->PC); cpu->PC++;
    uint16_t ptr = (ptr_hi << 8) | ptr_lo;

    /* Faithfully reproduce the famous 6502 indirect-JMP page-boundary
       bug: if the pointer's low byte is 0xFF, the high byte of the
       target is read from the start of the SAME page, not the next. */
    if (ptr_lo == 0x00FF)
        cpu->addr_abs = (cpu_read(cpu, ptr & 0xFF00) << 8) | cpu_read(cpu, ptr);
    else
        cpu->addr_abs = (cpu_read(cpu, ptr + 1) << 8) | cpu_read(cpu, ptr);

    return 0;
}

uint8_t IZX(_2A03CPU* cpu) {
    uint16_t t = cpu_read(cpu, cpu->PC); cpu->PC++;
    uint16_t lo = cpu_read(cpu, (t + cpu->X) & 0x00FF);
    uint16_t hi = cpu_read(cpu, (t + cpu->X + 1) & 0x00FF);
    cpu->addr_abs = (hi << 8) | lo;
    return 0;
}

uint8_t IZY(_2A03CPU* cpu) {
    uint16_t t = cpu_read(cpu, cpu->PC); cpu->PC++;
    uint16_t lo = cpu_read(cpu, t & 0x00FF);
    uint16_t hi = cpu_read(cpu, (t + 1) & 0x00FF);
    uint16_t base = (hi << 8) | lo;
    cpu->addr_abs = base + cpu->Y;
    return ((cpu->addr_abs & 0xFF00) != (base & 0xFF00)) ? 1 : 0;
}

void destroy_cpu(_2A03CPU* cpu)
{
    free(cpu->bus);
}