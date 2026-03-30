#include "view.h"

#include <stdio.h>

#include "debugger.h"
#include "main.h"
#include "utils.h"

const char *memToRegStr[2] = {"mem", "ula"};
const char *ulaSourceStr[2] = {"register", "immediate"};

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
    debugInstruction(instruction, pc, buffer);
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
        "│        Input 1        │           %04d            │            Value           │            %04d           │", input1, out->value);
    println(
        "├───────────────────────┼───────────────────────────┼────────────────────────────┼───────────────────────────┤");
    println(
        "│        Input 2        │           %04d            │           ZeroUla          │              %-1s            │", input2, boolStr[out->zeroUla == 0 ? 0 : 1]);
    println(
        "├───────────────────────┼───────────────────────────┼────────────────────────────┼───────────────────────────┤");
    println(
        "│      Ula Control      │           %04d            │              -             │              -            │", ulaControl);
    println(
        "├───────────────────────┴───────────────────────────┴────────────────────────────┴───────────────────────────┤");
}
