#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "../include/dtypes.h"
#include "../include/misc.h"
#include "../include/dashboard.h"

#ifndef _WIN32
#include <dlfcn.h>
#else
#include <windows.h>
#endif

callback_func *cb;
void **cb_h;
int n;


int load_callbacks(algoticks_config config){

    if (config.n_callbacks > 0){
    cb = (callback_func *) malloc(config.n_callbacks * sizeof(callback_func));
    cb_h = (void **) malloc(config.n_callbacks * sizeof(void));
    n = config.n_callbacks;
    }

    for (int i = 0; i < n; i++)
    {
        algoticks_cb_l l;
        l = load_cb(config.callbacks[i]);
        cb[i] = l.callback_func;
        cb_h[i] = l.handle;
    }

    return true;

}
int send_callbacks(algoticks_event event){

    for (int i = 0; i < n; i++)
    {

        ((callback_func)cb[i]) (&event);

    }

    return true;

}

int close_callbacks(){

for (int i = 0; i < n; i++)
    {
       #ifdef _WIN32
       FreeLibrary(cb_h[i]);
       #else
       dlclose(cb_h[i]);
       #endif
    }

    free(cb);
    free(cb_h);

    return true;

}

algoticks_event make_event_from_positionresult(algoticks_positionresult positionresult){

    algoticks_event ev = {0};
    if (strcmp(positionresult.hit_type, "SL") == 0) {
        ev.sl_h = true;
    }

    if (strcmp(positionresult.hit_type, "T") == 0) {
        ev.t_h = true;
    }

    ev.pnl = positionresult.pnl;
    return ev;
}

algoticks_event make_event_from_signal(algoticks_signal signal){

    algoticks_event ev = {0};
    ev.signal = signal;
    return ev;
}

algoticks_event make_event_from_position(algoticks_row pos_storage, algoticks_dashboard dashboard){

    algoticks_event ev = {0};

    strncpy(ev.date, pos_storage.date, 32);
    ev.a = dashboard.a;
    ev.b = dashboard.b;
    ev.pnl = getPnL(dashboard);
    ev.from_pos = true;

    return ev;
}