#include<time.h>
#include<stdio.h>
#include<string.h>
#include<stdbool.h>
#include "../include/dtypes.h"
#include "../include/debug.h"
#include "../include/misc.h"


void debug_msg(algoticks_settings settings, int debug_level, char *op, char *loc, char *remarks){
    
    //check if debug is enabled!
    if (settings.debug != true){
        return;
    }

    //if debug level less then 0 or greater then 3.
    if ( (debug_level <= 0) == true || (settings.debug_level > 3) == true ){
        return;
    }

    // if less then required debug level.
    if (settings.debug_level < debug_level){
        return;
    }

    //HRT = Human readable time
    char hrt[32];

    time_t now;

    //get time
    time(&now);

    sprintf(hrt, "%s", ctime(&now));

    //remove new line at end
    chomp(hrt);

    char buffer[5000];
    sprintf(buffer, "{\"date\": \"%s\", \"op\": \"%s\", \"loc\": \"%s\", \"msg\": \"%s\"}\n", hrt, op, loc, remarks);

    printf("%s", buffer);


}

void print_config_struct(algoticks_config config){
    printf("\n===*===\n");
    printf("[DEBUG] config.algo = %s\n", config.algo);
    printf("[DEBUG] config.datasource = %s\n", config.datasource);
    printf("[DEBUG] config.derivative[derivative_datasource, derivative_interval] = %s %d\n", config.derivative.derivative_datasource, config.derivative.derivative_interval);
    printf("[DEBUG] config.symbol = %s\n", config.symbol);
    printf("[DEBUG] config.interval = %d\n", config.interval);
    printf("[DEBUG] config.n_callbacks = %d\n", config.n_callbacks);

    if (config.n_callbacks > 0){
    printf("[DEBUG] config.callbacks =");
    for (int i = 0; i < config.n_callbacks; i++)
    {
        printf(" %s ",config.callbacks[i]);
    }
    printf("\n");

    }    

    printf("[DEBUG] config.candles = %d\n", config.candles);
    printf("[DEBUG] config.target = %f\n", config.target);
    printf("[DEBUG] config.stoploss = %f\n", config.stoploss);
    printf("[DEBUG] config.is_trailing_sl = %d\n", config.is_training_sl);
    printf("[DEBUG] config.trailing_sl_val = %f\n", config.trailing_sl_val);
    printf("[DEBUG] config.quantity = %d\n", config.quantity);

    printf("[DEBUG] config.sliding = %d\n", config.sliding);
    printf("[DEBUG] config.intraday = %d\n", config.intraday);
    printf("[DEBUG] config.skip_header = %d\n", config.skip_header);
}

void print_dashboard_struct(algoticks_dashboard dashboard){
    printf("\n===*===\n");
    printf("[DEBUG] Dashboard.a = %f\n", dashboard.a);
    printf("[DEBUG] Dashboard.b = %f\n", dashboard.b);
    printf("[DEBUG] Dashboard.date = %s\n", dashboard.date);
    printf("[DEBUG] Dashboard.pnl = %f\n", dashboard.pnl);
    printf("[DEBUG] Dashboard.is_short = %d\n", dashboard.is_short);
    printf("\n===*===\n");
}

void print_signal_struct(algoticks_signal signal){
    printf("\n===*===\n");
    printf("[DEBUG] signal.buy = %d\n", signal.buy);
    printf("[DEBUG] signal.neutral = %d\n", signal.neutral);
    printf("[DEBUG] signal.sell = %d\n", signal.sell);
    printf("\n===*===\n");
}

void print_simresult_struct(algoticks_simresult simresult){
    printf("\n===*===\n");
    printf("[DEBUG] signal.pnl = %f\n", simresult.pnl);
    printf("[DEBUG] signal.buy_signals = %d\n", simresult.buy_signals);
    printf("[DEBUG] signal.sell_signals = %d\n", simresult.sell_signals);
    printf("[DEBUG] signal.neutral_signals = %d\n", simresult.neutral_signals);
    printf("[DEBUG] signal.trgt_hits = %d\n", simresult.trgt_hits);
    printf("[DEBUG] signal.sl_hits = %d\n", simresult.sl_hits);
    printf("\n===*===\n");
}

void print_row_struct(algoticks_row row){
    printf("\n===*===\n");
    printf("[DEBUG] row.date = %s\n", row.date);
    printf("[DEBUG] row.open = %f\n", row.open);
    printf("[DEBUG] row.high = %f\n", row.high);
    printf("[DEBUG] row.low = %f\n", row.low);
    printf("[DEBUG] row.close = %f\n", row.close);
    printf("[DEBUG] row.volume = %d\n", row.volume);

    printf("[DEBUG] ti1 ti2 ti3 t_others = %f %f %f %s\n", row.technical_indicators.ti1,
                                                           row.technical_indicators.ti2,
                                                           row.technical_indicators.ti3,
                                                           row.technical_indicators.ti_others);
    printf("\n===*===\n");
}