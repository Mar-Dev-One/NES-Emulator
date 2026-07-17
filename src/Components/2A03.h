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

typedef enum CPUFlag
{
    FLAG_C = (1 << 0),
    FLAG_Z = (1 << 1),
    FLAG_I = (1 << 2),
    FLAG_D = (1 << 3),
    FLAG_B = (1 << 4),
    FLAG_U = (1 << 5),
    FLAG_V = (1 << 6),
    FLAG_N = (1 << 7)

} CPUFlag;


bool init_cpu(_2A03CPU* cpu);


bool cpu_reset(_2A03CPU* cpu);

void set_flag(_2A03CPU* cpu, CPUFlag flag, bool value);
bool get_flag(_2A03CPU* cpu, CPUFlag flag);

uint8_t cpu_read(_2A03CPU* cpu, uint16_t addr);
void cpu_write(_2A03CPU* cpu, uint16_t addr, uint8_t value);


void destroy_cpu(_2A03CPU* cpu);