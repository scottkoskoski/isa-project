#include "alu.h"

AluOp get_r_type_alu_op(uint8_t funct3, uint8_t funct7) {
    // Example mapping based on RISC-V instruction set
    if (funct3 == 0x0 && funct7 == 0x00) return Add;
    if (funct3 == 0x0 && funct7 == 0x20) return Sub;
    if (funct3 == 0x1) return Mul;
    if (funct3 == 0x4) return Div;
    if (funct3 == 0x5 && funct7 == 0x00) return RightShiftL;
    if (funct3 == 0x5 && funct7 == 0x20) return RightShiftA;
    if (funct3 == 0x6) return Or;
    if (funct3 == 0x7) return And;
    return Nop; // Default to no operation
}