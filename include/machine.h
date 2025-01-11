#ifndef MACHINE_H
#define MACHINE_H

#include <stdint.h>

#define NUM_OF_REGISTERS 32
#define SIZE_OF_MEMORY (1 << 20)

typedef struct {
    uint32_t registers[NUM_OF_REGISTERS];
    uint32_t program_counter;
    uint8_t *memory;
} VirtualMachine;

void initialize_machine(VirtualMachine *vm);
void free_machine(VirtualMachine *vm);
int32_t extend_sign_bit(int32_t value, uint8_t sign_bit_location);
int32_t read_from_register(VirtualMachine *vm, uint8_t register_index);
void write_to_register(VirtualMachine *vm, uint8_t register_index, uint32_t value);

#endif // MACHINE_H
