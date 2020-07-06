#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include "../../include/dtypes.h"

/*

3TECHNICAL DETAILS#

NAME:  EveningStar (EveningStar.so / EveningStar.dll)
MIN REQ CANDLES: 4
*/

int tall_candle_th = 3;

algoticks_signal analyze(algoticks_row *series, int n_candles)
{


    if (n_candles < 4)
    {
        printf("Error: EveningStar requires atleast 4 candles.\n");
        exit(1);
    }

    struct Signal signal;
    memset(&signal, 0, sizeof(signal));
    signal.neutral = true;

    if (series[1].close > series[0].close)
    {

        //check if c0 is red
        int c0_red = (series[0].open - series[0].close) < 0;

        if (c0_red)
        {
            if ((series[1].open - series[1].close) >= (series[0].close - series[0].open) * tall_candle_th)
            {

                if (series[2].close < series[1].close)
                {
                    if (series[3].close < series[2].close)
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
