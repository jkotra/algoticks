/* Declarations */
algoticks_cb_l load_cb(char *algo);
algo_func load_algo_func(char *algo);
void close_algo_func();
void write_simresult_to_csv(algoticks_simresult *simresult);
void create_setting_config_benchmark_files(int type);
int is_file_exists(const char * filename);
void filter_boundaries(algoticks_config *config, int is_short);
int is_target_hit(algoticks_dashboard dashboard, float target);
int is_stoploss_hit(algoticks_dashboard dashboard, float stoploss);
float brokerage_calc();
void convert_to_lowercase(char *str);
void free_algoticks_config(algoticks_config *config);
void free_algoticks_settings(algoticks_settings *settings);
void free_algoticks_benchmark(algoticks_benchmarkconfig *benchmark);

//optional optimization
void chomp(char *s);
void remove_quotes(char *str);