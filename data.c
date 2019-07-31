#include<stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "data.h"

/*************************
 Masks for data shifting.
*************************/
unsigned char masks_def[] = {0x0, 0x1, 0x3, 0x7, 0xF, 0x1F, 0x3F, 0x7F, 0xFF};

/*********************************************
 Helper function to convert char to dataline.
**********************************************/
dword from_char_to_data(char ch)
{
    dword temp;
    init_data_line(&temp);
    temp.data = ch;
    return temp;
}

/*******************************************************************
 Function to duplicating the string and returning address with str.
*******************************************************************/
char *duplicate_str(const char *str)
{
    int size = strlen(str) + 1;
    char *nString =(char*)malloc(size);

    if(!nString)
    {
        printf("Ooops Error occured: memory failed to be allocated (func-data.c->duplicate_str)\n");
        return NULL;
    }
    else
        strcpy(nString, str);

    return nString;
}

/**************************************************
 Helper function to remove spaces from the string.
**************************************************/
void removeSpaces(char **str)
{
    while(((**str)==' ' || (**str)=='\t')&& (**str != '\n'))
        (*str)++;
}

/*****************************
 Function creats binary code.
*****************************/
dword from_inst_to_data(instruction_line inst_line)
{
    dword tmp;
    init_data_line(&tmp);

    tmp.data |= (masks_def[COMB_WIDTH]   << COMB_OFFSET)   & (inst_line.comb << COMB_OFFSET);
    tmp.data |= (masks_def[DREG_WIDTH]   << DREG_OFFSET)   & (inst_line.dest_reg << DREG_OFFSET);
    tmp.data |= (masks_def[DADDR_WIDTH]  << DADDR_OFFSET)  & (inst_line.dest_addr << DADDR_OFFSET);
    tmp.data |= (masks_def[SREG_WIDTH]   << SREG_OFFSET)   & (inst_line.src_reg << SREG_OFFSET);
    tmp.data |= (masks_def[SADDR_WIDTH]  << SADDR_OFFSET)  & (inst_line.src_addr << SADDR_OFFSET);
    tmp.data |= (masks_def[OPCODE_WIDTH] << OPCODE_OFFSET) & (inst_line.opcode << OPCODE_OFFSET);
    tmp.data |= (masks_def[TYPE_WIDTH]   << TYPE_OFFSET)   & (inst_line.type << TYPE_OFFSET);
    tmp.data |= (masks_def[DBL_WIDTH]    << DBL_OFFSET)    & (inst_line.dbl << DBL_OFFSET);
    tmp.data |= (masks_def[RSVD_WIDTH]   << RSVD_OFFSET)   & (inst_line.rsvd << RSVD_OFFSET);
    return tmp;
}

/***********************************************************
 Helper function to return converted number to binary code.
************************************************************/
dword num_to_data(int num)
{
    unsigned int shift_bits_maks = 0x7FFFF;/*19 hex*/
    dword temp;

    init_data_line(&temp);

    if (num >= 0)
        temp.data = num & shift_bits_maks;

    else
    {
        temp.data = (num * -1) & shift_bits_maks;
        temp.data = ~(temp.data);
        (temp.data)++;
    }
    return temp;
}

/**********************************************
 Helper function to clear instruction command.
***********************************************/
void init_instruction_line(instruction_line *dword)
{
    dword->comb = 0;
    dword->dest_reg = 0;
    dword->dest_addr = 0;
    dword->src_reg = 0;
    dword->src_addr = 0;
    dword->opcode = 0;
    dword->type = 0;
    dword->dbl = 0;
    dword->rsvd = 0;
}

/************************
 Dataline initiolizator.
************************/
void init_data_line(dword *dword)
{
    dword->data = 0;
}