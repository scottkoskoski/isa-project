/* Scott Koskoski
 * 6/17/2024
 * RISC-V ISA Emulator Project
 * Part 2 - ELF
 */

#ifndef LOAD_ELF_H
#define LOAD_ELF_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "machine.h" // Part 1 - machine

#define EI_NIDENT 4 // We only care about the first 4 bytes
#define EM_RISCV 243 // RISC-V machine target
#define ET_EXEC 2 // Executable
#define PT_LOAD 1

typedef struct
{
    uint8_t e_ident[EI_NIDENT]; // Identification bytes - [0]=0x&f, [1]=69, [2]=76, [3]=70 (spells \x7fELF)
    uint8_t e_bitsize; // 0=none, 1=32-bit, 2=64-bit
    uint8_t e_endian; // 0=none, 1=little-endian, 2=big-endian
    uint8_t e_filever; // Ignore
    uint8_t e_osabi; // Must be == 0
    uint8_t e_abiver; // Ignore
    uint8_t e_padding[7]; // Nothing, pads out e_type below to align by 16 bytes
    uint16_t e_type; // 0=none, 1=relocatable, 2=executable, 3=shared object
    uint16_t e_machine; // The machine target for this file. 243=RISC-V
    uint32_t e_version; // Must be == 1
    uint32_t e_entry; // The memory address of the starting instruction
    uint32_t e_phoff; // Program header offset from the top of the file
    uint32_t e_shoff; // Section header offset from the top of the file
    uint32_t e_flags; // Flags regarding the ELF file
    uint16_t e_ehsize; // The size of the ELF header in bytes
    uint16_t e_phentsize; // Program header entry size
    uint16_t e_phnum; // Number of program headers in the file
    uint16_t e_shentsize; // Section header entry size
    uint16_t e_shnum; // Number of section headers in the file
    uint16_t e_shstrndx; // The index of the string section of the section header
} ELFHeader;

typedef struct
{
    uint32_t p_type; // Segment type
    uint32_t p_offset; // The offset from the start of the file where the program data is located
    uint32_t p_vaddr; // The virtual address where this data will be in memory
    uint32_t p_paddr; // The physical address where this data will be in memory (typically ignored)
    uint32_t p_filesz; // Size of the data in the file
    uint32_t p_memsz; // Size of the data in memory
    uint32_t p_flags; // Flags about the section, including read, write, and execute permissions
    uint32_t p_align; // The alignment of this section
} ELFProgramHeader;

int load_elf_file(const char *filename, uint8_t *memory, uint32_t memory_size, uint32_t *program_counter, ELFHeader *elf_header)
{
    // Open file
    FILE *file = fopen(filename, "rb");
    if (!file)
    {
        fprintf(stderr, "Could not open file %s\n", filename);
        return -1;
    }

    // Read ELF header
    if (fread(elf_header, 1, sizeof(ELFHeader), file) != sizeof(ELFHeader))
    {
        fprintf(stderr, "Could not read ELF Header\n");
        fclose(file);
        return -1;
    }

    // Seek to the program table
    if (fseek(file, elf_header->e_phoff, SEEK_SET) != 0)
    {
        fprintf(stderr, "Failed seeking to program header\n");
        fclose(file);
        return -1;
    }


    // Read program headers
    for (int i = 0; i < elf_header->e_phnum; i++)
    {
        ELFProgramHeader program_header;
        if (fread(&program_header, 1, sizeof(ELFProgramHeader), file) != sizeof(ELFProgramHeader))
        {
            fprintf(stderr, "Could not read program header\n");
            fclose(file);
            return -1;
        }

        // Check that p_type is PT_LOAD
        if (program_header.p_type != PT_LOAD)
        {
            printf("Segment is not PT_LOAD, skipping segment.\n");
            continue;
        }

        if (program_header.p_type == PT_LOAD)
        {
            // Ensure program header fits into memory
            if (program_header.p_vaddr + program_header.p_memsz > memory_size)
            {
                fprintf(stderr, "Program header does not fit into memory\n");
                fclose(file);
                return -1;
            }

            // Seek in the ELF file to the byte offset given in p_offset
            if (fseek(file, program_header.p_offset, SEEK_SET) != 0) {
                fprintf(stderr, "Failed seeking to segment data\n");
                fclose(file);
                return -1;
            }

            // Read into memory
            if (fread(memory + program_header.p_vaddr, 1, program_header.p_filesz, file) != program_header.p_filesz)
            {
                fprintf(stderr, "Could not read segment into memory\n");
                fclose(file);
                return -1;
            }

            // Zero out remaining bytes if p_memsz > p_filesz
            if (program_header.p_memsz > program_header.p_filesz)
            {
                memset(memory + program_header.p_vaddr + program_header.p_filesz, 0, program_header.p_memsz - program_header.p_filesz);
            }
        }
        else
        {
            fprintf(stderr, "Program header could not be loaded\n");
            fclose(file);
            return -1;
        }
    }

    // Set entry point address (e_entry in the ELF header) in the program counter register
    *program_counter = elf_header->e_entry;

    // Close file
    fclose(file);
    return 0;
}

// Check if the ELF file is legit
// We don't want to load an ELF file that is not for our specific machine
int check_elf_file(const ELFHeader *elf_header) {

    // Check e_ident to verify it is an actual ELF file
    if (memcmp(elf_header->e_ident, "\x7f""ELF", 4) != 0)
    {
        fprintf(stderr, "e_ident value is not valid\n");
        return -1;
    }

    // Check e_type to verify the file is an executable
    if (elf_header->e_type != ET_EXEC)
    {
        fprintf(stderr, "File is not an executable\n");
        return -1;
    }

    // Check e_machine to verify the file targets RISC-V
    if (elf_header->e_machine != EM_RISCV)
    {
        fprintf(stderr, "File does not target RISC-V\n");
        return -1;
    }

    // Check e_bitsize for 32-bit architecture
    if (elf_header->e_bitsize != 1)
    {
        fprintf(stderr, "File is not a 32-bit ELF file\n");
        return -1;
    }

    // If all checks pass return success
    printf("ELF file is valid\n");
    return 0;
}

#endif // LOAD_ELF_H


