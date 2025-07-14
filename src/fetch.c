#include "fetch.h"
#include <stdio.h>
#include <stdlib.h>

uint32_t fetch_instruction(VirtualMachine *vm) {
    if (vm->program_counter + 4 > SIZE_OF_MEMORY) {
        fprintf(stderr, "Program counter out of memory bounds\n");
        return -1;
    }
    uint32_t instruction = *(uint32_t *)(vm->memory + vm->program_counter);
    vm->program_counter += 4;
    return instruction;
}

int fetch(VirtualMachine *vm, Instruction *inst) {
    inst->inst = fetch_instruction(vm);
    if (inst->inst == 0) {
        return -1; // Return error for null instructions (or end of program)
    }
    inst->left = 0;
    inst->right = 0;
    inst->disp_strval = 0;
    inst->rd = 0;
    inst->rs1 = 0;
    inst->rs2 = 0;
    inst->funct3 = 0;
    inst->funct7 = 0;
    inst->memop = 0;
    inst->aluop = 0;
    inst->opcode = 0;   
    return 0;
}