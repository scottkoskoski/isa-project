#include "memory.h"
#include <stdio.h>

void memory_stage(VirtualMachine *vm, Instruction *inst, int32_t *result) {
    uint32_t address = (uint32_t)(*result);
    if (inst->memop == 1) { // LOAD operation
        switch (inst->funct3) {
            case 0: // LB (Load Byte)
                *result = (int8_t)vm->memory[address];
                break;
            case 1: // LH (Load Halfword)
                *result = (int16_t)(*(int16_t*)&vm->memory[address]);
                break;
            case 2: // LW (Load Word)
                *result = *(int32_t*)&vm->memory[address];
                break;
            default:
                fprintf(stderr, "Unsupported LOAD funct3: %u\n", inst->funct3);
                break;
        }
    } else if (inst->memop == 2) { // STORE operation
        switch (inst->funct3) {
            case 0: // SB (Store Byte)
                vm->memory[address] = (uint8_t)(inst->disp_strval);
                break;
            case 1: // SH (Store Halfword)
                *(uint16_t*)&vm->memory[address] = (uint16_t)(inst->disp_strval);
                break;
            case 2: // SW (Store Word)
                *(uint32_t*)&vm->memory[address] = (uint32_t)(inst->disp_strval);
                break;
            default:
                fprintf(stderr, "Unsupported STORE funct3: %u\n", inst->funct3);
                break;
        }
    }
}
