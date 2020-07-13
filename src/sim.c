#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "../include/dtypes.h"
#include "../include/csvutils.h"
#include "../include/dashboard.h"
#include "../include/timeutils.h"
#include "../include/sim.h"
#include "../include/misc.h"
#include "../include/debug.h"

/* this buffer is used for sprintf() to send to debung_msg function. */
char debug_msg_buffer[1024];

algoticks_positionresult take_position(algoticks_signal signal, FILE *fp, int curr, algoticks_settings settings, algoticks_config config, algoticks_row lastrow)
{

    //initialize pos res.
    struct PositionResult positionresult = {0};

    struct Dashboard dashboard;

    //set dashboard values
    dashboard.a = lastrow.close;
    dashboard.q = config.quantity;

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
    config = filter_boundaries(config, dashboard.is_short);

    struct Row pos_storage;

    // infinite loop
    while (1)
    {
        positionresult.n_steps++;

        if (curr == EOF)
        {
            if (settings.debug)
            {
                debug_msg(settings, 1, "EOF", "sim.c", config.datasource);
                sprintf(debug_msg_buffer, "%d", curr);
                debug_msg(settings, 2, "fp_curr", "sim.c", debug_msg_buffer);
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

        curr = read_csv(settings,config, fp, &pos_storage, curr);

        if ((pos_storage.date == NULL) || (pos_storage.close == 0))
        {
            continue;
        }

        dashboard.b = pos_storage.close;
        strncpy(dashboard.date, pos_storage.date, 32);
        positionresult.curr = curr;

        if (settings.print == true)
        {
            print_dashboard(settings, config, dashboard);
        }
        
        //intraday check condition.
        if (config.intraday)
        {

            //check if over intraday squareoff time!
            int intraday_check = is_date_over_or_eq_intraday(pos_storage.date, settings.intraday_min, settings.intraday_hour);

            if (intraday_check == true)
            {
                //debug msg
                sprintf(debug_msg_buffer, "H: %d S: %d Date: %s", settings.intraday_hour, settings.intraday_min, pos_storage.date);
                debug_msg(settings, 2, "intraday_squareoff", "sim.c", debug_msg_buffer);

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

        if (is_target_hit(dashboard, config.target) == true)
        {
            debug_msg(settings, 1, "Hit", "sim.c", "Target Hit!");

            strncpy(positionresult.hit_type, "T", 4);
            positionresult.pnl = getPnL(dashboard);

            if (config.is_training_sl)
            {
                config.target = (dashboard.b - dashboard.a) + config.trailing_sl_val;

                if (dashboard.is_short)
                {
                    config.stoploss += config.trailing_sl_val;
                }
                else
                {
                    config.stoploss -= -config.trailing_sl_val;
                }

                sprintf(debug_msg_buffer, "T:%f SL:%f", config.target, config.stoploss);
                debug_msg(settings, 1, "TSL_Adjust", "sim.c", debug_msg_buffer);

                continue;
            }

            break;
        }

        if (is_stoploss_hit(dashboard, config.stoploss) == true)
        {
            debug_msg(settings, 1, "Hit", "sim.c", "SL Hit!");

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

        //zero out pos_stotage
        memset(&pos_storage, 0, sizeof(pos_storage));
    }
    
    sprintf(debug_msg_buffer, "%f", positionresult.pnl);
    debug_msg(settings, 1, "PosPnl", "sim.c", debug_msg_buffer);

    return positionresult;
}

algoticks_simresult run_sim(algoticks_settings settings, algoticks_config config)
{
    // open and read CSV file.
    FILE *fp;
    fp = fopen(config.datasource, "rb");
    int curr = 0;

    // exit if file cannot be opened.
    if (fp == NULL)
    {
        printf("cannot Read datasource: %s \n", config.datasource);
        exit(1);
    }

    struct Row storage;

    struct SimResult simresult = {0};

    //add config to simresult
    simresult.config = config;

    algo_func analyze = load_algo_func(config.algo);

    //initialize and malloc for series
    struct Row* series;
    series = (algoticks_row*)malloc((config.candles) * sizeof(algoticks_row));


    while (curr != EOF)
    {


        for (int i = 0; i < config.candles; i++)
        {
            curr = read_csv(settings,config, fp, &series[i], curr);
        }

        curr = read_csv(settings, config, fp, &storage, curr);

        struct Signal signal;
        signal = analyze(series, config.candles);

        if (signal.buy == true)
        {
            simresult.buy_signals += 1;
            debug_msg(settings, 1, "signal", "sim.c", "Buy");
        }
        else if (signal.sell == true)
        {
            simresult.sell_signals += 1;
            debug_msg(settings, 1, "signal", "sim.c", "Sell");
        }
        else if (signal.neutral == true)
        {
            simresult.neutral_signals += 1;
            debug_msg(settings, 3, "signal", "sim.c", "Neutral");
        }
        else
        {
            printf("Unknown Signal received!\n");
            exit(1);
        }

        if (signal.neutral != true)
        {
            struct PositionResult positionresult;

            if (config.intraday == true){
                if (is_date_over_or_eq_intraday(storage.date, settings.intraday_hour, settings.intraday_min) == true){
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

            //DEBUG - hit_type
            debug_msg(settings, 1, "hit_type", "sim.c", positionresult.hit_type);
            

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
                debug_msg(settings, 1, "Hit", "sim.c", "Position did not hit any boundary");
            }

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
        if (config.sliding == true && (config.candles > 2) == true){
            if (curr != -1){
                curr = series[0].curr;
            }
        }

        //zero out storage
        memset(&storage, 0, sizeof(storage));

        //zero out series
        memset(series, 0, sizeof(series));
    }
    
    //close datasource file.
    fclose(fp);

    //close algo
    close_algo_func();

    //free series mem.
    free(series);

    write_simresult_to_csv(simresult);
    return simresult;
}