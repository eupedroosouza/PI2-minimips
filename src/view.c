#include "view.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "back.h"
#include "clock.h"
#include "colors.h"
#include "main.h"
#include "ula.h"
#include "utils.h"

const char *memToRegStr[2] = {"mem", "ula"};
const char *ulaSourceStr[2] = {"registrador", "imediato"};
const char *typeStr[4] = {"I", "J", "R", "O"};

// Registers

void createRegisterTable(char table[52][255]) {
    sprintf(table[0], "┌───────────────┐");
    sprintf(table[1], "│ Banco de Reg. │");
    sprintf(table[2], "├───────┬───────┤");
    sprintf(table[3], "│   "BOLD_WHITE"#"RESET"   │ "BOLD_WHITE"Valor"RESET" │");
    sprintf(table[4], "├───────┼───────┤");
    for (int i = 0; i < 8; i++) {
        sprintf(table[i + 5], "│  $%01d   │  %03d  │", i, registers.general[i]);
    }
    sprintf(table[11], "│  $pc  │  %03d  │", pc);
    sprintf(table[12], "└───────┴───────┘");
}

void createIRTable(char table[13][255]) {
    sprintf(table[0], "┌───────────────┐");
    sprintf(table[1], "│      IR       │");
    sprintf(table[2], "├───────┬───────┤");
    sprintf(table[3], "│   "BOLD_WHITE"#"RESET"   │ "BOLD_WHITE"Valor"RESET" │");
    sprintf(table[4], "├───────┼───────┤");
    sprintf(table[5], "│Opcode │  %03d  │", registers.IR.opcode);
    sprintf(table[6], "│   RS  │  %03d  │", registers.IR.rs);
    sprintf(table[7], "│   RT  │  %03d  │", registers.IR.rt);
    sprintf(table[8], "│   RD  │  %03d  │", registers.IR.rd);
    sprintf(table[9], "│ funct │  %03d  │", registers.IR.funct);
    sprintf(table[10], "│  imm  │  %03d  │", registers.IR.imm);
    sprintf(table[11], "│ addr  │  %03d  │", registers.IR.addr);
    sprintf(table[12], "└───────┴───────┘");
 }

void createIntermediateTable(char table[10][255]) {
    sprintf(table[0], "┌───────────────┐");
    sprintf(table[1], "│ Regs Intermed.│");
    sprintf(table[2], "├───────┬───────┤");
    sprintf(table[3], "│   "BOLD_WHITE"#"RESET"   │ "BOLD_WHITE"Valor"RESET" │");
    sprintf(table[4], "├───────┼───────┤");
    sprintf(table[5], "│  MDR  │  %03d  │", registers.MDR);
    sprintf(table[6], "│   A   │  %03d  │", registers.A);
    sprintf(table[7], "│   B   │  %03d  │", registers.B);
    sprintf(table[8], "│ULA out│  %03d  │", registers.ULAOut);
    sprintf(table[9], "└───────┴───────┘");
}


void showRegisters() {
    println(" Registradores:");
    char table[255][255];
    char IRtable[13][255];
    char intermediateTable[10][255];

    createRegisterTable(table);
    //createIRTable (IRtable);
    createIntermediateTable(intermediateTable);

    for (int i = 0; i < 13; i++) {
        println(table[i]);
    }

    println("");

    for (int i = 0; i < 13; i++) {
        println(IRtable[i]);
    }

    println("");

    for (int i = 0; i < 9; i++) {
        println(intermediateTable[i]);
    }
}

// End Registers

// Clock

void showClock(const Instruction *instruction, const Control *control) {
    println(
        "\n┌────────────────────────────────────────────────────────────────────────────────────────────────────────────┐");
    println(
        "│"BG_GREEN"                                                 "BOLD_WHITE
        "Clock                                                      "RESET"│");
    println(
        "├────────────────────────────────────────────────────────────────────────────────────────────────────────────┤");
    println(
        "│"BG_CYAN"                                               "BOLD_WHITE
        "Instrução                                                    "RESET"│");
    println(
        "├─────┬──────────────────┬──────┬─────────────────────────┬────────┬────┬─────┬─────┬─────┬─────┬──────┬─────┤");
    println(
        "│  #  │      Binário     │ Hexa │         Assembly        │  Tipo  │ OP │  RS │  RT │  RD │Funct│  Imm │ Addr│");
    println(
        "├─────┼──────────────────┼──────┼─────────────────────────┼────────┼────┼─────┼─────┼─────┼─────┼──────┼─────┤");
    char buffer[256];
    viewInstruction(instruction, pc, buffer);
    println(buffer);
    println(
        "├─────┴──────────────────┴──────┴─────────────────────────┴────────┴────┴─────┴─────┴─────┴─────┴──────┴─────┤");

    char memToRegBuffer[15];
    snprintf(memToRegBuffer, sizeof(memToRegBuffer), "%s (%d)", memToRegStr[control->memToReg], control->memToReg);
    char memToReg[15];
    centerString(memToRegBuffer, memToReg, 14);

    const int8_t ulaSourceValue = (control->ulaSourceA == 0) ? registers.general[instruction->rt] : instruction->imm;
    char ulaSourceBuffer[37];
    snprintf(ulaSourceBuffer, sizeof(ulaSourceBuffer), "%s (fonte: %d, valor: %04d)", ulaSourceStr[control->ulaSourceA],
             control->ulaSourceA, ulaSourceValue);
    char ulaSource[37];
    centerString(ulaSourceBuffer, ulaSource, 36);
    println(
        "│"BG_BLUE"                                               "BOLD_WHITE
        "Controle                                                     "RESET"│");
    println(
        "├──────────┬─────────────────┬─────────────────┬─────────┬─────────┬──────────────┬────────┬─────────┬───────┤");
    println(
        BOLD_WHITE
        "│ Fonte PC │   Ula Fonte A   │   Ula Fonte B   │ Esc Reg │ Reg Dst │ Mem para Reg │ Esc IR │ Esc Mem │  IouD │"
        RESET);
    println(
        "├──────────┼─────────────────┼─────────────────┼─────────┼─────────┼──────────────┼────────┼─────────┼───────┤");
    //  Ula Ctrl │ Esc Reg │  Esc Mem │
    printf(
        "│    %1d     │        %1d        │        %1d        │    %s    │    %1d    │%-13s│    %s   │    %s    │   %1d   │\n",
        control->pcSource,
        control->ulaSourceA,
        control->ulaSourceB,
        boolStr[control->wrtMem ? 1 : 0],
        control->regDst,
        memToReg,
        boolStr[control->wrtIr ? 1 : 0],
        boolStr[control->wrtMem ? 1 : 0],
        control->immOrData
        // boolStr[control->branch ? 1 : 0],
        // control->regDst,
        // memToReg,
        // control->ulaSourceA,
        // control->ulaSourceB,
        // control->ulaControl,
        // boolStr[control->wrtReg ? 1 : 0],
        // boolStr[control->wrtMem ? 1 : 0]
    );

    println(
        "├────────┬─┴──────────┬──────┴───────────┬─────┴─────────┴─────────┴──────────────┴────────┴─────────┴───────┤");
    println(
        BOLD_WHITE
        "│ Branch │   Esc PC   │   Controle ULA   │                               Estado                              │"
        RESET);
    println(
        "├────────┼────────────┼──────────────────┼───────────────────────────────────────────────────────────────────┤");
    println(
        "│    %s   │      %s     │        %03d       │                                %04d                               │",
        boolStr[control->branch ? 1 : 0],
        boolStr[control->wrtPc ? 1 : 0],
        control->ulaControl,
        state);
    println(
        "├────────┴────────────┴──────────────────┴───────────────────────────────────────────────────────────────────┤");
}

void showClockPc() {
    println(
        "│""     "BOLD_WHITE"PC     "RESET
        "│                                            %03d                                                │", pc);
    println(
        "├────────────┴───────────────────────────────────────────────────────────────────────────────────────────────┤");
}

void showClockInformation(char *msg, char *msg2) {
    println(
        "│"BG_YELLOW"                                              "BOLD_WHITE
        "Informações                                                   "RESET"│");
    println(
        "├────────────────────────────────────────────────────────────────────────────────────────────────────────────┤");
    completeWithSpace(msg, 108, 255);
    printf("│%s│\n", msg);
    if (strlen(msg2) > 0) {
        completeWithSpace(msg2, 108, 255);
        printf("│%s│\n", msg2);
    }
    println(
        "└────────────────────────────────────────────────────────────────────────────────────────────────────────────┘");
}


void showClockRegisters(const unsigned int reg1, const int value1, const unsigned int reg2, const int value2) {
    println(
        "│"BG_RED"                                             "BOLD_WHITE
        "Registradores                                                  "RESET"│");
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
        "│"BG_MAGENTA"                                                  "BOLD_WHITE
        "ULA                                                       "RESET"│");
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

void showClockRegs() {
    println(
        "│"BG_RED"                                       "BOLD_WHITE
        "Registradores Intermediários                                         "RESET"│");
    println(
        "├─────────────────────┬───────────────────────────┬─────────────────────────┬────────────────────────────────┤");
    println(
        BOLD_WHITE
        "│         MDR         │             A             |            B            |              ULA Out           │"
        RESET);
    println(
        "├─────────────────────┼───────────────────────────┼─────────────────────────┼────────────────────────────────┤");
    println(
        "│         %03d         │            %03d            │           %03d           │                 %03d            │",
        registers.MDR, registers.A, registers.B, registers.ULAOut);
    println(
        "├─────────────────────┴───────────────────────────┴─────────────────────────┴────────────────────────────────┤");
}

// End clock

// Statistics

void showStatistics() {
    println("");
    println("┌──────────────────────────────────────────────────────┐");
    println("│               Estatísticas de Execução               │");
    println("├──────────────────────────────────────────────────────┤");
    println("│ Total de Instruções Executadas: %-20d │", stats.executedInstructions);
    // println("│ Ciclos de Clock Totais: %-28d │", stats.totalCycles);
    println("├──────────────────────────────────────────────────────┤");
    println("│ Por Tipo de Instrução:                              │");
    println("│   Tipo R: %-42d │", stats.executedInstructionsPerType.r);
    println("│   Tipo I: %-42d │", stats.executedInstructionsPerType.i);
    println("│   Tipo J: %-42d │", stats.executedInstructionsPerType.j);
    println("│   Outros: %-42d │", stats.executedInstructionsPerType.other);
    println("├──────────────────────────────────────────────────────┤");
    println("│ Por Classe de Instrução:                            │");
    println("│   ADD   : %-42d │", stats.executedInstructionsPerClass.add);
    println("│   SUB   : %-42d │", stats.executedInstructionsPerClass.sub);
    println("│   AND   : %-42d │", stats.executedInstructionsPerClass.and_inst);
    println("│   OR    : %-42d │", stats.executedInstructionsPerClass.or_inst);
    println("│   ADDI  : %-42d │", stats.executedInstructionsPerClass.addi);
    println("│   LW    : %-42d │", stats.executedInstructionsPerClass.lw);
    println("│   SW    : %-42d │", stats.executedInstructionsPerClass.sw);
    println("│   BEQ   : %-42d │", stats.executedInstructionsPerClass.beq);
    println("│   J     : %-42d │", stats.executedInstructionsPerClass.j);
    println("│   Outras: %-42d │", stats.executedInstructionsPerClass.other);
    println("└──────────────────────────────────────────────────────┘");
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
    strcpy(memInstructionTable[0],
           "┌─────┬──────────────────┬──────┬─────────────────────────┬────────┬────┬─────┬─────┬─────┬─────┬──────┬─────┐");
    strcpy(memInstructionTable[1],
           "│  "BOLD_WHITE
           "#  │      Binário     │ Hexa │         Assembly        │  Type  │ OP │  RS │  RT │  RD │Funct│  Imm │ Addr"
           RESET"│");
    strcpy(memInstructionTable[2],
           "├─────┼──────────────────┼──────┼─────────────────────────┼────────┼────┼─────┼─────┼─────┼─────┼──────┼─────┤");
    for (int i = 0; i < MEM_SIZE; i++) {
        viewInstruction(&memory.instructions[i], i, memInstructionTable[i + 3]);
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
    int i;

    for (i = 0; i < 256 - (memory.size); i++) {
        sprintf(table[i + 3], "│ %03d │  %03d  │", memory.size + i, memory.instructions[i].data);
    }
    
    sprintf(table[i + 3], "└─────┴───────┘");
    i ++;
    for (; i < 256; i++) {
        sprintf(table[i + 3], "               ");
    }
    
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
    printf(
        "┌─────────────────┬────────────────────────────────────────────────────────────────────────────────────────────────────────────────┐\n");
    printf(
        "│"BOLD_WHITE
        "  Mem. de Dados  │                                            Memória de Instruções                                               "
        RESET"│\n");
    printf(
        "├─────────────────┼────────────────────────────────────────────────────────────────────────────────────────────────────────────────┤\n");

    char memDataTable[260][255];
    createDataMemTable(memDataTable);
    char memInstructionTable[260][512];
    createInstructionTable(memInstructionTable);

    for (int i = 0; i < 260; i++) {
        // Registers / MemData / MemInstruction
        printf("│ %s │ %s │\n", memDataTable[i], memInstructionTable[i]);
    }

    println(
        "└─────────────────┴────────────────────────────────────────────────────────────────────────────────────────────────────────────────┘\n");
}

void printBinary(int8_t n) {
    for (int i = 7; i >= 0; i--) {
        printf("%d", (n >> i) & 1); // imprime cada bit
    }
}

// Case 5 do menu. Função que escreve todos os dados do programa na tela
void printAllProgramData() {
    char registerTable[255][255];
    char IRtable [13][255];
    char intermediateTable[10][255];
    char memDataTable[260][255];
    char memInstructionTable[260][512];
    createRegisterTable(registerTable); // Função que printa os registradores
    createIRTable(IRtable);
    createIntermediateTable(intermediateTable);
    createDataMemTable(memDataTable); // Função que printa a memória de dados
    createInstructionTable(memInstructionTable); // Create a memInstructions table

    // Cabeçalho
    println(
    "┌──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────┐");
    println(
    "│                                                             "BOLD_WHITE
    "Todos os Dados do Programa                                                               │");
    println(
    "├───────────────────┬─────────────────┬────────────────────────────────────────────────────────────────────────────────────────────────────────────────┤");
    println(
    "│   Registradores   │  Mem. de Dados  │                                           Memória de Instruções                                                "
    RESET"│");
    println(
    "├───────────────────┼─────────────────┼────────────────────────────────────────────────────────────────────────────────────────────────────────────────┤");
    const int maxLinhas = 260; // MemData/MemInstruction é a maior
    char tabelaRegistradores[255];

    for (int i = 0; i < maxLinhas; i++) {
    strcpy(tabelaRegistradores, "                 "); // por padrão a linha é vazia

    if (i < 13) {

    // imprime banco de registradores
    strcpy(tabelaRegistradores, registerTable[i]);
        } else if (i >= 14 && i < 24) {
    // imprime registradores intermediários
    strcpy(tabelaRegistradores, intermediateTable[i - 14]);
            }
            else if (i >= 25 && i < 38) {
            // Imprime a tabela do IR logo abaixo (linhas 25 a 37)
            // i - 25 faz o índice do IRtable ir perfeitamente de 0 até 12
            strcpy(tabelaRegistradores, IRtable[i - 25]);
        }

    if (i <= memory.size + 2){
        // Registers / MemData / MemInstruction
         printf("│ %s │ %s │ %s │\n", tabelaRegistradores, memDataTable[i], memInstructionTable[i]); // imprime até memória de dados
            }  else if (i > memory.size && i - 3 < MEM_SIZE){

    printf("│ %s │ %s │ │ %03d │ ", tabelaRegistradores, memDataTable[i], i - 3); // imprime até numeração da memória

    printf("00000000");

    printBinary(memory.instructions[i - memory.size - 3].data); // chama printBinary para imprimir bit a bit

    printf(" │  %02x %03d │\n", memory.instructions[i - memory.size - 3].data); // dado
}
        

        }
    println(  "└──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────┘");

}

void showLastState() {
    println("┌────────────────────────────────────────┐");
    println("│"BG_MAGENTA"              "BOLD_WHITE"Último Estado             "RESET"│ ");
    println("├────────────┬───────────────────────────┤");
    println("│     "BOLD_WHITE"PC"RESET"     │            %03d            │", lastState->pc);
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

void viewStateOfMachine() {
    const CombinationalState C = makeCombinational();

    println(
        "┌────────────────────────────────────────────────────────────────────────────────────────────────────────────┐");
    println(
        "│"BG_YELLOW"                                                  Busca                                                     "RESET"│ ");
    println(
        "├────────────┬───────────────────────────────────────────────────────────────────────────────────────────────┤");
    println(
        "│  Endereço  │                                            %03d                                                │",
        C.memAddr);
    println(
        "├─────┬──────┴───────────┬──────┬─────────────────────────┬────────┬────┬─────┬─────┬─────┬─────┬──────┬─────┤");
    println(
        "│  #  │      Binário     │ Hexa │         Assembly        │  Tipo  │ OP │  RS │  RT │  RD │Funct│  Imm │ Addr│");
    println(
        "├─────┼──────────────────┼──────┼─────────────────────────┼────────┼────┼─────┼─────┼─────┼─────┼──────┼─────┤");
    char buffer[256];
    viewInstruction(&C.instruction, C.memAddr, buffer);
    println(buffer);
    println(
        "└─────┴──────────────────┴──────┴─────────────────────────┴────────┴────┴─────┴─────┴─────┴─────┴──────┴─────┘");
    println("                                                     │");
    println("                                                     ▼");
    println(
        "┌────────────────────────────────────────────────────────────────────────────────────────────────────────────┐");
    println(
        "│                                                    RI                                                      │ ");
    println(
        "├─────┬──────────────────┬──────┬─────────────────────────┬────────┬────┬─────┬─────┬─────┬─────┬──────┬─────┤");
    println(
        "│  #  │      Binário     │ Hexa │         Assembly        │  Tipo  │ OP │  RS │  RT │  RD │Funct│  Imm │ Addr│");
    println(
        "├─────┼──────────────────┼──────┼─────────────────────────┼────────┼────┼─────┼─────┼─────┼─────┼──────┼─────┤");
    viewInstruction(&registers.IR, -1, buffer);
    println(buffer);
    println(
        "├─────┴──────────────────┴──────┴─────────────────────────┴────────┴────┴─────┴─────┴─────┴─────┴──────┴─────┤");
    println(
        "│                                                   MDR                                                      │ ");
    println(
        "├────────────────────────────────────────────────────────────────────────────────────────────────────────────┤");
    println(
        "│                                                  %04d                                                      │",
        registers.MDR);
    println(
        "└────────────────────────────────────────────────────────────────────────────────────────────────────────────┘");
    println("                                                     │");
    println("                                                     ▼");



    const Control control = C.control;
    char memToRegBuffer[15];
    snprintf(memToRegBuffer, sizeof(memToRegBuffer), "%s (%d)", memToRegStr[control.memToReg], control.memToReg);
    char memToReg[15];
    centerString(memToRegBuffer, memToReg, 14);

    const int8_t ulaSourceValue = (control.ulaSourceA == 0) ? registers.general[registers.IR.rt] : registers.IR.imm;
    char ulaSourceBuffer[37];
    snprintf(ulaSourceBuffer, sizeof(ulaSourceBuffer), "%s (fonte: %d, valor: %04d)", ulaSourceStr[control.ulaSourceA],
             control.ulaSourceA, ulaSourceValue);
    char ulaSource[37];
    centerString(ulaSourceBuffer, ulaSource, 36);
    println("┌────────────────────────────────────────────────────────────────────────────────────────────────────────────┐");
    println("│"BG_MAGENTA BOLD_WHITE"                                             Decodificação                                                  "RESET"│");
    println("├────────────────────────────────────────────────────────────────────────────────────────────────────────────┤");
    println("│                                               Controle                                                     │");
    println("├──────────┬─────────────────┬─────────────────┬─────────┬─────────┬──────────────┬────────┬─────────┬───────┤");
    println(BOLD_WHITE"│ Fonte PC │   Ula Fonte A   │   Ula Fonte B   │ Esc Reg │ Reg Dst │ Mem para Reg │ Esc IR │ Esc Mem │  IouD │"RESET);
    println(
        "├──────────┼─────────────────┼─────────────────┼─────────┼─────────┼──────────────┼────────┼─────────┼───────┤");
    //  Ula Ctrl │ Esc Reg │  Esc Mem │
    printf(
        "│    %1d     │        %1d        │        %1d        │    %s    │    %1d    │%-13s│    %s   │    %s    │   %1d   │\n",
        control.pcSource,
        control.ulaSourceA,
        control.ulaSourceB,
        boolStr[control.wrtMem ? 1 : 0],
        control.regDst,
        memToReg,
        boolStr[control.wrtIr ? 1 : 0],
        boolStr[control.wrtMem ? 1 : 0],
        control.immOrData
    );


    println("├────────┬─┴──────────┬──────┴───────────┬─────┴─────────┴─────────┴───────┬──────┴────────┴─────────┴───────┤");
    println("│ Branch │   Esc PC   │   Controle ULA   │               Estado            │            Próx. Estado         │");
    println("├────────┼────────────┼──────────────────┼─────────────────────────────────┼─────────────────────────────────┤");
    println("│    %s   │      %s     │        %03d       │                %04d             │               %04d              │",
        boolStr[control.branch ? 1 : 0],
        boolStr[control.wrtPc ? 1 : 0],
        control.ulaControl,
        state,
        control.nextState);
    println("├────────┴────────────┴──────────────────┴─────────────────────────────────┴─────────────────────────────────┤");
    println("│                                            Registradores                                                   │");
    println("├─────────────────────┬──────────────────────────────────────────────────────────────────────────────────────┤");
    println("│       $%01d (A)        │                                          %04d                                        │", registers.IR.rs, C.A);

    println("│       $%01d (B)        │                                          %04d                                        │", registers.IR.rt, C.B);
    println("└─────────────────────┴──────────────────────────────────────────────────────────────────────────────────────┘");

    println("                                                     │");
    println("                                                     ▼");

    println("┌────────────────────────────────────────────────────────────────────────────────────────────────────────────┐");
    println("│                                                    A                                                       │ ");
    println("├────────────────────────────────────────────────────────────────────────────────────────────────────────────┤");
    println("│                                                  %04d                                                      │",registers.A);
    println("├────────────────────────────────────────────────────────────────────────────────────────────────────────────┤");
    println("│                                                    B                                                       │ ");
    println("├────────────────────────────────────────────────────────────────────────────────────────────────────────────┤");
    println("│                                                  %04d                                                      │",registers.B);
    println("└────────────────────────────────────────────────────────────────────────────────────────────────────────────┘");

    println("                                                     │");
    println("                                                     ▼");

    char equalVal[128];
    if (C.control.ulaControl == 6) {
        sprintf(equalVal, "%s", boolStr[C.ULAOut.equal == 0 ? 0 : 1]);
    } else {
        strcpy(equalVal, "-");
    }

    const int8_t input1 = C.control.ulaSourceA == 0 ? (int8_t) pc : registers.A;
    int8_t input2 = 0;
    switch (C.control.ulaSourceB) {
        case 0: {
            input2 = registers.B;
            break;
        }
        case 1: {
            input2 = 1;
            break;
        }
        case 2: {
            input2 = registers.IR.imm;
            break;
        }
        default: break;
    }

    println(
        "┌────────────────────────────────────────────────────────────────────────────────────────────────────────────┐");
    println(
        "│"BG_RED BOLD_WHITE"                                                  ULA                                                       "RESET"│");
    println(
        "├───────────────────────────────────────────────────┬────────────────────────────────────────────────────────┤");
    println(
        "│                    Entrada                        │                          Saída                         │");
    println(
        "├───────────────────────┬───────────────────────────┼────────────────────────────┬───────────────────────────┤");
    println(
        "│     Entrada 1 (A)     │           %04d            │          Resultado         │            %04d           │",
        input1, C.ULAOut.value);
    println(
        "├───────────────────────┼───────────────────────────┼────────────────────────────┼───────────────────────────┤");
    println(
        "│Entrada 2 (B, 1 ou imm)│           %04d            │         Val. Iguais        │              %-1s            │",
        input2, equalVal);
    println(
        "├───────────────────────┼───────────────────────────┼────────────────────────────┼───────────────────────────┤");
    println(
        "│     Controle ULA      │           %04d            │          Overflow          │              %-1s            │",
        C.control.ulaControl, boolStr[C.ULAOut.overflow == 0 ? 0 : 1]);
    println(
        "└────────────────────────────────────────────────────────────────────────────────────────────────────────────┘");

    println("                                                     │");
    println("                                                     ▼");
}
