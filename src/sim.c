#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <assert.h>
#include "../include/dtypes.h"
#include "../include/csvutils.h"
#include "../include/dashboard.h"
#include "../include/timeutils.h"
#include "../include/sim.h"
#include "../include/misc.h"
#include "../include/debug.h"
#include "../include/callbacks.h"


algoticks_simresult run_sim(algoticks_settings *settings, algoticks_config *config){

    // open and read CSV file.
    FILE *fp;
    assert(config->datasource != NULL);
    fp = fopen(config->datasource, "rb");
    int curr = 0;

    // exit if file cannot be opened.
    if (fp == NULL)
    {
        printf("cannot Read datasource: %s \n", config->datasource);
        exit(1);
    }

    struct Row storage;

    struct SimResult simresult = {0};
    struct PositionResult positionresult = {0};

    //add config to simresult. required for writing result to csv.
    simresult.config = *config;

    algo_func analyze = load_algo_func(config->algo);
    load_callbacks(config);

    //initialize and malloc for series
    struct Row* series;
    series = (algoticks_row*) malloc((config->candles) * sizeof(algoticks_row));


    while (curr != EOF)
    {

        for (int i = 0; i < config->candles && curr != -1; i++)
        {
            curr = read_csv(settings, config, fp, config->datasource, &series[i], curr);
            debug_msg(settings->debug, settings->debug_level, 3, __FILE__, __FUNCTION__, __LINE__, series[i].date);
        }

        if (curr == -1){
            break;
        }

        curr = read_csv(settings, config, fp, config->datasource, &storage, curr);

        struct Signal signal;
        signal = analyze(series, config->candles);

        if (signal.buy == true)
        {
            simresult.buy_signals += 1;
            debug_msg(settings->debug, settings->debug_level, 1, __FILE__, __FUNCTION__, __LINE__, "Buy");
        }
        else if (signal.sell == true)
        {
            simresult.sell_signals += 1;
            debug_msg(settings->debug, settings->debug_level, 1, __FILE__, __FUNCTION__, __LINE__, "Sell");
        }
        else if (signal.neutral == true)
        {
            simresult.neutral_signals += 1;
            debug_msg(settings->debug, settings->debug_level, 2, __FILE__, __FUNCTION__, __LINE__, "Neutral");
        }
        else
        {
            printf("Unknown Signal received!\n");
            exit(1);
        }

        if (signal.neutral != true)
        {
            {
                algoticks_event ev = make_event_from_signal(signal);
                send_callbacks(ev);
            }

            if (config->intraday == true){
                if (is_date_over_or_eq_intraday(storage.date, settings->intraday_hour, settings->intraday_min) == true){
                    continue;
                }
            }
            
            positionresult = take_position(signal, fp, curr, settings, config, storage);
            curr = positionresult.curr;

            simresult.pnl += positionresult.pnl;

            //update peak and bottom;
            if (simresult.pnl > simresult.peak)
            {
                simresult.peak = simresult.pnl;
            }
            if (simresult.pnl < simresult.bottom)
            {
                simresult.bottom = simresult.pnl;
            }

            if (strcmp(positionresult.hit_type, "T") == 0)
            {
                simresult.trgt_hits += 1;
                if (signal.buy == true){ simresult.b_trgt_hits += 1; }
                else if (signal.sell == true){ simresult.s_trgt_hits += 1; }
            }
            else if (strcmp(positionresult.hit_type, "SL") == 0)
            {
                simresult.sl_hits += 1;
                if (signal.buy == true){ simresult.b_sl_hits += 1; }
                else if (signal.sell == true){ simresult.s_sl_hits += 1; }
            }
            else
            {
                debug_msg(settings->debug, settings->debug_level, 1, __FILE__, __FUNCTION__, __LINE__, "Position did not hit any boundary");
            }

            //send callback
            algoticks_event ev = make_event_from_positionresult(positionresult);
            send_callbacks(ev);

            if (positionresult.eof == true)
            {
                break;
            }
            else
            {
                continue;
            }

            continue;
        }
        
        // -1 from back +1 from front. easy way to do it it set curr to 1st index of series.
        if (config->sliding == true && (config->candles > 2) == true){
            if (curr != -1){
                curr = series[0].curr;
            }
        }

        //zero out storage
        memset(&storage, 0, sizeof(storage));

        //zero out series
        for (int i = 0; i < config->candles; i++)
        {
            memset(&series[i], 0, sizeof(series[i]));
        }

        //print simresult.pnl
        char pnl[32];
        sprintf(pnl, "%f", simresult.pnl);
        debug_msg(settings->debug, settings->debug_level, 1, __FILE__, __FUNCTION__, __LINE__, pnl);
    }
    
    //close datasource file.
    fclose(fp);

    //close algo
    close_algo_func();
    close_callbacks();

    //free series mem.
    free(series);

    write_simresult_to_csv(&simresult);

    return simresult;
}

algoticks_positionresult take_position(algoticks_signal signal, FILE *fp, int curr, algoticks_settings *settings, algoticks_config *config, algoticks_row lastrow){
     
    //declare buffer for debug messages
    char *debug_msg_buffer;
    debug_msg_buffer = (char*)malloc(512 * sizeof(char));

    //initialize pos res.
    struct PositionResult positionresult = {0};

    struct Dashboard dashboard;

    //set dashboard values
    dashboard.a = lastrow.close;
    dashboard.q = config->quantity;

    if (signal.buy == true)
    {
        dashboard.is_short = 0;
    }
    else if (signal.sell == true)
    {
        dashboard.is_short = 1;
    }
    else
    {
        printf("Invalid signal!\n");
        exit(1);
    }

    //filter targets
    filter_boundaries(config, dashboard.is_short);

    struct Row pos_storage;

    // infinite loop
    while (true)
    {
        positionresult.n_steps++;

        if (curr == EOF || curr == -1)
        {
            if (settings->debug)
            {
                debug_msg(settings->debug, settings->debug_level, 1, __FILE__, __FUNCTION__, __LINE__, config->datasource); 
                sprintf(debug_msg_buffer, "%d", curr);
                debug_msg(settings->debug, settings->debug_level, 2, __FILE__, __FUNCTION__, __LINE__, debug_msg_buffer);
            }
            positionresult.pnl = getPnL(dashboard);

            if (positionresult.pnl > 0)
            {
                strncpy(positionresult.hit_type, "T", 4);
            }
            else{
                strncpy(positionresult.hit_type, "SL", 4);
            }


            positionresult.eof = true;
            break;
        }

        curr = read_csv(settings,config, fp, config->datasource, &pos_storage, curr);

        if ((pos_storage.date == NULL) || (pos_storage.close == 0))
        {
            continue;
        }

        dashboard.b = pos_storage.close;
        strncpy(dashboard.date, pos_storage.date, 32);
        positionresult.curr = curr;

        if (settings->print == true)
        {
            print_dashboard(settings, config, dashboard);
        }
        
        //intraday check condition.
        if (config->intraday)
        {

            //check if over intraday squareoff time!
            int intraday_check = is_date_over_or_eq_intraday(pos_storage.date, settings->intraday_hour, settings->intraday_min);

            if (intraday_check == true)
            {
                sprintf(debug_msg_buffer, "H: %d S: %d Date: %s", settings->intraday_hour, settings->intraday_min, pos_storage.date);
                debug_msg(settings->debug, settings->debug_level, 2, __FILE__, __FUNCTION__, __LINE__, debug_msg_buffer);

                positionresult.pnl = getPnL(dashboard);

                if (positionresult.pnl > 0)
                {
                    strncpy(positionresult.hit_type, "T", 4);
                }
                else
                {
                    strncpy(positionresult.hit_type, "SL", 4);
                }

                break;
            }
        }

        if (is_target_hit(dashboard, config->target) == true)
        {
            debug_msg(settings->debug, settings->debug_level, 1, __FILE__, __FUNCTION__, __LINE__, "Target Hit!");

            strncpy(positionresult.hit_type, "T", 4);
            positionresult.pnl = getPnL(dashboard);

            if (config->is_trailing_sl)
            {
                config->target = (dashboard.b - dashboard.a) + config->trailing_sl_val;

                if (dashboard.is_short)
                {
                    config->stoploss += config->trailing_sl_val;
                }
                else
                {
                    config->stoploss -= -config->trailing_sl_val;
                }

                sprintf(debug_msg_buffer, "T:%f SL:%f", config->target, config->stoploss);
                debug_msg(settings->debug, settings->debug_level, 1, __FILE__, __FUNCTION__, __LINE__, debug_msg_buffer);

                continue;
            }

            break;
        }

        if (is_stoploss_hit(dashboard, config->stoploss) == true)
        {
            debug_msg(settings->debug, settings->debug_level, 1, __FILE__, __FUNCTION__, __LINE__, "SL Hit!");

            positionresult.pnl = getPnL(dashboard);

            if (positionresult.pnl > 0)
            {
                strncpy(positionresult.hit_type, "T", 4);
            }
            else
            {
                strncpy(positionresult.hit_type, "SL", 4);
            }

            break;
        }
        

        //send callback from pos
        algoticks_event ev = make_event_from_position(pos_storage, dashboard);
        send_callbacks(ev);


        //zero out pos_stotage
        memset(&pos_storage, 0, sizeof(pos_storage));
    }
    
    if (positionresult.n_steps > 0){
    sprintf(debug_msg_buffer, "%f", positionresult.pnl);
    debug_msg(settings->debug, settings->debug_level, 1, __FILE__, __FUNCTION__, __LINE__, debug_msg_buffer);
    }
    
    free(debug_msg_buffer);
    positionresult.lastrow = pos_storage;
    return positionresult;
}