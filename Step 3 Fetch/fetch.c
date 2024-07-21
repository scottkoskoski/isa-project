/* Scott Koskoski
 * 7/4/2024
 * RISC-V ISA Emulator Project
 * Part 3 - Fetch
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "load_elf.h" // Part 2 - Load ELF

// Defining the Instruction structure
typedef struct {
    uint32_t inst; // The four-byte instruction
    uint32_t left; // The left operand
    uint32_t right; // The right operand
    uint32_t disp_strval; // The displacement for BRANCH instruction or the value to store for STORE instructions
    uint8_t rd; // The destination register index (0-31)
    uint8_t memop; // 0 = No mem op, 1 = Load mem op, 2 = Store mem op
    uint8_t aluop; // ALU operation
} Instruction;

// Initializing stack pointer
void initialize_stack_pointer(VirtualMachine *vm) {
    vm->registers[2] = SIZE_OF_MEMORY; // x2 is the stack pointer - setting to one byte past the last memory address
}

// Fetch instruction
uint32_t fetch_instruction(VirtualMachine *vm) {
    // Make sure program counter isn't out of memory bounds
    if (vm->program_counter + 4 > SIZE_OF_MEMORY) {
        fprintf(stderr, "Program counter out memory bounds\n");
        return -1;
    }
    uint32_t instruction = *(uint32_t *)(vm->memory + vm->program_counter); //pointing to instruction at current pc value past bottom of memory
    vm->program_counter += 4; // increment pc by 4 bytes
    return instruction;
}

// Full fetch stage
// Fills the Instruction structure with the fetched instruction
int fetch(VirtualMachine *vm, Instruction *inst) {
    inst->inst = fetch_instruction(vm);
    // Initializing other fields to zero, but we will use them in later stages
    inst->left = 0;
    inst->right = 0;
    inst->disp_strval = 0;
    inst->rd = 0;
    inst->memop = 0;
    inst->aluop = 0;

    return 0;
}

int main(int argc, char *argv[]) {
    if (argc!=2) {
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
    if(fetch(&vm, &inst) != 0) {
        fprintf(stderr, "Error fetching instruction\n");
        free_machine(vm);
        return -1;
    }

    printf("Fetched instruction: 0x%08X\n", inst.inst);

    free_machine(vm);
    return 0;
}


                  