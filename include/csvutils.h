#define MAXCHARPERLINE 1024
#define MAXCSVHEAD 11

/* Declarations */
int change_in_modified_date(char* filename);
int reopen_datasource(char* filename, FILE** fp);
algoticks_row tokenize_row(char *row);
int read_csv(algoticks_settings settings,algoticks_config config, FILE *fp, algoticks_row *storage, int seek_offset);
int is_quoted(char *str);