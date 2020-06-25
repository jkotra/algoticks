#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <dlfcn.h>
#include "../include/dtypes.h"
#include "../include/csvutils.h"
#include "../include/dashboard.h"
#include "../include/timeutils.h"
#include "../include/sim.h"
#include "../include/misc.h"
#include "../include/debug.h"

/* this buffer is used for sprintf() */
char debug_msg_buffer[1024];

algoticks_positionresult take_position(algoticks_signal signal, FILE *fp, int curr, algoticks_settings settings, algoticks_config config, algoticks_row lastrow)
{

    //initialize pos res.
    struct PositionResult positionresult;

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

            positionresult.eof = true;
            break;
        }

        curr = read_csv(fp, &pos_storage, curr, config, settings.debug);

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
                debug_msg(settings, 1, "TrailingSL_Adjust", "sim.c", debug_msg_buffer);

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

    return positionresult;
}

algoticks_simresult run_sim(algoticks_settings settings, algoticks_config config)
{
    // open and read CSV file.
    FILE *fp;
    fp = fopen(config.datasource, "r");
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

    //load algo
    void *handle;
    algoticks_signal (*analyze)(algoticks_row *, int);

    handle = dlopen(config.algo, RTLD_LAZY);

    if (!handle)
    {
        fprintf(stderr, "Error: %s\n", dlerror());
        exit(EXIT_FAILURE);
    }

    *(void **)(&analyze) = dlsym(handle, "analyze");

    if (!analyze)
    {
        fprintf(stderr, "Error: %s\n", dlerror());
        dlclose(handle);
        exit(EXIT_FAILURE);
    }
    else
    {
        sprintf(debug_msg_buffer, "%s", config.algo);
        debug_msg(settings, 3, "LoadAlgo", "sim.c", debug_msg_buffer);
    }

    while (curr != EOF)
    {

        struct Row series[config.candles + 1];

        for (int i = 0; i < config.candles; i++)
        {
            curr = read_csv(fp, &series[i], curr, config, settings.debug);
        }

        curr = read_csv(fp, &storage, curr, config, settings.debug);

        struct Signal signal;
        signal = analyze(&series, config.candles);

        if (signal.buy)
        {
            simresult.buy_signals += 1;
            debug_msg(settings, 3, "signal", "sim.c", "Buy");
        }
        else if (signal.sell)
        {
            simresult.sell_signals += 1;
            debug_msg(settings, 3, "signal", "sim.c", "Sell");
        }
        else if (signal.neutral)
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
            }
            else if (strcmp(positionresult.hit_type, "SL") == 0)
            {
                simresult.sl_hits += 1;
            }
            else
            {
                printf("Position did not hit any boundary\n");
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

        //zero out storage
        memset(&storage, 0, sizeof(storage));
    }

    write_simresult_to_csv(simresult);
    return simresult;
}