#include "NES.h"

#include <stdlib.h>

bool INIT_NES(NES* nes)
{
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
    if (!nes || !nes->cpu)
        return false;

    for (int i = 0; i < CPU_CYCLES_PER_FRAME; i++) {
        if (!cpu_clock(nes->cpu))
            return false;

        /* TODO: clock the PPU 3x per CPU cycle and the APU once per
           CPU cycle here, once those components exist. */
    }

    return true;
}

void DESTROY_NES(NES* nes)
{
    if (nes->cpu) {
        destroy_cpu(nes->cpu);
        free(nes->cpu);
        nes->cpu = NULL;
    }

    /* nes->bus is not freed separately — it's owned by the CPU
       (freed inside destroy_cpu) and nes->bus was only a reference to it. */
    nes->bus = NULL;
}