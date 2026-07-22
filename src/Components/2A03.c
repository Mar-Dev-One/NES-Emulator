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

    cpu->nmi_pending = false;
    cpu->irq_line = false;
    cpu->jammed = false;

    cpu->bus = malloc(sizeof(memory_bus));
    if (!cpu->bus)
        return false;

    memset(cpu->bus->ram, 0, 0x0800);
    cpu->bus->cart = NULL;

    /* Deliberately NOT calling reset_cpu() here: it reads the reset
       vector from cartridge space, which isn't mapped in yet at this
       point. The caller resets explicitly once a cartridge is
       attached (see NES_LOAD_CARTRIDGE), so the CPU only ever resets
       once with real data behind it -- reset_cpu decrements SP
       relative to its current value, so resetting twice would leave
       SP wrong. */

    return true;
}

bool reset_cpu(_2A03CPU* cpu)
{
    cpu->nmi_pending = false;
    cpu->irq_line = false;
    cpu->jammed = false;

    cpu_read(cpu, cpu->PC);                       /* cycle 1: dummy opcode fetch */
    cpu_read(cpu, cpu->PC);                       /* cycle 2: dummy operand fetch, PC not advanced */

    cpu_read(cpu, 0x0100 + cpu->SP); cpu->SP--;   /* cycle 3: phantom push PCH (no write) */
    cpu_read(cpu, 0x0100 + cpu->SP); cpu->SP--;   /* cycle 4: phantom push PCL (no write) */
    cpu_read(cpu, 0x0100 + cpu->SP); cpu->SP--;   /* cycle 5: phantom push P (no write) */

    set_flag(cpu, FLAG_U, true);
    set_flag(cpu, FLAG_I, true);

    uint8_t lo = cpu_read(cpu, 0xFFFC);           /* cycle 6 */
    uint8_t hi = cpu_read(cpu, 0xFFFD);           /* cycle 7 */

    cpu->PC = (uint16_t)(hi << 8) | lo;

    return true;
}

/* Both IRQ and NMI push PC (high byte first, then low), then P with
   B cleared and U set, set FLAG_I so the handler isn't itself
   interrupted, then load PC from the interrupt's vector. The only
   differences are: IRQ checks FLAG_I first and can be skipped, IRQ
   uses $FFFE/$FFFF vs NMI's $FFFA/$FFFB, and the cycle count (7 vs 8). */

bool irq_cpu(_2A03CPU* cpu)
{
    if (get_flag(cpu, FLAG_I))
        return false;   /* masked -- caller falls back to normal fetch */

    stack_push(cpu, (cpu->PC >> 8) & 0x00FF);
    stack_push(cpu, cpu->PC & 0x00FF);

    set_flag(cpu, FLAG_B, false);
    set_flag(cpu, FLAG_U, true);
    set_flag(cpu, FLAG_I, true);
    stack_push(cpu, cpu->P);

    uint8_t lo = cpu_read(cpu, 0xFFFE);
    uint8_t hi = cpu_read(cpu, 0xFFFF);
    cpu->PC = (uint16_t)(hi << 8) | lo;

    cpu->cycles_remaining = 7;
    return true;
}

void nmi_cpu(_2A03CPU* cpu)
{
    stack_push(cpu, (cpu->PC >> 8) & 0x00FF);
    stack_push(cpu, cpu->PC & 0x00FF);

    set_flag(cpu, FLAG_B, false);
    set_flag(cpu, FLAG_U, true);
    set_flag(cpu, FLAG_I, true);
    stack_push(cpu, cpu->P);

    uint8_t lo = cpu_read(cpu, 0xFFFA);
    uint8_t hi = cpu_read(cpu, 0xFFFB);
    cpu->PC = (uint16_t)(hi << 8) | lo;

    cpu->cycles_remaining = 8;
}

void request_nmi(_2A03CPU* cpu) { cpu->nmi_pending = true; }
void set_irq_line(_2A03CPU* cpu, bool asserted) { cpu->irq_line = asserted; }

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
    /* A JAM/KIL opcode locks the bus for good on real hardware -- no
       further instructions fetch, and even NMI/IRQ can't break it out.
       Reflect that by simply freezing here. */
    if (cpu->jammed)
        return true;

    if (cpu->cycles_remaining == 0) {
        if (cpu->nmi_pending) {
            cpu->nmi_pending = false;
            nmi_cpu(cpu);
        } else if (cpu->irq_line && irq_cpu(cpu)) {
            /* serviced -- irq_cpu() already set up cycles_remaining */
        } else {
            cpu->opcode = cpu_read(cpu, cpu->PC);
            cpu->PC++;

            const Instruction* instr = &opcode_table[cpu->opcode];
            cpu->cycles_remaining = instr->cycles;

            uint8_t extra_from_addr = instr->addrmode(cpu);
            uint8_t extra_from_op = instr->operate(cpu);
            cpu->cycles_remaining += (extra_from_addr & extra_from_op);
        }
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