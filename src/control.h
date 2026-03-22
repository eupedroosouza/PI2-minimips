#pragma once
#include "types.h"

Control makeControl(const Instruction *instruction);

void debugControl(const Control *control, const Instruction *instruction, const Register *registers);
