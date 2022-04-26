#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/stat.h>
#include "../include/dtypes.h"
#include "../include/csvutils.h"
#include "../include/debug.h"
#include "../include/misc.h"

#include <sys/types.h>

#include <zmq.h>
#include <uv.h>

#include<assert.h>


int is_header_skipped = false;

void reset_header_skip(){
    is_header_skipped = false;
}


int is_socket_init = false;
void* client_d = 0;

void* socket_init(char *port){

    void *context = zmq_ctx_new ();
    void *responder = zmq_socket (context, ZMQ_STREAM);
    char addr[18] = "tcp://127.0.0.1:";
    strcat(addr, port); //addr+port

    int rc = zmq_bind (responder, addr);
    assert (rc == 0);
    printf("* TCP socket listening at %s\n", addr);
    
    return responder;
}

int check_row_integrity(algoticks_row *row){
    //return true if all ok else false if close is 0 or date is NULL
    if (row->close == 0 || row->date == NULL){
        return false;
    }
    else{
        return true;
    }
}

/*
Example Header:
date,open,close,volume,high,low

0->0 -> header_template[0]="date"
1->1 -> header_template[1]="open"
2->4 -> header_template[2]="high"
3->5 -> header_template[3]="low"
4->2 -> header_template[4]="close"
5->3 -> header_template[5]="volume"

header_map = {0,1,4,5,2,3,(Optional: technical_indicator n)}

technical_indicators are optional! it is by default set to NULL.
*/

char header_template[MAXCSVHEAD][10] = {"date", "open", "high", "low", "close", "volume", "ti1", "ti2", "ti3", "ti_others"};
int header_map[MAXCSVHEAD] = {-1};

char *get_relative_header_mapping(int index)
{
    if ( (index < MAXCSVHEAD) && (index != -1) )
    {
        return header_template[header_map[index]];
    }

    return NULL;
}

char *get_header_mapping(int index)
{
    if ( (index < MAXCSVHEAD) && (index != -1) )
    {
        return header_template[index];
    }

    return NULL;
}

bool is_mapped(int key, char *header_name)
{

    char* header = get_relative_header_mapping(key);
    if (!header){
        return NULL;
    }
    return strcmp(header, header_name) == 0;

}


struct stat stat_info;
unsigned int datasource_lastmodified = -1;

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

int reopen_datasource(char* filename, FILE** fp, char* mode){

    if (freopen(filename, mode, *fp) != NULL){
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

    int row_pos = 0;
    int header_i = 0;

    token = strtok(row, ",");

    
    while (token != NULL && header_i < MAXCSVHEAD)
    {
        

        if ( get_relative_header_mapping(header_i) == NULL ){
            header_i++;
            continue;
        }

        #ifdef QUOTED_CHECK
            if (is_quoted(token) == true) { remove_quotes(token); }
        #endif

        if ( is_mapped(header_i, header_template[0]) )
        {
            strncpy(data.date, token, 32);
        }
        else if ( is_mapped(header_i, header_template[1]) )
        {
            data.open = atof(token);
        }
        else if ( is_mapped(header_i, header_template[2]) )
        {
            data.high = atof(token);
        }
        else if ( is_mapped(header_i, header_template[3]) )
        {
            data.low = atof(token);
        }
        else if ( is_mapped(header_i, header_template[4]) )
        {
            data.close = atof(token);
        }
        else if ( is_mapped(header_i, header_template[5]) )
        {
            data.volume = atoi(token);
        }

        // technical indicators
        else if ( is_mapped(header_i, header_template[6]) )
        {
            data.technical_indicators.is_ti1_p = true;
            data.technical_indicators.ti1 = atof(token);
        }
        else if ( is_mapped(header_i, header_template[7]) )
        {
            data.technical_indicators.is_ti2_p = true;
            data.technical_indicators.ti2 = atof(token);
        }
        else if ( is_mapped(header_i, header_template[8]) )
        {
            data.technical_indicators.is_ti3_p = true;
            data.technical_indicators.ti3 = atof(token);
        }
        else if ( is_mapped(header_i, header_template[9]) )
        {

            data.technical_indicators.is_ti_others_p = true;
            data.technical_indicators.ti_others = (char *)malloc((strlen(token) + 1) * sizeof(char));
            strcpy(data.technical_indicators.ti_others, token);
        }
        else
        {
            // unknown row position.
            break;
        }

        row_pos++;
        header_i++;
        token = strtok(NULL, ",");
    }

    return data;

}

int process_csv_header(algoticks_settings *settings, char *row){
        char *token;
        token = strtok(row, ",");
        int header_i = 0;

        while (token != NULL && header_i < MAXCSVHEAD)
        {
            #ifdef CHOMP
                chomp(token);
            #endif

            convert_to_lowercase(token);

            debug_msg(settings->debug, settings->debug_level, 4, __FILE__, __FUNCTION__, __LINE__, token);


            if( strcmp(token, header_template[0]) == 0 ){ 
                header_map[header_i] = 0; }

            else if( strcmp(token, header_template[1]) == 0 ){ 
                header_map[header_i] = 1; }

            else if( strcmp(token, header_template[2]) == 0 ){ 
                header_map[header_i] = 2; }

            else if( strcmp(token, header_template[3]) == 0 ){ 

                header_map[header_i] = 3; }

            else if( strcmp(token, header_template[4]) == 0 ){ 

                header_map[header_i] = 4; }

            else if( strcmp(token, header_template[5]) == 0 ){ 
                header_map[header_i] = 5; }

            else if( strcmp(token, header_template[6]) == 0 ){ 
                header_map[header_i] = 6; }

            else if( strcmp(token, header_template[7]) == 0 ){ 
                header_map[header_i] = 7; }

            else if( strcmp(token, header_template[8]) == 0 ){ 
                header_map[header_i] = 8; }

            else if( strcmp(token, header_template[9]) == 0 ){ 
                header_map[header_i] = 9; }            

            else { 
                header_map[header_i] = -1;
                }

            header_i++;
            token = strtok(NULL, ",");
        }
        
        //skip the 1st row i.e header
        is_header_skipped = true;
        return true;
}

void set_ohlcv_as_header() {

    for (int i = 0; i < MAXCSVHEAD; i++)
    {
        header_map[i] = i;
    }

}

void changed_cb(uv_handle_t *handle, const char *filename, int events, int status){

    if (events & UV_CHANGE){
        //printf("changed\n");
        uv_close(handle, NULL);
    }


}

int read_csv(algoticks_settings *settings,algoticks_config *config, FILE *fp, char *fname, algoticks_row *storage, int seek_offset){
   while(true) {
    if ( feof(fp) )
    {
        if (settings->is_live_data == true){
            
            if (settings->print != false){
            printf("watching for data in %s\n", config->datasource);
            }

            uv_loop_t *loop = uv_default_loop();
            uv_fs_event_t *event = malloc(sizeof(uv_fs_event_t)); 
            uv_fs_event_init(loop, event);
            uv_fs_event_start(event, (uv_fs_event_cb)changed_cb, config->datasource, UV_FS_EVENT);
            uv_run(loop, UV_RUN_DEFAULT);

            reopen_datasource(config->datasource, &fp, "rb");
            assert(uv_loop_close(loop) == 0);

        }else if (settings->is_live_data_socket == true){

            if (!is_socket_init){
                
                client_d = socket_init(settings->socket_port);

                if (client_d < 0){
                    printf("error creating socket!\n");
                    exit(1);
                }

                is_socket_init = true;
            }
            
            if (settings->debug && settings->debug_level > 2){
            printf("waiting for new data from 127.0.0.1:%s\n", settings->socket_port);
            }
            
            char buffer[4096];

            while(true){
            int bytes = zmq_recv (client_d, buffer, 4096, 0);

            if (bytes > 5){
                break;
            }

            }

            reopen_datasource(fname, &fp, "a");
            fprintf(fp, "%s", buffer);
            reopen_datasource(fname, &fp, "rb");

            //set seek
            fseek(fp, seek_offset, SEEK_SET);

            debug_msg(settings->debug, settings->debug_level, 1, __FILE__, __FUNCTION__, __LINE__, config->datasource);
            
        }
        else {
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


    if (config->interval > 0 && is_header_skipped == true && settings->is_live_data == false){
        for (int i = 0; i < config->interval; i++)
        {
            fgets(row, MAXCHARPERLINE, fp);
            debug_msg(settings->debug, settings->debug_level, 4, __FILE__, __FUNCTION__, __LINE__, row);
        }

    }
    

    //read row string from file
    fgets(row, MAXCHARPERLINE, fp);


    //remove white space at end
    #ifdef CHOMP
        chomp(row);
    #endif

    if (!is_header_skipped){
        if (config->skip_header != true){
            process_csv_header(settings, row);
        }
        else{
            set_ohlcv_as_header();
        }
        is_header_skipped = true;
    }

    curr_sp = ftell(fp);

    *storage = tokenize_row(row);
    
    #ifdef CHECK_ROW_INTEGRITY
        if (check_row_integrity(storage) == false){
            continue;
        }
    #endif
    
    storage->curr = curr_sp;
    
    return curr_sp;

    }

}