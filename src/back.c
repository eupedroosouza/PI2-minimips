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
    backState->memory = memory;
    backState->memory.dataSize = memory.dataSize;
    backState->previous = lastState;
    backState->state = state;
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
    state = lastState->state;
    memory = lastState->memory;
    println(" -> Os valores foram redefinidos para o estado anterior da máquina.");

    BackState *aux = lastState;
    lastState = lastState->previous;
    free(aux);
}
