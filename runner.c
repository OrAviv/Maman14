#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "data.h"
#include "hashtable.h"

/*Helper verible definitions for file runner.*/
static hash_node *opNode[HASHSIZE];
hash_node *tmp_node; /*Temp helper node.*/
dword array_data[MAX_SIZE]; /*Array for a data.*/
int entry_counter, extern_counter;
int i, instructionCounter, dataCounter; /*Helper Counters for instruction and data.*/
char *array_entry[MAX_SIZE];
boolean error_flag = FALSE, extern_flag = FALSE, entry_flag = FALSE; /*Helper flags*/

/*********************************************
 Helper function to find symbol in the line.
 Return: pointer to the symbbol.
*********************************************/
char *getSymbol(comp_line *full_l)
{
    char *tmp = (full_l->line_str);

    while (*(full_l->line_str) != '\0')
    {
        if (*(full_l->line_str) == ':' )
        {
            *((full_l->line_str)++) = '\0';

            /*Check the symbol*/
            if (!isalpha(tmp[0]))
            {
                ERROR("Error: The symbol must start by char", full_l->line_number)
                error_flag = TRUE;
            }
            return tmp;
        }

        (full_l->line_str)++;
    }

    full_l->line_str = tmp; /*Set pointer to the line beggining.*/

    /*In case of symbol missing*/
    return 0;

}

/********************************************************
 Innitializing of the hashtable as described in project.
********************************************************/
void initialize_table()
{
    tmp_node = add_node("mov" , MOV , 0 , opNode);
    tmp_node = add_node("cmp" , CMP , 0 , opNode);
    tmp_node = add_node("add" , ADD , 0 , opNode);
    tmp_node = add_node("sub" , SUB , 0 , opNode);
    tmp_node = add_node("not" , NOT , 0 , opNode);
    tmp_node = add_node("clr" , CLR , 0 , opNode);
    tmp_node = add_node("lea" , LEA , 0 , opNode);
    tmp_node = add_node("inc" , INC , 0 , opNode);
    tmp_node = add_node("dec" , DEC , 0 , opNode);
    tmp_node = add_node("jmp" , JMP , 0 , opNode);
    tmp_node = add_node("bne" , BNE , 0 , opNode);
    tmp_node = add_node("red" , RED , 0 , opNode);
    tmp_node = add_node("prn" , PRN , 0 , opNode);
    tmp_node = add_node("jsr" , JSR , 0 , opNode);
    tmp_node = add_node("rts" , RTS , 0 , opNode);
    tmp_node = add_node("stop" , STOP , 0 , opNode);
}

/********************************************************
 Helper function to extract .STRING from the code lines.
*********************************************************/
void stringExtractor(comp_line *code_line)
{
    /*Remove spaces.*/
    removeSpaces(&(code_line->line_str));
    (code_line->line_str)++;

    /*Get second sign loop.*/
    while ((code_line->line_str[0] != '\"') && (code_line->line_str[0] != '\0'))
    {
        array_data[dataCounter++] = from_char_to_data(code_line->line_str[0]);
        (code_line->line_str)++;
    }

    if (code_line->line_str[0] == '\"')
        array_data[dataCounter++] = from_char_to_data('\0');

    else
    {
        ERROR("Error: .STRING must end with \" sign", (*code_line).line_number)
        error_flag = TRUE;
        return;
    }
}

/********************************************************
 Helper function to extract defines data.
*********************************************************/
void defineExtractor(comp_line *code_line)
{
    int sign;
    int sum;

    /* LOOP TILL THE END OF LINE */
    while ((*code_line).line_str[0] != '\n') {
        while((*code_line).line_str[0] != '=')
            (code_line->line_str)++;
        (code_line->line_str)++;
        removeSpaces(&((*code_line).line_str));

        if (code_line->line_str[0] == '-') {
            sign = -1;
            (code_line->line_str)++;
        }


        sum = 0;
        sign = 1;
        if(isdigit((*code_line).line_str[0]))
        {
            while (isdigit((*code_line).line_str[0]))
            {
                sum *= 10;
                sum += ((*code_line).line_str[0]) - '0';
                (code_line->line_str)++;
            }
            array_data[dataCounter++] = num_to_data(sum * sign);
        }

    }
}


/******************************************************
 Helper function to extract .DATA from the code lines.
*******************************************************/
void dataExtractor(comp_line *code_line)
{
    int sign;
    int sum;
    char expecting_number = 0;

    /* LOOP TILL THE END OF LINE */
    while ((*code_line).line_str[0] != '\n')
    {
        removeSpaces(&((*code_line).line_str));

        if (code_line->line_str[0] == '-')
        {
            sign = -1;
            (code_line->line_str)++;
        }


            sum = 0;
            sign = 1;
            if(isdigit((*code_line).line_str[0]))
            {
                while (isdigit((*code_line).line_str[0]))
                {
                    sum *= 10;
                    sum += ((*code_line).line_str[0]) - '0';
                    (code_line->line_str)++;
                }

                array_data[dataCounter++] = num_to_data(sum * sign);
            }
            else
            {
                while ((code_line->line_str[0] != '\0') && (code_line->line_str[0] == ','))
                {
                    array_data[dataCounter++] = from_char_to_data(code_line->line_str[0]);
                    (code_line->line_str)++;
                }

                array_data[dataCounter++] = from_char_to_data('\0');
                (code_line->line_str)++;

            }

        /*Remove spaces.*/
        removeSpaces(&((*code_line).line_str));

        if (code_line->line_str[0] == ',')
        {
            expecting_number = 1;
            (code_line->line_str)++;
        }
        else
            expecting_number = 0;

    }

    if (expecting_number == 1)
    {
        ERROR("Error: expecting for number after , sign", (*code_line).line_number)
        error_flag = TRUE;
    }
}

/*************************************************************** 
 Helper function to extract .ENTRY command from the code lines.
****************************************************************/
void entryExtractor(comp_line *code_line)
{
    code_line->line_str = strtok(code_line->line_str, " \t\n");

    /*In case of no .ENTRY command available in the code.*/
    if (!code_line->line_str)
    {
        ERROR("Error: expect for .ENTRY command.", (*code_line).line_number)
        error_flag = TRUE;
        return;
    }

    else if(code_line->line_str[0]<'A' || code_line->line_str[0]>'Z')
    {
        ERROR("Error: symbol cannot start with sign.", (*code_line).line_number)
        error_flag = TRUE;
        return;
    }

    array_entry[entry_counter++] = duplicate_str(code_line->line_str);
    code_line->line_str = strtok(NULL, " \t\n");

    if (code_line->line_str)
    {
        ERROR("Error: extra words in the command end.", (*code_line).line_number)
        error_flag = TRUE;
    }
}

/*************************************************************** 
 Helper function to extract .EXTERN command from the code lines.
****************************************************************/
void externExtractor(comp_line *code_line)
{
    /*Skip the spaces*/
    code_line->line_str = strtok(code_line->line_str, " \t\n");

    /*If no value available after extern*/
    if (!code_line->line_str)
    {
        ERROR("Error: expecting for value after .EXTERN command.", (*code_line).line_number)
        error_flag = TRUE;
        return;
    }

    else if(code_line->line_str[0]<'A' || code_line->line_str[0]>'Z')
    {
        ERROR("Symbol cannot start with sign.", (*code_line).line_number)
        error_flag = TRUE;
        return;
    }

    add_node(code_line->line_str, '0',0, extern_list);
    extern_counter++;
    code_line->line_str = strtok(NULL, " \t\n");

    if (code_line->line_str)
    {
        ERROR("Error: Some extra words in the command found.", (*code_line).line_number)
        error_flag = TRUE;
    }
}

/******************************************************************************
 Function to enterpet the code line commands: .STRING, .DATA, .ENTRY, .EXTERN
*******************************************************************************/
void getCommand(comp_line *code_line, char *symbol)
{
    /*If symbol is .STRING*/
    if (strncmp(code_line->line_str, ".string", sizeof(".string") - 1) == 0)
    {
        if (symbol)
        {
            tmp_node = add_node(symbol, dataCounter,0,data_list);
        }
        code_line->line_str += (sizeof(".string") - 1);
        stringExtractor(code_line);
    }

        /*If symbol is .DATA*/
    else if (strncmp((*code_line).line_str, ".data", sizeof(".data") - 1) == 0)
    {
        if (symbol)
            tmp_node = add_node(symbol, dataCounter,0, data_list);

        code_line->line_str += (sizeof(".data") - 1);
        dataExtractor(code_line);
    }

        /*If symbol is .ENTRY*/
    else if (strncmp(code_line->line_str, ".entry", sizeof(".entry") - 1) == 0)
    {
        code_line->line_str += (sizeof(".entry") - 1);
        entryExtractor(code_line);
    }

        /*If symbol is .EXTERN*/
    else if (strncmp(code_line->line_str, ".extern", sizeof(".extern") - 1) == 0)
    {
        code_line->line_str += (sizeof(".extern") - 1);
        externExtractor(code_line);
    }
    /*/if symbol is .define*/
    else if (strncmp(code_line->line_str, ".define", sizeof(".define") - 1) == 0)
    {
        code_line->line_str += (sizeof(".define") - 1);
        defineExtractor(code_line);
    }
    else
    {
        ERROR("The command is not exists", (*code_line).line_number)
        error_flag = TRUE;
    }
    code_line->done_flag = 1;
}

/*********************************
 Helper operand counter function.
**********************************/
char operand_per_opcode(char *code_t)
{
    if (strcmp(code_t, "rts") == 0 || strcmp(code_t, "stop") == 0)
        return 0;

    else if (strcmp(code_t, "mov") == 0 || strcmp(code_t, "cmp") == 0 || strcmp(code_t, "add") == 0 || strcmp(code_t, "sub") == 0 || strcmp(code_t, "lea") == 0)
        return 2;

        /*not,clr,inc,dec,jmp,bne,red,prn,jsr second group.*/
    else
        return 1;
}

/*****************************************************************
 Function to get pointer to a line and extract whole instruction.
 Ruturn num of instruction words.
******************************************************************/
int getInstruction(comp_line *code_line)
{
    int instrCounter = 0;
    char num_of_exp_operands;

    removeSpaces(&(code_line->line_str));
    code_line->line_str= strtok(code_line->line_str, " \t\n");
    tmp_node = search_node(code_line->line_str, opNode);

    /*If this is not legal code.*/
    if (!tmp_node)
    {
        if ((code_line->line_str[0] == ';') || (code_line->line_str[0] == '\n'))
        {
            code_line->done_flag = 1;
            return 0;
        }
        else if(code_line->line_str!=NULL)
        {
            printf("%s Error: wrong instruction detected in line: %d\n", code_line->line_str,(*code_line).line_number);
            code_line->done_flag = 1;
            error_flag = TRUE;
            return 0;
        }
    }

    num_of_exp_operands = operand_per_opcode(code_line->line_str);
    code_line->instruction = (instruction_line *) malloc(sizeof(instruction_line) * 8);

    /*Tester for memory allocation.*/
    if(code_line->instruction != NULL)
    {
        init_instruction_line(code_line->instruction);
        instrCounter++;
        code_line->instruction->opcode = tmp_node->num;
    }
    else
    {
        printf("Error: memory cannot be allocated.\n");
        return 0;
    }

    /*Scan code lines for "\0"*/
    while (code_line->line_str[0] != '\0')
    {
        (code_line->line_str)++;
    }

//    (code_line->line_str)++;
    removeSpaces(&(code_line->line_str));

    code_line->instruction->dbl = code_line->line_str[0] - '0';
    (code_line->line_str)++;
    removeSpaces(&(code_line->line_str));
    code_line->line_str = strtok(code_line->line_str, " \t,\n");

    while (num_of_exp_operands-- > 0)
    {
        if (num_of_exp_operands == 1)
            code_line->src_opr = duplicate_str(code_line->line_str);
        else
            code_line->dest_opr = duplicate_str(code_line->line_str);

        switch (code_line->line_str[0])
        {
            case '#':
                instrCounter++;
                if (num_of_exp_operands == 1)
                    code_line->instruction->src_addr = 0;
                else
                    code_line->instruction->dest_addr = 0;
                break;
            case 'r':
                (code_line->line_str)++;

                /*Register number checker.*/
                if ((code_line->line_str[0] >= '0') && (code_line->line_str[0] <='7'))
                {
                    if (num_of_exp_operands == 1)
                    {
                        code_line->instruction->src_reg = code_line->line_str[0] - '0';
                        free(code_line->src_opr);
                        code_line->instruction->src_addr = 3;
                    }
                    else
                    {
                        code_line->instruction->dest_reg = code_line->line_str[0] - '0';
                        free(code_line->dest_opr);
                        code_line->instruction->dest_addr = 3;
                    }
                    break;
                }
                else
                {
                    ERROR("ITS NOT LEGAL REG NUMBER", (*code_line).line_number)
                    error_flag = TRUE;
                }
                break;
            default:
                instrCounter++;

//                if (strstr(code_line->line_str, "{!"))
//                {
//                    instrCounter++;
//
//                    if (num_of_exp_operands == 1)
//                        code_line->instruction->src_addr = 2;
//                    else
//                        code_line->instruction->dest_addr = 2;
//                }
//                else
//                {
                    if (num_of_exp_operands == 1)
                        code_line->instruction->src_addr = 1;
                    else
                        code_line->instruction->dest_addr = 1;
//                }
        }
        code_line->line_str = strtok(NULL, " \t,\n");
    }

    /*Instruction counter update.*/
    instructionCounter += instrCounter;
    return instrCounter;
}

/***************************************************
 First run on the document as defined by a project.
****************************************************/
int run1(comp_line *fp, int linesNum)
{
    char *symbol;
    int mem_words = 0;

    error_flag = FALSE;
    instructionCounter = 0;
    dataCounter = 0;
    extern_counter = 0;
    entry_counter = 0;

    /*Go throught all lines in the PS file*/
    for (i = 0; i < linesNum; i++)
    {
        symbol = getSymbol(fp + i);

        /*Remove spaces*/
        removeSpaces(&(fp[i].line_str));

        if (fp[i].line_str[0] == '.')
            getCommand(&fp[i], symbol);
        else
        {
            mem_words = getInstruction(&fp[i]);
            if (symbol)
                tmp_node = add_node(symbol, instructionCounter - mem_words, mem_words, instructions_list);
        }
    }
    return 0;
}

/***********************************************************************
Helper function for second run for printing data to the suitable files.
************************************************************************/
void operand_extractor(char *opr, int addnum, FILE *ob_fp, FILE *ext_fp, int *line_count, int org_line_num)
{
    char base_result[NUMBER_OF_DIGITS + 1], base_result1[NUMBER_OF_DIGITS + 1];
    char *tmp;
    int sign = 1, sum = 0;

    switch (addnum)
    {
        case 0:
            opr++;
            if (opr[0] == '-')
            {
                sign = -1;
                opr++;
            }
            while (isdigit(opr[0]))
            {
                sum *= 10;
                sum += opr[0] - '0';
                opr++;
            }

            /*Add number to the .ob file*/
            fprintf(ob_fp, "%s\t%s\t%c\n",base_to_base_con((*line_count) + LINE_OFFSET, BASE, base_result, NO_LZ) , base_to_base_con(num_to_data(sum * sign).data, BASE, base_result1, LZ), 'A');
            (*line_count)++;
            break;
        case 1:
            if ((tmp_node = search_node(opr, data_list)))
            {
                /*Write address of a symbol to the .ob file.*/
                fprintf(ob_fp, "%s\t%s\t%c\n", base_to_base_con((*line_count) + LINE_OFFSET, BASE, base_result, NO_LZ), base_to_base_con(num_to_data(tmp_node->num + instructionCounter + LINE_OFFSET + 1).data, BASE, base_result1, LZ), 'R');
                (*line_count)++;
            }

            else if ((tmp_node = search_node(opr, instructions_list)))
            {
                /*Write address of a symbol to the .ob file.*/
                fprintf(ob_fp, "%s\t%s\t%c\n", base_to_base_con((*line_count) + LINE_OFFSET, BASE, base_result, NO_LZ), base_to_base_con(num_to_data(tmp_node->num + LINE_OFFSET + 1).data, BASE, base_result1, LZ), 'R');
                (*line_count)++;
            }
            else if ((tmp_node = search_node(opr, extern_list)))
            {
                fprintf(ob_fp, "%s\t%s\t%c\n", base_to_base_con((*line_count)+ LINE_OFFSET, BASE, base_result, NO_LZ), base_to_base_con(num_to_data(0).data, BASE, base_result1, LZ), 'E');
                /*Write external to the .ext file.*/
                fprintf(ext_fp, "%s\t%s\n", opr, base_to_base_con(*line_count + LINE_OFFSET, BASE, base_result, NO_LZ));
                (*line_count)++;
            }
            else
            {
                ERROR("Error: cannot find symbol.", org_line_num)
                error_flag = TRUE;
            }
            break;
        case 2:
            for (i = 0 ;opr[i] != '{'; i++);
            opr[i] = '\0';
            if ((tmp_node = search_node(opr, data_list)))
            {
                fprintf(ob_fp, "%s\t%s\t%c\n", base_to_base_con((*line_count) + LINE_OFFSET, BASE, base_result, NO_LZ), base_to_base_con(num_to_data(tmp_node->num + instructionCounter + LINE_OFFSET + 1).data, BASE, base_result1, LZ), 'R');
                (*line_count)++;
            }

            else if ((tmp_node = search_node(opr, instructions_list)))
            {
                fprintf(ob_fp, "%s\t%s\t%c\n", base_to_base_con((*line_count) + LINE_OFFSET, BASE, base_result, NO_LZ), base_to_base_con(num_to_data(tmp_node->num + LINE_OFFSET + 1).data, BASE, base_result1, LZ), 'R');
                (*line_count)++;
            }
            else if ((tmp_node = search_node(opr, extern_list)))
            {
                fprintf(ob_fp, "%s\t%s\t%c\n", base_to_base_con((*line_count) + LINE_OFFSET, BASE, base_result, NO_LZ), base_to_base_con(num_to_data(0).data, BASE, base_result1, LZ), 'E');
                fprintf(ext_fp, "%s\t%s\n", opr, base_to_base_con(*line_count + LINE_OFFSET, BASE, base_result, NO_LZ));
                (*line_count)++;
            }
            else
            {
                ERROR("Error: cannot find symbol.", org_line_num)
                error_flag = TRUE;
            }

            opr[i] = '{';
            for (;opr[0] != '!'; opr++);
            opr++;
            tmp = opr;
            while(*tmp != '}' && *tmp !='\n' && *tmp!='\0'){
                tmp++;
            }

            /*Tester for right format.*/
            if (!(tmp[0] == '}'))
            {
                ERROR("Error: wrong dynamic addressing format detected, } is missing", org_line_num)
                error_flag = TRUE;
                break;
            }
                /*Legal format*/
            else
                tmp[0]='\0';

            if ((tmp_node = search_node(opr, instructions_list)))
            {
                fprintf(ob_fp, "%s\t%s\t%c\n", base_to_base_con((*line_count) + LINE_OFFSET, BASE, base_result, NO_LZ), base_to_base_con(tmp_node->mem_words, BASE, base_result1, LZ), 'A');
                (*line_count)++;
            }
            else
            {
                ERROR("Error: symbol format error detected.", org_line_num)
                error_flag = TRUE;
                break;
            }
            *tmp = '}';
            break;
    }
}

/***************************************************
 Second run on the document as defined by a project.
****************************************************/
int run2(comp_line *file, int num_of_lines, char *input_fileName)
{
    char conv_base_a[NUMBER_OF_DIGITS + 1], conv_base_b[NUMBER_OF_DIGITS +1], fileName[NAME_SIZE];/*TEMP ARRAYS FOR CODE SAVING*/
    int i, line_count = 0;
    FILE *ob, *ext, *ent;

    /*Files opening*/
    sprintf(fileName, "%s.ob", input_fileName);
    ob = fopen(fileName, "w");
    sprintf(fileName, "%s.ext", input_fileName);
    ext = fopen(fileName, "w");
    sprintf(fileName, "%s.ent", input_fileName);
    ent = fopen(fileName, "w");

    fprintf(ob, "\t%s   %s\n", base_to_base_con(instructionCounter, BASE, conv_base_a, NO_LZ), base_to_base_con(dataCounter, BASE, conv_base_b, NO_LZ));
    line_count++;

    for (i = 0; i < num_of_lines; i++)
    {
        if (!file[i].done_flag)
        {
            address_checker(file[i].instruction, file[i].line_number);
            fprintf(ob, "%s\t%s\t%c\n",base_to_base_con(line_count + LINE_OFFSET, BASE, conv_base_a, NO_LZ) , base_to_base_con(from_inst_to_data(*(file[i].instruction)).data, BASE, conv_base_b, LZ), 'A');
            line_count++;

            if (file[i].src_opr)
                operand_extractor(file[i].src_opr, file[i].instruction->src_addr, ob, ext, &line_count, file[i].line_number);

            if (file[i].dest_opr)
                operand_extractor(file[i].dest_opr, file[i].instruction->dest_addr, ob, ext, &line_count, file[i].line_number);
        }
    }

    /*Write .ob file*/
    for (i = 0; i < dataCounter; i++)
    {
        fprintf(ob, "%s\t%s\n", base_to_base_con(line_count + LINE_OFFSET, BASE, conv_base_a, NO_LZ), base_to_base_con(array_data[i].data, BASE, conv_base_b, LZ));
        line_count++;
    }

    /*Write .ent file*/
    for (i = 0; array_entry[i]; i++)
    {

        if((tmp_node = search_node(array_entry[i], instructions_list)))
            fprintf(ent, "%s\t%s\n", array_entry[i], base_to_base_con(tmp_node->num + LINE_OFFSET + 1, BASE, conv_base_a, NO_LZ));

        else if ((tmp_node = search_node(array_entry[i], data_list)))
            fprintf(ent, "%s\t%s\n", array_entry[i], base_to_base_con(tmp_node->num + LINE_OFFSET + 1 + instructionCounter, BASE, conv_base_a, NO_LZ));

        else
        {
            fprintf(stderr, "ERROR: CANT FIND ADDRESS FOR THE ENTRY: %s\n", array_entry[i]);
            error_flag = TRUE;
        }
    }

    /*Close all files*/
    fclose(ob);
    fclose(ext);
    fclose(ent);

    /*Hashtable reset*/
    reset_hash_table();

    /*Delete all created files in case of failure*/
    if (error_flag)
    {
        sprintf(fileName, "%s.ob", input_fileName);
        remove(fileName);
        sprintf(fileName, "%s.ext", input_fileName);
        remove(fileName);
        sprintf(fileName, "%s.ent", input_fileName);
        remove(fileName);
    }

        /*Tester if no externs provided.*/
    else if (extern_counter == 0)
    {
        sprintf(fileName, "%s.ext", input_fileName);
        remove(fileName);
    }
        /*Tester if no entrys provided.*/
    else if (entry_counter == 0)
    {
        sprintf(fileName, "%s.ent", input_fileName);
        remove(fileName);
    }
    return 0;
}

/********************************************************* 
 Function to take care of the legal and iligal addresses.
*********************************************************/
void address_checker(instruction_line *inst_line, int line_number)
{
    /*For each Code*/
    switch(inst_line->opcode)
    {
        case MOV:
        case ADD:
        case SUB:
            if (inst_line->dest_addr == 0)
            {
                ERROR("The address in invalid",line_number)
                error_flag = TRUE;
            }
            break;

        case NOT:
        case CLR:
        case INC:
        case DEC:
        case JMP:
        case BNE:
        case RED:
            if ((inst_line->dest_addr == 0) || (inst_line->src_addr != 0))
            {
                ERROR("The address in invalid",line_number)
                error_flag = TRUE;
            }
            break;

        case RTS:
        case STOP:
            if ((inst_line->dest_addr != 0) || (inst_line->src_addr != 0))
            {
                ERROR("The address in invalid",line_number)
                error_flag = TRUE;
            }
            break;

        case LEA:
            if ((inst_line->dest_addr == 0) || (inst_line->src_addr == 0))
            {
                ERROR("The address in invalid",line_number)
                error_flag = TRUE;
            }
            break;

        case PRN:
            if (inst_line->src_addr != 0)
            {
                ERROR("The address in invalid",line_number)
                error_flag = TRUE;
            }
            break;

        case JSR:
            if ((inst_line->dest_addr != 1) || (inst_line->src_addr != 0))
            {
                ERROR("The address in invalid",line_number)
                error_flag = TRUE;
            }
            break;
    }
}

/***************************************************
 Helper function to convert number to another base.
****************************************************/
char *base_to_base_con(int num, int base, char *res, int lead_zero)
{
    int counter = 0, i;
    char temp[NUMBER_OF_DIGITS + 1];

    for (i = 0; i < NUMBER_OF_DIGITS; i++)
    {
        res[i] = '0';
        temp[i] = '0';
    }

    while (num != 0)
    {
        temp[counter] = (num % base) + '0';
        num /= base;
        counter++;
    }

    for (i = 0; i < NUMBER_OF_DIGITS; i++){
        res[i] = temp[NUMBER_OF_DIGITS - 1 - i];
    }

    for (i = 0; i<NUMBER_OF_DIGITS; i++)
    {
        switch (res[i])
        {
            case '0':
                res[i] = SPACIAL_ZERO;
                break;
            case '1':
                res[i] = SPACIAL_ONE;
                break;
            case '2':
                res[i] = SPACIAL_TWO;
                break;
            case '3':
                res[i] = SPACIAL_THREE;
                break;
        }

    }

    res[i] = '\0';/*ADD THE NULL CHAR IN THE END:*/

    if (lead_zero)
        return res;
    else
        return res + NUMBER_OF_DIGITS - counter;
}