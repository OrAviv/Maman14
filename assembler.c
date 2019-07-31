#include <stdio.h>
#include <stdlib.h>
#include "hashtable.h"
#include "data.h"

int main(int argc, char **argv)
{
    int arg_num;
    char fileName[FILE_NAME_SIZE];
    FILE *fp;

    /******************************
     Initializing of the hashtable
    ******************************/
    initialize_table();

    /***********************************************************
        Run on all priveded arguments and execute the assembly.
    ************************************************************/
    for (arg_num = 1; arg_num < argc; arg_num++)
    {
        int lineNum, buf = BUFFER;
        comp_line *file_lines, *temp;

        sprintf(fileName, "%s.as", argv[arg_num]);

        /***********************
		 Open file for reading.
		***********************/
        fp = fopen(fileName, "r");

        /*****************************
         Tester for file appeareance.
        *****************************/
        if (fp == NULL)
        {
            printf("Oops Error occured the file specified is missing: %s\n", fileName);
            return 0;
        }

        file_lines = (comp_line*) malloc(sizeof(comp_line) * BUFFER);
        lineNum = 0;
        file_lines[lineNum].line_str =(char*) malloc(sizeof(char) * MAX_LINE_LEN);

        /*************************************************************
         Read each line from the provided file and put into array.
        **************************************************************/
        while (fgets(file_lines[lineNum].line_str, MAX_LINE_LEN, fp))
        {

            /************************
             If more memmory needed.
            ************************/
            if (lineNum == buf)
            {
                buf += BUFFER;
                temp =(comp_line*) realloc(file_lines, sizeof(comp_line) * buf);

                if (temp)
                    file_lines = temp;

                else
                {
                    printf("Opps Error occured: The memory cannot be relocated hence closing.\n");
                    return 0;
                }
            }
            file_lines[lineNum].line_number = lineNum + 1;
            file_lines[++lineNum].line_str = malloc(sizeof(char) * MAX_LINE_LEN);
        }

        free(file_lines[lineNum].line_str);

        /************************************
         Perform two runs on the readed file
        *************************************/
        run1(file_lines, lineNum);
        run2(file_lines, lineNum, argv[arg_num]);

        /******************************************
         Free memory allocated and close the file.
        ******************************************/
        free(file_lines);
        fclose(fp);

    }
    return 0;
}