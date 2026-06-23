#pragma once

#include "types.h"

// run combinational circuits from current state of machine
void createCombinational(CombinationalState *C);

// run sequential circuits
void clock();