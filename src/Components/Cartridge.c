#include "Cartridge.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INES_HEADER_SIZE 16
#define PRG_BANK_SIZE (16 * 1024)
#define CHR_BANK_SIZE (8 * 1024)
#define TRAINER_SIZE 512

bool cartridge_load(Cartridge* cart, const char* path)
{
    memset(cart, 0, sizeof(*cart));

    FILE* f = fopen(path, "rb");
    if (!f) {
        fprintf(stderr, "Cartridge: could not open '%s'\n", path);
        return false;
    }

    uint8_t header[INES_HEADER_SIZE];
    if (fread(header, 1, INES_HEADER_SIZE, f) != INES_HEADER_SIZE) {
        fprintf(stderr, "Cartridge: file too small for an iNES header\n");
        fclose(f);
        return false;
    }

    if (header[0] != 'N' || header[1] != 'E' || header[2] != 'S' || header[3] != 0x1A) {
        fprintf(stderr, "Cartridge: missing 'NES\\x1A' magic, not an iNES file\n");
        fclose(f);
        return false;
    }

    uint8_t prg_banks = header[4];
    uint8_t chr_banks = header[5];
    uint8_t flags6 = header[6];
    uint8_t flags7 = header[7];

    bool has_trainer = flags6 & 0x04;
    cart->vertical_mirroring = flags6 & 0x01;
    cart->four_screen = flags6 & 0x08;

    /* Mapper number is split across the low nibble of flags6 (bits 4-7)
       and the low nibble of flags7 (bits 4-7), forming the high nibble. */
    cart->mapper_id = (flags7 & 0xF0) | (flags6 >> 4);

    if (cart->mapper_id != 0) {
        fprintf(stderr, "Cartridge: mapper %u not supported yet (only mapper 0 / NROM)\n",
                cart->mapper_id);
        fclose(f);
        return false;
    }

    if (has_trainer)
        fseek(f, TRAINER_SIZE, SEEK_CUR);

    cart->prg_rom_size = (size_t)prg_banks * PRG_BANK_SIZE;
    cart->prg_rom = malloc(cart->prg_rom_size);
    if (!cart->prg_rom || fread(cart->prg_rom, 1, cart->prg_rom_size, f) != cart->prg_rom_size) {
        fprintf(stderr, "Cartridge: failed to read %zu bytes of PRG-ROM\n", cart->prg_rom_size);
        fclose(f);
        cartridge_destroy(cart);
        return false;
    }

    if (chr_banks > 0) {
        cart->chr_rom_size = (size_t)chr_banks * CHR_BANK_SIZE;
        cart->chr_rom = malloc(cart->chr_rom_size);
        if (!cart->chr_rom || fread(cart->chr_rom, 1, cart->chr_rom_size, f) != cart->chr_rom_size) {
            fprintf(stderr, "Cartridge: failed to read %zu bytes of CHR-ROM\n", cart->chr_rom_size);
            fclose(f);
            cartridge_destroy(cart);
            return false;
        }
    }
    /* chr_banks == 0 means the board expects CHR-RAM; that belongs to
       the PPU once it exists, not this cartridge struct. */

    fclose(f);

    printf("Cartridge: loaded '%s' -- mapper 0 (NROM), %u x 16KB PRG, %u x 8KB CHR, %s mirroring\n",
           path, prg_banks, chr_banks, cart->vertical_mirroring ? "vertical" : "horizontal");

    return true;
}

bool cartridge_cpu_read(Cartridge* cart, uint16_t addr, uint8_t* value)
{
    if (addr < 0x8000)
        return false;

    /* NROM: 32KB boards map straight through; 16KB boards mirror the
       single bank across both halves of $8000-$FFFF. */
    uint32_t mapped = addr & (cart->prg_rom_size > PRG_BANK_SIZE ? 0x7FFF : 0x3FFF);
    *value = cart->prg_rom[mapped];
    return true;
}

bool cartridge_cpu_write(Cartridge* cart, uint16_t addr, uint8_t value)
{
    (void)cart;
    (void)value;
    /* PRG-ROM is read-only on NROM; there's no PRG-RAM on this board.
       Silently ignore the write (real hardware would too, since there's
       no chip listening on the bus at that address). */
    return addr >= 0x8000;
}

void cartridge_destroy(Cartridge* cart)
{
    free(cart->prg_rom);
    free(cart->chr_rom);
    cart->prg_rom = NULL;
    cart->chr_rom = NULL;
    cart->prg_rom_size = 0;
    cart->chr_rom_size = 0;
}