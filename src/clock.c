#include "clock.h"
#include "stats.h"
#include "back.h"
#include "control.h"
#include "main.h"
#include "types.h"
#include "ula.h"
#include "view.h"

Combinational makeCombinational() {
    Combinational C = {0};


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
    C.input1 = input1;
    C.input2 = input2;
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
    const Combinational C = makeCombinational();
    viewStateOfMachine(&C);

    // Sequential
    if (C.wrtPc) {
        pc = (uint8_t) C.pc;
    }

if (state == 0) { // Se a FSM vai voltar pro Fetch (estado 0), significa que a instrução atual acabo
    updateStatistics(&registers.IR); 
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
}
