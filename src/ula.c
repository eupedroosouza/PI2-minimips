//Função representativa à ULA. Recebe 2 entradas em inteiro de 8 bits e o controle da ULA como parâmetro 
//e retorna um inteiro de 8 bits (int_8t) como resultado.
#include "ula.h"
#include "types.h"
#include "main.h"


ULAOut ula(const int8_t input1, const int8_t input2, const int ulaControl) {
    ULAOut out;
    out.overflow = false;
    out.equal = false;
    // Use 16 bits int to detect overflow
    int16_t valor = 0; // variavel out.value é int8_t (8 bits), por isso criamos uma variavel nova

    switch (ulaControl) {
        // ulaControl ja indica qual operação deve ser feita, vinda de control.c
        case 0: // ADD
        case 1: // ADDI
        case 3: // LW. Calcula endereço somando os regs
        case 7: // SW. Calcula endereço somando os regs
            valor = (int16_t) (input1 + input2);
            break;
        case 2: // SUB
            valor = (int16_t) (input1 - input2);
            break;
        case 4: // AND
            valor = (int16_t) (input1 & input2);
            break;
        case 5: // OR
            valor = (int16_t) (input1 | input2);
            break;
        case 6: // CMP
            valor = (int16_t) (input1 - input2);
            out.equal = (valor == 0);
            break;
        default: break;
    }

    if (valor > 127 || valor < -128) {
        valor = 0;
        out.overflow = true;
    }

    // Here, the value of "valor" is necessarily an 8-bit value, since it was checked whether an overflow occurred.
    out.value = (int8_t) valor;

    return out;
}
