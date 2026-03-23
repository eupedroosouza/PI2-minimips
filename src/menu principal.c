#include <stdio.h>
#include <stdlib.h>

void limparbuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void carregar_memoria_instrucoes() {
    printf("carregar ficheiro .mem\n");
}

void carregar_memoria_dados() {
    printf("carregar ficheiro .dat\n");
}

void imprimir_memorias() {
    printf("imprimir o estado atual das memorias\n");
}

void imprimir_registradores() {
    printf("imprimir o banco de registradores\n");
}

void imprimir_simulador() {
    printf("imprimir o estado completo do simulador\n");
}

void salvar_asm() {
    printf("salvar o programa em formato .asm\n");
}

void salvar_dat() {
    printf("salvar os dados em formato .dat\n");
}

void executar_run() {
    printf("executar todo o programa\n");
}

void executar_step() {
    printf("proxima instrucao\n");
}

void executar_back() {
    printf("voltar uma instrucao\n");
}


int main() {
    int opcao;

    do {
        printf("1. Carregar memoria de instrucoes (.mem)\n");
        printf("2. Carregar memoria de Dados (.dat)\n");
        printf("3. Imprimir memorias (instrucoes e dados)\n");
        printf("4. Imprimir banco de registradores\n");
        printf("5. Imprimir todo o simulador (registradores e memorias)\n");
        printf("6. Salvar .asm\n");
        printf("7. Salvar .dat\n");
        printf("8. Executar Programa (Run)\n");
        printf("9. Executar uma instrucao (Step)\n");
        printf("10. Voltar uma instrucao (Back)\n");
        printf("0. Sair do Simulador\n");
       
        
        
      if(scanf("%d", &opcao) != 1) {
            printf("erro\n");
            limparbuffer();
            opcao = -1;
            continue;
        }

        printf("\n");

        
        switch(opcao) {
            case 1:
                carregar_memoria_instrucoes();
                break;
            case 2:
                carregar_memoria_dados();
                break;
            case 3:
                imprimir_memorias();
                break;
            case 4:
                imprimir_registradores();
                break;
            case 5:
                imprimir_simulador();
                break;
            case 6:
                salvar_asm();
                break;
            case 7:
                salvar_dat();
                break;
            case 8:
                executar_run();
                break;
            case 9:
                executar_step();
                break;
            case 10:
                executar_back();
                break;
            case 0:
                printf("encerrar o simulador\n");
                break;
            default:
                printf("Opcao invalida! Tente novamente.\n");
        }
    } while(opcao != 0);

    return 0;
}