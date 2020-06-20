#define MAXCHARPERLINE 1024

/* Declarations */
algoticks_row tokenize_row(char *row);
int read_csv(FILE *fp, algoticks_row *storage, int seek_offset, algoticks_config config, int debug);
void write_to_report(algoticks_simresult simres);