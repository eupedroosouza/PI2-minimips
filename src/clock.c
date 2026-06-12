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
// static const Instruction *inst_atual = NULL;
// static Control ctrl_atual;
// static int8_t dado_rs = 0;
// static int8_t dado_rt = 0;
// static ULAOut resultado_ula;
// static int8_t dado_memoria_lido = 0;
// static unsigned int reg_escrita_destino = 0;

static char bufferInformation[1000];
static char bufferInformation2[1000];

void estagio_IF();
void estagio_ID();
void estagio_EX();
void estagio_MEM();
void estagio_WB();

void clock() {
    saveLastState();

    // bufferInformation[0] = '\0';
    // bufferInformation2[0] = '\0';

    // execução dos estágios em ordem. Cada estágio lê os dados do ciclo anterior antes de serem sobrescritos
    estagio_WB();   
    estagio_MEM();  
    estagio_EX();   
    estagio_ID();   
    estagio_IF();   

   // tem q modifica dpois  n fiz isso pq precisa q todo o clock ja esteja utilizando os registradpres de pipeline, provavel q de erro
    // if (pipeline.mem_wb.IR != NULL && inst_atual->type != OTHER) {
    //     if (ctrl_atual.jump) {
    //         pc = inst_atual->addr;
    //     } else if (ctrl_atual.branch && resultado_ula.equal) {
    //         pc = pc + inst_atual->imm;
    //     }
    //     showClock(inst_atual, &ctrl_atual);
    // }

    showClockPc();
    showClockInformation(bufferInformation, bufferInformation2);
}

// Busca a instrução na memória utilizando o PC atual
void estagio_IF() {
 
    const Instruction *inst = &memInstruction.instructions[pc];    
    
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
    // sinais são criados p/ a instrução que acabou de ser decodificada.
    Instruction inst = pipeline.if_id.IR;
    Control ctrl = makeControl(&inst); // sinal de controle, substituindo sinal global
    
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
    pipeline.id_ex.pc = pipeline.if_id.pc;
    pipeline.id_ex.ctrl = ctrl;            // sinal de controle armazenado no pipeline

  
}



void estagio_EX() {

    if (pipeline.id_ex.IR.type == OTHER)
        return;

    int8_t operando2 =
        pipeline.id_ex.ctrl.ulaSource
            ? pipeline.id_ex.imm
            : pipeline.id_ex.B;

    ULAOut resultado =
        ula(
            pipeline.id_ex.A,
            operando2,
            pipeline.id_ex.ctrl.ulaControl
        );

    pipeline.ex_mem.IR = pipeline.id_ex.IR;
    pipeline.ex_mem.ulaOut = resultado.value;
    pipeline.ex_mem.B = pipeline.id_ex.B;
    pipeline.ex_mem.ula_equal = resultado.equal;
    pipeline.ex_mem.ctrl = pipeline.id_ex.ctrl; // sinal de controle

    pipeline.ex_mem.reg_escrita_destino =
        pipeline.id_ex.ctrl.regDst
            ? pipeline.id_ex.IR.rd
            : pipeline.id_ex.IR.rt;
}

// acessa a memória de dados
void estagio_MEM() {

int8_t dado_memoria_lido = 0; // variável temporária

 if (pipeline.ex_mem.IR.type == OTHER) return;

    // SW
    if (pipeline.ex_mem.ctrl.wrtMem) {  // se o estado anterior manda sinal para salvar na memória

        if (pipeline.ex_mem.ulaOut >= 0 && pipeline.ex_mem.ulaOut < 256) { // condição para salvar
            memdata.data[pipeline.ex_mem.ulaOut] = pipeline.ex_mem.B; // grava na memória de dados o valor armazenado em B

            if (pipeline.ex_mem.ulaOut >= memdata.size) { // tamanho da memória expande para guardar nova posição
                memdata.size = pipeline.ex_mem.ulaOut + 1;
            }
        }
        sprintf(bufferInformation2, " [MEM] Escrita no endereco: %04d o valor: %04d.", pipeline.ex_mem.ulaOut, pipeline.ex_mem.B);
    } 
    // FIM DO SW

    // LW
    else if (pipeline.ex_mem.IR.opcode == LW_OPCODE) {
        if (pipeline.ex_mem.ulaOut >= 0 && pipeline.ex_mem.ulaOut < 256) {
            dado_memoria_lido = memdata.data[pipeline.ex_mem.ulaOut]; // Lê da memória o valor no endereço ulaOut e guarda em dado_memoria_lido.
        }
        sprintf(bufferInformation2, " [MEM] Leitura no endereco: %04d (lido: %04d).", pipeline.ex_mem.ulaOut, dado_memoria_lido);
    }
    // FIM DO LW

    // Salva nos registradores do pipeline MEM_WB o que será necessário para o próximo estágio
    pipeline.mem_wb.IR = pipeline.ex_mem.IR; // recebe instrução do registrador anterior
    pipeline.mem_wb.memData = dado_memoria_lido;
    pipeline.mem_wb.ulaOut = pipeline.ex_mem.ulaOut;
    pipeline.mem_wb.ctrl = pipeline.ex_mem.ctrl; // sinal de controle
    pipeline.mem_wb.reg_escrita_destino = pipeline.ex_mem.reg_escrita_destino;
}


// escreve resultado final no banco de registradores
void estagio_WB() {

    
    if (pipeline.mem_wb.IR.type == OTHER) return;

    // executa WB
    if (pipeline.mem_wb.ctrl.wrtReg) { // se a função do estagio anterior escreve nos regs

    // Escolhe qual valor será escrito
    // Se memToReg = 1: usa o valor vindo da memória (lw).
    // Se memToReg = 0: usa o resultado da ULA (add, sub, addi, etc.).
    int8_t valor_final =  pipeline.mem_wb.ctrl.memToReg ? pipeline.mem_wb.memData : pipeline.mem_wb.ulaOut; 
    
    // grava o valor final no banco de registradores
    registers[pipeline.mem_wb.reg_escrita_destino] = valor_final;

        if (pipeline.mem_wb.IR.opcode == R_TYPE_OPCODE) {
            sprintf(bufferInformation, " [WB] Escrito no registrador $%1d o resultado: %04d.", pipeline.mem_wb.reg_escrita_destino, valor_final);
        } else if (pipeline.mem_wb.IR.opcode != LW_OPCODE) {
            sprintf(bufferInformation, " [WB] Executada op na ULA e escrito no registrador: $%1d o valor: %04d.", pipeline.mem_wb.reg_escrita_destino, valor_final);
        } else {
            sprintf(bufferInformation, " [WB] Escrito no registrador $%1d o valor carregado: %04d.", pipeline.mem_wb.reg_escrita_destino, valor_final);
        }
    }

}
