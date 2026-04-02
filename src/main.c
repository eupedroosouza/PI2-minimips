#include "main.h"
#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include "colors.h"
#include "debugger.h"
#include "menu.h"
#include  "types.h"
#include  "utils.h"

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

    menu();

    return 0;
}

