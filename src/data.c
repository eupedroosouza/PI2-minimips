#include "data.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "main.h"
#include "instruction.h"
#include "view.h"

void loadUnifiedMemory() {
    char caminho[1000];
    printf("Digite o caminho do arquivo de memoria (.mem ou .txt): ");
    
    setbuf(stdin, NULL);
    if (fgets(caminho, sizeof(caminho), stdin) != NULL) {
        caminho[strcspn(caminho, "\n")] = 0; 
    }

    FILE *arquivo = fopen(caminho, "r");
    if (arquivo == NULL) {
        printf("\nErro: O arquivo '%s' nao foi encontrado!\n", caminho);
        return;
    }

    char linha[100];

    int i = 0;
    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        linha[strcspn(linha, "\r\n")] = 0; 

        
        if (strlen(linha) == 0) {
            continue;
        }

        if (strcmp(linha, ".data") == 0) {
            i = 128;
            continue;
        }

        decodeWord(&memory[i], linha);
        i++;
    }
    
    fclose(arquivo);
    printf("\nMemoria unificada carregada com sucesso!\n");

    
    if (debug) {
        printf("\n Conteudo da memoria apos o carregamento:\n");
        showMems(); 
    }
}

void saveUnifiedMemory() {
    char caminho[1000];
    printf("Digite o caminho para salvar o arquivo de memoria (.mem): ");
    
    setbuf(stdin, NULL);
    if (fgets(caminho, sizeof(caminho), stdin) != NULL) {
        caminho[strcspn(caminho, "\n")] = 0;
    }

    FILE *arquivo = fopen(caminho, "w");
    if (arquivo == NULL) {
        printf("\nErro: Nao foi possivel criar o arquivo '%s'!\n", caminho);
        return;
    }

    for (int i = 0; i < SPECIFIC_MEM_SIZE; i++) {
        fprintf(arquivo, "%s\n", memory[i].stringedInstruction);
    }
    fprintf(arquivo, ".data\n");
    for (int i = 0; i < SPECIFIC_MEM_SIZE; i++) {
        // Pega o dado e estende para 16 bits pra manter o sinal correto
        const int16_t valor = (int16_t) memory[i].data;
       
        for (int bit = 15; bit >= 0; bit--) {
            fprintf(arquivo, "%d", (valor >> bit) & 1);
        }
        fprintf(arquivo, "\n"); 
    }

    fclose(arquivo);
    printf("\nMemoria unificada salva com sucesso em '%s'!\n", caminho);
}
