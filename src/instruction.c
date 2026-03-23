#include "instruction.h"

#include <stdio.h>
#include <string.h>

#include "encoding.h"
#include "main.h"
#include "utils.h"

Instruction decodeInstruction(const char *serializedBinary) {
    Instruction instruction;

    strcpy(instruction.stringedInstruction, serializedBinary);
    //TODO: please, when do asm codec, replace this with valid content
    strcpy(instruction.asmInstruction, "undefined - waiting asm codec/saver");

    char opcodeBuffer[5];
    charsToString(opcodeBuffer, 4, serializedBinary[0], serializedBinary[1], serializedBinary[2], serializedBinary[3]);
    instruction.opcode = binaryToUnsignedInt(opcodeBuffer);

    if (instruction.opcode == R_TYPE_OPCODE) {
        instruction.type = R;
    } else if (instruction.opcode == ADDI_OPCODE || instruction.opcode == LW_OPCODE ||
               instruction.opcode == SW_OPCODE || instruction.opcode == BEQ_OPCODE) {
        instruction.type = I;
    } else if (instruction.opcode == J_OPCODE) {
        instruction.type = J;
    } else {
        instruction.type = OTHER;
    }

    char registerAndFunctBuffer[4];
    //rs
    charsToString(registerAndFunctBuffer, 3, serializedBinary[4], serializedBinary[5], serializedBinary[6]);
    instruction.rs = binaryToUnsignedInt(registerAndFunctBuffer);
    //rt
    charsToString(registerAndFunctBuffer, 3, serializedBinary[7], serializedBinary[8], serializedBinary[9]);
    instruction.rt = binaryToUnsignedInt(registerAndFunctBuffer);
    //rd
    charsToString(registerAndFunctBuffer, 3, serializedBinary[10], serializedBinary[11], serializedBinary[12]);
    instruction.rd = binaryToUnsignedInt(registerAndFunctBuffer);
    // funct
    charsToString(registerAndFunctBuffer, 3, serializedBinary[13], serializedBinary[14], serializedBinary[15]);
    instruction.funct = binaryToUnsignedInt(registerAndFunctBuffer);

    // imm
    char immediateBuffer[7];
    charsToString(immediateBuffer, 6, serializedBinary[10], serializedBinary[11], serializedBinary[12],
                  serializedBinary[13], serializedBinary[14], serializedBinary[15]);
    // imm extended 6 -> 8
    char extendedImmediateBuffer[9];
    extendSignal(immediateBuffer, extendedImmediateBuffer, 8);
    instruction.imm = (int8_t) complementOfTwoToInt(extendedImmediateBuffer);

    // addr
    char addressBuffer[9];
    charsToString(addressBuffer, 8, serializedBinary[8], serializedBinary[9], serializedBinary[10],
                  serializedBinary[11], serializedBinary[12],
                  serializedBinary[13], serializedBinary[14], serializedBinary[15]);
    instruction.addr = binaryToUnsignedInt(addressBuffer);

    return instruction;
}


void loadInstructionsOnMem() {  // Função que carrega arquivo de instrução .mem (binário)
    // Carrega arquivo de instrução .mem (binário)

    char caminho_arquivo_mem[1000];

    printf("\nDigite o caminho do arquivo que contém a memória de instrução .mem: ");
    fgets(caminho_arquivo_mem, sizeof (caminho_arquivo_mem), stdin);
    caminho_arquivo_mem[strcspn(caminho_arquivo_mem, "\n")] = 0;
    // Remove o \n no final, para fopen encontrar o endereço correto

    FILE *arquivo; // Cria variável de arquivo
    arquivo = fopen(caminho_arquivo_mem, "r"); // Abre arquivo .mem para leitura

    if (arquivo == NULL) {
        printf("\nErro ao ler arquivo!");
    } else {
        char string[17]; // Conjunto de 16 bits
        char linha[100];
        // Char que armazena temporariamente a linha lida. Logo depois o conteúdo é levado para "char string [17]"

        debugLn("Instruções em binário da memória:");
        int i = 0;
        // Contador de linha. While não para de se repetir até chegar no fim do arquivo, a cada loop o contador "i" irá soma
        while (fgets(linha, sizeof (linha), arquivo) != NULL) {
            sscanf(linha, "%16[^\n]\n", string); // Lê os primeiros 16 dígitos do .mem e armazena na variável string

            memInstruction.instructions[i] = decodeInstruction(string);
            // Pega os dados da variável string e coloca na estrutura memInstruction

            memInstruction.size++;
            i++;
        }

        fclose(arquivo); // Fecha arquivo

        // Imprime os dados da instrução que foram armazenados na matriz. Feito para conferir se esta armazenado corretamente
        for (int j = 0; j < i; j++) {
            debugInstruction(&memInstruction.instructions[j]);
        }

        println("Foram carregadas %d instruções na memória de instrução.", memInstruction.size);
    } //fim do else
}

void debugInstruction(Instruction *instruction) {
    debugLn("Instrução: %s | %s", instruction->stringedInstruction, instruction->asmInstruction);
}
