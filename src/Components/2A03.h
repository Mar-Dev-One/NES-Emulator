#pragma once

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

#include "memory_bus.h"

typedef struct Instruction Instruction;

typedef struct _2A03CPU
{
    uint8_t Accumulator;

    uint8_t X;
    uint8_t Y;

    uint8_t P;

    uint16_t PC;

    uint8_t SP;

    /*
    Cycles left to burn from the instruction currently "in flight".
    Real 6502 hardware does one bus operation per cycle; this field
    lets cpu_step() model "the instruction takes N cycles" without
    needing a full per-cycle state machine yet. */
    uint8_t cycles_remaining;

    /* 
    Working state for the instruction currently being decoded/executed.
    Not architectural registers -- just scratch space so addressing-mode
    and operation handlers can communicate. */
    uint8_t opcode;      /* opcode byte fetched at the start of this instruction */
    uint16_t addr_abs;   /* effective address resolved by the addressing mode */
    uint16_t addr_rel;   /* signed branch offset, for relative addressing */
    uint8_t fetched;     /* operand byte fetched from addr_abs (or the accumulator) */

    memory_bus* bus;

    /* Pending interrupt lines. NMI is edge-triggered: something (the
       PPU, once it exists) calls request_nmi() once at the moment
       vblank starts, and the CPU services it and clears the flag at
       the next instruction boundary. IRQ is level-triggered on real
       hardware -- multiple sources (APU frame counter, mappers) can
       hold the line low at once -- but until those exist this is a
       single asserted/cleared bool as a stand-in; a future multi-source
       IRQ would OR them together before calling set_irq_line(). */
    bool nmi_pending;
    bool irq_line;

    /* Set by one of the 12 JAM/KIL illegal opcodes ($02,$12,$22,$32,
       $42,$52,$62,$72,$92,$B2,$D2,$F2). Real hardware locks the bus
       permanently and only a physical reset recovers; cpu_clock()
       checks this and stops stepping once it's set. */
    bool jammed;

} _2A03CPU;

typedef enum CPUFlag
{
    FLAG_C = (1 << 0),
    FLAG_Z = (1 << 1),
    FLAG_I = (1 << 2),
    FLAG_D = (1 << 3),
    FLAG_B = (1 << 4),
    FLAG_U = (1 << 5),
    FLAG_V = (1 << 6),
    FLAG_N = (1 << 7)

} CPUFlag;


bool init_cpu(_2A03CPU* cpu);


bool reset_cpu(_2A03CPU* cpu);

/* Services a maskable interrupt request: pushes PC and P, sets I,
   and jumps to the vector at $FFFE/$FFFF -- but only if FLAG_I is
   currently clear. Returns false (does nothing) if IRQ is masked.
   Call only at an instruction boundary (cycles_remaining == 0);
   cpu_clock() does this automatically via irq_line. */
bool irq_cpu(_2A03CPU* cpu);

/* Services a non-maskable interrupt: same push sequence as IRQ, but
   always runs regardless of FLAG_I, and jumps to $FFFA/$FFFB instead.
   Call only at an instruction boundary; cpu_clock() does this
   automatically via nmi_pending. */
void nmi_cpu(_2A03CPU* cpu);

/* Raises a one-shot NMI request. Serviced (and cleared) the next time
   cpu_clock() is between instructions -- intended to be called once
   per frame by the PPU at the start of vblank. */
void request_nmi(_2A03CPU* cpu);

/* Sets or clears the IRQ line. Level-triggered: leave it asserted
   for as long as the interrupting source (APU frame IRQ, mapper IRQ)
   wants attention; the CPU will keep re-servicing it once per
   instruction boundary until FLAG_I is set or the source clears it. */
void set_irq_line(_2A03CPU* cpu, bool asserted);

void set_flag(_2A03CPU* cpu, CPUFlag flag, bool value);
bool get_flag(_2A03CPU* cpu, CPUFlag flag);

uint8_t cpu_read(_2A03CPU* cpu, uint16_t addr);
void cpu_write(_2A03CPU* cpu, uint16_t addr, uint8_t value);

/*
Advances the CPU by one clock cycle. If the current instruction has
finished, fetches and begins executing the next opcode. Returns true
on every cycle (reserved for future use, e.g. signalling a fault on
an illegal opcode). */
bool cpu_clock(_2A03CPU* cpu);

/* Pulls the operand byte for the current instruction's addressing
   mode. Skipped for IMP, which already put the accumulator's value
   in cpu->fetched. */
uint8_t fetch(_2A03CPU* cpu, const Instruction* instr);

/* RMW (read-modify-write) instructions write their result back to
   wherever the operand came from: the accumulator for IMP (e.g.
   "ASL A"), or memory otherwise. */
void write_back(_2A03CPU* cpu, const Instruction* instr, uint8_t value);

void stack_push(_2A03CPU* cpu, uint8_t value);
uint8_t stack_pop(_2A03CPU* cpu);

uint8_t IMP(_2A03CPU* cpu);
uint8_t IMM(_2A03CPU* cpu);
uint8_t ZP0(_2A03CPU* cpu);
uint8_t ZPX(_2A03CPU* cpu);
uint8_t ZPY(_2A03CPU* cpu);
uint8_t REL(_2A03CPU* cpu);
uint8_t ABS(_2A03CPU* cpu);
uint8_t ABX(_2A03CPU* cpu);
uint8_t ABY(_2A03CPU* cpu);
uint8_t IND(_2A03CPU* cpu);
uint8_t IZX(_2A03CPU* cpu);
uint8_t IZY(_2A03CPU* cpu);

void destroy_cpu(_2A03CPU* cpu);