#include "interface.h"

#include <curses.h>
#include <stdlib.h>
#include <time.h>

#include "back.h"
#include "data.h"
#include "instruction.h"
#include "main.h"
#include "reset.h"
#include "view.h"

#define WIDTH 207
#define HEIGHT 49


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
        WINDOW *selWin = newwin(36, 52, 8, (WIDTH - 46) / 2);
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
            printOption(selWin, 3, 0, select, "Exibir o simulador (registradores, memórias)");
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
                loadInstructionsOnMem();
                break;
            }
            case 2: {
                loadDataOnMem();
                break;
            }
            case 3: {
                printAllProgramData();
                break;
            }
            case 4: {
                saveInstructionOnAssembly();
                break;
            }
            case 5: {
                saveMemData();
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
                return;
            }
            default: break;
        }
    }
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
                clock();
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
