#include "back.h"

#include <stdio.h>
#include <stdlib.h>

#include "main.h"
#include "utils.h"
#include "view.h"

State *lastState = NULL;

void saveState() {
    State *state = malloc(sizeof(State));
    if (state == NULL) {
        printf("Não há mais memória para armazenar backs!\n");
        // check if need exit de program
        return;
    }
    state->pc = pc;
    for (int i = 0; i < REG_SIZE; i++) {
        state->registers[i] = registers.general[i];
    }
    for (int i = 0; i < MEM_SIZE; i++) {
        state->memory.data[i] = memory.data[i];
    }
    state->memory.dataSize = memory.dataSize;
    state->previous = lastState;
    lastState = state;
}

void invalidateState() {
    if (lastState == NULL) {
        return;
    }
    State *aux = lastState;
    while (aux != NULL) {
        State *prev = aux->previous;
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
    for (int i = 0; i < 8; i++) {
        registers.general[i] = lastState->registers[i];
    }

    for (int i = 0; i < memory.dataSize; i++) {
        memory.data[i] = lastState->memory.data[i];
    }
    println(" Retornando os valores do processador para:");
    showLastState();

    State *aux = lastState;
    lastState = lastState->previous;
    free(aux);
}
