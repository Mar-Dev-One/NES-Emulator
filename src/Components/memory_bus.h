#pragma once

#include <stdint.h>

typedef struct Bus {
    uint8_t ram[0x0800];
    uint8_t non_ram[0xFFFF - 0x0800];
} memory_bus;

uint8_t bus_read(memory_bus* bus, uint16_t addr);

void bus_write(memory_bus* bus, uint16_t addr, uint8_t value);