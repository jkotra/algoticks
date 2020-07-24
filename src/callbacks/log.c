#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include "../../include/dtypes.h"

char log_header[128] = "date,pnl\n";
int logheader_written = false;
FILE *fp;
FILE *fp2;

#ifdef _WIN32
__declspec(dllexport)
#endif
int callback_f(algoticks_event *event){

    if (fp == NULL){
        fp = fopen("pnl.log","w");
    }
    if (fp2 == NULL){
        fp2 = fopen("all.log","w");
    }

    if (!logheader_written){
        fprintf(fp, log_header);
        fprintf(fp2, log_header);
        logheader_written = true;
    }
    
    if (event->from_sim){
    fprintf(fp, "%s,%f\n", event->date, event->pnl);
    }

    if (event->from_pos){
    fprintf(fp2, "%s,%f\n", event->date, event->pnl);
    }


    return true;

}