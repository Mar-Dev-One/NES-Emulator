#include "NES.h"

#include <stdlib.h>
#include <string.h>

bool INIT_NES(NES* nes)
{
    memset(nes, 0, sizeof(*nes));

    nes->cpu = malloc(sizeof(_2A03CPU));
    if (!nes->cpu)
        return false;

    if (!init_cpu(nes->cpu)) {
        free(nes->cpu);
        nes->cpu = NULL;
        return false;
    }

    /* The CPU owns the allocation; NES just keeps a reference so
       other components (PPU/APU, once added) can share the same bus. */
    nes->bus = nes->cpu->bus;
    nes->cart_loaded = false;

    return true;
}

bool NES_LOAD_CARTRIDGE(NES* nes, const char* rom_path)
{
    if (!nes || !nes->cpu || !nes->bus)
        return false;

    if (!cartridge_load(&nes->cart, rom_path))
        return false;

    bus_attach_cartridge(nes->bus, &nes->cart);
    nes->cart_loaded = true;

    /* This is the CPU's one and only reset: init_cpu() deliberately
       skipped it since cartridge space wasn't mapped in yet. Now that
       it is, reset_cpu() reads the real reset vector and sets PC. */
    reset_cpu(nes->cpu);

    return true;
}

/* NTSC 2A03 runs at ~1.789773 MHz; NTSC frames land at ~60.0988 Hz.
   That works out to ~29780.5 CPU cycles per frame. Real hardware
   doesn't run in whole-frame chunks like this -- the CPU, PPU, and APU
   are all clocked together, cycle by cycle, with the PPU running 3x
   the CPU's rate. This constant is a stand-in until the PPU/APU exist
   and everything is driven from one shared clock. */
#define CPU_CYCLES_PER_FRAME 29781

bool ON_UPDATE_NES(NES* nes)
{
    if (!nes || !nes->cpu || !nes->cart_loaded)
        return false;

    for (int i = 0; i < CPU_CYCLES_PER_FRAME; i++) {
        if (!cpu_clock(nes->cpu))
            return false;
        
        printf("Cycle %d : A=0x%02X X=0x%02X Y=0x%02X SP=0x%02X PC=0x%04X\n",
           i, nes->cpu->Accumulator, nes->cpu->X, nes->cpu->Y, nes->cpu->SP, nes->cpu->PC);

        /* TODO: clock the PPU 3x per CPU cycle and the APU once per
           CPU cycle here, once those components exist. */
    }

    return true;
}

void DESTROY_NES(NES* nes)
{
    if (!nes)
        return;

    if (nes->cart_loaded) {
        cartridge_destroy(&nes->cart);
        nes->cart_loaded = false;
    }

    if (nes->cpu) {
        destroy_cpu(nes->cpu);
        free(nes->cpu);
        nes->cpu = NULL;
    }

    /* nes->bus is not freed separately — it's owned by the CPU
       (freed inside destroy_cpu) and nes->bus was only a reference to it. */
    nes->bus = NULL;
}