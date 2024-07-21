/* Scott Koskoski
 * RISC-V ISA Project
 * 7/11/2024
 *
 * ALU for Decode Stage
 */

#ifndef ALU_H
#define ALU_H

// Defining ALU operations
typedef enum {
	Nop,			// No operation
	Add,			// Signed addition
	Sub,			// Signed subtraction
	Mul,			// Signed multiplication
	Div,			// Signed division
	DivU,			// Unsigned division
	Rem,			// Signed remainder
	RemU,			// Unsigned remainder
	LeftShift,		// Logical left shift (<<)
	RightShiftA,	// Arithmetic right shift (>>)
	RightShiftL,	// Logical right shift (>>)
	Or,				// Bitwise inclusive OR (|)
	Xor,			// Bitwise exclusive OR (^)
	And,			// Bitwise AND (&)
	Slt,			// Set less than
	SltU,			// Set less than unsigned
	Cmp				// Compare
} AluOp;

#endif // ALU_H
