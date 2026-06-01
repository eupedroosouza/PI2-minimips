#include "stats.h"
#include <string.h>


Statistics stats;

// Inicializa ou reseta todas as estatísticas para zero
void initStatistics() {
    memset(&stats, 0, sizeof(Statistics));
}

// Computa os dados da instrução atualizada
void computeInstructionStats(const Instruction *instruction) {
    if (instruction == NULL || instruction->type == OTHER) return;

    // 1. Incrementa o total geral
    stats.executedInstructions++;

    // 2. Incrementa por Tipo (R, I, J)
    if (instruction->type == R) {
        stats.executedInstructionsPerType.r++;
    } else if (instruction->type == I) {
        stats.executedInstructionsPerType.i++;
    } else if (instruction->type == J) {
        stats.executedInstructionsPerType.j++;
    } else {
        stats.executedInstructionsPerType.other++;
    }

    // 3. Incrementa por Classe Específica (Opcode/Funct)
    if (instruction->opcode == R_TYPE_OPCODE) {
        switch (instruction->funct) {
            case ADD_FUNCT: stats.executedInstructionsPerClass.add++; break;
            case SUB_FUNCT: stats.executedInstructionsPerClass.sub++; break;
            case AND_FUNCT: stats.executedInstructionsPerClass.and_inst++; break;
            case OR_FUNCT:  stats.executedInstructionsPerClass.or_inst++; break;
            default: break;
        }
    } else {
        switch (instruction->opcode) {
            case LW_OPCODE:   stats.executedInstructionsPerClass.lw++; break;
            case SW_OPCODE:   stats.executedInstructionsPerClass.sw++; break;
            case ADDI_OPCODE: stats.executedInstructionsPerClass.addi++; break;
            case BEQ_OPCODE:  stats.executedInstructionsPerClass.beq++; break;
            case J_OPCODE:    stats.executedInstructionsPerClass.j++; break;
            default:          stats.executedInstructionsPerClass.other++; break;
        }
    }
}