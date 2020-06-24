#include<stdio.h>
#include<string.h>
#include <stdbool.h>
#include "../include/dtypes.h"
#include "../include/debug.h"
#include "../include/dashboard.h"


char *color_reset = "\033[0m";
char *pnlcolor;

float getPnL(algoticks_dashboard dashboard){

    int isshort = (dashboard.is_short == true);

    if (isshort){
        return -(dashboard.b - dashboard.a) * dashboard.q;
    }
    else{
        return (dashboard.b - dashboard.a) * dashboard.q;
    }

}

void print_dashboard(algoticks_settings settings,algoticks_config config,algoticks_dashboard dashboard){
    char buffer[1024];
    char *pnlcolor;
    char pnl_s[128];
    float percentage_change;

    float pnl = getPnL(dashboard);
    float pnl_i = pnl / dashboard.q;

    percentage_change = ((pnl)/(dashboard.q * dashboard.a)) * 100;
    
    if (settings.colors == true){
        if(pnl > 0){ pnlcolor = "\033[0;32m"; }else { pnlcolor = "\033[0;31m"; }
        sprintf(pnl_s, "%s%f \(%f%%\) %f %s", pnlcolor, pnl, percentage_change, pnl_i, color_reset);
    }else{
        sprintf(pnl_s, "%s%f \(%f%%\) %f", pnlcolor, pnl, percentage_change, pnl_i);
    }
    
    

    sprintf(buffer, "%s\t%s\t%f\t%f\t%s",dashboard.date, config.symbol,dashboard.a, dashboard.b, pnl_s);               
    printf("%s\n", buffer);

}