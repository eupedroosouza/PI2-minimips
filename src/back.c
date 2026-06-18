#include "back.h"

#include <stdio.h>
#include <stdlib.h>


#include "main.h"
#include "utils.h"
#include "view.h"

BackState *lastState = NULL;



void saveLastState(PipelineRegisters *pipeline) {
    BackState *backState = malloc(sizeof(BackState));
    if (backState == NULL) {
        printf("Não há mais memória para armazenar backs!\n");
        // check if need exit de program
        return;
    }
    backState->pc = pc;
    for (int i = 0; i < 8; i++) {
        backState->registers[i] = registers[i];
    }
    for (int i = 0; i < memData.size ; i++) {
        backState->memdata.data[i] = memData.data[i];
    }
    backState->memdata.size = memData.size;
    backState->pipeline = *pipeline;
    lastState = backState;
}

void invalidateLastState() {
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

void back(PipelineRegisters *pipeline) {
    if (lastState == NULL) {
        println(
            "Você ainda não executou nenhum ciclo (execute ciclos via run [8] ou step [9] para executar a função de voltar).");
        return;
    }

    pc = lastState->pc;
    for (int i = 0; i < 8; i++) {
        registers[i] = lastState->registers[i];
    }

    for (int i = 0; i < memData.size; i++) {
        memData.data[i] = lastState->memdata.data[i];
    }
    *pipeline = lastState->pipeline;
    println(" -> Os valores foram redefinidos para o estado anterior da máquina.");

    BackState *aux = lastState;
    lastState = lastState->previous;
    free(aux);
}



