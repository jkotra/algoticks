#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "../../include/dtypes.h"

#ifdef _WIN32
__declspec(dllexport)
#endif
algoticks_signal analyze(algoticks_row *series, int n_candles){

    /* This is a buy only algo indicator */

    struct Signal signal;
    memset(&signal, 0, sizeof(signal));

    for (int i = 1; i < n_candles; i++)
    {

        if(series[i].close > series[i-1].close){
            signal.buy = true;
        }
        else {
            signal.neutral = true;
            signal.sell = false;
            signal.buy = false;
            break;
        }
    }

    return signal;

}
