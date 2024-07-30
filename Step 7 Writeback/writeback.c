/* Scott Koskoski
 * 7/30/2024
 * RISC-V ISA Emulator Project
 * Part 7 - Writeback 
 */

#include <stdio.h>
#include <stdlib.h>
#include "fetch.h"
#include "load_elf.h"
#include "machine.h"
#include "alu.h"
#include "decode.h"
#include "execute.h"
#include "memory.h"

// Define the system call numbers
#define SYS_EXIT 0
#define SYS_PUTCHAR 1
#define SYS_GETCHAR 2
#define SYS_DEBUG 3

// Perform system calls
void perform_system_call(VirtualMachine *vm) {
    uint32_t syscall_num = vm->registers[17]; // a7 register (x17)
    uint32_t a0 = vm->registers[10]; // a0 register (x10)
    
    switch (syscall_num) {
        case SYS_EXIT:
            exit(a0); // Exit the virtual machine with the exit code in a0
            break;
        case SYS_PUTCHAR:
            putchar((char)a0); // Output the character in a0 to standard output
            break;
        case SYS_GETCHAR:
            vm->registers[10] = (uint32_t)getchar(); // Read a character from standard input into a0
            break;
        case SYS_DEBUG:
            printf("Debug: a0 = %u\n", a0); // Debug output
            break;
        default:
            fprintf(stderr, "Unknown system call: %u\n", syscall_num);
            break;
    }
}

// Writeback stage
void writeback_stage(VirtualMachine *vm, Instruction *inst, int32_t result) {
    // Write the result to the destination register unless it's the zero register
    if (inst->rd != 0) {
        vm->registers[inst->rd] = result;
    }
    
    // Handle system calls
    if (inst->opcode == 0x73) { // ECALL opcode
        perform_system_call(vm);
    }
    
    // Update the program counter
    if (inst->type == J_TYPE) {
        vm->program_counter += inst->disp_strval; // J-type: add displacement to the program counter
    } else if (inst->type == B_TYPE) {
        if (result) { // If the condition is true
            vm->program_counter += inst->disp_strval;
        } else {
            vm->program_counter += 4; // Otherwise, advance to the next instruction
        }
    } else {
        vm->program_counter += 4; // Non-branching instructions advance by 4
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

    while (1) {
        Instruction inst;
        if (fetch(&vm, &inst) != 0) {
            fprintf(stderr, "Error fetching instruction\n");
            break;
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

        // Perform writeback stage
        writeback_stage(&vm, &inst, result);

        // Print ALU operation result
        printf("Result after writeback stage: 0x%08X\n", result);
    }

    free_machine(vm);
    return 0;
}
