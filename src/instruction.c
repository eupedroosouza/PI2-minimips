#include "instruction.h"

#include <string.h>

#include "encoding.h"
#include "utils.h"

Instruction decodeInstruction(const char *serializedBinary) {
    Instruction instruction;

    strcpy(instruction.stringedInstruction, serializedBinary);
    //TODO: please, when do asm codec, replace this with valid content
    strcpy(instruction.asmInstruction, "undefined - waiting asm codec/saver");

    char opcodeBuffer[5];
    charsToString(opcodeBuffer, 4, serializedBinary[0], serializedBinary[1], serializedBinary[2], serializedBinary[3]);
    instruction.opcode = binaryToUnsignedInt(opcodeBuffer, 4);

    if (instruction.opcode == 0) {
        instruction.type = R;
    } else if (instruction.opcode == 4 || instruction.opcode == 11 ||
               instruction.opcode == 15 || instruction.opcode == 8) {
        instruction.type = I;
    } else if (instruction.opcode == 2) {
        instruction.type = J;
    } else {
        instruction.type = OTHER;
    }

    char registerAndFunctBuffer[4];
    //rd
    charsToString(registerAndFunctBuffer, 3, serializedBinary[4], serializedBinary[5], serializedBinary[6]);
    instruction.rd = binaryToUnsignedInt(registerAndFunctBuffer, 3);
    //rs
    charsToString(registerAndFunctBuffer, 3, serializedBinary[7], serializedBinary[8], serializedBinary[9]);
    instruction.rs = binaryToUnsignedInt(registerAndFunctBuffer, 3);
    //rt
    charsToString(registerAndFunctBuffer, 3, serializedBinary[10], serializedBinary[11], serializedBinary[12]);
    instruction.rt = binaryToUnsignedInt(registerAndFunctBuffer, 3);
    // funct
    charsToString(registerAndFunctBuffer, 3, serializedBinary[13], serializedBinary[14], serializedBinary[15]);
    instruction.funct = binaryToUnsignedInt(registerAndFunctBuffer, 3);

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
    instruction.addr = binaryToUnsignedInt(addressBuffer, 8);

    return instruction;
}
