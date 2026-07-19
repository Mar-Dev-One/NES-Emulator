#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

/* iNES header is 16 bytes: "NES\x1A", PRG size (16KB units),
   CHR size (8KB units), two flag bytes, then padding. Only mapper 0
   (NROM) is supported so far -- enough for nestest and a handful of
   early launch titles (Donkey Kong, Balloon Fight, etc). */
typedef struct Cartridge
{
    uint8_t* prg_rom;
    size_t   prg_rom_size;   /* in bytes */

    uint8_t* chr_rom;
    size_t   chr_rom_size;   /* in bytes; 0 means the board uses CHR-RAM */

    uint8_t  mapper_id;
    bool     four_screen;
    bool     vertical_mirroring;

} Cartridge;

/* Loads an iNES (.nes) file from disk. Returns false (and leaves
   cart zeroed) on a bad header, unsupported mapper, or I/O failure. */
bool cartridge_load(Cartridge* cart, const char* path);

/* CPU-side reads/writes to cartridge space ($8000-$FFFF for NROM).
   Returns false if the address isn't actually in cartridge range,
   so the bus knows to fall back to its own unmapped-region handling. */
bool cartridge_cpu_read(Cartridge* cart, uint16_t addr, uint8_t* value);
bool cartridge_cpu_write(Cartridge* cart, uint16_t addr, uint8_t value);

void cartridge_destroy(Cartridge* cart);