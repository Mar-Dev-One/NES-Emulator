#pragma once

#include <stdbool.h>

#include "2A03.h"
#include "memory_bus.h"

typedef struct NES
{
    _2A03CPU* cpu;
    memory_bus* bus;


} NES;

bool INIT_NES(NES* NES);

bool ON_UPDATE_NES(NES* nes);

void DESTROY_NES(NES* NES);