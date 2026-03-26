#include "control.h"

#include "utils.h"

Control makeControl(const Instruction *instruction) {
    Control control;
    control.ulaControl = 0; // dummy

    bool ulaControlWasDefined = false;

    // Se for um beq (branch equal)
    control.branch = false;
    if (instruction->opcode == BEQ_OPCODE) {
        control.ulaControl = 6;
        ulaControlWasDefined = true;
        // TODO: waiting ULA feat to take ZeroUla to make a AND to check if do branch or no
        if (true) {
            control.branch = true;
        }
    }

    // Se for um j (jump)
    if (instruction->opcode == J_OPCODE) {
        control.jump = true;
    } else {
        control.jump = false;
    }

    // Se for uma instrução do tipo R pegar o registrador $rd (11-13)
    if (instruction->type == R) {
        control.regDst = 1;
    } else {
        // Se não, pegar o $rt (8-10)
        control.regDst = 0;
    }

    // Se for uma instrução do tipo I a fonte da ula deve ser o immediato.
    if (instruction->type == I && instruction->opcode != BEQ_OPCODE) {
        control.ulaSource = 1;
    } else {
        // Se não, deve ser o valor do segundo registrador lido ($rt)
        control.ulaSource = 0;
    }

    // Se for um (lw) -> memToReg = 0
    if (instruction->opcode == LW_OPCODE || instruction->opcode == BEQ_OPCODE) {
        if (instruction->opcode == LW_OPCODE ) {
            control.ulaControl = 3;
            ulaControlWasDefined = true;
        }
        control.memToReg = 0;
    } else {
        control.memToReg = 1;
    }

    // Se for uma operação que escreve em registradores
    if (instruction->type == R || instruction->opcode == ADDI_OPCODE || instruction->opcode == LW_OPCODE) {
        control.wrtReg = true;
    } else {
        control.wrtReg = false;
    }

    // Set for um (sw) -> wrtMem = true
    if (instruction->opcode == SW_OPCODE) {
        control.ulaControl = 7;
        ulaControlWasDefined = true;
        control.wrtMem = true;
    } else {
        control.wrtMem = false;
    }

    // Se é um addi -> ulaControl = 1
    if (instruction->opcode == ADDI_OPCODE) {
        control.ulaControl = 1;
        ulaControlWasDefined = true;
    }

    if (!ulaControlWasDefined) {
        switch (instruction->funct) {
            case ADD_FUNCT: {
                control.ulaControl = 0;
                break;
            }
            case SUB_FUNCT: {
                control.ulaControl = 2;
                break;
            }
            case AND_FUNCT: {
                control.ulaControl = 4;
                break;
            }
            case OR_FUNCT: {
                control.ulaControl = 5;
                break;
            }
            default: {
                control.ulaControl = 0;
            };
        }
        ulaControlWasDefined = true;
    }

    return control;
}
