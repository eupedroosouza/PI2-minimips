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

WINDOW * createWindow() {
    (void) initscr();
    keypad(stdscr, TRUE); // enable use special keys
    cbreak();
    noecho(); // no send clicked button
    start_color(); // start clock supports
    curs_set(FALSE); // remove cursor
    init_pair(1, COLOR_BLACK, COLOR_WHITE);
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


void menu2() {
    while (1) {
        (void) initscr();
        keypad(stdscr, TRUE); // enable use special keys
        noecho(); // no send clicked button
        start_color(); // start clock supports
        curs_set(FALSE); // remove cursor

        init_pair(1, COLOR_BLACK, COLOR_WHITE); // black letter, white background

        WINDOW *win = newwin(HEIGHT, WIDTH, (LINES - HEIGHT) / 2, (COLS - WIDTH) / 2); // centralized window
        box(win, 0, 0); // border

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
        endwin();

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
    WINDOW *win = createWindow();

    wattron(win, A_BOLD);
    mvwprintw(win, 2, 2, "Digite o caminho do arquivo que contém a memória de instrução .mem:");
    wattroff(win, A_BOLD);

    mvwprintw(win, 3, 2, "> ");
    refresh();
    wrefresh(win);

    int idx = 0;
    char fileName[128];
    wmove(win, 3, 4);
    // mvwgetnstr(win, 3, col, fileName, sizeof(fileName) - 1);
    while (idx < 127) {
        const int ch = getch();

        if (ch == 27) { // ESC
            fileName[0] = '\0';
            goto end_win;
        }
        if (ch == '\n' || ch == '\r') {
            fileName[idx] = '\0';
            break;
        }
        if (ch == KEY_BACKSPACE || ch == 127 || ch == '\b') {
            if (idx > 0) {
                idx--;
                mvwaddch(win, 3, 4 + idx, ' ');
                wmove(win, 3, 4 + idx);
            }
        }

        else if (ch >= 32 && ch <= 126) {
            fileName[idx] = ch;
            mvwaddch(win, 3, 4 + idx, ch);
            idx++;
        }

        refresh();
        wrefresh(win);
    }

    char msg[256];
    FILE *arquivo = fopen(fileName, "r"); // Abre arquivo .mem para leitura
    if (strlen(fileName) == 0) {
        goto end_win;
    }
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

    mvwprintw(win, 4, 2, "> %s", msg);
    mvwprintw(win, 5, 2, "Pressione qualquer tecla para continuar.");
    refresh();
    wrefresh(win);

    getch();

    end_win:
    delwin(win);
}

void loadDataUI() {
    WINDOW *win = createWindow();

    wattron(win, A_BOLD);
    mvwprintw(win, 2, 2, "Digite o caminho do arquivo .dat: ");
    wattroff(win, A_BOLD);

    mvwprintw(win, 3, 2, "> ");
    refresh();
    wrefresh(win);

    echo();
    nocbreak();

    char fileName[128];
    mvwgetnstr(win, 3, 4, fileName, sizeof(fileName) - 1);

    noecho();
    cbreak();

    char msg[256];
    FILE *arquivo = fopen(fileName, "r");

    if (strlen(fileName) == 0) {
        sprintf(msg, "Não há nada para abrir!");
    } else if (arquivo == NULL) {
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

        memData.size =  i;
        fclose(arquivo);
        sprintf(msg, "A memória de dados foi carregada.");
    }

    mvwprintw(win, 4, 2, "> %s", msg);
    mvwprintw(win, 5, 2, "Pressione qualquer tecla para continuar.");
    refresh();
    wrefresh(win);

    getch();

    delwin(win);
}

void saveInstructionOnAssemblyUI() {
    WINDOW *win = createWindow();

    wattron(win, A_BOLD);
    mvwprintw(win, 2, 2, "Digite o diretório para salvar o arquivo .asm: ");
    wattroff(win, A_BOLD);

    mvwprintw(win, 3, 2, "> ");
    refresh();
    wrefresh(win);

    echo();
    nocbreak();

    char fileName[128];
    mvwgetnstr(win, 3, 4, fileName, sizeof(fileName) - 1);

    noecho();
    cbreak();

    char msg[256];
    FILE *arquivoDestino = fopen(fileName, "w");
    if (strlen(fileName) == 0) {
        sprintf(msg, "Não há nada para abrir!");
    } else if (arquivoDestino == NULL) {
        printf("\nErro ao abrir arquivo!");
    } else {
        for (int i = 0; i < 256; i++) {
            fprintf(arquivoDestino, "%s\n", memInstruction.instructions[i].asmInstruction);
        }
        fclose(arquivoDestino);
        sprintf(msg, "As instruções em Assembly foram salvas em: %s.", fileName);
    }

    mvwprintw(win, 4, 2, "> %s", msg);
    mvwprintw(win, 5, 2, "Pressione qualquer tecla para continuar.");
    refresh();
    wrefresh(win);

    getch();

    delwin(win);
}


void saveMemDataUI() {
    WINDOW *win = createWindow();

    wattron(win, A_BOLD);
    mvwprintw(win, 2, 2, "Digite o caminho para salvar o arquivo .dat: ");
    wattroff(win, A_BOLD);

    mvwprintw(win, 3, 2, "> ");
    refresh();
    wrefresh(win);

    echo();
    nocbreak();

    char fileName[128];
    mvwgetnstr(win, 3, 4, fileName, sizeof(fileName) - 1);

    noecho();
    cbreak();

    char msg[256];
    FILE *arquivo = fopen(fileName, "w");
    if (strlen(fileName) == 0) {
        sprintf(msg, "Não há nada para abrir!");
    } else if (arquivo == NULL) {
        printf("\nErro ao abrir arquivo!");
    } else {
        for (int i = 0; i < MEM_SIZE; i++) {
            fprintf(arquivo, "%d\n", memData.data[i]);
        }

        fclose(arquivo);
        sprintf(msg, "A memória de dados foi salva em: %s.", fileName);
    }

    mvwprintw(win, 4, 2, "> %s", msg);
    mvwprintw(win, 5, 2, "Pressione qualquer tecla para continuar.");
    refresh();
    wrefresh(win);

    getch();

    delwin(win);
}

void execution() {
    (void) initscr();
    noecho(); // no send clicked button
    start_color(); // start clock supports
    curs_set(FALSE); // remove cursor

    init_pair(1, COLOR_BLACK, COLOR_WHITE); // black letter, white background

    WINDOW *win = newwin(HEIGHT, WIDTH, (LINES - HEIGHT) / 2, (COLS - WIDTH) / 2); // centralized window
    box(win, 0, 0); // border

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
    mvwprintw(win, 47, 33, " ESC ");
    wattroff(win, COLOR_PAIR(1));
    mvwprintw(win, 47, 38, " Sair");
    // end buttons

    refresh();
    wrefresh(win);


    // buttons interactions (hehe)
    while (1) {
        const int ch = wgetch(win);
        // out of switch to break the while (not switch)
        if (ch == 27) {
            break;
        }
        switch (ch) {
            case 82: {
                break;
            }
            case 83: {
                clock(&pipeline);
                break;
            }
            case 66: {
                back();
                break;
            }
            default: break;
        }
    }

    delwin(win);
    endwin();
}
