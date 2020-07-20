#include <stdio.h>
#include <string.h>
#include <json-c/json.h>
#include <stdbool.h>
#include <math.h>
#include <ctype.h>
#include "../include/dtypes.h"
#include "../include/debug.h"
#include "../include/misc.h"

void check_config(algoticks_config config){

    if (config.candles <= 0){
        printf("`candles` cannot be <= 0\n");
        exit(1);
    }

    if (config.interval < 0){
        printf("`interval` cannot be <= 0\n");
        exit(1);
    }

    if (config.derivative.derivative_interval < 0){
        printf("`derivative_interval` cannot be <= 0\n");
        exit(1);
    } 

    if (config.quantity <= 0){
        printf("`quantityl` cannot be <= 0\n");
        exit(1);
    }

    if (config.target <= 0){
        printf("`target` cannot be <= 0\n");
        exit(1);
    }    

    if (config.stoploss <= 0){
        printf("`stoploss` cannot be <= 0\n");
        exit(1);
    }    

    if (config.trailing_sl_val <= 0){
        printf("`trailing_sl_val` cannot be <= 0\n");
        exit(1);
    }            

}

void check_settings(algoticks_settings settings){
    if (settings.debug_level <= -1){
        printf("debug_level' cannot be negative!");
        exit(0);
    }

    if (settings.intraday_hour > 24){
        printf("`intraday_hour` cannot be > 24!");
        exit(0);
    }

    if (settings.intraday_min > 60){
        printf("`intraday_min` cannot be > 60");
        exit(0);
    }
}

#if defined(__linux__) || defined(__unix__) || defined(__APPLE__)
#include<dlfcn.h>

void* handle;

algo_func load_algo_func(char *algo){

    algoticks_signal (*analyze)(algoticks_row *, int);

    handle = dlopen(algo, RTLD_LAZY);

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

    return analyze;

}

void close_algo_func(){

    int cr = dlclose(handle);

    if (cr != 0){
        printf("cannot close algo_func\n");
    }
    
}
#endif

#ifdef _WIN32
#include <windows.h>

HINSTANCE hinstLib;

algo_func load_algo_func(char *algo) {

  algo_func ProcAdd;

  // Get a handle to the DLL module.
  hinstLib = LoadLibrary(algo);

  // If the handle is valid, try to get the function address.
  if (hinstLib != NULL) {
    ProcAdd = (algo_func) GetProcAddress(hinstLib, "analyze");

    // If the function address is valid, call the function.
    if (NULL != ProcAdd) {
      return ProcAdd;

    } else {
      printf("cannot open %s\n", algo);
      exit(1);

    }

  }
  else{
      printf("cannot get handle on %s\n", algo);
      exit(1);
  }

}

void close_algo_func() {
  FreeLibrary(hinstLib);
}

# endif


// global vars for write_simresult_to_csv
int simresult_file_header = false;
char simresult_csv_header[1000] = "algo,pnl,datasource,derivative,symbol,candles,interval,target,stoploss,is_trailing_sl,trailing_sl_val,quantity,sliding,intraday,buy_signals,sell_signals,neutral_signals,trgt_hits,sl_hits,b_trgt_hits,s_trgt_hits,b_sl_hits,s_sl_hits,peak,bottom\n";

void write_simresult_to_csv(algoticks_simresult simresult)
{
    // config + sim result
    char buffer[4000];

    sprintf(buffer, "%s,%f,%s,%s,%s,%d,%d,%f,%f,%d,%f,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%f,%f\n",
            simresult.config.algo,
            simresult.pnl,
            simresult.config.datasource,
            simresult.config.derivative.derivative_datasource,
            simresult.config.symbol,
            simresult.config.candles,
            simresult.config.interval,
            simresult.config.target,
            simresult.config.stoploss,
            simresult.config.is_training_sl,
            simresult.config.trailing_sl_val,
            simresult.config.quantity,
            simresult.config.sliding,
            simresult.config.intraday,
            simresult.buy_signals,
            simresult.sell_signals,
            simresult.neutral_signals,
            simresult.trgt_hits,
            simresult.sl_hits,
            simresult.b_trgt_hits,
            simresult.s_trgt_hits,
            simresult.b_sl_hits,
            simresult.s_sl_hits,
            simresult.peak,
            simresult.bottom);
    FILE *fp;
    if (simresult_file_header == false)
    {
        fp = fopen("results.csv", "w+");
        fprintf(fp, simresult_csv_header);
        fclose(fp);

        simresult_file_header = true;
    }

    fp = fopen("results.csv", "a");
    fprintf(fp, buffer);
    fclose(fp);

}

int is_file_exists(const char *filename)
{
    /* try to open file to read */
    FILE *file;
    file = fopen(filename, "r");
    if (file != NULL)
    {
        fclose(file);
        return 1;
    }
    return 0;
}

void create_setting_config_benchmark_files(int type)
{

    /*
     type = 1 = settings.json
     type = 2 = config.json
     type = 3 = benchmark.json
    */

    char settings[1000] = "{ \n"
    "\"print\": true,\n"
    "\"colors\": true, \n"
    "\"debug\": false,  \n"
    "\"debug_level\": 1, \n\n"
    "\"intraday_hour\": 15, \n"
    "\"intraday_min\": 15 \n }";

    char config[1000] = "{\n"
    #ifdef _WIN32
    "\"algo\": \"algorithms/Reds.dll\",\n"
    #else
    "\"algo\": \"algorithms/Reds.so\",\n"
    #endif
    "\"datasource\": \"example.csv\",\n"
    "\"symbol\": \"EXAMPLE\", \n"
    "\"candles\": 3, \n"
    "\"interval\": 0, \n\n"
    "\"quantity\": 100,\n"
    "\"target\": 5, \n"
    "\"stoploss\": 7, \n"
    "\"is_training_sl\": false, \n"
    "\"trailing_sl_val\": 1, \n\n"
    "\"sliding\": false, \n"        
    "\"intraday\": true, \n"
    "\"skip_header\": true \n}\n";

    char benchmark[1000] = "{ \n"
    #ifdef _WIN32
    "\"algo\": [\"algorithms/Greens.dll\", \"algorithms/Reds.dll\"],\n"
    #else
    "\"algo\": [\"algorithms/Greens.so\", \"algorithms/Reds.so\"],\n"
    #endif
    "\"datasource\": [\"example.csv\"],\n"
    "\"symbol\": \"EXAMPLE\",\n"
    "\"candles\": [4,6,8],\n"
    "\"interval\": [5,10,15],\n\n"
    "\"quantity\": [10],\n"
    "\"target\": [1.5,2,2.5],\n"
    "\"stoploss\": [2,2.5,3,3.5],\n"
    "\"is_training_sl\": [true,false],\n"
    "\"trailing_sl_val\": [1,2,3],\n\n"
    "\"sliding\": [true,false],\n"                          
    "\"intraday\": [true,false], \n"
    "\"skip_header\": true \n }\n";

    // write to file

    if (type == 1)
    {
        FILE *settingsf;
        settingsf = fopen("settings.json", "w+");
        fprintf(settingsf, settings);
        fclose(settingsf);

        printf("\nsettings.json created!\n");
    }

    else if (type == 2)
    {
        FILE *configf;
        configf = fopen("config.json", "w+");
        fprintf(configf, config);
        fclose(configf);

        printf("\nconfig.json created!\n");
    }
    else if (type == 3)
    {
        FILE *benchf;
        benchf = fopen("benchmark.json", "w+");
        fprintf(benchf, benchmark);
        fclose(benchf);

        printf("\nbenchmark.json created!\n");
    }
    
}


algoticks_settings parse_settings_from_json(char *filename)
{


    char buffer[4096];
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

    json_object_object_get_ex(parsed_json, "intraday_hour", &intraday_hour);
    json_object_object_get_ex(parsed_json, "intraday_min", &intraday_min);

    settings.print = json_object_get_boolean(print);
    settings.colors = json_object_get_boolean(colors);
    settings.debug = json_object_get_boolean(debug);
    settings.debug_level = json_object_get_int(debug_level);

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

    struct json_object *target;
    struct json_object *stoploss;
    struct json_object *is_training_sl;
    struct json_object *trailing_sl_val;
    struct json_object *quantity;

    struct json_object *sliding;
    struct json_object *intraday;
    struct json_object *skip_header;

    struct Config config = {0};

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

    strncpy(config.algo, json_object_get_string(algo), 32);
    strncpy(config.datasource, json_object_get_string(datasource), 512);
    strncpy(config.symbol, json_object_get_string(symbol), 32);
    config.candles = json_object_get_int(candles);
    config.interval = json_object_get_int(interval);
    config.quantity = json_object_get_int(quantity);
    config.target = json_object_get_double(target);
    config.stoploss = json_object_get_double(stoploss);
    config.is_training_sl = json_object_get_boolean(is_training_sl);
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
            strncpy(config.derivative.derivative_datasource, json_object_get_string(derivative_datasource), 512);
        }

        int derivative_interval_exists = json_object_object_get_ex(derivative, "derivative_interval", &derivative_interval);

        if (derivative_interval_exists) {
            config.derivative.derivative_interval = json_object_get_int(derivative_interval);
        }

    }
    else{
        //set to None
        strncpy(config.derivative.derivative_datasource, "None", 512);

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
    struct json_object *is_training_sl;
    struct json_object *trailing_sl_val;
    struct json_object *quantity;

    struct json_object *sliding;
    struct json_object *intraday;
    struct json_object *skip_header;

    struct BenchmarkConfig benchmarkconfig = {0};

    parsed_json = json_tokener_parse(buffer);

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

    benchmarkconfig.n_algo = json_object_array_length(algo);
    for (int i = 0; i < benchmarkconfig.n_algo; i++)
    {
        tmp = json_object_array_get_idx(algo, i);
        strncpy(benchmarkconfig.algo[i], json_object_get_string(tmp), 32);
    }

    benchmarkconfig.n_datasource = json_object_array_length(datasource);
    for (int i = 0; i < benchmarkconfig.n_datasource; i++)
    {
        tmp = json_object_array_get_idx(datasource, i);
        strncpy(benchmarkconfig.datasource[i], json_object_get_string(tmp), 512);
    }

    strncpy(benchmarkconfig.symbol, json_object_get_string(symbol), 32);

    benchmarkconfig.n_candles = json_object_array_length(candles);
    for (int i = 0; i < benchmarkconfig.n_candles; i++)
    {
        tmp = json_object_array_get_idx(candles, i);
        benchmarkconfig.candles[i] = json_object_get_int(tmp);
    }

    benchmarkconfig.n_interval = json_object_array_length(interval);
    for (int i = 0; i < benchmarkconfig.n_interval; i++)
    {
        tmp = json_object_array_get_idx(interval, i);
        benchmarkconfig.interval[i] = json_object_get_int(tmp);
    }

    benchmarkconfig.n_target = json_object_array_length(target);
    for (int i = 0; i < benchmarkconfig.n_target; i++)
    {
        tmp = json_object_array_get_idx(target, i);
        benchmarkconfig.target[i] = json_object_get_double(tmp);
    }

    benchmarkconfig.n_stoploss = json_object_array_length(stoploss);
    for (int i = 0; i < benchmarkconfig.n_stoploss; i++)
    {
        tmp = json_object_array_get_idx(stoploss, i);
        benchmarkconfig.stoploss[i] = json_object_get_double(tmp);
    }

    benchmarkconfig.n_is_training_sl = json_object_array_length(is_training_sl);
    for (int i = 0; i < benchmarkconfig.n_is_training_sl; i++)
    {
        tmp = json_object_array_get_idx(is_training_sl, i);
        benchmarkconfig.is_training_sl[i] = json_object_get_boolean(tmp);
    }

    benchmarkconfig.n_trailing_sl_val = json_object_array_length(trailing_sl_val);
    for (int i = 0; i < benchmarkconfig.n_trailing_sl_val; i++)
    {
        tmp = json_object_array_get_idx(trailing_sl_val, i);
        benchmarkconfig.trailing_sl_val[i] = json_object_get_double(tmp);
    }

    benchmarkconfig.n_quantity = json_object_array_length(quantity);
    for (int i = 0; i < benchmarkconfig.n_quantity; i++)
    {
        tmp = json_object_array_get_idx(quantity, i);
        benchmarkconfig.quantity[i] = json_object_get_int(tmp);
    }

    benchmarkconfig.n_sliding = json_object_array_length(sliding);
    for (int i = 0; i < benchmarkconfig.n_sliding; i++)
    {
        tmp = json_object_array_get_idx(sliding, i);
        benchmarkconfig.sliding[i] = json_object_get_boolean(tmp);
    }

    benchmarkconfig.n_intraday = json_object_array_length(intraday);
    for (int i = 0; i < benchmarkconfig.n_intraday; i++)
    {
        tmp = json_object_array_get_idx(intraday, i);
        benchmarkconfig.intraday[i] = json_object_get_boolean(tmp);
    }

    benchmarkconfig.skip_header = json_object_get_boolean(skip_header);

    //parse derivative if exists.
    int derivative_exists = json_object_object_get_ex(parsed_json, "derivative", &derivative);

    if (derivative_exists){
        struct json_object *derivative_datasource, *derivative_interval;

        int derivative_datasource_exists = json_object_object_get_ex(derivative, "derivative_datasource", &derivative_datasource);
        if (derivative_datasource_exists){
            strncpy(benchmarkconfig.derivative.derivative_datasource, json_object_get_string(derivative_datasource), 512);
        }

        int derivative_interval_exists = json_object_object_get_ex(derivative, "derivative_interval", &derivative_interval);

        if (derivative_interval_exists) {
            benchmarkconfig.derivative.derivative_interval = json_object_get_int(derivative_interval);
        }

    }
    else{
        //set to None
        strncpy(benchmarkconfig.derivative.derivative_datasource, "None", 512);

    }
    //close config file!
    fclose(fp);

    //free json obj.
    json_object_put(parsed_json);

    return benchmarkconfig;
}

algoticks_config filter_boundaries(algoticks_config config, int is_short)
{

    if (is_short == true)
    {

        config.target = -(fabs(config.target));
        config.stoploss = fabs(config.stoploss);
        config.trailing_sl_val = -(fabs(config.trailing_sl_val));
    }
    else
    {

        config.target = fabs(config.target);
        config.stoploss = -(fabs(config.stoploss));
        config.trailing_sl_val = fabs(config.trailing_sl_val);
    }

    return config;
}

//essential boundary checking fuctions
int is_target_hit(algoticks_dashboard dashboard, float target)
{
    if (dashboard.is_short == true)
    {
        if ((dashboard.b - dashboard.a) < target)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        if ((dashboard.b - dashboard.a) >= target)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
}

int is_stoploss_hit(algoticks_dashboard dashboard, float stoploss)
{
    if (dashboard.is_short == true)
    {
        // sell / short
        if ((dashboard.b - dashboard.a) > stoploss)
        {
            return true;
        }
        else
        {
            //printf("SL HIT => Received: %f %f %d\n", abs_pnl, stoploss, is_short);
            return false;
        }
    }
    else
    {
        // buy
        if ((dashboard.b - dashboard.a) <= stoploss)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
}

/* TODO - brokerage calculator */
float brokerage_calc()
{

    return 1.0;
}


void convert_to_lowercase(char *str){
    for (int i = 0; i < strlen(str); i++)
    {
        str[i] = tolower(str[i]);
    }
    
}

void remove_quotes(char *str){ 
    int i;

    for (i = 1; i < strlen(str)-1; i++)
    {
        str[i-1] = str[i];
    }
    str[i-1] = '\0';
}

void chomp(char *s)
{
    /* This removes newline at end of string */
    while (*s && *s != '\n' && *s != '\r')
        s++;
    *s = 0;
}