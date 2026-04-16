#include "main.h"

#include <locale.h>
#include <stdio.h>
#include <stdlib.h>

#include "reset.h"

#ifdef _WIN32
#include <windows.h>
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif
#endif


#include "colors.h"
#include "encoding.h"
#include "instruction.h"
#include "menu.h"
#include  "types.h"
#include  "utils.h"

bool debug = false;

MemInstruction memInstruction;
MemData memData;

PC pc;
Register registers[8];

Statistics stats = {0};

int main(const int argCount, char *args[]) {
    if (argCount >= 1) {
        for (int i = 0; i < argCount; i++) {
            if (strcmp(args[i], "--debug") == 0) {
                debug = true;
            }
        }
    }


#ifdef _WIN32
    // Configura o terminal para usar UTF-8  no Windows (corrige os ascentos).
    SetConsoleOutputCP(CP_UTF8);
    // Configura o terminal para aceitar caracteres ANSI no Windows.
    const HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE) {
        DWORD dwMode = 0;
        if (GetConsoleMode(hOut, &dwMode)) {
            dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            if (!SetConsoleMode(hOut, dwMode)) {
                printf("Erro ao habilitar suporte ANSI no Windows.\n");
            }
        }
    }
#endif

    // header
    println(
        "                    "BOLD_WHITE"Simulador Mini-MIPS 8 bits monociclo - Versão 0.0.1"RESET"\n            Autores:  Pedro S. Moreira, Willian A. Correa, William N. A. Paiva\n\n                             Engenharia de Computação\n               Universidade Federal do Pampa (Unipampa) - Campus Bagé");

    // Create empty Instruction
    decodeInstruction(&emptyInstruction, "0000000000000000");

    // Reset (do that to prevents incorrect data (trash data) on not loaded data)
    resetRegisters();
    resetData();
    resetInstructions();
    // End-reset

    menu();

    return 0;
}

