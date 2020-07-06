#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include "../../include/dtypes.h"

/*

3TECHNICAL DETAILS#

NAME:  Two Black Gapping (TwoBlackGapping.so / TwoBlackGapping.dll)
MIN REQ CANDLES: 6
*/

algoticks_signal analyze(algoticks_row *series, int n_candles)
{

    if (n_candles < 6)
    {
        printf("Error: TwoBlackGapping requires atleast 6 candles.\n");
        exit(1);
    }

    struct Signal signal;
    memset(&signal, 0, sizeof(signal));
    signal.neutral = true;

    if (series[1].close > series[0].close && series[1].high > series[0].high)
    {
        if (series[2].close > series[1].close && series[2].high > series[1].high)
        {
            if (series[3].close < series[2].close)
            {

                if (series[4].close < series[3].close)
                {
                    if (series[5].open != series[4].close)
                    {
                        signal.sell = true;
                        signal.buy = false;
                        signal.neutral = false;
                    }
                }
            }
        }
    }

    return signal;
}
