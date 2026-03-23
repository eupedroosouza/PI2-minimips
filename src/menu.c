#include "menu.h"

#include <stdio.h>
#include <stdlib.h>

#include "clock.h"
#include "utils.h"

void menu() {
    while (1) {
        println("");
        println("1. Carregar memória de instruções (.mem)");
        println("2. Carregar memória de dados (.dat)");
        println("3. Imprimir memórias (instruções e dados)");
        println("4. Imprimir banco de registradores");
        println("5. Imprimir todo o simulador (registradores e memórias)");
        println("6. Salvar instruçôes em mnemônicos - assembly (.asm)");
        println("7. Salvar memória de dados (.dat)");
        println("8. Executa programa (run)");
        println("9. Executa uma instrução (step)");
        println("10. Voltar uma instrução (back)");
        println("11. Ativar/desativar depuração (debug)");
        println("0. Sair do Simulador");

        printf("\nDigite uma opção: ");

        const int opcao = readIntInStdinSafely();
        switch (opcao) {
            case 1: {
                break;
            }
            case 2: {
                break;
            }
            case 3: {
                break;
            }
            case 4: {
                break;
            }
            case 5: {
                break;
            }
            case 6: {
                break;
            }
            case 7: {
                break;
            }
            case 8: {
                break;
            }
            case 9: {
                clock();
                break;
            }
            case 10: {
                break;
            }
            case 11: {
                debug = !debug;
                if (debug) {
                    println("Depuração ativada.");
                } else {
                    println("Depuração desativada.");
                }
                break;
            }
            case 0: {
                println("Encerrando o simulador!");
                exit(0);
            }
            default: {
                println("Opção inválida! Tente novamente.");
            }
        }
    }
}
