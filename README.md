# RISC-V ISA Emulator

This project is part of a graduate course in computer architecture, implementing a complete 32-bit RISC-V ISA emulator in C. The emulator provides full support for all integer, multiplication, and division instructions as defined in the RV32IM specification, following a standard instruction cycle approach divided into discrete pipeline stages.

## Architecture Overview

The emulator implements a classical five-stage pipeline architecture with proper separation of concerns across each stage. Each stage is responsible for a specific aspect of instruction processing, enabling clear code organization and maintainability.

### Pipeline Stages

**1. Virtual Machine Setup**

-   Allocates 1 MiB of memory space for program execution
-   Defines 32 general-purpose registers following RV32IM architecture
-   Includes program counter (PC) management and register initialization routines
-   Header: `machine.h` | Source: `machine.c`

**2. ELF File Loading**

-   Reads and validates ELF executable files for RISC-V architecture
-   Loads text and data segments into emulated memory at appropriate addresses
-   Sets the program counter to the program entry point from ELF header
-   Header: `load_elf.h` | Implementation: Integrated in `main.c`

**3. Fetch Stage**

-   Retrieves the next instruction from memory using the current PC value
-   Increments the PC by 4 bytes for sequential instruction execution
-   Handles memory bounds checking and error conditions
-   Header: `fetch.h` | Source: `fetch.c`

**4. Decode Stage**

-   Decodes fetched instructions into opcode, operands, and control signals
-   Supports all six RISC-V instruction formats (R, I, S, B, U, J types)
-   Extracts register indices, immediate values, and function codes
-   Header: `decode.h` | Source: `decode.c`

**5. Execute Stage**

-   Performs arithmetic, logical, and comparison operations via ALU
-   Handles branch condition evaluation and jump target calculation
-   Updates program counter for control flow instructions (branches, jumps)
-   Manages return address storage for JAL and JALR instructions
-   Header: `execute.h` | Source: `execute.c`

**6. Memory Stage**

-   Handles load and store operations with the virtual machine memory
-   Implements byte, halfword, and word memory access patterns
-   Processes system calls (ECALL) and breakpoints (EBREAK)
-   Provides comprehensive memory bounds checking
-   Header: `memory.h` | Source: `memory.c`

**7. Writeback Stage**

-   Writes computed results back to destination registers
-   Enforces RISC-V convention that register x0 always reads as zero
-   Completes the instruction execution cycle
-   Header: `writeback.h` | Source: `writeback.c`

## Supported Instructions

The emulator implements the complete RV32IM instruction set specification:

**Base Integer Instructions (RV32I)**

-   Arithmetic: ADD, SUB, ADDI
-   Logical: AND, OR, XOR, ANDI, ORI, XORI
-   Shifts: SLL, SRL, SRA, SLLI, SRLI, SRAI
-   Comparisons: SLT, SLTU, SLTI, SLTIU
-   Memory: LB, LH, LW, LBU, LHU, SB, SH, SW
-   Branches: BEQ, BNE, BLT, BGE, BLTU, BGEU
-   Jumps: JAL, JALR
-   Upper immediates: LUI, AUIPC
-   System: ECALL, EBREAK

**Multiplication and Division Extension (RV32M)**

-   Multiplication: MUL, MULH, MULHSU, MULHU
-   Division: DIV, DIVU, REM, REMU

## Directory Structure

```
project/
├── include/
│   ├── machine.h          # Virtual machine and register definitions
│   ├── fetch.h            # Instruction fetch stage interface
│   ├── decode.h           # Instruction decode stage interface
│   ├── execute.h          # Execution and ALU operations interface
│   ├── memory.h           # Memory access stage interface
│   ├── writeback.h        # Register writeback stage interface
│   ├── load_elf.h         # ELF file format definitions
│   └── alu.h              # ALU operation definitions
├── src/
│   ├── machine.c          # Virtual machine implementation
│   ├── fetch.c            # Instruction fetch implementation
│   ├── decode.c           # Instruction decode implementation
│   ├── execute.c          # Execution stage and PC control
│   ├── memory.c           # Memory operations implementation
│   ├── writeback.c        # Register writeback implementation
│   └── alu.c              # ALU operation mapping
├── main.c                 # Main emulator loop and ELF loading
├── Makefile              # Build configuration
└── README.md             # Project documentation
```

## Build and Execution

**Prerequisites**

-   GCC compiler with C99 support
-   Make build system
-   RISC-V cross-compiler (for generating test programs)

**Compilation**

1. Navigate to the project root directory
2. Compile the emulator using the provided Makefile:
    ```bash
    make
    ```

**Execution**
Run the emulator with a RISC-V ELF executable:

```bash
./riscv_emulator <ELF_FILE>
```

**Cleanup**
Remove build artifacts:

```bash
make clean
```

## Testing

A simple test program is provided to verify emulator funcionality:

**Prerequisites:**

```bash
sudo apt install binutils-risc64-linux-gnu
```

Compile the test program:

```bash
riscv64-linux-gnu-as -march=rv32im -mabi=ilp32 -o test.o test.s
riscv64-linux-gnu-ld -m elf32lriscv -o test test.o
```

Run the test:

```bash
./riscv_emulator test
```

## Features and Specifications

**Current Implementation**

-   Complete RV32IM instruction set support with proper semantics
-   Full pipeline implementation with realistic stage separation
-   Comprehensive error handling and bounds checking
-   ELF file loading with program header processing
-   System call interface supporting program termination
-   Configurable instruction execution limits for testing
-   Detailed instruction tracing and debugging output

**Architecture Compliance**

-   32-bit RISC-V architecture (RV32IM)
-   32 general-purpose registers with x0 hardwired to zero
-   1 MiB virtual memory space
-   Little-endian memory organization
-   Standard RISC-V calling conventions

**Testing and Validation**

-   Instruction limit protection against infinite loops
-   Memory access bounds validation
-   ELF file format verification
-   Comprehensive error reporting and diagnostics

## Technical Notes

The emulator demonstrates several important computer architecture concepts including pipelined instruction execution, instruction set architecture implementation, and virtual machine design. The modular structure facilitates understanding of how modern processors handle instruction processing across multiple stages.

The implementation prioritizes correctness and clarity over performance optimization, making it suitable for educational purposes and architecture research. All RISC-V specification requirements are properly implemented, including edge cases such as division by zero handling and proper branch condition evaluation.

## Future Enhancements

Potential areas for extension include floating-point instruction support (RV32F), compressed instruction set (RV32C), privileged architecture features, and performance optimization techniques such as branch prediction and caching simulation.

```

```
