#include <stdio.h>
#include <string.h>

int binarioDecimal(char bin[]) {
    int decimal = 0;
    int tamanho = strlen(bin);

    for (int i = 0; i < tamanho; i++) {
        if (bin[i] == '1') {
            decimal |= (1 << (tamanho - 1 - i));
        }
    }
    return decimal;
}



int main() {
    char bin[33]; 

    printf("Digite um binario: ");
    scanf("%s", bin);

    printf("\nsem complemento de 2: %d\n", binarioDecimal(bin));
 
    return 0;
}