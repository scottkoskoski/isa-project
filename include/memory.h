#ifndef MEMORY_H
#define MEMORY_H

#include "fetch.h"

void memory_stage(VirtualMachine *vm, Instruction *inst, int32_t *result);

#endif // MEMORY_H