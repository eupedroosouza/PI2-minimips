#include <stdio.h>
#include <string.h>


void saveMemData() {
    char caminho_arquivo_dat[1000];

    printf("Digite o caminho para salvar o arquivo .dat: ");

    setbuf(stdin, NULL);
    if (fgets(caminho_arquivo_dat, sizeof(caminho_arquivo_dat), stdin) != NULL) {
        caminho_arquivo_dat[strcspn(caminho_arquivo_dat, "\n")] = 0;
    }


    FILE *arquivo = fopen(caminho_arquivo_dat, "w");

    if (arquivo == NULL) {
        printf("\nErro: Nao foi possivel criar ou abrir o arquivo\n");
        return;
    }


    for (int i = 0; i < memData.size; i++) {
        fprintf(arquivo, "%d\n", memData.data[i]);
    }

    fclose(arquivo);
    
 
}