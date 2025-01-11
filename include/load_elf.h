#ifndef LOAD_ELF_H
#define LOAD_ELF_H

#include <stdint.h>

#define PT_LOAD 1
#define ET_EXEC 2       // ELF file type: Executable file
#define EM_RISCV 243    // Machine type: RISC-V
#define EI_NIDENT 16

typedef struct {
    uint8_t e_ident[EI_NIDENT];
    uint16_t e_type;
    uint16_t e_machine;
    uint32_t e_version;
    uint32_t e_entry;
    uint32_t e_phoff;
    uint32_t e_shoff;
    uint32_t e_flags;
    uint16_t e_ehsize;
    uint16_t e_phentsize;
    uint16_t e_phnum;
    uint16_t e_shentsize;
    uint16_t e_shnum;
    uint16_t e_shstrndx;
} ELFHeader;

typedef struct {
    uint32_t p_type;
    uint32_t p_offset;
    uint32_t p_vaddr;
    uint32_t p_paddr;
    uint32_t p_filesz;
    uint32_t p_memsz;
    uint32_t p_flags;
    uint32_t p_align;
} ELFProgramHeader;

#endif // LOAD_ELF_H
