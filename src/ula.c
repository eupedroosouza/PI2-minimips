//Função representativa à ULA. Recebe 2 entradas em inteiro de 8 bits e o controle da ULA como parâmetro 
//e retorna um inteiro de 8 bits (int_8t) como resultado.
#include "ula.h"
#include "types.h"
#include "main.h"


ULAOut ula(int input1, int input2, int ulaControl) {
    ULAOut out;
    out.zeroUla = false; // Garante que zeroUla começa com valor limpo
    int valor = 0; // variavel out.value é int8_t (8 bits), por isso criamos uma variavel nova

    switch (ulaControl) {
        // ulaControl ja indica qual operação deve ser feita, vinda de control.c

        case 0: // ADD
        case 1: // ADDI
        case 3: // LW. Calcula endereço somando os regs
        case 7: // SW. Calcula endereço somando os regs
            valor = input1 + input2; 
            out.overflow = ((input1 > 0 && input2 > 0 && valor < 0) || (input1 < 0 && input2 < 0 && valor > 0)); // se resultar em overflow, manda sinal para out.overflow
            break;
        case 2: // SUB
        case 6: //subtrai pra comparar
            valor = input1 - input2;
            out.overflow = ((input1 > 0 && input2 > 0 && valor < 0) || (input1 < 0 && input2 < 0 && valor > 0)); 
            break;
        
            case 4: // AND
            out.value =  (input1 & input2);
            out.zeroUla = (out.value == 0);
            // Retorna 1 se o valor for 0. usado no clock.c para decidir se um BEQ deve ser executado
            return out;
        case 5: // OR
            out.value = (input1 | input2);
            out.zeroUla = (out.value == 0);
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
    } else {
        //compara o valor
        out.zeroUla = (valor == 0);
    }

    out.value = valor;

    return out;
}
