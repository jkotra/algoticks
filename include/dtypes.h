typedef struct Settings
{
    int print;
    int colors;
    int debug;
}algoticks_settings;

typedef struct BenchmarkConfig
{
    char algo[1024][32];
    int n_algo;

    char datasource[1024][512];
    int n_datasource;

    char symbol[32];

    int candles[1024];
    int n_candles;

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

    int intraday[2];
    int n_intraday;

    int skip_header;
    
}algoticks_benchmarkconfig;

typedef struct Config
{
    char algo[32];
    char datasource[512];
    char symbol[32];
    int candles;

    double target;
    double stoploss;
    int is_training_sl;
    double trailing_sl_val;
    int quantity;

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


typedef struct Row
{
    /* dataformat to store CSV data */
    char date[32];
    float open;
    float high;
    float low;
    float close;
    int volume;
    int n_rows;
}algoticks_row;


typedef struct PositionResult {

    int n_steps;

    char recv_signal;
    char hit_type[4];

    float pnl;

    int curr;
    int eof;
    
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

    float peak;
    float bottom;

}algoticks_simresult;


typedef struct Signal {
    int buy;
    int neutral;
    int sell;
}algoticks_signal;