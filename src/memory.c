#include "memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void memory_stage(VirtualMachine *vm, Instruction *inst, int32_t *result) {
    uint32_t address = (uint32_t)(*result);
    
    // Check memory bounds before accessing
    if (address >= SIZE_OF_MEMORY) {
        fprintf(stderr, "Memory access out of bounds: 0x%08X\n", address);
        exit(1);
    }
    
    if (inst->memop == 1) { // LOAD operation
        switch (inst->funct3) {
            case 0: { // LB (Load Byte)
                int8_t value = (int8_t)vm->memory[address];
                *result = (int32_t)value;
                break;
            }
            case 1: { // LH (Load Halfword)
                if (address + 1 >= SIZE_OF_MEMORY) {
                    fprintf(stderr, "Memory access out of bounds: 0x%08X\n", address);
                    exit(1);
                }
                int16_t value;
                memcpy(&value, &vm->memory[address], sizeof(int16_t));
                *result = (int32_t)value;
                break;
            }
            case 2: { // LW (Load Word)
                if (address + 3 >= SIZE_OF_MEMORY) {
                    fprintf(stderr, "Memory access out of bounds: 0x%08X\n", address);
                    exit(1);
                }
                int32_t value;
                memcpy(&value, &vm->memory[address], sizeof(int32_t));
                *result = value;
                break;
            }
            case 4: { // LBU (Load Byte Unsigned)
                uint8_t value = vm->memory[address];
                *result = (int32_t)value;
                break;
            }
            case 5: { // LHU (Load Halfword Unsigned)
                if (address + 1 >= SIZE_OF_MEMORY) {
                    fprintf(stderr, "Memory access out of bounds: 0x%08X\n", address);
                    exit(1);
                }
                uint16_t value;
                memcpy(&value, &vm->memory[address], sizeof(uint16_t));
                *result = (int32_t)value;
                break;
            }
            default:
                fprintf(stderr, "Unsupported LOAD funct3: %u\n", inst->funct3);
                break;
        }
    } else if (inst->memop == 2) { // STORE operation
        switch (inst->funct3) {
            case 0: { // SB (Store Byte)
                uint8_t value = (uint8_t)(inst->disp_strval);
                vm->memory[address] = value;
                break;
            }
            case 1: { // SH (Store Halfword)
                if (address + 1 >= SIZE_OF_MEMORY) {
                    fprintf(stderr, "Memory access out of bounds: 0x%08X\n", address);
                    exit(1);
                }
                uint16_t value = (uint16_t)(inst->disp_strval);
                memcpy(&vm->memory[address], &value, sizeof(uint16_t));
                break;
            }
            case 2: { // SW (Store Word)
                if (address + 3 >= SIZE_OF_MEMORY) {
                    fprintf(stderr, "Memory access out of bounds: 0x%08X\n", address);
                    exit(1);
                }
                uint32_t value = (uint32_t)(inst->disp_strval);
                memcpy(&vm->memory[address], &value, sizeof(uint32_t));
                break;
            }
            default:
                fprintf(stderr, "Unsupported STORE funct3: %u\n", inst->funct3);
                break;
        }
    } else if (inst->memop == 3) { // ECALL (System call)
        // Handle system calls based on register a7 (x17)
        uint32_t syscall_num = vm->registers[17]; // a7 register
        switch (syscall_num) {
            case 93: // sys_exit
                printf("Program terminated via ECALL (exit code: %d)\n", (int32_t)vm->registers[10]); // a0 register
                exit((int)vm->registers[10]);
                break;
            case 64: // sys_write
                // Simple implementation - just print to stdout
                printf("ECALL write: %d\n", (int32_t)vm->registers[10]);
                break;
            default:
                printf("Unsupported system call: %u\n", syscall_num);
                break;
        }
    } else if (inst->memop == 4) { // EBREAK
        printf("EBREAK encountered - stopping execution\n");
        exit(0);
    }
}