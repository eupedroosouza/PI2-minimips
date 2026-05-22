#pragma once
#include "types.h"

// Atualiza as estatísticas com base na instrução recém-executada
void updateStatistics(const Word *instruction);

// Reseta todos os contadores de estatísticas para 0
void resetStatistics();