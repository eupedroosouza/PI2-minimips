#pragma once

#include <stdint.h>
#include <stdbool.h>

#define R_TYPE_OPCODE 0
#define ADD_FUNCT     0
#define SUB_FUNCT     2
#define AND_FUNCT     4
#define OR_FUNCT      5
#define ADDI_OPCODE   4
#define LW_OPCODE    11
#define SW_OPCODE    15
#define BEQ_OPCODE    8
#define J_OPCODE      2

#define MEM_SIZE 256

typedef uint8_t PC;
typedef int8_t Register;

typedef enum {
    I, J, R, OTHER
} InstructionType;

typedef struct {
    char stringedInstruction[17]; // Bits carregados do .mem transformados em string
    char asmInstruction[255]; // Instrução em assembly (mnemônicos) para salvar no .asm
    InstructionType type;
    unsigned int opcode;
    unsigned int rs;
    unsigned int rt;
    unsigned int rd;
    unsigned int funct;
    int8_t imm;
    uint8_t addr;
} Instruction;

// 2^8 = 256
typedef struct {
    Instruction instructions[256];
    uint8_t size;
} MemInstruction;

typedef struct {
    int8_t data[256];
    int size;
} MemData;

// Estado (útil para a função de back)
typedef struct {
    PC pc;
    Register registers[8];
    MemData memData;
} State;

typedef struct {
    bool jump;
    bool branch;
    int regDst;
    int ulaSource;
    int memToReg;
    bool wrtReg;
    bool wrtMem;
    int ulaControl;
} Control;

typedef struct {
    bool zeroUla;
    int8_t value;
} ULAOut;

// Estrutura para contar quantas vezes CADA INSTRUÇÃO ESPECÍFICA foi executada.
typedef struct {
    int lw;
    int sw;
    int add;
    int addi;
    int sub;
    int and_inst; 
    int or_inst;
    int beq;
    int j;
    int other; 
} StatisticsPerClass;

// Estrutura para contar quantas instruções de CADA FORMATO (Tipo) rodaram.
typedef struct {
    int i; // lw, sw, beq, addi
    int j; // Jump
    int r; // add, sub, and, or
    int other;
} StatisticsPerType;


typedef struct {
    int executedInstructions; // Contador global de ciclos (1 ciclo = 1 instrução)
    StatisticsPerClass executedInstructionsPerClass;
    StatisticsPerType executedInstructionsPerType;
} Statistics;
