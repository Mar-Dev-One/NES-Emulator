#pragma once

#include <stdbool.h>

#include "2A03.h"
#include "memory_bus.h"
#include "Cartridge.h"

typedef struct NES
{
    _2A03CPU* cpu;
    memory_bus* bus;

    Cartridge cart;
    bool cart_loaded;

} NES;

/* Allocates and powers up the CPU/bus (registers zeroed, RAM cleared).
   No reset happens yet and no cartridge is attached -- call
   NES_LOAD_CARTRIDGE next, which performs the actual reset once
   cartridge space is mapped in and the reset vector means something. */
bool INIT_NES(NES* nes);

/* Loads an iNES ROM, attaches it to the bus, and re-runs reset_cpu so
   PC actually picks up the cartridge's reset vector instead of the
   zeroed placeholder memory it saw during INIT_NES. */
bool NES_LOAD_CARTRIDGE(NES* nes, const char* rom_path);

/* Runs one frame's worth of CPU cycles (~29781 NTSC cycles). Requires
   a cartridge to already be loaded. */
bool ON_UPDATE_NES(NES* nes);

void DESTROY_NES(NES* nes);