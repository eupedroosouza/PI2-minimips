#include "stats.h"
#include "main.h" 

void updateStatistics(const Word *instruction) {
    if (!instruction) return;

    // Incrementa contador geral de ciclos/instruções
    stats.executedInstructions++;

    // Incrementa os contadores por formato (Tipo) e por classe (Instrução Específica)
    switch (instruction->type) {
        case R:
            stats.executedInstructionsPerType.r++;
            switch (instruction->funct) {
                case ADD_FUNCT: stats.executedInstructionsPerClass.add++; break;
                case SUB_FUNCT: stats.executedInstructionsPerClass.sub++; break;
                case AND_FUNCT: stats.executedInstructionsPerClass.and_inst++; break;
                case OR_FUNCT:  stats.executedInstructionsPerClass.or_inst++; break;
                default: stats.executedInstructionsPerClass.other++; break;
            }
            break;

        case I:
            stats.executedInstructionsPerType.i++;
            switch (instruction->opcode) {
                case ADDI_OPCODE: stats.executedInstructionsPerClass.addi++; break;
                case LW_OPCODE:   stats.executedInstructionsPerClass.lw++; break;
                case SW_OPCODE:   stats.executedInstructionsPerClass.sw++; break;
                case BEQ_OPCODE:  stats.executedInstructionsPerClass.beq++; break;
                default: stats.executedInstructionsPerClass.other++; break;
            }
            break;

        case J:
            stats.executedInstructionsPerType.j++;
            if (instruction->opcode == J_OPCODE) {
                stats.executedInstructionsPerClass.j++;
            } else {
                stats.executedInstructionsPerClass.other++;
            }
            break;

        default:
            stats.executedInstructionsPerType.other++;
            stats.executedInstructionsPerClass.other++;
            break;
    }
}

void resetStatistics() {
    stats.executedInstructions = 0;
    
    // Zera a struct de Tipos
    stats.executedInstructionsPerType.i = 0;
    stats.executedInstructionsPerType.j = 0;
    stats.executedInstructionsPerType.r = 0;
    stats.executedInstructionsPerType.other = 0;

    // Zera a struct de Classes
    stats.executedInstructionsPerClass.lw = 0;
    stats.executedInstructionsPerClass.sw = 0;
    stats.executedInstructionsPerClass.add = 0;
    stats.executedInstructionsPerClass.addi = 0;
    stats.executedInstructionsPerClass.sub = 0;
    stats.executedInstructionsPerClass.and_inst = 0;
    stats.executedInstructionsPerClass.or_inst = 0;
    stats.executedInstructionsPerClass.beq = 0;
    stats.executedInstructionsPerClass.j = 0;
    stats.executedInstructionsPerClass.other = 0;
}
