#define DEBUG 0
#define INFO 1
#define ERROR 2
void printf(const char *str,...);
void printTimer(const char *str,...);
void printKb(const char *str,...);
void clrscr();
void set_debug_level(int debug_level);
void debug(const char * str,...);
void info(const char * str,...);
void error(const char * str,...);
