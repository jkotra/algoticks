#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../include/dtypes.h"
#include "../include/sim.h"
#include "../include/sim_derivative.h"
#include "../include/benchmark.h"
#include "../include/parser.h"
#include "../include/misc.h"
#include "../include/debug.h"

struct program_args
{
    int debug;
    char *debug_level;

    int benchmark;

    int live_datasource;
    int live_datasource_socket;
    char *tcp_socket_port;

    int derivative;

    char *settings_f;
    char *config_f;
    char *benchmark_f;

    /* WINDOWS ONLY */
    char *program_args_f;
};

#if defined(__linux__)
#include <argp.h>

const char *argp_program_version =
    "algoticks v2.0";
const char *argp_program_bug_address =
    "<jagadeesh@stdin.top>";

/* Program documentation. */
static char doc[] =
    "Algoticks - algorithmic trading simulator.";

/* The options we understand. */
static struct argp_option options[] = {
    {"debug", 'D', "LEVEL", 0, "Debug mode."},

    {"live", 'l', 0, 0, "Live mode. Wait for data at EOF"},
    {"derivative", 'd', 0, 0, "Derivative mode"},
    {"benchmark", 'b', 0, 0, "Benchmark mode"},
    {"socket", 's', "PORT",  0, "Open socket to receive data at EOF"},


    {"configfile", 'C', "FILE", 0, "Config file."},
    {"benchmarkfile", 'B', "FILE", 0, "Benchmark file."},
    {"settingsfile", 'S', "FILE", 0, "settings file."},
    {0}
};

static error_t parse_opt(int key, char *arg, struct argp_state *state)
{
    /* Get the input argument from argp_parse, which we
     know is a pointer to our arguments structure. */
    struct program_args *arguments = state->input;

    switch (key)
    {
    case 'D':
        arguments->debug = 1;
        arguments->debug_level = arg;
        break;
    case 'l':
        arguments->live_datasource = 1;
        break;

    case 'd':
        arguments->derivative = 1;
        break;

    case 'b':
        arguments->benchmark = 1;
        break;

    case 's':
        arguments->live_datasource_socket = 1;
        arguments->tcp_socket_port = arg;
        break;

    case 'C':
        arguments->config_f = arg;
        break;

    case 'B':
        arguments->benchmark_f = arg;
        break;

    case 'S':
        arguments->settings_f = arg;
        break;        

    default:
        return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

static struct argp argp = { options, parse_opt, 0, doc };
#endif

int main(int argc, char **argv)
{

    struct program_args arguments = {0};

    /* Defaults */
    arguments.config_f = "config.json";
    arguments.benchmark_f = "benchmark.json";
    arguments.settings_f = "settings.json";
    arguments.tcp_socket_port = "6060";
    arguments.debug = false;
    arguments.debug_level = 0;
    
    #if defined(__linux__)
    argp_parse (&argp, argc, argv, 0, 0, &arguments);
    #endif

    //in windows, we only take in single settings file as arg.
    #ifdef _WIN32
    for (size_t i = 0; i < argc; i++)
    {
        if(strstr(argv[i], ".json") != NULL){
            arguments.settings_f = argv[i];
            break;
        }
    }
    
    #endif

    struct Config config;
    struct Settings settings;

    settings = parse_settings_from_json(arguments.settings_f);

    #ifdef _WIN32
    arguments.config_f = settings.config_f;
    arguments.benchmark_f = settings.benchmark_f;
    #endif
    
    // mutually exclusive ways to feed data.
    if (settings.is_live_data && settings.is_live_data_socket){
        settings.is_live_data_socket = false;
    }

    #if defined(__linux__)
    if (arguments.debug) {
        settings.debug = true;
        settings.debug_level = atoi(arguments.debug_level);
    }

    if (arguments.derivative){
        settings.is_derivative = true;
    }

    if (arguments.live_datasource){
        settings.is_live_data = true;
    }

    if (arguments.live_datasource_socket){
        settings.is_live_data_socket = true;
        settings.socket_port = (char*) malloc(5 * sizeof(char));
        strncpy(settings.socket_port, arguments.tcp_socket_port, 5);
    }
    #endif

    config = parse_config_from_json(arguments.config_f);

    if (arguments.benchmark || settings.is_benchmark){
        run_benchmark(arguments.benchmark_f, settings);
    }
    else if(arguments.derivative || settings.is_derivative){
        run_sim_w_derivative(&settings, &config);
    }
    else{
        run_sim(&settings, &config);
    }
    

    free_algoticks_config(&config);
    free_algoticks_settings(&settings);
    
    return 0;
}