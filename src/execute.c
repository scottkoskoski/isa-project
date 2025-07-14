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

int should_branch(Instruction *inst, int32_t alu_result) {
    switch (inst->funct3) {
        case 0x0: // BEQ (Branch if Equal)
            return (alu_result == 0);
        case 0x1: // BNE (Branch if Not Equal)
            return (alu_result != 0);
        case 0x4: // BLT (Branch if Less Than)
            return (alu_result == 1);
        case 0x5: // BGE (Branch if Greater or Equal)
            return (alu_result == 0);
        case 0x6: // BLTU (Branch if Less Than Unsigned)
            return (alu_result == 1);
        case 0x7: // BGEU (Branch if Greater or Equal Unsigned)
            return (alu_result == 0);
        default:
            return 0;
    }
}

void update_program_counter(VirtualMachine *vm, Instruction *inst, int32_t alu_result) {
    switch (inst->type) {
        case B_TYPE: // Handle conditional branches
            if (should_branch(inst, alu_result)) {
                vm->program_counter = (vm->program_counter - 4) + inst->disp_strval;
                printf("Branch taken to PC: 0x%08X\n", vm->program_counter);
            } else {
                printf("Branch not taken\n");
            }
            break;
            
        case J_TYPE: // JAL (Jump and Link)
            if (inst->opcode == 0x6F) {
                vm->program_counter = (vm->program_counter - 4) + inst->disp_strval;
                printf("JAL to PC: 0x%08X\n", vm->program_counter);
            }
            break;
            
        case I_TYPE: // JALR (Jump and Link Register)
            if (inst->opcode == 0x67) {
                vm->program_counter = alu_result & ~1; // Clear LSB as per RISC-V spec
                printf("JALR to PC: 0x%08X\n", vm->program_counter);
            }
            break;
            
        default:
            // No PC update needed for other instruction types
            break;
    }
}

void execute_instruction(Instruction *inst, int32_t *result) {
    *result = perform_alu_operation(inst->aluop, inst->left, inst->right);
}

void execute_stage(VirtualMachine *vm, Instruction *inst, int32_t *result) {
    // First perform the ALU operation
    execute_instruction(inst, result);
    
    // For JAL instructions, we need to store the return address (PC + 4)
    if (inst->type == J_TYPE && inst->opcode == 0x6F) {
        *result = vm->program_counter; // This is PC + 4 (return address)
    }
    
    // For JALR instructions, we need to store the return address (PC + 4)
    if (inst->type == I_TYPE && inst->opcode == 0x67) {
        int32_t target_address = *result; // Save target before overwriting
        *result = vm->program_counter; // Store return address
        // We'll handle the jump in update_program_counter using the original ALU result
        int32_t temp_result = target_address;
        update_program_counter(vm, inst, temp_result);
        return; // Skip the normal PC update call
    }
    
    // Handle PC updates for branches and jumps
    update_program_counter(vm, inst, *result);
}