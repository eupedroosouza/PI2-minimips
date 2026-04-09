#include "view.h"

#include <stdio.h>
#include <string.h>

#include "main.h"
#include "utils.h"

const char *memToRegStr[2] = {"mem", "ula"};
const char *ulaSourceStr[2] = {"register", "immediate"};
const char *typeStr[4] = {"I", "J", "R", "O"};

void showRegisters() {
    println(" Registradores:");
    char table[255][255];
    createRegisterTable(table);
    for (int i = 0; i < 13; i++) {
        println(table[i]);
    }
}

void createRegisterTable(char table[13][255]) {
    sprintf(table[0], "┌───────┬───────┐");
    sprintf(table[1], "│   #   │ Value │");
    sprintf(table[2], "├───────┼───────┤");
    for (int i = 0; i < 8; i++) {
        sprintf(table[i + 3], "│  $%01d   │  %03d  │", i, registers[i]);
    }
    sprintf(table[11], "│  $pc  │  %03d  │", pc);
    sprintf(table[12], "└───────┴───────┘");
}


void showClock(const Instruction *instruction, const Control *control) {
    println(
        "┌────────────────────────────────────────────────────────────────────────────────────────────────────────────┐");
    println(
        "│                                                 Clock                                                      │");
    println(
        "├────────────┬───────────────────────────────────────────────────────────────────────────────────────────────┤");
    println(
        "│     PC     │                                            %03d                                                │",
        pc);
    println(
        "├────────────┴───────────────────────────────────────────────────────────────────────────────────────────────┤");
    println(
        "│                                              Instruction                                                   │");
    println(
        "├─────┬──────────────────┬────────────────────────────────┬────────┬────┬─────┬─────┬─────┬─────┬──────┬─────┤");
    println(
        "│  #  │      Binary      │            Assembly            │  Type  │ OP │  RS │  RT │  RD │Funct│  Imm │ Addr│");
    println(
        "├─────┼──────────────────┼────────────────────────────────┼────────┼────┼─────┼─────┼─────┼─────┼──────┼─────┤");
    char buffer[256];
    viewInstruction(instruction, pc, buffer);
    println(buffer);
    println(
        "├─────┴──────────────────┴────────────────────────────────┴────────┴────┴─────┴─────┴─────┴─────┴──────┴─────┤");

    char memToRegBuffer[14];
    snprintf(memToRegBuffer, sizeof(memToRegBuffer), "%s (%d)", memToRegStr[control->memToReg], control->memToReg);
    char memToReg[14];
    centerString(memToRegBuffer, memToReg, 13);

    const int8_t ulaSourceValue = (control->ulaSource == 0) ? registers[instruction->rt] : instruction->imm;
    char ulaSourceBuffer[37];
    snprintf(ulaSourceBuffer, sizeof(ulaSourceBuffer), "%s (source: %d, value: %04d)", ulaSourceStr[control->ulaSource],
             control->ulaSource, ulaSourceValue);
    char ulaSource[37];
    centerString(ulaSourceBuffer, ulaSource, 36);
    println(
        "│                                                Control                                                     │");
    println(
        "├──────┬────────┬─────────┬─────────────┬────────────────────────────────────┬──────────┬─────────┬──────────┤");
    println(
        "│ Jump │ Branch │ Reg Dst │  Mem to Reg │             Ula Source             │ Ula Ctrl │ Wrt Reg │  Wrt Mem │");
    println(
        "├──────┼────────┼─────────┼─────────────┼────────────────────────────────────┼──────────┼─────────┼──────────┤");
    printf("│  %s   │   %s    │   %2d    │%-13s│%-36s│    %2d    │    %s    │     %s    │\n",
           boolStr[control->jump ? 1 : 0],
           boolStr[control->branch ? 1 : 0],
           control->regDst,
           memToReg,
           ulaSource,
           control->ulaControl,
           boolStr[control->wrtReg ? 1 : 0],
           boolStr[control->wrtMem ? 1 : 0]);
    println(
        "├──────┴────────┴─────────┴─────────────┴────────────────────────────────────┴──────────┴─────────┴──────────┤");
}

void showClockInformation(char *msg) {
    println(
        "│                                              Information                                                   │");
    println(
        "├────────────────────────────────────────────────────────────────────────────────────────────────────────────┤");
    println("│%-108s│", msg);
    println(
        "└────────────────────────────────────────────────────────────────────────────────────────────────────────────┘");
}


void showClockRegisters(const unsigned int reg1, const int value1, const unsigned int reg2, const int value2) {
    println(
        "│                                               Registers                                                    │");
    println(
        "├──────────────┬─────────────────────────────────────────────────────────────────────────────────────────────┤");
    println(
        "│      $%1d      │                                          %04d                                               │",
        reg1, value1);
    println(
        "├──────────────┼─────────────────────────────────────────────────────────────────────────────────────────────┤");
    println(
        "│      $%1d      │                                          %04d                                               │",
        reg2, value2);
    println(
        "├──────────────┴─────────────────────────────────────────────────────────────────────────────────────────────┤");
}

void showClockUla(const int input1, const int input2, const int ulaControl, const ULAOut *out) {
    println(
        "│                                                  ULA                                                       │");
    println(
        "├───────────────────────────────────────────────────┬────────────────────────────────────────────────────────┤");
    println(
        "│                    Input                          │                         Output                         │");
    println(
        "├───────────────────────┬───────────────────────────┼────────────────────────────┬───────────────────────────┤");
    println(
        "│        Input 1        │           %04d            │            Value           │            %04d           │",
        input1, out->value);
    println(
        "├───────────────────────┼───────────────────────────┼────────────────────────────┼───────────────────────────┤");
    println(
        "│        Input 2        │           %04d            │           ZeroUla          │              %-1s            │",
        input2, boolStr[out->zeroUla == 0 ? 0 : 1]);
    println(
        "├───────────────────────┼───────────────────────────┼────────────────────────────┼───────────────────────────┤");
    println(
        "│      Ula Control      │           %04d            │              -             │              -            │",
        ulaControl);
    println(
        "├───────────────────────┴───────────────────────────┴────────────────────────────┴───────────────────────────┤");
}

void showStatistics() {
    const int CYCLE_TIME_NS = 4; // 4ns por ciclo

    int totalTime = stats.executedInstructions * CYCLE_TIME_NS;

    println("┌─────────────────────────────────────────────────────────────┐");
    println("│                 Estatísticas de Desempenho                  │");
    println("├─────────────────────────────────────────────────────────────┤");
    println("│ Total de Instruções Executadas: %-27d │", stats.executedInstructions);
    println("│ Tempo Total de Programa:        %-24d ns │", totalTime);
    println("├─────────────────────────────────────────────────────────────┤");
    println("│                      Por Tipo (Formato)                     │");
    println("├────────┬─────────────────────────┬──────────────────────────┤");
    println("│  Tipo  │       Quantidade        │        Tempo Gasto       │");
    println("├────────┼─────────────────────────┼──────────────────────────┤");
    println("│ R      │ %-23d │ %-21d ns │", stats.executedInstructionsPerType.r,
            stats.executedInstructionsPerType.r * CYCLE_TIME_NS);
    println("│ I      │ %-23d │ %-21d ns │", stats.executedInstructionsPerType.i,
            stats.executedInstructionsPerType.i * CYCLE_TIME_NS);
    println("│ J      │ %-23d │ %-21d ns │", stats.executedInstructionsPerType.j,
            stats.executedInstructionsPerType.j * CYCLE_TIME_NS);
    println("├────────┴─────────────────────────┴──────────────────────────┤");
    println("│                     Por Classe (Opcode)                     │");
    println("├────────┬─────────────────────────┬──────────────────────────┤");
    println("│ Classe │       Quantidade        │        Tempo Gasto       │");
    println("├────────┼─────────────────────────┼──────────────────────────┤");
    println("│ ADD    │ %-23d │ %-21d ns │", stats.executedInstructionsPerClass.add,
            stats.executedInstructionsPerClass.add * CYCLE_TIME_NS);
    println("│ SUB    │ %-23d │ %-21d ns │", stats.executedInstructionsPerClass.sub,
            stats.executedInstructionsPerClass.sub * CYCLE_TIME_NS);
    println("│ AND    │ %-23d │ %-21d ns │", stats.executedInstructionsPerClass.and_inst,
            stats.executedInstructionsPerClass.and_inst * CYCLE_TIME_NS);
    println("│ OR     │ %-23d │ %-21d ns │", stats.executedInstructionsPerClass.or_inst,
            stats.executedInstructionsPerClass.or_inst * CYCLE_TIME_NS);
    println("│ ADDI   │ %-23d │ %-21d ns │", stats.executedInstructionsPerClass.addi,
            stats.executedInstructionsPerClass.addi * CYCLE_TIME_NS);
    println("│ LW     │ %-23d │ %-21d ns │", stats.executedInstructionsPerClass.lw,
            stats.executedInstructionsPerClass.lw * CYCLE_TIME_NS);
    println("│ SW     │ %-23d │ %-21d ns │", stats.executedInstructionsPerClass.sw,
            stats.executedInstructionsPerClass.sw * CYCLE_TIME_NS);
    println("│ BEQ    │ %-23d │ %-21d ns │", stats.executedInstructionsPerClass.beq,
            stats.executedInstructionsPerClass.beq * CYCLE_TIME_NS);
    println("│ J      │ %-23d │ %-21d ns │", stats.executedInstructionsPerClass.j,
            stats.executedInstructionsPerClass.j * CYCLE_TIME_NS);
    println("└────────┴─────────────────────────┴──────────────────────────┘");
}


void showMems() {
    println("┌─────────────────────────────────────────────────────────────────────┐");
    println("│                        Memória de Instruções                        │");
    println("├─────┬──────────────────┬────────────────────────────────────────────┤");
    println("│  #  │      Binário     │                  Assembly                  │");
    println("├─────┼──────────────────┼────────────────────────────────────────────┤");

    if (memInstruction.size == 0) {
        println("│                 (Memória de instruções vazia)                       │");
    } else {
        for (int i = 0; i < memInstruction.size; i++) {
            char asmStr[255];

            // Verifica se a string de assembly não está vazia.
            // Se estiver vazia, coloca o valor mockado
            if (memInstruction.instructions[i].asmInstruction[0] != '\0') {
                sprintf(asmStr, "%s", memInstruction.instructions[i].asmInstruction);
            } else {
                sprintf(asmStr, "mock_instruction_%02d", i); // Valor mockado
            }

            println("│ %03d │ %-16s │ %-42s │",
                    i,
                    memInstruction.instructions[i].stringedInstruction,
                    asmStr);
        }
    }
    println("└─────┴──────────────────┴────────────────────────────────────────────┘");

    println("");


    println("┌───────────────────┐");
    println("│ Memória de Dados  │");
    println("├──────┬────────────┤");
    println("│  #   │   Valor    │");
    println("├──────┼────────────┤");

    if (memData.size == 0) {
        println("│   (Mem. vazia)    │");
    } else {
        for (int i = 0; i < memData.size; i++) {
            println("│ %03d  │    %-04d    │", i, memData.data[i]);
        }
    }
    println("└──────┴────────────┘");
}

// Função que escreve todos os dados do programa na tela
void printAllProgramData() {
    char registerTable[13][255];
    createRegisterTable(registerTable); // Função que printa os registradores
    char memDataTable[260][255];
    createDataMemTable(memDataTable); // Função que printa a memória de dados

    // Cabeçalho
    println("┌─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────┐");
    println("│                                                                   All Program Data                                                              │");
    println("├──────────────────┬─────────────────┬────────────────────────────────────────────────────────────────────────────────────────────────────────────┤");
    println("│    Registers     │     MemData     │                                               MemInstruction                                               │");
    println("├──────────────────┼─────────────────┼─────┬──────────────────┬────────────────────────────────┬────────┬────┬─────┬─────┬─────┬─────┬──────┬─────┤");

    int maxLinhas = 260; // MemData é a maior
    for (int i = 0; i < maxLinhas; i++) {
        // Registradores
        if (i < 13) {
            printf("│ %-16s│", registerTable[i]);
        } else {
            printf("│                  │");
        }

        // MemData
        if (i != 1) {
            printf(" %-16s │", memDataTable[i]);
        } else {
            printf(" %-16s ├", memDataTable[i]);
        }


        // MemInstruction
        if (i < 2) {
            switch (i) {
                case 0:
                    printf("  #  │      Binary      │            Assembly            │  Type  │ OP │  RS │  RT │  RD │Funct│ Imm  │ Addr│\n");
                    break;
                case 1:
                    printf("─────┼──────────────────┼────────────────────────────────┼────────┼────┼─────┼─────┼─────┼─────┼──────┼─────┤\n");
                    break;
                default:
                    break;
            }
        } else {
            const int idx = i - 2;
            if (idx < memInstruction.size) {
                char buffer[256];
                viewInstruction(&memInstruction.instructions[idx], idx, buffer);
                // debugInstruction já formata com │ no início e fim
                printf("%s\n", buffer + 3); // +1 pra pular o │ inicial que já foi impresso
            } else {
                printf("     │                  │                                │        │    │     │     │     │     │      │     │\n");
            }
        }
    }
    println("└──────────────────┴─────────────────┴─────┴──────────────────┴────────────────────────────────┴────────┴────┴─────┴─────┴─────┴─────┴──────┴─────┘");
}

void showLastState() {
    println("┌────────────────────────────────────────┐");
    println("│               Last State               │ ");
    println("├────────────┬───────────────────────────┤");
    println("│     PC     │            %03d            │", lastState.pc);
    println("├────────────┴──────┬────────────────────┤");
    println("│     Registers     │       MemData      │");
    println("├───────────────────┼────────────────────┤");
    char registerTable[13][255];
    createRegisterTable(registerTable);
    char memDataTable[260][255];
    createDataMemTable(memDataTable);
    for (int i = 0; i < 260; i++) {
        if (i < 13) {
            printf("│ %-17s │", registerTable[i]);
        } else {
            printf("│                   │");
        }
        printf("   %-17s  │\n", memDataTable[i]);
    }

    println("└───────────────────┴────────────────────┘");
}


void instructionsDebuggerHeader() {
    println("┌─────┬──────────────────┬──────┬─────────────────────────┬────────┬────┬─────┬─────┬─────┬─────┬──────┬─────┐");
    println("│  #  │      Binary      │ Hexa │         Assembly        │  Type  │ OP │  RS │  RT │  RD │Funct│  Imm │ Addr│");
    println("├─────┼──────────────────┼──────┼─────────────────────────┼────────┼────┼─────┼─────┼─────┼─────┼──────┼─────┤");
}

void viewInstruction(const Instruction *instruction, const int idx, char *buffer) {
    char strIdx[4];
    if (idx == -1) {
        strcpy(strIdx, " - ");
    } else {
        snprintf(strIdx, sizeof(strIdx), "%03d", idx);
    }

    char rs[128];
    if (instruction->type == J) {
        strcpy(rs, "-");
    } else {
        sprintf(rs, "%1d", instruction->rs);
    }

    char rt[128];
    if (instruction->type == J) {
        strcpy(rt, "-");
    } else {
        sprintf(rt, "%1d", instruction->rt);
    }

    char rd[128];
    if (instruction->type == J || instruction->type == I) {
        strcpy(rd, "-");
    } else {
        sprintf(rd, "%1d", instruction->rd);
    }

    char funct[128];
    if (instruction->type != R) {
        strcpy(funct, "-");
    } else {
        sprintf(funct, "%1d", instruction->funct);
    }

    char imm[128];
    if (instruction->type != I) {
        strcpy(imm, "  - ");
    } else {
        sprintf(imm, "%04d", instruction->imm);
    }

    char addr[128];
    if (instruction->type != J) {
        strcpy(addr, " - ");
    } else {
        sprintf(addr, "%03d", instruction->addr);
    }

    sprintf(buffer,
            "│ %-3s │ %-16s │ %s │ %-23.23s │    %-1s   │ %02d │  %s  │  %s  │  %s  │  %s  │ %s │ %s │",
            strIdx,
            instruction->stringedInstruction,
            instruction->hexa,
            instruction->asmInstruction,
            typeStr[instruction->type],
            instruction->opcode,
            rs,
            rt,
            rd,
            funct,
            imm,
            addr);
}

void instructionsDebuggerFooter() {
    println(
        "└─────┴──────────────────┴──────┴─────────────────────────┴────────┴────┴─────┴─────┴─────┴─────┴──────┴─────┘");
}

void viewInstructionWithIndex(const Instruction *instruction, const int idx) {
    if (!debug) {
        return;
    }
    println(" Instruction #%d debug:", idx);
    instructionsDebuggerHeader();
    char buffer[256];
    viewInstruction(instruction, idx, buffer);
    println(buffer);
    instructionsDebuggerFooter();
}

void viewInstructions(const Instruction *instructions, const int size) {
    if (!debug) {
        return;
    }
    println(" Instructions debug:");
    instructionsDebuggerHeader();
    for (int i = 0; i < size; i++) {
        char buffer[256];
        viewInstruction(&instructions[i], i, buffer);
        println(buffer);
    }
    instructionsDebuggerFooter();
}

void viewDataMem() {
    char table[260][255];
    createDataMemTable(table);
    for (int i = 0; i < 260; i++) {
        println(table[i]);
    }
}

void createDataMemTable(char table[260][255]) {
    sprintf(table[0], "┌─────┬───────┐");
    sprintf(table[1], "│  #  │ Value │");
    sprintf(table[2], "├─────┼───────┤");
    for (int i = 0; i < 256; i++) {
        sprintf(table[i + 3], "│ %03d │  %03d  │", i, memData.data[i]);
    }
    sprintf(table[259], "└─────┴───────┘");
}
