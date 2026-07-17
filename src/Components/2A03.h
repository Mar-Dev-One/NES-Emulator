#pragma once

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

#include "memory_bus.h"


typedef struct _2A03CPU
{
    uint8_t Accumulator;

    uint8_t X;
    uint8_t Y;

    uint8_t P;

    uint16_t PC;

    uint8_t SP;


    memory_bus* bus;

} _2A03CPU;


bool init_cpu(_2A03CPU* cpu);

uint8_t cpu_read(_2A03CPU* cpu, uint16_t addr);
void cpu_write(_2A03CPU* cpu, uint16_t addr, uint8_t value);

void destroy_cpu(_2A03CPU* cpu);