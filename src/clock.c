#include "clock.h"

#include <stdio.h>
#include <string.h>

#include "back.h"
#include "control.h"
#include "encoding.h"
#include "main.h"
#include "types.h"
#include "ula.h"
#include "view.h"
#include "stats.h" 

// VARIÁVEIS GLOBAIS TEMPORÁRIAS, modificar apos ser feito os registradores de pipeline
static const Instruction *inst_atual = NULL;
static Control ctrl_atual;
static int8_t dado_rs = 0;
static int8_t dado_rt = 0;
static ULAOut resultado_ula;
static int8_t dado_memoria_lido = 0;
static unsigned int reg_escrita_destino = 0;

static char bufferInformation[1000];
static char bufferInformation2[1000];

// Protótipos dos estágios
void estagio_IF();
void estagio_ID();
void estagio_EX();
void estagio_MEM();
void estagio_WB();

void clock() {
    saveLastState();

    bufferInformation[0] = '\0';
    bufferInformation2[0] = '\0';

    //  Executa as 5 etapas separadas 
    
    estagio_IF();
    estagio_ID();
    estagio_EX();
    estagio_MEM();
    estagio_WB();

    // Atualização do PC após a execução da instrução
    if (ctrl_atual.jump) {
        pc = inst_atual->addr;
    } else if (ctrl_atual.branch && resultado_ula.equal) {
        pc = pc + 1 + inst_atual->imm;
    } else {
        pc++;
    }

   
    if (inst_atual != NULL && inst_atual->type != OTHER) {
        // Mostra o quadro de Controle e da ULA para a instrução atual
        showClock(inst_atual, &ctrl_atual);
        int8_t operando2 = ctrl_atual.ulaSource ? inst_atual->imm : dado_rt;
        showClockUla(dado_rs, operando2, ctrl_atual.ulaControl, &resultado_ula);
    }

    
    showClockPc();
    showClockInformation(bufferInformation, bufferInformation2);
    
    
   
}

// IMPLEMENTAÇÃO DOS ESTÁGIOS 

void estagio_IF() {
    inst_atual = (pc < memInstruction.size) ? &memInstruction.instructions[pc] : &emptyInstruction;
    
    
    computeInstructionStats(inst_atual);
}

void estagio_ID() {
    if (inst_atual == NULL || inst_atual->type == OTHER) return;

    ctrl_atual = makeControl(inst_atual);
    dado_rs = registers[inst_atual->rs];
    dado_rt = registers[inst_atual->rt];
    reg_escrita_destino = ctrl_atual.regDst ? inst_atual->rd : inst_atual->rt;
}

void estagio_EX() {
    if (inst_atual == NULL || inst_atual->type == OTHER) return;

    int8_t operando2 = ctrl_atual.ulaSource ? inst_atual->imm : dado_rt;
    resultado_ula = ula(dado_rs, operando2, ctrl_atual.ulaControl);
}

void estagio_MEM() {
    if (inst_atual == NULL || inst_atual->type == OTHER) return;

    if (ctrl_atual.wrtMem) {
        if (resultado_ula.value >= 0 && resultado_ula.value < 256) {
            memData.data[resultado_ula.value] = dado_rt;
            if (resultado_ula.value >= memData.size) {
                memData.size = resultado_ula.value + 1;
            }
        }
        sprintf(bufferInformation, " Escrita na memoria no endereco: %04d o valor do registrador $%1d (valor: %04d).", resultado_ula.value, inst_atual->rt, dado_rt);
    } 
    else if (inst_atual->opcode == LW_OPCODE) {
        if (resultado_ula.value >= 0 && resultado_ula.value < 256) {
            dado_memoria_lido = memData.data[resultado_ula.value];
        }
        sprintf(bufferInformation, " Leitura da memoria no endereco: %04d (valor lido: %04d) e preparado para o registrador $%1d.", resultado_ula.value, dado_memoria_lido, inst_atual->rt);
    }
}

void estagio_WB() {
    if (inst_atual == NULL || inst_atual->type == OTHER) return;

    if (ctrl_atual.wrtReg) {
        int8_t valor_final = ctrl_atual.memToReg ? resultado_ula.value : dado_memoria_lido;
        registers[reg_escrita_destino] = valor_final;

        if (inst_atual->opcode == R_TYPE_OPCODE) {
            switch (inst_atual->funct) {
                case ADD_FUNCT:
                    sprintf(bufferInformation, " Operação ADD (soma) do registrador %1d (valor: %04d) com o registrador: %1d (valor: %04d).", inst_atual->rs, dado_rs, inst_atual->rt, dado_rt);
                    sprintf(bufferInformation2, " Resultado escrito no registrador %1d (resultado: %04d).", reg_escrita_destino, valor_final);
                    break;
                case SUB_FUNCT:
                    sprintf(bufferInformation, " Operação SUB (subtração) do registrador %1d (valor: %04d) com o registrador: %1d (valor: %04d).", inst_atual->rs, dado_rs, inst_atual->rt, dado_rt);
                    sprintf(bufferInformation2, " Resultado escrito no registrador %1d (resultado: %04d).", reg_escrita_destino, valor_final);
                    break;
                case AND_FUNCT:
                    sprintf(bufferInformation, " Operação AND (e / &) do registrador %1d (valor: %04d) com o registrador: %1d (valor: %04d).", inst_atual->rs, dado_rs, inst_atual->rt, dado_rt);
                    sprintf(bufferInformation2, " Resultado escrito no registrador %1d (resultado: %04d).", reg_escrita_destino, valor_final);
                    break;
                case OR_FUNCT:
                    sprintf(bufferInformation, " Operação OR (ou / |) do registrador %1d (valor: %04d) com o registrador: %1d (valor: %04d).", inst_atual->rs, dado_rs, inst_atual->rt, dado_rt);
                    sprintf(bufferInformation2, " Resultado escrito no registrador %1d (resultado: %04d).", reg_escrita_destino, valor_final);
                    break;
                default: break;
            }
        } else if (inst_atual->opcode != LW_OPCODE) {
            sprintf(bufferInformation, " Executada operacao na ULA e escrito no registrador: $%1d o valor: %04d.", reg_escrita_destino, valor_final);
        }
    }
}