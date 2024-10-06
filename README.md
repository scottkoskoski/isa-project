# isa-project

### RISC-V ISA Emulator

This project is part of a graduate course in computer architecture, where I am building a 32-bit RISC-V ISA emulator in C. The emulator includes support for all integer, multiplication, and division instructions as defined in the RV32IM specification. The emulator follows a standard instruction cycle approach, divided into several stages:

### Stages

1. **Virtual Machine Setup**
   - The first stage of the project is the setup of the virtual machine (VM), which emulates the RISC-V CPU and memory.
   - We allocate 1 MiB of memory and define 32 registers, as per the RV32IM architecture. The VM also includes a program counter (PC) register.
   - Header: `machine.h`
     - This file defines the structure and memory of the virtual machine, including:
       - Register file (32 registers of 32 bits each).
       - The PC register.
       - Memory array for the emulated environment.
     - This file also defines initialization routines for setting up memory, registers, and handling the program's entry point.
  
2. **Read ELF File**
   - The second stage involves reading an ELF (Executable and Linkable Format) file, which is a common binary format used for RISC-V programs.
   - This stage loads the ELF file into the virtual machine's memory.
   - Source: `load_elf.c`
   - Header: `load_elf.h`
     - The code in this stage reads the ELF file sections, relocates them in memory, and updates the entry point of the program in the VM's PC register.
     - It checks the ELF headers, validates them for compatibility with the RV32 architecture, and performs memory allocation for text and data segments.

3. **Fetch Cycle**
   - The fetch stage involves reading the next instruction from memory, based on the current value of the PC register.
   - The PC is used to access the memory location, and the instruction is fetched into a dedicated instruction register for further processing.
   - Header: `fetch.h`
     - The PC is incremented after each fetch (assuming sequential execution), unless a branch or jump instruction is encountered.
     - This stage forms the initial part of the instruction cycle, setting the stage for decoding and execution.
     - The instruction is stored in the fetched instruction register, which is passed to the decoder in the next stage.

4. **Decoding Cycle**
   - The decoding cycle interprets the instruction fetched in the previous stage.
   - The instruction is broken down into its respective opcode, source registers, destination register, and immediate values (if any).
   - Header: `decode.h`
     - This header file contains the logic to decode the binary representation of instructions into human-readable operations.
     - The decoder checks the instruction type (R, I, S, B, U, J) and determines the operation (add, sub, load, store, branch, etc.).
     - The decoded instruction is forwarded to the execute stage, along with necessary operands extracted from the register file.

5. **Execute Cycle**
   - The execute cycle carries out the operation defined by the decoded instruction.
   - Arithmetic, logical, and control flow instructions are executed in this stage.
   - Header: `execute.h`
     - This file defines how different operations are handled: add, subtract, multiply, divide, and logical operations (AND, OR, XOR).
     - It also contains code for handling branch and jump instructions, where the PC might be updated based on the result of the comparison.
     - The ALU (Arithmetic Logic Unit) performs the computation, and the result is either forwarded to the memory stage or directly to the writeback stage, depending on the instruction.

6. **Memory Cycle**
   - If the instruction is a load or store operation, the memory cycle is responsible for accessing the correct memory location.
   - For load instructions, data is fetched from memory; for store instructions, data is written to memory.
   - Header: `memory.h`
     - This file defines memory access functions, allowing the VM to read from or write to memory during execution.
     - Load operations retrieve values from memory addresses based on the computed effective address, while store operations write the computed result back to memory.

7. **Writeback Cycle**
   - The final stage in the cycle is the writeback stage, where the result of an arithmetic or memory operation is written back to the destination register.
   - This stage ensures the VM's register file is updated with the most recent data.
   - Source: `writeback.c`
     - This file handles writing the computed result from either the ALU or memory to the appropriate register.
     - Once the writeback is complete, the instruction cycle can proceed to the next instruction by going back to the fetch stage.

---

### How to Compile and Run

To compile the final stage (writeback.c) and test the emulator with an ELF file, follow these steps:

1. Navigate to the directory containing `writeback.c`.
2. Compile the emulator:
   ```bash```
   gcc -o writeback writeback.c
3. Run the emulator with your desired ELF file:
   ```bash```
   ./writeback <ELF FILE>

