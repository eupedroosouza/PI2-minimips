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

typedef struct {
    int8_t value;
    bool equal;
    bool overflow;
} ULAOut;



// --- REGISTRADORES INTERMEDIÁRIOS (PIPELINE) ---

typedef struct {
    Instruction IR;        
    uint8_t PCP1; // pc plus 1 (pc + 1)
} IF_ID; // busca de instrução

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
    int8_t A;
    int8_t B;
    uint8_t RT;
    uint8_t RD;
    int8_t imm;             
    uint8_t PCP1;             // PC que veio do IF_ID necessário para o cálculo do BEQ
    Control ctrl;           // Sinais de controle da instrução
} ID_EX; // decodificação da instrução

typedef struct {
    int8_t ulaOut;
    int8_t B;
    uint8_t RD;
    Control ctrl;           // Sinais de controle para as fases de MEM e WB
} EX_MEM; // acesso à memória de dados

typedef struct {
    int8_t MEM;
    int8_t ulaOut;          
    Control ctrl;           //Sinais de controle para a fase de WB
    uint8_t RD;
} MEM_WB; // escrita no banco de registradores

typedef struct {
    IF_ID IF;
    ID_EX ID;
    EX_MEM EX_MEM;
    MEM_WB MEM_WEB;
} PipelineRegisters; // Variável global unificada

// Estado (útil para a função de back)
typedef struct state {
    struct state *previous;
    PC pc;
    Register registers[8];
    PipelineRegisters pipeline;
    MemData memData;
} BackState;

typedef struct  {
    PC IF_PC;
    PC IF_PCP1;
    Control ID_control;
    bool ID_IncPC;
    int8_t ID_A;
    int8_t ID_B;
    int8_t ID_imm;
    uint8_t ID_PCP1;
    uint8_t ID_RT;
    uint8_t ID_RD;
    ULAOut EX_ulaOut;
    int8_t EX_B;
    uint8_t EX_RD;
    int8_t MEM_MEM;
    int8_t MEM_ulaOut;
    uint8_t MEM_RD;
    int8_t WB_DATA;
} CombinationalState;