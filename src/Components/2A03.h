#pragma once

#include <stdbool.h>
#include <stdint.h>


#include "Core/defines.h"
#include "64Kmemory.h"


typedef struct _2A03CPU
{
    uint8_t Accumulator;

    uint8_t X;
    uint8_t Y;

    uint8_t P;

    uint16_t PC;

    uint8_t SP;


    _64KMEM* memory;

} _2A03CPU;

bool init_cpu(_2A03CPU* cpu)
{
    cpu->Accumulator = 0;
    cpu->X = 0;
    cpu->Y = 0;
    cpu->P = 0;
    cpu->PC = 0;
    cpu->SP = 0;

    cpu->memory = malloc(sizeof(_64KMEM));
    if (!init_memory(cpu->memory))
        return false;

    return true;
}

uint8_t read_memory(_2A03CPU* cpu, uint16_t memory_address)
{
    uint8_t* memory = cpu->memory->buffer;
    return memory[memory_address];
}

void write_memory(_2A03CPU* cpu, uint16_t memory_address, char c)
{
    uint16_t* memory = cpu->memory->buffer;
    memory[memory_address] = c;
}

void destroy_cpu(_2A03CPU* cpu)
{
    destroy_memory(cpu->memory);
    free(cpu->memory);
}
