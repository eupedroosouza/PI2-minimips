#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include  "types.h"
#include  "utils.h"

MemInstruction memInstruction;
MemData memData;

PC pc;
Register registers[8];

State lastState;

int main() {
    // Configura o terminal para usar UTF-8  no Windows (corrige os ascentos).
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif

    // header
    println(
        "                    Simulador Mini-MIPS 8 bits monociclo - Versão 0.0.1\n            Autores:  Pedro S. Moreira, Willian A. Correa, William N. A. Paiva\n\n                             Engenharia de Computação\n               Universidade Federal do Pampa (Unipampa) - Campus Bagé");


    return 0;
}
