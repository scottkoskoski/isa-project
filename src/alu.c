#include "alu.h"

AluOp get_r_type_alu_op(uint8_t funct3, uint8_t funct7) {
    // R-type instruction mapping based on RISC-V specification
    switch (funct3) {
        case 0x0: // ADD/SUB/MUL
            if (funct7 == 0x01) return Mul;        // MUL
            return (funct7 == 0x20) ? Sub : Add;   // SUB : ADD
        case 0x1: // SLL/MULH
            if (funct7 == 0x01) return Mul;        // MULH (treat as MUL for simplicity)
            return LeftShift;                      // SLL
        case 0x2: // SLT/MULHSU
            if (funct7 == 0x01) return Mul;        // MULHSU (treat as MUL for simplicity)
            return Slt;                            // SLT
        case 0x3: // SLTU/MULHU
            if (funct7 == 0x01) return Mul;        // MULHU (treat as MUL for simplicity)
            return SltU;                           // SLTU
        case 0x4: // XOR/DIV
            if (funct7 == 0x01) return Div;        // DIV
            return Xor;                            // XOR
        case 0x5: // SRL/SRA/DIVU
            if (funct7 == 0x01) return DivU;       // DIVU
            return (funct7 == 0x20) ? RightShiftA : RightShiftL; // SRA : SRL
        case 0x6: // OR/REM
            if (funct7 == 0x01) return Rem;        // REM
            return Or;                             // OR
        case 0x7: // AND/REMU
            if (funct7 == 0x01) return RemU;       // REMU
            return And;                            // AND
        default:
            return Nop;
    }
}

AluOp get_i_type_alu_op(uint8_t funct3, uint8_t funct7) {
    // I-type immediate instruction mapping
    switch (funct3) {
        case 0x0: // ADDI
            return Add;
        case 0x1: // SLLI (Shift Left Logical Immediate)
            return LeftShift;
        case 0x2: // SLTI (Set Less Than Immediate)
            return Slt;
        case 0x3: // SLTIU (Set Less Than Immediate Unsigned)
            return SltU;
        case 0x4: // XORI
            return Xor;
        case 0x5: // SRLI/SRAI (Shift Right Immediate)
            return (funct7 == 0x20) ? RightShiftA : RightShiftL;
        case 0x6: // ORI
            return Or;
        case 0x7: // ANDI
            return And;
        default:
            return Nop;
    }
}