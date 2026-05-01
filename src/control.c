#include "control.h"

void makeControl(const unsigned int opcode, const unsigned int funct, Control *control) {
    // FSM for control
    switch (control->state) {
        case 0: {
            control->wrtMem = 0;
            control->ulaSourceA = 0;
            control->ulaSourceB = 1;
            control->immOrData = 0;
            control->wrtIr = 1;
            control->wrtPc = 1;
            control->pcSource = 0;
            control->regDst = 1;
            control->state = 1;
        }
        case 1: {
            control->ulaSourceA = 0;
            control->ulaSourceB = 2;
            control->ulaControl = 0;
            control->regDst = 0;
            switch (opcode) {
                case ADDI_OPCODE:
                case LW_OPCODE:
                case SW_OPCODE: {
                    control->state = 2;
                }
                case R_TYPE_OPCODE:
                    control->state = 7;
                case BEQ_OPCODE:
                    control->state = 9;
                case J_OPCODE:
                    control->state = 10;
                default: break;
            }
        }
        case 2: {
            control->ulaSourceA = 1;
            control->ulaSourceB = 2;
            control->ulaControl = 0;
            switch (opcode) {
                case LW_OPCODE: {
                    control->state = 3;
                }
                case SW_OPCODE: {
                   control->state = 5;
                }
                case ADDI_OPCODE: {
                    control->state = 6;
                }
                default: break;
            }
        }
        case 3: {
            control->wrtMem = 0;
            control->immOrData = 1;
            control->ulaSourceA = 1;
            control->ulaSourceB = 2;
            control->state = 4;
        }
        case 4: {
            control->wrtReg = 1;
            control->memToReg = 1;
            control->regDst = 0;
            control->ulaSourceA = 1;
            control->ulaSourceB = 2;
            control->state = 0;
        }
        case 5: {
            control->wrtMem = 1;
            control->immOrData = 1;
            control->ulaSourceA = 1;
            control->ulaSourceB = 2;
            control->state = 0;
        }
        case 6: {
            control->wrtMem = 0;
            control->wrtReg = 1;
            control->regDst = 0;
            control->memToReg = 0;
            control->ulaSourceA = 1;
            control->ulaSourceB = 2;
            control->state = 0;
        }
        case 7: {
            control->ulaSourceA = 1;
            control->ulaSourceB = 0;
            control->ulaControl = funct;
            control->regDst = 1;
            control->state = 8;
        }
        case 8: {
            control->regDst = 1;
            control->wrtReg = 1;
            control->memToReg = 0;
            control->state = 0;
        }
        case 9: {
            control->ulaSourceA =  1;
            control->ulaSourceB = 0;
            control->ulaControl = 2;
            control->branch = 1;
            control->wrtPc = 0;
            control->pcSource = 1;
            control->state = 0;
        }
        case 10: {
            control->wrtPc = 1;
            control->pcSource = 2;
            control->state = 0;
        }
        default: break;
    }

}
