#include<time.h>
#include<stdio.h>
#include<string.h>
#include<stdbool.h>
#include "../include/dtypes.h"
#include "../include/debug.h"


void debug_msg(char *op, char *loc, char *remarks, int log){
    
    //Human readable time
    char hrt[32];

    time_t now;

    //get time
    time(&now);

    sprintf(hrt, "%s", ctime(&now));

    //remove new line at end
    chomp(hrt);

    char buffer[5000];
    sprintf(buffer, "{\"date\": \"%s\", \"op\": \"%s\", \"loc\": \"%s\", \"remarks\": \"%s\"}\n", hrt, op, loc, remarks);

    printf("%s", buffer);

    if (log == true){
        FILE *fp;

        fp = fopen("debug.log","a+");

        fprintf(fp, buffer);
    }
}


void print_dashboard_struct(algoticks_dashboard dashboard){
    printf("\n===*===\n");
    printf("[DEBUG] Dashboard.a = %f\n", dashboard.a);
    printf("[DEBUG] Dashboard.b = %f\n", dashboard.b);
    printf("[DEBUG] Dashboard.date = %s\n", dashboard.date);
    printf("[DEBUG] Dashboard.pnl = %f\n", dashboard.pnl);
    printf("[DEBUG] Dashboard.is_short = %d\n", dashboard.is_short);
    printf("\n===*===\n");
}

void print_signal_struct(algoticks_signal signal){
    printf("\n===*===\n");
    printf("[DEBUG] signal.buy = %d\n", signal.buy);
    printf("[DEBUG] signal.neutral = %d\n", signal.neutral);
    printf("[DEBUG] signal.sell = %d\n", signal.sell);
    printf("\n===*===\n");
}

void print_simresult_struct(algoticks_simresult simresult){
    printf("\n===*===\n");
    printf("[DEBUG] signal.pnl = %f\n", simresult.pnl);
    printf("[DEBUG] signal.buy_signals = %d\n", simresult.buy_signals);
    printf("[DEBUG] signal.sell_signals = %d\n", simresult.sell_signals);
    printf("[DEBUG] signal.neutral_signals = %d\n", simresult.neutral_signals);
    printf("[DEBUG] signal.trgt_hits = %d\n", simresult.trgt_hits);
    printf("[DEBUG] signal.sl_hits = %d\n", simresult.sl_hits);
    printf("\n===*===\n");
}