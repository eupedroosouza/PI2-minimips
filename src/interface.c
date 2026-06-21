#include "interface.h"

#include <curses.h>
#include <stdlib.h>
#include <string.h>

#include "back.h"
#include "clock.h"
#include "data.h"
#include "instruction.h"
#include "main.h"
#include "reset.h"
#include "view.h"

#define WIDTH 207
#define HEIGHT 49

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
                endwin();
                printAllProgramData();

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
                // endwin();
                //
                // // temporary exec mode
                // while (1) {
                //     printf("select:\n[0] - step\n[1] - back step\n[2] - exit\n");
                //     int op;
                //     scanf("%d", &op);
                //     if (op == 0) {
                //         clock(&pipeline);
                //     } else if (op ==  1) {
                //         back();
                //     } else if (op ==  2) {
                //         break;
                //     }
                // }
                //
                //
                // // init ncurses
                // (void) initscr();
                // keypad(stdscr, TRUE); // enable use special keys
                // cbreak();
                // noecho(); // no send clicked button
                // start_color(); // start clock supports
                // curs_set(FALSE); // remove cursor
                // init_pair(1, COLOR_BLACK, COLOR_WHITE);  // black letter, white background
                break;
            }
            case 7: {
                showStatistics();
                break;
            }
            case 8: {
                resetInstructions();
                break;
            }
            case 9: {
                resetRegisters(&pipeline);
                break;
            }
            case 10: {
                resetData();
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


void refreshExecution(WINDOW *ifWin, WINDOW *regWin, WINDOW *memInstWin, WINDOW *memDataWin, const int *memInstIdx,
                      const int *memDataIdx) {
    werase(ifWin);
    wsyncup(ifWin);
    werase(regWin);
    wsyncup(regWin);
    werase(memInstWin);
    wsyncup(memInstWin);
    werase(memDataWin);
    wsyncup(memDataWin);

    // if
    box(ifWin, 0, 0);

    // header
    mvwaddch(ifWin, 2, 0, ACS_LTEE);
    mvwhline(ifWin, 2, 1, ACS_HLINE, 30);
    mvwaddch(ifWin, 2, 32 - 1, ACS_RTEE);
    mvwprintw(ifWin, 1, 7, "Busca de Instrução");
    // end if

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

    int regBase = 4;
    for (int i = 0; i < REG_SIZE; i++) {
        mvwprintw(regWin, regBase, 8, "$%d", i);
        mvwaddch(regWin, regBase, 16, ACS_VLINE);
        mvwprintw(regWin, regBase, 22, "%04d", registers[i]);
        regBase++;
    }
    // end registers
    //
    // // mem inst
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
    // insts
    int memInstBase = 3;
    for (int i = 0; i < 13; i++) {
        const int idx = *memInstIdx + i;
        mvwprintw(memInstWin, memInstBase, 2, "%03d", idx);
        mvwaddch(memInstWin, memInstBase, 6, ACS_VLINE);
        mvwprintw(memInstWin, memInstBase, 8, "%s", memInstruction.instructions[idx].asmInstruction);
        memInstBase++;
    }
    mvwaddch(memInstWin, 16, 0, ACS_LTEE);
    mvwhline(memInstWin, 16, 1, ACS_HLINE, 30);
    mvwaddch(memInstWin, 16, 6, ACS_BTEE);
    mvwaddch(memInstWin, 16, 30, ACS_RTEE);
    // footer
    wattron(memInstWin, COLOR_PAIR(1));
    mvwprintw(memInstWin, 17, 13, " ↓ ");
    wattroff(memInstWin, COLOR_PAIR(1));
    wattron(memInstWin, COLOR_PAIR(1));
    mvwprintw(memInstWin, 17, 17, " ↑ ");
    wattroff(memInstWin, COLOR_PAIR(1));


    // // end mem data
    box(memDataWin, 0, 0);
    mvwaddch(memDataWin, 0, 0, ACS_LTEE);
    mvwaddch(memDataWin, 0, 30, ACS_RTEE);
    // header
    mvwaddch(memDataWin, 2, 0, ACS_LTEE);
    mvwhline(memDataWin, 2, 1, ACS_HLINE, 30);
    mvwaddch(memDataWin, 2, 16, ACS_TTEE);
    mvwaddch(memDataWin, 2, 30, ACS_RTEE);
    mvwprintw(memDataWin, 1, 7, "Memória de Dados");
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
    // footer
    wattron(memDataWin, COLOR_PAIR(1));
    mvwprintw(memDataWin, 17, 8, " CTRL ");
    wattroff(memDataWin, COLOR_PAIR(1));
    mvwprintw(memDataWin, 17, 15, "+");
    wattron(memDataWin, COLOR_PAIR(1));
    mvwprintw(memDataWin, 17, 17, " ↓ ");
    wattroff(memDataWin, COLOR_PAIR(1));
    wattron(memDataWin, COLOR_PAIR(1));
    mvwprintw(memDataWin, 17, 21, " ↑ ");
    wattroff(memDataWin, COLOR_PAIR(1));


    mvwaddch(memDataWin, 18, 0, ACS_BTEE);
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
    WINDOW *ifWin = derwin(win, 20, 32, (HEIGHT - 20) / 2, 2);
    WINDOW *regWin = derwin(win, 13, 31, 0, 176);
    WINDOW *memInstWin = derwin(win, 19, 31, 12, 176);
    WINDOW *memDataWin = derwin(win, 19, 31, 30, 176);

    // 14
    int memInstIdx = 0;
    int memDataIdx = 0;

    refreshExecution(ifWin, regWin, memInstWin, memDataWin, &memInstIdx, &memDataIdx);

    touchwin(win);
    wrefresh(win);

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
                clock(&pipeline);
                refreshExecution(ifWin, regWin, memInstWin, memDataWin, &memInstIdx, &memDataIdx);
                break;
            }
            case 'B':
            case 'b': {
                back();
                break;
            }
            case KEY_UP: {
                if (memInstIdx > 0) {
                    memInstIdx--;
                }
                refreshExecution(ifWin, regWin, memInstWin, memDataWin, &memInstIdx, &memDataIdx);
                break;
            }
            case KEY_DOWN: {
                if ((memInstIdx + 13) < MEM_SIZE) {
                    memInstIdx++;
                }
                refreshExecution(ifWin, regWin, memInstWin, memDataWin, &memInstIdx, &memDataIdx);
                break;
            }
            case CTL_UP: {
                if (memDataIdx > 0) {
                    memDataIdx--;
                }
                refreshExecution(ifWin, regWin, memInstWin, memDataWin, &memInstIdx, &memDataIdx);
                break;
            }
            case CTL_DOWN: {
                if ((memDataIdx + 13) < MEM_SIZE) {
                    memDataIdx++;
                }
                refreshExecution(ifWin, regWin, memInstWin, memDataWin, &memInstIdx, &memDataIdx);
                break;
            }
            default: break;
        }
    }
}
