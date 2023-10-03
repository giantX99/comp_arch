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

        if (strcmp(rs1, "zero") != 0 || strcmp(rs1, "0") != 0) {
            rs1++;   
            H_rs1 = atoi(rs1);
        }

        if (strcmp(joker_card, "zero") != 0 || strcmp(joker_card, "0") != 0) {
            joker_card++;
            H_joker_card = atoi(joker_card);
        }

        H_joker_card = H_joker_card << 20;
        H_rs1 = H_rs1 << 15;
        H_rd = H_rd << 7;

        instruction = H_cmd | H_rd | H_rs1 | H_joker_card;
        //printf("Instruction: %08x\n", instruction);
        return instruction;
    } 
    else if (strcmp(cmd, "addi") == 0) {
        H_cmd = 0x00000013;
        H_rd = atoi(rd);

        if (strcmp(rs1, "zero") != 0 || strcmp(rs1, "0") != 0) {
            rs1++;   
            H_rs1 = atoi(rs1);
        }

        H_joker_card = atoi(joker_card);

        H_joker_card = H_joker_card << 20;
        H_rs1 = H_rs1 << 15;
        H_rd = H_rd << 7;

        instruction = H_cmd | H_rd | H_rs1 | H_joker_card;
        //printf("Instruction: %08x\n", instruction);
        return instruction;
    }
};

int main(int argc, char* argv[])
{
    if(argc != 2) {
        printf("ERROR, Expected 2 inputs ex: ./riscv-asmble input.txt\n");
        return 0;
    }
    FILE* ptrR;
    FILE* ptrW;
    char ch;
 
    // Opening file in reading mode
    ptrR = fopen(argv[1], "r");
    // Opening file in writing mode
    ptrW = fopen("output.txt", "w");
    
    if (NULL == ptrR) {
        printf("input file did not open\n");
    }
    if (NULL == ptrW) {
        printf("output file did not open\n");
    }
 
    char cmd[20];
    char rd[20];
    char rs1[20];
    char joker_card[20];
    int i;
     while (fscanf(ptrR, "%s", cmd) != EOF)  {
        fscanf(ptrR, "%s", rd);
        i = 0;
        while(rd[i] != ',') {
            i++;
        }
        rd[i] = '\0';
        fscanf(ptrR, "%s", rs1);
        i = 0;
        while(rs1[i] != ',') {
            i++;
        }
        rs1[i] = '\0';
        fscanf(ptrR, "%s", joker_card);
        //printf("%s %s, %s, %s\n", cmd, rd, rs1, joker_card);
        int instruction = decoder(cmd, rd, rs1, joker_card);
        fprintf(ptrW,"%08x\n",instruction);
     }
    
    // Closing the file
    fclose(ptrR);
    fclose(ptrW);
    return 0;
}