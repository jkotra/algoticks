#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include "../../include/dtypes.h"

/*

3TECHNICAL DETAILS#

NAME:  Abandoned Baby (AbandonedBaby.so / AbandonedBaby.dll)
MIN REQ CANDLES: 6
*/


#ifdef _WIN32
__declspec(dllexport)
#endif
algoticks_signal analyze(algoticks_row *series, int n_candles)
{


    if (n_candles < 6)
    {
        printf("Error: AbandonedBaby requires atleast 6 candles.\n");
        exit(1);
    }

    struct Signal signal;
    memset(&signal, 0, sizeof(signal));
    signal.neutral = true;

    for (int i = 1; i < 3; i++)
    {
        if (series[i].low > series[i - 1].low)
        {
            return signal;
        }
    }

    //abandoned baby
    if (series[3].low > series[4].low && series[4].open != series[3].close)
    {
        if (series[5].close > series[4].close && series[5].close > series[3].high)
        {

            signal.buy = true;
            signal.sell = false;
            signal.neutral = false;
        }
    }

    return signal;
}