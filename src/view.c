#include "view.h"

#include "main.h"
#include "utils.h"

void showRegisters() {
    println(" Registradores:");
    println("┌───────┬───────┐");
    println("│   #   │ Value │");
    println("├───────┼───────┤");
    for (int i = 0; i < 8; i++) {
        println("│  $%01d   │  %03d  │", i, registers[i]);
    }
    println("│  $pc  │  %03d  │", pc);
    println("└───────┴───────┘");
}
