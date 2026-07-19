
#include <stdio.h>

#include "Components/NES.h"
#include "Components/Instructions.h"

/* Runs nestest.nes in "automated" mode: no display, no controller,
   just a fixed instruction stream starting at $C000 designed to
   exercise the CPU. The reference trace (nestest.log, downloadable
   alongside the ROM) gives the exact register state expected after
   every instruction -- diff this program's output against it to find
   the first opcode where this emulator disagrees with real hardware.

   NOTE: this prints opcode + register state, not a full disassembly
   with resolved operands (e.g. "LDX #$00"), since there's no
   disassembler yet. That's enough to catch flag/register bugs; a
   later pass can add operand formatting to match nestest.log
   byte-for-byte, including its PPU:x,y column once a PPU exists. */

int main(int argc, char** argv)
{
    const char* rom_path = argc > 1 ? argv[1] : "nestest.nes";

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

    /* nestest's automated mode bypasses the reset vector and starts
       execution directly at $C000. NES_LOAD_CARTRIDGE's reset_cpu()
       call already left SP at $FD with the I and U flags set, which
       matches what nestest expects -- only PC needs overriding. */
    nes.cpu->PC = 0xC000;

    /* nestest.log's own convention labels the first instruction
       CYC:7, accounting for the 7 cycles a real reset sequence
       burns before the CPU fetches its first opcode. */
    long total_cycles = 7;

    /* This bounds clock cycles, not instructions -- instructions take
       2-7 cycles each, so this comfortably covers nestest's full
       legal-opcode run (and most of the illegal-opcode section, where
       divergence is expected until those are implemented). Stepping
       cpu_clock() directly here (rather than ON_UPDATE_NES's
       whole-frame loop) is deliberate: tracing needs to inspect state
       at each instruction boundary, not just after a full frame. */
    const long max_cycles = 30000;

    for (long c = 0; c < max_cycles; c++) {
        if (nes.cpu->cycles_remaining == 0) {
            uint16_t pc = nes.cpu->PC;
            uint8_t opcode = cpu_read(nes.cpu, pc);
            const Instruction* instr = &opcode_table[opcode];

            printf("%04X  %02X  %-4s  A:%02X X:%02X Y:%02X P:%02X SP:%02X CYC:%ld\n",
                   pc, opcode, instr->name,
                   nes.cpu->Accumulator, nes.cpu->X, nes.cpu->Y, nes.cpu->P, nes.cpu->SP,
                   total_cycles);
        }

        cpu_clock(nes.cpu);
        total_cycles++;
    }

    DESTROY_NES(&nes);
    return 0;
}