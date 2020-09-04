#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include "../../include/dtypes.h"
#include "../include/debug.h"

FILE *fp;
char hrt[32];

void chomp(char *s)
{
    /* This removes newline at end of string */
    while (*s && *s != '\n' && *s != '\r')
        s++;
    *s = 0;
}

void get_hrt(char *hrt)
{
    time_t now;

    //get time
    time(&now);

    sprintf(hrt, "%s", ctime(&now));
    chomp(hrt);
}

#ifdef _WIN32
__declspec(dllexport)
#endif
    int callback_f(algoticks_event *event)
{

    if (fp == NULL)
    {
        fp = fopen("total.log", "w");
    }
    else if(fp != NULL){
        fp = fopen("total.log", "a");
    }

    get_hrt(hrt);

    if (event->signal.buy != false)
    {
        fprintf(fp, "{\"date\": \"%s\", \"signal\": \"Buy\" }\n", hrt);
    }

    else if (event->signal.sell != false)
    {
        fprintf(fp, "{\"date\": \"%s\", \"signal\": \"Sell\" }\n", hrt);
    }

    else if (event->from_pos)
    {
        fprintf(fp, "{\"date\": \"%s\", \"positionPnL\": %f }\n", hrt, event->pnl);
    }

    else if (event->t_h)
    {
        fprintf(fp, "{\"date\": \"%s\", \"Hit\": \"Target\", \"simulationPnl\": %f }\n", hrt, event->pnl);
    }

    else if (event->sl_h)
    {
        fprintf(fp, "{\"date\": \"%s\", \"Hit\": \"SL\", \"simulationPnl\": %f }\n", hrt, event->pnl);
    }

    else if (event->tsl)
    {
        fprintf(fp, "{\"date\": \"%s\", \"Hit\": \"TSL\", \"T\": %f, \"SL\": %f }\n", hrt, event->tsl_t, event->tsl_sl);
    }

    fclose(fp);


    return true;
}