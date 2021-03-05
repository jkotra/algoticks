#include <stdio.h>
#include <string.h>
#include <json-c/json.h>
#include <stdbool.h>
#include <math.h>
#include <ctype.h>
#include <assert.h>
#include "../include/dtypes.h"
#include "../include/debug.h"
#include "../include/misc.h"

#if defined(__linux__) || defined(__unix__) || defined(__APPLE__)
#include<dlfcn.h>

algoticks_cb_l load_cb(char *cb){
    
    void* handle;
    callback_func callback;

    printf("%s\n", cb);

    handle = dlopen(cb, RTLD_LAZY);

    if (!handle)
    {
        fprintf(stderr, "Error: %s\n", dlerror());
        exit(EXIT_FAILURE);
    }

    *(void **)(&callback) = dlsym(handle, "callback_f");

    if (!callback)
    {
        fprintf(stderr, "Error: %s\n", dlerror());
        dlclose(handle);
        exit(EXIT_FAILURE);
    }

    algoticks_cb_l loader;
    loader.callback_func = callback;
    loader.handle = handle;

    return loader;

}

//this is for algo loading
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

algoticks_cb_l load_cb(char *algo) {
  
  HINSTANCE hinstLib;
  callback_func ProcAdd;

  // Get a handle to the DLL module.
  hinstLib = LoadLibrary(algo);

  // If the handle is valid, try to get the function address.
  if (hinstLib != NULL) {
    ProcAdd = (algo_func) GetProcAddress(hinstLib, "callback_f");

    // If the function address is valid, call the function.
    if (NULL != ProcAdd) {
      algoticks_cb_l l;
      l.callback_func = ProcAdd;
      l.handle = hinstLib;

      return l;

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

void write_simresult_to_csv(algoticks_simresult *simresult)
{
    // config + sim result
    char *buffer = (char*) malloc(4000 * sizeof(char));

    sprintf(buffer, "%s,%f,%s,%s,%s,%d,%d,%f,%f,%d,%f,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%f,%f\n",
            simresult->config.algo,
            simresult->pnl,
            simresult->config.datasource,
            simresult->config.derivative.derivative_datasource,
            simresult->config.symbol,
            simresult->config.candles,
            simresult->config.interval,
            simresult->config.target,
            simresult->config.stoploss,
            simresult->config.is_trailing_sl,
            simresult->config.trailing_sl_val,
            simresult->config.quantity,
            simresult->config.sliding,
            simresult->config.intraday,
            simresult->buy_signals,
            simresult->sell_signals,
            simresult->neutral_signals,
            simresult->trgt_hits,
            simresult->sl_hits,
            simresult->b_trgt_hits,
            simresult->s_trgt_hits,
            simresult->b_sl_hits,
            simresult->s_sl_hits,
            simresult->peak,
            simresult->bottom);
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
    free(buffer);
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

void filter_boundaries(algoticks_config *config, int is_short)
{

    if (is_short == true)
    {

        config->target = -(fabs(config->target));
        config->stoploss = fabs(config->stoploss);
        config->trailing_sl_val = -(fabs(config->trailing_sl_val));
    }
    else
    {

        config->target = fabs(config->target);
        config->stoploss = -(fabs(config->stoploss));
        config->trailing_sl_val = fabs(config->trailing_sl_val);
    }
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

void free_algoticks_config(algoticks_config *config){
    free(config->algo);
    free(config->datasource);
    free(config->symbol);

    if (config->derivative.derivative_interval != 0){
        free(config->derivative.derivative_datasource);
    }
    

    if (config->n_callbacks != 0) {
        for (size_t i = 0; i < config->n_callbacks; i++)
        {
            free(config->callbacks[i]);
        }
    }


}

void free_algoticks_settings(algoticks_settings *settings){
    free(settings->config_f);
    free(settings->benchmark_f);
    free(settings->socket_port);
}