#include "interface.h"

#include <curses.h>
#include <stdlib.h>
#include <string.h>

#include "back.h"
#include "clock.h"
#include "control.h"
#include "data.h"
#include "instruction.h"
#include "main.h"
#include "reset.h"
#include "utils.h"
#include "view.h"
#include "ula.h"

#define WIDTH 207
#define HEIGHT 49
#define MIN(a,b) ((a) < (b) ? (a) : (b))


WINDOW *createWindow() {
    WINDOW *win = newwin(HEIGHT, WIDTH, (LINES - HEIGHT) / 2, (COLS - WIDTH) / 2); // centralized window
    box(win, 0, 0); // border
    return win;
}

void printOption(WINDOW *win, const int option, const int offset, const int select, const char *msg) {
    const bool selected = select == option;
    wattron(win, COLOR_PAIR(1));
    mvwprintw(win, option, 1, " %02d ", (option + offset));
    wattroff(win, COLOR_PAIR(1));
    if (selected) {
        wattron(win, COLOR_PAIR(1));
    }
    mvwprintw(win, option, 6, " %s ", msg);
    if (selected) {
        wattroff(win, COLOR_PAIR(1));
    }
}

WINDOW *textInputUI(const char *msg, char *buffer) {
    WINDOW *win = createWindow();

    wattron(win, A_BOLD);
    mvwprintw(win, 2, 2, msg);
    wattroff(win, A_BOLD);

    mvwprintw(win, 3, 2, "> ");
    refresh();
    wrefresh(win);

    int idx = 0;
    wmove(win, 3, 4);
    while (idx < 127) {
        const int ch = getch();

        if (ch == 27) {
            // ESC
            buffer[0] = '\0';
            break;
        }
        if (ch == '\n' || ch == '\r') {
            buffer[idx] = '\0';
            break;
        }
        if (ch == KEY_BACKSPACE || ch == 127 || ch == '\b') {
            if (idx > 0) {
                idx--;
                mvwaddch(win, 3, 4 + idx, ' ');
                wmove(win, 3, 4 + idx);
            }
        } else if (ch >= 32 && ch <= 126) {
            buffer[idx] = ch;
            mvwaddch(win, 3, 4 + idx, ch);
            idx++;
        }

        refresh();
        wrefresh(win);
    }

    if (strlen(buffer) == 0) {
        delwin(win);
        return NULL;
    }

    return win;
}

void finalTextInputUI(WINDOW *win, const char *msg) {
    mvwprintw(win, 4, 2, "> %s", msg);
    mvwprintw(win, 5, 2, "Pressione qualquer tecla para continuar.");
    refresh();
    wrefresh(win);

    getch();

    delwin(win);
}

void confirmationUI(const char *msg) {
    WINDOW *win = createWindow();
    mvwprintw(win, 2, 2, "> %s", msg);
    mvwprintw(win, 3, 2, "Pressione qualquer tecla para continuar.");
    refresh();
    wrefresh(win);

    getch();

    delwin(win);
}


void menu2() {
    while (1) {
        WINDOW *win = createWindow(); // centralized window

        // title of simulator
        wattron(win, A_BOLD);
        mvwprintw(win, 2, (WIDTH - 32) / 2, "Simulador Mini-MIPS 8-bits Pipeline");
        wattroff(win, A_BOLD);
        mvwprintw(win, 3, (WIDTH - 56) / 2, "Pedro S. Moreira, Willian A. Correa, William N. A. Paiva");

        // create selection box
        WINDOW *selWin = derwin(win, 36, 52, 6, (WIDTH - 46) / 2);
        box(selWin, 0, 0);

        // help usage
        mvwprintw(win, 43, 96, "Use ");
        wattron(win, COLOR_PAIR(1));
        mvwprintw(win, 43, 100, " ↓ ");
        wattroff(win, COLOR_PAIR(1));
        wattron(win, COLOR_PAIR(1));
        mvwprintw(win, 43, 104, " ↑ ");
        wattroff(win, COLOR_PAIR(1));
        mvwprintw(win, 43, 107, " e ");
        wattron(win, COLOR_PAIR(1));
        mvwprintw(win, 43, 110, " ↵ ");
        wattroff(win, COLOR_PAIR(1));

        refresh();
        wrefresh(win);
        wrefresh(selWin);

        int select = 1;
        while (1) {
            printOption(selWin, 1, 0, select, "Carregar memória de instruções (.mem)");
            printOption(selWin, 2, 0, select, "Carregar memória de dados (.dat)");
            printOption(selWin, 3, 0, select, "Exibir o simulador");
            printOption(selWin, 4, 0, select, "Salvar instruçôes em Assembly (.asm)");
            printOption(selWin, 5, 0, select, "Salvar memória de dados (.dat)");
            printOption(selWin, 6, 0, select, "Modo de execução");
            printOption(selWin, 7, 0, select, "Estatísticas");
            printOption(selWin, 8, 0, select, "Redefinir memória de instruções");
            printOption(selWin, 9, 0, select, "Redefinir registradores");
            printOption(selWin, 10, 0, select, "Redefinir memória de dados");
            printOption(selWin, 11, -11, select, "Encerrar o simulador");
            refresh();
            wrefresh(win);
            wrefresh(selWin);
            const int ch = getch();
            // out of switch to pass break to for (no switch)
            if (ch == '\n' || ch == '\r') {
                // enter
                break;
            }
            switch (ch) {
                case KEY_UP: {
                    if (select > 0) {
                        select--;
                    }
                    break;
                }
                case KEY_DOWN: {
                    if (select < 11) {
                        select++;
                    }
                    break;
                }
                default: break;
            }
        }

        delwin(selWin);
        delwin(win);

        switch (select) {
            case 1: {
                loadInstructionsUI();
                break;
            }
            case 2: {
                loadDataUI();
                break;
            }
            case 3: {
                allProgramUI();
                break;
            }
            case 4: {
                saveInstructionOnAssemblyUI();
                break;
            }
            case 5: {
                saveMemDataUI();
                break;
            }
            case 6: {
                execution();
                break;
            }
            case 7: {
                showStatistics();
                break;
            }
            case 8: {
                resetInstructions();
                confirmationUI("A memória de instruções foi redefinida.");
                break;
            }
            case 9: {
                resetRegisters(&pipeline);
                confirmationUI("Os registradores foram redefinidos.");
                break;
            }
            case 10: {
                resetData();
                confirmationUI("A memória de dados foi redefinida.");
                break;
            }
            case 11: {
                exit(0);
            }
            default: break;
        }
    }
}

void loadInstructionsUI() {
    char fileName[128];
    WINDOW *win = textInputUI("Digite o caminho do arquivo que contém a memória de instrução .mem:", fileName);
    if (win == NULL) {
        return;
    }

    char msg[256];
    FILE *arquivo = fopen(fileName, "r"); // Abre arquivo .mem para leitura
    if (arquivo == NULL) {
        sprintf(msg, "Erro ao ler arquivo!");
    } else {
        char string[17]; // Conjunto de 16 bits
        char linha[100];

        int i = 0;
        while (fgets(linha, sizeof (linha), arquivo) != NULL) {
            sscanf(linha, "%16[^\n]\n", string);
            decodeInstruction(&memInstruction.instructions[i], string);
            i++;
        }
        memInstruction.size = i;

        fclose(arquivo);

        sprintf(msg, "Foram carregadas %d instruções na memória de instruções.", memInstruction.size);
        invalidateLastState();
    }

    finalTextInputUI(win, msg);
}

void loadDataUI() {
    char fileName[128];
    WINDOW *win = textInputUI("Digite o caminho do arquivo .dat: ", fileName);
    if (win == NULL) {
        return;
    }

    char msg[256];
    FILE *arquivo = fopen(fileName, "r");
    if (arquivo == NULL) {
        sprintf(msg, "Erro ao ler arquivo!");
    } else {
        resetData();
        char linha[100];
        int i = 0;

        while (fgets(linha, sizeof(linha), arquivo) != NULL && i < 256) {
            linha[strcspn(linha, "\r\n")] = 0;

            if (strlen(linha) > 0) {
                memData.data[i] = (int8_t) strtol(linha, NULL, 10);
                i++;
            }
        }

        memData.size = i;
        fclose(arquivo);
        sprintf(msg, "A memória de dados foi carregada.");
    }

    finalTextInputUI(win, msg);
}

void saveInstructionOnAssemblyUI() {
    char fileName[128];
    WINDOW *win = textInputUI("Digite o diretório para salvar o arquivo .asm:", fileName);
    if (win == NULL) {
        return;
    }

    char msg[256];
    FILE *arquivoDestino = fopen(fileName, "w");
    if (arquivoDestino == NULL) {
        printf("\nErro ao abrir arquivo!");
    } else {
        for (int i = 0; i < 256; i++) {
            fprintf(arquivoDestino, "%s\n", memInstruction.instructions[i].asmInstruction);
        }
        fclose(arquivoDestino);
        sprintf(msg, "As instruções em Assembly foram salvas em: %s.", fileName);
    }

    finalTextInputUI(win, msg);
}


void saveMemDataUI() {
    char fileName[128];
    WINDOW *win = textInputUI("Digite o caminho para salvar o arquivo .dat: ", fileName);
    if (win == NULL) {
        return;
    }

    char msg[256];
    FILE *arquivo = fopen(fileName, "w");
    if (arquivo == NULL) {
        sprintf(msg, "\nErro ao abrir arquivo!");
    } else {
        for (int i = 0; i < MEM_SIZE; i++) {
            fprintf(arquivo, "%d\n", memData.data[i]);
        }

        fclose(arquivo);
        sprintf(msg, "A memória de dados foi salva em: %s.", fileName);
    }

    finalTextInputUI(win, msg);
}

void instructionUI(WINDOW *win, const int base, const int idx, const Instruction *instruction) {
    instructionHeaderUI(win, base);
    instructionDataUI(win, base + 3, idx, instruction);
    instructionFooterUI(win, base + 4);
}

void instructionHeaderUI(WINDOW *win, const int base) {
    mvwaddch(win, base, 0, ACS_LTEE);
    mvwhline(win, base, 1, ACS_HLINE, 108);
    mvwaddch(win, base, 6, ACS_TTEE);
    mvwaddch(win, base, 25, ACS_TTEE);
    mvwaddch(win, base, 32, ACS_TTEE);
    mvwaddch(win, base, 58, ACS_TTEE);
    mvwaddch(win, base, 67, ACS_TTEE);
    mvwaddch(win, base, 72, ACS_TTEE);
    mvwaddch(win, base, 78, ACS_TTEE);
    mvwaddch(win, base, 84, ACS_TTEE);
    mvwaddch(win, base, 90, ACS_TTEE);
    mvwaddch(win, base, 96, ACS_TTEE);
    mvwaddch(win, base, 103, ACS_TTEE);
    mvwaddch(win, base, 109, ACS_RTEE);
    const int l1 = base + 1;
    mvwaddch(win, l1, 0, ACS_VLINE);
    mvwprintw(win, l1, 3, "#");
    mvwprintw(win, l1, 13, "Binário");
    mvwprintw(win, l1, 27, "Hexa");
    mvwprintw(win, l1, 41, "Assembly");
    mvwprintw(win, l1, 61, "Tipo");
    mvwprintw(win, l1, 69, "OP");
    mvwprintw(win, l1, 75, "RS");
    mvwprintw(win, l1, 81, "RT");
    mvwprintw(win, l1, 87, "RD");
    mvwprintw(win, l1, 91, "Funct");
    mvwprintw(win, l1, 99, "Imm");
    mvwprintw(win, l1, 104, "Addr");
    mvwaddch(win, l1, 6, ACS_VLINE);
    mvwaddch(win, l1, 25, ACS_VLINE);
    mvwaddch(win, l1, 32, ACS_VLINE);
    mvwaddch(win, l1, 58, ACS_VLINE);
    mvwaddch(win, l1, 67, ACS_VLINE);
    mvwaddch(win, l1, 72, ACS_VLINE);
    mvwaddch(win, l1, 78, ACS_VLINE);
    mvwaddch(win, l1, 84, ACS_VLINE);
    mvwaddch(win, l1, 90, ACS_VLINE);
    mvwaddch(win, l1, 96, ACS_VLINE);
    mvwaddch(win, l1, 103, ACS_VLINE);
    mvwaddch(win, l1, 109, ACS_VLINE);
    const int l2 = l1 + 1;
    mvwaddch(win, l2, 0, ACS_LTEE);
    mvwhline(win, l2, 1, ACS_HLINE, 108);
    mvwaddch(win, l2, 6, ACS_PLUS);
    mvwaddch(win, l2, 25, ACS_PLUS);
    mvwaddch(win, l2, 32, ACS_PLUS);
    mvwaddch(win, l2, 58, ACS_PLUS);
    mvwaddch(win, l2, 67, ACS_PLUS);
    mvwaddch(win, l2, 72, ACS_PLUS);
    mvwaddch(win, l2, 78, ACS_PLUS);
    mvwaddch(win, l2, 84, ACS_PLUS);
    mvwaddch(win, l2, 90, ACS_PLUS);
    mvwaddch(win, l2, 96, ACS_PLUS);
    mvwaddch(win, l2, 103, ACS_PLUS);
    mvwaddch(win, l2, 109, ACS_RTEE);
}

void instructionDataUI(WINDOW *win, const int base, const int idx, const Instruction *instruction) {
    char strIdx[4];
    if (idx == -1) {
        strcpy(strIdx, " - ");
    } else {
        snprintf(strIdx, sizeof(strIdx), "%03d", idx);
    }
    mvwprintw(win, base, 2, "%s", strIdx);
    mvwprintw(win, base, 8, "%s", instruction->stringedInstruction);
    mvwprintw(win, base, 27, "%s", instruction->hexa);
    mvwprintw(win, base, 34, "%s", instruction->asmInstruction);
    mvwprintw(win, base, 59 + (8 - strlen(typeStr[instruction->type])) / 2, "%s", typeStr[instruction->type]);
    char opcode[128];
    char rs[128];
    char rt[128];
    char rd[128];
    char funct[128];
    char imm[128];
    char addr[128];
    if (instruction->type == OTHER) {
        strcpy(opcode, " -");
        strcpy(rs, "-");
        strcpy(rt, "-");
        strcpy(rd, "-");
        strcpy(funct, "-");
        strcpy(imm, "  - ");
        strcpy(addr, " - ");
    } else {
        sprintf(opcode, "%02d", instruction->opcode);
        if (instruction->type == J) {
            strcpy(rs, "-");
            strcpy(rt, "-");
            sprintf(addr, "%03d", instruction->addr);
        } else {
            sprintf(rs, "%1d", instruction->rs);
            sprintf(rt, "%1d", instruction->rt);
            strcpy(addr, " - ");
        }
        if (instruction->type == R) {
            sprintf(rd, "%1d", instruction->rd);
            sprintf(funct, "%1d", instruction->funct);
        } else {
            strcpy(funct, "-");
            strcpy(rd, "-");
        }
        if (instruction->type == I) {
            sprintf(imm, "%04d", instruction->imm);
        } else {
            strcpy(imm, "  - ");
        }
    }
    mvwprintw(win, base, 69, "%s", opcode);
    mvwprintw(win, base, 75, "%s", rs);
    mvwprintw(win, base, 81, "%s", rt);
    mvwprintw(win, base, 87, "%s", rd);
    mvwprintw(win, base, 93, "%s", funct);
    mvwprintw(win, base, 98, "%s", imm);
    mvwprintw(win, base, 105, "%s", addr);
    mvwaddch(win, base, 0, ACS_VLINE);
    mvwaddch(win, base, 6, ACS_VLINE);
    mvwaddch(win, base, 25, ACS_VLINE);
    mvwaddch(win, base, 32, ACS_VLINE);
    mvwaddch(win, base, 58, ACS_VLINE);
    mvwaddch(win, base, 67, ACS_VLINE);
    mvwaddch(win, base, 72, ACS_VLINE);
    mvwaddch(win, base, 78, ACS_VLINE);
    mvwaddch(win, base, 84, ACS_VLINE);
    mvwaddch(win, base, 90, ACS_VLINE);
    mvwaddch(win, base, 96, ACS_VLINE);
    mvwaddch(win, base, 103, ACS_VLINE);
    mvwaddch(win, base, 109, ACS_VLINE);
}

void instructionFooterUI(WINDOW *win, const int base) {
    mvwaddch(win, base, 0, ACS_LTEE);
    mvwhline(win, base, 1, ACS_HLINE, 108);
    mvwaddch(win, base, 6, ACS_BTEE);
    mvwaddch(win, base, 25, ACS_BTEE);
    mvwaddch(win, base, 32, ACS_BTEE);
    mvwaddch(win, base, 58, ACS_BTEE);
    mvwaddch(win, base, 67, ACS_BTEE);
    mvwaddch(win, base, 72, ACS_BTEE);
    mvwaddch(win, base, 78, ACS_BTEE);
    mvwaddch(win, base, 84, ACS_BTEE);
    mvwaddch(win, base, 90, ACS_BTEE);
    mvwaddch(win, base, 96, ACS_BTEE);
    mvwaddch(win, base, 103, ACS_BTEE);
    mvwaddch(win, base, 109, ACS_RTEE);
}

void controlUI(WINDOW *win, const int base, const Instruction *instruction, const Control *control) {
    mvwaddch(win, base, 0, ACS_LTEE);
    mvwhline(win, base, 1, ACS_HLINE, 108);
    mvwaddch(win, base, 7, ACS_TTEE);
    mvwaddch(win, base, 16, ACS_TTEE);
    mvwaddch(win, base, 26, ACS_TTEE);
    mvwaddch(win, base, 40, ACS_TTEE);
    mvwaddch(win, base, 77, ACS_TTEE);
    mvwaddch(win, base, 88, ACS_TTEE);
    mvwaddch(win, base, 98, ACS_TTEE);
    mvwaddch(win, base, 109, ACS_RTEE);
    const int l1 = base + 1;
    mvwaddch(win, l1, 0, ACS_VLINE);
    mvwprintw(win, l1, 2, "Jump");
    mvwaddch(win, l1, 7, ACS_VLINE);
    mvwprintw(win, l1, 9, "Branch");
    mvwaddch(win, l1, 16, ACS_VLINE);
    mvwprintw(win, l1, 18, "Reg Dst");
    mvwaddch(win, l1, 26, ACS_VLINE);
    mvwprintw(win, l1, 28, "Mem para Reg");
    mvwaddch(win, l1, 40, ACS_VLINE);
    mvwprintw(win, l1, 55, "Ula Fonte");
    mvwaddch(win, l1, 77, ACS_VLINE);
    mvwprintw(win, l1, 79, "Ula Ctrl");
    mvwaddch(win, l1, 88, ACS_VLINE);
    mvwprintw(win, l1, 90, "Esc Reg");
    mvwaddch(win, l1, 98, ACS_VLINE);
    mvwprintw(win, l1, 101, "Esc Mem");
    mvwaddch(win, l1, 109, ACS_VLINE);
    const int l2 = l1 + 1;
    mvwaddch(win, l2, 0, ACS_LTEE);
    mvwhline(win, l2, 1, ACS_HLINE, 108);
    mvwaddch(win, l2, 7, ACS_PLUS);
    mvwaddch(win, l2, 16, ACS_PLUS);
    mvwaddch(win, l2, 26, ACS_PLUS);
    mvwaddch(win, l2, 40, ACS_PLUS);
    mvwaddch(win, l2, 77, ACS_PLUS);
    mvwaddch(win, l2, 88, ACS_PLUS);
    mvwaddch(win, l2, 98, ACS_PLUS);
    mvwaddch(win, l2, 109, ACS_RTEE);
    const int l3 = l2 + 1;
    mvwprintw(win, l3, 3, "%s", boolStr[control->jump]);
    mvwprintw(win, l3, 11, "%s", boolStr[control->branch]);
    char regDstBuffer[256];
    snprintf(regDstBuffer, sizeof(regDstBuffer), "%1d ($%1d)", control->regDst,
             control->regDst ? instruction->rd : instruction->rt);
    mvwprintw(win, l3, 19, "%s", regDstBuffer);
    char memToRegBuffer[14];

    const char *memToRegStr[] = {"ULA", "MEM"};
    const char *ulaSourceStr[] = {"Registrador", "Imediato"};

    snprintf(memToRegBuffer, sizeof(memToRegBuffer), "%s (%d)", memToRegStr[control->memToReg], control->memToReg);
    const int memToRegOffset = (13 - strlen(memToRegBuffer)) / 2;

    mvwprintw(win, l3, 27 + memToRegOffset, "%s", memToRegBuffer);
    const int8_t ulaSourceValue = (control->ulaSource == 0) ? registers[instruction->rt] : instruction->imm;
    char ulaSourceBuffer[37];
    snprintf(ulaSourceBuffer, sizeof(ulaSourceBuffer), "%s (fonte: %d, valor: %04d)", ulaSourceStr[control->ulaSource],
             control->ulaSource, ulaSourceValue);
    const int ulaSourceOffset = (36 - strlen(ulaSourceBuffer)) / 2;
    mvwprintw(win, l3, 41 + ulaSourceOffset, "%s", ulaSourceBuffer);
    mvwprintw(win, l3, 82, "%02d", control->ulaControl);
    mvwprintw(win, l3, 93, "%s", boolStr[control->wrtReg ? 1 : 0]);
    mvwprintw(win, l3, 104, "%s", boolStr[control->wrtMem ? 1 : 0]);
    mvwaddch(win, l3, 0, ACS_VLINE);
    mvwaddch(win, l3, 7, ACS_VLINE);
    mvwaddch(win, l3, 16, ACS_VLINE);
    mvwaddch(win, l3, 26, ACS_VLINE);
    mvwaddch(win, l3, 40, ACS_VLINE);
    mvwaddch(win, l3, 77, ACS_VLINE);
    mvwaddch(win, l3, 88, ACS_VLINE);
    mvwaddch(win, l3, 98, ACS_VLINE);
    mvwaddch(win, l3, 109, ACS_VLINE);
    const int l4 = l3 + 1;
    mvwaddch(win, l4, 0, ACS_LLCORNER);
    mvwhline(win, l4, 1, ACS_HLINE, 108);
    mvwaddch(win, l4, 7, ACS_BTEE);
    mvwaddch(win, l4, 16, ACS_BTEE);
    mvwaddch(win, l4, 26, ACS_BTEE);
    mvwaddch(win, l4, 40, ACS_BTEE);
    mvwaddch(win, l4, 77, ACS_BTEE);
    mvwaddch(win, l4, 88, ACS_BTEE);
    mvwaddch(win, l4, 98, ACS_BTEE);
    mvwaddch(win, l4, 109, ACS_LRCORNER);
}

void refreshExecution(WINDOW *ifWin, WINDOW *idWin, WINDOW *exWin, WINDOW *memWin, WINDOW *wbWin, WINDOW *regWin,
                      WINDOW *memInstWin, WINDOW *memDataWin, const int *memInstIdx, const int *memDataIdx) {
    werase(ifWin);
    wsyncup(ifWin);
    werase(idWin);
    wsyncup(idWin);
    werase(exWin);
    wsyncup(exWin);
    werase(memWin);
    wsyncup(memWin);
    werase(wbWin);
    wsyncup(wbWin);
    werase(regWin);
    wsyncup(regWin);
    werase(memInstWin);
    wsyncup(memInstWin);
    werase(memDataWin);
    wsyncup(memDataWin);

    CombinationalState C;
    createCombinational(&C);


    // CONTEÚDO DAS JANELAS DO MODO DE EXECUÇÃO
    // CONTEÚDO BUSCA DE INSTRUÇÃO
    // if
    box(ifWin, 0, 0);
    // header
    mvwaddch(ifWin, 2, 0, ACS_LTEE);
    mvwhline(ifWin, 2, 1, ACS_HLINE, 40);
    mvwaddch(ifWin, 2, 41, ACS_RTEE);
    mvwaddch(ifWin, 2, 10, ACS_TTEE);
    mvwaddch(ifWin, 2, 29, ACS_TTEE);
    mvwprintw(ifWin, 1, 10, "Busca de Instrução (IF)");
    // end header
    // first line
    mvwprintw(ifWin, 3, 2, "Esc PC");
    mvwaddch(ifWin, 3, 10, ACS_VLINE);
    mvwprintw(ifWin, 3, 16, "Fonte PC");
    mvwaddch(ifWin, 3, 29, ACS_VLINE);
    mvwprintw(ifWin, 3, 32, "Novo PC");
    mvwaddch(ifWin, 4, 0, ACS_LTEE);
    mvwhline(ifWin, 4, 1, ACS_HLINE, 40);
    mvwaddch(ifWin, 4, 10, ACS_PLUS);
    mvwaddch(ifWin, 4, 29, ACS_PLUS);
    mvwaddch(ifWin, 4, 41, ACS_RTEE);
    // end first line
    // first data
    mvwprintw(ifWin, 5, 5, "%s", boolStr[C.IF_wrtPC]); // todo: change with the real value after hazard processing
    mvwaddch(ifWin, 5, 10, ACS_VLINE);
    char pcSource[256];
    int pcSourceOffset = 0;
    // this needs change
    const int branch = pipeline.ID.ctrl.branch && C.EX_ulaOut.equal;
    if (C.IF_wrtPC) {
        if (branch == 0) {
            sprintf(pcSource, "PC +1");
        } else if (branch == 1) {
            sprintf(pcSource, "Branch");
        }
        if (C.ID_control.jump) {
            sprintf(pcSource, "Jump");
        }
    } else {
        sprintf(pcSource, "-");
    }
    pcSourceOffset = (18 - strlen(pcSource)) / 2;
    mvwprintw(ifWin, 5, 12 + pcSourceOffset, "%s", pcSource);
    mvwaddch(ifWin, 5, 29, ACS_VLINE);
    char newPc[256];
    if (C.IF_wrtPC) {
        sprintf(newPc, "%03d", C.IF_PC);
    } else {
        sprintf(newPc, " - ");
    }
    mvwprintw(ifWin, 5, 34, "%s", newPc);
    mvwaddch(ifWin, 6, 0, ACS_LTEE);
    mvwhline(ifWin, 6, 1, ACS_HLINE, 40);
    mvwaddch(ifWin, 6, 10, ACS_PLUS);
    mvwaddch(ifWin, 6, 29, ACS_BTEE);
    mvwaddch(ifWin, 6, 41, ACS_RTEE);
    // end data
    // second line
    mvwprintw(ifWin, 7, 5, "PC");
    mvwaddch(ifWin, 7, 10, ACS_VLINE);
    mvwprintw(ifWin, 7, 13, "Instrução (lida da memória)");
    // end second line
    // second data
    mvwaddch(ifWin, 8, 0, ACS_LTEE);
    mvwhline(ifWin, 8, 1, ACS_HLINE, 40);
    mvwaddch(ifWin, 8, 10, ACS_PLUS);
    mvwaddch(ifWin, 8, 41, ACS_RTEE);
    mvwprintw(ifWin, 9, 4, "%03d", pc);
    mvwaddch(ifWin, 9, 10, ACS_VLINE);
    const Instruction *inst = C.IF_selRI == 0 ? &memInstruction.instructions[pc] : &emptyInstruction;
    const int instOffset = (30 - strlen(inst->asmInstruction)) / 2;
    mvwprintw(ifWin, 9, 11 + instOffset, "%s", inst->asmInstruction);
    mvwaddch(ifWin, 10, 0, ACS_LTEE);
    mvwhline(ifWin, 10, 1, ACS_HLINE, 40);
    mvwaddch(ifWin, 10, 10, ACS_PLUS);
    mvwaddch(ifWin, 10, 41, ACS_RTEE);
    // end second data
    // second line
    mvwprintw(ifWin, 11, 3, "EscRI");
    mvwaddch(ifWin, 11, 10, ACS_VLINE);
    mvwprintw(ifWin, 11, 26, "%s", boolStr[C.IF_wrtRI]);
    // end second line
    // second data
    // mvwprintw(ifWin, 12, 4, "%03d", pc);
    // mvwaddch(ifWin, 12, 10, ACS_VLINE);
    // mvwprintw(ifWin, 12, 11 + instOffset, "%s", "loco");
    mvwaddch(ifWin, 12, 0, ACS_LTEE);
    mvwhline(ifWin, 12, 1, ACS_HLINE, 40);
    mvwaddch(ifWin, 12, 10, ACS_BTEE);
    mvwaddch(ifWin, 12, 41, ACS_RTEE);
    // end if


    // id
    // << CONTEÚDO DE DECODIFICAÇÃO DE INSTRUÇÃO >>
    box(idWin, 0, 0);
    // header
    mvwaddch(idWin, 2, 0, ACS_LTEE);
    mvwhline(idWin, 2, 1, ACS_HLINE, 108);
    mvwaddch(idWin, 2, 109, ACS_RTEE);
    mvwprintw(idWin, 1, 39, "Decodificação de Instrução (ID)");
    // end header
    // ri
    mvwprintw(idWin, 3, 54, "RI");
    instructionUI(idWin, 4, -1, &pipeline.IF.IR);
    // end ri
    // pc+1
    mvwaddch(idWin, 8, 11, ACS_TTEE);
    mvwprintw(idWin, 9, 3, "PC + 1");
    mvwaddch(idWin, 9, 11, ACS_VLINE);
    mvwaddch(idWin, 10, 0, ACS_LTEE);
    mvwhline(idWin, 10, 1, ACS_HLINE, 108);
    mvwaddch(idWin, 10, 11, ACS_BTEE);
    mvwaddch(idWin, 10, 109, ACS_RTEE);
    mvwprintw(idWin, 9, 58, "%03d", pipeline.IF.PCP1);
    // end pc+1
    // controle
    mvwprintw(idWin, 11, 51, "Controle");
    controlUI(idWin, 12, &pipeline.IF.IR, &C.ID_control);
    mvwaddch(idWin, 16, 0, ACS_LTEE);
    mvwaddch(idWin, 16, 109, ACS_RTEE);
    // end control
    // registers
    mvwaddch(idWin, 18, 0, ACS_LTEE);
    mvwhline(idWin, 18, 1, ACS_HLINE, 108);
    mvwaddch(idWin, 18, 109, ACS_RTEE);
    mvwprintw(idWin, 17, 48, "Registradores");
    mvwaddch(idWin, 18, 55, ACS_TTEE);
    mvwprintw(idWin, 19, 25, "A ($%1d)", pipeline.IF.IR.rs);
    mvwprintw(idWin, 19, 80, "B ($%1d)", pipeline.IF.IR.rt);
    mvwaddch(idWin, 19, 55, ACS_VLINE);
    mvwaddch(idWin, 20, 0, ACS_LTEE);
    mvwaddch(idWin, 20, 109, ACS_RTEE);
    mvwhline(idWin, 20, 1, ACS_HLINE, 108);
    mvwaddch(idWin, 20, 55, ACS_PLUS);
    mvwaddch(idWin, 21, 55, ACS_VLINE);
    mvwprintw(idWin, 21, 26, "%04d", C.ID_A);
    mvwprintw(idWin, 21, 81, "%04d", C.ID_B);
    mvwaddch(idWin, 22, 55, ACS_BTEE);
    // end registers
    // end id
    // FIM DO CONTEÚDO DE DECODIFICAÇÃO DE INSTRUÇÃO

    // ex
    // << CONTEÚDO DE EXECUÇÃO >>
    box(exWin, 0, 0);
    // header
    mvwaddch(exWin, 2, 0, ACS_LTEE);
    mvwhline(exWin, 2, 1, ACS_HLINE, 54);
    mvwaddch(exWin, 2, 55, ACS_RTEE);
    mvwprintw(exWin, 1, 22, "Execução (EX)");
    // end header

    // ula
    mvwaddch(exWin, 4, 0, ACS_LTEE);
    mvwhline(exWin, 4, 1, ACS_HLINE, 54);
    mvwaddch(exWin, 4, 55, ACS_RTEE);
    mvwaddch(exWin, 4, 27, ACS_TTEE);
    mvwprintw(exWin, 3, 26, "ULA");
    mvwprintw(exWin, 5, 9, "Entrada");
    mvwprintw(exWin, 5, 38, "Saída");
    mvwaddch(exWin, 5, 27, ACS_VLINE);
    mvwaddch(exWin, 6, 0, ACS_LTEE);
    mvwhline(exWin, 6, 1, ACS_HLINE, 54);
    mvwaddch(exWin, 6, 55, ACS_RTEE);
    mvwaddch(exWin, 6, 13, ACS_TTEE);
    mvwaddch(exWin, 6, 27, ACS_PLUS);
    mvwaddch(exWin, 6, 41, ACS_TTEE);

    char aUla[255];
    switch (C.EX_selA) {
        case 0: {
            sprintf(aUla, "A");
            break;
        }
        case 1: {
            sprintf(aUla, "forward mem");
            break;
        }
        case 2: {
            sprintf(aUla, "forward ex");
            break;
        }
        default: break;
    }
    const int aUlaOffset = (12 - strlen(aUla)) / 2;
    mvwprintw(exWin, 7, 1 + aUlaOffset, "%s", aUla);
    mvwprintw(exWin, 7, 18, "%04d",  C.EX_ulaInA);
    mvwprintw(exWin, 7, 30, "Resultado");
    mvwprintw(exWin, 7, 46, "%04d", C.EX_ulaOut.value);
    mvwaddch(exWin, 7, 13, ACS_VLINE);
    mvwaddch(exWin, 7, 27, ACS_VLINE);
    mvwaddch(exWin, 7, 41, ACS_VLINE);

    mvwaddch(exWin, 8, 0, ACS_LTEE);
    mvwhline(exWin, 8, 1, ACS_HLINE, 54);
    mvwaddch(exWin, 8, 55, ACS_RTEE);
    mvwaddch(exWin, 8, 13, ACS_PLUS);
    mvwaddch(exWin, 8, 27, ACS_PLUS);
    mvwaddch(exWin, 8, 41, ACS_PLUS);

    char bUla[255];
    switch (C.EX_selB) {
        case 0: {
            if (pipeline.ID.ctrl.ulaSource == 0) {
                sprintf(bUla, "B");
            } else {
                sprintf(bUla, "imediato");
            }
            break;
        }
        case 1: {
            sprintf(bUla, "forward mem");
            break;
        }
        case 2: {
            sprintf(bUla, "forward ex");
            break;
        }
        default: break;
    }
    const int bUlaOffset = (12 - strlen(bUla)) / 2;
    mvwprintw(exWin, 9, 1 + bUlaOffset, "%s", bUla);
    mvwprintw(exWin, 9, 18, "%04d", C.EX_ulaInB);
    mvwprintw(exWin, 9, 29, "Val. Iguais");
    mvwprintw(exWin, 9, 48, "%s", boolStr[C.EX_ulaOut.equal]);
    mvwaddch(exWin, 9, 13, ACS_VLINE);
    mvwaddch(exWin, 9, 27, ACS_VLINE);
    mvwaddch(exWin, 9, 41, ACS_VLINE);

    mvwaddch(exWin, 10, 0, ACS_LTEE);
    mvwhline(exWin, 10, 1, ACS_HLINE, 54);
    mvwaddch(exWin, 10, 55, ACS_RTEE);
    mvwaddch(exWin, 10, 13, ACS_PLUS);
    mvwaddch(exWin, 10, 27, ACS_PLUS);
    mvwaddch(exWin, 10, 41, ACS_PLUS);

    mvwprintw(exWin, 11, 1, "Controle ULA");
    mvwprintw(exWin, 11, 18, "%03d", pipeline.ID.ctrl.ulaControl);
    mvwprintw(exWin, 11, 31, "Overflow");
    mvwprintw(exWin, 11, 48, "%s", boolStr[C.EX_ulaOut.overflow]);
    mvwaddch(exWin, 11, 13, ACS_VLINE);
    mvwaddch(exWin, 11, 27, ACS_VLINE);
    mvwaddch(exWin, 11, 41, ACS_VLINE);

    mvwaddch(exWin, 12, 0, ACS_LTEE);
    mvwhline(exWin, 12, 1, ACS_HLINE, 54);
    mvwaddch(exWin, 12, 55, ACS_RTEE);
    mvwaddch(exWin, 12, 13, ACS_BTEE);
    mvwaddch(exWin, 12, 27, ACS_PLUS);
    mvwaddch(exWin, 12, 41, ACS_BTEE);

    // end ula

    // branch address
    mvwaddch(exWin, 14, 0, ACS_LTEE);
    mvwhline(exWin, 14, 1, ACS_HLINE, 54);
    mvwaddch(exWin, 14, 55, ACS_RTEE);
    mvwaddch(exWin, 14, 27, ACS_PLUS);
    mvwprintw(exWin, 13, 2, "End. Branch (pc+1 + imm)");
    mvwaddch(exWin, 13, 27, ACS_VLINE);
    mvwprintw(exWin, 13, 34, "%03d + %03d = %03d", pipeline.ID.PCP1, pipeline.ID.imm, (pipeline.ID.PCP1 + pipeline.ID.imm));
    // end branch address
    // rd
    mvwaddch(exWin, 16, 0, ACS_LTEE);
    mvwhline(exWin, 16, 1, ACS_HLINE, 54);
    mvwaddch(exWin, 16, 55, ACS_RTEE);
    mvwaddch(exWin, 16, 27, ACS_BTEE);
    mvwprintw(exWin, 15, 2, "Reg. Destino ($rt ou $rd)");
    mvwaddch(exWin, 15, 27, ACS_VLINE);
    mvwprintw(exWin, 15, 37, "%s = %1d", (pipeline.ID.ctrl.regDst ? "$rd" : "$rt"), C.EX_RD);
    // end rd

    // Instrução
    mvwprintw(exWin, 17, 23, "Instrução");
    mvwhline(exWin, 18, 1, ACS_HLINE, 54); // linha horizontal abaixo de "instrução"
    mvwaddch(exWin, 18, 0, ACS_LTEE); // fecha linha à esquerda
    mvwaddch(exWin, 18, 55, ACS_RTEE); // fecha linha à direita
    mvwprintw(exWin, 19, 20, "%s", pipeline.EX_MEM.IR.asmInstruction); // assembly
    // instructionDataUI (exWin, 19, 50, &pipeline.EX_MEM.IR);

    // end ex
    // FIM DO CONTEÚDO DE EXECUÇÃO

    // mem
    // << CONTEÚDO DE ESCRITA/LEITURA DA MEMÓRIA >>
    box(memWin, 0, 0);
    // header
    mvwaddch(memWin, 2, 0, ACS_LTEE);
    mvwhline(memWin, 2, 1, ACS_HLINE, 40);
    mvwaddch(memWin, 2, 41, ACS_RTEE);
    mvwprintw(memWin, 1, 6, "Escrita/Leitura da Memória (MEM)");
    // end header
    mvwaddch(memWin, 4, 0, ACS_LTEE);
    mvwhline(memWin, 4, 1, ACS_HLINE, 40);
    mvwaddch(memWin, 4, 41, ACS_RTEE);
    mvwprintw(memWin, 3, 16, "Endereço");
    mvwaddch(memWin, 6, 0, ACS_LTEE);
    mvwhline(memWin, 6, 1, ACS_HLINE, 40);
    mvwaddch(memWin, 6, 41, ACS_RTEE);
    mvwaddch(memWin, 6, 20, ACS_TTEE);
    mvwprintw(memWin, 5, 19, "%03d", pipeline.EX_MEM.ulaOut);
    mvwprintw(memWin, 7, 7, "Esc Mem");
    mvwprintw(memWin, 7, 28, "Dado");
    mvwaddch(memWin, 7, 20, ACS_VLINE);
    mvwaddch(memWin, 8, 0, ACS_LTEE);
    mvwhline(memWin, 8, 1, ACS_HLINE, 40);
    mvwaddch(memWin, 8, 20, ACS_PLUS);
    mvwaddch(memWin, 8, 41, ACS_RTEE);
    mvwprintw(memWin, 9, 11, "%s", boolStr[pipeline.EX_MEM.ctrl.wrtMem]);
    char dataStr[256];
    if (pipeline.EX_MEM.ctrl.wrtMem) {
        sprintf(dataStr, "%04d", pipeline.EX_MEM.B);
    } else {
        sprintf(dataStr, " -  ");
    }
    mvwprintw(memWin, 9, 28, "%s", dataStr);
    mvwaddch(memWin, 9, 20, ACS_VLINE);
    mvwaddch(memWin, 10, 0, ACS_LTEE);
    mvwhline(memWin, 10, 1, ACS_HLINE, 40);
    mvwaddch(memWin, 10, 20, ACS_BTEE);
    mvwaddch(memWin, 10, 41, ACS_RTEE);
    // end esc mem
    // read
    mvwaddch(memWin, 12, 0, ACS_LTEE);
    mvwhline(memWin, 12, 1, ACS_HLINE, 40);
    mvwaddch(memWin, 12, 41, ACS_RTEE);
    mvwprintw(memWin, 11, 17, "Leitura");

    mvwaddch(memWin, 14, 0, ACS_LTEE);
    mvwhline(memWin, 14, 1, ACS_HLINE, 40);
    mvwaddch(memWin, 14, 41, ACS_RTEE);
    char readStr[256];
    if (!pipeline.EX_MEM.ctrl.wrtMem) {
        sprintf(readStr, "%04d", C.MEM_MEM);
    } else {
        sprintf(readStr, " -  ");
    }
    mvwprintw(memWin, 13, 19, "%s", readStr);


    // Instrução
    // mvwhline (memWin, 16, 1, ACS_HLINE, 48); // linha horizontal acima de "instrução"
    // mvwaddch (memWin, 16, 41, ACS_RTEE); // fecha linha à direita
    mvwprintw(memWin, 15, 17, "Instrução");
    mvwhline(memWin, 16, 1, ACS_HLINE, 48); // linha horizontal abaixo de "instrução"
    mvwaddch(memWin, 16, 0, ACS_LTEE); // fecha linha à esquerda
    mvwaddch(memWin, 16, 41, ACS_RTEE); // fecha linha à direita
    mvwprintw(memWin, 17, 14, "%s", pipeline.MEM_WEB.IR.asmInstruction); // assembly

    // emd read
    // end mem
    // FIM DO CONTEÚDO DE ESCRITA NA MEMÓRIA

    // wb
    // << CONTEÚDO DE ESCRITA NO REGISTRADOR >>
    box(wbWin, 0, 0);
    mvwaddch(wbWin, 2, 0, ACS_LTEE);
    mvwhline(wbWin, 2, 1, ACS_HLINE, 40);
    mvwaddch(wbWin, 2, 20, ACS_TTEE);
    mvwaddch(wbWin, 2, 41, ACS_RTEE);
    mvwprintw(wbWin, 1, 8, "Escrita no Registrador (WB)");
    mvwaddch(wbWin, 4, 0, ACS_LTEE);
    mvwhline(wbWin, 4, 1, ACS_HLINE, 40);

    mvwaddch(wbWin, 4, 20, ACS_PLUS);
    mvwaddch(wbWin, 4, 41, ACS_RTEE);
    mvwprintw(wbWin, 3, 7, "Esc Reg");
    mvwaddch(wbWin, 3, 20, ACS_VLINE);
    mvwprintw(wbWin, 3, 25, "Reg. Destino");
    mvwaddch(wbWin, 6, 0, ACS_LTEE);
    mvwhline(wbWin, 6, 1, ACS_HLINE, 40);
    mvwaddch(wbWin, 6, 20, ACS_PLUS);
    mvwaddch(wbWin, 6, 41, ACS_RTEE);
    mvwprintw(wbWin, 5, 10, "%s", boolStr[pipeline.MEM_WEB.ctrl.wrtReg]);
    char regDstStr[255];
    if (pipeline.MEM_WEB.ctrl.wrtReg) {
        sprintf(regDstStr, "%1d", pipeline.MEM_WEB.RD);
    } else {
        sprintf(regDstStr, "-");
    }
    mvwprintw(wbWin, 5, 30, "%s", regDstStr);
    mvwaddch(wbWin, 5, 20, ACS_VLINE);
    mvwprintw(wbWin, 7, 8, "Fonte");
    mvwprintw(wbWin, 7, 28, "Dado");
    mvwaddch(wbWin, 7, 20, ACS_VLINE);
    mvwaddch(wbWin, 8, 0, ACS_LTEE);
    mvwhline(wbWin, 8, 1, ACS_HLINE, 40);
    mvwaddch(wbWin, 8, 20, ACS_PLUS);
    mvwaddch(wbWin, 8, 41, ACS_RTEE);
    char wrtRegSource[255];
    if (pipeline.MEM_WEB.ctrl.wrtReg) {
        if (pipeline.MEM_WEB.ctrl.memToReg) {
            sprintf(wrtRegSource, "ula");
        } else {
            sprintf(wrtRegSource, "memória");
        }
    } else {
        sprintf(wrtRegSource, "-");
    }
    const int wrtRegOffset = (19 - strlen(wrtRegSource)) / 2;
    mvwprintw(wbWin, 9, 1 + wrtRegOffset, "%s", wrtRegSource);
    char regDataStr[256];
    if (pipeline.EX_MEM.ctrl.wrtReg) {
        sprintf(regDataStr, "%04d", C.WB_DATA);
    } else {
        sprintf(regDataStr, " -  ");
    }
    mvwprintw(wbWin, 9, 28, "%s", regDataStr);
    mvwaddch(wbWin, 9, 20, ACS_VLINE);
    mvwaddch(wbWin, 10, 0, ACS_LTEE);
    mvwhline(wbWin, 10, 1, ACS_HLINE, 40);
    mvwaddch(wbWin, 10, 20, ACS_BTEE);
    mvwaddch(wbWin, 10, 41, ACS_RTEE);

    // Instrução
    mvwprintw(wbWin, 11, 17, "Instrução");
    mvwhline(wbWin, 12, 1, ACS_HLINE, 48); // linha horizontal abaixo de "instrução"
    mvwaddch(wbWin, 12, 0, ACS_LTEE); // fecha linha à esquerda
    mvwaddch(wbWin, 12, 41, ACS_RTEE); // fecha linha à direita
    mvwprintw(wbWin, 13, 14, "%s", pipeline.wb.IR.asmInstruction); // assembly

    // end wb
    // FIM DO WB


    // registers
    box(regWin, 0, 0);
    mvwprintw(regWin, 1, 9, "Registradores");
    mvwaddch(regWin, 0, 0, ACS_TTEE);
    mvwaddch(regWin, 2, 0, ACS_LTEE);
    mvwhline(regWin, 2, 1, ACS_HLINE, 29);
    mvwaddch(regWin, 2, 16, ACS_TTEE);
    mvwaddch(regWin, 2, 30, ACS_RTEE);
    // pc
    mvwprintw(regWin, 3, 7, "$pc");
    mvwaddch(regWin, 3, 16, ACS_VLINE);
    mvwprintw(regWin, 3, 22, "%04d", pc);
    // end pc
    // general purpose
    int regBase = 4;
    for (int i = 0; i < REG_SIZE; i++) {
        mvwprintw(regWin, regBase, 8, "$%d", i);
        mvwaddch(regWin, regBase, 16, ACS_VLINE);
        mvwprintw(regWin, regBase, 22, "%04d", registers[i]);
        regBase++;
    }
    // end general purpose
    // end registers

    // mem inst
    box(memInstWin, 0, 0);
    mvwaddch(memInstWin, 0, 0, ACS_LTEE);
    mvwaddch(memInstWin, 0, 16, ACS_BTEE);
    mvwaddch(memInstWin, 0, 30, ACS_RTEE);
    // header
    mvwaddch(memInstWin, 2, 0, ACS_LTEE);
    mvwhline(memInstWin, 2, 1, ACS_HLINE, 30);
    mvwaddch(memInstWin, 2, 6, ACS_TTEE);
    mvwaddch(memInstWin, 2, 30, ACS_RTEE);
    mvwprintw(memInstWin, 1, 5, "Memória de Instruções");
    // end header
    // instructions
    int memInstBase = 3;
    for (int i = 0; i < 13; i++) {
        const int idx = *memInstIdx + i;
        mvwprintw(memInstWin, memInstBase, 2, "%03d", idx);
        mvwaddch(memInstWin, memInstBase, 6, ACS_VLINE);
        if (idx == pc) {
            wattron(memInstWin, COLOR_PAIR(1));
        }
        mvwprintw(memInstWin, memInstBase, 8, "%s", memInstruction.instructions[idx].asmInstruction);
        if (idx == pc) {
            wattroff(memInstWin, COLOR_PAIR(1));
        }
        memInstBase++;
    }
    mvwaddch(memInstWin, 16, 0, ACS_LTEE);
    mvwhline(memInstWin, 16, 1, ACS_HLINE, 30);
    mvwaddch(memInstWin, 16, 6, ACS_BTEE);
    mvwaddch(memInstWin, 16, 30, ACS_RTEE);
    // end instructions
    // footer
    wattron(memInstWin, COLOR_PAIR(1));
    mvwprintw(memInstWin, 17, 13, " ↓ ");
    wattroff(memInstWin, COLOR_PAIR(1));
    wattron(memInstWin, COLOR_PAIR(1));
    mvwprintw(memInstWin, 17, 17, " ↑ ");
    wattroff(memInstWin, COLOR_PAIR(1));
    // end footer
    // end mem inst


    // mem data
    box(memDataWin, 0, 0);
    mvwaddch(memDataWin, 0, 0, ACS_LTEE);
    mvwaddch(memDataWin, 0, 30, ACS_RTEE);
    // header
    mvwaddch(memDataWin, 2, 0, ACS_LTEE);
    mvwhline(memDataWin, 2, 1, ACS_HLINE, 30);
    mvwaddch(memDataWin, 2, 16, ACS_TTEE);
    mvwaddch(memDataWin, 2, 30, ACS_RTEE);
    mvwprintw(memDataWin, 1, 7, "Memória de Dados");
    // end header
    // data
    int memDataBase = 3;
    for (int i = 0; i < 13; i++) {
        const int idx = *memDataIdx + i;
        mvwprintw(memDataWin, memDataBase, 7, "%03d", idx);
        mvwaddch(memDataWin, memDataBase, 16, ACS_VLINE);
        mvwprintw(memDataWin, memDataBase, 22, "%04d", memData.data[idx]);
        memDataBase++;
    }
    mvwaddch(memDataWin, 16, 0, ACS_LTEE);
    mvwhline(memDataWin, 16, 1, ACS_HLINE, 30);
    mvwaddch(memDataWin, 16, 16, ACS_BTEE);
    mvwaddch(memDataWin, 16, 30, ACS_RTEE);
    // end data
    // footer
    wattron(memDataWin, COLOR_PAIR(1));
    mvwprintw(memDataWin, 17, 13, " U ");
    wattroff(memDataWin, COLOR_PAIR(1));
    wattron(memDataWin, COLOR_PAIR(1));
    mvwprintw(memDataWin, 17, 17, " J ");
    wattroff(memDataWin, COLOR_PAIR(1));
    // end footer
    // end mem data
}


void execution() {
    WINDOW *win = createWindow();
    keypad(win, TRUE);

    // buttons
    wattron(win, COLOR_PAIR(1));
    mvwprintw(win, 47, 2, " R ");
    wattroff(win, COLOR_PAIR(1));
    mvwprintw(win, 47, 5, " Run");

    wattron(win, COLOR_PAIR(1));
    mvwprintw(win, 47, 10, " S ");
    wattroff(win, COLOR_PAIR(1));
    mvwprintw(win, 47, 13, " Step");

    wattron(win, COLOR_PAIR(1));
    mvwprintw(win, 47, 19, " B ");
    wattroff(win, COLOR_PAIR(1));
    mvwprintw(win, 47, 22, " Step Back");

    wattron(win, COLOR_PAIR(1));
    mvwprintw(win, 47, 33, " Q ");
    wattroff(win, COLOR_PAIR(1));
    mvwprintw(win, 47, 36, " Sair");
    // end buttons

    // sub windows
    WINDOW *ifWin = derwin(win, 23, 42, 1, 11);
    WINDOW *idWin = derwin(win, 23, 110, 1, 57);
    WINDOW *exWin = derwin(win, 23, 56, 24, 15);
    WINDOW *memWin = derwin(win, 23, 42, 24, 73);
    WINDOW *wbWin = derwin(win, 23, 42, 24, 117);
    WINDOW *regWin = derwin(win, 13, 31, 0, 176);
    WINDOW *memInstWin = derwin(win, 19, 31, 12, 176);
    WINDOW *memDataWin = derwin(win, 19, 31, 30, 176);

    // 14
    int memInstIdx = 0;
    int memDataIdx = 0;

    refreshExecution(ifWin, idWin, exWin, memWin, wbWin, regWin, memInstWin, memDataWin, &memInstIdx, &memDataIdx);

    touchwin(win);
    wrefresh(win);


    //     // <<< ESTÁGIOS DO PIPELINE >>>
    // // JANELA GLOBAL DOS ESTÁGIOS DO PIPELINE
    // WINDOW *pipeline_stage_window = newwin (24, 169, 25, 13);
    // refresh();
    //
    // box (pipeline_stage_window, 0, 0);
    //
    // mvwprintw (pipeline_stage_window, 1, 70, "Estágios do Pipeline");
    //
    // // linha horizontal
    // mvwhline (pipeline_stage_window, 2, 1, ACS_HLINE, 170);
    // // ajusta as bordas da linha
    // mvwaddch(pipeline_stage_window, 2, 0, ACS_LTEE);
    // mvwaddch (pipeline_stage_window, 2, 168, ACS_RTEE);
    //
    // wrefresh(pipeline_stage_window);
    // // FIM DA JANELA GLOBAL DOS ESTÁGIOS DO PIPELINE
    //
    //
    // // ESTÁGIO IF
    // WINDOW *pipeline_stage_IF = newwin (20, 33, 28, 15);
    // refresh ();
    //
    // box (pipeline_stage_IF, 0, 0);
    //
    // mvwprintw (pipeline_stage_IF, 1, 15, "IF");
    // // linha
    // mvwhline (pipeline_stage_IF, 2, 1, ACS_HLINE, 31);
    // // ajusta as bordas da linha
    // mvwaddch(pipeline_stage_IF, 2, 0, ACS_LTEE);
    // mvwaddch (pipeline_stage_IF, 2, 32, ACS_RTEE);
    //
    // // DADOS
    // // PC
    // mvwprintw (pipeline_stage_IF, 3, 2, "PC: ");
    // // instrução
    // mvwprintw (pipeline_stage_IF, 4, 2, "Instrução: ");
    //
    // wrefresh (pipeline_stage_IF);
    // // FIM ESTÁGIO IF
    //
    //
    // // ESTÁGIO ID
    // WINDOW *pipeline_stage_ID = newwin (20, 33, 28, 48);
    // refresh ();
    //
    // box (pipeline_stage_ID, 0, 0);
    //
    // mvwprintw (pipeline_stage_ID, 1, 15, "ID");
    // // linha
    // mvwhline (pipeline_stage_ID, 2, 1, ACS_HLINE, 31);
    // // ajusta as bordas da linha
    // mvwaddch(pipeline_stage_ID, 2, 0, ACS_LTEE);
    // mvwaddch (pipeline_stage_ID, 2, 32, ACS_RTEE);
    //
    // // DADOS
    // // regisradores
    // mvwprintw (pipeline_stage_ID, 3, 2, "RD: "); // numero dos regs
    // mvwprintw (pipeline_stage_ID, 4, 2, "RS: "); // numero dos regs
    // mvwprintw (pipeline_stage_ID, 5, 2, "RT: "); // numero dos regs
    // mvwprintw (pipeline_stage_ID, 6, 2, "A (Valor lido em RS): ");
    // mvwprintw (pipeline_stage_ID, 7, 2, "B (Valor lido em RT): ");
    // // imediato
    // mvwprintw (pipeline_stage_ID, 8, 2, "Imediato: ");
    // // SINAIS DE CONTROLE
    // wattron (pipeline_stage_ID, A_REVERSE);
    // mvwprintw (pipeline_stage_ID, 10, 1, "      SINAIS DE CONTROLE       ");
    // wattroff (pipeline_stage_ID, A_REVERSE);
    // mvwprintw (pipeline_stage_ID, 11, 2, "RegDst: ");
    // mvwprintw (pipeline_stage_ID, 12, 2, "ALUSrc: ");
    // mvwprintw (pipeline_stage_ID, 13, 2, "Branch: ");
    // mvwprintw (pipeline_stage_ID, 14, 2, "Jump: ");
    // mvwprintw (pipeline_stage_ID, 15, 2, "MemWrite: ");
    // mvwprintw (pipeline_stage_ID, 16, 2, "MemToReg: ");
    // mvwprintw (pipeline_stage_ID, 17, 2, "RegWrite: ");
    // mvwprintw (pipeline_stage_ID, 18, 2, "ULAControl: ");
    //
    // wrefresh (pipeline_stage_ID);
    //
    // // FIM ESTÁGIO ID
    //
    //
    // // ESTÁGIO EX
    // WINDOW *pipeline_stage_EX = newwin (20, 33, 28, 81);
    // refresh ();
    //
    // box (pipeline_stage_EX, 0, 0);
    //
    // mvwprintw (pipeline_stage_EX, 1, 15, "EX");
    // // linha
    // mvwhline (pipeline_stage_EX, 2, 1, ACS_HLINE, 31);
    // // ajusta as bordas da linha
    // mvwaddch(pipeline_stage_EX, 2, 0, ACS_LTEE);
    // mvwaddch (pipeline_stage_EX, 2, 32, ACS_RTEE);
    //
    // // dados
    // mvwprintw (pipeline_stage_EX, 3, 2, "Operando A: ");
    // mvwprintw (pipeline_stage_EX, 4, 2, "Operando B (ou imediato): ");
    // mvwprintw (pipeline_stage_EX, 5, 2, "ULAOut (Resultado da ULA): ");
    // mvwprintw (pipeline_stage_EX, 6, 2, "Equal: ");
    // mvwprintw (pipeline_stage_EX, 7, 2, "Reg destino calculado: ");
    //
    // wrefresh (pipeline_stage_EX);
    // // FIM ESTÁGIO EX
    //
    //
    // // ESTÁGIO MEM
    // WINDOW *pipeline_stage_MEM = newwin (20, 33, 28, 114);
    // refresh ();
    //
    // box (pipeline_stage_MEM, 0, 0);
    //
    // mvwprintw (pipeline_stage_MEM, 1, 15, "MEM");
    // // linha
    // mvwhline (pipeline_stage_MEM, 2, 1, ACS_HLINE, 31);
    // // ajusta as bordas da linha
    // mvwaddch(pipeline_stage_MEM, 2, 0, ACS_LTEE);
    // mvwaddch (pipeline_stage_MEM, 2, 32, ACS_RTEE);
    //
    // mvwprintw (pipeline_stage_MEM, 3, 2, "ULAOut (endereço de mem): ");
    // mvwprintw (pipeline_stage_MEM, 4, 2, "SW (dado para escrita): ");
    // mvwprintw (pipeline_stage_MEM, 5, 2, "LW (dado lido): ");
    // mvwprintw (pipeline_stage_MEM, 6, 2, "MemWrite: ");
    // mvwprintw (pipeline_stage_MEM, 7, 2, "MemToReg: ");
    // mvwprintw (pipeline_stage_MEM, 8, 2, "RegWrite: ");
    //
    // wrefresh (pipeline_stage_MEM);
    // // FIM ESTÁGIO MEM
    //
    //
    // // ESTÁGIO WB
    // WINDOW *pipeline_stage_WB = newwin (20, 33, 28, 147);
    // refresh ();
    //
    // box (pipeline_stage_WB, 0, 0);
    //
    // mvwprintw (pipeline_stage_WB, 1, 15, "WB");
    // // linha
    // mvwhline (pipeline_stage_WB, 2, 1, ACS_HLINE, 31);
    // // ajusta as bordas da linha
    // mvwaddch(pipeline_stage_WB, 2, 0, ACS_LTEE);
    // mvwaddch (pipeline_stage_WB, 2, 32, ACS_RTEE);
    //
    // mvwprintw (pipeline_stage_WB, 3, 2, "Registrador destino: ");
    // mvwprintw (pipeline_stage_WB, 4, 2, "Valor final: ");
    // mvwprintw (pipeline_stage_WB, 5, 2, "Origem do valor: ");
    // mvwprintw (pipeline_stage_WB, 6, 2, "ULA: ");
    // mvwprintw (pipeline_stage_WB, 7, 2, "Memória: ");
    // mvwprintw (pipeline_stage_WB, 8, 2, "RegWrite: ");
    //
    // wrefresh (pipeline_stage_WB);
    // FIM ESTÁGIO WB
    // <<< FIM DOS ESTÁGIOS DO PIPELINE >>>

    // buttons interactions (hehe)
    while (1) {
        const int ch = wgetch(win);
        mvwprintw(win, 1, 1, " ch: %d ", ch);
        switch (ch) {
            case 'Q':
            case 'q': {
                delwin(ifWin);
                delwin(regWin);
                werase(win);
                wrefresh(win);
                delwin(win);
                return;
            }
            case 'R':
            case 'r': {
                break;
            }
            case 'S':
            case 's': {
                clock();
                const int range = memInstIdx + 12;
                // if (memDataIdx < pc) {
                //     memDataIdx = pc;
                // }
                if (pc < memInstIdx) {
                    memInstIdx = pc;
                }
                if (pc > range) {
                    memInstIdx = pc < 243 ? pc : 243;
                }
                refreshExecution(ifWin, idWin, exWin, memWin, wbWin, regWin, memInstWin, memDataWin, &memInstIdx,
                                 &memDataIdx);
                break;
            }
            case 'B':
            case 'b': {
                back();
                refreshExecution(ifWin, idWin, exWin, memWin, wbWin, regWin, memInstWin, memDataWin, &memInstIdx,
                                 &memDataIdx);
                break;
            }
            case KEY_UP: {
                if (memInstIdx > 0) {
                    memInstIdx--;
                }
                refreshExecution(ifWin, idWin, exWin, memWin, wbWin, regWin, memInstWin, memDataWin, &memInstIdx,
                                 &memDataIdx);
                break;
            }
            case KEY_DOWN: {
                if ((memInstIdx + 13) < MEM_SIZE) {
                    memInstIdx++;
                }
                refreshExecution(ifWin, idWin, exWin, memWin, wbWin, regWin, memInstWin, memDataWin, &memInstIdx,
                                 &memDataIdx);
                break;
            }
            case 'U':
            case 'u': {
                if (memDataIdx > 0) {
                    memDataIdx--;
                }
                refreshExecution(ifWin, idWin, exWin, memWin, wbWin, regWin, memInstWin, memDataWin, &memInstIdx,
                                 &memDataIdx);
                break;
            }
            case 'J':
            case 'j': {
                if ((memDataIdx + 13) < MEM_SIZE) {
                    memDataIdx++;
                }
                refreshExecution(ifWin, idWin, exWin, memWin, wbWin, regWin, memInstWin, memDataWin, &memInstIdx,
                                 &memDataIdx);
                break;
            }
            default: break;
        }
    }
}

void refreshAllProgramUI(WINDOW *regWin, WINDOW *memInstWin, WINDOW *memDataWin, const int *memInstIdx,
                         const int *memDataIdx) {
    werase(regWin);
    wsyncup(regWin);
    werase(memInstWin);
    wsyncup(memInstWin);
    werase(memDataWin);
    wsyncup(memDataWin);

    // registers
    box(regWin, 0, 0);
    mvwprintw(regWin, 1, 9, "Registradores");
    mvwaddch(regWin, 2, 0, ACS_LTEE);
    mvwhline(regWin, 2, 1, ACS_HLINE, 29);
    mvwaddch(regWin, 2, 16, ACS_TTEE);
    mvwaddch(regWin, 2, 30, ACS_RTEE);
    // pc
    mvwprintw(regWin, 3, 7, "$pc");
    mvwaddch(regWin, 3, 16, ACS_VLINE);
    mvwprintw(regWin, 3, 22, "%04d", pc);
    // end pc
    // general purpose
    int regBase = 4;
    for (int i = 0; i < REG_SIZE; i++) {
        mvwprintw(regWin, regBase, 8, "$%d", i);
        mvwaddch(regWin, regBase, 16, ACS_VLINE);
        mvwprintw(regWin, regBase, 22, "%04d", registers[i]);
        regBase++;
    }
    // end general purpose
    // end registers

    // mem inst
    box(memInstWin, 0, 0);
    // header
    instructionHeaderUI(memInstWin, 2);
    mvwprintw(memInstWin, 1, 45, "Memória de Instruções");
    // end header
    // instructions
    int memInstBase = 5;
    for (int i = 0; i < 39; i++) {
        const int idx = *memInstIdx + i;
        instructionDataUI(memInstWin, memInstBase, idx, &memInstruction.instructions[idx]);
        memInstBase++;
    }
    instructionFooterUI(memInstWin, 44);
    // end instructions
    // footer
    wattron(memInstWin, COLOR_PAIR(1));
    mvwprintw(memInstWin, 45, 52, " ↓ ");
    wattroff(memInstWin, COLOR_PAIR(1));
    wattron(memInstWin, COLOR_PAIR(1));
    mvwprintw(memInstWin, 45, 56, " ↑ ");
    wattroff(memInstWin, COLOR_PAIR(1));
    // end footer
    // end mem inst


    // mem data
    box(memDataWin, 0, 0);
    // header
    mvwaddch(memDataWin, 2, 0, ACS_LTEE);
    mvwhline(memDataWin, 2, 1, ACS_HLINE, 30);
    mvwaddch(memDataWin, 2, 16, ACS_TTEE);
    mvwaddch(memDataWin, 2, 30, ACS_RTEE);
    mvwprintw(memDataWin, 1, 7, "Memória de Dados");
    // end header
    // data
    int memDataBase = 3;
    for (int i = 0; i < 41; i++) {
        const int idx = *memDataIdx + i;
        mvwprintw(memDataWin, memDataBase, 7, "%03d", idx);
        mvwaddch(memDataWin, memDataBase, 16, ACS_VLINE);
        mvwprintw(memDataWin, memDataBase, 22, "%04d", memData.data[idx]);
        memDataBase++;
    }
    mvwaddch(memDataWin, 44, 0, ACS_LTEE);
    mvwhline(memDataWin, 44, 1, ACS_HLINE, 30);
    mvwaddch(memDataWin, 44, 16, ACS_BTEE);
    mvwaddch(memDataWin, 44, 30, ACS_RTEE);
    // end data
    // footer
    wattron(memDataWin, COLOR_PAIR(1));
    mvwprintw(memDataWin, 45, 13, " U ");
    wattroff(memDataWin, COLOR_PAIR(1));
    wattron(memDataWin, COLOR_PAIR(1));
    mvwprintw(memDataWin, 45, 17, " J ");
    wattroff(memDataWin, COLOR_PAIR(1));
    // end footer
    // end mem data
}

void allProgramUI() {
    WINDOW *win = createWindow();
    keypad(win, TRUE);


    wattron(win, COLOR_PAIR(1));
    mvwprintw(win, 47, 2, " Q ");
    wattroff(win, COLOR_PAIR(1));
    mvwprintw(win, 47, 5, " Sair");

    WINDOW *regWin = derwin(win, 13, 31, 1, 8);
    WINDOW *memInstWin = derwin(win, 47, 110, 1, 48);
    WINDOW *memDataWin = derwin(win, 47, 31, 1, 166);

    int memInstIdx = 0;
    int memDataIdx = 0;

    refreshAllProgramUI(regWin, memInstWin, memDataWin, &memInstIdx, &memDataIdx);

    touchwin(win);
    wrefresh(win);

    while (1) {
        const int ch = wgetch(win);
        switch (ch) {
            case 'Q':
            case 'q': {
                werase(win);
                wrefresh(win);
                delwin(win);
                return;
            }
            case KEY_UP: {
                if (memInstIdx > 0) {
                    memInstIdx--;
                }
                refreshAllProgramUI(regWin, memInstWin, memDataWin, &memInstIdx, &memDataIdx);
                break;
            }
            case KEY_DOWN: {
                if ((memInstIdx + 39) < MEM_SIZE) {
                    memInstIdx++;
                }
                refreshAllProgramUI(regWin, memInstWin, memDataWin, &memInstIdx, &memDataIdx);
                break;
            }
            case 'U':
            case 'u': {
                if (memDataIdx > 0) {
                    memDataIdx--;
                }
                refreshAllProgramUI(regWin, memInstWin, memDataWin, &memInstIdx, &memDataIdx);
                break;
            }
            case 'J':
            case 'j': {
                if ((memDataIdx + 41) < MEM_SIZE) {
                    memDataIdx++;
                }
                refreshAllProgramUI(regWin, memInstWin, memDataWin, &memInstIdx, &memDataIdx);
                break;
            }
            default: break;
        }
    }
}
