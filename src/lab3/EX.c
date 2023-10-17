#include <stdio.h>
#include <stdlib.h>


int EX(int choice, char* op, int* X, int* Y){
    
    int ALU_out = NULL;
    
    if (choice == 0) {                                                  // Memory Reference (load/store address calculation)
        ALU_out = *X + *Y;
    } 
    else if (choice == 1) {                                             // Register-Register Operation

        if (strcmp(op, "add") == 0) {
            ALU_out = *X + *Y;

        } else if (strcmp(op, "sub") == 0) {
            ALU_out = *X - *Y;

        } else if (strcmp(op, "mul") == 0) {
            ALU_out = *X * *Y;

        } else if (strcmp(op, "div") == 0) {
            ALU_out = *X / *Y;

        } else if (strcmp(op, "shift_left") == 0) {
            ALU_out = *X << *Y;

        } else if (strcmp(op, "shift_right") == 0) {
            ALU_out = *X >> *Y;

        } else {
            printf("Please provide correct Operation argument\n");
            return NULL;
        }
    } 
    else {                                                              // Register-Immediate Operation
        
        if (strcmp(op, "add") == 0) {
            ALU_out = *X + *Y;

        } else if (strcmp(op, "sub") == 0) {
            ALU_out = *X - *Y;

        } else if (strcmp(op, "mul") == 0) {
            ALU_out = *X * *Y;

        } else if (strcmp(op, "div") == 0) {
            ALU_out = *X / *Y;

        } else if (strcmp(op, "shift_left") == 0) {
            ALU_out = *X << *Y;

        } else if (strcmp(op, "shift_right") == 0) {
            ALU_out = *X >> *Y;

        } else {
            printf("Please provide correct Operation argument\n");
            return NULL;
        }
    }

    return ALU_out;
}

