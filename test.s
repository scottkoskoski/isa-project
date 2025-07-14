.global _start

.text
_start:
    # Simple arithmetic - load immediate values
    addi x1, x0, 5       # x1 = 5
    addi x2, x0, 3       # x2 = 3
    
    # Add them together
    add x3, x1, x2       # x3 = x1 + x2 = 8
    
    # Subtract
    sub x4, x1, x2       # x4 = x1 - x2 = 2
    
    # Test a branch (if x3 > x2, add 1 to x4)
    blt x2, x3, add_one  # if x2 < x3, goto add_one
    j exit               # otherwise jump to exit
    
add_one:
    addi x4, x4, 1       # x4 = x4 + 1 = 3
    
exit:
    # Exit with code in x4 (should be 3)
    addi x17, x0, 93     # sys_exit system call number
    add x10, x4, x0      # exit code = x4
    ecall                # system call