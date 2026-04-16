//Função representativa à ULA. Recebe 2 entradas em inteiro de 8 bits e o controle da ULA como parâmetro 
//e retorna um inteiro de 8 bits (int_8t) como resultado.
#include "ula.h"
#include "types.h"
#include "main.h"


ULAOut ula(const int8_t input1, const int8_t input2, const int ulaControl) {
    ULAOut out;
    // Garante que os valores de UlaOUT comecem com valor limpo
    out.value = 0;
    out.overflow = false;
    out.equal = false;

    // Casting on operations are mandatory because in the background, before the operation, the values are converted to int32.
    // The int8_t is used to automatically treat overflows!
    switch (ulaControl) {
        // ulaControl ja indica qual operação deve ser feita, vinda de control.c
        case 0: // ADD
        case 1: // ADDI
        case 3: // LW. Calcula endereço somando os regs
        case 7: // SW. Calcula endereço somando os regs
            out.value = (int8_t) (input1 + input2);
            // se resultar em overflow, manda sinal para out.overflow
            break;
        case 2: // SUB/BEQ
        case 6:
            out.value = (int8_t) (input1 - input2);
            // Util only for branch (ulaControl = 6), check if sub of inputs is 0 (two equal inputs subtracted is 0)
            out.equal = (out.value == 0);
            break;
        case 4: // AND
            out.value = (int8_t) (input1 & input2);
            break;
        case 5: // OR
            out.value = (int8_t) (input1 | input2);
            break;
        default:
            break;
    }

    // Check if overflow was occurred
    out.overflow = ((input1 > 0 && input2 > 0 && out.value < 0) || (input1 < 0 && input2 < 0 && out.value > 0));
    return out;
}
