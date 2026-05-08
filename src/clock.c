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

// Variável global da Unidade de Controle.
// Mantém os estados salvos entre as chamadas da função (Pulsos de Clock)
Control control = {0}; 

void clock() {
    saveLastState();

    int currentState = control.state; // Guarda qual estado estamos executando AGORA

    // 1. Busca da Instrução
    // Como o PC só aumenta no final, ele vai apontar para a mesma instrução 
    // durante todos os 3 a 5 ciclos necessários para ela terminar!
    const Instruction *instruction = (pc < memInstruction.size) ? &memInstruction.instructions[pc] : &emptyInstruction;

    // 2. Atualiza a Unidade de Controle
    // Isso vai gerar as flags do ciclo atual e avançar o "control.state" pro próximo ciclo
    makeControl(instruction->opcode, instruction->funct, &control);

    // --- Coleta de Estatísticas Multiciclo ---
    stats.totalCycles++; // Incrementa 1 ciclo de clock
    
    // Contabiliza a instrução APENAS na primeira vez que passa por ela (Estado 0)
    if (currentState == 0 && instruction->type != OTHER) {
        stats.executedInstructions++;

        if (instruction->type == R) stats.executedInstructionsPerType.r++;
        else if (instruction->type == I) stats.executedInstructionsPerType.i++;
        else if (instruction->type == J) stats.executedInstructionsPerType.j++;
        else stats.executedInstructionsPerType.other++;

        if (instruction->opcode == R_TYPE_OPCODE) {
            switch (instruction->funct) {
                case ADD_FUNCT: stats.executedInstructionsPerClass.add++; break;
                case SUB_FUNCT: stats.executedInstructionsPerClass.sub++; break;
                case AND_FUNCT: stats.executedInstructionsPerClass.and_inst++; break;
                case OR_FUNCT:  stats.executedInstructionsPerClass.or_inst++; break;
            }
        } else {
            switch (instruction->opcode) {
                case LW_OPCODE: stats.executedInstructionsPerClass.lw++; break;
                case SW_OPCODE: stats.executedInstructionsPerClass.sw++; break;
                case ADDI_OPCODE: stats.executedInstructionsPerClass.addi++; break;
                case BEQ_OPCODE: stats.executedInstructionsPerClass.beq++; break;
                case J_OPCODE: stats.executedInstructionsPerClass.j++; break;
            }
        }
    }

    // 3. Preparar Entradas da ULA (Muxes A e B)
    int8_t input1 = (control.ulaSourceA == 0) ? pc : registers[instruction->rs];
    int8_t input2;
    if (control.ulaSourceB == 0)      input2 = registers[instruction->rt];
    else if (control.ulaSourceB == 1) input2 = 1; // Geralmente usado para somar PC+1 (caso use)
    else                              input2 = instruction->imm; // Extensão de sinal do Imediato

    // 4. Executar ULA
    // Se for Tipo-R (estado 7), a ULA faz a operação dita pelo 'funct'. Senão, usa a da Unidade de Controle
    int ulaOp = (currentState == 7) ? instruction->funct : control.ulaControl;
    ULAOut out = ula(input1, input2, ulaOp);

    // 5. Escrita em Registradores (Write-Back) - Protegido pela flag wrtReg
    if (control.wrtReg) {
        int destination = (control.regDst == 1) ? instruction->rd : instruction->rt;
        int value = (control.memToReg == 1) ? memInstruction.data[out.value] : out.value;
        registers[destination] = value;
    }

    // 6. Escrita na Memória de Dados (Store Word) - Protegido pela flag wrtMem
    if (control.wrtMem) {
        memInstruction.data[out.value] = registers[instruction->rt];
    }

    // 7. Atualização do PC - Agora o PC só pula de linha nos últimos estados!
    if (control.wrtPc) {
        if (control.pcSource == 0)      pc = pc + 1; // Avança +1 para a próxima instrução
        else if (control.pcSource == 2) pc = instruction->addr; // Salto de Jump
    } else if (control.branch && out.equal) {
        pc = pc + instruction->imm; // Salto de Branch (BEQ) se valores forem iguais
    }

    // 8. Textos informativos para imprimir na sua UI
    char nomeEstado[100] = "";
    switch(currentState) {
        case 0: strcpy(nomeEstado, "0 (Busca da Instrucao)"); break;
        case 1: strcpy(nomeEstado, "1 (Decodificacao e Leitura de Regs)"); break;
        case 2: strcpy(nomeEstado, "2 (Calculo de Endereco / Exec. ADDI)"); break;
        case 3: strcpy(nomeEstado, "3 (Acesso a Memoria - Lendo LW)"); break;
        case 4: strcpy(nomeEstado, "4 (Write-Back - Salvando LW)"); break;
        case 5: strcpy(nomeEstado, "5 (Acesso a Memoria - Escrevendo SW)"); break;
        case 6: strcpy(nomeEstado, "6 (Write-Back - Salvando ADDI)"); break;
        case 7: strcpy(nomeEstado, "7 (Execucao - Operacao ULA Tipo-R)"); break;
        case 8: strcpy(nomeEstado, "8 (Write-Back - Salvando Tipo-R)"); break;
        case 9: strcpy(nomeEstado, "9 (Conclusao Branch BEQ)"); break;
        case 10: strcpy(nomeEstado, "10 (Conclusao Jump)"); break;
    }

    char bufferInformation[255] = "";
    char bufferInformation2[255] = "";
    sprintf(bufferInformation, " Estado Executado: %s", nomeEstado);
    sprintf(bufferInformation2, " Proximo ciclo ira para o estado: %d", control.state);

    // --- RENDERIZAÇÃO DA INTERFACE ---
    // Exibe a tabela de sinais de controle atual da FSM
    showClock(instruction, &control);
    
    // Mostra os textos com o nome dos estados
    showClockInformation(bufferInformation, bufferInformation2);

    // Exibe a informação do PC atualizada
    showClockPc();
}