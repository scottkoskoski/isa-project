/* Scott Koskoski
 * 7/10/2024
 * RISC-V ISA Emulator Project
 * Part 4 - Decode
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "alu.h"
#include "fetch.h"

// Extract the opcode from an instruction
uint8_t get_opcode(uint32_t instruction) {
	return instruction & 0x7f; // Last 7 bits
}

// Determine ALU operation for R-TYPE instructions
AluOp get_r_type_alu_op(uint8_t funct3, uint8_t funct7) {
	switch (funct3) {
		case 0x0: return funct7 == 0x00 ? Add : funct7 == 0x20 ? Sub : funct7 == 0x01 ? Mul : Nop; // ADD, SUB, MUL
		case 0x1: return LeftShift; // SLL
		case 0x2: return Slt; // SLT
		case 0x3: return SltU; // SLTU
		case 0x4: return Xor; // XOR
		case 0x5: return funct7 == 0x00 ? RightShiftL : RightShiftA; // SRL or SRA
		case 0x6: return Or; // OR
		case 0x7: return And; // AND
		default: return Nop; // Default is no operation
	}
}

// Determine ALU operation for I-TYPE instructions
AluOp get_i_type_alu_op(uint8_t funct3) {
	switch (funct3) {
		case 0x0: return Add; // ADDI
		case 0x2: return Slt; // SLTI
		case 0x3: return SltU; // SLTIU
		case 0x4: return Xor; // XORI
		case 0x6: return Or; // ORI
		case 0x7: return And; // ANDI
		case 0x1: return LeftShift; // SLLI
		case 0x5: return RightShiftL; // SRLI or SRAI
		default: return Nop; // Default is no operation
	}
}


// Decoding
void decode_instruction(VirtualMachine *vm, Instruction *inst) {
	// Grab op code from instruction
	uint8_t opcode = get_opcode(inst->inst);

	// Cases for each instruction type
	switch (opcode) {
		case 0x33: // R-TYPE
			inst->type = R_TYPE;
			inst->rd = (inst->inst >> 7) & 0x1F; // bits [11:7] for rd
			inst->funct3 = (inst->inst >> 12) & 0x07; // bits [14:12] for funct3
			inst->rs1 = (inst->inst >> 15) & 0x1F; // bits [19:15] for rs1
			inst->rs2 = (inst->inst >> 20) & 0x1F; // bits [24:20] for rs2
			inst->funct7 = (inst->inst >> 25) & 0x7F; // bits [31:25] for funct7
			inst->left = read_from_register(vm, inst->rs1); // Read rs1 operand
			inst->right = read_from_register(vm, inst->rs2); // Read rs2 operand
			inst->aluop = get_r_type_alu_op(inst->funct3, inst->funct7); // Determine ALU operation
			break;
		case 0x13: // I-TYPE
		case 0x03: // Load I-TYPE
		case 0x67: // JALR I-TYPE
		case 0x73: // ECALL I-TYPE
			inst->type = I_TYPE;
			inst->rd = (inst->inst >> 7) & 0x1F; // bits [11:7] for rd
			inst->funct3 = (inst->inst >> 12) & 0x07; // bits [14:12] for funct3
			inst->rs1 = (inst->inst >> 15) & 0x1F; // bits [19:15] for rs1
			inst->right = extend_sign_bit(inst->inst >> 20, 11); // bits [31:20] and sign-extend for immediate value
			inst->left = read_from_register(vm, inst->rs1); // Read rs1 operand
			inst->aluop = get_i_type_alu_op(inst->funct3); // Determine ALU operation
			if (inst->funct3 == 0x5 && ((inst->inst >> 25) & 0x7F) == 0x20) {
				inst->aluop = RightShiftA; // SRAI
			}
			break;
		case 0x23: // S-TYPE
			inst->type = S_TYPE;
			inst->funct3 = (inst->inst >> 12) & 0x07; // bits [14:12] for funct3
			inst->rs1 = (inst->inst >> 15) & 0x1F; // bits [19:15] for rs1
			inst->rs2 = (inst->inst >> 20) & 0x1F; // bits [24:20] for rs2
			inst->right = ((inst->inst >> 7) & 0x1F) | // bits [11:7] for imm[4:0]
						  ((inst->inst >> 25) & 0xFE0); // bits [31:25] for imm[11:5]
			inst->left = read_from_register(vm, inst->rs2); // Read rs2 operand (value to store)
			inst->aluop = Add; // ALU operation is Add for address calculation
			break;
		case 0x63: // B-TYPE
			inst->type = B_TYPE;
			inst->funct3 = (inst->inst >> 12) & 0x07; // bits [14:12] for funct3
			inst->rs1 = (inst->inst >> 15) & 0x1F; // bits [19:15] for rs1
			inst->rs2 = (inst->inst >> 20) & 0x1F; // bits [24:20] for rs2
			inst->right = ((inst->inst >> 8) & 0x0F) | // bits [[11:8] for imm[4:1]
						  ((inst->inst >> 25) & 0x3F) | // bits [30:25] for imm[10:5]
						  ((inst->inst << 4) & 0x800) | // shift bit 7 left to imm[11]
						  ((inst->inst >> 19) & 0x1000); // bit 31 for imm[12]
			inst->left = read_from_register(vm, inst->rs1); // Read rs1 operand
			inst->right = read_from_register(vm, inst->rs2);
			inst->aluop = Sub; // ALU operation is Sub for branch comparions
			break;
		case 0x37: // LUI U-TYPE
		case 0x17: // AUIPC U-TYPE
			inst->type = U_TYPE;
			inst->rd = (inst->inst >> 7) & 0x1F; // bits [11:7] for rd
			inst->right = inst->inst & 0xFFFFF000; // bits [31:12] for immediate value
			inst->aluop = Add; // ALU operation is Add for LUI and AUIPC
			break;
		case 0x6F: // J-TYPE
			inst->type = J_TYPE;
			inst->rd = (inst->inst >> 7) & 0x1F; // bits [11:7] for rd
			inst->right = ((inst->inst >> 12) & 0xFF) | // bits [19:12] for imm[19:12]
						  ((inst->inst >> 20) & 0x1) | // bit 20 for imm[20]
						  ((inst->inst >> 21) & 0x3FF) | // bits [30:21] for imm[10:1]
						  ((inst->inst << 1) & 0x800); // bit 20 for imm[11] and shift
			inst->aluop = Add; // ALU operation is Add for JAL
			break;
		default:
			inst->type = UNSUPPORTED_TYPE;
			inst->aluop = Nop; // No operation for unsupported type
			break;
	}
}


// Comparison result structure
typedef struct {
	uint8_t neq_eq;		// 0 = left != right, 1 = left == right
	uint8_t ge_lt;		// 0 = left >= right, 1 = left < right
	uint8_t geu_ltu;	// 0 = leftU >= rightU, 1 = leftU < rightU
} CompareResult;

// Print decoded instruction
void print_decoded_instruction(const Instruction *inst) {
	printf("Instruction: 0x%08X\n", inst->inst);
	printf("Type: %d, rd: %u, rs1: %u, rs2: %u, funct3: %u, funct7: %u\n",
			inst->type, inst->rd, inst->rs1, inst->rs2, inst->funct3, inst->funct7);
	printf("Left operand: 0x08X, right operand: 0x%08X, immediate: 0x%08X\n",
			inst->left, inst->right, inst->disp_strval);
}

int main(int argc, char *argv[]) {
	if (argc!= 2) {
		fprintf(stderr, "Usage: %s <ELF file>\n", argv[0]);
		return -1;
	}

	const char *filename = argv[1];
	VirtualMachine vm;
	initialize_machine(&vm);
	initialize_stack_pointer(&vm);
	ELFHeader elf_header;

	// Load ELF file and get ELF header
	if (load_elf_file(filename, vm.memory, SIZE_OF_MEMORY, &vm.program_counter, &elf_header) != 0) {
		return -1;
	}

	printf("Program counter set to 0x%08X\n", vm.program_counter);

	Instruction inst;
	if (fetch(&vm, &inst) != 0) {
		fprintf(stderr, "Error fetching instruction\n");
		free_machine(vm);
		return -1;
	}

	// Decode fetched instruction
	decode_instruction(&vm, &inst);

	// Print decoded instruction
	print_decoded_instruction(&inst);

	free_machine(vm);				
	return 0;
}

		
