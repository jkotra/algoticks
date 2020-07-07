int is_date_over_or_eq_intraday(char *date, int intraday_hour, int intraday_min);
int is_date_after(char *date_a, char *date_b);
int get_time_with_sscanf_from_string(char* date, struct tm *time_struct);
