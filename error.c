#define _GNU_SOURCE // enable GNU feature set

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <execinfo.h> // generic stack trace
#include <errno.h>
#include "error.h"

// die and print system error
void dies(char* format, ...) {
  va_list args;
  va_start(args, format);
  vdie(format, args);
}

// die without printing system error
void die(char* format, ...) {
  va_list args;
  va_start(args, format);
  errno = 0;
  vdie(format, args);
}

void vdie(char* format, va_list args) {
  char* tmp;
  unsigned int chars_left = ERROR_MSG_MAX_LENGTH;
  int num;

  // generic stack trace
  void *bt[20];
  char **strings;
  size_t sz;
  unsigned int i;

  num = fprintf(stderr, "----------------------\n");
  if(num < 0) {
    exit(-1);
  }

  num = fprintf(stderr, "ERROR: ");
  if(num < 0) {
    exit(-1);
  }

  if(num < 0) {
    exit(-1);
  }


  tmp = malloc(sizeof(char) * (ERROR_MSG_MAX_LENGTH+1));
  if(!tmp) {
    exit(-1); // XXX what if this happens from inside a thread?
  }

  num = vsnprintf(tmp, chars_left, format, args);
  if(num < 0) {
    free(tmp);
    exit(-1);
  }

  chars_left -= num;

  if(errno) {

    num = snprintf(tmp+num, chars_left, "(errno indicated: %s)\n", strerror(errno)); // XXXX strerror is not thread safe
    if(num < 0) {
      exit(-1);
    }

  }
	
  fprintf(stderr, tmp);
  free(tmp);

  fprintf(stderr, "Stack Trace (generic):\n");

  sz = backtrace(bt, 20);
  strings = backtrace_symbols(bt, sz);

  for(i=1; i < sz; i++) {
    fprintf(stderr, "%2d: %p %s\n", i, bt[i], strings[i]);
  }
  
  exit(-1);

}


void nprint(char* str, int num_chars) {
  int i;
  for(i=0; i < num_chars; i++) {
    putchar((int) *(str + i));
  }

}
