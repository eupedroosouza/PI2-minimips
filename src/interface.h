#pragma once

#include  <curses.h>

WINDOW *textInputUI(const char *msg, char *buffer);

void finalTextInputUI(WINDOW *win, const char *msg);

void menu2();

void loadInstructionsUI();

void loadDataUI();

void saveInstructionOnAssemblyUI();

void saveMemDataUI();

void execution();
