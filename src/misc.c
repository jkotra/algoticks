#include<stdio.h>
#include<string.h>
#include<json-c/json.h>
#include "../include/dtypes.h"
#include "../include/debug.h"



void chomp(char *s) {
    /* This removes newline at end of string */
    while(*s && *s != '\n' && *s != '\r') s++;
    *s = 0;
}

algoticks_settings parse_settings_from_json(char *filename){

    /* Example:

    {
    "print": true,
    "colors": true,
    "debug": true
    }

    */

    char buffer[4096];
    FILE *fp;

    fp = fopen(filename,"r");
    if (fp == NULL){
        printf("cannot Read: %s \n", filename);
        exit(1);
    }
    fread(buffer, 1024, 1, fp);


    struct json_object *parsed_json;
    struct json_object *print;
	struct json_object *colors;
    
    struct json_object *debug;

    struct Settings settings;

    parsed_json = json_tokener_parse(buffer);

    json_object_object_get_ex(parsed_json, "print", &print);
	json_object_object_get_ex(parsed_json, "colors", &colors);
    json_object_object_get_ex(parsed_json, "debug", &debug);
    


    settings.print = json_object_get_boolean(print);
    settings.colors = json_object_get_boolean(colors);
    settings.debug = json_object_get_boolean(debug);
    

    return settings;
}

algoticks_config parse_config_from_json(char *filename){

    /* Example JSON:

    {
    "algo": "",
    "datasource": "",
    "symbol": "",
    "candles": 4,

    "quantity": 20,
    "start_date": "",
    "intraday": true

    }

    */

    char buffer[4096];
    FILE *fp;

    fp = fopen(filename,"r");
    if (fp == NULL){
        printf("cannot Read: %s \n", filename);
         exit(1);
    }
    fread(buffer, 1024, 1, fp);


    struct json_object *parsed_json;
    
    struct json_object *algo;
    struct json_object *datasource;
	struct json_object *symbol;
    struct json_object *candles;
    struct json_object *target;
    struct json_object *stoploss;
	struct json_object *is_training_sl;
    struct json_object *trailing_sl_val;
	struct json_object *quantity;
	struct json_object *intraday;
    struct json_object *skip_header;

    struct Config config;
    
    parsed_json = json_tokener_parse(buffer);


    json_object_object_get_ex(parsed_json, "algo", &algo);
    json_object_object_get_ex(parsed_json, "datasource", &datasource);
	json_object_object_get_ex(parsed_json, "symbol", &symbol);
    json_object_object_get_ex(parsed_json, "candles", &candles);
    json_object_object_get_ex(parsed_json, "target", &target);
	json_object_object_get_ex(parsed_json, "stoploss", &stoploss);
    json_object_object_get_ex(parsed_json, "is_training_sl", &is_training_sl);
    json_object_object_get_ex(parsed_json, "trailing_sl_val", &trailing_sl_val);
    json_object_object_get_ex(parsed_json, "quantity", &quantity);
    json_object_object_get_ex(parsed_json, "intraday", &intraday);
    json_object_object_get_ex(parsed_json, "skip_header", &skip_header);

    
     strncpy(config.algo, json_object_get_string(algo), 32);
     strncpy(config.datasource, json_object_get_string(datasource), 512);
     strncpy(config.symbol, json_object_get_string(symbol),32);
     config.candles = json_object_get_int(candles);
     config.quantity = json_object_get_int(quantity);
     config.target = json_object_get_double(target);
     config.stoploss = json_object_get_double(stoploss);
     config.is_training_sl = json_object_get_boolean(is_training_sl);
     config.trailing_sl_val = json_object_get_double(trailing_sl_val);
     config.intraday = json_object_get_boolean(intraday);
     config.skip_header = json_object_get_boolean(skip_header);

    return config;
}



/* brokerage calculator */


float brokerage_calc() {

    return 1.0;
}

