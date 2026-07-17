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
    memset(cpu->bus->ram, 'A', 0x0800);

    return true;
}

bool cpu_reset(_2A03CPU* cpu)
{

}

void set_flag(_2A03CPU* cpu, CPUFlag flag, bool value)
{
    if (value) {
        cpu->P |= flag;
    }else {
        cpu->P &= flag;
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
