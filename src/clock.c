#include "clock.h"
#include <stdio.h>
#include <string.h>

#include "back.h"
#include "control.h"
#include "main.h"
#include "types.h"
#include "ula.h"
#include "view.h"


ULAOut ulaOut;

char bufferInformation[255] = "";
char bufferInformation2[255] = "";


void clock() {
    saveState();

    const int currentState = state; // pega o estado atual do controle
    const Control control = makeControl(registers.IR.opcode, registers.IR.funct, &state);

    // stats.totalCycles++; // Incrementa 1 ciclo de clock
    if (currentState == 0 && registers.IR.type != OTHER) {
        stats.executedInstructions++;

        if (registers.IR.type == R) stats.executedInstructionsPerType.r++;
        else if (registers.IR.type == I) stats.executedInstructionsPerType.i++;
        else if (registers.IR.type == J) stats.executedInstructionsPerType.j++;
        else stats.executedInstructionsPerType.other++;

        if (registers.IR.opcode == R_TYPE_OPCODE) {
            switch (registers.IR.funct) {
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
            switch (registers.IR.opcode) {
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
        input1 = registers.general[registers.IR.rs];
    }

    // controle da entrada B da ULA
    int8_t input2;
    if (control.ulaSourceB == 0) {
        input2 = registers.general[registers.IR.rt];
    } else if (control.ulaSourceB == 1) {
        input2 = 1; // Geralmente usado para somar PC+1 (caso use)
    } else {
        input2 = registers.IR.imm; // Extensão de sinal do Imediato
    }

    int ulaOp;
    // usa o operador para decidir qual operador a ULA vai executar
    if (currentState == 7) {
        ulaOp = registers.IR.funct;
    } else {
        ulaOp = control.ulaControl;
    }

    ULAOut out = ula(input1, input2, ulaOp);
    // ULA é chamada, com suas entradas A, B e função. Resultado é armazenado em "out"

    // escreve o resultado final da instrução em um registrador
    if (control.wrtReg) {
        int destination;

        if (control.regDst == 1) {
            destination = registers.IR.rd; // A
        } else {
            destination = registers.IR.rt; // B
        }

        int value;

        if (control.memToReg == 1) {
            value = memory.data[out.value];
        } else {
            value = out.value;
        }

        registers.general[destination] = value;
    }


    // salva dados nos registradores intermediários (IR, MDR, A, B, ULAOUT) dependendo do estado do clock
    switch (currentState) {
        case 0: // FETCH
            registers.IR = memory.instructions[pc];
            registers.ULAOut = out.value; // PC + 1
            if (control.wrtPc) pc = registers.ULAOut;

            sprintf(bufferInformation, " Estado 0 (Fetch): Lendo instrução no endereço %03d e calculando PC+1.", pc);
            sprintf(bufferInformation2, " Próximo ciclo: Estado 1 (Decode) para decodificar '%s'.",
                    registers.IR.prettyAsmInstruction);
            break;

        case 1: // DECODE
            registers.A = registers.general[registers.IR.rs];
            registers.B = registers.general[registers.IR.rt];

            sprintf(bufferInformation, " Estado 1 (Decode): Lendo registradores rs($%d)=%d e rt($%d)=%d.", registers.IR.rs,
                    registers.A, registers.IR.rt, registers.B);
            sprintf(bufferInformation2, " Próximo ciclo: Estado %d baseado no opcode %d.", state, registers.IR.opcode);
            break;

        case 2: // EXECUTE / CÁLCULO DE ENDEREÇO
            registers.ULAOut = registers.A + registers.IR.imm;

            sprintf(bufferInformation, " Estado 2 (Addr Calc): Calculando endereço de memória: %d + %d = %d.", registers.A,
                    registers.IR.imm, registers.ULAOut);
            sprintf(bufferInformation2, " Próximo ciclo: Estado %d (Acesso à Memória).", state);
            break;

        case 3: // ACESSO À MEMÓRIA (LW)
            registers.MDR = memory.data[registers.ULAOut];

            sprintf(bufferInformation, " Estado 3 (Mem Read): Lendo valor %d da memória no endereço %d.", registers.MDR,
                    registers.ULAOut);
            sprintf(bufferInformation2, " Próximo ciclo: Estado 4 (Write Back do LW).");
            break;

        case 4: // WRITE BACK (LW)
            if (registers.IR.rt != 0) registers.general[registers.IR.rt] = registers.MDR;

            sprintf(bufferInformation, " Estado 4 (Write Back LW): Escrevendo valor %d no registrador $%d.", registers.MDR,
                    registers.IR.rt);
            sprintf(bufferInformation2, " Próximo ciclo: Estado 0 (Novo Fetch).");
            break;

        case 5: // ACESSO À MEMÓRIA (SW)
            memory.data[registers.ULAOut] = registers.B;

            sprintf(bufferInformation,
                    " Estado 5 (Mem Write): Escrevendo valor do reg B (%d) na memória no endereço %d.", registers.B,
                    registers.ULAOut);
            sprintf(bufferInformation2, " Próximo ciclo: Estado 0 (Novo Fetch).");
            break;

        case 6: // WRITE BACK (ADDI)
            if (registers.IR.rt != 0) registers.general[registers.IR.rt] = registers.ULAOut;

            sprintf(bufferInformation,
                    " Estado 6 (Write Back ADDI): Salvando resultado da soma (%d) no registrador $%d.", registers.ULAOut,
                    registers.IR.rt);
            sprintf(bufferInformation2, " Próximo ciclo: Estado 0 (Novo Fetch).");
            break;

        case 7: // EXECUÇÃO TIPO R
            ulaOut = ula(registers.A, registers.B, control.ulaControl);
            registers.ULAOut = ulaOut.value;

            sprintf(bufferInformation, " Estado 7 (Execute R-Type): Executando operação ULA entre $%d e $%d.",
                    registers.IR.rs, registers.IR.rt);
            sprintf(bufferInformation2, " Próximo ciclo: Estado 8 (Write Back R-Type).");
            break;

        case 8: // WRITE BACK TIPO R
            if (registers.IR.rd != 0) registers.general[registers.IR.rd] = registers.ULAOut;

            sprintf(bufferInformation, " Estado 8 (Write Back R): Salvando resultado %d no registrador de destino $%d.",
                    registers.ULAOut, registers.IR.rd);
            sprintf(bufferInformation2, " Próximo ciclo: Estado 0 (Novo Fetch).");
            break;

        case 9: // BEQ
            sprintf(bufferInformation, " Estado 9 (Branch): Comparando $%d (%d) com $%d (%d).", registers.IR.rs,
                    registers.general[registers.IR.rs], registers.IR.rt, registers.general[registers.IR.rt]);
            if (registers.general[registers.IR.rs] == registers.general[registers.IR.rt]) {
                pc = registers.ULAOut + registers.IR.imm - 1;
                strcat(bufferInformation, " Condição atendida! PC saltará.");
            } else {
                pc = registers.ULAOut;
                strcat(bufferInformation, " Condição não atendida. Segue fluxo normal.");
            }
            sprintf(bufferInformation2, " Próximo ciclo: Estado 0 (Novo Fetch).");
            break;

        case 10: // JUMP
            pc = registers.IR.addr;
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
