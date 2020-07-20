#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "../include/dtypes.h"
#include "../include/csvutils.h"
#include "../include/dashboard.h"
#include "../include/timeutils.h"
#include "../include/sim_derivative.h"
#include "../include/misc.h"
#include "../include/debug.h"

int curr_i = 0;
int curr_d = 0;

algoticks_simresult run_sim_w_derivative(algoticks_settings settings, algoticks_config config)
{

    // open and read CSV file.
    FILE *index;
    index = fopen(config.datasource, "rb");

    // exit if file cannot be opened.
    if (index == NULL)
    {
        printf("cannot Read datasource: %s \n", config.datasource);
        exit(1);
    }

    // open and read CSV file.
    FILE *derivative;
    derivative = fopen(config.derivative.derivative_datasource, "rb");

    // exit if file cannot be opened.
    if (derivative == NULL)
    {
        printf("cannot Read derivative datasource: %s \n", config.datasource);
        exit(1);
    }

    struct Row storage;

    struct SimResult simresult = {0};

    //add config to simresult
    simresult.config = config;

    algo_func analyze = load_algo_func(config.algo);

    //initialize and malloc for series
    struct Row *series;
    series = (algoticks_row *)malloc((config.candles) * sizeof(algoticks_row));

    while (curr_i != EOF)
    {

        for (int i = 0; i < config.candles && curr_i != -1; i++)
        {
            curr_i = read_csv(settings, config, index, &series[i], curr_i);
            debug_msg(settings, 3, "SeriesReadRow", "sim.c", series[i].date);
        }

        if (curr_i == -1){
                break;
        }

        curr_i = read_csv(settings, config, index, &storage, curr_i);

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

            if (config.intraday == true)
            {
                if (is_date_over_or_eq_intraday(storage.date, settings.intraday_hour, settings.intraday_min) == true)
                {
                    continue;
                }
            }

            positionresult = take_position_w_derivative(signal, index, derivative, settings, config, storage);
            curr_i = positionresult.curr;

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
                if (signal.buy == true)
                {
                    simresult.b_trgt_hits += 1;
                }
                else if (signal.sell == true)
                {
                    simresult.s_trgt_hits += 1;
                }
            }
            else if (strcmp(positionresult.hit_type, "SL") == 0)
            {
                simresult.sl_hits += 1;
                if (signal.buy == true)
                {
                    simresult.b_sl_hits += 1;
                }
                else if (signal.sell == true)
                {
                    simresult.s_sl_hits += 1;
                }
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
        if (config.sliding == true && (config.candles > 2) == true)
        {
            if (curr_i != -1)
            {
                curr_i = series[0].curr;
            }
        }

        //update sim pnl to user if debug
        if (settings.debug) {
        char pnlmsg[12];
        sprintf(pnlmsg, "%f", simresult.pnl);
        debug_msg(settings, 1, "SimPnl", "sim.c", pnlmsg);
        }

        //zero out storage
        memset(&storage, 0, sizeof(storage));

        //zero out series
        for (int i = 0; i < config.candles; i++)
        {
            memset(&series[i], 0, sizeof(series[i]));
        }
    }

    //close datasource(s) file.
    fclose(index);
    fclose(derivative);

    //close algo
    close_algo_func();

    //free series mem.
    free(series);

    curr_i = 0;
    curr_d = 0;

    //make sure we read header in next iteration (benchmark scene)
    reset_header_skip();

    write_simresult_to_csv(simresult);
    return simresult;
}
algoticks_positionresult take_position_w_derivative(algoticks_signal signal, FILE *index_f, FILE *derivative_f, algoticks_settings settings, algoticks_config config, algoticks_row lastrow)
{

    //declare buffer for debug messages

    char *debug_msg_buffer;
    debug_msg_buffer = (char*)malloc(512 * sizeof(char));

    //initialize pos res.
    struct PositionResult positionresult = {0};
    struct Row pos_storage;
    struct Dashboard dashboard;
    
    //set curr derivative
    curr_d = sync_curr(derivative_f, config.derivative.derivative_datasource, lastrow.date, curr_d, settings.debug);
    if (curr_i == -1){
        printf("NIF\n");
        exit(1);
    }
    
    //copy index derivative into config.derivative.index_datasource for later use to set index curr.
    strncpy(config.derivative.index_datasource ,config.datasource, 512);
    strncpy(config.datasource, config.derivative.derivative_datasource, 512);

    curr_d = read_csv(settings, config, derivative_f, &pos_storage, curr_d);

    //set required details in dashboard
    dashboard.a = pos_storage.close;
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


    // infinite loop
    while (true)
    {
        positionresult.n_steps++;

        if (curr_d == EOF)
        {
            if (settings.debug)
            {
                debug_msg(settings, 1, "EOF", "sim.c", config.datasource);
                sprintf(debug_msg_buffer, "%d", curr_d);
                debug_msg(settings, 2, "derivative_curr", "sim.c", debug_msg_buffer);
            }
            positionresult.pnl = getPnL(dashboard);

            if (positionresult.pnl > 0)
            {
                strncpy(positionresult.hit_type, "T", 4);
            }
            else
            {
                strncpy(positionresult.hit_type, "SL", 4);
            }

            positionresult.eof = true;
            break;
        }

        curr_d = read_csv(settings, config, derivative_f, &pos_storage, curr_d);

        if ((pos_storage.date == NULL) || (pos_storage.close == 0))
        {
            continue;
        }

        dashboard.b = pos_storage.close;
        strncpy(dashboard.date, pos_storage.date, 32);
        positionresult.curr = curr_d;

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

    if (positionresult.n_steps > 0)
    {
        sprintf(debug_msg_buffer, "%f", positionresult.pnl);
        debug_msg(settings, 1, "PosPnl", "sim.c", debug_msg_buffer);
    }


    //set curr index
    curr_i = sync_curr(index_f, config.derivative.index_datasource, pos_storage.date, curr_i, settings.debug);
    if (curr_i == -1){
        printf("NIF\n");
        exit(1);
    }
    else{
    positionresult.curr = curr_i;
    }
    
    free(debug_msg_buffer);

    return positionresult;
}