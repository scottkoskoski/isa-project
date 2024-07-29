/* Scott Koskoski
 * 7/28/2024
 * RISC-V ISA Emulator Project
 * Part 6 - Memory
 */

#include <stdio.h>
#include "fetch.h"
#include "load_elf.h"
#include "machine.h"
#include "alu.h"
#include "decode.h"
#include "execute.h"
#include "memory.h"

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
            case 4: // LBU (Load Byte Unsigned)
                *result = (uint8_t)vm->memory[address];
                break;
            case 5: // LHU (Load Halfword Unsigned)
                *result = (uint16_t)(*(uint16_t*)&vm->memory[address]);
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

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <ELF file>\n", argv[0]);
        return -1;
    }

    const char *filename = argv[1];
    VirtualMachine vm;
    initialize_machine(&vm);
    initialize_stack_pointer(&vm);
    ELFHeader elf_header;

    // Load ELF file and get the ELF header
    if (load_elf_file(filename, vm.memory, SIZE_OF_MEMORY, &vm.program_counter, &elf_header) != 0) {
        return -1;
    }

    // Now pass pointer to header to check_elf_file (feedback from part 2 ELF grading)
    if (check_elf_file(&elf_header) != 0) {
        return -1;
    }

    printf("Program counter set to 0x%08X\n", vm.program_counter);

    Instruction inst;
    if (fetch(&vm, &inst) != 0) {
        fprintf(stderr, "Error fetching instruction\n");
        free_machine(vm);
        return -1;
    }

    printf("Fetched instruction: 0x%08X\n", inst.inst);

    // Decode the fetched instruction
    decode_instruction(&vm, &inst);

    // Print decoded instruction for verification
    print_decoded_instruction(&inst);

    // Execute the instruction
    int32_t result;
    execute_instruction(&inst, &result);

    // Perform memory operations
    memory_stage(&vm, &inst, &result);

    // Print ALU operation result
    printf("Result after memory stage: 0x%08X\n", result);

    free_machine(vm);
    return 0;
}
