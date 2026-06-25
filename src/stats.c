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





void showStatistics() {
    int cycles_add  = stats.executedInstructionsPerClass.add * 4;
    int cycles_sub  = stats.executedInstructionsPerClass.sub * 4;
    int cycles_and  = stats.executedInstructionsPerClass.and_inst * 4;
    int cycles_or   = stats.executedInstructionsPerClass.or_inst * 4;
    int cycles_addi = stats.executedInstructionsPerClass.addi * 4;
    int cycles_lw   = stats.executedInstructionsPerClass.lw * 5;
    int cycles_sw   = stats.executedInstructionsPerClass.sw * 4;
    int cycles_beq  = stats.executedInstructionsPerClass.beq * 3;
    int cycles_j    = stats.executedInstructionsPerClass.j * 3;

    int totalCycles = cycles_add + cycles_sub + cycles_and + cycles_or + 
                      cycles_addi + cycles_lw + cycles_sw + cycles_beq + cycles_j;
                      
    float cpiGlobal = stats.executedInstructions > 0 ? (float)totalCycles / stats.executedInstructions : 0.0f;

    
    int h_height = 22;
    int h_width = 57;
    
    
    int start_y = (LINES - h_height) / 2;
    int start_x = (COLS - h_width) / 2;

    clear();
    refresh();

   
    WINDOW *win = newwin(h_height, h_width, start_y, start_x);
    if (win == NULL) return;

    
    wborder(win, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, 
                 ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);

   
    mvwprintw(win, 1, 1, "       ");
    wattron(win, A_BOLD); 
    wprintw(win, "ESTATISTICAS DE DESEMPENHO (pipeline)"); 
    wattroff(win, A_BOLD);

   
    mvwhline(win, 2, 0, ACS_HLINE, h_width);
    mvwaddch(win, 2, 0, ACS_LTEE);
    mvwaddch(win, 2, h_width - 1, ACS_RTEE);

   
    mvwprintw(win, 3, 2, "Total de Instrucoes Executadas: %-21d", stats.executedInstructions);
    mvwprintw(win, 4, 2, "Total de Ciclos de Clock:       %-21d", totalCycles);
    mvwprintw(win, 5, 2, "CPI Global do Programa:         %-21.2f", cpiGlobal);

   
    mvwhline(win, 6, 0, ACS_HLINE, h_width);
    mvwaddch(win, 6, 0, ACS_LTEE);
    mvwaddch(win, 6, 12, ACS_TTEE);
    mvwaddch(win, 6, 28, ACS_TTEE);
    mvwaddch(win, 6, 34, ACS_TTEE);
    mvwaddch(win, 6, h_width - 1, ACS_RTEE);

    
    mvwprintw(win, 7, 2, "Instrucao");
    mvwprintw(win, 7, 14, "Qtd Executada");
    mvwprintw(win, 7, 30, "CPI");
    mvwprintw(win, 7, 36, "Ciclos Acumulados");

   
    mvwhline(win, 8, 0, ACS_HLINE, h_width);
    mvwaddch(win, 8, 0, ACS_LTEE);
    mvwaddch(win, 8, 12, ACS_PLUS);
    mvwaddch(win, 8, 28, ACS_PLUS);
    mvwaddch(win, 8, 34, ACS_PLUS);
    mvwaddch(win, 8, h_width - 1, ACS_RTEE);

    
    mvwprintw(win, 9,  2, "ADD");  mvwprintw(win, 9,  14, "%-13d", stats.executedInstructionsPerClass.add);  mvwprintw(win, 9,  30, "4"); mvwprintw(win, 9,  36, "%-17d", cycles_add);
    mvwprintw(win, 10, 2, "SUB");  mvwprintw(win, 10, 14, "%-13d", stats.executedInstructionsPerClass.sub);  mvwprintw(win, 10, 30, "4"); mvwprintw(win, 10, 36, "%-17d", cycles_sub);
    mvwprintw(win, 11, 2, "AND");  mvwprintw(win, 11, 14, "%-13d", stats.executedInstructionsPerClass.and_inst); mvwprintw(win, 11, 30, "4"); mvwprintw(win, 11, 36, "%-17d", cycles_and);
    mvwprintw(win, 12, 2, "OR");   mvwprintw(win, 12, 14, "%-13d", stats.executedInstructionsPerClass.or_inst);  mvwprintw(win, 12, 30, "4"); mvwprintw(win, 12, 36, "%-17d", cycles_or);
    mvwprintw(win, 13, 2, "ADDI"); mvwprintw(win, 13, 14, "%-13d", stats.executedInstructionsPerClass.addi); mvwprintw(win, 13, 30, "4"); mvwprintw(win, 13, 36, "%-17d", cycles_addi);
    mvwprintw(win, 14, 2, "LW");   mvwprintw(win, 14, 14, "%-13d", stats.executedInstructionsPerClass.lw);   mvwprintw(win, 14, 30, "5"); mvwprintw(win, 14, 36, "%-17d", cycles_lw);
    mvwprintw(win, 15, 2, "SW");   mvwprintw(win, 15, 14, "%-13d", stats.executedInstructionsPerClass.sw);   mvwprintw(win, 15, 30, "4"); mvwprintw(win, 15, 36, "%-17d", cycles_sw);
    mvwprintw(win, 16, 2, "BEQ");  mvwprintw(win, 16, 14, "%-13d", stats.executedInstructionsPerClass.beq);  mvwprintw(win, 16, 30, "3"); mvwprintw(win, 16, 36, "%-17d", cycles_beq);
    mvwprintw(win, 17, 2, "J");   mvwprintw(win, 17, 14, "%-13d", stats.executedInstructionsPerClass.j);   mvwprintw(win, 17, 30, "3"); mvwprintw(win, 17, 36, "%-17d", cycles_j);

   
    mvwhline(win, h_height - 1, 0, ACS_HLINE, h_width);
    mvwaddch(win, h_height - 1, 0, ACS_LLCORNER);
    mvwaddch(win, h_height - 1, h_width - 1, ACS_LRCORNER);

   
    mvwvline(win, 7, 12, ACS_VLINE, 14);
    mvwvline(win, 7, 28, ACS_VLINE, 14);
    mvwvline(win, 7, 34, ACS_VLINE, 14);

    
    mvwaddch(win, h_height - 1, 12, ACS_BTEE);
    mvwaddch(win, h_height - 1, 28, ACS_BTEE);
    mvwaddch(win, h_height - 1, 34, ACS_BTEE);

    
    mvprintw(start_y + h_height + 1, (COLS - 42) / 2, "Pressione qualquer tecla para continuar...");

    wrefresh(win);
    wgetch(win);

    delwin(win);
}