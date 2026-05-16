#include "back.h"

#include <stdio.h>
#include <stdlib.h>

#include "main.h"
#include "utils.h"
#include "view.h"

BackState *lastState = NULL;

void saveState() {
    BackState *backState = malloc(sizeof(BackState));
    if (backState == NULL) {
        printf("Não há mais memória para armazenar backs!\n");
        // check if need exit de program
        return;
    }
    backState->pc = pc;
    backState->registers = registers;
    for (int i = 0; i < MEM_SIZE; i++) {
        backState->memory.data[i] = memory.data[i];
    }
    backState->memory.dataSize = memory.dataSize;
    backState->previous = lastState;
    lastState = backState;
}

void invalidateState() {
    if (lastState == NULL) {
        return;
    }
    BackState *aux = lastState;
    while (aux != NULL) {
        BackState *prev = aux->previous;
        free(aux);
        aux = prev;
    }
}

void back() {
    if (lastState == NULL) {
        println(
            "Você ainda não executou nenhum ciclo (execute ciclos via run [8] ou step [9] para executar a função de voltar).");
        return;
    }

    pc = lastState->pc;
    registers = lastState->registers;

    for (int i = 0; i < memory.dataSize; i++) {
        memory.data[i] = lastState->memory.data[i];
    }
    println(" Retornando os valores do processador para:");
    showLastState();

    BackState *aux = lastState;
    lastState = lastState->previous;
    free(aux);
}
