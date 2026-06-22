#pragma once

#include  <curses.h>

#include "types.h"

WINDOW *textInputUI(const char *msg, char *buffer);

void finalTextInputUI(WINDOW *win, const char *msg);

void confirmationUI(const char *msg);

void menu2();

void loadInstructionsUI();

void loadDataUI();

void saveInstructionOnAssemblyUI();

void saveMemDataUI();

void instructionUI(WINDOW *win, int base, int idx, const Instruction *instruction);

void execution();

void allProgramUI();