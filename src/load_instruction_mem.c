//Perguntar o caminho do arquivo que contém a memória de instrução (.mem),
//carregar via FILE, transformar cada instrução binária via fgets em uma string de 16 caracteres.
//Adicionar ao vetor de instruçôes dentro de memInstruction (jogar a informaçao para memInstruction) além de que por fim salvar seu tamanho também em memInstruction.

//Estrutura:
//void loadInstructionsOnMem();

#include <stdio.h>
#include <string.h>

#define I 1000

struct memInstruction { // Estrutura que irá armazenar as instruções da memória e o tamanho dela

char instruction[I][17];
int tamanho;

} mem;

void loadInstructionsOnMem(){  // Carrega arquivo de instrução .mem (binário)

char caminho_arquivo_mem[1000];

printf ("\nDigite o caminho do arquivo que contém a memória de instrução .mem: ");
fgets (caminho_arquivo_mem, sizeof (caminho_arquivo_mem), stdin);
caminho_arquivo_mem[strcspn(caminho_arquivo_mem, "\n")] = 0;    // Remove o \n no final, para fopen encontrar o endereço correto

FILE *arquivo;      // Cria variável de arquivo
arquivo = fopen (caminho_arquivo_mem, "r");     // Lê arquivo .mem

if (arquivo == NULL){
printf ("\nErro ao ler arquivo!");
}
 else{


char string [17];   // conjunto de 16 bits
char linha [100];

printf ("\nInstruções em binário da memória: \n");

int i = 0;  // Contador de linha. While não para de se repetir até chegar no fim do arquivo, a cada loop o contador "i" irá soma
while (fgets(linha, sizeof (linha), arquivo) != NULL){

sscanf (linha, "%16[^\n]\n", string);     // Lê os primeiros 16 dígitos do .mem e armazena na variável string
strcpy (mem.instruction[i], string);       // Pega os dados da variável string e coloca na estrutura memInstruction

mem.tamanho ++;
i ++;

}

fclose (arquivo);   // Fecha arquivo

// Imprime os dados da instrução que foram armazenados na matriz. Feito para conferir se esta armazenado corretamente
for (int j = 0 ; j < i ; j ++){
    for (int p = 0 ; p < 17; p ++){
    printf ("%c", mem.instruction[j][p]);
    }
printf ("\n");
}

printf ("\nTamanho da memória de instrução: %d", mem.tamanho);
    } //fim do else

}

int main (){

loadInstructionsOnMem();

return 0;
}
