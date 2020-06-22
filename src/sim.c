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

int curr = 0;

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
                debug_msg("EOF", "sim.c", config.datasource, true);
            }
            positionresult.pnl = getPnL(dashboard);

            if (positionresult.pnl > 0){
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

        if (is_target_hit(dashboard, config.target) == true)
        {
            printf("\n Target Hit! \n");

            strncpy(positionresult.hit_type, "T", 4);
            positionresult.pnl = getPnL(dashboard);

            if (config.is_training_sl)
            {
                config.target = (dashboard.b - dashboard.a) + config.trailing_sl_val;

                if (dashboard.is_short){
                config.stoploss += config.trailing_sl_val;
                }else{
                config.stoploss -= -config.trailing_sl_val;
                }

                continue;
            }


            break;
        }

        if (is_stoploss_hit(dashboard, config.stoploss) == true)
        {
            printf("\n SL Hit! \n");

            positionresult.pnl = getPnL(dashboard);

            if (positionresult.pnl > 0){
                strncpy(positionresult.hit_type, "T", 4);
            }
            else{
                strncpy(positionresult.hit_type, "SL", 4);
            }
            
            break;
        }

        memset(&pos_storage, 0, sizeof(pos_storage));  
    }

    return positionresult;
}

algoticks_simresult run_sim(algoticks_settings settings, algoticks_config config)
{
    // open and read CSV file.
    FILE *fp;
    fp = fopen(config.datasource, "r");

    // exit if file cannot be opened.
    if (fp == NULL)
    {
        printf("cannot Read: %s \n", config.datasource);
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
        printf("Algorithm %s loaded!\n", config.algo);
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
            if (settings.debug){
                debug_msg("signal", "sim.c", "Buy", false);
            }
            
        }
        else if (signal.sell)
        {
            simresult.sell_signals += 1;
            if (settings.debug){
                debug_msg("signal", "sim.c", "Sell", false);
            }
            
        }
        else if (signal.neutral)
        {
            simresult.neutral_signals += 1;
            if (settings.debug){
                debug_msg("signal", "sim.c", "Neutral", false);
            }
            
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
            if (simresult.pnl > simresult.peak){
                simresult.peak = simresult.pnl;
            }
            if (simresult.pnl < simresult.bottom){
                simresult.bottom = simresult.pnl;
            }
            
            //DEBUG - hit_type
            if (settings.debug){
                debug_msg("pos_result", "sim.c", positionresult.hit_type, false);
            }
            
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
            }else{
                continue;
            }

            //continue til EOF
            //FIX this
            //debug_msg("sim_pnl", "sim.c", ftoa(simresult.pnl), false);
            continue;
        }

        memset(&storage, 0, sizeof(storage));
    }
    if (settings.debug){
    print_simresult_struct(simresult);
    }

    write_simresult_to_csv(simresult);
    return simresult;
}