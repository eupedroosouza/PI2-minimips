#include "data.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "debugger.h"
#include "main.h"

void loadDataOnMem() {
    char caminho_arquivo_dat[1000];


    printf("Digite o caminho do arquivo .dat: ");

    setbuf(stdin, NULL);
    if (fgets(caminho_arquivo_dat, sizeof(caminho_arquivo_dat), stdin) != NULL) {
        caminho_arquivo_dat[strcspn(caminho_arquivo_dat, "\n")] = 0;
    }

    FILE *arquivo = fopen(caminho_arquivo_dat, "r");

    if (arquivo == NULL) {
        printf("\nErro: O arquivo '%s' nao foi encontrado!\n", caminho_arquivo_dat);
        return;
    }

    char linha[100];
    int i = 0;

    while (fgets(linha, sizeof(linha), arquivo) != NULL && i < 256) {
        linha[strcspn(linha, "\r\n")] = 0;

        if (strlen(linha) > 0) {
            memData.data[i] = (int8_t) strtol(linha, NULL, 10);
            i++;
        }
    }

    memData.size =  i;
    fclose(arquivo);

    if (debug) {
        printf(" Conteudo da memoria:\n");
        debugDataMem();
    }
}
