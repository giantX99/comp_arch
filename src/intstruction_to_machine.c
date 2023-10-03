#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>



int decoder(char* cmd, char* rd, char* rs1, char* joker_card){

    int H_cmd = 0x0;
    int H_rd = 0x0;
    int H_rs1 = 0x0;
    int H_joker_card = 0x0;

    int func3 = 0x0;
    int func7 = 0x00;

    int instruction = 0x0;
    int mask = 0x00000000;

    rd++;
    if (strcmp(cmd, "add") == 0){
        H_cmd = 0x00000033;
        H_rd = atoi(rd); 
        
        if (strcmp(rs1, "zero") != 0 || strcmp(rs1, '0') != 0) {
            rs1++;   
            H_rs1 = atoi(rs1);
        }
        
        if (strcmp(joker_card, "zero") != 0 || strcmp(joker_card, '0') != 0) {
            joker_card++;
            H_joker_card = atoi(joker_card);
        }

        H_joker_card << 20;
        H_rs1 << 15;
        H_rd << 7;

        instruction = H_cmd | H_rd | H_rs1 | H_joker_card;
        printf("Instruction: %x", instruction);
        return instruction;
    } 
    else if (strcmp(cmd, "addi") == 0) {
        H_cmd = 0x00000013;
        H_rd = atoi(rd);
        
        if (strcmp(rs1, "zero") != 0 || strcmp(rs1, '0') != 0) {
            rs1++;   
            H_rs1 = atoi(rs1);
        }

        H_joker_card = atoi(joker_card);

        H_joker_card << 20;
        H_rs1 << 15;
        H_rd << 7;

        instruction = H_cmd | H_rd | H_rs1 | H_joker_card;
        printf("Instruction: %x", instruction);
        return instruction;
    }
};