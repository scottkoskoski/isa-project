#include <stdio.h>
#include "execute.h"

int32_t perform_alu_operation(AluOp operation, int32_t left, int32_t right) {
    switch (operation) {
        case Add: return left + right;
        case Sub: return left - right;
        default: return 0;
    }
}

void execute_instruction(Instruction *inst, int32_t *result) {
    *result = perform_alu_operation(inst->aluop, inst->left, inst->right);
}
