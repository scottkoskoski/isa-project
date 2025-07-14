#include <stdio.h>
#include "execute.h"

int32_t perform_alu_operation(AluOp operation, int32_t left, int32_t right) {
    switch (operation) {
        case Add:
            return left + right;
        case Sub:
            return left - right;
        case Mul:
            return left * right;
        case Div:
            return (right != 0) ? left / right : 0; // Avoid division by zero
        case DivU:
            return (right != 0) ? (uint32_t)left / (uint32_t)right : 0;
        case Rem:
            return (right != 0) ? left % right : 0;
        case RemU:
            return (right != 0) ? (uint32_t)left % (uint32_t)right : 0;
        case LeftShift:
            return left << (right & 0x1F); // Mask to 5 bits for shift amount
        case RightShiftA:
            return left >> (right & 0x1F); // Arithmetic right shift
        case RightShiftL:
            return (uint32_t)left >> (right & 0x1F); // Logical right shift
        case Or:
            return left | right;
        case Xor:
            return left ^ right;
        case And:
            return left & right;
        case Slt:
            return (left < right) ? 1 : 0; // Set less than (signed)
        case SltU:
            return ((uint32_t)left < (uint32_t)right) ? 1 : 0; // Set less than unsigned
        case Nop:
        default:
            return 0;
    }
}

void execute_instruction(Instruction *inst, int32_t *result) {
    *result = perform_alu_operation(inst->aluop, inst->left, inst->right);
}