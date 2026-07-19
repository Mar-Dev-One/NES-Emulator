#include <stdio.h>

#include "Components/NES.h"

int main(int argc, char** argv)
{
    const char* rom_path = argc > 1 ? argv[1] : "C:\\Dev\\My\\NES-Emulator\\ROMs\\nestest.nes";

    NES nes;
    if (!INIT_NES(&nes)) {
        fprintf(stderr, "Failed to initialize NES\n");
        return 1;
    }

    if (!NES_LOAD_CARTRIDGE(&nes, rom_path)) {
        fprintf(stderr, "Failed to load cartridge '%s'\n", rom_path);
        DESTROY_NES(&nes);
        return 1;
    }

    printf("NES powered on. Reset vector -> PC=0x%04X\n", nes.cpu->PC);

    if (!ON_UPDATE_NES(&nes)) {
        fprintf(stderr, "Frame update failed\n");
        DESTROY_NES(&nes);
        return 1;
    }

    //printf("Ran one frame (~29781 CPU cycles). PC=0x%04X A=0x%02X X=0x%02X Y=0x%02X SP=0x%02X\n",
    //       nes.cpu->PC, nes.cpu->Accumulator, nes.cpu->X, nes.cpu->Y, nes.cpu->SP);

    DESTROY_NES(&nes);
    return 0;
}