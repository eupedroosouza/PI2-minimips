#include "stats.h"
#include <string.h>
#include <curses.h>

Statistics stats;

// Inicializa ou reseta todas as estatísticas para zero
void initStatistics() {
    memset(&stats, 0, sizeof(Statistics));
}

// Computa os dados da instrução atualizada
void computeInstructionStats(const Instruction *instruction) {
    if (instruction == NULL) return;
    
    // Ignora NOPs / Bolhas injetadas pelo pipeline para não sujar a contagem de instruções reais
    if (strcmp(instruction->stringedInstruction, "0000000000000000") == 0) return;
    
    if (instruction->type == OTHER) return;

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





void showStatistics() {

    WINDOW *winCentral = newwin(49, 207, (LINES - 49) / 2, (COLS - 207) / 2); // centralized window
    box(winCentral, 0, 0); // border
    wrefresh (winCentral);
    
    int height = 25;
    int width = 62;
    int start_y = (LINES - height) / 2;
    int start_x = (COLS - width) / 2;
    
    WINDOW *win = newwin(height, width, start_y, start_x);
    box(win, 0, 0);
    keypad(win, TRUE);

   
    wattron(win, A_BOLD);
    mvwprintw(win, 1, 2, "=== ESTATISTICAS DE DESEMPENHO (PIPELINE) ===");
    wattroff(win, A_BOLD);

   
    float cpi = 0.0;
    if (stats.finishedInstructions > 0) {
        cpi = (float)stats.totalCycles / stats.finishedInstructions;
    }

   
    mvwprintw(win, 3, 2, "Total de Clocks (Ciclos):    %d", stats.totalCycles);
    mvwprintw(win, 4, 2, "Bolhas (Stalls/Inuteis):     %d", stats.totalBubbles);
    mvwprintw(win, 5, 2, "Instrucoes Iniciadas (IF):   %d", stats.startedInstructions);
    mvwprintw(win, 6, 2, "Instrucoes Finalizadas (WB): %d", stats.finishedInstructions);
    
    wattron(win, A_BOLD);
    mvwprintw(win, 7, 2, "CPI Global do Sistema:       %.2f", cpi);
    wattroff(win, A_BOLD);

    
    mvwhline(win, 9, 1, ACS_HLINE, width - 2);

   
    wattron(win, A_BOLD);
    mvwprintw(win, 10, 2, "Inst.");
    mvwprintw(win, 10, 14, "Qtd. Exec.");
    mvwprintw(win, 10, 30, "Est. Uteis"); 
    mvwprintw(win, 10, 42, "Total Ciclos");
    wattroff(win, A_BOLD);

    mvwhline(win, 11, 1, ACS_HLINE, width - 2);

    
    int cycles_add  = stats.executedInstructionsPerClass.add * 4;
    int cycles_sub  = stats.executedInstructionsPerClass.sub * 4;
    int cycles_and  = stats.executedInstructionsPerClass.and_inst * 4;
    int cycles_or   = stats.executedInstructionsPerClass.or_inst * 4;
    int cycles_addi = stats.executedInstructionsPerClass.addi * 4;
    int cycles_lw   = stats.executedInstructionsPerClass.lw * 5;
    int cycles_sw   = stats.executedInstructionsPerClass.sw * 4;
    int cycles_beq  = stats.executedInstructionsPerClass.beq * 3;
    int cycles_j    = stats.executedInstructionsPerClass.j * 3;

   
    mvwprintw(win, 12, 2, "ADD");  mvwprintw(win, 12, 14, "%-13d", stats.executedInstructionsPerClass.add);      mvwprintw(win, 12, 30, "4"); mvwprintw(win, 12, 42, "%-17d", cycles_add);
    mvwprintw(win, 13, 2, "SUB");  mvwprintw(win, 13, 14, "%-13d", stats.executedInstructionsPerClass.sub);      mvwprintw(win, 13, 30, "4"); mvwprintw(win, 13, 42, "%-17d", cycles_sub);
    mvwprintw(win, 14, 2, "AND");  mvwprintw(win, 14, 14, "%-13d", stats.executedInstructionsPerClass.and_inst); mvwprintw(win, 14, 30, "4"); mvwprintw(win, 14, 42, "%-17d", cycles_and);
    mvwprintw(win, 15, 2, "OR");   mvwprintw(win, 15, 14, "%-13d", stats.executedInstructionsPerClass.or_inst);  mvwprintw(win, 15, 30, "4"); mvwprintw(win, 15, 42, "%-17d", cycles_or);
    mvwprintw(win, 16, 2, "ADDI"); mvwprintw(win, 16, 14, "%-13d", stats.executedInstructionsPerClass.addi);     mvwprintw(win, 16, 30, "4"); mvwprintw(win, 16, 42, "%-17d", cycles_addi);
    mvwprintw(win, 17, 2, "LW");   mvwprintw(win, 17, 14, "%-13d", stats.executedInstructionsPerClass.lw);       mvwprintw(win, 17, 30, "5"); mvwprintw(win, 17, 42, "%-17d", cycles_lw);
    mvwprintw(win, 18, 2, "SW");   mvwprintw(win, 18, 14, "%-13d", stats.executedInstructionsPerClass.sw);       mvwprintw(win, 18, 30, "4"); mvwprintw(win, 18, 42, "%-17d", cycles_sw);
    mvwprintw(win, 19, 2, "BEQ");  mvwprintw(win, 19, 14, "%-13d", stats.executedInstructionsPerClass.beq);      mvwprintw(win, 19, 30, "3"); mvwprintw(win, 19, 42, "%-17d", cycles_beq);
    mvwprintw(win, 20, 2, "J");    mvwprintw(win, 20, 14, "%-13d", stats.executedInstructionsPerClass.j);        mvwprintw(win, 20, 30, "3"); mvwprintw(win, 20, 42, "%-17d", cycles_j);

    
    mvwhline(win, 22, 1, ACS_HLINE, width - 2);
    wattron(win, A_DIM);
    mvwprintw(win, 23, 2, "Pressione qualquer tecla para voltar...");
    wattroff(win, A_DIM);

  
    wrefresh(win);
    wgetch(win);
    delwin(win);
}
