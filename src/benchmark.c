#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "../include/dtypes.h"
#include "../include/misc.h"
#include "../include/benchmark.h"
#include "../include/debug.h"
#include "../include/sim.h"
#include "../include/sim_derivative.h"
#include "../include/parser.h"

void run_benchmark(char *benchmark_config_file, algoticks_settings settings)
{
    
    

    //disable printing
    settings.print = false;
    settings.is_live_data = false;
    settings.is_live_data_socket = false;
    struct BenchmarkConfig benchmarkconfig;
    benchmarkconfig = parse_benchmark_from_json(benchmark_config_file);
    

    //total 11 fields in config = 11 loops

    int total_combinations = 1;
    total_combinations *= benchmarkconfig.n_algo;
    total_combinations *= benchmarkconfig.n_datasource;
    total_combinations *= benchmarkconfig.n_candles;
    total_combinations *= benchmarkconfig.n_interval;

    total_combinations *= benchmarkconfig.n_target;
    total_combinations *= benchmarkconfig.n_stoploss;
    total_combinations *=  benchmarkconfig.n_is_trailing_sl;
    total_combinations *= benchmarkconfig.n_trailing_sl_val;
    total_combinations *= benchmarkconfig.n_quantity;
    total_combinations *= benchmarkconfig.n_sliding;
    total_combinations *= benchmarkconfig.n_intraday;

    printf("Total Unique Combinations: %d\n", total_combinations);

    int combination_completed = 1;
    float progress = 0;
    size_t len;

    for (int n_algo = 0; n_algo < benchmarkconfig.n_algo; n_algo++)
    {
        for (int n_datasource = 0; n_datasource < benchmarkconfig.n_datasource; n_datasource++)
        {
            for (int n_candles = 0; n_candles < benchmarkconfig.n_candles; n_candles++)
            {
                for (int n_interval = 0; n_interval < benchmarkconfig.n_interval; n_interval++)
                {
                    for (int n_target = 0; n_target < benchmarkconfig.n_target; n_target++)
                    {
                        for (int n_stoploss = 0; n_stoploss < benchmarkconfig.n_stoploss; n_stoploss++)
                        {
                            for (int n_is_training_sl = 0; n_is_training_sl < benchmarkconfig.n_is_trailing_sl; n_is_training_sl++)
                            {
                                for (int n_trailing_sl_val = 0; n_trailing_sl_val < benchmarkconfig.n_trailing_sl_val; n_trailing_sl_val++)
                                {
                                    for (int n_quantity = 0; n_quantity < benchmarkconfig.n_quantity; n_quantity++)
                                    {
                                        for (int n_sliding = 0; n_sliding < benchmarkconfig.n_sliding; n_sliding++)
                                        {
                                            for (int n_intraday = 0; n_intraday < benchmarkconfig.n_intraday; n_intraday++)
                                            {
                                                // make a new config struct
                                                struct Config config;
                                                
                                                len = strlen(benchmarkconfig.algo_arr[n_algo]);
                                                config.algo = (char*) malloc(len * sizeof(char));
                                                strcpy(config.algo, benchmarkconfig.algo_arr[n_algo]);

                                                len = strlen(benchmarkconfig.datasource_arr[n_datasource]);
                                                config.datasource = (char*) malloc(len * sizeof(char)); 
                                                strcpy(config.datasource, benchmarkconfig.datasource_arr[n_datasource]);

                                                len = strlen(benchmarkconfig.symbol);
                                                config.symbol = (char*) malloc(len * sizeof(char));
                                                strcpy(config.symbol, benchmarkconfig.symbol);

                                                config.derivative = benchmarkconfig.derivative;
                                                config.candles = benchmarkconfig.candles_arr[n_candles];
                                                config.interval = benchmarkconfig.interval_arr[n_interval];

                                                config.target = benchmarkconfig.target_arr[n_target];
                                                config.stoploss = benchmarkconfig.stoploss_arr[n_stoploss];
                                                config.is_trailing_sl = benchmarkconfig.is_trailing_sl[n_is_training_sl];
                                                config.trailing_sl_val = benchmarkconfig.trailing_sl_val_arr[n_trailing_sl_val];
                                                config.quantity = benchmarkconfig.quantity_arr[n_quantity];
                                                config.n_callbacks = 0;
                                                config.sliding = benchmarkconfig.sliding[n_sliding];
                                                config.intraday = benchmarkconfig.intraday[n_intraday];

                                                config.skip_header = benchmarkconfig.skip_header;
                                                
                                                //print config combination to user if debug is enabled and debug level greater then 3.
                                                if (settings.debug == true && (settings.debug_level >= 4) == true){
                                                print_config_struct(config);
                                                }
                                                
                                                if (settings.is_derivative){
                                                    run_sim_w_derivative(&settings, &config);
                                                }
                                                else{
                                                    run_sim(&settings, &config);
                                                }

                                                combination_completed++;

                                                progress = ((float)combination_completed / total_combinations) * 100;
                                                printf("%d/%d (%f%%)\r", combination_completed, total_combinations, progress);
                                                fflush(stdout);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

}