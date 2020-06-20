#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <dlfcn.h>
#include <math.h>
#include "../include/dtypes.h"
#include "../include/csvutils.h"
#include "../include/dashboard.h"
#include "../include/timeutils.h"

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
        print_signal_struct(signal);
        exit(1);
    }

    struct Row pos_storage;

    // infinite loop
    while (1)
    {
        positionresult.n_steps++;

        if (curr == EOF)
        {
            if (settings.debug)
            {
                //todo
                debug_msg("EOF", "sim.c", config.datasource, false);
            }

            positionresult.eof = true;
            return positionresult;
        }

        curr = read_csv(fp, &pos_storage, curr, config, settings.debug);

        if ((pos_storage.date == NULL) || (pos_storage.close == 0))
        {
            continue;
        }

        dashboard.b = pos_storage.close;
        strncpy(dashboard.date, pos_storage.date, 32);
        positionresult.curr = curr;

        if (settings.print == 1)
        {
            print_dashboard(settings, config, dashboard);
        }

        if (config.target <= (getPnL(dashboard) / dashboard.q))
        {
            printf("\nTarget Hit!\n");
            if (config.is_training_sl)
            {
                config.target = dashboard.b + config.trailing_sl_val;
                config.stoploss = dashboard.b - config.trailing_sl_val;
                continue;
            }

            strncpy(positionresult.hit_type, "T", 4);
            positionresult.pnl = getPnL(dashboard);
            break;
        }

        if (config.stoploss >= (getPnL(dashboard) / dashboard.q))
        {
            printf("\nSL Hit!\n");

            strncpy(positionresult.hit_type, "SL", 4);
            positionresult.pnl = getPnL(dashboard);
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

    struct Dashboard dashboard;
    dashboard.q = config.quantity;

    struct Row storage;

    struct SimResult simresult = {0};

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

        //debug
        


        if (signal.buy)
        {
            simresult.buy_signals += 1;
            debug_msg("signal", "sim.c", "Buy", false);
        }
        else if (signal.sell)
        {
            simresult.sell_signals += 1;
            debug_msg("signal", "sim.c", "Sell", false);
        }
        else if (signal.neutral)
        {
            simresult.neutral_signals += 1;
            debug_msg("signal", "sim.c", "Neutral", false);
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

            if (positionresult.eof == true)
            {
                break;
            }

            curr = positionresult.curr;

            simresult.pnl += positionresult.pnl;

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
                continue;
            }

            //continue til EOF
            //debug_msg("sim_pnl", "sim.c", ftoa(simresult.pnl), false);
            continue;
        }

        memset(&storage, 0, sizeof(storage));
    }
    //debug
    print_simresult_struct(simresult);

    return simresult;
}