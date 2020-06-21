/* Declarations */
void write_simresult_to_csv(algoticks_simresult simresult);
void chomp(char *s);
algoticks_benchmarkconfig parse_benchmark_from_json(char *filename);
algoticks_settings parse_settings_from_json(char *filename);
algoticks_config parse_config_from_json(char *filename);
float brokerage_calc();