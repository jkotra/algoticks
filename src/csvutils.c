#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/stat.h>
#include "../include/dtypes.h"
#include "../include/csvutils.h"
#include "../include/debug.h"
#include "../include/misc.h"

int is_header_skipped = false;

/*
header_template holds the values that program expects to find in header of csv.
header_map is initially set to -1 (ln ~139)

Example:
date,open,close,volume,high,low

0->0 -> header_template[0]="date"
1->1 -> header_template[1]="open"
2->4 -> header_template[2]="high"
3->5 -> header_template[3]="low"
4->2 -> header_template[4]="close"
5->3 -> header_template[5]="volume"

header_map = {0,1,4,5,2,3,(Optional technical_indicator n)}

technical_indicators is optional! it is by default set to None.
*/

char header_template[MAXCSVHEAD][24] = {"date", "open", "high", "low", "close", "volume", "ti1", "ti2", "ti3", "ti_others"};
int header_map[MAXCSVHEAD] = {0};

struct stat stat_info;
unsigned int datasource_lastmodified = -1;

int check_row_integrity(algoticks_row row){
    //return true if all ok else false if close is 0 or date is NULL
    if (row.close == 0 || row.date == NULL){
        return false;
    }
    else{
        return true;
    }
}

int change_in_modified_date(char* filename){


        if (stat(filename, &stat_info) != 1){
            
            //if datasource_lastmodified is -1 then it's initial check, update last it to last modfied
        if (datasource_lastmodified == -1) { 
            datasource_lastmodified = stat_info.st_mtime;
            return false; 
        }
        
        // if file modified
        if (datasource_lastmodified != stat_info.st_mtime){

            //update var.
            datasource_lastmodified = stat_info.st_mtime;
            return true;
        }
        
    }

    return false;
}

int reopen_datasource(char* filename, FILE** fp){

    if (freopen(filename, "rb", *fp) != NULL){
        return true;
    }
    else{
        return false;
    }

}

// this checks if the first char of given string starts with quote.
int is_quoted(char *str){
    if ((str[0] == '"') || (str[0] == '\'')){
        return true;
    }else{
        return false;
    }
}

algoticks_row tokenize_row(char *row){

    char *token;
    struct Row data;
    memset(&data, 0, sizeof(data));
    strncpy(data.technical_indicators.ti_others, "None", 2048);

    int row_pos = 0;
    int header_i = 0;

    token = strtok(row, ",");

    
    while (token != NULL && header_i < MAXCSVHEAD)
    {
        
        if (header_map[header_i] == -1){
            header_i++;
            continue;
        }

        //for debugging
        //printf("header_i: %d header_map: %d header_template: %s\n",header_i, header_map[header_i], header_template[header_map[header_i]]);

        if (strcmp(header_template[header_map[header_i]],"date") == 0)
        {
            if (is_quoted(token) == true) { remove_quotes(token); }
            strncpy(data.date, token, 32);
        }
        else if (strcmp(header_template[header_map[header_i]],"open") == 0)
        {
            if (is_quoted(token) == true) { remove_quotes(token); }
            data.open = atof(token);
        }
        else if (strcmp(header_template[header_map[header_i]],"high") == 0)
        {
            if (is_quoted(token) == true) { remove_quotes(token); }
            data.high = atof(token);
        }
        else if (strcmp(header_template[header_map[header_i]],"low") == 0)
        {
            if (is_quoted(token) == true) { remove_quotes(token); }
            data.low = atof(token);
        }
        else if (strcmp(header_template[header_map[header_i]],"close") == 0)
        {
            if (is_quoted(token) == true) { remove_quotes(token); }
            data.close = atof(token);
        }
        else if (strcmp(header_template[header_map[header_i]],"volume") == 0)
        {
            if (is_quoted(token) == true) { remove_quotes(token); }
            data.volume = atoi(token);
        }

        //technical indicators
        else if (strcmp(header_template[header_map[header_i]],"ti1") == 0)
        {
            if (is_quoted(token) == true) { remove_quotes(token); }
            data.technical_indicators.is_ti1_p = true;
            data.technical_indicators.ti1 = atof(token);
        }
        else if (strcmp(header_template[header_map[header_i]],"ti2") == 0)
        {
            if (is_quoted(token) == true) { remove_quotes(token); }
            data.technical_indicators.is_ti2_p = true;
            data.technical_indicators.ti2 = atof(token);
        }
        else if (strcmp(header_template[header_map[header_i]],"ti3") == 0)
        {
            if (is_quoted(token) == true) { remove_quotes(token); }
            data.technical_indicators.is_ti3_p = true;
            data.technical_indicators.ti3 = atof(token);
        }
        else if (strcmp(header_template[header_map[header_i]],"ti_others") == 0)
        {
            
            if (is_quoted(token) == true) { remove_quotes(token); }
            data.technical_indicators.is_ti_others_p = true;
            strncpy(data.technical_indicators.ti_others, token,2048);
        }
        else
        {
            //unknown row position.
            break;
        }

        row_pos++;
        header_i++;
        token = strtok(NULL, ",");
    }

    return data;

}

int read_csv(algoticks_settings settings,algoticks_config config, FILE *fp, algoticks_row *storage, int seek_offset){

    if (feof(fp))
    {
        if (settings.is_live_data){
            while ((change_in_modified_date(config.datasource) == false))
            {
                printf("checking for new data in %s...\r", config.datasource);
                fflush(stdout);
                }

                //since we are using fgets() the new data written to data source MUST end with new line (\n)
                //reopen
                reopen_datasource(config.datasource, &fp);

                //set_seek
                fseek(fp, seek_offset, SEEK_SET);

                debug_msg(settings, 2, "DatasourceReopen","csvutils.c", config.datasource);

        }else {
        return EOF;
        }
    }

    if (seek_offset != 0)
    {
        fseek(fp, seek_offset, SEEK_SET);
    }



    // temp. storage array(s)
    char row[MAXCHARPERLINE];
    int curr_sp;
    

    while(true) {

    //interval in config
    if ((config.interval > 0) == true && is_header_skipped == true){
        for (int i = 0; i < config.interval; i++)
        {
            fgets(row, MAXCHARPERLINE, fp);
        }
    }
    

    //get the row
    fgets(row, MAXCHARPERLINE, fp);


    if ((config.skip_header == true) && (is_header_skipped == false)){

        char *token;
        token = strtok(row, ",");
        int header_i = 0;

        //set header_map to -1
        for (int i = 0; i <= MAXCSVHEAD; i++)
        {
            header_map[i] = -1;
        }

        while (token != NULL && header_i < MAXCSVHEAD)
        {
            chomp(token);
            convert_to_lowercase(token);

            debug_msg(settings, 2, "ReadHeader","csvutils.c",token);

            /*
            "date" = 0,
            "open" = 1,
            "high" = 2,
            "low" = 3,
            "close" = 4,
            "volume" = 5,

            "ti1" = 6,
            "ti2" = 7,
            "ti3" = 9,
            "ti_others" = 10
            */

            // Date
            if(strcmp(token, "date") == 0){ 
                header_map[header_i] = 0; }

            else if(strcmp(token, "open") == 0){ 
                header_map[header_i] = 1; }

            else if(strcmp(token, "high") == 0){ 
                header_map[header_i] = 2; }

            else if(strcmp(token, "low") == 0){ 

                header_map[header_i] = 3; }

            else if(strcmp(token, "close") == 0){ 

                header_map[header_i] = 4; }

            else if(strcmp(token, "volume") == 0){ 
                header_map[header_i] = 5; }

            else if(strcmp(token, "ti1") == 0){ 
                header_map[header_i] = 6; }

            else if(strcmp(token, "ti2") == 0){ 
                header_map[header_i] = 7; }

            else if(strcmp(token, "ti3") == 0){ 
                header_map[header_i] = 8; }

            else if(strcmp(token, "ti_others") == 0){ 
                header_map[header_i] = 9; }            

            else { printf("Unknown header element: %s\n", token);
                header_map[header_i] = -1;
                }

            header_i++;
            token = strtok(NULL, ",");
        }


        /* DEBUG 
        for (int i = 0; i < MAXCSVHEAD; i++)
        {
            printf("i:%d %d->%s\n",i, header_map[i], header_template[header_map[i]]);
        }
        */
        
        
        //skip the 1st row i.e header
        is_header_skipped = true;
        continue;
    }
    else if (config.skip_header == false){
        for (int i = 0; i < MAXCSVHEAD; i++)
        {
            header_map[i] = -1;
        }

        //set to ohlcv
        for (int i = 0; i < 5; i++)
        {
            header_map[i] = i;
        }
        
    }


    //remove white space at end
    chomp(row);

    curr_sp = ftell(fp);

    *storage = tokenize_row(row);
    storage->curr = curr_sp;

    
    return curr_sp;

    }

}