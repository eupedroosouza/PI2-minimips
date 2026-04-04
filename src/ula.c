//Função representativa à ULA. Recebe 2 entradas em inteiro de 8 bits e o controle da ULA como parâmetro 
//e retorna um inteiro de 8 bits (int_8t) como resultado.
#include "ula.h"
#include "types.h"
#include "main.h"


ULAOut ula(int8_t input1, int8_t input2, int ulaControl) {
    ULAOut out;
    out.zeroUla = false; // Garante que zeroUla começa com valor limpo
    int valor = 0; // variavel out.value é int8_t (8 bits), por isso criamos uma variavel nova

    switch (ulaControl) {
        // ulaControl ja indica qual operação deve ser feita, vinda de control.c

        case 0: // ADD
        case 1: // ADDI
        case 3: // LW. Calcula endereço somando os regs
        case 7: // SW. Calcula endereço somando os regs
            valor = (int) input1 + input2; // input1 e input2 são promovidos à int, pois int8_t vai de -128 a 127
            break;
        case 2: // SUB
            valor = (int) input1 - input2;
            break;
        case 4: // AND
            out.value = (int8_t) (input1 & input2);
            out.zeroUla = (out.value == 0);
            // Retorna 1 se o valor for 0. usado no clock.c para decidir se um BEQ deve ser executado
            return out;
        case 5: // OR
            out.value = (int8_t) (input1 | input2);
            out.zeroUla = (out.value == 0);
            return out;
        case 6: // BEQ. Comparar se 2 valores são iguais
            out.value = 0; // não deixa lixo de memória no campo
            out.zeroUla = (input1 == input2);
            return out;
        default:
            break;
    }

    if (valor > 127) {
        valor = 127;
        out.zeroUla = true;
    } else if (valor < -128) {
        valor = -128;
        out.zeroUla = true;
    }

    out.value = (int8_t) valor;

    return out;
}
