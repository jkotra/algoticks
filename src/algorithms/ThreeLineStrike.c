#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include "../../include/dtypes.h"


/*

3TECHNICAL DETAILS#

NAME:  Three Line Strike (ThreeLineStrike.so / ThreeLineStrike.dll)
MIN REQ CANDLES: 4
*/

#ifdef _WIN32
__declspec(dllexport)
#endif
algoticks_signal analyze(algoticks_row *series, int n_candles){



    if (n_candles < 4){
        printf("Error: ThreeLineStrike requires atleast 4 candles.\n");
        exit(1);
    }


    struct Signal signal;
    memset(&signal, 0, sizeof(signal));
    signal.neutral = true;

    if (series[0].low > series[1].low){
        if (series[1].low > series[2].low){
            if (series[3].close >= series[0].high){

                signal.buy = true;
                signal.sell = false;
                signal.neutral = false;

            }
            
        }
    }

    return signal;

}
