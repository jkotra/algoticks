typedef struct Settings
{
    int print;
    int colors;
    int debug;
    int debug_level;

    char config_f[512];
    char benchmark_f[512];

    int is_live_data;
    int is_live_data_socket;
    char socket_port[12];
    int is_derivative;
    int is_benchmark; /* for windows usage */

    int intraday_hour;
    int intraday_min;
    
}algoticks_settings;

typedef struct Dervative{
    char derivative_datasource[512];
    int derivative_interval;
}algoticks_derivative;

typedef struct BenchmarkConfig
{
    char algo[1024][32];
    int n_algo;

    char datasource[1024][512];
    int n_datasource;

    algoticks_derivative derivative;
    int n_derivative;

    char symbol[32];

    int candles[1024];
    int n_candles;

    int interval[1024];
    int n_interval;

    double target[1024];
    int n_target;

    double stoploss[1024];
    int n_stoploss;

    int is_training_sl[2];
    int n_is_training_sl;

    double trailing_sl_val[1024];
    int n_trailing_sl_val;

    int quantity[1024];
    int n_quantity;

    int sliding[2];
    int n_sliding;

    int intraday[2];
    int n_intraday;

    int skip_header;
    
}algoticks_benchmarkconfig;


typedef struct Config
{
    char algo[32];
    char datasource[512];
    algoticks_derivative derivative;
    char symbol[32];
    int candles;
    int interval;

    char callbacks[6][32];
    int n_callbacks;

    double target;
    double stoploss;
    int is_training_sl;
    double trailing_sl_val;
    int quantity;
    
    int sliding;
    int intraday;

    int skip_header;
}algoticks_config;


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
    char ti_others[2048];

}algoticks_tow_ti;

typedef struct Row
{
    /* dataformat to store CSV data */
    char date[32];
    float open;
    float high;
    float low;
    float close;
    int volume;

    algoticks_tow_ti technical_indicators;

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

    int tsl;
    float tsl_t;
    float tsl_sl;
    
    char date[64];
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
