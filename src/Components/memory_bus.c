#include "memory_bus.h"


uint8_t bus_read(memory_bus* bus, uint16_t addr) {
    if (addr < 0x2000)
        return bus->ram[addr & 0x07FF];

    /* TODO: route to PPU/APU registers, cartridge/mapper space, etc.
       For now fall back to the raw backing array so every address
       returns a defined value instead of undefined behavior. */
    return bus->non_ram[addr - 0x2000];
}

void bus_write(memory_bus* bus, uint16_t addr, uint8_t value) {
    if (addr < 0x2000) {
        bus->ram[addr & 0x07FF] = value;
        return;
    }

    /* TODO: route to PPU/APU registers, cartridge/mapper space, etc. */
    bus->non_ram[addr - 0x2000] = value;
}