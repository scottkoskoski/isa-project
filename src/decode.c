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
        default:
            inst->type = UNSUPPORTED_TYPE;
            inst->aluop = Nop;
            break;
    }
}

void print_decoded_instruction(const Instruction *inst) {
    printf("Instruction: 0x%08X\n", inst->inst);
    printf("Opcode: 0x%02X\n", inst->opcode);
    printf("rd: %u, rs1: %u, rs2: %u\n", inst->rd, inst->rs1, inst->rs2);
    printf("funct3: 0x%X, funct7: 0x%X\n", inst->funct3, inst->funct7);
    printf("Immediate: 0x%08X\n", inst->disp_strval);
}
