typedef struct Settings
{
    int print;
    int colors;
    int debug;
    int debug_level;

    char *config_f;
    char *benchmark_f;

    int is_live_data;
    int is_live_data_socket;
    char *socket_port;
    int is_derivative;
    int is_benchmark; /* for windows usage */

    int intraday_hour;
    int intraday_min;
    
}algoticks_settings;

typedef struct Dervative{
    char *derivative_datasource;
    int derivative_interval;
}algoticks_derivative;



typedef struct Config
{
    char *algo;
    char *datasource;
    algoticks_derivative derivative;
    char *symbol;
    int candles;
    int interval;

    char **callbacks;
    int n_callbacks;

    double target;
    double stoploss;
    int is_trailing_sl; /* FLAG */
    double trailing_sl_val;
    int quantity;
    
    int sliding; /* FLAG */
    int intraday; /* FLAG */

    int skip_header; /* FLAG */
}algoticks_config;

typedef struct BenchmarkConfig
{

    char *symbol;

    int n_algo;
    char **algo_arr;
    
    int n_datasource;
    char **datasource_arr;

    algoticks_derivative derivative;

    int n_candles;
    int *candles_arr;

    int n_interval;
    int *interval_arr;

    int n_target;
    double *target_arr;

    int n_stoploss;
    double *stoploss_arr;
    
    int n_is_trailing_sl;
    int *is_trailing_sl; /* array of bool */

    int n_trailing_sl_val;
    double *trailing_sl_val_arr;

    int n_quantity; 
    int *quantity_arr;
    
    int n_sliding;
    int *sliding; /* array of bool */

    int n_intraday;
    int *intraday; /* array of bool */

    int skip_header;
    
}algoticks_benchmarkconfig;

typedef struct Dashboard
{
    char date[32];
    float a;
    float b;
    float q;
    
    float pnl;
    int is_short;
}algoticks_dashboard;

typedef struct row_ti{

    int is_ti1_p;
    float ti1;

    int is_ti2_p;
    float ti2;

    int is_ti3_p;
    float ti3;

    int is_ti_others_p;
    char *ti_others;

}algoticks_row_ti;

typedef struct Row
{
    /* dataformat to store CSV data */
    char date[32];
    float open;
    float high;
    float low;
    float close;
    int volume;

    algoticks_row_ti technical_indicators;

    int curr;
    int n_rows; //this is to be set in case of Row array of non-predetermined size.
}algoticks_row;


typedef struct PositionResult {

    int n_steps;

    char recv_signal;
    char hit_type[4];

    float pnl;

    int curr;
    int eof;
    algoticks_row lastrow;
    
}algoticks_positionresult;


typedef struct SimResult{
    float pnl;

    algoticks_config config;
    algoticks_settings settings;

    int buy_signals;
    int sell_signals;
    int neutral_signals;

    int trgt_hits;
    int sl_hits;

    int b_trgt_hits;
    int b_sl_hits;

    int s_trgt_hits;
    int s_sl_hits;

    float peak;
    float bottom;

}algoticks_simresult;


typedef struct Signal {
    int buy;
    int neutral;
    int sell;
}algoticks_signal;

typedef struct Event {

    int from_sim;
    int from_pos;

    algoticks_signal signal;
    int t_h;
    int sl_h;

    int tsl; /* trailing SL */
    float tsl_t; /* trailing SL target */
    float tsl_sl; /* trailing SL stoploss */
    
    char date[32];
    float a;
    float b;
    float pnl;

}algoticks_event;

//function pointers
#if defined(__linux__) || defined(__unix__) || defined(__APPLE__)
typedef algoticks_signal (*algo_func)(algoticks_row *, int);
typedef int (*callback_func)(algoticks_event *);
#endif

#ifdef _WIN32
typedef algoticks_signal (__cdecl *algo_func)(algoticks_row *, int);
typedef int (__cdecl *callback_func)(algoticks_event *);
#endif

typedef struct CallbackLoader{
    callback_func callback_func;
    void *handle; //this need to be casted according to os
}algoticks_cb_l;
