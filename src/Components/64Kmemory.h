#pragma once

#include <stdlib.h>
#include <stdbool.h>
#include <memory.h>

#include "Core/defines.h"


typedef struct _64KMEM
{
    void* buffer;
} _64KMEM;


bool init_memory(_64KMEM* memory)
{
    memory->buffer = malloc(CPU_ADDRESS_SPACE * sizeof(uint8_t));
    memset(memory->buffer, 'D', CPU_ADDRESS_SPACE * sizeof(uint8_t));
    return true;
}

void destroy_memory(_64KMEM* memory)
{
    free(memory->buffer);
}

