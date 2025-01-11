#ifndef FETCH_H
#define FETCH_H

#include "machine.h"

typedef enum {
    R_TYPE, I_TYPE, S_TYPE, B_TYPE, U_TYPE, J_TYPE, UNSUPPORTED_TYPE
} InstructionType;

typedef struct {
    uint32_t inst;
    uint32_t left;
    uint32_t right;
    uint32_t disp_strval;
    uint8_t rd;
    uint8_t rs1;
    uint8_t rs2;
    uint8_t funct3;
    uint8_t funct7;
    uint8_t memop;
    uint8_t aluop;
    uint8_t opcode;
    InstructionType type;
} Instruction;

uint32_t fetch_instruction(VirtualMachine *vm);
int fetch(VirtualMachine *vm, Instruction *inst);

#endif // FETCH_H