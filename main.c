#include <stdio.h>
#include <string.h>
#include "machine.h"
#include "fetch.h"
#include "decode.h"
#include "execute.h"
#include "memory.h"
#include "writeback.h"
#include "load_elf.h"

int load_elf_file(const char *filename, uint8_t *memory, uint32_t memory_size, uint32_t *program_counter, ELFHeader *elf_header) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        fprintf(stderr, "Could not open file %s\n", filename);
        return -1;
    }

    if (fread(elf_header, 1, sizeof(ELFHeader), file) != sizeof(ELFHeader)) {
        fprintf(stderr, "Could not read ELF Header\n");
        fclose(file);
        return -1;
    }

    fseek(file, elf_header->e_phoff, SEEK_SET);
    for (int i = 0; i < elf_header->e_phnum; i++) {
        ELFProgramHeader program_header;
        if (fread(&program_header, 1, sizeof(ELFProgramHeader), file) != sizeof(ELFProgramHeader)) {
            fprintf(stderr, "Could not read program header\n");
            fclose(file);
            return -1;
        }

        if (program_header.p_type == PT_LOAD) {
            if (program_header.p_vaddr + program_header.p_memsz > memory_size) {
                fprintf(stderr, "Program header does not fit into memory\n");
                fclose(file);
                return -1;
            }

            fseek(file, program_header.p_offset, SEEK_SET);
            fread(memory + program_header.p_vaddr, 1, program_header.p_filesz, file);

            if (program_header.p_memsz > program_header.p_filesz) {
                memset(memory + program_header.p_vaddr + program_header.p_filesz, 0, program_header.p_memsz - program_header.p_filesz);
            }
        }
    }

    *program_counter = elf_header->e_entry;
    fclose(file);
    return 0;
}

int check_elf_file(const ELFHeader *elf_header) {
    if (memcmp(elf_header->e_ident, "\x7f""ELF", 4) != 0) {
        fprintf(stderr, "e_ident value is not valid\n");
        return -1;
    }

    if (elf_header->e_type != ET_EXEC || elf_header->e_machine != EM_RISCV) {
        fprintf(stderr, "Invalid ELF file for this machine\n");
        return -1;
    }

    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <ELF file>\n", argv[0]);
        return -1;
    }

    const char *filename = argv[1];
    VirtualMachine vm;
    initialize_machine(&vm);

    ELFHeader elf_header;

    // Load ELF file and validate
    if (load_elf_file(filename, vm.memory, SIZE_OF_MEMORY, &vm.program_counter, &elf_header) != 0) {
        free_machine(&vm);
        return -1;
    }

    if (check_elf_file(&elf_header) != 0) {
        free_machine(&vm);
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

        // Print decoded instruction for debugging
        print_decoded_instruction(&inst);

        // Execute the instruction
        int32_t result;
        execute_instruction(&inst, &result);

        // Perform memory operations
        memory_stage(&vm, &inst, &result);

        // Perform writeback stage
        writeback_stage(&vm, &inst, result);

        // Print result of the writeback stage
        printf("Result after writeback stage: 0x%08X\n", result);
    }

    free_machine(&vm);
    return 0;
}
