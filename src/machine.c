#include "machine.h"
#include <stdlib.h>
#include <string.h>

void initialize_machine(VirtualMachine *vm) {
    memset(vm->registers, 0, sizeof(vm->registers));
    vm->program_counter = 0;
    vm->memory = malloc(SIZE_OF_MEMORY);
    memset(vm->memory, 0, SIZE_OF_MEMORY);
}

void free_machine(VirtualMachine *vm) {
    free(vm->memory);
}

int32_t extend_sign_bit(int32_t value, uint8_t sign_bit_location) {
    int shift = 31 - sign_bit_location;
    return (value << shift) >> shift;
}

int32_t read_from_register(VirtualMachine *vm, uint8_t register_index) {
    if (register_index == 0) return 0;
    return vm->registers[register_index];
}

void write_to_register(VirtualMachine *vm, uint8_t register_index, uint32_t value) {
    if (register_index != 0) {
        vm->registers[register_index] = value;
    }
}