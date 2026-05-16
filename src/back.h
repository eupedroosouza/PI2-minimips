#pragma once
#include "types.h"


extern BackState *lastState;

void saveState();

void invalidateState();

void back();
