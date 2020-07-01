#define MAXCHARPERLINE 1024
#define MAXCSVHEAD 11

/* Declarations */
algoticks_row tokenize_row(char *row);
int read_csv(algoticks_settings settings,algoticks_config config, FILE *fp, algoticks_row *storage, int seek_offset);
int is_quoted(char *str);