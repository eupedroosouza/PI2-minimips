#include "clock.h"

#include "back.h"
#include "control.h"
#include "main.h"
#include "types.h"
#include "ula.h"
#include "view.h"

CombinationalState makeCombinational() {
    CombinationalState C = {0};


    // Decode
    const Instruction IR = registers.IR;
    const Register MDR = registers.MDR;
    C.A = registers.general[IR.rs];
    C.B = registers.general[IR.rt];
    // Create a control based on IR (but isn't clock yet, that step it's combinational :) )
    const Control C_Control = makeControl(IR.opcode, IR.funct, &state, false);
    C.control = C_Control;

    // ULA
    const Register A = registers.A;
    const Register B = registers.B;
    // Todo: on input1 the int8 with signal ocorres a error here (PC can be larger than 127) what do to solve that?
    const int8_t input1 = C_Control.ulaSourceA == 0 ? (int8_t) pc : A;
    int8_t input2;
    switch (C_Control.ulaSourceB) {
        case 0: {
            input2 = B;
            break;
        }
        case 1: {
            input2 = 1;
            break;
        }
        case 2: {
            input2 = IR.imm;
            break;
        }
        default: return C; // it can never happen
    }
    const ULAOut C_ULAOut = ula(input1, input2, C_Control.ulaControl);
    C.ULAOut = C_ULAOut;

    // Memory Access
    const Register Reg_ULAOut = registers.ULAOut;
    int16_t C_PC;
    switch (C_Control.pcSource) {
        case 0: {
            C_PC = (int16_t) C_ULAOut.value;
            break;
        }
        case 1: {
            C_PC = (int16_t) Reg_ULAOut;
            break;
        }
        case 2: {
            C_PC = IR.addr;
            break;
        }
        default: return C; // it can never happen
    }
    C.pc = C_PC;
    C.memData = B;

    // Reg Write
    C.regToWrite = C_Control.regDst == 0 ? IR.rt : IR.rd;
    C.regWriteData = C_Control.memToReg == 0 ? Reg_ULAOut : MDR;

    C.wrtPc = C_Control.wrtPc | (C_Control.branch && C_ULAOut.equal);

    // Load (needs do here because we need memAddr, but memAddr is ready only after the ULA)
    C.memAddr = C_Control.immOrData == 0 ? (int8_t) pc : Reg_ULAOut;
    C.instruction = memory.instructions[C.memAddr];
    // todo: fix that when memory was unified
    C.MDR = memory.instructions[C.memAddr].data;

    return C;
}

void clock() {
    saveState();

    // Combinational
    const CombinationalState C = makeCombinational();

    // Sequential
    if (C.wrtPc) {
        pc = (uint8_t) C.pc;
    }

    // Decode
    makeControl(registers.IR.opcode, registers.IR.funct, &state, true);
    if (C.control.wrtIr) {
        registers.IR = C.instruction;
    }
    registers.MDR = C.instruction.imm;

    // Load
    registers.A = C.A;
    registers.B = C.B;

    // ULA
    registers.ULAOut = C.ULAOut.value;

    // Memory Access
    if (C.control.wrtMem) {
        // todo: fix this when memory be unified
        memory.instructions[C.memAddr].data = C.memData;
    }

    // Register Write
    if (C.control.wrtReg) {
        registers.general[C.regToWrite] = C.regWriteData;
    }

    viewStateOfMachine();

    // const int currentState = state; // pega o estado atual do controle
    // const Control control = makeControl(registers.IR.opcode, registers.IR.funct, &state);
    //
    // // stats.totalCycles++; // Incrementa 1 ciclo de clock
    // if (currentState == 0 && registers.IR.type != OTHER) {
    //     stats.executedInstructions++;
    //
    //     if (registers.IR.type == R) stats.executedInstructionsPerType.r++;
    //     else if (registers.IR.type == I) stats.executedInstructionsPerType.i++;
    //     else if (registers.IR.type == J) stats.executedInstructionsPerType.j++;
    //     else stats.executedInstructionsPerType.other++;
    //
    //     if (registers.IR.opcode == R_TYPE_OPCODE) {
    //         switch (registers.IR.funct) {
    //             case ADD_FUNCT: stats.executedInstructionsPerClass.add++;
    //                 break;
    //             case SUB_FUNCT: stats.executedInstructionsPerClass.sub++;
    //                 break;
    //             case AND_FUNCT: stats.executedInstructionsPerClass.and_inst++;
    //                 break;
    //             case OR_FUNCT: stats.executedInstructionsPerClass.or_inst++;
    //                 break;
    //         }
    //     } else {
    //         switch (registers.IR.opcode) {
    //             case LW_OPCODE: stats.executedInstructionsPerClass.lw++;
    //                 break;
    //             case SW_OPCODE: stats.executedInstructionsPerClass.sw++;
    //                 break;
    //             case ADDI_OPCODE: stats.executedInstructionsPerClass.addi++;
    //                 break;
    //             case BEQ_OPCODE: stats.executedInstructionsPerClass.beq++;
    //                 break;
    //             case J_OPCODE: stats.executedInstructionsPerClass.j++;
    //                 break;
    //         }
    //     }
    // }
    //
    // // controle da entrada A da ULA
    // int8_t input1;
    // if (control.ulaSourceA == 0) {
    //     input1 = pc;
    // } else {
    //     input1 = registers.general[registers.IR.rs];
    // }
    //
    // // controle da entrada B da ULA
    // int8_t input2;
    // if (control.ulaSourceB == 0) {
    //     input2 = registers.general[registers.IR.rt];
    // } else if (control.ulaSourceB == 1) {
    //     input2 = 1; // Geralmente usado para somar PC+1 (caso use)
    // } else {
    //     input2 = registers.IR.imm; // Extensão de sinal do Imediato
    // }
    //
    // int ulaOp;
    // // usa o operador para decidir qual operador a ULA vai executar
    // if (currentState == 7) {
    //     ulaOp = registers.IR.funct;
    // } else {
    //     ulaOp = control.ulaControl;
    // }
    //
    // ULAOut out = ula(input1, input2, ulaOp);
    // // ULA é chamada, com suas entradas A, B e função. Resultado é armazenado em "out"
    //
    // // escreve o resultado final da instrução em um registrador
    // if (control.wrtReg) {
    //     int destination;
    //
    //     if (control.regDst == 1) {
    //         destination = registers.IR.rd; // A
    //     } else {
    //         destination = registers.IR.rt; // B
    //     }
    //
    //     int value;
    //
    //     if (control.memToReg == 1) {
    //         value = memory.data[out.value];
    //     } else {
    //         value = out.value;
    //     }
    //
    //     registers.general[destination] = value;
    // }
    //
    //
    // // salva dados nos registradores intermediários (IR, MDR, A, B, ULAOUT) dependendo do estado do clock
    // switch (currentState) {
    //     case 0: // FETCH
    //         registers.IR = memory.instructions[pc];
    //         registers.ULAOut = out.value; // PC + 1
    //         if (control.wrtPc) pc = registers.ULAOut;
    //
    //         sprintf(bufferInformation, " Estado 0 (Fetch): Lendo instrução no endereço %03d e calculando PC+1.", pc);
    //         sprintf(bufferInformation2, " Próximo ciclo: Estado 1 (Decode) para decodificar '%s'.",
    //                 registers.IR.prettyAsmInstruction);
    //         break;
    //
    //     case 1: // DECODE
    //         registers.A = registers.general[registers.IR.rs];
    //         registers.B = registers.general[registers.IR.rt];
    //
    //         sprintf(bufferInformation, " Estado 1 (Decode): Lendo registradores rs($%d)=%d e rt($%d)=%d.", registers.IR.rs,
    //                 registers.A, registers.IR.rt, registers.B);
    //         sprintf(bufferInformation2, " Próximo ciclo: Estado %d baseado no opcode %d.", state, registers.IR.opcode);
    //         break;
    //
    //     case 2: // EXECUTE / CÁLCULO DE ENDEREÇO
    //         registers.ULAOut = registers.A + registers.IR.imm;
    //
    //         sprintf(bufferInformation, " Estado 2 (Addr Calc): Calculando endereço de memória: %d + %d = %d.", registers.A,
    //                 registers.IR.imm, registers.ULAOut);
    //         sprintf(bufferInformation2, " Próximo ciclo: Estado %d (Acesso à Memória).", state);
    //         break;
    //
    //     case 3: // ACESSO À MEMÓRIA (LW)
    //         registers.MDR = memory.data[registers.ULAOut];
    //
    //         sprintf(bufferInformation, " Estado 3 (Mem Read): Lendo valor %d da memória no endereço %d.", registers.MDR,
    //                 registers.ULAOut);
    //         sprintf(bufferInformation2, " Próximo ciclo: Estado 4 (Write Back do LW).");
    //         break;
    //
    //     case 4: // WRITE BACK (LW)
    //         if (registers.IR.rt != 0) registers.general[registers.IR.rt] = registers.MDR;
    //
    //         sprintf(bufferInformation, " Estado 4 (Write Back LW): Escrevendo valor %d no registrador $%d.", registers.MDR,
    //                 registers.IR.rt);
    //         sprintf(bufferInformation2, " Próximo ciclo: Estado 0 (Novo Fetch).");
    //         break;
    //
    //     case 5: // ACESSO À MEMÓRIA (SW)
    //         memory.data[registers.ULAOut] = registers.B;
    //
    //         sprintf(bufferInformation,
    //                 " Estado 5 (Mem Write): Escrevendo valor do reg B (%d) na memória no endereço %d.", registers.B,
    //                 registers.ULAOut);
    //         sprintf(bufferInformation2, " Próximo ciclo: Estado 0 (Novo Fetch).");
    //         break;
    //
    //     case 6: // WRITE BACK (ADDI)
    //         if (registers.IR.rt != 0) registers.general[registers.IR.rt] = registers.ULAOut;
    //
    //         sprintf(bufferInformation,
    //                 " Estado 6 (Write Back ADDI): Salvando resultado da soma (%d) no registrador $%d.", registers.ULAOut,
    //                 registers.IR.rt);
    //         sprintf(bufferInformation2, " Próximo ciclo: Estado 0 (Novo Fetch).");
    //         break;
    //
    //     case 7: // EXECUÇÃO TIPO R
    //         ulaOut = ula(registers.A, registers.B, control.ulaControl);
    //         registers.ULAOut = ulaOut.value;
    //
    //         sprintf(bufferInformation, " Estado 7 (Execute R-Type): Executando operação ULA entre $%d e $%d.",
    //                 registers.IR.rs, registers.IR.rt);
    //         sprintf(bufferInformation2, " Próximo ciclo: Estado 8 (Write Back R-Type).");
    //         break;
    //
    //     case 8: // WRITE BACK TIPO R
    //         if (registers.IR.rd != 0) registers.general[registers.IR.rd] = registers.ULAOut;
    //
    //         sprintf(bufferInformation, " Estado 8 (Write Back R): Salvando resultado %d no registrador de destino $%d.",
    //                 registers.ULAOut, registers.IR.rd);
    //         sprintf(bufferInformation2, " Próximo ciclo: Estado 0 (Novo Fetch).");
    //         break;
    //
    //     case 9: // BEQ
    //         sprintf(bufferInformation, " Estado 9 (Branch): Comparando $%d (%d) com $%d (%d).", registers.IR.rs,
    //                 registers.general[registers.IR.rs], registers.IR.rt, registers.general[registers.IR.rt]);
    //         if (registers.general[registers.IR.rs] == registers.general[registers.IR.rt]) {
    //             pc = registers.ULAOut + registers.IR.imm - 1;
    //             strcat(bufferInformation, " Condição atendida! PC saltará.");
    //         } else {
    //             pc = registers.ULAOut;
    //             strcat(bufferInformation, " Condição não atendida. Segue fluxo normal.");
    //         }
    //         sprintf(bufferInformation2, " Próximo ciclo: Estado 0 (Novo Fetch).");
    //         break;
    //
    //     case 10: // JUMP
    //         pc = registers.IR.addr;
    //         sprintf(bufferInformation, " Estado 10 (Jump): PC desviado para o endereço de salto %d.", pc);
    //         sprintf(bufferInformation2, " Próximo ciclo: Estado 0 (Novo Fetch).");
    //         break;
    // }
    //
    // Instruction currentInstruction = memory.instructions[pc];
    //
    //
    // sprintf(bufferInformation2, " Proximo ciclo ira para o estado: %d", state);
    //
    //
    // // Exibe a tabela de sinais de controle atual da FSM
    // showClock(&currentInstruction, &control);
    //
    // showClockRegs();
    // showClockInformation(bufferInformation, bufferInformation2);
    //
    // // Imprime clock
    // showClockPc();
}
