/* Declarations */
void debug_msg(bool settings_debug, int settings_debug_level, int msg_debug_level, char *file_name, const char *function, int line_n, char *message);
void debug_msg_simple(char* msg);
/* struct debug */
void print_config_struct(algoticks_config *config);
void print_dashboard_struct(algoticks_dashboard *dashboard);
void print_signal_struct(algoticks_signal *signal);
void print_simresult_struct(algoticks_simresult *simresult);
void print_row_struct(algoticks_row *row);