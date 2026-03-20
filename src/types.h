#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef uint8_t PC;
typedef int8_t Register;

enum InstructionType {
    I, J, R, OTHER
};

typedef struct {
    char stringedInstruction[17]; // Bits carregados do .mem transformados em string
    char *asmInstruction; // Instrução em assembly (mnemônicos) para salvar no .asm
    enum InstructionType type;
    int opcode;
    int rs;
    int rt;
    int rd;
    int funct;
    int8_t imm; // 8-bits com sinal
    int addr;
} Instruction;

// 2^8 = 256
typedef struct {
    char instructions[256][16];
    uint8_t size;
} MemInstruction;

typedef struct {
    int8_t data[256];
    uint8_t size;
} MemData;

// Estado (útil para a função de back)
typedef struct {
    PC pc;
    Register registers[8];
    MemInstruction memInstruction;
    MemData memData;
} State;

typedef struct {
    bool jump;
    bool branch;
    int regDst;
    int ulaSource;
    bool memToReg;
    bool wrtReg;
    bool wrtMem;
    int ulaControl;
} Control;
