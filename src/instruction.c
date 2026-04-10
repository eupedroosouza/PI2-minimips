#include "instruction.h"

#include <stdio.h>
#include <string.h>

#include "back.h"
#include "colors.h"
#include "encoding.h"
#include "main.h"
#include "reset.h"
#include "utils.h"
#include "view.h"

Instruction emptyInstruction;

void decodeInstruction(Instruction *instruction, const char *serializedBinary) {
    strcpy(instruction->stringedInstruction, serializedBinary);

    const unsigned int binaryAsANumber = binaryToUnsignedInt(serializedBinary);
    sprintf(instruction->hexa, "%04x", binaryAsANumber);

    char opcodeBuffer[5];
    charsToString(opcodeBuffer, 4, serializedBinary[0], serializedBinary[1], serializedBinary[2], serializedBinary[3]);
    instruction->opcode = binaryToUnsignedInt(opcodeBuffer);

    if (instruction->opcode == R_TYPE_OPCODE) {
        instruction->type = R;
    } else if (instruction->opcode == ADDI_OPCODE || instruction->opcode == LW_OPCODE ||
               instruction->opcode == SW_OPCODE || instruction->opcode == BEQ_OPCODE) {
        instruction->type = I;
    } else if (instruction->opcode == J_OPCODE) {
        instruction->type = J;
    } else {
        instruction->type = OTHER;
    }

    char registerAndFunctBuffer[4];
    //rs
    charsToString(registerAndFunctBuffer, 3, serializedBinary[4], serializedBinary[5], serializedBinary[6]);
    instruction->rs = binaryToUnsignedInt(registerAndFunctBuffer);
    //rt
    charsToString(registerAndFunctBuffer, 3, serializedBinary[7], serializedBinary[8], serializedBinary[9]);
    instruction->rt = binaryToUnsignedInt(registerAndFunctBuffer);
    //rd
    charsToString(registerAndFunctBuffer, 3, serializedBinary[10], serializedBinary[11], serializedBinary[12]);
    instruction->rd = binaryToUnsignedInt(registerAndFunctBuffer);
    // funct
    charsToString(registerAndFunctBuffer, 3, serializedBinary[13], serializedBinary[14], serializedBinary[15]);
    instruction->funct = binaryToUnsignedInt(registerAndFunctBuffer);

    // imm
    char immediateBuffer[7];
    charsToString(immediateBuffer, 6, serializedBinary[10], serializedBinary[11], serializedBinary[12],
                  serializedBinary[13], serializedBinary[14], serializedBinary[15]);
    // imm extended 6 -> 8
    char extendedImmediateBuffer[9];
    extendSignal(immediateBuffer, extendedImmediateBuffer, 8);
    instruction->imm = (int8_t) complementOfTwoToInt(extendedImmediateBuffer);

    // addr
    char addressBuffer[9];
    charsToString(addressBuffer, 8, serializedBinary[8], serializedBinary[9], serializedBinary[10],
                  serializedBinary[11], serializedBinary[12],
                  serializedBinary[13], serializedBinary[14], serializedBinary[15]);
    instruction->addr = binaryToUnsignedInt(addressBuffer);

    convertToAssemblyInstruction(instruction, instruction->asmInstruction);
}


void loadInstructionsOnMem() {

    // Função que carrega arquivo de instrução .mem (binário)
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
        resetInstructions();
        char string[17]; // Conjunto de 16 bits
        char linha[100];
        // Char que armazena temporariamente a linha lida. Logo depois o conteúdo é levado para "char string [17]"

        int i = 0;
        // Contador de linha. While não para de se repetir até chegar no fim do arquivo, a cada loop o contador "i" irá soma
        while (fgets(linha, sizeof (linha), arquivo) != NULL) {
            sscanf(linha, "%16[^\n]\n", string); // Lê os primeiros 16 dígitos do .mem e armazena na variável string

            decodeInstruction(&memInstruction.instructions[i], string);
            // Pega os dados da variável string e coloca na estrutura memInstruction

            i++;
        }
        memInstruction.size = i;

        fclose(arquivo); // Fecha arquivo

        // Imprime os dados da instrução que foram armazenados na matriz. Feito para conferir se esta armazenado corretamente
        viewInstructions(memInstruction.instructions, memInstruction.size);

        println("Foram carregadas %d instruções na memória de instrução.", memInstruction.size);
        invalidateLastState();
    } //fim do else
}

void convertToAssemblyInstruction(const Instruction *instruction, char *buffer) {
    // Converte para mnemônio

    switch (instruction->opcode) {
        case R_TYPE_OPCODE:
            // switch interno para determinar qual instruções R-TYPE
            switch (instruction->funct) {
                case ADD_FUNCT:
                    snprintf(buffer, 255, "add $%d, $%d, $%d", instruction->rd, instruction->rs, instruction->rt);
                    break;
                case SUB_FUNCT:
                    snprintf(buffer, 255, "sub $%d, $%d, $%d", instruction->rd, instruction->rs, instruction->rt);
                    break;
                case AND_FUNCT:
                    snprintf(buffer, 255, "and $%d, $%d, $%d", instruction->rd, instruction->rs, instruction->rt);
                    break;
                case OR_FUNCT:
                    snprintf(buffer, 255, "or $%d, $%d, $%d", instruction->rd, instruction->rs, instruction->rt);
            }

            break; // fim do R_TYPE_OPCODE
        case ADDI_OPCODE:
            snprintf(buffer, 255, "addi $%d, $%d, %d", instruction->rt, instruction->rs, instruction->imm);
            break;
        case LW_OPCODE:
            snprintf(buffer, 255, "lw $%d, %d($%d)", instruction->rt, instruction->imm, instruction->rs);
            break;
        case SW_OPCODE:
            snprintf(buffer, 255, "sw $%d, %d($%d)", instruction->rt, instruction->imm, instruction->rs);
            break;
        case BEQ_OPCODE:
            snprintf(buffer, 255, "beq $%d, $%d, %d", instruction->rs, instruction->rt, instruction->imm);
            break;
        case J_OPCODE:
            snprintf(buffer, 255, "j %d", instruction->addr);
            break;
        default: ;
    }
}

// salva as instruções assembly em um arquivo .asm
void saveInstructionOnAssembly() {
    FILE *arquivoDestino;
    char caminho_salvar_asm[100];

    printf("\nDigite o diretório para salvar o arquivo .asm: ");
    fgets(caminho_salvar_asm, sizeof (caminho_salvar_asm), stdin);
    caminho_salvar_asm[strcspn(caminho_salvar_asm, "\n")] = 0;

    arquivoDestino = fopen(caminho_salvar_asm, "w");

    if (arquivoDestino == NULL) {
        printf("\nErro");
    } else {
        for (int i = 0; i < memInstruction.size; i++) {
            fprintf(arquivoDestino, "%s\n", memInstruction.instructions[i].asmInstruction);
        }
    }
    fclose(arquivoDestino);
}
