#ifndef DECODE_H
#define DECODE_H

#include "fetch.h"
#include "machine.h"
#include "alu.h"

uint8_t get_opcode(uint32_t instruction);
void decode_instruction(VirtualMachine *vm, Instruction *inst);
void print_decoded_instruction(const Instruction *inst);

#endif // DECODE_H
