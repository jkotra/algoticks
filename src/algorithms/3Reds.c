#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include<stdbool.h>
#include "../../include/dtypes.h"



algoticks_signal analyze(algoticks_row *series, int n_candles){

    /* This is a sell only algo indicator */


    struct Signal signal;
    memset(&signal, 0, sizeof(signal));

    for (int i = 1; i < n_candles; i++)
    {

        if(series[i].close < series[i-1].close){
            signal.sell = 1;
        }
        else {
            signal.neutral = 1;
            signal.sell = 0;
            signal.buy = 0;
            break;
        }
    }

    return signal;

}
