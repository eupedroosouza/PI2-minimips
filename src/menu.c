#include "menu.h"

#include <stdio.h>
#include <stdlib.h>

#include "back.h"
#include "clock.h"
#include "colors.h"
#include "data.h"
#include "instruction.h"
#include "main.h"
#include "utils.h"
#include "view.h"

#include "terminal.h"

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
        println("12. Exibir estatísticas de desempenho ");
        println("0. Sair do Simulador");

        printf("\nDigite uma opção: ");

        const int opcao = readIntInStdinSafely();
        switch (opcao) {
            case 1: {
                loadInstructionsOnMem();
                break;
            }
            case 2: {
                loadDataOnMem();
                break;
            }
            case 3: {
                showMems();

                break;
            }
            case 4: {
                showRegisters();
                break;
            }
            case 5: {
                printAllProgramData();
                break;
            }
            case 6: {
                saveInstructionOnAssembly();
                break;
            }
            case 7: {
                saveMemData();
                break;
            }
            case 8: {
                int execs = 0;
                println("Pressione 'P' para parar a execução do programa quando quiser.");
                println("A execução do programa vai iniciar em 3 segundos... Aguarde!");
                SLEEP_MS(3000);
                while (1) {
                    clock();
                    execs++;
                    if (_kbhit()) {
                        const char usedKey = (char) _getch();
                        if (usedKey == 'p' || usedKey == 'P') {
                            println("Pausando a execução do programa...");
                            break;
                        }
                    }
                }
                println("\nForam executadas %d instruções.", execs);
                break;
            }
            case 9: {
                clock();
                break;
            }
            case 10: {
                back();
                break;
            }
            case 11: {
                debug = !debug;
                if (debug) {
                    println(GREEN"Depuração ativada."RESET);
                } else {
                    println(RED"Depuração desativada."RESET);
                }
                break;
            }

            case 12: {
                showStatistics();
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
