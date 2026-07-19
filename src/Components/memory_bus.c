#include "memory_bus.h"
#include "Cartridge.h"

void bus_attach_cartridge(memory_bus* bus, Cartridge* cart)
{
    bus->cart = cart;
}

uint8_t bus_read(memory_bus* bus, uint16_t addr) {
    if (addr < 0x2000)
        return bus->ram[addr & 0x07FF];

    if (bus->cart) {
        uint8_t value;
        if (cartridge_cpu_read(bus->cart, addr, &value))
            return value;
    }

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

    if (bus->cart && cartridge_cpu_write(bus->cart, addr, value))
        return;

    /* TODO: route to PPU/APU registers, cartridge/mapper space, etc. */
    bus->non_ram[addr - 0x2000] = value;
}