/* Scott Koskoski
 * 7/18/2024
 * RISC-V ISA Emulator Project
 * Part 5 - Execute
 */

#ifndef EXECUTE_H
#define EXECUTE_H

#include <stdint.h>
#include "alu.h"
#include "decode.h"

// Perform ALU operations
int32_t perform_alu_operation(AluOp operation, int32_t left, int32_t right) {
    int32_t result = 0;

    switch (operation) {
        case Add:
            result = left + right; // Signed addition
            break;
        case Sub:
            result = left - right; // Signed subtraction
            break;
        case Mul:
            result = left * right; // Signed multiplication
            break;
        case Div:
            if (right != 0) result = left / right; // Signed division, no divide by zero
            break;
        case DivU:
            if ((uint32_t)right != 0) result = (uint32_t)left / (uint32_t)right; // Unsigned division, no divide by zero
            break;
        case Rem:
            if ((uint32_t)right != 0) result = (uint32_t)left % (uint32_t)right; // Signed remainder/modulo, no divide by zero
            break;
        case RemU:
            if ((uint32_t)right != 0) result = (uint32_t)left % (uint32_t)right; // Unsigned reminder/modulo, no divide by zero
            break;
        case LeftShift:
            result = left << right; // Logical left shift
            break;
        case RightShiftA:
            result = left >> right; // Arithmetic right shift
            break;
        case RightShiftL:
            result = (uint32_t)left >> (uint32_t)right; // Logical right shift
            break;
        case Or:
            result = left | right; // Bitwise inclusive OR
            break;
        case Xor:
            result = left ^ right; // Bitwise exclusive OR
            break;
        case And:
            result = left & right; // Bitwise AND
            break;
        case Slt:
            result = left < right ? 1 : 0; // Less than (signed)
            break;
        case SltU:
            result = (uint32_t)left < (uint32_t)right ? 1 : 0; // Less than (unsigned)
            break;
        case Cmp:
            // Comparison result for branches
            result = 0;
            result |= (left == right) << 0;  // NE/EQ
            result |= (left < right) << 1;   // GE/LT
            result |= ((uint32_t)left < (uint32_t)right) << 2; // GEU/LTU
            break;
        case Nop:
        default:
            result = 0; // No operation
            break;
    }

    return result;
}

// Execute instruction
void execute_instruction(Instruction *inst, int32_t *result) {
    // Perform ALU operation
    *result = perform_alu_operation(inst->aluop, inst->left, inst->right);

    // ALU calculates memory address plus offset for LOAD and STORE instructions
    if (inst->memop == 1 || inst->memop == 2) {
        *result = inst->left + inst->right;
    }
}

#endif // EXECUTE_H
