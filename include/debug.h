/* Declarations */
void debug_msg(algoticks_settings settings, int debug_level, char *op, char *loc, char *remarks);

/* struct debug */
void print_config_struct(algoticks_config config);
void print_dashboard_struct(algoticks_dashboard dashboard);
void print_signal_struct(algoticks_signal signal);
void print_simresult_struct(algoticks_simresult simresult);
void print_row_struct(algoticks_row row);