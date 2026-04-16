#include "view.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "colors.h"
#include "main.h"
#include "ula.h"
#include "utils.h"

const char *memToRegStr[2] = {"mem", "ula"};
const char *ulaSourceStr[2] = {"registrador", "imediato"};
const char *typeStr[4] = {"I", "J", "R", "O"};

// Registers

void createRegisterTable(char table[13][255]) {
    sprintf(table[0], "┌───────┬───────┐");
    sprintf(table[1], "│   "BOLD_WHITE"#"RESET"   │ "BOLD_WHITE"Valor"RESET" │");
    sprintf(table[2], "├───────┼───────┤");
    for (int i = 0; i < 8; i++) {
        sprintf(table[i + 3], "│  $%01d   │  %03d  │", i, registers[i]);
    }
    sprintf(table[11], "│  $pc  │  %03d  │", pc);
    sprintf(table[12], "└───────┴───────┘");
}

void showRegisters() {
    println(" Registradores:");
    char table[255][255];
    createRegisterTable(table);
    for (int i = 0; i < 13; i++) {
        println(table[i]);
    }
}

// End Registers

// Clock

void showClock(const Instruction *instruction, const Control *control) {
    println("┌""────────────────────────────────────────────────────────────────────────────────────────────────────────────┐");
    println("│"BG_GREEN"                                                 "BOLD_WHITE"Clock                                                      "RESET"│");
    println("├────────────────────────────────────────────────────────────────────────────────────────────────────────────┤");
    println("│"BG_CYAN"                                               "BOLD_WHITE"Instrução                                                    "RESET"│");
    println("├─────┬──────────────────┬──────┬─────────────────────────┬────────┬────┬─────┬─────┬─────┬─────┬──────┬─────┤");
    println("│  #  │      Binário     │ Hexa │         Assembly        │  Tipo  │ OP │  RS │  RT │  RD │Funct│  Imm │ Addr│");
    println("├─────┼──────────────────┼──────┼─────────────────────────┼────────┼────┼─────┼─────┼─────┼─────┼──────┼─────┤");
    char buffer[256];
    viewInstruction(instruction, pc, buffer);
    println(buffer);
    println("├─────┴──────────────────┴──────┴─────────────────────────┴────────┴────┴─────┴─────┴─────┴─────┴──────┴─────┤");

    char memToRegBuffer[14];
    snprintf(memToRegBuffer, sizeof(memToRegBuffer), "%s (%d)", memToRegStr[control->memToReg], control->memToReg);
    char memToReg[14];
    centerString(memToRegBuffer, memToReg, 13);

    const int8_t ulaSourceValue = (control->ulaSource == 0) ? registers[instruction->rt] : instruction->imm;
    char ulaSourceBuffer[37];
    snprintf(ulaSourceBuffer, sizeof(ulaSourceBuffer), "%s (fonte: %d, valor: %04d)", ulaSourceStr[control->ulaSource],
             control->ulaSource, ulaSourceValue);
    char ulaSource[37];
    centerString(ulaSourceBuffer, ulaSource, 36);
    println(
        "│"BG_BLUE"                                               "BOLD_WHITE"Controle                                                     "RESET"│");
    println(
        "├──────┬────────┬─────────┬─────────────┬────────────────────────────────────┬──────────┬─────────┬──────────┤");
    println(
        "│ Jump │ Branch │ Reg Dst │ Mem para Reg│              Ula Fonte             │ Ula Ctrl │ Esc Reg │  Esc Mem │");
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

void showClockPc() {
    println("│""     "BOLD_WHITE"PC     "RESET"│                                            %03d                                                │",pc);
    println("├────────────┴───────────────────────────────────────────────────────────────────────────────────────────────┤");
}

void showClockInformation(char *msg, char *msg2) {
    println("│"BG_YELLOW"                                              "BOLD_WHITE"Informações                                                   "RESET"│");
    println("├────────────────────────────────────────────────────────────────────────────────────────────────────────────┤");
    completeWithSpace(msg, 108, 255);
    printf("│%s│\n", msg);
    if (strlen(msg2) > 0) {
        completeWithSpace(msg2, 108, 255);
        printf("│%s│\n", msg2);
    }
    println("└────────────────────────────────────────────────────────────────────────────────────────────────────────────┘");
}


void showClockRegisters(const unsigned int reg1, const int value1, const unsigned int reg2, const int value2) {
    println(
        "│"BG_RED"                                             "BOLD_WHITE"Registradores                                                  "RESET"│");
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

    char equalVal[128];
    if (ulaControl == 6) {
        sprintf(equalVal, "%s", boolStr[out->equal == 0 ? 0 : 1]);
    } else {
        strcpy(equalVal, "-");
    }

    println(
        "│"BG_MAGENTA"                                                  "BOLD_WHITE"ULA                                                       "RESET"│");
    println(
        "├───────────────────────────────────────────────────┬────────────────────────────────────────────────────────┤");
    println(
        "│                    Entrada                        │                          Saída                         │");
    println(
        "├───────────────────────┬───────────────────────────┼────────────────────────────┬───────────────────────────┤");
    println(
        "│       Entrada 1       │           %04d            │          Resultado         │            %04d           │",
        input1, out->value);
    println(
        "├───────────────────────┼───────────────────────────┼────────────────────────────┼───────────────────────────┤");
    println(
        "│       Entrada 2       │           %04d            │         Val. Iguais        │              %-1s            │",
        input2, equalVal);
    println(
        "├───────────────────────┼───────────────────────────┼────────────────────────────┼───────────────────────────┤");
    println(
        "│     Controle ULA      │           %04d            │          Overflow          │              %-1s            │",
        ulaControl, boolStr[out->overflow == 0 ? 0 : 1]);
    println(
        "├────────────┬──────────┴───────────────────────────┴────────────────────────────┴───────────────────────────┤");
}

// End clock

// Statistics

void showStatistics() {
    const int CYCLE_TIME_NS = 4; // 4ns por ciclo

    int totalTime = stats.executedInstructions * CYCLE_TIME_NS;

    println("┌─────────────────────────────────────────────────────────────┐");
    println("│                 Estatísticas de Desempenho                  │");
    println("├─────────────────────────────────────────────────────────────┤");
    println("│ Tempo de cada ciclo: 4ns                                    │");
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

// End-Statistics

// Instruction

void viewInstruction(const Instruction *instruction, const int idx, char *buffer) {
    char strIdx[4];
    if (idx == -1) {
        strcpy(strIdx, " - ");
    } else {
        snprintf(strIdx, sizeof(strIdx), "%03d", idx);
    }

    char opcode[128];
    char rs[128];
    char rt[128];
    char rd[128];
    char funct[128];
    char imm[128];
    char addr[128];

    if (instruction->type == OTHER) {
        strcpy(opcode, " -");
        strcpy(rs, "-");
        strcpy(rt, "-");
        strcpy(rd, "-");
        strcpy(funct, "-");
        strcpy(imm, "  - ");
        strcpy(addr, " - ");
    } else {
        sprintf(opcode, "%02d", instruction->opcode);
        if (instruction->type == J) {
            strcpy(rs, "-");
            strcpy(rt, "-");
            sprintf(addr, "%03d", instruction->addr);
        } else {
            sprintf(rs, "%1d", instruction->rs);
            sprintf(rt, "%1d", instruction->rt);
            strcpy(addr, " - ");
        }
        if (instruction->type == R) {
            sprintf(rd, "%1d", instruction->rd);
            sprintf(funct, "%1d", instruction->funct);
        } else {
            strcpy(funct, "-");
            strcpy(rd, "-");
        }
        if (instruction->type == I) {
            sprintf(imm, "%04d", instruction->imm);
        } else {
            strcpy(imm, "  - ");
        }
    }

    char prettyInstruction[512];
    strcpy(prettyInstruction, instruction->prettyAsmInstruction);
    completeWithSpace(prettyInstruction, 23, 512);

    sprintf(buffer,
            "│ %-3s │ %-16s │ %s │ %s │    %-1s   │ %02d │  %s  │  %s  │  %s  │  %s  │ %s │ %s │",
            strIdx,
            instruction->stringedInstruction,
            instruction->hexa,
            prettyInstruction,
            typeStr[instruction->type],
            instruction->opcode,
            rs,
            rt,
            rd,
            funct,
            imm,
            addr);
}


void createInstructionTable(char memInstructionTable[260][512]) {
    strcpy(memInstructionTable[0], "┌─────┬──────────────────┬──────┬─────────────────────────┬────────┬────┬─────┬─────┬─────┬─────┬──────┬─────┐");
    strcpy(memInstructionTable[1], "│  "BOLD_WHITE"#  │      Binário     │ Hexa │         Assembly        │  Type  │ OP │  RS │  RT │  RD │Funct│  Imm │ Addr"RESET"│");
    strcpy(memInstructionTable[2], "├─────┼──────────────────┼──────┼─────────────────────────┼────────┼────┼─────┼─────┼─────┼─────┼──────┼─────┤");
    for (int i = 0; i < MEM_SIZE; i++) {
        viewInstruction(&memInstruction.instructions[i], i, memInstructionTable[i + 3]);
    }
    strcpy(memInstructionTable[259],
      "└─────┴──────────────────┴──────┴─────────────────────────┴────────┴────┴─────┴─────┴─────┴─────┴──────┴─────┘");
}

void viewInstructions() {
    char memInstructionTable[260][512];
    createInstructionTable(memInstructionTable);
    for (int i = 0; i < 260; i++) {
        println(memInstructionTable[i]);
    }
}

// End Instruction

// Mem Data

void createDataMemTable(char table[260][255]) {
    sprintf(table[0], "┌─────┬───────┐");
    sprintf(table[1], "│  "BOLD_WHITE"#"RESET"  │ "BOLD_WHITE"Valor"RESET" │");
    sprintf(table[2], "├─────┼───────┤");
    for (int i = 0; i < 256; i++) {
        sprintf(table[i + 3], "│ %03d │  %03d  │", i, memData.data[i]);
    }
    sprintf(table[259], "└─────┴───────┘");
}


void viewDataMem() {
    char table[260][255];
    createDataMemTable(table);
    for (int i = 0; i < 260; i++) {
        println(table[i]);
    }
}

// End Mem Data

// Misc

// case 3 do menu
void showMems() {

    printf("┌─────────────────┬────────────────────────────────────────────────────────────────────────────────────────────────────────────────┐\n");
    printf("│"BOLD_WHITE"  Mem. de Dados  │                                            Memória de Instruções                                               "RESET"│\n");
    printf("├─────────────────┼────────────────────────────────────────────────────────────────────────────────────────────────────────────────┤\n");

    char memDataTable[260][255];
    createDataMemTable(memDataTable);
    char memInstructionTable[260][512];
    createInstructionTable(memInstructionTable);

    for (int i = 0; i < 260; i++) {
        // Registers / MemData / MemInstruction
        printf("│ %s │ %s │\n",memDataTable[i], memInstructionTable[i]);
    }

    println("└─────────────────┴────────────────────────────────────────────────────────────────────────────────────────────────────────────────┘\n");
}

// Case 5 do menu. Função que escreve todos os dados do programa na tela
void printAllProgramData() {
    char registerTable[13][255];
    createRegisterTable(registerTable); // Função que printa os registradores
    char memDataTable[260][255];
    createDataMemTable(memDataTable); // Função que printa a memória de dados
    char memInstructionTable[260][512];
    createInstructionTable(memInstructionTable); // Create a memInstructions table

    // Cabeçalho
    println("┌──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────┐");
    println("│                                                             "BOLD_WHITE"Todos os Dados do Programa                                                               │");
    println("├───────────────────┬─────────────────┬────────────────────────────────────────────────────────────────────────────────────────────────────────────────┤");
    println("│   Registradores   │  Mem. de Dados  │                                           Memória de Instruções                                                "RESET"│");
    println("├───────────────────┼─────────────────┼────────────────────────────────────────────────────────────────────────────────────────────────────────────────┤");

    const int maxLinhas = 260; // MemData/MemInstruction é a maior
    for (int i = 0; i < maxLinhas; i++) {
        // Registers / MemData / MemInstruction
        printf("│ %s │ %s │ %s │\n", i < 13 ? registerTable[i] : "                 ", memDataTable[i], memInstructionTable[i]);
    }
    println("└──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────┘");
}

void showLastState() {
    println("┌────────────────────────────────────────┐");
    println("│"BG_MAGENTA"              "BOLD_WHITE"Último Estado             "RESET"│ ");
    println("├────────────┬───────────────────────────┤");
    println("│     "BOLD_WHITE"PC"RESET"     │            %03d            │", lastState.pc);
    println("├────────────┴──────┬────────────────────┤");
    println("│   "BOLD_WHITE"Registradores"RESET"   │  "BOLD_WHITE"Memória de Dados"RESET"  │");
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


// End Misc