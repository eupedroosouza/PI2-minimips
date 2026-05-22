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
#define REG_SIZE 8

typedef uint8_t PC;
typedef int8_t Register;

typedef enum {
    I, J, R, OTHER
} InstructionType;

typedef struct {
    char stringedInstruction[17]; // Bits carregados do .mem transformados em string
    char hexa[255]; // Hexadecimal em hexadecimal  (útil para comparação com outros simuladores com o Logisim)
    char asmInstruction[255]; // Instrução em assembly (mnemônicos) para salvar no .asm
    char prettyAsmInstruction[512];
    InstructionType type;
    unsigned int opcode;
    unsigned int rs;
    unsigned int rt;
    unsigned int rd;
    unsigned int funct;
    int8_t imm;
    uint8_t addr;
    int8_t data; //memoria de dados
} Instruction;

// 2^8 = 256
typedef struct {
    Instruction instructions[MEM_SIZE];
    uint8_t size; // tamanho da memória de instrução
    uint8_t dataSize; // tamanho da memória de dados
} Memory; // Memória geral

// registradores intermediários
typedef struct {
    Instruction IR; // reg de memória de instrução
    Register MDR; // reg de dados da memória
    Register A, B; // regs A e B, valores saindo do banco de registradores para a ULA
    Register ULAOut; // reg de saida da ULA
    Register general[8]; // regs de propósito geral
} Registers;

// Estado (útil para a função de back)
typedef struct state {
    struct state *previous;
    PC pc;
    Registers registers;
    int state;
    Memory memory;
} BackState;

typedef struct {
    int pcSource;
    int ulaSourceB;
    int ulaSourceA;
    bool wrtReg;
    int regDst;
    int memToReg;
    bool wrtIr;
    bool wrtMem;
    int immOrData;
    bool branch;
    bool wrtPc;
    int ulaControl;
    // misc
    int nextState; // only for view
} Control;

typedef struct {
    int8_t value;
    bool equal;
    bool overflow;
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

typedef struct {
    // Load
    Instruction instruction;
    int8_t MDR;
    // Decode
    int8_t A;
    int8_t B;
    Control control; // = control based in actual state of machine
    // ULA
    int8_t input1;
    int8_t input2;
    ULAOut ULAOut;
    // Memory Access (and PCSource)
    int16_t pc; // use 16 bit to prevents PC overflow (you need transform in an int8_t or uint8_t again before the use)
    int8_t memAddr;
    int8_t memData;
    // Register Write
    unsigned int regToWrite;
    int8_t regWriteData;
    // Misc
    bool wrtPc;
} Combinational;
