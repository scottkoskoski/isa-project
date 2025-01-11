// FILE: include/alu.h
#ifndef ALU_H
#define ALU_H

#include <stdint.h>

typedef enum {
    Add,
    Sub,
    Mul,
    Div,
    DivU,
    Rem,
    RemU,
    LeftShift,
    RightShiftA,
    RightShiftL,
    Or,
    Xor,
    And,
    Slt,
    SltU,
    Nop
} AluOp;

// Function declaration
AluOp get_r_type_alu_op(uint8_t funct3, uint8_t funct7);

#endif // ALU_H
