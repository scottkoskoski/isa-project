# isa-project

### RISC-V ISA Emulator

This project is part of a graduate course in computer architecture, where I am building a 32-bit RISC-V ISA emulator in C. The emulator includes support for all integer, multiplication, and division instructions as defined in the RV32IM specification. It adheres to the standard instruction cycle approach, divided into several stages:

---

### Stages

1. **Virtual Machine Setup**
   - Allocates 1 MiB of memory and defines 32 registers (RV32IM architecture).
   - Includes a program counter (PC) and register initialization routines.
   - Header: `machine.h`
   - Source: `machine.c`

2. **Read ELF File**
   - Reads and validates ELF files, loading text and data segments into memory.
   - Updates the PC register with the program entry point.
   - Header: `load_elf.h`

3. **Fetch Cycle**
   - Fetches the next instruction from memory using the PC.
   - Increments the PC unless altered by branch or jump instructions.
   - Header: `fetch.h`
   - Source: `fetch.c`

4. **Decoding Cycle**
   - Decodes fetched instructions into opcode, operands, and control information.
   - Supports RV32IM instruction types (R, I, S, B, U, J).
   - Header: `decode.h`
   - Source: `decode.c`

5. **Execute Cycle**
   - Executes arithmetic, logical, and control flow operations.
   - Leverages an ALU to compute results or update the PC for branch/jump instructions.
   - Header: `execute.h`
   - Source: `execute.c`

6. **Memory Cycle**
   - Handles load and store operations, accessing the VM's memory.
   - Header: `memory.h`
   - Source: `memory.c`

7. **Writeback Cycle**
   - Writes results of operations back to the destination register.
   - Handles system calls (e.g., ECALL).
   - Header: `writeback.h`
   - Source: `writeback.c`

---

### Directory Structure

```
project/
├── include/
│   ├── machine.h
│   ├── fetch.h
│   ├── decode.h
│   ├── execute.h
│   ├── memory.h
│   ├── writeback.h
│   └── load_elf.h
├── src/
│   ├── machine.c
│   ├── fetch.c
│   ├── decode.c
│   ├── execute.c
│   ├── memory.c
│   ├── writeback.c
├── main.c
└── Makefile
```

---

### How to Compile and Run

1. Navigate to the root directory of the project.
2. Use the `Makefile` to compile the emulator:
   ```bash
   make
   ```
3. Run the emulator with an ELF file:
   ```bash
   ./riscv_emulator <ELF_FILE>
   ```
4. To clean up build artifacts:
   ```bash
   make clean
   ```

---

### Features and Future Work

- **Implemented Features**:
  - Full RV32IM support for integer, multiplication, and division operations.
  - Modular design with separate stages for fetch, decode, execute, memory, and writeback.

- **Planned Enhancements**:
  - Add support for floating-point operations (RV32IMF).
  - Implement debugging features like instruction tracing.

---

Feel free to reach out if you have any questions or suggestions for improving this project.