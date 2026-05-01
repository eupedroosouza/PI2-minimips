#include "menu.h"

#include <stdio.h>
#include <stdlib.h>
#include "reset.h"
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
        println("2. Imprimir memórias (instruções e dados)");
        println("3. Imprimir banco de registradores");
        println("4. Imprimir todo o simulador (registradores e memórias)");
        println("5. Salvar instruçôes em mnemônicos - assembly (.asm)");
        println("6. Salvar memória de dados (.dat)");
        println("7. Executa programa (run)");
        println("8. Executa uma instrução (step)");
        println("9. Voltar uma instrução (back)");
        println("10. Exibir estatísticas de desempenho");
        println("11. Resetar memória de instruções");
        println("12. Resetar registradores");
        println("13. Resetar memória de dados ");
        println("14. Ativar/desativar depuração (debug)");
        println("0. Sair do Simulador");

        printf("\nDigite uma opção: ");

        const int opcao = readIntInStdinSafely();
        switch (opcao) {
            case 1: {
                loadInstructionsOnMem();
                break;
            }
            case 2: {
                showMems();

                break;
            }
            case 3: {
                showRegisters();
                break;
            }
            case 4: {
                printAllProgramData();
                break;
            }
            case 5: {
                saveInstructionOnAssembly();
                break;
            }
            case 6: {
                saveMemData();
                break;
            }
            case 7: {
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
            case 8: {
                clock();
                break;
            }
            case 9: {
                back();
                break;
            }

            case 10: {
                showStatistics();
                break;
            }

            case 11: {
                resetInstructions();
                break;
            }

            case 12: {
                resetRegisters();
                break;
            }

            case 13: {
                resetData();
                break;
            }
            case 14: {
                debug = !debug;
                if (debug) {
                    println(GREEN"Depuração ativada."RESET);
                } else {
                    println(RED"Depuração desativada."RESET);
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
