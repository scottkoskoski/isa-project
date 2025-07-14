#include "alu.h"

AluOp get_r_type_alu_op(uint8_t funct3, uint8_t funct7) {
    // R-type instruction mapping based on RISC-V specification
    switch (funct3) {
        case 0x0: // ADD/SUB
            return (funct7 == 0x20) ? Sub : Add;
        case 0x1: // SLL (Shift Left Logical)
            return LeftShift;
        case 0x2: // SLT (Set Less Than)
            return Slt;
        case 0x3: // SLTU (Set Less Than Unsigned)
            return SltU;
        case 0x4: // XOR
            return Xor;
        case 0x5: // SRL/SRA (Shift Right)
            return (funct7 == 0x20) ? RightShiftA : RightShiftL;
        case 0x6: // OR
            return Or;
        case 0x7: // AND
            return And;
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