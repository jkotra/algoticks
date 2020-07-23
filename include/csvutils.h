#define MAXCHARPERLINE 1024
#define MAXCSVHEAD 11

/* Declarations */
void reset_header_skip();
int check_row_integrity(algoticks_row *row);
int change_in_modified_date(char* filename);
int reopen_datasource(char* filename, FILE** fp);
algoticks_row tokenize_row(char *row);
void set_ohlcv_as_header();
int process_csv_header(algoticks_settings settings, char *row);
int read_csv(algoticks_settings settings,algoticks_config config, FILE *fp, char *fname, algoticks_row *storage, int seek_offset);
int is_quoted(char *str);