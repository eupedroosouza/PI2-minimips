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
    int8_t data[256]; // mem de dados
    int MemDatasize;   // tamanho da mem de dados
} Memory;



// --- REGISTRADORES INTERMEDIÁRIOS (PIPELINE) ---

typedef struct {
    Instruction IR;        
    uint8_t pc;            
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
    Instruction IR;        
    int8_t A;               
    int8_t B;               
    int8_t imm;             
    uint8_t pc;             // PC que veio do IF_ID necessário para o cálculo do BEQ
    Control ctrl;           // Sinais de controle da instrução
} ID_EX; // decodificação da instrução

typedef struct {
    Instruction IR;        
    int8_t ulaOut;          
    int8_t B;              
    bool ula_equal;         // Guarda a flag equal  para o BEQ
    Control ctrl;           // Sinais de controle para as fases de MEM e WB
    unsigned int reg_escrita_destino; //Lembra em qual registrador salvar no final
} EX_MEM; // acesso à memória de dados

typedef struct {
    Instruction IR;        
    int8_t memData;         
    int8_t ulaOut;          
    Control ctrl;           //Sinais de controle para a fase de WB
    unsigned int reg_escrita_destino; //Lembra em qual registrador salvar no final
} MEM_WB; // escrita no banco de registradores

typedef struct {
    IF_ID if_id;
    ID_EX id_ex;
    EX_MEM ex_mem;
    MEM_WB mem_wb;
} PipelineRegisters; // Variável global unificada

// Estado (útil para a função de back)
typedef struct state {
    struct state *previous;
    PC pc;
    Register registers[8];
    PipelineRegisters pipelineRegisters;
    Memory memData;
} BackState;

typedef struct {
    int8_t value;
    bool equal;
    bool overflow;
} ULAOut;
