#include "writeback.h"
#include <stdio.h>

void writeback_stage(VirtualMachine *vm, Instruction *inst, int32_t result) {
    if (inst->rd != 0) {
        vm->registers[inst->rd] = result;
    }
}
