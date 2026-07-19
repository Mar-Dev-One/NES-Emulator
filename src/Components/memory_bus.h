#pragma once

#include <stdint.h>

typedef struct Cartridge Cartridge;

typedef struct Bus {
    uint8_t ram[0x0800];
    uint8_t non_ram[0x10000 - 0x0800];

    /* NULL until a ROM is loaded and attached; when set, reads/writes
       in cartridge space ($8000-$FFFF for NROM) are routed there
       instead of falling back to the raw non_ram placeholder. */
    Cartridge* cart;
} memory_bus;

void bus_attach_cartridge(memory_bus* bus, Cartridge* cart);

uint8_t bus_read(memory_bus* bus, uint16_t addr);

void bus_write(memory_bus* bus, uint16_t addr, uint8_t value);