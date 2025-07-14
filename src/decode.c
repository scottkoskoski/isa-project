#include "decode.h"
#include "alu.h"
#include <stdio.h>

uint8_t get_opcode(uint32_t instruction) {
    return instruction & 0x7F;
}

void decode_instruction(VirtualMachine *vm, Instruction *inst) {
    uint8_t opcode = get_opcode(inst->inst);
    inst->opcode = opcode;
    
    switch (opcode) {
        case 0x33: // R-TYPE (Register-Register operations)
            inst->type = R_TYPE;
            inst->rd = (inst->inst >> 7) & 0x1F;
            inst->funct3 = (inst->inst >> 12) & 0x07;
            inst->rs1 = (inst->inst >> 15) & 0x1F;
            inst->rs2 = (inst->inst >> 20) & 0x1F;
            inst->funct7 = (inst->inst >> 25) & 0x7F;
            inst->left = read_from_register(vm, inst->rs1);
            inst->right = read_from_register(vm, inst->rs2);
            inst->aluop = get_r_type_alu_op(inst->funct3, inst->funct7);
            inst->memop = 0;
            break;
        
        case 0x13: // I-TYPE (Immediate arithmetic)
            inst->type = I_TYPE;
            inst->rd = (inst->inst >> 7) & 0x1F;
            inst->funct3 = (inst->inst >> 12) & 0x07;
            inst->rs1 = (inst->inst >> 15) & 0x1F;
            inst->left = read_from_register(vm, inst->rs1);
            
            // Extract 12-bit immediate and sign extend
            int32_t imm = (inst->inst >> 20) & 0xFFF;
            
            // For shift operations, extract funct7 and limit immediate to 5 bits
            if (inst->funct3 == 0x1 || inst->funct3 == 0x5) { // SLLI, SRLI, SRAI
                inst->funct7 = (inst->inst >> 25) & 0x7F;
                inst->right = imm & 0x1F; // Only use lower 5 bits for shift amount
            } else {
                inst->funct7 = 0;
                inst->right = extend_sign_bit(imm, 11); // Sign extend from bit 11
            }
            
            inst->disp_strval = inst->right;
            inst->aluop = get_i_type_alu_op(inst->funct3, inst->funct7);
            inst->memop = 0;
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
            inst->funct7 = 0;
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
            inst->funct7 = 0;
            inst->rd = 0; // No destination register for stores
            break;
            
        case 0x63: // B-TYPE (Branch)
            inst->type = B_TYPE;
            inst->funct3 = (inst->inst >> 12) & 0x07;
            inst->rs1 = (inst->inst >> 15) & 0x1F;
            inst->rs2 = (inst->inst >> 20) & 0x1F;
            inst->left = read_from_register(vm, inst->rs1);
            inst->right = read_from_register(vm, inst->rs2);
            
            // Extract 13-bit immediate for branch offset
            // Bits: 12|10:5|4:1|11 -> 31|30:25|11:8|7
            imm = ((inst->inst >> 31) & 0x1) << 12 |     // bit 12
                  ((inst->inst >> 7) & 0x1) << 11 |      // bit 11
                  ((inst->inst >> 25) & 0x3F) << 5 |     // bits 10:5
                  ((inst->inst >> 8) & 0xF) << 1;        // bits 4:1
            inst->disp_strval = extend_sign_bit(imm, 12);
            
            // Set ALU operation based on branch type for comparison
            switch (inst->funct3) {
                case 0x0: // BEQ
                case 0x1: // BNE
                    inst->aluop = Sub; // Compare by subtraction
                    break;
                case 0x4: // BLT
                case 0x5: // BGE
                    inst->aluop = Slt; // Signed comparison
                    break;
                case 0x6: // BLTU
                case 0x7: // BGEU
                    inst->aluop = SltU; // Unsigned comparison
                    break;
                default:
                    inst->aluop = Nop;
                    break;
            }
            
            inst->memop = 0;
            inst->funct7 = 0;
            inst->rd = 0; // No destination register for branches
            break;
            
        case 0x37: // U-TYPE (LUI - Load Upper Immediate)
            inst->type = U_TYPE;
            inst->rd = (inst->inst >> 7) & 0x1F;
            
            // Extract 20-bit immediate and shift left by 12
            imm = (inst->inst >> 12) & 0xFFFFF;
            inst->right = imm << 12;
            inst->left = 0; // LUI loads immediate directly
            
            inst->aluop = Add; // Simply pass through the immediate
            inst->memop = 0;
            inst->funct3 = 0;
            inst->funct7 = 0;
            inst->rs1 = 0;
            inst->rs2 = 0;
            inst->disp_strval = inst->right;
            break;
            
        case 0x17: // U-TYPE (AUIPC - Add Upper Immediate to PC)
            inst->type = U_TYPE;
            inst->rd = (inst->inst >> 7) & 0x1F;
            
            // Extract 20-bit immediate and shift left by 12
            imm = (inst->inst >> 12) & 0xFFFFF;
            inst->right = imm << 12;
            inst->left = vm->program_counter - 4; // Current PC (before increment)
            
            inst->aluop = Add; // Add immediate to PC
            inst->memop = 0;
            inst->funct3 = 0;
            inst->funct7 = 0;
            inst->rs1 = 0;
            inst->rs2 = 0;
            inst->disp_strval = inst->right;
            break;
            
        case 0x6F: // J-TYPE (JAL - Jump and Link)
            inst->type = J_TYPE;
            inst->rd = (inst->inst >> 7) & 0x1F;
            
            // Extract 21-bit immediate for jump offset
            // Bits: 20|10:1|11|19:12 -> 31|30:21|20|19:12
            imm = ((inst->inst >> 31) & 0x1) << 20 |      // bit 20
                  ((inst->inst >> 12) & 0xFF) << 12 |     // bits 19:12
                  ((inst->inst >> 20) & 0x1) << 11 |      // bit 11
                  ((inst->inst >> 21) & 0x3FF) << 1;      // bits 10:1
            inst->disp_strval = extend_sign_bit(imm, 20);
            
            inst->left = vm->program_counter - 4; // Current PC
            inst->right = inst->disp_strval;
            inst->aluop = Add; // Calculate target address
            inst->memop = 0;
            inst->funct3 = 0;
            inst->funct7 = 0;
            inst->rs1 = 0;
            inst->rs2 = 0;
            break;
            
        case 0x67: // I-TYPE (JALR - Jump and Link Register)
            inst->type = I_TYPE;
            inst->rd = (inst->inst >> 7) & 0x1F;
            inst->funct3 = (inst->inst >> 12) & 0x07;
            inst->rs1 = (inst->inst >> 15) & 0x1F;
            inst->left = read_from_register(vm, inst->rs1);
            
            // Extract 12-bit immediate and sign extend
            imm = (inst->inst >> 20) & 0xFFF;
            inst->right = extend_sign_bit(imm, 11);
            inst->disp_strval = inst->right;
            
            inst->aluop = Add; // Calculate target address (rs1 + imm)
            inst->memop = 0;
            inst->funct7 = 0;
            inst->rs2 = 0;
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
            
            inst->left = 0;
            inst->right = 0;
            inst->disp_strval = 0;
            inst->funct7 = 0;
            inst->rs2 = 0;
            break;

        default:
            inst->type = UNSUPPORTED_TYPE;
            inst->aluop = Nop;
            inst->memop = 0;
            inst->rd = 0;
            inst->rs1 = 0;
            inst->rs2 = 0;
            inst->funct3 = 0;
            inst->funct7 = 0;
            inst->left = 0;
            inst->right = 0;
            inst->disp_strval = 0;
            break;
    }
}

void print_decoded_instruction(const Instruction *inst) {
    printf("Instruction: 0x%08X\n", inst->inst);
    printf("Opcode: 0x%02X\n", inst->opcode);
    
    const char* type_names[] = {"R_TYPE", "I_TYPE", "S_TYPE", "B_TYPE", "U_TYPE", "J_TYPE", "UNSUPPORTED_TYPE"};
    printf("Type: %s\n", type_names[inst->type]);
    
    printf("rd: %u, rs1: %u, rs2: %u\n", inst->rd, inst->rs1, inst->rs2);
    printf("funct3: 0x%X, funct7: 0x%X\n", inst->funct3, inst->funct7);
    printf("Left: 0x%08X, Right: 0x%08X\n", inst->left, inst->right);
    printf("disp_strval: 0x%08X\n", inst->disp_strval);
    printf("ALU Op: %d, Mem Op: %d\n", inst->aluop, inst->memop);
}