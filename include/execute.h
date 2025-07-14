#ifndef EXECUTE_H
#define EXECUTE_H

#include "decode.h"

int32_t perform_alu_operation(AluOp operation, int32_t left, int32_t right);
void execute_instruction(Instruction *inst, int32_t *result);
int should_branch(Instruction *inst, int32_t alu_result);
void update_program_counter(VirtualMachine *vm, Instruction *inst, int32_t alu_result);
void execute_stage(VirtualMachine *vm, Instruction *inst, int32_t *result);

#endif // EXECUTE_H