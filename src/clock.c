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
// extern PipelineRegisters pipeline;


// VARIÁVEIS GLOBAIS qnd for terminado a implementaçao dos registradores de pipeline, essas ficaram obsoletas
// static const Instruction *inst_atual = NULL;
// static Control ctrl_atual;
// static int8_t dado_rs = 0;
// static int8_t dado_rt = 0;
// static ULAOut resultado_ula;
// static int8_t dado_memoria_lido = 0;
// static unsigned int reg_escrita_destino = 0;

void estagio_IF(PipelineRegisters *pipeline);

void estagio_ID(PipelineRegisters *pipeline);

void estagio_EX(PipelineRegisters *pipeline);

void estagio_MEM(PipelineRegisters *pipeline);

void estagio_WB(PipelineRegisters *pipeline);

void clock(PipelineRegisters *pipeline) {
    saveLastState();

    estagio_WB(pipeline);
    estagio_MEM(pipeline);
    estagio_EX(pipeline);
    estagio_ID(pipeline);
    estagio_IF(pipeline);
}

// Busca a instrução na memória utilizando o PC atual
void estagio_IF(PipelineRegisters *pipeline) {
    printf("<< start if >>\n");
    // manda a intruçao pros registradores de pipeline
    const Instruction IF = pipeline->IF.IR;
    printf("current if_ir: %s\n", pipeline->IF.IR.asmInstruction);
    const Instruction *inst = &memInstruction.instructions[pc];
    printf("pc: %d\n", pc);
    printf("new if_ir: %s\n", inst->asmInstruction);
    pipeline->IF.IR = *inst;
    pipeline->IF.PCP1 = pc + 1;
    printf("pc + 1: %d\n", pipeline->IF.PCP1);

    if (inst->type != OTHER) {
        computeInstructionStats(inst);
    }

    // let's define new pc
    // select calculated branch address or pc + 1 (pcp1)
    // hmmmm, call ULA it's necessary? seems so, to anticipate
    const int8_t operando2 = pipeline->ID.ctrl.ulaSource ? pipeline->ID.imm : pipeline->ID.B;
    const ULAOut resultado = ula(pipeline->ID.A, operando2, pipeline->ID.ctrl.ulaControl);
    const int branch = pipeline->EX_MEM.ctrl.branch && resultado.equal;
    printf("select a branch: %d\n", branch);
    if (branch == 0) {
        pc = pc + 1;
    } else if (branch == 1) {
        pc = pipeline->ID.imm + pipeline->ID.PCP1;
    }
    // select calculated address based on branch address or pc+1 and or even jump
    printf("select a jump: %d\n", pipeline->EX_MEM.ctrl.jump);
    if (pipeline->EX_MEM.ctrl.jump) {
        pc = IF.addr;
    }
    printf("new pc: %d\n", pc);
    printf("<< end if >>\n");
}

void estagio_ID(PipelineRegisters *pipeline) {
    printf("<< start id >>\n");
    // le os registradores de pipeline q vieram do if
    // sinais são criados p/ a instrução que acabou de ser decodificada.
    const Instruction inst = pipeline->IF.IR;
    printf("decoding inst %s\n", inst.asmInstruction);
    pipeline->ID.A = registers[inst.rs]; // Valor lido de RS
    pipeline->ID.B = registers[inst.rt]; // Valor lido de RT
    pipeline->ID.RS = inst.rs;
    pipeline->ID.RD = inst.rd;
    pipeline->ID.imm = inst.imm; // Valor imediato estendido
    pipeline->ID.PCP1 = pipeline->IF.PCP1;
    pipeline->ID.ctrl = makeControl(&inst); // sinal de controle armazenado no pipeline

    printf("<< end id >>\n");
}


void estagio_EX(PipelineRegisters *pipeline) {
    printf("<< start ex >>\n");

    const int8_t operando2 = pipeline->ID.ctrl.ulaSource ? pipeline->ID.imm : pipeline->ID.B;
    const ULAOut resultado = ula(pipeline->ID.A, operando2, pipeline->ID.ctrl.ulaControl);

    printf("ula source op 2 is %d and value %d", pipeline->ID.ctrl.ulaSource, operando2);
    printf("ula result is %d and equal %d", resultado.value, resultado.equal);

    pipeline->EX_MEM.ulaOut = resultado.value;
    pipeline->EX_MEM.B = pipeline->ID.B;
    pipeline->EX_MEM.RD = pipeline->ID.ctrl.regDst ? pipeline->ID.RS : pipeline->ID.RD;
    printf("rd mux is %d and value %d ", pipeline->ID.ctrl.regDst,
           (pipeline->ID.ctrl.regDst ? pipeline->ID.RS : pipeline->ID.RD));
    pipeline->EX_MEM.ctrl = pipeline->ID.ctrl; // sinal de controle

    printf("<< end ex >>\n");
}

// acessa a memória de dados
void estagio_MEM(PipelineRegisters *pipeline) {
    // SW

    printf("<< start mem >>\n");
    printf("wrt mem:  %d\n", pipeline->MEM_WEB.ctrl.wrtMem);
    if (pipeline->EX_MEM.ctrl.wrtMem) {
        printf("written to mem %d on address %d'\n", pipeline->EX_MEM.B, pipeline->EX_MEM.ulaOut);
        memData.data[pipeline->EX_MEM.ulaOut] = pipeline->EX_MEM.B;
    }
    // FIM DO SW

    // Salva nos registradores do pipeline MEM_WB o que será necessário para o próximo estágio
    pipeline->MEM_WEB.MEM = memData.data[pipeline->EX_MEM.ulaOut];
    printf("read %d from address: %d", memData.data[pipeline->EX_MEM.ulaOut], pipeline->EX_MEM.ulaOut);
    pipeline->MEM_WEB.ulaOut = pipeline->EX_MEM.ulaOut;
    pipeline->MEM_WEB.ctrl = pipeline->EX_MEM.ctrl; // sinal de controle
    pipeline->MEM_WEB.RD = pipeline->EX_MEM.RD;

    printf("<< end mem >>\n");
}


// escreve resultado final no banco de registradores
void estagio_WB(PipelineRegisters *pipeline) {
    printf("<< start wb >>\n");
    printf("wrt reg:  %d\n", pipeline->MEM_WEB.ctrl.wrtReg);

    // executa WB
    if (pipeline->MEM_WEB.ctrl.wrtReg) {
        // se a função do estagio anterior escreve nos regs

        // Escolhe qual valor será escrito
        // Se memToReg = 0: usa o valor vindo da memória (lw).
        // Se memToReg = 1: usa o resultado da ULA (add, sub, addi, etc.).
        registers[pipeline->MEM_WEB.RD] = pipeline->MEM_WEB.ctrl.memToReg
                                              ? pipeline->MEM_WEB.ulaOut
                                              : pipeline->MEM_WEB.MEM;
        printf("written %d from %d on reg dst %d\n",
               (pipeline->MEM_WEB.ctrl.memToReg ? pipeline->MEM_WEB.ulaOut : pipeline->MEM_WEB.MEM),
               pipeline->MEM_WEB.ctrl.memToReg, pipeline->MEM_WEB.RD);
    }
    printf("<< end wb >>\n");
}
