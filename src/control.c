#include "control.h"

Control makeControl(const Instruction *instruction) {
    Control control;
    // Initial values
    control.jump = false;
    control.branch = false;
    control.regDst = 0;
    control.ulaSource = 0;
    control.memToReg = 1;
    control.wrtReg = false;
    control.wrtMem = false;
    control.ulaControl = 0;

    switch (instruction->type) {
        case I: {
            control.ulaSource = 1;
            switch (instruction->opcode) {
                case BEQ_OPCODE: {
                    control.branch = true;
                    control.ulaControl = 6;
                    control.ulaSource = 0;
                    control.memToReg = 0;
                    break;
                }
                case LW_OPCODE: {
                    control.wrtReg = true;
                    control.memToReg = 0;
                    control.ulaControl = 3;
                    break;
                }
                case ADDI_OPCODE: {
                    control.ulaControl = 1;
                    control.wrtReg = true;
                    break;
                }
                case SW_OPCODE: {
                    control.ulaControl = 7;
                    control.memToReg = 0;
                    control.wrtMem = true;
                    break;
                }
                default: break;
            }
            break;
        }
        case R: {
            control.regDst = 1;
            control.wrtReg = true;
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
                default: break;
            }
            break;
        }
        case J: {
            control.jump = true;
            break;
        }
        case OTHER: break;
    }

    return control;
}
