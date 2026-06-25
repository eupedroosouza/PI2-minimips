#pragma once
#include "types.h"

// Estrutura para contar quantas instruções de CADA FORMATO (Tipo) rodaram
typedef struct {
    int r;
    int i;
    int j;
    int other;
} StatisticsPerType;

// Estrutura para contar quantas vezes CADA INSTRUÇÃO ESPECÍFICA foi executada
typedef struct {
    int lw;
    int sw;
    int add;
    int addi;
    int sub;
    int and_inst; 
    int or_inst;
    int beq;
    int j;
    int other; 
} StatisticsPerClass;

// Estrutura principal de Estatísticas
typedef struct {
    int executedInstructions;
    StatisticsPerType executedInstructionsPerType;
    StatisticsPerClass executedInstructionsPerClass;
} Statistics;


extern Statistics stats;


void initStatistics();
void computeInstructionStats(const Instruction *instruction);
void showStatistics();