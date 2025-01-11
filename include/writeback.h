#ifndef WRITEBACK_H
#define WRITEBACK_H

#include "memory.h"

void writeback_stage(VirtualMachine *vm, Instruction *inst, int32_t result);

#endif // WRITEBACK_H