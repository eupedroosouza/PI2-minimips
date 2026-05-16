#include "clock.h"
#include <stdio.h>
#include <string.h>

#include "back.h"
#include "control.h"
#include "main.h"
#include "types.h"
#include "ula.h"
#include "view.h"

int state = 0;
Registradores reg;
ULAOut ulaOut;

char bufferInformation[255] = "";
char bufferInformation2[255] = "";


void clock() {
    saveState();

    const int currentState = state; // pega o estado atual do controle
    const Control control = makeControl(reg.IR.opcode, reg.IR.funct, &state);

    // stats.totalCycles++; // Incrementa 1 ciclo de clock
    if (currentState == 0 && reg.IR.type != OTHER) {
        stats.executedInstructions++;

        if (reg.IR.type == R) stats.executedInstructionsPerType.r++;
        else if (reg.IR.type == I) stats.executedInstructionsPerType.i++;
        else if (reg.IR.type == J) stats.executedInstructionsPerType.j++;
        else stats.executedInstructionsPerType.other++;

        if (reg.IR.opcode == R_TYPE_OPCODE) {
            switch (reg.IR.funct) {
                case ADD_FUNCT: stats.executedInstructionsPerClass.add++;
                    break;
                case SUB_FUNCT: stats.executedInstructionsPerClass.sub++;
                    break;
                case AND_FUNCT: stats.executedInstructionsPerClass.and_inst++;
                    break;
                case OR_FUNCT: stats.executedInstructionsPerClass.or_inst++;
                    break;
            }
        } else {
            switch (reg.IR.opcode) {
                case LW_OPCODE: stats.executedInstructionsPerClass.lw++;
                    break;
                case SW_OPCODE: stats.executedInstructionsPerClass.sw++;
                    break;
                case ADDI_OPCODE: stats.executedInstructionsPerClass.addi++;
                    break;
                case BEQ_OPCODE: stats.executedInstructionsPerClass.beq++;
                    break;
                case J_OPCODE: stats.executedInstructionsPerClass.j++;
                    break;
            }
        }
    }

    // controle da entrada A da ULA
    int8_t input1;
    if (control.ulaSourceA == 0) {
        input1 = pc;
    } else {
        input1 = registers[reg.IR.rs];
    }

    // controle da entrada B da ULA
    int8_t input2;
    if (control.ulaSourceB == 0) {
        input2 = registers[reg.IR.rt];
    } else if (control.ulaSourceB == 1) {
        input2 = 1; // Geralmente usado para somar PC+1 (caso use)
    } else {
        input2 = reg.IR.imm; // Extensão de sinal do Imediato
    }

    int ulaOp;
    // usa o operador para decidir qual operador a ULA vai executar
    if (currentState == 7) {
        ulaOp = reg.IR.funct;
    } else {
        ulaOp = control.ulaControl;
    }

    ULAOut out = ula(input1, input2, ulaOp);
    // ULA é chamada, com suas entradas A, B e função. Resultado é armazenado em "out"

    // escreve o resultado final da instrução em um registrador
    if (control.wrtReg) {
        int destination;

        if (control.regDst == 1) {
            destination = reg.IR.rd; // A
        } else {
            destination = reg.IR.rt; // B
        }

        int value;

        if (control.memToReg == 1) {
            value = memory.data[out.value];
        } else {
            value = out.value;
        }

        registers[destination] = value;
    }


    // salva dados nos registradores intermediários (IR, MDR, A, B, ULAOUT) dependendo do estado do clock
    switch (currentState) {
        case 0: // FETCH
            reg.IR = memory.instructions[pc];
            reg.ULAOut = out.value; // PC + 1
            if (control.wrtPc) pc = reg.ULAOut;

            sprintf(bufferInformation, " Estado 0 (Fetch): Lendo instrução no endereço %03d e calculando PC+1.", pc);
            sprintf(bufferInformation2, " Próximo ciclo: Estado 1 (Decode) para decodificar '%s'.",
                    reg.IR.prettyAsmInstruction);
            break;

        case 1: // DECODE
            reg.A = registers[reg.IR.rs];
            reg.B = registers[reg.IR.rt];

            sprintf(bufferInformation, " Estado 1 (Decode): Lendo registradores rs($%d)=%d e rt($%d)=%d.", reg.IR.rs,
                    reg.A, reg.IR.rt, reg.B);
            sprintf(bufferInformation2, " Próximo ciclo: Estado %d baseado no opcode %d.", state, reg.IR.opcode);
            break;

        case 2: // EXECUTE / CÁLCULO DE ENDEREÇO
            reg.ULAOut = reg.A + reg.IR.imm;

            sprintf(bufferInformation, " Estado 2 (Addr Calc): Calculando endereço de memória: %d + %d = %d.", reg.A,
                    reg.IR.imm, reg.ULAOut);
            sprintf(bufferInformation2, " Próximo ciclo: Estado %d (Acesso à Memória).", state);
            break;

        case 3: // ACESSO À MEMÓRIA (LW)
            reg.MDR = memory.data[reg.ULAOut];

            sprintf(bufferInformation, " Estado 3 (Mem Read): Lendo valor %d da memória no endereço %d.", reg.MDR,
                    reg.ULAOut);
            sprintf(bufferInformation2, " Próximo ciclo: Estado 4 (Write Back do LW).");
            break;

        case 4: // WRITE BACK (LW)
            if (reg.IR.rt != 0) registers[reg.IR.rt] = reg.MDR;

            sprintf(bufferInformation, " Estado 4 (Write Back LW): Escrevendo valor %d no registrador $%d.", reg.MDR,
                    reg.IR.rt);
            sprintf(bufferInformation2, " Próximo ciclo: Estado 0 (Novo Fetch).");
            break;

        case 5: // ACESSO À MEMÓRIA (SW)
            memory.data[reg.ULAOut] = reg.B;

            sprintf(bufferInformation,
                    " Estado 5 (Mem Write): Escrevendo valor do reg B (%d) na memória no endereço %d.", reg.B,
                    reg.ULAOut);
            sprintf(bufferInformation2, " Próximo ciclo: Estado 0 (Novo Fetch).");
            break;

        case 6: // WRITE BACK (ADDI)
            if (reg.IR.rt != 0) registers[reg.IR.rt] = reg.ULAOut;

            sprintf(bufferInformation,
                    " Estado 6 (Write Back ADDI): Salvando resultado da soma (%d) no registrador $%d.", reg.ULAOut,
                    reg.IR.rt);
            sprintf(bufferInformation2, " Próximo ciclo: Estado 0 (Novo Fetch).");
            break;

        case 7: // EXECUÇÃO TIPO R
            ulaOut = ula(reg.A, reg.B, control.ulaControl);
            reg.ULAOut = ulaOut.value;

            sprintf(bufferInformation, " Estado 7 (Execute R-Type): Executando operação ULA entre $%d e $%d.",
                    reg.IR.rs, reg.IR.rt);
            sprintf(bufferInformation2, " Próximo ciclo: Estado 8 (Write Back R-Type).");
            break;

        case 8: // WRITE BACK TIPO R
            if (reg.IR.rd != 0) registers[reg.IR.rd] = reg.ULAOut;

            sprintf(bufferInformation, " Estado 8 (Write Back R): Salvando resultado %d no registrador de destino $%d.",
                    reg.ULAOut, reg.IR.rd);
            sprintf(bufferInformation2, " Próximo ciclo: Estado 0 (Novo Fetch).");
            break;

        case 9: // BEQ
            sprintf(bufferInformation, " Estado 9 (Branch): Comparando $%d (%d) com $%d (%d).", reg.IR.rs,
                    registers[reg.IR.rs], reg.IR.rt, registers[reg.IR.rt]);
            if (registers[reg.IR.rs] == registers[reg.IR.rt]) {
                pc = reg.ULAOut + reg.IR.imm - 1;
                strcat(bufferInformation, " Condição atendida! PC saltará.");
            } else {
                pc = reg.ULAOut;
                strcat(bufferInformation, " Condição não atendida. Segue fluxo normal.");
            }
            sprintf(bufferInformation2, " Próximo ciclo: Estado 0 (Novo Fetch).");
            break;

        case 10: // JUMP
            pc = reg.IR.addr;
            sprintf(bufferInformation, " Estado 10 (Jump): PC desviado para o endereço de salto %d.", pc);
            sprintf(bufferInformation2, " Próximo ciclo: Estado 0 (Novo Fetch).");
            break;
    }

    Instruction currentInstruction = memory.instructions[pc];


    sprintf(bufferInformation2, " Proximo ciclo ira para o estado: %d", state);


    // Exibe a tabela de sinais de controle atual da FSM
    showClock(&currentInstruction, &control);

    showClockRegs();
    showClockInformation(bufferInformation, bufferInformation2);

    // Imprime clock
    showClockPc();
}
