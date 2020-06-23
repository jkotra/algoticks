#include <stdio.h>
#include "../include/dtypes.h"
#include "../include/misc.h"
#include "../include/benchmark.h"
#include "../include/debug.h"
#include "../include/sim.h"

void benchmark(char *benchmark_config_file, algoticks_settings settings)
{

    struct BenchmarkConfig benchmarkconfig;
    benchmarkconfig = parse_benchmark_from_json(benchmark_config_file);


    //total 11 fields in config = 11 loops

    /*

    {
    "algo": ["3Reds.so"],
    "datasource": ["example.csv"],
    "symbol": ["SUNPHARMA"],
    "candles": [4,6,8],

    "target": [35,50],
    "stoploss": [-70,-50],
    "is_training_sl": [true,false],
    "trailing_sl_val": [2,4],
    "quantity": [100],

    "intraday": [true],
    "skip_header": [true]

}
*/ 
   int total_combinations = 1;
   total_combinations *= benchmarkconfig.n_algo;
   total_combinations *= benchmarkconfig.n_datasource;
   total_combinations *= benchmarkconfig.n_candles;
   total_combinations *= benchmarkconfig.n_target;
   total_combinations *= benchmarkconfig.n_stoploss;
   total_combinations *= benchmarkconfig.n_is_training_sl;
   total_combinations *= benchmarkconfig.n_trailing_sl_val;
   total_combinations *= benchmarkconfig.n_quantity;
   total_combinations *= benchmarkconfig.n_intraday;

   printf("Total Unique Combinations: %d\n", total_combinations);

   int combination_completed = 0;

    for (int n_algo = 0; n_algo < benchmarkconfig.n_algo; n_algo++)
    {
        for (int n_datasource = 0; n_datasource < benchmarkconfig.n_datasource; n_datasource++)
        {
            for (int n_candles = 0; n_candles < benchmarkconfig.n_candles; n_candles++)
            {
                for (int n_target = 0; n_target < benchmarkconfig.n_target; n_target++)
                {
                    for (int n_stoploss = 0; n_stoploss < benchmarkconfig.n_stoploss; n_stoploss++)
                    {
                        for (int n_is_training_sl = 0; n_is_training_sl < benchmarkconfig.n_is_training_sl; n_is_training_sl++)
                        {
                            for (int n_trailing_sl_val = 0; n_trailing_sl_val < benchmarkconfig.n_trailing_sl_val; n_trailing_sl_val++)
                            {
                                for (int n_quantity = 0; n_quantity < benchmarkconfig.n_quantity; n_quantity++)
                                {
                                    for (int n_intraday = 0; n_intraday < benchmarkconfig.n_intraday; n_intraday++)
                                    {
                                        /* make a new struct here */
                                        struct Config config;

                                        strncpy(config.algo, benchmarkconfig.algo[n_algo], 32);
                                        strncpy(config.datasource, benchmarkconfig.datasource[n_datasource], 32);
                                        strncpy(config.symbol, benchmarkconfig.symbol, 32);

                                        config.candles = benchmarkconfig.candles[n_candles];
                                        config.target = benchmarkconfig.target[n_target];
                                        config.stoploss = benchmarkconfig.stoploss[n_stoploss];
                                        config.is_training_sl = benchmarkconfig.is_training_sl[n_is_training_sl];
                                        config.trailing_sl_val = benchmarkconfig.trailing_sl_val[n_trailing_sl_val];
                                        config.quantity = benchmarkconfig.quantity[n_quantity];
                                        config.intraday = benchmarkconfig.intraday[n_intraday];

                                        config.skip_header = benchmarkconfig.skip_header;

                                        if (settings.debug){
                                            print_config_struct(config);
                                        }
                                        
                                        run_sim(settings, config);
                                        combination_completed++;
                                        printf("%d/%d\r",combination_completed, total_combinations);
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