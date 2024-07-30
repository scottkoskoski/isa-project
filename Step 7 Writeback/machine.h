/* Scott Koskoski
 * 6/16/2024
 * RISC-V ISA Emulator Project
 * Part 1 - Machine
 *
 * Creating the virtual machine with register file, RAM, and ALU core unit.
 * */

#ifndef MACHINE_H
#define MACHINE_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM_OF_REGISTERS 32
#define SIZE_OF_MEMORY (1 << 20) // 1 MiB of memory (2^20)

// Building the VM
typedef struct {
    uint32_t registers[NUM_OF_REGISTERS]; // 32 32-bit registers
    uint32_t program_counter; // program counter
    uint8_t *memory; // memory
} VirtualMachine;

// Initializing the machine for start-up
void initialize_machine(VirtualMachine *vm) {
    for (int i = 0; i < NUM_OF_REGISTERS; i++) {
        vm->registers[i] = 0;
    } // initializing each register to 0

    vm->program_counter = 0; // initializing program counter to 0 (first position)

    vm->memory = malloc(SIZE_OF_MEMORY); // initializing memory

    memset(vm->memory, 0, SIZE_OF_MEMORY); // setting all memory to 0 initially
}

// Freeing memory
void free_machine(VirtualMachine vm) {
    free(vm.memory);
}

// sign-extension function that takes arguments for the value and the sign bit location
int32_t extend_sign_bit(int32_t value, uint8_t sign_bit_location) {
    // first calculate how far to shift left to the most significant bit
    int8_t shift_magnitude = 31 - sign_bit_location;
    // shift the value left so the sign bit is the most significant bit
    // then shift back to the right and fill in the shifted bits with the sign bit
    return (value << shift_magnitude) >> shift_magnitude;
}

// API for reading and writing to registers

// Read
int32_t read_from_register(VirtualMachine *vm, uint8_t register_index) {
    if (register_index >= NUM_OF_REGISTERS) {
        fprintf(stderr, "Register is out of bounds - %u > 31\n", register_index);
        return 0;
    }

    // Zero register must return 0
    if (register_index == 0) {
        return 0;
    }
    return vm->registers[register_index];
}

// Write
void write_to_register(VirtualMachine *vm, uint8_t register_index, uint32_t value_to_write) {
    if (register_index >= NUM_OF_REGISTERS) {
        fprintf(stderr, "Register is out of bounds - %u > 31\n", register_index);
        return;
    }

    // Ignore all writes to the zero register
    if (register_index == 0) {
        return;
    }

    vm->registers[register_index] = value_to_write;
}

#endif // MACHINE_H

                          
