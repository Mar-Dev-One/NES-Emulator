#include "memory_bus.h"


uint8_t bus_read(memory_bus* bus, uint16_t addr) {
    if (addr < 0x2000)
        return bus->ram[addr & 0x07FF];

}

void bus_write(memory_bus* bus, uint16_t addr, uint8_t value) {
    if (addr < 0x2000)
        bus->ram[addr & 0x07FF] = value;
}