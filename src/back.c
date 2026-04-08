#include "back.h"

#include <stdio.h>

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



