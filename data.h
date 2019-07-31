#ifndef DATA_H

#define COMB_OFFSET 0
#define DREG_OFFSET 2
#define DADDR_OFFSET 5
#define SREG_OFFSET 7
#define SADDR_OFFSET 10
#define OPCODE_OFFSET 12
#define TYPE_OFFSET 16
#define DBL_OFFSET 17
#define RSVD_OFFSET 18

#define MAX_SIZE 1000
#define LINE_OFFSET 99
#define NAME_SIZE 20
#define LZ 1
#define NO_LZ 0

#define MAX_LINE_LEN 80
#define BUFFER 100
#define FILE_NAME_SIZE 25

/***************************
 Required base by a project
****************************/
#define BASE 8
#define NUMBER_OF_DIGITS 7 /*Max number for the base*/

#define COMB_WIDTH 2     /*bits 0-1*/
#define DREG_WIDTH 3     /*bits 2-4*/
#define DADDR_WIDTH 2    /*bits 5-6*/
#define SREG_WIDTH 3     /*bits 7-9*/
#define SADDR_WIDTH 2	 /*bits 10-11*/
#define OPCODE_WIDTH 4	 /*bits 12-15*/
#define TYPE_WIDTH 1     /*bits 16*/
#define DBL_WIDTH 1      /*bits 17*/
#define RSVD_WIDTH 2     /*bits 18-19*/

#define MOV 0
#define CMP 1
#define ADD 2
#define SUB 3
#define NOT 4
#define CLR 5
#define LEA 6
#define INC 7
#define DEC 8
#define JMP 9
#define BNE 10
#define RED 11
#define PRN 12
#define JSR 13
#define RTS 14
#define STOP 15

#define ERROR(MSG,LINE) fprintf(stderr, "Error occured in the line: %d- %s\n", LINE, MSG);

/****************************
 Bitfieled code line struct.
*****************************/
typedef struct bit_code_line {
    unsigned int comb      : COMB_WIDTH; /*bits 0-1*/
    unsigned int dest_reg  : DREG_WIDTH; /*bits 2-4*/
    unsigned int dest_addr : DADDR_WIDTH; /*bits 5-6*/
    unsigned int src_reg   : SREG_WIDTH; /*bits 7-9*/
    unsigned int src_addr  : SADDR_WIDTH; /*bits 10-11*/
    unsigned int opcode    : OPCODE_WIDTH; /*bits 12-15*/
    unsigned int type      : TYPE_WIDTH; /*bits 16*/
    unsigned int dbl       : DBL_WIDTH; /*bits 17*/
    unsigned int rsvd      : RSVD_WIDTH; /*bits 18-19*/
} instruction_line;

/*******************
 Full line struct.
*******************/
typedef struct completed_line
{
    char *line_str;
    instruction_line *instruction;
    unsigned int line_number;
    char *src_opr;
    char *dest_opr;
    char done_flag;
} comp_line;

typedef enum {FALSE,TRUE} boolean;

/**************************************************
 Word size as defined in the project instructions.
***************************************************/
typedef struct word
{
    unsigned int data : 20;
} dword;

/***************************************************
 First run on the document as defined by a project.
****************************************************/
int run1(comp_line *file, int num_of_lines);

/***************************************************
 Second run on the document as defined by a project.
****************************************************/
int run2(comp_line *file, int num_of_lines, char *input_fileName);

/********************************************************
 Innitializing of the hashtable as described in project.
********************************************************/
void initialize_table();

/*******************************************************************
 Function to duplicating the string and returning address with str.
*******************************************************************/
char *duplicate_str(const char *str);

/**************************************************
 Helper function to remove spaces from the string.
**************************************************/
void removeSpaces(char **str);

/***************************************************
 Helper function to convert number to another base.
****************************************************/
char *base_to_base_con(int num, int base, char *res, int lead_zero);

/*********************************************
 Helper function to convert char to dataline.
**********************************************/
dword from_char_to_data(char ch);

/************************
 Dataline initiolizator.
************************/
void init_data_line(dword *dword);

/*****************************
 Function creats binary code.
*****************************/
dword from_inst_to_data(instruction_line inst_line);

/***********************************************************
 Helper function to return converted number to binary code.
************************************************************/
dword num_to_data(int num);

/**********************************************
 Helper function to clear instruction command.
***********************************************/
void init_instruction_line(instruction_line *dword);

/********************************************
 Helper function to make sure address is ok.
********************************************/
void address_checker(instruction_line *inst_line, int line_number);

#endif