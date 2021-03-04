#include <stdio.h>
#include <string.h>
#include <json-c/json.h>
#include <stdbool.h>
#include <ctype.h>
#include <assert.h>
#include "../include/dtypes.h"
#include "../include/debug.h"

algoticks_settings parse_settings_from_json(char *filename)
{


    char buffer[4096];
    size_t len;
    FILE *fp;

    fp = fopen(filename, "r");
    if (fp == NULL)
    {
        printf("cannot Read: %s \n", filename);
        exit(1);
    }
    fread(buffer, 1024, 1, fp);

    struct json_object *parsed_json;
    struct json_object *print;
    struct json_object *colors;

    struct json_object *debug;
    struct json_object *debug_level;

    struct json_object *derivative;
    struct json_object *benchmark;

    struct json_object *config_f;
    struct json_object *benchmark_f;

    struct json_object *is_live_data;
    struct json_object *socket;
    struct json_object *socket_port;

    struct json_object *intraday_hour;
    struct json_object *intraday_min;

    struct Settings settings = {0};

    parsed_json = json_tokener_parse(buffer);

    if (parsed_json == NULL){
        printf("Invalid JSON!\n");
        exit(1);
    }

    json_object_object_get_ex(parsed_json, "print", &print);
    json_object_object_get_ex(parsed_json, "colors", &colors);
    json_object_object_get_ex(parsed_json, "debug", &debug);
    json_object_object_get_ex(parsed_json, "debug_level", &debug_level);

    json_object_object_get_ex(parsed_json, "derivative", &derivative);
    json_object_object_get_ex(parsed_json, "benchmark", &benchmark);

    json_object_object_get_ex(parsed_json, "config_f", &config_f);
    json_object_object_get_ex(parsed_json, "benchmark_f", &benchmark_f);

    json_object_object_get_ex(parsed_json, "is_live_data", &is_live_data);
    json_object_object_get_ex(parsed_json, "socket", &socket);
    json_object_object_get_ex(parsed_json, "socket_port", &socket_port);

    json_object_object_get_ex(parsed_json, "intraday_hour", &intraday_hour);
    json_object_object_get_ex(parsed_json, "intraday_min", &intraday_min);

    settings.print = json_object_get_boolean(print);
    settings.colors = json_object_get_boolean(colors);
    settings.debug = json_object_get_boolean(debug);
    settings.debug_level = json_object_get_int(debug_level);

    settings.is_derivative = json_object_get_boolean(derivative);
    settings.is_benchmark = json_object_get_boolean(benchmark);

    settings.is_live_data = json_object_get_boolean(is_live_data);
    
    settings.is_live_data_socket = json_object_get_boolean(socket);
    
    len = json_object_get_string_len(socket_port);
    settings.socket_port = (char*) malloc(len * sizeof(char));
    strcpy(settings.socket_port, json_object_get_string(socket_port));

    len = json_object_get_string_len(config_f);
    settings.config_f = (char*) malloc(len * sizeof(char));
    strcpy(settings.config_f, json_object_get_string(config_f));
    
    len = json_object_get_string_len(benchmark_f);
    settings.benchmark_f = (char*) malloc(len * sizeof(char));
    strcpy(settings.benchmark_f, json_object_get_string(benchmark_f));


    settings.intraday_hour = json_object_get_int(intraday_hour);
    settings.intraday_min = json_object_get_int(intraday_min);

    //close config file!
    fclose(fp);

    //free json obj.
    json_object_put(parsed_json);

    return settings;
}

algoticks_config parse_config_from_json(char *filename)
{


    char buffer[4096];
    size_t len;
    FILE *fp;

    fp = fopen(filename, "r");
    if (fp == NULL)
    {
        printf("cannot Read: %s \n", filename);
        exit(1);
    }
    fread(buffer, 1024, 1, fp);

    struct json_object *parsed_json;

    struct json_object *algo;
    struct json_object *datasource;
    struct json_object *derivative;
    struct json_object *symbol;
    struct json_object *candles;
    struct json_object *interval;
    struct json_object *callbacks;

    struct json_object *target;
    struct json_object *stoploss;
    struct json_object *is_training_sl;
    struct json_object *trailing_sl_val;
    struct json_object *quantity;

    struct json_object *sliding;
    struct json_object *intraday;
    struct json_object *skip_header;

    struct json_object *tmp;

    struct Config config;

    parsed_json = json_tokener_parse(buffer);

    if (parsed_json == NULL){
        printf("Invalid JSON!\n");
        exit(1);
    }

    json_object_object_get_ex(parsed_json, "algo", &algo);
    json_object_object_get_ex(parsed_json, "datasource", &datasource);
    json_object_object_get_ex(parsed_json, "symbol", &symbol);
    json_object_object_get_ex(parsed_json, "candles", &candles);
    json_object_object_get_ex(parsed_json, "interval", &interval);
    json_object_object_get_ex(parsed_json, "target", &target);
    json_object_object_get_ex(parsed_json, "stoploss", &stoploss);
    json_object_object_get_ex(parsed_json, "is_training_sl", &is_training_sl);
    json_object_object_get_ex(parsed_json, "trailing_sl_val", &trailing_sl_val);
    json_object_object_get_ex(parsed_json, "quantity", &quantity);
    json_object_object_get_ex(parsed_json, "sliding", &sliding);
    json_object_object_get_ex(parsed_json, "intraday", &intraday);
    json_object_object_get_ex(parsed_json, "skip_header", &skip_header);
    

    len = json_object_get_string_len(algo);
    config.algo = (char*) malloc(len * sizeof(char));
    strcpy(config.algo, json_object_get_string(algo));

    len = json_object_get_string_len(datasource);
    config.datasource = (char*) malloc(len * sizeof(char));
    strcpy(config.datasource, json_object_get_string(datasource));

    len = json_object_get_string_len(symbol);
    config.symbol = (char*) malloc(len * sizeof(char));
    strcpy(config.symbol, json_object_get_string(symbol));



    config.candles = json_object_get_int(candles);
    config.interval = json_object_get_int(interval);
    config.quantity = json_object_get_int(quantity);
    config.target = json_object_get_double(target);
    config.stoploss = json_object_get_double(stoploss);
    config.is_trailing_sl = json_object_get_boolean(is_training_sl);
    config.trailing_sl_val = json_object_get_double(trailing_sl_val);
    config.sliding = json_object_get_boolean(sliding);
    config.intraday = json_object_get_boolean(intraday);
    config.skip_header = json_object_get_boolean(skip_header);

    //parse derivative if exists.
    int derivative_exists = json_object_object_get_ex(parsed_json, "derivative", &derivative);

    if (derivative_exists){
        struct json_object *derivative_datasource, *derivative_interval;

        int derivative_datasource_exists = json_object_object_get_ex(derivative, "derivative_datasource", &derivative_datasource);
        if (derivative_datasource_exists){

            len = json_object_get_string_len(derivative_datasource);
            config.derivative.derivative_datasource = (char*) malloc(len * sizeof(char));
            strcpy(config.derivative.derivative_datasource, json_object_get_string(derivative_datasource));

        }

        int derivative_interval_exists = json_object_object_get_ex(derivative, "derivative_interval", &derivative_interval);

        if (derivative_interval_exists) {
            config.derivative.derivative_interval = json_object_get_int(derivative_interval);
        }

    }
    else{
        //set to None
        config.derivative.derivative_datasource = (char*) malloc(5);
        strcpy(config.derivative.derivative_datasource, "None");
        config.derivative.derivative_interval = 0;

    }

    //parse callbacks if exists.
    int callbacks_exists = json_object_object_get_ex(parsed_json, "callbacks", &callbacks);

    if (callbacks_exists){
        config.n_callbacks = json_object_array_length(callbacks);
        config.callbacks = (char**) malloc(config.n_callbacks * sizeof(char));
        assert (config.n_callbacks <= 6);

        for (int i = 0; i < config.n_callbacks; i++)
        {
            tmp = json_object_array_get_idx(callbacks, i);
            len = json_object_get_string_len(tmp);
            config.callbacks[i] = (char*) malloc(len * sizeof(char));
            strcpy(config.callbacks[i], json_object_get_string(tmp));
            printf("%s\n", config.callbacks[i]);
        } 
    }
    else{
        config.n_callbacks = 0;
    }

    //close config file!
    fclose(fp);

    //free json obj.
    json_object_put(parsed_json);

    return config;
}

algoticks_benchmarkconfig parse_benchmark_from_json(char *filename)
{



    char buffer[4096];
    size_t len;
    FILE *fp;

    fp = fopen(filename, "r");
    if (fp == NULL)
    {
        printf("cannot Read: %s \n", filename);
        exit(1);
    }
    fread(buffer, 1024, 1, fp);

    struct json_object *parsed_json;
    struct json_object *tmp;

    struct json_object *algo;
    struct json_object *datasource;
    struct json_object *derivative;
    struct json_object *symbol;
    struct json_object *interval;

    struct json_object *candles;
    struct json_object *target;
    struct json_object *stoploss;
    struct json_object *is_trailing_sl;
    struct json_object *trailing_sl_val;
    struct json_object *quantity;

    struct json_object *sliding;
    struct json_object *intraday;
    struct json_object *skip_header;

    struct BenchmarkConfig benchmarkconfig;

    parsed_json = json_tokener_parse(buffer);

    json_object_object_get_ex(parsed_json, "algo", &algo);
    json_object_object_get_ex(parsed_json, "datasource", &datasource);
    json_object_object_get_ex(parsed_json, "symbol", &symbol);
    json_object_object_get_ex(parsed_json, "candles", &candles);
    json_object_object_get_ex(parsed_json, "interval", &interval);
    json_object_object_get_ex(parsed_json, "target", &target);
    json_object_object_get_ex(parsed_json, "stoploss", &stoploss);
    json_object_object_get_ex(parsed_json, "is_trailing_sl", &is_trailing_sl);
    json_object_object_get_ex(parsed_json, "trailing_sl_val", &trailing_sl_val);
    json_object_object_get_ex(parsed_json, "quantity", &quantity);
    json_object_object_get_ex(parsed_json, "sliding", &sliding);
    json_object_object_get_ex(parsed_json, "intraday", &intraday);
    json_object_object_get_ex(parsed_json, "skip_header", &skip_header);
    
    len = json_object_get_string_len(symbol);
    benchmarkconfig.symbol = (char*) malloc(len * sizeof(char));
    strcpy(benchmarkconfig.symbol, json_object_get_string(symbol));

    benchmarkconfig.n_algo = json_object_array_length(algo);
    benchmarkconfig.algo_arr = (char**) malloc(benchmarkconfig.n_algo + 1 * sizeof(char));

    for (size_t i = 0; i < benchmarkconfig.n_algo; i++)
    {
        tmp = json_object_array_get_idx(algo, i);
        len = json_object_get_string_len(tmp);
        benchmarkconfig.algo_arr[i] = (char*) malloc(len * sizeof(char));
        strcpy(benchmarkconfig.algo_arr[i], json_object_get_string(tmp));
    }


    benchmarkconfig.n_datasource = json_object_array_length(datasource);
    benchmarkconfig.datasource_arr = (char**) malloc(benchmarkconfig.n_datasource * sizeof(char));

    for (size_t i = 0; i < benchmarkconfig.n_datasource; i++)
    {
        tmp = json_object_array_get_idx(datasource, i);
        len = json_object_get_string_len(tmp);
        benchmarkconfig.datasource_arr[i] = (char*) malloc(len * sizeof(char));
        strcpy(benchmarkconfig.datasource_arr[i], json_object_get_string(tmp));
    }

    /* DERIVATIVE */
    int derivative_exists = json_object_object_get_ex(parsed_json, "derivative", &derivative);

    if (derivative_exists){
        struct json_object *derivative_datasource, *derivative_interval;

        int derivative_datasource_exists = json_object_object_get_ex(derivative, "derivative_datasource", &derivative_datasource);
        if (derivative_datasource_exists){

            len = json_object_get_string_len(derivative_datasource);
            benchmarkconfig.derivative.derivative_datasource = (char*) malloc(len * sizeof(char));
            strcpy(benchmarkconfig.derivative.derivative_datasource, json_object_get_string(derivative_datasource));

        }

        int derivative_interval_exists = json_object_object_get_ex(derivative, "derivative_interval", &derivative_interval);

        if (derivative_interval_exists) {
            benchmarkconfig.derivative.derivative_interval = json_object_get_int(derivative_interval);
        }

    }
    else{
        //set to None
        benchmarkconfig.derivative.derivative_datasource = (char*) malloc(5);
        strcpy(benchmarkconfig.derivative.derivative_datasource, "None");
        benchmarkconfig.derivative.derivative_interval = 0;
    }

    /* CANDLES */
    benchmarkconfig.n_candles = json_object_array_length(candles);
    benchmarkconfig.candles_arr = (int*) malloc(benchmarkconfig.n_candles * sizeof(int));

    for (size_t i = 0; i < benchmarkconfig.n_candles; i++)
    {
        tmp = json_object_array_get_idx(candles, i);
        len = json_object_get_string_len(tmp);
        benchmarkconfig.candles_arr[i] = json_object_get_int(tmp);
    }
    
    /* INTERVAL */
    benchmarkconfig.n_interval = json_object_array_length(interval);
    benchmarkconfig.interval_arr = (int*) malloc(benchmarkconfig.n_interval * sizeof(int));

    for (size_t i = 0; i < benchmarkconfig.n_interval; i++)
    {
        tmp = json_object_array_get_idx(interval, i);
        benchmarkconfig.interval_arr[i] = json_object_get_int(tmp);
    }

    
    /* TARGET */

    benchmarkconfig.n_target = json_object_array_length(target);
    benchmarkconfig.target_arr = (double*) malloc(benchmarkconfig.n_target * sizeof(double));

    for (size_t i = 0; i < benchmarkconfig.n_target; i++)
    {
        tmp = json_object_array_get_idx(target, i);
        benchmarkconfig.target_arr[i] = json_object_get_double(tmp);
    }
    

    /* STOPLOSS */

    benchmarkconfig.n_stoploss = json_object_array_length(stoploss);
    benchmarkconfig.stoploss_arr = (double*) malloc(benchmarkconfig.n_stoploss * sizeof(double));

    for (size_t i = 0; i < benchmarkconfig.n_stoploss; i++)
    {
        tmp = json_object_array_get_idx(stoploss, i);
        benchmarkconfig.stoploss_arr[i] = json_object_get_double(tmp);
    }

    /* TRAILING SL */

    len = json_object_array_length(is_trailing_sl);
    benchmarkconfig.n_is_trailing_sl = len;
    bool trailing_sl_true;
    benchmarkconfig.is_trailing_sl = (int*) malloc(len * sizeof(int));
    for (size_t i = 0; i < len; i++)
    {
        tmp = json_object_array_get_idx(is_trailing_sl, i);
        benchmarkconfig.is_trailing_sl[i] = json_object_get_boolean(tmp);

        if (benchmarkconfig.is_trailing_sl[i] == true){
            trailing_sl_true = true;
        }
    }
    
    
    if (trailing_sl_true){
        benchmarkconfig.n_trailing_sl_val = json_object_array_length(trailing_sl_val);
        benchmarkconfig.trailing_sl_val_arr = (double*) malloc(benchmarkconfig.n_trailing_sl_val * sizeof(double));

        for (size_t i = 0; i < benchmarkconfig.n_trailing_sl_val; i++)
        {
            tmp = json_object_array_get_idx(trailing_sl_val, i);
            benchmarkconfig.trailing_sl_val_arr[i] = json_object_get_double(tmp); 
        }
    
    }

    /* QUANTITY */
    
    benchmarkconfig.n_quantity = json_object_array_length(quantity);
    benchmarkconfig.quantity_arr = (int*) malloc(benchmarkconfig.n_quantity* sizeof(int));

    for (size_t i = 0; i < benchmarkconfig.n_quantity; i++)
    {
        tmp = json_object_array_get_idx(quantity, i);
        benchmarkconfig.quantity_arr[i] = json_object_get_int(tmp);
    }
    

    /* SLIDING */

    len = json_object_array_length(sliding);
    benchmarkconfig.n_sliding = len;
    benchmarkconfig.sliding = (int*) malloc(len * sizeof(int));
    for (size_t i = 0; i < len; i++)
    {
        tmp = json_object_array_get_idx(sliding, i);
        benchmarkconfig.sliding[i] = json_object_get_boolean(tmp);
    }
    
    /* INTRADAY */

    len = json_object_array_length(intraday);
    benchmarkconfig.n_intraday = len;
    benchmarkconfig.intraday = (int*) malloc(len * sizeof(int));
    for (size_t i = 0; i < len; i++)
    {
        tmp = json_object_array_get_idx(intraday, i);
        benchmarkconfig.intraday[i] = json_object_get_boolean(tmp);
    }

    /* SKIP_HEADER */

    benchmarkconfig.skip_header = json_object_get_boolean(skip_header);

    //close config file!
    fclose(fp);

    //free json obj.
    json_object_put(parsed_json);

    return benchmarkconfig;
}