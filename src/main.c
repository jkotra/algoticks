#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../include/dtypes.h"
#include "../include/misc.h"
#include "../include/sim.h"
#include "../include/benchmark.h"

int debug_flag = false;
int benchmark_flag = false;
int config_need_exit_flag = false;
int live_datasource_flag = false;

char settings_file[64] = "settings.json";
char config_file[64] = "config.json";
char benchmark_file[64] = "benchmark.json";

void print_version_and_exit()
{
    printf("algoticks v0.1\n");
    exit(0);
}

void print_help_and_exit()
{

    printf("-V\t\t\tPrint Version and Exit.\n");
    printf("-H\t\t\tPrint this message and Exit.\n");
    printf("-D\t\t\tEnable Debug.\n");
    printf("-L\t\t\tIndicate datasource is updated in realtime.\n\n\n");

    printf("--settings [*.JSON]\t\t\tCustom settings file. Default: settings.json\n");
    printf("--config [*.JSON]\t\t\tCustom config file. Default: config.json\n");
    printf("--benchmark -B [(Optional)[*.JSON]\tCustom config file. Default: benchmark.json\n");
    exit(0);
}

int main(int argc, char **argv)
{

    if (argc > 1)
    {

        for (int i = 0; i < argc; i++)
        {

            // version
            if (strcmp(argv[i], "-V") == 0)
            {
                print_version_and_exit();
            }

            //help
            else if (strcmp(argv[i], "-H") == 0)
            {
                print_help_and_exit();
            }
            
            //debug
            else if (strcmp(argv[i], "-D") == 0)
            {
                debug_flag = true;
            }
            else if (strcmp(argv[i], "-L") == 0)
            {
                live_datasource_flag = true;
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

    //check if req files really exist
    if (!is_file_exists(settings_file)){
        create_setting_config_enchmark_files(1);
        config_need_exit_flag = true;
    }

    if (!is_file_exists(config_file)){
        create_setting_config_enchmark_files(2);
        config_need_exit_flag = true;
    }

    if ((is_file_exists(benchmark_file) == false) && (benchmark_flag == true)){
        create_setting_config_enchmark_files(3);
        config_need_exit_flag = true;
    }

    if (config_need_exit_flag == true){
        printf("\nPlease edit settings.json, config.json and benchmark.json accordingly, program will use these configs on next run!\nBye!\n");
        exit(0);
    }



    settings = parse_settings_from_json(settings_file);

    //set debug if passed as arg.
    if (debug_flag == true)
    {
        settings.debug = true;
    }

    //set is_live_data to true as arg.
    if (live_datasource_flag == true){
        settings.is_live_data = true;
    }

    if (benchmark_flag)
    {
        benchmark(benchmark_file, settings);
    }
    else
    {
        config = parse_config_from_json(config_file);
        run_sim(settings, config);
    }

    return 0;
}