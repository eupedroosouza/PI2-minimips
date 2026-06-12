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

// Acessa o registrador de pipeline global definido no projeto
extern PipelineRegisters pipeline;


// VARIÁVEIS GLOBAIS qnd for terminado a implementaçao dos registradores de pipeline, essas ficaram obsoletas
static const Instruction *inst_atual = NULL;
static Control ctrl_atual;
static int8_t dado_rs = 0;
static int8_t dado_rt = 0;
static ULAOut resultado_ula;
static int8_t dado_memoria_lido = 0;
static unsigned int reg_escrita_destino = 0;

static char bufferInformation[1000];
static char bufferInformation2[1000];

void estagio_IF();
void estagio_ID();
void estagio_EX();
void estagio_MEM();
void estagio_WB();

void clock() {
    saveLastState();

    bufferInformation[0] = '\0';
    bufferInformation2[0] = '\0';

    
    estagio_WB();   
    estagio_MEM();  
    estagio_EX();   
    estagio_ID();   
    estagio_IF();   

   // tem q modifica dpois  n fiz isso pq precisa q todo o clock ja esteja utilizando os registradpres de pipeline, provavel q de erro
    if (inst_atual != NULL && inst_atual->type != OTHER) {
        if (ctrl_atual.jump) {
            pc = inst_atual->addr;
        } else if (ctrl_atual.branch && resultado_ula.equal) {
            pc = pc + inst_atual->imm;
        }
        showClock(inst_atual, &ctrl_atual);
    }

    showClockPc();
    showClockInformation(bufferInformation, bufferInformation2);
}



void estagio_IF() {
    // Busca a instrução na memória utilizando o PC atual
    const Instruction *inst = (pc < memory.size) ? &memory.instructions[pc] : &emptyInstruction;
    
    // manda a intruçao pros registradores de pipeline 
    pipeline.if_id.IR = *inst;
    pipeline.if_id.pc = pc; 

    if (inst->type != OTHER) {
        computeInstructionStats(inst);
    }

    // Incrementa o PC para a próxima busca
    pc++;
}

void estagio_ID() {
    // le os registradores de pipeline q vieram do if
    Instruction inst = pipeline.if_id.IR;

    if (inst.type == OTHER) {
        // Se for uma instrução inválida limpa o registrador de saída para evitar lixo no EX
        pipeline.id_ex.IR = emptyInstruction;
        pipeline.id_ex.A = 0;
        pipeline.id_ex.B = 0;
        pipeline.id_ex.imm = 0;
        return;
    }

    
  
    pipeline.id_ex.IR = inst;
    pipeline.id_ex.A = registers[inst.rs]; // Valor lido de RS
    pipeline.id_ex.B = registers[inst.rt]; // Valor lido de RT
    pipeline.id_ex.imm = inst.imm;         // Valor imediato estendido
    
  
}



void estagio_EX() {
   
    if (inst_atual == NULL || inst_atual->type == OTHER) return;

    int8_t operando2 = ctrl_atual.ulaSource ? inst_atual->imm : dado_rt;
    resultado_ula = ula(dado_rs, operando2, ctrl_atual.ulaControl);
    
    reg_escrita_destino = ctrl_atual.regDst ? inst_atual->rd : inst_atual->rt;
}

void estagio_MEM() {

 if (inst_atual == NULL || inst_atual->type == OTHER) return;

    if (ctrl_atual.wrtMem) {
        if (resultado_ula.value >= 0 && resultado_ula.value < 256) {
            memory.data[resultado_ula.value] = dado_rt;
            if (resultado_ula.value >= memory.MemDatasize) {
                memory.MemDatasize = resultado_ula.value + 1;
            }
        }
        sprintf(bufferInformation2, " [MEM] Escrita no endereco: %04d o valor: %04d.", resultado_ula.value, dado_rt);
    } 
    else if (pipeline.mem_wb.IR.opcode == LW_OPCODE) {
        if (resultado_ula.value >= 0 && resultado_ula.value < 256) {
            dado_memoria_lido = memory.data[resultado_ula.value];
        }
        sprintf(bufferInformation2, " [MEM] Leitura no endereco: %04d (lido: %04d).", resultado_ula.value, dado_memoria_lido);
    }

    pipeline.mem_wb.IR = pipeline.ex_mem.IR; // recebe instrução do registrador anterior
    pipeline.mem_wb.memData = dado_memoria_lido;
    pipeline.mem_wb.ulaOut = pipeline.ex_mem.ulaOut;
    pipeline.mem_wb.ctrl = pipeline.ex_mem.ctrl;
    pipeline.mem_wb.reg_escrita_destino = pipeline.ex_mem.reg_escrita_destino;
}

void estagio_WB() {

    
    if (inst_atual == NULL || inst_atual->type == OTHER) return;

    if (pipeline.mem_wb.ctrl.wrtReg) {
    int8_t valor_final =  pipeline.mem_wb.ctrl.memToReg ? pipeline.mem_wb.memData : pipeline.mem_wb.ulaOut;        
    
    registers[pipeline.mem_wb.reg_escrita_destino] = valor_final;

        if (inst_atual->opcode == R_TYPE_OPCODE) {
            sprintf(bufferInformation, " [WB] Escrito no registrador $%1d o resultado: %04d.", reg_escrita_destino, valor_final);
        } else if (inst_atual->opcode != LW_OPCODE) {
            sprintf(bufferInformation, " [WB] Executada op na ULA e escrito no registrador: $%1d o valor: %04d.", reg_escrita_destino, valor_final);
        } else {
            sprintf(bufferInformation, " [WB] Escrito no registrador $%1d o valor carregado: %04d.", reg_escrita_destino, valor_final);
        }
    }

}
