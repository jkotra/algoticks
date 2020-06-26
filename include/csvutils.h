#define MAXCHARPERLINE 1024
#define MAXCSVHEAD 8

/* Declarations */
algoticks_row tokenize_row(char *row);
int read_csv(algoticks_settings settings, FILE *fp, algoticks_row *storage, int seek_offset, algoticks_config config, int debug);
int is_quoted(char *str);