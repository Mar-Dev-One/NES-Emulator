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

bool ON_UPDATE_NES(NES* nes)
{
    /* TODO: step the CPU one instruction / clock the PPU & APU. */
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