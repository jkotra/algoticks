#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../include/dtypes.h"
#include "../include/misc.h"
#include "../include/sim.h"
#include "../include/sim_derivative.h"
#include "../include/benchmark.h"
#include "../include/debug.h"

int debug_flag = false;
int debug_lvl = false;
int benchmark_flag = false;
int config_need_exit_flag = false;
int live_datasource_flag = false;
int live_datasource_tcp_socket_flag = false;

char tcp_socket_port[12] = "5757";

int derivative_flag = false;

char settings_file[64] = "settings.json";
char config_file[64] = "config.json";
char benchmark_file[64] = "benchmark.json";

void print_version_and_exit()
{
    printf("algoticks v1.4\n");
    exit(0);
}

void print_help_and_exit()
{

    printf("-V -v\t\t\tPrint Version and Exit.\n");
    printf("-H -h\t\t\tPrint this message and Exit.\n");
    printf("-D\t\t\tEnable Debug.\n");
    printf("-L\t\t\twait for new data at EOF in datasource.\n");
    printf("-S [PORT](Default: 5757)\t\t\twait for new data at EOF on TCP socket.\n");
    printf("--derivative\t\tDerivative mode.\n\n\n");

    printf("--settings [*.JSON]\t\t\tCustom settings file. Default: settings.json\n");
    printf("--config [*.JSON]\t\t\tCustom config file. Default: config.json\n");
    printf("--benchmark -B [(Optional)[*.JSON]\tCustom benchmark file. Default: benchmark.json\n");
    exit(0);
}

int main(int argc, char **argv)
{

    if (argc > 1)
    {

        for (int i = 0; i < argc; i++)
        {

            //version
            if (strcmp(argv[i], "-V") == 0 || strcmp(argv[i], "-v") == 0)
            {
                print_version_and_exit();
            }

            //help
            else if (strcmp(argv[i], "-H") == 0 || strcmp(argv[i], "-h") == 0)
            {
                print_help_and_exit();
            }

            //derivative
            else if (strcmp(argv[i], "--derivative") == 0)
            {
                derivative_flag = true;
            }

            //debug
            else if (strcmp(argv[i], "-D") == 0)
            {
                debug_flag = true;

                int debug_lvl_arg = i + 1;

                //if debug level present, parse it.
                if (argc > debug_lvl_arg)
                {
                    if (strcmp(&argv[debug_lvl_arg][0], "-") != 0)
                    {
                        debug_lvl = atoi(&(argv[debug_lvl_arg])[0]);
                    }
                }
            }
            else if (strcmp(argv[i], "-L") == 0)
            {
                live_datasource_flag = true;
                live_datasource_tcp_socket_flag = false;
            }

            else if (strcmp(argv[i], "-S") == 0)
            {
                live_datasource_flag = false;
                live_datasource_tcp_socket_flag = true;

                char next_arg[6];
                strncpy(next_arg, argv[i+1], 6);

                if (next_arg[0] != '-') {
                    strncpy(tcp_socket_port, argv[i + 1], 12);
                }

                
            }

            // for --settings
            else if (strcmp(argv[i], "--settings") == 0)
            {
                strncpy(settings_file, argv[i + 1], 64);
            }

            // for --config
            else if (strcmp(argv[i], "--config") == 0)
            {
                strncpy(config_file, argv[i + 1], 64);
            }

            // for --benchmark
            else if ((strcmp(argv[i], "--benchmark") == 0) || (strcmp(argv[i], "-B") == 0))
            {
                // if custom .jsom file provided.
                if (i + 1 < argc)
                {
                    strncpy(benchmark_file, argv[i + 1], 64);
                }

                benchmark_flag = 1;
            }

            else
            {
                // unknown arg.
                continue;
            }
        }
    }

    struct Config config;
    struct Settings settings;

    //check if req. files exist, if no, create and exit!
    if (!is_file_exists(settings_file))
    {
        create_setting_config_benchmark_files(1);
        config_need_exit_flag = true;
    }

    if (!is_file_exists(config_file))
    {
        create_setting_config_benchmark_files(2);
        config_need_exit_flag = true;
    }

    if ((is_file_exists(benchmark_file) == false) && (benchmark_flag == true))
    {
        create_setting_config_benchmark_files(3);
        config_need_exit_flag = true;
    }

    if (config_need_exit_flag == true)
    {
        printf("\nPlease edit settings.json, config.json and benchmark.json accordingly, program will use these configs on next run!\nBye!\n");
        exit(0);
    }

    settings = parse_settings_from_json(settings_file);

    //set debug if passed as arg.
    if (debug_flag == true)
    {
        settings.debug = true;

        if (debug_lvl != false)
        {
            settings.debug_level = debug_lvl;
        }
    }

    if (derivative_flag == true)
    {
        settings.is_derivative = true;
    }

    //set is_live_data to true as arg.
    if (live_datasource_flag == true)
    {
        settings.is_live_data = true;
        settings.is_live_data_socket = false;
    }
    else if (live_datasource_tcp_socket_flag == true)
    {

        settings.is_live_data_socket = true;
        settings.is_live_data = false;

        strncpy(settings.socket_port, tcp_socket_port, 6);
        
        // socket feature not supported on windows (yet!)
        #ifdef _WIN32
        settings.is_live_data_socket = false;
        #endif
    }
    else
    {
        settings.is_live_data = false;
    }

    config = parse_config_from_json(config_file);

    if (benchmark_flag)
    {
        benchmark(benchmark_file, settings);
    }
    else if (settings.is_derivative)
    {
        run_sim_w_derivative(settings, config);
    }
    else
    {
        run_sim(settings, config);
    }

    return 0;
}