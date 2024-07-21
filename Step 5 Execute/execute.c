/* Scott Koskoski
 * 7/18/2024
 * RISC-V ISA Emulator Project
 * Part 5 - Execute
 */

#include <stdint.h>
#include "alu.h"
#include "decode.h"

// Perform ALU operations
int32_t perform_alu_operation(AluOp operation, int32_t left, int32_t right, CompareResult *cmp_result) {
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
			if (cmp_result != NULL) {
				cmp_result->neq_eq = (left == right); // Comparison result for NE/EQ
				cmp_result->ge_lt = (left < right); // Comparison result for GE/LT
				cmp_result->geu_ltu = ((uint32_t)left < (uint32_t)right); // Comparison result for GEU/LTU
			}
			break;
		case Nop:
		default:
			result = 0; // No operation
			break;
	}

	return result;
}


// Execute instruction
void execute_instruction(Instruction *inst, int32_t *result, CompareResult *cmp_result) {
	// Perform ALU operation
	*result = perform_alu_operation(inst->aluop, inst->left, inst->right, cmp_result);

	// ALU calculates memory address plus offset for LOAD and STORE instructions
	if (inst->memop == 1 || inst->memop == 2) {
		*result = inst->left + inst->right;
	}
}

int main(int argc, char *argv[]) {
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <ELF file>\n", argv[0]);
		return -1;
	}

	const char *filename = argv[1];
	VirtualMachine vm;
	initialize_machine(&vm);
	initialize_stack_pointer(&vm);
	ELFHeader elf_header;

	// Load ELF file and get the ELF header
	if (load_elf_file(filename, vm.memory, SIZE_OF_MEMORY, &vm.program_counter, &elf_header) != 0) {
		return -1;
	}

	// Pass pointer to check_elf_file
	if (check_elf_file(&elf_header) != 0) {
		return -1;
	}

	printf("Program counter set to 0x%08X\n", vm.program_counter);

	Instruction inst;
	if (fetch(&vm, &inst) != 0) {
		fprintf(stderr, "Error fetching instruction\n");
		free_machine(vm);
		return -1;
	}

	printf("Fetched instruction: 0x%08X\nb", inst.inst);

	// Decode fetched instruction
	decode_instruction(&vm, &inst);

	// Print decoded instruction
	print_decoded_instruction(&inst);

	// Execute instruction
	int32_t result;
	CompareResult cmp_result;
	execute_instruction(&inst, &result, &cmp_result);

	// Print ALU operation result
	printf("ALU result: 0x08X\n", result);
	printf("Comparison result - NE/EQ: %u, GE/LT: %u, GEU/LTU: %u\n", cmp_result.neq_eq, cmp_result.ge_lt, cmp_result.geu_ltu);

	free_machine(vm);
	return 0;
}


