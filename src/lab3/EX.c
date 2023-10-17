#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


void EX(){

    EX_MEM.IR = ID_EX.IR;
    EX_MEM.A = ID_EX.A;
    EX_MEM.B = ID_EX.B;
    EX_MEM.Imm = ID_EX.Imm;


    char* operator = decoderEX(EX_MEM.IR);
    uint32_t* X = (EX_MEM.IR);
    uint32_t* Y = X;
    if (*EX_MEM.B == NULL) {
        Y = EX_MEM.Imm;
    } else {
        Y = EX_MEM.B;
    }
    
    if (strcomp(operator, "mem") == 0) {        // Memory Reference (load/store address calculation)
        EX_MEM.ALUOutput = *X + *Y;
    } 
    else if (strcmp(operator, "add") == 0) {    // Register-Register and Register-Immediate Operation
        EX_MEM.ALUOutput = *X + *Y;

    } else if (strcmp(operator, "sub") == 0) {
        EX_MEM.ALUOutput = *X - *Y;

    } else if (strcmp(operator, "mul") == 0) {
        EX_MEM.ALUOutput = *X * *Y;

    } else if (strcmp(operator, "div") == 0) {
        EX_MEM.ALUOutput = *X / *Y;

    } else if (strcmp(operator, "sll") == 0) {
        EX_MEM.ALUOutput = *X << *Y;

    } else if (strcmp(operator, "srl") == 0) {
        EX_MEM.ALUOutput = *X >> *Y;

    } else if (strcmp(operator, "and") == 0) {
        EX_MEM.ALUOutput = *X & *Y;

    } else if (strcmp(operator, "or") == 0) {
        EX_MEM.ALUOutput = *X | *Y;

    } else if (strcmp(operator, "xor") == 0) {
        EX_MEM.ALUOutput = *X ^ *Y;

    }   

    else {
        printf("Operator decoder malfunction\n");
    }
    
}

