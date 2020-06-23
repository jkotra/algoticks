#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../include/dtypes.h"
#include "../include/csvutils.h"
#include "../include/debug.h"
#include "../include/misc.h"

int skip_header = true;

algoticks_row tokenize_row(char *row){

    char *token;
    struct Row data;
    int row_pos = 0;

    token = strtok(row, ",");

    while (token != NULL)
    {

        if (row_pos == 0)
        {
            strncpy(data.date, token, 32);
        }
        else if (row_pos == 1)
        {
            data.open = atof(token);
        }
        else if (row_pos == 2)
        {
            data.high = atof(token);
        }
        else if (row_pos == 3)
        {
            data.low = atof(token);
        }
        else if (row_pos == 4)
        {
            data.close = atof(token);
        }
        else if (row_pos == 5)
        {
            data.volume = atoi(token);
        }
        else
        {
            //unknown row position.
            break;
        }

        row_pos++;
        token = strtok(NULL, ",");
    }

    return data;

}

int read_csv(FILE *fp, algoticks_row *storage, int seek_offset, algoticks_config config, int debug){

    if (feof(fp) == true)
    {
        return EOF;
    }

    if (seek_offset != 0)
    {
        fseek(fp, seek_offset, SEEK_SET);
    }



    // temp. storage array(s)
    char row[MAXCHARPERLINE];
    int curr_sp;

    while(1) {
    


    //get the row
    fgets(row, MAXCHARPERLINE, fp);


    if ((config.skip_header == true) && (skip_header == true)){
        skip_header = false;
        continue;
    }


    //remove white space at end
    chomp(row);

    curr_sp = ftell(fp);

    *storage = tokenize_row(row);
    
    return curr_sp;

    }

}