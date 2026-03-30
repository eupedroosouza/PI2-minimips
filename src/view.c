#include "view.h"

#include <stdio.h>

#include "main.h"
#include "utils.h"

void showRegisters() {
    println(" Registradores:");
    char table[255][255];
    createRegisterTable(table);
    for (int i = 0; i < 13; i++) {
        println(table[i]);
    }
}

void createRegisterTable(char table[13][255]){
    sprintf(table[0], "┌───────┬───────┐");
    sprintf(table[1], "│   #   │ Value │");
    sprintf(table[2], "├───────┼───────┤");
    for (int i = 0; i < 8; i++) {
        sprintf(table[i + 3], "│  $%01d   │  %03d  │", i, registers[i]);
    }
    sprintf(table[11], "│  $pc  │  %03d  │", pc);
    sprintf(table[12], "└───────┴───────┘");
}
