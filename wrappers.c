#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include "error.h"
#include "wrappers.h"


// wrapped malloc that dies if it fails
void* wmalloc(size_t num_bytes) {
	void* mem;

	mem = malloc(num_bytes);
	if(!mem) {
		die("memory allocation failure");
	}

	return mem;
}


size_t wfread(void *ptr, size_t size, size_t nmemb, FILE* stream) {
  size_t n;
  
  n = fread(ptr, size, nmemb, stream);
  if(n == 0) {
    if(ferror(stream) != 0) {
      fclose(stream);
      dies("Error reading from file");
    }
  }
  return n;
}
