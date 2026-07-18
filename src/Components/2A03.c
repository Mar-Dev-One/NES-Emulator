#include "2A03.h"

#include <string.h>

bool init_cpu(_2A03CPU* cpu)
{
    cpu->Accumulator = 0;
    cpu->X = 0;
    cpu->Y = 0;
    cpu->P = 0;
    cpu->PC = 0;
    cpu->SP = 0;

    cpu->bus = malloc(sizeof(memory_bus));
    if (!cpu->bus)
        return false;

    memset(cpu->bus->ram, 0, 0x0800);

    reset_cpu(cpu);

    return true;
}

void reset_cpu(_2A03CPU* cpu)
{
    cpu->SP -= 3;

    set_flag(cpu, FLAG_U, true);
    set_flag(cpu, FLAG_I, true);

    uint8_t lo = cpu_read(cpu, 0xFFFC);
    uint8_t hi = cpu_read(cpu, 0xFFFD);
    cpu->PC = (uint16_t)(hi << 8) | lo;
    
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

void destroy_cpu(_2A03CPU* cpu)
{
    free(cpu->bus);
}
