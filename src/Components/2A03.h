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