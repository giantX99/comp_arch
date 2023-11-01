#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>

#include "mu-riscv.h"

/***************************************************************/
/* Print out a list of commands available                                                                  */
/***************************************************************/
void help() {
	printf("------------------------------------------------------------------\n\n");
	printf("\t**********MU-RISCV Help MENU**********\n\n");
	printf("sim\t-- simulate program to completion \n");
	printf("run <n>\t-- simulate program for <n> instructions\n");
	printf("rdump\t-- dump register values\n");
	printf("reset\t-- clears all registers/memory and re-loads the program\n");
	printf("input <reg> <val>\t-- set GPR <reg> to <val>\n");
	printf("mdump <start> <stop>\t-- dump memory from <start> to <stop> address\n");
	printf("high <val>\t-- set the HI register to <val>\n");
	printf("low <val>\t-- set the LO register to <val>\n");
	printf("print\t-- print the program loaded into memory\n");
	printf("show\t-- print the current content of the pipeline registers\n");
	printf("forward\t-- enables/disables pipeline forwarding\n");
	printf("?\t-- display help menu\n");
	printf("quit\t-- exit the simulator\n\n");
	printf("------------------------------------------------------------------\n\n");
}

/***************************************************************/
/* Read a 32-bit word from memory                                                                            */
/***************************************************************/
uint32_t mem_read_32(uint32_t address)
{
	int i;
	for (i = 0; i < NUM_MEM_REGION; i++) {
		if ( (address >= MEM_REGIONS[i].begin) &&  ( address <= MEM_REGIONS[i].end) ) {
			uint32_t offset = address - MEM_REGIONS[i].begin;
			return (MEM_REGIONS[i].mem[offset+3] << 24) |
					(MEM_REGIONS[i].mem[offset+2] << 16) |
					(MEM_REGIONS[i].mem[offset+1] <<  8) |
					(MEM_REGIONS[i].mem[offset+0] <<  0);
		}
	}
	return 0;
}

/***************************************************************/
/* Write a 32-bit word to memory                                                                                */
/***************************************************************/
void mem_write_32(uint32_t address, uint32_t value)
{
	int i;
	uint32_t offset;
	for (i = 0; i < NUM_MEM_REGION; i++) {
		if ( (address >= MEM_REGIONS[i].begin) && (address <= MEM_REGIONS[i].end) ) {
			offset = address - MEM_REGIONS[i].begin;

			MEM_REGIONS[i].mem[offset+3] = (value >> 24) & 0xFF;
			MEM_REGIONS[i].mem[offset+2] = (value >> 16) & 0xFF;
			MEM_REGIONS[i].mem[offset+1] = (value >>  8) & 0xFF;
			MEM_REGIONS[i].mem[offset+0] = (value >>  0) & 0xFF;
		}
	}
}

/***************************************************************/
/* Execute one cycle                                                                                                              */
/***************************************************************/
void cycle() {
	handle_pipeline();
	show_pipeline();
	// if hazard is false update pc else pc=pc
	if(hazard == false) {
		CURRENT_STATE = NEXT_STATE;
		NEXT_STATE.PC += 4;
	}
	CYCLE_COUNT++;
}

/***************************************************************/
/* Simulate RISCV for n cycles                                                                                       */
/***************************************************************/
void run(int num_cycles) {

	if (RUN_FLAG == FALSE) {
		printf("Simulation Stopped\n\n");
		return;
	}

	printf("Running simulator for %d cycles...\n\n", num_cycles);
	int i;
	for (i = 0; i < num_cycles; i++) {
		if (RUN_FLAG == FALSE) {
			printf("Simulation Stopped.\n\n");
			break;
		}
		cycle();
	}
}

/***************************************************************/
/* simulate to completion                                                                                               */
/***************************************************************/
void runAll() {
	if (RUN_FLAG == FALSE) {
		printf("Simulation Stopped.\n\n");
		return;
	}

	printf("Simulation Started...\n\n");
	while (RUN_FLAG){
		cycle();
	}
	printf("Simulation Finished.\n\n");
}

/***************************************************************/
/* Dump a word-aligned region of memory to the terminal                              */
/***************************************************************/
void mdump(uint32_t start, uint32_t stop) {
	uint32_t address;

	printf("-------------------------------------------------------------\n");
	printf("Memory content [0x%08x..0x%08x] :\n", start, stop);
	printf("-------------------------------------------------------------\n");
	printf("\t[Address in Hex (Dec) ]\t[Value]\n");
	for (address = start; address <= stop; address += 4){
		printf("\t0x%08x (%d) :\t0x%08x\n", address, address, mem_read_32(address));
	}
	printf("\n");
}

/***************************************************************/
/* Dump current values of registers to the teminal                                              */
/***************************************************************/
void rdump() {
	int i;
	printf("-------------------------------------\n");
	printf("Dumping Register Content\n");
	printf("-------------------------------------\n");
	printf("# Instructions Executed\t: %u\n", INSTRUCTION_COUNT);
	printf("PC\t: 0x%08x\n", CURRENT_STATE.PC);
	printf("-------------------------------------\n");
	printf("[Register]\t[Value]\n");
	printf("-------------------------------------\n");
	for (i = 0; i < MIPS_REGS; i++){
		printf("[R%d]\t: 0x%08x\n", i, CURRENT_STATE.REGS[i]);
	}
	printf("-------------------------------------\n");
	printf("[HI]\t: 0x%08x\n", CURRENT_STATE.HI);
	printf("[LO]\t: 0x%08x\n", CURRENT_STATE.LO);
	printf("-------------------------------------\n");
}

/***************************************************************/
/* Read a command from standard input.                                                               */
/***************************************************************/
void handle_command() {
	char buffer[20];
	uint32_t start, stop, cycles;
	uint32_t register_no;
	int register_value;
	int hi_reg_value, lo_reg_value;

	printf("MU-RISCV SIM:> ");

	if (scanf("%s", buffer) == EOF){
		exit(0);
	}

	switch(buffer[0]) {
		case 'F':
		case 'f':
			if(buffer[1] == 'o' || buffer[1] == 'O') {
				if(ENABLE_FORWARDING == 0) {
					ENABLE_FORWARDING = 1;
				} else {
					ENABLE_FORWARDING = 0;
				}
				ENABLE_FORWARDING == 0 ? printf("Forwarding OFF\n") : printf("Forwarding ON\n");
			}
			break;
		case 'S':
		case 's':
			if (buffer[1] == 'h' || buffer[1] == 'H'){
				show_pipeline();
			}else {
				runAll();
			}
			break;
		case 'M':
		case 'm':
			if (scanf("%x %x", &start, &stop) != 2){
				break;
			}
			mdump(start, stop);
			break;
		case '?':
			help();
			break;
		case 'Q':
		case 'q':
			printf("**************************\n");
			printf("Exiting MU-RISCV! Good Bye...\n");
			printf("**************************\n");
			exit(0);
		case 'R':
		case 'r':
			if (buffer[1] == 'd' || buffer[1] == 'D'){
				rdump();
			}else if(buffer[1] == 'e' || buffer[1] == 'E'){
				reset();
			}
			else {
				if (scanf("%d", &cycles) != 1) {
					break;
				}
				run(cycles);
			}
			break;
		case 'I':
		case 'i':
			if (scanf("%u %i", &register_no, &register_value) != 2){
				break;
			}
			CURRENT_STATE.REGS[register_no] = register_value;
			NEXT_STATE.REGS[register_no] = register_value;
			break;
		case 'H':
		case 'h':
			if (scanf("%i", &hi_reg_value) != 1){
				break;
			}
			CURRENT_STATE.HI = hi_reg_value;
			NEXT_STATE.HI = hi_reg_value;
			break;
		case 'L':
		case 'l':
			if (scanf("%i", &lo_reg_value) != 1){
				break;
			}
			CURRENT_STATE.LO = lo_reg_value;
			NEXT_STATE.LO = lo_reg_value;
			break;
		case 'P':
		case 'p':
			print_program();
			break;
		default:
			printf("Invalid Command.\n");
			break;
	}
}

/***************************************************************/
/* reset registers/memory and reload program                                                    */
/***************************************************************/
void reset() {
	int i;
	/*reset registers*/
	for (i = 0; i < MIPS_REGS; i++){
		CURRENT_STATE.REGS[i] = 0;
	}
	CURRENT_STATE.HI = 0;
	CURRENT_STATE.LO = 0;

	for (i = 0; i < NUM_MEM_REGION; i++) {
		uint32_t region_size = MEM_REGIONS[i].end - MEM_REGIONS[i].begin + 1;
		memset(MEM_REGIONS[i].mem, 0, region_size);
	}

	/*load program*/
	load_program();

	/*reset PC*/
	INSTRUCTION_COUNT = 0;
	CURRENT_STATE.PC =  MEM_TEXT_BEGIN;
	NEXT_STATE = CURRENT_STATE;
	NEXT_STATE.PC += 4;
	RUN_FLAG = TRUE;
	PIPE_EXECUTE = 1;
}

/***************************************************************/
/* Allocate and set memory to zero                                                                            */
/***************************************************************/
void init_memory() {
	int i;
	for (i = 0; i < NUM_MEM_REGION; i++) {
		uint32_t region_size = MEM_REGIONS[i].end - MEM_REGIONS[i].begin + 1;
		MEM_REGIONS[i].mem = malloc(region_size);
		memset(MEM_REGIONS[i].mem, 0, region_size);
	}
}

/**************************************************************/
/* load program into memory                                                                                      */
/**************************************************************/
void load_program() {
	FILE * fp;
	int i, word;
	uint32_t address;

	/* Open program file. */
	fp = fopen(prog_file, "r");
	if (fp == NULL) {
		printf("Error: Can't open program file %s\n", prog_file);
		exit(-1);
	}

	/* Read in the program. */

	i = 0;
	while( fscanf(fp, "%x\n", &word) != EOF ) {
		address = MEM_TEXT_BEGIN + i;
		mem_write_32(address, word);
		printf("writing 0x%08x into address 0x%08x (%d)\n", word, address, address);
		i += 4;
	}
	PROGRAM_SIZE = i/4;
	INSTRUCTION_COUNT = 0;
	printf("Program loaded into memory.\n%d words written into memory.\n\n", PROGRAM_SIZE);
	fclose(fp);
}

//same as decoderOP but returns simpiler definitions for each instruct to implement EX easier
char * decoderEX(uint32_t ir) 
{
	//error checking
	if(ir == 0)
	{
		return "bad";
	}
	uint32_t maskopcode = 0x7F;
	uint32_t opcode = ir & maskopcode;
	if(opcode == 51) { //R-type
		/*
		uint32_t maskrd = 0xF80;
		uint32_t rd = ir & maskrd;
		rd = rd >> 7;
		*/
		uint32_t maskf3 = 0x7000;
		uint32_t f3 = ir & maskf3;
		f3 = f3 >> 12;
		/*
		uint32_t maskrs1 = 0xF8000;
		uint32_t rs1 = ir & maskrs1;
		rs1 = rs1 >> 15;
		uint32_t maskrs2 = 0x1F00000;
	    uint32_t rs2 = ir & maskrs2;
		rs2 = rs2 >> 20;
		*/
	    uint32_t maskf7 = 0xFE000000;
		uint32_t f7 = ir & maskf7;
		f7 = f7 >> 25;
		switch(f3){
		case 0:
			switch(f7){
				case 0:	//add
					return "add";
				case 32:	//sub
					return "sub";
				default:
					return "bad";
				}
		case 4:				//xor
			return "xor";
		case 6: 			//or
			return "or";
		case 7:				//and
			return "and";
		case 1: 			//sll
			return "sll";
		case 5:
			switch(f7) {
				case 0: 	//srl
					return "srl";
				case 32:	//sra
					return "sra";
			}
		case 2:
			return "slt";
		case 3:
			return "sltu";
		default:
			return "bad";
	} 			
	} else if(opcode == 19) { //I-imm
		/*
		uint32_t maskrd = 0xF80;
		uint32_t rd = ir & maskrd;
		rd = rd >> 7;
		*/
		uint32_t maskf3 = 0x7000;
		uint32_t f3 = ir & maskf3;
		f3 = f3 >> 12;
		/*
		uint32_t maskrs1 = 0xF8000;
		uint32_t rs1 = ir & maskrs1;
		rs1 = rs1 >> 15;
		*/
		uint32_t maskimm = 0xFFF00000;
		uint32_t imm = ir & maskimm;
		imm = imm >> 20;
		//uint32_t imm0_4 = (imm << 7) >> 7;
		uint32_t imm5_11 = imm >> 5;
		switch (f3)
		{
		case 0: //addi
			return "add";

		case 4: //xori
			return "xor";
	
		case 6: //ori
			return "or";
	
		case 7: //andi
			return "and";
	
		case 1: //slli
			return "sll";
	
		case 5: //srli and srai
			switch (imm5_11)
			{
				case 0: //srli
					return "srl";

				case 32: //srai
					return "sra";
		
				default:
					return "bad";
			}
	
		case 2:
			return "slt";

		case 3:
			return "sltu";

		default:
			return "bad";
		}
		
	} else if(opcode == 3) { //I-load
		/*
		uint32_t maskrd = 0xF80;
		uint32_t rd = ir & maskrd;
		rd = rd >> 7;
		*/
		uint32_t maskf3 = 0x7000;
		uint32_t f3 = ir & maskf3;
		f3 = f3 >> 12;
		/*
		uint32_t maskrs1 = 0xF8000;
		uint32_t rs1 = ir & maskrs1;
		rs1 = rs1 >> 15;
		uint32_t maskimm = 0xFFF00000;
		uint32_t imm = ir & maskimm;
		imm = imm >> 20;
		*/
		switch (f3)
		{
		case 0: //lb
			return "mem";

		case 1: //lh
			return "mem";

		case 2: //load			
			return "mem";
	
		default:
			return "bad";
	} 

	} else if(opcode == 35) { //S-type
		/*
		uint32_t maskimm0_4 = 0xF80;
		uint32_t imm0_4 = ir & maskimm0_4;
		imm0_4 = imm0_4 >> 7;
		*/
		uint32_t maskf3 = 0x7000;
		uint32_t f3 = ir & maskf3;
		f3 = f3 >> 12;
		/*
		uint32_t maskrs1 = 0xF8000;
		uint32_t rs1 = ir & maskrs1;
		rs1 = rs1 >> 15;
		uint32_t maskrs2 = 0x1F00000;
	    uint32_t rs2 = ir & maskrs2;
		rs2 = rs2 >> 20;
	    uint32_t maskimm5_11 = 0xFE000000;
		uint32_t imm5_11 = ir & maskimm5_11;
		imm5_11 = imm5_11 >> 25;
		uint32_t imm = (imm5_11 << 5) + imm0_4;
		*/
		switch (f3)
		{
		case 0: //sb
			return "mem";
	
		case 1: //sh
			return "mem";

		case 2: //sw
			return "mem";

		default:
			return "bad";
		}
	} else if(opcode == 99) { //B-type
		return "Branch instruction"; //not yet implemented
	} else {
		return "ERROR: not yet implemented";
	}
}

char * decoderOP(uint32_t ir) 
{
//error checking
	if(ir == 0)
	{
		return "bad";
	}
uint32_t maskopcode = 0x7F;
	uint32_t opcode = ir & maskopcode;
	if(opcode == 51) { //R-type
		/*
		uint32_t maskrd = 0xF80;
		uint32_t rd = ir & maskrd;
		rd = rd >> 7;
		*/
		uint32_t maskf3 = 0x7000;
		uint32_t f3 = ir & maskf3;
		f3 = f3 >> 12;
		/*
		uint32_t maskrs1 = 0xF8000;
		uint32_t rs1 = ir & maskrs1;
		rs1 = rs1 >> 15;
		uint32_t maskrs2 = 0x1F00000;
	    uint32_t rs2 = ir & maskrs2;
		rs2 = rs2 >> 20;
		*/
	    uint32_t maskf7 = 0xFE000000;
		uint32_t f7 = ir & maskf7;
		f7 = f7 >> 25;
		switch(f3){
		case 0:
			switch(f7){
				case 0:	//add
					return "add";
				case 32:	//sub
					return "sub";
				default:
					return "bad";
				}
		case 4:				//xor
			return "xor";
		case 6: 			//or
			return "or";
		case 7:				//and
			return "and";
		case 1: 			//sll
			return "sll";
		case 5:
			switch(f7) {
				case 0: 	//srl
					return "srl";
				case 32:	//sra
					return "sra";
			}
		case 2:
			return "slt";
		case 3:
			return "sltu";
		default:
			return "bad";
	} 			
	} else if(opcode == 19) { //I-imm
		/*
		uint32_t maskrd = 0xF80;
		uint32_t rd = ir & maskrd;
		rd = rd >> 7;
		*/
		uint32_t maskf3 = 0x7000;
		uint32_t f3 = ir & maskf3;
		f3 = f3 >> 12;
		/*
		uint32_t maskrs1 = 0xF8000;
		uint32_t rs1 = ir & maskrs1;
		rs1 = rs1 >> 15;
		*/
		uint32_t maskimm = 0xFFF00000;
		uint32_t imm = ir & maskimm;
		imm = imm >> 20;
		//uint32_t imm0_4 = (imm << 7) >> 7;
		uint32_t imm5_11 = imm >> 5;
		switch (f3)
		{
		case 0: //addi
			return "addi";

		case 4: //xori
			return "xori";
	
		case 6: //ori
			return "ori";
	
		case 7: //andi
			return "andi";
	
		case 1: //slli
			return "slli";
	
		case 5: //srli and srai
			switch (imm5_11)
			{
				case 0: //srli
					return "srli";

				case 32: //srai
					return "srai";
		
				default:
					return "bad";
			}
	
		case 2:
			return "slti";

		case 3:
			return "sltiu";

		default:
			return "bad";
		}
		
	} else if(opcode == 3) { //I-load
		/*
		uint32_t maskrd = 0xF80;
		uint32_t rd = ir & maskrd;
		rd = rd >> 7;
		*/
		uint32_t maskf3 = 0x7000;
		uint32_t f3 = ir & maskf3;
		f3 = f3 >> 12;
		/*
		uint32_t maskrs1 = 0xF8000;
		uint32_t rs1 = ir & maskrs1;
		rs1 = rs1 >> 15;
		uint32_t maskimm = 0xFFF00000;
		uint32_t imm = ir & maskimm;
		imm = imm >> 20;
		*/
		switch (f3)
		{
		case 0: //lb
			return "lb";

		case 1: //lh
			return "lh";

		case 2: //lw
			return "lw";
	
		default:
			return "bad";
	} 

	} else if(opcode == 35) { //S-type
		/*
		uint32_t maskimm0_4 = 0xF80;
		uint32_t imm0_4 = ir & maskimm0_4;
		imm0_4 = imm0_4 >> 7;
		*/
		uint32_t maskf3 = 0x7000;
		uint32_t f3 = ir & maskf3;
		f3 = f3 >> 12;
		/*
		uint32_t maskrs1 = 0xF8000;
		uint32_t rs1 = ir & maskrs1;
		rs1 = rs1 >> 15;
		uint32_t maskrs2 = 0x1F00000;
	    uint32_t rs2 = ir & maskrs2;
		rs2 = rs2 >> 20;
	    uint32_t maskimm5_11 = 0xFE000000;
		uint32_t imm5_11 = ir & maskimm5_11;
		imm5_11 = imm5_11 >> 25;
		uint32_t imm = (imm5_11 << 5) + imm0_4;
		*/
		switch (f3)
		{
		case 0: //sb
			return "sb";
	
		case 1: //sh
			return "sh";

		case 2: //sw
			return "sw";

		default:
			return "bad";
		}
	} else if(opcode == 99) { //B-type
		return "Branch instruction"; //not yet implemented
	} else {
		return "ERROR: not yet implemented";
	}
}

uint32_t decoderRD(uint32_t ir) {
	//error checking
	if(ir == 0)
	{
		return 0;
	}
	uint32_t maskopcode = 0x7F;
	uint32_t opcode = ir & maskopcode;
	if(opcode == 51) { //R-type
		uint32_t maskrd = 0xF80;
		uint32_t rd = ir & maskrd;
		rd = rd >> 7;
		return rd;
	} else if(opcode == 19) { //I-imm
		uint32_t maskrd = 0xF80;
		uint32_t rd = ir & maskrd;
		rd = rd >> 7;
		return rd;
	} else if(opcode == 3) { //I-load
		uint32_t maskrd = 0xF80;
		uint32_t rd = ir & maskrd;
		rd = rd >> 7;
		return rd;
	} else if(opcode == 35) { //S-type
		uint32_t maskrs1 = 0xF8000;
		uint32_t rs1 = ir & maskrs1;
		rs1 = rs1 >> 15;
		uint32_t maskimm0_4 = 0xF80;
		uint32_t imm0_4 = ir & maskimm0_4;
		imm0_4 = imm0_4 >> 7;
		return imm0_4;
	} else if(opcode == 99) { //B-type
		return -1; //not yet implemented
	} else { 				//All other instruction types
		return -1; //not yet implemented
	}
}

uint32_t decoderRS1(uint32_t ir) {
	//error checking
	if(ir == 0)
	{
		return 0;
	}
	uint32_t maskopcode = 0x7F;
	uint32_t opcode = ir & maskopcode;
	if(opcode == 51) { //R-type
		uint32_t maskrs1 = 0xF8000;
		uint32_t rs1 = ir & maskrs1;
		rs1 = rs1 >> 15;
		return rs1;
	} else if(opcode == 19) { //I-imm
		uint32_t maskrs1 = 0xF8000;
		uint32_t rs1 = ir & maskrs1;
		rs1 = rs1 >> 15;
		return rs1;
	} else if(opcode == 3) { //I-load
		uint32_t maskrs1 = 0xF8000;
		uint32_t rs1 = ir & maskrs1;
		rs1 = rs1 >> 15;
		return rs1;
	} else if(opcode == 35) { //S-type
		uint32_t maskrs1 = 0xF8000;
		uint32_t rs1 = ir & maskrs1;
		rs1 = rs1 >> 15;
		return rs1;
	} else if(opcode == 99) { //B-type
		return -1; //not yet implemented
	} else { 				//All other instruction types
		return -1; //not yet implemented
	}
}

uint32_t decoderWild(uint32_t ir) {
	//error checking
	if(ir == 0)
	{
		return 0;
	}
	
	uint32_t maskopcode = 0x7F;
	uint32_t opcode = ir & maskopcode;
	if(opcode == 51) { //R-type
		uint32_t maskrs1 = 0xF8000;
		uint32_t rs1 = ir & maskrs1;
		rs1 = rs1 >> 15;
		return rs1;
	} else if(opcode == 19) { //I-imm
		uint32_t maskimm = 0xFFF00000;
		uint32_t imm = ir & maskimm;
		imm = imm >> 20;
		uint32_t imm0_4 = (imm << 7) >> 7;
		//uint32_t imm5_11 = imm >> 5;
		uint32_t maskf3 = 0x7000;
		uint32_t f3 = ir & maskf3;
		f3 = f3 >> 12;
		switch(f3) {
			case 1:
				return imm0_4;
			case 5:
				return imm0_4;
			default:
				return imm;
		}
	} else if(opcode == 3) { //I-load
		uint32_t maskimm = 0xFFF00000;
		uint32_t imm = ir & maskimm;
		imm = imm >> 20;
		return imm;
	} else if(opcode == 35) { //S-type
		uint32_t maskrs2 = 0x1F00000;
	    uint32_t rs2 = ir & maskrs2;
		rs2 = rs2 >> 20;
		return rs2;
	} else if(opcode == 99) { //B-type
		return -1; //not yet implemented
	} else { 				//All other instruction types
		return -1; //not yet implemented
	}
}

/************************************************************/
/* maintain the pipeline                                                                                           */
/************************************************************/
void handle_pipeline()
{
	/*INSTRUCTION_COUNT should be incremented when instruction is done*/
	/*Since we do not have branch/jump instructions, INSTRUCTION_COUNT should be incremented in WB stage */
	WB();
	MEM();
	EX();
	ID();
	IF();
	// if statements for data hazard; set hazard to true if necessary
	if(INSTRUCTION_COUNT == PROGRAM_SIZE) {
		RUN_FLAG = FALSE;
	}
}

/************************************************************/
/* writeback (WB) pipeline stage:                                                                          */
/************************************************************/
void WB()
{
	
	//figuring out instruction
	char * instruction = decoderOP(MEM_WB.IR);
	//all instructions use write, unless no instruction is loaded
	if(strcmp(instruction, "bad") == 0) {
		EX_MEM.RegWrite = false;
	} else {
		EX_MEM.RegWrite = true;
	}
	uint32_t maskopcode = 0x7F;
	uint32_t opcode = MEM_WB.IR & maskopcode;
	if(opcode == 0) {
		return;
	}
	//Load/Store Instruction
	if(strcmp(instruction,"sb") == 0|| strcmp(instruction,"sh") == 0 || strcmp(instruction,"sw") == 0 || strcmp(instruction,"lb") == 0 || strcmp(instruction,"lh") == 0 || strcmp(instruction,"lw") == 0) {
		if(strcmp(instruction,"lb") == 0 || strcmp(instruction,"lh") == 0 || strcmp(instruction,"lw") == 0) { //load
			uint32_t rt = decoderRD(MEM_WB.IR);
			NEXT_STATE.REGS[rt] = MEM_WB.LMD; //load
		} else {} //store
	} else { //ALU Instruction
		uint32_t rd = decoderRD(MEM_WB.IR);
		NEXT_STATE.REGS[rd] = MEM_WB.ALUOutput;
	}
	INSTRUCTION_COUNT++;
}

/************************************************************/
/* memory access (MEM) pipeline stage:                                                          */
/************************************************************/
void MEM()
{
	//figuring out instruction
	char * instruction = decoderOP(EX_MEM.IR);
	//all instructions use write, unless no instruction is loaded
	if(strcmp(instruction, "bad") == 0) {
		EX_MEM.RegWrite = false;
	} else {
		EX_MEM.RegWrite = true;
	}
	uint32_t maskopcode = 0x7F;
	uint32_t opcode = EX_MEM.IR & maskopcode;
	if(opcode == 0) {
		return;
	}
	//Load/Store Instruction
	if(strcmp(instruction,"sb") == 0|| strcmp(instruction,"sh") == 0 || strcmp(instruction,"sw") == 0 || strcmp(instruction,"lb") == 0 || strcmp(instruction,"lh") == 0 || strcmp(instruction,"lw") == 0) {
		MEM_WB.IR = EX_MEM.IR;
		if(strcmp(instruction,"sb") == 0|| strcmp(instruction,"sh") == 0 || strcmp(instruction,"sw") == 0) {
			mem_write_32(EX_MEM.ALUOutput, EX_MEM.B); //store
		} else if(strcmp(instruction,"lb") == 0 || strcmp(instruction,"lh") == 0 || strcmp(instruction,"lw") == 0) {
			MEM_WB.LMD = mem_read_32(EX_MEM.ALUOutput); //load
		}
	} else { //ALU Instructs
		MEM_WB.IR = EX_MEM.IR;
		MEM_WB.ALUOutput = EX_MEM.ALUOutput;
	}
	switch(ENABLE_FORWARDING) {
		case 1:
			if(MEM_WB.RegWrite && (decoderRD(MEM_WB.IR) != 0) &&
			(decoderRD(MEM_WB.IR) == decoderRS1(ID_EX.IR))) {
				forwardA = MEM_WB.ALUOutput;
				EX_MEM.RegWrite = false;
			}
			if(MEM_WB.RegWrite && (decoderRD(MEM_WB.IR) != 0) &&
			(decoderRD(MEM_WB.IR) == decoderWild(ID_EX.IR))) {
				forwardB = MEM_WB.ALUOutput;
				EX_MEM.RegWrite = false;
			}
			break;
		default:
			break;
	}
	return;
}

/************************************************************/
/* execution (EX) pipeline stage:                                                                          */
/************************************************************/
void EX()
{	
	
	EX_MEM.IR = ID_EX.IR;
    EX_MEM.A = ID_EX.A;
    EX_MEM.B = ID_EX.B;
    EX_MEM.imm = ID_EX.imm;

    char* operator = decoderEX(EX_MEM.IR);
	uint32_t maskopcode = 0x7F;
	uint32_t opcode = ID_EX.IR & maskopcode;
	if(opcode == 0) {
		return;
	}
    uint32_t X = EX_MEM.A;
    uint32_t Y;
    if (opcode == 51) {
        Y = EX_MEM.B;
    } else {
        Y = EX_MEM.imm;
    }
    
    if (strcmp(operator, "mem") == 0) {        // Memory Reference (load/store address calculation)
        EX_MEM.ALUOutput = X + Y;
    } 
    else if (strcmp(operator, "add") == 0) {    // Register-Register and Register-Immediate Operation
        EX_MEM.ALUOutput = X + Y;

    } else if (strcmp(operator, "sub") == 0) {
        EX_MEM.ALUOutput = X - Y;

    } else if (strcmp(operator, "mul") == 0) {
        EX_MEM.ALUOutput = X * Y;

    } else if (strcmp(operator, "div") == 0) {
        EX_MEM.ALUOutput = X / Y;

    } else if (strcmp(operator, "sll") == 0) {
        EX_MEM.ALUOutput = X << Y;

    } else if (strcmp(operator, "srl") == 0) {
        EX_MEM.ALUOutput = X >> Y;

    } else if (strcmp(operator, "and") == 0) {
        EX_MEM.ALUOutput = X & Y;

    } else if (strcmp(operator, "or") == 0) {
        EX_MEM.ALUOutput = X | Y;

    } else if (strcmp(operator, "xor") == 0) {
        EX_MEM.ALUOutput = X ^ Y;

    }
	switch(ENABLE_FORWARDING) {
		case 1:
			if(EX_MEM.RegWrite && (decoderRD(EX_MEM.IR) != 0) &&
			(decoderRD(EX_MEM.IR) == decoderRS1(ID_EX.IR))) {
				forwardA = EX_MEM.ALUOutput;
				EX_MEM.RegWrite = false;
			}
			if(EX_MEM.RegWrite && (decoderRD(EX_MEM.IR) != 0) &&
			(decoderRD(EX_MEM.IR) == decoderWild(ID_EX.IR))) {
				forwardB = EX_MEM.ALUOutput;
				EX_MEM.RegWrite = false;
			}
			break;
		default:
			break;
	}
    
}

/************************************************************/
/* instruction decode (ID) pipeline stage:                                                         */
/************************************************************/
void ID()
{
	ID_EX.IR = IF_ID.IR;
	uint32_t maskopcode = 0x7F;
	uint32_t opcode = ID_EX.IR & maskopcode;
	if(opcode == 0) {
		return;
	}
	
	if(EX_MEM.RegWrite && (decoderRD(EX_MEM.IR) != 0) &&
	(decoderRD(EX_MEM.IR) == decoderRS1(ID_EX.IR))) {
		printf("%x,%x,%x\n",EX_MEM.RegWrite,decoderRD(EX_MEM.IR), decoderRS1(ID_EX.IR));
		//Stall
		hazard = true;
	}
	else if(EX_MEM.RegWrite && (decoderRD(EX_MEM.IR) != 0) &&
	(decoderRD(EX_MEM.IR) == decoderWild(ID_EX.IR))) {
		//Stall
		hazard = true;
	}
	else if(MEM_WB.RegWrite && (decoderRD(MEM_WB.IR) != 0) &&
	(decoderRD(MEM_WB.IR) == decoderRS1(ID_EX.IR))) {
		//Stall
		hazard = true;
	}
	else if(MEM_WB.RegWrite && (decoderRD(MEM_WB.IR) != 0) &&
	(decoderRD(MEM_WB.IR) == decoderWild(ID_EX.IR))) {
		//Stall
		hazard = true;
	} else {
	hazard = false;
	// Storing rs1 and rs2 into temp registers 
	//both decoders would need to be pass by refrence so I can access the different needed fields
	
	uint32_t rs1;
	if(opcode == 51) { //reg-reg instruction
		rs1 = decoderRS1(ID_EX.IR);
		ID_EX.A = CURRENT_STATE.REGS[rs1];
		if(forwardA != 0) {
			ID_EX.A = forwardA;
			forwardA = 0;
		}
		uint32_t rs2 = decoderWild(ID_EX.IR);
		ID_EX.B = CURRENT_STATE.REGS[rs2];
		if(forwardB != 0) {
			ID_EX.B = forwardB;
			forwardB = 0;
		}
	} else {
		rs1 = decoderRS1(ID_EX.IR);
		uint32_t immediate = decoderWild(ID_EX.IR);
		ID_EX.A = CURRENT_STATE.REGS[rs1];
		if(forwardA != 0) {
			ID_EX.A = forwardA;
			forwardA = 0;
		}
        ID_EX.imm = immediate;
	}
	}
	return;
}

/************************************************************/
/* instruction fetch (IF) pipeline stage:                                                              */
/************************************************************/
void IF()
{
	if(EX_MEM.RegWrite && (decoderRD(EX_MEM.IR) != 0) &&
	(decoderRD(EX_MEM.IR) == decoderRS1(ID_EX.IR))) {
		printf("%x,%x,%x\n",EX_MEM.RegWrite,decoderRD(EX_MEM.IR), decoderRS1(ID_EX.IR));
		//Stall
		hazard = true;
	}
	else if(EX_MEM.RegWrite && (decoderRD(EX_MEM.IR) != 0) &&
	(decoderRD(EX_MEM.IR) == decoderWild(ID_EX.IR))) {
		//Stall
		hazard = true;
	}
	else if(MEM_WB.RegWrite && (decoderRD(MEM_WB.IR) != 0) &&
	(decoderRD(MEM_WB.IR) == decoderRS1(ID_EX.IR))) {
		//Stall
		hazard = true;
	}
	else if(MEM_WB.RegWrite && (decoderRD(MEM_WB.IR) != 0) &&
	(decoderRD(MEM_WB.IR) == decoderWild(ID_EX.IR))) {
		//Stall
		hazard = true;
	} else {
	hazard = false;
	IF_ID.IR = mem_read_32(CURRENT_STATE.PC);
	IF_ID.PC = CURRENT_STATE.PC;
	}
	return;
}


/************************************************************/
/* Initialize Memory                                                                                                    */
/************************************************************/
void initialize() {
	init_memory();
	CURRENT_STATE.PC = MEM_TEXT_BEGIN;
	NEXT_STATE = CURRENT_STATE;
	NEXT_STATE.PC += 4;
	RUN_FLAG = TRUE;
}

/************************************************************/
/* Print the program loaded into memory (in RISCV assembly format)    */
/************************************************************/
void print_program(){
	/*IMPLEMENT THIS*/
}

/************************************************************/
/* Print the current pipeline                               */
/************************************************************/
void show_pipeline(){
	/*IMPLEMENT THIS*/
	printf("PipeLine execution: %d\n",PIPE_EXECUTE);
	printf("Current PC  	%x\n", CURRENT_STATE.PC);
	if(strcmp(decoderOP(IF_ID.IR),"bad") != 0) {
		printf("IF/ID.IR    	%x %s x%d, x%d, x%d\n", IF_ID.IR, decoderOP(IF_ID.IR), decoderRD(IF_ID.IR), decoderRS1(IF_ID.IR), decoderWild(IF_ID.IR));
	} else {
		printf("IF/ID.IR\n");
	}
	printf("IF/ID.PC	%x\n\n", IF_ID.PC);
	if(strcmp(decoderOP(ID_EX.IR),"bad") != 0) {
		printf("ID/EX.IR	%x %s x%d, x%d, x%d\n", ID_EX.IR, decoderOP(ID_EX.IR), decoderRD(ID_EX.IR), decoderRS1(ID_EX.IR), decoderWild(IF_ID.IR));
	} else {
		printf("ID/EX.IR	0\n");
	}
	printf("ID/EX.A		%x\n", ID_EX.A);
	printf("ID/EX.B		%x\n", ID_EX.B);
	printf("ID/EX.imm	%x\n\n", ID_EX.imm);
	printf("EX/MEM.IR	%x\n", EX_MEM.IR);
	printf("EX/MEM.A	%x\n", EX_MEM.A);
	printf("EX/MEM.B	%x\n", EX_MEM.B);
	printf("EX/MEM.ALUOutput%x\n\n", EX_MEM.ALUOutput);
	printf("MEM/WB.IR	%x\n", MEM_WB.IR);
	printf("MEM/WB.ALUOutput%x\n", MEM_WB.ALUOutput);
	printf("MEM/WB.LMD	%x\n\n", MEM_WB.LMD);
	PIPE_EXECUTE++;
}

/***************************************************************/
/* main                                                                                                                                   */
/***************************************************************/
int main(int argc, char *argv[]) {
	printf("\n**************************\n");
	printf("Welcome to MU-RISCV SIM...\n");
	printf("**************************\n\n");

	if (argc < 2) {
		printf("Error: You should provide input file.\nUsage: %s <input program> \n\n",  argv[0]);
		exit(1);
	}

	strcpy(prog_file, argv[1]);
	initialize();
	load_program();
	help();
	while (1){
		handle_command();
	}
	return 0;
}
