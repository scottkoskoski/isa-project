#include "decode.h"
#include "alu.h"
#include <stdio.h>

uint8_t get_opcode(uint32_t instruction) {
    return instruction & 0x7F;
}

void decode_instruction(VirtualMachine *vm, Instruction *inst) {
    uint8_t opcode = get_opcode(inst->inst);
    switch (opcode) {
        case 0x33: // R-TYPE
            inst->type = R_TYPE;
            inst->rd = (inst->inst >> 7) & 0x1F;
            inst->funct3 = (inst->inst >> 12) & 0x07;
            inst->rs1 = (inst->inst >> 15) & 0x1F;
            inst->rs2 = (inst->inst >> 20) & 0x1F;
            inst->funct7 = (inst->inst >> 25) & 0x7F;
            inst->left = read_from_register(vm, inst->rs1);
            inst->right = read_from_register(vm, inst->rs2);
            inst->aluop = get_r_type_alu_op(inst->funct3, inst->funct7);
            break;
        
        case 0x13: // I-TYPE (Immediate arithmetic)
            inst->type = I_TYPE;
            inst->rd = (inst->inst >> 7) & 0x1F;
            inst->funct3 = (inst->inst >> 12) & 0x07;
            inst->rs1 = (inst->inst >> 15) & 0x1F;
            inst->left = read_from_register(vm, inst->rs1);
            // Extract 12-bit immediate and sign extend
            int32_t imm = (inst->inst >> 20) & 0xFFF;
            inst->right = extend_sign_bit(imm, 11); // Sign extend from bit 11
            inst->disp_strval = inst->right;
            inst->aluop = get_i_type_alu_op(inst->funct3, inst->funct7);
            break;
            
        case 0x03: // I-TYPE (Load)
            inst->type = I_TYPE;
            inst->rd = (inst->inst >> 7) & 0x1F;
            inst->funct3 = (inst->inst >> 12) & 0x07;
            inst->rs1 = (inst->inst >> 15) & 0x1F;
            inst->left = read_from_register(vm, inst->rs1);
            // Extract 12-bit immediate and sign extend
            imm = (inst->inst >> 20) & 0xFFF;
            inst->right = extend_sign_bit(imm, 11);
            inst->disp_strval = inst->right;
            inst->memop = 1; // Load operation
            inst->aluop = Add; // Address calculation
            break;
            
        case 0x23: // S-TYPE (Store)
            inst->type = S_TYPE;
            inst->funct3 = (inst->inst >> 12) & 0x07;
            inst->rs1 = (inst->inst >> 15) & 0x1F;
            inst->rs2 = (inst->inst >> 20) & 0x1F;
            inst->left = read_from_register(vm, inst->rs1);
            inst->disp_strval = read_from_register(vm, inst->rs2); // Value to store
            // Extract 12-bit immediate (bits 31:25 and 11:7)
            imm = ((inst->inst >> 25) & 0x7F) << 5 | ((inst->inst >> 7) & 0x1F);
            inst->right = extend_sign_bit(imm, 11);
            inst->memop = 2; // Store operation
            inst->aluop = Add; // Address calculation
            break;
            
        case 0x73: // SYSTEM (ECALL, EBREAK)
            inst->type = I_TYPE;
            inst->rd = (inst->inst >> 7) & 0x1F;
            inst->funct3 = (inst->inst >> 12) & 0x07;
            inst->rs1 = (inst->inst >> 15) & 0x1F;
            imm = (inst->inst >> 20) & 0xFFF;
            if (imm == 0) {
                // ECALL - Environment call (system call)
                inst->aluop = Nop;
                inst->memop = 3; // Special value for system call
            } else if (imm == 1) {
                // EBREAK - Environment break (debugger breakpoint)
                inst->aluop = Nop;
                inst->memop = 4; // Special value for breakpoint
            }
            break;

        default:
            inst->type = UNSUPPORTED_TYPE;
            inst->aluop = Nop;
            break;
    }
}

void print_decoded_instruction(const Instruction *inst) {
    printf("Instruction: 0x%08X\n", inst->inst);
    printf("Opcode: 0x%02X\n", inst->opcode);
    printf("Type: %d\n", inst->type);
    printf("rd: %u, rs1: %u, rs2: %u\n", inst->rd, inst->rs1, inst->rs2);
    printf("funct3: 0x%X, funct7: 0x%X\n", inst->funct3, inst->funct7);
    printf("Left: 0x%08X, Right: 0x%08X\n", inst->left, inst->right);
    printf("disp_strval: 0x%08X\n", inst->disp_strval);
    printf("ALU Op: %d, Mem Op: %d\n", inst->aluop, inst->memop);
}