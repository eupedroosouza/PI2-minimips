#include "interface.h"

#include <curses.h>
#include <time.h>

#include "back.h"

#define WIDTH 207
#define HEIGHT 49

void menu2() {
    (void) initscr();
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
    // selections
    // 1
    wattron(selWin, COLOR_PAIR(1));
    mvwprintw(selWin, 1, 1, " 1 ");
    wattroff(selWin, COLOR_PAIR(1));
    mvwprintw(selWin, 1, 4, " Carregar memória de instruções (.mem)");
    // 2
    wattron(selWin, COLOR_PAIR(1));
    mvwprintw(selWin, 2, 1, " 2 ");
    wattroff(selWin, COLOR_PAIR(1));
    mvwprintw(selWin, 2, 4, " Carregar memória de dados (.dat)");
    // 3
    wattron(selWin, COLOR_PAIR(1));
    mvwprintw(selWin, 3, 1, " 3 ");
    wattroff(selWin, COLOR_PAIR(1));
    mvwprintw(selWin, 3, 4, " Exibir o simulador (registradores, memórias)");
    // 4
    wattron(selWin, COLOR_PAIR(1));
    mvwprintw(selWin, 4, 1, " 4 ");
    wattroff(selWin, COLOR_PAIR(1));
    mvwprintw(selWin, 4, 4, " Salvar instruçôes em Assembly (.asm)");
    // 5
    wattron(selWin, COLOR_PAIR(1));
    mvwprintw(selWin, 5, 1, " 5 ");
    wattroff(selWin, COLOR_PAIR(1));
    mvwprintw(selWin, 5, 4, " Salvar memória de dados (.dat)");
    // 6
    wattron(selWin, COLOR_PAIR(1));
    mvwprintw(selWin, 6, 1, " 6 ");
    wattroff(selWin, COLOR_PAIR(1));
    mvwprintw(selWin, 6, 4, " Modo de execução");
    // 7
    wattron(selWin, COLOR_PAIR(1));
    mvwprintw(selWin, 7, 1, " 7 ");
    wattroff(selWin, COLOR_PAIR(1));
    mvwprintw(selWin, 7, 4, " Redefinir memória de instruções");
    // 8
    wattron(selWin, COLOR_PAIR(1));
    mvwprintw(selWin, 8, 1, " 8 ");
    wattroff(selWin, COLOR_PAIR(1));
    mvwprintw(selWin, 8, 4, " Redefinir registradores");
    // 9
    wattron(selWin, COLOR_PAIR(1));
    mvwprintw(selWin, 9, 1, " 9 ");
    wattroff(selWin, COLOR_PAIR(1));
    mvwprintw(selWin, 9, 4, " Redefinir memória de dados");
    // 0
    wattron(selWin, COLOR_PAIR(1));
    mvwprintw(selWin, 10, 1, " 0 ");
    wattroff(selWin, COLOR_PAIR(1));
    mvwprintw(selWin, 10, 4, " Encerrar o simulador");

    // help usage
    mvwprintw(win, 43, 83, "Use ");
    wattron(win, COLOR_PAIR(1));
    mvwprintw(win, 43, 87, " ↓ ");
    wattroff(win, COLOR_PAIR(1));
    wattron(win, COLOR_PAIR(1));
    mvwprintw(win, 43, 91, " ↑ ");
    wattroff(win, COLOR_PAIR(1));
    mvwprintw(win, 43, 94, " para navegar ou digite o número!");

    refresh();
    wrefresh(win);
    wrefresh(selWin);

    int select = 0;
    while (1) {
        

    }
    getch();

    delwin(selWin);
    delwin(win);
    endwin();
}

void printOption(WINDOW *win, const int option, const int select, const char *msg) {
    const bool selected = select == option;
    wattron(win, COLOR_PAIR(1));
    mvwprintw(win, option, 1, " %1d ", option);
    wattroff(win, COLOR_PAIR(1));
    if (selected) {
        wattron(win, COLOR_PAIR(1));
    }
    mvwprintw(win, option, 4, msg);
    if (selected) {
        wattroff(win, COLOR_PAIR(1));
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
