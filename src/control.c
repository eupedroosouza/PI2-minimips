#include "control.h"

void makeControl(const unsigned int opcode, const unsigned int funct, Control *control) {
    switch (control->state) {
        case 0: // Busca
            control->branch = 0;
            control->wrtMem = 0; control->wrtReg = 0;
            control->ulaSourceA = 0; control->ulaSourceB = 1;
            control->wrtIr = 1; 
            control->wrtPc = 0; // <-- AQUI FOI ALTERADO: O PC NÃO AVANÇA MAIS NO ESTADO 0
            control->regDst = 1;
            control->state = 1;
            break;

        case 1: // Decodificação
            control->branch = 0;
            control->wrtIr = 0; control->wrtPc = 0;
            control->ulaSourceA = 0; control->ulaSourceB = 2;
            switch (opcode) {
                case ADDI_OPCODE:
                case LW_OPCODE:
                case SW_OPCODE: control->state = 2; break;
                case R_TYPE_OPCODE: control->state = 7; break;
                case BEQ_OPCODE: control->state = 9; break;
                case J_OPCODE: control->state = 10; break;
                default: control->state = 0; break;
            }
            break;

        case 2: // Execução / Cálculo de Endereço
            control->branch = 0;
            control->ulaSourceA = 1; control->ulaSourceB = 2;
            if (opcode == LW_OPCODE) control->state = 3;
            else if (opcode == SW_OPCODE) control->state = 5;
            else if (opcode == ADDI_OPCODE) control->state = 6;
            break;

        case 3: // Acesso Memória (LW)
            control->branch = 0;
            control->ulaSourceA = 1; control->ulaSourceB = 2;
            control->state = 4; break;

        case 4: // Write-back (LW) - Fim da instrução
            control->branch = 0;
            control->wrtReg = 1; control->regDst = 0; control->memToReg = 1;
            control->wrtPc = 1; control->pcSource = 0; // Avança o PC
            control->state = 0; break;

        case 5: // Acesso Memória (SW) - Fim da instrução
            control->branch = 0;
            control->wrtMem = 1; control->wrtReg = 0;
            control->wrtPc = 1; control->pcSource = 0; // Avança o PC
            control->state = 0; break;

        case 6: // Write-back (ADDI) - Fim da instrução
            control->branch = 0;
            control->wrtReg = 1; control->regDst = 0; control->memToReg = 0;
            control->wrtMem = 0;
            control->wrtPc = 1; control->pcSource = 0; // Avança o PC
            control->state = 0; break;

        case 7: // Execução Tipo-R
            control->branch = 0;
            control->ulaSourceA = 1; control->ulaSourceB = 0;
            control->ulaControl = funct; control->wrtReg = 0; control->wrtMem = 0;
            control->state = 8;
            break;

        case 8: // Write-back Tipo-R - Fim da instrução
            control->branch = 0;
            control->wrtReg = 1; control->regDst = 1; control->memToReg = 0;
            control->wrtMem = 0;
            control->wrtPc = 1; control->pcSource = 0; // Avança o PC
            control->state = 0; break;

        case 9: // Branch (BEQ) - Fim da instrução
            control->ulaSourceA = 1; control->ulaSourceB = 0;
            control->ulaControl = 2; control->branch = 1; control->wrtPc = 0;
            control->state = 0; break;

        case 10: // Jump - Fim da instrução
            control->branch = 0;
            control->wrtPc = 1; control->pcSource = 2;
            control->state = 0; break;

        default: control->state = 0; break;
    }
}
