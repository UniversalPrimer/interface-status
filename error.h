#include <stdarg.h>

#define ERROR_MSG_MAX_LENGTH 256

void dies(char* format, ...);
void die(char* format, ...);
void vdie(char* format, va_list args);
void nprint(char* str, int num_chars);
