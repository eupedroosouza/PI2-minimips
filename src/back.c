#include "back.h"

#include <stdio.h>

#include "debugger.h"
#include "main.h"
#include "utils.h"
#include "view.h"

State lastState;
bool alreadySavedAState = false;


void saveLastState() {
    alreadySavedAState = true;
    lastState.pc = pc;
    for (int i = 0; i < 8; i++) {
        lastState.registers[i] = registers[i];
    }
    for (int i = 0; i < memData.size; i++) {
        lastState.memData.data[i] = memData.data[i];
    }
    lastState.memData.size = memData.size;
}

void invalidateLastState() {
    alreadySavedAState = false;
}

void showLastState() {
    println("┌────────────────────────────────────────┐");
    println("│               Last State               │ ");
    println("├────────────┬───────────────────────────┤");
    println("│     PC     │            %03d            │", lastState.pc);
    println("├────────────┴──────┬────────────────────┤");
    println("│     Registers     │       MemData      │");
    println("├───────────────────┼────────────────────┤");
    char registerTable[13][255];
    createRegisterTable(registerTable);
    char memDataTable[260][255];
    debugDataMemTable(memDataTable);
    for (int i = 0; i < 260; i++) {
        if (i < 13) {
            printf("│ %-17s │", registerTable[i]);
        } else {
            printf("│                   │");
        }
        printf("   %-17s  │\n", memDataTable[i]);
    }

    println("└───────────────────┴────────────────────┘");
}

void back() {
    if (!alreadySavedAState) {
        println("Você ainda não executou nenhum ciclo (execute ciclos via run [8] ou step [9] para executar a função de voltar).");
        return;
    }

    pc = lastState.pc;
    for (int i = 0; i < 8; i++) {
        registers[i] = lastState.registers[i];
    }

    for (int i = 0; i < memData.size; i++) {
        memData.data[i] = lastState.memData.data[i];
    }
    println(" Retornando os valores do processador para:");
    showLastState();
}



