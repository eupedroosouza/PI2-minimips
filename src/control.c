#include "control.h"

Control makeControl(const unsigned int opcode, const unsigned int funct,  int *state, const bool clock) {
    Control control = {0}; // initialize control with empty

    int newState = 0;
    switch (*state) {
        case 0: // Busca
            control.wrtMem = true;
            control.ulaSourceA = 0;
            control.immOrData = 0;
            control.wrtIr = true;
            control.ulaSourceB = 1;
            control.ulaControl = 0;
            control.wrtPc = true;
            control.pcSource = 0;
            control.regDst = 1;
            newState = 1;
            break;
        case 1: // Decodificação
            control.ulaSourceA = 0;
            control.ulaSourceB = 2;
            control.ulaControl = 0;
            control.regDst = 1;
            switch (opcode) {
                case ADDI_OPCODE:
                case LW_OPCODE:
                case SW_OPCODE: {
                    newState = 2;
                    break;
                }
                case R_TYPE_OPCODE: {
                    newState = 7;
                    break;
                }
                case BEQ_OPCODE: {
                    newState = 9;
                    break;
                }
                case J_OPCODE: {
                    newState = 10;
                    break;
                }
                default: break;
            }
            break;
        case 2: // Execução / Cálculo de Endereço
            control.ulaSourceA = 1;
            control.ulaSourceB = 2;
            control.ulaControl = 0;
            switch (opcode) {
                case LW_OPCODE: {
                    newState = 3;
                    break;
                }
                case SW_OPCODE: {
                    newState = 5;
                    break;
                }
                case ADDI_OPCODE: {
                    newState = 6;
                    break;
                }
                default: break;
            }
            break;

        case 3: // Acesso Memória (LW)
            control.wrtMem = false;
            control.immOrData = 1;
            control.ulaSourceA = 1;
            control.ulaSourceB = 2;
            newState = 4;
            break;

        case 4: // Write-back (LW) - Fim da instrução
            control.wrtReg = true;
            control.memToReg = 1;
            control.regDst = 0;
            control.ulaSourceA = 1;
            control.ulaSourceB = 2;
            newState = 0;
            break;

        case 5: // Acesso Memória (SW) - Fim da instrução
            control.wrtMem = true;
            control.immOrData = 1;
            control.ulaSourceA = 1;
            control.ulaSourceB = 2;
            newState = 0;
            break;

        case 6: // Write-back (ADDI) - Fim da instrução
            control.wrtMem = false;
            control.wrtReg = true;
            control.regDst = 0;
            control.memToReg = 0;
            control.ulaSourceA = 1;
            control.ulaSourceB = 2;
            newState = 0;
            break;
        case 7: // Execução Tipo-R
            control.ulaSourceA = 1;
            control.ulaSourceB = 0;
            control.ulaControl = (int) funct;
            control.regDst = 1;
            newState = 8;
            break;

        case 8: // Write-back Tipo-R - Fim da instrução
            control.regDst = 1;
            control.wrtReg = 1;
            control.memToReg = 0;
            newState = 0;
            break;

        case 9: // Branch (BEQ) - Fim da instrução
            control.ulaSourceA = 1;
            control.ulaSourceB = 0;
            control.ulaControl = 2;
            control.branch = 1;
            control.wrtPc = false;
            control.pcSource = 1;
            newState = 0;
            break;

        case 10: // Jump - Fim da instrução
            control.wrtPc = true;
            control.pcSource = 2;
            newState = 0;
            break;
        default: break;
    }
    if (clock) {
        *state = newState;
    }

    return control;
}